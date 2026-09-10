/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ChatShortcutActions.h"

#include "Event.h"
#include "Formations.h"
#include "PlayerbotTextMgr.h"
#include "Playerbots.h"
#include "PositionValue.h"
#include "ServerFacade.h"

static std::map<std::string, std::string> GetDefaultPlaceholders(PlayerbotAI* botAI, Player* bot, Player* master)
{
    std::map<std::string, std::string> placeholders;
    AreaTableEntry const* current_area = botAI->GetCurrentArea();
    AreaTableEntry const* current_zone = botAI->GetCurrentZone();
    placeholders["%area_name"] = current_area ? PlayerbotAI::GetLocalizedAreaName(current_area) : PlayerbotTextMgr::instance().GetBotText("string_unknown_area");
    placeholders["%zone_name"] = current_zone ? PlayerbotAI::GetLocalizedAreaName(current_zone) : PlayerbotTextMgr::instance().GetBotText("string_unknown_area");
    placeholders["%my_class"] = botAI->GetChatHelper()->FormatClass(bot->getClass());
    placeholders["%my_race"] = botAI->GetChatHelper()->FormatRace(bot->getRace());
    placeholders["%my_level"] = std::to_string(bot->GetLevel());

    if (master)
    {
        Unit* target = botAI->GetUnit(master->GetTarget());
        if (!target)
            target = bot->GetSelectedUnit();
        placeholders["%target_name"] = target ? target->GetName() : "target";
    }
    else
    {
        placeholders["%target_name"] = "target";
    }

    return placeholders;
}

void PositionsResetAction::ResetReturnPosition()
{
    PositionMap& posMap = context->GetValue<PositionMap&>("position")->Get();
    PositionInfo pos = posMap["return"];
    pos.Reset();
    posMap["return"] = pos;
}

void PositionsResetAction::SetReturnPosition(float x, float y, float z)
{
    PositionMap& posMap = context->GetValue<PositionMap&>("position")->Get();
    PositionInfo pos = posMap["return"];
    pos.Set(x, y, z, botAI->GetBot()->GetMapId());
    posMap["return"] = pos;
}

void PositionsResetAction::ResetStayPosition()
{
    PositionMap& posMap = context->GetValue<PositionMap&>("position")->Get();
    PositionInfo pos = posMap["stay"];
    pos.Reset();
    posMap["stay"] = pos;
}

void PositionsResetAction::SetStayPosition(float x, float y, float z)
{
    PositionMap& posMap = context->GetValue<PositionMap&>("position")->Get();
    PositionInfo pos = posMap["stay"];
    pos.Set(x, y, z, botAI->GetBot()->GetMapId());
    posMap["stay"] = pos;
}

bool FollowChatShortcutAction::Execute(Event /*event*/)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    // botAI->Reset();
    // -stay belongs on both lines. Leaving it set out of combat meant a bot told to follow kept the
    // stay strategy, and anything that re-anchors a stay position (SeeSpellAction does) brought the
    // whole return-to-stay behaviour back from the dead.
    botAI->ChangeStrategy("+follow,-stay,-passive,-grind,-move from group", BOT_STATE_NON_COMBAT);
    botAI->ChangeStrategy("-stay,-follow,-passive,-grind,-move from group", BOT_STATE_COMBAT);
    botAI->GetAiObjectContext()->GetValue<GuidVector>("prioritized targets")->Reset();

    PositionMap& posMap = context->GetValue<PositionMap&>("position")->Get();
    PositionInfo pos = posMap["return"];
    pos.Reset();
    posMap["return"] = pos;

    pos = posMap["stay"];
    pos.Reset();
    posMap["stay"] = pos;

    if (bot->IsInCombat())
    {
        Formation* formation = AI_VALUE(Formation*, "formation");
        std::string const target = formation->GetTargetName();
        bool moved = false;
        if (!target.empty())
            moved = Follow(AI_VALUE(Unit*, target));
        else
        {
            WorldLocation loc = formation->GetLocation();
            if (Formation::IsNullLocation(loc) || loc.GetMapId() == MAPID_INVALID)
                return false;

            MovementPriority priority = botAI->GetState() == BOT_STATE_COMBAT ? MovementPriority::MOVEMENT_COMBAT : MovementPriority::MOVEMENT_NORMAL;
            moved = MoveTo(loc.GetMapId(), loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ(), false, false, false,
                        true, priority);
        }

        if (bot->GetPet())
            botAI->PetFollow();

        if (moved && ServerFacade::instance().IsDistanceLessOrEqualThan(bot->GetDistance(master), sPlayerbotAIConfig.farDistance))
        {
            botAI->TellMaster(PlayerbotTextMgr::instance().GetBotTextOrDefault(
                "following", "Following", GetDefaultPlaceholders(botAI, bot, master)));
            return true;
        }
    }

    if (master)
    {
        if (bot->isDead())
        {
            bot->ResurrectPlayer(1.0f, false);
            botAI->TellMasterNoFacing("Back from the grave!");
        }

        bot->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TELEPORTED | AURA_INTERRUPT_FLAG_CHANGE_MAP);
        bot->TeleportTo(master->GetMapId(), master->GetPositionX(), master->GetPositionY(), master->GetPositionZ(), master->GetOrientation());
        botAI->TellMaster(PlayerbotTextMgr::instance().GetBotTextOrDefault(
            "following", "Following", GetDefaultPlaceholders(botAI, bot, master)));
        return true;
    }

    botAI->TellMaster(PlayerbotTextMgr::instance().GetBotTextOrDefault(
        "following", "Following", GetDefaultPlaceholders(botAI, bot, master)));
    return true;
}

bool StayChatShortcutAction::Execute(Event /*event*/)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    botAI->Reset();
    botAI->ChangeStrategy("+stay,-passive,-move from group", BOT_STATE_NON_COMBAT);
    botAI->ChangeStrategy("+stay,-follow,-passive,-move from group", BOT_STATE_COMBAT);

    SetReturnPosition(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ());
    SetStayPosition(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ());

    botAI->TellMaster(PlayerbotTextMgr::instance().GetBotTextOrDefault(
        "staying", "Staying", GetDefaultPlaceholders(botAI, bot, master)));
    return true;
}

bool MoveFromGroupChatShortcutAction::Execute(Event /*event*/)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    // dont need to remove stay or follow, move from group takes priority over both
    // (see their isUseful() methods)
    botAI->ChangeStrategy("+move from group", BOT_STATE_NON_COMBAT);
    botAI->ChangeStrategy("+move from group", BOT_STATE_COMBAT);

    botAI->TellMaster(PlayerbotTextMgr::instance().GetBotTextOrDefault(
        "move_from_group", "Moving away from group", GetDefaultPlaceholders(botAI, bot, master)));
    return true;
}

bool FleeChatShortcutAction::Execute(Event /*event*/)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    botAI->Reset();
    botAI->ChangeStrategy("+follow,-stay,+passive", BOT_STATE_NON_COMBAT);
    botAI->ChangeStrategy("+follow,-stay,+passive", BOT_STATE_COMBAT);

    ResetReturnPosition();
    ResetStayPosition();

    if (bot->GetMapId() != master->GetMapId() || bot->GetDistance(master) > sPlayerbotAIConfig.sightDistance)
    {
        botAI->TellError(PlayerbotTextMgr::instance().GetBotTextOrDefault(
            "fleeing_far", "I will not flee with you - too far away", GetDefaultPlaceholders(botAI, bot, master)));
        return true;
    }

    botAI->TellMaster(PlayerbotTextMgr::instance().GetBotTextOrDefault(
        "fleeing", "Fleeing", GetDefaultPlaceholders(botAI, bot, master)));
    return true;
}

bool GoawayChatShortcutAction::Execute(Event /*event*/)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    botAI->Reset();
    botAI->ChangeStrategy("+runaway,-stay", BOT_STATE_NON_COMBAT);
    botAI->ChangeStrategy("+runaway,-stay", BOT_STATE_COMBAT);

    ResetReturnPosition();
    ResetStayPosition();

    botAI->TellMaster(PlayerbotTextMgr::instance().GetBotTextOrDefault(
        "running_away", "Running away", GetDefaultPlaceholders(botAI, bot, master)));
    return true;
}

bool GrindChatShortcutAction::Execute(Event /*event*/)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    botAI->Reset();
    botAI->ChangeStrategy("+grind,-passive,-stay", BOT_STATE_NON_COMBAT);

    ResetReturnPosition();
    ResetStayPosition();

    botAI->TellMaster(PlayerbotTextMgr::instance().GetBotTextOrDefault(
        "grinding", "Grinding", GetDefaultPlaceholders(botAI, bot, master)));
    return true;
}

bool TankAttackChatShortcutAction::Execute(Event /*event*/)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    if (!botAI->IsTank(bot))
        return false;

    botAI->Reset();
    botAI->ChangeStrategy("-passive", BOT_STATE_NON_COMBAT);
    botAI->ChangeStrategy("-passive", BOT_STATE_COMBAT);

    ResetReturnPosition();
    ResetStayPosition();

    std::map<std::string, std::string> placeholders = GetDefaultPlaceholders(botAI, bot, master);
    Unit* attackTarget = master->GetSelectedUnit();
    if (!attackTarget)
        attackTarget = bot->GetVictim();

    placeholders["%target_name"] = attackTarget
        ? attackTarget->GetName()
        : PlayerbotTextMgr::instance().GetBotTextOrDefault("string_unknown_target", "our target", {});

    botAI->TellMaster(PlayerbotTextMgr::instance().GetBotTextOrDefault(
        "attacking", "Attacking", placeholders));
    return true;
}

bool MaxDpsChatShortcutAction::Execute(Event /*event*/)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    if (!botAI->ContainsStrategy(STRATEGY_TYPE_DPS))
        return false;

    botAI->Reset();

    botAI->ChangeStrategy("-threat,-conserve mana,-cast time,+dps debuff,+boost", BOT_STATE_COMBAT);
    botAI->TellMaster("Max DPS!");

    return true;
}

bool NaxxChatShortcutAction::Execute(Event /*event*/)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    botAI->Reset();
    botAI->ChangeStrategy("+naxx", BOT_STATE_NON_COMBAT);
    botAI->ChangeStrategy("+naxx", BOT_STATE_COMBAT);
    botAI->TellMasterNoFacing("Add Naxx Strategies!");
    // bot->Say("Add Naxx Strategies!", LANG_UNIVERSAL);
    return true;
}

bool BwlChatShortcutAction::Execute(Event /*event*/)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    botAI->Reset();
    botAI->ChangeStrategy("+bwl", BOT_STATE_NON_COMBAT);
    botAI->ChangeStrategy("+bwl", BOT_STATE_COMBAT);
    botAI->TellMasterNoFacing("Add Bwl Strategies!");
    return true;
}
