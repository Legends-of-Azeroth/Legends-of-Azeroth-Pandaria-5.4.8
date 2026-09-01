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
#include "ahnkahet.h"
#include "Random.h"

enum Spells
{
    SPELL_INSANITY                              = 57496,
    SPELL_SUMMON_TWISTED_VISAGE_PHASE_1         = 57500,
    SPELL_SUMMON_TWISTED_VISAGE_PHASE_2         = 57501,
    SPELL_SUMMON_TWISTED_VISAGE_PHASE_3         = 57502,
    SPELL_SUMMON_TWISTED_VISAGE_PHASE_4         = 57503,
    SPELL_SUMMON_TWISTED_VISAGE_PHASE_5         = 57504,
    SPELL_SUMMON_TWISTED_VISAGE_SPAWN           = 57506,
    SPELL_SUMMON_TWISTED_VISAGE_SPAWN_EFFECT    = 57507,
    SPELL_INSANITY_1                            = 57508,
    SPELL_INSANITY_2                            = 57509,
    SPELL_INSANITY_3                            = 57510,
    SPELL_INSANITY_4                            = 57511,
    SPELL_INSANITY_5                            = 57512,
    SPELL_INSANITY_SWITCH                       = 57538,
    SPELL_TWISTED_VISAGE_VISUAL_PASSIVE         = 57551,
    SPELL_TWISTED_VISAGE_DEATH_VISUAL           = 57555,
    SPELL_CLEAR_ALL_INSANITY                    = 57558,
    SPELL_INSANITY_CHANNEL_VISUAL               = 57561,
    SPELL_MIND_FLAY                             = 57941,
    SPELL_SHADOW_BOLT_VOLLEY                    = 57942,
    SPELL_SHIVER                                = 57949,
    SPELL_SUMMON_TWISTED_VISAGE_SPAWN_2         = 59982,
    SPELL_VOLAZJ_WHISPER_AGGRO                  = 60291,
    SPELL_VOLAZJ_WHISPER_INSANITY               = 60292,
    SPELL_VOLAZJ_WHISPER_SLAY_01                = 60293,
    SPELL_VOLAZJ_WHISPER_SLAY_02                = 60294,
    SPELL_VOLAZJ_WHISPER_SLAY_03                = 60295,
    SPELL_VOLAZJ_WHISPER_DEATH_01               = 60296,
    SPELL_VOLAZJ_WHISPER_DEATH_02               = 60297,

    // Twisted Visage
    SPELL_AVENGERS_SHIELD                       = 57799,
    SPELL_BLOOD_PLAGUE                          = 57601,
    SPELL_BLOODTHIRST                           = 57790,
    SPELL_CAT_FORM                              = 57655,
    SPELL_CONSECRATION                          = 57798,
    SPELL_CORRUPTION                            = 57645,
    SPELL_DEATH_GRIP                            = 49560,
    SPELL_DEVASTATE                             = 57795,
    SPELL_DISENGAGE                             = 57635,
    SPELL_EARTH_SHIELD                          = 57802,
    SPELL_EARTH_SHOCK                           = 57783,
    SPELL_EVISCERATE                            = 57641,
    SPELL_FIREBALL                              = 57628,
    SPELL_FROST_NOVA                            = 57629,
    SPELL_GREATER_HEAL                          = 57775,
    SPELL_HAMSTRING                             = 9080,
    SPELL_HEALING_WAVE                          = 57785,
    SPELL_INTERCEPT                             = 61490,
    SPELL_JUDGEMENT_OF_LIGHT                    = 57774,
    SPELL_LIFEBLOOM                             = 57762,
    SPELL_LIGHTNING_BOLT                        = 57781,
    SPELL_MANGLE                                = 57657,
    SPELL_MIND_FLAY_1                           = 57779,
    SPELL_MOONFIRE                              = 57647,
    SPELL_MORTAL_STRIKE                         = 57789,
    SPELL_NOURISH                               = 57765,
    SPELL_PLAGUE_STRIKE                         = 57599,
    SPELL_RENEW                                 = 57777,
    SPELL_SHADOW_WORD                           = 57778,
    SPELL_SUNDER_ARMOR                          = 57807,
    SPELL_THUNDER_CLAP                          = 57832,
    SPELL_RIP                                   = 57661,
    SPELL_WRATH                                 = 57648,
    SPELL_SEAL_OF_COMMAND                       = 57769,
    SPELL_SHADOW_BOLT                           = 57644,
    SPELL_THUNDERSTORM                          = 57784,
    SPELL_SHOOT                                 = 57589,
    SPELL_SINISTER_STRIKE                       = 57640,
};

enum VolazjCreatures
{
    NPC_TWISTED_VISAGE_1                        = 30621,
    NPC_TWISTED_VISAGE_2                        = 30622,
    NPC_TWISTED_VISAGE_3                        = 30623,
    NPC_TWISTED_VISAGE_4                        = 30624,
    NPC_TWISTED_VISAGE_5                        = 30625,
};

enum Yells
{
    SAY_AGGRO                                     = 0,
    SAY_AGGRO_W                                   = 1,
    SAY_INSANITY                                  = 2,
    SAY_INSANITY_W                                = 3,
    SAY_DEATH_1                                   = 4,
    SAY_DEATH_W_1                                 = 5,
    SAY_DEATH_2                                   = 6,
    SAY_DEATH_W_2                                 = 7,
    SAY_SLAY_1                                    = 8,
    SAY_SLAY_W_1                                  = 9,
    SAY_SLAY_2                                    = 10,
    SAY_SLAY_W_2                                  = 11,
    SAY_SLAY_3                                    = 12,
    SAY_SLAY_W_3                                  = 13,
};

enum Achievements
{
    ACHIEV_QUICK_DEMISE_START_EVENT               = 20382,
};

Spells SummonTwistedVisageSpells[] =
{
    SPELL_SUMMON_TWISTED_VISAGE_PHASE_1,
    SPELL_SUMMON_TWISTED_VISAGE_PHASE_2,
    SPELL_SUMMON_TWISTED_VISAGE_PHASE_3,
    SPELL_SUMMON_TWISTED_VISAGE_PHASE_4,
    SPELL_SUMMON_TWISTED_VISAGE_PHASE_5,
};

Spells InsanitySpells[] =
{
    SPELL_INSANITY_1,
    SPELL_INSANITY_2,
    SPELL_INSANITY_3,
    SPELL_INSANITY_4,
    SPELL_INSANITY_5,
};

std::map<VolazjCreatures, Spells> const TwistedVisageToInsanity
{
    { NPC_TWISTED_VISAGE_1, SPELL_INSANITY_1 },
    { NPC_TWISTED_VISAGE_2, SPELL_INSANITY_2 },
    { NPC_TWISTED_VISAGE_3, SPELL_INSANITY_3 },
    { NPC_TWISTED_VISAGE_4, SPELL_INSANITY_4 },
    { NPC_TWISTED_VISAGE_5, SPELL_INSANITY_5 },
};

class boss_volazj : public CreatureScript
{
    public:
        boss_volazj() : CreatureScript("boss_volazj") { }

        struct boss_volazjAI : public ScriptedAI
        {
            boss_volazjAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                scheduler
                    .CancelAll()
                    .SetValidator([this] { return !me->HasUnitState(UNIT_STATE_CASTING); })
                    .Schedule(Seconds(8), [this](TaskContext mindFlay)
                    {
                        DoCastVictim(SPELL_MIND_FLAY);
                        mindFlay.Repeat(Seconds(20));
                    })
                    .Schedule(Seconds(5), [this](TaskContext shadowBoltVolley)
                    {
                        DoCastVictim(SPELL_SHADOW_BOLT_VOLLEY);
                        shadowBoltVolley.Repeat();
                    })
                    .Schedule(Seconds(15), [this](TaskContext shiver)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            DoCast(target, SPELL_SHIVER);
                        shiver.Repeat();
                    });

                instance->SetData(DATA_HERALD_VOLAZJ, NOT_STARTED);
                instance->DoStopTimedAchievement(CRITERIA_START_TYPE_EVENT, ACHIEV_QUICK_DEMISE_START_EVENT);

                // Visible for all players in insanity
                me->SetPhaseMask((1|16|32|64|128|256),true);
                // Used for Insanity handling
                insanityHandled = 0;

                // Cleanup
                summons.DespawnAll();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                DoCastAOE(SPELL_CLEAR_ALL_INSANITY, true);
                me->RemoveAllAuras();
                me->SetControlled(false, UNIT_STATE_STUNNED);
                _EnterEvadeMode();
                me->GetMotionMaster()->MoveTargetedHome();
                Reset();
            }

            void JustEngagedWith(Unit* who) override
            {
                Talk(SAY_AGGRO);
                DoCastAOE(SPELL_VOLAZJ_WHISPER_AGGRO, true);

                instance->SetData(DATA_HERALD_VOLAZJ, IN_PROGRESS);
                instance->DoStartCriteria(CRITERIA_START_TYPE_EVENT, ACHIEV_QUICK_DEMISE_START_EVENT);
            }

            void JustDied(Unit* killer) override
            {
                switch (urand(0, 1))
                {
                    case 0: Talk(SAY_DEATH_1); DoCastAOE(SPELL_VOLAZJ_WHISPER_DEATH_01, true); break;
                    case 1: Talk(SAY_DEATH_2); DoCastAOE(SPELL_VOLAZJ_WHISPER_DEATH_02, true); break;
                }

                instance->SetData(DATA_HERALD_VOLAZJ, DONE);

                summons.DespawnAll();

                DoCastAOE(SPELL_CLEAR_ALL_INSANITY, true);
            }

            void KilledUnit(Unit* victim) override
            {
                switch (urand(0, 2))
                {
                    case 0: Talk(SAY_SLAY_1); DoCastAOE(SPELL_VOLAZJ_WHISPER_SLAY_01, true); break;
                    case 1: Talk(SAY_SLAY_2); DoCastAOE(SPELL_VOLAZJ_WHISPER_SLAY_02, true); break;
                    case 2: Talk(SAY_SLAY_3); DoCastAOE(SPELL_VOLAZJ_WHISPER_SLAY_03, true); break;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                switch (summon->GetEntry())
                {
                    case NPC_TWISTED_VISAGE_1:
                    case NPC_TWISTED_VISAGE_2:
                    case NPC_TWISTED_VISAGE_3:
                    case NPC_TWISTED_VISAGE_4:
                    case NPC_TWISTED_VISAGE_5:
                        summons.Summon(summon);
                        break;
                }
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);

                if (!me->IsInCombat())
                    return;

                if (summons.empty())
                {
                    DoCastAOE(SPELL_CLEAR_ALL_INSANITY, true);

                    insanityHandled = 0;
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetControlled(false, UNIT_STATE_STUNNED);
                    me->RemoveAurasDueToSpell(SPELL_INSANITY_CHANNEL_VISUAL);
                }
                else if (!summons.HasEntry(summon->GetEntry()))
                    DoCastAOE(SPELL_INSANITY_SWITCH, true);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_INSANITY:
                        // Not good target or too many players
                        if (target->GetTypeId() != TYPEID_PLAYER || insanityHandled > 4)
                            return;

                        // First target - start channel visual and set self as unnattackable
                        if (!insanityHandled)
                        {
                            Talk(SAY_INSANITY);
                            DoCastAOE(SPELL_VOLAZJ_WHISPER_INSANITY, true);
                            // Channel visual
                            DoCast(me, SPELL_INSANITY_CHANNEL_VISUAL, true);
                            // Unattackable
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            me->SetControlled(true, UNIT_STATE_STUNNED);
                        }

                        // phase mask
                        target->CastSpell(target, InsanitySpells[insanityHandled], true);

                        // summon twisted party members for this target
                        for (auto&& ref : me->GetMap()->GetPlayers())
                            if (Player* player = ref.GetSource())
                                if (player->IsAlive() && player != target)
                                    player->CastSpell(player, SummonTwistedVisageSpells[insanityHandled], true);

                        ++insanityHandled;
                        break;
                    case SPELL_INSANITY_SWITCH:
                        for (auto&& pair : TwistedVisageToInsanity)
                        {
                            if (target->HasAura(pair.second) && !summons.HasEntry(pair.first))
                            {
                                target->RemoveAurasDueToSpell(pair.second);
                                if (Creature* visage = ObjectAccessor::GetCreature(*me, summons.front()))
                                {
                                    auto itr = TwistedVisageToInsanity.find((VolazjCreatures)visage->GetEntry());
                                    ASSERT(itr != TwistedVisageToInsanity.end());
                                    target->CastSpell(target, itr->second, true);
                                }
                            }
                        }
                        break;
                    case SPELL_CLEAR_ALL_INSANITY:
                        for (auto&& spell : InsanitySpells)
                            target->RemoveAurasDueToSpell(spell);
                        break;
                    case SPELL_VOLAZJ_WHISPER_AGGRO:    Talk(SAY_AGGRO_W,    target); break;
                    case SPELL_VOLAZJ_WHISPER_INSANITY: Talk(SAY_INSANITY_W, target); break;
                    case SPELL_VOLAZJ_WHISPER_SLAY_01:  Talk(SAY_SLAY_W_1,   target); break;
                    case SPELL_VOLAZJ_WHISPER_SLAY_02:  Talk(SAY_SLAY_W_2,   target); break;
                    case SPELL_VOLAZJ_WHISPER_SLAY_03:  Talk(SAY_SLAY_W_3,   target); break;
                    case SPELL_VOLAZJ_WHISPER_DEATH_01: Talk(SAY_DEATH_W_1,  target); break;
                    case SPELL_VOLAZJ_WHISPER_DEATH_02: Talk(SAY_DEATH_W_2,  target); break;
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                    damage = 0;

                if (!me->HealthBelowPctDamaged(66, 0) && me->HealthBelowPctDamaged(66, damage) ||
                    !me->HealthBelowPctDamaged(33, 0) && me->HealthBelowPctDamaged(33, damage))
                {
                    me->InterruptNonMeleeSpells(false);
                    DoCast(me, SPELL_INSANITY, false);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (insanityHandled)
                    return;

                scheduler.Update(diff);

                DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* instance = me->GetInstanceScript();
            SummonList summons { me };
            TaskScheduler scheduler;
            uint32 insanityHandled;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_volazjAI(creature);
        }
};

class npc_twisted_visage : public CreatureScript
{
    public:

        npc_twisted_visage() : CreatureScript("npc_twisted_visage") { }

        struct npc_twisted_visageAI : public ScriptedAI
        {
            npc_twisted_visageAI(Creature* creature) : ScriptedAI(creature)
            {
                switch (me->GetEntry())
                {
                    case NPC_TWISTED_VISAGE_1: me->SetPhaseMask( 16, false); break;
                    case NPC_TWISTED_VISAGE_2: me->SetPhaseMask( 32, false); break;
                    case NPC_TWISTED_VISAGE_3: me->SetPhaseMask( 64, false); break;
                    case NPC_TWISTED_VISAGE_4: me->SetPhaseMask(128, false); break;
                    case NPC_TWISTED_VISAGE_5: me->SetPhaseMask(256, false); break;
                }
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (Creature* volazj = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_HERALD_VOLAZJ)))
                        volazj->AI()->JustSummoned(me);

                DoCast(summoner, SPELL_SUMMON_TWISTED_VISAGE_SPAWN, true);

                if (Player* player = summoner->ToPlayer())
                {
                    _class = player->GetClass();

                    if (_class == CLASS_DRUID && player->HasAura(768) || player->HasAura(9634))
                        DoCast(me, SPELL_CAT_FORM);

                    if (_class == CLASS_PALADIN && roll_chance_i(50))
                        DoCast(me, SPELL_SEAL_OF_COMMAND);
                }
            }

            void Unsummoned() override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (Creature* volazj = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_HERALD_VOLAZJ)))
                        volazj->AI()->SummonedCreatureDespawn(me);
            }

            void Reset() override
            {
                spellTimer = urand(2000, 7000);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->DespawnOrUnsummon();
            }

            void JustDied(Unit* killer) override
            {
                DoCast(me, SPELL_TWISTED_VISAGE_DEATH_VISUAL, true);
                me->DespawnOrUnsummon(1000);
            }

            void JustEngagedWith(Unit* who) override { }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_SUMMON_TWISTED_VISAGE_SPAWN:
                        target->CastSpell(me, SPELL_SUMMON_TWISTED_VISAGE_SPAWN_EFFECT, true);
                        break;
                }
            }

             void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                    if (spellTimer <= diff)
                    {
                        switch (_class)
                        {
                            case CLASS_MAGE:
                                DoCastVictim(RAND(SPELL_FIREBALL, SPELL_FROST_NOVA));
                                break;
                            case CLASS_WARLOCK:
                                DoCastVictim(RAND(SPELL_CORRUPTION, SPELL_SHADOW_BOLT));
                                break;
                            case CLASS_SHAMAN:
                                DoCastVictim(RAND(SPELL_BLOODTHIRST, SPELL_EARTH_SHIELD, SPELL_EARTH_SHOCK, SPELL_HEALING_WAVE, SPELL_LIGHTNING_BOLT, SPELL_THUNDERSTORM));
                                break;
                            case CLASS_DEATH_KNIGHT:
                                DoCastVictim(RAND(SPELL_BLOOD_PLAGUE, SPELL_DEATH_GRIP, SPELL_PLAGUE_STRIKE));
                                break;
                            case CLASS_PALADIN:
                                DoCastVictim(RAND(SPELL_AVENGERS_SHIELD, SPELL_JUDGEMENT_OF_LIGHT, SPELL_GREATER_HEAL));
                                break;
                            case CLASS_HUNTER:
                                DoCastVictim(RAND(SPELL_DISENGAGE, SPELL_SHOOT));
                                break;
                            case CLASS_ROGUE:
                                DoCastVictim(RAND(SPELL_EVISCERATE, SPELL_SINISTER_STRIKE));
                                break;
                            case CLASS_PRIEST:
                                DoCastVictim(RAND(SPELL_RENEW, SPELL_MIND_FLAY_1, SPELL_CONSECRATION, SPELL_SHADOW_WORD));
                                break;
                            case CLASS_WARRIOR:
                                DoCastVictim(RAND(SPELL_INTERCEPT, SPELL_DEVASTATE, SPELL_MORTAL_STRIKE, SPELL_HAMSTRING, SPELL_SUNDER_ARMOR, SPELL_THUNDER_CLAP));
                                break;
                            case CLASS_DRUID:
                                if (me->GetAura(SPELL_CAT_FORM))
                                    DoCastVictim(RAND(SPELL_RIP, SPELL_MANGLE));
                                else
                                    DoCastVictim(RAND(SPELL_LIFEBLOOM, SPELL_MOONFIRE, SPELL_NOURISH, SPELL_WRATH));
                                break;
                        }
                        spellTimer = urand(5000, 7000);
                }
                else
                    spellTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            uint32 _class;
            uint32 spellTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_twisted_visageAI(creature);
        }

};

class spell_insanity : public SpellScriptLoader
{
    public:
        spell_insanity() : SpellScriptLoader("spell_insanity") { }

        class spell_insanity_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_insanity_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                // Don't stall the fight since solo players will have nobody to kill inside Insanity phases
                if (targets.size() == 1)
                    FinishCast(SPELL_FAILED_DONT_REPORT);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_insanity_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_insanity_SpellScript();
        }
};

void AddSC_boss_volazj()
{
    new boss_volazj;
    new npc_twisted_visage;
    new spell_insanity();
}
