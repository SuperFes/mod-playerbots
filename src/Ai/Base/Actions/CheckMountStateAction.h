/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_CHECKMOUNTSTATEACTION_H
#define PLAYERBOTS_CHECKMOUNTSTATEACTION_H

#include <unordered_map>
#include <vector>

#include "UseItemAction.h"

const uint16 SPELL_TRAVEL_FORM = 783;
const uint16 SPELL_FLIGHT_FORM = 33943;
const uint16 SPELL_SWIFT_FLIGHT_FORM = 40120;

struct MountData
{
    bool swiftMount = false;
    // Outer map: index (0 for ground, 1 for flight), inner map: effect speed -> vector of spell IDs.
    std::map<uint32, std::map<int32, std::vector<uint32>>> allSpells;
    // Default mount speed.
    int32 maxSpeed = 59;
};

struct PreferredMountCache
{
    std::vector<uint32> groundMounts;
    std::vector<uint32> flightMounts;
};

class PlayerbotAI;

class CheckMountStateAction : public UseItemAction
{
public:
    CheckMountStateAction(PlayerbotAI* botAI) : UseItemAction(botAI, "check mount state", true) {}

    bool Execute(Event event) override;
    bool isUseful() override;
    bool isPossible() override { return true; }
    bool Mount();

    // Same choice of mount as always - matched to the master's type and speed - but applied without
    // a cast bar. For arriving somewhere already mounted, where a 1.5s cast is 1.5s of falling.
    bool MountInstantly();

    static void CompleteDismount(Player* bot);

private:
    Player* master;
    ShapeshiftForm masterInShapeshiftForm;
    ShapeshiftForm botInShapeshiftForm;
    static std::unordered_map<uint32, PreferredMountCache> mountCache;
    static bool preferredMountTableChecked;
    // Cast the chosen mount, honouring _instant.
    bool CastMount(uint32 spellId) const;

    bool _instant = false;
    float CalculateDismountDistance() const;
    float CalculateMountDistance() const;
    // False when the bot is too high to let go of a flying mount. Callers must propagate that so
    // the tick falls through to "follow", which is what brings the bot back down.
    bool Dismount();

    // Height above the ground below, or a negative value where there is no ground data.
    float HeightAboveGround() const;
    void ClearStaleFlightFlags();
    bool ShouldFollowMasterMountState(Player* master, bool noAttackers, bool shouldMount) const;
    bool ShouldDismountForMaster(Player* master) const;
    bool IsInFollowPosition(Player* master) const;
    int32 CalculateMasterMountSpeed(Player* master) const;
    bool CheckForSwiftMount() const;
    std::map<uint32, std::map<int32, std::vector<uint32>>> GetAllMountSpells() const;
    bool TryForms(Player* master, int32 masterMountType, int32 masterSpeed) const;
    bool TryPreferredMount(Player* master) const;
    uint32 GetMountType(Player* master) const;
    bool TryRandomMountFiltered(const std::map<int32, std::vector<uint32>>& spells, int32 masterSpeed) const;
};

#endif
