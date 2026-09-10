/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "AuctionAction.h"

#include "Item.h"
#include "ItemTemplate.h"
#include "ItemUsageValue.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Playerbots.h"

namespace
{
    // Auction durations the client offers, in minutes. Bots use the longest: they are not watching
    // the market and a short listing just means the item comes back in the post.
    constexpr uint32 AUCTION_ETIME_LONG = 1440;

    bool CanBeListed(Item* item)
    {
        if (!item || item->IsSoulBound() || item->IsInTrade())
            return false;

        ItemTemplate const* proto = item->GetTemplate();
        if (!proto)
            return false;

        // Bind-on-pickup never reaches the auction house, and conjured goods vanish.
        if (proto->Bonding == BIND_WHEN_PICKED_UP || proto->Bonding == BIND_QUEST_ITEM)
            return false;

        if (proto->Flags & ITEM_FLAG_CONJURED)
            return false;

        if (item->GetUInt32Value(ITEM_FIELD_DURATION))
            return false;

        return proto->SellPrice > 0;
    }
}

ObjectGuid AuctionSurplusAction::FindAuctioneer() const
{
    GuidVector const npcs = botAI->GetAiObjectContext()->GetValue<GuidVector>("nearest npcs")->Get();
    for (ObjectGuid const guid : npcs)
        if (bot->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_AUCTIONEER))
            return guid;

    return ObjectGuid::Empty;
}

std::vector<Item*> AuctionSurplusAction::FindAuctionable() const
{
    std::vector<Item*> found;

    for (Item* item : botAI->GetInventoryItems())
    {
        if (!CanBeListed(item))
            continue;

        ItemTemplate const* proto = item->GetTemplate();

        std::string param = std::to_string(proto->ItemId);
        if (int32 const randomProperty = item->GetItemRandomPropertyId())
            param += "," + std::to_string(randomProperty);

        if (AI_VALUE2(ItemUsage, "item usage", param) == ITEM_USAGE_AH)
            found.push_back(item);
    }

    return found;
}

bool AuctionSurplusAction::isUseful()
{
    if (!sPlayerbotAIConfig.auctionSurplus)
        return false;

    if (bot->IsInCombat() || botAI->GetState() == BOT_STATE_COMBAT || bot->isMoving())
        return false;

    if (botAI->GetIdleSeconds() < sPlayerbotAIConfig.tradeskillIdleSeconds)
        return false;

    return FindAuctioneer() && !FindAuctionable().empty();
}

bool AuctionSurplusAction::ListItem(ObjectGuid auctioneer, Item* item)
{
    ItemTemplate const* proto = item->GetTemplate();
    if (!proto)
        return false;

    uint32 const count = item->GetCount();

    // No market history to price against, so anchor on vendor value and ask for a multiple of it -
    // roughly what a player does when they cannot be bothered to check the auction house.
    uint32 const base = proto->SellPrice * count;
    uint32 const bid = std::max<uint32>(1, base * sPlayerbotAIConfig.auctionPriceMultiplier);
    uint32 const buyout = bid + bid / 4;

    WorldPacket packet(CMSG_AUCTION_SELL_ITEM);
    packet << auctioneer;
    packet << uint32(1);
    packet << item->GetGUID();
    packet << count;
    packet << bid;
    packet << buyout;
    packet << uint32(AUCTION_ETIME_LONG);

    // The handler validates, charges the deposit and removes the item from the bag.
    bot->GetSession()->HandleAuctionSellItem(packet);

    return true;
}

bool AuctionSurplusAction::Execute(Event /*event*/)
{
    ObjectGuid const auctioneer = FindAuctioneer();
    if (!auctioneer)
        return false;

    std::vector<Item*> const items = FindAuctionable();
    if (items.empty())
        return false;

    uint32 listed = 0;
    for (Item* item : items)
    {
        if (listed >= sPlayerbotAIConfig.auctionMaxPerVisit)
            break;

        if (ListItem(auctioneer, item))
            ++listed;
    }

    if (!listed)
        return false;

    if (botAI->RollSocial(2))
    {
        std::map<std::string, std::string> placeholders;
        placeholders["%count"] = std::to_string(listed);

        botAI->SaySocial(PlayerbotTextMgr::instance().GetBotTextOrDefault(
            "auction_posted", "Put a few things up on the auction house.", placeholders));
    }

    return true;
}
