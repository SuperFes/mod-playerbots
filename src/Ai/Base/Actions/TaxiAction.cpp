/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "TaxiAction.h"

#include "Event.h"
#include "LastMovementValue.h"
#include "PlayerbotTextMgr.h"
#include "Playerbots.h"
#include "PlayerbotAIConfig.h"
#include "Config.h"

bool TaxiAction::Execute(Event event)
{
    botAI->RemoveShapeshift();

    LastMovement& movement = context->GetValue<LastMovement&>("last taxi")->Get();

    WorldPacket& p = event.getPacket();
    std::string const param = event.getParam();
    if ((!p.empty() && (p.GetOpcode() == CMSG_TAXICLEARALLNODES || p.GetOpcode() == CMSG_TAXICLEARNODE)) ||
        param == "clear")
    {
        movement.taxiNodes.clear();
        movement.Set(nullptr);
        botAI->SaySocial(PlayerbotTextMgr::instance().GetBotTextOrDefault(
            "taxi_ready_next_flight", "I am ready for the next flight", {}));
        return true;
    }

    GuidVector units = *context->GetValue<GuidVector>("nearest npcs");
    Creature* npc = nullptr;
    for (ObjectGuid const guid : units)
    {
        Creature* npcTemp = ObjectAccessor::GetCreature(*bot, guid);
        if (!npcTemp || !npcTemp->IsAlive())
            continue;

        if (!(npcTemp->GetNpcFlags() & UNIT_NPC_FLAG_FLIGHTMASTER))
            continue;

        if (bot->GetDistance(npcTemp) <= sPlayerbotAIConfig.farDistance)
        {
            npc = npcTemp;
            break;
        }
    }

    if (!npc && movement.taxiMaster)
    {
        Creature* npcTemp = ObjectAccessor::GetCreature(*bot, movement.taxiMaster);
        if (npcTemp && npcTemp->IsAlive() && (npcTemp->GetNpcFlags() & UNIT_NPC_FLAG_FLIGHTMASTER))
        {
            npc = npcTemp;
        }
    }

    if (npc)
    {
        if (bot->GetDistance(npc) > INTERACTION_DISTANCE + 2.0f)
        {
            bot->NearTeleportTo(npc->GetPositionX(), npc->GetPositionY(), npc->GetPositionZ(), npc->GetOrientation());
        }

        bot->GetSession()->SendLearnNewTaxiNode(npc);
        uint32 curloc = sObjectMgr->GetNearestTaxiNode(npc->GetPositionX(), npc->GetPositionY(), npc->GetPositionZ(),
                                                       npc->GetMapId(), bot->GetTeamId());

        std::vector<uint32> nodes;
        for (uint32 i = 0; i < sTaxiPathStore.GetNumRows(); ++i)
        {
            if (TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(i))
                if (entry->from == curloc)
                {
                    uint8 field = uint8((i - 1) / 32);
                    if (field < TaxiMaskSize)
                        nodes.push_back(i);
                }
        }

        // Only for follower bots
        if (botAI->HasRealPlayerMaster())
        {
            uint32 index = botAI->GetGroupSlotIndex(bot);
            uint32 delay = sPlayerbotAIConfig.botTaxiDelayMin +
                          index * sPlayerbotAIConfig.botTaxiGapMs +
                          urand(0, sPlayerbotAIConfig.botTaxiGapJitterMs);

            delay = std::min(delay, sPlayerbotAIConfig.botTaxiDelayMax);

            // Copy the route rather than capturing the LastMovement reference: this runs up to several
            // seconds later, by which point the master may have cleared it.
            std::vector<uint32> const nodes = movement.taxiNodes;
            ObjectGuid const botGuid = bot->GetGUID();
            ObjectGuid const npcGuid = npc->GetGUID();

            // schedule the take-off
            botAI->AddTimedEvent(
                [botGuid, npcGuid, nodes]() -> void
                {
                    // Resolve from the GUID - a raw Player* would dangle if the bot logged out or was
                    // removed during the stagger delay.
                    Player* self = ObjectAccessor::FindPlayer(botGuid);
                    if (!self)
                        return;

                    PlayerbotAI* selfAI = GET_PLAYERBOT_AI(self);
                    if (!selfAI)
                        return;

                    if (nodes.size() < 2)
                    {
                        selfAI->SaySocial(PlayerbotTextMgr::instance().GetBotTextOrDefault(
                            "taxi_no_route_remembered", "I did not catch which flight you took", {}));
                        return;
                    }

                    if (self->IsInCombat())
                    {
                        selfAI->SaySocial(PlayerbotTextMgr::instance().GetBotTextOrDefault(
                            "taxi_in_combat", "I am in combat and cannot board the flight", {}));
                        return;
                    }

                    // Player::ActivateTaxiPathTo refuses outright if the bot is mounted, shapeshifted or
                    // casting - but only when a flight master is passed. Those guards exist to stop a
                    // modified client starting a flight mounted; a real client dismounts you when you pick
                    // a destination, and they are meaningless for a server-side bot. Clear the states
                    // ourselves rather than dropping the flight master, because that argument is what
                    // applies the reputation price discount and records the flight master's faction.
                    self->RemoveAurasByType(SPELL_AURA_MOUNTED);

                    if (self->IsInDisallowedMountForm())
                        self->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

                    if (self->IsNonMeleeSpellCast(false))
                        self->InterruptNonMeleeSpells(false);

                    // Falls back to no flight master only if it has gone out of range - full fare beats
                    // not flying at all.
                    Creature* flightMaster = ObjectAccessor::GetCreature(*self, npcGuid);

                    if (!self->ActivateTaxiPathTo(nodes, flightMaster, 0))
                    {
                        selfAI->SaySocial(PlayerbotTextMgr::instance().GetBotTextOrDefault(
                            "taxi_could_not_board", "I could not board that flight - check my gold", {}));
                    }
                },
                delay);
            botAI->SetNextCheckDelay(delay + 50);
            return true;
        }

        if (param == "?")
        {
            botAI->TellMasterNoFacing("=== Taxi ===");

            uint32 index = 1;
            for (uint32 node : nodes)
            {
                TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(node);
                if (!entry)
                    continue;

                TaxiNodesEntry const* dest = sTaxiNodesStore.LookupEntry(entry->to);
                if (!dest)
                    continue;

                std::ostringstream out;
                out << index++ << ": " << dest->name[0];
                botAI->TellMasterNoFacing(out.str());
            }

            return true;
        }

        uint32 selected = atoi(param.c_str());
        if (selected)
        {
            uint32 path = nodes[selected - 1];
            TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(path);
            if (!entry)
                return false;

            return bot->ActivateTaxiPathTo({entry->from, entry->to}, npc, 0);
        }

        if (!movement.taxiNodes.empty() && !bot->ActivateTaxiPathTo(movement.taxiNodes, npc, 0))
        {
            movement.taxiNodes.clear();
            movement.Set(nullptr);
            botAI->TellError(PlayerbotTextMgr::instance().GetBotTextOrDefault(
                "taxi_cant_fly_with_you", "I can't fly with you", {}));
            return false;
        }

        return true;
    }

    botAI->TellError(PlayerbotTextMgr::instance().GetBotTextOrDefault(
        "taxi_no_flightmaster_nearby", "Cannot find any flightmaster to talk", {}));
    return false;
}
