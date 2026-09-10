-- Fallback for %target_name when the master has nothing selected.
-- Target table: ai_playerbot_texts
--
-- Mirrors the existing string_unknown_area fallback.

INSERT INTO `ai_playerbot_texts` (`name`, `text`, `say_type`, `reply_type`) VALUES
('string_unknown_target', 'our target', 0, 0),
('string_unknown_target', 'that thing', 0, 0),
('string_unknown_target', 'whatever you are pointing at', 0, 0);
