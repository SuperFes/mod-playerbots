-- Removes duplicate (name, text) rows from the bot text pool.
-- Target table: ai_playerbot_texts
--
-- Roughly a quarter of the table was duplicated, so the real variety was well below the row counts:
-- broadcast_looting_item_artifact had 7 rows but 4 distinct lines, legendary 17/9, epic 27/14,
-- dungeon_wipe 40/20. Since PlayerbotTextMgr picks a row at random, duplicates are not harmless -
-- they weight those lines higher and make bots sound more repetitive than the counts suggest.
--
-- Numbered last in this batch so it runs after every expansion file and cleans the table in one pass.
--
-- Locale safety: 34 duplicate rows carry translations. Keeping the lowest id blindly would drop
-- them, so for each duplicate group this keeps the lowest id that HAS locale text, and only falls
-- back to the plain lowest id when no row in the group is translated.

DROP TABLE IF EXISTS `ai_playerbot_texts_dedup_keep`;

CREATE TABLE `ai_playerbot_texts_dedup_keep` (
  `id` SMALLINT(20) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=UTF8;

INSERT INTO `ai_playerbot_texts_dedup_keep` (`id`)
SELECT COALESCE(
         MIN(CASE WHEN (`text_loc1` <> '' OR `text_loc2` <> '' OR `text_loc3` <> '' OR `text_loc4` <> ''
                     OR `text_loc5` <> '' OR `text_loc6` <> '' OR `text_loc7` <> '' OR `text_loc8` <> '')
                  THEN `id` END),
         MIN(`id`))
FROM `ai_playerbot_texts`
GROUP BY `name`, `text`;

DELETE FROM `ai_playerbot_texts`
WHERE `id` NOT IN (SELECT `id` FROM `ai_playerbot_texts_dedup_keep`);

DROP TABLE `ai_playerbot_texts_dedup_keep`;
