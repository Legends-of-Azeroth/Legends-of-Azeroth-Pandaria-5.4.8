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

enum CreatureIds
{
    NPC_AEONAXX = 50062,
};

class boss_aeonaxx : public CreatureScript
{
    public:
        boss_aeonaxx() : CreatureScript("boss_aeonaxx") { }

        struct boss_aeonaxxAI : public ScriptedAI
        {
            boss_aeonaxxAI(Creature* creature) : ScriptedAI(creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;
                
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_aeonaxxAI(creature);
        }
};
enum realmOfEarthTypes
{
    SPELL_DEEPHOLM_INTRO_TELEPORT = 84073,
    SPELL_DEEPHOLM_INTRO_TAXI     = 84101,

    NPC_AGGRA            = 45006,
    NPC_WYVERN           = 45005,
    NPC_WYVERN_TRANSPORT = 45024,

    QUEST_DEEPHOLM_REALM_OF_EARTH = 27123,

    TALK_INTRO = 0,
    TALK_SPECIAL_1 = 1,
    TALK_SPECIAL_2 = 2,
    TALK_SPECIAL_3 = 3,
    TALK_SPECIAL_4 = 4,
    TALK_SPECIAL_5 = 5,
    TALK_SPECIAL_6 = 6,
    TALK_SPECIAL_7 = 7,
    TALK_SPECIAL_8 = 8,
    TALK_SPECIAL_9 = 9,
    TALK_SPECIAL_10 = 10,
};

const Position wyvernMaelstormPath[10] =
{
    { 810.61f, 1009.42f, 17.96f, 0.0f },
    { 788.45f, 857.77f, -33.96f, 0.0f },
    { 850.80f, 705.77f, -60.97f, 0.0f },
    { 968.77f, 741.15f, -83.58f, 0.0f },
    { 940.87f, 828.13f, -102.28f, 0.0f },
    { 864.19f, 839.58f, -125.39f, 0.0f },
    { 868.62f, 759.38f, -145.01f, 0.0f },
    { 933.94f, 762.14f, -171.41f, 0.0f },
    { 910.86f, 808.24f, -197.10f, 0.0f },
    { 893.38f, 789.12f, -247.18f, 0.0f },
};

const Position wyvernDeepholmPath[8] =
{
    { 1040.91f, 544.689f, 681.86f, 3.97935f },
    { 1098.07f, 627.78f, 485.69f, 0.0f },
    { 1100.67f, 655.00f, 362.34f, 0.0f },
    { 974.18f, 664.47f, 294.96f, 0.0f },
    { 893.63f, 633.28f, 196.87f, 0.0f },
    { 858.82f, 573.17f, 148.96f, 0.0f },
    { 846.16f, 496.09f, -1.66f, 0.0f },
    { 915.32f, 503.90f, -35.16f, 0.0f },
};

// Aronus 45005
struct npc_deepholm_wyvern : public CreatureAI
{
    npc_deepholm_wyvern(Creature* creature) : CreatureAI(creature)
    {
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (clicker && clicker->ToPlayer() && clicker->ToPlayer()->GetQuestStatus(QUEST_DEEPHOLM_REALM_OF_EARTH) == QUEST_STATUS_COMPLETE)
            clicker->CastSpell(clicker, SPELL_DEEPHOLM_INTRO_TAXI, true);
    }

    void UpdateAI(const uint32 /*diff*/) override { }
};

// Wyvern 45024
struct npc_deepholm_wyvern_ride : public CreatureAI
{
    npc_deepholm_wyvern_ride(Creature* creature) : CreatureAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid targetGUID;
    ObjectGuid aggraGUID;
    uint32 delay;

    void IsSummonedBy(Unit* summoner) override
    {
        targetGUID = summoner->GetGUID();
        me->OverrideInhabitType(INHABIT_AIR);
        me->SetAnimTier(AnimTier::Fly);
        me->UpdateMovementFlags();
        me->SetSpeed(MOVE_FLIGHT, 2.5f);

        if (me->GetMapId() != 646) // Maelstorm
        {
            me->SetSpeed(MOVE_FLIGHT, 3.1f);

            if (Creature* aggra = me->SummonCreature(NPC_AGGRA, *me, TEMPSUMMON_MANUAL_DESPAWN))
            {
                aggraGUID = aggra->GetGUID();
                aggra->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
            }

            me->StopMoving();
            Movement::MoveSplineInit init(me);
            for (auto itr : wyvernMaelstormPath)
                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

            init.SetSmooth();
            init.SetFly();
            init.Launch();

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                if (Player* target = ObjectAccessor::GetPlayer(*me, targetGUID))
                {
                    target->_ExitVehicle();
                    target->CastSpell(target, SPELL_DEEPHOLM_INTRO_TELEPORT, true);
                }

                if (Creature* aggra = ObjectAccessor::GetCreature(*me, aggraGUID))
                    aggra->DespawnOrUnsummon(500);

                me->DespawnOrUnsummon(500);
            });
        }
        else // Deepholm
        {
            me->SetSpeed(MOVE_FLIGHT, 2.6f);

            scheduler
                .Schedule(Milliseconds(4000), [this](TaskContext context)
            {
                if (Creature* aggra = me->SummonCreature(NPC_AGGRA, *me, TEMPSUMMON_MANUAL_DESPAWN))
                {
                    aggraGUID = aggra->GetGUID();
                    aggra->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
                }

                me->StopMoving();
                Movement::MoveSplineInit init(me);
                for (auto itr : wyvernDeepholmPath)
                    init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                init.SetSmooth();
                init.SetFly();
                init.Launch();

                scheduler
                    .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                {
                    if (Player* target = ObjectAccessor::GetPlayer(*me, targetGUID))
                        target->ExitVehicle();

                    if (Creature* aggra = ObjectAccessor::GetCreature(*me, aggraGUID))
                    {
                        aggra->AI()->Talk(TALK_SPECIAL_10);
                        aggra->DespawnOrUnsummon(5000);
                    }

                    me->DespawnOrUnsummon(500);
                });
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Aggra 45006
struct npc_maelstorm_aggra : public CreatureAI
{
    npc_maelstorm_aggra(Creature* creature) : CreatureAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid summonerGUID;
    uint32 delay;

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (me->GetMapId() != 646) // Maelstorm
        {
            delay = 3200;
            scheduler
                .Schedule(Milliseconds(delay), [this](TaskContext context)
            {
                Talk(TALK_INTRO);
            });

            scheduler
                .Schedule(Milliseconds(delay += 10000), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_1);
            });

            scheduler
                .Schedule(Milliseconds(delay += 10500), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_2);
            });

            scheduler
                .Schedule(Milliseconds(delay += 8500), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_3);
            });
        }
        else // Deepholm
        {
            delay = 1250;
            scheduler
                .Schedule(Milliseconds(delay), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_4);
            });

            scheduler
                .Schedule(Milliseconds(delay += 3900), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_5);
            });

            scheduler
                .Schedule(Milliseconds(delay += 6000), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_6);
            });

            scheduler
                .Schedule(Milliseconds(delay += 14400), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_7);
            });

            scheduler
                .Schedule(Milliseconds(delay += 14600), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_8);
            });

            scheduler
                .Schedule(Milliseconds(delay += 9800), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_9);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Deepholm Intro Teleport 84073
class spell_deepholm_intro_teleport : public AuraScript
{
    PrepareAuraScript(spell_deepholm_intro_teleport);

    bool Load() override
    {
        isLoading = false;
        return true;
    }

    void OnUpdate(uint32 /*diff*/, AuraEffect* aurEff)
    {
        if (GetOwner() && GetOwner()->ToUnit() && GetOwner()->ToUnit()->GetMapId() == 646 && !isLoading)
        {
            isLoading = true;
            GetOwner()->ToUnit()->CastSpell(GetOwner()->ToUnit(), SPELL_DEEPHOLM_INTRO_TAXI, true);
        }
    }

    private:
        bool isLoading;

    void Register() override
    {
        OnEffectUpdate += AuraEffectUpdateFn(spell_deepholm_intro_teleport::OnUpdate, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

void AddSC_deepholm()
{
    new boss_aeonaxx();
    new creature_script<npc_deepholm_wyvern>("npc_deepholm_wyvern");
    new creature_script<npc_deepholm_wyvern_ride>("npc_deepholm_wyvern_ride");
    new creature_script<npc_maelstorm_aggra>("npc_maelstorm_aggra");

    new aura_script<spell_deepholm_intro_teleport>("spell_deepholm_intro_teleport");
}
