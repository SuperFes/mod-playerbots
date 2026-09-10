/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "CityVendorMgr.h"

#include <vector>

#include "Player.h"
#include "Playerbots.h"

void CityVendorMgr::LoadAll()
{
    _byMap.clear();

    // UNIT_NPC_FLAG_VENDOR. Continent maps only: a bot is never strolling around an instance, and
    // excluding them keeps the index to the settlements that matter.
    QueryResult result = WorldDatabase.Query(
        "SELECT c.map, c.position_x, c.position_y, c.position_z "
        "FROM creature c JOIN creature_template ct ON ct.entry = c.id "
        "WHERE (ct.npcflag & 128) AND c.map IN (0, 1, 530, 571)");

    if (!result)
    {
        LOG_INFO("playerbots", "CityVendorMgr: no vendor spawns found, bots will wander aimlessly instead");
        return;
    }

    uint32 total = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 const mapId = fields[0].Get<uint32>();

        Spot spot;
        spot.x = fields[1].Get<float>();
        spot.y = fields[2].Get<float>();
        spot.z = fields[3].Get<float>();

        _byMap[mapId].push_back(spot);
        ++total;
    } while (result->NextRow());

    LOG_INFO("playerbots", "CityVendorMgr: {} vendor locations across {} maps", total, _byMap.size());
}

bool CityVendorMgr::PickNearby(Player* bot, float maxDistance, Spot& out) const
{
    if (!bot)
        return false;

    auto const it = _byMap.find(bot->GetMapId());
    if (it == _byMap.end())
        return false;

    float const botX = bot->GetPositionX();
    float const botY = bot->GetPositionY();
    float const maxSq = maxDistance * maxDistance;

    // Collect rather than take the nearest: always walking to the closest shop would send every bot
    // to the same doorway, and they would look like a queue rather than a crowd.
    std::vector<Spot const*> candidates;
    for (Spot const& spot : it->second)
    {
        float const dx = spot.x - botX;
        float const dy = spot.y - botY;

        if (dx * dx + dy * dy <= maxSq)
            candidates.push_back(&spot);
    }

    if (candidates.empty())
        return false;

    out = *candidates[urand(0, candidates.size() - 1)];

    return true;
}
