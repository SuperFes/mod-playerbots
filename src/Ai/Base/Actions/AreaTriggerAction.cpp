/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "AreaTriggerAction.h"

#include "Event.h"
#include "LastMovementValue.h"
#include "PlayerbotTextMgr.h"
#include "Playerbots.h"
#include "Transport.h"

bool ReachAreaTriggerAction::Execute(Event event)
{
    if (botAI->IsRealPlayer())  // Do not trigger own area trigger.
        return false;

    uint32 triggerId;
    WorldPacket p(event.getPacket());
    p.rpos(0);
    p >> triggerId;

    AreaTrigger const* at = sObjectMgr->GetAreaTrigger(triggerId);
    if (!at)
        return false;

    AreaTriggerTeleport const* atTeleport = sObjectMgr->GetAreaTriggerTeleport(triggerId);

    // If this areatrigger has a teleport destination and bot is already on that destination map
    // (e.g. from OnPlayerMapChanged), we have already arrived!
    if (atTeleport && bot->GetMapId() == atTeleport->target_mapId)
        return true;

    if (!atTeleport)
    {
        WorldPacket p1(CMSG_AREATRIGGER);
        p1 << triggerId;
        p1.rpos(0);
        bot->GetSession()->HandleAreaTriggerOpcode(p1);

        return true;
    }

    if (bot->GetMapId() != at->map)
    {
        // If master is on the target map, bring the bot to master directly
        Player* master = botAI->GetMaster();
        if (master && master->GetMapId() == atTeleport->target_mapId)
        {
            bot->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TELEPORTED | AURA_INTERRUPT_FLAG_CHANGE_MAP);
            bot->TeleportTo(atTeleport->target_mapId, atTeleport->target_X, atTeleport->target_Y, atTeleport->target_Z, atTeleport->target_Orientation);
            return true;
        }

        return true;
    }

    if (bot->IsInAreaTriggerRadius(at, 2.0f))
    {
        bot->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TELEPORTED | AURA_INTERRUPT_FLAG_CHANGE_MAP);
        bot->TeleportTo(atTeleport->target_mapId, atTeleport->target_X, atTeleport->target_Y, atTeleport->target_Z, atTeleport->target_Orientation);
        botAI->TellMaster(PlayerbotTextMgr::instance().GetBotTextOrDefault("hello", "Hello", {}));
        return true;
    }

    bot->GetMotionMaster()->MovePoint(
        /*id*/ at->map,
        /*coords*/ at->x, at->y, at->z,
        /*forcedMovement*/ FORCED_MOVEMENT_NONE,
        /*speed*/ 0.0f,             // default speed (not handled here)
        /*orientation*/ 0.0f,       // keep current orientation of bot
        /*generatePath*/ true,      // true => terrain path (2d mmap); false => straight spline (3d vmap)
        /*forceDestination*/ false);

    float distance = bot->GetDistance(at->x, at->y, at->z);
    float delay = 1000.0f * distance / bot->GetSpeed(MOVE_RUN) + sPlayerbotAIConfig.reactDelay;
    botAI->SetNextCheckDelay(delay);
    context->GetValue<LastMovement&>("last area trigger")->Get().lastAreaTrigger = triggerId;

    return true;
}

bool AreaTriggerAction::Execute(Event /*event*/)
{
    LastMovement& movement = context->GetValue<LastMovement&>("last area trigger")->Get();

    uint32 triggerId = movement.lastAreaTrigger;
    movement.lastAreaTrigger = 0;

    if (!sObjectMgr->GetAreaTrigger(triggerId))
        return false;

    AreaTriggerTeleport const* atTeleport = sObjectMgr->GetAreaTriggerTeleport(triggerId);
    if (atTeleport)
    {
        bot->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TELEPORTED | AURA_INTERRUPT_FLAG_CHANGE_MAP);
        bot->TeleportTo(atTeleport->target_mapId, atTeleport->target_X, atTeleport->target_Y, atTeleport->target_Z, atTeleport->target_Orientation);
        botAI->TellMaster(PlayerbotTextMgr::instance().GetBotTextOrDefault("hello", "Hello", {}));
        return true;
    }

    WorldPacket p(CMSG_AREATRIGGER);
    p << triggerId;
    p.rpos(0);
    bot->GetSession()->HandleAreaTriggerOpcode(p);

    botAI->TellMaster(PlayerbotTextMgr::instance().GetBotTextOrDefault("hello", "Hello", {}));
    return true;
}
