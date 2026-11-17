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

#include "GameObjectAI.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "GridNotifiers.h"
#include "brewing_storm.h"

enum Spells
{
    SPELL_BLANSHES_BOOMER_BREW_POWER_BAR   = 112038,
    SPELL_LIGHTNING_IMPACT                 = 111544, // triggered by lightning channel
    SPELL_ON_FIRE                          = 111995, // impale kegs
    SPELL_BREW                             = 112040, // cosmetic brew bubbles...
    SPELL_LIGHTNING_CHANNEL                = 111828,
    SPELL_EXTINGUISHING                    = 111961,
    SPELL_BLANCHES_ELIXIR_OF_REPLENISHMENT = 114663,
    SPELL_HONORARY_BREWMASTER_KEG          = 136288,
    SPELL_BOOMER_BREW_STRIKE_BUTTON        = 115101, // at 3td chapter
    SPELL_BOOMER_BREW_STRIKE               = 115058,
    SPELL_VILETONGUE_STING                 = 127280,
    SPELL_LEAPING_CLEAVE                   = 132247,
    SPELL_TORCH_TOSS                       = 141771,
    SPELL_WIND_SLASH                       = 131594,
    SPELL_WIND_SLASH_AUR                   = 131596,
    SPELL_SWAMP_SMASH                      = 115013,
    SPELL_EARTH_SHATTERING                 = 122142,
    SPELL_BLANCHES_ELEXIR_EFF              = 121951,
    SPELL_PERFECT_POUR_ACHIEV              = 114876,
    SPELL_PARTY_OF_SIX_ACHIEV              = 127411,
};

enum Events
{
    EVENT_PREPARE_TO_STORM = 1,
    EVENT_LIGHTNING_ROD,
    EVENT_VILETONGUE_SAUROKS,
    EVENT_CHAPTER_2_PREPARE,
    EVENT_ESCORT,
    EVENT_TORCH_TOSS,
    EVENT_BLANCHE_POTION,
    EVENT_BREATHING,
    EVENT_TOSS,
    EVENT_WIND_SLASH,
    EVENT_SWAMP_SMASH,
    EVENT_EARTH_SHATTERING,
};

enum Yells
{
    TALK_INTRO,
    TALK_KEGS,
    TALK_LIGHTNING_STRIKE,
    TALK_SAUROKS,
    TALK_STORM,
    TALK_POTION_ANN,
    TALK_LIGHTNING_HIT_PLAYER,
    TALK_CHAPTER_2_PREPARE,
    TALK_CHAPTER_2_OUTRO,
    TALK_CHAPTER_2_BEGUN,
    TALK_SAUROKS_AT_HIGH_CLIFF,
    TALK_PLAYER_LOW,
    TALK_AT_CLIFF,
    TALK_AT_WALL_CLIFF,
    TALK_JOKE,
    TALK_BREATHING,
    TALK_READY_TO_MOVE,
    TALK_CHAPTER_THREE_INTRO,
    TALK_CHAPTER_THREE_BEGIN,
    TALK_BOOMERS_BREW_ANN,
    TALK_BOROKHALA_INTRO,
    TALK_BOROKHALA_LOW,
    TALK_SCENARIO_END,
};

// Brewmaster Blanche 58740
class npc_brewmaster_blanche : public CreatureScript
{
    public:
        npc_brewmaster_blanche() : CreatureScript("npc_brewmaster_blanche") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 1:
                    creature->AI()->DoAction(ACTION_MAKE_BOOMERS_BREW);
                    creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 2:
                    creature->AI()->DoAction(ACTION_ROAD_TO_THUNDERPAW);
                    creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 3:
                    creature->AI()->DoAction(ACTION_SAVE_THUNDERPAW_REFUGE);
                    creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    break;
            }

            player->CLOSE_GOSSIP_MENU();

            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                if (instance->GetData(DATA_MAKE_BOOMERS_BREW) < DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, TEXT_CHAPTER_MAKE_BOOMERS_BREW, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                if (instance->GetData(DATA_ROAD_TO_THUNDERPAW) < DONE && instance->GetData(DATA_ROAD_TO_THUNDERPAW - 1) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, TEXT_CHAPTER_ROAD_TO_THUNDERPAW, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

                if (instance->GetData(DATA_SAVE_THUNDERPAW_REFUGE) < DONE && instance->GetData(DATA_SAVE_THUNDERPAW_REFUGE - 1) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, TEXT_CHAPTER_SAVE_THUNDERPAW_REFUGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            }

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        struct npc_brewmaster_blancheAI : public ScriptedAI
        {
            npc_brewmaster_blancheAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            SummonList summons;
            EventMap events, nonCombatEvents;
            InstanceScript* instance;
            uint32 wp;
            bool SayRoods, SaySauroks, SayBrewBar;
            bool inSecondStage;
            uint32 sauroksCount;
            uint32 allowAchiev;
            uint32 partyofSix;
            bool hasLaunched;

            void IsSummonedBy(Unit* summoner) override { }

            void InitializeAI() override
            {
               wp            = 0;
               sauroksCount  = 0;
               allowAchiev   = 1;
               partyofSix    = 1;
               SayRoods      = false;
               SaySauroks    = false;
               SayBrewBar    = false;
               inSecondStage = false;
               hasLaunched   = false;
               instance = me->GetInstanceScript();
               me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
               Reset();
            }

            void Reset() override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_PERFECT_POUR, 1);
                me->GetMap()->SetWorldState(WORLDSTATE_PARTY_OF_SIX, 1); // controlled by spells
                me->SetWalk(false);
            }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_MAKE_BOOMERS_BREW:
                        Talk(TALK_INTRO);
                        SendBoomersBrewBarToPlayers();
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        nonCombatEvents.ScheduleEvent(EVENT_PREPARE_TO_STORM, 4500);
                        nonCombatEvents.ScheduleEvent(EVENT_BLANCHE_POTION, urand(49 * IN_MILLISECONDS, 59 * IN_MILLISECONDS));
                        break;
                    case ACTION_BREW_POWER:
                        SendBoomersBrewBarToPlayers(true);
                        break;
                    case ACTION_CHAPTER_ONE_COMPLETED:
                        SendBoomersBrewBarToPlayers(false, true);
                        nonCombatEvents.Reset();
                        Talk(TALK_CHAPTER_2_PREPARE);
                        Talk(TALK_CHAPTER_2_OUTRO);

                        if (instance)
                            instance->SetData(DATA_MAKE_BOOMERS_BREW, DONE);

                        nonCombatEvents.ScheduleEvent(EVENT_CHAPTER_2_PREPARE, 8000);
                        break;
                    case ACTION_ROAD_TO_THUNDERPAW:
                        if (hasLaunched)
                            break;

                        hasLaunched = true;
                        //Talk()
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        sauroksCount = 0;
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 2000);
                        break;
                    case ACTION_SAVE_THUNDERPAW_REFUGE:
                        //Talk();
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 2500);
                        break;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (!inSecondStage)
                    return;

                sauroksCount++;
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
            {
                if (!inSecondStage)
                    return;

                sauroksCount--;
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_HIT_BY_ROAD:
                        return allowAchiev;
                    case TYPE_PARTY_OF_SIX:
                        return partyofSix;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_HIT_BY_ROAD:
                        allowAchiev = data;
                        break;
                    case TYPE_PARTY_OF_SIX:
                        partyofSix = data;
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                wp++;

                switch (pointId)
                {
                    case 6:
                        if (Creature* SkirmisherStalker = GetClosestCreatureWithEntry(me, NPC_VILETONGUE_STALKER, 35.0f, true))
                            SkirmisherStalker->AI()->DoAction(ACTION_VILETONGUE_AT_GROUND);

                        if (Creature* raiderStalker = ObjectAccessor::GetCreature(*me, SelectAnyStalkerGUID(NPC_VILETONGUE_VEHICLE)))
                            raiderStalker->AI()->DoAction(ACTION_VILETONGUE_AT_HILL);

                        Talk(TALK_SAUROKS_AT_HIGH_CLIFF);
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 5000);
                        break;
                    case 14:
                        Talk(TALK_AT_CLIFF);

                        if (Creature* SkirmisherStalker = GetClosestCreatureWithEntry(me, NPC_VILETONGUE_STALKER, 35.0f, true))
                            SkirmisherStalker->AI()->DoAction(ACTION_VILETONGUE_AT_GROUND);

                        me->SetFacingTo(abs(me->GetOrientation() + M_PI / 2));
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 5000);
                        break;
                    case 15:
                        if (Creature* SkirmisherStalker = GetClosestCreatureWithEntry(me, NPC_VILETONGUE_STALKER, 35.0f, true))
                            SkirmisherStalker->AI()->DoAction(ACTION_VILETONGUE_AT_GROUND);

                        Talk(TALK_AT_WALL_CLIFF);
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 5000);
                        break;
                    case 16:
                        if (Creature* SkirmisherStalker = GetClosestCreatureWithEntry(me, NPC_VILETONGUE_STALKER, 35.0f, true))
                            SkirmisherStalker->AI()->DoAction(ACTION_VILETONGUE_AT_HILL);

                        Talk(TALK_JOKE);
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 5000);
                        break;
                    case 18:
                        Talk(TALK_BREATHING);
                        nonCombatEvents.ScheduleEvent(EVENT_BREATHING, 7.5 * IN_MILLISECONDS);
                        break;
                    case 34:
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        Talk(TALK_CHAPTER_THREE_BEGIN);
                        Talk(TALK_BOOMERS_BREW_ANN);

                        if (instance)
                        {
                            instance->DoCastSpellOnPlayers(SPELL_BOOMER_BREW_STRIKE_BUTTON);
                            instance->SetData(DATA_ROAD_TO_THUNDERPAW, DONE);
                        }
                        break;
                    default:
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, urand(100, 200));
                        break;
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ScriptedAI::EnterEvadeMode();
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                if (HasChapterOneCompleted())
                    DoAction(ACTION_CHAPTER_ONE_COMPLETED);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PREPARE_TO_STORM:
                            Talk(TALK_KEGS);
                            BrewOnKegs();
                            nonCombatEvents.ScheduleEvent(EVENT_LIGHTNING_ROD, 5000);
                            nonCombatEvents.ScheduleEvent(EVENT_VILETONGUE_SAUROKS, urand(9000, 12000));
                            break;
                        case EVENT_LIGHTNING_ROD:
                            if (!SayRoods)
                            {
                                SayRoods = true;
                                Talk(TALK_LIGHTNING_STRIKE);
                            }

                            if (Creature* selectedRod = ObjectAccessor::GetCreature(*me, SelectAnyStalkerGUID(NPC_LIGHTNING_ROD)))
                                selectedRod->CastSpell(selectedRod, SPELL_LIGHTNING_CHANNEL, false);

                            nonCombatEvents.ScheduleEvent(EVENT_LIGHTNING_ROD, urand(8000, 14000));
                            break;
                        case EVENT_VILETONGUE_SAUROKS:
                            if (!SaySauroks)
                            {
                                SaySauroks = true;
                                Talk(TALK_SAUROKS);
                            }

                            if (Creature* SkirmisherStalker = GetClosestCreatureWithEntry(me, NPC_VILETONGUE_STALKER, 35.0f, true))
                                SkirmisherStalker->AI()->DoAction(ACTION_VILETONGUE_AT_GROUND);

                            if (Creature* raiderStalker = ObjectAccessor::GetCreature(*me, SelectAnyStalkerGUID(NPC_VILETONGUE_VEHICLE)))
                                raiderStalker->AI()->DoAction(ACTION_VILETONGUE_AT_HILL);

                            if (instance && instance->GetData(DATA_MAKE_BOOMERS_BREW) < DONE)
                                nonCombatEvents.ScheduleEvent(EVENT_VILETONGUE_SAUROKS, urand(35000, 40000));
                            break;
                        case EVENT_CHAPTER_2_PREPARE:
                            Talk(TALK_CHAPTER_2_BEGUN);
                            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            nonCombatEvents.ScheduleEvent(EVENT_BLANCHE_POTION, urand(49 * IN_MILLISECONDS, 59 * IN_MILLISECONDS));
                            DoCast(me, SPELL_HONORARY_BREWMASTER_KEG);
                            inSecondStage = true;
                            break;
                        case EVENT_ESCORT:
                            if (sauroksCount)
                            {
                                nonCombatEvents.RescheduleEvent(EVENT_ESCORT, 1 * IN_MILLISECONDS);
                                break;
                            }
                            me->GetMotionMaster()->MovementExpired();

                            if (wp < 35)
                                me->GetMotionMaster()->MovePoint(wp, BlancheWaypoints[wp]);
                            break;
                        case EVENT_BLANCHE_POTION:
                            if (Player* pItr = me->FindNearestPlayer(100.0f))
                                DoCast(pItr, SPELL_BLANCHES_ELIXIR_OF_REPLENISHMENT);

                            Talk(TALK_POTION_ANN);
                            nonCombatEvents.ScheduleEvent(EVENT_BLANCHE_POTION, urand(49 * IN_MILLISECONDS, 59 * IN_MILLISECONDS));
                            break;
                        case EVENT_BREATHING:
                            Talk(TALK_READY_TO_MOVE);
                            nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 1.5 * IN_MILLISECONDS);
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }

        private:
            void SendBoomersBrewBarToPlayers(bool update = false, bool remove = false)
            {
                std::list<Player*> PlayersInScenario;
                GetPlayerListInGrid(PlayersInScenario, me, 150.0f);

                for (auto&& itr : PlayersInScenario)
                    if (!itr->HasAura(SPELL_BLANSHES_BOOMER_BREW_POWER_BAR))
                        me->AddAura(SPELL_BLANSHES_BOOMER_BREW_POWER_BAR, itr);

                if (update)
                    for (auto&& itr : PlayersInScenario)
                        if (itr->GetPower(POWER_ALTERNATE_POWER) < 100)
                            itr->SetPower(POWER_ALTERNATE_POWER, itr->GetPower(POWER_ALTERNATE_POWER) + 5);

                if (remove)
                    for (auto&& itr : PlayersInScenario)
                        if (itr->HasAura(SPELL_BLANSHES_BOOMER_BREW_POWER_BAR))
                            itr->RemoveAura(SPELL_BLANSHES_BOOMER_BREW_POWER_BAR);
            }

            bool HasChapterOneCompleted()
            {
                if (Player* itr = me->FindNearestPlayer(100.0f)) // we need check anyone player 
                {
                    if (!SayBrewBar && (itr->GetPower(POWER_ALTERNATE_POWER) > 50))
                    {
                        SayBrewBar = true;
                        Talk(TALK_STORM);
                    }

                    if (itr->GetPower(POWER_ALTERNATE_POWER) == 100)
                        return true;
                }

                return false;
            }

            ObjectGuid SelectAnyStalkerGUID(uint32 npc_entry)
            {
                std::list<Creature*> StalkerList;
                GetCreatureListWithEntryInGrid(StalkerList, me, npc_entry, 30.0f);

                if (StalkerList.empty())
                    return ObjectGuid::Empty;

                return Trinity::Containers::SelectRandomContainerElement(StalkerList)->GetGUID();
            }

            void BrewOnKegs()
            {
                std::list<Creature*> BrewKegs;
                GetCreatureListWithEntryInGrid(BrewKegs, me, NPC_BREWKEG, 150.0f);

                for (auto&& itr : BrewKegs)
                    itr->AI()->DoAction(ACTION_MAKE_BOOMERS_BREW);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_brewmaster_blancheAI(creature);
        }
};

// Viletongue`s Saurok 58738, 58737
struct npc_viletongue_sauroks : public ScriptedAI
{
    npc_viletongue_sauroks(Creature* creature) : ScriptedAI(creature) { }

    EventMap events, nonCombatEvents;
    bool poisoned;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        if (who)
            DoCast(who, SPELL_LEAPING_CLEAVE);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
            me->SetInCombatWithZone();

            events.ScheduleEvent(EVENT_TORCH_TOSS, urand(5000, 15000));
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (me->GetEntry() == NPC_VILETONGUE_SKIRMISHER && !poisoned && HealthBelowPct(50))
        {
            poisoned = true;

            if (attacker)
                me->CastSpell(attacker, SPELL_VILETONGUE_STING, false);
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
            if (eventId == EVENT_TORCH_TOSS)
            {
                if (Unit* vict = me->GetVictim())
                    DoCast(vict, SPELL_TORCH_TOSS);

                events.ScheduleEvent(EVENT_TORCH_TOSS, urand(5000, 15000));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// BrewKeg 58916
struct npc_brewkeg : public ScriptedAI
{
    npc_brewkeg(Creature* creature) : ScriptedAI(creature) { }

    EventMap events, nonCombatEvents;

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (!me->HasAura(SPELL_ON_FIRE))
            return;

        clicker->CastSpell(me, SPELL_EXTINGUISHING, false);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_MAKE_BOOMERS_BREW)
            nonCombatEvents.ScheduleEvent(EVENT_PREPARE_TO_STORM, urand(1000, 5500));
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_PREPARE_TO_STORM)
                DoCast(me, SPELL_BREW);
            break;
        }
    }
};

// Viletongue`s stalker 59635, 59650
struct npc_viletongue_stalker : public ScriptedAI
{
    npc_viletongue_stalker(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetDisplayFromModel(1);
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_VILETONGUE_AT_GROUND:
                for (uint8 i = 0; i < 2; i++)
                {
                    if (Creature* blanche = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_BREWMASTER_BLANCHE) : ObjectGuid::Empty))
                        if (Creature* raider = blanche->SummonCreature(NPC_VILETONGUE_RAIDER, me->GetPositionX() + frand(-3.0f, 5.0f), me->GetPositionY() + frand(-2.0f, 3.0f), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                            raider->SetInCombatWithZone();
                }
                break;
            case ACTION_VILETONGUE_AT_HILL:
                for (uint8 i = 0; i < 2; i++)
                {
                    if (Creature* blanche = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_BREWMASTER_BLANCHE) : ObjectGuid::Empty))
                    {
                        if (Creature* skirmisher = blanche->SummonCreature(NPC_VILETONGUE_SKIRMISHER, me->GetPositionX() + frand(-2.0f, 2.0f), me->GetPositionY() + frand(-1.0f, 1.0f), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            skirmisher->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                            skirmisher->GetMotionMaster()->MoveJump(CalculateGroundPos(me->GetPosition(), skirmisher->GetGUID()), 8.0f, 15.0f, EVENT_JUMP);
                        }
                    }
                }
                break;
        }
    }

    void UpdateAI(uint32 diff) override { }

    private:
        Position pos;
        // Main method for calculate position for jump into ground from hill
        // diff between stalker that`ll spawn creature and current location around 7-8y
        Position CalculateGroundPos(Position cur, ObjectGuid viletongueGUID)
        {
            float x = 0, y = 0, z = 4.5f, o = cur.GetOrientation();

            if (Creature* viletongue = ObjectAccessor::GetCreature(*me, viletongueGUID))
                GetPositionWithDistInOrientation(viletongue, 8.0f + frand(-0.5f, 2.0f), o, x, y);

            pos.Relocate(x, y, cur.GetPositionZ() + z, o);
            return pos;
        }
};

// Borokhula the Destroyer 58739
struct npc_borokhula_the_destroyer : public ScriptedAI
{
    npc_borokhula_the_destroyer(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;
    EventMap events;
    float x, y;
    bool SayAtLow;

    void InitializeAI() override
    {
        me->setActive(true);
        x = 0.0f; y = 0.0f;
        instance = me->GetInstanceScript();
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
        Reset();
    }

    void Reset() override
    {
        SayAtLow = false;
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SWAMP_SMASH, urand(3 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_EARTH_SHATTERING, urand(12 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_BOROKHULA_INIT)
        {
            GetPositionWithDistInOrientation(me, 25.0f, me->GetOrientation(), x, y);
            me->GetMotionMaster()->MoveJump(x, y, 450.77f, 20.0f, 20.0f, EVENT_JUMP);
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->SetHomePosition(*me);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
            me->SetInCombatWithZone();

            if (Creature* blanche = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_BREWMASTER_BLANCHE) : ObjectGuid::Empty))
                blanche->AI()->Talk(TALK_BOROKHALA_INTRO);
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/) override
    {
        if (!SayAtLow && HealthBelowPct(70))
        {
            SayAtLow = true;

            if (Creature* Blanche = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_BREWMASTER_BLANCHE) : ObjectGuid::Empty))
                Blanche->AI()->Talk(TALK_BOROKHALA_LOW);
        }
    }

    void JustDied(Unit* killer) override
    {
        if (instance)
        {
            instance->SetData(DATA_SAVE_THUNDERPAW_REFUGE, DONE);

            if (Creature* blanche = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_BREWMASTER_BLANCHE)))
            {
                blanche->AI()->Talk(TALK_SCENARIO_END);

                if (blanche->AI()->GetData(TYPE_HIT_BY_ROAD))
                    instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_PERFECT_POUR_ACHIEV);

                if (blanche->AI()->GetData(TYPE_PARTY_OF_SIX))
                    instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_PARTY_OF_SIX_ACHIEV);
            }
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
            switch (eventId)
            {
                case EVENT_SWAMP_SMASH:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_SWAMP_SMASH);

                    events.ScheduleEvent(EVENT_SWAMP_SMASH, urand(7.5 * IN_MILLISECONDS, 13.5 * IN_MILLISECONDS));
                    break;
                case EVENT_EARTH_SHATTERING:
                    DoCast(me, SPELL_EARTH_SHATTERING);
                    events.ScheduleEvent(EVENT_EARTH_SHATTERING, urand(15 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Viletongue Decimator 71353
struct npc_viletongue_decimator : public ScriptedAI
{
    npc_viletongue_decimator(Creature* creature) : ScriptedAI(creature), summons(me) { }

    EventMap events;
    SummonList summons;

    void Reset() override
    {
        events.Reset();
        summons.DespawnAll();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_TORCH_TOSS, urand(7 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_WIND_SLASH, 5 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->CastSpell(summon, SPELL_WIND_SLASH_AUR, true);
        summon->GetMotionMaster()->MoveRandom(3.5f);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_TORCH_TOSS:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                        DoCast(target, SPELL_TORCH_TOSS);
                    else if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_TORCH_TOSS);
                
                    events.ScheduleEvent(EVENT_TORCH_TOSS, urand(7 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                    break;
                case EVENT_WIND_SLASH:
                    DoCast(me, SPELL_WIND_SLASH);
                    events.ScheduleEvent(EVENT_WIND_SLASH, 12.5 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// lightning impact 111544
class spell_brewing_storm_lightning_impact : public SpellScript
{
    PrepareSpellScript(spell_brewing_storm_lightning_impact);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
        {
            target->CastSpell(target, SPELL_ON_FIRE, true);

            if (Creature* Blanche = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(NPC_BREWMASTER_BLANCHE) : ObjectGuid::Empty))
                Blanche->GetAI()->DoAction(ACTION_BREW_POWER);
        }
        else if (Player* target = GetHitPlayer())
            if (Creature* blanche = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(NPC_BREWMASTER_BLANCHE) : ObjectGuid::Empty))
                blanche->AI()->SetData(TYPE_HIT_BY_ROAD, 0);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return (target->GetEntry() != NPC_BREWKEG && !target->ToPlayer()) || target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_ON_FIRE); });

        uint32 affectedCount = urand(1, 2);

        if (targets.size() > affectedCount)
            Trinity::Containers::RandomResizeList(targets, affectedCount);
    }


    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_brewing_storm_lightning_impact::HandleDummy, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_brewing_storm_lightning_impact::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// lightning channel 111828
class spell_brewing_storm_lightning_channel : public AuraScript
{
    PrepareAuraScript(spell_brewing_storm_lightning_channel)

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_LIGHTNING_IMPACT, false);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_brewing_storm_lightning_channel::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Boomer Brew Strike 115058
class spell_boomer_brew_strike : public SpellScript
{
    PrepareSpellScript(spell_boomer_brew_strike);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (target->HasAura(SPELL_BOOMER_BREW_STRIKE) && target->GetEntry() != NPC_BOROKHULA_THE_DESTROYER)
                GetCaster()->Kill(target);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_boomer_brew_strike::HandleDummy, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// 340. Summoned by 114663 - Blanche`s Elexir
class sat_blanches_elexir : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_BLANCHES_ELEXIR_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_BLANCHES_ELEXIR_EFF);
    }
};

void AddSC_brewing_storm_scenario()
{
    new npc_brewmaster_blanche();
    new creature_script<npc_viletongue_sauroks>("npc_viletongue_sauroks");
    new creature_script<npc_brewkeg>("npc_brewkeg");
    new creature_script<npc_viletongue_stalker>("npc_viletongue_stalker");
    new creature_script<npc_borokhula_the_destroyer>("npc_borokhula_the_destroyer");
    new creature_script<npc_viletongue_decimator>("npc_viletongue_decimator");

    new spell_script<spell_brewing_storm_lightning_impact>("spell_brewing_storm_lightning_impact");
    new aura_script<spell_brewing_storm_lightning_channel>("spell_brewing_storm_lightning_channel");
    new spell_script<spell_boomer_brew_strike>("spell_boomer_brew_strike");
    new atrigger_script<sat_blanches_elexir>("sat_blanches_elexir");
}
