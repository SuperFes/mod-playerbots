-- Expansion: World Events, Accomplishment Reactions, Wipes, Weather & City Arrivals
-- Target table: ai_playerbot_texts

INSERT INTO `ai_playerbot_texts` (`name`, `text`, `say_type`, `reply_type`) VALUES

-- Achievement / Accomplishment Reactions
('broadcast_achievement_complete', 'Grats on the achievement %other_name!', 0, 0),
('broadcast_achievement_complete', 'Gz %other_name!', 0, 0),
('broadcast_achievement_complete', 'Congratulations %other_name!', 0, 0),
('broadcast_achievement_complete', 'Nice achievement %other_name!', 0, 0),
('broadcast_achievement_complete', 'Grats!', 0, 0),
('broadcast_achievement_complete', 'GZ %other_name! Big milestone!', 0, 0),
('broadcast_achievement_complete', 'Awesome job %other_name! Grats!', 0, 0),
('broadcast_achievement_complete', 'Grats on achievement!', 0, 0),
('broadcast_achievement_complete', 'Huge gz %other_name!', 0, 0),
('broadcast_achievement_complete', 'Congrats %other_name! Well earned!', 0, 0),
('broadcast_achievement_complete', 'Gz on the points %other_name!', 0, 0),
('broadcast_achievement_complete', 'Another achievement unlocked! Grats %other_name!', 0, 0),
('broadcast_achievement_complete', 'Way to go %other_name! Gz!', 0, 0),
('broadcast_achievement_complete', 'Champ status! Congrats %other_name!', 0, 0),

-- Dungeon Wipe & Corpse Run Lines
('broadcast_dungeon_wipe', 'Oof... tough wipe guys. Releasing spirit.', 0, 0),
('broadcast_dungeon_wipe', 'My bad on that pull! Running back to corpse now.', 0, 0),
('broadcast_dungeon_wipe', 'Corpse run time! Meet you guys at the dungeon entrance.', 0, 0),
('broadcast_dungeon_wipe', 'That boss mechanic caught us off guard. Let''s try again!', 0, 0),
('broadcast_dungeon_wipe', 'Wipe happens! Rebuffing as soon as we reach our corpses.', 0, 0),
('broadcast_dungeon_wipe', 'Dang, almost had it! Releasing spirit.', 0, 0),
('broadcast_dungeon_wipe', 'Don''t forget to repair gear before we pull again!', 0, 0),
('broadcast_dungeon_wipe', 'Targeting order was a bit messy, let''s focus mark next time.', 0, 0),
('broadcast_dungeon_wipe', 'Back from the graveyard! Ready for round 2.', 0, 0),
('broadcast_dungeon_wipe', 'Unfortunate wipe, but we''ve got this on the next pull!', 0, 0),

-- Taxi Flight Arrival Comments
('broadcast_flight_arrival', 'Made it to %zone_name in one piece!', 0, 0),
('broadcast_flight_arrival', 'Long flight... glad to be back on solid ground in %zone_name.', 0, 0),
('broadcast_flight_arrival', 'Touchdown in %zone_name! Time to get to work.', 0, 0),
('broadcast_flight_arrival', 'Great view from up there! Arrived at %area_name.', 0, 0),
('broadcast_flight_arrival', 'Gryphon flight complete! Ready for action in %zone_name.', 0, 0),
('broadcast_flight_arrival', 'Finally landed in %zone_name. My legs were falling asleep!', 0, 0),
('broadcast_flight_arrival', 'Wind in my face, solid ground beneath my feet in %area_name.', 0, 0),
('broadcast_flight_arrival', 'Landed safely at the flight master in %zone_name.', 0, 0),
('broadcast_flight_arrival', 'Ah, fresh air in %zone_name after that flight.', 0, 0),

-- Capital City & Zone Arrival Chatter
('broadcast_city_arrival', 'Ah, good to be back in %zone_name!', 0, 0),
('broadcast_city_arrival', '%zone_name is looking busy today! Time to hit the AH.', 0, 0),
('broadcast_city_arrival', 'Just arrived in %zone_name. Anyone doing quests around here?', 0, 0),
('broadcast_city_arrival', 'Finally made it to %zone_name!', 0, 0),
('broadcast_city_arrival', 'Checking in at %zone_name! Time to restock food and water.', 0, 0),
('broadcast_city_arrival', 'City guards in %zone_name look as vigilant as ever.', 0, 0),
('broadcast_city_arrival', 'Stopping by %zone_name for repairs and auction house checks.', 0, 0),
('broadcast_city_arrival', 'Always feels lively in %zone_name!', 0, 0),
('broadcast_city_arrival', 'Arrived in %area_name. Where''s the nearest class trainer?', 0, 0),
('broadcast_city_arrival', 'Home sweet home in %zone_name.', 0, 0),

-- Weather & Atmosphere Comments
('broadcast_weather_comment', 'Love the ambient rain falling in %zone_name.', 0, 0),
('broadcast_weather_comment', 'Brr, freezing snow in %zone_name today! Need a campfire.', 0, 0),
('broadcast_weather_comment', 'Dark storm clouds rolling over %zone_name...', 0, 0),
('broadcast_weather_comment', 'Nothing beats rainy weather in %zone_name.', 0, 0),
('broadcast_weather_comment', 'Visibility is rough with all this fog in %area_name.', 0, 0),
('broadcast_weather_comment', 'Listen to that thunder in %zone_name!', 0, 0),
('broadcast_weather_comment', 'Snowing again in %zone_name! Cozy vibes.', 0, 0),
('broadcast_weather_comment', 'Rainy days in %zone_name are perfect for fishing.', 0, 0),
('broadcast_weather_comment', 'Warm sunshine in %zone_name for a change!', 0, 0),
('broadcast_weather_comment', 'Heavy downpour in %area_name, hope my gear doesn''t rust.', 0, 0);
