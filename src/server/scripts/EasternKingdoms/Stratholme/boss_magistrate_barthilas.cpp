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

#include "Map.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "stratholme.h"

enum BarthilasEvents
{
    EVENT_DRAINING_BLOW = 1,
    EVENT_CROWD_PUMMEL  = 2,
    EVENT_MIGHTY_BLOW   = 3,
    EVENT_FURIOUS_ANGER = 4
};

enum BarthilasModels
{
    MODEL_NORMAL        = 10433,
    MODEL_HUMAN         = 3637
};

enum BarthilasSpells
{
    SPELL_DRAININGBLOW  = 16793,
    SPELL_CROWDPUMMEL   = 10887,
    SPELL_MIGHTYBLOW    = 14099,
    SPELL_FURIOUS_ANGER = 16791
};

struct boss_magistrate_barthilas : public BossAI
{
    boss_magistrate_barthilas(Creature* creature) : BossAI(creature, NPC_BARTHILAS) { }

    uint8 AngerCount;

    void Reset() override
    {
        AngerCount = 0;
        
        if (me->IsAlive())
            me->SetDisplayId(MODEL_NORMAL);
        else
            me->SetDisplayId(MODEL_HUMAN);

        BossAI::Reset();
    }

    void ReceiveEmote(Player* player, uint32 emote) override
    {
        if (emote == 1000)
            Talk(YELL_MAGISTRATE_BARTHILAS);

        BossAI::ReceiveEmote(player, emote);
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_DRAINING_BLOW, 20s);
        events.ScheduleEvent(EVENT_CROWD_PUMMEL, 15s);
        events.ScheduleEvent(EVENT_MIGHTY_BLOW, 10s);
        events.ScheduleEvent(EVENT_FURIOUS_ANGER, 5s);

        BossAI::JustEngagedWith(who);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER && me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) && me->IsWithinDistInMap(who, 10.0f))
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        BossAI::MoveInLineOfSight(who);
    }

    void JustDied(Unit* killer) override
    {
        me->SetDisplayId(MODEL_HUMAN);

        BossAI::JustDied(killer);
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
                case EVENT_DRAINING_BLOW:
                    DoCastVictim(SPELL_DRAININGBLOW);
                    events.Repeat(15s);
                    break;
                case EVENT_CROWD_PUMMEL:
                    DoCastVictim(SPELL_CROWDPUMMEL);
                    events.Repeat(15s);
                    break;
                case EVENT_MIGHTY_BLOW:
                    DoCastVictim(SPELL_MIGHTYBLOW);
                    events.Repeat(20s);
                    break;
                case EVENT_FURIOUS_ANGER:
                    if (AngerCount > 25)
                        break;
                    ++AngerCount;
                    DoCastSelf(SPELL_FURIOUS_ANGER);
                    events.Repeat(4s);
                    break;
                default:
                    break;              
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_magistrate_barthilas()
{
    RegisterStratholmeCreatureAI(boss_magistrate_barthilas);
}
