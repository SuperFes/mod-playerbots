/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "LfgActions.h"
#include "AiFactory.h"
#include "ItemVisitors.h"
#include "LFGMgr.h"
#include "Opcodes.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "PlayerbotTextMgr.h"
#include "RandomPlayerbotMgr.h"

using namespace lfg;

namespace
{
    constexpr uint8 CLASSIC_MAX_LEVEL = 60;
    constexpr uint8 TBC_MAX_LEVEL = 70;

    bool IsExpansionOutleveled(LFGDungeonEntry const* dungeon, uint8 level)
    {
        // Seasonal bosses keep their original expansion but are level 78-82.
        if (dungeon->Flags & LFG_FLAG_SEASONAL)
            return false;

        switch (dungeon->ExpansionLevel)
        {
            case EXPANSION_CLASSIC:
                return level > CLASSIC_MAX_LEVEL;
            case EXPANSION_THE_BURNING_CRUSADE:
                return level > TBC_MAX_LEVEL;
            default:
                return false;
        }
    }
}

bool LfgJoinAction::Execute(Event /*event*/) { return JoinLFG(); }

uint32 LfgJoinAction::GetRoles()
{
    if (!RandomPlayerbotMgr::instance().IsRandomBot(bot))
    {
        if (PlayerbotAI::IsTank(bot))
            return PLAYER_ROLE_TANK;
        if (PlayerbotAI::IsHeal(bot))
            return PLAYER_ROLE_HEALER;
        else
            return PLAYER_ROLE_DAMAGE;
    }

    uint8 spec = AiFactory::GetPlayerSpecTab(bot);
    switch (bot->getClass())
    {
        case CLASS_DRUID:
            if (spec == 2)
                return PLAYER_ROLE_HEALER;
            else if (spec == 1 && bot->HasAura(16931) /* thick hide */)
                return PLAYER_ROLE_TANK;
            else
                return PLAYER_ROLE_DAMAGE;
            break;
        case CLASS_PALADIN:
            if (spec == 1)
                return PLAYER_ROLE_TANK;
            else if (!spec)
                return PLAYER_ROLE_HEALER;
            else
                return PLAYER_ROLE_DAMAGE;
            break;
        case CLASS_PRIEST:
            if (spec != 2)
                return PLAYER_ROLE_HEALER;
            else
                return PLAYER_ROLE_DAMAGE;
            break;
        case CLASS_SHAMAN:
            if (spec == 2)
                return PLAYER_ROLE_HEALER;
            else
                return PLAYER_ROLE_DAMAGE;
            break;
        case CLASS_WARRIOR:
            if (spec == 2)
                return PLAYER_ROLE_TANK;
            else
                return PLAYER_ROLE_DAMAGE;
            break;
        case CLASS_DEATH_KNIGHT:
            if (spec == 0)
                return PLAYER_ROLE_TANK;
            else
                return PLAYER_ROLE_DAMAGE;
            break;

        default:
            return PLAYER_ROLE_DAMAGE;
            break;
    }

    return PLAYER_ROLE_DAMAGE;
}

bool LfgJoinAction::JoinLFG()
{
    // check if already in lfg
    LfgState state = sLFGMgr->GetState(bot->GetGUID());
    if (state != LFG_STATE_NONE)
        return false;

    /*ItemCountByQuality visitor;
    IterateItems(&visitor, ITERATE_ITEMS_IN_EQUIP);
    bool random = urand(0, 100) < 20;
    bool heroic = urand(0, 100) < 50 &&
                  (visitor.count[ITEM_QUALITY_EPIC] >= 3 || visitor.count[ITEM_QUALITY_RARE] >= 10) &&
                  bot->GetLevel() >= 70;
    bool rbotAId = !heroic && (urand(0, 100) < 50 && visitor.count[ITEM_QUALITY_EPIC] >= 5 &&
                               (bot->GetLevel() == 60 || bot->GetLevel() == 70 || bot->GetLevel() == 80));*/

    LfgDungeonSet list;
    std::vector<uint32> selected;

    std::vector<uint32> dungeons = RandomPlayerbotMgr::instance().LfgDungeons[bot->GetTeamId()];
    if (!dungeons.size())
        return false;

    for (std::vector<uint32>::iterator i = dungeons.begin(); i != dungeons.end(); ++i)
    {
        LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(*i);
        if (!dungeon || (dungeon->TypeID != LFG_TYPE_RANDOM && dungeon->TypeID != LFG_TYPE_DUNGEON &&
                         dungeon->TypeID != LFG_TYPE_HEROIC && dungeon->TypeID != LFG_TYPE_RAID))
            continue;

        auto const& botLevel = bot->GetLevel();

        /*LFG_TYPE_RANDOM on classic is 15-58 so bot over level 25 will never queue*/
        if ((dungeon->MinLevel && (botLevel < dungeon->MinLevel || botLevel > dungeon->MaxLevel)) ||
            (botLevel > dungeon->MinLevel + 10 && dungeon->TypeID == LFG_TYPE_DUNGEON))
            continue;

        // LFG level ranges run past the expansion cap: Stratholme is 55-65.
        if (!sPlayerbotAIConfig.RandomBotJoinLfgOutleveledExpansion && IsExpansionOutleveled(dungeon, botLevel))
            continue;

        selected.push_back(dungeon->ID);
        list.insert(dungeon->ID);
    }

    if (!selected.size())
        return false;

    if (list.empty())
        return false;

    bool many = list.size() > 1;
    LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(*list.begin());

    // check role for console msg
    std::string _roles = "multiple roles";
    uint32 roleMask = GetRoles();
    if (roleMask & PLAYER_ROLE_TANK)
        _roles = "TANK";

    if (roleMask & PLAYER_ROLE_HEALER)
        _roles = "HEAL";

    if (roleMask & PLAYER_ROLE_DAMAGE)
        _roles = "DPS";

    LOG_INFO("playerbots", "Bot {} {}:{} <{}>: queues LFG, Dungeon as {} ({})", bot->GetGUID().ToString().c_str(),
             bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName().c_str(), _roles,
             many ? "several dungeons" : dungeon->Name[0]);

    // Set RbotAId Browser comment
    std::string const _gs = std::to_string(botAI->GetEquipGearScore(bot/*, false, false*/));

    // JoinLfg is not threadsafe, so make packet and queue into session
    // sLFGMgr->JoinLfg(bot, roleMask, list, _gs);

    WorldPacket* data = new WorldPacket(CMSG_LFG_JOIN);
    *data << (uint32)roleMask;
    *data << (bool)false;
    *data << (bool)false;
    // Slots
    *data << (uint8)(list.size());
    for (uint32 dungeon : list)
        *data << (uint32)dungeon;
    // Needs
    *data << (uint8)3 << (uint8)0 << (uint8)0 << (uint8)0;
    *data << _gs;
    bot->GetSession()->QueuePacket(data);

    return true;
}

bool LfgRoleCheckAction::Execute(Event /*event*/)
{
    if (bot->GetGroup())
    {
        uint32 newRoles = GetRoles();
        // if (currentRoles == newRoles)
        //     return false;

        WorldPacket* packet = new WorldPacket(CMSG_LFG_SET_ROLES);
        *packet << (uint8)newRoles;
        bot->GetSession()->QueuePacket(packet);
        // sLFGMgr->SetRoles(bot->GetGUID(), newRoles);
        // sLFGMgr->UpdateRoleCheck(group->GetGUID(), bot->GetGUID(), newRoles);

        LOG_INFO("playerbots", "Bot {} {}:{} <{}>: LFG roles checked", bot->GetGUID().ToString().c_str(),
                 bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName().c_str());

        return true;
    }

    return false;
}

bool LfgAcceptAction::WouldAbandonMaster() const
{
    Group* group = bot->GetGroup();

    if (!group || group->isLFGGroup())
        return false;

    if (!botAI->HasGameClientMaster() || !group->IsMember(botAI->GetMaster()->GetGUID()))
        return false;

    // If the party queued together the proposal is for all of us, and accepting is what keeps us
    // together. Only a leftover solo queue entry - the bot was queued before being invited, and
    // LFGGroupScript::OnAddMember only clears those still in LFG_STATE_QUEUED - pulls it away.
    return sLFGMgr->GetState(group->GetGUID()) == LFG_STATE_NONE;
}

bool LfgAcceptAction::AnswerProposal(uint32 id, bool accept)
{
    botAI->GetAiObjectContext()->GetValue<uint32>("lfg proposal")->Set(0);

    if (accept)
        bot->ClearUnitState(UNIT_STATE_ALL_STATE);

    WorldPacket* packet = new WorldPacket(CMSG_LFG_PROPOSAL_RESULT);
    *packet << id << accept;
    bot->GetSession()->QueuePacket(packet);

    if (!accept)
        return true;

    if (RandomPlayerbotMgr::instance().IsRandomBot(bot) && !bot->GetGroup())
    {
        RandomPlayerbotMgr::instance().Refresh(bot);
        botAI->ResetStrategies();
    }

    botAI->Reset();
    return true;
}

bool LfgAcceptAction::Execute(Event event)
{
    uint32 id = AI_VALUE(uint32, "lfg proposal");

    // Nothing stored - take the id straight off the proposal packet.
    if (!id && !event.getPacket().empty())
    {
        WorldPacket p(event.getPacket());
        uint32 dungeonId;
        uint8 state;
        p >> dungeonId >> state >> id;
    }

    if (!id)
        return false;

    // Decline rather than ignore: the rest of the proposal waits out the full 40 second timeout
    // otherwise, and everyone in it eats a decline cooldown for a bot that was never going to come.
    bool const accept = !bot->IsInCombat() && !bot->isDead() && !WouldAbandonMaster();

    return AnswerProposal(id, accept);
}

bool LfgLeaveAction::Execute(Event /*event*/)
{
    // Don't leave if lfg strategy enabled
    // if (botAI->HasStrategy("lfg", BOT_STATE_NON_COMBAT))
    //    return false;

    // Don't leave if already invited / in dungeon
    if (sLFGMgr->GetState(bot->GetGUID()) > LFG_STATE_QUEUED)
        return false;

    // Don't drop a queue we deliberately joined. The "seldom" tick (RandomTrigger, ~300s)
    // otherwise pulls random bots straight back out, so LFGQueue::CheckCompatibility never
    // sees a role-complete pool that holds still long enough to form a group. Turning
    // RandomBotJoinLfg off still lets whoever is mid-queue fall through and leave.
    // Config bool is tested first so the O(currentBots) IsRandomBot() scan is skipped
    // whenever the feature is disabled.
    if (sPlayerbotAIConfig.RandomBotJoinLfg && RandomPlayerbotMgr::instance().IsRandomBot(bot))
        return false;

    WorldPacket* packet = new WorldPacket(CMSG_LFG_LEAVE);
    bot->GetSession()->QueuePacket(packet);
    // sLFGMgr->LeaveLfg(bot->GetGUID());
    return true;
}

bool LfgLeaveAction::isUseful() { return true; }

void LfgTeleportAction::RequestTeleport(bool out)
{
    bot->ClearUnitState(UNIT_STATE_ALL_STATE);

    WorldPacket* packet = new WorldPacket(CMSG_LFG_TELEPORT);
    *packet << out;
    bot->GetSession()->QueuePacket(packet);
}

bool LfgTeleportAction::Execute(Event event)
{
    bool out = false;

    WorldPacket p(event.getPacket());
    if (!p.empty())
    {
        p.rpos(0);
        p >> out;
    }

    RequestTeleport(out);

    return true;
}

bool LfgEnterDungeonAction::Execute(Event /*event*/)
{
    // Stop wherever we are first: the teleport is refused outright while falling or mid-jump, and a
    // bot that is still running its follow path can be back in the air by the time this is handled.
    if (bot->GetMotionMaster())
        bot->GetMotionMaster()->Clear(true);

    bot->StopMoving();

    RequestTeleport(false);

    return true;
}

namespace
{
    char const* TeleportDeniedReason(uint32 err)
    {
        switch (err)
        {
            case lfg::LFG_TELEPORTERROR_PLAYER_DEAD:
                return "I am dead";
            case lfg::LFG_TELEPORTERROR_FALLING:
                return "I am still falling";
            case lfg::LFG_TELEPORTERROR_IN_VEHICLE:
                return "I am stuck on a vehicle";
            case lfg::LFG_TELEPORTERROR_FATIGUE:
                return "I am out of breath";
            case lfg::LFG_TELEPORTERROR_INVALID_LOCATION:
                return "the way in is closed to me";
            case lfg::LFG_TELEPORTERROR_COMBAT:
                return "I am still in combat";
            default:
                return "the way in was refused";
        }
    }
}

bool LfgTeleportDeniedAction::Execute(Event event)
{
    WorldPacket p(event.getPacket());
    if (p.empty())
        return false;

    p.rpos(0);
    uint32 err = 0;
    p >> err;

    // The retry trigger fires every few seconds; only one of those refusals is worth hearing about.
    time_t const now = time(nullptr);
    if (_lastTold && now - _lastTold < 30)
        return false;

    _lastTold = now;

    std::map<std::string, std::string> placeholders;
    placeholders["%reason"] = TeleportDeniedReason(err);

    botAI->SayToParty(PlayerbotTextMgr::instance().GetBotTextOrDefault(
        "lfg_teleport_denied", "Cannot get in - %reason.", placeholders));

    return true;
}

bool LfgJoinAction::isUseful()
{
    if (!sPlayerbotAIConfig.RandomBotJoinLfg)
    {
        // botAI->ChangeStrategy("-lfg", BOT_STATE_NON_COMBAT);
        return false;
    }

    if (bot->GetLevel() < 15)
        return false;

    // Covers SelfBots and bots serving a human: neither should queue, accept a proposal and vanish
    // into an instance behind the player's back.
    if (botAI->HasGameClientMaster())
        return false;

    if (bot->GetGroup() && bot->GetGroup()->GetLeaderGUID() != bot->GetGUID())
    {
        // botAI->ChangeStrategy("-lfg", BOT_STATE_NON_COMBAT);
        return false;
    }

    if (bot->IsBeingTeleported())
        return false;

    if (bot->InBattleground())
        return false;

    if (bot->InBattlegroundQueue())
        return false;

    if (bot->isDead())
        return false;

    if (!RandomPlayerbotMgr::instance().IsRandomBot(bot))
        return false;

    Map* map = bot->GetMap();
    if (map && map->Instanceable())
        return false;

    LfgState state = sLFGMgr->GetState(bot->GetGUID());
    if (state != LFG_STATE_NONE)
        return false;

    return true;
}
