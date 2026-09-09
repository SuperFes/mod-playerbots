/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_FORMATIONS_H
#define PLAYERBOTS_FORMATIONS_H

#include "Action.h"
#include "NamedObjectContext.h"
#include "PlayerbotAIConfig.h"
#include "TravelMgr.h"

class Group;
class Player;
class PlayerbotAI;

class Formation : public AiNamedObject
{
public:
    Formation(PlayerbotAI* botAI, std::string const name) : AiNamedObject(botAI, name) {}
    virtual ~Formation() = default;
    virtual std::string const GetTargetName() { return ""; }
    virtual WorldLocation GetLocation() { return NullLocation; }
    virtual float GetMaxDistance() { return sPlayerbotAIConfig.followDistance; }
    static WorldLocation NullLocation;
    static bool IsNullLocation(WorldLocation const& loc);

protected:
    float GetFollowAngle();

    // Where this bot sits in the follow order, for queueing single file. Valid after GetFollowAngle.
    uint32 GetSlotIndex() const { return _cachedSlotIndex; }

    // Resolves a reachable position for this bot: its normal slot when the flank is clear, or a
    // place in a single-file queue behind the master when it is not. Confinement is sticky for a
    // few seconds, because the underlying raycast flickers around geometry edges and a bot that
    // re-decided every tick would visibly jitter between the flank and the line.
    bool ResolveSlotOrQueue(Player* master, Map* map, float angle, float range, float dx, float dy,
                            float& x, float& y, float& z);

private:
    uint64 GetRosterKey(Player* master, Group* group) const;

    uint64 _cachedRosterKey = 0;
    float _cachedSlotOffset = 0.0f;
    uint32 _cachedSlotIndex = 1;
    bool _slotCacheValid = false;

    time_t _confinedUntil = 0;
};

class FollowFormation : public Formation
{
public:
    FollowFormation(PlayerbotAI* botAI, std::string const name) : Formation(botAI, name) {}
};

class MoveFormation : public Formation
{
public:
    MoveFormation(PlayerbotAI* botAI, std::string const name) : Formation(botAI, name) {}

protected:
    WorldLocation MoveLine(std::vector<Player*> line, float diff, float cx, float cy, float cz, float orientation,
                           float range);
    WorldLocation MoveSingleLine(std::vector<Player*> line, float diff, float cx, float cy, float cz, float orientation,
                                 float range);
};

class MoveAheadFormation : public MoveFormation
{
public:
    MoveAheadFormation(PlayerbotAI* botAI, std::string const name) : MoveFormation(botAI, name) {}

    WorldLocation GetLocation() override;
    virtual WorldLocation GetLocationInternal() { return NullLocation; }
};

class FormationValue : public ManualSetValue<Formation*>
{
public:
    FormationValue(PlayerbotAI* botAI);
    ~FormationValue();

    std::string const Save() override;
    bool Load(std::string const value) override;
};

class SetFormationAction : public Action
{
public:
    SetFormationAction(PlayerbotAI* botAI) : Action(botAI, "set formation") {}

    bool Execute(Event event) override;
};

#endif
