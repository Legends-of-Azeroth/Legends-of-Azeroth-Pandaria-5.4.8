/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
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

#include "ScriptPCH.h"
#include "Spell.h"
#include "throne_of_the_tides.h"

enum ScriptTexts
{
    SAY_AGGRO   = 0,
    SAY_KILL    = 1,
    SAY_DEATH   = 2,
    SAY_66      = 3,
    SAY_33      = 4,
};

enum Spells
{
    SPELL_FUNGAL_SPORES         = 76001,
    SPELL_FUNGAL_SPORES_DMG     = 80564,
    SPELL_SHOCK_BLAST           = 76008,
    SPELL_SUMMON_GEYSER         = 75722,
    SPELL_GEYSER_VISUAL         = 75699,
    SPELL_GEYSER_ERRUPT         = 75700,
    SPELL_GEYSER_ERRUPT_ENTRY   = 94046,
    SPELL_WATERSPOUT            = 75683,
    SPELL_WATERSPOUT_KNOCK      = 75690,
    SPELL_WATERSPOUT_SUMMON     = 90495,
    SPELL_WATERSPOUT_VISUAL     = 90440,
    SPELL_WATERSPOUT_DMG        = 90479,
    SPELL_VISUAL_INFIGHT_AURA   = 91349,

    SPELL_ACHIEVEMENT           = 94042,

    // honnor guard
    SPELL_ARC_SLASH             = 75907,
    SPELL_ENRAGE                = 75998,

    // tempest witch
    SPELL_CHAIN_LIGHTNING       = 75813,
    SPELL_LIGHTNING_SURGE       = 75992,
    SPELL_LIGHTNING_SURGE_DMG   = 75993,
};

enum Events
{
    EVENT_GEYSER            = 1,
    EVENT_GEYSER_ERRUPT     = 2,
    EVENT_FUNGAL_SPORES     = 3,
    EVENT_SHOCK_BLAST       = 4,
    EVENT_WATERSPOUT_END    = 5,
    EVENT_START_ATTACK      = 6,
    EVENT_ARC_SLASH         = 7,
    EVENT_LIGHTNING_SURGE   = 8,
    EVENT_CHAIN_LIGHTNING   = 9,
};

enum Points
{
    POINT_CENTER_1      = 1,
    POINT_CENTER_2      = 2,
    POINT_WATERSPOUT    = 3,
};

enum Adds
{
    NPC_TEMPEST_WITCH       = 44404,
    NPC_HONNOR_GUARD        = 40633,
    NPC_WATERSPOUT          = 48571,
    NPC_WATERSPOUT_H        = 49108,
    NPC_GEYSER              = 40597,
};

const Position summonPos[3] =
{
    {174.41f, 802.323f, 808.368f, 0.014f},
    {200.517f, 787.687f, 808.368f, 2.056f},
    {200.558f, 817.046f, 808.368f, 4.141f}
};

const Position centerPos = {192.05f, 802.52f, 807.64f, 3.14f};

class boss_lady_nazjar : public CreatureScript
{
    public:
        boss_lady_nazjar() : CreatureScript("boss_lady_nazjar") { }

        struct boss_lady_nazjarAI : public BossAI
        {
            boss_lady_nazjarAI(Creature* creature) : BossAI(creature, DATA_LADY_NAZJAR)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }

            uint8 uiSpawnCount;
            uint8 uiPhase;

            void Reset() override
            {
                _Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                uiPhase = 0;
                uiSpawnCount = 3;
                me->SetReactState(REACT_AGGRESSIVE);
                events.Reset();
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
            {
                switch (summon->GetEntry())
                {
                    case NPC_TEMPEST_WITCH:
                    case NPC_HONNOR_GUARD:
                        uiSpawnCount--;
                        break;
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                    if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_SHOCK_BLAST)
                        for (uint8 i = 0; i < 3; ++i)
                            if (spell->Effects[i].Effect == SPELL_EFFECT_INTERRUPT_CAST)
                                me->InterruptSpell(CURRENT_GENERIC_SPELL);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                switch (summon->GetEntry())
                {
                    case NPC_TEMPEST_WITCH:
                    case NPC_HONNOR_GUARD:
                        if (me->IsInCombat())
                            summon->SetInCombatWithZone();
                        break;
                    case NPC_WATERSPOUT:
                    case NPC_WATERSPOUT_H:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        {
                            float x, y;
                            me->GetNearPoint2D(nullptr, x, y, 30.0f, me->GetAngle(target->GetPositionX(), target->GetPositionY()));
                            summon->GetMotionMaster()->MovePoint(POINT_WATERSPOUT, x, y, 808.0f);
                        }
                        break;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_GEYSER, 11000);
                events.ScheduleEvent(EVENT_FUNGAL_SPORES, urand(3000,10000));
                events.ScheduleEvent(EVENT_SHOCK_BLAST, urand(6000,12000));
                instance->SetData(DATA_LADY_NAZJAR, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                Talk(SAY_DEATH);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE)
                {
                    if (pointId == POINT_CENTER_1)
                    {
                        Talk(SAY_66);
                        SetCombatMovement(false);
                        if (IsHeroic())
                            DoCast(me, SPELL_WATERSPOUT_SUMMON, true);
                        DoCast(me, SPELL_WATERSPOUT);
                        me->SummonCreature(NPC_HONNOR_GUARD, summonPos[0]);
                        me->SummonCreature(NPC_TEMPEST_WITCH, summonPos[1]);
                        me->SummonCreature(NPC_TEMPEST_WITCH, summonPos[2]);
                        events.ScheduleEvent(EVENT_WATERSPOUT_END, 60000);
                    }
                    else if (pointId == POINT_CENTER_2)
                    {
                        Talk(SAY_33);
                        SetCombatMovement(false);
                        if (IsHeroic())
                            DoCast(me, SPELL_WATERSPOUT_SUMMON, true);
                        DoCast(me, SPELL_WATERSPOUT);
                        me->SummonCreature(NPC_HONNOR_GUARD, summonPos[0]);
                        me->SummonCreature(NPC_TEMPEST_WITCH, summonPos[1]);
                        me->SummonCreature(NPC_TEMPEST_WITCH, summonPos[2]);
                        events.ScheduleEvent(EVENT_WATERSPOUT_END, 60000);
                    }
                }
            }

            void WaterspoutEnd()
            {
                uiPhase++;
                events.CancelEvent(EVENT_WATERSPOUT_END);
                me->RemoveAurasDueToSpell(SPELL_WATERSPOUT_SUMMON);
                me->InterruptNonMeleeSpells(false);
                me->CastStop();
                SetCombatMovement(true);
                me->SetReactState(REACT_AGGRESSIVE);
                me->GetMotionMaster()->MoveChase(me->GetVictim());
                events.RescheduleEvent(EVENT_GEYSER, 11000);
                events.RescheduleEvent(EVENT_FUNGAL_SPORES, urand(3000,10000));
                events.RescheduleEvent(EVENT_SHOCK_BLAST, urand(6000,12000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->GetPositionX() < 117.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                if ((uiPhase == 1 || uiPhase == 3) && uiSpawnCount == 0)
                {
                    WaterspoutEnd();
                    return;
                }

                if (me->HealthBelowPct(66) && uiPhase == 0)
                {              
                    uiPhase = 1;
                    uiSpawnCount = 3;
                    me->InterruptNonMeleeSpells(false);
                    me->SetReactState(REACT_PASSIVE);
                    me->GetMotionMaster()->MovePoint(POINT_CENTER_1, centerPos);
                    return;
                }
                if (me->HealthBelowPct(33) && uiPhase == 2)
                {              
                    uiPhase = 3;
                    uiSpawnCount = 3;
                    me->InterruptNonMeleeSpells(false);
                    me->SetReactState(REACT_PASSIVE);
                    me->GetMotionMaster()->MovePoint(POINT_CENTER_2, centerPos);
                    return;
                } 

                switch (uiPhase)
                {
                    case 1:
                    case 3:
                    {
                        events.Update(diff);

                        while (uint32 eventId = events.ExecuteEvent())
                        {
                            switch (eventId)
                            {
                            case EVENT_WATERSPOUT_END:
                                WaterspoutEnd();
                                break;
                            }
                        }
                        break;
                    }
                    case 0:
                    case 2:
                    case 4:
                    {
                        events.Update(diff);

                        if (me->HasUnitState(UNIT_STATE_CASTING))
                            return;

                        while (uint32 eventId = events.ExecuteEvent())
                        {
                            switch (eventId)
                            {
                                case EVENT_GEYSER:
                                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                        DoCast(target, SPELL_SUMMON_GEYSER);
                                    events.ScheduleEvent(EVENT_GEYSER, urand(14000,17000));
                                    break;
                                case EVENT_FUNGAL_SPORES:
                                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                        DoCast(target,SPELL_FUNGAL_SPORES);
                                    events.ScheduleEvent(EVENT_FUNGAL_SPORES, urand(15000,18000));
                                    break;
                                case EVENT_SHOCK_BLAST:
                                    DoCast(me->GetVictim(), SPELL_SHOCK_BLAST);
                                    events.ScheduleEvent(EVENT_SHOCK_BLAST, urand(12000,14000));
                                    break;
                            }
                        }
                        DoMeleeAttackIfReady();
                        break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_lady_nazjarAI>(creature);
        }
};

class npc_lady_nazjar_honnor_guard : public CreatureScript
{
    public:
        npc_lady_nazjar_honnor_guard() : CreatureScript("npc_lady_nazjar_honnor_guard") { }

        struct npc_lady_nazjar_honnor_guardAI : public ScriptedAI
        {
            npc_lady_nazjar_honnor_guardAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            EventMap events;
            bool bEnrage;

            void Reset() override
            {
                bEnrage = false;
                events.ScheduleEvent(EVENT_START_ATTACK, 2000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->HealthBelowPct(35) && !bEnrage)
                {
                    DoCast(me, SPELL_ENRAGE);
                    bEnrage = true;
                    return;
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_START_ATTACK:
                            me->SetReactState(REACT_AGGRESSIVE);
                            events.ScheduleEvent(EVENT_ARC_SLASH, 5000);
                            break;
                        case EVENT_ARC_SLASH:
                            DoCast(me, SPELL_ARC_SLASH);
                            events.ScheduleEvent(EVENT_ARC_SLASH, urand(7000, 10000));
                            break;
                    }
                }
                DoMeleeAttackIfReady();                
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lady_nazjar_honnor_guardAI (creature);
        }
};

class npc_lady_nazjar_tempest_witch : public CreatureScript
{
    public:
        npc_lady_nazjar_tempest_witch() : CreatureScript("npc_lady_nazjar_tempest_witch") { }

        struct npc_lady_nazjar_tempest_witchAI : public ScriptedAI
        {
            npc_lady_nazjar_tempest_witchAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            EventMap events;

            void Reset() override
            {
                events.ScheduleEvent(EVENT_START_ATTACK, 2000);
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
                        case EVENT_START_ATTACK:
                            me->SetReactState(REACT_AGGRESSIVE);
                            events.ScheduleEvent(EVENT_LIGHTNING_SURGE, urand(5000, 7000));
                            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 2000);
                            break;
                        case EVENT_LIGHTNING_SURGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_LIGHTNING_SURGE);
                            events.ScheduleEvent(EVENT_LIGHTNING_SURGE, urand(10000, 15000));
                            break;
                        case EVENT_CHAIN_LIGHTNING:
                            DoCast(me->GetVictim(), SPELL_CHAIN_LIGHTNING);
                            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 2000);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lady_nazjar_tempest_witchAI (creature);
        }
};

class npc_lady_nazjar_waterspout : public CreatureScript
{
    public:
        npc_lady_nazjar_waterspout() : CreatureScript("npc_lady_nazjar_waterspout") { }

        struct npc_lady_nazjar_waterspoutAI : public ScriptedAI
        {
            npc_lady_nazjar_waterspoutAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, 0.3f);
                DoCast(me, SPELL_WATERSPOUT_VISUAL, true);
                bHit = false;
            }

            bool bHit;

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE)
                    if (pointId == POINT_WATERSPOUT)
                        me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff) override
            {
                if (bHit)
                    return;

                if (Unit* target = me->SelectNearestTarget(2.0f))
                {
                    if (target->GetTypeId() != TYPEID_PLAYER)
                        return;
                    bHit = true;
                    target->CastSpell(target, SPELL_WATERSPOUT_DMG, true);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lady_nazjar_waterspoutAI (creature);
        }
};

class npc_lady_nazjar_geyser : public CreatureScript
{
    public:
        npc_lady_nazjar_geyser() : CreatureScript("npc_lady_nazjar_geyser") { }

        struct npc_lady_nazjar_geyserAI : public ScriptedAI
        {
            npc_lady_nazjar_geyserAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
            }

            uint32 uiErruptTimer;
            bool bErrupt;
            std::set<uint64> affectedEnemyGuids;
            std::set<uint64> killedEnemyGuids;

            void Reset() override
            {
                uiErruptTimer = 5000;
                bErrupt = false;
                DoCast(me, SPELL_GEYSER_VISUAL, true);
            }

            void SpellHitTarget(Unit* target, const SpellInfo* spell) override
            {
                if (IsHeroic() && spell->Id == SPELL_GEYSER_ERRUPT_ENTRY)
                {
                    if (killedEnemyGuids.find(target->GetGUID()) != killedEnemyGuids.end())
                    {
                        if (Creature* lady = me->FindNearestCreature(NPC_LADY_NAZJAR, 100))
                            lady->CastSpell(lady, SPELL_ACHIEVEMENT, true);
                    }
                    else
                        affectedEnemyGuids.insert(target->GetGUID());
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (affectedEnemyGuids.find(victim->GetGUID()) != affectedEnemyGuids.end())
                {
                    if (Creature* lady = me->FindNearestCreature(NPC_LADY_NAZJAR, 100))
                        lady->CastSpell(lady, SPELL_ACHIEVEMENT, true);
                }
                else
                    killedEnemyGuids.insert(victim->GetGUID());
            }

            void UpdateAI(uint32 diff) override
            {
                if (uiErruptTimer <= diff && !bErrupt)
                {
                    bErrupt = true;
                    me->RemoveAurasDueToSpell(SPELL_GEYSER_VISUAL);
                    DoCast(me, SPELL_GEYSER_ERRUPT, true);
                    me->DespawnOrUnsummon(1000);
                }
                else
                    uiErruptTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_lady_nazjar_geyserAI>(creature);
        }
};

void AddSC_boss_lady_nazjar()
{
    new boss_lady_nazjar();
    new npc_lady_nazjar_honnor_guard();
    new npc_lady_nazjar_tempest_witch();
    new npc_lady_nazjar_waterspout();
    new npc_lady_nazjar_geyser();
}