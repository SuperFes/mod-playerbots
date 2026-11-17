/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_ITEMDESIRE_H
#define PLAYERBOTS_ITEMDESIRE_H

#include "Common.h"

class Player;
struct ItemTemplate;

// How much a particular bot wants a particular item, on a scale that is comparable between bots so
// a group can decide who needs a drop most.
//
// Three signals, all from data the server already has:
//   - the stat-weight gain over whatever occupies that slot now (the bulk of the score)
//   - completion of an item set the bot is already wearing pieces of
//   - presence on the bot's class/spec best-in-slot list
//
// Returns 0 for an item the bot cannot use or that is not an upgrade, so "nobody wants it" is
// representable and callers can treat any positive value as genuine interest.
class ItemDesire
{
public:
    // Set-piece and best-in-slot weightings, and the margin required to defer, are configurable:
    // Playerbots.LootDesireSetBonus / LootDesireBisBonus / LootCourtesyMargin.
    // How far below the bot's current gear score the BiS lookup may fall back when the exact tier
    // has no rows. Without this a levelling bot matches nothing and never gets the BiS bonus.
    static constexpr uint16 BIS_TIER_SEARCH_DROP = 60;

    static float Calculate(Player* bot, ItemTemplate const* proto, int32 randomPropertyId);

private:
    static uint32 CountEquippedSetPieces(Player* bot, uint32 itemSet);
    static bool IsBestInSlot(Player* bot, ItemTemplate const* proto, uint8 slot);
};

#endif
