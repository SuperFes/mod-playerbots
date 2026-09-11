-- Bots acknowledging an over-payment rather than quietly pocketing the difference.
-- Target table: ai_playerbot_texts
--
-- The refusal case already names a price via trade_want_money_for_this; this is the accepting side.

INSERT INTO `ai_playerbot_texts` (`name`, `text`, `say_type`, `reply_type`) VALUES
('trade_value_gain', 'That is %money more than I asked for - thank you', 0, 0),
('trade_value_gain', 'You have overpaid by %money, but I will not argue.', 0, 0),
('trade_value_gain', 'Generous - %money more than this is worth.', 0, 0),
('trade_value_gain', '%money over the odds. Pleasure doing business.', 0, 0),
('trade_value_gain', 'I owe you %money on that one.', 0, 0);
