/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_LFGTRIGGERS_H
#define PLAYERBOTS_LFGTRIGGERS_H

#include "Trigger.h"

class PlayerbotAI;

class LfgProposalActiveTrigger : public Trigger
{
public:
    LfgProposalActiveTrigger(PlayerbotAI* botAI) : Trigger(botAI, "lfg proposal active", 20 * 2000) {}

    bool IsActive() override;
};

// A bot that belongs to a dungeon-finder group but is not in its dungeon.
//
// LFGMgr::MakeNewGroup teleports the whole group in when the queue pops, but LFGMgr::TeleportPlayer
// refuses while a player is dead, falling, mid-jump, in combat, fatigued or on a vehicle - and
// nothing ever retries. A bot that happened to be jumping down a step at the moment the queue popped
// is simply left standing where it was, with the rest of the party inside.
class OutsideLfgDungeonTrigger : public Trigger
{
public:
    OutsideLfgDungeonTrigger(PlayerbotAI* botAI) : Trigger(botAI, "outside lfg dungeon", 5 * 1000) {}

    bool IsActive() override;
};

class UnknownDungeonTrigger : public Trigger
{
public:
    UnknownDungeonTrigger(PlayerbotAI* botAI) : Trigger(botAI, "unknown dungeon", 20 * 2000) {}

    bool IsActive() override;
};

#endif
