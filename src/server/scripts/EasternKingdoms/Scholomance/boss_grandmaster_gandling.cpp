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
    SPELL_HARSH_LESSON                 = 113395,
    SPELL_IMMOLATE                     = 113141,
    SPELL_INCINERATE                   = 113136,
    SPELL_RISE                         = 113143,

    // cosmetic
    SPELL_GANDlING_FIRE_AURA           = 114904, // on dummy that at sky
    SPELL_COSMETIC_CAST                = 21389,  // on me

    // subjects
    SPELL_EXPLOSIVE_PAIN               = 113280, // aura
    SPELL_EXPLOSIVE_PAIN_DMG           = 113312, // in 8y
    SPELL_EXPLOSIVE_PAIN_COSMETIC_AURA = 114882, // triggered to despawn subject
};

enum Events
{
    EVENT_IMMOLATE = 1,
    EVENT_RISE,
    EVENT_HARSH_LESSON,
    EVENT_INCINERATE,
};

enum Texts
{
    TALK_AGGRO        = 0,
    TALK_SLAY,
    TALK_DEATH,
    TALK_HARSH_LESSON,
};

enum RoomTriggersGUID
{
    NPC_ROOM_EAST_LOW                  = 538657,
    NPC_ROOM_NORTH_LOW                 = 538431,
    NPC_ROOM_WEST_LOW                  = 538658,
    NPC_ROOM_EAST_HIGH                 = 538675,
    NPC_ROOM_NORTH_HIGH                = 538327,
    NPC_ROOM_WEST_HIGH                 = 538668,
};

class boss_grandmaster_gandling : public CreatureScript
{
    public:
        boss_grandmaster_gandling() : CreatureScript("boss_grandmaster_gandling") { }

        struct boss_grandmaster_gandlingAI: public BossAI
        {
            boss_grandmaster_gandlingAI(Creature* creature) : BossAI(creature, DATA_GRANDMASTER_GANDLING) { }

            void Reset() override
            {
                _Reset();
                events.Reset();
                me->setRegeneratingHealth(true);
                me->SetReactState(REACT_AGGRESSIVE);
                HandleDoors(true);
            }

            void InitializeAI() override
            {
                SetEquipmentSlots(false, GandlingStaff, 0, EQUIP_NO_CHANGE); 
                me->CastSpell(me, SPELL_COSMETIC_CAST, false);
                SetCombatMovement(false);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(TALK_DEATH);
                if (instance)
                {
                    instance->SetData(DATA_GRANDMASTER_GANDLING, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_GRANDMASTER_GANDLING, FAIL);
                }
                summons.DespawnAll();
                HandleDoors(true);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                Talk(TALK_AGGRO);

                if (instance)
                {
                    instance->SetData(DATA_GRANDMASTER_GANDLING, IN_PROGRESS);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }
                events.ScheduleEvent(EVENT_IMMOLATE, urand(5000, 10000));
                events.ScheduleEvent(EVENT_RISE, 45 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_HARSH_LESSON, urand(25000, 40000));
                events.ScheduleEvent(EVENT_INCINERATE, 0.5 * IN_MILLISECONDS);

                if (Creature* FireStalker = GetClosestCreatureWithEntry(me, NPC_GANDLING_FIRE_STALKER, 50.0f, true))
                    if (FireStalker->HasAura(SPELL_GANDlING_FIRE_AURA))
                        FireStalker->RemoveAura(SPELL_GANDlING_FIRE_AURA);

                HandleDoors();
            }

            void HandleDoors(bool reset = false)
            {
                if (instance)
                    if (GameObject* Idoor = GetClosestGameObjectWithEntry(me, GO_GANDLING_INTRO_DOOR, 150.0f))
                        instance->HandleGameObject(ObjectGuid::Empty, reset, Idoor);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SLAY);
            }

            ObjectGuid RoomController()
            {
                std::list <Creature*> Controllers;
                GetCreatureListWithEntryInGrid(Controllers, me, NPC_GANDLING_ROOM_STALKER, 200.0f);

                if (Controllers.empty())
                    return ObjectGuid::Empty;

                ObjectGuid RoomGUID = Trinity::Containers::SelectRandomContainerElement(Controllers)->GetGUID();

                return RoomGUID;
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING) && me->GetCurrentSpell(CURRENT_GENERIC_SPELL) && me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id != SPELL_INCINERATE)
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_IMMOLATE:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                                me->CastSpell(target, SPELL_IMMOLATE, false);
                            events.ScheduleEvent(EVENT_IMMOLATE, urand(5000, 10000));
                            break;
                        case EVENT_RISE:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            DoCast(me, SPELL_RISE);
                            events.ScheduleEvent(EVENT_RISE, 1 * MINUTE * IN_MILLISECONDS + 10 * IN_MILLISECONDS);
                            break;
                        case EVENT_HARSH_LESSON:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* controller = ObjectAccessor::GetUnit(*me, RoomController()))
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                {
                                    me->CastSpell(target, SPELL_HARSH_LESSON, false);
                                    Talk(TALK_HARSH_LESSON, target);
                                    target->ToPlayer()->TeleportTo(controller->GetMapId(), controller->GetPositionX(), controller->GetPositionY(), controller->GetPositionZ(), controller->GetOrientation(), TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET);
                                }

                            events.ScheduleEvent(EVENT_HARSH_LESSON, 60000);
                            break;
                        case EVENT_INCINERATE:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* vic = me->GetVictim())
                                DoCast(vic, SPELL_INCINERATE);

                            events.ScheduleEvent(EVENT_INCINERATE, urand(2.5 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                            break;
                    }
                }

                EnterEvadeIfOutOfCombatArea(diff, 43.0f);
            }

            private:
                bool HandleRescheduleEventsIfCastAny(uint32 eventId)
                {
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        events.RescheduleEvent(eventId, urand(0.2 * IN_MILLISECONDS, 0.45 * IN_MILLISECONDS));
                        return true;
                    }

                    return false;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_grandmaster_gandlingAI(creature);
        }
};

// Gandling room stalker 59167
class npc_gandling_room_stalker : public CreatureScript
{
    public:
        npc_gandling_room_stalker() : CreatureScript("npc_gandling_room_stalker") { }

        struct npc_gandling_room_stalkerAI : public ScriptedAI
        {
            npc_gandling_room_stalkerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            uint32 GuidDoorType;

            void InitializeAI() override
            {
                switch (me->GetDBTableGUIDLow())
                {
                    case NPC_ROOM_EAST_LOW:
                        GuidDoorType = GO_HARSH_LESSON_EAST_LOW;
                        break;
                    case NPC_ROOM_NORTH_LOW:
                        GuidDoorType = GO_HARSH_LESSON_NORTH_LOW;
                        break;
                    case NPC_ROOM_WEST_LOW:
                        GuidDoorType = GO_HARSH_LESSON_WEST_LOW;
                        break;
                    case NPC_ROOM_EAST_HIGH:
                        GuidDoorType = GO_HARSH_LESSON_EAST_HIGH;
                        break;
                    case NPC_ROOM_NORTH_HIGH:
                        GuidDoorType = GO_HARSH_LESSON_NORTH_HIGH;
                        break;
                    case NPC_ROOM_WEST_HIGH:
                        GuidDoorType = GO_HARSH_LESSON_WEST_HIGH;
                        break;
                }
            }

            bool HasAliveExpiredSubjectInMyRoom()
            {
                std::list<Creature*> ExpiredSubjects;
                GetCreatureListWithEntryInGrid(ExpiredSubjects, me, NPC_EXPIRED_TEST_SUBJECT, 45.0f);

                for (auto&& itr : ExpiredSubjects)
                    if (itr->IsAlive() && abs(itr->GetPositionZ() - me->GetPositionZ()) < 5.0f )
                        return true;

                return false;
            }

            void Reset() override { }

            void DoAction(int32 /*actionId*/) override { }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (HasAliveExpiredSubjectInMyRoom())
                    return;

                if (GameObject* RoomClosest = GetClosestGameObjectWithEntry(me, GuidDoorType, 80.0f))
                    RoomClosest->UseDoorOrButton();
                me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_gandling_room_stalkerAI(creature);
        }
};

// Explosive Pain 113280
class spell_explosive_pain : public SpellScriptLoader
{
    public:
        spell_explosive_pain() : SpellScriptLoader("spell_explosive_pain") { }

        class spell_explosive_pain_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_explosive_pain_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetOwner()->ToUnit())
                    owner->CastSpell(owner, SPELL_EXPLOSIVE_PAIN_DMG, true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_explosive_pain_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_explosive_pain_AuraScript();
        }
};

class PainTargetSelector
{
    public:
        PainTargetSelector() { }

        bool operator ()(WorldObject const* object) const
        {
            return object && object->GetEntry() != NPC_EXPIRED_TEST_SUBJECT;
        }
};

// Explosive Pain proc 113312
class spell_explosive_pain_proc : public SpellScriptLoader
{
    public:
        spell_explosive_pain_proc() : SpellScriptLoader("spell_explosive_pain_proc") { }

        class spell_explosive_pain_proc_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_explosive_pain_proc_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(PainTargetSelector());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_explosive_pain_proc_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_explosive_pain_proc_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_explosive_pain_proc_SpellScript();
        }
};

void AddSC_boss_grandmaster_gandling()
{
    new boss_grandmaster_gandling();
    new npc_gandling_room_stalker();
    new spell_explosive_pain();
    new spell_explosive_pain_proc();
}