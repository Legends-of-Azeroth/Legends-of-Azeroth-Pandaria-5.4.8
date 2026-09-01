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
SDName: Boss_Eredar_Twins
SD%Complete: 100
SDComment:
EndScriptData */

#include "ScriptPCH.h"
#include "sunwell_plateau.h"

enum Quotes
{
    YELL_INTRO_SAC_1            = 0,
    YELL_INTRO_SAC_3            = 1,
    YELL_INTRO_SAC_5            = 2,
    YELL_INTRO_SAC_7            = 3,
    YELL_SAC_DEAD               = 4,
    EMOTE_SHADOW_NOVA           = 5,
    YELL_ENRAGE                 = 6,
    YELL_SISTER_ALYTHESS_DEAD   = 7,
    YELL_SAC_KILL               = 8,
    YELL_SHADOW_NOVA            = 9,

    YELL_INTRO_ALY_2            = 0,
    YELL_INTRO_ALY_4            = 1,
    YELL_INTRO_ALY_6            = 2,
    YELL_INTRO_ALY_8            = 3,
    EMOTE_CONFLAGRATION         = 4,
    YELL_ALY_KILL               = 5,
    YELL_ALY_DEAD               = 6,
    YELL_SISTER_SACROLASH_DEAD  = 7,
    YELL_CANFLAGRATION          = 8,
    YELL_BERSERK                = 9,
};

enum Spells
{
    // Lady sacrolash spells
    SPELL_DARK_TOUCHED      =   45347,
    SPELL_SHADOW_BLADES     =   45248, // 10 secs
    SPELL_DARK_STRIKE       =   45271,
    SPELL_SHADOW_NOVA       =   45329, // 30-35 secs
    SPELL_CONFOUNDING_BLOW  =   45256, // 25 secs

    // Shadow Image spells
    SPELL_SHADOW_FURY       =   45270,
    SPELL_IMAGE_VISUAL      =   45263,

    // Misc spells
    SPELL_ENRAGE            =   46587,
    SPELL_EMPOWER           =   45366,
    SPELL_DARK_FLAME        =   45345,

    // Grand Warlock Alythess spells
    SPELL_PYROGENICS        =   45230, // 15secs
    SPELL_FLAME_TOUCHED     =   45348,
    SPELL_CONFLAGRATION     =   45342, // 30-35 secs
    SPELL_BLAZE             =   45235, // On main target every 3 secs
    SPELL_FLAME_SEAR        =   46771,
    SPELL_BLAZE_SUMMON      =   45236, // 187366 GO
    SPELL_BLAZE_BURN        =   45246
};

class boss_sacrolash : public CreatureScript
{
    public:
        boss_sacrolash() : CreatureScript("boss_sacrolash") { }

        struct boss_sacrolashAI : public ScriptedAI
        {
            boss_sacrolashAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint32 shadowbladesTimer;
            uint32 shadownovaTimer;
            uint32 confoundingblowTimer;
            uint32 shadowimageTimer;
            uint32 conflagrationTimer;
            uint32 enrageTimer;
            bool sisterDeath;
            bool enraged;

            void Reset() override
            {
                enraged = false;

                if (instance)
                {
                    if (Creature* temp = Unit::GetCreature((*me), instance->GetGuidData(DATA_ALYTHESS)))
                    {
                        if (temp->isDead())
                            temp->Respawn();
                        else if (temp->GetVictim())
                            me->GetThreatManager().addThreat(temp->GetVictim(),0.0f);
                    }
                }

                if (!me->IsInCombat())
                {
                    shadowbladesTimer = 10000;
                    shadownovaTimer = 30000;
                    confoundingblowTimer = 25000;
                    shadowimageTimer = 20000;
                    conflagrationTimer = 30000;
                    enrageTimer = 360000;

                    sisterDeath = false;
                    me->SetLootMode(0);
                }

                if (instance)
                    instance->SetData(DATA_EREDAR_TWINS_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (instance->GetData(DATA_KALECGOS_EVENT) != DONE || instance->GetData(DATA_BRUTALLUS_EVENT) != DONE || instance->GetData(DATA_FELMYST_EVENT) != DONE)
                {
                    instance->DoCastSpellOnPlayers(SPELL_ELEPORT_TO_APEX_POINT);
                    EnterEvadeMode();
                    return;
                }

                DoZoneInCombat();

                if (instance)
                {
                    Creature* temp =  Unit::GetCreature((*me),instance->GetGuidData(DATA_ALYTHESS));
                    if (temp && temp->IsAlive() && !(temp->GetVictim()))
                        temp->AI()->AttackStart(who);
                }

                if (instance)
                    instance->SetData(DATA_EREDAR_TWINS_EVENT, IN_PROGRESS);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                if (rand() % 4 == 0)
                    Talk(YELL_SAC_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                // Only if ALY death
                if (sisterDeath)
                {
                    Talk(YELL_SAC_DEAD);

                    if (instance)
                        instance->SetData(DATA_EREDAR_TWINS_EVENT, DONE);
                }
                else
                {
                    me->SetLootRecipient(NULL);
                    me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                }
            }

            void SpellHitTarget(Unit* target, const SpellInfo* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_SHADOW_BLADES:
                    case SPELL_SHADOW_NOVA:
                    case SPELL_CONFOUNDING_BLOW:
                    case SPELL_SHADOW_FURY:
                        HandleTouchedSpells(target, SPELL_DARK_TOUCHED);
                        break;
                    case SPELL_CONFLAGRATION:
                        HandleTouchedSpells(target, SPELL_FLAME_TOUCHED);
                        break;
                }
            }

            void HandleTouchedSpells(Unit* target, uint32 touchedType)
            {
                switch (touchedType)
                {
                    case SPELL_FLAME_TOUCHED:
                        if (!target->HasAura(SPELL_DARK_FLAME))
                        {
                            if (target->HasAura(SPELL_DARK_TOUCHED))
                            {
                                target->RemoveAurasDueToSpell(SPELL_DARK_TOUCHED);
                                target->CastSpell(target, SPELL_DARK_FLAME, true);
                            }
                            else
                                target->CastSpell(target, SPELL_FLAME_TOUCHED, true);
                        }
                        break;
                    case SPELL_DARK_TOUCHED:
                        if (!target->HasAura(SPELL_DARK_FLAME))
                        {
                            if (target->HasAura(SPELL_FLAME_TOUCHED))
                            {
                                target->RemoveAurasDueToSpell(SPELL_FLAME_TOUCHED);
                                target->CastSpell(target, SPELL_DARK_FLAME, true);
                            }
                            else
                                target->CastSpell(target, SPELL_DARK_TOUCHED, true);
                        }
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!sisterDeath)
                {
                    if (instance)
                    {
                        Creature* temp = Unit::GetCreature((*me),instance->GetGuidData(DATA_ALYTHESS));
                        if (temp && temp->isDead())
                        {
                            Talk(YELL_SISTER_ALYTHESS_DEAD);
                            DoCast(me, SPELL_EMPOWER);
                            me->InterruptSpell(CURRENT_GENERIC_SPELL);
                            me->ResetLootMode();
                            sisterDeath = true;
                        }
                    }
                }

                if (!UpdateVictim())
                    return;

                if (sisterDeath)
                {
                    if (conflagrationTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            me->InterruptSpell(CURRENT_GENERIC_SPELL);
                            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                            if (target)
                                DoCast(target, SPELL_CONFLAGRATION);
                            conflagrationTimer = 30000+(rand()%5000);
                        }
                    } else conflagrationTimer -= diff;
                }
                else
                {
                    if (shadownovaTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                            if (target)
                                DoCast(target, SPELL_SHADOW_NOVA);

                            if (!sisterDeath)
                            {
                                if (target)
                                    Talk(EMOTE_SHADOW_NOVA, target);
                                Talk(YELL_SHADOW_NOVA);
                            }
                            shadownovaTimer = 30000 + (rand() % 5000);
                        }
                    } else shadownovaTimer -=diff;
                }

                if (confoundingblowTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                        if (target)
                            DoCast(target, SPELL_CONFOUNDING_BLOW);
                        confoundingblowTimer = 20000 + (rand()%5000);
                    }
                } else confoundingblowTimer -=diff;

                if (shadowimageTimer <= diff)
                {
                    Unit* target = nullptr;
                    Creature* temp = nullptr;
                    for (uint8 i = 0; i < 3; ++i)
                    {
                        target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                        temp = DoSpawnCreature(NPC_SHADOW_IMAGE, 0, 0, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 10000ms);
                        if (temp && target)
                        {
                            temp->AddThreat(target,1000000);//don't change target(healers)
                            temp->AI()->AttackStart(target);
                        }
                    }
                    shadowimageTimer = 20000;
                } else shadowimageTimer -=diff;

                if (shadowbladesTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me, SPELL_SHADOW_BLADES);
                        shadowbladesTimer = 10000;
                    }
                } else shadowbladesTimer -=diff;

                if (enrageTimer < diff && !enraged)
                {
                    me->InterruptSpell(CURRENT_GENERIC_SPELL);
                    Talk(YELL_ENRAGE);
                    DoCast(me, SPELL_ENRAGE);
                    enraged = true;
                } else enrageTimer -= diff;

                if (me->isAttackReady() && !me->IsNonMeleeSpellCasted(false))
                {
                    // If we are within range melee the target
                    if (me->IsWithinMeleeRange(me->GetVictim()))
                    {
                        HandleTouchedSpells(me->GetVictim(), SPELL_DARK_TOUCHED);
                        me->AttackerStateUpdate(me->GetVictim());
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_sacrolashAI (creature);
        };
};

class boss_alythess : public CreatureScript
{
    public:
        boss_alythess() : CreatureScript("boss_alythess") { }

        struct boss_alythessAI : public ScriptedAI
        {
            boss_alythessAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                introStepCounter = 10;
                SetCombatMovement(false);
            }

            InstanceScript* instance;
            uint32 introStepCounter;
            uint32 introYellTimer;
            uint32 conflagrationTimer;
            uint32 blazeTimer;
            uint32 pyrogenicsTimer;
            uint32 shadownovaTimer;
            uint32 flamesearTimer;
            uint32 enrageTimer;
            bool sisterDeath;
            bool enraged;
            
            void Reset() override
            {
                enraged = false;

                if (instance)
                {
                    Creature* temp = Unit::GetCreature((*me),instance->GetGuidData(DATA_SACROLASH));
                    if (temp)
                    {
                        if (temp->isDead())
                            temp->Respawn();
                        else if (temp->GetVictim())
                            me->GetThreatManager().addThreat(temp->GetVictim(),0.0f);
                    }
                }

                if (!me->IsInCombat())
                {
                    conflagrationTimer = 45000;
                    blazeTimer = 100;
                    pyrogenicsTimer = 15000;
                    shadownovaTimer = 40000;
                    enrageTimer = 360000;
                    flamesearTimer = 15000;
                    introYellTimer = 10000;

                    sisterDeath = false;
                    me->SetLootMode(0);
                }

                if (instance)
                    instance->SetData(DATA_EREDAR_TWINS_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (instance->GetData(DATA_KALECGOS_EVENT) != DONE || instance->GetData(DATA_BRUTALLUS_EVENT) != DONE || instance->GetData(DATA_FELMYST_EVENT) != DONE)
                {
                    instance->DoCastSpellOnPlayers(SPELL_ELEPORT_TO_APEX_POINT);
                    EnterEvadeMode();
                    return;
                }

                DoZoneInCombat();

                if (instance)
                {
                    Creature* temp =  Unit::GetCreature((*me),instance->GetGuidData(DATA_SACROLASH));
                    if (temp && temp->IsAlive() && !(temp->GetVictim()))
                        temp->AI()->AttackStart(who);
                }

                if (instance)
                    instance->SetData(DATA_EREDAR_TWINS_EVENT, IN_PROGRESS);
            }

            void AttackStart(Unit* who) override
            {
                if (!me->IsInCombat())
                {
                    ScriptedAI::AttackStart(who);
                }
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!who || me->GetVictim())
                    return;

                if (who->isTargetableForAttack() && who->isInAccessiblePlaceFor(me) && me->IsHostileTo(who))
                {
                    float attackRadius = me->GetAttackDistance(who);
                    if (me->IsWithinDistInMap(who, attackRadius) && me->GetDistanceZ(who) <= CREATURE_Z_ATTACK_RANGE && me->IsWithinLOSInMap(who))
                    {
                        if (!me->IsInCombat())
                        {
                            DoStartNoMovement(who);
                        }
                    }
                }
                else if (introStepCounter == 10 && me->IsWithinLOSInMap(who)&& me->IsWithinDistInMap(who, 30))
                {
                    introStepCounter = 0;
                }
            }

            void KilledUnit(Unit * /*victim*/) override
            {
                if (rand()%4 == 0)
                {
                    Talk(YELL_ALY_KILL);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (sisterDeath)
                {
                    Talk(YELL_ALY_DEAD);

                    if (instance)
                        instance->SetData(DATA_EREDAR_TWINS_EVENT, DONE);
                }
                else
                {
                    me->SetLootRecipient(NULL);
                    me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                }
            }

            void SpellHitTarget(Unit* target,const SpellInfo* spell) override
            {
                switch(spell->Id)
                {
                    case SPELL_BLAZE:
                        target->CastSpell(target, SPELL_BLAZE_SUMMON, true);
                    case SPELL_CONFLAGRATION:
                    case SPELL_FLAME_SEAR:
                        HandleTouchedSpells(target, SPELL_FLAME_TOUCHED);
                        break;
                    case SPELL_SHADOW_NOVA:
                        HandleTouchedSpells(target, SPELL_DARK_TOUCHED);
                        break;
                }
            }

            void HandleTouchedSpells(Unit* target, uint32 touchedType)
            {
                switch (touchedType)
                {
                    case SPELL_FLAME_TOUCHED:
                        if (!target->HasAura(SPELL_DARK_FLAME))
                        {
                            if (target->HasAura(SPELL_DARK_TOUCHED))
                            {
                                target->RemoveAurasDueToSpell(SPELL_DARK_TOUCHED);
                                target->CastSpell(target, SPELL_DARK_FLAME, true);
                            }
                            else
                                target->CastSpell(target, SPELL_FLAME_TOUCHED, true);
                        }
                        break;
                    case SPELL_DARK_TOUCHED:
                        if (!target->HasAura(SPELL_DARK_FLAME))
                        {
                            if (target->HasAura(SPELL_FLAME_TOUCHED))
                            {
                                target->RemoveAurasDueToSpell(SPELL_FLAME_TOUCHED);
                                target->CastSpell(target, SPELL_DARK_FLAME, true);
                            }
                            else
                                target->CastSpell(target, SPELL_DARK_TOUCHED, true);
                        }
                        break;
                }
            }

            uint32 IntroStep(uint32 step)
            {
                Creature* sacrolash = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_SACROLASH) : ObjectGuid::Empty);
                switch (step)
                {
                    case 0: return 0;
                    case 1:
                        if (sacrolash)
                            sacrolash->AI()->Talk(YELL_INTRO_SAC_1);
                        return 1000;
                    case 2: Talk(YELL_INTRO_ALY_2); return 1000;
                    case 3:
                        if (sacrolash)
                            sacrolash->AI()->Talk(YELL_INTRO_SAC_3);
                        return 2000;
                    case 4: Talk(YELL_INTRO_ALY_4); return 1000;
                    case 5:
                        if (sacrolash)
                            sacrolash->AI()->Talk(YELL_INTRO_SAC_5);
                        return 2000;
                    case 6: Talk(YELL_INTRO_ALY_6); return 1000;
                    case 7:
                        if (sacrolash)
                            sacrolash->AI()->Talk(YELL_INTRO_SAC_7);
                        return 3000;
                    case 8: Talk(YELL_INTRO_ALY_8); return 900000;
                }
                return 10000;
            }

            void UpdateAI(uint32 diff) override
            {
                if (introStepCounter < 9)
                {
                    if (introYellTimer <= diff)
                    {
                        introYellTimer = IntroStep(++introStepCounter);
                    } else introYellTimer -= diff;
                }

                if (!sisterDeath)
                {
                    if (instance)
                    {
                        Unit* temp = Unit::GetUnit((*me),instance->GetGuidData(DATA_SACROLASH));
                        if (temp && temp->isDead())
                        {
                            Talk(YELL_SISTER_SACROLASH_DEAD);
                            DoCast(me, SPELL_EMPOWER);
                            me->InterruptSpell(CURRENT_GENERIC_SPELL);
                            me->ResetLootMode();
                            sisterDeath = true;
                        }
                    }
                }
                if (!me->GetVictim())
                {
                    if (instance)
                    {
                        Creature* sisiter = Unit::GetCreature((*me),instance->GetGuidData(DATA_SACROLASH));
                        if (sisiter && !sisiter->isDead() && sisiter->GetVictim())
                        {
                            me->AddThreat(sisiter->GetVictim(),0.0f);
                            DoStartNoMovement(sisiter->GetVictim());
                            me->Attack(sisiter->GetVictim(),false);
                        }
                    }
                }

                if (!UpdateVictim())
                    return;

                if (sisterDeath)
                {
                    if (shadownovaTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                            if (target)
                                DoCast(target, SPELL_SHADOW_NOVA);
                            shadownovaTimer= 30000+(rand()%5000);
                        }
                    } else shadownovaTimer -=diff;
                }
                else
                {
                    if (conflagrationTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            me->InterruptSpell(CURRENT_GENERIC_SPELL);
                            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                            if (target)
                                DoCast(target, SPELL_CONFLAGRATION);
                            conflagrationTimer = 30000+(rand()%5000);

                            if (!sisterDeath)
                            {
                                if (target)
                                    Talk(EMOTE_CONFLAGRATION, target);
                                Talk(YELL_CANFLAGRATION);
                            }

                            blazeTimer = 4000;
                        }
                    } else conflagrationTimer -= diff;
                }

                if (flamesearTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me, SPELL_FLAME_SEAR);
                        flamesearTimer = 15000;
                    }
                } else flamesearTimer -=diff;

                if (pyrogenicsTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me, SPELL_PYROGENICS, true);
                        pyrogenicsTimer = 15000;
                    }
                } else pyrogenicsTimer -= diff;

                if (blazeTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me->GetVictim(), SPELL_BLAZE);
                        blazeTimer = 3800;
                    }
                } else blazeTimer -= diff;

                if (enrageTimer < diff && !enraged)
                {
                    me->InterruptSpell(CURRENT_GENERIC_SPELL);
                    Talk(YELL_BERSERK);
                    DoCast(me, SPELL_ENRAGE);
                    enraged = true;
                } else enrageTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_alythessAI(creature);
        };
};

class npc_shadow_image : public CreatureScript
{
    public:
        npc_shadow_image() : CreatureScript("npc_shadow_image") { }

        struct npc_shadow_imageAI : public ScriptedAI
        {
            npc_shadow_imageAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 shadowfuryTimer;
            uint32 killTimer;
            uint32 darkstrikeTimer;

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                shadowfuryTimer = 5000 + (rand()%15000);
                darkstrikeTimer = 3000;
                killTimer = 15000;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void SpellHitTarget(Unit* target, const SpellInfo* spell) override
            {
                switch(spell->Id)
                {
                    case SPELL_SHADOW_FURY:
                    case SPELL_DARK_STRIKE:
                        if (!target->HasAura(SPELL_DARK_FLAME))
                        {
                            if (target->HasAura(SPELL_FLAME_TOUCHED))
                            {
                                target->RemoveAurasDueToSpell(SPELL_FLAME_TOUCHED);
                                target->CastSpell(target, SPELL_DARK_FLAME, true);
                            }
                            else
                                target->CastSpell(target,SPELL_DARK_TOUCHED,true);
                        }
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->HasAura(SPELL_IMAGE_VISUAL))
                    DoCast(me, SPELL_IMAGE_VISUAL);

                if (killTimer <= diff)
                {
                    me->Kill(me);
                    killTimer = 9999999;
                } else killTimer -= diff;

                if (!UpdateVictim())
                    return;

                if (shadowfuryTimer <= diff)
                {
                    DoCast(me, SPELL_SHADOW_FURY);
                    shadowfuryTimer = 10000;
                } else shadowfuryTimer -=diff;

                if (darkstrikeTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        // If we are within range melee the target
                        if (me->IsWithinMeleeRange(me->GetVictim()))
                            DoCast(me->GetVictim(), SPELL_DARK_STRIKE);
                    }
                    darkstrikeTimer = 3000;
                } else darkstrikeTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shadow_imageAI(creature);
        };
};

void AddSC_boss_eredar_twins()
{
    new boss_sacrolash();
    new boss_alythess();
    new npc_shadow_image();
}
