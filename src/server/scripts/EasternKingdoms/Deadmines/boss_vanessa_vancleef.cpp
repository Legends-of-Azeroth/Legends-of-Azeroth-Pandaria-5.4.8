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
#include "deadmines.h"
#include "LFGMgr.h"
#include "Group.h"

enum ScriptedTexts
{
    SAY_AGGRO   = 0,
    SAY_DEATH   = 1,
    SAY_KILL    = 2,
    SAY_ADDS    = 3,
    SAY_BOMB_1  = 4,
    SAY_BOMB_2  = 5,
    SAY_INTRO_1 = 8,
    SAY_INTRO_2 = 9,
    SAY_INTRO_3 = 10,
    SAY_INTRO_4 = 11,
    SAY_INTRO_5 = 12,
};

enum Spells
{
    SPELL_DEADLY_BLADES         = 92622,
    SPELL_BACKSLASH_TARGETING   = 92620,
    SPELL_BACKSLASH             = 92619,
    SPELL_DEFLECTION            = 92614,
    SPELL_VENGEANCE_OF_VANCLEEF = 95542,
    SPELL_POWDER_EXPLOSION      = 96283,
    SPELL_SITTING               = 89279,
    SPELL_NOXIOUS_CONCOCTION    = 92100,
    SPELL_VANESSA_ACHIEVEMENT   = 95654,
    SPELL_SUMMON_ENFORCER       = 92616,
    SPELL_SUMMON_SHADOWGUARD    = 92618,
    SPELL_SUMMON_BLOOD_WIZARD   = 92617,

    // Defias Enforcer
    SPELL_CHARGE                = 90928,
    SPELL_BLOODBATH             = 90925,
    SPELL_RECKLESSNESS          = 90929,

    // Defias Blood Wizard
    SPELL_BLOODWASH             = 90946,
    SPELL_BLOODBOLT             = 90938,
    SPELL_RAGEZONE              = 90932,
};

enum Adds
{
    NPC_DEFIAS_ENFORCER             = 49850,
    NPC_DEFIAS_SHADOWGUARD          = 49852,
    NPC_DEFIAS_BLOOD_WIZARD         = 49854,

    NPC_ROPE                        = 49550, // 95527

    // event
    NPC_VANESSA_EVENT               = 49671, // 48143 69676

    NPC_VANESSA_TRAP_BUNNY          = 49454,

    NPC_GLUBTOK_EVENT               = 49670,

    NPC_HELIX_EVENT                 = 49674,

    NPC_CHATTERING_HORROR           = 49495,
    NPC_DARKWEB_DEVOURER            = 49494,

    NPC_FOEREAPER_EVENT             = 49681,
    NPC_VANESSA_LIGHTNING_PLATTER   = 49520,
    NPC_VANESSA_LIGHTNING_STALKER   = 49521,

    NPC_RIPSNARL_EVENT              = 49682,
    NPC_ENRAGED_WORGEN              = 49532,
    NPC_ERIK_HARRINGTON             = 49535, // 2
    NPC_EMME_HARRINGTON             = 49534, // 1
    NPC_CALISSA_HARRINGTON          = 49536, // 3 92608
    NPC_JAMES_HARRINGTON            = 49539,
};

enum Events
{
    EVENT_INTRO_1       = 1,
    EVENT_INTRO_2       = 2,
    EVENT_INTRO_3       = 3,
    EVENT_INTRO_4       = 4,
    EVENT_INTRO_5       = 5,
    EVENT_INTRO_6       = 6,
    EVENT_INTRO_7       = 7,
    EVENT_ADDS_1        = 8,
    EVENT_ADDS_2        = 9,
    EVENT_ADDS_3        = 10,
    EVENT_BACKSLASH     = 11,
    EVENT_DEFLECTION    = 12,
    EVENT_CONTINUE      = 13,
};

#define ACHIEVEMENT_EVENT 27527

const Position addsPos[3] = 
{
    {-67.115807f, -829.708740f, 40.959011f, 1.344201f},
    {-69.161903f, -814.125977f, 40.722912f, 1.611237f},
    {-57.057121f, -816.067200f, 41.852272f, 5.565719f}
};

class boss_vanessa_vancleef : public CreatureScript
{
    public:
        boss_vanessa_vancleef() : CreatureScript("boss_vanessa_vancleef") { }

        struct boss_vanessa_vancleefAI : public BossAI
        {
            boss_vanessa_vancleefAI(Creature* creature) : BossAI(creature, DATA_VANESSA)
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
                me->setActive(true);
                me->SetReactState(REACT_DEFENSIVE);
            }

            void Reset() override
            {
                me->SetReactState(REACT_DEFENSIVE);
                _Reset();

                // by default right now cuz poison phase doesn`t work...
                me->GetMap()->SetWorldState(WORLDSTATE_VANCLEEF_VINDICATION, 1);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    if (instance->GetData(DATA_VANESSA_EVENT) == DONE)
                        me->SetVisible(true);
                    else
                        me->SetVisible(false);
                }
                phase = 0;
            }

            void JustEngagedWith(Unit* /*who*/) override 
            {
                // by default right now cuz poison phase doesn`t work...
                me->GetMap()->SetWorldState(WORLDSTATE_VANCLEEF_VINDICATION, 1);

                _JustEngagedWith();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                    instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_VANESSA_ACHIEVEMENT);
                }

                Talk(SAY_AGGRO);

                events.ScheduleEvent(EVENT_BACKSLASH, urand(10000, 15000));

                DoZoneInCombat();
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim && victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                instance->DoFinishLFGDungeon(326);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->HealthBelowPct(50) && phase == 0)
                {
                    phase = 1;
                    Talk(SAY_ADDS);
                    events.ScheduleEvent(EVENT_ADDS_1, 1000);
                    return;
                }
                
                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BACKSLASH:
                            DoCast(me, SPELL_DEADLY_BLADES);
                            events.ScheduleEvent(EVENT_CONTINUE, 6000);
                            break;
                        case EVENT_CONTINUE:
                            me->GetMotionMaster()->MoveChase(me->GetVictim());
                            break;
                        case EVENT_ADDS_1:
                            me->SummonCreature(NPC_DEFIAS_ENFORCER, addsPos[0]);
                            events.ScheduleEvent(EVENT_ADDS_2, 5000);
                            break;
                        case EVENT_ADDS_2:
                            me->SummonCreature(NPC_DEFIAS_SHADOWGUARD, addsPos[1]);
                            events.ScheduleEvent(EVENT_ADDS_3, 5000);
                            break;
                        case EVENT_ADDS_3:
                            me->SummonCreature(NPC_DEFIAS_BLOOD_WIZARD, addsPos[2]);
                            events.ScheduleEvent(EVENT_ADDS_1, 60000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            uint8 phase;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_vanessa_vancleefAI>(creature);
        }
};

class npc_vanessa_vancleef_a_note_from_vanessa : public CreatureScript
{
    public:
        npc_vanessa_vancleef_a_note_from_vanessa() : CreatureScript("npc_vanessa_vancleef_a_note_from_vanessa") { }
     
        bool OnGossipHello(Player* /*player*/, Creature* creature) override
        {
            InstanceScript* instance = creature->GetInstanceScript();
            if (!instance)
                return true;

            if (instance->GetBossState(DATA_CAPTAIN) != DONE)
                return true;

            if (instance->GetData(DATA_VANESSA_EVENT) != NOT_STARTED)
                return true;

            creature->SummonCreature(NPC_VANESSA_SITTING, vanessaPos);
            instance->SetData(DATA_VANESSA_EVENT, IN_PROGRESS);

            creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            creature->DespawnOrUnsummon(1000);
            return true;
        }
};

class npc_vanessa_vancleef_vanessa_sitting : public CreatureScript
{
    public:
        npc_vanessa_vancleef_vanessa_sitting() : CreatureScript("npc_vanessa_vancleef_vanessa_sitting") { }

        struct npc_vanessa_vancleef_vanessa_sittingAI : public ScriptedAI
        {
            npc_vanessa_vancleef_vanessa_sittingAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->setActive(true);
                instance = me->GetInstanceScript();
            }

            void Reset() override
            {
                events.Reset();
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                events.ScheduleEvent(EVENT_INTRO_1, 1000);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
                
                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_1:
                            Talk(SAY_INTRO_1);
                            events.ScheduleEvent(EVENT_INTRO_2, 5000);
                            break;
                        case EVENT_INTRO_2:
                            Talk(SAY_INTRO_2);
                            events.ScheduleEvent(EVENT_INTRO_3, 8000);
                            break;
                        case EVENT_INTRO_3:
                            Talk(SAY_INTRO_3);
                            events.ScheduleEvent(EVENT_INTRO_4, 8000);
                            break;
                        case EVENT_INTRO_4:
                            Talk(SAY_INTRO_4);
                            events.ScheduleEvent(EVENT_INTRO_5, 5000);
                            break;
                        case EVENT_INTRO_5:
                            Talk(SAY_INTRO_5);
                            DoCastAOE(SPELL_NOXIOUS_CONCOCTION);
                            me->RemoveAura(SPELL_SITTING);
                            events.ScheduleEvent(EVENT_INTRO_6, 5000);
                            break;
                        case EVENT_INTRO_6:
                            me->GetMotionMaster()->MoveJump(centershipPos.GetPositionX(), centershipPos.GetPositionY(), centershipPos.GetPositionZ(), 5.f, 10.f);
                            events.ScheduleEvent(EVENT_INTRO_7, 5000);
                            break;
                        case EVENT_INTRO_7:
                            instance->SetData(DATA_VANESSA_EVENT, DONE);
                            if (Creature* pVanessa = me->FindNearestCreature(NPC_VANESSA_VANCLEEF, 40.0f))
                                pVanessa->SetVisible(true);
                            instance->DoStartCriteria(CRITERIA_START_TYPE_EVENT, ACHIEVEMENT_EVENT);
                            me->DespawnOrUnsummon(1000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* instance;
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_vanessa_vancleef_vanessa_sittingAI>(creature);
        }
};

class npc_vanessa_vancleef_defias_enforcer : public CreatureScript
{
        public:
            npc_vanessa_vancleef_defias_enforcer() : CreatureScript("npc_vanessa_vancleef_defias_enforcer") { }

            struct npc_vanessa_vancleef_defias_enforcerAI : public ScriptedAI
            {
                npc_vanessa_vancleef_defias_enforcerAI(Creature* creature) : ScriptedAI(creature)
                {
                    me->setActive(true);
                }

                void Reset() override { }

                void UpdateAI(uint32 diff) override
                {
                    if (!UpdateVictim())
                        return;

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;

                    DoMeleeAttackIfReady();
                }
            };

            CreatureAI* GetAI(Creature* creature) const override
            {
                return GetInstanceAI<npc_vanessa_vancleef_defias_enforcerAI>(creature);
            }
};

class spell_vanessa_vancleef_backslash_targeting : public SpellScriptLoader
{
    public:
        spell_vanessa_vancleef_backslash_targeting() : SpellScriptLoader("spell_vanessa_vancleef_backslash_targeting") { }

        class spell_vanessa_vancleef_backslash_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_vanessa_vancleef_backslash_targeting_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), SPELL_BACKSLASH, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_vanessa_vancleef_backslash_targeting_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_vanessa_vancleef_backslash_targeting_SpellScript();
        }
};

void AddSC_boss_vanessa_vancleef()
{
    new boss_vanessa_vancleef();
    new npc_vanessa_vancleef_a_note_from_vanessa();
    new npc_vanessa_vancleef_vanessa_sitting();
    new spell_vanessa_vancleef_backslash_targeting();
}