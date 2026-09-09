/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SUGGESTWHATTODOACTION_H
#define PLAYERBOTS_SUGGESTWHATTODOACTION_H

#include "InventoryAction.h"

class PlayerbotAI;

class SuggestWhatToDoAction : public InventoryAction
{
public:
    SuggestWhatToDoAction(PlayerbotAI* botAI, std::string const name = "suggest what to do");

    bool Execute(Event event) override;
    bool isUseful() override;

protected:
    using Suggestion = std::function<void()>;
    std::vector<Suggestion> suggestions;
    void specificQuest();
    void grindReputation();
    void grindMaterials();
    void something();
    void toxicLinks();
    void somethingToxic();
    void thunderfury();

    std::vector<uint32> GetIncompletedQuests();

private:
    static std::map<std::string, uint8> factions;
    const int32_t _dbc_locale;
};

// A bot saying what it is working towards. Only bots whose personality gave them an ambition have
// anything to say here, so most of the group stays quiet.
class MentionAmbitionAction : public SuggestWhatToDoAction
{
public:
    MentionAmbitionAction(PlayerbotAI* botAI) : SuggestWhatToDoAction(botAI, "mention ambition") {}

    bool isUseful() override;
    bool Execute(Event event) override;
};

// "I have a battle rez", "Heroism is up". The kind of thing a real group member says unprompted,
// and genuinely useful to the player rather than just flavour.
class UtilityCalloutAction : public SuggestWhatToDoAction
{
public:
    UtilityCalloutAction(PlayerbotAI* botAI, std::string const name = "utility callout")
        : SuggestWhatToDoAction(botAI, name)
    {
    }

    // Only calls out cooldowns worth interrupting someone for.
    enum class CalloutKind : uint8
    {
        BattleRez,
        Haste,
        Mana,
        Save,
        Utility
    };

    struct Cooldown
    {
        uint32 spellId;
        CalloutKind kind;
    };

    bool isUseful() override;
    bool Execute(Event event) override;

protected:

    // Ready and worth mentioning, or nullptr.
    Cooldown const* FindReadyCooldown() const;
    static char const* TextKeyFor(CalloutKind kind);
};

// Same callouts, but fired when someone actually dies - which is when a battle rez matters.
class BattleRezCalloutAction : public UtilityCalloutAction
{
public:
    BattleRezCalloutAction(PlayerbotAI* botAI) : UtilityCalloutAction(botAI, "battle rez callout") {}

    bool isUseful() override;
};

// Local muttering in /say rather than a channel: an observation about where the bot is standing,
// audible only to people actually there. Brings broadcast_city_arrival into use - the pool existed
// but nothing had ever emitted it.
class AmbientSayAction : public SuggestWhatToDoAction
{
public:
    AmbientSayAction(PlayerbotAI* botAI) : SuggestWhatToDoAction(botAI, "ambient say") {}

    bool isUseful() override;
    bool Execute(Event event) override;
};

class SuggestTradeAction : public SuggestWhatToDoAction
{
public:
    SuggestTradeAction(PlayerbotAI* botAI);
    bool Execute(Event event) override;
};

class SuggestDungeonAction : public SuggestWhatToDoAction
{
public:
    SuggestDungeonAction(PlayerbotAI* botAI);

    bool Execute(Event event) override;
private:
    static std::map<std::string, uint8> instances;
};

#endif
