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
#include "AreaBoundary.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "EventMap.h"
#include "GameObject.h"
#include "InstanceScript.h"
#include "Log.h"
#include "Map.h"
#include "MotionMaster.h"
#include "Player.h"
#include "stratholme.h"
#include "Unit.h"
#include "ScriptedCreature.h"
#include "TemporarySummon.h"

enum InstanceEvents
{
    EVENT_BARON_RUN             = 1,
    EVENT_SLAUGHTER_SQUARE      = 2,
    EVENT_ABOM                  = 3
};

EllipseBoundary const beforeScarletGate(Position(3671.158f, -3181.79f), 60.0f, 40.0f);

class instance_stratholme : public InstanceMapScript
{
    public:
        instance_stratholme() : InstanceMapScript(StratholmeScriptName, 329) { }

        struct instance_stratholme_InstanceMapScript : public InstanceScript
        {
            instance_stratholme_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetHeaders(DataHeader);

                for (uint8 i = 0; i < 5; ++i)
                    IsSilverHandDead[i] = false;

                IsHearthsingerSpawned = false;
                IsRamsteinSummoned = false;
                IsTimmySpawned = false;
                ScarletsKilled = 0;
                UsedPostboxes = 0;
                BlackGuardsKilledCounter = 4;
            }

            GuidSet CrystalsGUID;
            GuidSet AbomnationGUID;
            GuidSet PlaceEcarlateGUID;
            GuidList SlaughterAboGUID;
            ObjectGuid ServiceEntranceGUID;
            ObjectGuid GauntletGate1GUID;
            ObjectGuid Ziggurat1GUID;
            ObjectGuid Ziggurat2GUID;
            ObjectGuid Ziggurat3GUID;
            ObjectGuid Ziggurat4GUID;
            ObjectGuid Ziggurat5GUID;
            ObjectGuid GauntletGateGUID;
            ObjectGuid SlaugtherGateGUID;
            ObjectGuid ElderGateGUID;
            ObjectGuid YsidaCageGUID;
            ObjectGuid RatTrapGate1GUID;
            ObjectGuid RatTrapGate2GUID;
            ObjectGuid RatTrapGate3GUID;
            ObjectGuid RatTrapGate4GUID;
            ObjectGuid AuriusGUID;
            ObjectGuid QuestplayerGUID;
            ObjectGuid DathrohanGUID;
            ObjectGuid BarthilasGUID;
            ObjectGuid RivendareGUID;
            ObjectGuid YsidaGUID;
            ObjectGuid YsidaTriggerGUID;
            ObjectGuid RamsteinGUID;
            ObjectGuid TimmyGUID;
            ObjectGuid HearthsingerGUID;
            uint32 UltimatumDebuff[4] =
            {
                SPELL_BARON_ULTIMATUM_45MIN,
                SPELL_BARON_ULTIMATUM_10MIN,
                SPELL_BARON_ULTIMATUM_5MIN,
                SPELL_BARON_ULTIMATUM_1MIN
            };
            uint32 UsedPostboxes;
            uint8 AbomsKilledCounter;
            uint8 ScarletsKilled;
            uint8 BlackGuardsKilledCounter;
            uint8 YellCounter;
            std::string strInstData;
            Position const timmyTheCruelSpawnPosition = { 3625.358f, -3188.108f, 130.3985f, 4.834562f };
            bool IsSilverHandDead[5];
            bool IsTimmySpawned;
            bool IsHearthsingerSpawned;
            bool IsRamsteinSummoned;

            bool StartSlaugtherSquare()
            {
                uint32 count = CrystalsGUID.size();

                for (GuidSet::iterator i = CrystalsGUID.begin(); i != CrystalsGUID.end(); ++i)
                {
                    if (Creature* crystal = instance->GetCreature(*i))
                    {
                        if (!crystal->IsAlive())
                            --count;
                    }
                    else
                        --count;
                }

                if (!count)
                {
                    UpdateGoState(GauntletGateGUID, GO_STATE_ACTIVE, false);
                    UpdateGoState(SlaugtherGateGUID, GO_STATE_ACTIVE, false);
                    SetData(TYPE_ALL_CRYSTALS_DIED, DONE);
                    return true;
                }
                else
                {
                    TC_LOG_DEBUG("scripts", "Instance Stratholme: Cannot open slaugther square yet.");
                    return false;
                }
            }

            // If withRestoreTime true, then newState will be ignored and GO should be restored to original state after 10 seconds
            void UpdateGoState(ObjectGuid goGuid, uint32 newState, bool withRestoreTime)
            {
                if (!goGuid)
                    return;

                if (GameObject* go = instance->GetGameObject(goGuid))
                {
                    if (withRestoreTime)
                        go->UseDoorOrButton(10);
                    else
                        go->SetGoState((GOState)newState);
                }
            }

            void OnCreatureCreate(Creature* creature) override
            {
                InstanceScript::OnCreatureCreate(creature);

                switch (creature->GetEntry())
                {
                    case NPC_BARON:
                        RivendareGUID = creature->GetGUID();
                        if (GetData(TYPE_RAMSTEIN) != DONE)
                            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        break;
                    case NPC_YSIDA:
                        YsidaGUID = creature->GetGUID();
                        creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        break;
                    case NPC_YSIDA_TRIGGER:
                        YsidaTriggerGUID = creature->GetGUID();
                        break;
                    case NPC_CRYSTAL:
                        CrystalsGUID.insert(creature->GetGUID());
                        break;
                    case NPC_ABOM_BILE:
                    case NPC_ABOM_VENOM:
                        AbomnationGUID.insert(creature->GetGUID());
                        break;
                    case NPC_BARTHILAS:
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        break;
                    case NPC_RAMSTEIN:
                        RamsteinGUID = creature->GetGUID();
                        break;
                    case NPC_TIMMY_THE_CRUEL:
                        TimmyGUID = creature->GetGUID();
                        break;
                    case NPC_HEARTHSINGER_FORRESTEN:
                        IsHearthsingerSpawned = true;
                        break;
                    case NPC_AURIUS_1:
                        AuriusGUID = creature->GetGUID();
                        break;
                    case NPC_DATHROHAN:
                        DathrohanGUID = creature->GetGUID();
                        break;
                }

                PlaceEcarlateGUID.insert(creature->GetGUID());

                if (!IsHearthsingerSpawned)
                {
                    switch (urand(1, 4))
                    {
                        case 1:
                            creature->SummonCreature(NPC_HEARTHSINGER_FORRESTEN, 3717.1f, -3499.8f, 130.6f, 2.14583f, TEMPSUMMON_DEAD_DESPAWN, 0);
                            break;
                        case 2:
                            creature->SummonCreature(NPC_HEARTHSINGER_FORRESTEN, 3696.47f, -3370.6f, 131.71f, 3.354f, TEMPSUMMON_DEAD_DESPAWN, 0);
                            break;
                        case 3:
                            creature->SummonCreature(NPC_HEARTHSINGER_FORRESTEN, 3556.1f, -3397.61f, 134.1f, 3.147f, TEMPSUMMON_DEAD_DESPAWN, 0);
                            break;
                        default:
                            creature->SummonCreature(NPC_HEARTHSINGER_FORRESTEN, 3595.75f, -3509.93f, 137.6f, 5.74213f, TEMPSUMMON_DEAD_DESPAWN, 0);
                            break;
                    }
                }
            }

            void OnUnitDeath(Unit* who) override
            {
                Creature* creature = who->ToCreature();
                if (!creature)
                    return;

                switch (who->GetEntry())
                {
                    case NPC_CRIMSON_GUARDSMAN:
                    case NPC_CRIMSON_CONJUROR:
                    case NPC_CRIMSON_INITATE:
                    case NPC_CRIMSON_GALLANT:
                        if (!IsTimmySpawned)
                        {
                            Position pos = who->ToCreature()->GetHomePosition();
                            // check if they're in front of the entrance
                            if (beforeScarletGate.IsWithinBoundary(pos))
                            {
                                if (++ScarletsKilled >= TIMMY_THE_CRUEL_CRUSADERS_REQUIRED)
                                {
                                    instance->SummonCreature(NPC_TIMMY_THE_CRUEL, timmyTheCruelSpawnPosition);
                                    IsTimmySpawned = true;
                                }
                            }
                        }
                        break;
                    case NPC_BLACK_GUARD:
                        BlackGuardsKilledCounter--;
                        if (!BlackGuardsKilledCounter)
                        {
                            if (Creature* baron = instance->GetCreature(RivendareGUID))
                                baron->AI()->Talk(YELL_BLACK_GUARD_DEATH);
                        }
                        break;
                    case NPC_ABOM_BILE:
                    case NPC_ABOM_VENOM:
                        SetData(TYPE_RAMSTEIN, SPECIAL);
                        break;
                    default:
                        break;
                }
            }

            void OnCreatureRemove(Creature* creature) override
            {
                InstanceScript::OnCreatureRemove(creature);

                switch (creature->GetEntry())
                {
					case NPC_CRYSTAL:
                        CrystalsGUID.erase(creature->GetGUID());
                        break;
                    case NPC_ABOM_BILE:
                    case NPC_ABOM_VENOM:
                        AbomnationGUID.erase(creature->GetGUID());
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                InstanceScript::OnGameObjectCreate(go);

                switch (go->GetEntry())
                {
                    case GO_SERVICE_ENTRANCE:
                        ServiceEntranceGUID = go->GetGUID();
                        break;
                    case GO_GAUNTLET_GATE1:
                        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                        GauntletGate1GUID = go->GetGUID();
                        break;
                    case GO_ZIGGURAT1:
                        Ziggurat1GUID = go->GetGUID();
                        if (GetData(TYPE_BARONESS) == IN_PROGRESS)
                            HandleGameObject(Ziggurat1GUID, true, go);
                        break;
                    case GO_ZIGGURAT2:
                        Ziggurat2GUID = go->GetGUID();
                        if (GetData(TYPE_NERUB) == IN_PROGRESS)
                            HandleGameObject(Ziggurat2GUID, true, go);
                        break;
                    case GO_ZIGGURAT3:
                        Ziggurat3GUID = go->GetGUID();
                        if (GetData(TYPE_PALLID) == IN_PROGRESS)
                            HandleGameObject(Ziggurat3GUID, true, go);
                        break;
                    case GO_ZIGGURAT4:
                        Ziggurat4GUID = go->GetGUID();
                        if (GetData(TYPE_RAMSTEIN) == DONE)
                            HandleGameObject(Ziggurat4GUID, true, go);
                        break;
                    case GO_ZIGGURAT5:
                        Ziggurat5GUID = go->GetGUID();
                        if (GetData(TYPE_RAMSTEIN) == DONE)
                            HandleGameObject(Ziggurat5GUID, true, go);
                        break;
                    case GO_PORT_GAUNTLET:
                        GauntletGateGUID = go->GetGUID();
                        if (GetData(TYPE_ALL_CRYSTALS_DIED) == DONE)
                            HandleGameObject(GauntletGateGUID, true, go);
                        break;
                    case GO_PORT_SLAUGTHER:
                        SlaugtherGateGUID = go->GetGUID();
                        if (GetData(TYPE_ALL_CRYSTALS_DIED) == DONE)
                            HandleGameObject(SlaugtherGateGUID, true, go);
                        break;
                    case GO_PORT_ELDERS:
                        ElderGateGUID = go->GetGUID();
                        break;
                    case GO_GATE_TRAP_RAT_1:
                        RatTrapGate1GUID = go->GetGUID();
                        break;
                    case GO_GATE_TRAP_RAT_2:
                        RatTrapGate2GUID = go->GetGUID();
                        break;
                    case GO_GATE_TRAP_RAT_3:
                        RatTrapGate3GUID = go->GetGUID();
                        break;
                    case GO_GATE_TRAP_RAT_4:
                        RatTrapGate4GUID = go->GetGUID();
                        break;
                    case GO_YSIDA_CAGE:
                        YsidaCageGUID = go->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_SH_QUEST:
                        if (IsSilverHandDead[0] && IsSilverHandDead[1] && IsSilverHandDead[2] && IsSilverHandDead[3] && IsSilverHandDead[4])
                            return 1;
                        return 0;
                    default:
                        break;
                }
                return 0;
            }

            ObjectGuid GetGuidData(uint32 data) const override
            {
                switch (data)
                {
                    case DATA_BARON:
                        return RivendareGUID;
                    case DATA_YSIDA_TRIGGER:
                        return YsidaTriggerGUID;
                    case DATA_AURIUS:
                        return AuriusGUID;
                    case DATA_QUESTPLAYER:
                        return QuestplayerGUID;
                    case NPC_DATHROHAN:
                        return DathrohanGUID;
                }
                return InstanceScript::GetGuidData(data);
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case TYPE_BARON:
                        if (state == IN_PROGRESS)
                        {
                            if (GetData(TYPE_EVENT_AURIUS) == SPECIAL)
                            {
                                if (Player* player = instance->GetPlayer(GetGuidData(DATA_QUESTPLAYER)))
                                {
                                    player->SummonCreature(NPC_AURIUS_2, 4045.71f, -3357.38f, 115.10f, 2.08f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1800000);
                                    SetData(TYPE_EVENT_AURIUS, IN_PROGRESS);
                                }
                            }

                            if (GameObject* go = instance->GetGameObject(Ziggurat4GUID))
                                if (go->GetGoState() != GO_STATE_READY)
                                    UpdateGoState(Ziggurat4GUID, GO_STATE_READY, false);

                            UpdateGoState(Ziggurat5GUID, GO_STATE_READY, false);

                            if (GameObject* go = instance->GetGameObject(GauntletGateGUID))
                                if (go->GetGoState() != GO_STATE_READY)
                                    UpdateGoState(GauntletGateGUID, GO_STATE_READY, false);
                        }

                        if (state == DONE)
                        {
                            if (GetData(TYPE_BARON_RUN) == IN_PROGRESS)
                            {
                                Map::PlayerList const& players = instance->GetPlayers();

                                for (auto const& i : players)
                                {
                                    if (Player* player = i.GetSource())
                                    {
                                        for (uint32 i = 0; i <= sizeof(UltimatumDebuff); i++)
                                            DoRemoveAurasDueToSpellOnPlayers(i);

                                        if (player->GetQuestStatus(QUEST_DEAD_MAN_PLEA) == QUEST_STATUS_INCOMPLETE)
                                            player->AreaExploredOrEventHappens(QUEST_DEAD_MAN_PLEA);
                                    }
                                }
                                SetData(TYPE_BARON_RUN, DONE);
                            }

                            if (GameObject* go = instance->GetGameObject(Ziggurat4GUID))
                                if (go->GetGoState() != GO_STATE_ACTIVE)
                                    UpdateGoState(Ziggurat4GUID, GO_STATE_ACTIVE, false);

                            UpdateGoState(Ziggurat5GUID, GO_STATE_ACTIVE, false);
                            UpdateGoState(GauntletGateGUID, GO_STATE_ACTIVE, false);
                        }

                        if (state == FAIL)
                        {
                            if (GameObject* go = instance->GetGameObject(Ziggurat4GUID))
                                if (go->GetGoState() != GO_STATE_ACTIVE)
                                    UpdateGoState(Ziggurat4GUID, GO_STATE_ACTIVE, false);

                            UpdateGoState(Ziggurat5GUID, GO_STATE_ACTIVE, false);

                            if (GameObject* go = instance->GetGameObject(GauntletGateGUID))
                                if (go->GetGoState() != GO_STATE_ACTIVE)
                                    UpdateGoState(GauntletGateGUID, GO_STATE_ACTIVE, false);
                        }
                        break;
                    case TYPE_BARONESS:
                        if (state == DONE)
                            UpdateGoState(Ziggurat1GUID, GO_STATE_ACTIVE, false);
                        break;
                    case TYPE_NERUB:
                        if (state == DONE)
                            UpdateGoState(Ziggurat2GUID, GO_STATE_ACTIVE, false);
                        break;
                    case TYPE_PALLID:
                        if (state == DONE)
                            UpdateGoState(Ziggurat3GUID, GO_STATE_ACTIVE, false);
                        break;
                    case TYPE_RAMSTEIN:
                        if (state == SPECIAL)
                        {
                            uint32 count = AbomnationGUID.size();
                            for (GuidSet::iterator itr = AbomnationGUID.begin(); itr != AbomnationGUID.end();)
                            {
                                if (Creature* Abom = instance->GetCreature(*itr))
                                {
                                    ++itr;
                                    if (!Abom->IsAlive())
                                        --count;
                                }
                                else
                                {
                                    AbomnationGUID.erase(itr++);
                                    --count;
                                }
                            }

                            if (!count)
                            {
                                if (Creature* BossRivendare = instance->GetCreature(RivendareGUID))
                                    BossRivendare->AI()->Talk(YELL_RAMSTEIN_THE_GORGER);

                                UpdateGoState(Ziggurat4GUID, GO_STATE_ACTIVE, true);

                                if (Creature* BossRivendare = instance->GetCreature(RivendareGUID))
                                {
                                    if (TempSummon* BossRamstein = BossRivendare->SummonCreature(NPC_RAMSTEIN, 4032.35f, -3380.567f, 119.739571f, 4.7614f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 1800000))
                                    {
                                        BossRamstein->GetMotionMaster()->MovePoint(0, 4033.009f, -3404.3293f, 115.3554f);
                                        BossRamstein->SetHomePosition(4033.009f, -3404.3293f, 115.3554f, 4.788970f);
                                        TC_LOG_DEBUG("scripts", "Instance Stratholme: Ramstein spawned.");
                                    }
                                }
                            }
                            else
                                TC_LOG_DEBUG("scripts", "Instance Stratholme: %u Abomnation left to kill.", count);
                        }
                        if (state == IN_PROGRESS)
                            HandleGameObject(GauntletGateGUID, false);

                        if (state == FAIL)
                        {
                            if (GameObject* go = instance->GetGameObject(GauntletGateGUID))
                                if (go->GetGoState() != GO_STATE_ACTIVE)
                                    UpdateGoState(GauntletGateGUID, GO_STATE_ACTIVE, false);
                        }

                        if (state == DONE)
                        {
                            UpdateGoState(Ziggurat4GUID, GO_STATE_ACTIVE, false);
                            events.ScheduleEvent(EVENT_SLAUGHTER_SQUARE, 1min);
                            TC_LOG_DEBUG("scripts", "Instance Stratholme: Slaugther event will continue in 1 minute.");
                        }
                    default:
                        break;
                }
                return true;
            }

            void SetGuidData(uint32 type, ObjectGuid data) override
            {
                switch (type)
                {
                    case DATA_QUESTPLAYER:
                        QuestplayerGUID = data;
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_BARON_RUN:
                        if (data == IN_PROGRESS)
                        {
                            if (data == IN_PROGRESS || data == FAIL)
                                break;

                            YellCounter = 0;
                            events.ScheduleEvent(EVENT_BARON_RUN, 0);

                            TC_LOG_DEBUG("scripts", "Instance Stratholme: Baron run in progress.");
                        }
                        if (data == FAIL)
                        {
                            // Remove ultimatum debuff if still existing (if Ysida
                            // dies before the timer of 45 min is finished) (GM?)
                            for (uint32 i = 0; i <= sizeof(UltimatumDebuff); i++)
                                DoRemoveAurasDueToSpellOnPlayers(i);
                        }
                        if (data == DONE)
                        {
                            if (Creature* ysida = instance->GetCreature(YsidaGUID))
                            {
                                if (GameObject* cage = instance->GetGameObject(YsidaCageGUID))
                                    cage->UseDoorOrButton();

                                float x, y, z;
                                //! This spell handles the Dead man's plea quest completion
                                ysida->CastSpell(nullptr, SPELL_YSIDA_SAVED, true);
                                ysida->SetWalk(true);
                                ysida->AI()->Talk(SAY_BARON_RUN_SUCCESS);
                                ysida->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                                ysida->GetClosePoint(x, y, z, ysida->GetObjectScale() / 3, 4.0f);
                                ysida->GetMotionMaster()->MovePoint(1, x, y, z);

                                Map::PlayerList const& players = instance->GetPlayers();
                                for (auto const& i : players)
                                {
                                    if (Player* player = i.GetSource())
                                    {
                                        if (player->IsGameMaster())
                                            continue;

                                        //! im not quite sure what this one is supposed to do
                                        //! this is server-side spell
                                        player->CastSpell(ysida, SPELL_YSIDA_CREDIT_EFFECT, true);
                                    }
                                }
                            }
                        }
                        break;
                    case TYPE_SH_AELMAR:
                        IsSilverHandDead[0] = (data) ? true : false;
                        break;
                    case TYPE_SH_CATHELA:
                        IsSilverHandDead[1] = (data) ? true : false;
                        break;
                    case TYPE_SH_GREGOR:
                        IsSilverHandDead[2] = (data) ? true : false;
                        break;
                    case TYPE_SH_NEMAS:
                        IsSilverHandDead[3] = (data) ? true : false;
                        break;
                    case TYPE_SH_VICAR:
                        IsSilverHandDead[4] = (data) ? true : false;
                        break;
                    case TYPE_CRYSTAL_DIED:
                        StartSlaugtherSquare();
                        break;
                    case TYPE_ALL_CRYSTALS_DIED:
                    case TYPE_EVENT_AURIUS:
                    case TYPE_POSTMASTER:
                        if (data == IN_PROGRESS)
                        {
                            ++UsedPostboxes;

                            if (UsedPostboxes == 2)
                                SetData(TYPE_POSTMASTER, SPECIAL);
                        }
                        break;
                    default:
                        break;
                }
                if (data == DONE)
                    SaveToDB();
            }

            //! Work in progress
            bool ComparePositionRatTrap()
            {
                float x1 = 3907.45f;
                float y1 = -3550.41f;
                float x2 = 3909.34f;
                float y2 = -3540.14f;
                float x3 = 3930.1f;
                float y3 = -3554.4f;
                float x4 = 3931.9f;
                float y4 = -3544.6f;

                Map::PlayerList const& players = instance->GetPlayers();
                for (auto const& i : players)
                {
                    if (Player* player = i.GetSource())
                    {
                        if (player->GetPositionZ() < 135 && player->GetPositionZ() > 130)
                            continue;

                        if (player->GetPositionX() < x3 && player->GetPositionX() > x2 &&
                            player->GetPositionY() < y3 && player->GetPositionY() > y2 + 4)
                            continue;

                        return true;
                    }
                }
                return false;
            }

            //! Work in progress
            bool CheckPositionRatTrap()
            {
                Map::PlayerList const& players = instance->GetPlayers();
                for (auto const& i : players)
                {
                    if (Player* player = i.GetSource())
                    {
                        if (player->IsAlive() &&
                            player->GetPositionX() < 3621.32 && player->GetPositionX() > 3603.18 &&
                            player->GetPositionY() < -3335 && player->GetPositionY() > -3340.46 &&
                            player->GetPositionZ() < 130 && player->GetPositionZ() > 123)
                            return true;
                    }
                }
                return false;
            }

            void Update(uint32 diff) override
            {
                if (events.Empty())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BARON_RUN:
                            // If Ysida does die somehow (GM?)
                            if (!instance->GetCreature(YsidaGUID))
                                SetData(TYPE_BARON_RUN, FAIL);

                            switch (GetData(TYPE_BARON_RUN))
                            {
                                case IN_PROGRESS:
                                    if (YellCounter == 0)
                                    {
                                        YellCounter++;

                                        if (Creature* BossRivendare = instance->GetCreature(RivendareGUID))
                                            BossRivendare->AI()->Talk(YELL_BARON_RUN_45MIN);

                                        for (uint32 i = 0; i <= sizeof(UltimatumDebuff); i++)
                                            DoRemoveAurasDueToSpellOnPlayers(i);

                                        DoCastSpellOnPlayers(SPELL_BARON_ULTIMATUM_45MIN);                                                             
                                        events.Repeat(35min);
                                    }
                                    else if (YellCounter == 1)
                                    {
                                        YellCounter++;

                                        if (Creature* BossRivendare = instance->GetCreature(RivendareGUID))
                                            BossRivendare->AI()->Talk(YELL_BARON_RUN_10MIN);

                                        for (uint32 i = 0; i <= sizeof(UltimatumDebuff); i++)
                                            DoRemoveAurasDueToSpellOnPlayers(i);

                                        DoCastSpellOnPlayers(SPELL_BARON_ULTIMATUM_10MIN);
                                        events.Repeat(5min);
                                    }
                                    else if (YellCounter == 2)
                                    {
                                        YellCounter++;

                                        if (Creature* BossRivendare = instance->GetCreature(RivendareGUID))
                                            BossRivendare->AI()->Talk(YELL_BARON_RUN_5MIN);

                                        for (uint32 i = 0; i <= sizeof(UltimatumDebuff); i++)
                                            DoRemoveAurasDueToSpellOnPlayers(i);

                                        DoCastSpellOnPlayers(SPELL_BARON_ULTIMATUM_5MIN);
                                        events.Repeat(2s);
                                    }
                                    else if (YellCounter == 3) // Ysida\'s answer part
                                    {
                                        YellCounter++;

                                        if (Creature* Ysida = instance->GetCreature(YsidaGUID))
                                            Ysida->AI()->Talk(YELL_BARON_RUN_5MIN_Y);

                                        events.Repeat(4min);
                                    }
                                    else if (YellCounter == 4)
                                    {
                                        YellCounter++;

                                        for (uint32 i = 0; i <= sizeof(UltimatumDebuff); i++)
                                            DoRemoveAurasDueToSpellOnPlayers(i);

                                        DoCastSpellOnPlayers(SPELL_BARON_ULTIMATUM_1MIN);
                                        events.Repeat(1min);
                                    }
                                    else if (YellCounter == 5)
                                    {
                                        YellCounter = 6;

                                        if (GetData(TYPE_BARON_RUN) != DONE)
                                            SetData(TYPE_BARON_RUN, FAIL);

                                        events.Repeat(0s);
                                    }
                                    break;
                                case DONE:
                                    events.CancelEvent(EVENT_BARON_RUN);
                                    break;
                                case FAIL:
                                    if (Creature* BossRivendare = instance->GetCreature(RivendareGUID))
                                        BossRivendare->AI()->Talk(YELL_BARON_RUN_FAILED_R);

                                    if (instance->GetGameObject(YsidaCageGUID))
                                        instance->GetGameObject(YsidaCageGUID)->SetGoState(GO_STATE_ACTIVE);

                                    if (Creature* Ysida = instance->GetCreature(YsidaGUID))
                                    {
                                        Ysida->AI()->Talk(YELL_BARON_RUN_FAILED_Y);
                                        Ysida->CastSpell(Ysida, SPELL_PERM_FEIGN_DEATH, true);
                                    }
                                    events.CancelEvent(EVENT_BARON_RUN);
                                    break;
                            }
                            TC_LOG_DEBUG("scripts", "Instance Stratholme: Baron run event reached end. Event has state %u.", GetData(TYPE_BARON_RUN));
                            break;
                        case EVENT_SLAUGHTER_SQUARE:
                            if (GetData(TYPE_RAMSTEIN) == IN_PROGRESS)
                            {
                                UpdateGoState(Ziggurat4GUID, GO_STATE_READY, false);
                                if (Creature* BossRamstein = instance->GetCreature(RamsteinGUID))
                                    BossRamstein->AI()->Talk(YELL_RAMSTEIN_THE_GORGER);
                            }
                            else
                            {
                                if (Creature* BossRivendare = instance->GetCreature(RivendareGUID))
                                {
                                    for (uint8 i = 0; i < 5; ++i)
                                    {
                                        if (Creature* BossBlackGuards = BossRivendare->SummonCreature(NPC_BLACK_GUARD, 4032.84f + float(urand(0, 2)), -3380.567f + float(urand(0, 2)), 119.739571f, 4.7614f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 1800000))
                                        {
                                            BossBlackGuards->GetMotionMaster()->MovePoint(0, 4033.34f, -3419.75f, 116.35f);
                                            BossBlackGuards->SetHomePosition(4033.34f, -3419.75f, 116.35f, 4.80f);

                                            if (i == 0)
                                                BossBlackGuards->AI()->Talk(YELL_BLACK_GUARD_SPAWN);
                                        }
                                    }
                                    UpdateGoState(Ziggurat4GUID, GO_STATE_ACTIVE, false);
                                    UpdateGoState(Ziggurat5GUID, GO_STATE_ACTIVE, false);
                                    instance->GetCreature(RivendareGUID)->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                                    TC_LOG_DEBUG("scripts", "Instance Stratholme: Black guard sentries spawned. Opening gates to baron.");
                                }
                            }
                            break;
                    }
                }
            }
            private:
                EventMap events;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_stratholme_InstanceMapScript(map);
        }
};

void AddSC_instance_stratholme()
{
    new instance_stratholme();
}
