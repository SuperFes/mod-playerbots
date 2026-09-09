/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "Formations.h"

#include "Arrow.h"
#include "Event.h"
#include "Map.h"
#include "Playerbots.h"
#include "ServerFacade.h"

WorldLocation Formation::NullLocation = WorldLocation();

// Personal space is this fraction of the follow distance, and the slow drift is this fraction of
// that again. Both are bounded so Formation::GetMaxDistance() can state an honest tolerance - the
// follow logic and the dismount gate both read it.
static constexpr float FORMATION_SPREAD_FRACTION = 0.35f;
static constexpr float FORMATION_DRIFT_FRACTION = 0.25f;
static constexpr uint32 FORMATION_DRIFT_PERIOD_MIN = 7;
static constexpr uint32 FORMATION_DRIFT_PERIOD_MAX = 15;

// How much of a slot's angular width a bot may sit off-centre, so identical indices do not overlap.
static constexpr float SLOT_BIAS_FRACTION = 0.35f;

bool IsSameLocation(WorldLocation const& a, WorldLocation const& b)
{
    return a.GetPositionX() == b.GetPositionX() && a.GetPositionY() == b.GetPositionY() &&
           a.GetPositionZ() == b.GetPositionZ() && a.GetMapId() == b.GetMapId();
}

bool Formation::IsNullLocation(WorldLocation const& loc) { return IsSameLocation(loc, Formation::NullLocation); }

bool ValidateTargetContext(Unit* a, Unit* b, Map*& outMap)
{
    if (!a || !b || a == b)
        return false;

    if (!a->IsInWorld() || !b->IsInWorld())
        return false;

    if (a->IsDuringRemoveFromWorld() || b->IsDuringRemoveFromWorld())
        return false;

    Map* map = a->GetMap();
    if (!map || map != b->GetMap())
        return false;

    outMap = map;

    return true;
}

bool ValidateTargetContext(Unit* a, Unit* b)
{
    Map* unused = nullptr;
    return ValidateTargetContext(a, b, unused);
}

WorldLocation MoveAheadFormation::GetLocation()
{
    Player* master = GetMaster();
    if (!ValidateTargetContext(master, bot))
        return Formation::NullLocation;

    WorldLocation loc = GetLocationInternal();
    if (Formation::IsNullLocation(loc))
        return loc;

    float x = loc.GetPositionX();
    float y = loc.GetPositionY();
    float z = loc.GetPositionZ();

    // if (master->isMoving())
    // {
    //     float ori = master->GetOrientation();
    //     float x1 = x + sPlayerbotAIConfig.tooCloseDistance * cos(ori);
    //     float y1 = y + sPlayerbotAIConfig.tooCloseDistance * sin(ori);
    //     float ground = map->GetHeight(x1, y1, z);
    //     if (ground > INVALID_HEIGHT)
    //     {
    //         x = x1;
    //         y = y1;
    //     }
    // }

    // float ground = map->GetHeight(x, y, z);
    // if (ground <= INVALID_HEIGHT)
    //     return Formation::NullLocation;

    // z += CONTACT_DISTANCE;
    // bot->UpdateAllowedPositionZ(x, y, z);
    return WorldLocation(master->GetMapId(), x, y, z);
}

class MeleeFormation : public FollowFormation
{
public:
    MeleeFormation(PlayerbotAI* botAI) : FollowFormation(botAI, "melee") {}

    std::string const GetTargetName() override { return "group leader"; }
};

class QueueFormation : public FollowFormation
{
public:
    QueueFormation(PlayerbotAI* botAI) : FollowFormation(botAI, "queue") {}

    std::string const GetTargetName() override { return "line target"; }
};

// A bot's habitual place in the formation, expressed relative to the master's facing so "half a step
// off the left flank" stays true as the master turns. Scaled by followDistance rather than
// tooCloseDistance (a combat-spacing value) so it tracks the dial that actually sets party spread.
// personalSpace is sampled on the unit disc, so |offset| never exceeds spread.
void GetPersonalSpaceOffset(PlayerbotAI* botAI, Player* master, float driftX, float driftY,
                            float& outX, float& outY)
{
    BotPersonality const& personality = botAI->GetPersonality();
    float const spread = sPlayerbotAIConfig.followDistance * FORMATION_SPREAD_FRACTION;

    float const localX = personality.personalSpaceX * spread + driftX;
    float const localY = personality.personalSpaceY * spread + driftY;

    float const orientation = master->GetOrientation();
    outX = localX * std::cos(orientation) - localY * std::sin(orientation);
    outY = localX * std::sin(orientation) + localY * std::cos(orientation);
}

class NearFormation : public MoveAheadFormation
{
public:
    NearFormation(PlayerbotAI* botAI) : MoveAheadFormation(botAI, "near") {}

    WorldLocation GetLocationInternal() override
    {
        Player* master = GetMaster();
        Map* map = nullptr;
        if (!ValidateTargetContext(master, bot, map))
            return Formation::NullLocation;

        float range = sPlayerbotAIConfig.followDistance;
        float angle = GetFollowAngle();

        float offsetX = 0.0f;
        float offsetY = 0.0f;
        GetPersonalSpaceOffset(botAI, master, 0.0f, 0.0f, offsetX, offsetY);

        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        ResolveSlotOrQueue(master, map, angle, range, offsetX, offsetY, x, y, z);

        return WorldLocation(master->GetMapId(), x, y, z);
    }

    // Must cover the offset, or the bot would be judged out of position at its own standing spot.
    float GetMaxDistance() override
    {
        return sPlayerbotAIConfig.followDistance * (1.0f + FORMATION_SPREAD_FRACTION);
    }
};

class ChaosFormation : public MoveAheadFormation
{
public:
    ChaosFormation(PlayerbotAI* botAI) : MoveAheadFormation(botAI, "chaos"), lastChangeTime(0) {}

    WorldLocation GetLocationInternal() override
    {
        Player* master = GetMaster();
        Map* map = nullptr;
        if (!ValidateTargetContext(master, bot, map))
            return Formation::NullLocation;

        float range = sPlayerbotAIConfig.followDistance;
        float angle = GetFollowAngle();

        // Previously this re-rolled a +/- tooCloseDistance/2 offset every 3 seconds for every bot, which
        // at the default follow distance was a larger jitter than the ring itself - bots snapped between
        // grid cells rather than holding a position. Now the bulk of the offset is the bot's persistent
        // standing spot and only a small remainder drifts, on a staggered period so the group does not
        // shift on one beat.
        time_t now = time(nullptr);
        if (!lastChangeTime || now - lastChangeTime >= static_cast<time_t>(driftPeriod))
        {
            lastChangeTime = now;
            driftPeriod = urand(FORMATION_DRIFT_PERIOD_MIN, FORMATION_DRIFT_PERIOD_MAX);

            float const driftLimit =
                sPlayerbotAIConfig.followDistance * FORMATION_SPREAD_FRACTION * FORMATION_DRIFT_FRACTION;

            driftX = frand(-driftLimit, driftLimit);
            driftY = frand(-driftLimit, driftLimit);
        }

        GetPersonalSpaceOffset(botAI, master, driftX, driftY, dx, dy);
        dr = std::sqrt(dx * dx + dy * dy);

        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        ResolveSlotOrQueue(master, map, angle, range, dx, dy, x, y, z);

        return WorldLocation(master->GetMapId(), x, y, z);
    }

    float GetMaxDistance() override { return sPlayerbotAIConfig.followDistance + dr; }

private:
    time_t lastChangeTime;
    uint32 driftPeriod = FORMATION_DRIFT_PERIOD_MIN;
    float driftX = 0.f;
    float driftY = 0.f;
    float dx = 0.f;
    float dy = 0.f;
    float dr = 0.f;
};

class CircleFormation : public MoveFormation
{
public:
    CircleFormation(PlayerbotAI* botAI) : MoveFormation(botAI, "circle") {}

    WorldLocation GetLocation() override
    {
        Unit* target = AI_VALUE(Unit*, "current target");
        Player* master = GetMaster();

        // Fix: if no target OR target is the bot, fall back to master
        if (!target || target == bot)
            target = master;

        Map* map = nullptr;
        if (!ValidateTargetContext(master, bot, map))
            return Formation::NullLocation;

        float range = 2.0f;
        switch (bot->getClass())
        {
            case CLASS_HUNTER:
            case CLASS_MAGE:
            case CLASS_PRIEST:
            case CLASS_WARLOCK:
                range = botAI->GetRange("flee");
                break;
            case CLASS_DRUID:
                if (!botAI->IsTank(bot))
                    range = botAI->GetRange("flee");
                break;
            case CLASS_SHAMAN:
                if (botAI->IsHeal(bot))
                    range = botAI->GetRange("flee");
                break;
        }

        float angle = GetFollowAngle();
        float x = target->GetPositionX() + cos(angle) * range;
        float y = target->GetPositionY() + sin(angle) * range;
        float z = target->GetPositionZ();

        if (!map->CheckCollisionAndGetValidCoords(target, target->GetPositionX(), target->GetPositionY(),
                                                  target->GetPositionZ(), x, y, z))
        {
            x = target->GetPositionX() + cos(angle) * range;
            y = target->GetPositionY() + sin(angle) * range;
            z = target->GetPositionZ();
            target->UpdateAllowedPositionZ(x, y, z);
        }

        return WorldLocation(bot->GetMapId(), x, y, z);
    }
};

class LineFormation : public MoveAheadFormation
{
public:
    LineFormation(PlayerbotAI* botAI) : MoveAheadFormation(botAI, "line") {}

    WorldLocation GetLocationInternal() override
    {
        Group* group = bot->GetGroup();
        if (!group)
            return Formation::NullLocation;

        float range = 2.0f;

        Player* master = GetMaster();
        if (!master)
            return Formation::NullLocation;

        float x = master->GetPositionX();
        float y = master->GetPositionY();
        float z = master->GetPositionZ();
        float orientation = master->GetOrientation();

        std::vector<Player*> players;
        players.reserve(group->GetMembersCount());

        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->GetSource();
            if (!member || member == master)
                continue;

            players.push_back(member);
        }

        players.insert(players.begin() + players.size() / 2, master);
        return MoveLine(players, 0.0f, x, y, z, orientation, range);
    }
};

class ShieldFormation : public MoveFormation
{
public:
    ShieldFormation(PlayerbotAI* botAI) : MoveFormation(botAI, "shield") {}

    WorldLocation GetLocation() override
    {
        Group* group = bot->GetGroup();
        if (!group)
            return Formation::NullLocation;

        float range = sPlayerbotAIConfig.followDistance;

        Player* master = GetMaster();
        if (!master)
            return Formation::NullLocation;

        float x = master->GetPositionX();
        float y = master->GetPositionY();
        float z = master->GetPositionZ();
        float orientation = master->GetOrientation();

        std::vector<Player*> tanks;
        std::vector<Player*> dps;

        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->GetSource();
            if (!member || member == master)
                continue;

            if (botAI->IsTank(member))
                tanks.push_back(member);
            else
                dps.push_back(member);
        }

        if (botAI->IsTank(master))
            tanks.insert(tanks.begin() + (tanks.size() + 1) / 2, master);
        else
            dps.insert(dps.begin() + (dps.size() + 1) / 2, master);

        if (botAI->IsTank(bot) && botAI->IsTank(master))
            return MoveLine(tanks, 0.0f, x, y, z, orientation, range);

        if (!botAI->IsTank(bot) && !botAI->IsTank(master))
            return MoveLine(dps, 0.0f, x, y, z, orientation, range);

        if (botAI->IsTank(bot) && !botAI->IsTank(master))
        {
            float diff = (tanks.size() % 2 == 0) ? -sPlayerbotAIConfig.tooCloseDistance / 2.0f : 0.0f;
            return MoveLine(tanks, diff, x + cos(orientation) * range, y + sin(orientation) * range, z, orientation,
                            range);
        }

        if (!botAI->IsTank(bot) && botAI->IsTank(master))
        {
            float diff = (dps.size() % 2 == 0) ? -sPlayerbotAIConfig.tooCloseDistance / 2.0f : 0.0f;
            return MoveLine(dps, diff, x - cos(orientation) * range, y - sin(orientation) * range, z, orientation,
                            range);
        }

        return Formation::NullLocation;
    }
};

class FarFormation : public FollowFormation
{
public:
    FarFormation(PlayerbotAI* botAI) : FollowFormation(botAI, "far") {}

    WorldLocation GetLocation() override
    {
        Player* master = GetMaster();
        Map* map = nullptr;
        if (!ValidateTargetContext(master, bot, map))
            return Formation::NullLocation;

        float range = sPlayerbotAIConfig.farDistance;
        float followRange = sPlayerbotAIConfig.followDistance;

        if (ServerFacade::instance().GetDistance2d(bot, master) <= range)
            return Formation::NullLocation;

        float angleToBot = master->GetAngle(bot);
        float followAngle = GetFollowAngle();

        float x = master->GetPositionX() + cos(angleToBot) * range + cos(followAngle) * followRange;
        float y = master->GetPositionY() + sin(angleToBot) * range + sin(followAngle) * followRange;
        float z = master->GetPositionZ();

        float ground = master->GetMapHeight(x, y, z + 30.0f);
        if (ground <= INVALID_HEIGHT)
        {
            float minDist = 0.f;
            float minX = 0.f, minY = 0.f;

            for (float a = 0.0f; a <= 2 * M_PI; a += M_PI / 16.0f)
            {
                float tx = master->GetPositionX() + cos(a) * range + cos(followAngle) * followRange;
                float ty = master->GetPositionY() + sin(a) * range + sin(followAngle) * followRange;

                float dist = ServerFacade::instance().GetDistance2d(bot, tx, ty);
                float tg = master->GetMapHeight(tx, ty, z + 30.0f);

                if (tg > INVALID_HEIGHT && (!minDist || dist < minDist))
                {
                    minDist = dist;
                    minX = tx;
                    minY = ty;
                }
            }

            if (!minDist)
                return Formation::NullLocation;

            float lz = z;
            if (!map->CheckCollisionAndGetValidCoords(master, master->GetPositionX(), master->GetPositionY(),
                                                      master->GetPositionZ(), minX, minY, lz))
            {
                lz = z + master->GetHoverHeight();
                master->UpdateAllowedPositionZ(minX, minY, lz);
            }

            return WorldLocation(bot->GetMapId(), minX, minY, lz);
        }

        if (!map->CheckCollisionAndGetValidCoords(master, master->GetPositionX(), master->GetPositionY(),
                                                  master->GetPositionZ(), x, y, z))
        {
            x = master->GetPositionX() + cos(angleToBot) * range + cos(followAngle) * followRange;
            y = master->GetPositionY() + sin(angleToBot) * range + sin(followAngle) * followRange;
            z = master->GetPositionZ() + master->GetHoverHeight();
            master->UpdateAllowedPositionZ(x, y, z);
        }

        return WorldLocation(bot->GetMapId(), x, y, z);
    }
};

// Membership key for the slot cache: the ordered set of group member GUIDs plus the master. Alive
// state and map are deliberately excluded - a death or a zone must not reshuffle everyone's slot.
uint64 Formation::GetRosterKey(Player* master, Group* group) const
{
    uint64 key = master ? master->GetGUID().GetRawValue() : 0;

    if (group)
    {
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            if (Player* member = ref->GetSource())
            {
                // Order-independent mix so iteration order cannot invalidate the cache on its own.
                key ^= member->GetGUID().GetRawValue() * 0x9E3779B97F4A7C15ull;
            }
        }
    }

    return key;
}

float Formation::GetFollowAngle()
{
    Player* master = GetMaster();
    Group* group = bot->GetGroup();
    PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);

    // If there's no master and no group
    if (!master && !group)
        return 0.0f;

    float const start = (master ? master->GetOrientation() : 0.0f);

    // The slot only changes when membership does, so recompute on change and reuse otherwise. This is
    // what stops the party swivelling every time someone dies, zones, or is resurrected.
    uint64 const rosterKey = GetRosterKey(master, group);
    if (_slotCacheValid && rosterKey == _cachedRosterKey)
        return start + _cachedSlotOffset;

    uint32 index = 1;
    uint32 total = 1;

    std::vector<Player*> roster;

    if (group)
    {
        bool left = true;  // Used for alternating tanks' positions

        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->GetSource();

            // Skip invalid members only. Dead and out-of-map members keep their slot: removing them
            // renumbers everyone behind them and the whole formation rotates at once.
            if (!member)
                continue;

            // Skip the master
            if (member == master)
                continue;

            // Put DPS in the middle
            if (!botAI->IsTank(member) && !botAI->IsHeal(member))
            {
                roster.insert(roster.begin() + roster.size() / 2, member);
            }

            // Put Healers in the middle
            else if (botAI->IsHeal(member))
            {
                roster.insert(roster.begin() + roster.size() / 2, member);
            }

            // Handle tanks (alternate between front and back)
            else if (botAI->IsTank(member))
            {
                if (left)
                    roster.push_back(member);  // Place tank at the back
                else
                    roster.insert(roster.begin(), member);  // Place tank at the front

                left = !left;  // Alternate for the next tank
            }

            total++;
        }
    }
    else if (master)
    {
        // If the bot is following a master, look up the bot's position in the master's list
        PlayerbotMgr* masterBotMgr = GET_PLAYERBOT_MGR(master);
        if (masterBotMgr && !GET_PLAYERBOT_AI(master))
        {
            for (auto it = masterBotMgr->GetPlayerBotsBegin(); it != masterBotMgr->GetPlayerBotsEnd(); ++it)
            {
                if (it->second == bot)
                {
                    index = total;  // Found bot in master's list, set the index
                    break;
                }
                ++total;
            }
        }
    }

    // Find the bot's position in the roster
    auto it = std::find(roster.begin(), roster.end(), bot);
    if (it != roster.end())
    {
        index = std::distance(roster.begin(), it) + 1;  // Find bot's index in the roster
    }

    float offset = (0.125f + 1.75f * index / total + (total == 2 ? 0.125f : 0.0f)) * M_PI;

    // Nudge within the slot so two bots that resolve to the same index do not stand exactly on top of
    // each other. Stable per bot, so it does not read as drift.
    if (botAI)
    {
        float const slotWidth = static_cast<float>(M_PI) * 1.75f / static_cast<float>(std::max(total, 1u));
        float const bias = (botAI->GetPersonality().slotBias % 1000u) / 1000.0f - 0.5f;
        offset += bias * slotWidth * SLOT_BIAS_FRACTION;
    }

    _cachedRosterKey = rosterKey;
    _cachedSlotOffset = offset;
    _cachedSlotIndex = index;
    _slotCacheValid = true;

    return start + offset;
}

// How long a bot keeps queueing after its flank clears. Without this the raycast flickering at
// geometry edges would bounce a bot between the flank and the line every tick.
static constexpr time_t CONFINEMENT_HOLD_SECONDS = 4;

bool Formation::ResolveSlotOrQueue(Player* master, Map* map, float angle, float range, float dx,
                                   float dy, float& x, float& y, float& z)
{
    float const mx = master->GetPositionX();
    float const my = master->GetPositionY();
    float const mz = master->GetPositionZ();

    time_t const now = time(nullptr);
    bool const stillConfined = now < _confinedUntil;

    if (!stillConfined)
    {
        x = mx + std::cos(angle) * range + dx;
        y = my + std::sin(angle) * range + dy;
        z = mz + master->GetHoverHeight();

        // On success this writes back a collision-adjusted position, which is exactly what we want.
        if (map->CheckCollisionAndGetValidCoords(master, mx, my, mz, x, y, z))
            return true;

        // The flank is unreachable - a wall, a bridge railing, a corridor. Previously this branch
        // recomputed the same blocked point and shoved the bot at it anyway.
        _confinedUntil = now + CONFINEMENT_HOLD_SECONDS;
    }

    // Fall in behind the master, spaced by follow order so the group forms a line rather than a
    // pile. Index is stable while group membership is, so bots hold their place in the queue.
    float const behind = master->GetOrientation() + static_cast<float>(M_PI);
    float const spacing = range * (1.0f + static_cast<float>(GetSlotIndex()));

    x = mx + std::cos(behind) * spacing;
    y = my + std::sin(behind) * spacing;
    z = mz + master->GetHoverHeight();

    if (map->CheckCollisionAndGetValidCoords(master, mx, my, mz, x, y, z))
        return true;

    // Even the queue is blocked, so stand where the master is standing; it is by definition
    // reachable, and the collision avoidance will sort out the overlap.
    x = mx;
    y = my;
    z = mz;

    return false;
}

FormationValue::FormationValue(PlayerbotAI* botAI)
    : ManualSetValue<Formation*>(botAI, new ChaosFormation(botAI), "formation")
{
}

FormationValue::~FormationValue()
{
    if (value)
    {
        delete value;
        value = nullptr;
    }
}

std::string const FormationValue::Save() { return value ? value->getName() : "?"; }

bool FormationValue::Load(std::string const formation)
{
    if (formation == "melee")
    {
        if (value)
            delete value;

        value = new MeleeFormation(botAI);
    }
    else if (formation == "queue")
    {
        if (value)
            delete value;

        value = new QueueFormation(botAI);
    }
    else if (formation == "chaos" || formation == "default")
    {
        if (value)
            delete value;

        value = new ChaosFormation(botAI);
    }
    else if (formation == "circle")
    {
        if (value)
            delete value;

        value = new CircleFormation(botAI);
    }
    else if (formation == "line")
    {
        if (value)
            delete value;

        value = new LineFormation(botAI);
    }
    else if (formation == "shield")
    {
        if (value)
            delete value;

        value = new ShieldFormation(botAI);
    }
    else if (formation == "arrow")
    {
        if (value)
            delete value;

        value = new ArrowFormation(botAI);
    }
    else if (formation == "near")
    {
        if (value)
            delete value;

        value = new NearFormation(botAI);
    }
    else if (formation == "far")
    {
        if (value)
            delete value;

        value = new FarFormation(botAI);
    }
    else
        return false;

    return true;
}

bool SetFormationAction::Execute(Event event)
{
    std::string const formation = event.getParam();

    FormationValue* value = (FormationValue*)context->GetValue<Formation*>("formation");
    if (formation == "?" || formation.empty())
    {
        std::ostringstream str;
        str << "Formation: |cff00ff00" << value->Get()->getName();
        botAI->TellMaster(str);
        return true;
    }

    if (formation == "show")
    {
        WorldLocation loc = value->Get()->GetLocation();
        if (!Formation::IsNullLocation(loc))
            botAI->Ping(loc.GetPositionX(), loc.GetPositionY());

        return true;
    }

    if (!value->Load(formation))
    {
        std::ostringstream str;
        str << "Invalid formation: |cffff0000" << formation;
        botAI->TellMaster(str);
        botAI->TellMaster(
            "Please set to any of:|cffffffff chaos (default), near, queue, circle, line, shield, arrow, melee, far");
        return false;
    }

    std::ostringstream str;
    str << "Formation set to: " << formation;
    botAI->TellMaster(str);
    return true;
}

WorldLocation MoveFormation::MoveLine(std::vector<Player*> line, float diff, float cx, float cy, float cz,
                                      float orientation, float range)
{
    if (line.size() < 5)
    {
        return MoveSingleLine(line, diff, cx, cy, cz, orientation, range);
    }

    uint32 lines = ceil((double)line.size() / 5.0);
    for (uint32 i = 0; i < lines; i++)
    {
        float radius = range * i;
        float x = cx + cos(orientation) * radius;
        float y = cy + sin(orientation) * radius;

        std::vector<Player*> singleLine;
        for (uint32 j = 0; j < 5 && !line.empty(); j++)
        {
            singleLine.push_back(line[line.size() - 1]);
            line.pop_back();
        }

        WorldLocation loc = MoveSingleLine(singleLine, diff, x, y, cz, orientation, range);
        if (!Formation::IsNullLocation(loc))
            return loc;
    }

    return Formation::NullLocation;
}

WorldLocation MoveFormation::MoveSingleLine(std::vector<Player*> line, float diff, float cx, float cy, float cz,
                                            float orientation, float range)
{
    float count = line.size();
    float angleLeft = orientation - M_PI / 2.0f;
    float x0 = cx + std::cos(angleLeft) * (range * std::floor(count / 2.0f) + diff);
    float y0 = cy + std::sin(angleLeft) * (range * std::floor(count / 2.0f) + diff);

    uint32 index = 0;
    for (Player* member : line)
    {
        if (member == bot)
        {
            float angleRight = orientation + M_PI / 2.0f;
            float radius = range * index;

            float lx = x0 + std::cos(angleRight) * radius;
            float ly = y0 + std::sin(angleRight) * radius;
            float lz = cz;

            Player* master = botAI->GetMaster();
            Map* map = master ? master->GetMap() : nullptr;

            // if not fully in world ignore collision corrections.
            if (!master || !map || !bot || map != bot->GetMap() || !master->IsInWorld() ||
                master->IsDuringRemoveFromWorld() || !bot->IsInWorld() || bot->IsDuringRemoveFromWorld())
            {
                return WorldLocation(bot->GetMapId(), lx, ly, lz);
            }

            // if fully loaded check collision and applies coordinate corrections if needed
            map->CheckCollisionAndGetValidCoords(master, master->GetPositionX(), master->GetPositionY(),
                                                 master->GetPositionZ(), lx, ly, lz);

            return WorldLocation(bot->GetMapId(), lx, ly, lz);
        }

        ++index;
    }

    return Formation::NullLocation;
}
