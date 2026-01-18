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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "scarlet_monastery.h"

enum ePhases
{
    PHASE_ONE,
    PHASE_TWO
};

enum Spells
{
    SPELL_FLASH_OF_STEEL           = 115629, // Durand
    SPELL_DASHING_STRIKE           = 115739,
    SPELL_DASHING_STRIKE_TARGET    = 122519,
    SPELL_DASHING_STRIKE_INVISIBLE = 115676,
    SPELL_DASHING_STRIKE_KB        = 122520,
    SPELL_DASHING_STRIKE_AURA      = 115704, // unknown effect
    SPELL_POWER_WORD_SHIELD        = 22187, // Whitemane
    SPELL_SMITE                    = 114848,
    SPELL_HEAL                     = 36678,
    SPELL_MASS_RESURRETION         = 113134,
    SPELL_DEEP_SLEEP               = 9256, // at 50% health of Whitemane
    SPELL_REVIVE                   = 9232 // same
};

enum eDurandEvents
{
    EVENT_FLASH_OF_STEEL           = 1,
    EVENT_DASHING_STRIKE           = 2,
    EVENT_DASHING_STRIKE_BACK      = 3
};

enum eWhitemaneEvents
{
    EVENT_INTRO                    = 1,
    EVENT_SMITE                    = 2,
    EVENT_MASS_RESURRETION         = 3,
    EVENT_HEAL                     = 4,
};

enum eActions
{
    ACTION_DURAND                  = 1,
    ACTION_INTRO                   = 2,
    ACTION_LAST_PHASE              = 3
};

enum Durand_Yells
{
    DURAND_TALK_INTRO              = 1,
    DURAND_TALK_SLAY               = 2
};

enum Whitemane_Yells
{
    WHITEMANE_TALK_AGGRO           = 0,
    WHITEMANE_TALK_DEATH           = 1,
    WHITEMANE_TALK_SLAY            = 2,
    WHITEMANE_TALK_RESSURETION     = 3,
};

const Position Whitemane_intro = { 740.48f, 606.35f, 15.027f, 6.26f };

class boss_commander_durand : public CreatureScript
{
    public:
        boss_commander_durand() : CreatureScript("boss_commander_durand") { }

        struct boss_commander_durandAI : public BossAI
        {
            boss_commander_durandAI(Creature* creature) : BossAI(creature, BOSS_DURAND) { }

            ePhases phases;
            uint32 _flashcount;
            uint32 _dashingcount;
            bool _dashingcheck;
            bool _fakedeath;
            bool _restore;

            void Reset() override
            {
                _Reset();
                events.Reset();
                me->setRegeneratingHealth(true);
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                if (instance)
                    instance->SetData(BOSS_DURAND, NOT_STARTED);
                phases = PHASE_ONE;
                _flashcount = 0;
                _dashingcount = 0;
                _dashingcheck = false;
                _fakedeath = false;
                _restore = false;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                if (instance)
                {
                    instance->SetData(BOSS_DURAND, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    // Quest Ender 
                    if (Creature* HoodedCrusader = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HOODED_CRUSADER_OUTRO)))
                    {
                        HoodedCrusader->SetVisible(true);
                        HoodedCrusader->GetMotionMaster()->MovePoint(0, Whitemane_intro);
                    }
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (me->GetHealth() <= damage && !_fakedeath)
                {

                    //On first death, fake death and open door, as well as initiate whitemane if exist
                    if (Creature* Whitemane = Unit::GetCreature(*me, instance->GetGuidData(BOSS_WHITEMANE)))
                    {
                        damage = 0;
                        Whitemane->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        Whitemane->AI()->DoAction(ACTION_INTRO);
                        instance->SetBossState(BOSS_WHITEMANE, IN_PROGRESS);

                        if (me->IsNonMeleeSpellCasted(false))
                            me->InterruptNonMeleeSpells(false);

                        me->ClearComboPointHolders();
                        me->RemoveAllAuras();
                        me->ClearAllReactives();
                        _fakedeath = true;
                        _restore = true;
                        me->SetHealth(IsHeroic() ? 1000000 : 5000);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->SetReactState(REACT_PASSIVE);
                        me->SetStandState(UNIT_STAND_STATE_DEAD);
                        me->AttackStop();
                        events.Reset();

                        if (GameObject* WhiteDoor = GetClosestGameObjectWithEntry(me, GO_WHITEMANE_ENTRANCE, 50.0f))
                            WhiteDoor->UseDoorOrButton(4500);
                    }
                }

                if (_dashingcheck)
                    damage = 0;
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_DURAND)
                    BossAI::EnterEvadeMode();
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(BOSS_DURAND, FAIL);
                }
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                if (instance)
                {
                    instance->SetData(BOSS_DURAND, IN_PROGRESS);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }
                Talk(DURAND_TALK_INTRO);
                events.ScheduleEvent(EVENT_FLASH_OF_STEEL, 9000);
                events.ScheduleEvent(EVENT_DASHING_STRIKE, urand(24000, 25000));
                me->CallForHelp(VISIBLE_RANGE);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (_restore && me->GetHealthPct() >= 50)
                {
                    _restore = false;
                    events.ScheduleEvent(EVENT_FLASH_OF_STEEL, 9000);
                    events.ScheduleEvent(EVENT_DASHING_STRIKE, urand(24000, 25000));
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                    {
                        me->Attack(target, true);
                        me->GetMotionMaster()->MoveChase(target);
                    }
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FLASH_OF_STEEL:
                            if (_flashcount < 6)
                            {
                                _flashcount++;
                                if (Unit* player = SelectTarget(SELECT_TARGET_FARTHEST))
                                    me->CastSpell(player, SPELL_FLASH_OF_STEEL, false);
                                events.ScheduleEvent(EVENT_FLASH_OF_STEEL, urand(500, 1000));
                            }
                            else
                            {
                                _flashcount = 0;
                                events.ScheduleEvent(EVENT_DASHING_STRIKE, urand(21000, 26000));
                            }
                            break;
                        case EVENT_DASHING_STRIKE:
                            if (_dashingcount < 6)
                            {
                                if (!_dashingcheck) 
                                { 
                                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); 
                                    DoCast(me, SPELL_DASHING_STRIKE_INVISIBLE);
                                    _dashingcheck = true; 
                                    Talk(DURAND_TALK_SLAY); 
                                }
                                _dashingcount++;
                                if (Unit* player = SelectTarget(SELECT_TARGET_RANDOM))
                                {
                                    DoCastAOE(SPELL_DASHING_STRIKE_TARGET);
                                    DoCast(me, SPELL_DASHING_STRIKE_AURA);
                                    me->CastSpell(player, SPELL_DASHING_STRIKE, false);
                                }
                                events.ScheduleEvent(EVENT_DASHING_STRIKE, urand(500, 1000));
                            }
                            else
                            {
                                _dashingcount = 0;
                                _dashingcheck = false;
                                events.ScheduleEvent(EVENT_DASHING_STRIKE, urand(24000, 25000));
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            }
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_commander_durandAI(creature);
        }
};

class boss_high_inqusitior_whitemane : public CreatureScript
{
    public:
        boss_high_inqusitior_whitemane() : CreatureScript("boss_high_inqusitior_whitemane") { }

        struct boss_high_inqusitior_whitemaneAI : public BossAI
        {
            boss_high_inqusitior_whitemaneAI(Creature* creature) : BossAI(creature, BOSS_WHITEMANE) { }

            bool _switch, InRessurection;

            void InitializeAI() override
            {
                SetCombatMovement(false);
                IsHeroic() ? me->SetMaxPower(POWER_MANA, 120000) : me->SetMaxPower(POWER_MANA, 6415);
                me->SetPower(POWER_MANA, me->GetMaxPower(POWER_MANA));
                _switch = false;
                InRessurection = false;
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_INTRO:
                        me->GetMotionMaster()->MovePoint(0, Whitemane_intro);
                        events.ScheduleEvent(EVENT_INTRO, me->GetSplineDuration());
                        break;
                    case ACTION_LAST_PHASE:
                        InRessurection = false;
                        me->SetReactState(REACT_AGGRESSIVE);
                        instance->SetBossState(BOSS_DURAND, SPECIAL);
                        events.ScheduleEvent(EVENT_HEAL, urand(15000, 16000));
                        events.ScheduleEvent(EVENT_MASS_RESURRETION, 10 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_SMITE, 0.5 * IN_MILLISECONDS);

                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DEEP_SLEEP);

                        if (Player* itr = me->FindNearestPlayer(80.0f))
                            me->AI()->AttackStart(itr);
                        break;
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override  
            {
                if (InRessurection && damage >= me->GetHealth())
                    damage = 0;

                if (me->HealthBelowPctDamaged(50, damage) && !_switch)
                {
                    damage = 0;
                    _switch = true;
                    events.Reset();
                    scheduler.CancelAll();
                    me->InterruptNonMeleeSpells(true);
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    DoCastAOE(SPELL_DEEP_SLEEP);
                    DoCast(me, SPELL_POWER_WORD_SHIELD);
                    InRessurection = true;

                    if (Unit* Durand = ObjectAccessor::GetUnit(*me, instance->GetGuidData(BOSS_DURAND)))
                        me->GetMotionMaster()->MovePoint(1, Durand->GetPositionX() + frand(-1.5f, 1.5f), Durand->GetPositionY() + frand(-0.8f, 0.8f), Durand->GetPositionZ(), Durand->GetOrientation());

                    // sometimes it stuck, and she doesn`t appear to durand near (stuck on half of path if distance too long, in result MovementInForm not called)
                    scheduler
                        .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext /*context*/) // blizzlike
                    {
                        Talk(WHITEMANE_TALK_RESSURETION);

                        if (Unit* Durand = Unit::GetCreature(*me, instance->GetGuidData(BOSS_DURAND)))
                        {
                            me->SetFacingTo(me->GetAngle(Durand));
                            DoCast(Durand, SPELL_REVIVE);
                        }
                    });
                }
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                me->setRegeneratingHealth(true);
                if (instance)
                    instance->SetData(BOSS_WHITEMANE, NOT_STARTED);
                _switch = false;
                InRessurection = false;
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(WHITEMANE_TALK_DEATH);
                _JustDied();
                if (instance)
                {
                    instance->SetData(BOSS_WHITEMANE, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }

                // Quest Object
                DoCast(me, SPELL_WHITEMANE_CORPSE_SPELL_FOCUS);
                DoCastAOE(SPELL_WHITEMANE_KILL_CREDIT);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(BOSS_WHITEMANE, FAIL);
                }
                summons.DespawnAll();
                scheduler.CancelAll();
            }

            void AttackStart(Unit* target) override
            {
                if (!target)
                    return;

                if (me->Attack(target, false))
                    DoStartNoMovement(target);
            }

            void CastInterrupted(SpellInfo const* /*spell*/) override
            {
                events.RescheduleEvent(EVENT_SMITE, 6 * IN_MILLISECONDS);

                if (Unit* vict = me->GetVictim())
                    me->GetMotionMaster()->MoveChase(vict);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                if (instance)
                {
                    instance->SetData(BOSS_WHITEMANE, IN_PROGRESS);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }

                Talk(WHITEMANE_TALK_AGGRO);
                events.ScheduleEvent(EVENT_MASS_RESURRETION, 20 * IN_MILLISECONDS);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    if (Player* _player = me->FindNearestPlayer(VISIBLE_RANGE))
                        me->AI()->AttackStart(_player);
                    else
                    {
                        BossAI::EnterEvadeMode();
                        if (Unit* Whitemane = Unit::GetCreature(*me, instance->GetGuidData(BOSS_WHITEMANE)))
                            Whitemane->GetAI()->DoAction(ACTION_DURAND);
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO:
                            events.ScheduleEvent(EVENT_SMITE, 0.5 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_HEAL, urand(9000, 13000));
                            break;
                        case EVENT_SMITE:
                            me->StopMoving();
                            me->GetMotionMaster()->MovementExpired();

                            if (Unit* vict = me->GetVictim())
                            {
                                if (me->GetExactDist2d(vict) > 30.0f)
                                    DoStartMovement(vict);
                                else
                                    DoStartNoMovement(vict);
                            }

                            if (Unit* target = me->GetVictim())
                                if (!InRessurection)
                                    DoCast(target, SPELL_SMITE);

                            events.ScheduleEvent(EVENT_SMITE, urand(2.6 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                            break;
                        case EVENT_HEAL:
                        {
                            if (me->HasUnitState(UNIT_STATE_CASTING) && me->GetCurrentSpell(CURRENT_GENERIC_SPELL) && (me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_REVIVE || me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_MASS_RESURRETION))
                                return;

                            me->InterruptNonMeleeSpells(true);
                            uint32 result = urand(1, 2);
                            switch (result)
                            {
                                case 1:
                                    me->CastSpell(me, SPELL_HEAL, false);
                                    break;
                                case 2:
                                    if (Unit* Durand = Unit::GetCreature(*me, instance->GetGuidData(BOSS_DURAND)))
                                        me->CastSpell(Durand, SPELL_HEAL, false);
                                    break;
                                default:
                                    break;
                            }
                            events.ScheduleEvent(EVENT_HEAL, urand(9000, 13000));
                            break;
                        }
                        case EVENT_MASS_RESURRETION:
                            if (me->HasUnitState(UNIT_STATE_CASTING) && me->GetCurrentSpell(CURRENT_GENERIC_SPELL) && me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_REVIVE)
                                return;

                            me->InterruptNonMeleeSpells(true);
                            DoCast(me, SPELL_MASS_RESURRETION);
                            events.ScheduleEvent(EVENT_MASS_RESURRETION, 20 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_high_inqusitior_whitemaneAI(creature);
        }
};

// Scarlet Ressurection 9232
class spell_sc_scarlet_ressurection : public SpellScript
{
    PrepareSpellScript(spell_sc_scarlet_ressurection);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            caster->SetFullHealth();
            caster->AI()->DoAction(ACTION_LAST_PHASE);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_sc_scarlet_ressurection::HandleAfterCast);
    }
};

// Mass Ressurection 113134
class spell_sc_mass_ressurection : public SpellScript
{
    PrepareSpellScript(spell_sc_mass_ressurection);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([this](WorldObject const* target)
        {
            if (target->GetTypeId() != TYPEID_UNIT)
                return true;

            switch (target->GetEntry())
            {
                case NPC_SCARLET_FANATIC:
                case NPC_SCARLET_PURIFIER:
                case NPC_SCARLET_ZEALOT:
                case NPC_SCARLET_JUDICATOR:
                case NPC_SCARLET_INITIATE:
                    break;
                default:
                    return true;
            }
            return target->ToCreature()->IsAlive() || GetCaster()->GetExactDist(target) > 80.0f;
        });
    }

    void HandleHit()
    {
        Creature* target = GetHitUnit()->ToCreature();
        target->setDeathState(JUST_RESPAWNED);
        target->m_PlayerDamageReq = std::numeric_limits<uint32>::max();
        if (Player* player = target->FindNearestPlayer(100.0f))
            target->AI()->AttackStart(player);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sc_mass_ressurection::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnHit += SpellHitFn(spell_sc_mass_ressurection::HandleHit);
    }
};

void AddSC_boss_whitemane_and_durand()
{
    new boss_commander_durand();
    new boss_high_inqusitior_whitemane();
    new spell_script<spell_sc_scarlet_ressurection>("spell_sc_scarlet_ressurection");
    new spell_script<spell_sc_mass_ressurection>("spell_sc_mass_ressurection");
}