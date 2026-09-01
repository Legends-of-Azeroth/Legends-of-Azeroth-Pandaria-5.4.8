/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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

#include "stormstout_brewery.h"

struct ValuePair
{
    uint32 entry_1;
    uint32 entry_2;
};

static const ValuePair yanzhuPairs[3] =
{
    { NPC_YEASTY_ALEMENTAL,  NPC_BUBBLING_ALEMENTAL },
    { NPC_FIZZY_ALEMENTAL,   NPC_SUDSY_ALEMENTAL    },
    { NPC_BLOATED_ALEMENTAL, NPC_STOUT_ALEMENTAL    },
};

static const Position gaoWaypoints[] =
{
    { 0.f, 0.f, 0.f                    },
    { -703.3f, 1187.5f, 166.7f         },
    { -707.4f, 1189.8f, 166.7f         },
    { -709.6f, 1185.0f, 166.7f, 4.905f }
};

static const Position chenWaypoints[] =
{
    { -673.7f, 1155.7f, 166.7f          },
    { 0.f,     0.f,     0.f             }, // empty
    { -708.1f, 1180.2f, 166.7f, 1.832f  }
};

static const Position largeBrewPos[] =
{
    { -702.09f, 1158.76f, 166.22f, 0.24f },
    { -704.20f, 1166.94f, 166.22f, 0.24f }
};

static const Position smallBrewPos[] =
{
    { -709.23f, 1152.57f, 166.22f, 0.17f },
    { -711.52f, 1160.55f, 166.22f, 0.17f },
    { -713.58f, 1168.67f, 166.22f, 0.17f },
    { -701.03f, 1154.85f, 166.22f, 0.17f },
    { -703.38f, 1162.76f, 166.22f, 0.17f },
    { -705.75f, 1170.97f, 166.22f, 0.17f },
    { -693.20f, 1157.04f, 166.22f, 0.17f },
    { -695.33f, 1165.07f, 166.22f, 0.17f },
    { -697.44f, 1173.05f, 166.22f, 0.17f },
};

static const Position middleBrewPos[] =
{
    { -700.52f, 1167.84f, 166.22f, 0.25f },
    { -698.25f, 1159.97f, 166.22f, 0.25f },
    { -708.61f, 1165.69f, 166.22f, 0.25f },
    { -706.43f, 1157.70f, 166.22f, 0.25f },
};

static const Position yanzhuPos[] =
{
    { -703.44f, 1162.43f, 166.22f, 0.24f }
};
static const Position gaoPotPos[] =
{
    { -676.96f, 1193.96f, 166.79f, 1.82f }
};

static const Position sudsPos[2] =
{
    { -696.25f, 1138.78f, 166.75f, 1.82f },
    { -663.20f, 1172.84f, 166.74f, 3.38f }
};

const uint32 brewSpells[3] =
{
    128253,
    128255,
    128257,
};

class AliveCheck
{
    public:
        AliveCheck(Creature* creature) : _creature(creature) { }

        bool operator()(ObjectGuid guid)
        {
            return (GetAffectedCreature(guid) && !GetAffectedCreature(guid)->IsAlive());
        }

    private:
        Creature* _creature;
        Creature* GetAffectedCreature(ObjectGuid guid)
        {
            Creature* creature = ObjectAccessor::GetCreature(*_creature, guid);
            return creature ? creature : nullptr;
        }
};

class npc_uncle_gao : public CreatureScript
{
    public:
        npc_uncle_gao() : CreatureScript("npc_uncle_gao") { }

        enum EncounterStage
        {
            STAGE_MIDDLE_ADDS,
            STAGE_SMALL_ADDS,
            STAGE_LARGE_ADDS,
            STAGE_BOSS
        };

        enum Talks
        {
            TALK_SUMMON1,
            TALK_SUMMON2,
            TALK_SUMMON3,
            TALK_SUMMON_BOSS,
            TALK_BOSS_DEATH,
            TALK_OUTRO_1,
            TALK_OUTRO_2,
            TALK_OUTRO_3,
            TALK_OUTRO_4,
            TALK_OUTRO_5,
            TALK_OUTRO_6
        };

        enum Events
        {
            EVENT_NONE,
            EVENT_HANDLE_SUMMONING,
            EVENT_HANDLE_SPELLCASTING,
            EVENT_HANDLE_CRAFT_ANIMATION,
            EVENT_NEXT_POT,
            EVENT_OUTRO_1, // such flavor
            EVENT_OUTRO_2, // name of stormstout
            EVENT_OUTRO_3,
            EVENT_OUTRO_4,
            EVENT_OUTRO_5,
            EVENT_OUTRO_6,
            EVENT_OUTRO_7,
            EVENT_MOVECHECK,
            EVENT_UPDATE_ENCOUNTER,
        };

        enum Actions
        {
            ACTION_START_OUTRO,
            ACTION_OUTRO_3,
            ACTION_OUTRO_4,
            ACTION_OUTRO_6,
            ACTION_START_BOSS,
            ACTION_SUMMON_BOSS
        };

        enum Spells
        {
            SPELL_BREW_FINALE_WHEAT    = 128253,
            SPELL_BREW_FINALE_MEDIUM   = 128255,
            SPELL_BREW_FINALE_DARK     = 128257,

            SPELL_SMALL_SPAWN          = 128242,
            SPELL_MEDIUM_SPAWN         = 128243,
            SPELL_LARGE_SPAWN          = 128244
        };

        struct npc_uncle_gaoAI : public ScriptedAI
        {
            npc_uncle_gaoAI(Creature* creature) : ScriptedAI(creature)
            {
                encounterStarted = false;
            }

            bool encounterStarted;
            uint32 encounterStage;
            uint32 waypoint;
            uint32 addStore[3];
            std::list<ObjectGuid> currentStageGuidList;
            EventMap events;
            InstanceScript* instance;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                Reset();
            }

            void Reset() override
            {
                if (instance && instance->GetBossState(DATA_YAN_ZHU) == DONE)
                {
                    encounterStarted = true;

                    me->NearTeleportTo(gaoWaypoints[3].m_positionX, gaoWaypoints[3].m_positionY, gaoWaypoints[3].m_positionZ, gaoWaypoints[3].GetOrientation());

                    GetChenAndDoAction(4);
                    return;
                }
            }

            uint32 GetAddToSummonEntry(uint32 type)
            {
                if (instance) // always uses second value, broken in 24459 FIXME
                    return instance->GetData(type) ? yanzhuPairs[type - 3].entry_2 : yanzhuPairs[type - 3].entry_1;

                return 0;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!encounterStarted)
                    if (who && who->ToPlayer() && who->GetDistance(me) < 30.f && who->GetPositionZ() > 160.f)
                        StartEncounter();
            }

            void StartEncounter()
            {
                encounterStarted = true;
                encounterStage = 0;

                for (int i = 3; i < 5 + 1; i++)
                    addStore[i - 3] = GetAddToSummonEntry(i);

                AdvanceEncounter();
            }

            bool DoSummonSmallAdds(int n)
            {
                if (n < 0)
                    return false;

                if (Creature* creature = me->SummonCreature(addStore[0], smallBrewPos[n], TEMPSUMMON_CORPSE_TIMED_DESPAWN, Milliseconds(urand(14000, 24000))))
                {
                    currentStageGuidList.push_back(creature->GetGUID());
                    creature->CastSpell(creature, SPELL_SMALL_SPAWN);
                }

                return DoSummonSmallAdds(n - 1);
            }

            bool DoSummonMediumAdds(int n)
            {
                if (n < 0)
                    return false;

                if (Creature* creature = me->SummonCreature(addStore[1], middleBrewPos[n], TEMPSUMMON_CORPSE_TIMED_DESPAWN, Milliseconds(urand(14000, 24000))))
                {
                    currentStageGuidList.push_back(creature->GetGUID());
                    creature->CastSpell(creature, SPELL_MEDIUM_SPAWN);
                }

                return DoSummonMediumAdds(n - 1);
            }

            bool DoSummonLargeAdds(int n)
            {
                if (n < 0)
                    return false;

                if (Creature* creature = me->SummonCreature(addStore[2], largeBrewPos[n], TEMPSUMMON_CORPSE_TIMED_DESPAWN, Milliseconds(urand(14000, 24000))))
                {
                    currentStageGuidList.push_back(creature->GetGUID());
                    creature->CastSpell(creature, SPELL_LARGE_SPAWN);
                }

                return DoSummonLargeAdds(n - 1);
            }

            void GetChenAndDoAction(int32 action)
            {
                if (instance)
                    if (Creature* chen = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_CHEN_YANZHU)))
                        chen->AI()->DoAction(action);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case 0:
                        waypoint++;
                        me->SetStandState(UNIT_STAND_STATE_KNEEL);
                        events.ScheduleEvent(EVENT_OUTRO_1, 3300);
                        break;
                    case 1:
                    case 2:
                        waypoint++;
                        events.ScheduleEvent(EVENT_MOVECHECK, 100);
                        break;
                    case 3:
                        waypoint++;
                        events.ScheduleEvent(EVENT_OUTRO_7, 3400);
                        me->SetFacingTo(gaoWaypoints[pointId].GetOrientation());
                        break;
                    case 101:
                        me->SetFacingTo(gaoPotPos->GetOrientation());
                        me->HandleEmoteCommand(EMOTE_STATE_USE_STANDING);
                        break;
                }
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_OUTRO:
                    {
                        me->HandleEmoteCommand(EMOTE_STATE_NONE);

                        if (instance)
                        {
                            if (Creature* yanZhu = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YAN_ZHU)))
                            {
                                float x, y;
                                GetPositionWithDistInOrientation(me, (me->GetDistance(yanZhu) - 1.5f), me->GetAngle(yanZhu), x, y);

                                waypoint = 0;
                                me->SetWalk(false);
                                me->GetMotionMaster()->MovePoint(waypoint, x, y, me->GetMap()->GetHeight(x, y, me->GetPositionZ(), true, 100.0f));
                            }
                        }

                        events.Reset();
                        Talk(TALK_BOSS_DEATH);
                        break;
                    }
                    case ACTION_OUTRO_3:
                        events.ScheduleEvent(EVENT_OUTRO_3, 6000);
                        break;
                    case ACTION_OUTRO_4:
                        events.ScheduleEvent(EVENT_OUTRO_4, 5300);
                        break;
                    case ACTION_OUTRO_6:
                        events.ScheduleEvent(EVENT_OUTRO_6, 3800);
                        break;
                    case ACTION_START_BOSS:
                        if (encounterStarted)
                            return;
                        StartEncounter();
                        break;
                    case ACTION_SUMMON_BOSS:
                        AdvanceEncounter();
                        break;
                }
            }

            bool UpdateCurrentEncounterState(bool isBoss)
            {
                if (!isBoss)
                {
                    if (currentStageGuidList.empty())
                        return AdvanceEncounter();

                    currentStageGuidList.remove_if(AliveCheck(me));
                }

                return true;
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon)
                    summon->AI()->DoZoneInCombat();
            }

            bool AdvanceEncounter()
            {
                events.CancelEvent(EVENT_UPDATE_ENCOUNTER);

                switch (encounterStage)
                {
                    case 0:
                        events.ScheduleEvent(EVENT_HANDLE_SUMMONING, 9000);
                        events.ScheduleEvent(EVENT_HANDLE_SPELLCASTING, 5000);
                        events.ScheduleEvent(EVENT_NEXT_POT, 20000);
                        DoTalk(TALK_SUMMON1);
                        break;
                    case 1:
                        events.ScheduleEvent(EVENT_HANDLE_SUMMONING, 12000);
                        events.ScheduleEvent(EVENT_HANDLE_SPELLCASTING, 7000);
                        DoTalk(TALK_SUMMON2);
                        break;
                    case 2:
                        events.ScheduleEvent(EVENT_HANDLE_SUMMONING, 9000);
                        events.ScheduleEvent(EVENT_HANDLE_SPELLCASTING, 5000);
                        DoTalk(TALK_SUMMON3);
                        break;
                    case 3:
                        events.ScheduleEvent(EVENT_HANDLE_SPELLCASTING, 100);
                        events.ScheduleEvent(EVENT_HANDLE_SUMMONING, 5100);
                        break;
                }

                encounterStage++;
                return true;
            }

            void DoHandleSpellcasting()
            {
                if (Creature* caster = GetClosestCreatureWithEntry(me, NPC_BARREL_TOSS_BUNNY, 10.f))
                {
                    uint32 spell = 0;
                    switch (encounterStage-1)
                    {
                        case 0:
                        case 1:
                        case 2:
                            spell = brewSpells[encounterStage - 1];
                            break;
                        case 3:
                            for (int i = 0; i < 2; i++)
                                caster->CastSpell(caster, brewSpells[i], true);
                            break;
                    }

                    caster->CastSpell(caster, spell, true);
                }
            }

            void DoTalk(uint8 id)
            {
                Talk(id);
                events.ScheduleEvent(EVENT_HANDLE_CRAFT_ANIMATION, 2100);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_OUTRO_1:
                            Talk(TALK_OUTRO_1);
                            events.ScheduleEvent(EVENT_OUTRO_2, 8900);
                            break;
                        case EVENT_OUTRO_2:
                            Talk(TALK_OUTRO_2);
                            me->SetStandState(UNIT_STAND_STATE_STAND);

                            // Start Chen's part of the script
                            if (Creature* chen = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_CHEN_YANZHU)))
                            {
                                chen->SetVisible(true);
                                chen->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                            }
                            GetChenAndDoAction(0);
                            break;
                        case EVENT_OUTRO_3:
                            Talk(TALK_OUTRO_3);

                            // Set Chen's next timer (EVENT_OUTRO_1) 10k
                            GetChenAndDoAction(1);
                            break;
                        case EVENT_OUTRO_4:
                            Talk(TALK_OUTRO_4);

                            GetChenAndDoAction(2);
                            // call action to chens script setting his timer to 2500
                            // and move timer to 6500
                            // also set our own move timer (which also is timer for next event) to 6700
                            // "and there are virmen"
                            events.ScheduleEvent(EVENT_OUTRO_5, 6600);
                            break;
                        case EVENT_OUTRO_5:
                            Talk(TALK_OUTRO_5);
                            me->SetWalk(true);
                            events.ScheduleEvent(EVENT_MOVECHECK, 100);
                            break;
                        case EVENT_OUTRO_6:
                            Talk(TALK_OUTRO_6);
                            break;
                        case EVENT_OUTRO_7:
                            GetChenAndDoAction(3);
                            break;
                        case EVENT_MOVECHECK:
                            me->GetMotionMaster()->MovePoint(waypoint, gaoWaypoints[waypoint]);
                            break;
                        case EVENT_HANDLE_SUMMONING:
                            events.ScheduleEvent(EVENT_UPDATE_ENCOUNTER, 3000);

                            switch (encounterStage - 1)
                            {
                                case STAGE_MIDDLE_ADDS:
                                    DoSummonMediumAdds(3);
                                    break;
                                case STAGE_SMALL_ADDS:
                                    DoSummonSmallAdds(8);
                                    break;
                                case STAGE_LARGE_ADDS:
                                    DoSummonLargeAdds(1);
                                    break;
                                case STAGE_BOSS:
                                {
                                    events.CancelEvent(EVENT_UPDATE_ENCOUNTER);

                                    DoTalk(TALK_SUMMON_BOSS);
                                    if (Creature* creature = me->SummonCreature(NPC_YAN_ZHU, *yanzhuPos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, Milliseconds(5 * MINUTE*IN_MILLISECONDS)))
                                    {
                                        currentStageGuidList.push_back(creature->GetGUID());
                                        creature->CastSpell(creature, SPELL_LARGE_SPAWN);
                                        creature->CastSpell(creature, SPELL_MEDIUM_SPAWN);
                                        creature->CastSpell(creature, SPELL_SMALL_SPAWN);
                                    }
                                    break;
                                }
                            }
                            break;
                        case EVENT_HANDLE_SPELLCASTING:
                            DoHandleSpellcasting();
                            break;
                        case EVENT_UPDATE_ENCOUNTER:
                            events.ScheduleEvent(EVENT_UPDATE_ENCOUNTER, 1000);
                            UpdateCurrentEncounterState(encounterStage > STAGE_BOSS);
                            break;
                        case EVENT_NEXT_POT:
                            me->GetMotionMaster()->MovePoint(101, *gaoPotPos);
                            break;
                        case EVENT_HANDLE_CRAFT_ANIMATION:
                            me->HandleEmoteCommand(EMOTE_STATE_USE_STANDING);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_uncle_gaoAI(creature);
        }
};

class npc_chen_yanzhu : public CreatureScript
{
    public:
        npc_chen_yanzhu() : CreatureScript("npc_chen_yanzhu") { }

        enum Talks
        {
            TALK_CARE,
            TALK_COST,
            TALK_VIRMEN,
            TALK_FIRE
        };

        enum Actions
        {
            ACTION_INIT,
            ACTION_OUTRO_1,
            ACTION_OUTRO_2,
            ACTION_OUTRO_3,
        };

        enum Events
        {
            EVENT_NONE,
            EVENT_MOVECHECK,
            EVENT_MOVEBOSS,
            EVENT_CARE,
            EVENT_OUTRO_1,
            EVENT_OUTRO_2
        };

        struct npc_chen_yanzhuAI : public ScriptedAI
        {
            npc_chen_yanzhuAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            uint32 waypoint;
            EventMap events;
            InstanceScript* instance;

            void Reset() override
            {
                InitializeMyself();
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_INIT:
                    {
                        waypoint = 0;
                        me->SetWalk(false);
                        events.ScheduleEvent(EVENT_MOVECHECK, 100);
                        me->SetVisible(true);
                        break;
                    }
                    case ACTION_OUTRO_1:
                        events.ScheduleEvent(EVENT_OUTRO_1, 10000);
                        break;
                    case ACTION_OUTRO_2:
                        events.ScheduleEvent(EVENT_OUTRO_2, 2500);
                        events.ScheduleEvent(EVENT_MOVECHECK, 6500);
                        me->SetWalk(true);
                        break;
                    case ACTION_OUTRO_3:
                        Talk(TALK_FIRE);
                        GetGaoAndDoAction(3);
                        break;
                    case 4:
                        InitializeMyself();
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case 0:
                        events.ScheduleEvent(EVENT_MOVEBOSS, 100);
                        break;
                    case 1:
                        events.ScheduleEvent(EVENT_CARE, 2400);
                        break;
                    case 2:
                        me->SetFacingTo(chenWaypoints[pointId].GetOrientation());
                        break;
                }

                waypoint++;
            }

            void GetGaoAndDoAction(int32 action)
            {
                if (instance)
                    if (Creature* gao = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_UNCLE_GAO)))
                        gao->AI()->DoAction(action);
            }

            void InitializeMyself()
            {
                if (instance)
                {
                    if (instance->GetBossState(DATA_YAN_ZHU) == DONE)
                    {
                        me->NearTeleportTo(chenWaypoints[2].m_positionX, chenWaypoints[2].m_positionY, chenWaypoints[2].m_positionZ, chenWaypoints[2].GetOrientation());
                        me->SetVisible(true);
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CARE:
                            Talk(TALK_CARE);
                            GetGaoAndDoAction(1); // Start Gao's next timer
                            break;
                        case EVENT_OUTRO_1:
                            Talk(TALK_COST);
                            GetGaoAndDoAction(2); // Start Gao's next timer
                            break;
                        case EVENT_OUTRO_2:
                            Talk(TALK_VIRMEN);
                            break;
                        case EVENT_MOVECHECK:
                            me->GetMotionMaster()->MovePoint(waypoint, chenWaypoints[waypoint]);
                            break;
                        case EVENT_MOVEBOSS:
                            if (instance)
                            {
                                if (Creature* yanzhu = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YAN_ZHU)))
                                {
                                    float x, y;
                                    GetPositionWithDistInOrientation(me, (me->GetDistance(yanzhu) - 1.5f), me->GetAngle(yanzhu), x, y);
                                    me->GetMotionMaster()->MovePoint(1, x, y, me->GetMap()->GetHeight(x, y, me->GetPositionZ(), true, 100.f));
                                }
                            }
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_chen_yanzhuAI(creature);
        }
};

class boss_yanzhu : public CreatureScript
{
    public:
        boss_yanzhu() : CreatureScript("boss_yanzhu") { }

        enum Spells
        {
            SPELL_BLACKOUT             = 106851,
            SPELL_BREW_BOLT            = 114548,
            SPELL_BLOAT                = 106546,
            SPELL_CARBONATION          = 115003,
            SPELL_BUBBLE_SHIELD        = 106563,
            SPELL_SUDSY                = 114468,
            SPELL_SUDSY_JUMP           = 114470,
        };

        enum Events
        {
            EVENT_NONE,
            EVENT_WALL_OF_SUDS,
            EVENT_BLOAT,
            EVENT_BLACKOUT,
            EVENT_ADDS,
            EVENT_BUBBLE_SHIELD,
            EVENT_CARBONATION
        };

        enum Creatures
        {
            NPC_BUBBLE_SHIELD           = 65522
        };

        struct boss_yanzhuAI : public BossAI
        {
            boss_yanzhuAI(Creature* creature) : BossAI(creature, DATA_YAN_ZHU) { }

            std::vector<ObjectGuid> guidsVector;
            float bubleFacing;
            uint32 largeEventType, lowEventType;

            void InitializeAI() override
            {
                SetCombatMovement(false);
                largeEventType = EVENT_CARBONATION; // temp while wall and jumps in construction...
                lowEventType = EVENT_BLACKOUT;
                Reset();
            }

            void JustEngagedWith(Unit* who) override
            {
                _JustEngagedWith();

                // Disabled until targeting type is fixed
                /* if (me->HasAura(SPELL_SUDSY_BREW))
                    events.ScheduleEvent(EVENT_WALL_OF_SUDS, urand(9000,17000)); */

                if (me->HasAura(SPELL_YEASTY_BREW))
                    events.ScheduleEvent(EVENT_ADDS, urand(9000, 17000));

                if (me->HasAura(SPELL_BUBBLING_BREW))
                {
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    events.ScheduleEvent(EVENT_BUBBLE_SHIELD, 6000, 14000);
                }

                events.ScheduleEvent(lowEventType, 8000);
                events.ScheduleEvent(EVENT_BLOAT, 8000);
                events.ScheduleEvent(largeEventType, urand(9000, 17000));

                if (largeEventType == EVENT_WALL_OF_SUDS)
                    instance->DoAddAuraOnPlayers(SPELL_SUDSY);
            }

            void Reset() override
            {
                _Reset();

                if (instance)
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUDSY);

                events.Reset();
                summons.DespawnAll();
                bubleFacing = -M_PI / 4;
            }

            // Hack, but we haven`t movement flag jump...
            void SudsyAreaController()
            {
                std::list<Player*> playersInAreaList;
                GetPlayerListInGrid(playersInAreaList, me, 100.0f);

                for (auto&& itr : playersInAreaList)
                {
                    if (SudsyInCenterQuadr(itr->GetGUID()))
                    {
                        if (itr->GetPositionZ() > 166.15f + 0.1f && itr->HasAura(SPELL_SUDSY))
                        {
                            // not send jump to player whos falling
                            if (itr->HasUnitMovementFlag(MOVEMENTFLAG_FALLING) && itr->GetPositionZ() > 166.72f)
                                continue;

                            itr->CastSpell(itr, SPELL_SUDSY_JUMP, false);
                        }
                    }
                    else
                        if (itr->GetPositionZ() > 166.72f + 0.1f && itr->HasAura(SPELL_SUDSY))
                        {
                            // not send jump to player whos falling
                            if (itr->HasUnitMovementFlag(MOVEMENTFLAG_FALLING) && itr->GetPositionZ() > 167.32f)
                                continue;

                            itr->CastSpell(itr, SPELL_SUDSY_JUMP, false);
                        }
                }
            }

            bool SudsyInCenterQuadr(ObjectGuid playerGUID)
            {
                Player* itr = ObjectAccessor::FindPlayer(playerGUID);
                if (!itr)
                    return false;

                if (itr->GetPositionX() > -718.98f  && itr->GetPositionX() < -687.76f &&
                    itr->GetPositionY() > 1147.53f && itr->GetPositionY() < 1178.26f)
                    return true;

                return false;
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                _EnterEvadeMode();

                for (auto&& guid : guidsVector)
                {
                    if (Creature* bubble = ObjectAccessor::GetCreature(*me, guid))
                    {
                        bubble->ExitVehicle();
                        bubble->AddObjectToRemoveList();
                    }
                }

                guidsVector.clear();

                me->AttackStop();
                me->RemoveAllAurasExceptType(SPELL_AURA_DUMMY);

                me->GetMotionMaster()->MoveTargetedHome();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUDSY);

                Reset();
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();

                if (instance)
                {
                    if (Creature* gao = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_UNCLE_GAO)))
                        gao->AI()->DoAction(0);

                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUDSY);
                }

                for (auto&& guid : guidsVector)
                    if (Creature* bubble = ObjectAccessor::GetCreature(*me, guid))
                        bubble->AddObjectToRemoveList();

                guidsVector.clear();
            }

            void DoSummonWallOfSuds()
            {
                for (int i = 0; i < 2; ++i)
                    if (Creature* suds = me->SummonCreature(NPC_WALL_OF_SUDS, sudsPos[i]))
                        suds->SetCanFly(true);
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon)
                    summons.Summon(summon);
            }

            bool DoSummonFizzyBubbles(int n)
            {
                if (n < 0)
                    return false;

                Position pos = me->GetRandomNearPosition(20.f);
                pos.m_positionZ += frand(1.5f, 3.0f);

                me->SummonCreature(NPC_FIZZY_BUBBLE, pos, TEMPSUMMON_TIMED_DESPAWN, Milliseconds(12 * IN_MILLISECONDS));

                return DoSummonFizzyBubbles(n - 1);
            }

            // function should return false if it does what we want it to do
            bool UpdateAttackState()
            {
                if (me->isAttackReady() && !me->HasUnitState(UNIT_STATE_CASTING))
                {
                    if (me->GetVictim())
                    {
                        if (me->IsWithinMeleeRange(me->GetVictim()))
                        {
                            me->AttackerStateUpdate(me->GetVictim());
                            me->resetAttackTimer();
                            return false;
                        }
                        else
                        {
                            if (!DoSpellAttackIfReady(SPELL_BREW_BOLT) && !IsCombatMovementAllowed())
                            {
                                std::list<Unit*> targetList;
                                me->GetAttackableUnitListInRange(targetList, 35.f);

                                if (targetList.empty())
                                    return true;

                                std::list<Unit*>::iterator itr = targetList.begin();
                                Unit* unit;

                                for (; itr != targetList.end();)
                                {
                                    unit = *itr;

                                    if (unit)
                                    {
                                        AttackStart(unit);
                                        return false;
                                    }
                                    else
                                        itr++;
                                }

                                for (auto&& itr : me->GetThreatManager().getThreatList())
                                {
                                    if (Unit* unit = ObjectAccessor::GetUnit(*me, (*itr).getUnitGuid()))
                                    {
                                        if (me->GetDistance(unit) < 70.f)
                                        {
                                            AttackStart(unit);
                                            return false;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    return true;
                }
                return false;
            }

            Position GetBubleShieldPos(float prevOri)
            {
                float x, y;
                GetPositionWithDistInOrientation(me, 3.0f, prevOri + M_PI / 4, x, y);

                Position pos = { x, y, me->GetPositionZ() + 8.0f, float(prevOri + M_PI / 4) };  // Fuck GCC

                return pos;
            }

            void ApplyBubbleShield(int n)
            {
                for (int i = n; i < 8; i++)
                {
                    if (Creature* bubbleShield = me->SummonCreature(NPC_BUBBLE_SHIELD, GetBubleShieldPos(bubleFacing), TEMPSUMMON_CORPSE_DESPAWN))
                    {
                        me->AddAura(SPELL_BUBBLE_SHIELD, me);
                        bubleFacing += M_PI / 4;
                        guidsVector.push_back(bubbleShield->GetGUID());
                    }
                }
            }

            bool DoSummonAdds(int n)
            {
                if (n < 0)
                    return false;

                Position pos = me->GetRandomNearPosition(frand(5.f, 11.f));

                if (Creature* pAdd = me->SummonCreature(NPC_YEASTY_ALEMENTAL, pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, Milliseconds(urand(6000, 14000))))
                {
                    if (pAdd->AI())
                        pAdd->AI()->DoZoneInCombat();
                }

                return DoSummonAdds(n - 1);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                SudsyAreaController();

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WALL_OF_SUDS:
                            DoSummonWallOfSuds();
                            events.ScheduleEvent(EVENT_WALL_OF_SUDS, 13000);
                            break;
                        case EVENT_BLOAT:
                            if (Unit* unit = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.f, true))
                            {
                                DoCast(unit, SPELL_BLOAT);
                                events.ScheduleEvent(EVENT_BLOAT, urand(10000, 18000));
                            }
                            else
                                events.ScheduleEvent(EVENT_BLOAT, 1000);
                            break;
                        case EVENT_BLACKOUT:
                            DoCast(SPELL_BLACKOUT);
                            events.ScheduleEvent(EVENT_BLACKOUT, 8000);
                            break;
                        case EVENT_BUBBLE_SHIELD:
                            if (me->HasAura(SPELL_BUBBLE_SHIELD))
                            {
                                events.RescheduleEvent(EVENT_BUBBLE_SHIELD, 10 * IN_MILLISECONDS);
                                break;
                            }

                            bubleFacing = -M_PI / 4;
                            ApplyBubbleShield(0);
                            events.ScheduleEvent(EVENT_BUBBLE_SHIELD, 30000);
                            break;
                        case EVENT_ADDS:
                            DoSummonAdds(7);
                            events.ScheduleEvent(EVENT_ADDS, 30000);
                            break;
                        case EVENT_CARBONATION:
                            events.ScheduleEvent(EVENT_CARBONATION, urand(35000, 48000));
                            DoCast(SPELL_CARBONATION);
                            DoSummonFizzyBubbles(5);
                            break;
                    }
                }

                SetCombatMovement(UpdateAttackState());
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_yanzhuAI(creature);
        }
};

class npc_yeasty_alemental : public CreatureScript
{
    public:
        npc_yeasty_alemental() : CreatureScript("npc_yeasty_alemental") { }

        enum Spells
        {
            SPELL_BREW_BOLT     = 116155,
            SPELL_FERMENT       = 106859
        };

        enum Events
        {
            EVENT_NONE,
            EVENT_FERMENT
        };

        struct npc_yeasty_alementalAI : public ScriptedAI
        {
            npc_yeasty_alementalAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
                SetCombatMovement(false);
            }

            EventMap events;
            InstanceScript* instance;

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FERMENT, 10000);

                std::list<Creature*>temp;
                GetCreatureListWithEntryInGrid(temp, me, me->GetEntry(), 10.f);

                for (auto&& itr : temp)
                    itr->AI()->DoZoneInCombat();
            }

            void Reset() override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FERMENT:
                            if (instance)
                            {
                                if (Creature* boss = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YAN_ZHU)))
                                    DoCast(boss, SPELL_FERMENT);

                                events.ScheduleEvent(EVENT_FERMENT, 15000);
                            }
                            break;
                    }
                }

                if (!DoSpellAttackIfReady(SPELL_BREW_BOLT))
                {
                    SetCombatMovement(true);
                    AttackStart(me->GetVictim());
                    DoMeleeAttackIfReady();
                    return;
                }

                SetCombatMovement(false);
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_yeasty_alementalAI(creature);
        }
};

class npc_sudsy_alemental : public CreatureScript
{
    public:
        npc_sudsy_alemental() : CreatureScript("npc_sudsy_alemental") { }

        enum Spells
        {
            SPELL_BREW_BOLT   = 115650,
            SPELL_SUDS        = 116178,
            SPELL_SUDS_AURA   = 116179
        };

        enum Creatures
        {
            NPC_SUDS_TRIGGER = 56748
        };

        enum Events
        {
            EVENT_NONE,
            EVENT_SUDS,
            EVENT_BREW_BOLT
        };

        struct npc_sudsy_alementalAI : public ScriptedAI
        {
            npc_sudsy_alementalAI(Creature* creature) : ScriptedAI(creature) {  }

            EventMap events;

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_SUDS, urand(5000, 15000));
                events.ScheduleEvent(EVENT_BREW_BOLT, urand(4000, 11000));

                std::list<Creature*>temp;
                GetCreatureListWithEntryInGrid(temp, me, me->GetEntry(), 10.f);

                for (auto&& itr : temp)
                    itr->AI()->DoZoneInCombat();
            }

            void Reset() override { }

            void JustSummoned(Creature* summon) override
            {
                if (summon)
                    summon->CastSpell(summon, SPELL_SUDS_AURA, true);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->AttackStop();
                me->CombatStop(true);
                me->RemoveAllAurasExceptType(SPELL_AURA_DUMMY);
                me->SetLootRecipient(NULL);
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SUDS:
                            DoCast(SPELL_SUDS);
                            events.ScheduleEvent(EVENT_SUDS, urand(10000, 16000));
                            break;
                        case EVENT_BREW_BOLT:
                            if (Unit* unit = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.f, true))
                                DoCast(unit, SPELL_BREW_BOLT);
                            events.ScheduleEvent(EVENT_BREW_BOLT, urand(6000, 14000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sudsy_alementalAI(creature);
        }
};

class npc_fizzy_alemental : public CreatureScript
{
    public:
        npc_fizzy_alemental() : CreatureScript("npc_fizzy_alemental") { }

        enum Spells
        {
            SPELL_CARBONATION    = 116162,
            SPELL_BREW_BOLT      = 115650,
            SPELL_CARBONATE      = 116170
        };

        enum Events
        {
            EVENT_NONE,
            EVENT_BREW_BOLT,
            EVENT_CARBONATION
        };

        struct npc_fizzy_alementalAI : public ScriptedAI
        {
            npc_fizzy_alementalAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_CARBONATION, urand(5000, 15000));
                events.ScheduleEvent(EVENT_BREW_BOLT, urand(4000, 11000));

                std::list<Creature*>temp;
                GetCreatureListWithEntryInGrid(temp, me, me->GetEntry(), 10.f);

                for (auto&& itr : temp)
                    itr->AI()->DoZoneInCombat();
            }

            void Reset() override { }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->AttackStop();
                me->CombatStop(true);
                me->RemoveAllAurasExceptType(SPELL_AURA_DUMMY);
                me->SetLootRecipient(NULL);

                me->GetMotionMaster()->MoveTargetedHome();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BREW_BOLT:
                            if (Unit* unit = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.f, true))
                                DoCast(unit, SPELL_BREW_BOLT);

                            events.ScheduleEvent(EVENT_BREW_BOLT, urand(6000, 14000));
                            break;
                        case EVENT_CARBONATION:
                            if (Unit* unit = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.f, true))
                                DoCast(unit, SPELL_CARBONATION);

                            events.ScheduleEvent(EVENT_CARBONATION, urand(1600, 10000)); // i love this crash type
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_fizzy_alementalAI(creature);
        }
};

class npc_bloated_alemental : public CreatureScript
{
    public:
        npc_bloated_alemental() : CreatureScript("npc_bloated_alemental") { }

        enum Spells
        {
            SPELL_BLOAT        = 106546,
            SPELL_BREW_BOLT    = 115652
        };

        enum Events
        {
            EVENT_NONE,
            EVENT_BLOAT,
            EVENT_BREW_BOLT
        };

        struct npc_bloated_alementalAI : public ScriptedAI
        {
            npc_bloated_alementalAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_BLOAT, urand(5000, 15000));
                events.ScheduleEvent(EVENT_BREW_BOLT, urand(4000, 11000));

                std::list<Creature*>temp;
                GetCreatureListWithEntryInGrid(temp, me, me->GetEntry(), 10.f);

                for (auto&& itr : temp)
                    itr->AI()->DoZoneInCombat();
            }

            void Reset() override { }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->AttackStop();
                me->CombatStop(true);
                me->RemoveAllAurasExceptType(SPELL_AURA_DUMMY);
                me->SetLootRecipient(NULL);
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BREW_BOLT:
                            if (Unit* unit = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.f, true))
                                DoCast(unit, SPELL_BREW_BOLT);

                            events.ScheduleEvent(EVENT_BREW_BOLT, urand(6000, 14000));
                            break;
                        case EVENT_BLOAT:
                            if (Unit* unit = SelectTarget(SELECT_TARGET_RANDOM, 0, 99.f, true))
                                DoCast(unit, SPELL_BLOAT);

                            events.ScheduleEvent(EVENT_BLOAT, urand(8000,16000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bloated_alementalAI(creature);
        }
};

class npc_stout_alemental : public CreatureScript
{
    public:
        npc_stout_alemental() : CreatureScript("npc_stout_alemental") { }

        enum Spells
        {
            SPELL_BLACKOUT     = 106851,
            SPELL_BREW_BOLT    = 115652
        };

        enum Events
        {
            EVENT_NONE,
            EVENT_BLACKOUT,
            EVENT_BREW_BOLT
        };

        struct npc_stout_alementalAI : public ScriptedAI
        {
            npc_stout_alementalAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_BLACKOUT, 8000);
                events.ScheduleEvent(EVENT_BREW_BOLT, urand(4000, 11000));

                std::list<Creature*> temp;
                GetCreatureListWithEntryInGrid(temp, me, me->GetEntry(), 10.f);

                for (auto&& itr : temp)
                    itr->AI()->DoZoneInCombat();
            }

            void Reset() override { }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->AttackStop();
                me->CombatStop(true);
                me->RemoveAllAurasExceptType(SPELL_AURA_DUMMY);
                me->SetLootRecipient(NULL);
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BREW_BOLT:
                            if (Unit* unit = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.f, true))
                                DoCast(unit, SPELL_BREW_BOLT);

                            events.ScheduleEvent(EVENT_BREW_BOLT, urand(6000, 14000));
                            break;
                        case EVENT_BLACKOUT:
                            DoCast(SPELL_BLACKOUT);
                            events.ScheduleEvent(EVENT_BLACKOUT, urand(6000, 10000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_stout_alementalAI(creature);
        }
};

class npc_bubbling_alemental : public CreatureScript
{
    public:
        npc_bubbling_alemental() : CreatureScript("npc_bubbling_alemental") { }

        enum Spells
        {
            SPELL_FORCE_VEHICLE = 46598,
            SPELL_BUBBLE_SHIELD = 128708,
            SPELL_BREW_BOLT     = 116155
        };

        enum Creatures
        {
            NPC_BUBBLE_SHIELD   = 65522
        };

        enum Events
        {
            EVENT_NONE,
            EVENT_BUBBLE_SHIELD,
            EVENT_MOVE,
        };

        struct npc_bubbling_alementalAI : public ScriptedAI
        {
            npc_bubbling_alementalAI(Creature* creature) : ScriptedAI(creature) { }

            std::vector<ObjectGuid> guidsVector;
            EventMap events, cosmeticEvents;
            uint32 wp;

            void InitializeAI() override
            {
                me->GetMotionMaster()->Clear();

                if (!me->GetDBTableGUIDLow())
                    SetCombatMovement(false);

                cosmeticEvents.ScheduleEvent(EVENT_MOVE, 1500);
                wp = 0;
            }

            void IsSummonedBy(Unit* summoner) override
            {
                cosmeticEvents.Reset();
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_BUBBLE_SHIELD, 5000);
                me->GetMotionMaster()->Clear();
                std::list<Creature*>temp;
                GetCreatureListWithEntryInGrid(temp, me, me->GetEntry(), 10.f);

                for (auto&& itr : temp)
                    itr->AI()->DoZoneInCombat();
            }

            void ApplyBubbleShield(int n)
            {
                for (int i = n; i < 2; i++)
                {
                    if (Creature* bubbleShield = me->SummonCreature(NPC_BUBBLE_SHIELD, me->GetPositionX() + frand(-1.5f, 1.5f), me->GetPositionY() + frand(-0.8f, 0.8f), me->GetPositionZ() + 3.0f, me->GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN))
                    {
                        me->AddAura(SPELL_BUBBLE_SHIELD, me);
                        guidsVector.push_back(bubbleShield->GetGUID());
                    }
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                wp++;

                if (wp > 3) // Reset our path
                    wp = 0;

                cosmeticEvents.ScheduleEvent(EVENT_MOVE, urand(100, 200));
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                for (auto&& guid : guidsVector)
                {
                    if (Creature* bubble = ObjectAccessor::GetCreature(*me, guid))
                    {
                        bubble->ExitVehicle();
                        bubble->AddObjectToRemoveList();
                    }
                }

                guidsVector.clear();

                me->AttackStop();
                me->CombatStop(true);
                me->RemoveAllAurasExceptType(SPELL_AURA_DUMMY);
                me->SetLootRecipient(NULL);

                me->GetMotionMaster()->MoveTargetedHome();
            }

            void JustDied(Unit* killer) override
            {
                for (auto&& guid : guidsVector)
                    if (Creature* bubble = ObjectAccessor::GetCreature(*me, guid))
                        bubble->AddObjectToRemoveList();

                guidsVector.clear();

                ScriptedAI::JustDied(killer);
            }

            void UpdateAI(uint32 diff) override
            {
                cosmeticEvents.Update(diff);

                while (uint32 eventId = cosmeticEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_MOVE)
                    {
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->Clear();

                        Position pos;
                        float x = bublingAlementalPath[wp].GetPositionX() + frand(-2.0f, 2.0f), y = bublingAlementalPath[wp].GetPositionY() + frand(-1.0f, 4.0f);
                        pos.Relocate(x, y, bublingAlementalPath[wp].GetPositionZ(), bublingAlementalPath[wp].GetOrientation());
                        me->GetMotionMaster()->MovePoint(wp, pos);
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BUBBLE_SHIELD:
                            if (me->GetAura(SPELL_BUBBLE_SHIELD))
                                ApplyBubbleShield(0 + me->GetAura(SPELL_BUBBLE_SHIELD)->GetStackAmount());
                            else
                                ApplyBubbleShield(0);

                            events.ScheduleEvent(EVENT_BUBBLE_SHIELD, 12000);
                            break;
                    }
                }

                if (!DoSpellAttackIfReady(SPELL_BREW_BOLT))
                {
                    SetCombatMovement(true);
                    AttackStart(me->GetVictim());
                    DoMeleeAttackIfReady();
                    return;
                }

                SetCombatMovement(false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bubbling_alementalAI(creature);
        }
};

class npc_carbonate_trigger : public CreatureScript
{
    public:
        npc_carbonate_trigger() : CreatureScript("npc_carbonate_trigger") { }

        enum Spells
        {
            SPELL_CARBONATE_AURA  = 116168
        };

        enum Events
        {
            EVENT_NONE,
            EVENT_DESPAWN,
            EVENT_CARBONATE
        };

        struct npc_carbonate_triggerAI : public ScriptedAI
        {
            npc_carbonate_triggerAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            EventMap events;

            void Reset() override
            {
                if (me->HasAura(SPELL_CARBONATE_AURA))
                    return;

                events.ScheduleEvent(EVENT_DESPAWN, 8200);
                events.ScheduleEvent(EVENT_CARBONATE, 100);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DESPAWN:
                            me->AddObjectToRemoveList();
                            break;
                        case EVENT_CARBONATE:
                            DoCast(SPELL_CARBONATE_AURA);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_carbonate_triggerAI(creature);
        }
};

class npc_suds_trigger : public CreatureScript
{
    public:
        npc_suds_trigger() : CreatureScript("npc_suds_trigger") { }

        enum Spells
        {
            SPELL_SUDS_AURA = 116179
        };

        enum Events
        {
            EVENT_NONE,
            EVENT_DESPAWN,
            EVENT_SUDS
        };

        struct npc_suds_triggerAI : public ScriptedAI
        {
            npc_suds_triggerAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            EventMap events;

            void Reset() override
            {
                if (me->HasAura(SPELL_SUDS_AURA))
                    return;

                events.ScheduleEvent(EVENT_DESPAWN, 8200);
                events.ScheduleEvent(EVENT_SUDS, 100);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DESPAWN:
                            me->AddObjectToRemoveList();
                            break;
                        case EVENT_SUDS:
                            me->AddAura(SPELL_SUDS_AURA, me);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_suds_triggerAI(creature);
        }
};

class npc_bubble_shield : public CreatureScript
{
    public:
        npc_bubble_shield() : CreatureScript("npc_bubble_shield") { }

        enum Spells
        {
            SPELL_REMOVE_SHIELD     = 106563,
            SPELL_REMOVE_SHIELD2    = 128708
        };

        struct npc_bubble_shieldAI : public ScriptedAI
        {
            npc_bubble_shieldAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
                SetCombatMovement(false);
            }

            ObjectGuid summonerGUID;
            InstanceScript* instance;

            void IsSummonedBy(Unit* summoner) override
            {
                summonerGUID = summoner->GetGUID();
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    if (instance)
                        if (Unit* alemental = ObjectAccessor::GetUnit(*me, summonerGUID))
                            alemental->RemoveAuraFromStack(alemental->GetEntry() == NPC_YAN_ZHU ? SPELL_REMOVE_SHIELD : SPELL_REMOVE_SHIELD2);

                    me->AddObjectToRemoveList();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bubble_shieldAI(creature);
        }
};

class npc_fizzy_bubble : public CreatureScript
{
    public:
        npc_fizzy_bubble() : CreatureScript("npc_fizzy_bubble") { }

        enum Events
        {
            EVENT_NONE,
            EVENT_DESPAWN
        };

        enum eSpells :uint32
        {
            SPELL_FIZZI_BUBLE_AUR        = 114458,
            SPELL_FIZZI_BUBLE_PLAYER_AUR = 114459,
        };

        struct npc_fizzy_bubbleAI : public ScriptedAI
        {
            npc_fizzy_bubbleAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void InitializeAI() override
            {
                me->CastSpell(me, SPELL_FIZZI_BUBLE_AUR, false);
                me->AddUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY);
                me->SetCanFly(true);
            }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                clicker->AddAura(SPELL_FIZZI_BUBLE_PLAYER_AUR, clicker);

                me->AddObjectToRemoveList();
                return;
            }

            void Reset() override
            {
                events.ScheduleEvent(EVENT_DESPAWN, 20000);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DESPAWN:
                            me->AddObjectToRemoveList();
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_fizzy_bubbleAI(creature);
        }
};

class npc_wall_of_suds : public CreatureScript
{
    public:
        npc_wall_of_suds() : CreatureScript("npc_wall_of_suds") { }

        enum Spells
        {
            SPELL_WALL_OF_SUDS    = 114467,
        };

        enum Events
        {
            EVENT_NONE,
            EVENT_INITIALIZE,
            EVENT_MOVE_NEXT
        };

        struct npc_wall_of_sudsAI : public ScriptedAI
        {
            npc_wall_of_sudsAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                //me->SetDisplayId(11686); have aura like 90 angle, maybe need use apply movement force?
                //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
                me->CastSpell(me, SPELL_WALL_OF_SUDS, false);
                me->SetWalk(true);
                me->SetSpeed(MOVE_WALK, 0.9f);
                MoveNextOrientatedPosition();
            }

            void DoAction(int32 /*actionId*/) override { }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                    me->DespawnOrUnsummon();
            }

            void MoveNextOrientatedPosition()
            {
                float newX, newY;
                GetPositionWithDistInOrientation(me, 90.0f, me->GetOrientation(), newX, newY);

                me->GetMotionMaster()->MovePoint(0, newX, newY, me->GetPositionZ(), me->GetOrientation());

            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_wall_of_sudsAI(creature);
        }
};

class spell_brew_finale : public SpellScriptLoader
{
    public:
        spell_brew_finale() : SpellScriptLoader("spell_brew_finale") { }

        class spell_brew_finale_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_brew_finale_SpellScript);

            void OnBeforeCast()
            {
                if (!GetCaster())
                    return;

                std::list<Creature*>temps;
                GetCreatureListWithEntryInGrid(temps, GetCaster(), NPC_BARREL_TOSS_BUNNY, 50.f);

                temps.remove_if([=](WorldObject* obj) { return obj->GetDistance(GetCaster()) > 15.f && obj->GetDistance(GetCaster()) < 38.f; });

                if (!temps.empty())
                    std::list<Creature*>::iterator itr = temps.begin();
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_brew_finale_SpellScript::OnBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_brew_finale_SpellScript();
        }
};

class spell_wall_of_suds : public SpellScriptLoader
{
    public:
        spell_wall_of_suds() : SpellScriptLoader("spell_wall_of_suds") { }

        class spell_wall_of_suds_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_wall_of_suds_SpellScript);

            void SelectTargets(std::list<WorldObject*>&targets)
            {
                targets.remove_if([=](WorldObject* obj) { return obj->GetGUID() == GetCaster()->GetGUID(); });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_wall_of_suds_SpellScript::SelectTargets, EFFECT_0, 130);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_wall_of_suds_SpellScript::SelectTargets, EFFECT_1, 130);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_wall_of_suds_SpellScript::SelectTargets, EFFECT_2, 130);
            }
        };
};

class spell_blackout_brew : public SpellScriptLoader
{
    public:
        spell_blackout_brew() : SpellScriptLoader("spell_blackout_brew") { }

        enum Spells
        {
            SPELL_BLACKOUT_BREW  = 106851,
            SPELL_BLACKOUT_DRUNK = 106857
        };

        class spell_blackout_brew_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_blackout_brew_AuraScript);

            void HandleEffectPeriodic(const AuraEffect* aurEff)
            {
                if (GetOwner() && GetOwner()->ToUnit())
                {
                    // if jumping or moving, we should take a stack away
                    if (GetOwner()->ToUnit()->isMoving() || GetOwner()->ToUnit()->HasUnitMovementFlag(MOVEMENTFLAG_FALLING))
                    {
                        if (GetStackAmount() - 1 <= 0)
                            Remove(AURA_REMOVE_BY_EXPIRE);
                        else
                            SetStackAmount(GetStackAmount() - 1);
                    }
                    else
                    {
                        if (GetStackAmount() + 1 >= 10)
                        {
                            GetOwner()->ToUnit()->AddAura(SPELL_BLACKOUT_DRUNK, GetOwner()->ToUnit());
                            Remove(AURA_REMOVE_BY_EXPIRE);
                        }
                        else
                            SetStackAmount(GetStackAmount() + 1);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_blackout_brew_AuraScript::HandleEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_blackout_brew_AuraScript();
        }
};

class spell_bloat_aura : public SpellScriptLoader
{
    public:
        spell_bloat_aura() : SpellScriptLoader("spell_bloat_aura") { }

        enum Creatures
        {
            NPC_BLOATED_STALKER      = 59482,
            SPELL_FORCE_VEHICLE      = 46598,
            SPELL_GUSHING_BREW       = 106549
        };

        class spell_bloat_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_bloat_aura_AuraScript);

            void HandleOnApply(const AuraEffect* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetOwner() && GetOwner()->ToUnit())
                {
                    Position pos = GetOwner()->GetPosition();
                    for (int i = 0; i < 2; ++i)
                    {
                        if (Creature* stalker = GetOwner()->SummonCreature(NPC_BLOATED_STALKER, GetOwner()->GetPositionX(), GetOwner()->GetPositionY(), GetOwner()->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000ms))
                        {
                            stalker->CastSpell(GetOwner()->ToUnit(), SPELL_FORCE_VEHICLE, true);
                            stalker->CastSpell(stalker, SPELL_GUSHING_BREW, true);
                        }
                    }
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_bloat_aura_AuraScript::HandleOnApply, EFFECT_1, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_bloat_aura_AuraScript();
        }
};

class spell_carbonation : public SpellScriptLoader
{
    public:
        spell_carbonation() : SpellScriptLoader("spell_carbonation") { }

        class spell_carbonation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_carbonation_SpellScript);

            void SelectTargets(std::list<WorldObject*>&targets)
            {
                targets.remove_if([=](WorldObject* obj) { return obj && (obj->GetPositionZ() > (GetCaster()->GetPositionZ() + 4.9f)); });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_carbonation_SpellScript::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_carbonation_SpellScript::SelectTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_carbonation_SpellScript();
        }
};

// Brew Finale Wheat Effect 128254, 128256, 128258
class spell_brew_finale_wheat_effect : public SpellScriptLoader
{
    public:
        spell_brew_finale_wheat_effect() : SpellScriptLoader("spell_brew_finale_wheat_effect") { }

        class spell_brew_finale_wheat_effect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_brew_finale_wheat_effect_SpellScript);

            void SelectTargets(SpellDestination& dest)
            {
                float x = frand(-718.97f, -687.75f);
                float y = frand(1147.53f, 1178.26f);
                float z = 166.14f;

                Position newPos = { x, y, z, 0 };
                dest.Relocate(newPos);
            }


            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_brew_finale_wheat_effect_SpellScript::SelectTargets, EFFECT_0, TARGET_DEST_DEST_RANDOM);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_brew_finale_wheat_effect_SpellScript();
        }
};

// Fizzi Buble 114459
class spell_fizzi_buble : public SpellScriptLoader
{
    public:
        spell_fizzi_buble() : SpellScriptLoader("spell_fizzi_buble") { }

        class spell_fizzi_buble_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fizzi_buble_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* owner = GetOwner()->ToPlayer())
                    owner->SetCanFly(false);
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* owner = GetOwner()->ToPlayer())
                    owner->SetCanFly(true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_fizzi_buble_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_FLY, AURA_EFFECT_HANDLE_REAL);
                OnEffectApply += AuraEffectApplyFn(spell_fizzi_buble_AuraScript::OnApply, EFFECT_0, SPELL_AURA_FLY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_fizzi_buble_AuraScript();
        }
};

void AddSC_boss_yanzhu()
{
    new npc_uncle_gao();
    new npc_chen_yanzhu();
    new boss_yanzhu();
    new npc_bubble_shield();
    new npc_yeasty_alemental();
    new npc_sudsy_alemental();
    new npc_fizzy_alemental();
    new npc_bloated_alemental();
    new npc_stout_alemental();
    new npc_bubbling_alemental();
    new npc_carbonate_trigger();
    new npc_suds_trigger();
    new npc_fizzy_bubble();
    new npc_wall_of_suds();
    new spell_blackout_brew();
    new spell_bloat_aura();
    new spell_carbonation();
    new spell_brew_finale_wheat_effect();
    new spell_fizzi_buble();
}