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

/* ScriptData
SDName: Boss_Brutallus
SD%Complete: 80
SDComment: Find a way to start the intro, best code for the intro
EndScriptData */

#include "ScriptPCH.h"
#include "sunwell_plateau.h"

enum Quotes
{
    YELL_INTRO                          = 0,
    YELL_INTRO_BREAK_ICE                = 1,
    YELL_INTRO_CHARGE                   = 2,
    YELL_INTRO_KILL_MADRIGOSA           = 3,
    YELL_INTRO_TAUNT                    = 4,

    YELL_AGGRO                          = 5,
    YELL_KILL                           = 6,
    YELL_LOVE                           = 7,
    YELL_BERSERK                        = 8,
    YELL_DEATH                          = 9,

    YELL_MADR_ICE_BARRIER               = 0,
    YELL_MADR_INTRO                     = 1,
    YELL_MADR_ICE_BLOCK                 = 2,
    YELL_MADR_TRAP                      = 3,
    YELL_MADR_DEATH                     = 4,
};

enum Spells
{
    SPELL_METEOR_SLASH                  = 45150,
    SPELL_BURN                          = 46394,
    SPELL_STOMP                         = 45185,
    SPELL_BERSERK                       = 26662,
    SPELL_DUAL_WIELD                    = 42459,

    SPELL_INTRO_FROST_BLAST             = 45203,
    SPELL_INTRO_FROSTBOLT               = 44843,
    SPELL_INTRO_ENCAPSULATE             = 45665,
    SPELL_INTRO_ENCAPSULATE_CHANELLING  = 45661,

    SPELL_SUMMON_BRUTALLUS_DEATH_CLOUD  = 45884,
};

class boss_brutallus : public CreatureScript
{
    public:
        boss_brutallus() : CreatureScript("boss_brutallus") { }

        struct boss_brutallusAI : public ScriptedAI
        {
            boss_brutallusAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                intro = true;
            }

            InstanceScript* instance;
            uint32 slashTimer;
            uint32 burnTimer;
            uint32 stompTimer;
            uint32 berserkTimer;
            uint32 introPhase;
            uint32 IntroPhaseTimer;
            uint32 IntroFrostBoltTimer;
            bool intro;
            bool isIntro;
            bool enraged;

            void Reset() override
            {
                slashTimer = 11000;
                stompTimer = 30000;
                burnTimer = 60000;
                berserkTimer = 360000;

                introPhase = 0;
                IntroPhaseTimer = 0;
                IntroFrostBoltTimer = 0;

                isIntro = false;
                enraged = false;

                DoCast(me, SPELL_DUAL_WIELD, true);

                if (fightingPlayers)
                {
                    instance->SetData(DATA_BRUTALLUS_EVENT, NOT_STARTED);
                    fightingPlayers = false;
                }
            }

            void JustEngagedWith(Unit* who) override
            {
                if (instance->GetData(DATA_KALECGOS_EVENT) != DONE)
                {
                    instance->DoCastSpellOnPlayers(SPELL_ELEPORT_TO_APEX_POINT);
                    EnterEvadeMode();
                    return;
                }

                if (!fightingPlayers && who->IsControlledByPlayer())
                {
                    Talk(YELL_AGGRO);

                    fightingPlayers = true;
                    instance->SetData(DATA_BRUTALLUS_EVENT, IN_PROGRESS);
                }
            }

            void DamageTaken(Unit* attacker, uint32& /*damage*/) override
            {
                if (!fightingPlayers && attacker->IsControlledByPlayer())
                {
                    Talk(YELL_AGGRO);

                    fightingPlayers = true;
                    instance->SetData(DATA_BRUTALLUS_EVENT, IN_PROGRESS);
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(YELL_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(YELL_DEATH);

                DoCast(me, SPELL_SUMMON_BRUTALLUS_DEATH_CLOUD, true);

                if (instance)
                {
                    if (!fightingPlayers)
                        instance->SetData(DATA_BRUTALLUS_EVENT, IN_PROGRESS);

                    fightingPlayers = false;
                    instance->SetData(DATA_BRUTALLUS_EVENT, DONE);
                    if (Creature* madrigosa = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_MADRIGOSA)))
                    {
                        uint32 delay = 0;
                        madrigosa->m_Events.Schedule(delay += 65000, [madrigosa]() { madrigosa->CastSpell(madrigosa, SPELL_SUMMON_FELBLAZE, true); });
                        madrigosa->m_Events.Schedule(delay +=  9000, [madrigosa]() { madrigosa->SetVisible(false); });
                    }
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                if (!intro)
                    ScriptedAI::EnterEvadeMode();
            }

            void StartIntro()
            {
                if (!intro || isIntro)
                    return;

                TC_LOG_DEBUG("scripts", "Start intro");
                Creature* madrigosa = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_MADRIGOSA) : ObjectGuid::Empty);
                if (madrigosa)
                {
                    madrigosa->Respawn();
                    madrigosa->setActive(true);
                    isIntro = true;
                    madrigosa->SetMaxHealth(me->GetMaxHealth());
                    madrigosa->SetHealth(me->GetMaxHealth());
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->Attack(madrigosa, true);
                    madrigosa->Attack(me, true);
                }
                else
                {
                    // madrigosa not found, end intro
                    TC_LOG_DEBUG("scripts", "madrigosa was not found");
                    EndIntro();
                }
            }

            void EndIntro()
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                intro = false;
                isIntro = false;
                TC_LOG_DEBUG("scripts", "End intro");
            }

            void AttackStart(Unit* who) override
            {
                if (!who || intro || isIntro)
                    return;
                ScriptedAI::AttackStart(who);
            }

            void DoIntro()
            {
                Creature* madrigosa = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_MADRIGOSA) : ObjectGuid::Empty);
                if (!madrigosa)
                    return;

                switch (introPhase)
                {
                    case 0:
                        madrigosa->AI()->Talk(YELL_MADR_ICE_BARRIER);
                        IntroPhaseTimer = 7000;
                        ++introPhase;
                        break;
                    case 1:
                        me->SetInFront(madrigosa);
                        madrigosa->SetInFront(me);
                        madrigosa->AI()->Talk(YELL_MADR_INTRO);
                        IntroPhaseTimer = 9000;
                        ++introPhase;
                        break;
                    case 2:
                        madrigosa->AI()->Talk(YELL_INTRO);
                        IntroPhaseTimer = 13000;
                        ++introPhase;
                        break;
                    case 3:
                        madrigosa->CastSpell(me, SPELL_INTRO_FROST_BLAST, true);
                        madrigosa->SetFlying(true);
                        madrigosa->OverrideInhabitType(INHABIT_AIR);
                        me->AttackStop();
                        madrigosa->AttackStop();
                        IntroFrostBoltTimer = 3000;
                        IntroPhaseTimer = 18000; // 28000;
                        ++introPhase;
                        break;
                    case 4:
                        Talk(YELL_INTRO_BREAK_ICE);
                        IntroPhaseTimer = 6000;
                        ++introPhase;
                        break;
                    case 5:
                        madrigosa->CastSpell(me, SPELL_INTRO_ENCAPSULATE_CHANELLING, false);
                        madrigosa->AI()->Talk(YELL_MADR_TRAP);
                        DoCast(me, SPELL_INTRO_ENCAPSULATE);
                        IntroPhaseTimer = 11000;
                        ++introPhase;
                        break;
                    case 6:
                        Talk(YELL_INTRO_CHARGE);
                        IntroPhaseTimer = 5000;
                        ++introPhase;
                        break;
                    case 7:
                        madrigosa->SetFlying(false);
                        madrigosa->ResetInhabitTypeOverride();
                        madrigosa->CastSpell(madrigosa, 29266, true); // Permanent Feign Death
                        madrigosa->AI()->Talk(YELL_MADR_DEATH);
                        me->SetFullHealth();
                        me->AttackStop();
                        IntroPhaseTimer = 4000;
                        ++introPhase;
                        break;
                    case 8:
                        Talk(YELL_INTRO_KILL_MADRIGOSA);
                        me->SetFacingTo(0.14f);
                        IntroPhaseTimer = 8000;
                        ++introPhase;
                        break;
                    case 9:
                        Talk(YELL_INTRO_TAUNT);
                        IntroPhaseTimer = 5000;
                        ++introPhase;
                        break;
                    case 10:
                        EndIntro();
                        break;
                }
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!who->isTargetableForAttack() || !me->IsHostileTo(who))
                    return;
                if (instance && intro)
                {
                    if (instance->GetData(DATA_KALECGOS_EVENT) != DONE)
                    {
                        instance->DoCastSpellOnPlayers(SPELL_ELEPORT_TO_APEX_POINT);
                        EnterEvadeMode();
                        return;
                    }

                    instance->SetData(DATA_BRUTALLUS_EVENT, SPECIAL);
                }

                if (intro && !isIntro)
                    StartIntro();
                if (!intro)
                    ScriptedAI::MoveInLineOfSight(who);
            }

            void UpdateAI(uint32 diff) override
            {
                if (isIntro)
                {
                    if (IntroPhaseTimer <= diff)
                        DoIntro();
                    else IntroPhaseTimer -= diff;

                    if (introPhase == 3 + 1)
                    {
                        if (IntroFrostBoltTimer <= diff)
                        {
                            if (Creature* madrigosa = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_MADRIGOSA) : ObjectGuid::Empty))
                            {
                                madrigosa->CastSpell(me, SPELL_INTRO_FROSTBOLT, true);
                                IntroFrostBoltTimer = 2000;
                            }
                        } else IntroFrostBoltTimer -= diff;
                    }
                    if (!UpdateVictim())
                        return;
                    DoMeleeAttackIfReady();
                }

                if (!UpdateVictim() || isIntro)
                    return;

                if (!fightingPlayers && me->GetVictim() && me->GetVictim()->IsControlledByPlayer())
                {
                    Talk(YELL_AGGRO);

                    fightingPlayers = true;
                    instance->SetData(DATA_BRUTALLUS_EVENT, IN_PROGRESS);
                }

                if (slashTimer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_METEOR_SLASH);
                    slashTimer = 11000;
                } else slashTimer -= diff;

                if (stompTimer <= diff)
                {
                    Talk(YELL_LOVE);
                    DoCast(me->GetVictim(), SPELL_STOMP);
                    stompTimer = 30000;
                } else stompTimer -= diff;

                if (burnTimer <= diff)
                {
                    std::list<Unit*> targets;
                    SelectTargetList(targets, 10, SELECT_TARGET_RANDOM, 100, true);
                    for (auto&& target : targets)
                        if (!target->HasAura(SPELL_BURN))
                        {
                            target->CastSpell(target, SPELL_BURN, true);
                            break;
                        }
                    burnTimer = urand(60000,180000);
                } else burnTimer -= diff;

                if (berserkTimer < diff && !enraged)
                {
                    Talk(YELL_BERSERK);
                    DoCast(me, SPELL_BERSERK);
                    enraged = true;
                } else berserkTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            bool fightingPlayers = false;
        };


        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_brutallusAI(creature);
        }
};


void AddSC_boss_brutallus()
{
    new boss_brutallus();
}
