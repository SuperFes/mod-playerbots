-- Text for bot ambitions - the long-term goal a bot works towards.
-- Target table: ai_playerbot_texts
--
-- Only bots whose personality gave them an ambition ever say any of this: at PersonalityStrength 1
-- roughly a third want nothing in particular, so these lines are not universal chatter.
--
-- Placeholders, all supplied by MentionAmbitionAction:
--   ambition_item           %item_link
--   ambition_item_located   %item_link %location %source_name
--   ambition_set            %set_name %total
--   ambition_set_progress   %set_name %owned %total

INSERT INTO `ai_playerbot_texts` (`name`, `text`, `say_type`, `reply_type`) VALUES
-- a specific item, source unknown
('ambition_item', 'One day I will get my hands on %item_link.', 0, 0),
('ambition_item', 'Still chasing %item_link. Still nothing.', 0, 0),
('ambition_item', 'All I really want is %item_link.', 0, 0),
('ambition_item', 'I have been after %item_link for longer than I care to admit.', 0, 0),
('ambition_item', 'If %item_link ever drops for me I am never taking it off.', 0, 0),
('ambition_item', 'My whole build comes together the day I get %item_link.', 0, 0),
('ambition_item', 'Everyone has a white whale. Mine is %item_link.', 0, 0),
('ambition_item', 'Do not ask how many times I have tried for %item_link.', 0, 0),
('ambition_item', 'Saving a bag slot for %item_link. Optimistic, I know.', 0, 0),
('ambition_item', 'I dream about %item_link.', 0, 0),
('ambition_item', 'Someone tell me %item_link is real and not a myth.', 0, 0),
('ambition_item', 'Working towards %item_link, slowly.', 0, 0),

-- a specific item we can name the source of
('ambition_item_located', 'I need %item_link. It drops in %location - been meaning to go back.', 0, 0),
('ambition_item_located', 'Anyone up for %location? %item_link has my name on it.', 0, 0),
('ambition_item_located', '%source_name in %location has %item_link and refuses to hand it over.', 0, 0),
('ambition_item_located', 'One more run at %location and maybe %item_link finally drops.', 0, 0),
('ambition_item_located', 'I keep going back to %location for %item_link. One day.', 0, 0),
('ambition_item_located', '%item_link is in %location. That is the whole plan.', 0, 0),
('ambition_item_located', 'If anyone is heading to %location, I am after %item_link.', 0, 0),
('ambition_item_located', '%source_name owes me %item_link at this point.', 0, 0),
('ambition_item_located', 'Been farming %location for %item_link. Send help.', 0, 0),
('ambition_item_located', 'My goal is simple: %location, then %item_link, then retire.', 0, 0),
('ambition_item_located', 'I have run %location so often I could walk it blindfolded. Still no %item_link.', 0, 0),
('ambition_item_located', 'Someone organise a %location run, I need %item_link.', 0, 0),

-- a whole set, nothing collected yet
('ambition_set', 'I am going to collect the whole %set_name set. All %total pieces.', 0, 0),
('ambition_set', 'Long term plan: the full %set_name set.', 0, 0),
('ambition_set', 'Someday I will be wearing every piece of %set_name.', 0, 0),
('ambition_set', 'All %total pieces of %set_name. That is the dream.', 0, 0),
('ambition_set', 'I have decided. %set_name. The complete set.', 0, 0),
('ambition_set', 'Not interested in odd pieces - I want all of %set_name.', 0, 0),
('ambition_set', 'The %set_name set is the goal. It may take a while.', 0, 0),
('ambition_set', 'Starting the hunt for %set_name properly.', 0, 0),

-- a whole set, partway there
('ambition_set_progress', '%owned of %total on the %set_name set. Getting there.', 0, 0),
('ambition_set_progress', 'Only %owned pieces of %set_name so far. Long way to go.', 0, 0),
('ambition_set_progress', '%owned out of %total for %set_name. Slow but steady.', 0, 0),
('ambition_set_progress', 'The %set_name set is %owned/%total complete. I can taste it.', 0, 0),
('ambition_set_progress', 'Just %owned pieces of %set_name. The rest are hiding from me.', 0, 0),
('ambition_set_progress', '%set_name: %owned down, %total to go... well, you know what I mean.', 0, 0),
('ambition_set_progress', 'Halfway to nothing. %owned of %total on %set_name.', 0, 0),
('ambition_set_progress', 'Wearing %owned pieces of %set_name and counting.', 0, 0),
('ambition_set_progress', 'Every drop brings %set_name closer. %owned of %total.', 0, 0),
('ambition_set_progress', 'I refuse to stop until %set_name is all %total pieces.', 0, 0);
