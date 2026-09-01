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

enum Spells
{
    SPELL_HOPPER_ANIM_REPLACEMENT       = 114284,
    SPELL_HAMMER_COSMETIC_1             = 114530,
    SPELL_SUMMON_HAMMER                 = 114531,
    SPELL_SUMMON_HAMMER2                = 114532,
    SPELL_HAMMER_VISUAL                 = 114533,
    SPELL_BLINKING_COSMETIC_AURA        = 114287,
    SPELL_LIT_FUSE                      = 114290,

    SPELL_EXPLOSIVE_BREW                = 114291,

    SPELL_SMASH_AURA                    = 111662,
    SPELL_SMASH                         = 111666,

    SPELL_COSMETIC_EARTHQUAKE           = 114928,
    SPELL_HOPTALLUS_INTRO_SOUND         = 106790,
    SPELL_HOPTALLUS_KEG_SCENE           = 114366,
    SPELL_HOPTALLUS_SCREECH             = 114367,

    SPELL_FURLWIND                      = 112992,
    SPELL_CARROT_BREATH                 = 112944,
    SPELL_SUMMON_BREATH_STALKER         = 112956,

    SPELL_CARROT_DOOR                   = 116005,

    // Achievement
    SPELL_SMASH_ACHIEVEMENT             = 116286,
    SPELL_CLINGER_ACHIEVEMENT           = 116271,
};

enum Misc
{
    NPC_CARROT_BREATH_STALKER           = 59018
};

enum hopEvents
{
    EVENT_MOVE = 1,
    EVENT_JUMP_TO_WATER = 2,
    EVENT_CHECK_POS,
};

static const Position hoptallusHopPos = { -698.62f, 1259.4f, 162.79f, 0.24f };
static const Position hoplingSpawns[] =
{
    { -719.31f, 1248.27f, 166.8f, 0.24f },
    { -719.95f, 1250.89f, 166.8f, 0.24f },
    { -720.66f, 1253.79f, 166.8f, 0.24f },
    { -721.25f, 1256.18f, 166.8f, 0.24f },
    { -721.94f, 1259.02f, 166.8f, 0.24f }
};

static const Position hopperSpawns[] =
{
    { -726.7f, 1257.9f, 166.8f, 0.24f },
    { -720.86f, 1253.44f, 166.8f, 0.24f },
    { -726.86f, 1247.41f, 166.8f, 0.25f },
    { -719.38f, 1255.37f, 166.8f, 0.25f}
};

static const Position nibblerWaypints[] =
{
    { -704.96f, 1292.42f, 162.7f, 4.95f },
    { -698.4f, 1267.73f, 162.7f, 4.97f },
    { -690.44f, 1254.47f, 162.8f, 5.32f },
    { -683.54f, 1252.15f, 162.82f, 6.f }, // pick up here
    { -663.1f, 1256.7f, 154.8f, 5.86f },
    { -653.1f, 1242.9f, 154.8f, 4.91f },
    { -639.69f, 1192.53f, 139.15f, 0.28f }, // jump
    { -601.1f, 1201.59f, 138.5f, 4.85f },
    { -598.2f, 1188.f, 138.65f, 4.7f },
    { -598.4f, 1151.8f, 138.3f, 4.86f },
    { -592.1f, 1127.8f, 138.5f, 4.98f },
    { -566.1f, 1102.2f, 142.1f, 5.44f },
};

static const uint32 hoplings[5] = { 59461, 56631, 59459, 59458,  59460 };
static const uint32 hoppers[2] = { 56718, 59426 };

void InitPathForHopplings(Creature* me, ObjectGuid hoppGUID)
{
    Unit* hoppler = ObjectAccessor::GetUnit(*me, hoppGUID);
    if (!hoppler)
        return;

    hoppler->GetMotionMaster()->MovementExpired();
    hoppler->GetMotionMaster()->Clear();

    Movement::MoveSplineInit init(hoppler);
    for (auto&& itr : hoplingPath)
    {
        float x, y, z;
        hoppler->GetRandomPoint(itr, 3.0f, x, y, z);
        init.Path().push_back(G3D::Vector3(x, y, z));
    }

    init.SetSmooth();
    init.SetUncompressed();
    init.Launch();
}

class npc_nibbler : public CreatureScript
{
    public:
        npc_nibbler() : CreatureScript("npc_nibbler") { }

        enum Events
        {
            EVENT_NONE,
            EVENT_CHECK,
            EVENT_MOVE,
            EVENT_JUMP,
            EVENT_TALKPANDA
        };

        enum Spells
        {
            SPELL_RIDE_VEHICLE      = 93970
        };

        struct npc_nibblerAI : public ScriptedAI
        {
            npc_nibblerAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 waypoint;
            uint32 pandaPoint;
            std::vector<ObjectGuid> pandaGuidsVector;
            std::vector<ObjectGuid>::const_iterator pandaItr;
            EventMap events;
            InstanceScript* instance;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetVisible(false);
                Reset();
            }

            void InitRun()
            {
                me->SetVisible(true);
                events.ScheduleEvent(EVENT_MOVE, 100);
                waypoint = 0;
            }

            void PrepPandas()
            {
                pandaPoint = 0;

                std::list<Creature*> temp;
                GetCreatureListWithEntryInGrid(temp, me, 59075, 20.f);

                for (auto&& itr : temp)
                    pandaGuidsVector.push_back(itr->GetGUID());

                events.ScheduleEvent(EVENT_TALKPANDA, 8000);
                pandaItr = pandaGuidsVector.begin();
            }

            void Cleanup()
            {
                Creature* bunny = GetClosestCreatureWithEntry(me, 45979, 10.f);
                if (bunny)
                    bunny->RemoveAurasDueToSpell(SPELL_CARROT_DOOR);
                me->SetVisible(false);
            }

            void DoAction(int32 actionId) override
            {
                events.ScheduleEvent(EVENT_CHECK, 500);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                switch (pointId)
                {
                    case 0:
                    case 1:
                    case 2:
                    case 4:
                    case 5:
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                        events.ScheduleEvent(EVENT_MOVE, 100);
                        break;
                    case 6:
                        events.ScheduleEvent(EVENT_JUMP, 100);
                        PrepPandas();
                        break;
                    case 11:
                        Cleanup();
                        break;
                    case 3:
                        if (Creature* bunny = GetClosestCreatureWithEntry(me, 45979, 10.f))
                            bunny->CastSpell(me, SPELL_RIDE_VEHICLE, true);

                        if (GameObject* go = GetClosestGameObjectWithEntry(me, GO_INVIS_DOOR, 10.f))
                            go->AddObjectToRemoveList();

                        events.ScheduleEvent(EVENT_MOVE, 100);
                        break;
                }

                waypoint++;
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
                
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK:
                            if (instance && instance->GetBossState(DATA_HOPTALLUS) == DONE)
                                InitRun();
                            break;
                        case EVENT_MOVE:
                            me->GetMotionMaster()->MovePoint(waypoint, nibblerWaypints[waypoint]);
                            break;
                        case EVENT_JUMP:
                            me->GetMotionMaster()->MoveJump(nibblerWaypints[waypoint].GetPositionX(), nibblerWaypints[waypoint].GetPositionY(), nibblerWaypints[waypoint].GetPositionZ(), 15.f, 15.f, waypoint);
                            break;
                        case EVENT_TALKPANDA:
                            if (pandaItr == pandaGuidsVector.end())
                                break;
                            else
                            {
                                events.ScheduleEvent(EVENT_TALKPANDA, 9500);

                                if (Creature* panda = ObjectAccessor::GetCreature(*me, *pandaItr))
                                {
                                    if (panda->AI())
                                        panda->AI()->Talk(pandaPoint);

                                    pandaPoint++;
                                }
                                pandaItr++;
                            }
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_nibblerAI(creature);
        }
};

class boss_hoptallus : public CreatureScript
{
    public:
        boss_hoptallus() : CreatureScript("boss_hoptallus") { }

        enum Events
        {
            EVENT_NONE,
            EVENT_FURLWIND,
            EVENT_CARROT_BREATH,
            EVENT_CARROT_STALKER,
            EVENT_SCREECH,
            EVENT_ENTERCOMBAT,
            EVENT_SUMMON_HOPLINGS,
            EVENT_SUMMON_HOPPERS
        };

        enum Talks
        {
            TALK_AGGRO              = 0, // Oh yeah!
            TALK_FURLWIND           = 1, // Gonna spins around!
            TALK_CARROT_BREATH      = 2, // Urp... eats too many carrots...
            TALK_DEATH              = 3, // You have... turnip... for a head...
            TALK_SCREECH            = 4, // Hoptallus lets out a loud screech! The virmen are coming!
            TALK_SLAY               = 5  // You die!
        };

        struct boss_hoptallusAI : public BossAI
        {
            boss_hoptallusAI(Creature* creature) : BossAI(creature, DATA_HOPTALLUS) { }

            bool introStarted;
            uint32 hoplingWaves;
            uint32 hopperWaves;

            void InitializeAI() override
            {
                if (Creature* bunny = GetClosestCreatureWithEntry(me, 45979, 45.f))
                    bunny->AddAura(SPELL_CARROT_DOOR, bunny);

                me->setActive(true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);

                introStarted = instance->GetBossState(DATA_HOPTALLUS) == SPECIAL ? true : false;

                if (instance->GetBossState(DATA_HOPTALLUS) == SPECIAL && me->IsAlive())
                    InitJump();
            }

            void Reset() override
            {
                _Reset();
            }

            void InitJump()
            {
                me->SetReactState(REACT_PASSIVE);

                introStarted = true;

                if (GameObject* keg = GetClosestGameObjectWithEntry(me, GO_SHAKING_KEG, 50.0f))
                    keg->UseDoorOrButton();

                if (instance)
                    instance->SetBossState(DATA_HOPTALLUS, SPECIAL); // we need cancel event before join in combat

                DoCast(SPELL_HOPTALLUS_KEG_SCENE);
                DoCast(SPELL_HOPTALLUS_INTRO_SOUND);
                DoCast(SPELL_COSMETIC_EARTHQUAKE);

                me->GetMotionMaster()->MoveJump(hoptallusHopPos.m_positionX, hoptallusHopPos.m_positionY, hoptallusHopPos.m_positionZ, 10.f, 10.f, 1);
                me->SetHomePosition(hoptallusHopPos);

                events.ScheduleEvent(EVENT_ENTERCOMBAT, 8000);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == 1)
                    me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!introStarted && who) // Small hack to make sure boss isn't triggered from behind
                    if (who->GetTypeId() == TYPEID_PLAYER && who->GetDistance(me) < 20.f && who->GetPositionX() > -705.f)
                        InitJump();

                ScriptedAI::MoveInLineOfSight(who);
            }

            bool DoSummonHoplings(int n)
            {
                if (n < 0)
                {
                    if (instance->GetBossState(DATA_HOPTALLUS) == SPECIAL)
                    {
                        hoplingWaves++;

                        if (hoplingWaves < 3)
                            events.ScheduleEvent(EVENT_SUMMON_HOPLINGS, 12 * IN_MILLISECONDS);
                        else
                            hoplingWaves = 0;
                    }

                    return false;
                }

                if (Creature* creature = me->SummonCreature(hoplings[n], hoplingSpawns[n], TEMPSUMMON_CORPSE_TIMED_DESPAWN, Milliseconds(urand(2000, 5000))))
                {
                    creature->AddAura(SPELL_HOPPER_ANIM_REPLACEMENT, creature);

                    Position pos = me->GetRandomNearPosition(10.f);
                    creature->GetMotionMaster()->MoveJump(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 15.f, 15.f, EVENT_JUMP);

                    if (creature->AI() && instance->GetBossState(DATA_HOPTALLUS) == SPECIAL) // not send area in combat while pre event
                        creature->AI()->DoZoneInCombat();
                }

                return DoSummonHoplings(n - 1);
            }

            bool DoSummonHoppers(int n)
            {
                if (n < 0)
                {
                    if (instance->GetBossState(DATA_HOPTALLUS) == SPECIAL)
                    {
                        hopperWaves++;

                        if (hopperWaves < 2)
                            events.ScheduleEvent(EVENT_SUMMON_HOPPERS, 16 * IN_MILLISECONDS);
                        else
                            hopperWaves = 0;
                    }

                    return false;
                }

                if (Creature* creature = me->SummonCreature(hoppers[urand(0, 1)], hopperSpawns[n], TEMPSUMMON_CORPSE_TIMED_DESPAWN, Milliseconds(urand(4000, 8000))))
                {
                    Position pos = me->GetRandomNearPosition(10.f);
                    creature->GetMotionMaster()->MoveJump(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 10.f, 10.f, EVENT_JUMP);

                    if (creature->AI() && instance->GetBossState(DATA_HOPTALLUS) == SPECIAL)
                        creature->AI()->DoZoneInCombat();
                }

                return DoSummonHoppers(n - 1);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                _EnterEvadeMode();

                summons.DespawnAll();
                events.Reset();

                me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                me->AttackStop();
                me->GetMotionMaster()->MovePoint(194, hoptallusHopPos);
            }

            void KilledUnit(Unit* victim) override
            {
                Talk(TALK_SLAY);
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();

                Talk(TALK_DEATH);

                if (Creature* nibbler = GetClosestCreatureWithEntry(me, NPC_NIBBLER, 100.0f, true))
                    nibbler->AI()->DoAction(0);
            }

            void JustEngagedWith(Unit* who) override
            {
                _JustEngagedWith();

                Talk(TALK_AGGRO);

                events.ScheduleEvent(EVENT_SCREECH, 30000);
                events.ScheduleEvent(EVENT_FURLWIND, urand(10000, 16000));
                events.ScheduleEvent(EVENT_CARROT_BREATH, urand(15000, 30000));

                hoplingWaves = 0;
                hopperWaves = 0;
            }

            void DoAction(int32 actionId) override
            {
                DoSummonHoplings(urand(3, 4));
                DoSummonHoppers(urand(1, 2));
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon)
                    summons.Summon(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                if (me->HasAura(SPELL_FURLWIND) || me->HasAura(SPELL_CARROT_BREATH) || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ENTERCOMBAT:
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                            me->SetReactState(REACT_AGGRESSIVE);
                            DoZoneInCombat();
                            break;
                        case EVENT_FURLWIND:
                            DoCast(me, SPELL_FURLWIND);
                            Talk(TALK_FURLWIND);
                            events.ScheduleEvent(EVENT_FURLWIND, urand(40000, 50000));
                            break;
                        case EVENT_CARROT_BREATH:
                            DoCast(SPELL_SUMMON_BREATH_STALKER);
                            DoCast(SPELL_CARROT_BREATH);
                            Talk(TALK_CARROT_BREATH);
                            events.ScheduleEvent(EVENT_CARROT_BREATH, urand(25000, 27000));
                            break;
                        case EVENT_SCREECH:
                            events.ScheduleEvent(EVENT_SCREECH, 30000);
                            events.ScheduleEvent(EVENT_SUMMON_HOPLINGS, 2000);
                            events.ScheduleEvent(EVENT_SUMMON_HOPPERS, 4000);
                            DoCast(SPELL_HOPTALLUS_SCREECH);
                            Talk(TALK_SCREECH);
                            break;
                        case EVENT_SUMMON_HOPLINGS:
                            DoSummonHoplings(4);
                            break;
                        case EVENT_SUMMON_HOPPERS:
                            DoSummonHoppers(3);
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_hoptallusAI(creature);
        }
};

class npc_hammer_bopper : public CreatureScript
{
    public:
        npc_hammer_bopper() : CreatureScript("npc_hammer_bopper") { }

        struct npc_hammer_bopperAI : public ScriptedAI
        {
            npc_hammer_bopperAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;

            void IsSummonedBy(Unit* summoner) override
            {
                instance = me->GetInstanceScript();

                me->AddAura(SPELL_HAMMER_COSMETIC_1, me);
                me->AddAura(SPELL_HOPPER_ANIM_REPLACEMENT, me);
                events.ScheduleEvent(EVENT_CHECK_POS, 5 * IN_MILLISECONDS);
            }
            
            void JustDied(Unit* killer) override
            {
                DoCast(me, SPELL_SUMMON_HAMMER, true);
                me->RemoveAurasDueToSpell(SPELL_HAMMER_COSMETIC_1);
            }

            void JustEngagedWith(Unit* who) override
            {
                me->GetMotionMaster()->Clear();
                events.Reset();
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->DespawnOrUnsummon();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                switch (pointId)
                {
                    case EVENT_JUMP:
                        if (instance && (instance->GetBossState(DATA_HOPTALLUS) != SPECIAL || instance->GetBossState(DATA_HOPTALLUS) != IN_PROGRESS))
                        {
                            InitPathForHopplings(me, me->GetGUID());
                            events.ScheduleEvent(EVENT_MOVE, me->GetSplineDuration());
                        }
                        break;
                    case EVENT_JUMP + 1:
                        me->DespawnOrUnsummon(1000);
                        break;
                }

                if (type != POINT_MOTION_TYPE)
                    return;
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE:
                        {
                            me->GetMotionMaster()->MoveRandom(5.0f);
                            events.ScheduleEvent(EVENT_JUMP_TO_WATER, urand(3 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                            break;
                        }
                        case EVENT_JUMP_TO_WATER:
                        {
                            float oriJump = frand(1.94f, 4.38f);
                            float x, y;
                            GetPositionWithDistInOrientation(me, frand(80.0f, 95.0f), oriJump, x, y);
                            me->GetMotionMaster()->MoveJump(x, y, 134.28f, 15.0f, 9.0f, EVENT_JUMP + 1);
                            break;
                        }
                        case EVENT_CHECK_POS:
                        {
                            if (me->GetPositionZ() > 164.0f)
                                me->DespawnOrUnsummon();
                            break;
                        }
                    }
                }

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_hammer_bopperAI(creature);
        }
};

class npc_explosive_hopper : public CreatureScript
{
    public:
        npc_explosive_hopper() : CreatureScript("npc_explosive_hopper") { }

        enum Events
        {
            EVENT_NONE,
            EVENT_EXPLODE,
        };

        struct npc_explosive_hopperAI : public ScriptedAI
        {
            npc_explosive_hopperAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events, cosmeticEvents;
            InstanceScript* instance;

            void IsSummonedBy(Unit* summoner) override
            {
                instance = me->GetInstanceScript();
                me->AddAura(SPELL_HOPPER_ANIM_REPLACEMENT, me);
                cosmeticEvents.ScheduleEvent(EVENT_CHECK_POS, 5 * IN_MILLISECONDS);
            }

            void JustEngagedWith(Unit* who) override
            {
                me->GetMotionMaster()->Clear();
                cosmeticEvents.Reset();

                events.ScheduleEvent(EVENT_EXPLODE, urand(3000, 5000));
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->DespawnOrUnsummon();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                switch (pointId)
                {
                    case EVENT_JUMP:
                        if (instance && (instance->GetBossState(DATA_HOPTALLUS) != SPECIAL || instance->GetBossState(DATA_HOPTALLUS) != IN_PROGRESS))
                        {
                            InitPathForHopplings(me, me->GetGUID());
                            cosmeticEvents.ScheduleEvent(EVENT_MOVE, me->GetSplineDuration());
                        }
                        break;
                    case EVENT_JUMP + 1:
                        me->DespawnOrUnsummon(1000);
                        break;
                }

                if (type != POINT_MOTION_TYPE)
                    return;
            }

            void Reset() override { }

            void UpdateAI(uint32 diff) override
            {
                cosmeticEvents.Update(diff);

                while (uint32 eventId = cosmeticEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE:
                        {
                            me->GetMotionMaster()->MoveRandom(5.0f);
                            cosmeticEvents.ScheduleEvent(EVENT_JUMP_TO_WATER, urand(3 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                            break;
                        }
                        case EVENT_JUMP_TO_WATER:
                        {
                            float oriJump = frand(1.94f, 4.38f);
                            float x, y;
                            GetPositionWithDistInOrientation(me, frand(80.0f, 95.0f), oriJump, x, y);
                            me->GetMotionMaster()->MoveJump(x, y, 134.28f, 15.0f, 9.0f, EVENT_JUMP + 1);
                            break;
                        }
                        case EVENT_CHECK_POS:
                        {
                            if (me->GetPositionZ() > 164.0f)
                                me->DespawnOrUnsummon();
                            break;
                        }
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_EXPLODE:
                            DoCast(me, SPELL_LIT_FUSE, true);
                            DoCast(me, SPELL_BLINKING_COSMETIC_AURA, true);
                            DoCast(SPELL_EXPLOSIVE_BREW);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_explosive_hopperAI(creature);
        }
};

class npc_hammer : public CreatureScript
{
    public:
        npc_hammer() : CreatureScript("npc_hammer") { }

        struct npc_hammerAI : public ScriptedAI
        {
            npc_hammerAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->AddAura(SPELL_HAMMER_VISUAL, me);
            }

            void OnSpellClick(Unit* clicker, bool& result) override
            {
                clicker->AddAura(SPELL_SMASH_AURA, clicker);

                me->AddObjectToRemoveList();
                return;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_hammerAI(creature);
        }
};

class npc_carrot_breath_stalker : public CreatureScript
{
    public:
        npc_carrot_breath_stalker() : CreatureScript("npc_carrot_breath_stalker") { }

        enum Events
        {
            EVENT_NONE,
            EVENT_MOVE
        };

        struct npc_carrot_breath_stalkerAI : public ScriptedAI
        {
            npc_carrot_breath_stalkerAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            bool counterClockwise;
            float x, y;
            float arcPoint;
            EventMap events;
            InstanceScript* instance;

            void IsSummonedBy(Unit* summoner) override
            {
                me->GetMotionMaster()->Clear();

                events.ScheduleEvent(EVENT_MOVE, 1500);
                x = me->GetPositionX();
                y = me->GetPositionY();
                arcPoint = 0.0f;

                counterClockwise = urand(0, 1);

                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                if (Creature* hoppy = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_HOPTALLUS)))
                {
                    hoppy->SetGuidValue(UNIT_FIELD_CHANNEL_OBJECT, me->GetGUID());
                    hoppy->SetTarget(me->GetGUID());
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                    events.ScheduleEvent(EVENT_MOVE, 100);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE:
                        {
                            float newX = x + 10.0f * cos(arcPoint * M_PI / 16);
                            float newY = counterClockwise ? y - 10.0f * sin(arcPoint * M_PI / 16) : y + 10.0f * sin(arcPoint * M_PI / 16);
                            me->GetMotionMaster()->MovePoint(0, newX, newY, 162.784f);

                            arcPoint++;

                            if (Creature* hoppy = ObjectAccessor::GetCreature(*me, me->GetInstanceScript()->GetGuidData(DATA_HOPTALLUS)))
                            {
                                hoppy->SetGuidValue(UNIT_FIELD_CHANNEL_OBJECT, me->GetGUID());
                                hoppy->SetTarget(me->GetGUID());
                                hoppy->UpdateOrientation(hoppy->GetAngle(me));
                            }
                            break;
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_carrot_breath_stalkerAI(creature);
        }
};

// Hoplings 59461,56631,59458,59459
class npc_sb_hopling : public CreatureScript
{
    public:
        npc_sb_hopling() : CreatureScript("npc_sb_hopling") { }

        struct npc_sb_hoplingAI : public ScriptedAI
        {
            npc_sb_hoplingAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;

            void IsSummonedBy(Unit* summoner) override
            {
                instance = me->GetInstanceScript();
                events.ScheduleEvent(EVENT_CHECK_POS, 5 * IN_MILLISECONDS);
            }

            void JustEngagedWith(Unit* /*who*/) override 
            {
                me->GetMotionMaster()->Clear();
                events.Reset();
            }

            void Move()
            {
                InitPathForHopplings(me, me->GetGUID());
                events.ScheduleEvent(EVENT_MOVE, me->GetSplineDuration());
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                switch (pointId)
                {
                    case EVENT_JUMP:
                        if (instance && (instance->GetBossState(DATA_HOPTALLUS) != SPECIAL || instance->GetBossState(DATA_HOPTALLUS) != IN_PROGRESS))
                            Move();
                        break;
                    case EVENT_JUMP + 1:
                        me->DespawnOrUnsummon(1000);
                        break;
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->DespawnOrUnsummon();
            }

            void JustDied(Unit* killer) override
            {
                ScriptedAI::JustDied(killer);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE:
                        {
                            me->GetMotionMaster()->MoveRandom(3.0f);
                            events.ScheduleEvent(EVENT_JUMP_TO_WATER, urand(3 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                            break;
                        }
                        case EVENT_JUMP_TO_WATER:
                        {
                            float oriJump = frand(1.94f, 4.38f);
                            float x, y;
                            GetPositionWithDistInOrientation(me, frand(80.0f, 95.0f), oriJump, x, y);
                            me->GetMotionMaster()->MoveJump(x, y, 134.28f, 15.0f, 9.0f, EVENT_JUMP + 1);
                            break;
                        }
                        case EVENT_CHECK_POS:
                        {
                            if (me->GetPositionZ() > 164.0f)
                                me->DespawnOrUnsummon();
                            break;
                        }
                    }
                }

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sb_hoplingAI(creature);
        }
};

class spell_hammer_smash : public SpellScriptLoader
{
    public:
        spell_hammer_smash() : SpellScriptLoader("spell_hammer_smash") { }

        class spell_hammer_smash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hammer_smash_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SMASH))
                    return false;
                return true;
            }

            void SelectTargets(std::list<WorldObject*>&targets)
            {
                targets.remove_if([=](WorldObject* obj) { return obj && obj->ToUnit() && !obj->ToUnit()->HasAura(SPELL_HOPPER_ANIM_REPLACEMENT); });

                if (Unit* caster = GetCaster())
                    if (targets.size() >= 100)
                        caster->CastSpell(caster, SPELL_SMASH_ACHIEVEMENT, false);

            }

            void HandleSpellEffectHit(SpellEffIndex idx)
            {
                if (Unit* unit = GetHitUnit())
                    if (unit->IsAlive() && unit->GetEntry() != NPC_EXPLOSIVE_HOPPER)
                        unit->Kill(unit);
            }

            void HandleScriptEffectHit(SpellEffIndex idx)
            {
                GetCaster()->RemoveAuraFromStack(GetSpellInfo()->Effects[idx].BasePoints);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_hammer_smash_SpellScript::HandleSpellEffectHit, EFFECT_0, SPELL_EFFECT_KNOCK_BACK);
                OnEffectHitTarget += SpellEffectFn(spell_hammer_smash_SpellScript::HandleSpellEffectHit, EFFECT_1, SPELL_EFFECT_KNOCK_BACK);
                OnEffectHit       += SpellEffectFn(spell_hammer_smash_SpellScript::HandleScriptEffectHit, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hammer_smash_SpellScript::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hammer_smash_SpellScript::SelectTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }

        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_hammer_smash_SpellScript();
        }
};

class spell_carrot_breath : public SpellScriptLoader
{
    public:
        spell_carrot_breath() : SpellScriptLoader("spell_carrot_breath") { }

        class spell_carrot_breath_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_carrot_breath_AuraScript);

            void HandleOnRemove(const AuraEffect* aurEff, AuraEffectHandleModes mode)
            {
                if (Creature* creature = GetOwner()->ToCreature())
                {
                    creature->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                    creature->SetReactState(REACT_AGGRESSIVE);

                    if (Creature* stalker = GetClosestCreatureWithEntry(creature, NPC_CARROT_BREATH_STALKER, 40.f))
                        stalker->AddObjectToRemoveList();
                }
            }

            void HandleOnApply(const AuraEffect* aurEff, AuraEffectHandleModes mode)
            {
                if (Creature* creature = GetOwner()->ToCreature())
                {
                    creature->AddUnitState(UNIT_STATE_CANNOT_TURN);
                    creature->SetReactState(REACT_PASSIVE);
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_carrot_breath_AuraScript::HandleOnRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                OnEffectApply += AuraEffectApplyFn(spell_carrot_breath_AuraScript::HandleOnApply, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_carrot_breath_AuraScript();
        }
};

// Furlwind 112992
class spell_furlwind : public SpellScriptLoader
{
    public:
        spell_furlwind() : SpellScriptLoader("spell_furlwind") { }

        class spell_furlwind_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_furlwind_AuraScript);

            void HandleOnApply(const AuraEffect* aurEff, AuraEffectHandleModes mode)
            {
                if (Creature* creature = GetOwner()->ToCreature())
                    creature->ClearUnitState(UNIT_STATE_CASTING);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_furlwind_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_furlwind_AuraScript();
        }
};

// Explosive Brew 114291
class spell_sb_explosive_brew : public SpellScriptLoader
{
    public:
        spell_sb_explosive_brew() : SpellScriptLoader("spell_sb_explosive_brew") { }

        class spell_sb_explosive_brew_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sb_explosive_brew_SpellScript);

            void HandleSpellEffectHit(SpellEffIndex idx)
            {
                if (Unit* unit = GetHitUnit())
                    if (unit->GetEntry() == NPC_CLINGER_HOZEN)
                        unit->CastSpell(unit, SPELL_CLINGER_ACHIEVEMENT, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sb_explosive_brew_SpellScript::HandleSpellEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sb_explosive_brew_SpellScript();
        }
};

void AddSC_boss_hoptallus()
{
    new boss_hoptallus();
    new npc_explosive_hopper();
    new npc_hammer_bopper();
    new npc_hammer();
    new npc_carrot_breath_stalker();
    new npc_nibbler();
    new npc_sb_hopling();
    new spell_hammer_smash();
    new spell_carrot_breath();
    new spell_furlwind();
    new spell_sb_explosive_brew();
}