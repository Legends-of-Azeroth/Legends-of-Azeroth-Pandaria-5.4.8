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

#include "siege_of_niuzao_temple.h"
#include "CombatAI.h"

// Sik'thik Guardian - 61928
class npc_sikthik_guardian : public CreatureScript
{
    enum Spells
    {
        SPELL_MALLEABLE_RESIN           = 121422
    };

    public:
        npc_sikthik_guardian() : CreatureScript("npc_sikthik_guardian") { }

        struct npc_sikthik_guardianAI : public ScriptedAI
        {
            npc_sikthik_guardianAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                chargeTimer = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (chargeTimer <= diff)
                {
                    DoCast(SELECT_TARGET_RANDOM, SPELL_MALLEABLE_RESIN, true, 1);
                    chargeTimer = 10000;
                }
                else chargeTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            uint32 chargeTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sikthik_guardianAI(creature);
        }
};


class npc_sikthik_amber_weaver : public CreatureScript
{
    enum Spells
    {
        SPELL_RESIN_WEAVE_COSMETIC      = 120596,
        SPELL_RESIN_WEAVING             = 121114,
        SPELL_RESIN_SHELL               = 120946,
    };

    public:
        npc_sikthik_amber_weaver() : CreatureScript("npc_sikthik_amber_weaver") { }

        struct npc_sikthik_amber_weaverAI : public ArcherAI
        {
            npc_sikthik_amber_weaverAI(Creature* creature) : ArcherAI(creature)
            {
                visualTimer = 3000;

                me->m_CombatDistance = 15.0f;
                me->m_SightDistance = 15.0f;
                m_minRange = 10.0f;
            }

            void Reset() override
            {
                resinShellTimer = urand(10000, 16000);
                resinWeavingtimer = urand(6000, 12000);
            }

            void JustReachedHome() override
            {
                visualTimer = 1000;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->InterruptNonMeleeSpells(true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (visualTimer)
                {
                    if (visualTimer <= diff)
                    {
                        DoCast(me, SPELL_RESIN_WEAVE_COSMETIC, false);
                        visualTimer = 0;
                    } else visualTimer -= diff;
                }

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (resinWeavingtimer <= diff)
                {
                    DoCast(SELECT_TARGET_RANDOM, SPELL_RESIN_WEAVING, false);
                    resinWeavingtimer = 10000;
                    return;
                } else resinWeavingtimer -= diff;

                if (resinShellTimer <= diff)
                {
                    DoCast(me, SPELL_RESIN_SHELL, false);
                    resinShellTimer = 20000;
                } else resinShellTimer -= diff;

                DoMeleeAttackIfReady();
            }
        private:
            uint32 visualTimer;
            uint32 resinShellTimer;
            uint32 resinWeavingtimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sikthik_amber_weaverAI(creature);
        }
};

// Resin weaving - 121114
class spell_resin_weaving : public SpellScriptLoader
{
    enum 
    {
        SPELL_ENCASED_IN_RESIN          = 121116
    };

    public:
        spell_resin_weaving() : SpellScriptLoader("spell_resin_weaving") { }

        class spell_resin_weaving_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_resin_weaving_AuraScript);
            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Unit*  owner = GetUnitOwner())
                {
                    if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                        owner->CastSpell(owner, SPELL_ENCASED_IN_RESIN, true);
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_resin_weaving_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_resin_weaving_AuraScript();
        }
};

class npc_resin_flake : public CreatureScript
{
    enum Spells
    {
        // SPELL_RESIDUE_AURA           = 120940,
        SPELL_RESIDUE           = 120938,
    };

    public:
        npc_resin_flake() : CreatureScript("npc_resin_flake") { }

        struct npc_resin_flakeAI : public ScriptedAI
        {
            npc_resin_flakeAI(Creature* creature) : ScriptedAI(creature) { }

            void JustDied(Unit* /*killer*/) override
            {
                DoCast(me, SPELL_RESIDUE, true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_resin_flakeAI(creature);
        }
};

// Sap Spray 61964
class npc_sap_spray : public CreatureScript
{
    public:
        npc_sap_spray() : CreatureScript("npc_sap_spray") { }

        enum Spells
        {
            SPELL_SAP_SPRAY  = 120586,
            SPELL_SAP_PUDDLE = 120591,
        };

        struct npc_sap_sprayAI : public ScriptedAI
        {
            npc_sap_sprayAI(Creature* creature) : ScriptedAI(creature) { }

            bool activated;

            void InitializeAI() override
            {
                activated = false;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!activated && me->FindNearestPlayer(10.0f))
                {
                    activated = true;

                    if (Creature* puddle = GetClosestCreatureWithEntry(me, NPC_SAP_PUDDLE_2, 20.0f, true))
                    {
                        me->CastSpell(me, SPELL_SAP_SPRAY, false);
                        puddle->CastSpell(me, SPELL_SAP_PUDDLE, false);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sap_sprayAI(creature);
        }
};

static const Position attackPos = { 2080.853f, 5056.36f, 143.6199f, 0.0f };

enum
{
    DATA_CATAPULT_ASSIGNED      = 10,

    NPC_MANTID_SOLDIER          = 62348,

    SPELL_CATAPULT_CHANNEL      = 124067,
    SPELL_CATAPULT_PRECAST      = 124083,
    SPELL_FIRE_CATAPULT         = 124017
};

// Mantid Catapult - 63565
class npc_mantid_catapult : public CreatureScript
{
    enum
    {
        EVENT_INIT              = 1,
        EVENT_SOLDIER_CHANNEL,
        EVENT_CAST,
        EVENT_FIRE,
    };

    public:
        npc_mantid_catapult() : CreatureScript("npc_mantid_catapult") { }

        struct npc_mantid_catapultAI : public ScriptedAI
        {
            npc_mantid_catapultAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->setActive(true);
                events.Reset();
                if (soldierList.empty())
                    events.ScheduleEvent(EVENT_INIT, urand(1000, 6000));
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INIT:
                        {
                            std::list<Creature*> cList;
                            me->GetCreatureListWithEntryInGrid(cList, NPC_MANTID_SOLDIER, 50.0f);

                            cList.sort(Trinity::ObjectDistanceOrderPred(me));

                            for (auto &&itr : cList)
                            {
                                if (itr->IsAlive()
                                    &&!itr->AI()->GetData(DATA_CATAPULT_ASSIGNED))
                                {
                                    soldierList.push_back(itr->GetGUID());
                                    itr->AI()->SetGUID(me->GetGUID(), soldierList.size());
                                }

                                if (soldierList.size() >= 3)
                                    break;
                            }

                            if (soldierList.empty())
                                return;

                            events.ScheduleEvent(EVENT_SOLDIER_CHANNEL, urand(5000, 18000)); // To make sure catapults dont fire simultaneously
                            break;
                        }
                        case EVENT_SOLDIER_CHANNEL:
                        {
                            bool gotSoldier = false;
                            bool combat = true;

                            // If soldiers are dead -> stop all events
                            // If soldiers are in combat -> reschedule this check

                            for (auto &&itr : soldierList)
                            {
                                if (Creature* soldier = Creature::GetCreature(*me, itr))
                                {
                                    if (soldier->IsAlive())
                                    {
                                        gotSoldier = true;
                                        if (!soldier->IsInCombat())
                                        {
                                            combat = false;
                                            soldier->CastSpell(soldier, SPELL_CATAPULT_CHANNEL, true);
                                        }
                                    }
                                }
                            }

                            if (combat)
                                events.ScheduleEvent(EVENT_SOLDIER_CHANNEL, 5000);
                            else if (gotSoldier)
                                events.ScheduleEvent(EVENT_CAST, 2000);
                            break;
                        }
                        case EVENT_CAST:
                            DoCast(me, SPELL_CATAPULT_PRECAST, false);
                            events.ScheduleEvent(EVENT_FIRE, 3000); // To make sure catapults dont fire simultaneously
                            break;
                        case EVENT_FIRE:
                        {
                            Position pos = attackPos;
                            me->MovePosition(pos, frand(20.0f, 50.0f), (float)rand_norm() * static_cast<float>(2 * M_PI));
                            if (!me->IsWithinDist2d(&pos, 200.0f))
                                pos = me->GetNearPosition(199.0f, me->GetAngle(&pos));

                            me->CastSpell(pos, SPELL_FIRE_CATAPULT, false);

                            events.ScheduleEvent(EVENT_SOLDIER_CHANNEL, 10000); // To make sure catapults dont fire simultaneously
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        private:
            EventMap events;
            std::list<ObjectGuid> soldierList;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mantid_catapultAI(creature);
        }
};

// Sik'Thik soldier - 62348 + 61448
class npc_mantid_soldier_catapult : public CreatureScript
{
    enum Spells
    {
        SPELL_SERRATED_BLADE        = 119840
    };

    public:
        npc_mantid_soldier_catapult() : CreatureScript("npc_mantid_soldier_catapult") { }

        struct npc_mantid_soldier_catapultAI : public ScriptedAI
        {
            npc_mantid_soldier_catapultAI(Creature* creature) : ScriptedAI(creature)
            {
                catapultGUID = ObjectGuid::Empty;
                counter = 0;
            }

            void Reset() override
            {
                me->setActive(true);
                bladetimer = urand(5000, 6000);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_CATAPULT_ASSIGNED)
                {
                    return catapultGUID;
                    //catapultGUID = 1; // wtf
                }
                return 0;
            }

            void SetGUID(ObjectGuid guid, int32 type) override
            {
                // 62384 only
                catapultGUID = guid;
                counter = type;

                // update home position based on the counter
                // soldiers line up behind the catapult
                if (Creature* catapult = Creature::GetCreature(*me, guid))
                {
                    //Arrange around the position of the "spoon"
                    Position anchorPos, myPos;
                    anchorPos = catapult->GetNearPosition(-9.0f, 0.0f);
                    myPos = anchorPos;
                    me->MovePosition(myPos, INTERACTION_DISTANCE, float(M_PI / 2.0) * (float)counter);
                    myPos.SetOrientation(myPos.GetAngle(&anchorPos));

                    me->SetHomePosition(myPos);
                    EnterEvadeMode();
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (bladetimer <= diff)
                {
                    DoCastVictim(SPELL_SERRATED_BLADE, false);
                    bladetimer = urand(4000, 7000);
                } else bladetimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            ObjectGuid catapultGUID;
            int32 counter;
            uint32 bladetimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mantid_soldier_catapultAI(creature);
        }
};

 // Quest: Somewhere Inside
enum eSomewhereInside
{
    GO_MANTID_CAGE          = 213935,
    NPC_PRISONER            = 64520,
    SPELL_INTERACT          = 125993,
    SPELL_STEALTH           = 86603
};

// Shado Pan Prisoner - 64520
class npc_niuzao_shado_pan_prisoner : public CreatureScript
{
    public:
        npc_niuzao_shado_pan_prisoner() : CreatureScript("npc_niuzao_shado_pan_prisoner") { }

        struct npc_niuzao_shado_pan_prisonerAI : public ScriptedAI
        {
            npc_niuzao_shado_pan_prisonerAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                stealthTimer = 0;
                exitTimer = 0;
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_INTERACT)
                {
                    me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    Player * player = caster->ToPlayer();
                    GameObject * cage = me->FindNearestGameObject(GO_MANTID_CAGE, 5.0f);
                    if (player && cage)
                    {
                        player->KilledMonsterCredit(NPC_PRISONER, me->GetGUID());
                        cage->UseDoorOrButton(DAY);
                        exitTimer = 2000;
                    }
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 1)
                    stealthTimer = 1000;
            }

            void UpdateAI(uint32 diff) override
            {
                if (exitTimer)
                {
                    if (exitTimer <= diff)
                    {
                        exitTimer = 0;
                        Position pos = me->GetNearPosition(3.0f, 0.0f);
                        me->GetMotionMaster()->MovePoint(1, pos);
                    } else exitTimer -= diff;
                }

                if (stealthTimer)
                {
                    if (stealthTimer <= diff)
                    {
                        stealthTimer = 0;
                        DoCast(me, SPELL_STEALTH, true);
                        Position pos = me->GetNearPosition(20.0f, 0.0f);
                        me->GetMotionMaster()->MovePoint(2, pos);
                        me->DespawnOrUnsummon(3000);
                    } else stealthTimer -= diff;
                }
            }
        private:
            uint32 exitTimer;
            uint32 stealthTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_niuzao_shado_pan_prisonerAI(creature);
        }
};

/*

class npc_sikthik_guardian : public CreatureScript
{
    public:
        npc_sikthik_guardian() : CreatureScript("npc_sikthik_guardian") { }

        struct npc_sikthik_guardianAI : public ScriptedAI
        {
            npc_sikthik_guardianAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {

            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sikthik_guardianAI(creature);
        }
};
*/

// Sik'Thik Battle-Mender - 67093
class npc_sikthik_battle_mender : public CreatureScript
{
    enum Spells
    {
        SPELL_MENDING           = 131968,
        SPELL_WINDS_GRACE       = 131972
    };

    public:
        npc_sikthik_battle_mender() : CreatureScript("npc_sikthik_battle_mender") { }

        struct npc_sikthik_battle_menderAI : public ScriptedAI
        {
            npc_sikthik_battle_menderAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                mendingTimer = urand(5000, 6000);
                graceTimer = urand(1000, 2000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (mendingTimer <= diff)
                {
                    DoCast(SPELL_MENDING);
                    mendingTimer = urand(1000, 2000);
                } else mendingTimer -= diff;

                if (graceTimer <= diff)
                {
                    DoCast(me, SPELL_WINDS_GRACE, false);
                    graceTimer = 60000;
                } else graceTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            uint32 mendingTimer;
            uint32 graceTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sikthik_battle_menderAI(creature);
        }
};

// Sik'Thik Bladedancer - 61436
class npc_sikthik_bladedancer : public CreatureScript
{
    enum Spells
    {
        SPELL_BLADE_DANCE           = 124253,
        SPELL_SIKTHIK_STRIKE        = 119354
    };

    public:
        npc_sikthik_bladedancer() : CreatureScript("npc_sikthik_bladedancer") { }

        struct npc_sikthik_bladedancerAI : public ScriptedAI
        {
            npc_sikthik_bladedancerAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                danceTimer = urand(5000, 6000);
                strikeTimer = urand(1000, 2000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (danceTimer <= diff)
                {
                    DoCastVictim(SPELL_BLADE_DANCE, false);
                    danceTimer = urand(7000, 9000);
                } else danceTimer -= diff;

                if (strikeTimer <= diff)
                {
                    DoCastVictim(SPELL_SIKTHIK_STRIKE, false);
                    strikeTimer = urand(5000, 5500);
                } else strikeTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            uint32 danceTimer;
            uint32 strikeTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sikthik_bladedancerAI(creature);
        }
};

// Sik'Thik Builder - 62633
class npc_sikthik_builder : public CreatureScript
{
    enum Spells
    {
        SPELL_HURL_BRICK        = 121762
    };

    public:
        npc_sikthik_builder() : CreatureScript("npc_sikthik_builder") { }

        struct npc_sikthik_builderAI : public ScriptedAI
        {
            npc_sikthik_builderAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                brixTimer = urand(1000, 2000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (brixTimer <= diff)
                {
                    DoCastVictim(SPELL_HURL_BRICK, false);
                    brixTimer = 500;
                } else brixTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            uint32 brixTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sikthik_builderAI(creature);
        }
};

// Sik'Thik engineer - 62632
class npc_sikthik_engineer : public CreatureScript
{
    enum Spells
    {
        SPELL_BURNING_PITCH             = 122259,
        SPELL_CRYSTALLIZE               = 122244,
        SPELL_CRYSTALLIZED_PITCH        = 122246
    };

    public:
        npc_sikthik_engineer() : CreatureScript("npc_sikthik_engineer") { }

        struct npc_sikthik_engineerAI : public ScriptedAI
        {
            npc_sikthik_engineerAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                pitchTimer = urand(1000, 2000);
                crystallizeTimer = 4000;
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_CRYSTALLIZE && target->HasAura(SPELL_BURNING_PITCH))
                    DoCast(target, SPELL_CRYSTALLIZED_PITCH, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (pitchTimer <= diff)
                {
                    DoCast(me, SPELL_BURNING_PITCH, false);
                    pitchTimer = urand(4000, 5000);
                    crystallizeTimer = 1000;
                } else pitchTimer -= diff;

                if (crystallizeTimer <= diff)
                {
                    DoCast(me, SPELL_CRYSTALLIZE, false);
                    crystallizeTimer = 10000;
                } else crystallizeTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            uint32 pitchTimer;
            uint32 crystallizeTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sikthik_engineerAI(creature);
        }
};

// Sik'Thik Vanguard - 61434
class npc_sikthik_vanguard : public CreatureScript
{
    enum Spells
    {
        SPELL_BESIEGE                       = 119347,
        SPELL_WILL_OF_THE_EMPRESS           = 124172
    };

    public:
        npc_sikthik_vanguard() : CreatureScript("npc_sikthik_vanguard") { }

        struct npc_sikthik_vanguardAI : public ScriptedAI
        {
            npc_sikthik_vanguardAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                besiegeTimer = urand(5000, 6000);
                willTimer = 100;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (besiegeTimer <= diff)
                {
                    DoCast(SELECT_TARGET_FARTHEST, SPELL_BESIEGE, false, 1, 0.0f, true, -SPELL_BESIEGE);
                    besiegeTimer = urand(10000, 15000);
                } else besiegeTimer -= diff;

                if (willTimer <= diff)
                {
                    DoCast(me, SPELL_WILL_OF_THE_EMPRESS, true);
                    willTimer = urand(18000, 20000);
                } else willTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            uint32 besiegeTimer;
            uint32 willTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sikthik_vanguardAI(creature);
        }
};

uint8 const chumKiuPathLenght = 10;
Position const chumKiuPath[chumKiuPathLenght] =
{
    { 1851.93f, 5214.89f, 131.169f, 0.0f },
    { 1851.16f, 5214.26f, 131.169f, 0.0f },
    { 1847.3f,  5209.29f, 131.169f, 0.0f },
    { 1848.58f, 5203.1f,  131.169f, 0.0f },
    { 1855.8f,  5199.65f, 131.234f, 0.0f },
    { 1864.0f,  5197.16f, 131.234f, 0.0f },
    { 1872.96f, 5188.56f, 131.234f, 0.0f },
    { 1874.48f, 5181.34f, 131.234f, 0.0f },
    { 1870.9f,  5172.17f, 134.295f, 0.0f },
    { 1870.9f,  5172.17f, 134.295f, 0.0f },
};

 // Shado-Master Chum Kiu - 64517
class npc_chum_kiu : public CreatureScript
{
    enum Spells
    {
        SPELL_BESIEGE                       = 119347,
        SPELL_WILL_OF_THE_EMPRESS           = 124172
    };

    public:
        npc_chum_kiu() : CreatureScript("npc_chum_kiu") { }

        struct npc_chum_kiuAI : public ScriptedAI
        {
            npc_chum_kiuAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* summoner) override
            {
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
                me->GetMotionMaster()->MoveSplinePath(chumKiuPath, chumKiuPathLenght, false, true, 3.0f, false, false, false);
                unstealthTimer = me->GetSplineDuration() + 1000;
            }

            void UpdateAI(uint32 diff) override
            {
                if (unstealthTimer)
                {
                    if (unstealthTimer <= diff)
                    {
                        me->SetFacingTo(0.4886922f);
                        me->RemoveAllAuras();
                        unstealthTimer = 0;
                        arrivalTimer = 1000;
                    } else unstealthTimer -= diff;
                }

                if (arrivalTimer)
                {
                    if (arrivalTimer <= diff)
                    {
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
                        if (Player * target = me->SelectNearestPlayer(50.0f))
                            Talk(0, target);
                        arrivalTimer = 0;
                    } else arrivalTimer -= diff;
                }
            }

        private:
            uint32 unstealthTimer;
            uint32 arrivalTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_chum_kiuAI(creature);
        }
};
void AddSC_siege_of_niuzao_temple()
{
    // Hollowed Out Tree
    new npc_sikthik_guardian();
    new npc_sikthik_amber_weaver();
    new npc_resin_flake();
    new npc_sap_spray();
    new npc_mantid_soldier_catapult();
    new npc_mantid_catapult();
    new npc_niuzao_shado_pan_prisoner();
    new npc_sikthik_battle_mender();
    new npc_sikthik_bladedancer();
    new npc_sikthik_builder();
    new npc_sikthik_engineer();
    new npc_sikthik_vanguard();
    new spell_resin_weaving();
    new npc_chum_kiu();
};