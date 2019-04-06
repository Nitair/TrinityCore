# Stratholme Instance Script - Add missing texts
DELETE FROM `creature_text` WHERE `CreatureID` IN (10440, 16031, 10399, 10435, 10411, 10439);
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
# Baron Rivendare - Baron Run
(10440, 0, 0, 'Intruders! More pawns of the Argent Dawn, no doubt. I already count one of their number among my prisoners. Withdraw from my domain before she is executed!', 14, 0, 0, 0, 0, 0, 11812, 3, 'Baron Rivendare (Stratholme) - YELL_BARON_RUN_45MIN'),
(10440, 1, 0, 'You\'re still here? Your foolishness is amusing! The Argent Dawn wench needn\'t suffer in vain. Leave at once and she shall be spared!', 14, 0, 0, 0, 0, 0, 11813, 3, 'Baron Rivendare (Stratholme) - YELL_BARON_RUN_10MIN'),
(10440, 2, 0, 'I shall take great pleasure in taking this poor wretch\'s life! It\'s not too late, she needn\'t suffer in vain. Turn back and her death shall be merciful.', 14, 0, 0, 0, 0, 0, 11815, 3, 'Baron Rivendare (Stratholme) - YELL_BARON_RUN_5MIN'),
(10440, 3, 0, 'May this prisoner\'s death serve as a warning. None shall defy the scourge and live!', 14, 0, 0, 0, 0, 0, 11814, 3, 'Baron Rivendare (Stratholme) - YELL_BARON_RUN_FAILED_R'),
# Baron Rivendare - Misc
(10440, 4, 0, 'So you see fit to toy with the Lichking\'s creations? Ramstein, be sure to give the intruders a proper greeding.', 14, 0, 0, 0, 0, 0, 11815, 3, 'Baron Rivendare (Stratholme) - YELL_BARON_RUN_EVENT_FAILED'),
(10440, 5, 0, 'Time to take matters into my own hands. Come. Enter my domain and challenge the might of the Scourge!', 14, 0, 0, 0, 0, 0, 11814, 3, 'Baron Rivendare (Stratholme) - YELL_BARON_RUN_EVENT_FAILED'),
# Ysida Harmon
(16031, 0, 0, 'My death means nothing.... light... will... prevail!', 14, 0, 0, 0, 0, 0, 11817, 3, 'Ysida Harmon (Stratholme) - YELL_BARON_RUN_FAILED_Y'),
(16031, 1, 0, 'Don\'t worry about me!  Slay this dreadful beast and cleanse this world of his foul taint!', 14, 0, 0, 0, 0, 0, 11816, 3, 'Ysida Harmon (Stratholme) - YELL_BARON_RUN_5MIN_Y'),
(16031, 2, 0, 'You did it... you\'ve slain Baron Rivendare!  The Argent Dawn shall hear of your valiant deeds!', 14, 0, 0, 0, 0, 0, 11931, 3, 'Ysida Harmone (Stratholme) - SAY_BARON_RUN_SUCCESS'),
# Thuzadin Acolyte
(10399, 0, 0, 'One of the Ash\'ari Crystals has been destroyed! Slay the intruders!', 14, 0, 0, 0, 0, 0, 6492, 3, 'Thuzadin Acolyte (Stratholme) - YELL_ONE_CRYSTAL_DESTROYED_1'),
(10399, 1, 0, 'An Ash\'ari Crystal has been toppled! Restore the ziggurat before the Slaughterhouse is vulnerable!', 14, 0, 0, 0, 0, 0, 6527, 3, 'Thuzadin Acolyte (Stratholme) - YELL_ONE_CRYSTAL_DESTROYED_2'),
(10399, 2, 0, 'An Ash\'ari Crystal has been restored!', 14, 0, 0, 0, 0, 0, 6493, 3, 'Thuzadin Acolyte (Stratholme) - YELL_ONE_CRYSTAL_RESTORED_1'),
(10399, 3, 0, 'Our summoning is complete! The Ash\'ari crystal has been restored!', 14, 0, 0, 0, 0, 0, 6528, 3, 'Thuzadin Acolyte (Stratholme) - YELL_ONE_CRYSTAL_RESTORED_2'),
(10399, 4, 0, 'Success is ours! The Ash\'ari Crystal has been restored!', 14, 0, 0, 0, 0, 0, 6529, 3, 'Thuzadin Acolyte (Stratholme) - YELL_ONE_CRYSTAL_RESTORED_3'),
(10399, 5, 0, 'An Ash\'ari Crystal has fallen! Stay true to the Lich King, my brethren, and attempt to resummon it.', 14, 0, 0, 0, 0, 0, 6526, 3, 'Thuzadin Acolyte (Stratholme) - YELL_ONE_RESTORE_CRYSTAL'),
(10399, 6, 0, 'The Ash\'ari Crystals have been destroyed! The Slaughterhouse is vulnerable!', 14, 0, 0, 0, 0, 0, 6289, 3, 'Thuzadin Acolyte (Stratholme) - YELL_ALL_CRYSTAL_DESTROYED'),
# Magistrate Barthilas
(10435, 0, 0, 'Intruders at the Service Gate! Lord Rivendare must be warned!', 14, 0, 0, 0, 0, 0, 6162, 3, 'Magistrate Barthilas (Stratholme) - YELL_MAGISTRATE_BARTHILAS'),
# Ramstein the Gorger
(10439, 0, 0, 'Ramstein hunger for flesh!', 14, 0, 0, 0, 0, 0, 6425, 3, 'Ramstein the Gorger (Stratholme) - YELL_RAMSTEIN_INFIGHT_1'),
(10439, 1, 0, '$R flesh... must feed!', 14, 0, 0, 0, 0, 0, 6494, 3, 'Ramstein the Gorger (Stratholme) - YELL_RAMSTEIN_INFIGHT_2'),
# Eye of Naxxramas
(10411, 0, 0, 'The living are here!', 14, 0, 0, 0, 0, 0, 6512, 3, 'Eye of Naxxramas (Stratholme) - YELL_EYE_OF_NAXXRAMAS');

# Stratholme Instance Script - GameObject Changes
UPDATE `gameobject_template` SET `ScriptName` = 'go_service_gate' WHERE `entry` = 175368;

# Stratholme Instance Script - Creature Changes
UPDATE `creature_template` SET `ScriptName` = 'npc_rat_plague_victim' WHERE `entry` = 10441;
UPDATE `creature` SET `position_x` = 4044.34, `position_y` = -3334.23, `position_z` = 115.06, `orientation` = 4.1421 WHERE `guid` = 16031;
DELETE FROM `creature` WHERE `id` = 10558;
UPDATE `creature_template` SET `MovementType` = 0, `ScriptName` = 'npc_crystal_zigurat' WHERE `entry` = 10415;

