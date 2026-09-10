-- Bots announcing they are teleporting to a master who changed map.
-- Target table: ai_playerbot_texts
--
-- Distinct from city_rejoin: this is following through a portal or into an instance, which reads
-- differently from wrapping up errands in town.

INSERT INTO `ai_playerbot_texts` (`name`, `text`, `say_type`, `reply_type`) VALUES
('cross_map_rejoin', 'Right behind you.', 0, 0),
('cross_map_rejoin', 'Coming through!', 0, 0),
('cross_map_rejoin', 'Do not pull anything yet, I am on my way.', 0, 0),
('cross_map_rejoin', 'Hold the door, I am coming.', 0, 0),
('cross_map_rejoin', 'Almost lost you there.', 0, 0),
('cross_map_rejoin', 'Following you in.', 0, 0),
('cross_map_rejoin', 'Wait for me at the entrance!', 0, 0),
('cross_map_rejoin', 'Catching up - do not start without me.', 0, 0),
('cross_map_rejoin', 'Through the portal, give me a second.', 0, 0),
('cross_map_rejoin', 'I hate portals. On my way.', 0, 0),
('cross_map_rejoin', 'Not getting left outside again.', 0, 0),
('cross_map_rejoin', 'Right, I am in.', 0, 0);
