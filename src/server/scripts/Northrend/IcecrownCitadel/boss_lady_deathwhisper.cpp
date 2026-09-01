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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Group.h"
#include "icecrown_citadel.h"
#include "PlayerAI.h"
#include "Transport.h"
#include "Random.h"

enum Yells
{
    // Lady Deathwhisper
    SAY_INTRO_1                 = 0,
    SAY_INTRO_2                 = 1,
    SAY_INTRO_3                 = 2,
    SAY_INTRO_4                 = 3,
    SAY_INTRO_5                 = 4,
    SAY_INTRO_6                 = 5,
    SAY_INTRO_7                 = 6,
    SAY_AGGRO                   = 7,
    SAY_PHASE_2                 = 8,
    EMOTE_PHASE_2               = 9,
    SAY_DOMINATE_MIND           = 10,
    SAY_DARK_EMPOWERMENT        = 11,
    SAY_DARK_TRANSFORMATION     = 12,
    SAY_ANIMATE_DEAD            = 13,
    SAY_KILL                    = 14,
    SAY_BERSERK                 = 15,
    SAY_DEATH                   = 16,

    // Darnavan
    SAY_DARNAVAN_AGGRO          = 0,
    SAY_DARNAVAN_RESCUED        = 1,
};

enum Spells
{
    // Lady Deathwhisper
    SPELL_SHADOW_CHANNELING           = 43897, // Prefight, during intro
    SPELL_MANA_BARRIER                = 70842,
    SPELL_DEATH_AND_DECAY             = 71001,
    SPELL_DOMINATE_MIND               = 71289,
    SPELL_SHADOW_BOLT                 = 71254,
    SPELL_DARK_TRANSFORMATION_T       = 70895,
    SPELL_DARK_EMPOWERMENT_T          = 70896,
    SPELL_DARK_MARTYRDOM_T            = 70897,
    SPELL_FROSTBOLT                   = 71420,
    SPELL_FROSTBOLT_VOLLEY            = 72905,
    SPELL_TOUCH_OF_INSIGNIFICANCE     = 71204,
    SPELL_SUMMON_SHADE                = 71363,

    // Achievement
    SPELL_FULL_HOUSE                  = 72827, // Does not exist in dbc but still can be used for criteria check

    // Both Adds
    SPELL_TELEPORT_VISUAL             = 41236,

    // Fanatics
    SPELL_NECROTIC_STRIKE             = 70659,
    SPELL_SHADOW_CLEAVE               = 70670,
    SPELL_VAMPIRIC_MIGHT              = 70674,
    SPELL_DARK_TRANSFORMATION         = 70900,
    SPELL_FANATIC_DETERMINATION       = 71235,
    SPELL_DARK_MARTYRDOM_FANATIC      = 71236,

    // Adherents
    SPELL_FROST_FEVER                 = 67767,
    SPELL_DEATHCHILL_BOLT             = 70594,
    SPELL_DEATHCHILL_BLAST            = 70906,
    SPELL_CURSE_OF_TORPOR             = 71237,
    SPELL_SHORUD_OF_THE_OCCULT        = 70768,
    SPELL_DARK_EMPOWERMENT            = 70901,
    SPELL_ADHERENT_DETERMINATION      = 71234,
    SPELL_DARK_MARTYRDOM_ADHERENT     = 70903,

    // Vengeful Shade
    SPELL_VENGEFUL_BLAST_T            = 71494,
    SPELL_VENGEFUL_BLAST              = 71544,

    // Darnavan
    SPELL_BLADESTORM                  = 65947,
    SPELL_CHARGE                      = 65927,
    SPELL_INTIMIDATING_SHOUT          = 65930,
    SPELL_MORTAL_STRIKE               = 65926,
    SPELL_SHATTERING_THROW            = 65940,
    SPELL_SUNDER_ARMOR                = 65936,

    // Dominate Mind player's spells
    SPELL_DISPELL_MAGIC               = 988,   // Priest
    TALENT_DISPERSION                 = 47585,
    SPELL_PSYCHIC_SCREAM              = 10890,
    SPELL_MIND_BLAST                  = 48127,
    SPELL_MIND_FLAY                   = 48156,
    SPELL_FLASH_HEAL                  = 48071,
    TALENT_BERSERK                    = 50334, // Druid
    SPELL_DIRE_BEAR_FORM              = 9634,
    SPELL_MANGLE_BEAR                 = 48564,
    SPELL_CAT_FORM                    = 768,
    SPELL_MANGLE_CAT                  = 48566,
    SPELL_TREE_OF_LIFE                = 33891,
    SPELL_NOURISH                     = 50464,
    SPELL_MOONKIN_FORM                = 24858,
    SPELL_STARFALL                    = 53201,
    SPELL_WRATH                       = 48461,
    SPELL_BEAR_FORM                   = 5487,
    SPELL_CAT_TRAVEL_FORM             = 783,
    SPELL_CYCLONE                     = 33786,
    SPELL_FROST_TRAP                  = 13809, // Hunter
    SPELL_EXPLOSIVE_TRAP              = 49066,
    SPELL_DETERRENCE                  = 19263,
    TALENT_AIMED_SHOT                 = 19434,
    SPELL_AIMED_SHOT                  = 49050,
    SPELL_AUTO_SHOT                   = 75,
    TALENT_BEACON_OF_LIGHT            = 53563, // Paladin
    SPELL_FLASH_OF_LIGHT              = 48785,
    SPELL_LAY_ON_HANDS                = 27154,
    TALENT_AVENGERS_SHIELD            = 31935,
    SPELL_HUMMER_OF_JUSTICE           = 10308,
    SPELL_AVENGERS_SHIELD             = 48827,
    SPELL_FORBEARANCE                 = 25771,
    SPELL_DIVINE_SHIELD               = 642,
    SPELL_JUDGEMENT_OF_LIGHT          = 20271,
    TALENT_BLADESTORM                 = 46924, // Warrior
    SPELL_MORTAL_STRIKE_PLAYER        = 47486,
    TALENT_SHOCKWAVE                  = 46968,
    SPELL_SHOCKWAVE                   = 46968,
    SPELL_CLEAVE                      = 47520,
    SPELL_BERSERKER_STANCE            = 2458,
    SPELL_SHADOW_BOLT_PLAYER          = 47809, // Warlock
    SPELL_FEAR                        = 6215,
    SPELL_FROSTBOLT_PLAYER            = 42842, // Mage
    SPELL_CONE_OF_COLD                = 27087,
    SPELL_POLYMORPH                   = 12826,
    SPELL_CLOAK_OF_SHADOWS            = 31224, // Rogue
    SPELL_BLIND                       = 2094,
    SPELL_FAN_OF_KNIVES               = 51723,
    SPELL_SINISTER_STRIKE             = 48638,
    SPELL_DEATH_COIL                  = 49895, // Death Knight
    SPELL_DEATH_GRIP                  = 49560,
    SPELL_DEATH_AND_DECAY_PLAYER      = 49938,
    TALENT_RIPTIDE                    = 61295, // Shaman
    SPELL_CHAIN_HEAL                  = 55459,
    SPELL_CHAIN_LIGHTNING             = 25442,
    TALENT_THUNDERSTORM               = 51490,
    SPELL_LIGHTNING_BOLT              = 49238,
    SPELL_STORMSTRIKE                 = 17364,
    TALENT_FERAL_SPIRIT               = 51533
};

enum Events
{
    // Lady Deathwhisper
    EVENT_INTRO_2                       = 1,
    EVENT_INTRO_3                       = 2,
    EVENT_INTRO_4                       = 3,
    EVENT_INTRO_5                       = 4,
    EVENT_INTRO_6                       = 5,
    EVENT_INTRO_7                       = 6,
    EVENT_BERSERK                       = 7,
    EVENT_DEATH_AND_DECAY               = 8,
    EVENT_DOMINATE_MIND                 = 9,
    EVENT_DOMINATE_MIND_ACTION          = 91,

    // Phase 1 only
    EVENT_P1_SUMMON_WAVE                = 10,
    EVENT_P1_SHADOW_BOLT                = 11,
    EVENT_P1_EMPOWER_CULTIST            = 12,
    EVENT_P1_REANIMATE_CULTIST          = 13,

    // Phase 2 only
    EVENT_P2_SUMMON_WAVE                = 14,
    EVENT_P2_FROSTBOLT                  = 15,
    EVENT_P2_FROSTBOLT_VOLLEY           = 16,
    EVENT_P2_TOUCH_OF_INSIGNIFICANCE    = 17,
    EVENT_P2_SUMMON_SHADE               = 18,
    
    // Cult Fanatic
    EVENT_FANATIC_NECROTIC_STRIKE       = 20,
    EVENT_FANATIC_SHADOW_CLEAVE         = 21,
    EVENT_FANATIC_VAMPIRIC_MIGHT        = 22,

    // Cult Adherent
    //EVENT_ADHERENT_FROST_FEVER          = 23,
    //EVENT_ADHERENT_DEATHCHILL           = 24,
    EVENT_ADHERENT_CURSE_OF_TORPOR      = 25,
    EVENT_ADHERENT_SHORUD_OF_THE_OCCULT = 26,

    // Darnavan
    EVENT_DARNAVAN_BLADESTORM           = 27,
    EVENT_DARNAVAN_CHARGE               = 28,
    EVENT_DARNAVAN_INTIMIDATING_SHOUT   = 29,
    EVENT_DARNAVAN_MORTAL_STRIKE        = 30,
    EVENT_DARNAVAN_SHATTERING_THROW     = 31,
    EVENT_DARNAVAN_SUNDER_ARMOR         = 32,

    EVENT_SHADE_SUICIDE                 = 33,
};

enum Phases
{
    PHASE_ALL        = 0,
    PHASE_INTRO      = 1,
    PHASE_ONE        = 2,
    PHASE_TWO        = 3,
};

enum MiscData
{
    NPC_DARNAVAN_10         = 38472,
    NPC_DARNAVAN_25         = 38485,
    NPC_DARNAVAN_CREDIT_10  = 39091,
    NPC_DARNAVAN_CREDIT_25  = 39092,

    ACTION_COMPLETE_QUEST   = -384720,
    POINT_DESPAWN           = 384721,

    DATA_READY_TO_EMPOWER   = 100,
    DATA_READY_TO_REANIMATE = 200,
    DATA_REANIMATION_DONE   = 300
};

enum Actions
{
    ACTION_START_INTRO
};

#define NPC_DARNAVAN RAID_MODE<uint32>(NPC_DARNAVAN_10, NPC_DARNAVAN_25, NPC_DARNAVAN_10, NPC_DARNAVAN_25)
#define NPC_DARNAVAN_CREDIT RAID_MODE<uint32>(NPC_DARNAVAN_CREDIT_10, NPC_DARNAVAN_CREDIT_25, NPC_DARNAVAN_CREDIT_10, NPC_DARNAVAN_CREDIT_25)
#define QUEST_DEPROGRAMMING RAID_MODE<uint32>(QUEST_DEPROGRAMMING_10, QUEST_DEPROGRAMMING_25, QUEST_DEPROGRAMMING_10, QUEST_DEPROGRAMMING_25)

uint32 const SummonEntries[2] = { NPC_CULT_FANATIC, NPC_CULT_ADHERENT };
#define GUID_CULTIST    1

Position const SummonPositions[7] =
{
    { -578.7066f, 2154.167f, 51.01529f, 1.692969f }, // 1 Left Door 1 (Cult Fanatic)
    { -598.9028f, 2155.005f, 51.01530f, 1.692969f }, // 2 Left Door 2 (Cult Adherent)
    { -619.2864f, 2154.460f, 51.01530f, 1.692969f }, // 3 Left Door 3 (Cult Fanatic)
    { -578.6996f, 2269.856f, 51.01529f, 4.590216f }, // 4 Right Door 1 (Cult Adherent)
    { -598.9688f, 2269.264f, 51.01529f, 4.590216f }, // 5 Right Door 2 (Cult Fanatic)
    { -619.4323f, 2268.523f, 51.01530f, 4.590216f }, // 6 Right Door 3 (Cult Adherent)
    { -524.2480f, 2211.920f, 62.90960f, 3.141592f }, // 7 Upper (Random Cultist)
};

class boss_lady_deathwhisper : public CreatureScript
{
    public:
        boss_lady_deathwhisper() : CreatureScript("boss_lady_deathwhisper") { }

        struct boss_lady_deathwhisperAI : public BossAI
        {
            boss_lady_deathwhisperAI(Creature* creature) : BossAI(creature, DATA_LADY_DEATHWHISPER), _dominateMindCount(RAID_MODE<uint8>(0, 1, 1, 3)), _introDone(false), _darnavanGUID() { }

            void Reset() override
            {
                _Reset();
                _dominateMindTargets[0] = nullptr;
                _dominateMindTargets[1] = nullptr;
                _dominateMindTargets[2] = nullptr;
                me->SetPower(POWER_MANA, me->GetMaxPower(POWER_MANA));
                events.SetPhase(PHASE_ONE);
                _waveCounter = 0;
                _nextVengefulShadeTargetGUID = ObjectGuid::Empty;
                _lastEmpoweredCultistGUID = ObjectGuid::Empty;
                _lastReanimatedCultistGUID = ObjectGuid::Empty;
                if (Creature* darnavan = ObjectAccessor::GetCreature(*me, _darnavanGUID))
                {
                    darnavan->DespawnOrUnsummon();
                    _darnavanGUID = ObjectGuid::Empty;
                }
                DoCast(me, SPELL_SHADOW_CHANNELING);
                me->RemoveAurasDueToSpell(SPELL_BERSERK);
                me->RemoveAurasDueToSpell(SPELL_MANA_BARRIER);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                // Apply DK's Death Grip immune
                me->ApplySpellImmune(0, IMMUNITY_ID, 49560, true);
                me->m_SightDistance = 120.0f;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (me->GetExactDist(who) <= 15.0f)
                    BossAI::MoveInLineOfSight(who);
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_START_INTRO:
                        if (!_introDone && me->IsAlive())
                        {
                            _introDone = true;
                            Talk(SAY_INTRO_1);
                            events.SetPhase(PHASE_INTRO);
                            events.ScheduleEvent(EVENT_INTRO_2, 11000, 0, PHASE_INTRO);
                            events.ScheduleEvent(EVENT_INTRO_3, 21000, 0, PHASE_INTRO);
                            events.ScheduleEvent(EVENT_INTRO_4, 31500, 0, PHASE_INTRO);
                            events.ScheduleEvent(EVENT_INTRO_5, 39500, 0, PHASE_INTRO);
                            events.ScheduleEvent(EVENT_INTRO_6, 48500, 0, PHASE_INTRO);
                            events.ScheduleEvent(EVENT_INTRO_7, 58000, 0, PHASE_INTRO);
                        }
                        break;
                    default:
                        break;
                }
            }

            void AttackStart(Unit* victim) override
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (victim && me->Attack(victim, true) && !events.IsInPhase(PHASE_ONE))
                    me->GetMotionMaster()->MoveChase(victim);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!instance->CheckRequiredBosses(DATA_LADY_DEATHWHISPER, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(LIGHT_S_HAMMER_TELEPORT);
                    return;
                }

                me->setActive(true);
                DoZoneInCombat();

                events.Reset();
                events.SetPhase(PHASE_ONE);
                // Phase-independent events
                events.ScheduleEvent(EVENT_BERSERK, 10 * MINUTE * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_DEATH_AND_DECAY, 10 * IN_MILLISECONDS);
                // Phase one only
                events.ScheduleEvent(EVENT_P1_SUMMON_WAVE, 5 * IN_MILLISECONDS, 0, PHASE_ONE);
                events.ScheduleEvent(EVENT_P1_SHADOW_BOLT, urand(5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS), 0, PHASE_ONE);
                events.ScheduleEvent(EVENT_P1_EMPOWER_CULTIST, urand(20 * IN_MILLISECONDS, 30 * IN_MILLISECONDS), 0, PHASE_ONE);
                events.ScheduleEvent(EVENT_P1_REANIMATE_CULTIST, urand(18 * IN_MILLISECONDS, 32 * IN_MILLISECONDS), 0, PHASE_ONE);

                if (GetDifficulty() != RAID_DIFFICULTY_10MAN_NORMAL)
                    events.ScheduleEvent(EVENT_DOMINATE_MIND, 27 * IN_MILLISECONDS);

                Talk(SAY_AGGRO);
                DoStartNoMovement(who);
                me->RemoveAurasDueToSpell(SPELL_SHADOW_CHANNELING);
                DoCast(me, SPELL_MANA_BARRIER, true);

                instance->SetBossState(DATA_LADY_DEATHWHISPER, IN_PROGRESS);
            }

            void JustDied(Unit* killer) override
            {
                Talk(SAY_DEATH);

                std::set<uint32> livingAddEntries;
                // Full House achievement
                for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                    if (Unit* unit = ObjectAccessor::GetUnit(*me, *itr))
                        if (unit->IsAlive() && unit->GetEntry() != NPC_VENGEFUL_SHADE && unit->GetEntry() != NPC_EMPOWERED_ADHERENT)
                            livingAddEntries.insert(unit->GetEntry());

                if (livingAddEntries.size() >= 5)
                    instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_FULL_HOUSE, 0, me);

                if (Creature* darnavan = ObjectAccessor::GetCreature(*me, _darnavanGUID))
                {
                    if (darnavan->IsAlive())
                    {
                        darnavan->SetFaction(35);
                        darnavan->CombatStop(true);
                        darnavan->GetMotionMaster()->MoveIdle();
                        darnavan->SetReactState(REACT_PASSIVE);
                        darnavan->m_Events.Schedule(10 * IN_MILLISECONDS, [darnavan]() { darnavan->GetMotionMaster()->MovePoint(POINT_DESPAWN, SummonPositions[6]); });
                        darnavan->AI()->Talk(SAY_DARNAVAN_RESCUED);
                        if (Player* owner = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
                        {
                            if (Group* group = owner->GetGroup())
                            {
                                for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
                                    if (Player* member = itr->GetSource())
                                        member->KilledMonsterCredit(NPC_DARNAVAN_CREDIT, ObjectGuid::Empty);
                            }
                            else
                                owner->KilledMonsterCredit(NPC_DARNAVAN_CREDIT, ObjectGuid::Empty);
                        }
                    }
                }

                _JustDied();
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                instance->SetBossState(DATA_LADY_DEATHWHISPER, FAIL);

                summons.DespawnAll();
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void DamageTaken(Unit* /*damageDealer*/, uint32& damage) override
            {
                // Phase transition
                if (events.IsInPhase(PHASE_ONE) && (int32)damage > me->GetPower(POWER_MANA))
                {
                    Talk(SAY_PHASE_2);
                    Talk(EMOTE_PHASE_2);
                    DoStartMovement(me->GetVictim());
                    damage -= me->GetPower(POWER_MANA);
                    me->SetPower(POWER_MANA, 0);
                    me->RemoveAurasDueToSpell(SPELL_MANA_BARRIER);
                    events.SetPhase(PHASE_TWO);
                    events.ScheduleEvent(EVENT_P2_FROSTBOLT, urand(10 * IN_MILLISECONDS, 12 * IN_MILLISECONDS), 0, PHASE_TWO);
                    events.ScheduleEvent(EVENT_P2_FROSTBOLT_VOLLEY, urand(19 * IN_MILLISECONDS, 21 * IN_MILLISECONDS), 0, PHASE_TWO);
                    events.ScheduleEvent(EVENT_P2_TOUCH_OF_INSIGNIFICANCE, urand(6 * IN_MILLISECONDS, 9 * IN_MILLISECONDS), 0, PHASE_TWO);
                    events.ScheduleEvent(EVENT_P2_SUMMON_SHADE, urand(12 * IN_MILLISECONDS, 15 * IN_MILLISECONDS), 0, PHASE_TWO);
                    // On heroic mode Lady Deathwhisper is immune to taunt effects in phase 2 and continues summoning adds
                    if (IsHeroic())
                    {
                        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                        events.ScheduleEvent(EVENT_P2_SUMMON_WAVE, 45 * IN_MILLISECONDS, 0, PHASE_TWO);
                    }
                    DoResetThreat();
                }
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_DARNAVAN)
                    _darnavanGUID = summon->GetGUID();
                else
                    summons.Summon(summon);

                Unit* target = nullptr;
                if (summon->GetEntry() == NPC_VENGEFUL_SHADE)
                {
                    target = ObjectAccessor::GetUnit(*me, _nextVengefulShadeTargetGUID);
                    summon->AddThreat(target, 100000000.0f);
                    _nextVengefulShadeTargetGUID = ObjectGuid::Empty;
                }
                else
                    target = SelectTarget(SELECT_TARGET_RANDOM);

                if (target && summon->AI())
                {
                    summon->AI()->AttackStart(target);
                    DoZoneInCombat(summon, 150.0f);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if ((!UpdateVictim() && !events.IsInPhase(PHASE_INTRO)) || !CheckInRoom())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) && !events.IsInPhase(PHASE_INTRO))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_2:
                            Talk(SAY_INTRO_2);
                            break;
                        case EVENT_INTRO_3:
                            Talk(SAY_INTRO_3);
                            break;
                        case EVENT_INTRO_4:
                            Talk(SAY_INTRO_4);
                            break;
                        case EVENT_INTRO_5:
                            Talk(SAY_INTRO_5);
                            break;
                        case EVENT_INTRO_6:
                            Talk(SAY_INTRO_6);
                            break;
                        case EVENT_INTRO_7:
                            Talk(SAY_INTRO_7);
                            break;
                        case EVENT_DEATH_AND_DECAY:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_DEATH_AND_DECAY);

                            events.ScheduleEvent(EVENT_DEATH_AND_DECAY, urand(22 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                            break;
                        case EVENT_DOMINATE_MIND:
                            Talk(SAY_DOMINATE_MIND);
                            for (uint8 i = 0; i < _dominateMindCount; i++)
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, events.IsInPhase(PHASE_ONE) ? 0 : 1, 0.0f, true, -SPELL_DOMINATE_MIND))
                                {
                                    DoCast(target, SPELL_DOMINATE_MIND);
                                    _dominateMindTargets[i] = target;
                                }

                            events.ScheduleEvent(EVENT_DOMINATE_MIND, urand(40 * IN_MILLISECONDS, 45 * IN_MILLISECONDS));
                            events.ScheduleEvent(EVENT_DOMINATE_MIND_ACTION, 2 * IN_MILLISECONDS);
                            break;
                        case EVENT_DOMINATE_MIND_ACTION:
                        {
                            bool _dominateMindInProgress = false;

                            for (uint8 i = 0; i < _dominateMindCount; i++)
                            {
                                if (!_dominateMindTargets[i])
                                    continue;

                                Player* _dominateMindTarget = _dominateMindTargets[i]->ToPlayer();
                                if (_dominateMindTarget && _dominateMindTarget->IsAlive())
                                {
                                    if (_dominateMindTarget->HasAura(SPELL_DOMINATE_MIND))
                                    {
                                        ApplyDominateMindAction(_dominateMindTarget);
                                        _dominateMindInProgress = true;
                                        continue;
                                    }
                                    me->SetInCombatWith(_dominateMindTarget);
                                    _dominateMindTarget->SetInCombatWith(me);
                                    me->AddThreat(_dominateMindTarget, 0.0f);
                                }
                                _dominateMindTargets[i] = nullptr;
                            }

                            if (_dominateMindInProgress)
                                events.ScheduleEvent(EVENT_DOMINATE_MIND_ACTION, urand(1 * IN_MILLISECONDS, 2 * IN_MILLISECONDS));

                            break;
                        }
                        case EVENT_P1_SUMMON_WAVE:
                            SummonWaveP1();
                            events.ScheduleEvent(EVENT_P1_SUMMON_WAVE, (IsHeroic() ? 45 : 60) * IN_MILLISECONDS, 0, PHASE_ONE);
                            break;
                        case EVENT_P1_SHADOW_BOLT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_SHADOW_BOLT);

                            events.ScheduleEvent(EVENT_P1_SHADOW_BOLT, urand(5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS), 0, PHASE_ONE);
                            return;
                        case EVENT_P1_EMPOWER_CULTIST:
                        {
                            Creature* empoweringCultist = GetRandomReadyCultist(DATA_READY_TO_EMPOWER);
                            if (empoweringCultist)
                            {
                                _lastEmpoweredCultistGUID = empoweringCultist->GetGUID();
                                Talk(empoweringCultist->GetEntry() == NPC_CULT_FANATIC ? SAY_DARK_TRANSFORMATION : SAY_DARK_EMPOWERMENT);
                                DoCast(empoweringCultist, empoweringCultist->GetEntry() == NPC_CULT_FANATIC ? SPELL_DARK_TRANSFORMATION_T : SPELL_DARK_EMPOWERMENT_T, true);
                            }
                            events.ScheduleEvent(EVENT_P1_EMPOWER_CULTIST, urand(12 * IN_MILLISECONDS, 22 * IN_MILLISECONDS), 0, PHASE_ONE);
                            break;
                        }
                        case EVENT_P1_REANIMATE_CULTIST:
                        {
                            Creature* reanimatingCultist = GetRandomReadyCultist(DATA_READY_TO_REANIMATE);
                            if (reanimatingCultist)
                            {
                                _lastReanimatedCultistGUID = reanimatingCultist->GetGUID();
                                DoCast(reanimatingCultist, SPELL_DARK_MARTYRDOM_T, true);
                            }
                            events.ScheduleEvent(EVENT_P1_REANIMATE_CULTIST, urand(14 * IN_MILLISECONDS, 20 * IN_MILLISECONDS), 0, PHASE_ONE);
                            break;
                        }
                        case EVENT_P2_FROSTBOLT:
                            DoCastVictim(SPELL_FROSTBOLT);
                            events.ScheduleEvent(EVENT_P2_FROSTBOLT, urand(10 * IN_MILLISECONDS, 11 * IN_MILLISECONDS), 0, PHASE_TWO);
                            return;
                        case EVENT_P2_FROSTBOLT_VOLLEY:
                            DoCastAOE(SPELL_FROSTBOLT_VOLLEY);
                            events.ScheduleEvent(EVENT_P2_FROSTBOLT_VOLLEY, urand(13 * IN_MILLISECONDS, 15 * IN_MILLISECONDS), 0, PHASE_TWO);
                            break;
                        case EVENT_P2_TOUCH_OF_INSIGNIFICANCE:
                            DoCastVictim(SPELL_TOUCH_OF_INSIGNIFICANCE);
                            events.ScheduleEvent(EVENT_P2_TOUCH_OF_INSIGNIFICANCE, urand(9 * IN_MILLISECONDS, 13 * IN_MILLISECONDS), 0, PHASE_TWO);
                            break;
                        case EVENT_P2_SUMMON_SHADE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                            {
                                _nextVengefulShadeTargetGUID = target->GetGUID();
                                DoCast(target, SPELL_SUMMON_SHADE);
                            }
                            events.ScheduleEvent(EVENT_P2_SUMMON_SHADE, urand(18 * IN_MILLISECONDS, 23 * IN_MILLISECONDS), 0, PHASE_TWO);
                            break;
                        case EVENT_P2_SUMMON_WAVE:
                            SummonWaveP2();
                            events.ScheduleEvent(EVENT_P2_SUMMON_WAVE, 45 * IN_MILLISECONDS, 0, PHASE_TWO);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            Talk(SAY_BERSERK);
                            break;
                        default:
                            break;
                    }
                }

                if (!me->HasAura(SPELL_MANA_BARRIER))
                    DoMeleeAttackIfReady(true);
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_REANIMATION_DONE)
                    if (data == NPC_REANIMATED_FANATIC || data == NPC_REANIMATED_ADHERENT)
                        Talk(SAY_ANIMATE_DEAD);
            }

            void ApplyDominateMindAction(Player* player)
            {
                if (_useInternalPlayerAI)
                    return;

                if (player->HasUnitState(UNIT_STATE_CASTING)) 
                    return;

                Unit* victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true, -SPELL_DOMINATE_MIND);
                Creature* ally = GetRandomSummonOrBoss();
                switch (player->GetClass())
                {
                    case CLASS_PRIEST:
                        if (player->HasTalent(TALENT_DISPERSION, player->GetActiveSpec()))
                        {
                            if (roll_chance_i(30))
                                player->CastSpell(victim, SPELL_DISPELL_MAGIC, false);
                            else if (roll_chance_i(20) && !player->HasSpellCooldown(SPELL_PSYCHIC_SCREAM))
                                player->CastSpell(player, SPELL_PSYCHIC_SCREAM, false);
                            else if (!player->HasSpellCooldown(SPELL_MIND_BLAST))
                                player->CastSpell(victim, SPELL_MIND_BLAST, false);
                            else
                                player->CastSpell(victim, SPELL_MIND_FLAY, false);
                        }
                        else
                        {
                            if (roll_chance_i(30))
                                player->CastSpell(ally, SPELL_DISPELL_MAGIC, false);
                            else
                                player->CastSpell(ally, SPELL_FLASH_HEAL, false);
                        }
                        break;
                    case CLASS_DRUID:
                        if (player->HasTalent(TALENT_BERSERK, player->GetActiveSpec()))
                        {
                            if (player->HasAura(SPELL_DIRE_BEAR_FORM))
                            {
                                if (!player->HasSpellCooldown(SPELL_MANGLE_BEAR))
                                    player->CastSpell(player->GetVictim(), SPELL_MANGLE_BEAR, false);
                            }
                            else if (player->HasAura(SPELL_CAT_FORM))
                            {
                                if (!player->HasSpellCooldown(SPELL_MANGLE_CAT))
                                    player->CastSpell(player->GetVictim(), SPELL_MANGLE_CAT, false);
                            }
                        }
                        else if (player->HasAura(SPELL_TREE_OF_LIFE))
                            player->CastSpell(ally, SPELL_NOURISH, false);
                        else if (player->HasAura(SPELL_MOONKIN_FORM))
                        {
                            if (!player->HasSpellCooldown(SPELL_STARFALL))
                                player->CastSpell(victim, SPELL_STARFALL, false);
                            else
                                player->CastSpell(victim, SPELL_WRATH, false);
                        }
                        else
                        {
                            player->RemoveAurasDueToSpell(SPELL_BEAR_FORM);
                            player->RemoveAurasDueToSpell(SPELL_DIRE_BEAR_FORM);
                            player->RemoveAurasDueToSpell(SPELL_CAT_FORM);
                            player->RemoveAurasDueToSpell(SPELL_CAT_TRAVEL_FORM);
                            if (roll_chance_i(30))
                            {
                                if (Unit* cycloneTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true, -SPELL_DOMINATE_MIND))
                                    player->CastSpell(cycloneTarget, SPELL_CYCLONE, false);
                            }
                            else
                                player->CastSpell(victim, SPELL_WRATH, false);
                        }
                        break;
                    case CLASS_HUNTER:
                        if (roll_chance_i(50) && !player->HasSpellCooldown(SPELL_FROST_TRAP) && !player->HasSpellCooldown(SPELL_EXPLOSIVE_TRAP))
                            player->CastSpell(player, RAND<uint32>(SPELL_FROST_TRAP, SPELL_EXPLOSIVE_TRAP), false);
                        else if (roll_chance_i(30) && !player->HasSpellCooldown(SPELL_DETERRENCE))
                            player->CastSpell(player, SPELL_DETERRENCE, false);
                        else if (player->HasTalent(TALENT_AIMED_SHOT, player->GetActiveSpec()))
                        {    
                            if (!player->HasSpellCooldown(SPELL_AIMED_SHOT))
                            {
                                player->CastSpell(player->GetVictim(), SPELL_AIMED_SHOT, false);
                                break; 
                            }
                        }
                        player->CastSpell(victim, SPELL_AUTO_SHOT, false);
                        break;
                    case CLASS_PALADIN:
                        if (roll_chance_i(20) && !player->HasAura(SPELL_FORBEARANCE))
                            player->CastSpell(player, SPELL_DIVINE_SHIELD, false);
                        else if (player->HasTalent(TALENT_BEACON_OF_LIGHT, player->GetActiveSpec()))
                        {
                            if (roll_chance_i(20) && !player->HasSpellCooldown(SPELL_LAY_ON_HANDS))
                                player->CastSpell(ally, SPELL_LAY_ON_HANDS, false);
                            else
                                player->CastSpell(ally, SPELL_FLASH_OF_LIGHT, false);
                        }
                        else if (player->HasTalent(TALENT_AVENGERS_SHIELD, player->GetActiveSpec()))
                        {
                            if (roll_chance_i(30) && !player->HasSpellCooldown(SPELL_HUMMER_OF_JUSTICE))
                                player->CastSpell(player->GetVictim(), SPELL_HUMMER_OF_JUSTICE, false);
                            else if (!player->HasSpellCooldown(SPELL_AVENGERS_SHIELD))
                                player->CastSpell(player->GetVictim(), SPELL_AVENGERS_SHIELD, false);
                        }
                        else if (!player->HasSpellCooldown(SPELL_JUDGEMENT_OF_LIGHT))
                            player->CastSpell(player->GetVictim(), SPELL_JUDGEMENT_OF_LIGHT, false);

                        break;
                    case CLASS_WARRIOR:
                        if (player->HasTalent(TALENT_BLADESTORM, player->GetActiveSpec()))
                        {
                            if (!player->HasSpellCooldown(SPELL_MORTAL_STRIKE_PLAYER))
                                player->CastSpell(player->GetVictim(), SPELL_MORTAL_STRIKE_PLAYER, false);
                        }
                        else if (player->HasTalent(TALENT_SHOCKWAVE, player->GetActiveSpec()))
                        {
                            if (!player->HasSpellCooldown(SPELL_SHOCKWAVE))
                                player->CastSpell(player->GetVictim(), SPELL_SHOCKWAVE, false);
                        }
                        else if (!player->HasSpellCooldown(SPELL_CLEAVE))
                            player->CastSpell(player->GetVictim(), SPELL_CLEAVE, false);
                        
                        player->RemoveAurasDueToSpell(SPELL_BERSERKER_STANCE);
                        break;                
                    case CLASS_WARLOCK:
                        if (roll_chance_i(50))
                        {
                            if (Unit* fearTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true, -SPELL_DOMINATE_MIND))
                                player->CastSpell(fearTarget, SPELL_FEAR, false);
                        }
                        else
                            player->CastSpell(victim, SPELL_SHADOW_BOLT_PLAYER, false);

                        break;
                    case CLASS_MAGE:
                        if (roll_chance_i(50))
                        {
                            if (roll_chance_i(50))
                            {
                                if (Unit* polymorphTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true, -SPELL_DOMINATE_MIND))
                                    player->CastSpell(polymorphTarget, SPELL_POLYMORPH, false);
                            }
                            else
                                player->CastSpell(victim, SPELL_CONE_OF_COLD, false);
                        }
                        else
                            player->CastSpell(victim, SPELL_FROSTBOLT_PLAYER, false);

                        break;
                    case CLASS_ROGUE:
                        if (roll_chance_i(30) && !player->HasSpellCooldown(SPELL_CLOAK_OF_SHADOWS))
                            player->CastSpell(player, SPELL_CLOAK_OF_SHADOWS, false);
                        else if (roll_chance_i(50) && !player->HasSpellCooldown(SPELL_BLIND))
                        {
                            if (roll_chance_i(50))
                            {
                                if (Unit* blindTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 10.0f, true, -SPELL_DOMINATE_MIND))
                                    player->CastSpell(blindTarget, SPELL_BLIND, false);
                            }
                            else
                                player->CastSpell(player->GetVictim(), SPELL_FAN_OF_KNIVES, false);
                        }
                        else
                            player->CastSpell(player->GetVictim(), SPELL_SINISTER_STRIKE, false);

                        break;
                    case CLASS_DEATH_KNIGHT:
                        if (roll_chance_i(50))
                        {
                            if (roll_chance_i(30))
                                player->CastSpell(victim, SPELL_DEATH_GRIP, false);
                            else if (!player->HasSpellCooldown(SPELL_DEATH_AND_DECAY_PLAYER))
                                if (Unit* decayTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true, -SPELL_DOMINATE_MIND))
                                    player->CastSpell(decayTarget, SPELL_DEATH_AND_DECAY_PLAYER, false);
                        }
                        else
                            player->CastSpell(player->GetVictim(), SPELL_DEATH_COIL, false);

                        break;
                    case CLASS_SHAMAN:
                        if (player->HasTalent(TALENT_RIPTIDE, player->GetActiveSpec()))
                            player->CastSpell(ally, SPELL_CHAIN_HEAL, false);
                        else if (roll_chance_i(30) && !player->HasSpellCooldown(SPELL_CHAIN_LIGHTNING))
                            player->CastSpell(victim, SPELL_CHAIN_LIGHTNING, false);
                        else if (player->HasTalent(TALENT_THUNDERSTORM, player->GetActiveSpec()))
                            player->CastSpell(victim, SPELL_LIGHTNING_BOLT, false);
                        else if (player->HasTalent(TALENT_FERAL_SPIRIT, player->GetActiveSpec()))
                            player->CastSpell(player->GetVictim(), SPELL_STORMSTRIKE, false);
                        
                        break;
                }
            }

            Creature* GetRandomSummonOrBoss()
            {
                if (summons.empty())
                    return me;

                std::list<Creature*> alivedSummons;
                for (SummonList::iterator summonsItr = summons.begin(); summonsItr != summons.end(); ++summonsItr)
                    if (Creature* summon = ObjectAccessor::GetCreature(*me, *summonsItr))
                        if (summon->IsAlive())
                            alivedSummons.push_back(summon);

                if (alivedSummons.empty())
                    return me;

                std::list<Creature*>::iterator summonsItr = alivedSummons.begin();
                std::advance(summonsItr, urand(0, alivedSummons.size() - 1));

                return *summonsItr;
            }

            void SummonWaveP1()
            {
                uint8 addIndex = _waveCounter & 1;
                uint8 addIndexOther = uint8(addIndex ^ 1);

                // Summon first add, replace it with Darnavan if weekly quest is active
                if (_waveCounter || (WeeklyIndexICC)instance->GetData(DATA_WEEKLY_QUEST_INDEX) != WeeklyIndexICC::Deprogramming)
                    Summon(SummonEntries[addIndex], SummonPositions[addIndex * 3]);
                else
                    Summon(NPC_DARNAVAN, SummonPositions[addIndex * 3]);

                Summon(SummonEntries[addIndexOther], SummonPositions[addIndex * 3 + 1]);
                Summon(SummonEntries[addIndex], SummonPositions[addIndex * 3 + 2]);
                if (Is25ManRaid())
                {
                    Summon(SummonEntries[addIndexOther], SummonPositions[addIndexOther * 3]);
                    Summon(SummonEntries[addIndex], SummonPositions[addIndexOther * 3 + 1]);
                    Summon(SummonEntries[addIndexOther], SummonPositions[addIndexOther * 3 + 2]);
                    Summon(SummonEntries[urand(0, 1)], SummonPositions[6]);
                }

                ++_waveCounter;
            }

            void SummonWaveP2()
            {
                if (Is25ManRaid())
                {
                    uint8 addIndex = _waveCounter & 1;
                    Summon(SummonEntries[addIndex], SummonPositions[addIndex * 3]);
                    Summon(SummonEntries[addIndex ^ 1], SummonPositions[addIndex * 3 + 1]);
                    Summon(SummonEntries[addIndex], SummonPositions[addIndex * 3+ 2]);
                }
                else
                    Summon(SummonEntries[urand(0, 1)], SummonPositions[6]);

                ++_waveCounter;
            }

            void Summon(uint32 entry, const Position& pos)
            {
                if (TempSummon* summon = me->SummonCreature(entry, pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, Milliseconds(10 * IN_MILLISECONDS)))
                    summon->AI()->DoCast(summon, SPELL_TELEPORT_VISUAL);
            }

            Creature* GetRandomReadyCultist(uint32 type)
            {
                if (summons.empty())
                    return nullptr;

                // Get all cultists ready for transform
                std::list<Creature*> readyCultists;
                for (SummonList::iterator summonsItr = summons.begin(); summonsItr != summons.end(); ++summonsItr)
                    if (Creature* summon = ObjectAccessor::GetCreature(*me, *summonsItr))
                        if (summon->IsAlive() && (summon->GetEntry() == NPC_CULT_FANATIC || summon->GetEntry() == NPC_CULT_ADHERENT))
                            if (summon->GetAI()->GetData(type))
                            {
                                uint64 summonGUID = summon->GetGUID();
                                if (summonGUID != _lastEmpoweredCultistGUID && summonGUID != _lastReanimatedCultistGUID)
                                    readyCultists.push_back(summon);
                            }

                if (readyCultists.empty())
                    return nullptr;

                // Select random cultist
                return Trinity::Containers::SelectRandomContainerElement(readyCultists);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (Spell* currentCast = me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                    if (currentCast->m_spellInfo->Id == SPELL_FROSTBOLT)
                        for (uint8 i = 0; i < 3; ++i)
                            if (spell->HasEffect(SPELL_EFFECT_INTERRUPT_CAST))
                                me->InterruptSpell(CURRENT_GENERIC_SPELL, false);
            }

            PlayerAI* GetAIForCharmedPlayer(Player* player) override
            {
                return _useInternalPlayerAI ? nullptr : new SimpleCharmedPlayerAI(player, true);
            }

            private:
                Unit* _dominateMindTargets[3];
                ObjectGuid _nextVengefulShadeTargetGUID;
                ObjectGuid _darnavanGUID;
                std::deque<uint64> _reanimationQueue;
                uint32 _waveCounter;
                uint8 const _dominateMindCount;
                bool _introDone;
                ObjectGuid _lastEmpoweredCultistGUID;
                ObjectGuid _lastReanimatedCultistGUID;

                bool _useInternalPlayerAI = true;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<boss_lady_deathwhisperAI>(creature);
        }
};

typedef boss_lady_deathwhisper::boss_lady_deathwhisperAI DeathwisperAI;

class npc_cult_fanatic : public CreatureScript
{
    public:
        npc_cult_fanatic() : CreatureScript("npc_cult_fanatic") { }

        struct npc_cult_fanaticAI : public ScriptedAI
        {
            npc_cult_fanaticAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_FANATIC_NECROTIC_STRIKE, urand(10 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FANATIC_SHADOW_CLEAVE, urand(14 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FANATIC_VAMPIRIC_MIGHT, urand(20 * IN_MILLISECONDS, 27 * IN_MILLISECONDS));
                _aliveTimer = 0;
                _martyrdomCaster = nullptr;
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                DoZoneInCombat(me, 150.0f);
                if (!me->IsInCombat())
                    ScriptedAI::EnterEvadeMode();
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_DARK_TRANSFORMATION_T:
                        me->InterruptNonMeleeSpells(true);
                        DoCast(me, SPELL_DARK_TRANSFORMATION);
                        break;
                    case SPELL_DARK_TRANSFORMATION:
                        me->UpdateEntry(NPC_DEFORMED_FANATIC);
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoZoneInCombat(me, 150.0f);
                        break;
                    case SPELL_DARK_MARTYRDOM_T:
                        _martyrdomCaster = caster;
                        me->InterruptNonMeleeSpells(true);
                        me->RemoveAurasByType(SPELL_AURA_MOD_STUN);
                        me->RemoveAurasByType(SPELL_AURA_MOD_PACIFY);
                        me->RemoveAurasByType(SPELL_AURA_MOD_SILENCE);
                        me->RemoveAurasByType(SPELL_AURA_MOD_PACIFY_SILENCE);
                        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
                        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
                        DoCast(me, SPELL_DARK_MARTYRDOM_FANATIC);
                        break;
                    case SPELL_DARK_MARTYRDOM_FANATIC:
                        if (_martyrdomCaster)
                            _martyrdomCaster->GetAI()->SetData(DATA_REANIMATION_DONE, NPC_REANIMATED_FANATIC);

                        me->UpdateEntry(NPC_REANIMATED_FANATIC);
                        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, false);
                        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, false);
                        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
                        DoCast(me, SPELL_FANATIC_DETERMINATION, true);
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoZoneInCombat(me, 150.0f);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                _aliveTimer += diff;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FANATIC_NECROTIC_STRIKE:
                            DoCastVictim(SPELL_NECROTIC_STRIKE);
                            events.ScheduleEvent(EVENT_FANATIC_NECROTIC_STRIKE, urand(11 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
                            break;
                        case EVENT_FANATIC_SHADOW_CLEAVE:
                            DoCastVictim(SPELL_SHADOW_CLEAVE);
                            events.ScheduleEvent(EVENT_FANATIC_SHADOW_CLEAVE, urand(9 * IN_MILLISECONDS, 11 * IN_MILLISECONDS));
                            break;
                        case EVENT_FANATIC_VAMPIRIC_MIGHT:
                            DoCast(me, SPELL_VAMPIRIC_MIGHT);
                            events.ScheduleEvent(EVENT_FANATIC_VAMPIRIC_MIGHT, urand(20 * IN_MILLISECONDS, 27 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_READY_TO_EMPOWER:
                        return _aliveTimer > 8 * IN_MILLISECONDS;
                    case DATA_READY_TO_REANIMATE:
                        return _aliveTimer > 12 * IN_MILLISECONDS;
                    default:
                        return 0;
                }
            }

            protected:
                EventMap events;

            private:
                uint32 _aliveTimer;
                Unit* _martyrdomCaster;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_cult_fanaticAI>(creature);
        }
};

class npc_cult_adherent : public CreatureScript
{
    public:
        npc_cult_adherent() : CreatureScript("npc_cult_adherent") { }

        struct npc_cult_adherentAI : public ScriptedAI
        {
            npc_cult_adherentAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_ADHERENT_CURSE_OF_TORPOR, urand(14 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ADHERENT_SHORUD_OF_THE_OCCULT, urand(32 * IN_MILLISECONDS, 39 * IN_MILLISECONDS));
                autoCastTimer.SetInterval(2500);
                _aliveTimer = 0;
                _martyrdomCaster = nullptr;
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                DoZoneInCombat(me, 150.0f);
                if (!me->IsInCombat())
                    ScriptedAI::EnterEvadeMode();
            }

            void AttackStart(Unit* victim) override
            {
                if (victim && me->Attack(victim, false))
                    casterMovement.Chase(victim);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_DARK_EMPOWERMENT_T:
                        me->InterruptNonMeleeSpells(true);
                        DoCast(me, SPELL_DARK_EMPOWERMENT);
                        break;
                    case SPELL_DARK_EMPOWERMENT:
                        me->UpdateEntry(NPC_EMPOWERED_ADHERENT);
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoZoneInCombat(me, 150.0f);
                        break;
                    case SPELL_DARK_MARTYRDOM_T:
                        _martyrdomCaster = caster;
                        me->InterruptNonMeleeSpells(true);
                        me->RemoveAurasByType(SPELL_AURA_MOD_STUN);
                        me->RemoveAurasByType(SPELL_AURA_MOD_PACIFY);
                        me->RemoveAurasByType(SPELL_AURA_MOD_SILENCE);
                        me->RemoveAurasByType(SPELL_AURA_MOD_PACIFY_SILENCE);
                        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
                        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
                        DoCast(me, SPELL_DARK_MARTYRDOM_ADHERENT);
                        break;
                    case SPELL_DARK_MARTYRDOM_ADHERENT:
                        if (_martyrdomCaster)
                            _martyrdomCaster->GetAI()->SetData(DATA_REANIMATION_DONE, NPC_REANIMATED_ADHERENT);

                        me->UpdateEntry(NPC_REANIMATED_ADHERENT);
                        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, false);
                        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, false);
                        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
                        DoCast(me, SPELL_ADHERENT_DETERMINATION, true);
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoZoneInCombat(me, 150.0f);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                autoCastTimer.Update(diff);
                _aliveTimer += diff;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                casterMovement.Update(diff);

                //while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (events.ExecuteEvent())
                    {
                        case EVENT_ADHERENT_SHORUD_OF_THE_OCCULT:
                            DoCast(me, SPELL_SHORUD_OF_THE_OCCULT);
                            events.ScheduleEvent(EVENT_ADHERENT_SHORUD_OF_THE_OCCULT, urand(27 * IN_MILLISECONDS, 32 * IN_MILLISECONDS));
                            break;
                        case EVENT_ADHERENT_CURSE_OF_TORPOR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                                DoCast(target, SPELL_CURSE_OF_TORPOR);

                            events.ScheduleEvent(EVENT_ADHERENT_CURSE_OF_TORPOR, urand(9 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
                            break;
                        default:
                            if (autoCastTimer.Passed())
                            {
                                DoCastVictim(me->GetEntry() == NPC_EMPOWERED_ADHERENT ? SPELL_DEATHCHILL_BLAST : SPELL_DEATHCHILL_BOLT);
                                autoCastTimer.Reset();
                            }
                            break;
                    }
                }

                if (casterMovement.IsUnableToCast())
                    DoMeleeAttackIfReady();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_READY_TO_EMPOWER:
                        return _aliveTimer > 8 * IN_MILLISECONDS;
                    case DATA_READY_TO_REANIMATE:
                        return _aliveTimer > 12 * IN_MILLISECONDS;
                    default:
                        return 0;
                }
            }

            protected:
                EventMap events;
                CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_DEATHCHILL_BOLT).Spell(SPELL_DEATHCHILL_BLAST);
                IntervalTimer autoCastTimer;

            private:
                uint32 _aliveTimer;
                Unit* _martyrdomCaster;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_cult_adherentAI>(creature);
        }
};

class npc_vengeful_shade : public CreatureScript
{
    public:
        npc_vengeful_shade() : CreatureScript("npc_vengeful_shade") { }

        struct npc_vengeful_shadeAI : public ScriptedAI
        {
            npc_vengeful_shadeAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void Reset() override
            {
                me->AddAura(SPELL_VENGEFUL_BLAST_T, me);
                events.ScheduleEvent(EVENT_SHADE_SUICIDE, 8 * IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* /*target*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_VENGEFUL_BLAST)
                    me->Kill(me);
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
                        case EVENT_SHADE_SUICIDE: 
                            me->Kill(me);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            protected:
                EventMap events;
         };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_vengeful_shadeAI>(creature);
        }
};

class npc_darnavan : public CreatureScript
{
    public:
        npc_darnavan() : CreatureScript("npc_darnavan") { }

        struct npc_darnavanAI : public ScriptedAI
        {
            npc_darnavanAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_DARNAVAN_BLADESTORM, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_DARNAVAN_INTIMIDATING_SHOUT, urand(20 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DARNAVAN_MORTAL_STRIKE, urand(25 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DARNAVAN_SUNDER_ARMOR, urand(5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                _canCharge = true;
                _canShatter = true;
            }

            void JustDied(Unit* killer) override
            {
                events.Reset();
                if (Player* owner = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
                {
                    if (Group* group = owner->GetGroup())
                    {
                        for (GroupReference *itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
                        {
                            Player* member = itr->GetSource();
                            if (member && member->IsInMap(me))
                                member->FailQuest(QUEST_DEPROGRAMMING);
                        }
                    }
                    else if (owner->IsInMap(me))
                        owner->FailQuest(QUEST_DEPROGRAMMING);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE || pointId != POINT_DESPAWN)
                    return;

                me->DespawnOrUnsummon();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_DARNAVAN_AGGRO);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                DoZoneInCombat(me, 150.0f);
                if (!me->IsInCombat())
                    ScriptedAI::EnterEvadeMode();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (_canShatter && me->GetVictim() && me->GetVictim()->IsImmunedToDamage(SPELL_SCHOOL_MASK_NORMAL))
                {
                    DoCastVictim(SPELL_SHATTERING_THROW);
                    _canShatter = false;
                    events.ScheduleEvent(EVENT_DARNAVAN_SHATTERING_THROW, 30 * IN_MILLISECONDS);
                    return;
                }

                if (_canCharge && !me->IsWithinMeleeRange(me->GetVictim()))
                {
                    DoCastVictim(SPELL_CHARGE);
                    _canCharge = false;
                    events.ScheduleEvent(EVENT_DARNAVAN_CHARGE, 20 * IN_MILLISECONDS);
                    return;
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DARNAVAN_BLADESTORM:
                            DoCast(SPELL_BLADESTORM);
                            events.ScheduleEvent(EVENT_DARNAVAN_BLADESTORM, urand(90 * IN_MILLISECONDS, 100 * IN_MILLISECONDS));
                            break;
                        case EVENT_DARNAVAN_CHARGE:
                            _canCharge = true;
                            break;
                        case EVENT_DARNAVAN_INTIMIDATING_SHOUT:
                            DoCast(SPELL_INTIMIDATING_SHOUT);
                            events.ScheduleEvent(EVENT_DARNAVAN_INTIMIDATING_SHOUT, urand(90 * IN_MILLISECONDS, 120 * IN_MILLISECONDS));
                            break;
                        case EVENT_DARNAVAN_MORTAL_STRIKE:
                            DoCastVictim(SPELL_MORTAL_STRIKE);
                            events.ScheduleEvent(EVENT_DARNAVAN_MORTAL_STRIKE, urand(15 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                            break;
                        case EVENT_DARNAVAN_SHATTERING_THROW:
                            _canShatter = true;
                            break;
                        case EVENT_DARNAVAN_SUNDER_ARMOR:
                            DoCastVictim(SPELL_SUNDER_ARMOR);
                            events.ScheduleEvent(EVENT_DARNAVAN_SUNDER_ARMOR, urand(3 * IN_MILLISECONDS, 7 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            protected:
                EventMap events;

            private:
                bool _canCharge;
                bool _canShatter;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_darnavanAI>(creature);
        }
};

class spell_deathwhisper_mana_barrier : public SpellScriptLoader
{
    public:
        spell_deathwhisper_mana_barrier() : SpellScriptLoader("spell_deathwhisper_mana_barrier") { }

        class spell_deathwhisper_mana_barrier_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_deathwhisper_mana_barrier_AuraScript);

            void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();

                if (Unit* caster = GetCaster())
                {
                    int32 missingHealth = int32(caster->GetMaxHealth() - caster->GetHealth());
                    caster->ModifyHealth(missingHealth);
                    caster->ModifyPower(POWER_MANA, -missingHealth);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_deathwhisper_mana_barrier_AuraScript::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_deathwhisper_mana_barrier_AuraScript();
        }
};

class at_lady_deathwhisper_entrance : public AreaTriggerScript
{
    public:
        at_lady_deathwhisper_entrance() : AreaTriggerScript("at_lady_deathwhisper_entrance") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* ladyDeathwhisper = ObjectAccessor::GetCreature(*player, instance->GetGuidData(DATA_LADY_DEATHWHISPER)))
                    ladyDeathwhisper->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

void AddSC_boss_lady_deathwhisper()
{
    new boss_lady_deathwhisper();
    new npc_cult_fanatic();
    new npc_cult_adherent();
    new npc_vengeful_shade();
    new npc_darnavan();
    new spell_deathwhisper_mana_barrier();
    new at_lady_deathwhisper_entrance();
}
