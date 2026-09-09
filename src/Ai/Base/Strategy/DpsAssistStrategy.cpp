/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DpsAssistStrategy.h"

#include "Playerbots.h"

void DpsAssistStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // Healers pick targets one band below a critical heal (ACTION_CRITICAL_HEAL + 10 = 40), so
    // target acquisition can never preempt keeping someone alive.
    float const relevance = PlayerbotAI::IsHeal(botAI->GetBot(), true) ? 29.0f : 50.0f;

    triggers.push_back(
        new TriggerNode("not dps target active", { NextAction("dps assist", relevance) }));
}

void DpsAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("not dps aoe target active", { NextAction("dps aoe", 50.0f) }));
}
