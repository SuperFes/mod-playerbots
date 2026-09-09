-- Party-chat callouts for major cooldowns.
-- Target table: ai_playerbot_texts
--
-- Spell ids behind these were verified against Spell.dbc rather than taken from memory.
-- Placeholders: %spell_name (the actual ability), %my_class.
--
-- Battle rez fires the moment someone dies; the rest only while the group is idle, so these stay
-- information rather than chatter.

INSERT INTO `ai_playerbot_texts` (`name`, `text`, `say_type`, `reply_type`) VALUES
-- battle rez: Rebirth, Raise Ally, Soulstone Resurrection
('callout_battle_rez', 'I have %spell_name ready - say the word.', 0, 0),
('callout_battle_rez', 'Battle rez available. Who do you want up?', 0, 0),
('callout_battle_rez', '%spell_name is up if we need it.', 0, 0),
('callout_battle_rez', 'I can rez someone right now.', 0, 0),
('callout_battle_rez', 'Do not release! I have %spell_name.', 0, 0),
('callout_battle_rez', 'Hold on, %spell_name is off cooldown.', 0, 0),
('callout_battle_rez', 'Stay down, I can bring you back.', 0, 0),
('callout_battle_rez', 'Battle rez ready. Just tell me who.', 0, 0),
('callout_battle_rez', 'I still have %spell_name if that helps.', 0, 0),
('callout_battle_rez', 'Nobody run back yet - %spell_name is up.', 0, 0),

-- haste: Bloodlust, Heroism
('callout_haste', '%spell_name is ready whenever you want it.', 0, 0),
('callout_haste', 'Say when for %spell_name.', 0, 0),
('callout_haste', 'I have %spell_name up - just call it.', 0, 0),
('callout_haste', '%spell_name available. Best saved for the burn phase?', 0, 0),
('callout_haste', 'Heroism/Bloodlust off cooldown here.', 0, 0),
('callout_haste', 'Ready to pop %spell_name on your call.', 0, 0),
('callout_haste', 'Do not let me forget I have %spell_name.', 0, 0),
('callout_haste', '%spell_name is up. Do we save it or use it?', 0, 0),

-- mana: Innervate, Hymn of Hope
('callout_mana', '%spell_name is ready if a healer needs it.', 0, 0),
('callout_mana', 'I have %spell_name available - who is low?', 0, 0),
('callout_mana', 'Shout if you want %spell_name.', 0, 0),
('callout_mana', '%spell_name off cooldown. Healers, speak up.', 0, 0),
('callout_mana', 'Mana help available whenever.', 0, 0),
('callout_mana', 'Got %spell_name ready to go.', 0, 0),

-- big saves: Lay on Hands, Divine Intervention, Guardian Spirit, Pain Suppression, Tranquility, Shield Wall
('callout_save', '%spell_name is up if things go badly.', 0, 0),
('callout_save', 'I have %spell_name in my back pocket.', 0, 0),
('callout_save', 'Emergency button ready: %spell_name.', 0, 0),
('callout_save', 'If someone is about to die, I have %spell_name.', 0, 0),
('callout_save', '%spell_name available - do not be shy about asking.', 0, 0),
('callout_save', 'Saving %spell_name for when it actually matters.', 0, 0),
('callout_save', 'One %spell_name, ready when needed.', 0, 0),
('callout_save', 'I can cover one big mistake with %spell_name.', 0, 0),

-- utility: Ritual of Refreshment, Misdirection, Tricks of the Trade
('callout_utility', '%spell_name is ready if anyone wants it.', 0, 0),
('callout_utility', 'I can do %spell_name whenever.', 0, 0),
('callout_utility', 'Just say the word for %spell_name.', 0, 0),
('callout_utility', '%spell_name off cooldown over here.', 0, 0),
('callout_utility', 'Happy to use %spell_name on whoever is pulling.', 0, 0),
('callout_utility', 'Reminder that I have %spell_name.', 0, 0);
