/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "BotAppearanceMgr.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>

#include "AmbitionMgr.h"
#include "DBCStores.h"
#include "ItemTemplate.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Playerbots.h"

#ifdef MOD_TRANSMOG
#include "Transmogrification.h"
#endif

namespace
{
    // Slots worth dressing. Tabard and shirt are deliberately left alone - they are the player's
    // own statement, and a bot overwriting them looks like a bug.
    uint8 const DRESSABLE_SLOTS[] = {
        EQUIPMENT_SLOT_HEAD,     EQUIPMENT_SLOT_SHOULDERS, EQUIPMENT_SLOT_CHEST,
        EQUIPMENT_SLOT_WAIST,    EQUIPMENT_SLOT_LEGS,      EQUIPMENT_SLOT_FEET,
        EQUIPMENT_SLOT_WRISTS,   EQUIPMENT_SLOT_HANDS
    };

    // A colour has to turn up across most of the body to dress a bot in it.
    uint32 const MIN_SLOTS_FOR_COLOUR = 6;

    // Only the eight slots above; anything else is not part of an outfit.
    uint8 SlotForInventoryType(uint32 invType)
    {
        switch (invType)
        {
            case INVTYPE_HEAD:      return EQUIPMENT_SLOT_HEAD;
            case INVTYPE_SHOULDERS: return EQUIPMENT_SLOT_SHOULDERS;
            case INVTYPE_CHEST:
            case INVTYPE_ROBE:      return EQUIPMENT_SLOT_CHEST;
            case INVTYPE_WAIST:     return EQUIPMENT_SLOT_WAIST;
            case INVTYPE_LEGS:      return EQUIPMENT_SLOT_LEGS;
            case INVTYPE_FEET:      return EQUIPMENT_SLOT_FEET;
            case INVTYPE_WRISTS:    return EQUIPMENT_SLOT_WRISTS;
            case INVTYPE_HANDS:     return EQUIPMENT_SLOT_HANDS;
            default:                return NULL_SLOT;
        }
    }

    uint32 Mix(uint32 x)
    {
        x += 0x9E3779B9u;
        x = (x ^ (x >> 16)) * 0x21F0AAADu;
        x = (x ^ (x >> 15)) * 0x735A2D97u;
        return x ^ (x >> 15);
    }
}

bool BotAppearanceMgr::ParseTexture(std::string const& texture, std::string& outFamily, std::string& outColour)
{
    // "Plate_B_02Red_Glove_AL" -> family "Plate_B_02", colour "Red"
    std::vector<std::string> parts;
    std::string current;
    for (char const c : texture)
    {
        if (c == '_')
        {
            parts.push_back(current);
            current.clear();
        }
        else
            current += c;
    }
    parts.push_back(current);

    // Material, family letter, variant+colour, then at least a piece name.
    if (parts.size() < 4)
        return false;

    std::string const& variant = parts[2];

    // Split the variant token where the digits stop: "02Red" -> "02" + "Red".
    size_t split = 0;
    while (split < variant.size() && std::isdigit(static_cast<unsigned char>(variant[split])))
        ++split;

    if (split == 0)
        return false;

    outFamily = parts[0] + "_" + parts[1] + "_" + variant.substr(0, split);
    outColour = variant.substr(split);

    return true;
}

char const* BotAppearanceMgr::FindTexture(uint32 displayId)
{
    ItemDisplayInfoEntry const* display = sItemDisplayInfoStore.LookupEntry(displayId);
    if (!display)
        return nullptr;

    for (uint32 i = 0; i < MAX_ITEM_DISPLAY_TEXTURES; ++i)
        if (display->texture[i] && *display->texture[i])
            return display->texture[i];

    return nullptr;
}

void BotAppearanceMgr::LoadAll()
{
    _bySlot.clear();
    _usableColours.clear();

    std::map<std::string, std::set<uint8>> colourSlots;

    ItemTemplateContainer const* items = sObjectMgr->GetItemTemplateStore();
    if (!items)
        return;

    for (auto const& [entry, proto] : *items)
    {
        if (proto.Class != ITEM_CLASS_ARMOR || !proto.DisplayInfoID)
            continue;

        uint8 const slot = SlotForInventoryType(proto.InventoryType);
        if (slot == NULL_SLOT)
            continue;

        char const* texture = FindTexture(proto.DisplayInfoID);
        if (!texture)
            continue;

        Appearance appearance;
        if (!ParseTexture(texture, appearance.family, appearance.colour))
            continue;

        appearance.itemId = proto.ItemId;
        appearance.displayId = proto.DisplayInfoID;
        appearance.material = proto.Material;
        appearance.requiredLevel = proto.RequiredLevel;

        if (!appearance.colour.empty())
            colourSlots[appearance.colour].insert(slot);

        _bySlot[slot].push_back(appearance);
    }

    for (auto const& [colour, slots] : colourSlots)
        if (slots.size() >= MIN_SLOTS_FOR_COLOUR)
            _usableColours.push_back(colour);

    std::sort(_usableColours.begin(), _usableColours.end());

    uint32 total = 0;
    for (auto const& [slot, list] : _bySlot)
        total += list.size();

    LOG_INFO("playerbots", "BotAppearanceMgr: {} appearances across {} slots, {} usable colours",
             total, _bySlot.size(), _usableColours.size());
}

std::vector<BotAppearanceMgr::Appearance> const* BotAppearanceMgr::Candidates(uint8 slot) const
{
    auto const it = _bySlot.find(slot);
    return it == _bySlot.end() ? nullptr : &it->second;
}

void BotAppearanceMgr::ApplyTo(Player* bot)
{
#ifndef MOD_TRANSMOG
    (void)bot;
#else
    if (!bot || !sPlayerbotAIConfig.botTransmog || _usableColours.empty())
        return;

    PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
    if (!botAI)
        return;

    BotPersonality const& personality = botAI->GetPersonality();
    uint32 const seed = bot->GetGUID().GetCounter();

    // A bot chasing a whole set wears that set's look while it collects the real pieces - the
    // appearance and the ambition end up telling the same story.
    uint32 aspirationalSet = 0;
    BotAmbition const ambition = sAmbitionMgr->ResolveFor(bot);
    if (ambition.IsSet())
        aspirationalSet = ambition.setId;

    std::string const chosenColour = _usableColours[Mix(seed ^ 0xC01Du) % _usableColours.size()];

    // Eclectic bots hold the colour and let the family wander; everyone else picks a family too.
    bool const holdFamily = personality.taste != BotTaste::Eclectic;
    std::string chosenFamily;

    for (uint8 const slot : DRESSABLE_SLOTS)
    {
        Item* worn = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!worn)
            continue;

        std::vector<Appearance> const* candidates = Candidates(slot);
        if (!candidates || candidates->empty())
            continue;

        // Immaculate bots in a set ambition wear the real set look wherever a piece exists.
        if (personality.taste == BotTaste::Immaculate && aspirationalSet)
        {
            if (ItemSetEntry const* set = sItemSetStore.LookupEntry(aspirationalSet))
            {
                bool applied = false;
                for (uint32 i = 0; i < MAX_ITEM_SET_ITEMS && !applied; ++i)
                {
                    ItemTemplate const* piece =
                        set->itemId[i] ? sObjectMgr->GetItemTemplate(set->itemId[i]) : nullptr;

                    if (piece && SlotForInventoryType(piece->InventoryType) == slot)
                    {
                        sTransmogrification->SetFakeEntry(bot, piece->ItemId, slot, worn);
                        applied = true;
                    }
                }

                if (applied)
                    continue;
            }
        }

        // Terrible taste re-rolls the colour every slot, sincerely and disastrously.
        std::string const wantColour = personality.taste == BotTaste::Terrible
            ? _usableColours[Mix(seed ^ (slot * 0x9E37u)) % _usableColours.size()]
            : chosenColour;

        std::vector<Appearance const*> matches;
        for (Appearance const& candidate : *candidates)
        {
            if (candidate.colour != wantColour)
                continue;

            if (candidate.requiredLevel > bot->GetLevel())
                continue;

            // Terrible bots also ignore material, so plate wearers turn up in odd-looking cloth.
            if (personality.taste != BotTaste::Terrible && candidate.material != worn->GetTemplate()->Material)
                continue;

            if (holdFamily && !chosenFamily.empty() && candidate.family != chosenFamily)
                continue;

            matches.push_back(&candidate);
        }

        // Holding a family is a preference, not a requirement: fall back to colour alone rather
        // than leaving a slot undressed and breaking the line of the outfit.
        if (matches.empty() && holdFamily && !chosenFamily.empty())
        {
            for (Appearance const& candidate : *candidates)
                if (candidate.colour == wantColour && candidate.requiredLevel <= bot->GetLevel() &&
                    candidate.material == worn->GetTemplate()->Material)
                    matches.push_back(&candidate);
        }

        if (matches.empty())
            continue;

        Appearance const* pick = matches[Mix(seed ^ (slot * 0x51EDu)) % matches.size()];

        if (holdFamily && chosenFamily.empty())
            chosenFamily = pick->family;

        sTransmogrification->SetFakeEntry(bot, pick->itemId, slot, worn);
    }
#endif
}
