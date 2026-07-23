-- Expansion: Player Behaviors, Durability Complaints, and Extra Variations
-- Target table: ai_playerbot_texts

INSERT INTO `ai_playerbot_texts` (`name`, `text`, `say_type`, `reply_type`) VALUES

-- Low Durability Complaints
('broadcast_durability_low', 'Yellow durability warning... hope my armor holds!', 0, 0),
('broadcast_durability_low', 'My gear is getting pretty beat up. Need a repair anvil ASAP!', 0, 0),
('broadcast_durability_low', 'Durability is critical! Anyone got a Jeeves or repair bot?', 0, 0),
('broadcast_durability_low', 'My armor is almost broken! Stopping by a smith soon.', 0, 0),
('broadcast_durability_low', 'Oof, durability warning popping up. Time for repairs!', 0, 0),

-- Extra General Realm Chat Banter
('suggest_something', 'LF 1 Tank for Heroic dungeon run! PST!', 0, 0),
('suggest_something', 'Need 1 Healer to complete our group! PST for invite.', 0, 0),
('suggest_something', 'Anyone selling Northrend herbs or ores in bulk?', 0, 0),
('suggest_something', 'What is your go-to profession for making gold?', 0, 0),
('suggest_something', 'Anyone want to team up for elite quests in %zone_name?', 0, 0),
('suggest_something', 'Don''t forget to set your hearthstone at the local inn!', 0, 0),

-- Additional Weather Remarks
('broadcast_weather_comment', 'Clear skies in %zone_name today! Great day for grinding.', 0, 0),
('broadcast_weather_comment', 'Foggy morning in %area_name.', 0, 0),

-- Additional Flight & City Arrivals
('broadcast_flight_arrival', 'Landed at %area_name flight point! Ready to roll.', 0, 0),
('broadcast_city_arrival', 'Greetings %zone_name! Time to turn in quests.', 0, 0);
