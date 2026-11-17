/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WithinAreaTrigger.h"
#include "LastMovementValue.h"
#include "Playerbots.h"

bool WithinAreaTrigger::IsActive()
{
    LastMovement& movement = context->GetValue<LastMovement&>("last area trigger")->Get();
    if (!movement.lastAreaTrigger)
        return false;

    AreaTrigger const* at = sObjectMgr->GetAreaTrigger(movement.lastAreaTrigger);
    if (!at)
        return false;

    if (!sObjectMgr->GetAreaTriggerTeleport(movement.lastAreaTrigger))
        return false;

    return bot->IsInAreaTriggerRadius(at, 2.0f);
}
