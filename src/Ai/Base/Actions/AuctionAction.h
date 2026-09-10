/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_AUCTIONACTION_H
#define PLAYERBOTS_AUCTIONACTION_H

#include <vector>

#include "InventoryAction.h"

class Item;
class PlayerbotAI;

// Posting surplus gear to the auction house while standing at an auctioneer.
//
// ItemUsageValue already separates "worth a vendor's copper" from "worth listing", but nothing acted
// on the distinction - SellAction vendored both. This takes the ITEM_USAGE_AH half and lists it.
//
// Drives WorldSession::HandleAuctionSellItem rather than building an AuctionEntry directly. The
// module already has hand-rolled auction code in StoreLootAction::AuctionItem that calls
// Item::CreateItem and posts a brand new item without touching the bot's bags - fine for an auction
// house populator, an item duplication bug for selling what a bot actually owns. Going through the
// handler gets validation, the deposit, and correct removal from the bag.
class AuctionSurplusAction : public InventoryAction
{
public:
    AuctionSurplusAction(PlayerbotAI* botAI) : InventoryAction(botAI, "auction surplus") {}

    bool isUseful() override;
    bool Execute(Event event) override;

private:
    // Items the bot holds that are classed as auction-worthy and may legally be listed.
    std::vector<Item*> FindAuctionable() const;

    ObjectGuid FindAuctioneer() const;

    // Start/buyout derived from vendor value, since that is the only price signal available
    // server-side without a market history.
    bool ListItem(ObjectGuid auctioneer, Item* item);
};

#endif
