-- Initial expansion of playerbot texts and phrases across all game events
-- Target table: ai_playerbot_texts

INSERT INTO `ai_playerbot_texts` (`name`, `text`, `say_type`, `reply_type`) VALUES

-- =========================================================
-- 1. LOOTING BROADCASTS
-- =========================================================
-- Poor Quality Items
('broadcast_looting_item_poor', 'Great... more vendor trash %item_link.', 0, 0),
('broadcast_looting_item_poor', 'Why do I keep picking up garbage like %item_link?', 0, 0),
('broadcast_looting_item_poor', '%item_link... straight to the vendor it goes.', 0, 0),
('broadcast_looting_item_poor', 'My bags are filling up with junk like %item_link.', 0, 0),
('broadcast_looting_item_poor', 'Another day, another bag full of %item_link.', 0, 0),
('broadcast_looting_item_poor', 'Ugh, I need a bigger inventory if I keep looting %item_link.', 0, 0),
('broadcast_looting_item_poor', 'Who even drops %item_link? Total pocket lint.', 0, 0),
('broadcast_looting_item_poor', 'Selling %item_link to the nearest merchant for copper...', 0, 0),

-- Normal Quality Items
('broadcast_looting_item_normal', 'Just picked up %item_link.', 0, 0),
('broadcast_looting_item_normal', 'Added %item_link to my bag.', 0, 0),
('broadcast_looting_item_normal', 'Found some %item_link in %zone_name.', 0, 0),
('broadcast_looting_item_normal', '%item_link, not bad for handy supplies.', 0, 0),
('broadcast_looting_item_normal', 'Looted %item_link. Might be useful later.', 0, 0),
('broadcast_looting_item_normal', 'Stashing %item_link away in my backpack.', 0, 0),
('broadcast_looting_item_normal', 'Picked up %item_link while exploring %area_name.', 0, 0),

-- Uncommon Quality Items (Greens)
('broadcast_looting_item_uncommon', 'Nice, just looted %item_link!', 0, 0),
('broadcast_looting_item_uncommon', '%item_link will definitely come in handy.', 0, 0),
('broadcast_looting_item_uncommon', 'Scored %item_link in %zone_name!', 0, 0),
('broadcast_looting_item_uncommon', 'Solid drop! %item_link is a good upgrade candidate.', 0, 0),
('broadcast_looting_item_uncommon', 'Hey, a green! %item_link freshly acquired.', 0, 0),
('broadcast_looting_item_uncommon', 'Not bad at all! %item_link looted.', 0, 0),
('broadcast_looting_item_uncommon', 'Money money money, or maybe an upgrade: %item_link!', 0, 0),

-- Rare Quality Items (Blues)
('broadcast_looting_item_rare', 'Sweet! Freshly looted %item_link in %zone_name!', 0, 0),
('broadcast_looting_item_rare', 'RNGesus is smiling on me today: %item_link!', 0, 0),
('broadcast_looting_item_rare', 'Check this out! Just looted %item_link!', 0, 0),
('broadcast_looting_item_rare', 'Huge upgrade alert! %item_link for %my_class!', 0, 0),
('broadcast_looting_item_rare', 'Is %item_link Hunter BiS? Or maybe for %my_class?', 0, 0),
('broadcast_looting_item_rare', 'A blue item! %item_link just dropped for me in %area_name!', 0, 0),
('broadcast_looting_item_rare', 'Boom! %item_link acquired! Luck is on my side today.', 0, 0),

-- Epic Quality Items (Purples)
('broadcast_looting_item_epic', 'WOAH! Look at what just dropped: %item_link!!!', 0, 0),
('broadcast_looting_item_epic', 'No way! An epic drop! %item_link is mine!', 0, 0),
('broadcast_looting_item_epic', 'Jackpot! %item_link in %zone_name! Best day ever!', 0, 0),
('broadcast_looting_item_epic', 'Holy light! An epic item! %item_link looted!', 0, 0),
('broadcast_looting_item_epic', 'Speechless... %item_link just dropped! Still shaking!', 0, 0),

-- Legendary Quality Items (Oranges)
('broadcast_looting_item_legendary', 'UNBELIEVABLE! I just got %item_link! Truly legendary!', 0, 0),
('broadcast_looting_item_legendary', 'History was made today... %item_link is mine!', 0, 0),
('broadcast_looting_item_legendary', 'Did someone say %item_link?! IT IS REAL!', 0, 0),

-- =========================================================
-- 2. KILL BROADCASTS
-- =========================================================
-- Normal Kills
('broadcast_killed_normal', 'Another enemy down in %zone_name.', 0, 0),
('broadcast_killed_normal', 'Target eliminated. Moving on!', 0, 0),
('broadcast_killed_normal', 'Clean kill in %area_name.', 0, 0),
('broadcast_killed_normal', 'Scratch one more off the list.', 0, 0),
('broadcast_killed_normal', 'Easy target down. Next!', 0, 0),

-- Elite Kills
('broadcast_killed_elite', 'Down goes the elite! Great fight!', 0, 0),
('broadcast_killed_elite', 'That elite didn''t stand a chance against %my_class!', 0, 0),
('broadcast_killed_elite', 'Elite mob defeated in %zone_name! Excellent teamwork!', 0, 0),
('broadcast_killed_elite', 'Challenging fight, but the elite has fallen!', 0, 0),

-- Rare & Rare Elite Kills
('broadcast_killed_rare', 'Rare spawn defeated in %zone_name! Time to loot!', 0, 0),
('broadcast_killed_rare', 'Found and killed a rare in %area_name!', 0, 0),
('broadcast_killed_rareelite', 'Rare Elite destroyed! Big loot coming up!', 0, 0),
('broadcast_killed_rareelite', 'Tough rare elite conquered in %zone_name!', 0, 0),

-- World Boss Kills
('broadcast_killed_worldboss', 'VICTORY! The world boss has fallen!', 0, 0),
('broadcast_killed_worldboss', 'EPIC BATTLE! World boss eliminated in %zone_name!', 0, 0),
('broadcast_killed_worldboss', 'WE DID IT! World boss defeated! Glory to our team!', 0, 0),

-- Player Kills (PVP)
('broadcast_killed_player', 'Sit down! Enemy player defeated in %zone_name!', 0, 0),
('broadcast_killed_player', 'PVP kill confirmed! For Azeroth!', 0, 0),
('broadcast_killed_player', 'Outplayed! Another enemy player down!', 0, 0),
('broadcast_killed_player', 'Back to the spirit healer for you! PVP kill in %area_name!', 0, 0),
('broadcast_killed_player', 'Victory in combat! Enemy player defeated!', 0, 0),

-- =========================================================
-- 3. LEVEL UP BROADCASTS
-- =========================================================
('broadcast_levelup_generic', 'DING! Reached level %my_level!', 0, 0),
('broadcast_levelup_generic', 'Level %my_level achieved in %zone_name! Getting stronger every day!', 0, 0),
('broadcast_levelup_generic', 'Level %my_level! New spells and talents unlocked!', 0, 0),
('broadcast_levelup_generic', 'DING! Level %my_level. The grind continues!', 0, 0),
('broadcast_levelup_generic', 'Power growing! Just hit level %my_level!', 0, 0),

('broadcast_levelup_10x', 'Milestone reached! Level %my_level unlocked!', 0, 0),
('broadcast_levelup_10x', 'Double digits / milestone! Level %my_level achieved!', 0, 0),
('broadcast_levelup_10x', 'Hit level %my_level! Time to check the trainer for major upgrades!', 0, 0),

('broadcast_levelup_max_level', 'MAX LEVEL ACHIEVED! Ready for endgame raids and PVP!', 0, 0),
('broadcast_levelup_max_level', 'Finally level 80! Time to gear up for Heroics and Naxxramas!', 0, 0),
('broadcast_levelup_max_level', 'Level cap reached! Let the real game begin!', 0, 0),

-- =========================================================
-- 4. QUEST BROADCASTS & DIALOGUE
-- =========================================================
('broadcast_quest_accepted_generic', 'Started new quest: %quest_link.', 0, 0),
('broadcast_quest_accepted_generic', 'Accepted quest %quest_link in %zone_name.', 0, 0),
('broadcast_quest_accepted_generic', 'Added %quest_link to my quest log. Let''s get to work!', 0, 0),

('broadcast_quest_update_add_kill_objective_progress', 'Progress on %quest_link: %current/%total objectives completed.', 0, 0),
('broadcast_quest_update_add_item_objective_progress', 'Gathered item for %quest_link: %current/%total collected.', 0, 0),

('broadcast_quest_update_complete', 'Quest objective complete for %quest_link!', 0, 0),
('broadcast_quest_update_complete', 'All objectives finished for %quest_link! Ready to turn in!', 0, 0),

('broadcast_quest_turned_in', 'Quest turned in: %quest_link. On to the next adventure!', 0, 0),
('broadcast_quest_turned_in', 'Handed in %quest_link! Got my rewards and experience.', 0, 0),
('broadcast_quest_turned_in', 'Quest completed and turned in: %quest_link!', 0, 0),

('quest_accepted', 'Quest accepted! Let''s do this.', 0, 0),
('quest_accepted', 'Got the quest, lead the way!', 0, 0),
('quest_status_completed', 'Quest is complete! Ready to claim the reward.', 0, 0),
('quest_status_complete_single_reward', 'Done! Claiming my reward for %quest.', 0, 0),

-- =========================================================
-- 5. SUGGESTIONS & BROADCAST CHAT
-- =========================================================
('suggest_instance', 'Anyone interested in running %instance_name? I can fill as %my_class!', 0, 0),
('suggest_instance', 'Looking to group up for %instance_name! Hit me up!', 0, 0),
('suggest_instance', 'LFG for %instance_name! Ready to tank/heal/dps!', 0, 0),
('suggest_instance', 'Need a solid group for %instance_name! PST if interested.', 0, 0),

('suggest_sell', 'Selling %count x %item_link for %price gold! PST if interested.', 0, 0),
('suggest_sell', 'WTB / WTS: Offering %item_link x%count for %price gold.', 0, 0),

('suggest_something', 'Looking for some adventure in %zone_name! Anyone want to group up?', 0, 0),
('suggest_something', 'Cruising around %area_name. Anyone doing quests around here?', 0, 0),

('broadcast_guild_recruitment', 'Friendly guild recruiting active players of all levels! PST for invite!', 0, 0),
('broadcast_guild_recruitment', 'Join our guild for dungeon runs, levelling, and fun! All classes welcome.', 0, 0),

('broadcast_guild_promotion', 'Congratulations %other_name on the guild promotion! Well deserved!', 0, 0),
('broadcast_guild_promotion', 'Grats on the promotion %other_name! Keep up the great work!', 0, 0),

('broadcast_guild_demotion', 'Tough luck %other_name, but you''ll climb back up in no time.', 0, 0),

-- =========================================================
-- 6. TACTICAL & COMBAT SAY CALLOUTS
-- =========================================================
('critical health', 'Help! Critical health! I need healing NOW!', 0, 0),
('critical health', 'I''m taking heavy damage! Heal me please!', 0, 0),
('critical health', 'Health critical! Using defensive cooldowns!', 0, 0),
('critical health', 'Mayday! Mayday! Low health on %my_class!', 0, 0),

('low health', 'Health is getting low, watch out!', 0, 0),
('low health', 'Taking damage, could use a quick heal!', 0, 0),
('low health', 'Watch my HP bar, getting low!', 0, 0),

('low mana', 'Out of mana! Give me a second to drink.', 0, 0),
('low mana', 'OOM! Pausing for mana refill.', 0, 0),
('low mana', 'Low mana! Need a moment to recharge.', 0, 0),
('low mana', 'Running on empty! Need mana drink break.', 0, 0),

('low ammo', 'Running low on %ammo! Need to restock soon.', 0, 0),
('no ammo', 'I''m completely out of %ammo! Can''t attack at range!', 0, 0),

('taunt', 'Hold on to your hats, taking aggro on %target_name!', 0, 0),
('taunt', 'Taunting %target_name! Focus fire!', 0, 0),
('taunt', 'Taking threat on %target_name, keep DPSing!', 0, 0),

('aoe', 'Unleashing AoE damage on these targets!', 0, 0),
('aoe', 'Area of Effect spell coming down!', 0, 0),
('aoe', 'Clump them up, dropping AoE now!', 0, 0),

('attacking', 'Engaging %target_name! Attack!', 0, 0),
('attacking', 'Charging into battle against %target_name!', 0, 0),
('attacking', 'Targeting %target_name! Open fire!', 0, 0),

('fleeing', 'Tactical retreat! Fall back!', 0, 0),
('fleeing', 'Too many enemies! Pulling back!', 0, 0),
('fleeing_far', 'Retreating to a safe distance!', 0, 0),

('hello', 'Greetings, adventurer!', 0, 0),
('hello', 'Hey there! Ready to fight?', 0, 0),
('hello', 'Well met, friend!', 0, 0),
('hello', 'Hello! Good to see you in %zone_name.', 0, 0),

('goodbye', 'Safe travels, friend!', 0, 0),
('goodbye', 'Until next time! Take care out there.', 0, 0),

('following', 'Right behind you!', 0, 0),
('following', 'Lead the way, I''m right with you.', 0, 0),

('staying', 'Holding position right here.', 0, 0),
('staying', 'Standing guard at this spot.', 0, 0),

('grinding', 'Grinding mobs in %zone_name for exp and loot.', 0, 0),
('guarding', 'Standing guard in %area_name. All quiet for now.', 0, 0),

-- =========================================================
-- 7. REAGENT & CLASS BUFF CALLOUTS
-- =========================================================
('rp_missing_reagent_generic', 'I need class reagents to cast group buffs!', 0, 0),
('rp_missing_reagent_arcane_brilliance', 'Out of Arcane Powder for Arcane Brilliance!', 0, 0),
('rp_missing_reagent_gift_of_the_wild', 'Out of Wild Thorns for Gift of the Wild!', 0, 0),
('rp_missing_reagent_greater_blessing', 'Out of Symbol of Kings for Greater Blessings!', 0, 0);
