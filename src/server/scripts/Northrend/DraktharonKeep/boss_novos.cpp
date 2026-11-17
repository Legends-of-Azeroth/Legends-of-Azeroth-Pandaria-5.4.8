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
#include "SpellScript.h"
#include "CreatureTextMgr.h"
#include "ScriptedCreature.h"
#include "drak_tharon_keep.h"

enum Yells
{
    SAY_AGGRO                       = 0,
    SAY_SUMMONING_ADDS              = 1,
    SAY_ARCANE_FIELD                = 2,
    SAY_DEATH                       = 3,
    SAY_KILL                        = 4,
    EMOTE_SUMMONING_ADDS            = 5
};

enum Spells
{
    SPELL_BEAM_CHANNEL              = 52106,
    SPELL_CLEAR_ALL_DEBUFFS         = 34098,
    SPELL_ARCANE_BLAST              = 49198,
    SPELL_ARCANE_FIELD              = 47346,
    SPELL_DESPAWN_CRYSTAL_HANDLERS  = 51403,

    SPELL_SUMMON_RISEN_SHADOWCASTER = 49105,
    SPELL_SUMMON_FETID_TROLL_CORPSE = 49103,
    SPELL_SUMMON_HULKING_CORPSE     = 49104,
    SPELL_SUMMON_CRYSTAL_HANDLER    = 49179,
    SPELL_SUMMON_COPY_OF_MINIONS    = 59933,

    SPELL_BLIZZARD                  = 49034,
    SPELL_FROSTBOLT                 = 49037,
    SPELL_WRATH_OF_MISERY           = 50089,
    SPELL_SUMMON_MINIONS            = 59910,

    // Risen Shadowcaster
    SPELL_SHADOW_BOLT               = 51363,

    // Hulking Corpse
    SPELL_THUNDERCLAP               = 55635,
    SPELL_THUNDERCLAP_H             = 58975,

    // Crystal Handler
    SPELL_FLASH_OF_DARKNESS         = 49668,
    SPELL_CRYSTAL_HANDLER_DEATH_1   = 47336,
    SPELL_CRYSTAL_HANDLER_DEATH_2   = 55801,
    SPELL_CRYSTAL_HANDLER_DEATH_3   = 55803,
    SPELL_CRYSTAL_HANDLER_DEATH_4   = 55805,
};

enum Misc
{
    ACTION_RESET_CRYSTALS,
    ACTION_ACTIVATE_CRYSTAL,
    ACTION_DEACTIVATE,

    EVENT_ATTACK,

    EVENT_WRATH_OF_MISERY,
    EVENT_BLIZZARD,
    EVENT_SUMMON_MINIONS,

    DATA_NOVOS_ACHIEV,
    DATA_CRYSTAL_SPELL,
    DATA_CRYSTAL_LIMIT,
    DATA_CRYSTAL_TIMER_MIN,
    DATA_CRYSTAL_TIMER_MAX,
};

struct SummonerInfo
{
    uint32 data, spell, limit, timerMin, timerMax;
};

const SummonerInfo summoners[] =
{
    { DATA_NOVOS_SUMMONER_1, SPELL_SUMMON_FETID_TROLL_CORPSE, 0,  2000, 11000 },
    { DATA_NOVOS_SUMMONER_2, SPELL_SUMMON_RISEN_SHADOWCASTER, 0,  7000, 20000 },
    { DATA_NOVOS_SUMMONER_3, SPELL_SUMMON_HULKING_CORPSE,     0, 45000, 47000 },
    { DATA_NOVOS_SUMMONER_4, SPELL_SUMMON_CRYSTAL_HANDLER,    4, 16000, 16000 },
};

uint32 const crystalHandlerDeathSpells[4] = { SPELL_CRYSTAL_HANDLER_DEATH_1, SPELL_CRYSTAL_HANDLER_DEATH_2, SPELL_CRYSTAL_HANDLER_DEATH_3, SPELL_CRYSTAL_HANDLER_DEATH_4 };

// not from sniff, but approximation based on their position when they appear in sniffs
Position const crystalHandlerSummonPositions[2] =
{
    { -308.525208f, -703.471680f, 27.291899f, 0.0f },
    { -422.668640f, -706.116821f, 28.575941f, 0.0f },
};

#define MAX_Y_COORD_OH_NOVOS        -771.95f

class boss_novos : public CreatureScript
{
    public:
        boss_novos() : CreatureScript("boss_novos") { }

        struct boss_novosAI : public BossAI
        {
            boss_novosAI(Creature* creature) : BossAI(creature, DATA_NOVOS) { }

            void Reset() override
            {
                _Reset();

                DoCastAOE(SPELL_DESPAWN_CRYSTAL_HANDLERS, true);

                events.ScheduleEvent(EVENT_WRATH_OF_MISERY, 6000);
                events.ScheduleEvent(EVENT_BLIZZARD, 12000);
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SUMMON_MINIONS, 15000);

                autoCastTimer.SetInterval(2500);

                me->GetMap()->SetWorldState(WORLDSTATE_OH_NOVOS, 1);
                _crystalHandlerCount = 0;
                SetCrystalsStatus(false);
                SetSummonerStatus(false);
                SetBubbled(false);
            }

            void JustEngagedWith(Unit* /*victim*/) override
            {
                _JustEngagedWith();
                Talk(SAY_AGGRO);

                SetCrystalsStatus(true);
                SetSummonerStatus(true);
                SetBubbled(true);
            }

            void AttackStart(Unit* victim) override
            {
                if (victim && me->Attack(victim, true))
                    casterMovement.Chase(victim);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_DESPAWN_CRYSTAL_HANDLERS)
                    if (target->GetTypeId() == TYPEID_UNIT)
                        target->ToCreature()->DespawnOrUnsummon();
            }

            bool CanAIAttack(Unit const* who) const override
            {
                return me->IsWithinDistInMap(who, 100.0f, false); // 100 yds is blizzlike
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || _bubbled)
                    return;

                events.Update(diff);
                autoCastTimer.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                casterMovement.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_BLIZZARD:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                            DoCast(target, SPELL_BLIZZARD);
                        events.ScheduleEvent(EVENT_BLIZZARD, me->HasUnitState(UNIT_STATE_CASTING) ? urand(13000, 20000) : 1000);
                        break;
                    case EVENT_WRATH_OF_MISERY:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                            DoCast(target, SPELL_WRATH_OF_MISERY);
                        events.ScheduleEvent(EVENT_WRATH_OF_MISERY, me->HasUnitState(UNIT_STATE_CASTING) ? urand(8000, 16000) : 1000);
                        break;
                    case EVENT_SUMMON_MINIONS:
                        DoCast(me, SPELL_SUMMON_MINIONS);
                        events.ScheduleEvent(EVENT_SUMMON_MINIONS, 45000);
                        break;
                    default:
                        if (autoCastTimer.Passed())
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50, true))
                                DoCast(target, SPELL_FROSTBOLT);
                            autoCastTimer.Reset();
                        }
                        break;
                }

                if (casterMovement.IsUnableToCast())
                    DoMeleeAttackIfReady();
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_CRYSTAL_HANDLER_DIED)
                    CrystalHandlerDied();
            }

            void MoveInLineOfSight(Unit* who) override
            {
                BossAI::MoveInLineOfSight(who);

                if (me->GetMap()->GetWorldState(WORLDSTATE_OH_NOVOS) == 0 || !who || who->GetTypeId() != TYPEID_UNIT || who->GetPositionY() < MAX_Y_COORD_OH_NOVOS)
                    return;

                uint32 entry = who->GetEntry();
                if (entry == NPC_HULKING_CORPSE || entry == NPC_RISEN_SHADOWCASTER || entry == NPC_FETID_TROLL_CORPSE)
                    if (!who->ToTempSummon() || who->ToTempSummon()->GetSummoner() != me)
                        me->GetMap()->SetWorldState(WORLDSTATE_OH_NOVOS, 0);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_CRYSTAL_HANDLER)
                {
                    Talk(EMOTE_SUMMONING_ADDS, summon);
                    Talk(SAY_SUMMONING_ADDS, summon);
                }
            }

        private:
            CasterMovement casterMovement = CasterMovement(me).Spell(SPELL_BLIZZARD).Spell(SPELL_FROSTBOLT).Spell(SPELL_WRATH_OF_MISERY);
            IntervalTimer autoCastTimer;
            uint8 _crystalHandlerCount;
            bool _bubbled;

            void SetBubbled(bool state)
            {
                _bubbled = state;
                if (!state)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        me->CastStop();
                }
                else
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                    DoCast(me, SPELL_CLEAR_ALL_DEBUFFS);
                    DoCast(me, SPELL_ARCANE_BLAST);
                    DoCast(me, SPELL_ARCANE_FIELD);
                }
            }

            void SetSummonerStatus(bool active)
            {
                for (auto&& summoner : summoners)
                    if (ObjectGuid guid = instance->GetGuidData(summoner.data))
                        if (Creature* crystalChannelTarget = ObjectAccessor::GetCreature(*me, guid))
                        {
                            if (active)
                            {
                                crystalChannelTarget->AI()->SetData(DATA_CRYSTAL_SPELL, summoner.spell);
                                crystalChannelTarget->AI()->SetData(DATA_CRYSTAL_LIMIT, summoner.limit);
                                crystalChannelTarget->AI()->SetData(DATA_CRYSTAL_TIMER_MIN, summoner.timerMin);
                                crystalChannelTarget->AI()->SetData(DATA_CRYSTAL_TIMER_MAX, summoner.timerMax);
                            }
                            else
                                crystalChannelTarget->AI()->Reset();
                        }
            }

            void SetCrystalsStatus(bool active)
            {
                for (uint8 i = 0; i < 4; i++)
                    if (ObjectGuid guid = instance->GetGuidData(DATA_NOVOS_CRYSTAL_1 + i))
                        if (GameObject* crystal = ObjectAccessor::GetGameObject(*me, guid))
                            SetCrystalStatus(crystal, active);
            }

            void SetCrystalStatus(GameObject* crystal, bool active)
            {
                crystal->SetGoState(active ? GO_STATE_ACTIVE : GO_STATE_READY);
                if (Creature* crystalChannelTarget = crystal->FindNearestCreature(NPC_CRYSTAL_CHANNEL_TARGET, 5.0f))
                {
                    if (active)
                        crystalChannelTarget->CastSpell((Unit*)NULL, SPELL_BEAM_CHANNEL);
                    else if (crystalChannelTarget->HasUnitState(UNIT_STATE_CASTING))
                        crystalChannelTarget->CastStop();
                }
            }

            void CrystalHandlerDied()
            {
                for (uint8 i = 0; i < 4; i++)
                    if (ObjectGuid guid = instance->GetGuidData(DATA_NOVOS_CRYSTAL_1 + i))
                        if (GameObject* crystal = ObjectAccessor::GetGameObject(*me, guid))
                            if (crystal->GetGoState() == GO_STATE_ACTIVE)
                            {
                                SetCrystalStatus(crystal, false);
                                break;
                            }

                if (++_crystalHandlerCount >= 4)
                {
                    Talk(SAY_ARCANE_FIELD);
                    SetSummonerStatus(false);
                    SetBubbled(false);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_novosAI(creature);
        }
};

class npc_crystal_channel_target : public CreatureScript
{
    public:
        npc_crystal_channel_target() : CreatureScript("npc_crystal_channel_target") { }

        struct npc_crystal_channel_targetAI : public ScriptedAI
        {
            npc_crystal_channel_targetAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                spell = 0;
                limit = 0;
                timerMin = 0;
                timerMax = 0;
                timer = 0;
                count = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (spell)
                {
                    if (timer <= diff)
                    {
                        DoCast(me, spell, true);
                        timer = urand(timerMin, timerMax);
                    }
                    else
                        timer -= diff;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                return count;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_CRYSTAL_SPELL: spell = data; break;
                    case DATA_CRYSTAL_LIMIT: limit = data; break;
                    case DATA_CRYSTAL_TIMER_MIN: timerMin = data; if (timerMin && timerMax && timerMin <= timerMax) timer = urand(timerMin, timerMax); break;
                    case DATA_CRYSTAL_TIMER_MAX: timerMax = data; if (timerMin && timerMax && timerMin <= timerMax) timer = urand(timerMin, timerMax); break;
                    default: break;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (ObjectGuid guid = instance->GetGuidData(DATA_NOVOS))
                        if (Creature* novos = ObjectAccessor::GetCreature(*me, guid))
                            novos->AI()->JustSummoned(summon);

                if (spell == SPELL_SUMMON_CRYSTAL_HANDLER)
                {
                    if (count < 4)
                        summon->AI()->SetData(0, crystalHandlerDeathSpells[count]);

                    DoZoneInCombat(summon, 100.0f);
                    summon->GetMotionMaster()->Clear();
                    summon->GetMotionMaster()->MovePath(summon->GetEntry() * 100 + count % 2, false);
                }
                else
                    summon->GetMotionMaster()->MovePath(summon->GetEntry() * 100, false);

                if (limit && ++count >= limit)
                {
                    spell = 0;
                    timerMin = 0;
                    timerMax = 0;
                    timer = 0;
                }
            }

         private:
            uint32 spell;
            uint32 limit;
            uint32 timerMin;
            uint32 timerMax;
            uint32 timer;
            uint32 count = 0;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_crystal_channel_targetAI(creature);
        }
};

class npc_novos_minion : public CreatureScript
{
    public:
        npc_novos_minion() : CreatureScript("npc_novos_minion") { }

        struct npc_novos_minionAI : public ScriptedAI
        {
            npc_novos_minionAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                events.Reset();
                if (me->GetEntry() == NPC_RISEN_SHADOWCASTER)
                    events.ScheduleEvent(EVENT_ATTACK, urand(2000, 5000));
                else
                    events.ScheduleEvent(EVENT_ATTACK, urand(5000, 10000));
            }

            void JustDied(Unit* killer) override
            {
                if (deathSpell)
                    DoCast(me, deathSpell, true);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == WAYPOINT_MOTION_TYPE && pointId == 2 && me->GetEntry() == NPC_CRYSTAL_HANDLER)
                {
                    me->m_Events.Schedule(1, [this]()
                    {
                        if (!me->IsInCombat())
                            return;

                        if (me->GetVictim())
                            me->GetMotionMaster()->MoveChase(me->GetVictim());
                        else
                            DoZoneInCombat(me, 100.0f);
                    });
                }
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (me->IsWithinDist(who, 35) && !me->IsInCombat() && me->CanStartAttack(who, false))
                    AttackStart(who);
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (me->GetEntry() == NPC_CRYSTAL_HANDLER)
                    deathSpell = data;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == EVENT_ATTACK)
                {
                    switch (me->GetEntry())
                    {
                        case NPC_RISEN_SHADOWCASTER:
                            DoCastVictim(SPELL_SHADOW_BOLT);
                            events.ScheduleEvent(EVENT_ATTACK, urand(4000, 13000));
                            break;
                        case NPC_HULKING_CORPSE:
                            DoCastAOE(DUNGEON_MODE(SPELL_THUNDERCLAP, SPELL_THUNDERCLAP_H));
                            events.ScheduleEvent(EVENT_ATTACK, 46000);
                            break;
                        case NPC_CRYSTAL_HANDLER:
                            DoCastVictim(SPELL_FLASH_OF_DARKNESS);
                            events.ScheduleEvent(EVENT_ATTACK, urand(6000, 18000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            uint32 deathSpell = 0;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_novos_minionAI(creature);
        }
};

class spell_novos_summon_minions : public SpellScriptLoader
{
    public:
        spell_novos_summon_minions() : SpellScriptLoader("spell_novos_summon_minions") { }

        class spell_novos_summon_minions_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_novos_summon_minions_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_COPY_OF_MINIONS))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                for (uint8 i = 0; i < 2; ++i)
                    GetCaster()->CastSpell((Unit*)NULL, SPELL_SUMMON_COPY_OF_MINIONS, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_novos_summon_minions_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_novos_summon_minions_SpellScript();
        }
};

class spell_summon_crystal_handler : public SpellScriptLoader
{
    public:
        spell_summon_crystal_handler() : SpellScriptLoader("spell_summon_crystal_handler") { }

        class spell_summon_crystal_handler_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_summon_crystal_handler_SpellScript);

            void SelectDest(SpellDestination& pos)
            {
                if (Unit* caster = GetCaster())
                {
                    if (caster->GetTypeId() == TYPEID_UNIT && caster->IsAIEnabled)
                    {
                        Position const& dest = crystalHandlerSummonPositions[caster->ToCreature()->AI()->GetData(0) % 2];
                        pos._position.m_positionX += dest.GetPositionX() - caster->GetPositionX();
                        pos._position.m_positionY += dest.GetPositionY() - caster->GetPositionY();
                        pos._position.m_positionZ += dest.GetPositionZ() - caster->GetPositionZ();
                    }
                }
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_summon_crystal_handler_SpellScript::SelectDest, EFFECT_0, TARGET_DEST_CASTER_RADIUS);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_summon_crystal_handler_SpellScript();
        }
};

void AddSC_boss_novos()
{
    new boss_novos();
    new npc_crystal_channel_target();
    new npc_novos_minion();
    new spell_novos_summon_minions();
    new spell_summon_crystal_handler();
}
