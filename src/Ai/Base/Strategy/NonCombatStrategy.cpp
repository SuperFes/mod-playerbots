/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "NonCombatStrategy.h"

#include "PlayerbotAIConfig.h"

void NonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("random", { NextAction("clean quest log", 1.0f) }));
    // Must outrank "follow" below: a master riding away keeps "far from master" true, so at a lower
    // relevance the mount check never gets a tick and the bot chases on foot indefinitely.
    // The action returns false when there is nothing to mount or dismount, so it never steals a tick.
    triggers.push_back(new TriggerNode("timer", { NextAction("check mount state", 66.0f) }));
    triggers.push_back(new TriggerNode("far from master", { NextAction("follow", 65.0f) }));
}

void CollisionStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("collision", { NextAction("move out of collision", 2.0f) }));
}

void MountStrategy::InitTriggers(std::vector<TriggerNode*>& /*triggers*/)
{
}

void TradeskillStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    if (!sPlayerbotAIConfig.tradeskillWhenIdle && !sPlayerbotAIConfig.idleSit)
        return;

    // Low relevance throughout: anything the bot could be doing instead should win. These only get a
    // tick because nothing else wanted one.
    if (sPlayerbotAIConfig.tradeskillWhenIdle)
    {
        triggers.push_back(new TriggerNode("bot idle", { NextAction("craft random item", 1.2f) }));
        triggers.push_back(new TriggerNode("bot idle", { NextAction("use random recipe", 1.1f) }));
        triggers.push_back(new TriggerNode("bot idle", { NextAction("enchant random item", 1.0f) }));
        triggers.push_back(new TriggerNode("bot idle", { NextAction("disenchant random item", 0.9f) }));
    }

    // Last resort: nothing to craft, nothing to do, so sit down like a person would.
    if (sPlayerbotAIConfig.idleSit)
        triggers.push_back(new TriggerNode("bot idle", { NextAction("idle sit", 0.5f) }));
}

void WorldBuffStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode(
            "need world buff",
            {
                NextAction("world buff", 1.0f)
            }
        )
    );
}

void MasterFishingStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode(
            "very often",
            {
                NextAction("move near water" , 10.0f)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "very often",
            {
                NextAction("go fishing" , 10.0f)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "random",
            {
                NextAction("end master fishing", 12.0f),
                NextAction("equip upgrades packet action", 6.0f)
            }
        )
    );
}
