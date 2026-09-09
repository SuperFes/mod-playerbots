-- Bots announcing that they are on their way after the master leaves a city.
-- Target table: ai_playerbot_texts
--
-- Emitted once per departure, in party chat, immediately before the bot teleports.
-- Placeholder: %my_class.

INSERT INTO `ai_playerbot_texts` (`name`, `text`, `say_type`, `reply_type`) VALUES
('city_rejoin', 'Coming! Give me a second.', 0, 0),
('city_rejoin', 'Right behind you - just finishing up here.', 0, 0),
('city_rejoin', 'On my way!', 0, 0),
('city_rejoin', 'Wait for me!', 0, 0),
('city_rejoin', 'Catching up now.', 0, 0),
('city_rejoin', 'Hold on, I am coming.', 0, 0),
('city_rejoin', 'Leaving already? I am on it.', 0, 0),
('city_rejoin', 'Packing up, be right there.', 0, 0),
('city_rejoin', 'Do not start without me.', 0, 0),
('city_rejoin', 'Coming, coming.', 0, 0),
('city_rejoin', 'Almost forgot we were going somewhere.', 0, 0),
('city_rejoin', 'One moment - right, I am with you.', 0, 0),
('city_rejoin', 'Following you out.', 0, 0),
('city_rejoin', 'That is me done in town anyway.', 0, 0),
('city_rejoin', 'I will catch you up.', 0, 0);
