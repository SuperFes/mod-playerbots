/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_AMBITIONMGR_H
#define PLAYERBOTS_AMBITIONMGR_H

#include <string>
#include <unordered_map>

#include "BotPersonality.h"
#include "Common.h"

class Player;

// What a bot is working towards, if anything.
//
// Deliberately derived rather than stored: it is a pure function of the bot's personality seed plus
// its class, spec and current gear tier. That means no persistence to keep in sync, and an ambition
// that advances on its own as the bot out-grows a tier - which is what you would want anyway.
struct BotAmbition
{
    BotAmbitionKind kind = BotAmbitionKind::None;

    uint32 itemId = 0;       // kind == Item
    uint32 setId = 0;        // kind == Set

    uint32 mapId = 0;        // 0 when we cannot say where it drops
    std::string sourceName;  // creature that drops it, empty when unknown

    bool IsSet() const { return kind == BotAmbitionKind::Set && setId; }
    bool IsItem() const { return kind == BotAmbitionKind::Item && itemId; }
    bool KnowsSource() const { return mapId != 0; }
};

class AmbitionMgr
{
public:
    // Aim this far above the bot's current gear score: an ambition should be something it does not
    // have yet, but could plausibly get.
    static constexpr uint16 TIER_STEP = 40;
    static constexpr uint16 TIER_SEARCH_DROP = 60;

    static AmbitionMgr* instance()
    {
        static AmbitionMgr inst;
        return &inst;
    }

    // One pass over the loot tables at startup, building item -> where it drops. Only rare and
    // better, and only creatures spawned on instance maps: nobody aspires to a trash drop in
    // Elwynn Forest, and the continent maps are where the noise lives.
    void LoadAll();

    BotAmbition ResolveFor(Player* bot) const;

    bool HasSource(uint32 itemId) const { return _sources.count(itemId) > 0; }

    // Localised instance name, or empty when the source is unknown.
    std::string DescribeLocation(BotAmbition const& ambition) const;

    // How many pieces of a set the bot already wears, and how many the set has in total.
    static uint32 CountSetPiecesOwned(Player* bot, uint32 setId);
    static uint32 CountSetPiecesTotal(uint32 setId);

private:
    AmbitionMgr() = default;

    struct ItemSource
    {
        uint32 mapId = 0;
        std::string creatureName;
    };

    std::unordered_map<uint32, ItemSource> _sources;
};

#define sAmbitionMgr AmbitionMgr::instance()

#endif
