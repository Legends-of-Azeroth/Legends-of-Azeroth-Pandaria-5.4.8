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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Vehicle.h"
#include "VehicleDefines.h"
#include "scholomance.h"

enum Spells
{
    SPELL_WRACK_SOUL          = 111631, // debuff that jump to player from player
    SPELL_ICE_WRATH           = 111610, // just periodic debuff
    SPELL_TOUCH_OF_THE_GRAVE  = 111606, // self buff for diff attack
    SPELL_FRIGID_GRASP_DAMAGE = 114886,
    SPELL_FRIGID_GRASP        = 111239, // on dummy
    SPELL_FROST_GRASP         = 109295, // Heroic only

    SPELL_ARCANE_BOMG         = 113859,
    SPELL_BURN                = 111574,
    SPELL_SHADOW_BOLT         = 113809,
    SPELL_PHYLACTERY_FILLED   = 111256, // cosmetic area aura
    SPELL_SUMMON_BOOKS        = 111669, // should summon books only at hit this spell to triggers 58917
};

enum Events
{
    EVENT_WRACK_SOUL = 1,
    EVENT_ICE_WRATH,
    EVENT_TOUCH_OF_THE_GRAVE,
    EVENT_FRIGID_GRASP,
    EVENT_MOVE_WALL,

    EVENT_ARCANE_BOMB,
    EVENT_SHADOW_BOLT,
    EVENT_COSMETIC,
};

enum Actions
{
    ACTION_ACTIVATE = 0,
    ACTION_DESTROY_WALL,
};

enum Yells
{
    TALK_INTRO = 0,
    TALK_AGGRO,
    TALK_WRACK_SOUL,
    TALK_FAKE_DEATH,
    TALK_DEATH,
    TALK_WAVE_OF_ICE,
    TALK_ESCAPE,
};

class boss_instructor_chillheart : public CreatureScript
{
    public:
        boss_instructor_chillheart() : CreatureScript("boss_instructor_chillheart") { }

        struct boss_instructor_chillheartAI : public BossAI
        {
            boss_instructor_chillheartAI(Creature* creature) : BossAI(creature, DATA_INSTRUCTOR_CHILLHEART) { }

            bool fake, canDie;

            void Reset() override
            {
                _Reset();
                events.Reset();
                me->setRegeneratingHealth(true);
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                RemoveBooks();
                me->CastSpell(me, SPELL_COSMETIC_SHADOW_CHANNEL, false);
                fake = false;
                canDie = false;
                HandleDoors(true);

                if (Creature* IceWallStalker = GetClosestCreatureWithEntry(me, NPC_ICE_WALL_STALKER, 100.0f))
                    IceWallStalker->AI()->DoAction(ACTION_DESTROY_WALL);

                if (Creature* Phylactery = GetClosestCreatureWithEntry(me, NPC_PHYLACTERY, 100.0f, true))
                    Phylactery->AI()->Reset();
            }

            void InitializeAI() override
            {
                Reset();
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();
                Talk(TALK_DEATH);
                RemoveBooks();

                if (instance)
                {
                    instance->SetData(DATA_INSTRUCTOR_CHILLHEART, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }

                if (Creature* IceWallStalker = GetClosestCreatureWithEntry(me, NPC_ICE_WALL_STALKER, 100.0f))
                    IceWallStalker->AI()->DoAction(ACTION_DESTROY_WALL);

                me->RemoveAllAreasTrigger();
            }

            void RemoveCosmeticEvent()
            {
                std::list<Creature*> Casters;
                GetCreatureListWithEntryInGrid(Casters, me, NPC_SCHOLOMANCE_NEOPHYTE, 150.0f);
                GetCreatureListWithEntryInGrid(Casters, me, NPC_SCHOLOMANCE_ACOLYTE, 150.0f);

                for (auto&& itr : Casters)
                    if (itr->IsAlive())
                        me->DealDamage(itr, itr->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }

            void RemoveBooks()
            {
                std::list<Creature*> books;
                GetCreatureListWithEntryInGrid(books, me, NPC_ANARCHIST_ARCANIST, 150.0f);
                GetCreatureListWithEntryInGrid(books, me, NPC_ANTONIDAS_GUIDE, 150.0f);
                GetCreatureListWithEntryInGrid(books, me, NPC_WANDERS_BOOK, 150.0f);

                for (auto&& itr : books)
                    itr->DespawnOrUnsummon();
            }

            void HandleDoors(bool reset = false)
            {
                if (instance)
                    if (GameObject* Idoor = GetClosestGameObjectWithEntry(me, GO_CHILLHEART_INTRO_DOOR, 150.0f))
                    {
                        instance->HandleGameObject(ObjectGuid::Empty, reset, Idoor);
                        Idoor->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                    }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_INSTRUCTOR_CHILLHEART, FAIL);
                }
                summons.DespawnAll();
                HandleDoors(true);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                Talk(TALK_AGGRO);
                events.ScheduleEvent(EVENT_ICE_WRATH, urand(6500, 8000));
                events.ScheduleEvent(EVENT_FRIGID_GRASP, urand(10000, 15000));
                events.ScheduleEvent(EVENT_TOUCH_OF_THE_GRAVE, 1000);
                RemoveCosmeticEvent();
                Talk(TALK_WAVE_OF_ICE);
                HandleDoors();

                if (Creature* IceWallStalker = GetClosestCreatureWithEntry(me, NPC_ICE_WALL_STALKER, 100.0f))
                    IceWallStalker->AI()->DoAction(ACTION_ACTIVATE);

                if (instance)
                {
                    instance->SetData(DATA_INSTRUCTOR_CHILLHEART, IN_PROGRESS);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_WRACK_SOUL, 2500);
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_FROSTGRASP_STALKER)
                {
                    summon->CastSpell(summon, SPELL_FRIGID_GRASP, false);

                    if (IsHeroic())
                        summon->CastSpell(summon, SPELL_FROST_GRASP, true);

                    summon->DespawnOrUnsummon(7000);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override { }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_INTRO)
                    Talk(TALK_INTRO);

                switch (actionId)
                {
                    case ACTION_INTRO:
                        break;
                    case ACTION_CHILLHEART_DEATH:
                        canDie = false;
                        me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override 
            {
                if (canDie)
                    damage = 0;

                if (damage >= me->GetHealth() && !fake)
                {
                    damage = 0;
                    Talk(TALK_FAKE_DEATH);
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    events.Reset();
                    fake = true;
                    canDie = true;
                    Talk(TALK_ESCAPE);
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                    if (Creature* IceWallStalker = GetClosestCreatureWithEntry(me, NPC_ICE_WALL_STALKER, 100.0f))
                        IceWallStalker->AI()->DoAction(ACTION_DESTROY_WALL);

                    if (Creature* Phylactery = GetClosestCreatureWithEntry(me, NPC_PHYLACTERY, 100.0f))
                        Phylactery->AI()->DoAction(ACTION_ACTIVATE);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;


                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WRACK_SOUL:
                            Talk(TALK_WRACK_SOUL);
                            if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST))
                                me->CastSpell(target, SPELL_WRACK_SOUL, false);
                            break;
                        case EVENT_FRIGID_GRASP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                if (target->IsAlive())
                                    me->SummonCreature(NPC_FROSTGRASP_STALKER, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
                            events.ScheduleEvent(EVENT_FRIGID_GRASP, urand(10000, 15000));
                            break;
                        case EVENT_TOUCH_OF_THE_GRAVE:
                            me->CastSpell(me, SPELL_TOUCH_OF_THE_GRAVE, false);
                            events.ScheduleEvent(EVENT_TOUCH_OF_THE_GRAVE, urand(18000, 19000));
                            break;
                        case EVENT_ICE_WRATH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 70.0f, true))
                                me->AddAura(SPELL_ICE_WRATH, target);
                            events.ScheduleEvent(EVENT_ICE_WRATH, urand(11000, 16000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 53.0f);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_instructor_chillheartAI(creature);
        }
};

// instructor chillhearts phylactery 58664
class npc_instructor_chillhearts_phylactery : public CreatureScript
{
    public:
        npc_instructor_chillhearts_phylactery() : CreatureScript("npc_instructor_chillhearts_phylactery") { }

        struct npc_instructor_chillhearts_phylacteryAI : public ScriptedAI
        {
            npc_instructor_chillhearts_phylacteryAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 timer;
            EventMap nonCombatEvents;

            void IsSummonedBy(Unit* summoner) override { }

            void Reset() override 
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->RemoveAurasDueToSpell(SPELL_PHYLACTERY_FILLED);
                // skip a bit of time for initialize our npc
                nonCombatEvents.ScheduleEvent(EVENT_COSMETIC, 2 * IN_MILLISECONDS);
            }

            void InitializeAI() override 
            { 
                Reset();
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (me->GetHealth() <= damage)
                    if (Creature* Chillheart = GetClosestCreatureWithEntry(me, NPC_INSTRUCTOR_CHILLHEART, 100.0f, true))
                        Chillheart->AI()->DoAction(ACTION_CHILLHEART_DEATH);
            }

            void CosmeticEvent()
            {
                if (Creature* Chillheart = GetClosestCreatureWithEntry(me, NPC_INSTRUCTOR_CHILLHEART, 100.0f, true))
                    Chillheart->CastSpell(me, SPELL_COSMETIC_SHADOW_CHANNEL, false);

                std::list<Creature*> ScholomancePhanatics;
                GetCreatureListWithEntryInGrid(ScholomancePhanatics, me, NPC_SCHOLOMANCE_ACOLYTE, 16.0f);
                GetCreatureListWithEntryInGrid(ScholomancePhanatics, me, NPC_SCHOLOMANCE_NEOPHYTE, 16.0f);

                for (auto&& itr : ScholomancePhanatics)
                    if (itr->IsAlive())
                        itr->CastSpell(me, SPELL_COSMETIC_SHADOW_CHANNEL, false);
            }

            void DoAction(int32 actionId) override 
            {
                if (actionId == ACTION_ACTIVATE)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                    // Cometic effects
                    me->CastSpell(me, SPELL_FILL_PHYLACTERY, false);
                    me->CastSpell(me, SPELL_PHYLACTERY_FILLED, false);

                    // Summon books
                    me->CastSpell(me, SPELL_SUMMON_BOOKS, false);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override 
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_COSMETIC)
                        CosmeticEvent();
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_instructor_chillhearts_phylacteryAI(creature);
        }
};

// ice wall stalkers 68291
class npc_ice_wall_stalker : public CreatureScript
{
    public:
        npc_ice_wall_stalker() : CreatureScript("npc_ice_wall_stalker") { }

        struct npc_ice_wall_stalkerAI : public ScriptedAI
        {
            npc_ice_wall_stalkerAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            SummonList summons;
            EventMap nonCombatEvents;
            float PosY;

            void IsSummonedBy(Unit* /*summoner*/) override { }

            void Reset() override { }

            void InitializeAI() override { }

            void JustSummoned(Creature* summon) override
            {
                summon->CastSpell(summon, SPELL_ICE_WALL_DUMMY_2, false);
                summon->DespawnOrUnsummon(7000);
            }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_ACTIVATE:
                        me->CastSpell(me, SPELL_ICE_WALL_DUMMY_1, false);
                        CreateTriggerWall(me->GetGUID());
                        PosY = me->GetPositionY() + 4.0f;
                        nonCombatEvents.ScheduleEvent(EVENT_MOVE_WALL, 7000);
                        break;
                    case ACTION_DESTROY_WALL:
                        nonCombatEvents.Reset();
                        PosY = me->GetPositionY() + 4.0f;
                        summons.DespawnAll();
                        me->RemoveAllAreasTrigger();
                        break;
                    default:
                        break;
                }
            }

            void CreateTriggerWall(ObjectGuid Main)
            {
                uint32 mod = 1;

                if (Creature* IceWall = ObjectAccessor::GetCreature(*me, Main))
                {
                    for (uint8 i = 0; i < 7; i++) // dist between walls ~ 70y [7*5*2~65]
                    {
                        me->SummonCreature(NPC_ICE_WALL_MOVE, IceWall->GetPositionX() + 5.0f*mod, IceWall->GetPositionY(), IceWall->GetPositionZ(), IceWall->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
                        me->SummonCreature(NPC_ICE_WALL_MOVE, IceWall->GetPositionX() - 5.0f*mod, IceWall->GetPositionY(), IceWall->GetPositionZ(), IceWall->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
                        mod++;
                    }
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/) override { }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_MOVE_WALL)
                    {
                        if (Creature* IceWallTrigger = me->SummonCreature(NPC_ICE_WALL_MOVE, me->GetPositionX(), me->GetPositionY() + PosY, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            IceWallTrigger->CastSpell(IceWallTrigger, SPELL_ICE_WALL_DUMMY_1);
                            CreateTriggerWall(IceWallTrigger->GetGUID());
                        }
                        PosY += 4.0f;
                        nonCombatEvents.ScheduleEvent(EVENT_MOVE_WALL, 7000);
                    }
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ice_wall_stalkerAI(creature);
        }
};

// Magical Tomes 59707, 66240, 59227, 59711
class npc_magical_tomes : public CreatureScript
{
    public:
        npc_magical_tomes() : CreatureScript("npc_magical_tomes") { }

        struct npc_magical_tomesAI : public ScriptedAI
        {
            npc_magical_tomesAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void IsSummonedBy(Unit* summoner) override 
            { 
                me->SetReactState(REACT_PASSIVE); 
                DoZoneInCombat();
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                switch (me->GetEntry())
                {
                    case NPC_ANARCHIST_ARCANIST:
                        events.ScheduleEvent(EVENT_ARCANE_BOMB, urand(100, 200));
                        me->GetMotionMaster()->MovePoint(1, PhylateryPoint());
                        break;
                    case NPC_ANTONIDAS_GUIDE:
                        me->CastSpell(me, SPELL_BURN, false);
                        me->GetMotionMaster()->MovePoint(1, PhylateryPoint());
                        break;
                    case NPC_WANDERS_BOOK:
                        events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(100, 200));
                        me->GetMotionMaster()->MovePoint(1, PhylateryPoint());
                        break;
                    case NPC_ARCANE_BOMB_CHASE:
                        me->SummonCreature(NPC_ARCANE_BOMB, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 7.0f, TEMPSUMMON_MANUAL_DESPAWN);
                        break;
                }
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
            }

            Position PhylateryPoint()
            {
                // Still wrong, just shutting up the analyzer
                float x = me->GetPositionX(), y = me->GetPositionY();
                Position pos;

                if (Creature* Phylactery = GetClosestCreatureWithEntry(me, NPC_PHYLACTERY, 100.0f, true))
                    GetPositionWithDistInOrientation(me, me->GetDistance2d(Phylactery), me->GetAngle(Phylactery), x, y);

                pos = { x + frand(-6.0f, 6.0f), y + frand(-4.5f, 4.5f), me->GetPositionZ(), me->GetOrientation() };

                return pos;
            }

            void InitializeAI() override { }

            void JustSummoned(Creature* summon) override 
            {
                switch (summon->GetEntry())
                {
                    case NPC_ARCANE_BOMB:
                        summon->CastSpell(summon, SPELL_ARCANE_BOMB_COSMETIC, false);
                        break;
                    case NPC_ARCANE_BOMB_CHASE:
                        summon->SetSpeed(MOVE_RUN, 0.4f);
                        summon->SetSpeed(MOVE_WALK, 0.4f);
                        summon->SetSpeed(MOVE_FLIGHT, 0.4f);
                        summon->CastSpell(summon, SPELL_ARCNE_BOMB_COSMETIC_SPHERE, false);
                        summon->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 7.0f);
                        break;
                }
            }

            void DoAction(int32 /*actionId*/) override { }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override 
            {
                damage = 0;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case 0:
                        me->CastSpell(me, SPELL_ARCANE_BOMG, false);

                        if (Creature* arcane = GetClosestCreatureWithEntry(me, NPC_ARCANE_BOMB, 0.5f))
                            arcane->DespawnOrUnsummon();

                        me->DespawnOrUnsummon();
                        break;
                    case 1:
                        me->SetHomePosition(*me);
                        me->GetMotionMaster()->MoveRandom(18.0f);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override 
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARCANE_BOMB:
                            me->SummonCreature(NPC_ARCANE_BOMB_CHASE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
                            events.ScheduleEvent(EVENT_ARCANE_BOMB, urand(4000, 6000));
                            break;
                        case EVENT_SHADOW_BOLT:
                            if (Unit* itr = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                                me->CastSpell(itr, SPELL_SHADOW_BOLT, false);

                            events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(3000, 5000));
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_magical_tomesAI(creature);
        }
};

// Wrack soul 111631
class spell_aura_wrack_soul : public SpellScriptLoader
{
    public:
        spell_aura_wrack_soul() : SpellScriptLoader("spell_aura_wrack_soul") { }

        class spell_aura_wrack_soul_script : public AuraScript
        {
            PrepareAuraScript(spell_aura_wrack_soul_script)

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (caster->IsAlive())
                        if (Player* target = GetTarget()->ToPlayer())
                            caster->CastSpell(target, SPELL_WRACK_SOUL); // dummy
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_aura_wrack_soul_script::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_aura_wrack_soul_script();
        }
};

// Summon Books 111669
class spell_summon_books : public SpellScriptLoader
{
    public:
        spell_summon_books() : SpellScriptLoader("spell_summon_books") { }

        class spell_summon_books_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_summon_books_SpellScript);

            void SelectTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_BOOK_SPAWN_TARGET; });
            }

            void HandleDummyEffect(SpellEffIndex effIndex)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetHitUnit())
                        caster->SummonCreature(GetAnyBook(), target->GetPositionX() + frand(-3.0f, 3.0f), target->GetPositionY() + frand(-2.0f, 2.0f), caster->GetPositionZ() + 7.0f, TEMPSUMMON_MANUAL_DESPAWN);
            }

            // idea to get equal count of books each type
            uint32 GetAnyBook()
            {
                std::map <uint32, uint32> Books;

                Books.insert(std::pair<uint32, uint32>(NPC_ANARCHIST_ARCANIST, GetCountExistedBook(NPC_ANARCHIST_ARCANIST)));
                Books.insert(std::pair<uint32, uint32>(NPC_ANTONIDAS_GUIDE, GetCountExistedBook(NPC_ANTONIDAS_GUIDE)));
                Books.insert(std::pair<uint32, uint32>(NPC_WANDERS_BOOK, GetCountExistedBook(NPC_WANDERS_BOOK)));

                // doesn`t matter if 2 or 3 books have equal count of used books. It should select smth
                uint8 selectedType = std::min(Books.begin()->second, Books.end()->second);
                uint32 selectedBook = 0;

                // map find doesn`t work, then use handle finder...
                for (auto &&itr : Books)
                    if (itr.second == selectedType)
                        selectedBook = itr.first;

                return selectedBook;
            }

            uint32 GetCountExistedBook(uint32 bookType)
            {
                std::list<Creature*> Books;
                GetCreatureListWithEntryInGrid(Books, GetCaster(), bookType, 100.0f);

                return Books.size();
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_summon_books_SpellScript::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_summon_books_SpellScript::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_summon_books_SpellScript();
        }
};

class sat_ice_wall : public SpellAreaTriggerScript
{
    public:
        sat_ice_wall() : SpellAreaTriggerScript("sat_ice_wall") { }

        class sat_ice_wall_IAreaTriggerOnce : public IAreaTriggerOnce
        {
            bool CheckTriggering(WorldObject* triggering) override
            {
                Player* player = triggering->ToPlayer();

                if (!player)
                    return false;

                return player->IsAlive() && (m_target->GetExactDist2d(player) < m_range);
            }

            void OnTrigger(WorldObject* triggering) override
            {
                Unit* target = triggering->ToUnit();

                if (target)
                    target->CastSpell(target, SPELL_ICE_WALL_INSTA_KILL, true);
            }
        };

        IAreaTrigger* GetInterface() const override
        {
            return new sat_ice_wall_IAreaTriggerOnce();
        }
};

void AddSC_boss_instructor_chillheart()
{
    new boss_instructor_chillheart();
    new npc_instructor_chillhearts_phylactery();
    new npc_ice_wall_stalker();
    new npc_magical_tomes();
    new spell_aura_wrack_soul();
    new spell_summon_books();
    new sat_ice_wall();
}
