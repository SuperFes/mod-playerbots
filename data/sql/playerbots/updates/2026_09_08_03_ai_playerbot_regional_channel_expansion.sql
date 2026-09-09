-- Variety expansion for the keys that reach the regional channels (General / Trade / LookingForGroup
-- / GuildRecruitment), which were the thinnest pools still in regular use.
-- Target table: ai_playerbot_texts
--
-- Distinct lines before this file:
--   response_wtb_items_channel 5, suggest_toxic_links 12, broadcast_guild_recruitment 15,
--   suggest_quest 57, response_wtb_items_whisper 59, response_lfg_quests_channel 66, suggest_sell 83.
--
-- Placeholders taken from what each key already uses, not assumed:
--   response_wtb_items_channel   %formatted_item_links
--   response_wtb_items_whisper   %formatted_item_links %cost_gold
--   response_lfg_quests_channel  %quest_links %zone_name %my_role
--   suggest_quest                %quest_link
--   suggest_trade                %category
--   suggest_instance             %instance_name %my_class %my_role
--   suggest_toxic_links          %random_inventory_item_link %random_taken_quest_or_item_link
--   broadcast_guild_recruitment  %my_class

INSERT INTO `ai_playerbot_texts` (`name`, `text`, `say_type`, `reply_type`) VALUES
-- -------------------------------------------------------- WTB in channel (5 -> 25)
('response_wtb_items_channel', 'WTB %formatted_item_links, paying well!', 0, 0),
('response_wtb_items_channel', 'Looking to buy %formatted_item_links, PST.', 0, 0),
('response_wtb_items_channel', 'Anyone selling %formatted_item_links?', 0, 0),
('response_wtb_items_channel', 'WTB %formatted_item_links - name your price.', 0, 0),
('response_wtb_items_channel', 'Need %formatted_item_links, will pay above auction house.', 0, 0),
('response_wtb_items_channel', 'Buying %formatted_item_links, gold ready.', 0, 0),
('response_wtb_items_channel', 'WTB %formatted_item_links, whisper me.', 0, 0),
('response_wtb_items_channel', 'Still after %formatted_item_links if anyone has spares.', 0, 0),
('response_wtb_items_channel', 'Paying good gold for %formatted_item_links.', 0, 0),
('response_wtb_items_channel', 'WTB %formatted_item_links - been looking all week.', 0, 0),
('response_wtb_items_channel', 'Anybody got %formatted_item_links sitting in the bank?', 0, 0),
('response_wtb_items_channel', 'In the market for %formatted_item_links.', 0, 0),
('response_wtb_items_channel', 'WTB %formatted_item_links, happy to overpay.', 0, 0),
('response_wtb_items_channel', 'Seeking %formatted_item_links, reasonable offers accepted.', 0, 0),
('response_wtb_items_channel', 'If you have %formatted_item_links I have gold.', 0, 0),
('response_wtb_items_channel', 'WTB %formatted_item_links - bulk welcome.', 0, 0),
('response_wtb_items_channel', 'Chasing %formatted_item_links, PST with a price.', 0, 0),
('response_wtb_items_channel', 'Anyone farming %formatted_item_links? I will buy the lot.', 0, 0),
('response_wtb_items_channel', 'WTB %formatted_item_links, no lowballs please.', 0, 0),
('response_wtb_items_channel', 'Trying to buy %formatted_item_links before the raid tonight.', 0, 0),

-- -------------------------------------------------------- WTB whisper (59 -> 74)
('response_wtb_items_whisper', 'I can do %cost_gold for %formatted_item_links, deal?', 0, 0),
('response_wtb_items_whisper', 'Would %cost_gold work for %formatted_item_links?', 0, 0),
('response_wtb_items_whisper', 'Offering %cost_gold for %formatted_item_links.', 0, 0),
('response_wtb_items_whisper', '%formatted_item_links for %cost_gold? I am interested.', 0, 0),
('response_wtb_items_whisper', 'Best I can manage is %cost_gold for %formatted_item_links.', 0, 0),
('response_wtb_items_whisper', 'I will take %formatted_item_links at %cost_gold if that is fair.', 0, 0),
('response_wtb_items_whisper', 'Still have %formatted_item_links? I can pay %cost_gold.', 0, 0),
('response_wtb_items_whisper', 'Happy to trade %cost_gold for %formatted_item_links right now.', 0, 0),
('response_wtb_items_whisper', 'Where are you? I have %cost_gold ready for %formatted_item_links.', 0, 0),
('response_wtb_items_whisper', 'That is a bit steep. %cost_gold for %formatted_item_links?', 0, 0),
('response_wtb_items_whisper', 'Meet me at the bank, %cost_gold for %formatted_item_links.', 0, 0),
('response_wtb_items_whisper', 'Deal at %cost_gold for %formatted_item_links, if you are quick.', 0, 0),
('response_wtb_items_whisper', 'I need %formatted_item_links badly. %cost_gold?', 0, 0),
('response_wtb_items_whisper', 'Can you hold %formatted_item_links for me? %cost_gold.', 0, 0),
('response_wtb_items_whisper', 'Fair price for %formatted_item_links would be about %cost_gold.', 0, 0),

-- -------------------------------------------------------- LFG in channel (66 -> 81)
('response_lfg_quests_channel', 'LFG %quest_links in %zone_name, I can go as %my_role.', 0, 0),
('response_lfg_quests_channel', 'Anyone doing %quest_links? I am in %zone_name.', 0, 0),
('response_lfg_quests_channel', 'Need help with %quest_links, %my_role here.', 0, 0),
('response_lfg_quests_channel', 'LFM for %quest_links in %zone_name!', 0, 0),
('response_lfg_quests_channel', 'Struggling with %quest_links, anyone free?', 0, 0),
('response_lfg_quests_channel', 'Group forming for %quest_links, whisper me.', 0, 0),
('response_lfg_quests_channel', 'Two of us on %quest_links, room for more in %zone_name.', 0, 0),
('response_lfg_quests_channel', 'Looking for a group, %quest_links, will play %my_role.', 0, 0),
('response_lfg_quests_channel', 'Who else has %quest_links? Let us team up.', 0, 0),
('response_lfg_quests_channel', 'Sitting in %zone_name waiting on %quest_links.', 0, 0),
('response_lfg_quests_channel', 'Happy to help anyone with %quest_links, I am %my_role.', 0, 0),
('response_lfg_quests_channel', 'LFG %quest_links, been stuck on this for ages.', 0, 0),
('response_lfg_quests_channel', 'Anyone in %zone_name up for %quest_links?', 0, 0),
('response_lfg_quests_channel', 'Forming for %quest_links, need a couple more.', 0, 0),
('response_lfg_quests_channel', 'Would love a hand with %quest_links.', 0, 0),

-- -------------------------------------------------------- guild recruitment (15 -> 35)
('broadcast_guild_recruitment', 'Friendly %my_class looking for an active guild.', 0, 0),
('broadcast_guild_recruitment', 'LF guild that actually runs dungeons. %my_class here.', 0, 0),
('broadcast_guild_recruitment', 'Any guilds recruiting? I am a reliable %my_class.', 0, 0),
('broadcast_guild_recruitment', 'Guildless %my_class seeking good company.', 0, 0),
('broadcast_guild_recruitment', 'LFGuild - social, not hardcore.', 0, 0),
('broadcast_guild_recruitment', 'Looking for a guild with people who talk. %my_class.', 0, 0),
('broadcast_guild_recruitment', 'Recruiting all levels! Friendly guild, no drama.', 0, 0),
('broadcast_guild_recruitment', 'We are recruiting - dungeons, questing, and bad jokes.', 0, 0),
('broadcast_guild_recruitment', 'Small guild looking to grow, all welcome.', 0, 0),
('broadcast_guild_recruitment', 'LF guild for levelling and eventually raiding.', 0, 0),
('broadcast_guild_recruitment', 'Guild recruiting! We have a bank tab and everything.', 0, 0),
('broadcast_guild_recruitment', 'Returning %my_class looking for a home.', 0, 0),
('broadcast_guild_recruitment', 'Anyone recruiting? Happy to help with guild quests.', 0, 0),
('broadcast_guild_recruitment', 'Casual guild recruiting casual players. Perfect match.', 0, 0),
('broadcast_guild_recruitment', 'LFGuild - I bring enchanting and enthusiasm.', 0, 0),
('broadcast_guild_recruitment', 'Guild looking for members. No attendance requirements.', 0, 0),
('broadcast_guild_recruitment', 'Solo %my_class tired of soloing.', 0, 0),
('broadcast_guild_recruitment', 'Recruiting friendly players for a laid back guild.', 0, 0),
('broadcast_guild_recruitment', 'LF active guild, any faction-appropriate offers considered.', 0, 0),
('broadcast_guild_recruitment', 'Guild with a heartbeat wanted. %my_class available.', 0, 0),

-- -------------------------------------------------------- suggest quest (57 -> 72)
('suggest_quest', 'Anyone else working on %quest_link?', 0, 0),
('suggest_quest', 'Is %quest_link soloable or should I find help?', 0, 0),
('suggest_quest', 'Stuck on %quest_link, any tips?', 0, 0),
('suggest_quest', 'Where does %quest_link actually start?', 0, 0),
('suggest_quest', '%quest_link is taking forever.', 0, 0),
('suggest_quest', 'Just picked up %quest_link, wish me luck.', 0, 0),
('suggest_quest', 'Does anyone remember how %quest_link works?', 0, 0),
('suggest_quest', 'Two of us on %quest_link, want to join?', 0, 0),
('suggest_quest', '%quest_link has terrible drop rates.', 0, 0),
('suggest_quest', 'Skipping %quest_link, life is too short.', 0, 0),
('suggest_quest', 'Finally understood what %quest_link wants from me.', 0, 0),
('suggest_quest', 'Is %quest_link worth doing for the reward?', 0, 0),
('suggest_quest', 'Anyone got %quest_link in their log? Let us share.', 0, 0),
('suggest_quest', 'The escort on %quest_link keeps dying.', 0, 0),
('suggest_quest', 'Halfway through %quest_link and regretting it.', 0, 0),

-- -------------------------------------------------------- suggest trade (91 -> 106)
('suggest_trade', 'Anyone dealing in %category today?', 0, 0),
('suggest_trade', 'Selling %category, fair prices.', 0, 0),
('suggest_trade', 'WTB %category in bulk.', 0, 0),
('suggest_trade', 'Best place to buy %category around here?', 0, 0),
('suggest_trade', '%category prices are absurd right now.', 0, 0),
('suggest_trade', 'Trading %category, whisper offers.', 0, 0),
('suggest_trade', 'Anyone crafting %category? I will supply mats.', 0, 0),
('suggest_trade', 'Looking for a reliable %category supplier.', 0, 0),
('suggest_trade', 'Undercut on %category again. Ruthless out there.', 0, 0),
('suggest_trade', 'Got spare %category if anyone needs it.', 0, 0),
('suggest_trade', 'The %category market has completely crashed.', 0, 0),
('suggest_trade', 'Will trade %category for equivalent value.', 0, 0),
('suggest_trade', 'Stocking up on %category before prices climb.', 0, 0),
('suggest_trade', 'Anyone know a fair rate for %category?', 0, 0),
('suggest_trade', 'Clearing out my bank - %category available.', 0, 0),

-- -------------------------------------------------------- suggest instance (106 -> 121)
('suggest_instance', 'LFG %instance_name, I can go %my_role.', 0, 0),
('suggest_instance', 'Anyone up for %instance_name?', 0, 0),
('suggest_instance', 'Need one more for %instance_name!', 0, 0),
('suggest_instance', '%instance_name run forming, whisper me.', 0, 0),
('suggest_instance', 'Never done %instance_name, happy to learn.', 0, 0),
('suggest_instance', 'Is %instance_name worth running at my level?', 0, 0),
('suggest_instance', 'Third attempt at %instance_name tonight. We will get it.', 0, 0),
('suggest_instance', '%my_class looking for %instance_name group.', 0, 0),
('suggest_instance', 'Anyone know a shortcut in %instance_name?', 0, 0),
('suggest_instance', 'LFM %instance_name - need a %my_role.', 0, 0),
('suggest_instance', '%instance_name has the best loot for my slot.', 0, 0),
('suggest_instance', 'Summoning for %instance_name, who needs a lift?', 0, 0),
('suggest_instance', 'Quick %instance_name run before I log?', 0, 0),
('suggest_instance', 'That last boss in %instance_name is brutal.', 0, 0),
('suggest_instance', 'Farming %instance_name until it gives me what I want.', 0, 0),

-- -------------------------------------------------------- suggest sell (83 -> 98)
('suggest_sell', 'Selling %item_formatted_link, %cost_gold.', 0, 0),
('suggest_sell', '%item_formatted_link going for %cost_gold, first come first served.', 0, 0),
('suggest_sell', 'WTS %item_formatted_link - %cost_gold or best offer.', 0, 0),
('suggest_sell', 'Clearing bag space: %item_formatted_link, %cost_gold.', 0, 0),
('suggest_sell', 'Anyone want %item_formatted_link? %cost_gold.', 0, 0),
('suggest_sell', '%item_formatted_link for sale, asking %cost_gold.', 0, 0),
('suggest_sell', 'Cheap: %item_formatted_link at %cost_gold.', 0, 0),
('suggest_sell', 'Selling %item_formatted_link, will haggle.', 0, 0),
('suggest_sell', 'WTS %item_formatted_link, %cost_gold, no auction house fees.', 0, 0),
('suggest_sell', 'Last call on %item_formatted_link for %cost_gold.', 0, 0),
('suggest_sell', 'Undercutting the auction house: %item_formatted_link, %cost_gold.', 0, 0),
('suggest_sell', 'Have %item_formatted_link spare, %cost_gold to a good home.', 0, 0),
('suggest_sell', 'Selling %item_formatted_link. Serious offers only.', 0, 0),
('suggest_sell', '%item_formatted_link, %cost_gold, I am in town now.', 0, 0),
('suggest_sell', 'Do not need %item_formatted_link any more. %cost_gold.', 0, 0),

-- -------------------------------------------------------- toxic links (12 -> 27)
('suggest_toxic_links', 'Check out %random_inventory_item_link, better than anything you have.', 0, 0),
('suggest_toxic_links', 'Bet none of you have %random_inventory_item_link.', 0, 0),
('suggest_toxic_links', 'Casually carrying %random_inventory_item_link.', 0, 0),
('suggest_toxic_links', 'Some of us have %random_inventory_item_link. Some of us do not.', 0, 0),
('suggest_toxic_links', 'Just linking %random_inventory_item_link for no reason at all.', 0, 0),
('suggest_toxic_links', 'Anyone else got %random_inventory_item_link or just me?', 0, 0),
('suggest_toxic_links', 'Working on %random_taken_quest_or_item_link, not that you asked.', 0, 0),
('suggest_toxic_links', 'Look at %random_inventory_item_link and weep.', 0, 0),
('suggest_toxic_links', 'Been carrying %random_inventory_item_link since before it was cool.', 0, 0),
('suggest_toxic_links', 'Still better geared than most. %random_inventory_item_link', 0, 0),
('suggest_toxic_links', 'Nobody asked, but %random_inventory_item_link.', 0, 0),
('suggest_toxic_links', 'This is what real progress looks like: %random_inventory_item_link', 0, 0),
('suggest_toxic_links', 'Already finished %random_taken_quest_or_item_link. Easy.', 0, 0),
('suggest_toxic_links', 'Imagine not having %random_inventory_item_link.', 0, 0),
('suggest_toxic_links', 'Linking %random_inventory_item_link again because I can.', 0, 0);
