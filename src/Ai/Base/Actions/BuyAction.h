/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_BUYACTION_H
#define PLAYERBOTS_BUYACTION_H

#include <set>

#include "InventoryAction.h"

class FindItemVisitor;
class ObjectGuid;
class Item;
class Player;
class PlayerbotAI;

struct ItemTemplate;
struct VendorItem;
struct VendorItemData;
class Creature;

class BuyAction : public InventoryAction
{
public:
    BuyAction(PlayerbotAI* botAI) : InventoryAction(botAI, "buy") {}

    bool Execute(Event event) override;

protected:
    bool BuyItem(VendorItemData const* tItems, ObjectGuid vendorguid, ItemTemplate const* proto);

private:
    /** True if the vendor has stock and the bot can pay the extended cost; gold is checked by the core. */
    bool CanAfford(VendorItem const* tItem, ItemTemplate const* proto, Creature* vendor) const;
    bool TradeItem(FindItemVisitor* visitor, int8 slot);
    bool TradeItem(Item const* item, int8 slot);
};

// Restocking consumables at a vendor: spell reagents for any class that needs them, plus ammo.
// Bots already notice and announce being out of reagents - nothing has ever refilled them, so a
// priest would tell you about its missing candles indefinitely.
class BuyReagentsAction : public BuyAction
{
public:
    BuyReagentsAction(PlayerbotAI* botAI) : BuyAction(botAI) {}

    std::string const getName() override { return "buy reagents"; }
    bool isUseful() override;
    bool Execute(Event event) override;

private:
    // Reagent item ids for known spells the bot currently cannot cast for want of them.
    std::set<uint32> FindMissingReagents() const;
};

#endif
