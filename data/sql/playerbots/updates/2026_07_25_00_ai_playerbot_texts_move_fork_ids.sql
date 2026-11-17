-- Moves the fork's own ai_playerbot_texts rows out of the id range upstream assigns by hand.
-- Target table: ai_playerbot_texts
--
-- Upstream inserts with explicit ids (1901+ from 2026_07_26_00 on), while the fork's text updates
-- let AUTO_INCREMENT pick, so both claim the ids right after 1900. Every row above 1900 at this point
-- came from the fork, so shift them to start at 10001 and leave 1901-10000 to upstream.
-- Text lookups are by name, nothing references these ids.
-- Skipped once anything sits above 10000, so a re-run never moves upstream's later 1901+ rows.
SET @fork_texts_moved := (SELECT COUNT(*) FROM `ai_playerbot_texts` WHERE `id` > 10000);
UPDATE `ai_playerbot_texts` SET `id` = `id` + 8100 WHERE `id` > 1900 AND `id` <= 10000 AND @fork_texts_moved = 0;

-- InnoDB does not always raise the counter when ids are updated upward; values at or below the
-- current maximum are clamped to MAX(id) + 1, so new fork rows keep landing above 10000.
ALTER TABLE `ai_playerbot_texts` AUTO_INCREMENT = 1;
