-- Bots explaining why the dungeon finder would not let them in.
-- Target table: ai_playerbot_texts
--
-- %reason is filled from the SMSG_LFG_TELEPORT_DENIED code.

INSERT INTO `ai_playerbot_texts` (`name`, `text`, `say_type`, `reply_type`) VALUES
('lfg_teleport_denied', 'Cannot get in - %reason.', 0, 0),
('lfg_teleport_denied', 'It will not let me through, %reason.', 0, 0),
('lfg_teleport_denied', 'Stuck out here - %reason.', 0, 0),
('lfg_teleport_denied', 'Give me a moment, %reason.', 0, 0),
('lfg_teleport_denied', 'No luck getting in: %reason.', 0, 0),
('lfg_teleport_denied', 'Still outside. %reason.', 0, 0),
('lfg_teleport_denied', 'The portal will not take me, %reason.', 0, 0),
('lfg_teleport_denied', 'Trying to join you - %reason.', 0, 0);
