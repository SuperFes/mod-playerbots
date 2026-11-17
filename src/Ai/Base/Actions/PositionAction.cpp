/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "PositionAction.h"
#include "Event.h"
#include "Playerbots.h"
#include "PositionValue.h"

void TellPosition(PlayerbotAI* botAI, std::string const name, PositionInfo pos)
{
    std::ostringstream out;
    out << "Position " << name;

    if (pos.isSet())
    {
        float x = pos.x;
        float y = pos.y;
        Map2ZoneCoordinates(x, y, botAI->GetBot()->GetZoneId());
        out << " is set to " << x << "," << y;
    }
    else
        out << " is not set";

    botAI->TellMaster(out);
}

bool PositionAction::Execute(Event event)
{
    std::string const param = event.getParam();
    if (param.empty())
        return false;

    Player* master = GetMaster();
    if (!master)
        return false;

    PositionMap& posMap = context->GetValue<PositionMap&>("position")->Get();
    if (param == "?")
    {
        for (PositionMap::iterator i = posMap.begin(); i != posMap.end(); ++i)
        {
            if (i->second.isSet())
                TellPosition(botAI, i->first, i->second);
        }

        return true;
    }

    std::vector<std::string> params = split(param, ' ');
    if (params.size() != 2)
    {
        botAI->TellMaster("Whisper position <name> ?/set/reset");
        return false;
    }

    std::string const name = params[0];
    std::string const action = params[1];
    PositionInfo pos = posMap[name];
    if (action == "?")
    {
        TellPosition(botAI, name, pos);
        return true;
    }

    std::vector<std::string> coords = split(action, ',');
    if (coords.size() == 3)
    {
        pos.Set(atoi(coords[0].c_str()), atoi(coords[1].c_str()), atoi(coords[2].c_str()), botAI->GetBot()->GetMapId());
        posMap[name] = pos;

        std::ostringstream out;
        out << "Position " << name << " is set";
        botAI->TellMaster(out);
        return true;
    }

    if (action == "set")
    {
        pos.Set(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(), botAI->GetBot()->GetMapId());
        posMap[name] = pos;

        std::ostringstream out;
        out << "Position " << name << " is set";
        botAI->TellMaster(out);
        return true;
    }

    if (action == "reset")
    {
        pos.Reset();
        posMap[name] = pos;

        std::ostringstream out;
        out << "Position " << name << " is reset";
        botAI->TellMaster(out);
        return true;
    }

    return false;
}

bool MoveToPositionAction::Execute(Event /*event*/)
{
    PositionInfo pos = context->GetValue<PositionMap&>("position")->Get()[qualifier];
    if (!pos.isSet())
    {
        std::ostringstream out;
        out << "Position " << qualifier << " is not set";
        botAI->TellMaster(out);
        return false;
    }

    return MoveTo(bot->GetMapId(), pos.x, pos.y, pos.z, idle);
}

bool MoveToPositionAction::isUseful()
{
    PositionInfo pos = context->GetValue<PositionMap&>("position")->Get()[qualifier];
    float distance = AI_VALUE2(float, "distance", std::string("position_") + qualifier);
    return pos.isSet() && distance > sPlayerbotAIConfig.FollowDistance && distance < sPlayerbotAIConfig.ReactDistance;
}

bool SetReturnPositionAction::Execute(Event /*event*/)
{
    PositionMap& posMap = context->GetValue<PositionMap&>("position")->Get();
    PositionInfo returnPos = posMap["return"];
    PositionInfo randomPos = posMap["random"];
    if (returnPos.isSet() && !randomPos.isSet())
    {
        float angle = 2 * M_PI * urand(0, 1000) / 100.0f;
        float dist = sPlayerbotAIConfig.FollowDistance * urand(0, 1000) / 1000.0f;
        float x = returnPos.x + cos(angle) * dist;
        float y = returnPos.y + sin(angle) * dist;
        float z = bot->GetPositionZ();
        bot->UpdateAllowedPositionZ(x, y, z);

        if (!bot->IsWithinLOS(x, y, z))
            return false;

        randomPos.Set(x, y, z, bot->GetMapId());
        posMap["random"] = randomPos;
        return true;
    }

    return false;
}

bool SetReturnPositionAction::isUseful()
{
    PositionMap& posMap = context->GetValue<PositionMap&>("position")->Get();
    return posMap["return"].isSet() && !posMap["random"].isSet();
}

bool ReturnAction::isUseful()
{
    PositionInfo pos = context->GetValue<PositionMap&>("position")->Get()[qualifier];
    return pos.isSet() && AI_VALUE2(float, "distance", "position_random") > sPlayerbotAIConfig.FollowDistance;
}

bool ReturnToStayPositionAction::isUseful()
{
    // Holding a post and roaming a city freely are contradictory: whatever left a stay position on
    // this bot, it is not what the bot is doing right now.
    if (botAI->IsCityAutonomyActive())
        return false;

    return MoveToPositionAction::isUseful();
}

bool ReturnToStayPositionAction::isPossible()
{
    // A predicate only. This used to whisper the master and rewrite the stay position from in here,
    // but the engine calls isPossible() while merely weighing actions it may never run - so a bot
    // announced that it was staying put while it was in fact following, and re-anchored itself as a
    // side effect of being thought about.
    return AI_VALUE(PositionMap&, "position")["stay"].isSet();
}

bool ReturnToStayPositionAction::Execute(Event event)
{
    PositionMap& posMap = AI_VALUE(PositionMap&, "position");
    PositionInfo stayPosition = posMap["stay"];
    if (!stayPosition.isSet())
        return false;

    // On another map the coordinates are not far away, they are meaningless - GetDistance would be
    // comparing positions from two different coordinate spaces.
    bool const otherMap = stayPosition.mapId != bot->GetMapId();
    bool const tooFar = otherMap ||
        bot->GetDistance(stayPosition.x, stayPosition.y, stayPosition.z) > sPlayerbotAIConfig.ReactDistance;

    if (!tooFar)
        return MoveToPositionAction::Execute(event);

    LOG_DEBUG("playerbots", "Bot {} abandoning stay position ({}, {}, {} on map {}) - {} away, on {} map",
              bot->GetName(), stayPosition.x, stayPosition.y, stayPosition.z, stayPosition.mapId,
              otherMap ? -1.0f : bot->GetDistance(stayPosition.x, stayPosition.y, stayPosition.z),
              otherMap ? "a different" : "the same");

    stayPosition.Set(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(), bot->GetMapId());
    posMap["stay"] = stayPosition;

    botAI->TellMaster("The stay position is too far to return. I am going to stay where I am now");

    return true;
}
