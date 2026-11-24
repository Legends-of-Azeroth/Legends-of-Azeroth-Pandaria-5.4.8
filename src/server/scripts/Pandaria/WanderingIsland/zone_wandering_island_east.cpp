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

/* ScriptDataSDName: WanderingIslandEast
SD%Complete: 100
SDComment: Quest support: 29774, 29775
SDCategory: WanderingIsland
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "SpellScript.h"
#include "Vehicle.h"

// AT - Bassin Curse (Area 6991/6992/6993)
class AreaTrigger_at_bassin_curse : public AreaTriggerScript
{
    public:
        AreaTrigger_at_bassin_curse() : AreaTriggerScript("AreaTrigger_at_bassin_curse") { }

        enum eTriggers
        {
            AREA_CRANE              = 6991,
            AREA_SKUNK              = 6988,
            AREA_FROG               = 6987,
            AREA_FROG_EXIT          = 6986,
            AREA_TURTLE             = 7012,
            AREA_CROCODILE          = 6990
        };

        enum eSpells
        {
            SPELL_FROG              = 102938,
            SPELL_SKUNK             = 102939,
            SPELL_TURTLE            = 102940,
            SPELL_CRANE             = 102941,
            SPELL_CROCODILE         = 102942
        };

        void AddOrRemoveSpell(Player* player, uint32 spellId)
        {
            RemoveAllSpellsExcept(player, spellId);

            if (!player->HasAura(spellId))
            {
                if (!player->IsOnVehicle())
                    player->AddAura(spellId, player);
            }
            else
                player->RemoveAurasDueToSpell(spellId);
        }

        void RemoveAllSpellsExcept(Player* player, uint32 spellId)
        {
            uint32 spellTable[5] = {SPELL_FROG, SPELL_SKUNK, SPELL_TURTLE, SPELL_CRANE, SPELL_CROCODILE};

            for (uint8 i = 0; i < 5; ++i)
                if (spellId != spellTable[i])
                    player->RemoveAurasDueToSpell(spellTable[i]);
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            switch (trigger->ID)
            {
                case AREA_CRANE:     AddOrRemoveSpell(player, SPELL_CRANE);     break;
                case AREA_SKUNK:     AddOrRemoveSpell(player, SPELL_SKUNK);     break;
                case AREA_FROG:      AddOrRemoveSpell(player, SPELL_FROG);      break;
                case AREA_FROG_EXIT: RemoveAllSpellsExcept(player, 0);          break;
                case AREA_TURTLE:    AddOrRemoveSpell(player, SPELL_TURTLE);    break;
                case AREA_CROCODILE: AddOrRemoveSpell(player, SPELL_CROCODILE); break;
            }
            return true;
        }
};

// NPC 54993 - Balance Pole (Water Temple)
struct npc_tushui_monkAI : public ScriptedAI
{
    npc_tushui_monkAI(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        std::list<Creature*> poleList;
        GetCreatureListWithEntryInGrid(poleList, me, 54993, 25.0f);

        if (poleList.empty())
        {
            me->DespawnOrUnsummon(1000);
            return;
        }

        Trinity::Containers::RandomResizeList(poleList, 1);

        for (auto&& creature: poleList)
            me->EnterVehicle(creature);

        me->SetFaction(2357);
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->ExitVehicle();
        me->DespawnOrUnsummon(1000);
    }
};

// Rock Jump - 103069 / 103070 / 103077
class spell_rock_jump: public SpellScriptLoader
{
    public:
        spell_rock_jump() : SpellScriptLoader("spell_rock_jump") { }

        class spell_rock_jump_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rock_jump_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (caster->GetPositionZ() < 90.0f)
                        caster->GetMotionMaster()->MoveJump(1045.36f, 2848.47f, 91.38f, 10.0f, 10.0f);
                    else if (caster->GetPositionZ() < 92.0f)
                        caster->GetMotionMaster()->MoveJump(1054.42f, 2842.65f, 92.96f, 10.0f, 10.0f);
                    else if (caster->GetPositionZ() < 94.0f)
                        caster->GetMotionMaster()->MoveJump(1063.66f, 2843.49f, 95.50f, 10.0f, 10.0f);
                    else
                    {
                        caster->GetMotionMaster()->MoveJump(1078.42f, 2845.07f, 95.16f, 10.0f, 10.0f);

                        if (caster->ToPlayer())
                            caster->ToPlayer()->KilledMonsterCredit(57476);
                    }
                }
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_rock_jump_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_JUMP_DEST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rock_jump_SpellScript();
        }
};

Position rocksPos[4] =
{
    {1102.05f, 2882.11f, 94.32f, 0.11f},
    {1120.01f, 2883.20f, 96.44f, 4.17f},
    {1128.09f, 2859.44f, 97.64f, 2.51f},
    {1111.52f, 2849.84f, 94.84f, 1.94f}
};

// NPC 55558/60916 - Shu/Wugou (Water Spirits) | Quest 29774
struct npc_shu_water_spiritAI : public ScriptedAI
{
    npc_shu_water_spiritAI(Creature* creature) : ScriptedAI(creature) { }

    EventMap _events;
    uint8 actualPlace;
    ObjectGuid waterSpoutGUID;

    enum eShuSpells
    {
        SPELL_WATER_SPOUT_SUMMON    = 116810,
        SPELL_WATER_SPOUT_WARNING   = 116695,
        SPELL_WATER_SPOUT_EJECT     = 116696,
        SPELL_WATER_SPOUT_VISUAL    = 117057
    };

    enum eEvents
    {
        EVENT_CHANGE_PLACE          = 1,
        EVENT_SUMMON_WATER_SPOUT    = 2,
        EVENT_WATER_SPOUT_VISUAL    = 3,
        EVENT_WATER_SPOUT_EJECT     = 4,
        EVENT_WATER_SPOUT_DESPAWN   = 5
    };

    void Reset() override
    {
        _events.Reset();
        actualPlace = 0;
        waterSpoutGUID = ObjectGuid::Empty;

        _events.ScheduleEvent(EVENT_CHANGE_PLACE, 5000);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != EFFECT_MOTION_TYPE)
            return;

        if (pointId == 1)
        {
            me->RemoveAurasDueToSpell(SPELL_WATER_SPOUT_WARNING);
            if (Player* player = me->SelectNearestPlayerNotGM(50.0f))
            {
                me->SetOrientation(me->GetAngle(player));
                me->SetFacingToObject(player);
                _events.ScheduleEvent(EVENT_SUMMON_WATER_SPOUT, 2000);
            }
            else
                _events.ScheduleEvent(EVENT_CHANGE_PLACE, 5000);
        }
    }

    Creature* getWaterSpout()
    {
        return me->GetMap()->GetCreature(waterSpoutGUID);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        switch (_events.ExecuteEvent())
        {
            case EVENT_CHANGE_PLACE:
            {
                uint8 newPlace = 0;

                do
                {
                    newPlace = urand(0, 3);
                }
                while (newPlace == actualPlace);

                me->GetMotionMaster()->MoveJump(rocksPos[newPlace].GetPositionX(), rocksPos[newPlace].GetPositionY(), rocksPos[newPlace].GetPositionZ(), 10.0f, 10.0f, 1);
                me->AddAura(SPELL_WATER_SPOUT_WARNING, me); // Just visual
                actualPlace = newPlace;
                break;
            }
            case EVENT_SUMMON_WATER_SPOUT:
            {
                float x = 0.0f, y = 0.0f;
                GetPositionWithDistInOrientation(me, 5.0f, me->GetOrientation() + frand(-M_PI, M_PI), x, y);
                waterSpoutGUID = ObjectGuid::Empty;

                if (Creature* waterSpout = me->SummonCreature(60488, x, y, 92.189629f))
                    waterSpoutGUID = waterSpout->GetGUID();

                _events.ScheduleEvent(EVENT_WATER_SPOUT_VISUAL, 500);
                _events.ScheduleEvent(EVENT_WATER_SPOUT_EJECT, 7500);
                break;
            }
            case EVENT_WATER_SPOUT_VISUAL:
            {
                if (Creature* waterSpout = getWaterSpout())
                    waterSpout->CastSpell(waterSpout, SPELL_WATER_SPOUT_WARNING, true);
                break;
            }
            case EVENT_WATER_SPOUT_EJECT:
            {
                if (Creature* waterSpout = getWaterSpout())
                {
                    std::list<Player*> playerList;
                    GetPlayerListInGrid(playerList, waterSpout, 1.0f);

                    for (auto&& player: playerList)
                        player->CastSpell(player, SPELL_WATER_SPOUT_EJECT, true);

                    waterSpout->CastSpell(waterSpout, SPELL_WATER_SPOUT_VISUAL, true);
                }
                _events.ScheduleEvent(EVENT_WATER_SPOUT_DESPAWN, 3000);
                break;
            }
            case EVENT_WATER_SPOUT_DESPAWN:
            {
                if (Creature* waterSpout = getWaterSpout())
                    waterSpout->DespawnOrUnsummon();

                waterSpoutGUID = ObjectGuid::Empty;

                _events.ScheduleEvent(EVENT_CHANGE_PLACE, 2000);
                break;
            }
        }
    }
};

// Shu Benediction - 103245, 105889, 105890
class spell_shu_benediction : public AuraScript
{
    PrepareAuraScript(spell_shu_benediction);

    uint32 SelectCreature(uint32 spellId)
    {
        switch (spellId)
        {
            case 103245: return 55213;
            case 105889: return 60916;
            case 105890: return 55558;
        }
        return 0;
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        if (!target)
            return;

        uint32 npcEntry = SelectCreature(GetSpellInfo()->Id);
        if (!npcEntry)
            return;

        std::list<Creature*> npcList;
        GetCreatureListWithEntryInGrid(npcList, target, npcEntry, 20.0f);
        for (auto&& shu: npcList)
            if (shu->ToTempSummon())
                if (shu->ToTempSummon()->GetOwnerGUID() == target->GetGUID())
                    return;

        // From here we know that the player does not have Huo yet
        if (TempSummon* tempShu = target->SummonCreature(npcEntry, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0ms))
        {
            tempShu->SetExplicitSeerGuid(target->GetGUID());
            tempShu->SetOwnerGUID(target->GetGUID());
            tempShu->GetMotionMaster()->MoveFollow(target, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        }
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        if (!target)
            return;

        uint32 npcEntry = SelectCreature(GetSpellInfo()->Id);
        if (!npcEntry)
            return;

        std::list<Creature*> npcList;
        GetCreatureListWithEntryInGrid(npcList, target, npcEntry, 20.0f);
        for (auto&& shu: npcList)
            if (shu->ToTempSummon())
                if (shu->ToTempSummon()->GetOwnerGUID() == target->GetGUID())
                    shu->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnEffectApply  += AuraEffectApplyFn (spell_shu_benediction::OnApply,  EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectRemove += AuraEffectRemoveFn(spell_shu_benediction::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// Grab Carriage - 115904
class spell_grab_carriage: public SpellScriptLoader
{
    public:
        spell_grab_carriage() : SpellScriptLoader("spell_grab_carriage") { }

        class spell_grab_carriage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_grab_carriage_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();

                if (!caster)
                    return;

                Creature* carriage = NULL;
                Creature* yak      = NULL;

                if (caster->GetAreaId() == 5826) // Bassins chantants
                {
                    carriage = caster->SummonCreature(57208, 979.06f, 2863.87f, 87.88f, 4.7822f, TEMPSUMMON_MANUAL_DESPAWN, 0ms);
                    yak      = caster->SummonCreature(57207, 979.37f, 2860.29f, 88.22f, 4.4759f, TEMPSUMMON_MANUAL_DESPAWN, 0ms);
                }
                else if (caster->GetAreaId() == 5881) // Ferme Dai-Lo
                {
                    carriage = caster->SummonCreature(57208, 588.70f, 3165.63f, 88.86f, 4.4156f, TEMPSUMMON_MANUAL_DESPAWN, 0ms);
                    yak      = caster->SummonCreature(59499, 587.61f, 3161.91f, 89.31f, 4.3633f, TEMPSUMMON_MANUAL_DESPAWN, 0ms);
                }
                else if (caster->GetAreaId() == 5833) // Epave du Chercheciel
                {
                    carriage = caster->SummonCreature(57208, 264.37f, 3867.60f, 73.56f, 0.9948f, TEMPSUMMON_MANUAL_DESPAWN, 0ms);
                    yak      = caster->SummonCreature(57743, 268.38f, 3872.36f, 74.50f, 0.8245f, TEMPSUMMON_MANUAL_DESPAWN, 0ms);
                }

                if (!carriage || !yak)
                    return;

                carriage->SetExplicitSeerGuid(caster->GetGUID());
                yak->SetExplicitSeerGuid(caster->GetGUID());

                //carriage->CastSpell(yak, 108627, true);
                carriage->GetMotionMaster()->MoveFollow(yak, 0.0f, M_PI);
                caster->EnterVehicle(carriage, 0);
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_grab_carriage_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_grab_carriage_SpellScript();
        }
};

// NPC 57208/59496/57740 - Delivery Cart (Carriage Vehicle)
struct npc_nourished_yakAI : public EscortAI
{
    npc_nourished_yakAI(Creature* creature) : EscortAI(creature) { }

    uint32 IntroTimer;
    uint8 waypointToEject;

    void Reset() override
    {
        waypointToEject = 100;

        if (me->IsSummon())
        {
            IntroTimer = 2500;

            // The Singing Pools -> Dai-Lo Farmstead
            if (me->GetAreaId() == 5826)
                waypointToEject = 24;
            // Dai-Lo Farmstead -> Temple
            else if (me->GetAreaId() == 5881) // Ferme Dai-Lo
                waypointToEject = 22;
            // Epave -> Temple
            else if (me->GetAreaId() == 5833) // New Allies quest Carraige
                waypointToEject = 18;
        }
        else
            IntroTimer = 0;
    }

    void WaypointReached(uint32 waypointId, uint32 pathId) override
    {
        if (waypointId == waypointToEject)
        {
            if (Creature* vehicle = GetClosestCreatureWithEntry(me, 57208, 50.0f))
                vehicle->DespawnOrUnsummon();
            if (Creature* vehicle = GetClosestCreatureWithEntry(me, 59496, 50.0f))
                vehicle->DespawnOrUnsummon();
            if (Creature* vehicle = GetClosestCreatureWithEntry(me, 57740, 50.0f))
                vehicle->DespawnOrUnsummon();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (IntroTimer)
        {
            if (IntroTimer <= diff)
            {
                SetRun(true); Start(false);
                IntroTimer = 0;
            }
            else
                IntroTimer -= diff;
        }

        EscortAI::UpdateAI(diff);
    }
};

// NPC 55558 - Shu (Water Spirit Dailo) | Quest 29774
struct npc_water_spirit_dailoAI : public ScriptedAI
{
    npc_water_spirit_dailoAI(Creature* creature) : ScriptedAI(creature) { }

    uint64 playerGuid = 0;
    uint32 eventTimer = 0;
    uint8 eventProgress = 0;

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        if (player->GetQuestStatus(29774) == QUEST_STATUS_INCOMPLETE)
            AddGossipItemFor(player, 13727, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        SendGossipMenuFor(player, 13727, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*sender*/, uint32 action) override
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            CloseGossipMenuFor(player);
            player->KilledMonsterCredit(55548, ObjectGuid::Empty);
            player->RemoveAurasDueToSpell(59073); // Remove Phase 2, first water spirit disappear

            if (Creature* shu = player->SummonCreature(55558, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0ms, player->GetGUID()))
            {
                if (shu->AI())
                {
                    shu->AI()->DoAction(0);
                    shu->AI()->SetGUID(player->GetGUID());
                }
            }
        }
        return true;
    }

    void Reset() override
    {
        eventTimer = 0;
        eventProgress = 0;
        playerGuid = 0;
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == 0)
            eventTimer = 2500;
    }

    void SetGUID(ObjectGuid guid, int32 /*id*/ = 0) override
    {
        playerGuid = guid;
    }

    void MovementInform(uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE)
            return;

        switch (pointId)
        {
            case 1:
                eventTimer = 250;
                ++eventProgress;
                break;
            case 2:
                eventTimer = 250;
                ++eventProgress;
                break;
            case 3:
                if (Creature* wugou = GetClosestCreatureWithEntry(me, 60916, 20.0f))
                    me->SetFacingToObject(wugou);
                me->SetEmoteState(EMOTE_STATE_READY_UNARMED);
                eventTimer = 2000;
                ++eventProgress;
                break;
            default:
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (eventTimer)
        {
            if (eventTimer <= diff)
            {
                switch (eventProgress)
                {
                    case 0:
                        me->GetMotionMaster()->MovePoint(1, 650.30f, 3127.16f, 89.62f);
                        eventTimer = 0;
                        break;
                    case 1:
                        me->GetMotionMaster()->MovePoint(2, 625.25f, 3127.88f, 87.95f);
                        eventTimer = 0;
                        break;
                    case 2:
                        me->GetMotionMaster()->MovePoint(3, 624.44f, 3142.94f, 87.75f);
                        eventTimer = 0;
                        break;
                    case 3:
                        if (Creature* wugou = GetClosestCreatureWithEntry(me, 60916, 20.0f))
                            wugou->CastSpell(wugou, 118027, false);
                        me->SetEmoteState(EMOTE_ONESHOT_NONE);
                        eventTimer = 3000;
                        ++eventProgress;
                        break;
                    case 4:
                        eventTimer = 0;
                        if (Player* owner = ObjectAccessor::FindPlayer(ObjectGuid(playerGuid)))
                        {
                            owner->KilledMonsterCredit(55547, ObjectGuid::Empty);
                            owner->RemoveAurasDueToSpell(59074); // Remove phase 4, asleep wugou disappear

                            if (Creature* wugou = GetClosestCreatureWithEntry(me, 60916, 20.0f))
                                if (Creature* newWugou = owner->SummonCreature(60916, wugou->GetPositionX(), wugou->GetPositionY(), wugou->GetPositionZ(), wugou->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0ms, owner->GetGUID()))
                                    newWugou->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);

                            me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, -PET_FOLLOW_ANGLE);
                        }
                        break;
                    default:
                        break;
                }
            }
            else
                eventTimer -= diff;
        }
    }
};

// AT - Middle Temple from East
class AreaTrigger_at_middle_temple_from_east : public AreaTriggerScript
{
public:
    AreaTrigger_at_middle_temple_from_east() : AreaTriggerScript("AreaTrigger_at_middle_temple_from_east") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
    {
        if (Creature* shu = player->FindNearestCreature(55558, 25.0f))
            shu->DespawnOrUnsummon();

        if (Creature* wugou = player->FindNearestCreature(60916, 25.0f))
            wugou->DespawnOrUnsummon();

        return true;
    }
};

// NPC - Delivery Cart Tender | Quest 29775
struct npc_delivery_cart_tenderAI : public ScriptedAI
{
    npc_delivery_cart_tenderAI(Creature* creature) : ScriptedAI(creature) { }

    bool hasSaidIntro = false;
    bool hasSaidIntro2 = false;

    void UpdateAI(uint32 /*diff*/) override
    {
        std::list<Player*> playerList;
        GetPlayerListInGrid(playerList, me, 15.0f);

        for (auto&& player : playerList)
        {
            if (me->GetPositionX() == 974.718994f && me->GetPositionY() == 2863.25f)
            {
                if (!hasSaidIntro)
                {
                    Talk(0);
                    hasSaidIntro = true;
                }
            }
            else if (player->GetQuestStatus(29775) == QUEST_STATUS_COMPLETE)
            {
                if (!hasSaidIntro2)
                {
                    Talk(1);
                    hasSaidIntro2 = true;
                }
            }
        }
    }
};

void AddSC_wandering_island_east()
{
    new AreaTrigger_at_bassin_curse();
    RegisterCreatureAI(npc_tushui_monkAI);
    new spell_rock_jump();
    RegisterCreatureAI(npc_shu_water_spiritAI);
    new aura_script<spell_shu_benediction>("spell_shu_benediction");
    new spell_grab_carriage();
    RegisterCreatureAI(npc_nourished_yakAI);
    RegisterCreatureAI(npc_water_spirit_dailoAI);
    new AreaTrigger_at_middle_temple_from_east();
    RegisterCreatureAI(npc_delivery_cart_tenderAI);
}
