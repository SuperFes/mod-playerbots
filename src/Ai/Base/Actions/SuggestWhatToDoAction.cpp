/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "SuggestWhatToDoAction.h"
#include "AiFactory.h"
#include "AmbitionMgr.h"
#include "BroadcastHelper.h"
#include "Channel.h"
#include "ChatHelper.h"
#include "DBCStores.h"
#include "Event.h"
#include "ItemVisitors.h"
#include "Playerbots.h"
#include "ServerFacade.h"
#include "SpellMgr.h"
#include "Weather.h"
#include <functional>

// Was urand(300, 600) plus up to 300s of jitter - 5 to 15 minutes per bot, which read as silence.
// PlayerbotAI::SocialCooldown scales these by the bot's chattiness and Playerbots.SocialCooldownScale.
// An ambition is a long arc; mentioning it every few minutes would spoil it.
// Long enough that a callout is information, not chatter.
// Local chatter is heard by anyone standing nearby, so it wants to be rare.
static constexpr uint32 AMBIENT_COOLDOWN_MIN = 420;
static constexpr uint32 AMBIENT_COOLDOWN_MAX = 1200;

static constexpr uint32 CALLOUT_COOLDOWN_MIN = 300;
static constexpr uint32 CALLOUT_COOLDOWN_MAX = 900;

static constexpr uint32 AMBITION_COOLDOWN_MIN = 900;
static constexpr uint32 AMBITION_COOLDOWN_MAX = 2400;

static constexpr uint32 SUGGEST_COOLDOWN_MIN = 90;
static constexpr uint32 SUGGEST_COOLDOWN_MAX = 240;

enum eTalkType
{
    /*0x18*/ General = ChannelFlags::CHANNEL_FLAG_GENERAL | ChannelFlags::CHANNEL_FLAG_NOT_LFG,
    /*0x3C*/ Trade = ChannelFlags::CHANNEL_FLAG_CITY | ChannelFlags::CHANNEL_FLAG_GENERAL |
                     ChannelFlags::CHANNEL_FLAG_NOT_LFG | ChannelFlags::CHANNEL_FLAG_TRADE,
    /*0x18*/ LocalDefence = ChannelFlags::CHANNEL_FLAG_GENERAL | ChannelFlags::CHANNEL_FLAG_NOT_LFG,
    /*x038*/ GuildRecruitment =
        ChannelFlags::CHANNEL_FLAG_CITY | ChannelFlags::CHANNEL_FLAG_GENERAL | ChannelFlags::CHANNEL_FLAG_NOT_LFG,
    /*0x50*/ LookingForGroup = ChannelFlags::CHANNEL_FLAG_LFG | ChannelFlags::CHANNEL_FLAG_GENERAL
};

std::map<std::string, uint8> SuggestDungeonAction::instances;
std::map<std::string, uint8> SuggestWhatToDoAction::factions;

SuggestWhatToDoAction::SuggestWhatToDoAction(PlayerbotAI* botAI, std::string const name)
    : InventoryAction{botAI, name}, _dbc_locale{sWorld->GetDefaultDbcLocale()}
{
    suggestions.push_back(std::bind(&SuggestWhatToDoAction::specificQuest, this));
    suggestions.push_back(std::bind(&SuggestWhatToDoAction::grindReputation, this));
    suggestions.push_back(std::bind(&SuggestWhatToDoAction::something, this));
    suggestions.push_back(std::bind(&SuggestWhatToDoAction::grindMaterials, this));
    suggestions.push_back(std::bind(&SuggestWhatToDoAction::somethingToxic, this));
    suggestions.push_back(std::bind(&SuggestWhatToDoAction::toxicLinks, this));
}

bool SuggestWhatToDoAction::isUseful()
{
    if (!sRandomPlayerbotMgr.IsRandomBot(bot) || bot->GetGroup() || bot->GetInstanceId() || bot->GetBattleground())
        return false;

    std::string qualifier = "suggest what to do";
    time_t lastSaid = AI_VALUE2(time_t, "last said", qualifier);
    return time(nullptr) >= lastSaid;
}

bool SuggestWhatToDoAction::Execute(Event /*event*/)
{
    uint32 index = rand() % suggestions.size();
    auto fnct_ptr = suggestions[index];
    fnct_ptr();

    std::string const qualifier = "suggest what to do";
    botAI->GetAiObjectContext()->GetValue<time_t>("last said", qualifier)
        ->Set(time(nullptr) + botAI->SocialCooldown(SUGGEST_COOLDOWN_MIN, SUGGEST_COOLDOWN_MAX));

    return true;
}

std::vector<uint32> SuggestWhatToDoAction::GetIncompletedQuests()
{
    std::vector<uint32> result;

    for (uint16 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 questId = bot->GetQuestSlotQuestId(slot);
        if (!questId)
            continue;

        QuestStatus status = bot->GetQuestStatus(questId);
        if (status == QUEST_STATUS_INCOMPLETE || status == QUEST_STATUS_NONE)
            result.push_back(questId);
    }

    return result;
}

void SuggestWhatToDoAction::specificQuest()
{
    std::vector<uint32> quests = GetIncompletedQuests();
    if (quests.empty())
        return;

    BroadcastHelper::BroadcastSuggestQuest(botAI, quests, bot);
}

void SuggestWhatToDoAction::grindMaterials()
{
    /*if (bot->GetLevel() <= 5)
        return;

    auto result = CharacterDatabase.Query("SELECT distinct category, multiplier FROM ahbot_category where category not
    in ('other', 'quest', 'trade', 'reagent') and multiplier > 3 order by multiplier desc limit 10"); if (!result)
        return;

    std::map<std::string, double> categories;
    do
    {
        Field* fields = result->Fetch();
        categories[fields[0].Get<std::string>()] = fields[1].Get<float>();
    } while (result->NextRow());

    for (std::map<std::string, double>::iterator i = categories.begin(); i != categories.end(); ++i)
    {
        if (urand(0, 10) < 3)
        {
            std::string name = i->first;
            double multiplier = i->second;

            for (int j = 0; j < ahbot::CategoryList::instance.size(); j++)
            {
                ahbot::Category* category = ahbot::CategoryList::instance[j];
                if (name == category->GetName())
                {
                    std::string item = category->GetLabel();
                    transform(item.begin(), item.end(), item.begin(), ::tolower);
                    std::ostringstream itemout;
                    itemout << "|c0000b000" << item << "|r";
                    item = itemout.str();

                    std::map<std::string, std::string> placeholders;
                    placeholders["%role"] = chat->formatClass(bot, AiFactory::GetPlayerSpecTab(bot));
                    placeholders["%category"] = item;

                    spam(PlayerbotTextMgr::instance().GetBotText("suggest_trade", placeholders), urand(0, 1) ? 0x3C : 0x18, !urand(0, 2), !urand(0,
    3)); return;
                }
            }
        }
    }*/
}

void SuggestWhatToDoAction::grindReputation()
{
    if (factions.empty())
    {
        factions["Argent Dawn"] = 60;
        factions["Bloodsail Buccaneers"] = 40;
        factions["Brood of Nozdormu"] = 60;
        factions["Cenarion Circle"] = 55;
        factions["Darkmoon Faire"] = 20;
        factions["Hydraxian Waterlords"] = 60;
        factions["Ravenholdt"] = 20;
        factions["Thorium Brotherhood"] = 40;
        factions["Timbermaw Hold"] = 50;
        factions["Wintersaber Trainers"] = 50;
        factions["Booty Bay"] = 30;
        factions["Everlook"] = 40;
        factions["Gadgetzan"] = 50;
        factions["Ratchet"] = 20;

        factions["Ashtongue Deathsworn"] = 70;
        factions["Cenarion Expedition"] = 62;
        factions["The Consortium"] = 65;
        factions["Honor Hold"] = 66;
        factions["Keepers of Time"] = 68;
        factions["Netherwing"] = 65;
        factions["Ogri'la"] = 65;
        factions["The Scale of the Sands"] = 65;
        factions["Sporeggar"] = 65;
        factions["Tranquillien"] = 10;
        factions["The Violet Eye"] = 70;

        factions["Argent Crusade"] = 75;
        factions["Ashen Verdict"] = 75;
        factions["The Kalu'ak"] = 72;
        factions["Kirin Tor"] = 75;
        factions["Knights of the Ebon Blade"] = 77;
        factions["The Sons of Hodir"] = 78;
        factions["The Wyrmrest Accord"] = 77;
    }

    std::vector<std::string> levels;
    levels.push_back("honored");
    levels.push_back("revered");
    levels.push_back("exalted");

    std::vector<std::string> allowedFactions;
    for (auto it : factions)
    {
        if ((int)bot->GetLevel() >= it.second) allowedFactions.push_back(it.first);
    }

    if (allowedFactions.empty()) return;

    BroadcastHelper::BroadcastSuggestGrindReputation(botAI, levels, allowedFactions, bot);
}

void SuggestWhatToDoAction::something()
{
    BroadcastHelper::BroadcastSuggestSomething(botAI, bot);
}

void SuggestWhatToDoAction::somethingToxic()
{
    BroadcastHelper::BroadcastSuggestSomethingToxic(botAI, bot);
}

void SuggestWhatToDoAction::toxicLinks()
{
    BroadcastHelper::BroadcastSuggestToxicLinks(botAI, bot);
}

void SuggestWhatToDoAction::thunderfury()
{
    BroadcastHelper::BroadcastSuggestThunderfury(botAI, bot);
}

class FindTradeItemsVisitor : public IterateItemsVisitor
{
public:
    FindTradeItemsVisitor(uint32 quality) : IterateItemsVisitor(), quality(quality) {}

    bool Visit(Item* item) override
    {
        ItemTemplate const* proto = item->GetTemplate();
        if (proto->Quality != quality)
            return true;

        if (proto->Class == ITEM_CLASS_TRADE_GOODS && proto->Bonding == NO_BIND)
        {
            if (proto->Quality == ITEM_QUALITY_NORMAL && item->GetCount() > 1 &&
                item->GetCount() == item->GetMaxStackCount())
                stacks.push_back(proto->ItemId);

            items.push_back(proto->ItemId);
            count[proto->ItemId] += item->GetCount();
        }

        return true;
    }

    std::map<uint32, uint32> count;
    std::vector<uint32> stacks;
    std::vector<uint32> items;

private:
    uint32 quality;
};

SuggestDungeonAction::SuggestDungeonAction(PlayerbotAI* botAI) : SuggestWhatToDoAction(botAI, "suggest dungeon") {}

bool SuggestDungeonAction::Execute(Event /*event*/)
{
    // TODO: use PlayerbotDungeonRepository::instance()

    std::string const qualifier = "suggest what to do";
    botAI->GetAiObjectContext()->GetValue<time_t>("last said", qualifier)
        ->Set(time(nullptr) + botAI->SocialCooldown(SUGGEST_COOLDOWN_MIN, SUGGEST_COOLDOWN_MAX));

    if (!sPlayerbotAIConfig.RandomBotSuggestDungeons || bot->GetGroup())
        return false;

    if (instances.empty())
    {
        instances["Ragefire Chasm"] = 15;
        instances["Deadmines"] = 18;
        instances["Wailing Caverns"] = 18;
        instances["Shadowfang Keep"] = 25;
        instances["Blackfathom Deeps"] = 20;
        instances["Stockade"] = 20;
        instances["Gnomeregan"] = 35;
        instances["Razorfen Kraul"] = 35;
        instances["Maraudon"] = 50;
        instances["Scarlet Monestery"] = 40;
        instances["Uldaman"] = 45;
        instances["Dire Maul"] = 58;
        instances["Scholomance"] = 59;
        instances["Razorfen Downs"] = 40;
        instances["Strathholme"] = 59;
        instances["Zul'Farrak"] = 45;
        instances["Blackrock Depths"] = 55;
        instances["Temple of Atal'Hakkar"] = 55;
        instances["Lower Blackrock Spire"] = 57;

        instances["Hellfire Citidel"] = 65;
        instances["Coilfang Reservoir"] = 65;
        instances["Auchindoun"] = 65;
        instances["Cavens of Time"] = 68;
        instances["Tempest Keep"] = 69;
        instances["Magister's Terrace"] = 70;

        instances["Utgarde Keep"] = 75;
        instances["The Nexus"] = 75;
        instances["Ahn'kahet: The Old Kingdom"] = 75;
        instances["Azjol-Nerub"] = 75;
        instances["Drak'Tharon Keep"] = 75;
        instances["Violet Hold"] = 80;
        instances["Gundrak"] = 77;
        instances["Halls of Stone"] = 77;
        instances["Halls of Lightning"] = 77;
        instances["Oculus"] = 77;
        instances["Utgarde Pinnacle"] = 77;
        instances["Trial of the Champion"] = 80;
        instances["Forge of Souls"] = 80;
        instances["Pit of Saron"] = 80;
        instances["Halls of Reflection"] = 80;
    }

    std::vector<std::string> allowedInstances;
    for (auto it : instances)
    {
        if (bot->GetLevel() >= it.second) allowedInstances.push_back(it.first);
    }

    if (allowedInstances.empty()) return false;

    BroadcastHelper::BroadcastSuggestInstance(botAI, allowedInstances, bot);
    return true;
}

SuggestTradeAction::SuggestTradeAction(PlayerbotAI* botAI) : SuggestWhatToDoAction(botAI, "suggest trade") {}

bool SuggestTradeAction::Execute(Event /*event*/)
{
    std::string const qualifier = "suggest what to do";
    botAI->GetAiObjectContext()->GetValue<time_t>("last said", qualifier)
        ->Set(time(nullptr) + botAI->SocialCooldown(SUGGEST_COOLDOWN_MIN, SUGGEST_COOLDOWN_MAX));

    uint32 quality = urand(0, 100);
    if (quality > 95)
        quality = ITEM_QUALITY_LEGENDARY;
    else if (quality > 90)
        quality = ITEM_QUALITY_EPIC;
    else if (quality > 75)
        quality = ITEM_QUALITY_RARE;
    else if (quality > 50)
        quality = ITEM_QUALITY_UNCOMMON;
    else
        quality = ITEM_QUALITY_NORMAL;

    uint32 item = 0, count = 0;
    while (quality-- > ITEM_QUALITY_POOR)
    {
        FindTradeItemsVisitor visitor(quality);
        IterateItems(&visitor);
        if (!visitor.stacks.empty())
        {
            uint32 index = urand(0, visitor.stacks.size() - 1);
            item = visitor.stacks[index];
        }

        if (!item)
        {
            if (!visitor.items.empty())
            {
                uint32 index = urand(0, visitor.items.size() - 1);
                item = visitor.items[index];
            }
        }

        if (item)
        {
            count = visitor.count[item];
            break;
        }
    }

    if (!item || !count)
        return false;

    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item);
    if (!proto)
        return false;

    uint32 price = proto->SellPrice * sRandomPlayerbotMgr.GetSellMultiplier(bot) * count;
    if (!price)
        return false;

    BroadcastHelper::BroadcastSuggestSell(botAI, proto, count, price, bot);
    return true;
}

bool MentionAmbitionAction::isUseful()
{
    if (!sPlayerbotAIConfig.BotAmbitions)
        return false;

    if (botAI->GetPersonality().ambition == BotAmbitionKind::None)
        return false;

    std::string const qualifier = "mention ambition";
    return time(nullptr) >= AI_VALUE2(time_t, "last said", qualifier);
}

bool MentionAmbitionAction::Execute(Event /*event*/)
{
    BotAmbition const ambition = sAmbitionMgr->ResolveFor(bot);

    std::map<std::string, std::string> placeholders;
    std::string key;

    if (ambition.IsSet())
    {
        ItemSetEntry const* set = sItemSetStore.LookupEntry(ambition.setId);
        if (!set)
            return false;

        uint32 const owned = AmbitionMgr::CountSetPiecesOwned(bot, ambition.setId);
        uint32 const total = AmbitionMgr::CountSetPiecesTotal(ambition.setId);

        placeholders["%set_name"] = set->name[sWorld->GetDefaultDbcLocale()];
        placeholders["%owned"] = std::to_string(owned);
        placeholders["%total"] = std::to_string(total);

        key = owned ? "ambition_set_progress" : "ambition_set";
    }
    else if (ambition.IsItem())
    {
        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(ambition.itemId);
        if (!proto)
            return false;

        placeholders["%item_link"] = chat->FormatItem(proto);

        std::string const location = sAmbitionMgr->DescribeLocation(ambition);
        if (!location.empty())
        {
            placeholders["%location"] = location;
            placeholders["%source_name"] = ambition.sourceName;
            key = "ambition_item_located";
        }
        else
            key = "ambition_item";
    }
    else
        return false;

    botAI->SaySocial(PlayerbotTextMgr::instance().GetBotTextOrDefault(key, "", placeholders));

    std::string const qualifier = "mention ambition";
    botAI->GetAiObjectContext()->GetValue<time_t>("last said", qualifier)
        ->Set(time(nullptr) + botAI->SocialCooldown(AMBITION_COOLDOWN_MIN, AMBITION_COOLDOWN_MAX));

    return true;
}

// Cooldowns worth announcing. Deliberately short: a bot listing every ability it owns is noise,
// and the point is the handful a group leader actually plans around.
static std::vector<UtilityCalloutAction::Cooldown> const& GetCalloutTable()
{
    static std::vector<UtilityCalloutAction::Cooldown> const table = {
        // battle rez - the one that changes what a group does mid-fight
        { 20484, UtilityCalloutAction::CalloutKind::BattleRez },  // Rebirth (druid)
        { 61999, UtilityCalloutAction::CalloutKind::BattleRez },  // Raise Ally (death knight)
        { 27239, UtilityCalloutAction::CalloutKind::BattleRez },  // Soulstone Resurrection (warlock)

        { 2825,  UtilityCalloutAction::CalloutKind::Haste },      // Bloodlust (horde shaman)
        { 32182, UtilityCalloutAction::CalloutKind::Haste },      // Heroism (alliance shaman)

        { 29166, UtilityCalloutAction::CalloutKind::Mana },       // Innervate (druid)
        { 64901, UtilityCalloutAction::CalloutKind::Mana },       // Hymn of Hope (priest)

        { 633,   UtilityCalloutAction::CalloutKind::Save },       // Lay on Hands (paladin)
        { 19752, UtilityCalloutAction::CalloutKind::Save },       // Divine Intervention (paladin)
        { 47788, UtilityCalloutAction::CalloutKind::Save },       // Guardian Spirit (priest)
        { 33206, UtilityCalloutAction::CalloutKind::Save },       // Pain Suppression (priest)
        { 740,   UtilityCalloutAction::CalloutKind::Save },       // Tranquility (druid)
        { 871,   UtilityCalloutAction::CalloutKind::Save },       // Shield Wall (warrior)

        { 43987, UtilityCalloutAction::CalloutKind::Utility },    // Ritual of Refreshment (mage)
        { 34477, UtilityCalloutAction::CalloutKind::Utility },    // Misdirection (hunter)
        { 57934, UtilityCalloutAction::CalloutKind::Utility },    // Tricks of the Trade (rogue)
    };

    return table;
}

char const* UtilityCalloutAction::TextKeyFor(CalloutKind kind)
{
    switch (kind)
    {
        case CalloutKind::BattleRez: return "callout_battle_rez";
        case CalloutKind::Haste:     return "callout_haste";
        case CalloutKind::Mana:      return "callout_mana";
        case CalloutKind::Save:      return "callout_save";
        default:                     return "callout_utility";
    }
}

UtilityCalloutAction::Cooldown const* UtilityCalloutAction::FindReadyCooldown() const
{
    for (Cooldown const& cooldown : GetCalloutTable())
    {
        if (!bot->HasSpell(cooldown.spellId))
            continue;

        // Ready now - announcing one that is still on cooldown would be worse than saying nothing.
        if (bot->HasSpellCooldown(cooldown.spellId))
            continue;

        return &cooldown;
    }

    return nullptr;
}

bool UtilityCalloutAction::isUseful()
{
    if (!sPlayerbotAIConfig.UtilityCallouts)
        return false;

    // Only worth saying to someone who can act on it.
    if (!botAI->HasGameClientMaster() || !bot->GetGroup())
        return false;

    if (time(nullptr) < AI_VALUE2(time_t, "last said", getName()))
        return false;

    return FindReadyCooldown() != nullptr;
}

bool UtilityCalloutAction::Execute(Event /*event*/)
{
    Cooldown const* ready = FindReadyCooldown();
    if (!ready)
        return false;

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(ready->spellId);
    if (!spellInfo)
        return false;

    std::map<std::string, std::string> placeholders;
    placeholders["%spell_name"] = spellInfo->SpellName[sWorld->GetDefaultDbcLocale()];
    placeholders["%my_class"] = chat->FormatClass(bot->getClass());

    botAI->SayToParty(PlayerbotTextMgr::instance().GetBotTextOrDefault(
        TextKeyFor(ready->kind), "%spell_name is ready.", placeholders));

    botAI->GetAiObjectContext()->GetValue<time_t>("last said", getName())
        ->Set(time(nullptr) + botAI->SocialCooldown(CALLOUT_COOLDOWN_MIN, CALLOUT_COOLDOWN_MAX));

    return true;
}

// A death is exactly when a battle rez is worth hearing about, so this ignores the normal cooldown
// on chatter - but only for that one kind.
bool BattleRezCalloutAction::isUseful()
{
    if (!sPlayerbotAIConfig.UtilityCallouts)
        return false;

    if (!botAI->HasGameClientMaster() || !bot->GetGroup())
        return false;

    Cooldown const* ready = FindReadyCooldown();
    return ready && ready->kind == CalloutKind::BattleRez;
}

namespace
{
    // Capital cities. AreaTableEntry::flags carries 312 for all of them; AREA_FLAG_CAPITAL is the
    // bit the core itself tests when deciding resting state.
    bool IsInCapitalCity(Player* bot)
    {
        AreaTableEntry const* zone = sAreaTableStore.LookupEntry(bot->GetZoneId());
        return zone && (zone->flags & AREA_FLAG_CAPITAL);
    }

    // Text pool for the weather actually happening, not weather in general. Commenting on rain
    // during clear skies is worse than saying nothing, which is why these are tagged by state.
    char const* WeatherTextKey(WeatherState state)
    {
        switch (state)
        {
            case WEATHER_STATE_FINE:
                return "weather_fine";
            case WEATHER_STATE_FOG:
                return "weather_fog";
            case WEATHER_STATE_LIGHT_RAIN:
            case WEATHER_STATE_MEDIUM_RAIN:
            case WEATHER_STATE_HEAVY_RAIN:
            case WEATHER_STATE_BLACKRAIN:
                return "weather_rain";
            case WEATHER_STATE_LIGHT_SNOW:
            case WEATHER_STATE_MEDIUM_SNOW:
            case WEATHER_STATE_HEAVY_SNOW:
            case WEATHER_STATE_BLACKSNOW:
                return "weather_snow";
            case WEATHER_STATE_LIGHT_SANDSTORM:
            case WEATHER_STATE_MEDIUM_SANDSTORM:
            case WEATHER_STATE_HEAVY_SANDSTORM:
                return "weather_sandstorm";
            case WEATHER_STATE_THUNDERS:
                return "weather_storm";
            default:
                return "weather_rough";
        }
    }
}

bool AmbientSayAction::isUseful()
{
    if (!sPlayerbotAIConfig.AmbientSay)
        return false;

    if (bot->IsInCombat() || botAI->GetState() == BOT_STATE_COMBAT)
        return false;

    // Either somewhere worth remarking on, or weather worth remarking on.
    if (!IsInCapitalCity(bot) && !bot->IsOutdoors())
        return false;

    return time(nullptr) >= AI_VALUE2(time_t, "last said", getName());
}

bool AmbientSayAction::Execute(Event /*event*/)
{
    AreaTableEntry const* zone = sAreaTableStore.LookupEntry(bot->GetZoneId());
    AreaTableEntry const* area = sAreaTableStore.LookupEntry(bot->GetAreaId());
    if (!zone)
        return false;

    std::map<std::string, std::string> placeholders;
    placeholders["%zone_name"] = botAI->GetLocalizedAreaName(zone);
    placeholders["%area_name"] = area ? botAI->GetLocalizedAreaName(area) : placeholders["%zone_name"];

    std::string key;

    if (IsInCapitalCity(bot))
    {
        key = "broadcast_city_arrival";
    }
    else if (bot->IsOutdoors())
    {
        // Returns null for zones with no weather data at all, which is most instances.
        Weather* weather = bot->GetMap()->GetOrGenerateZoneDefaultWeather(bot->GetZoneId());
        if (!weather)
            return false;

        WeatherState const state = weather->GetWeatherState();

        // Bad weather can draw either a specific remark or a general grumble; clear skies only get
        // the fine lines, since "weather sucks today" in sunshine is exactly the problem to avoid.
        key = WeatherTextKey(state);
        if (state != WEATHER_STATE_FINE && botAI->RollSocial(2))
            key = "weather_rough";
    }

    if (key.empty())
        return false;

    std::string const line = PlayerbotTextMgr::instance().GetBotTextOrDefault(key, "", placeholders);

    if (line.empty())
        return false;

    botAI->Say(line);

    botAI->GetAiObjectContext()->GetValue<time_t>("last said", getName())
        ->Set(time(nullptr) + botAI->SocialCooldown(AMBIENT_COOLDOWN_MIN, AMBIENT_COOLDOWN_MAX));

    return true;
}
