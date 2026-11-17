/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SELLACTION_H
#define PLAYERBOTS_SELLACTION_H

#include "InventoryAction.h"

class FindItemVisitor;
class Item;
class PlayerbotAI;

class SellAction : public InventoryAction
{
public:
    SellAction(PlayerbotAI* botAI, std::string const name = "sell") : InventoryAction(botAI, name) {}

    bool Execute(Event event) override;
    void Sell(FindItemVisitor* visitor);
    void Sell(Item* item, bool force = false);

private:
    uint32 GetQuestItemRequirement(uint32 itemId);
};

// Clearing out junk while standing at a vendor, unprompted. Deliberately does not walk anywhere:
// SellAction already finds a vendor among the nearby NPCs, so this only fires when the bot happens
// to be next to one - which is what a person does while the group is stopped in town.
class VendorDeclutterAction : public SellAction
{
public:
    VendorDeclutterAction(PlayerbotAI* botAI) : SellAction(botAI, "vendor declutter") {}

    bool isUseful() override;
    bool Execute(Event event) override;

private:
    bool HasVendorNearby() const;
};

#endif
