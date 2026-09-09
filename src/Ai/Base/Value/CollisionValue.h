/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_COLLISIONVALUE_H
#define PLAYERBOTS_COLLISIONVALUE_H

#include "NamedObjectContext.h"
#include "Value.h"

class Creature;
class PlayerbotAI;
class Player;

class CollisionValue : public BoolCalculatedValue, public Qualified
{
public:
    CollisionValue(PlayerbotAI* botAI, std::string const name = "collision")
        : BoolCalculatedValue(botAI, name), Qualified()
    {
    }

    bool Calculate() override;

    // Returns the nearest overlapping NPC, or nullptr. Used by MoveOutOfCollisionAction
    // to determine the step-away direction instead of moving randomly.
    static Creature* FindOverlappingNPC(Player* bot);
};

#endif
