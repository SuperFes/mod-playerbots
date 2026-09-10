/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_LFGACTIONS_H
#define PLAYERBOTS_LFGACTIONS_H

#include <string>

#include "InventoryAction.h"

class PlayerbotAI;

class LfgJoinAction : public InventoryAction
{
public:
    LfgJoinAction(PlayerbotAI* botAI, std::string const name = "lfg join") : InventoryAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;

protected:
    bool JoinLFG();
    uint32 GetRoles();
};

class LfgAcceptAction : public LfgJoinAction
{
public:
    LfgAcceptAction(PlayerbotAI* botAI) : LfgJoinAction(botAI, "lfg accept") {}

    bool Execute(Event event) override;
    bool isUseful() override { return true; }

protected:
    // Accepting forms a new LFG group, which pulls the bot out of whatever party it is in. Refuse
    // when that party belongs to a real player.
    bool WouldAbandonMaster() const;

    bool AnswerProposal(uint32 id, bool accept);
};

class LfgRoleCheckAction : public LfgJoinAction
{
public:
    LfgRoleCheckAction(PlayerbotAI* botAI) : LfgJoinAction(botAI, "lfg role check") {}

    bool Execute(Event event) override;
    bool isUseful() override { return true; }
};

class LfgLeaveAction : public Action
{
public:
    LfgLeaveAction(PlayerbotAI* botAI) : Action(botAI, "lfg leave") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class LfgTeleportAction : public Action
{
public:
    LfgTeleportAction(PlayerbotAI* botAI, std::string const name = "lfg teleport") : Action(botAI, name) {}

    bool Execute(Event event) override;

protected:
    // out = leave the dungeon, !out = go back in.
    void RequestTeleport(bool out);
};

// Retry the entry teleport for a bot the dungeon finder left outside. See OutsideLfgDungeonTrigger.
class LfgEnterDungeonAction : public LfgTeleportAction
{
public:
    LfgEnterDungeonAction(PlayerbotAI* botAI) : LfgTeleportAction(botAI, "lfg enter dungeon") {}

    bool Execute(Event event) override;
};

// Say why the retry was refused rather than standing outside in silence.
class LfgTeleportDeniedAction : public Action
{
public:
    LfgTeleportDeniedAction(PlayerbotAI* botAI) : Action(botAI, "lfg teleport denied") {}

    bool Execute(Event event) override;

private:
    time_t _lastTold = 0;
};

#endif
