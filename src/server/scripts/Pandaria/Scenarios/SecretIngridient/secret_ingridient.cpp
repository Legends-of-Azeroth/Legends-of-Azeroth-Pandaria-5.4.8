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

#include "secret_ingridient.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "LFGMgr.h"
#include "Random.h"

// Sungshin Ironpaw 73316
struct npc_secret_ing_sungshin_iron_paw : public ScriptedAI
{
    npc_secret_ing_sungshin_iron_paw(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    uint32 delay;
    ObjectGuid selectedScholarGUID;
    bool isTraining;
    bool firstSeat;
    bool firstTable;
    bool firstNoodle;
    bool firstDelivery;
    bool firstEaten;
    std::map <uint32, uint32> waitingQueue;

    void Reset() override 
    {
        isTraining = true;
        firstSeat = true;
        firstTable = true;
        firstNoodle = true;
        firstDelivery = true;
        firstEaten = true;
        selectedScholarGUID = ObjectGuid::Empty;
        waitingQueue.clear();

        for (uint32 i = 1; i < 6; i++)
            waitingQueue.insert(std::pair<uint32, uint32>(i, 0));
    }

    uint32 GetData(uint32 type) const override
    {
        switch (type)
        {
            case TYPE_CLICK_ON_TRAINING:
                return isTraining ? 1 : 0;
            case TYPE_QUEUE_PLACE:
                for (auto&& itr : waitingQueue)
                    if (!itr.second)
                        return itr.first;
                break;
        }

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        switch (type)
        {
            case TYPE_QUEUE_PLACE:
                waitingQueue.find(data)->second = 0;
                break;
            case TYPE_HOLD_QUEUE:
                waitingQueue.find(data)->second = 1;
                break;
        }
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        selectedScholarGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return selectedScholarGUID;
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_INTRO:
                delay = 5000;

                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    if (Player* target = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_PLAYER) : ObjectGuid::Empty))
                        Talk(TALK_INTRO, target);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 8500), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_1);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 3000), [this](TaskContext context)
                {
                    // Summon Scholar
                    if (Creature* sholar = me->SummonCreature(NPC_SHOLAR, scholarPath[0], TEMPSUMMON_MANUAL_DESPAWN))
                        sholar->AI()->DoAction(ACTION_INTRO);
                });
                break;
            case ACTION_SPECIAL_1:
                delay = 2500;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_2);
                });
                break;
            case ACTION_SELECT_SEAT_ON_TRAINING:
                if (!firstSeat)
                    break;

                firstSeat = false;

                Talk(TALK_SPECIAL_3);

                // Spawn seats for scenario
                for (auto&& itr : seatPlaces)
                    me->CastSpell(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ(), SPELL_SPAWN_SEAT, true);
                break;
            case ACTION_CUSTOMER_AT_TABLE_TRAINING:
                if (!firstTable)
                    break;

                firstTable = false;

                Talk(TALK_SPECIAL_4);

                scheduler
                    .Schedule(Milliseconds(8500), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_5);

                    // Add button for coocking (not available to make it, spells initialize for vehicle at start, so just prevent use it until this phase)
                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SetData(DATA_AVAILABLE_COOK_NOODLE, 1);
                });
                break;
            case ACTION_FIRST_NOODLE_IS_READY:
                if (!firstNoodle)
                    break;

                firstNoodle = false;

                Talk(TALK_SPECIAL_6);
                break;
            case ACTION_EAT_SENT:
                if (!firstDelivery)
                    break;

                firstDelivery = false;

                Talk(TALK_SPECIAL_7);
                break;
            case ACTION_FIRST_NOODLE_EATEN:
                if (!firstEaten)
                    break;

                firstEaten = false;
                Talk(TALK_SPECIAL_8);

                delay = 6000;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_9);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 8400), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_10);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 8500), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_11);
                    isTraining = false;

                    // Queue is started!
                    if (me->GetInstanceScript())
                    {
                        me->GetInstanceScript()->SetData(DATA_AVAILABLE_COOK_WHOLE, 1);
                        me->GetInstanceScript()->DoUpdateWorldState(WORLDSTATE_CLIENTS, 0);
                    }

                    scheduler
                        .Schedule(Milliseconds(4000), [this](TaskContext context)
                    {
                        if (GetData(TYPE_QUEUE_PLACE)) // check queue not full
                            if (Creature* sholar = me->SummonCreature(NPC_SHOLAR, scholarPath[0], TEMPSUMMON_MANUAL_DESPAWN))
                                sholar->AI()->DoAction(ACTION_INTRO);

                        context.Repeat(Seconds(10));
                    });
                });
                break;
        }
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Noodle Stand 72952
struct npc_secret_ing_noodle_stand : public ScriptedAI
{
    npc_secret_ing_noodle_stand(Creature* creature) : ScriptedAI(creature) 
    { 
        hasUse = false;
    }

    TaskScheduler scheduler;
    ObjectGuid summonerGUID;
    bool hasUse;

    void IsSummonedBy(Unit* summoner) override
    {
        me->NearTeleportTo(noodleSpawnPos.GetPositionX(), noodleSpawnPos.GetPositionY(), noodleSpawnPos.GetPositionZ(), me->GetOrientation());
        summonerGUID = summoner->GetGUID();
    }

    void Reset() override 
    {
        if (hasUse) // in case if player leave vehicle
            return;

        hasUse = true;

        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            if (Player* target = ObjectAccessor::GetPlayer(*me, summonerGUID))
                target->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Scholar 72916
struct npc_secret_ing_scholar : public ScriptedAI
{
    npc_secret_ing_scholar(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid tableGUID;
    uint32 ourPositionInQueue;
    uint32 leaveNotEaten;
    bool hasInLeaveProgress;

    void Reset() override
    {
        tableGUID = ObjectGuid::Empty;
        hasInLeaveProgress = false;
        ourPositionInQueue = 0;
        leaveNotEaten = 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_EATEN_OR_NOT)
            leaveNotEaten = data;
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (me->HasAura(SPELL_WAITING_FOR_FOOD))
        {
            if (clicker->GetVehicleKit())
            {
                if (Unit* passenger = clicker->GetVehicleKit()->GetPassenger(0))
                {
                    passenger->ExitVehicle();
                    passenger->SetVisible(false);
                    
                    if (passenger->ToCreature())
                        passenger->ToCreature()->DespawnOrUnsummon(1000);

                    uint32 deliverEntry = invSoupVisualType.find(passenger->GetEntry())->second[1];
                    clicker->CastSpell(me, deliverEntry, true);
                }
            }

            return;
        }

        if (Creature* ironPaw = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SUNGSHIN_IRONPAW) : ObjectGuid::Empty))
        {
            if (!me->HasAura(SPELL_TIMER_VISUAL_40) && !ironPaw->AI()->GetData(TYPE_CLICK_ON_TRAINING)) // prevent if time is gone (for training this aura not used)
                return;

            clicker->CastSpell(me, SPELL_NOT_IN_LINE, true);
            ironPaw->AI()->SetGUID(me->GetGUID());

            if (ironPaw->AI()->GetData(TYPE_CLICK_ON_TRAINING))
                ironPaw->AI()->DoAction(ACTION_SELECT_SEAT_ON_TRAINING);
        }
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        tableGUID = guid;
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_INTRO:
            {
                if (Creature* ironPaw = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SUNGSHIN_IRONPAW) : ObjectGuid::Empty))
                {
                    Movement::MoveSplineInit pInit(me);

                    if (ironPaw->AI()->GetData(TYPE_CLICK_ON_TRAINING))
                    {
                        for (auto&& itr : scholarPath)
                            pInit.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));
                    }
                    else
                    {
                        pInit.Path().push_back(G3D::Vector3(scholarPath[0].GetPositionX(), scholarPath[0].GetPositionY(), scholarPath[0].GetPositionZ()));
                        pInit.Path().push_back(G3D::Vector3(scholarPath[1].GetPositionX(), scholarPath[1].GetPositionY(), scholarPath[1].GetPositionZ()));

                        // Calculate queue position for each
                        ourPositionInQueue = ironPaw->AI()->GetData(TYPE_QUEUE_PLACE) - 1;
                        ironPaw->AI()->SetData(TYPE_HOLD_QUEUE, ourPositionInQueue + 1);

                        if (Player* target = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_PLAYER) : ObjectGuid::Empty))
                        {
                            float x = scholarPath[2].GetPositionX() + (2.5f * ourPositionInQueue * cos(Position::NormalizeOrientation(target->GetOrientation() + M_PI + M_PI / 2)));
                            float y = scholarPath[2].GetPositionY() + (2.5f * ourPositionInQueue * sin(Position::NormalizeOrientation(target->GetOrientation() + M_PI + M_PI / 2)));
                            pInit.Path().push_back(G3D::Vector3(x, y, scholarPath[2].GetPositionZ()));
                        }
                    }

                    pInit.Launch();

                    scheduler
                        .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                    {
                        if (Player* target = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_PLAYER) : ObjectGuid::Empty))
                            me->SetFacingToObject(target);

                        DoCast(me, SPELL_WAITING_IN_LINE);
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                        if (Creature* ironPawMisc = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SUNGSHIN_IRONPAW) : ObjectGuid::Empty))
                        {
                            if (ironPawMisc->AI()->GetData(TYPE_CLICK_ON_TRAINING))
                            {
                                ironPawMisc->AI()->DoAction(ACTION_SPECIAL_1);
                                ironPawMisc->CastSpell(ironPawMisc, SPELL_CLICK_ME, true);
                            }
                            else
                            {
                                DoCast(me, SPELL_TIMER_VISUAL_40);

                                if (roll_chance_i(30)) // random text with random chance
                                    Talk(urand(0, 1));
                            }
                        }
                    });
                }
                break;
            }
            case ACTION_MOVE_TO_TABLE:
                me->StopMoving();

                if (Creature* selectedTable = ObjectAccessor::GetCreature(*me, tableGUID))
                {
                    me->RemoveAurasDueToSpell(SPELL_TIMER_VISUAL_40);
                    me->GetMotionMaster()->MovePoint(0, *selectedTable);

                    scheduler
                        .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                    {
                        if (Creature* myTable = ObjectAccessor::GetCreature(*me, tableGUID))
                        {
                            DoCast(me, SPELL_WAITING_FOR_FOOD);
                            DoCast(myTable, VEHICLE_SPELL_RIDE_HARDCODED);
                            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                            if (Player* coocker = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_PLAYER) : ObjectGuid::Empty))
                                me->SetFacingTo(me->GetAngle(coocker));

                            if (Creature* ironPaw = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SUNGSHIN_IRONPAW) : ObjectGuid::Empty))
                                if (ironPaw->AI()->GetData(TYPE_CLICK_ON_TRAINING))
                                    ironPaw->AI()->DoAction(ACTION_CUSTOMER_AT_TABLE_TRAINING);
                        }
                    });
                }
                break;
            case ACTION_SCHOLAR_LEAVE:
            {
                if (hasInLeaveProgress)
                    break;

                hasInLeaveProgress = true;

                if (Creature* ironPaw = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SUNGSHIN_IRONPAW) : ObjectGuid::Empty))
                    ironPaw->AI()->SetData(TYPE_QUEUE_PLACE, ourPositionInQueue + 1);

                if (Creature* table = ObjectAccessor::GetCreature(*me, tableGUID))
                    table->AI()->Reset();

                me->ExitVehicle();
                me->StopMoving();

                Movement::MoveSplineInit pInit(me);

                for (auto&& itr : cholarLeavePath)
                    pInit.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                pInit.Launch();
                me->DespawnOrUnsummon(me->GetSplineDuration());

                if (leaveNotEaten)
                    break;

                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SetData(DATA_EATEN_COUNT, me->GetInstanceScript()->GetData(DATA_EATEN_COUNT) + 1);
                break;
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Seat 72928
struct npc_secret_ing_seat : public ScriptedAI
{
    npc_secret_ing_seat(Creature* creature) : ScriptedAI(creature) 
    { 
        hasRotate = false;
    }

    TaskScheduler scheduler;
    bool hasRotate;

    void Reset() override
    {
        DoCast(me, SPELL_SEAT_AVAILABLE);

        scheduler
            .Schedule(Milliseconds(3000), [this](TaskContext context)
        {
            // Force to player
            if (!hasRotate)
            {
                hasRotate = true;
                me->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() - M_PI / 2));
            }

            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        });
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (Creature* ironPaw = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SUNGSHIN_IRONPAW) : ObjectGuid::Empty))
        {
            if (!ironPaw->AI()->GetGUID())
                return;

            clicker->CastSpell(me, SPELL_SELECT_SEAT, true);

            if (Creature* selectedScholar = ObjectAccessor::GetCreature(*me, ironPaw->AI()->GetGUID()))
            {
                me->RemoveAurasDueToSpell(SPELL_SEAT_AVAILABLE);
                selectedScholar->RemoveAurasDueToSpell(SPELL_WAITING_IN_LINE);
                selectedScholar->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                ironPaw->AI()->SetGUID(ObjectGuid::Empty);
                selectedScholar->AI()->SetGUID(me->GetGUID());
                selectedScholar->AI()->DoAction(ACTION_MOVE_TO_TABLE);
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Noodle Soup 72961, 72962, 72963
struct npc_secret_ing_noodle_soup : public ScriptedAI
{
    npc_secret_ing_noodle_soup(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        uint32 visualSoup = invSoupVisualType.find(me->GetEntry())->second[0];

        if (Creature* ironPaw = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SUNGSHIN_IRONPAW) : ObjectGuid::Empty))
            if (ironPaw->AI()->GetData(TYPE_CLICK_ON_TRAINING))
                visualSoup = SPELL_NOODLE_SOUP_FIRST; // no timer for first

        DoCast(me, visualSoup);
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

        if (Creature* noodle = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NOODLE_STAND) : ObjectGuid::Empty))
            DoCast(noodle, VEHICLE_SPELL_RIDE_HARDCODED);
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (!clicker->HasAura(SPELL_VEHICLE_SET_UP))
            clicker->CastSpell(clicker, SPELL_VEHICLE_SET_UP, true);

        if (Vehicle* kit = clicker->GetVehicleKit()) // Prevent multi-handind noodle for player
            if (kit->GetPassenger(0))
                return;

        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        DoCast(clicker, VEHICLE_SPELL_RIDE_HARDCODED, true);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Sungshin Ironpaw 64231
class npc_secret_ing_sungshin_ironpaw_queue : public CreatureScript
{
    public:
        npc_secret_ing_sungshin_ironpaw_queue() : CreatureScript("npc_secret_ing_sungshin_ironpaw_queue") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 1:
                {
                    player->KilledMonsterCredit(NPC_QUEUE_CREDIT);
                    lfg::LfgDungeonSet scenario = { 745 };
                    sLFGMgr->JoinLfg(player, lfg::PLAYER_ROLE_DAMAGE, scenario, "");
                    break;
                }
                case GOSSIP_ACTION_INFO_DEF + 2:
                    player->GetSession()->SendTrainerList(creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 3:
                    player->GetSession()->SendListInventory(creature->GetGUID());
                    break;
            }

            player->CLOSE_GOSSIP_MENU();

            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

            if (player->GetQuestStatus(QUEST_SECRET_INGRIDIENT_IS) == QUEST_STATUS_INCOMPLETE && !player->GetGroup() && !player->IsUsingLfg())
                player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
};

// Make Noodle Soup 146421
class spell_secret_ing_make_noodle_soup : public SpellScript
{
    PrepareSpellScript(spell_secret_ing_make_noodle_soup);

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
        {
            // Summon Noodle by hands
            caster->SummonCreature(GetSpellInfo()->Effects[EFFECT_0].BasePoints, *caster, TEMPSUMMON_MANUAL_DESPAWN);

            if (Creature* ironPaw = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_SUNGSHIN_IRONPAW) : ObjectGuid::Empty))
                if (ironPaw->AI()->GetData(TYPE_CLICK_ON_TRAINING))
                    ironPaw->AI()->DoAction(ACTION_FIRST_NOODLE_IS_READY);
        }
    }

    SpellCastResult CheckCast()
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_AVAILABLE_COOK_NOODLE) && !hasSoMuchNoodleOnTable())
            return SPELL_CAST_OK;

        return SPELL_FAILED_NOT_READY;
    }

    private:
        // Check if we have more 4 noodle on table - prevent it, cuz it`ll break us from vehicle
        bool hasSoMuchNoodleOnTable()
        {
            if (Creature* noodleStand = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(NPC_NOODLE_STAND) : ObjectGuid::Empty))
            {
                if (Vehicle * kit = noodleStand->GetVehicleKit())
                {
                    for (uint8 i = 1; i < 5; i++)
                    {
                        if (kit->GetPassenger(i))
                            continue;

                        return false;
                    }
                }
            }

            return true;
        }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_secret_ing_make_noodle_soup::HandleAfterCast);
        OnCheckCast += SpellCheckCastFn(spell_secret_ing_make_noodle_soup::CheckCast);
    }
};

// Eating Noodle Soup 146472, 146473, 146474
class spell_secret_ing_eating_noodle_soup : public SpellScript
{
    PrepareSpellScript(spell_secret_ing_eating_noodle_soup);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Creature* target = GetHitCreature())
            target->RemoveAurasDueToSpell(SPELL_WAITING_FOR_FOOD);

        if (Creature* ironPaw = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(NPC_SUNGSHIN_IRONPAW) : ObjectGuid::Empty))
            if (ironPaw->AI()->GetData(TYPE_CLICK_ON_TRAINING))
                ironPaw->AI()->DoAction(ACTION_EAT_SENT);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_secret_ing_eating_noodle_soup::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Eating Noodle Soup 146472
class spell_secret_ing_eating_noodle_soup_aura : public AuraScript
{
    PrepareAuraScript(spell_secret_ing_eating_noodle_soup_aura);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            // Scholar Should Leave
            owner->AI()->DoAction(ACTION_SCHOLAR_LEAVE);

            if (Creature* ironPaw = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(NPC_SUNGSHIN_IRONPAW) : ObjectGuid::Empty))
                if (ironPaw->AI()->GetData(TYPE_CLICK_ON_TRAINING))
                    ironPaw->AI()->DoAction(ACTION_FIRST_NOODLE_EATEN);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_secret_ing_eating_noodle_soup_aura::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Eating Great Noodle Soup 146473, 146474
class spell_secret_ing_eating_great_noodle_soup_aura : public AuraScript
{
    PrepareAuraScript(spell_secret_ing_eating_great_noodle_soup_aura);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_SCHOLAR_LEAVE);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_secret_ing_eating_great_noodle_soup_aura::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Make Great Noodle Soups 146422, 146423
class spell_secret_ing_make_great_noodle_soup : public SpellScript
{
    PrepareSpellScript(spell_secret_ing_make_great_noodle_soup);

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
            caster->SummonCreature(GetSpellInfo()->Effects[EFFECT_0].BasePoints, *caster, TEMPSUMMON_MANUAL_DESPAWN);
    }

    SpellCastResult CheckCast()
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_AVAILABLE_COOK_WHOLE) && !hasSoMuchNoodleOnTable())
            return SPELL_CAST_OK;

        return SPELL_FAILED_NOT_READY;
    }

    private:
        // Check if we have more 4 noodle on table - prevent it, cuz it`ll break us from vehicle
        bool hasSoMuchNoodleOnTable()
        {
            if (Creature* noodleStand = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(NPC_NOODLE_STAND) : ObjectGuid::Empty))
            {
                if (Vehicle * kit = noodleStand->GetVehicleKit())
                {
                    for (uint8 i = 1; i < 5; i++)
                    {
                        if (kit->GetPassenger(i))
                            continue;

                        return false;
                    }
                }
            }

            return true;
        }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_secret_ing_make_great_noodle_soup::HandleAfterCast);
        OnCheckCast += SpellCheckCastFn(spell_secret_ing_make_great_noodle_soup::CheckCast);
    }
};

// Soup Timer 30 147621
class spell_secret_ing_soup_timer : public AuraScript
{
    PrepareAuraScript(spell_secret_ing_soup_timer);

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (aurEff->GetBase()->GetDuration() > 0)
            return;

        if (Creature* owner = GetOwner()->ToCreature())
            owner->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_secret_ing_soup_timer::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Waiting for Food 146448, Waiting in Line 146506, Timer Visual 40
class spell_secret_ing_waiting_for_food : public AuraScript
{
    PrepareAuraScript(spell_secret_ing_waiting_for_food);

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (aurEff->GetBase()->GetDuration() > 0)
            return;

        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->AI()->SetData(TYPE_EATEN_OR_NOT, 1);
            owner->AI()->DoAction(ACTION_SCHOLAR_LEAVE);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_secret_ing_waiting_for_food::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_secret_ingridient()
{
    new creature_script<npc_secret_ing_sungshin_iron_paw>("npc_secret_ing_sungshin_iron_paw");
    new creature_script<npc_secret_ing_noodle_stand>("npc_secret_ing_noodle_stand");
    new creature_script<npc_secret_ing_scholar>("npc_secret_ing_scholar");
    new creature_script<npc_secret_ing_seat>("npc_secret_ing_seat");
    new creature_script<npc_secret_ing_noodle_soup>("npc_secret_ing_noodle_soup");
    new npc_secret_ing_sungshin_ironpaw_queue();

    new spell_script<spell_secret_ing_make_noodle_soup>("spell_secret_ing_make_noodle_soup");
    new spell_script<spell_secret_ing_eating_noodle_soup>("spell_secret_ing_eating_noodle_soup");
    new aura_script<spell_secret_ing_eating_noodle_soup_aura>("spell_secret_ing_eating_noodle_soup_aura");
    new aura_script<spell_secret_ing_eating_great_noodle_soup_aura>("spell_secret_ing_eating_great_noodle_soup_aura");
    new spell_script<spell_secret_ing_make_great_noodle_soup>("spell_secret_ing_make_great_noodle_soup");
    new aura_script<spell_secret_ing_soup_timer>("spell_secret_ing_soup_timer");
    new aura_script<spell_secret_ing_waiting_for_food>("spell_secret_ing_waiting_for_food");
}