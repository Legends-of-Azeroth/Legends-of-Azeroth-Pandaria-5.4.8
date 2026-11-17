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
SDName: Boss_Terestian_Illhoof
SD%Complete: 95
SDComment: Complete! Needs adjustments to use spell though.
SDCategory: Karazhan
EndScriptData */

#include "ScriptPCH.h"
#include "karazhan.h"
#include "PassiveAI.h"

enum TerestianIllhoof
{
    SAY_SLAY                    = 0,
    SAY_DEATH                   = 1,
    SAY_AGGRO                   = 2,
    SAY_SACRIFICE               = 3,
    SAY_SUMMON                  = 4,
};

enum Spells
{
    SPELL_SUMMON_DEMONCHAINS    = 30120, // Summons demonic chains that maintain the ritual of sacrifice.
    SPELL_DEMON_CHAINS          = 30206, // Instant - Visual Effect
    SPELL_ENRAGE                = 23537, // Increases the caster's attack speed by 50% and the Physical damage it deals by 219 to 281 for 10 min.
    SPELL_SHADOW_BOLT           = 30055, // Hurls a bolt of dark magic at an enemy, inflicting Shadow damage.
    SPELL_SACRIFICE             = 30115, // Teleports and adds the debuff
    SPELL_BERSERK               = 32965, // Increases attack speed by 75%. Periodically casts Shadow Bolt Volley.
    SPELL_SUMMON_FIENDISIMP     = 30184, // Summons a Fiendish Imp.
    SPELL_SUMMON_IMP            = 30066, // Summons Kil'rek

    SPELL_FIENDISH_PORTAL       = 30171, // Opens portal and summons Fiendish Portal, 2 sec cast
    SPELL_FIENDISH_PORTAL_1     = 30179, // Opens portal and summons Fiendish Portal, instant cast

    SPELL_FIREBOLT              = 30050, // Blasts a target for 150 Fire damage.
    SPELL_BROKEN_PACT           = 30065, // All damage taken increased by 25%.
    SPELL_AMPLIFY_FLAMES        = 30053, // Increases the Fire damage taken by an enemy by 500 for 25 sec.
};

enum Creatures
{
    NPC_DEMONCHAINS             = 17248,
    NPC_FIENDISHIMP             = 17267,
    NPC_PORTAL                  = 17265,
    NPC_KILREK                  = 17229
};

class npc_kilrek : public CreatureScript
{
    public:
        npc_kilrek() : CreatureScript("npc_kilrek") { }

        struct npc_kilrekAI : public ScriptedAI
        {
            npc_kilrekAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            ObjectGuid TerestianGUID;
            uint32 AmplifyTimer;

            void Reset() override
            {
                TerestianGUID = ObjectGuid::Empty;
                AmplifyTimer = 2000;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (!instance)
                {
                    ERROR_INST_DATA(me);
                    return;
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                {
                    ObjectGuid TerestianGUID = instance->GetGuidData(DATA_TERESTIAN);
                    if (TerestianGUID)
                    {
                        Unit* terestian = Unit::GetUnit(*me, TerestianGUID);
                        if (terestian && terestian->IsAlive())
                            DoCast(terestian, SPELL_BROKEN_PACT, true);
                    }
                } else ERROR_INST_DATA(me);
            }

            void UpdateAI(uint32 diff) override
            {
                // Return since we have no target
                if (!UpdateVictim())
                    return;

                if (AmplifyTimer <= diff)
                {
                    me->InterruptNonMeleeSpells(false);
                    DoCast(me->GetVictim(), SPELL_AMPLIFY_FLAMES);

                    AmplifyTimer = urand(10000,20000);
                } else AmplifyTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_kilrekAI(creature);
        }
};

class npc_demon_chain : public CreatureScript
{
    public:
        npc_demon_chain() : CreatureScript("npc_demon_chain") { }

        struct npc_demon_chainAI : public ScriptedAI
        {
            npc_demon_chainAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid SacrificeGUID;

            void Reset() override
            {
                SacrificeGUID = ObjectGuid::Empty;
            }

            void JustEngagedWith(Unit* /*who*/) override { }
            void AttackStart(Unit* /*who*/) override { }
            void MoveInLineOfSight(Unit* /*who*/) override { }

            void JustDied(Unit* /*killer*/) override
            {
                if (SacrificeGUID)
                {
                    if (Unit* sacrifice = Unit::GetUnit(*me,SacrificeGUID))
                        sacrifice->RemoveAurasDueToSpell(SPELL_SACRIFICE);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_demon_chainAI(creature);
        }
};

class npc_fiendish_portal : public CreatureScript
{
    public:
        npc_fiendish_portal() : CreatureScript("npc_fiendish_portal") { }

        struct npc_fiendish_portalAI : public PassiveAI
        {
            npc_fiendish_portalAI(Creature* creature) : PassiveAI(creature), summons(me) { }

            SummonList summons;

            void Reset() override
            {
                DespawnAllImp();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                DoZoneInCombat(summon);
            }

            void DespawnAllImp()
            {
                summons.DespawnAll();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_fiendish_portalAI(creature);
        }
};

#define SPELL_FIREBOLT  30050   // Blasts a target for 181-209 Fire damage.

class npc_fiendish_imp : public CreatureScript
{
    public:
        npc_fiendish_imp() : CreatureScript("npc_fiendish_imp") { }

        struct npc_fiendish_impAI : public ScriptedAI
        {
            npc_fiendish_impAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 FireboltTimer;

            void Reset() override
            {
                FireboltTimer = 2000;

                me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                // Return since we have no target
                if (!UpdateVictim())
                    return;

                if (FireboltTimer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_FIREBOLT);
                    FireboltTimer = 2200;
                } else FireboltTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_fiendish_impAI(creature);
        }
};

class boss_terestian_illhoof : public CreatureScript
{
    public:
        boss_terestian_illhoof() : CreatureScript("boss_terestian_illhoof") { }

        struct boss_terestianAI : public ScriptedAI
        {
            boss_terestianAI(Creature* creature) : ScriptedAI(creature)
            {
                for (uint8 i = 0; i < 2; ++i)
                    PortalGUID[i] = ObjectGuid::Empty;
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            ObjectGuid PortalGUID[2];
            uint8 portalsCount;

            uint32 SacrificeTimer;
            uint32 ShadowboltTimer;
            uint32 SummonTimer;
            uint32 BerserkTimer;

            bool SummonedPortals;
            bool Berserk;

            void Reset() override
            {
                for (uint8 i = 0; i < 2; ++i)
                {
                    if (PortalGUID[i])
                    {
                        if (Creature* portal = Unit::GetCreature(*me, PortalGUID[i]))
                        {
                            CAST_AI(npc_fiendish_portal::npc_fiendish_portalAI, portal->AI())->DespawnAllImp();
                            portal->DespawnOrUnsummon();
                        }

                        PortalGUID[i] = ObjectGuid::Empty;
                    }
                }

                portalsCount        =     0;
                SacrificeTimer      = 30000;
                ShadowboltTimer     =  5000;
                SummonTimer         = 10000;
                BerserkTimer        = 600000;

                SummonedPortals     = false;
                Berserk             = false;

                if (instance)
                    instance->SetData(TYPE_TERESTIAN, NOT_STARTED);

                me->RemoveAurasDueToSpell(SPELL_BROKEN_PACT);

                if (Minion* Kilrek = me->GetFirstMinion())
                {
                    if (!Kilrek->IsAlive())
                    {
                        Kilrek->UnSummon();
                        DoCast(me, SPELL_SUMMON_IMP, true);
                    }
                }
                else DoCast(me, SPELL_SUMMON_IMP, true);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_PORTAL)
                {
                    PortalGUID[portalsCount] = summon->GetGUID();
                    ++portalsCount;

                    if (summon->GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL) == SPELL_FIENDISH_PORTAL_1)
                    {
                        Talk(SAY_SUMMON);
                        SummonedPortals = true;
                    }
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                for (uint8 i = 0; i < 2; ++i)
                {
                    if (PortalGUID[i])
                    {
                        if (Creature* portal = Unit::GetCreature(*me, PortalGUID[i]))
                            portal->DespawnOrUnsummon();

                        PortalGUID[i] = ObjectGuid::Empty;
                    }
                }

                Talk(SAY_DEATH);

                if (instance)
                    instance->SetData(TYPE_TERESTIAN, DONE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (SacrificeTimer <= diff)
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 100, true);
                    if (target && target->IsAlive())
                    {
                        DoCast(target, SPELL_SACRIFICE, true);
                        DoCast(target, SPELL_SUMMON_DEMONCHAINS, true);

                        if (Creature* Chains = me->FindNearestCreature(NPC_DEMONCHAINS, 5000))
                        {
                            CAST_AI(npc_demon_chain::npc_demon_chainAI, Chains->AI())->SacrificeGUID = target->GetGUID();
                            Chains->CastSpell(Chains, SPELL_DEMON_CHAINS, true);
                            Talk(SAY_SACRIFICE);
                            SacrificeTimer = 30000;
                        }
                    }
                } else SacrificeTimer -= diff;

                if (ShadowboltTimer <= diff)
                {
                    DoCast(SelectTarget(SELECT_TARGET_TOPAGGRO, 0), SPELL_SHADOW_BOLT);
                    ShadowboltTimer = 10000;
                } else ShadowboltTimer -= diff;

                if (SummonTimer <= diff)
                {
                    if (!PortalGUID[0])
                        DoCast(me->GetVictim(), SPELL_FIENDISH_PORTAL, false);

                    if (!PortalGUID[1])
                        DoCast(me->GetVictim(), SPELL_FIENDISH_PORTAL_1, false);

                    if (PortalGUID[0] && PortalGUID[1])
                    {
                        if (Creature* portal = Unit::GetCreature(*me, PortalGUID[urand(0,1)]))
                            portal->CastSpell(me->GetVictim(), SPELL_SUMMON_FIENDISIMP, false);
                        SummonTimer = 5000;
                    }
                } else SummonTimer -= diff;

                if (!Berserk)
                {
                    if (BerserkTimer <= diff)
                    {
                        DoCast(me, SPELL_BERSERK);
                        Berserk = true;
                    } else BerserkTimer -= diff;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_terestianAI(creature);
        }
};

void AddSC_boss_terestian_illhoof()
{
    new boss_terestian_illhoof();
    new npc_fiendish_imp();
    new npc_fiendish_portal();
    new npc_kilrek();
    new npc_demon_chain();
}
