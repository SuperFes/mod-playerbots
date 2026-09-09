/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ItemDesire.h"

#include "AiFactory.h"
#include "AmbitionMgr.h"
#include "BisListMgr.h"
#include "ItemTemplate.h"
#include "Player.h"
#include "Playerbots.h"
#include "RandomItemMgr.h"
#include "StatsWeightCalculator.h"

uint32 ItemDesire::CountEquippedSetPieces(Player* bot, uint32 itemSet)
{
    if (!itemSet)
        return 0;

    uint32 count = 0;
    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
    {
        Item* equipped = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!equipped)
            continue;

        if (ItemTemplate const* equippedProto = equipped->GetTemplate())
            if (equippedProto->ItemSet == itemSet)
                ++count;
    }

    return count;
}

bool ItemDesire::IsBestInSlot(Player* bot, ItemTemplate const* proto, uint8 slot)
{
    // The BiS table is keyed by gear tier, so ask for the tier nearest the bot's current gear rather
    // than a fixed one - a levelling bot should be compared against gear it could plausibly hold.
    PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
    if (!botAI)
        return false;

    uint16 const gearScore = static_cast<uint16>(botAI->GetEquipGearScore(bot));
    uint8 const faction = bot->GetTeamId() == TEAM_ALLIANCE ? 1 : 2;

    std::map<uint8, uint32> const bis = BisListMgr::instance()->GetBisForNearest(
        gearScore, BIS_TIER_SEARCH_DROP, bot->getClass(), AiFactory::GetPlayerSpecTab(bot), faction);

    auto const it = bis.find(slot);
    return it != bis.end() && it->second == proto->ItemId;
}

float ItemDesire::Calculate(Player* bot, ItemTemplate const* proto, int32 randomPropertyId)
{
    if (!bot || !proto)
        return 0.0f;

    PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
    if (!botAI)
        return 0.0f;

    // Where would it go, and can this bot use it at all?
    uint8 const slot = botAI->FindEquipSlot(proto, NULL_SLOT, true);
    if (slot == NULL_SLOT)
        return 0.0f;

    if (proto->Class == ITEM_CLASS_ARMOR &&
        !sRandomItemMgr.CanEquipArmor(proto, bot->getClass(), bot->GetLevel()))
        return 0.0f;

    if (proto->Class == ITEM_CLASS_WEAPON && !sRandomItemMgr.CanEquipWeapon(proto, bot->getClass()))
        return 0.0f;

    StatsWeightCalculator calculator(bot);
    calculator.SetItemSetBonus(false);
    calculator.SetOverflowPenalty(false);

    float const newScore = calculator.CalculateItem(proto->ItemId, randomPropertyId);
    if (newScore <= 0.0f)
        return 0.0f;

    float equippedScore = 0.0f;
    if (Item* equipped = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
    {
        if (ItemTemplate const* equippedProto = equipped->GetTemplate())
        {
            equippedScore = calculator.CalculateItem(equippedProto->ItemId,
                                                     equipped->GetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID));
        }
    }

    // Only an actual gain counts. An empty slot makes the whole score the gain, which is correct -
    // a bot with nothing there wants it far more than one sidegrading.
    float desire = newScore - equippedScore;
    if (desire <= 0.0f)
        return 0.0f;

    // Wearing pieces of the same set makes another piece worth more than its stats alone say.
    desire += newScore * sPlayerbotAIConfig.lootDesireSetBonus *
              static_cast<float>(CountEquippedSetPieces(bot, proto->ItemSet));

    if (IsBestInSlot(bot, proto, slot))
        desire += newScore * sPlayerbotAIConfig.lootDesireBisBonus;

    // The thing this bot has actually been chasing. Weighted above best-in-slot on purpose: a bot
    // that has talked about wanting this should visibly want it more than one that merely benefits.
    BotAmbition const ambition = sAmbitionMgr->ResolveFor(bot);
    if ((ambition.IsItem() && ambition.itemId == proto->ItemId) ||
        (ambition.IsSet() && proto->ItemSet && proto->ItemSet == ambition.setId))
    {
        desire += newScore * sPlayerbotAIConfig.lootDesireAmbitionBonus;
    }

    return desire;
}
