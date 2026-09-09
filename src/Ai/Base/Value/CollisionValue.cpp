/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "CollisionValue.h"

#include "CellImpl.h"
#include "Creature.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Playerbots.h"
#include "ServerFacade.h"

// Returns the nearest NPC (Creature) that the bot is currently overlapping, or nullptr.
// Overlap is defined as the 2D distance being less than the creature's CombatReach.
// Dead, uninteractable, and pet/summon NPCs are excluded so bots don't jitter around
// corpses or their own pets.
Creature* CollisionValue::FindOverlappingNPC(Player* bot)
{
    if (!bot || !bot->IsInWorld())
        return nullptr;

    constexpr float NPC_AVOID_RADIUS = 5.0f; // search range; creatures further than this are irrelevant

    std::list<Creature*> creatures;
    Acore::AnyUnitInObjectRangeCheck range_check(bot, NPC_AVOID_RADIUS);
    Acore::CreatureListSearcher<Acore::AnyUnitInObjectRangeCheck> searcher(bot, creatures, range_check);
    Cell::VisitObjects(bot, searcher, NPC_AVOID_RADIUS);

    Creature* closest = nullptr;
    float closestOverlap = 0.0f;

    for (Creature* creature : creatures)
    {
        if (!creature || creature->GetGUID() == bot->GetGUID())
            continue;

        // Skip dead/despawned NPCs — a bot standing on a corpse is fine
        if (!creature->IsAlive() || !creature->IsInWorld())
            continue;

        // Skip pets and player-controlled summons — they move with the group
        if (creature->IsPet() || creature->IsControlledByPlayer())
            continue;

        // Skip NPCs the bot is actively targeting/fighting
        if (bot->GetTarget() == creature->GetGUID())
            continue;

        float dist2d = bot->GetExactDist2d(creature);
        float threshold = creature->GetCombatReach(); // half the model's collision radius

        if (dist2d < threshold)
        {
            // Track the most-overlapping creature so we step away from the worst offender
            float overlap = threshold - dist2d;
            if (overlap > closestOverlap)
            {
                closestOverlap = overlap;
                closest = creature;
            }
        }
    }

    return closest;
}

bool CollisionValue::Calculate()
{
    // Skip collision checking if the bot is currently on a transport (elevator, ship, zeppelin, etc.)
    // because platforms are small and stepping away can cause the bot to fall to their death.
    if (bot->GetTransport())
        return false;

    Unit* target = AI_VALUE(Unit*, qualifier);
    if (!target)
        return false;

    // Check NPC overlap first — this is the primary use case (bots standing in NPCs)
    if (FindOverlappingNPC(bot) != nullptr)
        return true;

    // Also check player/unit overlap at the original tight contactDistance
    std::list<Unit*> targets;
    float range = sPlayerbotAIConfig.contactDistance;
    Acore::AnyUnitInObjectRangeCheck u_check(bot, range);
    Acore::UnitListSearcher<Acore::AnyUnitInObjectRangeCheck> searcher(bot, targets, u_check);
    Cell::VisitObjects(bot, searcher, range);

    for (Unit* unit : targets)
    {
        if (bot == unit)
            continue;

        float dist = ServerFacade::instance().GetDistance2d(bot, unit->GetPositionX(), unit->GetPositionY());
        if (ServerFacade::instance().IsDistanceLessThan(dist, unit->GetCombatReach()))
            return true;
    }

    return false;
}
