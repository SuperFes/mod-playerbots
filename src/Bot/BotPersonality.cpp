/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "BotPersonality.h"

#include <algorithm>
#include <cmath>

namespace
{
    // splitmix32 finalizer. GUID counters are sequential, so hashing is not optional here: without it
    // adjacent bots get near-identical traits and every trait on one bot moves with the others.
    constexpr uint32_t Mix(uint32_t x)
    {
        x += 0x9E3779B9u;
        x = (x ^ (x >> 16)) * 0x21F0AAADu;
        x = (x ^ (x >> 15)) * 0x735A2D97u;
        return x ^ (x >> 15);
    }

    // Independent [0,1) draw per trait: each stream index decorrelates from the others.
    float Unit(uint32_t seed, uint32_t stream)
    {
        return static_cast<float>(Mix(seed ^ (stream * 0x85EBCA6Bu)) >> 8) * (1.0f / 16777216.0f);
    }

    float Band(float low, float high, float unit) { return low + (high - low) * unit; }

    // strength 0 pins the trait at neutral; 1 gives the sampled value; above 1 extrapolates.
    float Scale(float neutral, float sampled, float strength) { return neutral + (sampled - neutral) * strength; }
}

BotPersonality BotPersonality::FromSeed(uint32_t seed, float strength)
{
    strength = std::clamp(strength, 0.0f, STRENGTH_MAX);

    BotPersonality p;

    p.reactionScale = std::clamp(
        Scale(1.0f, Band(REACTION_BAND_LOW, REACTION_BAND_HIGH, Unit(seed, 1)), strength),
        REACTION_MIN, REACTION_MAX);

    p.dismountSlack = std::clamp(
        Scale(1.0f, Band(DISMOUNT_BAND_LOW, DISMOUNT_BAND_HIGH, Unit(seed, 2)), strength),
        DISMOUNT_MIN, DISMOUNT_MAX);

    p.socialScale = std::clamp(
        Scale(1.0f, Band(SOCIAL_BAND_LOW, SOCIAL_BAND_HIGH, Unit(seed, 3)), strength),
        SOCIAL_MIN, SOCIAL_MAX);

    // Sampled in polar form so the offset lands uniformly on a disc of radius 1. Drawing x and y
    // independently would over-represent the corners of a square and let the magnitude reach sqrt(2),
    // which would in turn make Formation::GetMaxDistance() understate how far the bot may stand.
    float const angle = Unit(seed, 4) * 6.28318530718f;
    float const radius = std::sqrt(Unit(seed, 5));
    float const offsetX = Scale(0.0f, radius * std::cos(angle), strength);
    float const offsetY = Scale(0.0f, radius * std::sin(angle), strength);

    float const magnitude = std::sqrt(offsetX * offsetX + offsetY * offsetY);
    if (magnitude > SPACE_MAX)
    {
        p.personalSpaceX = offsetX * (SPACE_MAX / magnitude);
        p.personalSpaceY = offsetY * (SPACE_MAX / magnitude);
    }
    else
    {
        p.personalSpaceX = offsetX;
        p.personalSpaceY = offsetY;
    }

    p.slotBias = Mix(seed ^ 0x5BF03635u);

    return p;
}

float BotPersonality::ApplyJitter(float base, float jitter, float roll01)
{
    roll01 = std::clamp(roll01, 0.0f, 1.0f);
    return base * ((1.0f - jitter) + 2.0f * jitter * roll01);
}
