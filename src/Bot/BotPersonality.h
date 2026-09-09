/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_BOTPERSONALITY_H
#define PLAYERBOTS_BOTPERSONALITY_H

#include <cstdint>

// Per-bot traits derived from a stable seed (the bot's GUID counter), so a given character behaves
// like the same individual across restarts. Deliberately free of AzerothCore types: the whole struct
// is pure arithmetic so it can be unit tested without a live Player.
//
// Traits are a *bias*, not a value. A trait alone would only replace "every bot identical" with
// "every bot individually metronomic"; call sites that model an action a person performs repeatedly
// re-roll around the trait via ApplyJitter(). Spatial traits (personal space, slot bias) are used
// directly - a habitual standing spot that jitters per tick is noise, not character.
// What, if anything, a bot is working towards. Most of the interest is in NONE being common: if
// every bot is chasing something the whole idea stops being a character trait.
enum class BotAmbitionKind : uint8_t
{
    None = 0,   // content with what it has
    Item,       // one specific upgrade it keeps an eye out for
    Set         // a whole tier set, which is a much longer arc
};

// How a bot puts an outfit together. Most look deliberate; the interest is in the tails.
enum class BotTaste : uint8_t
{
    Coordinated = 0,  // one colour family, consistently
    Eclectic,         // mixes families on purpose, holds one colour
    Immaculate,       // a full matched set, no compromises
    Terrible          // clashing colours and mismatched materials, entirely sincere
};

struct BotPersonality
{
    // Hard bounds. Traits are clamped here after strength scaling so an exaggerated
    // PersonalityStrength can widen the spread without producing absurd behaviour.
    static constexpr float REACTION_MIN = 0.40f;
    static constexpr float REACTION_MAX = 3.00f;
    static constexpr float DISMOUNT_MIN = 0.50f;
    static constexpr float DISMOUNT_MAX = 2.00f;
    static constexpr float SOCIAL_MIN = 0.05f;
    static constexpr float SOCIAL_MAX = 6.00f;
    static constexpr float GENEROSITY_MIN = 0.20f;
    static constexpr float GENEROSITY_MAX = 4.00f;
    static constexpr float SPACE_MAX = 2.00f;

    // Sampled bands at strength 1.0.
    static constexpr float REACTION_BAND_LOW = 0.65f;
    static constexpr float REACTION_BAND_HIGH = 2.20f;
    static constexpr float DISMOUNT_BAND_LOW = 0.85f;
    static constexpr float DISMOUNT_BAND_HIGH = 1.35f;
    static constexpr float SOCIAL_BAND_LOW = 0.15f;
    static constexpr float SOCIAL_BAND_HIGH = 3.00f;
    static constexpr float GENEROSITY_BAND_LOW = 0.50f;
    static constexpr float GENEROSITY_BAND_HIGH = 1.80f;

    // Per-decision re-roll widths, applied by call sites through ApplyJitter().
    static constexpr float REACTION_JITTER = 0.15f;
    static constexpr float DISMOUNT_JITTER = 0.10f;

    static constexpr float STRENGTH_MAX = 2.0f;

    // Share of bots with any ambition at strength 1, and the share of those chasing a whole set
    // rather than a single item. At strength 0 nobody has one, matching every other trait.
    static constexpr float AMBITION_SHARE = 0.65f;
    static constexpr float AMBITION_SET_SHARE = 0.35f;

    // Taste splits: the rest are Coordinated. Terrible is rare on purpose - it is funny once per
    // party, not once per bot.
    static constexpr float TASTE_ECLECTIC_SHARE = 0.22f;
    static constexpr float TASTE_IMMACULATE_SHARE = 0.10f;
    static constexpr float TASTE_TERRIBLE_SHARE = 0.06f;

    // Builds the traits for a bot. `seed` is the bot's GUID counter; `strength` scales every trait
    // away from neutral, so 0 reproduces uniform bots exactly and values above 1 exaggerate.
    static BotPersonality FromSeed(uint32_t seed, float strength);

    // Re-roll around a trait. `roll01` is a uniform value in [0,1) supplied by the caller (urand/frand
    // at the call site) so this stays pure and testable. Returns base scaled by [1-jitter, 1+jitter].
    static float ApplyJitter(float base, float jitter, float roll01);

    float reactionScale = 1.0f;   // multiplies AI react delay; >1 is a slower, more deliberate bot
    float personalSpaceX = 0.0f;  // habitual formation offset, in units of the formation spread...
    float personalSpaceY = 0.0f;  // ...sampled on a disc so magnitude never exceeds 1
    float dismountSlack = 1.0f;   // multiplies the keep-mounted range
    float socialScale = 1.0f;     // multiplies emote/talk frequency; <1 is a quiet bot
    float generosity = 1.0f;      // >1 defers loot to a groupmate on a smaller margin
    uint32_t slotBias = 0;        // stable tie-break for formation slot ordering
    BotAmbitionKind ambition = BotAmbitionKind::None;
    BotTaste taste = BotTaste::Coordinated;
};

#endif
