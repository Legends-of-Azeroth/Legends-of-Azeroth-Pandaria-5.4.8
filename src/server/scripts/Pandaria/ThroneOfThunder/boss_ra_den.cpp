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

#include "ScriptMgr.h"
#include "throne_of_thunder.h"

enum Spells
{
    /*Shared*/
    SPELL_LFR_REP_GAIN             = 140242,
    SPELL_MATERIALS_OF_CREATION    = 138321, // possible it should ping trigger ? invisible spheres?
    SPELL_LINGERING_ENERGIES       = 138450, // on aborb any sphere
    SPELL_SUMMON_ESSENCE_OF_VITA   = 138324,
    SPELL_SUMMON_ESSENCE_OF_ANIMA  = 138323,
                                   
    /*Vita*/                       
    SPELL_EMBUED_WITH_VITA         = 138332,
    SPELL_UNLEASHED_VITA           = 138330,
    SPELL_FATAL_STRIKE             = 138334,
    SPELL_UNSTABLE_VITA            = 138308, // so, seems like - on remove this - cast 138371 to farthest, triggerering 138370 and apply this
    SPELL_UNSTABLE_VITA_DUMMY      = 138371, // ok, just visual eff
    SPELL_UNSTABLE_VITA_EFF        = 138370,
    SPELL_UNSTABLE_VITA_INIT       = 138297, // by encounter
    SPELL_VITA_SENSITIVY           = 138372,
    SPELL_SUMMON_CRACKLING_STALKER = 138339,
    SPELL_DRAW_VITA                = 138328,
    SPELL_ABSORB_VITA              = 138326,
    SPELL_CAUTERIZING_FLARE        = 138337, // what the spells of anima type it should remove? we`ll never know cuz anima phase always dodge :D
    SPELL_CRACKLE                  = 138340,

    /*Anima*/
    SPELL_EMBUED_WITH_ANIMA        = 138331,
    SPELL_UNLEASHED_ANIMA          = 138329,
    SPELL_MURDEROUS_STRIKE         = 138333,
    SPELL_UNSTABLE_ANIMA_INIT      = 138288, // by encounter
    SPELL_UNSTABLE_ANIMA_EFF       = 138295,
    SPELL_UNSTABLE_ANIMA_VISUAL    = 138294,
    SPELL_ANIMA_SENSITIVY          = 139318,
    SPELL_ANIMA_EXPLOSION          = 140998, // if unstable anima eff not cause anyone
    SPELL_SUMMON_SANGUINE_HORROR   = 138338,
    SPELL_DRAW_ANIMA               = 138327,
    SPELL_ABSORB_ANIMA             = 138325,
    SPELL_SANGUINE_VOLLEY          = 138341,

    /*Ruin*/
    SPELL_RUIN_BOLT                = 139087, // if victim not in range
    SPELL_RUIN_AURA                = 139073,
    SPELL_RUIN_EFF                 = 139074,
    CORRUPTED_VITA_EFF             = 139072,
    CORRUPTED_ANIMA_EFF            = 139071,
    SPELL_TWISTED_VITA             = 139078, // chain lightning
    SPELL_TWISTED_ANIMA            = 139075, // aoe. if not cause anyone - cast 140998
    SPELL_CALL_ESSENSE             = 139040,
                                   
    /*Cosmetic*/                   
    SPELL_HEAVY_CHAINS_RIGHT       = 140030,
    SPELL_HEAVY_CHAINS_LEFT        = 140032,
    SPELL_KNEEL                    = 142479,

    SPELL_RA_DEN_KILL_CREDIT       = 140182,
};

enum Actions
{
    ACTION_ABSORB_ANIMA = 1,
    ACTION_ABSORB_VITA,
    ACTION_SELECTED_CORRUPTED_ESSENCE,
    ACTION_SPECIAL_STRIKE,
};

enum Events
{
    EVENT_MATERIALS_OF_CREATION = 1,
    EVENT_UNSTABLE_VITA,
    EVENT_SUMM_CRACKLING_STALKER,
    EVENT_UNSTABLE_ANIMA,
    EVENT_SUMM_SANGUINE_HORROR,
    EVENT_CALL_ESSENCE,
    EVENT_CRACKLE,
    EVENT_FATAL_STRIKE,
    EVENT_MURDEROUS_STRIKE,
};

enum Phases
{
    PHASE_NONE,
    PHASE_ANIMA_ESSENCE,
    PHASE_VITA_ESSENCE,
    PHASE_RUIN,
    PHASE_DEFEATED,
};

enum Yells
{
    TALK_INTRO_1,
    TALK_INTRO_2,
    TALK_INTRO_3,
    TALK_AGGRO,
    TALK_MATERIALS_OF_CREATION,
    TALK_PHASE_2,
    TALK_PHASE_2_OUTRO,
    TALK_DEATH, // not real death
    TALK_DEATH_2,
    TALK_ESSENCE_OF_ANIMA,
    TALK_ESSENCE_OF_VITA,
};

// Ra-Den 69473
class boss_ra_den : public CreatureScript
{
    public:
        boss_ra_den() : CreatureScript("boss_ra_den") { }
    
        struct boss_ra_denAI : public BossAI
        {
            boss_ra_denAI(Creature* creature) : BossAI(creature, DATA_RA_DEN) { }

            bool introDone, hasBeenDefeated;
            float x, y;
            ObjectGuid targetGUID, currentAnima, currentVita;
            uint32 delay;
            uint8 phase;

            void InitializeAI() override
            {
                me->setActive(true);
                introDone = false;
                hasBeenDefeated = false;
                delay = 0;

                if (instance && instance->GetData(DATA_RA_DEN_EVENT) != DONE)
                {
                    // In case if encounter load before
                    me->m_Events.Schedule(1500, [this]()
                    {
                        std::list<Creature*> bunnyList;
                        GetCreatureListWithEntryInGrid(bunnyList, me, NPC_GEN_PURPOSE_BUNNY_FLYING, 100.0f);

                        for (auto&& itr : bunnyList)
                            itr->CastSpell(me, itr->GetPositionY() < 4670.0f ? SPELL_HEAVY_CHAINS_RIGHT : SPELL_HEAVY_CHAINS_LEFT, true);
                    });

                    me->HandleEmoteStateCommand(EMOTE_STATE_CUSTOM_SPELL_02);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                }

                Reset();
            }
    
            void Reset() override
            {
                // Prevent issue multi kill at server restart
                if (instance && instance->GetBossState(DATA_RA_DEN) == DONE)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                _Reset();
                
                phase = PHASE_NONE;
                me->SetReactState(REACT_AGGRESSIVE);
                x = 0.0f; y = 0.0f;
                currentAnima = ObjectGuid::Empty;
                currentVita  = ObjectGuid::Empty;

                me->SetPowerType(POWER_MANA);
                me->SetMaxPower(POWER_MANA, 100);
                me->SetPower(POWER_MANA, 0);
                me->setRegeneratingMana(false);

                if (instance && instance->GetData(DATA_RA_DEN_EVENT) == DONE)
                {
                    scheduler
                        .Schedule(Seconds(2), [this](TaskContext /*context*/)
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                    });
                }
            }
    
            void JustEngagedWith(Unit* who) override
            {
                Talk(TALK_AGGRO);
                _JustEngagedWith();

                instance->DoUpdateWorldState(WORLDSTATE_ATTEMPS_REMAINING, 50);
                events.ScheduleEvent(EVENT_MATERIALS_OF_CREATION, 10.5 * IN_MILLISECONDS);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_CURRENT_PHASE)
                    return phase;

                return 0;
            }
    
            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_INTRO:
                        if ((instance && instance->GetData(DATA_RA_DEN_EVENT) == DONE) || introDone)
                            break;
                    
                        introDone = true;
                    
                        Talk(TALK_INTRO_1);
                    
                        delay = 0;
                        me->m_Events.Schedule(delay += 15000, [this]()
                        {
                            Talk(TALK_INTRO_2);
                        });
                    
                        me->m_Events.Schedule(delay += 26000, [this]()
                        {
                            Talk(TALK_INTRO_3);
                        });
                    
                        me->m_Events.Schedule(delay += 6000, [this]()
                        {
                            me->HandleEmoteStateCommand(EMOTE_STATE_CUSTOM_SPELL_03); // should`ve seems like hook on emotes :/
                        });
                    
                        me->m_Events.Schedule(delay += 2800, [this]()
                        {
                            me->RemoveAurasDueToSpell(SPELL_HEAVY_CHAINS_RIGHT);
                        });
                    
                        me->m_Events.Schedule(delay += 6850, [this]()
                        {
                            me->RemoveAurasDueToSpell(SPELL_HEAVY_CHAINS_LEFT);
                        });
                    
                        me->m_Events.Schedule(delay += 3000, [this]()
                        {
                            if (instance)
                                instance->SetData(DATA_RA_DEN_EVENT, DONE);
                    
                            me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                        });
                        break;
                    case ACTION_ABSORB_ANIMA:
                        Talk(TALK_ESSENCE_OF_ANIMA);

                        if (phase != PHASE_ANIMA_ESSENCE)
                            DoCast(me, SPELL_LINGERING_ENERGIES, true);

                        if (Creature* curAnima = ObjectAccessor::GetCreature(*me, currentAnima))
                            curAnima->CastSpell(curAnima, SPELL_DRAW_ANIMA, true);

                        if (Creature* curVita = ObjectAccessor::GetCreature(*me, currentVita))
                        {
                            if (curVita->IsAlive())
                                summons.DespawnEntry(NPC_ESSENSE_OF_ANIMA);
                            else
                            {
                                summons.DespawnEntry(NPC_ESSENSE_OF_ANIMA);
                                summons.DespawnEntry(NPC_ESSENSE_OF_VITA);
                            }
                        }
                        else
                        {
                            summons.DespawnEntry(NPC_ESSENSE_OF_ANIMA);
                            summons.DespawnEntry(NPC_ESSENSE_OF_VITA);
                        }

                        SetPhase(PHASE_ANIMA_ESSENCE);
                        break;
                    case ACTION_ABSORB_VITA:
                        Talk(TALK_ESSENCE_OF_VITA);

                        if (phase != PHASE_VITA_ESSENCE)
                            DoCast(me, SPELL_LINGERING_ENERGIES, true);

                        if (Creature* curVita = ObjectAccessor::GetCreature(*me, currentVita))
                            curVita->CastSpell(curVita, SPELL_DRAW_VITA, true);

                        if (Creature* curAnima = ObjectAccessor::GetCreature(*me, currentAnima))
                        {
                            if (curAnima->IsAlive())
                                summons.DespawnEntry(NPC_ESSENSE_OF_VITA);
                            else
                            {
                                summons.DespawnEntry(NPC_ESSENSE_OF_ANIMA);
                                summons.DespawnEntry(NPC_ESSENSE_OF_VITA);
                            }
                        }
                        else
                        {
                            summons.DespawnEntry(NPC_ESSENSE_OF_ANIMA);
                            summons.DespawnEntry(NPC_ESSENSE_OF_VITA);
                        }

                        SetPhase(PHASE_VITA_ESSENCE);
                        break;
                    case ACTION_SPECIAL_STRIKE:
                        if (phase == PHASE_RUIN)
                            break;

                        events.ScheduleEvent(phase == PHASE_ANIMA_ESSENCE ? EVENT_MURDEROUS_STRIKE : EVENT_FATAL_STRIKE, 0.5 * IN_MILLISECONDS, phase);
                        break;
                }
            }

            void SetPhase(uint8 newPhase)
            {
                uint8 oldPhase = phase;
                phase = newPhase;
                events.SetPhase(newPhase);

                switch (newPhase)
                {
                    case PHASE_ANIMA_ESSENCE:
                        me->RemoveAurasDueToSpell(SPELL_EMBUED_WITH_VITA);

                        if (oldPhase != PHASE_ANIMA_ESSENCE)
                        {
                            DoCast(me, SPELL_EMBUED_WITH_ANIMA, true);
                            DoCast(me, SPELL_UNSTABLE_ANIMA_INIT);
                            events.ScheduleEvent(EVENT_SUMM_SANGUINE_HORROR, 12 * IN_MILLISECONDS, PHASE_ANIMA_ESSENCE);
                            events.CancelEvent(EVENT_SUMM_CRACKLING_STALKER);
                        }

                        if (instance)
                        {
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VITA_SENSITIVY);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_VITA);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_VITA_INIT);
                        }
                        break;
                    case PHASE_VITA_ESSENCE:
                        me->RemoveAurasDueToSpell(SPELL_EMBUED_WITH_ANIMA);

                        if (oldPhase != PHASE_VITA_ESSENCE)
                        {
                            DoCast(me, SPELL_EMBUED_WITH_VITA, true);
                            DoCast(me, SPELL_UNSTABLE_VITA_INIT);
                            events.ScheduleEvent(EVENT_SUMM_CRACKLING_STALKER, 12 * IN_MILLISECONDS, PHASE_VITA_ESSENCE);
                            events.CancelEvent(EVENT_SUMM_SANGUINE_HORROR);
                        }

                        if (instance)
                        {
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANIMA_SENSITIVY);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNLEASHED_ANIMA);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_ANIMA_INIT);
                        }

                        break;
                    case PHASE_RUIN:
                        Talk(TALK_PHASE_2_OUTRO);

                        me->RemoveAurasDueToSpell(SPELL_EMBUED_WITH_ANIMA);
                        me->RemoveAurasDueToSpell(SPELL_EMBUED_WITH_VITA);

                        if (instance)
                        {
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANIMA_SENSITIVY);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNLEASHED_ANIMA);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_ANIMA_INIT);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VITA_SENSITIVY);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_VITA);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_VITA_INIT);
                        }

                        HandleSummonCorruptedEssence();
                        me->RemoveChanneledCast(targetGUID);
                        me->SetPower(POWER_MANA, 0);

                        me->StopMoving();
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->MoveIdle();
                        me->SetReactState(REACT_DEFENSIVE);
                        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                        DoCast(me, SPELL_RUIN_AURA);

                        events.ScheduleEvent(EVENT_CALL_ESSENCE, 15.5 * IN_MILLISECONDS, PHASE_RUIN);
                        break;
                    case PHASE_DEFEATED:
                        JustDied(me);

                        me->m_Events.Schedule(8000, [this]()
                        {
                            Talk(TALK_DEATH_2);
                        });
                        break;
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (hasBeenDefeated)
                {
                    damage = 0;
                    return;
                }

                if (HealthBelowPct(40) && phase != PHASE_RUIN)
                {
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                    phase = PHASE_RUIN;
                    Talk(TALK_PHASE_2);
                    events.CancelEvent(EVENT_MATERIALS_OF_CREATION);
                    events.CancelEvent(EVENT_SUMM_CRACKLING_STALKER);
                    events.CancelEvent(EVENT_SUMM_SANGUINE_HORROR);
                    events.CancelEvent(EVENT_MURDEROUS_STRIKE);
                    events.CancelEvent(EVENT_FATAL_STRIKE);
                    me->InterruptNonMeleeSpells(true, SPELL_MATERIALS_OF_CREATION);
                    summons.DespawnEntry(NPC_ESSENSE_OF_ANIMA);
                    summons.DespawnEntry(NPC_ESSENSE_OF_VITA);

                    if (Unit* target = me->GetVictim())
                        targetGUID = target->GetGUID();

                    me->PrepareChanneledCast(me->GetOrientation());
                    me->GetMotionMaster()->MovePoint(0, radenCenter);
                }

                if (HealthBelowPct(5) && !hasBeenDefeated)
                {
                    hasBeenDefeated = true;
                    SetPhase(PHASE_DEFEATED);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                    SetPhase(PHASE_RUIN);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->SetSpeed(MOVE_RUN, 0.55f);

                // Immune
                summon->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                summon->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                summon->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                summon->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                summon->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                summon->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                summon->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                summon->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                summon->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                summon->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                summon->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);

                // Some issue with animation
                switch (summon->GetEntry())
                {
                    case NPC_CORRUPTED_VITA:
                        summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                        summon->NearTeleportTo(summon->GetPositionX(), summon->GetPositionY(), 1.0f, summon->GetOrientation());
                        break;
                    case NPC_ESSENSE_OF_VITA:
                        summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                        currentVita = summon->GetGUID();
                        summon->NearTeleportTo(summon->GetPositionX(), summon->GetPositionY(), 1.0f, summon->GetOrientation());
                        break;
                    case NPC_ESSENSE_OF_ANIMA:
                        summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                        currentAnima = summon->GetGUID();
                        break;
                    case NPC_CORRUPTED_ANIMA:
                        summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                        break;
                    case NPC_SANGUINE_HORROR:
                    case NPC_CRACKLING_STALKER:
                        summon->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, false);
                        summon->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, false);
                        break;
                }

                summon->SetInCombatWithZone();
            }
    
            void EnterEvadeMode() override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();
    
                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
    
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_RA_DEN, FAIL);

                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANIMA_SENSITIVY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VITA_SENSITIVY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_ANIMA_INIT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_VITA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_VITA_INIT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNLEASHED_ANIMA);
                }

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();
            }
    
            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
    
                Talk(TALK_DEATH);
                DoCastAOE(SPELL_RA_DEN_KILL_CREDIT, true);

                if (Player* player = me->GetMap()->GetFirstPlayerInInstance())
                    player->RewardPersonalLootAndCurrency(me);

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANIMA_SENSITIVY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VITA_SENSITIVY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_ANIMA_INIT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_VITA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_VITA_INIT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNLEASHED_ANIMA);
                }

                me->CombatStop();
                me->SetReactState(REACT_PASSIVE);
                me->SetFaction(35);
                me->RemoveAllAuras();

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();

                me->setRegeneratingHealth(false);
                me->setRegeneratingMana(false);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                DoCast(me, SPELL_KNEEL);
                me->SummonGameObject(GO_CACHE_OF_STORMS, radenChestPos.GetPositionX(), radenChestPos.GetPositionY(), radenChestPos.GetPositionZ(), radenChestPos.GetOrientation(), { }, 0);
            }
    
            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;
    
                events.Update(diff);
    
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MATERIALS_OF_CREATION:
                            Talk(TALK_MATERIALS_OF_CREATION);
                            DoCast(me, SPELL_MATERIALS_OF_CREATION);
                            events.ScheduleEvent(EVENT_MATERIALS_OF_CREATION, urand(32.5 * IN_MILLISECONDS, 35 * IN_MILLISECONDS));
                            break;
                        case EVENT_FATAL_STRIKE:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_FATAL_STRIKE, true);

                            me->SetPower(POWER_MANA, 0);
                            break;
                        case EVENT_MURDEROUS_STRIKE:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_MURDEROUS_STRIKE, true);

                            me->SetPower(POWER_MANA, 0);
                            break;
                        case EVENT_SUMM_CRACKLING_STALKER:
                            DoCast(me, SPELL_SUMMON_CRACKLING_STALKER);
                            events.ScheduleEvent(EVENT_SUMM_CRACKLING_STALKER, 41.5 * IN_MILLISECONDS, PHASE_VITA_ESSENCE);
                            break;
                        case EVENT_SUMM_SANGUINE_HORROR:
                            DoCast(me, SPELL_SUMMON_SANGUINE_HORROR);
                            events.ScheduleEvent(EVENT_SUMM_CRACKLING_STALKER, 41 * IN_MILLISECONDS, PHASE_ANIMA_ESSENCE);
                            break;
                        case EVENT_CALL_ESSENCE:
                            DoCast(me, SPELL_CALL_ESSENSE);
                            events.ScheduleEvent(EVENT_CALL_ESSENCE, 15.5 * IN_MILLISECONDS, PHASE_RUIN);
                            break;
                    }
                }
    
                if (phase == PHASE_RUIN)
                    if (Unit* target = me->GetVictim())
                        if (me->GetExactDist2d(target) > 10.0f)
                            DoCast(me, SPELL_RUIN_BOLT);

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff);
            }

            private:
                // Anima orbs move on outer circle with high intensivity.
                // Vita Inverted (inner, low)
                void HandleSummonCorruptedEssence()
                {
                    float sOri = M_PI / 2;

                    do
                    {
                        // Vita Spawn
                        GetPositionWithDistInOrientation(me, 46.0f, Position::NormalizeOrientation(sOri), x, y);
                        me->SummonCreature(NPC_CORRUPTED_VITA, x, y, me->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);

                        // Anima Spawn
                        GetPositionWithDistInOrientation(me, 56.0f, Position::NormalizeOrientation(sOri + M_PI / 12), x, y);
                        me->SummonCreature(NPC_CORRUPTED_ANIMA, x, y, me->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);

                        sOri += M_PI / 6;

                    } while (sOri < (5 * M_PI) / 2);
                }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_ra_denAI(creature);
        }
};

// Essence of Anima, Essence of Vita 69869, 69870
struct npc_essence_of_creation : public ScriptedAI
{
    npc_essence_of_creation(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();
        DoCast(me, me->GetEntry() == NPC_ESSENSE_OF_ANIMA ? SPELL_DRAW_ANIMA : SPELL_DRAW_VITA);
        me->ClearUnitState(UNIT_STATE_CASTING);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* raDen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_RA_DEN) : ObjectGuid::Empty))
            raDen->AI()->DoAction(me->GetEntry() == NPC_ESSENSE_OF_ANIMA ? ACTION_ABSORB_VITA : ACTION_ABSORB_ANIMA);
    }

    void UpdateAI(uint32 diff) override
    {
        if (Creature* raDen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_RA_DEN) : ObjectGuid::Empty))
        {
            if (raDen->IsWithinDist(me, 2.0f, false))
                raDen->AI()->DoAction(me->GetEntry() == NPC_ESSENSE_OF_ANIMA ? ACTION_ABSORB_ANIMA : ACTION_ABSORB_VITA);
            else
                me->GetMotionMaster()->MoveFollow(raDen, 0.0f, me->GetAngle(raDen));
        }
    }
};

// Corrupted Essence of Anima, Corrupted Essence of Vita 69957, 69958
struct npc_corrupted_essence_of_creation : public ScriptedAI
{
    npc_corrupted_essence_of_creation(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;
    float x, y, dist;
    bool choiseMe, onDeath;

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();
        choiseMe = false;
        onDeath  = false;

        dist = me->GetEntry() == NPC_CORRUPTED_ANIMA ? 56.0f : 46.0f;

        Movement::MoveSplineInit init(me);
        for (uint8 i = 1; i < 25; ++i)
        {
            x = radenCenter.GetPositionX() + dist * cos(Position::NormalizeOrientation(me->GetAngle(radenCenter.GetPositionX(), radenCenter.GetPositionY()) + (i * M_PI / 12) - M_PI));
            y = radenCenter.GetPositionY() + dist * sin(Position::NormalizeOrientation(me->GetAngle(radenCenter.GetPositionX(), radenCenter.GetPositionY()) + (i * M_PI / 12) - M_PI));
            init.Path().push_back(G3D::Vector3(x, y, me->GetPositionZ()));
        }

        init.SetUncompressed();
        init.SetCyclic();
        init.Launch();
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_SELECTED_CORRUPTED_ESSENCE)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
            me->StopMoving();
            choiseMe = true;
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (!choiseMe)
            damage = 0;

        if (damage >= me->GetHealth() && !onDeath)
        {
            damage = 0;
            onDeath = true;

            switch (me->GetEntry())
            {
                case NPC_CORRUPTED_ANIMA:
                    DoCast(me, SPELL_TWISTED_ANIMA);
                    break;
                case NPC_CORRUPTED_VITA:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                        DoCast(target, SPELL_TWISTED_VITA);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        DoCast(target, SPELL_TWISTED_VITA);
                    break;
            }

            me->DespawnOrUnsummon();
        }
    }

    void UpdateAI(uint32 diff) override 
    {
        if (!choiseMe)
            return;

        if (Creature* raDen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_RA_DEN) : ObjectGuid::Empty))
        {
            if (raDen->IsWithinDist(me, 2.0f, false))
            {
                choiseMe = false;
                me->CastSpell(raDen, me->GetEntry() == NPC_CORRUPTED_ANIMA ? CORRUPTED_ANIMA_EFF : CORRUPTED_VITA_EFF, true);
                me->DespawnOrUnsummon();
            }
            else
                me->GetMotionMaster()->MoveFollow(raDen, 0.0f, me->GetAngle(raDen));
        }
    }
};

// Crackling Stalker 69872
struct npc_crackling_stalker : public ScriptedAI
{
    npc_crackling_stalker(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;
    EventMap events;

    void IsSummonedBy(Unit* summoner) override
    {
        events.ScheduleEvent(EVENT_CRACKLE, 3 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCast(me, SPELL_CAUTERIZING_FLARE, true);
    }

    void CastInterrupted(SpellInfo const* spell) override
    {
        events.RescheduleEvent(EVENT_CRACKLE, 6 * IN_MILLISECONDS);
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
            if (eventId == EVENT_CRACKLE)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_CRACKLE);

                events.ScheduleEvent(EVENT_CRACKLE, urand(3.5 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Sanguine Horror 69871
struct npc_sanguine_horror : public ScriptedAI
{
    npc_sanguine_horror(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
        volleyAvailable = false;

        scheduler.Schedule(Milliseconds(2000), [this](TaskContext /*context*/)
        {
            volleyAvailable = true;
        });
    }

    TaskScheduler scheduler;
    bool volleyAvailable;

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        scheduler.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (Unit* target = me->GetVictim())
            if (!me->IsWithinMeleeRange(target, 5.15f) && volleyAvailable)
                DoCast(me, SPELL_SANGUINE_VOLLEY);

        DoMeleeAttackIfReady();
    }
};

// Fatal Strike 138334
class spell_raden_fatal_strike : public SpellScript
{
    PrepareSpellScript(spell_raden_fatal_strike);

    void RecalculateDamage(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            SetHitDamage(int32(target->GetMaxHealth() * 0.8));
    }

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Unit* target = GetHitUnit())
            if (HasAnyDefence(target))
                PreventHitEffect(effIndex);
    }

    bool HasAnyDefence(Unit* target)
    {
        for (auto dItr : ProtectiveSpells)
            if (target->HasAura(dItr))
                return true;

        return false;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_raden_fatal_strike::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHitTarget += SpellEffectFn(spell_raden_fatal_strike::HandleEffectHit, EFFECT_1, SPELL_EFFECT_INSTAKILL);
    }
};

// Embued with Vita 138332
class spell_embued_with_vita : public AuraScript
{
    PrepareAuraScript(spell_embued_with_vita);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_UNLEASHED_VITA, true);
    }

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            if (owner->GetPower(POWER_MANA) > 99)
                owner->AI()->DoAction(ACTION_SPECIAL_STRIKE);

    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_embued_with_vita::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_402, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_embued_with_vita::OnTrigger, EFFECT_1, SPELL_AURA_PERIODIC_ENERGIZE);
    }
};

// Unstable Vita Visual 138371
class spell_unstable_vita_visual : public SpellScript
{
    PrepareSpellScript(spell_unstable_vita_visual);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_UNSTABLE_VITA_EFF, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_unstable_vita_visual::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Unstable Vita Eff 138370
class spell_unstable_vita_eff : public SpellScript
{
    PrepareSpellScript(spell_unstable_vita_eff);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Unit* target = GetHitUnit())
        {
            if (!target->HasAura(SPELL_VITA_SENSITIVY))
                PreventHitEffect(effIndex);

            target->CastSpell(target, SPELL_VITA_SENSITIVY, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_unstable_vita_eff::HandleEffectHit, EFFECT_1, SPELL_EFFECT_INSTAKILL);
    }
};

// Unstable Vita Init 138297
class spell_unstable_vita_init : public SpellScript
{
    PrepareSpellScript(spell_unstable_vita_init);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if(TankSpecTargetSelector());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_unstable_vita_init::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Unstable Vita 138308 (players), 138297 (by encounter)
class spell_unstable_vita_aura : public AuraScript
{
    PrepareAuraScript(spell_unstable_vita_aura);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (owner->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
                SetDuration(5 * IN_MILLISECONDS);
    }

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            // jump only if same phase is active
            if (Creature* raDen = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_RA_DEN) : ObjectGuid::Empty))
                if (raDen->AI()->GetData(TYPE_CURRENT_PHASE) == PHASE_VITA_ESSENCE)
                    owner->CastSpell(owner, SPELL_UNSTABLE_VITA, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_unstable_vita_aura::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_unstable_vita_aura::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Unstable Vita 138308
class spell_unstable_vita : public SpellScript
{
    PrepareSpellScript(spell_unstable_vita);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
            targets.sort(Trinity::ObjectDistanceOrderPred(caster, false));

        if (targets.size() > 1)
            targets.resize(1);
    }

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->CastSpell(target, SPELL_UNSTABLE_VITA_DUMMY, true);
                caster->CastSpell(caster, SPELL_VITA_SENSITIVY, true);

                // We should hit another players who stay between us
                std::list<Player*> pList;
                GetPlayerListInGrid(pList, caster, 200.0f);

                for (auto&& itr : pList)
                    if (itr->IsInBetween(caster, target, 2.0f) && itr->IsAlive() && itr->GetGUID() != caster->GetGUID() && itr->GetGUID() != target->GetGUID())
                        caster->CastSpell(itr, SPELL_UNSTABLE_VITA_DUMMY, true);

            }
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_unstable_vita::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
        OnEffectHitTarget += SpellEffectFn(spell_unstable_vita::HandleEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Materials of Creation 138321 (should ping to anything, but no data)
class spell_materials_of_creation : public SpellScript
{
    PrepareSpellScript(spell_materials_of_creation);

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
        {
            sOri = frand(0, 2 * M_PI);

            for (uint32 i = 0; i < 2; i++)
            {
                sOri = Position::NormalizeOrientation(sOri + M_PI * i);
                x = caster->GetPositionX() + (56.0f * cos(sOri));
                y = caster->GetPositionY() + (56.0f * sin(sOri));

                // Recalculate position for one of the orb if encounter around of wall
                if (radenCenter.GetExactDist2d(x, y) > 57.0f)
                {
                    x = radenCenter.GetPositionX() + (56.0f * cos(sOri));
                    y = radenCenter.GetPositionY() + (56.0f * sin(sOri));
                }

                // Spawn orbs
                caster->CastSpell(x, y, caster->GetPositionZ(), SPELL_SUMMON_ESSENCE_OF_ANIMA + i, true);
            }
        }
    }

    private:
        float x, y, sOri;

    void Register() override
    {
        AfterCast += SpellCastFn(spell_materials_of_creation::HandleAfterCast);
    }
};

// Embued with Anima 138331
class spell_embued_with_anima : public AuraScript
{
    PrepareAuraScript(spell_embued_with_anima);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_UNLEASHED_ANIMA, true);
    }

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            if (owner->GetPower(POWER_MANA) > 99)
                owner->AI()->DoAction(ACTION_SPECIAL_STRIKE);

    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_embued_with_anima::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_402, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_embued_with_anima::OnTrigger, EFFECT_1, SPELL_AURA_OBS_MOD_POWER);
    }
};

// Call Essence 139040
class spell_call_essence : public SpellScript
{
    PrepareSpellScript(spell_call_essence);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_CORRUPTED_ANIMA && target->GetEntry() != NPC_CORRUPTED_VITA; });

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
            target->AI()->DoAction(ACTION_SELECTED_CORRUPTED_ESSENCE);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_call_essence::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_call_essence::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Unstable Anima 138288
class spell_unstable_anima_aura : public AuraScript
{
    PrepareAuraScript(spell_unstable_anima_aura);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_UNSTABLE_ANIMA_VISUAL, true);
    }

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            owner->RemoveAurasDueToSpell(SPELL_UNSTABLE_ANIMA_VISUAL);

            // only if owner dead (or smth else) while same phase is active
            if (Creature* raDen = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_RA_DEN) : ObjectGuid::Empty))
                if (raDen->AI()->GetData(TYPE_CURRENT_PHASE) == PHASE_ANIMA_ESSENCE)
                    raDen->CastSpell(raDen, SPELL_UNSTABLE_ANIMA_INIT, true);
        }
    }

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            owner->CastSpell(owner, SPELL_UNSTABLE_ANIMA_VISUAL, true);
            owner->CastSpell(owner, SPELL_UNSTABLE_ANIMA_EFF, true);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_unstable_anima_aura::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_unstable_anima_aura::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_unstable_anima_aura::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Unstable Anima 138288
class spell_unstable_anima : public SpellScript
{
    PrepareSpellScript(spell_unstable_anima);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if(TankSpecTargetSelector());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_unstable_anima::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Unstable Anima Eff 138295
class spell_unstable_anima_eff : public SpellScript
{
    PrepareSpellScript(spell_unstable_anima_eff);

    uint8 targetsCount;

    bool Load() override
    {
        targetsCount = 1;
        return true;
    }

    void RecalculateDamage(SpellEffIndex /*effIndex*/)
    {
        if (targetsCount > 0)
            SetHitDamage(GetHitDamage() / targetsCount);
    }

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Unit* target = GetHitUnit())
        {
            if (!target->HasAura(SPELL_ANIMA_SENSITIVY))
                PreventHitEffect(effIndex);

            target->CastSpell(target, SPELL_ANIMA_SENSITIVY, true);
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        // if damage aren`t split - wipe
        if (targets.empty())
            if (Unit* caster = GetCaster())
                if (Creature* raDen = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(DATA_RA_DEN) : ObjectGuid::Empty))
                    raDen->CastSpell(raDen, SPELL_ANIMA_EXPLOSION, true);

        targetsCount = targets.size();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_unstable_anima_eff::HandleEffectHit, EFFECT_1, SPELL_EFFECT_INSTAKILL);
        OnEffectHitTarget += SpellEffectFn(spell_unstable_anima_eff::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_unstable_anima_eff::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Twisted Anima 139075
class spell_twisted_anima : public SpellScript
{
    PrepareSpellScript(spell_twisted_anima);

    uint8 targetsCount;

    bool Load() override
    {
        targetsCount = 1;
        return true;
    }

    void RecalculateDamage(SpellEffIndex /*effIndex*/)
    {
        if (targetsCount > 0)
            SetHitDamage(GetHitDamage() / targetsCount);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        // if damage aren`t split - wipe
        if (targets.empty())
            if (Unit* caster = GetCaster())
                if (Creature* raDen = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(DATA_RA_DEN) : ObjectGuid::Empty))
                    raDen->CastSpell(raDen, SPELL_ANIMA_EXPLOSION, true);

        targetsCount = targets.size();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_twisted_anima::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_twisted_anima::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Murderous Strike 138333
class spell_murderous_strike : public AuraScript
{
    PrepareAuraScript(spell_murderous_strike);

    uint32 currentHealh;

    bool Load() override
    {
        currentHealh = 1;
        return true;
    }

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            currentHealh = owner->GetHealth();
    }

    void CalculateTick(AuraEffect const*, int32& damage)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            damage = int32(currentHealh);
    }

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, float& amount, bool& canBeRecalculated)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            amount = owner->GetHealth();

        canBeRecalculated = false;
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_murderous_strike::HandleAuraEffectApply, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_murderous_strike::HandleEffectCalcAmount, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
        DoEffectCalcTickAmount += AuraEffectCalcTickAmountFn(spell_murderous_strike::CalculateTick, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// Draw Vita 138328
class spell_draw_vita : public AuraScript
{
    PrepareAuraScript(spell_draw_vita);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        // Exploit
        if (aurEff->GetBase()->GetDuration() > 0)
            return;

        if (Creature* raDen = GetOwner()->ToCreature())
            raDen->AI()->DoAction(ACTION_ABSORB_ANIMA);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_draw_vita::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Areatrigger 8949
class AreaTrigger_forward_ra_den : public AreaTriggerScript
{
    public:
        AreaTrigger_forward_ra_den() : AreaTriggerScript("AreaTrigger_forward_ra_den") { }
    
        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* raden = ObjectAccessor::GetCreature(*player, instance->GetGuidData(DATA_RA_DEN)))
                    raden->AI()->DoAction(ACTION_START_INTRO);
    
            return true;
        }
};

void AddSC_boss_ra_den()
{
    new boss_ra_den();
    new creature_script<npc_essence_of_creation>("npc_essence_of_creation");
    new creature_script<npc_corrupted_essence_of_creation>("npc_corrupted_essence_of_creation");
    new creature_script<npc_crackling_stalker>("npc_crackling_stalker");
    new creature_script<npc_sanguine_horror>("npc_sanguine_horror");
    new spell_script<spell_raden_fatal_strike>("spell_raden_fatal_strike");
    new aura_script<spell_embued_with_vita>("spell_embued_with_vita");
    new spell_script<spell_unstable_vita_visual>("spell_unstable_vita_visual");
    new spell_script<spell_unstable_vita_eff>("spell_unstable_vita_eff");
    new spell_script<spell_unstable_vita_init>("spell_unstable_vita_init");
    new aura_script<spell_unstable_vita_aura>("spell_unstable_vita_aura");
    new spell_script<spell_unstable_vita>("spell_unstable_vita");
    new spell_script<spell_materials_of_creation>("spell_materials_of_creation");
    new aura_script<spell_embued_with_anima>("spell_embued_with_anima");
    new spell_script<spell_call_essence>("spell_call_essence");
    new aura_script<spell_unstable_anima_aura>("spell_unstable_anima_aura");
    new spell_script<spell_unstable_anima>("spell_unstable_anima");
    new spell_script<spell_unstable_anima_eff>("spell_unstable_anima_eff");
    new spell_script<spell_twisted_anima>("spell_twisted_anima");
    new aura_script<spell_murderous_strike>("spell_murderous_strike");
    new aura_script<spell_draw_vita>("spell_draw_vita");
    new AreaTrigger_forward_ra_den();
}
