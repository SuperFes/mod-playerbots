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
    static constexpr float SPACE_MAX = 2.00f;

    // Sampled bands at strength 1.0.
    static constexpr float REACTION_BAND_LOW = 0.65f;
    static constexpr float REACTION_BAND_HIGH = 2.20f;
    static constexpr float DISMOUNT_BAND_LOW = 0.85f;
    static constexpr float DISMOUNT_BAND_HIGH = 1.35f;
    static constexpr float SOCIAL_BAND_LOW = 0.15f;
    static constexpr float SOCIAL_BAND_HIGH = 3.00f;

    // Per-decision re-roll widths, applied by call sites through ApplyJitter().
    static constexpr float REACTION_JITTER = 0.15f;
    static constexpr float DISMOUNT_JITTER = 0.10f;

    static constexpr float STRENGTH_MAX = 2.0f;

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
    uint32_t slotBias = 0;        // stable tie-break for formation slot ordering
};

#endif
