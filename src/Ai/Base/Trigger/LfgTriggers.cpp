/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "LfgTriggers.h"

#include "Group.h"
#include "LFGMgr.h"

#include "Playerbots.h"

bool LfgProposalActiveTrigger::IsActive() { return AI_VALUE(uint32, "lfg proposal"); }

bool OutsideLfgDungeonTrigger::IsActive()
{
    Group* group = bot->GetGroup();
    if (!group || !group->isLFGGroup())
        return false;

    ObjectGuid const gguid = group->GetGUID();
    if (sLFGMgr->GetState(gguid) != lfg::LFG_STATE_DUNGEON)
        return false;

    uint32 const mapId = sLFGMgr->GetDungeonMapId(gguid);
    if (!mapId || bot->GetMapId() == mapId)
        return false;

    // Only ask again once the conditions that made the automatic teleport fail have cleared,
    // otherwise every retry earns another refusal.
    return bot->IsAlive() && !bot->IsInCombat() && !bot->IsFalling() &&
           !bot->HasUnitState(UNIT_STATE_JUMPING) && !bot->GetVehicle() && !bot->IsBeingTeleported();
}

bool UnknownDungeonTrigger::IsActive()
{
    return botAI->HasActivePlayerMaster() && botAI->GetMaster() && botAI->GetMaster()->IsInWorld() &&
           botAI->GetMaster()->GetMap()->IsDungeon() && bot->GetMapId() == botAI->GetMaster()->GetMapId();
}
