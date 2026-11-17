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

/* Script Data Start
SDName: Boss svala
SDAuthor: for I-core
SD%Complete:
SDComment:
SDCategory:
Script Data End */

#include "ScriptPCH.h"
#include "utgarde_pinnacle.h"

enum Spells
{
    SPELL_CALL_FLAMES                        = 48258,
    SPELL_SINISTER_STRIKE                    = 15667,
    SPELL_SINISTER_STRIKE_H                  = 59409,
    SPELL_SVALA_TRANSFORMING1                = 54140,
    SPELL_SVALA_TRANSFORMING2                = 54205,
    SPELL_BALL_OF_FLAME                      = 48246,
    SPELL_TRANSFORMING_CHANNEL               = 54142,
    SPELL_ARTHAS_VISUAL                      = 54134,
    SPELL_RITUAL_OF_THE_SWORD_TARGETING      = 54148,
    SPELL_RITUAL_PREPARATION                 = 48267,
    SPELL_SUMMON_RITUAL_CHANNELER_1          = 48271,
    SPELL_SUMMON_RITUAL_CHANNELER_2          = 48274,
    SPELL_SUMMON_RITUAL_CHANNELER_3          = 48275,
    SPELL_RITUAL_OF_THE_SWORD_DISARM         = 54159,
    SPELL_SIMPLE_TELEPORT                    = 12980,
    SPELL_RITUAL_OF_THE_SWORD_TELEPORT       = 48276,
    SPELL_RITUAL_STRIKE_MISSILE              = 48331,
};

enum Yells
{
    // Svala
    SAY_DIALOG_WITH_ARTHAS_1                 = 0,

    // Svala Sorrowgrave
    SAY_DIALOG_WITH_ARTHAS_2                 = 0,
    SAY_DIALOG_WITH_ARTHAS_3                 = 1,
    SAY_AGGRO                                = 2,
    SAY_SLAY                                 = 3,
    SAY_DEATH                                = 4,
    SAY_SACRIFICE_PLAYER                     = 5,

    // Image of Arthas
    SAY_DIALOG_OF_ARTHAS_1                   = 0,
    SAY_DIALOG_OF_ARTHAS_2                   = 1
};

enum Npcs
{
    NPC_ARTHAS                               = 29280, // Image of Arthas
    NPC_RITUAL_CHANNELER                     = 27281,
    NPC_SCOURGE_HULK                         = 26555,
};

enum ChannelerSpells
{
    // ritual channeler's spells
    SPELL_PARALYZE                           = 48278,
    SPELL_SHADOWS_IN_THE_DARK                = 59407
};

enum Misc
{
    DATA_SVALA_DISPLAY_ID                    = 25944,
};

enum IntroPhase
{
    IDLE,
    INTRO,
    FINISHED
};

enum CombatPhase
{
    NORMAL,
    SACRIFICING
};

enum DisplayIds
{
    DISPLAY_SVALA_WITH_SWORD                = 26096,
};

class boss_svala : public CreatureScript
{
    public:
        boss_svala() : CreatureScript("boss_svala") { }

        struct boss_svalaAI : public ScriptedAI
        {
            ObjectGuid uiDoodadMirror;
        
            boss_svalaAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 uiIntroTimer;

            uint8 uiIntroPhase;

            ObjectGuid uiSvalaGuid;
            ObjectGuid uiArthas;

            IntroPhase Phase;

            InstanceScript* instance;

            void Reset() override
            {
                Phase = IDLE;
                uiIntroTimer = 1 * IN_MILLISECONDS;
                uiIntroPhase = 0;
                uiArthas = ObjectGuid::Empty;

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);

                if (instance)
                    if (instance->GetData(DATA_SVALA_SORROWGRAVE_EVENT) != DONE)
                        instance->SetData(DATA_SVALA_SORROWGRAVE_EVENT, NOT_STARTED);
                    else
                        Phase = FINISHED;

                uiDoodadMirror = instance? instance->GetGuidData(DATA_DOODAD_UTGARDE_MIRROR_FX01) : ObjectGuid::Empty;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!who)
                    return;

                if (Phase == IDLE && who->isTargetableForAttack() && me->IsHostileTo(who) && me->IsWithinDistInMap(who, 40))
                {
                    Phase = INTRO;
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                    if (Creature* pArthas = me->SummonCreature(NPC_ARTHAS, 295.81f, -366.16f, 92.57f, 1.58f, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        pArthas->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_DISABLE_MOVE);
                        uiArthas = pArthas->GetGUID();
                        if (GameObject* go = GameObject::GetGameObject(*me, uiDoodadMirror))
                            go->Use(me);
                    }
                
                }
            }

            void AttackStart(Unit* who) override { }

            void UpdateAI(uint32 diff) override
            {
                if (Phase != INTRO)
                    return;

                if (uiIntroTimer <= diff)
                {
                    switch (uiIntroPhase)
                    {
                        case 0:
                            Talk(SAY_DIALOG_WITH_ARTHAS_1);
                            ++uiIntroPhase;
                            uiIntroTimer = 8 * IN_MILLISECONDS;
                            break;
                        case 1:
                            if (Creature* pArthas = Creature::GetCreature(*me, uiArthas))
                                pArthas->AI()->Talk(SAY_DIALOG_OF_ARTHAS_1);
                            ++uiIntroPhase;
                            uiIntroTimer = 10 * IN_MILLISECONDS;
                            break;
                        case 2:
                            DoCast(me, SPELL_SVALA_TRANSFORMING1);
                            if (Creature* pArthas = Creature::GetCreature(*me, uiArthas))
                                pArthas->CastSpell(me, SPELL_TRANSFORMING_CHANNEL, false);
                            me->GetMotionMaster()->MovePoint(0, 296.0, -346.0, 95.0);
                            ++uiIntroPhase;
                            uiIntroTimer = 8 * IN_MILLISECONDS;
                            break;
                        case 3:
                            DoCast(me, SPELL_SVALA_TRANSFORMING2);
                            ++uiIntroPhase;
                            uiIntroTimer = 200;
                            break;
                        case 4:
                            if (Creature* pSvalaSorrowgrave = me->SummonCreature(NPC_SVALA_SORROWGRAVE, 296.632f, -346.075f, 95.6307f, 1.58f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, Milliseconds(60 * IN_MILLISECONDS)))
                            {
                                if (Creature* pArthas = Creature::GetCreature(*me, uiArthas))
                                    pArthas->CastStop();
                                uiSvalaGuid = pSvalaSorrowgrave->GetGUID();
                                pSvalaSorrowgrave->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
                                me->SetDisplayId(DATA_SVALA_DISPLAY_ID);
                                me->SetVisible(false);
                                pSvalaSorrowgrave->AI()->Talk(SAY_DIALOG_WITH_ARTHAS_2);
                                ++uiIntroPhase;
                                uiIntroTimer = 12 * IN_MILLISECONDS;
                            }
                            else 
                               Reset();
                            break;
                        case 5:
                            if (Creature* pArthas = Creature::GetCreature(*me, uiArthas))
                                pArthas->AI()->Talk(SAY_DIALOG_OF_ARTHAS_2);
                            ++uiIntroPhase;
                            uiIntroTimer = 9 * IN_MILLISECONDS;
                            break;

                        case 6:
                             if (Creature* pSvalaSorrowgrave = Creature::GetCreature(*me, uiSvalaGuid))
                                 pSvalaSorrowgrave->AI()->Talk(SAY_DIALOG_WITH_ARTHAS_3);
                             if (Creature* pArthas = Creature::GetCreature(*me, uiArthas))
                                pArthas->SetVisible(false);
                             ++uiIntroPhase;
                             uiIntroTimer = 15 * IN_MILLISECONDS;
                             break;
                        case 7:
                            if (Creature* pSvalaSorrowgrave = Creature::GetCreature(*me, uiSvalaGuid))
                                pSvalaSorrowgrave->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
                            if (Creature* pArthas = Creature::GetCreature(*me, uiArthas))
                                pArthas->DisappearAndDie();
                            Phase = FINISHED;
                    }
                } else uiIntroTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_svalaAI(creature);
        }
};

class npc_ritual_channeler : public CreatureScript
{
    public:
        npc_ritual_channeler() : CreatureScript("npc_ritual_channeler") { }

        struct npc_ritual_channelerAI : public ScriptedAI
        {
            npc_ritual_channelerAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                DoCast(me, SPELL_SIMPLE_TELEPORT, true);
                me->m_Events.Schedule(1200, [this]()
                {
                    if (IsHeroic())
                        DoCast(me, SPELL_SHADOWS_IN_THE_DARK);

                    std::list<Player*> players = me->GetNearestPlayersList(100.0f);
                    auto itr = std::find_if(players.begin(), players.end(), [](Player* player) { return player->HasAura(SPELL_RITUAL_PREPARATION); });
                    if (itr != players.end())
                    {
                        DoCast(*itr, SPELL_PARALYZE);
                        DoZoneInCombat();
                    }
                });
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ritual_channelerAI(creature);
        }
};

class boss_svala_sorrowgrave : public CreatureScript
{
    public:
        boss_svala_sorrowgrave() : CreatureScript("boss_svala_sorrowgrave") { }

        struct boss_svala_sorrowgraveAI : public ScriptedAI
        {
            boss_svala_sorrowgraveAI(Creature* creature) : ScriptedAI(creature), summons(creature)
            {
                instance = creature->GetInstanceScript();
            }
        
            bool bFlames;
            bool bMove;

            ObjectGuid uiFlameBrazier_1;
            ObjectGuid uiFlameBrazier_2;
            uint32 uiSinsterStrikeTimer;
            uint32 uiCallFlamesTimer;
            uint8 uiFlamesCount;
            uint32 uiSacrificeTimer;
            uint32 uiMoveTimer;
            uint32 uiHealthAmountModifier;

            ObjectGuid uiDoodadMirror;

            CombatPhase Phase;

            SummonList summons;
        
            InstanceScript* instance;

            void Reset() override
            {
                me->CastStop();
                me->SetReactState(REACT_DEFENSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED);
                uiFlameBrazier_1 = instance? instance->GetGuidData(DATA_FLAME_BRAZIER_1) : ObjectGuid::Empty;
                uiFlameBrazier_2 = instance? instance->GetGuidData(DATA_FLAME_BRAZIER_2) : ObjectGuid::Empty;
                uiDoodadMirror   = instance? instance->GetGuidData(DATA_DOODAD_UTGARDE_MIRROR_FX01) : ObjectGuid::Empty;
                uiSinsterStrikeTimer = 7 * IN_MILLISECONDS;
                uiCallFlamesTimer = 10 * IN_MILLISECONDS;
                uiSacrificeTimer = 2 * IN_MILLISECONDS;
                uiFlamesCount = 0;
                uiHealthAmountModifier = 1;
                uiMoveTimer = 23 * IN_MILLISECONDS;
            
                bFlames = false;
                bMove = true;

                Phase = NORMAL;

                summons.DespawnAll();

                if (instance)
                    instance->SetData(DATA_SVALA_SORROWGRAVE_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->SetReactState(REACT_AGGRESSIVE);
                Talk(SAY_AGGRO);

                if (instance)
                    instance->SetData(DATA_SVALA_SORROWGRAVE_EVENT, IN_PROGRESS);
                
                if (GameObject* go = GameObject::GetGameObject(*me, uiDoodadMirror))
                    go->ResetDoorOrButton();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }
        
            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_RITUAL_OF_THE_SWORD_TARGETING)
                {
                    me->InterruptNonMeleeSpells(false);
                    DoCast(target, SPELL_RITUAL_PREPARATION, true);
                    Talk(SAY_SACRIFICE_PLAYER);
                    DoCast(me, SPELL_SUMMON_RITUAL_CHANNELER_1, true);
                    DoCast(me, SPELL_SUMMON_RITUAL_CHANNELER_2, true);
                    DoCast(me, SPELL_SUMMON_RITUAL_CHANNELER_3, true);
                    me->GetMotionMaster()->Clear();
                    DoCast(me, SPELL_RITUAL_OF_THE_SWORD_DISARM, true);
                    DoCast(me, SPELL_RITUAL_OF_THE_SWORD_TELEPORT);
                    me->AttackStop();
                    Phase = SACRIFICING;
                }
            }

            void KilledUnit(Unit* victim) override
            {
                Talk(SAY_SLAY);
            }

            void DamageDealt(Unit* victim, uint32& damage, DamageEffectType) override
            {
                if (victim->GetEntry() == NPC_SCOURGE_HULK && damage >= victim->GetHealth())
                    instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, victim->GetEntry(), 1, victim);
            }

            void UpdateAI(uint32 diff) override
            {
                if (Phase == NORMAL)
                {
                    if (!bMove && uiMoveTimer > diff)
                    {
                        uiMoveTimer -= diff;
                        return;
                    }
                    else if (!bMove)
                    {
                        DoStartMovement(me->GetVictim());
                        bMove = true;
                    }

                    if (!UpdateVictim())
                        return;

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;

                    if (uiSinsterStrikeTimer <= diff)
                    {
                        DoCast(me->GetVictim(), DUNGEON_MODE(SPELL_SINISTER_STRIKE, SPELL_SINISTER_STRIKE_H));
                        uiSinsterStrikeTimer = urand(5 * IN_MILLISECONDS, 9 * IN_MILLISECONDS);
                    } else uiSinsterStrikeTimer -= diff;

                    if (uiCallFlamesTimer <= diff) // move to send event scripts?
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        {
                            if (!bFlames)
                            {
                                DoCast(target, SPELL_CALL_FLAMES);
                                bFlames = true;
                            }

                            if (uiFlamesCount < 3)
                            {
                                if (Creature* pBrazier = Creature::GetCreature(*me, RAND(uiFlameBrazier_1, uiFlameBrazier_2))) 
                                {
                                    if (IsHeroic()) // find correct spell
                                    {
                                        int dmg = 3825 + rand() % 1350;
                                        pBrazier->CastCustomSpell(pBrazier, SPELL_BALL_OF_FLAME, &dmg, 0, 0, true);
                                    }
                                    else
                                        pBrazier->CastSpell(pBrazier, SPELL_BALL_OF_FLAME, true);
                                }
                                uiCallFlamesTimer = 1 * IN_MILLISECONDS;
                                ++uiFlamesCount;
                            }
                            else
                            { 
                                bFlames = false;
                                uiCallFlamesTimer = urand(8 * IN_MILLISECONDS, 12 * IN_MILLISECONDS);
                                uiFlamesCount = 0;
                            }
                        }
                    } else uiCallFlamesTimer -= diff;

                    if ((me->GetHealth() * 100 / me->GetMaxHealth()) < (100 - (50 * uiHealthAmountModifier)))
                    {
                        ++uiHealthAmountModifier;
                        DoCastAOE(SPELL_RITUAL_OF_THE_SWORD_TARGETING, true);
                    }

                    DoMeleeAttackIfReady();
                }
                else // SACRIFICING
                {
                    if (uiSacrificeTimer <= diff)
                    {
                        DoCast(me, SPELL_RITUAL_STRIKE_MISSILE, true);
                        bMove = false;
                        Phase = NORMAL;
                        uiMoveTimer = 25 * IN_MILLISECONDS;
                        uiSacrificeTimer = 2 * IN_MILLISECONDS;
                        uiSinsterStrikeTimer = urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS);
                        uiCallFlamesTimer = urand(13 * IN_MILLISECONDS, 18 * IN_MILLISECONDS);
                    }
                    else uiSacrificeTimer -= diff;
                }
            }

            void JustDied(Unit* killer) override
            {
                if (instance)
                {
                    Creature* pSvala = Unit::GetCreature(*me, instance->GetGuidData(DATA_SVALA));
                    if (pSvala && pSvala->IsAlive())
                        killer->Kill(pSvala);

                    instance->SetData(DATA_SVALA_SORROWGRAVE_EVENT, DONE);
                }
                Talk(SAY_DEATH);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_svala_sorrowgraveAI(creature);
        }
};

void AddSC_boss_svala()
{
    new boss_svala();
    new npc_ritual_channeler();
    new boss_svala_sorrowgrave();
}
