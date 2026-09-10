/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_CITYVENDORMGR_H
#define PLAYERBOTS_CITYVENDORMGR_H

#include <unordered_map>
#include <vector>

#include "Common.h"

class Player;

// Somewhere for a bot to be walking to.
//
// Wandering at random reads as broken; walking towards a shop reads as an errand, even when the bot
// has no intention of buying anything. The nearby-NPC list cannot supply this - it is capped by
// sight range, so a bot standing at a city gate cannot see anything in the middle of town. The
// spawn table can, so this indexes vendor positions once at startup and hands out destinations.
class CityVendorMgr
{
public:
    static CityVendorMgr* instance()
    {
        static CityVendorMgr inst;
        return &inst;
    }

    void LoadAll();

    struct Spot
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    // A vendor within maxDistance of the bot, chosen at random among those in range. Returns false
    // when there is nothing nearby, which is the common case outside settlements.
    bool PickNearby(Player* bot, float maxDistance, Spot& out) const;

    bool IsLoaded() const { return !_byMap.empty(); }

private:
    CityVendorMgr() = default;

    std::unordered_map<uint32, std::vector<Spot>> _byMap;
};

#define sCityVendorMgr CityVendorMgr::instance()

#endif
