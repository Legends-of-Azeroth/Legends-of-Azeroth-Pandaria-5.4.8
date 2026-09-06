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
SDName: Boss Volkhan
SD%Complete: 90%
SDComment: for I-core
SDCategory: Halls of Lightning
EndScriptData */

#include "ScriptPCH.h"
#include "halls_of_lightning.h"

enum Spells
{
    SPELL_HEAT                              = 52387,
    SPELL_HEAT_H                            = 59528,
    SPELL_SHATTERING_STOMP                  = 52237,
    SPELL_SHATTERING_STOMP_H                = 59529,

    SPELL_TEMPER                            = 52238,
    SPELL_TEMPER_DUMMY                      = 52654,
    SPELL_TEMPER_SUMMON                     = 52661,

    SPELL_SUMMON_MOLTEN_GOLEM               = 52405,

    // Molten Golem
    SPELL_BLAST_WAVE                        = 23113,
    SPELL_IMMOLATION_STRIKE                 = 52433,
    SPELL_IMMOLATION_STRIKE_H               = 59530,
    SPELL_SHATTER                           = 52429,
    SPELL_SHATTER_H                         = 59527,
};

enum Yells
{
    SAY_AGGRO                               = 0,
    SAY_FORGE                               = 1,
    SAY_STOMP                               = 2,
    SAY_SLAY                                = 3,
    SAY_DEATH                               = 4,
    EMOTE_TO_ANVIL                          = 5,
    EMOTE_SHATTER                           = 6,
};

enum Npcs
{
    NPC_VOLKHAN_ANVIL                       = 28823,
    NPC_MOLTEN_GOLEM                        = 28695,
    NPC_BRITTLE_GOLEM                       = 28681,
};

enum Misc
{
    GO_VOLKHAN_TEMPER_VISUAL                = 190858,

    MAX_GOLEM                               = 2,

    ACHIEVEMENT_SHATTER_RESISTANT           = 2042
};

class boss_volkhan : public CreatureScript
{
    public:
        boss_volkhan() : CreatureScript("boss_volkhan") { }

        struct boss_volkhanAI : public ScriptedAI
        {
            boss_volkhanAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            std::list<ObjectGuid> m_lGolemGUIDList;

            bool m_bHasTemper;
            bool m_bIsStriking;
            bool m_bCanShatterGolem;

            uint8 GolemsShattered;
            uint32 m_uiPause_Timer;
            uint32 m_uiShatteringStomp_Timer;
            uint32 m_uiShatter_Timer;
            uint32 m_uiDelay_Timer;
            uint32 SUMMON_PHASE;

            uint32 temperVisualTimer;

            uint32 m_uiHealthAmountModifier;

            void Reset() override
            {
                m_bIsStriking = false;
                m_bHasTemper = false;
                m_bCanShatterGolem = false;

                m_uiPause_Timer = 3500;
                m_uiShatteringStomp_Timer = 0;
                m_uiShatter_Timer = 5000;
                m_uiDelay_Timer = 1000;
                SUMMON_PHASE = 0;
                GolemsShattered = 0;

                temperVisualTimer = urand(10000, 20000);

                m_uiHealthAmountModifier = 1;

                DespawnGolem();
                m_lGolemGUIDList.clear();

                if (instance)
                    instance->SetData(TYPE_VOLKHAN, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);

                if (instance)
                    instance->SetData(TYPE_VOLKHAN, IN_PROGRESS);
            }

            void AttackStart(Unit* who) override
            {
                if (me->Attack(who, true))
                {
                    me->GetThreatManager().AddThreat(who, 0.0f);
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);

                    if (!m_bHasTemper)
                        me->GetMotionMaster()->MoveChase(who);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                DespawnGolem();

                if (instance)
                    instance->SetData(TYPE_VOLKHAN, DONE);

                if (IsHeroic() && GolemsShattered < 5)
                {
                    AchievementEntry const* AchievShatterResistant = sAchievementStore.LookupEntry(ACHIEVEMENT_SHATTER_RESISTANT);
                    if (AchievShatterResistant)
                    {
                        Map* map = me->GetMap();
                        if (map && map->IsDungeon())
                        {
                            for (auto&& ref : map->GetPlayers())
                                ref.GetSource()->CompletedAchievement(AchievShatterResistant);
                        }
                    }
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_SLAY);
            }

            void DespawnGolem()
            {
                if (m_lGolemGUIDList.empty())
                    return;

                for (auto&& guid : m_lGolemGUIDList)
                {
                    if (Creature* temp = Unit::GetCreature(*me, guid))
                    {
                        if (temp->IsAlive())
                            temp->DespawnOrUnsummon();
                    }
                }

                m_lGolemGUIDList.clear();
            }

            void ShatterGolem()
            {
                if (m_lGolemGUIDList.empty())
                    return;

                for (auto&& guid : m_lGolemGUIDList)
                {
                    if (Creature* temp = Unit::GetCreature(*me, guid))
                    {
                        // only shatter brittle golems
                        if (temp->IsAlive() && temp->GetEntry() == NPC_BRITTLE_GOLEM)
                        {
                            temp->CastSpell(temp, DUNGEON_MODE(SPELL_SHATTER, SPELL_SHATTER_H), false);
                            GolemsShattered += 1;
                        }
                    }
                }
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_MOLTEN_GOLEM)
                {
                    m_lGolemGUIDList.push_back(summon->GetGUID());

                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                    if (target)
                        summon->GetMotionMaster()->MoveFollow(target, 0.0f, 0.0f);

                    // why healing when just summoned?
                    summon->CastSpell(summon, DUNGEON_MODE(SPELL_HEAT, SPELL_HEAT_H), false, NULL, NULL, me->GetGUID());
                }
            }

            void JustReachedHome() override
            {
                if (SUMMON_PHASE == 2)
                {
                    me->SetOrientation(2.29f);
                    SUMMON_PHASE = 3;
                }
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_TEMPER || spell->Id == SPELL_TEMPER_DUMMY)
                    if (GameObject* visual = me->FindNearestGameObject(GO_VOLKHAN_TEMPER_VISUAL, 30.0f))
                        visual->SendCustomAnim(0);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                {
                    if (temperVisualTimer <= diff)
                    {
                        DoCastAOE(SPELL_TEMPER_SUMMON, true);
                        DoCastAOE(SPELL_TEMPER_DUMMY);
                        temperVisualTimer = urand(10000, 20000);
                    }
                    else
                        temperVisualTimer -= diff;

                    return;
                }

                if (m_bIsStriking)
                {
                    if (m_uiPause_Timer <= diff)
                    {
                        if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                        {
                            if (me->GetVictim())
                                me->GetMotionMaster()->MoveChase(me->GetVictim());
                        }

                        m_bHasTemper = false;
                        m_bIsStriking = false;
                        m_uiPause_Timer = 3500;
                    }
                    else
                        m_uiPause_Timer -= diff;

                    return;
                }

                // When to start shatter? After 60, 40 or 20% hp?
                if (!m_bHasTemper && m_uiHealthAmountModifier >= 3)
                {
                    if (m_uiShatteringStomp_Timer <= diff)
                    {
                        // should he stomp even if he has no brittle golem to shatter?

                        Talk(SAY_STOMP);

                        DoCast(me, SPELL_SHATTERING_STOMP);

                        Talk(EMOTE_SHATTER);

                        m_uiShatteringStomp_Timer = 30000;
                        m_bCanShatterGolem = true;
                    }
                    else
                        m_uiShatteringStomp_Timer -= diff;
                }

                // Shatter Golems 3 seconds after Shattering Stomp
                if (m_bCanShatterGolem)
                {
                    if (m_uiShatter_Timer <= diff)
                    {
                        ShatterGolem();
                        m_uiShatter_Timer = 3000;
                        m_bCanShatterGolem = false;
                    }
                    else
                        m_uiShatter_Timer -= diff;
                }

                // Health check
                if (!m_bCanShatterGolem && me->HealthBelowPct(100 - 20 * m_uiHealthAmountModifier))
                {
                    ++m_uiHealthAmountModifier;

                    if (me->IsNonMeleeSpellCasted(false))
                        me->InterruptNonMeleeSpells(false);

                    Talk(SAY_FORGE);

                    m_bHasTemper = true;

                    SUMMON_PHASE = 1;
                }

                if (SUMMON_PHASE > 0)
                {
                    // 1 - Start run to Anvil
                    if (SUMMON_PHASE == 1)
                    {
                        Talk(EMOTE_TO_ANVIL);
                        me->GetMotionMaster()->MoveTargetedHome();
                        SUMMON_PHASE = 2; //Set Next Phase
                    }
                    // 2 - Check if reached Anvil
                    // This is handled in: void JustReachedHome()

                    // 3 - Cast Temper on the Anvil
                    if (SUMMON_PHASE == 3)
                    {
                        if (Unit* target = GetClosestCreatureWithEntry(me, NPC_VOLKHAN_ANVIL, 1000.0f, true))
                        {
                            me->SetOrientation(2.29f);
                            DoCast(target, SPELL_TEMPER_SUMMON, true);
                            DoCast(target, SPELL_TEMPER_DUMMY, false);
                        }

                        m_uiDelay_Timer = 1000; // Delay 2 seconds before next phase can begin
                        SUMMON_PHASE = 4; // Set Next Phase
                    }

                    // 4 - Wait for delay to expire
                    if (SUMMON_PHASE == 4)
                    {
                        if (m_uiDelay_Timer <= diff)
                        {
                            Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0);
                            if (target)
                            {
                                me->SetReactState(REACT_AGGRESSIVE);
                                me->SetInCombatWith(target);
                                me->GetMotionMaster()->MoveFollow(target, 0.0f, 0.0f);
                            }
                            SUMMON_PHASE = 5;
                        }
                        else
                            m_uiDelay_Timer -= diff;
                    }

                    // 5 - Spawn the Golems
                    if (SUMMON_PHASE == 5)
                    {
                        if (Creature* pCreatureTarget = GetClosestCreatureWithEntry(me, NPC_VOLKHAN_ANVIL, 1000.0f, true))
                            for (uint8 i = 0; i < MAX_GOLEM; ++i)
                                me->CastSpell(pCreatureTarget, SPELL_SUMMON_MOLTEN_GOLEM, true);

                        m_bIsStriking = true;
                        SUMMON_PHASE = 0; // Reset back to Phase 0 for next time
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_volkhanAI(creature);
        }
};

class npc_molten_golem : public CreatureScript
{
    public:
        npc_molten_golem() : CreatureScript("npc_molten_golem") { }

        struct npc_molten_golemAI : public ScriptedAI
        {
            npc_molten_golemAI(Creature* creature) : ScriptedAI(creature) { }

            bool m_bIsFrozen;

            uint32 m_uiBlast_Timer;
            uint32 m_uiDeathDelay_Timer;
            uint32 m_uiImmolation_Timer;

            void Reset() override
            {
                m_bIsFrozen = false;

                m_uiBlast_Timer = 20000;
                m_uiDeathDelay_Timer = 0;
                m_uiImmolation_Timer = 5000;
            }

            void AttackStart(Unit* who) override
            {
                if (me->Attack(who, true))
                {
                    me->GetThreatManager().AddThreat(who, 0.0f);
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);

                    if (!m_bIsFrozen)
                        me->GetMotionMaster()->MoveChase(who);
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage > me->GetHealth())
                {
                    me->UpdateEntry(NPC_BRITTLE_GOLEM);
                    me->SetHealth(1);
                    damage = 0;
                    me->RemoveAllAuras();
                    me->AttackStop();
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    if (me->IsNonMeleeSpellCasted(false))
                        me->InterruptNonMeleeSpells(false);
                    if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
                        me->GetMotionMaster()->MovementExpired();
                    m_bIsFrozen = true;
                }
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                // this is the dummy effect of the spells
                if (spell->Id == SPELL_SHATTER || spell->Id == SPELL_SHATTER_H)
                {
                    if (me->GetEntry() == NPC_BRITTLE_GOLEM)
                        me->DespawnOrUnsummon();
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                {
                    if (instance->GetData(TYPE_VOLKHAN) != IN_PROGRESS)
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }

                // Return since we have no target or if we are frozen
                if (!UpdateVictim() || m_bIsFrozen)
                    return;

                if (m_uiBlast_Timer <= diff)
                {
                    DoCast(me, SPELL_BLAST_WAVE);
                    m_uiBlast_Timer = 20000;
                }
                else
                    m_uiBlast_Timer -= diff;

                if (m_uiImmolation_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_IMMOLATION_STRIKE);
                    m_uiImmolation_Timer = 5000;
                }
                else
                    m_uiImmolation_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_molten_golemAI(creature);
        }
};

void AddSC_boss_volkhan()
{
    new boss_volkhan();
    new npc_molten_golem();
}
