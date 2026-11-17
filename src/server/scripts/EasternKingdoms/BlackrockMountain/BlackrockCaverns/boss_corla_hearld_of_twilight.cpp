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
#include "blackrock_caverns.h"

// Add Nether Essence Channel animation
// Add Spell for a Charmed Player

enum ScriptTexts
{
    SAY_AGGRO        = 0,
    SAY_KILL         = 1,
    SAY_TRANSFORM    = 2,
    SAY_DEATH        = 3,
};

enum Spells
{
    // Corlas Spells
    SPELL_EVOLUTION                     = 75697,
    SPELL_TWILIGHT_EVOLUTION            = 75732,

    SPELL_AURA_OF_ACCELERATION          = 75817,
    SPELL_DARK_COMMAND                  = 75823,

    SPELL_KNEELING_IN_SUPPLICATION      = 75608,
    SPELL_DRAIN_ESSENCE_CHANNELING      = 75645,

    SPELL_NETHERESSENCE_AURA            = 75649,
    SPELL_NETHERESSENCE_VISUAL          = 75650,
    SPELL_NETHERBEAM                    = 75677,

    // Envolved Twilight Zealot & Twilight Drakonid
    SPELL_FORCE_BLAST                   = 76522,
    SPELL_GRAVITY_STRIKE                = 76561,
    SPELL_GRIEVOUS_WHIRL                = 93658,
    SPELL_SHADOW_STRIKE                 = 66134,
};

enum Events
{
    EVENT_DARK_COMMAND                  = 1,
    EVENT_FORCE_BLAST                   = 2,
    EVENT_GRAVITY_STRIKE                = 3,
    EVENT_GRIEVOUS_WHIRL                = 4,
    EVENT_SHADOW_STRIKE                 = 5,
};

enum Actions
{
    ACTION_TRIGGER_START_CHANNELING     = 1,
    ACTION_TRIGGER_STOP_CHANNELING      = 2,
};

Position const summonPositions[4] =
{
    { 573.676f, 980.619f, 155.354f, 1.58448f }, // Twilight Zealot Spawn Positions
    { 580.919f, 982.981f, 155.354f, 2.05572f },
    { 565.629f, 983.0f,   155.354f, 0.68912f },
    { 573.272f, 907.707f, 178.506f, 1.55050f }, // Nether Essence Trigger SPawning Position
};

class boss_corla_herald_of_twilight : public CreatureScript
{
    public:
        boss_corla_herald_of_twilight() : CreatureScript("boss_corla_herald_of_twilight") { }

        struct boss_corla_herald_of_twilightAI : public BossAI
        {
            boss_corla_herald_of_twilightAI(Creature* creature) : BossAI(creature, DATA_CORLA)
            {
                for (uint8 i = 0; i <= DUNGEON_MODE(1, 2); i++)
                    NetherEssenceTrigger[i] = NULL;

                for (uint8 i = 0; i <= DUNGEON_MODE(1, 2); i++)
                    TwilightZealotsList[i] = NULL;

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->setActive(true);
            }

            void Reset() override
            {
                if (!me->IsAlive())
                    return;

                me->ResetLootMode();
                events.Reset();
                summons.DespawnAll();
                instance->SetBossState(DATA_CORLA, NOT_STARTED);
                dragonsEvolved = 0;

                me->GetMap()->SetWorldState(WORLDSTATE_ARRESTED_DEVELOPMENT, 0);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                for (uint8 i = 0; i <= DUNGEON_MODE(1, 2); i++)
                {
                    if (NetherEssenceTrigger[i])
                    {
                        NetherEssenceTrigger[i]->DespawnOrUnsummon();
                        NetherEssenceTrigger[i] = NULL;
                    }
                    TwilightZealotsList[i] = NULL;
                }

                me->GetMotionMaster()->MoveTargetedHome();
                me->RemoveAllAuras();

                // Resets Twilight Zealots
                for (uint8 i = 0; i <= DUNGEON_MODE(1, 2); i++)
                {
                    if (TwilightZealotsList[i] == NULL)
                        TwilightZealotsList[i] = me->SummonCreature(NPC_TWILIGHT_ZEALOT_CORLA, summonPositions[i], TEMPSUMMON_MANUAL_DESPAWN);

                    // Spawns the Trigger if neccesary
                    if (NetherEssenceTrigger[i] == NULL)
                        NetherEssenceTrigger[i] = TwilightZealotsList[i]->SummonCreature(NPC_NETHER_ESSENCE_TRIGGER, summonPositions[3], TEMPSUMMON_MANUAL_DESPAWN);

                    // Respawns the Zealots if neccesary
                    if (TwilightZealotsList[i]->isDead())
                        TwilightZealotsList[i]->Respawn();

                    TwilightZealotsList[i]->RemoveAura(SPELL_AURA_OF_ACCELERATION);
                    TwilightZealotsList[i]->RemoveAura(SPELL_TWILIGHT_EVOLUTION);
                    TwilightZealotsList[i]->RemoveAura(SPELL_EVOLUTION);

                    TwilightZealotsList[i]->NearTeleportTo(summonPositions[i].GetPositionX(), summonPositions[i].GetPositionY(), summonPositions[i].GetPositionZ(), summonPositions[i].GetOrientation());

                    if (!TwilightZealotsList[i]->HasAura(SPELL_KNEELING_IN_SUPPLICATION))
                        TwilightZealotsList[i]->CastSpell(TwilightZealotsList[i], SPELL_KNEELING_IN_SUPPLICATION, true);
                }

                // Stops Channeling from the Triggers to the Zealots
                for (uint8 i = 0; i <= DUNGEON_MODE(1,2); i++)
                    NetherEssenceTrigger[i]->GetAI()->DoAction(ACTION_TRIGGER_STOP_CHANNELING);

                me->CastSpell(me, SPELL_DRAIN_ESSENCE_CHANNELING, true);
                HandleRemoveCharmedPlayers();
            }

            void HandleRemoveCharmedPlayers(bool death = true)
            {
                std::list<Player*> targestList;
                GetPlayerListInGrid(targestList, me, 300.0f);

                for (auto &itr : targestList)
                    if (itr->IsCharmed())
                        death ? me->Kill(itr) : itr->RemoveCharmAuras();
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_EVOLVED_SUCCESSED && ++dragonsEvolved > 2)
                    me->GetMap()->SetWorldState(WORLDSTATE_ARRESTED_DEVELOPMENT, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                me->CastStop();
                me->GetMotionMaster()->MoveChase(me->GetVictim());

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_DARK_COMMAND, 20000);

                // Informs the trigger on what Twilight Zealot he should channel the nether essence
                for (uint8 i = 0; i <= DUNGEON_MODE(1, 2); i++)
                    NetherEssenceTrigger[i]->GetAI()->DoAction(ACTION_TRIGGER_START_CHANNELING);

                NetherEssenceTrigger[0]->CastSpell(NetherEssenceTrigger[0], SPELL_NETHERESSENCE_AURA, true);

                DoCastAOE(SPELL_AURA_OF_ACCELERATION);

                Talk(SAY_AGGRO);
            }

            void KilledUnit(Unit* who) override
            {
                if (who && who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void JustReachedHome() override
            {
                me->CastSpell(me, SPELL_DRAIN_ESSENCE_CHANNELING, true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                for (uint8 i = 0; i <= DUNGEON_MODE(1, 2); i++)
                {
                    if (NetherEssenceTrigger[i])
                    {
                        NetherEssenceTrigger[i]->DespawnOrUnsummon();
                        NetherEssenceTrigger[i] = NULL;
                    }
                    TwilightZealotsList[i] = NULL;
                }

                Talk(SAY_DEATH);
                HandleRemoveCharmedPlayers(false);
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
                        case EVENT_DARK_COMMAND:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200, true))
                                DoCast(target,SPELL_DARK_COMMAND);

                            events.ScheduleEvent(EVENT_DARK_COMMAND, 20000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            Creature* TwilightZealotsList[3];
            Creature* NetherEssenceTrigger[3];
            uint32 dragonsEvolved;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_corla_herald_of_twilightAI>(creature);
        }
};

class npc_twilight_zealot : public CreatureScript
{
    public:
        npc_twilight_zealot() : CreatureScript("npc_twilight_zealot") { }

        struct npc_twilight_zealotAI : public ScriptedAI
        {
            npc_twilight_zealotAI(Creature* creature) : ScriptedAI(creature), intialized(false) {}

            void Reset() override
            {
                events.Reset();
                intialized = false;

                me->SetReactState(REACT_PASSIVE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!intialized && !me->HasAura(SPELL_KNEELING_IN_SUPPLICATION))
                {
                    events.ScheduleEvent(EVENT_FORCE_BLAST, 10000);
                    events.ScheduleEvent(EVENT_GRAVITY_STRIKE, 22000);
                    events.ScheduleEvent(EVENT_GRIEVOUS_WHIRL, 7000);
                    events.ScheduleEvent(EVENT_SHADOW_STRIKE, 14000);

                    intialized = true;

                    me->SetReactState(REACT_AGGRESSIVE);

                    me->GetMotionMaster()->MoveChase(GetPlayerAtMinimumRange(1.0f));
                    me->Attack(GetPlayerAtMinimumRange(1.0f), false);
                }

                if (!UpdateVictim() || !me->HasAura(SPELL_TWILIGHT_EVOLUTION))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FORCE_BLAST:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200, true))
                                DoCast(target,SPELL_FORCE_BLAST);
                            events.ScheduleEvent(EVENT_FORCE_BLAST, 10000);
                            break;
                        case EVENT_GRAVITY_STRIKE:
                            DoCastVictim(SPELL_GRAVITY_STRIKE);
                            events.ScheduleEvent(EVENT_GRAVITY_STRIKE, 22000);
                            break;
                        case EVENT_GRIEVOUS_WHIRL:
                            DoCastAOE(SPELL_GRIEVOUS_WHIRL);
                            events.ScheduleEvent(EVENT_GRIEVOUS_WHIRL, 7000);
                            break;
                        case EVENT_SHADOW_STRIKE:
                            DoCastVictim(SPELL_SHADOW_STRIKE);
                            events.ScheduleEvent(EVENT_SHADOW_STRIKE, 14000);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            bool intialized;
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_zealotAI>(creature);
        }
};

class npc_corla_netheressence_trigger : public CreatureScript
{
    public:
        npc_corla_netheressence_trigger() : CreatureScript("npc_corla_netheressence_trigger") { }

        struct npc_corla_netheressence_triggerAI : public ScriptedAI
        {
            npc_corla_netheressence_triggerAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
                zealotGuid.Clear();
                channelTargetGuid.Clear();
                lastTargetGuid.Clear();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!channelTargetGuid && !zealotGuid)
                    return;

                Unit* zealot = ObjectAccessor::GetUnit(*me, zealotGuid);
                if (!zealot)
                    return;

                if (zealot->HasAura(SPELL_TWILIGHT_EVOLUTION) || zealot->isDead())
                    return;

                if (uiCheckPlayerIsBetween <= diff)
                {
                    Unit* channelTarget = zealot;
                    Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();

                    if (!PlayerList.isEmpty())
                    {
                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        {
                            if (i->GetSource()->IsInBetween(me, zealot, 1.0f))
                                channelTarget = i->GetSource();
                        }
                    }

                    if (channelTarget)
                    {
                        me->CastSpell(channelTarget, SPELL_NETHERBEAM, true);
                        channelTargetGuid = channelTarget->GetGUID();
                    }

                    zealot->SetAuraStack(SPELL_EVOLUTION, channelTarget, channelTarget->GetAuraCount(SPELL_EVOLUTION) + 1);

                    if (Aura* aura = channelTarget->GetAura(SPELL_EVOLUTION))
                        aura->RefreshDuration();

                    if (Aura* evolve = channelTarget->GetAura(SPELL_EVOLUTION))
                    {
                        if (evolve->GetStackAmount() > 99)
                        {
                            if (channelTarget == zealot)
                                channelTarget->RemoveAllAuras();
                            if (InstanceScript* instance = me->GetInstanceScript())
                            {
                                if (Creature* Corla = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_CORLA)))
                                {
                                    if (channelTarget->ToPlayer())
                                        Corla->CastSpell(channelTarget, SPELL_TWILIGHT_EVOLUTION, true);
                                    else
                                        channelTarget->AddAura(SPELL_TWILIGHT_EVOLUTION, channelTarget);
                                }
                            }
                        }
                    }

                    uiCheckPlayerIsBetween = 175;
                } else uiCheckPlayerIsBetween -= diff;
            }

            void IsSummonedBy(Unit* summoner) override
            {
                zealotGuid = summoner->GetGUID();
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_TRIGGER_START_CHANNELING:
                        CharmedPlayerList.clearReferences();
                        channelTargetGuid = zealotGuid;
                        uiCheckPlayerIsBetween = 100;
                        //uiNetherEssenceVisual = 500;

                        // To avoid that on beginning no spell is casted.
                        lastTargetGuid = me->GetGUID();
                        break;

                    case ACTION_TRIGGER_STOP_CHANNELING:
                        me->RemoveAllAuras();
                        lastTargetGuid.Clear();
                        channelTargetGuid.Clear();
                        break;
                }
            }

        private:
            ObjectGuid zealotGuid;
            ObjectGuid channelTargetGuid;
            ObjectGuid lastTargetGuid;
            Map::PlayerList CharmedPlayerList;

            // We need np EventMap becouse we have only 1 Event
            uint32 uiCheckPlayerIsBetween;
            uint32 uiNetherEssenceVisual;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_corla_netheressence_triggerAI>(creature);
        }
};

// Twilight Evolution 75732
class spell_twilight_evolution : public AuraScript
{
    PrepareAuraScript(spell_twilight_evolution);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->SetReactState(REACT_AGGRESSIVE);

            if (Creature* corla = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_CORLA) : ObjectGuid::Empty))
                corla->AI()->SetData(TYPE_EVOLVED_SUCCESSED, 1);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_twilight_evolution::OnAuraEffectApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_boss_corla_herald_of_twilight()
{
    new boss_corla_herald_of_twilight();
    new npc_twilight_zealot();
    new npc_corla_netheressence_trigger();
    new aura_script<spell_twilight_evolution>("spell_twilight_evolution");
}