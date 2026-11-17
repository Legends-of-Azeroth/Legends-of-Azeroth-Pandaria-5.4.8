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
#include "nexus.h"

enum Spells
{
    SPELL_BATTLE_SHOUT                           = 31403,
    SPELL_CHARGE                                 = 60067,
    SPELL_FRIGHTENING_SHOUT                      = 19134,
    SPELL_WHIRLWIND                              = 38619,
    SPELL_WHIRLWIND_ADD                          = 38618,
    SPELL_FROZEN_PRISON                          = 47543, // Frozen block around them - on creature create and leave combat

    // Add Spells
    // Alliance Cleric
    SPELL_POWER_WORD_SHIELD                      = 35944,
    SPELL_SHADOW_WORD_DEATH                      = 56920, // 47697,
    SPELL_FLASH_HEAL                             = 17843,

    // Alliance Range
    SPELL_INCENDIARY_SHOT                        = 47777,
    SPELL_RAPID_SHOT                             = 48191,
    SPELL_SHOOT                                  = 15620,

    // Alliance Berserker
    SPELL_BLOODLUST                              = 21049,
    SPELL_FRENZY                                 = 47774,
    SPELL_WAR_STOMP                              = 38682,
};

enum Yells
{
    SAY_AGGRO                                    = 0,
    SAY_KILL                                     = 1,
    SAY_DEATH                                    = 2,
};

class boss_commander_stoutbeard : public CreatureScript
{
    public:
        boss_commander_stoutbeard() : CreatureScript("boss_commander_stoutbeard") { }

        struct boss_commander_stoutbeardAI : public BossAI
        {
            boss_commander_stoutbeardAI(Creature* creature) : BossAI(creature, DATA_COMMANDER)
            {
                //me->CastSpell(me, SPELL_FROZEN_PRISON, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);  // Death Grip
            }

            uint32 uiBattleShout_Timer;
            uint32 uiCharge_Timer;
            uint32 uiFrighteningShout_Timer;
            uint32 uiWhirlwind_Timer;
            uint32 uiWhirlwindAdd_Timer;

            bool IsWhirlwindTime;

            void Reset() override
            {
                uiBattleShout_Timer = urand(200000, 215000);
                uiCharge_Timer = urand(15000, 17500);
                uiFrighteningShout_Timer = urand(10000, 12500);
                uiWhirlwind_Timer = 10000;
                uiWhirlwindAdd_Timer = 10500;
                me->SetFaction(16);

                IsWhirlwindTime = false;

                if (instance)
                    instance->SetData(DATA_COMMANDER, NOT_STARTED);

                DespawnCreatures(NPC_ALLIANCE_CLERIC, 250);

                me->SummonCreature(NPC_ALLIANCE_CLERIC, 419.047668f, 189.318436f, -35.019547f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000ms);
                me->SummonCreature(NPC_ALLIANCE_CLERIC, 431.265961f, 183.628845f, -35.019547f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000ms);
            }

            void DespawnCreatures(uint32 entry, float distance)
            {
                std::list<Creature*> m_creatures;
                GetCreatureListWithEntryInGrid(m_creatures, me, entry, distance);
         
                if (m_creatures.empty())
                    return;
         
                for (auto&& creature : m_creatures)
                    creature->DespawnOrUnsummon();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                DoCast(me, SPELL_BATTLE_SHOUT);
                
                me->RemoveAllAuras();

                if (instance)
                    instance->SetData(DATA_COMMANDER, IN_PROGRESS);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                if (instance)
                    instance->SetData(DATA_COMMANDER, DONE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasAura(SPELL_FROZEN_PRISON))
                   me->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);

                if (uiCharge_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_CHARGE);
                    uiCharge_Timer = urand(10500, 12000);
                } else uiCharge_Timer -= diff;

                if (uiBattleShout_Timer <= diff)
                {
                    DoCastAOE(SPELL_BATTLE_SHOUT);
                    uiBattleShout_Timer = 200000;
                } else uiBattleShout_Timer -= diff;

                if (uiFrighteningShout_Timer <= diff)
                {
                    DoCastAOE(SPELL_FRIGHTENING_SHOUT);
                    uiFrighteningShout_Timer = urand(8000, 10000);
                } else uiFrighteningShout_Timer -= diff;

                if (uiWhirlwind_Timer <= diff && !IsWhirlwindTime)
                {
                    DoCastAOE(SPELL_WHIRLWIND);
                    uiWhirlwindAdd_Timer = 500;
                    IsWhirlwindTime = true;
                } else uiWhirlwind_Timer -= diff;

                if (uiWhirlwindAdd_Timer <= diff && IsWhirlwindTime)
                {
                    DoCastAOE(SPELL_WHIRLWIND_ADD);
                    uiWhirlwind_Timer = urand(10000, 12500);
                    IsWhirlwindTime = false;
                } else uiWhirlwindAdd_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_commander_stoutbeardAI(creature);
        }
};

class npc_alliance_cleric : public CreatureScript
{
    public:
        npc_alliance_cleric() : CreatureScript("npc_alliance_cleric") { }

        struct npc_alliance_clericAI : public ScriptedAI
        {
            npc_alliance_clericAI(Creature* creature) : ScriptedAI(creature)
            {
                //me->CastSpell(me, SPELL_FROZEN_PRISON, true);
            }

            uint32 uiFlashHeal_Timer;
            uint32 uiShadowWordDeath_Timer;
            uint32 uiPowerWordShield_Timer;

            void Reset() override
            {
                uiFlashHeal_Timer = urand(9000, 10500);
                uiShadowWordDeath_Timer = urand(5500, 8500);
                uiPowerWordShield_Timer = urand(2000, 4000);
                me->SetFaction(16);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->RemoveAllAuras();
            }

            void JustReachedHome() override
            {
               /*me->CastSpell(me, SPELL_FROZEN_PRISON, true); */
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasAura(SPELL_FROZEN_PRISON))
                   me->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);
                
                if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                {
                    if (uiFlashHeal_Timer <= diff)
                    {
                        DoCast(target, SPELL_FLASH_HEAL);
                        uiFlashHeal_Timer = urand(7000, 8000);
                    } else uiFlashHeal_Timer -= diff;

                    if (uiPowerWordShield_Timer <= diff)
                    {
                        DoCast(target, SPELL_POWER_WORD_SHIELD);
                        uiPowerWordShield_Timer = urand(9000, 10000);
                    } else uiPowerWordShield_Timer -= diff;
                }

                if (uiShadowWordDeath_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                        DoCast(target, SPELL_SHADOW_WORD_DEATH);
                    uiShadowWordDeath_Timer = urand(4500, 5500);
                } else uiShadowWordDeath_Timer -= diff;     
                
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_alliance_clericAI(creature);
        }
};

class npc_alliance_ranger : public CreatureScript
{
    public:
        npc_alliance_ranger() : CreatureScript("npc_alliance_ranger") { }

        struct npc_alliance_rangerAI : public ScriptedAI
        {
            npc_alliance_rangerAI(Creature* creature) : ScriptedAI(creature)
            {
                //me->CastSpell(me, SPELL_FROZEN_PRISON, true); 
            }

            uint32 uiRapidShot_Timer;
            uint32 uiIncendiaryShot_Timer;
            uint32 uiShoot_Timer;
            
            uint8 i;

            void Reset() override
            {
                uiRapidShot_Timer = urand(12500, 14000);
                uiIncendiaryShot_Timer = urand(6500, 8000);
                uiShoot_Timer = urand(2500, 3000);
                me->SetFaction(16);

                //me->CastSpell(me, SPELL_FROZEN_PRISON, true);
            }

            void JustEngagedWith(Unit* who) override
            {
                me->RemoveAllAuras();
            }

            void JustReachedHome() override
            {
               //me->CastSpell(me, SPELL_FROZEN_PRISON, true);
            }

            Unit* FindTarget()
            {
                i = 0;
                Unit* target;

                while (i < 5)
                {
                    target = SelectTarget(SELECT_TARGET_TOPAGGRO, i);
                    i++;

                    if (!me->IsWithinDistInMap(target, 5.0f))
                        return target;
                }
                return 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasAura(SPELL_FROZEN_PRISON))
                   me->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);

                if (uiIncendiaryShot_Timer <= diff)
                {
                    if (Unit* target = FindTarget())
                        DoCast(target, SPELL_INCENDIARY_SHOT);
                    uiIncendiaryShot_Timer = urand(6500, 7500);
                } else uiIncendiaryShot_Timer -= diff;

                if (uiShoot_Timer <= diff)
                {
                    if (Unit* target = FindTarget())
                        DoCast(target, SPELL_SHOOT);
                    uiShoot_Timer = urand(2500, 3000);
                } else uiShoot_Timer -= diff;
                
                if (uiRapidShot_Timer <= diff)
                {
                    DoCastAOE(SPELL_RAPID_SHOT);
                    uiRapidShot_Timer = urand(12500, 14000);
                } else uiRapidShot_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_alliance_rangerAI(creature);
        }
};

class npc_alliance_berserker : public CreatureScript
{
    public:
        npc_alliance_berserker() : CreatureScript("npc_alliance_berserker") { }

        struct npc_alliance_berserkerAI : public ScriptedAI
        {
            npc_alliance_berserkerAI(Creature* creature) : ScriptedAI(creature)
            {
                /*me->CastSpell(me, SPELL_FROZEN_PRISON, true); */
            }

            uint32 uiBloodlust_Timer;
            uint32 uiFrenzy_Timer;
            uint32 uiWarStomp_Timer;

            void Reset() override
            {
                uiBloodlust_Timer = urand(5000, 7500);
                uiFrenzy_Timer = urand(2500, 4000);
                uiWarStomp_Timer = urand(6000, 8000);
                me->SetFaction(16);
            }

            void JustReachedHome() override
            {
               /*me->CastSpell(me, SPELL_FROZEN_PRISON, true); */
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->RemoveAllAuras();
            }
            
            Creature* SelectRandomFriendlyMissingBuff(uint32 spell)
            {
                std::list<Creature*> lst = DoFindFriendlyMissingBuff(40.0f, spell);
                std::list<Creature*>::const_iterator itr = lst.begin();
                if (lst.empty())
                    return NULL;
                advance(itr, rand() % lst.size());
                return (*itr);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasAura(SPELL_FROZEN_PRISON))
                    me->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);

                if (uiBloodlust_Timer <= diff)
                {
                    if (Creature* target = SelectRandomFriendlyMissingBuff(SPELL_BLOODLUST)) //He should casts this only on allies, not on self...
                        DoCast(target, SPELL_BLOODLUST);
                    uiBloodlust_Timer = urand(12000, 15000);
                } else uiBloodlust_Timer -= diff;

                if (uiFrenzy_Timer <= diff)
                {
                    DoCast(me, SPELL_FRENZY);
                    uiFrenzy_Timer = 200000;
                } else uiFrenzy_Timer -= diff;

                if (uiWarStomp_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_WAR_STOMP);
                    uiWarStomp_Timer = urand(7000, 8500);
                } else uiWarStomp_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_alliance_berserkerAI(creature);
        }
};
  
void AddSC_boss_commander_stoutbeard() 
{ 
    new boss_commander_stoutbeard();
    new npc_alliance_cleric();
    new npc_alliance_ranger();
    new npc_alliance_berserker();
}
