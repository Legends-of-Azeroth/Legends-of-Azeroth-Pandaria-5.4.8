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
SDName: Boss Loken
SD%Complete: 60%
SDComment: Missing intro. Remove hack of Pulsing Shockwave when core supports. Aura is not working (59414)
SDCategory: Halls of Lightning
EndScriptData */

#include "ScriptPCH.h"
#include "halls_of_lightning.h"

enum Spells
{
    SPELL_ARC_LIGHTNING                           = 52921,
    SPELL_LIGHTNING_NOVA_N                        = 52960,
    SPELL_LIGHTNING_NOVA_H                        = 59835,

    SPELL_PULSING_SHOCKWAVE_N                     = 52961,
    SPELL_PULSING_SHOCKWAVE_H                     = 59836,
    SPELL_PULSING_SHOCKWAVE_AURA                  = 59414,
};

enum Yells
{
    SAY_AGGRO                                     = 0,
    SAY_INTRO_1                                   = 1,
    SAY_INTRO_2                                   = 2,
    SAY_NOVA                                      = 3,
    SAY_SLAY                                      = 4,
    SAY_DEATH                                     = 5,
    SAY_75HEALTH                                  = 6,
    SAY_50HEALTH                                  = 7,
    SAY_25HEALTH                                  = 8,
    EMOTE_NOVA                                    = 9,
};

enum Misc
{
    ACHIEV_TIMELY_DEATH_START_EVENT               = 1867,
};

class boss_loken : public CreatureScript
{
    public:
        boss_loken() : CreatureScript("boss_loken") { }

        struct boss_lokenAI : public ScriptedAI
        {
            boss_lokenAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            bool m_bIsAura;
            bool bHit;
            bool introDone = false;

            uint32 m_uiArcLightning_Timer;
            uint32 m_uiLightningNova_Timer;
            uint32 m_uiPulsingShockwave_Timer;
            uint32 m_uiResumePulsingShockwave_Timer;
            uint32 m_uiHealthAmountModifier;
            uint32 AchievTimer;

            void Reset() override
            {
                m_bIsAura = false;

                m_uiArcLightning_Timer = 15000;
                m_uiLightningNova_Timer = 20000;
                m_uiPulsingShockwave_Timer = 2000;
                m_uiResumePulsingShockwave_Timer = 15000;
                AchievTimer = 0;
                m_uiHealthAmountModifier = 1;

                if (instance)
                {
                    instance->SetData(TYPE_LOKEN, NOT_STARTED);
                    //instance->DoStopTimedAchievement(CRITERIA_START_TYPE_EVENT, ACHIEV_TIMELY_DEATH_START_EVENT);
                }

                me->SetStandState(UNIT_STAND_STATE_SIT);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                ScriptedAI::MoveInLineOfSight(who);

                if (!introDone && who->GetTypeId() == TYPEID_PLAYER && me->GetExactDist2d(who) <= 75.0f && who->GetPositionZ() >= 50.0f)
                {
                    introDone = true;
                    Talk(SAY_INTRO_1);
                    me->m_Events.Schedule(20500, [this]() { Talk(SAY_INTRO_2); });
                }
            }

            void SpellHitTarget(Unit* target, const SpellInfo* spell) override
            {
                if (spell->Id==SPELL_ARC_LIGHTNING && bHit)
                    if (rand() % 100 < DUNGEON_MODE(65, 80))
                    {
                        bHit = false;
                        DoCast(target->GetNextRandomRaidMemberOrPet(10.0f), SPELL_ARC_LIGHTNING);
                    }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
               
                if (instance)
                {
                    instance->SetData(TYPE_LOKEN, IN_PROGRESS);
                    //instance->DoStartTimedAchievement(CRITERIA_START_TYPE_EVENT, ACHIEV_TIMELY_DEATH_START_EVENT);
                   
                    if (instance->GetData(TYPE_IONAR) != DONE)
                    {
                        Map* map = me->GetMap();
                        if (map->IsDungeon())
                        {
                            Map::PlayerList const& PlayerList = map->GetPlayers();
                            if (PlayerList.isEmpty())
                                return;
     
                            float fDist = 0;
                            for (auto&& ref : PlayerList)
                                if (Player* player = ref.GetSource())
                                    if (player->IsAlive() && !player->IsGameMaster())
                                        me->DealDamage(player, player->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        }
                    }
                }

                me->SetStandState(UNIT_STAND_STATE_STAND);
            }

            void JustDied(Unit* /*killer*/) override
            {
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PULSING_SHOCKWAVE_AURA); // ID - 59414 Pulsing Shockwave Aura have duration -1
                Talk(SAY_DEATH);

                if (instance)
                    instance->SetData(TYPE_LOKEN, DONE);

                AchievementEntry const* AchievTimelyDeath = sAchievementStore.LookupEntry(ACHIEV_TIMELY_DEATH_START_EVENT);
                Map* map = me->GetMap();
                            
                if (IsHeroic() && AchievTimer < 120000 && map && map->IsDungeon() && AchievTimelyDeath)
                {
                    for (auto&& ref : map->GetPlayers())
                        ref.GetSource()->CompletedAchievement(AchievTimelyDeath);
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_SLAY);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                // Achievement counter
                if (instance)
                    if (instance->GetData(TYPE_LOKEN) == IN_PROGRESS)
                        AchievTimer += diff;

                if (m_bIsAura)
                {
                    // workaround for PULSING_SHOCKWAVE
                    if (m_uiPulsingShockwave_Timer <= diff)
                    {
                        Map* map = me->GetMap();
                        if (map->IsDungeon())
                        {
                            Map::PlayerList const& PlayerList = map->GetPlayers();

                            if (PlayerList.isEmpty())
                                return;

                            float fDist = 0;
                            for (auto&& ref : PlayerList)
                            {
                                if (Player* player = ref.GetSource())
                                    if (player->IsAlive() && player->isTargetableForAttack())
                                    {
                                        float fDistTemp = me->GetExactDist(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());
                                        if (fDistTemp>fDist)
                                            fDist = fDistTemp;
                                    }
                            }

                            int32 dmg = int32(DUNGEON_MODE(100, 150) * fDist);
                            int32 range = 200;
                            me->CastCustomSpell(me->GetVictim(), DUNGEON_MODE(52942, 59837), &dmg, &range, 0, false);
                        }
                        m_uiPulsingShockwave_Timer = 2000;
                    } else m_uiPulsingShockwave_Timer -= diff;
                }
                else
                {
                    if (m_uiResumePulsingShockwave_Timer <= diff)
                    {
                        // breaks at movement, can we assume when it's time, this spell is casted and also must stop movement?
                        DoCast(me, SPELL_PULSING_SHOCKWAVE_AURA, true);

                        DoCast(me, DUNGEON_MODE(SPELL_PULSING_SHOCKWAVE_N,SPELL_PULSING_SHOCKWAVE_H)); // need core support
                        m_bIsAura = true;
                        m_uiResumePulsingShockwave_Timer = 0;
                    }
                    else
                        m_uiResumePulsingShockwave_Timer -= diff;
                }

                if (m_uiArcLightning_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_ARC_LIGHTNING);

                    bHit=true;
                    m_uiArcLightning_Timer = 15000 + rand() % 1000;
                }
                else
                    m_uiArcLightning_Timer -= diff;

                if (m_uiLightningNova_Timer <= diff)
                {
                    Talk(SAY_NOVA);
                    Talk(EMOTE_NOVA);
                    DoCast(me, DUNGEON_MODE(SPELL_LIGHTNING_NOVA_N,SPELL_LIGHTNING_NOVA_H));

                    m_bIsAura = false;
                    m_uiResumePulsingShockwave_Timer = DUNGEON_MODE(5000, 4000); // Pause Pulsing Shockwave aura
                    m_uiLightningNova_Timer = 20000 + rand() % 1000;
                }
                else
                    m_uiLightningNova_Timer -= diff;

                // Health check
                if (HealthBelowPct(100 - 25 * m_uiHealthAmountModifier))
                {
                    switch (m_uiHealthAmountModifier)
                    {
                        case 1: Talk(SAY_75HEALTH); break;
                        case 2: Talk(SAY_50HEALTH); break;
                        case 3: Talk(SAY_25HEALTH); break;
                    }

                    ++m_uiHealthAmountModifier;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_lokenAI(creature);
        }
};

void AddSC_boss_loken()
{
    new boss_loken();
}
