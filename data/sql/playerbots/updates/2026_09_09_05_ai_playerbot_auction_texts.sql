-- Bots mentioning they have listed things at the auction house.
-- Target table: ai_playerbot_texts
-- Placeholder: %count (how many items were listed this visit).

INSERT INTO `ai_playerbot_texts` (`name`, `text`, `say_type`, `reply_type`) VALUES
('auction_posted', 'Put %count things up on the auction house.', 0, 0),
('auction_posted', 'Listed %count items. Someone will want them.', 0, 0),
('auction_posted', 'Cleared out some bag space - %count on the auction house.', 0, 0),
('auction_posted', 'That is %count items listed. Fingers crossed.', 0, 0),
('auction_posted', 'Auction house has %count of my things now.', 0, 0),
('auction_posted', 'Posted %count items. Undercut me and we will have words.', 0, 0),
('auction_posted', 'Might make a little gold off these %count.', 0, 0),
('auction_posted', 'Listed a few bits I was never going to use.', 0, 0),
('auction_posted', 'Bags feel lighter already.', 0, 0),
('auction_posted', 'If nobody buys these I am vendoring the lot.', 0, 0),
('auction_posted', 'Priced them to move.', 0, 0),
('auction_posted', 'Someone out there needs %count slightly used items.', 0, 0);
