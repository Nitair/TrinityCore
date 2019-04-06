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

#include "ScriptMgr.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "Group.h"
#include "InstanceScript.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "stratholme.h"
#include "TemporarySummon.h"

/*######
## go_gauntlet_gate (this is the _first_ of the gauntlet gates, two exist)
######*/

class go_gauntlet_gate : public GameObjectScript
{
    public:
        go_gauntlet_gate() : GameObjectScript("go_gauntlet_gate") { }

        struct go_gauntlet_gateAI : public GameObjectAI
        {
            go_gauntlet_gateAI(GameObject* go) : GameObjectAI(go), instance(go->GetInstanceScript()) { }

            InstanceScript* instance;

            bool GossipHello(Player* player) override
            {
                if (instance->GetData(TYPE_BARON_RUN) != NOT_STARTED)
                    return false;

                instance->SetData(TYPE_BARON_RUN, IN_PROGRESS);
                return false;
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return GetStratholmeAI<go_gauntlet_gateAI>(go);
        }
};

/*######
## go_service_gate
######*/

class go_service_gate : public GameObjectScript
{
public:
    go_service_gate() : GameObjectScript("go_service_gate") { }

    struct go_service_gateAI : public GameObjectAI
    {
        go_service_gateAI(GameObject* go) : GameObjectAI(go), instance(go->GetInstanceScript()), FirstAnnounce(true) { }

        InstanceScript* instance;

        bool GossipHello(Player* player) override
        {
            if ((instance->GetData(TYPE_BARON_RUN) != NOT_STARTED) || !FirstAnnounce)
                return false;

            std::list<Creature*> listBarthilas;
            GetCreatureListWithEntryInGrid(listBarthilas, me, NPC_BARTHILAS, 1000);
            for (std::list<Creature*>::const_iterator itr = listBarthilas.begin(); itr != listBarthilas.end(); ++itr)
            {
                if (!(*itr)->IsAlive())
                    continue;

                (*itr)->AI()->ReceiveEmote(player, 1000);
                (*itr)->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                FirstAnnounce = false;
            }
            return true;
        }
    private:
        bool FirstAnnounce;
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return GetStratholmeAI<go_service_gateAI>(go);
    }
};

/*######
## go_postbox
######*/

class go_postbox : public GameObjectScript
{
public:
    go_postbox() : GameObjectScript("go_postbox") { }

    struct go_postboxAI : public GameObjectAI
    {
        go_postboxAI(GameObject* go) : GameObjectAI(go), instance(go->GetInstanceScript()) { }

        InstanceScript* instance;

        bool GossipHello(Player* player) override
        {
            if (instance->GetData(TYPE_POSTMASTER) == DONE)
                return false;

            // When the data is Special, spawn the postmaster
            if (instance->GetData(TYPE_POSTMASTER) == SPECIAL)
            {
                player->CastSpell(player, SPELL_SUMMON_POSTMASTER, true);
                instance->SetData(TYPE_POSTMASTER, DONE);
            }
            else
                instance->SetData(TYPE_POSTMASTER, IN_PROGRESS);

            // Summon 3 postmen for each postbox
            float fX, fY, fZ;
            for (uint8 i = 0; i < 3; ++i)
            {

                Position const postmen = { player->GetPositionX(), player->GetPositionY(), player->GetPositionZ() };
                player->GetRandomPoint(postmen, 6.0f, fX, fY, fZ);
                player->SummonCreature(NPC_UNDEAD_POSTMAN, fX, fY, fZ, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
            }

            return false;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return GetStratholmeAI<go_postboxAI>(go);
    }
};

/*######
## npc_restless_soul
######*/

enum RestlessSoul
{
    // Spells
    SPELL_EGAN_BLASTER      = 17368,
    SPELL_SOUL_FREED        = 17370,

    // Quest
    QUEST_RESTLESS_SOUL     = 5282,

    // Creatures
    NPC_RESTLESS            = 11122,
    NPC_FREED               = 11136
};

class npc_restless_soul : public CreatureScript
{
public:
    npc_restless_soul() : CreatureScript("npc_restless_soul") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetStratholmeAI<npc_restless_soulAI>(creature);
    }

    struct npc_restless_soulAI : public ScriptedAI
    {
        npc_restless_soulAI(Creature* creature) : ScriptedAI(creature)
        {
            Initialize();
        }

        void Initialize()
        {
            Tagger.Clear();
            Die_Timer = 5000;
            Tagged = false;
        }

        ObjectGuid Tagger;
        uint32 Die_Timer;
        bool Tagged;

        void Reset() override
        {
            Initialize();
        }

        void JustEngagedWith(Unit* /*who*/) override { }

        void SpellHit(Unit* caster, SpellInfo const* spell) override
        {
            if (Tagged || spell->Id != SPELL_EGAN_BLASTER)
                return;

            Player* player = caster->ToPlayer();
            if (!player || player->GetQuestStatus(QUEST_RESTLESS_SOUL) != QUEST_STATUS_INCOMPLETE)
                return;

            Tagged = true;
            Tagger = caster->GetGUID();
        }

        void JustSummoned(Creature* summoned) override
        {
            summoned->CastSpell(summoned, SPELL_SOUL_FREED, false);

            if (Player* player = ObjectAccessor::GetPlayer(*me, Tagger))
                summoned->GetMotionMaster()->MoveFollow(player, 0.0f, 0.0f);
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (Tagged)
                me->SummonCreature(NPC_FREED, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 300000);
        }

        void UpdateAI(uint32 diff) override
        {
            if (Tagged)
            {
                if (Die_Timer <= diff)
                {
                    if (Unit* temp = ObjectAccessor::GetUnit(*me, Tagger))
                    {
                        if (Player* player = temp->ToPlayer())
                            player->KilledMonsterCredit(NPC_RESTLESS, me->GetGUID());
                        me->KillSelf();
                    }
                }
                else
                    Die_Timer -= diff;
            }
        }
    };

};

/*######
## npc_spectral_ghostly_citizen
######*/

enum GhostlyCitizenSpells
{
    SPELL_HAUNTING_PHANTOM        = 16336,
    SPELL_DEBILITATING_TOUCH      = 16333,
    SPELL_SLAP                    = 6754
};

class npc_spectral_ghostly_citizen : public CreatureScript
{
public:
    npc_spectral_ghostly_citizen() : CreatureScript("npc_spectral_ghostly_citizen") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetStratholmeAI<npc_spectral_ghostly_citizenAI>(creature);
    }

    struct npc_spectral_ghostly_citizenAI : public ScriptedAI
    {
        npc_spectral_ghostly_citizenAI(Creature* creature) : ScriptedAI(creature)
        {
            Initialize();
        }

        void Initialize()
        {
            Die_Timer = 5000;
            HauntingTimer = 8000;
            TouchTimer = 2000;
            Tagged = false;
        }

        uint32 Die_Timer;
        uint32 HauntingTimer;
        uint32 TouchTimer;
        bool Tagged;

        void Reset() override
        {
            Initialize();
        }

        void JustEngagedWith(Unit* /*who*/) override { }

        void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
        {
            if (!Tagged && spell->Id == SPELL_EGAN_BLASTER)
                Tagged = true;
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (Tagged)
            {
                for (uint32 i = 1; i <= 4; ++i)
                {
                     //100%, 50%, 33%, 25% chance to spawn
                     if (urand(1, i) == 1)
                         DoSummon(NPC_RESTLESS, me, 20.0f, 600000);
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (Tagged)
            {
                if (Die_Timer <= diff)
                    me->KillSelf();
                else Die_Timer -= diff;
            }

            if (!UpdateVictim())
                return;

            //HauntingTimer
            if (HauntingTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_HAUNTING_PHANTOM);
                HauntingTimer = 11000;
            }
            else HauntingTimer -= diff;

            //TouchTimer
            if (TouchTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_DEBILITATING_TOUCH);
                TouchTimer = 7000;
            }
            else TouchTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void ReceiveEmote(Player* player, uint32 emote) override
        {
            switch (emote)
            {
                case TEXT_EMOTE_DANCE:
                    EnterEvadeMode();
                    break;
                case TEXT_EMOTE_RUDE:
                    if (me->IsWithinDistInMap(player, 5))
                        DoCast(player, SPELL_SLAP, false);
                    else
                        me->HandleEmoteCommand(EMOTE_ONESHOT_RUDE);
                    break;
                case TEXT_EMOTE_WAVE:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                    break;
                case TEXT_EMOTE_BOW:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    break;
                case TEXT_EMOTE_KISS:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_FLEX);
                    break;
                default:
                    break;
            }
        }
    };

};

/*######
## npc_crystal_zigurat
######*/

enum ZiguratEvents
{
    EVENT_CHECK_ACOLYTE = 1
};

struct npc_crystal_zigurat : public ScriptedAI
{
    npc_crystal_zigurat(Creature* creature) : ScriptedAI(creature), instance(me->GetInstanceScript()), events() { }

    GuidList FindAcolyte;
    InstanceScript* instance;

    void JustDied(Unit* /*killer*/) override
    {
        if (TempSummon* Announcer = me->SummonCreature(NPC_THUZADIN_ACOLYTE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 100, 0, TEMPSUMMON_TIMED_DESPAWN, 1))
        {
            Announcer->AI()->Talk(YELL_ONE_CRYSTAL_DESTROYED_2);

            instance->SetData(TYPE_CRYSTAL_DIED, IN_PROGRESS);

            if (instance->GetData(TYPE_ALL_CRYSTALS_DIED) == DONE)
                Announcer->AI()->Talk(YELL_ALL_CRYSTAL_DESTROYED);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!me->IsAlive())
            return;

        events.Update(diff);

        if (events.Empty())
            events.ScheduleEvent(EVENT_CHECK_ACOLYTE, 0);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CHECK_ACOLYTE:
                    if (FindAcolyte.empty())
                    {
                        std::list<Creature*> creatures;
                        std::list<Creature*>::iterator itr;
                        GetCreatureListWithEntryInGrid(creatures, me, NPC_THUZADIN_ACOLYTE, 50.0f);
                        for (itr = creatures.begin(); itr != creatures.end(); ++itr)
                            FindAcolyte.push_back((*itr)->GetGUID());
                        return;
                    }

                    for (GuidList::iterator itr = FindAcolyte.begin(); itr != FindAcolyte.end(); ++itr)
                        if (Creature* Acolyte = instance->instance->GetCreature((*itr)))
                            if (Acolyte && Acolyte->IsAlive())
                                return;

                    me->KillSelf();
                    events.CancelEvent(EVENT_CHECK_ACOLYTE);
                    break;
                default:
                    break;
            }
        }
    }
private:
    EventMap events;
};

/*######
## npc_aurius
######*/

enum AuriusQuests
{
    QUEST_AURIUS_RECKONING          = 5125,
    QUEST_THE_MEDALLION_OF_FAITH    = 5122
};

struct npc_aurius : public ScriptedAI
{
    npc_aurius(Creature* creature) : ScriptedAI(creature), instance(me->GetInstanceScript()) { }

    InstanceScript* instance;
    uint32 entry;
    bool IsFakeDeath;

    void Reset() override
    {
        if (me->GetEntry() == NPC_AURIUS_1)
            instance->SetData(TYPE_EVENT_AURIUS, NOT_STARTED);

        IsFakeDeath = false;
    }

    void FakeDeath()
    {
        if (!IsFakeDeath)
        {
            IsFakeDeath = true;
            me->StopMoving();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
            me->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
            me->SetStandState(UNIT_STAND_STATE_DEAD);
            me->AddUnitState(UNIT_STATE_DIED);
            me->CombatStop();
            me->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION);
            me->InterruptNonMeleeSpells(true);
            me->GetThreatManager().ClearAllThreat();
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage)
    {
        if (damage >= me->GetHealth())
        {
            if (me->GetHealth() > 1)
                damage = me->GetHealth() - 1;
            else
                damage = 0;

            FakeDeath();
        }
    }

    void QuestCompleted(Player* player, Quest const* quest)
    {
        if ((entry == NPC_AURIUS_1) && (quest->GetQuestId() == QUEST_THE_MEDALLION_OF_FAITH))
        {
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            instance->SetGuidData(DATA_QUESTPLAYER, player->GetGUID());
            instance->SetData(TYPE_EVENT_AURIUS, SPECIAL);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        switch (entry)
        {
            case NPC_AURIUS_1:
            {
                switch (instance->GetData(TYPE_BARON))
                {
                    case IN_PROGRESS:
                    case FAIL:
                    case DONE:
                    {
                        if ((instance->GetData(TYPE_EVENT_AURIUS)) != NOT_STARTED)
                            me->SetVisible(false);
                        break;
                    }
                }
                break;
            }
            case NPC_AURIUS_2:
            {
                switch (instance->GetData(TYPE_BARON))
                {
                    case IN_PROGRESS:
                    {
                        if (((instance->GetData(TYPE_EVENT_AURIUS)) == IN_PROGRESS) && (me->GetStandState() != UNIT_STAND_STATE_DEAD))
                        {
                            if (Creature* Target = me->GetMap()->GetCreature(instance->GetGuidData(DATA_BARON)))
                            {
                                if (Target->GetHealthPct() <= 20.0f)
                                    FakeDeath();
                                else
                                    me->AI()->AttackStart(Target);
                            }

                        }
                        break;
                    }
                    case FAIL:
                    {
                        if ((instance->GetData(TYPE_EVENT_AURIUS)) == IN_PROGRESS)
                        {
                            FakeDeath();
                            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                            instance->SetData(TYPE_EVENT_AURIUS, FAIL);
                        }
                        break;
                    }
                    case DONE:
                    {
                        if ((instance->GetData(TYPE_EVENT_AURIUS)) == IN_PROGRESS)
                        {
                            FakeDeath();
                            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                            instance->SetData(TYPE_EVENT_AURIUS, DONE);
                        }
                        break;
                    }
                }
                break;
            }
            default:
                break;
        }
        DoMeleeAttackIfReady();
    }
};

bool QuestComplete_npc_aurius(Player* player, Creature* creature, Quest const* quest)
{
    if (npc_aurius* ScriptedAI = dynamic_cast<npc_aurius*>(creature->AI()))
    {
        ScriptedAI->QuestCompleted(player, quest);
        return true;
    }
    return false;
}

class spell_ysida_saved_credit : public SpellScript
{
    PrepareSpellScript(spell_ysida_saved_credit);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_YSIDA_SAVED });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject* obj)
        {
            return obj->GetTypeId() != TYPEID_PLAYER;
        });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Player* player = GetHitUnit()->ToPlayer())
        {
            player->AreaExploredOrEventHappens(QUEST_DEAD_MAN_PLEA);
            player->KilledMonsterCredit(NPC_YSIDA);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ysida_saved_credit::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_ysida_saved_credit::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

enum HauntingPhantoms
{
    SPELL_SUMMON_SPITEFUL_PHANTOM = 16334,
    SPELL_SUMMON_WRATH_PHANTOM    = 16335
};

class spell_stratholme_haunting_phantoms : public AuraScript
{
    PrepareAuraScript(spell_stratholme_haunting_phantoms);

    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& amplitude)
    {
        isPeriodic = true;
        amplitude = irand(30, 90) * IN_MILLISECONDS;
    }

    void HandleDummyTick(AuraEffect const* /*aurEff*/)
    {
        if (roll_chance_i(50))
            GetTarget()->CastSpell(nullptr, SPELL_SUMMON_SPITEFUL_PHANTOM, true);
        else
            GetTarget()->CastSpell(nullptr, SPELL_SUMMON_WRATH_PHANTOM, true);
    }

    void HandleUpdatePeriodic(AuraEffect* aurEff)
    {
        aurEff->CalculatePeriodic(GetCaster());
    }

    void Register() override
    {
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_stratholme_haunting_phantoms::CalcPeriodic, EFFECT_0, SPELL_AURA_DUMMY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_stratholme_haunting_phantoms::HandleDummyTick, EFFECT_0, SPELL_AURA_DUMMY);
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_stratholme_haunting_phantoms::HandleUpdatePeriodic, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

void AddSC_stratholme()
{
    new go_gauntlet_gate();
    new go_service_gate();
    new go_postbox();
    new npc_restless_soul();
    new npc_spectral_ghostly_citizen();
    RegisterStratholmeCreatureAI(npc_crystal_zigurat);
    RegisterStratholmeCreatureAI(npc_aurius);
    RegisterSpellScript(spell_ysida_saved_credit);
    RegisterAuraScript(spell_stratholme_haunting_phantoms);
}
