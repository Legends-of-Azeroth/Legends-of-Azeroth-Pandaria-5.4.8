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
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "ulduar.h"
#include "Player.h"

enum VezaxYells
{
    // General Vezax
    SAY_AGGRO                                   = 0,
    SAY_SLAY                                    = 1,
    SAY_SURGE_OF_DARKNESS                       = 2,
    SAY_DEATH                                   = 3,
    SAY_BERSERK                                 = 4,
    SAY_HARDMODE                                = 5,
    EMOTE_ANIMUS                                = 6,
    EMOTE_BARRIER                               = 7,
    EMOTE_SURGE_OF_DARKNESS                     = 8,
    EMOTE_BARRIER_END                           = 9,

    // Saronite Vapors
    EMOTE_VAPORS                                = 0
};

enum VezaxSpells
{
    SPELL_AURA_OF_DESPAIR                       = 62692,
    SPELL_AURA_OF_DESPAIR_EFFECT_DESPAIR        = 64848,
    SPELL_CORRUPTED_RAGE                        = 68415,
    SPELL_MARK_OF_THE_FACELESS                  = 63276,
    SPELL_MARK_OF_THE_FACELESS_DAMAGE           = 63278,
    SPELL_SARONITE_BARRIER                      = 63364,
    SPELL_SEARING_FLAMES                        = 62661,
    SPELL_SHADOW_CRASH                          = 62660,
    SPELL_SHADOW_CRASH_HIT                      = 62659,
    SPELL_SHADOW_CRASH_AURA                     = 63277, // Triggered Cloud
    SPELL_SURGE_OF_DARKNESS                     = 62662,
    SPELL_SUMMON_SARONITE_VAPORS                = 63081,
    // Saronit Animus
    SPELL_PROFOUND_DARKNESS                     = 63420,
    SPELL_SUMMON_SARONITE_ANIMUS                = 63145,
    SPELL_VISUAL_SARONITE_ANIMUS                = 63319,
    SPELL_PROFOUND_OF_DARKNESS                  = 63420,
    SPELL_VERTEX_SHADE_BLACK                    = 39833,

    // Saronit Vapor
    SPELL_SARONITE_VAPORS                       = 63323,
    SPELL_SARONITE_VAPOR_AURA                   = 63322, // Unknown Aura Dummy needs Scripting ?
    SPELL_SARONITE_VAPORS_MANA                  = 63337,
    SPELL_SARONITE_VAPORS_DAMAGE                = 63338,

    // Player Shaman
    SPELL_SHAMANTIC_RAGE                        = 30823,

    // Enrage
    SPELL_BERSERK                               = 47008,
};

enum Events
{
    EVENT_SHADOW_CRASH                  = 1,
    EVENT_SEARING_FLAMES                = 2,
    EVENT_SURGE_OF_DARKNESS             = 3,
    EVENT_MARK_OF_THE_FACELESS          = 4,
    EVENT_SUMMON_SARONITE_VAPOR         = 5,
    EVENT_BERSERK                       = 6
};

enum NPCs
{
    NPC_GENERAL_VEZAX                            = 33271,
    NPC_SARONITE_VAPOR                           = 33488,
    NPC_SARONITE_ANIMUS                          = 33524,
    NPC_VEZAX_BUNNY                              = 33500,
};

enum AchievData
{
    DATA_SMELL_OF_SARONITE          = 0,
    DATA_SHADOWDODGER               = 1
};

enum VezaxActions
{
    ACTION_VAPORS_DIE       = 0, // Only used since a saronite vapor does not _really_ die
    ACTION_VAPORS_COMBINE   = 1,
};

class boss_general_vezax : public CreatureScript
{
    public:
        boss_general_vezax() : CreatureScript("boss_general_vezax") { }

        struct boss_general_vezaxAI : public BossAI
        {
            boss_general_vezaxAI(Creature* creature) : BossAI(creature, BOSS_VEZAX) { }

            void Reset() override
            {
                _Reset();
                _shadowDodger = true;
                _notHardModed = true;
                me->GetMap()->SetWorldState(WORLDSTATE_SMELL_OF_SARONITE, 0);
                _vaporsSpawned = 0;
                _vaporsAlive = 0;

                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                me->GetMap()->SetWorldState(WORLDSTATE_SHADOW_DODGER, 1);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (who->GetTypeId() != TYPEID_PLAYER)
                {
                    _EnterEvadeMode();
                    return;
                }

                _JustEngagedWith();
                Talk(SAY_AGGRO);
                DoCast(me, SPELL_AURA_OF_DESPAIR);
                events.ScheduleEvent(EVENT_SHADOW_CRASH, urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SEARING_FLAMES, 12*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_MARK_OF_THE_FACELESS, urand(35*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SUMMON_SARONITE_VAPOR, 30*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SURGE_OF_DARKNESS, 1*MINUTE*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BERSERK, 10*MINUTE*IN_MILLISECONDS);
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
                        case EVENT_SHADOW_CRASH:
                            if (Unit* target = CheckPlayersInRange(RAID_MODE(4, 9), 15.0f, 200.0f))
                                DoCast(target, SPELL_SHADOW_CRASH);
                            events.ScheduleEvent(EVENT_SHADOW_CRASH, urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS));
                            return;
                        case EVENT_SEARING_FLAMES:
                            DoCastAOE(SPELL_SEARING_FLAMES);
                            events.ScheduleEvent(EVENT_SEARING_FLAMES, urand(14*IN_MILLISECONDS, 17.5*IN_MILLISECONDS));
                            return;
                        case EVENT_MARK_OF_THE_FACELESS:
                            if (Unit* target = CheckPlayersInRange(RAID_MODE(4, 9), 15.0f, 200.0f))
                                DoCast(target, SPELL_MARK_OF_THE_FACELESS);
                            events.ScheduleEvent(EVENT_MARK_OF_THE_FACELESS, urand(35*IN_MILLISECONDS, 45*IN_MILLISECONDS));
                            return;
                        case EVENT_SURGE_OF_DARKNESS:
                            Talk(EMOTE_SURGE_OF_DARKNESS);
                            Talk(SAY_SURGE_OF_DARKNESS);
                            DoCast(me, SPELL_SURGE_OF_DARKNESS);
                            events.ScheduleEvent(EVENT_SURGE_OF_DARKNESS, urand(50*IN_MILLISECONDS, 70*IN_MILLISECONDS));
                            return;
                        case EVENT_SUMMON_SARONITE_VAPOR:
                            if (_vaporsSpawned >= 6)
                                return;
                            DoCast(me, SPELL_SUMMON_SARONITE_VAPORS, true);   // Spells summons 33488 in a random place in 40 meters
                            if (_vaporsAlive >= 6) // summons include both vapors and saronite animus, but since the animus was not spawned yet...
                            {
                                events.CancelEvent(EVENT_SUMMON_SARONITE_VAPOR);    // Should always be cancelled after six vapors got spawned
                                if (_notHardModed)                   // If animus was not spawned yet and no vapor got killed yet...
                                    if (Creature* bunny = me->FindNearestCreature(NPC_VEZAX_BUNNY, 100.0f))
                                        bunny->AI()->DoAction(ACTION_VAPORS_COMBINE);
                            }
                            else if (_vaporsSpawned < 6)
                                events.ScheduleEvent(EVENT_SUMMON_SARONITE_VAPOR, urand(30*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_BERSERK:
                            Talk(SAY_BERSERK);
                            DoCast(me, SPELL_BERSERK);
                            return;
                        default:
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void SpellHitTarget(Unit* who, SpellInfo const* spell) override
            {
                if (who && who->GetTypeId() == TYPEID_PLAYER && spell->Id == SPELL_SHADOW_CRASH_HIT)
                {
                    me->GetMap()->SetWorldState(WORLDSTATE_SHADOW_DODGER, 0);
                    _shadowDodger = false;
                }
            }

            void JustSummoned(Creature* summoned) override
            {
                summons.Summon(summoned);   // Placed here for the check below
                switch (summoned->GetEntry())
                {
                    case NPC_SARONITE_ANIMUS:
                    {
                        events.CancelEvent(EVENT_SEARING_FLAMES);
                        Talk(SAY_HARDMODE);
                        Talk(EMOTE_BARRIER);
                        me->InterruptNonMeleeSpells(false);
                        DoCast(SPELL_SARONITE_BARRIER);
                        me->AddLootMode(LOOT_MODE_HARD_MODE_1);
                        std::list<Creature*> saronitevapros;
                        GetCreatureListWithEntryInGrid(saronitevapros, me, NPC_SARONITE_VAPOR, 200);
                        for (auto&& vapros : saronitevapros)
                            if (vapros)
                                vapros->DespawnOrUnsummon();
                        break;
                    }
                    case NPC_SARONITE_VAPOR:
                        ++_vaporsSpawned;
                        ++_vaporsAlive;
                        break;
                    default:
                        break;
                }
                DoZoneInCombat(summoned);
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
            {
                switch (summon->GetEntry())
                {
                    case NPC_SARONITE_ANIMUS:
                        _notHardModed = false;
                        me->GetMap()->SetWorldState(WORLDSTATE_SMELL_OF_SARONITE, 1);
                        Talk(EMOTE_BARRIER_END);
                        me->RemoveAurasDueToSpell(SPELL_SARONITE_BARRIER);
                        events.ScheduleEvent(EVENT_SEARING_FLAMES, urand(7*IN_MILLISECONDS, 12*IN_MILLISECONDS));
                        break;
                    default:
                        break;
                }
                summons.Despawn(summon);
            }

            void KilledUnit(Unit* /*who*/) override
            {
                if (!urand(0,5))
                    Talk(SAY_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AURA_OF_DESPAIR);
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_SHADOWDODGER:
                        return _shadowDodger ? 1 : 0;
                    // Hardmode-condition: !_notHardModed <=> Saronite Animus dead; vaporSummonedCount>=6 <=> Saronite Animus summoned; !_vaporKilled <=> one or more vapors got killed
                    case DATA_SMELL_OF_SARONITE:
                        return !_notHardModed;
                    default:
                        break;
                }
                return 0;
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {   
                    case ACTION_VAPORS_DIE:
                        --_vaporsAlive;
                        break;
                    default:
                        break;
                }
            }

            /*  Player Range Check
                Purpose: If there are playersMin people within (rangeMin, rangeMax): return a random player in that range.
                If not, return a random target within 150.0f .
            */
            Unit* CheckPlayersInRange(uint8 playersMin, float rangeMin, float rangeMax)
            {
                Map* map = me->GetMap();
                if (map && map->IsDungeon())
                {
                    std::list<Player*> playerList;
                    for (auto&& itr : map->GetPlayers())
                    {
                        if (Player* player = itr.GetSource())
                        {
                            float distance = player->GetDistance(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                            if (rangeMin > distance || distance > rangeMax)
                                continue;

                            playerList.push_back(player);
                        }
                    }

                    if (playerList.empty() || playerList.size()<playersMin)
                        return SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true);

                    return Trinity::Containers::SelectRandomContainerElement(playerList);
                }
                return 0;
            }

            private:
                bool _shadowDodger;
                bool _notHardModed; // HardMode
                uint32 _vaporsSpawned;
                uint32 _vaporsAlive;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_general_vezaxAI>(creature);
        }
};

class boss_saronite_animus : public CreatureScript
{
    public:
        boss_saronite_animus() : CreatureScript("npc_saronite_animus") { }

        struct boss_saronite_animusAI : public ScriptedAI
        {
            boss_saronite_animusAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                Reset();
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->DespawnOrUnsummon();
            }

            void Reset() override
            {
                DoCast(me, SPELL_VERTEX_SHADE_BLACK);
                _profoundTimer = 3*IN_MILLISECONDS;

                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (_profoundTimer <= diff)
                {
                    DoCastAOE(SPELL_PROFOUND_OF_DARKNESS, true);
                    _profoundTimer = RAID_MODE(7*IN_MILLISECONDS, 3*IN_MILLISECONDS);
                }
                else
                    _profoundTimer -= diff;

                DoMeleeAttackIfReady();
            }

            private:
                InstanceScript* instance;
                uint32 _profoundTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_saronite_animusAI>(creature);
        }
};

class npc_saronite_vapors : public CreatureScript
{
    public:
        npc_saronite_vapors() : CreatureScript("npc_saronite_vapors") { }

        struct npc_saronite_vaporsAI : public ScriptedAI
        {
            npc_saronite_vaporsAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                Talk(EMOTE_VAPORS);
                instance = me->GetInstanceScript();
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_ID, 49560, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                me->SetReactState(REACT_PASSIVE);
                Reset();
            }

            void Reset() override
            {
                _randomMoveTimer = urand(3*IN_MILLISECONDS, 4.5*IN_MILLISECONDS);
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_VAPORS_COMBINE)
                {
                    _combining = true;
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance && instance->GetBossState(BOSS_VEZAX) != IN_PROGRESS)
                    me->DespawnOrUnsummon();

                if (_combining || me->HasAura(63323))
                    return;

                if (_randomMoveTimer <= diff)
                {
                    me->GetMotionMaster()->MoveRandom(30.0f);
                    _randomMoveTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
                }
                else
                    _randomMoveTimer -= diff;
            }

            SpellMissInfo SpellHitResult(Unit* attacker, SpellInfo const* spell, Spell const* /*spellInstance*/) override
            {
                return _combining ? SPELL_MISS_EVADE : SPELL_MISS_NONE;
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                if (_combining)
                {
                    damage = 0;
                    return;
                }

                // This can't be on JustDied. In 63322 dummy handler caster needs to be this NPC
                // if caster == target then damage mods will increase the damage taken
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    me->SetReactState(REACT_PASSIVE);
                    me->GetMotionMaster()->Clear(false);
                    me->GetMotionMaster()->MoveIdle();
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    me->SetHealth(me->GetMaxHealth());
                    me->RemoveAllAuras();
                    DoCast(me, SPELL_SARONITE_VAPORS);
                    me->DespawnOrUnsummon(30*IN_MILLISECONDS);

                    if (Creature* Vezax = me->GetCreature(*me, instance->GetGuidData(BOSS_VEZAX)))
                        Vezax->AI()->DoAction(ACTION_VAPORS_DIE);
                }
            }

            private:
                InstanceScript* instance;
                uint32 _randomMoveTimer;
                bool _combining = false;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_saronite_vaporsAI>(creature);
        }
};

class npc_vezax_bunny : public CreatureScript
{
    public:
        npc_vezax_bunny() : CreatureScript("npc_vezax_bunny") { }

        struct npc_vezax_bunnyAI : public ScriptedAI
        {
            npc_vezax_bunnyAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                _vapors = 0;
                me->RemoveAurasDueToSpell(SPELL_VISUAL_SARONITE_ANIMUS);
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_VAPORS_COMBINE)
                {
                    DoCast(me, SPELL_VISUAL_SARONITE_ANIMUS, true);

                    std::list<Creature*> vapors;
                    GetCreatureListWithEntryInGrid(vapors, me, NPC_SARONITE_VAPOR, 200.0f);
                    for (auto&& vapor : vapors)
                    {
                        vapor->AI()->DoAction(ACTION_VAPORS_COMBINE);
                        vapor->GetMotionMaster()->Clear();
                        vapor->GetMotionMaster()->MovePoint(0, *me, false);
                    }
                }
            }

            void JustSummoned(Creature* summon) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (Creature* vezax = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_VEZAX)))
                        vezax->AI()->JustSummoned(summon);
            }

            void SummonedCreatureDies(Creature* summon, Unit* killer) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (Creature* vezax = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_VEZAX)))
                        vezax->AI()->SummonedCreatureDies(summon, killer);
            }

            void UpdateAI(uint32 diff) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                {
                    if (instance->GetBossState(BOSS_VEZAX) != IN_PROGRESS)
                    {
                        if (_vapors || me->HasAura(SPELL_VISUAL_SARONITE_ANIMUS))
                            Reset();
                        return;
                    }
                }

                if (!me->HasAura(SPELL_VISUAL_SARONITE_ANIMUS))
                    return;

                if (Creature* vapor = me->FindNearestCreature(NPC_SARONITE_VAPOR, 5.0f))
                {
                    vapor->DespawnOrUnsummon();
                    if (++_vapors == 6)
                    {
                        DoCast(SPELL_SUMMON_SARONITE_ANIMUS);
                        Reset();
                    }
                }
            }

        private:
            uint32 _vapors;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_vezax_bunnyAI>(creature);
        }
};

enum FacelessHorrorEnums
{
    SPELL_DEATH_GRIP        = 64429,
    SPELL_SHADOW_CRASH_FH   = 63722,
    SPELL_VOID_WAVE         = 63703,
    SPELL_SUMMON_VOID_BEAST = 63708,
    SPELL_VOID_BARRIER      = 63710,

    EVENT_DEATH_GRIP = 1,
    EVENT_SHADOW_CRASH_FH,

    NPC_VOID_BEAST = 33806,
};

class npc_faceless_horror : public CreatureScript
{
    public:
        npc_faceless_horror() : CreatureScript("npc_faceless_horror") { }

        struct npc_faceless_horrorAI : public ScriptedAI
        {
            npc_faceless_horrorAI(Creature* creature) : ScriptedAI(creature), summons(creature) { }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
                percentagesHandled[0] = false;
                percentagesHandled[1] = false;
                percentagesHandled[2] = false;
            }

            void JustEngagedWith(Unit* who) override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_DEATH_GRIP, urand(6000, 8000));
                events.ScheduleEvent(EVENT_SHADOW_CRASH_FH, urand(2000, 4000));
            }

            void JustDied(Unit* killer) override
            {
                summons.DespawnAll();
            }

            void DamageTaken(Unit* who, uint32& damage) override
            {
                if (!percentagesHandled[0] && me->HealthBelowPctDamaged(75, damage))
                    Summon(0);
                if (!percentagesHandled[1] && me->HealthBelowPctDamaged(50, damage))
                    Summon(1);
                if (!percentagesHandled[2] && me->HealthBelowPctDamaged(25, damage))
                    Summon(2);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDies(Creature* summon, Unit* killer) override
            {
                summons.Despawn(summon);
                if (summon->GetEntry() == NPC_VOID_BEAST)
                    me->RemoveAurasDueToSpell(SPELL_VOID_BARRIER);
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
                        case EVENT_DEATH_GRIP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, SpellTargetSelector(me, SPELL_DEATH_GRIP)))
                                DoCast(target, SPELL_DEATH_GRIP);
                            events.ScheduleEvent(EVENT_DEATH_GRIP, urand(6000, 8000));
                            break;
                        case EVENT_SHADOW_CRASH_FH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, SpellTargetSelector(me, SPELL_SHADOW_CRASH_FH)))
                                DoCast(target, SPELL_SHADOW_CRASH_FH);
                            events.ScheduleEvent(EVENT_SHADOW_CRASH_FH, urand(8000, 10000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            SummonList summons;
            bool percentagesHandled[3];

            void Summon(uint8 index)
            {
                percentagesHandled[index] = true;
                DoCast(me, SPELL_SUMMON_VOID_BEAST, true);
                DoCast(me, SPELL_VOID_BARRIER, true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_faceless_horrorAI>(creature);
        }
};

class spell_aura_of_despair_aura : public SpellScriptLoader // Spell 62692
{
    public:
        spell_aura_of_despair_aura() : SpellScriptLoader("spell_aura_of_despair_aura") { }

        class spell_aura_of_despair_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_aura_of_despair_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_AURA_OF_DESPAIR_EFFECT_DESPAIR))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_CORRUPTED_RAGE))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
                    return;

                Player* target = GetTarget()->ToPlayer();

                if (target->GetClass() == CLASS_SHAMAN && target->HasSpell(SPELL_SHAMANTIC_RAGE))
                    target->CastSpell(target, SPELL_CORRUPTED_RAGE, true);

                target->CastSpell(target, SPELL_AURA_OF_DESPAIR_EFFECT_DESPAIR, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
                    return;

                Player* target = GetTarget()->ToPlayer();

                target->RemoveAurasDueToSpell(SPELL_CORRUPTED_RAGE);
                target->RemoveAurasDueToSpell(SPELL_AURA_OF_DESPAIR_EFFECT_DESPAIR);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_aura_of_despair_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PREVENT_REGENERATE_POWER, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_aura_of_despair_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PREVENT_REGENERATE_POWER, AURA_EFFECT_HANDLE_REAL);
            }

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_aura_of_despair_AuraScript();
        }
};

class spell_mark_of_the_faceless : public SpellScriptLoader
{
    public:
        spell_mark_of_the_faceless() : SpellScriptLoader("spell_mark_of_the_faceless") { }

        class spell_mark_of_the_faceless_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mark_of_the_faceless_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_MARK_OF_THE_FACELESS_DAMAGE))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* aurEff)
            {
                if (Unit* caster = GetCaster())
                    caster->CastCustomSpell(SPELL_MARK_OF_THE_FACELESS_DAMAGE, SPELLVALUE_BASE_POINT1, aurEff->GetAmount(), GetTarget(), true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mark_of_the_faceless_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mark_of_the_faceless_AuraScript();
        }
};

class spell_mark_of_the_faceless_drain : public SpellScriptLoader // 63278
{
    public:
        spell_mark_of_the_faceless_drain() : SpellScriptLoader("spell_mark_of_the_faceless_drain") { }

        class spell_mark_of_the_faceless_drain_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mark_of_the_faceless_drain_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove(GetExplTargetUnit()); // The target of this spell should _not_ be in this list
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mark_of_the_faceless_drain_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mark_of_the_faceless_drain_SpellScript();
        }
};

class spell_death_grip : public SpellScriptLoader
{
   public:
       spell_death_grip() : SpellScriptLoader("spell_death_grip") { }

       class spell_death_grip_SpellScript : public SpellScript
       {
           PrepareSpellScript(spell_death_grip_SpellScript);

           void HandleDummy(SpellEffIndex effIndex)
           {
                if (WorldLocation const* dest = GetExplTargetDest())
                    if (Unit* target = GetHitUnit())
                        target->CastSpell(dest->GetPositionX(), dest->GetPositionY(), dest->GetPositionZ(), GetEffectValue(), true);
            }

           void Register() override
           {
               OnEffectHitTarget += SpellEffectFn(spell_death_grip_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
           }
       };

       SpellScript* GetSpellScript() const override
       {
           return new spell_death_grip_SpellScript();
       }
};

void AddSC_boss_general_vezax()
{
    new boss_general_vezax();
    new boss_saronite_animus();
    new npc_saronite_vapors();
    new npc_vezax_bunny();
    new npc_faceless_horror();

    new spell_aura_of_despair_aura();
    new spell_mark_of_the_faceless_drain();
    new spell_mark_of_the_faceless();
    new spell_death_grip();
}
