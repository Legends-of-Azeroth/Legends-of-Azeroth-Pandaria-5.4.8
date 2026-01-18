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

/* ScriptData
SDName: Boss_Kalecgos
SD%Complete: 95
SDComment:
SDCategory: Sunwell_Plateau
EndScriptData */

#include "ScriptPCH.h"
#include "sunwell_plateau.h"

enum Yells
{
    SAY_SATH_AGGRO                              = 0,
    SAY_SATH_SLAY                               = 1,
    SAY_SATH_DEATH                              = 2,
    SAY_SATH_SPELL1                             = 3,
    SAY_SATH_SPELL2                             = 4,

    SAY_EVIL_AGGRO                              = 0,
    SAY_EVIL_SLAY                               = 1,
    SAY_GOOD_PLRWIN                             = 2,
    SAY_EVIL_ENRAGE                             = 3,

    SAY_GOOD_AGGRO                              = 0,
    SAY_GOOD_NEAR_DEATH                         = 1,
    SAY_GOOD_NEAR_DEATH2                        = 2,
};

enum Spells
{
    AURA_SUNWELL_RADIANCE                       = 45769,
    AURA_SPECTRAL_EXHAUSTION                    = 44867,
    AURA_SPECTRAL_REALM_FORCE_REACTION          = 44852,
    AURA_SPECTRAL_REALM                         = 46021,
    AURA_SPECTRAL_INVISIBILITY                  = 44801,
    AURA_DEMONIC_VISUAL                         = 44800,

    SPELL_SPECTRAL_BLAST                        = 44869,
    SPELL_SPECTRAL_BLAST_SELF                   = 44866,
    SPELL_SPECTRAL_BLAST_SELF_VISUAL            = 46648,
    SPELL_TELEPORT_SPECTRAL                     = 46019,
    SPELL_ARCANE_BUFFET                         = 45018,
    SPELL_FROST_BREATH                          = 44799,
    SPELL_TAIL_LASH                             = 45122,

    SPELL_BANISH                                = 44836,
    SPELL_TRANSFORM_KALEC                       = 44670,
    SPELL_ENRAGE                                = 44807,

    SPELL_CORRUPTION_STRIKE                     = 45029,
    SPELL_AGONY_CURSE                           = 45032,
    SPELL_SHADOW_BOLT                           = 45031,

    SPELL_HEROIC_STRIKE                         = 45026,
    SPELL_REVITALIZE                            = 45027
};

enum SWPActions
{
    DO_ENRAGE                                   =  1,
    DO_BANISH                                   =  2,
};

#define GO_FAILED   "You are unable to use this currently."

#define FLY_X   1679
#define FLY_Y   900
#define FLY_Z   82

#define CENTER_X    1705
#define CENTER_Y    930
#define RADIUS      30

#define DRAGON_REALM_Z  53.079f
#define DEMON_REALM_Z   -74.558f

#define MAX_PLAYERS_IN_SPECTRAL_REALM 0 //over this, teleport object won't work, 0 disables check

uint32 wildMagic[] = { 44978, 45001, 45002, 45004, 45006, 45010 };

class boss_kalecgos : public CreatureScript
{
    public:
        boss_kalecgos() : CreatureScript("boss_kalecgos") { }

        struct boss_kalecgosAI : public ScriptedAI
        {
            boss_kalecgosAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                justReset = false;
                me->setActive(true);
            }

            InstanceScript* instance;
            uint32 arcaneBuffetTimer;
            uint32 frostBreathTimer;
            uint32 wildMagicTimer;
            uint32 spectralBlastTimer;
            uint32 tailLashTimer;
            uint32 checkTimer;
            uint32 talkTimer;
            uint32 talkSequence;
            uint32 ResetTimer;
            bool isFriendly;
            bool isEnraged;
            bool isBanished;
            bool justReset;

            void Reset() override
            {
                if (instance)
                {
                    if (instance->GetData(DATA_KALECGOS_EVENT) != DONE)
                        instance->SetData(DATA_KALECGOS_EVENT, NOT_STARTED);

                    if (Creature* Sath = Unit::GetCreature(*me, instance->GetGuidData(DATA_SATHROVARR)))
                        Sath->AI()->EnterEvadeMode();
                }

                me->SetFaction(14);
                if (!justReset) //first reset at create
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                    me->SetFlying(false);
                    me->SetVisible(true);
                    me->SetStandState(UNIT_STAND_STATE_SLEEP);
                }
                me->SetFullHealth();//dunno why it does not resets health at evade..
                arcaneBuffetTimer = 8000;
                frostBreathTimer = 15000;
                wildMagicTimer = 10000;
                tailLashTimer = 25000;
                spectralBlastTimer = urand(20000,25000);
                checkTimer = 1000;
                ResetTimer = 30000;

                talkTimer = 0;
                talkSequence = 0;
                isFriendly = false;
                isEnraged = false;
                isBanished = false;
            }

            void EnterEvadeMode() override
            {
                justReset = true;
                me->SetVisible(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                ScriptedAI::EnterEvadeMode();
            }

            void DoAction(int32 param) override
            {
                switch (param)
                {
                    case DO_ENRAGE:
                        isEnraged = true;
                        me->CastSpell(me, SPELL_ENRAGE, true);
                        break;
                    case DO_BANISH:
                        isBanished = true;
                        me->CastSpell(me, SPELL_BANISH, true);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (talkTimer)
                {
                    if (!talkSequence)
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                        me->InterruptNonMeleeSpells(true);
                        me->RemoveAllAuras();
                        me->DeleteThreatList();
                        me->CombatStop();
                        ++talkSequence;
                    }
                    if (talkTimer <= diff)
                    {
                        if (isFriendly)
                            GoodEnding();
                        else
                            BadEnding();
                        ++talkSequence;
                    } else talkTimer -= diff;
                }
                else
                {
                    if (justReset)
                    {
                        if (ResetTimer <= diff)
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
                            me->SetFlying(false);
                            me->SetVisible(true);
                            me->SetStandState(UNIT_STAND_STATE_SLEEP);
                            ResetTimer = 10000;
                            justReset = false;
                        } else ResetTimer -= diff;
                        return;
                    }
                    if (!UpdateVictim())
                        return;

                    if (checkTimer <= diff)
                    {
                        if (me->GetDistance(CENTER_X, CENTER_Y, DRAGON_REALM_Z) >= 75)
                        {
                            me->AI()->EnterEvadeMode();
                            return;
                        }
                        if (HealthBelowPct(10) && !isEnraged)
                        {
                            if (Creature* Sath = Unit::GetCreature(*me, instance->GetGuidData(DATA_SATHROVARR)))
                                Sath->AI()->DoAction(DO_ENRAGE);
                            DoAction(DO_ENRAGE);
                        }
                        if (!isBanished && HealthBelowPct(1))
                        {
                            if (Creature* Sath = Unit::GetCreature(*me, instance->GetGuidData(DATA_SATHROVARR)))
                            {
                                if (Sath->HasAura(SPELL_BANISH))
                                {
                                    Sath->DealDamage(Sath, Sath->GetHealth());
                                    return;
                                }
                                else
                                    DoAction(DO_BANISH);
                            }
                            else
                            {
                                TC_LOG_ERROR("scripts", "TSCR: Didn't find Shathrowar, kalecgos event reseted.");
                                EnterEvadeMode();
                                return;
                            }
                        }
                        checkTimer = 1000;
                    } else checkTimer -= diff;

                    if (arcaneBuffetTimer <= diff)
                    {
                        DoCastAOE(SPELL_ARCANE_BUFFET);
                        arcaneBuffetTimer = 8000;
                    } else arcaneBuffetTimer -= diff;

                    if (frostBreathTimer <= diff)
                    {
                        DoCastAOE(SPELL_FROST_BREATH);
                        frostBreathTimer = 15000;
                    } else frostBreathTimer -= diff;

                    if (tailLashTimer <= diff)
                    {
                        DoCastAOE(SPELL_TAIL_LASH);
                        tailLashTimer = 15000;
                    } else tailLashTimer -= diff;

                    if (wildMagicTimer <= diff)
                    {
                        DoCastAOE(wildMagic[rand()%6]);
                        wildMagicTimer = 20000;
                    } else wildMagicTimer -= diff;

                    if (spectralBlastTimer <= diff)
                    {
                        spectralBlastTimer = 1000;
                        DoCastAOE(SPELL_SPECTRAL_BLAST, true);
                    } else spectralBlastTimer -= diff;

                    DoMeleeAttackIfReady();
                }
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (justReset)//boss is invisible, don't attack
                    return;

                if (!me->GetVictim() && who->isTargetableForAttack() && (me->IsHostileTo(who)))
                {
                    float attackRadius = me->GetAttackDistance(who);
                    if (me->IsWithinDistInMap(who, attackRadius))
                        AttackStart(who);
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (damage >= me->GetHealth() && attacker != me)
                    damage = 0;
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (!spell || spell->Id != SPELL_SPECTRAL_BLAST)
                    return;

                spectralBlastTimer = 20000+rand()%5000;
                target->RemoveAurasDueToSpell(SPELL_ARCANE_BUFFET);
                target->CastSpell(target, SPELL_SPECTRAL_BLAST_SELF, true);
                DoCast(target, SPELL_SPECTRAL_BLAST_SELF_VISUAL, true);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->SetStandState(UNIT_STAND_STATE_STAND);
                Talk(SAY_EVIL_AGGRO);
                DoZoneInCombat();

                if (instance && instance->GetData(DATA_KALECGOS_EVENT) != DONE)
                    instance->SetData(DATA_KALECGOS_EVENT, IN_PROGRESS);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_EVIL_SLAY);
            }

            void MovementInform(uint32 type,uint32 id) override
            {
                if (type != EFFECT_MOTION_TYPE || id != 1)
                    return;

                me->SetVisible(false);
                if (isFriendly)
                {
                    me->setDeathState(JUST_DIED);

                    for (auto&& itr : me->GetMap()->GetPlayers())
                    {
                        if (Player* player = itr.GetSource())
                            me->GetMap()->ToInstanceMap()->PermBindAllPlayers(player);
                    }
                }
                else
                {
                    me->GetMotionMaster()->MoveTargetedHome();
                    talkTimer = 1000;
                }
            }

            void GoodEnding()
            {
                switch (talkSequence)
                {
                    case 1:
                        me->SetFaction(35);
                        talkTimer = 1000;
                        break;
                    case 2:
                        Talk(SAY_GOOD_PLRWIN);
                        talkTimer = 10000;
                        break;
                    case 3:
                        me->SetFlying(true);
                        me->GetMotionMaster()->MoveTakeoff(1, Position { FLY_X, FLY_Y, FLY_Z });
                        talkTimer = 600000;
                        break;
                    default:
                        break;
                }
            }

            void BadEnding()
            {
                switch (talkSequence)
                {
                    case 1:
                        Talk(SAY_EVIL_ENRAGE);
                        talkTimer = 3000;
                        break;
                    case 2:
                        me->SetFlying(true);
                        me->GetMotionMaster()->MoveTakeoff(1, Position { FLY_X, FLY_Y, FLY_Z });
                        talkTimer = 15000;
                        break;
                    case 3:
                        EnterEvadeMode();
                        break;
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_kalecgosAI(creature);
        }
};

class boss_kalec : public CreatureScript
{
    public:
        boss_kalec() : CreatureScript("boss_kalec") { }

        struct boss_kalecAI : public ScriptedAI
        {
            boss_kalecAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }
            
            InstanceScript* instance;
            uint32 revitalizeTimer;
            uint32 heroicStrikeTimer;
            uint32 yellTimer;
            uint32 yellSequence;
            bool isEnraged; // if demon is enraged

            void Reset() override
            {
                revitalizeTimer = 5000;
                heroicStrikeTimer = 3000;
                yellTimer = 5000;
                yellSequence = 0;

                isEnraged = false;
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (attacker->GetGUID() != instance->GetGuidData(DATA_SATHROVARR))
                    damage = 0;
                else if (isEnraged)
                    damage *= 3;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->HasAura(AURA_SPECTRAL_INVISIBILITY))
                    me->CastSpell(me, AURA_SPECTRAL_INVISIBILITY, true);
                if (!UpdateVictim())
                    return;

                if (yellTimer <= diff)
                {
                    switch (yellSequence)
                    {
                        case 0:
                            Talk(SAY_GOOD_AGGRO);
                            ++yellSequence;
                            break;
                        case 1:
                            if (HealthBelowPct(50))
                            {
                                Talk(SAY_GOOD_NEAR_DEATH);
                                ++yellSequence;
                            }
                            break;
                        case 2:
                            if (HealthBelowPct(10))
                            {
                                Talk(SAY_GOOD_NEAR_DEATH2);
                                ++yellSequence;
                            }
                            break;
                        default:
                            break;
                    }
                    yellTimer = 5000;
                }

                if (revitalizeTimer <= diff)
                {
                    DoCast(me, SPELL_REVITALIZE);
                    revitalizeTimer = 5000;
                } else revitalizeTimer -= diff;

                if (heroicStrikeTimer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_HEROIC_STRIKE);
                    heroicStrikeTimer = 2000;
                } else heroicStrikeTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_kalecAI(creature);
        }
};

class go_kalecgos_teleporter : public GameObjectScript
{
    public:
        go_kalecgos_teleporter() : GameObjectScript("go_kalecgos_teleporter") { }

        bool OnGossipHello(Player* player, GameObject* go)
        {
            uint8 spectralPlayers = 0;
            Map* map = go->GetMap();
            if (!map->IsDungeon())
                return true;

            for (auto&& itr : map->GetPlayers())
            {
                if (itr.GetSource() && itr.GetSource()->GetPositionZ() < DEMON_REALM_Z + 5)
                    ++spectralPlayers;
            }

            uint8 maxSpectralPlayers =  MAX_PLAYERS_IN_SPECTRAL_REALM;
            if (player->HasAura(AURA_SPECTRAL_EXHAUSTION) || (maxSpectralPlayers && spectralPlayers >= maxSpectralPlayers))
                player->GetSession()->SendNotification(GO_FAILED);
            else
                player->CastSpell(player, SPELL_TELEPORT_SPECTRAL, true);

            return true;
        }
};

class boss_sathrovarr : public CreatureScript
{
    public:
        boss_sathrovarr() : CreatureScript("boss_sathrovarr") { }

        struct boss_sathrovarrAI : public ScriptedAI
        {
            boss_sathrovarrAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                kalecGUID = ObjectGuid::Empty;
                isStarted = false;
            }

            InstanceScript* instance;
            ObjectGuid kalecGUID;
            uint32 corruptionStrikeTimer;
            uint32 agonyCurseTimer;
            uint32 shadowBoltTimer;
            uint32 checkTimer;
            uint32 resetThreat;
            bool isEnraged;
            bool isBanished;
            bool isStarted;

            void Reset() override
            {
                me->SetFullHealth();//dunno why it does not resets health at evade..
                me->setActive(true);

                if (instance && instance->GetData(DATA_KALECGOS_EVENT) != DONE)
                    instance->SetData(DATA_KALECGOS_EVENT, NOT_STARTED);

                if (kalecGUID)
                {
                    if (Creature* kalec = Unit::GetCreature(*me, kalecGUID))
                        kalec->setDeathState(JUST_DIED);
                    kalecGUID = ObjectGuid::Empty;
                }

                shadowBoltTimer = urand(7, 10) * 1000;
                agonyCurseTimer = 20000;
                corruptionStrikeTimer = 13000;
                checkTimer = 1000;
                resetThreat = 1000;
                isEnraged = false;
                isBanished = false;

                me->CastSpell(me, AURA_DEMONIC_VISUAL, true);
                if (isStarted)
                    TeleportAllPlayersBack();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                isStarted = true;
                if (Creature* kalec = me->SummonCreature(NPC_KALEC, me->GetPositionX() + 10, me->GetPositionY() + 5, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 0))
                {
                    kalecGUID = kalec->GetGUID();
                    me->CombatStart(kalec);
                    me->AddThreat(kalec, 100.0f);
                    kalec->setActive(true);
                }
                Talk(SAY_SATH_AGGRO);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (damage >= me->GetHealth() && attacker != me)
                    damage = 0;
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetGUID() == kalecGUID)
                {
                    TeleportAllPlayersBack();
                    if (Creature* kalecgos = Unit::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                    {
                        CAST_AI(boss_kalecgos::boss_kalecgosAI, kalecgos->AI())->talkTimer = 1;
                        CAST_AI(boss_kalecgos::boss_kalecgosAI, kalecgos->AI())->isFriendly = false;
                    }
                    EnterEvadeMode();
                    return;
                }
                Talk(SAY_SATH_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_SATH_DEATH);
                me->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), DRAGON_REALM_Z, me->GetOrientation());
                TeleportAllPlayersBack();
                if (Creature* kalecgos = Unit::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                {
                    CAST_AI(boss_kalecgos::boss_kalecgosAI, kalecgos->AI())->talkTimer = 1;
                    CAST_AI(boss_kalecgos::boss_kalecgosAI, kalecgos->AI())->isFriendly = true;
                }

                if (instance)
                    instance->SetData(DATA_KALECGOS_EVENT, DONE);
            }

            void TeleportAllPlayersBack()
            {
                Map* map = me->GetMap();
                if (!map->IsDungeon())
                    return;

                for (auto&& itr : map->GetPlayers())
                {
                    if (itr.GetSource()->GetPositionZ() <= DRAGON_REALM_Z-5)
                    {
                        itr.GetSource()->RemoveAura(AURA_SPECTRAL_REALM_FORCE_REACTION);
                        itr.GetSource()->RemoveAura(AURA_SPECTRAL_REALM);
                        itr.GetSource()->TeleportTo(me->GetMap()->GetId(), itr.GetSource()->GetPositionX(), itr.GetSource()->GetPositionY(), DRAGON_REALM_Z + 5, itr.GetSource()->GetOrientation());
                    }
                }
            }

            void DoAction(int32 param) override
            {
                switch (param)
                {
                    case DO_ENRAGE:
                        isEnraged = true;
                        me->CastSpell(me, SPELL_ENRAGE, true);
                        break;
                    case DO_BANISH:
                        isBanished = true;
                        me->CastSpell(me, SPELL_BANISH, true);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->HasAura(AURA_SPECTRAL_INVISIBILITY))
                    me->CastSpell(me, AURA_SPECTRAL_INVISIBILITY, true);

                if (!UpdateVictim())
                    return;

                if (checkTimer <= diff)
                {
                    Creature* kalec = Unit::GetCreature(*me, kalecGUID);
                    if (!kalec || (kalec && !kalec->IsAlive()))
                    {
                        if (Creature* kalecgos = Unit::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                            kalecgos->AI()->EnterEvadeMode();
                            return;
                    }

                    if (HealthBelowPct(10) && !isEnraged)
                    {
                        if (Creature* kalecgos = Unit::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                            kalecgos->AI()->DoAction(DO_ENRAGE);
                        DoAction(DO_ENRAGE);
                    }

                    Creature* kalecgos = Unit::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON));
                    if (kalecgos)
                    {
                        if (!kalecgos->IsInCombat())
                        {
                            me->AI()->EnterEvadeMode();
                            return;
                        }
                    }

                    if (!isBanished && HealthBelowPct(1))
                    {
                        if (kalecgos)
                        {
                            if (kalecgos->HasAura(SPELL_BANISH))
                            {
                                me->DealDamage(me, me->GetHealth());
                                return;
                            }
                            else
                                DoAction(DO_BANISH);
                        }
                        else
                        {
                            EnterEvadeMode();
                            return;
                        }
                    }
                    checkTimer = 1000;
                } else checkTimer -= diff;

                if (resetThreat <= diff)
                {
                    for (auto&& itr : me->GetThreatManager().getThreatList())
                    {
                        if (Unit* unit = Unit::GetUnit(*me, itr->getUnitGuid()))
                            if (unit->GetPositionZ() > me->GetPositionZ()+5)
                                me->GetThreatManager().modifyThreatPercent(unit,-100);
                    }
                    resetThreat = 1000;
                } else resetThreat -= diff;

                if (shadowBoltTimer <= diff)
                {
                    if (!(rand() % 5))
                        Talk(SAY_SATH_SPELL1);
                    DoCast(me, SPELL_SHADOW_BOLT);
                    shadowBoltTimer = 7000+(rand()%3000);
                } else shadowBoltTimer -= diff;

                if (agonyCurseTimer <= diff)
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                    if (!target) target = me->GetVictim();
                    DoCast(target, SPELL_AGONY_CURSE);
                    agonyCurseTimer = 20000;
                } else agonyCurseTimer -= diff;

                if (corruptionStrikeTimer <= diff)
                {
                    if (!(rand()%5))
                        Talk(SAY_SATH_SPELL2);
                    DoCast(me->GetVictim(), SPELL_CORRUPTION_STRIKE);
                    corruptionStrikeTimer = 13000;
                } else corruptionStrikeTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_sathrovarrAI(creature);
        }
};

void AddSC_boss_kalecgos()
{
    new boss_kalecgos();
    new boss_sathrovarr();
    new boss_kalec();
    new go_kalecgos_teleporter();
}
