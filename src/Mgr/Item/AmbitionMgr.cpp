/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "AmbitionMgr.h"

#include <algorithm>
#include <set>
#include <vector>

#include "AiFactory.h"
#include "BisListMgr.h"
#include "DBCStores.h"
#include "ItemTemplate.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Playerbots.h"

namespace
{
    // Continents. Everything else with a spawn is a dungeon, raid or battleground.
    char const* CONTINENT_MAPS = "0, 1, 530, 571";

    // Same finaliser as BotPersonality, so an ambition is stable per bot without needing storage.
    uint32 Mix(uint32 x)
    {
        x += 0x9E3779B9u;
        x = (x ^ (x >> 16)) * 0x21F0AAADu;
        x = (x ^ (x >> 15)) * 0x735A2D97u;
        return x ^ (x >> 15);
    }
}

void AmbitionMgr::LoadAll()
{
    _sources.clear();

    // Direct drops and reference-table drops both matter: most raid loot goes through a reference,
    // and looking only at creature_loot_template finds barely a third of what is reachable.
    QueryResult result = WorldDatabase.Query(
        "SELECT src.item, MIN(c.map), MIN(ct.name) FROM ("
        "  SELECT clt.Item AS item, clt.Entry AS entry FROM creature_loot_template clt"
        "  UNION ALL"
        "  SELECT rlt.Item, clt2.Entry FROM reference_loot_template rlt"
        "    JOIN creature_loot_template clt2 ON clt2.Reference = rlt.Entry"
        ") src "
        "JOIN creature c ON c.id = src.entry "
        "JOIN creature_template ct ON ct.entry = src.entry "
        "JOIN item_template it ON it.entry = src.item "
        "WHERE it.Quality >= 3 AND c.map NOT IN ({}) "
        "GROUP BY src.item",
        CONTINENT_MAPS);

    if (!result)
    {
        LOG_INFO("playerbots", "AmbitionMgr: no drop sources found, bots will not name where an item drops");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        ItemSource source;
        uint32 const itemId = fields[0].Get<uint32>();
        source.mapId = fields[1].Get<uint32>();
        source.creatureName = fields[2].Get<std::string>();

        _sources[itemId] = source;
    } while (result->NextRow());

    LOG_INFO("playerbots", "AmbitionMgr: loaded drop sources for {} items", _sources.size());
}

std::string AmbitionMgr::DescribeLocation(BotAmbition const& ambition) const
{
    if (!ambition.KnowsSource())
        return "";

    MapEntry const* map = sMapStore.LookupEntry(ambition.mapId);
    if (!map)
        return "";

    return map->name[sWorld->GetDefaultDbcLocale()];
}

uint32 AmbitionMgr::CountSetPiecesTotal(uint32 setId)
{
    ItemSetEntry const* set = sItemSetStore.LookupEntry(setId);
    if (!set)
        return 0;

    uint32 total = 0;
    for (uint32 i = 0; i < MAX_ITEM_SET_ITEMS; ++i)
        if (set->itemId[i])
            ++total;

    return total;
}

uint32 AmbitionMgr::CountSetPiecesOwned(Player* bot, uint32 setId)
{
    ItemSetEntry const* set = sItemSetStore.LookupEntry(setId);
    if (!bot || !set)
        return 0;

    uint32 owned = 0;
    for (uint32 i = 0; i < MAX_ITEM_SET_ITEMS; ++i)
    {
        if (!set->itemId[i])
            continue;

        if (bot->GetItemCount(set->itemId[i], true) > 0)
            ++owned;
    }

    return owned;
}

BotAmbition AmbitionMgr::ResolveFor(Player* bot) const
{
    BotAmbition ambition;

    if (!bot || !sPlayerbotAIConfig.botAmbitions)
        return ambition;

    PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
    if (!botAI)
        return ambition;

    BotAmbitionKind const kind = botAI->GetPersonality().ambition;
    if (kind == BotAmbitionKind::None)
        return ambition;

    // Aim a tier above what the bot is wearing now.
    uint16 const gearScore = static_cast<uint16>(botAI->GetEquipGearScore(bot)) + TIER_STEP;
    uint8 const faction = bot->GetTeamId() == TEAM_ALLIANCE ? 1 : 2;

    std::map<uint8, uint32> const bis = sBisListMgr->GetBisForNearest(
        gearScore, TIER_SEARCH_DROP, bot->getClass(), AiFactory::GetPlayerSpecTab(bot), faction);

    if (bis.empty())
        return ambition;

    uint32 const seed = bot->GetGUID().GetCounter();

    if (kind == BotAmbitionKind::Set)
    {
        // Any set the bot's aspirational gear belongs to and has not already finished.
        std::set<uint32> candidates;
        for (auto const& [slot, itemId] : bis)
        {
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId);
            if (!proto || !proto->ItemSet)
                continue;

            uint32 const total = CountSetPiecesTotal(proto->ItemSet);
            if (total > 1 && CountSetPiecesOwned(bot, proto->ItemSet) < total)
                candidates.insert(proto->ItemSet);
        }

        if (candidates.empty())
            return ambition;

        std::vector<uint32> ordered(candidates.begin(), candidates.end());
        ambition.kind = BotAmbitionKind::Set;
        ambition.setId = ordered[Mix(seed ^ 0xA53Cu) % ordered.size()];
        return ambition;
    }

    // Single item. Prefer one whose drop source we can name - a bot that can say where it is going
    // is far more interesting than one that just wants a thing, and only a quarter of items resolve.
    std::vector<uint32> withSource;
    std::vector<uint32> anyItem;

    for (auto const& [slot, itemId] : bis)
    {
        if (bot->GetItemCount(itemId, true) > 0)
            continue;

        anyItem.push_back(itemId);
        if (HasSource(itemId))
            withSource.push_back(itemId);
    }

    std::vector<uint32> const& pool = withSource.empty() ? anyItem : withSource;
    if (pool.empty())
        return ambition;

    ambition.kind = BotAmbitionKind::Item;
    ambition.itemId = pool[Mix(seed ^ 0x51EDu) % pool.size()];

    auto const it = _sources.find(ambition.itemId);
    if (it != _sources.end())
    {
        ambition.mapId = it->second.mapId;
        ambition.sourceName = it->second.creatureName;
    }

    return ambition;
}
