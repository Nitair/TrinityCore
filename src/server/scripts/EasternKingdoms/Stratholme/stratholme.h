/*
 * Copyright (C) 2008-2019 TrinityCore <https://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DEF_STRATHOLME_H
#define DEF_STRATHOLME_H

#include "CreatureAIImpl.h"

#define StratholmeScriptName "instance_stratholme"
#define DataHeader "STR"

enum StratholmeTypes
{
    TYPE_BARON_RUN          = 0,
    TYPE_BARONESS           = 1,
    TYPE_NERUB              = 2,
    TYPE_PALLID             = 3,
    TYPE_RAMSTEIN           = 4,
    TYPE_BARON              = 5,
    TYPE_ALL_CRYSTALS_DIED  = 6,
    TYPE_EVENT_AURIUS       = 7,
    TYPE_RAMSTEIN_EVENT     = 8,
    TYPE_POSTMASTER         = 9,
    MAX_ENCOUNTER           = 10,
    TYPE_CRYSTAL_DIED       = 11
};

enum StratholmeData
{
    DATA_BARON              = 10,
    DATA_YSIDA_TRIGGER      = 11,
    DATA_AURIUS             = 12,
    DATA_QUESTPLAYER        = 13
};

enum StratholmeOtherTypes
{
    TYPE_SH_QUEST           = 20,
    TYPE_SH_CATHELA         = 21,
    TYPE_SH_GREGOR          = 22,
    TYPE_SH_NEMAS           = 23,
    TYPE_SH_VICAR           = 24,
    TYPE_SH_AELMAR          = 25,
};

enum StratholmeCreatures
{
    // Undead side
    NPC_BARON                   = 10440,
    NPC_YSIDA_TRIGGER           = 16100,
    NPC_RAMSTEIN                = 10439,
    NPC_ABOM_BILE               = 10416,
    NPC_ABOM_VENOM              = 10417,
    NPC_BLACK_GUARD             = 10394,
    NPC_YSIDA                   = 16031,
    NPC_BARTHILAS               = 10435,
    NPC_EYE_OF_NAXXRAMAS        = 10411,
    NPC_MINDLESS_UNDEAD         = 11030,
    NPC_VENGEFUL_PHANTOM        = 10387,    // Adds for The Unforgiven
    NPC_THE_UNFORGIVEN          = 10516,
    NPC_THUZADIN_ACOLYTE        = 10399,    // Acolytes in ziggurats
    NPC_CRYSTAL                 = 10415,    // Three ziggurat crystals
    NPC_DATHROHAN               = 10812,
    NPC_AURIUS_1                = 10917,
    NPC_AURIUS_2                = 10931,
    NPC_RAT                     = 10441,
    NPC_ASTICOT                 = 10536,
    NPC_INSECTE                 = 10461,

    // Living side
    NPC_CRIMSON_GUARDSMAN       = 10418,
    NPC_CRIMSON_CONJUROR        = 10419,
    NPC_CRIMSON_INITATE         = 10420,
    NPC_CRIMSON_GALLANT         = 10424,
    NPC_HEARTHSINGER_FORRESTEN  = 10558,
    NPC_UNDEAD_POSTMAN          = 11142,
    NPC_TIMMY_THE_CRUEL         = 10808
};

enum StratholmeGameObjects
{
    GO_DOOR_HALAZZI         = 186303,
    GO_SERVICE_ENTRANCE     = 175368,
    GO_GAUNTLET_GATE1       = 175357,
    GO_ZIGGURAT1            = 175380,  // baroness
    GO_ZIGGURAT2            = 175379,  // nerub'enkan
    GO_ZIGGURAT3            = 175381,  // maleki
    GO_ZIGGURAT4            = 175405,  // rammstein
    GO_ZIGGURAT5            = 175796,  // baron
    GO_PORT_GAUNTLET        = 175374,  // port from gauntlet to slaugther
    GO_PORT_SLAUGTHER       = 175373,  // port at slaugther
    GO_PORT_ELDERS          = 175377,  // port at elders square
    GO_GATE_TRAP_RAT_1      = 175355,
    GO_GATE_TRAP_RAT_2      = 175354,
    GO_GATE_TRAP_RAT_3      = 175351,
    GO_GATE_TRAP_RAT_4      = 175350,
    GO_YSIDA_CAGE           = 181071
};

enum StratholmeQuests
{
    QUEST_DEAD_MAN_PLEA     = 8945
};

enum StratholmeSpells
{
    SPELL_PERM_FEIGN_DEATH      = 29266,
    SPELL_YSIDA_SAVED           = 31912,
    SPELL_YSIDA_CREDIT_EFFECT   = 31913,
    SPELL_BARON_ULTIMATUM_45MIN = 27861,
    SPELL_BARON_ULTIMATUM_10MIN = 27863,
    SPELL_BARON_ULTIMATUM_5MIN  = 27864,
    SPELL_BARON_ULTIMATUM_1MIN  = 27865,
    SPELL_SUMMON_POSTMASTER     = 24627
};

enum StratholmeMisc
{
    //! amount of crusade monsters required to be killed in order for timmy the cruel to spawn
    TIMMY_THE_CRUEL_CRUSADERS_REQUIRED = 15
};

enum StratholmeZiguratTexts
{
    YELL_ONE_CRYSTAL_DESTROYED_1    = 0, // One of the Ash\'ari Crystals has been destroyed! Slay the intruders!
    YELL_ONE_CRYSTAL_DESTROYED_2    = 1, // An Ash\'ari Crystal has been toppled! Restore the ziggurat before the Slaughterhouse is vulnerable!
    YELL_ONE_CRYSTAL_RESTORED_1     = 2, // An Ash\'ari Crystal has been restored!
    YELL_ONE_CRYSTAL_RESTORED_2     = 3, // Our summoning is complete! The Ash\'ari crystal has been restored!
    YELL_ONE_CRYSTAL_RESTORED_3     = 4, // Success is ours! The Ash\'ari Crystal has been restored!
    YELL_ONE_RESTORE_CRYSTAL        = 5, // An Ash\'ari Crystal has fallen! Stay true to the Lich King, my brethren, and attempt to resummon it.
    YELL_ALL_CRYSTAL_DESTROYED      = 6  // The Ash\'ari Crystals have been destroyed! The Slaughterhouse is vulnerable!
};

enum StratholmeBaronRunTexts
{
    YELL_BARON_RUN_45MIN        = 0, // Intruders! More pawns of the Argent Dawn, no doubt. I already count one of their number among my prisoners. Withdraw from my domain before she is executed!
    YELL_BARON_RUN_10MIN        = 1, // You\'re still here? Your foolishness is amusing! The Argent Dawn wench needn\'t suffer in vain. Leave at once and she shall be spared!
    YELL_BARON_RUN_5MIN         = 2, // I shall take great pleasure in taking this poor wretch\'s life! It\'s not too late, she needn\'t suffer in vain. Turn back and her death shall be merciful.
    YELL_BARON_RUN_FAILED_R     = 3, // May this prisoner\'s death serve as a warning. None shall defy the scourge and live!
    YELL_RAMSTEIN_THE_GORGER    = 4, // So you see fit to toy with the Lichking\'s creations? Ramstein, be sure to give the intruders a proper greeding.
    YELL_BLACK_GUARD_DEATH      = 5  // Time to take matters into my own hands. Come. Enter my domain and challenge the might of the Scourge!
};

enum StratholmeYsidaTexts
{
    YELL_BARON_RUN_FAILED_Y     = 0, // My death means nothing.... light... will... prevail!
    YELL_BARON_RUN_5MIN_Y       = 1, // Don\'t worry about me!  Slay this dreadful beast and cleanse this world of his foul taint!
    SAY_BARON_RUN_SUCCESS       = 2  // You did it... you\'ve slain Baron Rivendare!  The Argent Dawn shall hear of your valiant deeds!
};

enum StratholmeRamstein
{
    YELL_RAMSTEIN_INFIGHT_1     = 0,  // Ramstein hunger for flesh!
    YELL_RAMSTEIN_INFIGHT_2     = 1,  // $R flesh... must feed!
};

enum StratholmeTexts
{
    YELL_MAGISTRATE_BARTHILAS   = 0,  // Intruders at the Service Gate! Lord Rivendare must be warned!
    
    YELL_EYE_OF_NAXXRAMAS       = 2,  // The living are here!                        

    
    YELL_BLACK_GUARD_SPAWN      = 6,  // ???

    // Postmaster Malown
    YELL_POSTMASTER_START       = 8, // Here comes the postmaster!
    YELL_POSTMASTER_INFIGHT     = 9, // Prepare to be Malowned!
    YELL_POSTMASTER_KILL        = 10 // You've been MALOWNED!

    // Trash before Ramstein
    // Broadcast ID 6257: %s explodes and releases several Bile Slimes!
    // Broadcast ID 6258: %s belches out a disgusting Bile Slime!
};

template <class AI, class T>
inline AI* GetStratholmeAI(T* obj)
{
    return GetInstanceAI<AI>(obj, StratholmeScriptName);
}

#define RegisterStratholmeCreatureAI(ai_name) RegisterCreatureAIWithFactory(ai_name, GetStratholmeAI)

#endif
