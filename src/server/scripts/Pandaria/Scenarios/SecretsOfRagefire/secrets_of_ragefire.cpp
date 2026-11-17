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

#include "secrets_of_ragefire.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "SpellScript.h"
#include "Vehicle.h"

enum Spells
{
    SPELL_FUSE                  = 140896,
    SPELL_TWISTED_ELEMENTS      = 142296,
    SPELL_GLACIAL_FREEZE_TOTEM  = 142320,
    SPELL_RUINED_EARTH          = 142306,
    SPELL_RUINED_EARTH_SELECTOR = 142307,
    SPELL_RUINED_EARTH_SUMM     = 142308,
    SPELL_RUINED_EARTH_SUMM_2   = 142309,
    SPELL_RUINED_EARTH_SUMM_3   = 142316,
    SPELL_RUINED_EARTH_VISUAL   = 142310,
    SPELL_RUINED_EARTH_EFF      = 142311,
    SPELL_GLACIAL_FREEZE        = 142321,
    SPELL_GLACIAL_FREEZE_EFF    = 142322,
    SPELL_LAVA_BURST            = 142338,
    SPELL_POISON_BOLT_TOTEM     = 142339,
    SPELL_EMBERSTRIKES          = 142406,
    SPELL_EMBERSTRIKES_EFF      = 142407,
    SPELL_POOL_OF_EMBERS        = 142413,
    SPELL_SHADOW_STEP           = 80576,
    SPELL_HUGE_EXPLOSION        = 141954,
    SPELL_DEMOLISH_ARMOR        = 142764,
    SPELL_DIRE_RAGE             = 142760,
    SPELL_ATTACH_EGG_YOLK       = 141891,
    SPELL_ATTACH_POOL_PONY      = 141889,
    SPELL_ATTACH_BATTERY_1      = 141887,
    SPELL_ATTACH_BATTERY_2      = 141888,
    SPELL_ATTACH_CANNON_BALL_1  = 141879,
    SPELL_ATTACH_CANNON_BALL_2  = 141880,
    SPELL_ATTACH_CANNON_BALL_3  = 141881,
    SPELL_POISON_BOLT           = 142345,
    SPELL_FLAME_BREATH          = 142745,
    SPELL_SHATTERING_STOMP      = 142771,
    SPELL_SHATTERED_EARTH       = 142766,
    SPELL_SHATTERED_EARTH_EFF   = 142768,
};

enum Events
{
    EVENT_TWISTED_ELEMENTS = 1,
    EVENT_GLACIAL_FREEZE_TOTEM,
    EVENT_RUINED_EARTH,
    EVENT_LAVA_BURST,
    EVENT_POISON_BOLT_TOTEM,
    EVENT_EMBERSTRIKES,
    EVENT_SHADOWSTEP,
    EVENT_DEMOLISH_ARMOR,
    EVENT_DIRE_RAGE,
    EVENT_FLAME_BREATH,
    EVENT_SHATTERING_STOMP,
};

// Grit 70605
struct npc_ragefire_grit : public ScriptedAI
{
    npc_ragefire_grit(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    uint32 delay;
    float x, y;
    bool reachGates;

    void Reset() override
    {
        scheduler.CancelAll();
        delay = 0;
        x = 0.0f; y = 0.0f;
        reachGates = false;
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
                delay = 500;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    Talk(TALK_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 3200), [this](TaskContext context)
                {
                    if (Creature* newt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NEWT) : ObjectGuid::Empty))
                        newt->AI()->Talk(TALK_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 2100), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_1);

                    // Send Newt move forward
                    if (Creature* newt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NEWT) : ObjectGuid::Empty))
                    {
                        Movement::MoveSplineInit init(newt);

                        for (auto&& itr : newtDetonatorPath)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                        init.Launch();
                    }
                });

                scheduler
                    .Schedule(Milliseconds(delay += 3900), [this](TaskContext context)
                {
                    if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                        ticker->AI()->Talk(TALK_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 1000), [this](TaskContext context)
                {
                    // Ticker, Patch, Volt should move to Newt
                    if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                    {
                        Movement::MoveSplineInit init(ticker);

                        for (auto&& itr : tickerDetonatorPath)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                        init.Launch();
                    }

                    if (Creature* volt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLT) : ObjectGuid::Empty))
                    {
                        Movement::MoveSplineInit init(volt);

                        for (auto&& itr : voltDetonatorPath)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                        init.Launch();
                    }

                    if (Creature* patch = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PATCH) : ObjectGuid::Empty))
                    {
                        Movement::MoveSplineInit init(patch);

                        for (auto&& itr : pathDetonatorPath)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                        init.Launch();
                    }
                });

                scheduler
                    .Schedule(Milliseconds(delay += 2500), [this](TaskContext context)
                {
                    if (Player* owner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                        Talk(TALK_SPECIAL_2, owner);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 3800), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_3);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6100), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_4);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6100), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_5);

                    // Move to detonator
                    Movement::MoveSplineInit init(me);

                    for (auto&& itr : gritDetonatorPath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                    init.Launch();

                    delay = me->GetSplineDuration();
                    scheduler
                        .Schedule(Milliseconds(delay), [this](TaskContext context)
                    {
                        Talk(TALK_SPECIAL_6);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 2700), [this](TaskContext context)
                    {
                        if (Creature* newt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NEWT) : ObjectGuid::Empty))
                        {
                            newt->AI()->Talk(TALK_SPECIAL_1);

                            // Move to detonator path
                            newt->StopMoving();

                            Movement::MoveSplineInit init(newt);

                            for (auto&& itr : newtMechaPath)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                            init.Launch();

                            scheduler
                                .Schedule(Milliseconds(newt->GetSplineDuration()), [this](TaskContext context)
                            {
                                if (Creature* newt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NEWT) : ObjectGuid::Empty))
                                    newt->AI()->Talk(TALK_SPECIAL_2);

                                // Activate Bomb!
                                if (Creature* detonator = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_DETONATOR) : ObjectGuid::Empty))
                                {
                                    detonator->SetVisible(true);
                                    detonator->SetFaction(35);
                                    detonator->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                                }
                            });
                        }
                    });
                });
                break;
            case ACTION_INNER_RAGEFIRE:
            {
                if (GameObject* entranceDoor = ObjectAccessor::GetGameObject(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(GO_IRON_GATE) : ObjectGuid::Empty))
                    entranceDoor->SetGoState(GO_STATE_ACTIVE);

                // Update Condition
                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_REACH_RAGEFIRE, 1, me->GetInstanceScript()->GetScenarioGUID(), time(NULL), 0, 0));

                // Remove bombs
                std::list<Creature*> bombs;
                GetCreatureListWithEntryInGrid(bombs, me, NPC_BOMB, 80.0f);

                for (auto&& itr : bombs)
                    itr->DespawnOrUnsummon();

                delay = 2000;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_7);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 1000), [this](TaskContext context)
                {
                    // Move in Cavern!
                    if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                    {
                        ticker->StopMoving();
                        Movement::MoveSplineInit init(ticker);

                        for (auto&& itr : tickerRagefirePath)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                        init.Launch();
                    }

                    if (Creature* volt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLT) : ObjectGuid::Empty))
                    {
                        volt->StopMoving();
                        Movement::MoveSplineInit init(volt);

                        for (auto&& itr : voltRagefirePath)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                        init.Launch();
                    }

                    if (Creature* patch = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PATCH) : ObjectGuid::Empty))
                    {
                        patch->StopMoving();
                        Movement::MoveSplineInit init(patch);

                        for (auto&& itr : patchRagefirePath)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                        init.Launch();
                    }

                    if (Creature* newt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NEWT) : ObjectGuid::Empty))
                    {
                        newt->StopMoving();
                        Movement::MoveSplineInit init(newt);

                        for (auto&& itr : newtRagefirePath)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                        init.Launch();
                    }

                    me->StopMoving();
                    Movement::MoveSplineInit init(me);

                    for (auto&& itr : gritRagefirePath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                    init.Launch();

                    delay = me->GetSplineDuration();
                    scheduler
                        .Schedule(Milliseconds(delay), [this](TaskContext context)
                    {
                        Talk(TALK_SPECIAL_8);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 2000), [this](TaskContext context)
                    {
                        if (Creature* xorenth = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_DARK_SHAMAN_XORENTH) : ObjectGuid::Empty))
                            xorenth->AI()->Talk(TALK_INTRO);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 1600), [this](TaskContext context)
                    {
                        // Attack intruders!
                        if (Creature* xorenth = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_DARK_SHAMAN_XORENTH) : ObjectGuid::Empty))
                        {
                            std::list<Creature*> korkronsList;
                            GetCreatureListWithEntryInGrid(korkronsList, me, NPC_KORKRON_DARK_SHAMAN, 80.0f);
                            GetCreatureListWithEntryInGrid(korkronsList, me, NPC_KORKRON_EMBERGUARD, 80.0f);
                            GetCreatureListWithEntryInGrid(korkronsList, me, NPC_KORKRON_SHADOWBLADE, 80.0f);

                            for (auto&& itr : korkronsList)
                                itr->SetInCombatWithZone();
                        }

                        if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                            ticker->AI()->Talk(TALK_SPECIAL_1);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 4700), [this](TaskContext context)
                    {
                        Talk(TALK_SPECIAL_9);
                    });
                });
                break;
            }
            case ACTION_RECONNAISSANCE:
                // Goblind move Forward
                for (auto&& itr : goblinsList)
                {
                    if (Creature* gobSquad = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(itr) : ObjectGuid::Empty))
                    {
                        GetPositionWithDistInOrientation(gobSquad, 45.0f, gobSquad->GetAngle(me), x, y);
                        float z = gobSquad->GetPositionZ();
                        gobSquad->UpdateAllowedPositionZ(x, y, z);
                        gobSquad->GetMotionMaster()->MovePoint(0, x, y, z);

                        if (me->GetEntry() == itr)
                        {
                            delay = me->GetSplineDuration();
                            scheduler
                                .Schedule(Milliseconds(delay), [this](TaskContext context)
                            {
                                if (Creature* volt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLT) : ObjectGuid::Empty))
                                    volt->AI()->Talk(TALK_INTRO);
                            });

                            scheduler
                                .Schedule(Milliseconds(delay += 4500), [this](TaskContext context)
                            {
                                Talk(TALK_SPECIAL_12);
                            });

                            scheduler
                                .Schedule(Milliseconds(delay += 3500), [this](TaskContext context)
                            {
                                Talk(TALK_SPECIAL_13);
                            });
                        }
                    }
                }
                break;
            case ACTION_SEALED_GATES:
            {
                if (Player* owner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    Talk(TALK_SPECIAL_14, owner);

                // Send Move to Sealed Gates
                if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                {
                    ticker->StopMoving();
                    Movement::MoveSplineInit init(ticker);

                    init.MoveTo(G3D::Vector3(tickerSealedGatePath.GetPositionX(), tickerSealedGatePath.GetPositionY(), tickerSealedGatePath.GetPositionZ()));

                    init.Launch();
                }

                if (Creature* volt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLT) : ObjectGuid::Empty))
                {
                    volt->StopMoving();
                    Movement::MoveSplineInit init(volt);

                    for (auto&& itr : voltSealedGatePath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                    init.Launch();
                }

                if (Creature* patch = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PATCH) : ObjectGuid::Empty))
                {
                    patch->StopMoving();
                    Movement::MoveSplineInit init(patch);

                    for (auto&& itr : pathSealedGatePath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                    init.Launch();
                }

                if (Creature* newt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NEWT) : ObjectGuid::Empty))
                {
                    newt->StopMoving();
                    Movement::MoveSplineInit init(newt);

                    for (auto&& itr : newtSealedGatePath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                    init.Launch();
                }

                me->StopMoving();
                Movement::MoveSplineInit init(me);

                init.MoveTo(G3D::Vector3(gritSealedGatePath.GetPositionX(), gritSealedGatePath.GetPositionY(), gritSealedGatePath.GetPositionZ()));

                init.Launch();
                break;
            }
            case ACTION_REACH_SEALED_GATES:
            {
                if (reachGates)
                    break;

                reachGates = true;
                Talk(TALK_SPECIAL_15);

                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_MEET_WITH_TICKER, 1, me->GetInstanceScript()->GetScenarioGUID(), time(NULL), 0, 0));

                delay = 1000;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SetData(PROTDRAKES_DATA, DONE);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 2700), [this](TaskContext context)
                {
                    if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                        ticker->AI()->Talk(TALK_SPECIAL_2);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 2700), [this](TaskContext context)
                {
                    // Take Mantid Bomb in hands
                    if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                    {
                        ticker->AI()->Talk(TALK_SPECIAL_3);

                        if (Creature* bombTarget = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MANTID_BOMB_TARGET) : ObjectGuid::Empty))
                            bombTarget->SummonCreature(NPC_MANTID_BOMB, *bombTarget, TEMPSUMMON_MANUAL_DESPAWN);
                    }
                });

                scheduler
                    .Schedule(Milliseconds(delay += 3600), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_16);

                    if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                        ticker->GetMotionMaster()->MovePoint(0, tickerSealedGateReachPoint);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6100), [this](TaskContext context)
                {
                    if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                    {
                        ticker->AI()->Talk(TALK_SPECIAL_4);
                        ticker->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() + M_PI));
                    }
                });

                scheduler
                    .Schedule(Milliseconds(delay += 4900), [this](TaskContext context)
                {
                    if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                        ticker->AI()->Talk(TALK_SPECIAL_5);

                    // Allow to pickup objects
                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SetData(DATA_ALLOW_BRING_ITEMS, DONE);
                });
                break;
            case ACTION_EXPLOSIVE_GATES:
            {
                if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                    ticker->AI()->Talk(TALK_SPECIAL_11);

                delay = 2350;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                        ticker->AI()->Talk(TALK_SPECIAL_12);

                    if (Creature* mantidBomb = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MANTID_BOMB) : ObjectGuid::Empty))
                    {
                        // Visual Upgrade
                        mantidBomb->CastSpell(mantidBomb, SPELL_ATTACH_EGG_YOLK, true);
                        mantidBomb->CastSpell(mantidBomb, SPELL_ATTACH_POOL_PONY, true);
                        mantidBomb->CastSpell(mantidBomb, SPELL_ATTACH_BATTERY_1, true);
                        mantidBomb->CastSpell(mantidBomb, SPELL_ATTACH_BATTERY_2, true);
                        mantidBomb->CastSpell(mantidBomb, SPELL_ATTACH_CANNON_BALL_1, true);
                        mantidBomb->CastSpell(mantidBomb, SPELL_ATTACH_CANNON_BALL_2, true);
                        mantidBomb->CastSpell(mantidBomb, SPELL_ATTACH_CANNON_BALL_3, true);
                    }
                });

                scheduler
                    .Schedule(Milliseconds(delay += 2900), [this](TaskContext context)
                {
                    if (Creature* newt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NEWT) : ObjectGuid::Empty))
                        newt->AI()->Talk(TALK_SPECIAL_4);

                    if (Creature* volt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLT) : ObjectGuid::Empty))
                        volt->AI()->Talk(TALK_SPECIAL_1);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 3700), [this](TaskContext context)
                {
                    if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                        ticker->AI()->Talk(TALK_SPECIAL_13);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 2600), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_17);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6100), [this](TaskContext context)
                {
                    if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                        ticker->AI()->Talk(TALK_SPECIAL_14);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 3600), [this](TaskContext context)
                {
                    if (Creature* ticker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
                        ticker->AI()->Talk(TALK_SPECIAL_15);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 2700), [this](TaskContext context)
                {
                    // Explosion!
                    if (Creature* mantidBomb = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_MANTID_BOMB) : ObjectGuid::Empty))
                    {
                        mantidBomb->CastSpell(mantidBomb, SPELL_HUGE_EXPLOSION, false);
                        mantidBomb->DespawnOrUnsummon(1000);
                    }

                    if (Creature* portalController = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_EMERGENCY_TELEPORTER) : ObjectGuid::Empty))
                    {
                        for (auto&& itr : goblinsList)
                        {
                            if (Creature* gobSquad = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(itr) : ObjectGuid::Empty))
                            {
                                GetPositionWithDistInOrientation(gobSquad, 35.0f, gobSquad->GetAngle(portalController), x, y);
                                float z = gobSquad->GetPositionZ();
                                gobSquad->UpdateAllowedPositionZ(x, y, z);
                                gobSquad->GetMotionMaster()->MoveJump(x, y, z, 25.0f, 25.0f);
                            }
                        }
                    }
                });

                scheduler
                    .Schedule(Milliseconds(delay += 3900), [this](TaskContext context)
                {
                    if (Creature* volt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLT) : ObjectGuid::Empty))
                        volt->AI()->Talk(TALK_SPECIAL_2);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 1000), [this](TaskContext context)
                {
                    if (Creature* patch = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PATCH) : ObjectGuid::Empty))
                        patch->AI()->Talk(TALK_SPECIAL_1);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 1500), [this](TaskContext context)
                {
                    if (Creature* volt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLT) : ObjectGuid::Empty))
                        volt->AI()->Talk(TALK_SPECIAL_3);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 2100), [this](TaskContext context)
                {
                    if (Creature* patch = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PATCH) : ObjectGuid::Empty))
                        patch->AI()->Talk(TALK_SPECIAL_2);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 1500), [this](TaskContext context)
                {
                    if (Creature* newt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NEWT) : ObjectGuid::Empty))
                        newt->AI()->Talk(TALK_SPECIAL_5);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 1380), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_18);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 4900), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_19);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 1000), [this](TaskContext context)
                {
                    if (Creature* patch = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PATCH) : ObjectGuid::Empty))
                        patch->AI()->Talk(TALK_SPECIAL_3);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 2500), [this](TaskContext context)
                {
                    if (Creature* elaglo = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_OVERSEER_ELAGLO) : ObjectGuid::Empty))
                        elaglo->AI()->DoAction(ACTION_START_INTRO);
                });
                break;
            }
            case ACTION_GOB_SQUAD_DEFENDED:
                if (Creature* patch = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PATCH) : ObjectGuid::Empty))
                    patch->AI()->Talk(TALK_SPECIAL_4);

                scheduler
                    .Schedule(Milliseconds(5500), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_22);
                });
                break;
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Detonator 70662
struct npc_ragefire_detonator : public ScriptedAI
{
    npc_ragefire_detonator(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    bool hasClick;
    uint32 moveDuration;

    void Reset() override
    {
        hasClick = false;
        moveDuration = 0;
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        // Prevent Multy Clicking
        if (hasClick)
            return;

        hasClick = true;
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

        // Send link to bombs
        std::list<Creature*> bunnyList;
        GetCreatureListWithEntryInGrid(bunnyList, me, NPC_SLG_GENERIC_MOP, 20.0f);

        for (auto&& itr : bunnyList)
        {
            itr->RemoveAllAuras();
            itr->CastSpell(itr, SPELL_FUSE, true);
            
            if (Creature* bomb = me->FindNearestCreature(NPC_BOMB, 50.0f, true))
            {
                itr->GetMotionMaster()->MovePoint(0, *bomb);

                if (!moveDuration)
                    moveDuration = itr->GetSplineDuration();
            }
        }

        if (moveDuration)
        {
            scheduler
                .Schedule(Milliseconds(moveDuration), [this](TaskContext context)
            {
                if (Creature* grit = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_GRIT) : ObjectGuid::Empty))
                    grit->AI()->DoAction(ACTION_INNER_RAGEFIRE);
            });
        }
    }

    void UpdateAI(uint32 diff) override 
    { 
        scheduler.Update(diff);
    };
};

// Dark Shaman Xorenth 70683
struct npc_ragefire_dark_shaman_xorenth : public customCreatureAI
{
    npc_ragefire_dark_shaman_xorenth(Creature* creature) : customCreatureAI(creature) 
    { 
        korkronDied = 0;
        delay = 0;
        x = 0.0f; y = 0.0f;
    }

    TaskScheduler scheduler;
    uint32 korkronDied;
    uint32 delay;
    float x, y;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_TWISTED_ELEMENTS, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_GLACIAL_FREEZE_TOTEM, 8 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_RUINED_EARTH, 16.5 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summon->SetInCombatWithZone();

        if (summon->GetEntry() == NPC_GLACIAL_FREEZE_TOTEM)
            summon->CastSpell(summon, SPELL_GLACIAL_FREEZE, true);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetInstanceScript())
            me->GetInstanceScript()->SetData(DATA_INFILTRATION, DONE);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && ++korkronDied > 5 && me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_INFILTRATION) != DONE)
        {
            Talk(TALK_SPECIAL_1);

            delay = 2400;
            scheduler
                .Schedule(Milliseconds(delay), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_2);

                // Send peons inner sanctum
                std::list<Creature*> peonsList;
                GetCreatureListWithEntryInGrid(peonsList, me, NPC_RAGEFIRE_PEON, 150.0f);

                for (auto&& itr : peonsList)
                {
                    Movement::MoveSplineInit init(itr);

                    for (auto&& itr : korkronPeonsLeavePath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-7.0f, 7.0f), itr.GetPositionY() + frand(-7.0f, 7.0f), itr.GetPositionZ()));

                    init.Launch();
                    itr->DespawnOrUnsummon(itr->GetSplineDuration());
                }

                // Goblind move Forward
                for (auto&& itr : goblinsList)
                {
                    if (Creature* gobSquad = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(itr) : ObjectGuid::Empty))
                    {
                        gobSquad->SetFacingToObject(me);
                        GetPositionWithDistInOrientation(gobSquad, 25.0f, gobSquad->GetAngle(me), x, y);
                        float z = gobSquad->GetPositionZ();
                        gobSquad->UpdateAllowedPositionZ(x, y, z);
                        gobSquad->GetMotionMaster()->MovePoint(0, x, y, z);
                    }
                }
            });

            scheduler
                .Schedule(Milliseconds(delay += 5000), [this](TaskContext context)
            {
                if (Creature* grit = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_GRIT) : ObjectGuid::Empty))
                    grit->AI()->Talk(TALK_SPECIAL_10);
            });

            scheduler
                .Schedule(Milliseconds(delay += 2250), [this](TaskContext context)
            {
                DoCast(me, SPELL_SEALED_GATE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                Talk(TALK_SPECIAL_3);
            });

            scheduler
                .Schedule(Milliseconds(delay += 4800), [this](TaskContext context)
            {
                if (Creature* grit = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_GRIT) : ObjectGuid::Empty))
                    grit->AI()->Talk(TALK_SPECIAL_11);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_GLACIAL_FREEZE_TOTEM) // Announce
                Talk(TALK_SPECIAL_4);

            if (eventId == SPELL_RUINED_EARTH) // Announce
                Talk(TALK_SPECIAL_5);

            ExecuteTargetEvent(SPELL_TWISTED_ELEMENTS, urand(2.6 * IN_MILLISECONDS, 3 * IN_MILLISECONDS), EVENT_TWISTED_ELEMENTS, eventId);
            ExecuteTargetEvent(SPELL_GLACIAL_FREEZE_TOTEM, 16.5 * IN_MILLISECONDS, EVENT_GLACIAL_FREEZE_TOTEM, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_RUINED_EARTH, 35 * IN_MILLISECONDS, EVENT_RUINED_EARTH, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// RUined Earth 71262
struct npc_ragefire_ruined_earth : public ScriptedAI
{
    npc_ragefire_ruined_earth(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        // Triggering Spell not exist
        scheduler
            .Schedule(Milliseconds(1500), [this](TaskContext context)
        {
            DoCast(me, SPELL_RUINED_EARTH_EFF);
            context.Repeat(Seconds(1));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    };
};

// Korkron Dark Shaman 71245
struct npc_ragefire_korkron_dark_shaman : public customCreatureAI
{
    npc_ragefire_korkron_dark_shaman(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_LAVA_BURST, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_POISON_BOLT_TOTEM, urand(8 * IN_MILLISECONDS, 12.5 * IN_MILLISECONDS));
    }

    void JustSummoned(Creature* summon) override
    {
        summon->SetInCombatWithZone();
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
            me->SetHomePosition(*me);
            me->SetInCombatWithZone();
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_INFILTRATION) == DONE)
            return;

        if (Creature* xorenth = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_DARK_SHAMAN_XORENTH) : ObjectGuid::Empty))
            xorenth->AI()->DoAction(ACTION_START_INTRO);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_LAVA_BURST, urand(2.1 * IN_MILLISECONDS, 2.5 * IN_MILLISECONDS), EVENT_LAVA_BURST, eventId);
            ExecuteTargetEvent(SPELL_POISON_BOLT_TOTEM, 16.5 * IN_MILLISECONDS, EVENT_POISON_BOLT_TOTEM, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Korkron Emberguard 70798
struct npc_ragefire_korkron_emberguard : public customCreatureAI
{
    npc_ragefire_korkron_emberguard(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_EMBERSTRIKES, urand(6.5 * IN_MILLISECONDS, 12.5 * IN_MILLISECONDS));
    }

    void JustSummoned(Creature* summon) override
    {
        summon->SetInCombatWithZone();
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
            me->SetHomePosition(*me);
            me->SetInCombatWithZone();
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_INFILTRATION) == DONE)
            return;

        if (Creature* xorenth = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_DARK_SHAMAN_XORENTH) : ObjectGuid::Empty))
            xorenth->AI()->DoAction(ACTION_START_INTRO);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_EMBERSTRIKES, 18 * IN_MILLISECONDS, EVENT_EMBERSTRIKES, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Investigated Item 71209, 71031, 70901
struct npc_ragefire_investigated_item : public ScriptedAI
{
    npc_ragefire_investigated_item(Creature* creature) : ScriptedAI(creature) { }

    bool hasClick;

    void Reset() override
    {
        me->SetDisplayFromModel(1);

        if (me->GetEntry() == NPC_PANDARIA_ARTIFACTS)
            me->SetObjectScale(2.0f);

        hasClick = false;
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        // Prevent Multy Clicking
        if (hasClick)
            return;

        hasClick = true;
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

        if (me->GetInstanceScript())
        {
            me->GetInstanceScript()->SetData(DATA_RECONNAISSANCE, 1);

            // Update Condition
            me->GetInstanceScript()->SendScenarioProgressUpdate(CriteriaProgressData(investigatedItems.find(me->GetEntry())->second, 1, me->GetInstanceScript()->GetScenarioGUID(), time(NULL), 0, 0));
        }

        // Yells for pickup
        switch (me->GetEntry())
        {
            case NPC_PANDARIA_ARTIFACTS:
            {
                if (Creature* newt = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NEWT) : ObjectGuid::Empty))
                    newt->AI()->Talk(TALK_SPECIAL_3);
                break;
            }
            case NPC_PROTO_DRAKE_EGGS:
            {
                if (Creature* patch = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PATCH) : ObjectGuid::Empty))
                    patch->AI()->Talk(TALK_INTRO);
                break;
            }
        }
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Korkron Shadowblade 71244
struct npc_ragefire_korkron_shadowblade : public customCreatureAI
{
    npc_ragefire_korkron_shadowblade(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHADOWSTEP, urand(6.5 * IN_MILLISECONDS, 12.5 * IN_MILLISECONDS));
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_INFILTRATION) == DONE)
            return;

        if (Creature* xorenth = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_DARK_SHAMAN_XORENTH) : ObjectGuid::Empty))
            xorenth->AI()->DoAction(ACTION_START_INTRO);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
            me->SetHomePosition(*me);
            me->SetInCombatWithZone();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_SHADOWSTEP)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                {
                    DoCast(target, SPELL_SHADOW_STEP);
                    AttackStart(target);
                    me->SetReactState(REACT_PASSIVE);
                }

                events.ScheduleEvent(EVENT_SHADOWSTEP, urand(6.5 * IN_MILLISECONDS, 12.5 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Bring Items 71195, 71175, 71197, 71176
struct npc_ragefire_bring_items : public ScriptedAI
{
    npc_ragefire_bring_items(Creature* creature) : ScriptedAI(creature) { }

    uint32 allowCharges;
    uint32 requiredAuraId;

    void Reset() override
    {
        auto ptr = invBringItemsType.find(me->GetEntry());
        allowCharges = ptr->second[0];
        requiredAuraId = ptr->second[1];
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        // Prevent Multy Clicking
        if (!allowCharges || clicker->HasAura(requiredAuraId))
            return;

        allowCharges--;

        if (!allowCharges)
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

        clicker->CastSpell(clicker, requiredAuraId, true);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Overseer Elaglo 71030
struct npc_ragefire_overseer_elaglo : public customCreatureAI
{
    npc_ragefire_overseer_elaglo(Creature* creature) : customCreatureAI(creature) 
    {
        x = 0.0f; y = 0.0f;
    }

    TaskScheduler scheduler;
    std::vector<ObjectGuid> summonerGUIDs;
    uint32 waveId;
    float x, y;

    void Reset() override
    {
        events.Reset();
        summonerGUIDs.clear();
        waveId = 0;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_DEMOLISH_ARMOR, urand(8 * IN_MILLISECONDS, 12.5 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_SHATTERING_STOMP, 6.5 * IN_MILLISECONDS);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->SetVisible(true);
            Talk(TALK_SPECIAL_2);
            me->GetInstanceScript()->SetData(DATA_HOLDOUT, IN_PROGRESS);

            scheduler
                .Schedule(Milliseconds(3700), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_3);
            });

            scheduler
                .Schedule(Milliseconds(3600), [this](TaskContext context)
            {
                if (Creature* grit = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_GRIT) : ObjectGuid::Empty))
                    grit->AI()->Talk(TALK_SPECIAL_20);

                Talk(TALK_SPECIAL_4);
                Talk(TALK_SPECIAL_5); // Announce

                // Start summon waves here
                for (auto&& itr : invKorkronAssault1)
                {
                    if (Creature* assault = me->SummonCreature(itr.first, itr.second, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        assault->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
                        GetPositionWithDistInOrientation(assault, 35.0f, assault->GetOrientation(), x, y);
                        float z = assault->GetPositionZ();
                        assault->UpdateAllowedPositionZ(x, y, z);
                        assault->GetMotionMaster()->MoveJump(x, y, z, 20.0f, 20.0f, EVENT_JUMP);
                    }
                }
            });
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetInstanceScript())
            me->GetInstanceScript()->SetData(DATA_HOLDOUT, DONE);
    }

    void JustSummoned(Creature* summon) override
    {
        summonerGUIDs.push_back(summon->GetGUID());
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
            me->SetHomePosition(*me);
            me->SetInCombatWithZone();
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (std::find(summonerGUIDs.begin(), summonerGUIDs.end(), summon->GetGUID()) != summonerGUIDs.end())
            summonerGUIDs.erase(std::find(summonerGUIDs.begin(), summonerGUIDs.end(), summon->GetGUID()));

        // Summon new wave
        if (summonerGUIDs.empty())
        {
            waveId++;

            if (waveId > 3) // Elaglo has arrived in combat
            {
                Talk(TALK_SPECIAL_6);
                GetPositionWithDistInOrientation(me, 35.0f, me->GetOrientation(), x, y);
                float z = me->GetPositionZ();
                me->UpdateAllowedPositionZ(x, y, z);
                me->GetMotionMaster()->MoveJump(x, y, z, 20.0f, 20.0f, EVENT_JUMP);
                return;
            }

            std::multimap<uint32, Position> tempWaveStrore = invAssaultType.find(waveId)->second;

            for (auto&& itr : tempWaveStrore)
            {
                if (Creature* assault = me->SummonCreature(itr.first, itr.second, TEMPSUMMON_MANUAL_DESPAWN))
                {
                    assault->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
                    GetPositionWithDistInOrientation(assault, 45.0f, assault->GetOrientation(), x, y);
                    float z = assault->GetPositionZ();
                    assault->UpdateAllowedPositionZ(x, y, z);
                    assault->GetMotionMaster()->MoveJump(x, y, z, 20.0f, 20.0f, EVENT_JUMP);
                }
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DEMOLISH_ARMOR:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_DEMOLISH_ARMOR);

                    events.ScheduleEvent(EVENT_DEMOLISH_ARMOR, urand(6.5 * IN_MILLISECONDS, 12.5 * IN_MILLISECONDS));
                    break;
                case EVENT_SHATTERING_STOMP:
                    DoCast(me, SPELL_SHATTERING_STOMP);
                    events.ScheduleEvent(EVENT_SHATTERING_STOMP, 10 * IN_MILLISECONDS);
                    break;
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Korkron Dire Solder 70665
struct npc_ragefire_korkron_dire_solder : public customCreatureAI
{
    npc_ragefire_korkron_dire_solder(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_DIRE_RAGE, 6 * IN_MILLISECONDS);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
            me->SetHomePosition(*me);
            me->SetInCombatWithZone();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_DIRE_RAGE)
            {
                DoCast(me, SPELL_DIRE_RAGE);
                events.ScheduleEvent(EVENT_DIRE_RAGE, 10 * IN_MILLISECONDS);
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Poison Bolt Totem 71334
struct npc_ragefire_poison_bolt_totem : public ScriptedAI
{
    npc_ragefire_poison_bolt_totem(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            DoCast(me, SPELL_POISON_BOLT);
            context.Repeat(Milliseconds(2500));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    };
};

// Flame Hound 70702
struct npc_ragefire_flame_hound : public customCreatureAI
{
    npc_ragefire_flame_hound(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FLAME_BREATH, urand(4.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
            me->SetHomePosition(*me);
            me->SetInCombatWithZone();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_FLAME_BREATH)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_FLAME_BREATH);

                events.ScheduleEvent(EVENT_FLAME_BREATH, 10 * IN_MILLISECONDS);
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Shattered Earth
struct npc_ragefire_shattered_earth : public ScriptedAI
{
    npc_ragefire_shattered_earth(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetDisplayFromModel(1);
        DoCast(me, SPELL_SHATTERED_EARTH);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// RUined Earth Selector 142307
class spell_ragefire_ruined_earth_selector : public SpellScript
{
    PrepareSpellScript(spell_ragefire_ruined_earth_selector);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ragefire_ruined_earth_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// 987. Summoned by 142407 - Emberstrikes
class sat_ragefire_emberstrikes : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && (triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() || triggering->ToCreature() && IsGobSquad(triggering->ToCreature()->GetEntry()));
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* itr = triggering->ToPlayer())
                caster->CastSpell(itr, SPELL_POOL_OF_EMBERS, true);
            else if (Creature* itr = triggering->ToCreature())
            {
                // Not deal damage from spell - they`r 35 faction in sniffs, we`r 16, just show that`s achiev fail
                itr->SetHealth((uint32)(itr->GetMaxHealth() * 0.9));
                caster->GetMap()->SetWorldState(WORLDSTATE_FEW_PROUD_GOB_SQUAD, 0);
            }
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_POOL_OF_EMBERS);
        else if (Creature* itr = triggering->ToCreature())
            itr->RemoveAurasDueToSpell(SPELL_POOL_OF_EMBERS);
    }

    bool IsGobSquad(uint32 entry)
    {
        for (auto&& itr : goblinsList)
            if (itr == entry)
                return true;

        return false;
    }
};

// Areatrigger 9078
class AreaTrigger_at_ragefire_sealed_gate : public AreaTriggerScript
{
    public:
        AreaTrigger_at_ragefire_sealed_gate() : AreaTriggerScript("AreaTrigger_at_ragefire_sealed_gate") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (player->GetInstanceScript() && player->GetInstanceScript()->GetData(DATA_RECONNAISSANCE) != DONE)
                return false;

            if (Creature* grit = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_GRIT) : ObjectGuid::Empty))
                grit->AI()->DoAction(ACTION_REACH_SEALED_GATES);

            return true;
        }
};

// Areatrigger 9091
class AreaTrigger_at_ragefire_sealed_gate_items : public AreaTriggerScript
{
    public:
        AreaTrigger_at_ragefire_sealed_gate_items() : AreaTriggerScript("AreaTrigger_at_ragefire_sealed_gate_items") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            uint32 tempCondition = hasPlayerDelivering(player);

            if (!tempCondition)
                return false;

            if (Creature* ticker = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_TICKER) : ObjectGuid::Empty))
            {
                ticker->GetInstanceScript()->SetData(tempCondition, ticker->GetInstanceScript()->GetData(tempCondition) + 1);
                ticker->GetInstanceScript()->SendScenarioProgressUpdate(CriteriaProgressData(tempCondition, ticker->GetInstanceScript()->GetData(tempCondition), ticker->GetInstanceScript()->GetScenarioGUID(), time(NULL), 0, 0));

                // Random Yell
                ticker->AI()->Talk(urand(6, 10));

                // Check condition Available
                if (HasIngridientsCollected(player))
                    ticker->GetInstanceScript()->SetData(DATA_SEALED_GATE, DONE);
            }

            return true;
        }

    private:
        uint32 hasPlayerDelivering(Player* target)
        {
            for (auto&& itr : invDeliveringType)
            {
                if (target && target->HasAura(itr.first))
                {
                    target->RemoveAurasDueToSpell(itr.first);
                    return itr.second;
                }
            }

            return 0;
        }

        bool HasIngridientsCollected(Player* target)
        {
            if (target && target->GetInstanceScript() && target->GetInstanceScript()->GetData(CRITERIA_CANNON_BALLS) > 2 && target->GetInstanceScript()->GetData(CRITERIA_BATTERY) > 1 && target->GetInstanceScript()->GetData(CRITERIA_POOL_PONY) && target->GetInstanceScript()->GetData(CRITERIA_EGGS))
                return true;

            return false;
        }
};

void AddSC_secrets_of_ragefire()
{
    new creature_script<npc_ragefire_grit>("npc_ragefire_grit");
    new creature_script<npc_ragefire_detonator>("npc_ragefire_detonator");
    new creature_script<npc_ragefire_dark_shaman_xorenth>("npc_ragefire_dark_shaman_xorenth");
    new creature_script<npc_ragefire_ruined_earth>("npc_ragefire_ruined_earth");
    new creature_script<npc_ragefire_korkron_dark_shaman>("npc_ragefire_korkron_dark_shaman");
    new creature_script<npc_ragefire_korkron_emberguard>("npc_ragefire_korkron_emberguard");
    new creature_script<npc_ragefire_investigated_item>("npc_ragefire_investigated_item");
    new creature_script<npc_ragefire_korkron_shadowblade>("npc_ragefire_korkron_shadowblade");
    new creature_script<npc_ragefire_bring_items>("npc_ragefire_bring_items");
    new creature_script<npc_ragefire_overseer_elaglo>("npc_ragefire_overseer_elaglo");
    new creature_script<npc_ragefire_korkron_dire_solder>("npc_ragefire_korkron_dire_solder");
    new creature_script<npc_ragefire_poison_bolt_totem>("npc_ragefire_poison_bolt_totem");
    new creature_script<npc_ragefire_flame_hound>("npc_ragefire_flame_hound");
    new creature_script<npc_ragefire_shattered_earth>("npc_ragefire_shattered_earth");

    new spell_script<spell_ragefire_ruined_earth_selector>("spell_ragefire_ruined_earth_selector");
    new atrigger_script<sat_ragefire_emberstrikes>("sat_ragefire_emberstrikes");
    new AreaTrigger_at_ragefire_sealed_gate();
    new AreaTrigger_at_ragefire_sealed_gate_items();
}