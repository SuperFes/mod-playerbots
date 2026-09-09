-- Text for the taxi failure cases that previously failed silently.
-- Target table: ai_playerbot_texts
--
-- The follower-bot take-off was scheduled on a timer and discarded the result of
-- ActivateTaxiPathTo, so a bot that could not board simply did not, with no message.
-- These keys back the three reported cases. The C++ passes defaults to GetBotTextOrDefault,
-- so the feature works before this file is applied - this only adds variety.

INSERT INTO `ai_playerbot_texts` (`name`, `text`, `say_type`, `reply_type`) VALUES
-- route never recorded (the master's activate-taxi packet did not reach the bot)
('taxi_no_route_remembered', 'I did not catch which flight you took.', 0, 0),
('taxi_no_route_remembered', 'Missed the route - where are you flying to?', 0, 0),
('taxi_no_route_remembered', 'I have no flight path recorded, sorry.', 0, 0),
('taxi_no_route_remembered', 'Did not see which destination you picked.', 0, 0),
('taxi_no_route_remembered', 'No route remembered - I will meet you there the slow way.', 0, 0),
('taxi_no_route_remembered', 'I missed that. Which flight master did you use?', 0, 0),
('taxi_no_route_remembered', 'Never got the flight details.', 0, 0),
('taxi_no_route_remembered', 'I do not know where that flight goes.', 0, 0),

-- in combat when the staggered take-off fired
('taxi_in_combat', 'I am in combat and cannot board the flight.', 0, 0),
('taxi_in_combat', 'Something is attacking me - cannot take off.', 0, 0),
('taxi_in_combat', 'Fighting! I will catch the next one.', 0, 0),
('taxi_in_combat', 'Cannot board while in combat, give me a moment.', 0, 0),
('taxi_in_combat', 'Pulled into a fight right as you took off.', 0, 0),
('taxi_in_combat', 'In combat, the flight master will not take me.', 0, 0),
('taxi_in_combat', 'Busy fighting - go on without me.', 0, 0),

-- ActivateTaxiPathTo refused (usually not enough gold, or an unknown path)
('taxi_could_not_board', 'I could not board that flight - check my gold.', 0, 0),
('taxi_could_not_board', 'Flight refused. I might not have enough coin.', 0, 0),
('taxi_could_not_board', 'Could not get on that flight, sorry.', 0, 0),
('taxi_could_not_board', 'The flight master turned me away.', 0, 0),
('taxi_could_not_board', 'No good - I cannot take that path.', 0, 0),
('taxi_could_not_board', 'Boarding failed. I may not know that route.', 0, 0),
('taxi_could_not_board', 'I am short on gold for that flight.', 0, 0),
('taxi_could_not_board', 'That flight would not accept me.', 0, 0),
('taxi_could_not_board', 'Failed to take off - I will make my own way.', 0, 0),
('taxi_could_not_board', 'Cannot board. Meet you at the other end eventually.', 0, 0);
