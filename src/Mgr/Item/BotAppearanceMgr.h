/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_BOTAPPEARANCEMGR_H
#define PLAYERBOTS_BOTAPPEARANCEMGR_H

#include <string>
#include <unordered_map>
#include <vector>

#include "BotPersonality.h"
#include "Common.h"

class Player;

// Gives each bot a stable, deliberate-looking outfit via mod-transmog.
//
// WoW's armour textures are named {Material}_{Family}_{Variant}{Colour}_{Piece}_{Region}, for
// example "Plate_B_02Red_Glove_AL". That gives two usable keys for free: the look family
// ("Plate_B_02") and the colour ("Red"). Two pieces sharing a colour read as an outfit even when
// they come from completely unrelated items, which is what makes dressing a bot possible at all.
//
// The whole thing is derived from the personality seed, so a bot wears the same outfit across
// restarts and you start recognising it by silhouette.
class BotAppearanceMgr
{
public:
    static BotAppearanceMgr* instance()
    {
        static BotAppearanceMgr inst;
        return &inst;
    }

    // Indexes every equippable armour appearance by slot, material, family and colour.
    void LoadAll();

    // Dresses the bot according to its taste. Safe to call repeatedly; it only ever sets the
    // transmog appearance of items the bot is actually wearing.
    void ApplyTo(Player* bot);

    bool IsLoaded() const { return !_bySlot.empty(); }

    struct Appearance
    {
        uint32 itemId = 0;
        uint32 displayId = 0;
        std::string family;  // "Plate_B_02"
        std::string colour;  // "Red", empty when the texture carries none
        int32 material = 0;   // ItemTemplate::Material is signed
        uint32 requiredLevel = 0;
    };

private:
    BotAppearanceMgr() = default;

    // Pulls the family and colour out of a texture name, or returns false if it does not parse.
    static bool ParseTexture(std::string const& texture, std::string& outFamily, std::string& outColour);

    // First non-empty armour texture on a display entry; which index is populated varies by slot.
    static char const* FindTexture(uint32 displayId);

    std::vector<Appearance> const* Candidates(uint8 slot) const;

    // slot -> every appearance that can occupy it
    std::unordered_map<uint8, std::vector<Appearance>> _bySlot;
    // colours that appear on enough slots to dress a whole bot in
    std::vector<std::string> _usableColours;
};

#define sBotAppearanceMgr BotAppearanceMgr::instance()

#endif
