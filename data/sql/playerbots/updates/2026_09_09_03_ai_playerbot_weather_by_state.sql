-- Weather commentary, tagged by actual weather state.
-- Target table: ai_playerbot_texts
--
-- broadcast_weather_comment held 45 lines that nothing ever emitted, and they could not simply be
-- wired up: the pool mixes rain, snow, fog, storms and clear skies, so emitting it blind meant bots
-- complaining about downpours in sunshine. This reclassifies every one of those lines into a
-- state-specific key and adds the cases the original pool had none of.
--
-- weather_rough is the catch-all for any bad weather, used for variety alongside the specific key.
-- Placeholders unchanged: %zone_name, %area_name.

-- weather_fine
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fine' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'beautiful day';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fine' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Beautiful skies over %area_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fine' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Clear skies for once in %area_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fine' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Clear skies in %zone_name today! Great day for grinding.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fine' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Good visibility today across %area_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fine' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Lovely day for it in %area_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fine' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'nice weather in %zone_name';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fine' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Quiet evening in %zone_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fine' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'The light over %zone_name right now is something else.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fine' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Warm sunshine in %zone_name for a change!';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fine' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Too hot. Far too hot. %zone_name.';

-- weather_fog
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fog' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Cannot see a thing in this. %zone_name is soup.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fog' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'cant see anything in this fog';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fog' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Foggy morning in %area_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fog' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Mist so thick in %area_name I nearly walked into a mob.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fog' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Visibility is rough with all this fog in %area_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_fog' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'so dark here';

-- weather_rain
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rain' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Heavy downpour in %area_name, hope my gear doesn''t rust.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rain' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'it never stops raining';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rain' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Love the ambient rain falling in %zone_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rain' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Nothing beats rainy weather in %zone_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rain' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Perfect fishing weather in %zone_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rain' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Rain again. Of course it is. %zone_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rain' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'raining again';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rain' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Rainy days in %zone_name are perfect for fishing.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rain' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Soaked through. Thanks, %zone_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rain' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'You could set a clock by the rain in %zone_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rain' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Does the sun ever come out in %area_name?';

-- weather_snow
UPDATE `ai_playerbot_texts` SET `name` = 'weather_snow' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Brr, freezing snow in %zone_name today! Need a campfire.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_snow' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Freezing out here in %zone_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_snow' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'snowing';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_snow' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Snowing again in %zone_name! Cozy vibes.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_snow' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Snowing in %zone_name. Naturally.';

-- weather_storm
UPDATE `ai_playerbot_texts` SET `name` = 'weather_storm' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Dark storm clouds rolling over %zone_name...';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_storm' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Listen to that thunder in %zone_name!';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_storm' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Storm rolling in over %zone_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_storm' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'This wind is going straight through my armour.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_storm' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Wind is picking up in %zone_name.';

-- weather_rough
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rough' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Grim weather over %zone_name today.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rough' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'I did not pack for %zone_name weather.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rough' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'I would trade a level for better weather in %zone_name.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rough' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Weather in %zone_name has a personality, and it is a mean one.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rough' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'Weather like this makes me want to sit by an inn fire.';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rough' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'weather sucks today';
UPDATE `ai_playerbot_texts` SET `name` = 'weather_rough' WHERE `name` = 'broadcast_weather_comment' AND `text` = 'You get used to the climate in %zone_name. Eventually.';

-- states the original pool did not cover, plus a few extra per bucket
INSERT INTO `ai_playerbot_texts` (`name`, `text`, `say_type`, `reply_type`) VALUES
('weather_sandstorm', 'Sand everywhere. I will be finding it for weeks. %zone_name.', 0, 0),
('weather_sandstorm', 'Cannot see through this sandstorm in %area_name.', 0, 0),
('weather_sandstorm', '%zone_name is trying to bury us alive.', 0, 0),
('weather_sandstorm', 'Grit in my eyes, grit in my pack, grit in my rations.', 0, 0),
('weather_sandstorm', 'This sandstorm is relentless.', 0, 0),
('weather_sandstorm', 'Wind and sand. Wonderful. %zone_name.', 0, 0),
('weather_sandstorm', 'I have swallowed half of %zone_name already.', 0, 0),
('weather_sandstorm', 'Keep close, I can barely see you in this.', 0, 0),
('weather_fine', 'Perfect weather to be out in %zone_name.', 0, 0),
('weather_fine', 'Not a cloud over %area_name.', 0, 0),
('weather_fine', 'Days like this are why I do not stay in the city.', 0, 0),
('weather_storm', 'That thunder went right through me.', 0, 0),
('weather_storm', 'Lightning over %zone_name. Best not stand on high ground.', 0, 0),
('weather_storm', 'The sky over %area_name looks furious.', 0, 0),
('weather_snow', 'My boots are full of snow.', 0, 0),
('weather_snow', 'Everything in %zone_name is white and I can feel none of my fingers.', 0, 0),
('weather_fog', 'I can hear things in this fog I would rather not.', 0, 0),
('weather_fog', 'Stay close, %area_name is thick today.', 0, 0);
