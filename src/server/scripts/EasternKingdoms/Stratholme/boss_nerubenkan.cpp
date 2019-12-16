/*
 * Copyright (C) 2008-2019 TrinityCore <https://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "stratholme.h"

enum Spells
{
    SPELL_ENCASINGWEBS          = 4962,
    SPELL_PIERCEARMOR           = 6016,
    SPELL_CRYPT_SCARABS         = 31602,
    SPELL_RAISEUNDEADSCARAB     = 17235
};

enum Nerubenkan_Events
{
    EVENT_CRYPT_SCARABS         = 0,
    EVENT_ENCASING_WEBS         = 1,
    EVENT_PIERCE_ARMOR          = 2,
    EVENT_RAISE_UNDEAD_SCARAB   = 3
};

enum Nerubenkan_Summs
{
    NPC_UNDEAD_SCARAB           = 10876
};

struct boss_nerubenkan : public BossAI
{
public:
    boss_nerubenkan(Creature* creature) : BossAI(creature, TYPE_PALLID) { }

    void Reset()
    {
        BossAI::Reset();
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_CRYPT_SCARABS, 3000);
        events.ScheduleEvent(EVENT_ENCASING_WEBS, 7000);
        events.ScheduleEvent(EVENT_PIERCE_ARMOR, 19000);
        events.ScheduleEvent(EVENT_RAISE_UNDEAD_SCARAB, 3000);

        BossAI::JustEngagedWith(who);
    }

    void RaiseUndeadScarab(Unit* victim)
    {
        if (Creature* pUndeadScarab = DoSpawnCreature(NPC_UNDEAD_SCARAB, float(irand(-9, 9)), float(irand(-9, 9)), 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 180000))
            if (pUndeadScarab->AI())
                pUndeadScarab->AI()->AttackStart(victim);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CRYPT_SCARABS:
                    DoCastVictim(SPELL_ENCASINGWEBS);
                    events.Repeat(30s);
                    break;
                case EVENT_ENCASING_WEBS:
                    if (urand(0, 3) < 2)
                        DoCastVictim(SPELL_PIERCEARMOR);
                    events.Repeat(35s);
                    break;
                case EVENT_PIERCE_ARMOR:
                    DoCastVictim(SPELL_CRYPT_SCARABS);
                    events.Repeat(20s);
                    break;
                case EVENT_RAISE_UNDEAD_SCARAB:
                    RaiseUndeadScarab(me->GetVictim()); 
                    events.Repeat(16s);
                    break;
                default:
                    break;
            }
        }
        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_nerubenkan()
{
    RegisterStratholmeCreatureAI(boss_nerubenkan);
}
