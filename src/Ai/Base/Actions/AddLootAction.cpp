/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "AddLootAction.h"

#include "CellImpl.h"
#include "Corpse.h"
#include "Event.h"
#include "GameObject.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "LootObjectStack.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "ServerFacade.h"

bool AddLootAction::Execute(Event event)
{
    ObjectGuid guid = event.getObject();
    if (!guid)
        return false;

    GameObject* go = botAI->GetGameObject(guid);
    if (!go || go->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER ||
        sPlayerbotAIConfig.DisallowedGameObjects.contains(go->GetEntry()))
        return false;

    LootObject loot(bot, guid);
    if (loot.IsEmpty() || !loot.IsLootPossible(bot))
        return false;

    return AI_VALUE(LootObjectStack*, "available loot")->Add(guid);
}

bool AddAllLootAction::Execute(Event /*event*/)
{
    bool added = false;

    GuidVector gos = context->GetValue<GuidVector>("nearest game objects")->Get();
    for (GuidVector::iterator i = gos.begin(); i != gos.end(); i++)
        added |= AddLoot(*i);

    GuidVector corpses = context->GetValue<GuidVector>("nearest corpses")->Get();
    for (GuidVector::iterator i = corpses.begin(); i != corpses.end(); i++)
        added |= AddLoot(*i);

    // Add player's own corpse if it has loot (e.g. from tombstone/corpse recovery modules)
    if (bot->IsAlive())
    {
        Corpse* playerCorpse = bot->GetCorpse();
        if (playerCorpse && playerCorpse->IsInWorld() && playerCorpse->GetMapId() == bot->GetMapId())
        {
            added |= AddLoot(playerCorpse->GetGUID());
        }
    }

    return added;
}

bool AddLootAction::isUseful() { return true; }

bool AddAllLootAction::isUseful() { return true; }

bool AddAllLootAction::AddLoot(ObjectGuid guid)
{
    if (guid.IsGameObject() && sPlayerbotAIConfig.DisallowedGameObjects.contains(guid.GetEntry()))
        return false;

    return AI_VALUE(LootObjectStack*, "available loot")->Add(guid);
}

bool AddGatheringLootAction::AddLoot(ObjectGuid guid)
{
    LootObject loot(bot, guid);
    if (loot.IsEmpty() || !loot.IsLootPossible(bot) || loot.skillId == SKILL_NONE)
        return false;

    return AddAllLootAction::AddLoot(guid);
}
