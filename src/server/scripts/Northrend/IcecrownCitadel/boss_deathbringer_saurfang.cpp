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
#include "Group.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellAuras.h"
#include "icecrown_citadel.h"
#include "Transport.h"

enum ScriptTexts
{
    // Deathbringer Saurfang
    SAY_INTRO_ALLIANCE_2            = 0,
    SAY_INTRO_ALLIANCE_3            = 1,
    SAY_INTRO_ALLIANCE_6            = 2,
    SAY_INTRO_HORDE_2               = 4,
    SAY_INTRO_HORDE_4               = 5,
    SAY_INTRO_HORDE_9               = 6,
    SAY_AGGRO                       = 7,
    SAY_MARK_OF_THE_FALLEN_CHAMPION = 8,
    SAY_BLOOD_BEASTS                = 9,
    SAY_KILL                        = 10,
    SAY_FRENZY                      = 11,
    SAY_BERSERK                     = 12,
    SAY_DEATH                       = 13,

    // High Overlord Saurfang
    SAY_INTRO_HORDE_1               = 0,  // Kor'kron, move out! Champions, watch your backs. The Scourge have been...
    SAY_INTRO_HORDE_3               = 1,  // My boy died at the Wrath Gate. I am here only to collect his body.
    SAY_INTRO_HORDE_5               = 2,  // We named him Dranosh. It means "Heart of Draenor" in orcish. I would not let the warlocks take him. My boy would be safe, hidden away by the elders of Garadar.
    SAY_INTRO_HORDE_6               = 3,  // I made a promise to his mother before she died; that I would cross the Dark Portal alone - whether I lived or died, my son would be safe. Untainted...
    SAY_INTRO_HORDE_7               = 4,  // Today, I fulfill that promise.
    SAY_INTRO_HORDE_8               = 5,  // High Overlord Saurfang charges!
    SAY_OUTRO_ALLIANCE_8            = 6,  // Behind you lies the body of my only son. Nothing will keep me from him.
    SAY_OUTRO_ALLIANCE_13           = 8,  // Blood and thunder, my son.
    SAY_OUTRO_ALLIANCE_15           = 10, // I will not forget this... kindness. I thank you, Highness
    SAY_OUTRO_ALLIANCE_17           = 19, // %s nods.
    SAY_OUTRO_HORDE_1               = 11, // %s coughs.
    SAY_OUTRO_HORDE_2               = 12, // %s weeps over the corpse of his son.
    SAY_OUTRO_HORDE_3               = 13, // You will have a proper ceremony in Nagrand next to the pyres of your mother and ancestors.
    SAY_OUTRO_HORDE_4               = 14, // Honor, young heroes... no matter how dire the battle... Never forsake it!

    // Muradin Bronzebeard
    SAY_INTRO_ALLIANCE_1            = 0,  // Let's get a move on then! Move ou...
    SAY_INTRO_ALLIANCE_4            = 1,  // A lone orc against the might of the Alliance???
    SAY_INTRO_ALLIANCE_5            = 2,  // Charge!!!
    SAY_OUTRO_ALLIANCE_1            = 3,  // %s gasps for air.
    SAY_OUTRO_ALLIANCE_2            = 4,  // That was Saurfang's boy - the Horde commander at the Wrath Gate. Such a tragic end...
    SAY_OUTRO_ALLIANCE_3            = 5,  // What in the... There, in the distance!
    SAY_OUTRO_ALLIANCE_5            = 7,  // Soldiers, fall in! Looks like the Horde are comin' to take another shot!
    SAY_OUTRO_ALLIANCE_7            = 9,  // Don't force me hand, orc. We can't let ye pass.
    SAY_OUTRO_ALLIANCE_9            = 10, // I... I can't do it. Get back on yer ship and we'll spare yer life.
    SAY_OUTRO_ALLIANCE_23           = 12, // Right away, yer majesty!

    // Lady Jaina Proudmoore
    SAY_OUTRO_ALLIANCE_18           = 0,  // %s cries.
    SAY_OUTRO_ALLIANCE_20           = 2,  // %s wipes her tears away and smiles at King Varian Wrynn.
    SAY_OUTRO_ALLIANCE_21           = 1,  // It was nothing, your majesty. Just... I'm proud of my king.

    // King Varian Wrynn
    SAY_OUTRO_ALLIANCE_11           = 0,  // Stand down, Muradin. Let a grieving father pass.
    SAY_OUTRO_ALLIANCE_16           = 1,  // I... I was not at the Wrath Gate, but the soldiers who survived told me much of what happened. Your son fought with honor. He died a hero's death. He deserves a hero's burial.
    SAY_OUTRO_ALLIANCE_19           = 2,  // Jaina? Why are you crying?
    SAY_OUTRO_ALLIANCE_22           = 3,  // Bah! Muradin, secure the deck and prepare our soldiers for an assault on the upper citadel. I'll send out another regiment from Stormwind.
};

enum Spells
{
    // Deathbringer Saurfang
    SPELL_ZERO_POWER                          = 72242,
    SPELL_GRIP_OF_AGONY                       = 70572, // Intro
    SPELL_BLOOD_LINK                          = 72178,
    SPELL_MARK_OF_THE_FALLEN_CHAMPION_S       = 72256,
    SPELL_RUNE_OF_BLOOD_S                     = 72408,

    SPELL_SUMMON_BLOOD_BEAST                  = 72172,
    SPELL_SUMMON_BLOOD_BEAST_25_MAN           = 72356, // Additional cast, does not replace
    SPELL_FRENZY                              = 72737,
    SPELL_BLOOD_NOVA_TRIGGER                  = 72378,
    SPELL_BLOOD_NOVA                          = 72380,
    SPELL_BLOOD_POWER                         = 72371,
    SPELL_BLOOD_LINK_POWER                    = 72195,
    SPELL_BLOOD_LINK_DUMMY                    = 72202,
    SPELL_MARK_OF_THE_FALLEN_CHAMPION         = 72293,
    SPELL_BOILING_BLOOD                       = 72385,
    SPELL_RUNE_OF_BLOOD                       = 72410,

    // Blood Beast
    SPELL_BLOOD_LINK_BEAST                    = 72176,
    SPELL_RESISTANT_SKIN                      = 72723,
    SPELL_SCENT_OF_BLOOD                      = 72769, // Heroic only

    SPELL_RIDE_VEHICLE                        = 70640, // Outro
    SPELL_ACHIEVEMENT                         = 72928,
    SPELL_REMOVE_MARKS_OF_THE_FALLEN_CHAMPION = 72257,
    SPELL_PERMANENT_FEIGN_DEATH               = 70628,

    SPELL_SIMPLE_TELEPORT                     = 70618,
    SPELL_SIMPLE_TELEPORT_SMALL               = 12980,
};

enum EventTypes
{
    EVENT_INTRO_ALLIANCE_1 = 1,
    EVENT_INTRO_ALLIANCE_2,
    EVENT_INTRO_ALLIANCE_3,
    EVENT_INTRO_ALLIANCE_4,
    EVENT_INTRO_ALLIANCE_5,
    EVENT_INTRO_ALLIANCE_6,
    EVENT_INTRO_ALLIANCE_7,

    EVENT_INTRO_HORDE_1,
    EVENT_INTRO_HORDE_2,
    EVENT_INTRO_HORDE_3,
    EVENT_INTRO_HORDE_4,
    EVENT_INTRO_HORDE_5,
    EVENT_INTRO_HORDE_6,
    EVENT_INTRO_HORDE_7,
    EVENT_INTRO_HORDE_8,
    EVENT_INTRO_HORDE_9,
    EVENT_INTRO_HORDE_10,

    EVENT_INTRO_FINISH,

    EVENT_BERSERK,
    EVENT_SUMMON_BLOOD_BEAST,
    EVENT_BOILING_BLOOD,
    EVENT_BLOOD_NOVA,
    EVENT_RUNE_OF_BLOOD,
    EVENT_BOSS_ROOM,

    EVENT_GROUP_POST_NPC = 1,
};

enum Phases
{
    PHASE_INTRO_A       = 1,
    PHASE_INTRO_H       = 2,
    PHASE_COMBAT        = 3,
};

enum Actions
{
    ACTION_START_EVENT                  = -3781300,
    ACTION_CONTINUE_INTRO               = -3781301,
    ACTION_CHARGE                       = -3781302,
    ACTION_START_OUTRO                  = -3781303,
    ACTION_INTERRUPT_INTRO              = -3781305,
    ACTION_RESET_INTRO                  = -3781306,
    ACTION_MARK_OF_THE_FALLEN_CHAMPION  = -72293,
};

enum Misc
{
    DATA_MADE_A_MESS                    = 45374613, // 4537, 4613 are achievement IDs

    GOSSIP_MENU_MURADIN_BRONZEBEARD     = 10934,
    GOSSIP_MENU_HIGH_OVERLORD_SAURFANG  = 10952,

    SPAWN_GROUP_ENTRANCE_THE_DAMNED_EVENT   = 275,
};

enum MovePoints
{
    POINT_EXIT = 5, // waypoint id, don't change

    POINT_SAURFANG = 3781300,
    POINT_FIRST_STEP,
    POINT_CHARGE,
    POINT_CHOKE,

    POINT_HORDE_OUTRO_0, // High Overlord Saurfang walks to the bottom of the stairs
    POINT_HORDE_OUTRO_1, // High Overlord Saurfang raises up the stairs
    POINT_HORDE_OUTRO_2, // High Overlord Saurfang walks down the stairs
    POINT_HORDE_OUTRO_3, // High Overlord Saurfang walks to Scourge Transporter
    POINT_HORDE_OUTRO_4, // High Overlord Saurfang walks to Orgrim's Hammer

    POINT_ALLIANCE_OUTRO_0,  // Muradin Bronzebeard approaches Deathbringer Saurfang's corpse
    POINT_ALLIANCE_OUTRO_1,  // Muradin Bronzebeard begins approaching The Mighty Wind
    POINT_ALLIANCE_OUTRO_2,  // Muradin Bronzebeard approaches The Mighty Wind
    POINT_ALLIANCE_OUTRO_3,  // Skybreaker Marines take position behind Muradin Bronzebeard
    POINT_ALLIANCE_OUTRO_4,  // High Overlord Saurfang approaches Muradin Bronzebeard
    POINT_ALLIANCE_OUTRO_5,  // Muradin Bronzebeard takes position near Lady Jaina Proudmoor
    POINT_ALLIANCE_OUTRO_6,  // Skybreaker Marines take position behind Muradin Bronzebeard
    POINT_ALLIANCE_OUTRO_7,  // High Overlord Saurfang walks to the bottom of the stairs
    POINT_ALLIANCE_OUTRO_8,  // High Overlord Saurfang raises up the stairs
    POINT_ALLIANCE_OUTRO_9,  // High Overlord Saurfang walks down the stairs
    POINT_ALLIANCE_OUTRO_10, // High Overlord Saurfang approaches Alliance party
    POINT_ALLIANCE_OUTRO_11, // High Overlord Saurfang approaches King Varian Wrynn
    POINT_ALLIANCE_OUTRO_12, // High Overlord Saurfang approaches The Mighty Wind
};

Position const deathbringerPos = {-495.6833f, 2211.33f, 541.1138f, M_PI};
Position const firstStepPos = {-541.3177f, 2211.365f, 539.2921f, 0.0f};

Position const chargePos[6] =
{
    {-509.6505f, 2211.377f, 539.2872f, 0.0f}, // High Overlord Saurfang/Muradin Bronzebeard
    {-508.7480f, 2211.897f, 539.2870f, 0.0f}, // front left
    {-509.2929f, 2211.411f, 539.2870f, 0.0f}, // front right
    {-506.6607f, 2211.367f, 539.2870f, 0.0f}, // back middle
    {-506.1137f, 2213.306f, 539.2870f, 0.0f}, // back left
    {-509.0040f, 2211.743f, 539.2870f, 0.0f}  // back right
};

Position const chokePos[6] =
{
    {-514.4834f, 2211.334f, 549.2887f, 0.0f}, // High Overlord Saurfang/Muradin Bronzebeard
    {-510.1081f, 2211.592f, 546.3773f, 0.0f}, // front left
    {-513.3210f, 2211.396f, 551.2882f, 0.0f}, // front right
    {-507.3684f, 2210.353f, 545.7497f, 0.0f}, // back middle
    {-507.0486f, 2212.999f, 545.5512f, 0.0f}, // back left
    {-510.7041f, 2211.069f, 546.5298f, 0.0f}  // back right
};

// Can't use BossAI::_DespawnAtEvade because the boss is linked to itself in `linked_respawn` which prevents him from ever respawning
void DespawnHelper(Creature* me)
{
    me->SetVisible(false);
    me->m_Events.Schedule(10000, [me] { me->SetVisible(true); });
}

class boss_deathbringer_saurfang : public CreatureScript
{
    public:
        boss_deathbringer_saurfang() : CreatureScript("boss_deathbringer_saurfang") { }

        struct boss_deathbringer_saurfangAI : public BossAI
        {
            boss_deathbringer_saurfangAI(Creature* creature) : BossAI(creature, DATA_DEATHBRINGER_SAURFANG)
            {
                ASSERT(creature->GetVehicleKit()); // we dont actually use it, just check if exists
                _introDone = false;
                _dead = me->isDead();
                _fallenChampionCastCount = 0;
            }

            void Reset() override
            {
                if (_dead)
                    return;

                _Reset();
                events.SetPhase(PHASE_COMBAT);
                _frenzied = false;
                me->SetPower(POWER_ENERGY, 0);
                DoCast(me, SPELL_ZERO_POWER, true);
                DoCast(me, SPELL_BLOOD_LINK, true);
                DoCast(me, SPELL_BLOOD_POWER, true);
                DoCast(me, SPELL_MARK_OF_THE_FALLEN_CHAMPION_S, true);
                DoCast(me, SPELL_RUNE_OF_BLOOD_S, true);
                me->RemoveAurasDueToSpell(SPELL_BERSERK);
                me->RemoveAurasDueToSpell(SPELL_FRENZY);

                me->GetMap()->SetWorldState(Is25ManRaid() ? WORLDSTATE_IVE_GONE_AND_MADE_A_MESS_25 : WORLDSTATE_IVE_GONE_AND_MADE_A_MESS_10, 1);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (_dead)
                    return;

                if (!instance->CheckRequiredBosses(DATA_DEATHBRINGER_SAURFANG, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(LIGHT_S_HAMMER_TELEPORT);
                    return;
                }

                // oh just screw intro, enter combat - no exploits please
                me->setActive(true);
                DoZoneInCombat();

                events.Reset();
                events.SetPhase(PHASE_COMBAT);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                if (!_introDone)
                {
                    DoCast(me, SPELL_GRIP_OF_AGONY);
                    if (Creature* creature = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_SAURFANG_EVENT_NPC)))
                        creature->AI()->DoAction(ACTION_INTERRUPT_INTRO);
                }
                _introDone = true;

                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_SUMMON_BLOOD_BEAST, 40000, 0, PHASE_COMBAT);
                events.ScheduleEvent(EVENT_BERSERK, IsHeroic() ? 360000 : 480000, 0, PHASE_COMBAT);
                events.ScheduleEvent(EVENT_BOILING_BLOOD, 15500, 0, PHASE_COMBAT);
                events.ScheduleEvent(EVENT_BLOOD_NOVA, 17000, 0, PHASE_COMBAT);
                events.ScheduleEvent(EVENT_RUNE_OF_BLOOD, 20000, 0, PHASE_COMBAT);
                events.ScheduleEvent(EVENT_BOSS_ROOM, 5000, 0, PHASE_COMBAT);

                _fallenChampionCastCount = 0;
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_THE_FALLEN_CHAMPION);
                instance->SetBossState(DATA_DEATHBRINGER_SAURFANG, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/) override
            {
             //   _JustDied();
             //   DoCast(me, SPELL_ACHIEVEMENT, true);
             //   Talk(SAY_DEATH);
             //   me->RemoveAurasDueToSpell(SPELL_GRIP_OF_AGONY);

             //   instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_THE_FALLEN_CHAMPION);
             //   if (Creature* creature = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_SAURFANG_EVENT_NPC)))
             //       creature->AI()->DoAction(ACTION_START_OUTRO);
            }

            void AttackStart(Unit* victim) override
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC))
                    return;

                ScriptedAI::AttackStart(victim);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                float x, y, z, o;
                me->GetRespawnPosition(x, y, z, &o);
                me->SetHomePosition(x, y, z, o);
                DespawnHelper(me);
                _introDone = false;

                ScriptedAI::EnterEvadeMode();
                //if (_introDone)
                //   me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                if (_dead)
                    return;

                if (Creature* creature = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_SAURFANG_EVENT_NPC)))
                    creature->AI()->DoAction(ACTION_RESET_INTRO);
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                instance->SetBossState(DATA_DEATHBRINGER_SAURFANG, FAIL);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_THE_FALLEN_CHAMPION);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (damage >= me->GetHealth())
                    damage = me->GetHealth() - 1;

                if (!_frenzied && HealthBelowPct(31)) // AT 30%, not below
                {
                    _frenzied = true;
                    DoCast(me, SPELL_FRENZY, true);
                    Talk(SAY_FRENZY);
                }
                if (!_dead && me->GetHealth() < FightWonValue)
                {
                    _dead = true;
                    instance->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, me->GetEntry(), me);
                    _JustDied();
                    _EnterEvadeMode();
                    me->setDeathState(JUST_DIED);
                    auto clearCombat = [this]() { me->DeleteThreatList(); me->CombatStop(true); };
                    me->m_Events.Schedule( 1000, clearCombat);
                    me->m_Events.Schedule( 5000, clearCombat);
                    me->m_Events.Schedule(10000, clearCombat);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();
                    me->NearTeleportTo(deathbringerPos.GetPositionX(), deathbringerPos.GetPositionY(), deathbringerPos.GetPositionZ(), deathbringerPos.GetOrientation());
                    me->Relocate(deathbringerPos.GetPositionX(), deathbringerPos.GetPositionY(), deathbringerPos.GetPositionZ(), deathbringerPos.GetOrientation());
                    me->UpdateObjectVisibility(); // FUCKING TELEPORT ALREADY, YOU LITTLE PIECE OF SHIT, STOP BUGGING OUT RANDOMLY

                    DoCastAOE(SPELL_REMOVE_MARKS_OF_THE_FALLEN_CHAMPION, true);
                    DoCast(me, SPELL_ACHIEVEMENT, true);
                    Talk(SAY_DEATH);

                    //instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_THE_FALLEN_CHAMPION);
                    if (Creature* creature = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_SAURFANG_EVENT_NPC)))
                    {
                        DoCast(me, SPELL_PERMANENT_FEIGN_DEATH, true);
                        creature->AI()->DoAction(ACTION_START_OUTRO);
                    }
                    else
                        attacker->Kill(me); // Just a safeguard
                }
            }

            void JustSummoned(Creature* summon) override
            {
                Unit* target = nullptr;
                if ((target = SelectTarget(SELECT_TARGET_RANDOM, 0, -12.0f, true)) || // First attempt to find a target outside of melee range
                    (target = SelectTarget(SELECT_TARGET_RANDOM, 2,   0.0f, true)) || // If it fails - attempt to find a non-tank target
                    (target = SelectTarget(SELECT_TARGET_RANDOM, 0,   0.0f, true)))   // If all fails - screw it, just pick a random target
                {
                    summon->AI()->AttackStart(target);
                    summon->AddThreat(target, 5000);
                }

                summon->AI()->DoCast(summon, SPELL_BLOOD_LINK_BEAST, true);
                summon->AI()->DoCast(summon, SPELL_RESISTANT_SKIN, true);
                summon->AI()->DoCast(summon, 65208, true); // Self Stun (3 secs)
                summons.Summon(summon);
                DoZoneInCombat(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE && pointId != POINT_SAURFANG)
                    return;

                instance->HandleGameObject(instance->GetGuidData(GO_SAURFANG_S_DOOR), false);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_MARK_OF_THE_FALLEN_CHAMPION:
                        Talk(SAY_MARK_OF_THE_FALLEN_CHAMPION);
                        break;
                    case 72255: // Mark of the Fallen Champion, triggered id
                    case 72444:
                    case 72445:
                    case 72446:
                        if (me->GetPower(POWER_ENERGY) != me->GetMaxPower(POWER_ENERGY))
                            target->CastCustomSpell(SPELL_BLOOD_LINK_DUMMY, SPELLVALUE_BASE_POINT0, IsHeroic() ? 2 : 1, me, true);
                        break;
                    default:
                        break;
                }
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_BLOOD_LINK_POWER)
                    if (Aura* bloodPower = me->GetAura(SPELL_BLOOD_POWER))
                        bloodPower->RecalculateAmountOfEffects();
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(DATA_DEATHBRINGER_SAURFANG) != IN_PROGRESS && me->GetPower(POWER_ENERGY) != 0)
                    me->SetPower(POWER_ENERGY, 0);

                if (!UpdateVictim() && !(events.IsInPhase(PHASE_INTRO_A) || events.IsInPhase(PHASE_INTRO_H)))
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_ALLIANCE_2:
                            Talk(SAY_INTRO_ALLIANCE_2);
                            break;
                        case EVENT_INTRO_ALLIANCE_3:
                            Talk(SAY_INTRO_ALLIANCE_3);
                            break;
                        case EVENT_INTRO_ALLIANCE_6:
                            DoCast(me, SPELL_GRIP_OF_AGONY);
                            break;
                        case EVENT_INTRO_ALLIANCE_7:
                            Talk(SAY_INTRO_ALLIANCE_6);
                            break;
                        case EVENT_INTRO_HORDE_2:
                            me->GetMotionMaster()->MovePoint(POINT_SAURFANG, deathbringerPos.GetPositionX(), deathbringerPos.GetPositionY(), deathbringerPos.GetPositionZ());
                            Talk(SAY_INTRO_HORDE_2);
                            break;
                        case EVENT_INTRO_HORDE_4:
                            Talk(SAY_INTRO_HORDE_4);
                            break;
                        case EVENT_INTRO_HORDE_9:
                            DoCast(me, SPELL_GRIP_OF_AGONY);
                            break;
                        case EVENT_INTRO_HORDE_10:
                            Talk(SAY_INTRO_HORDE_9);
                            break;
                        case EVENT_INTRO_FINISH:
                            events.SetPhase(PHASE_COMBAT);
                            _introDone = true;
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->SetInCombatWithZone();
                            break;
                        case EVENT_SUMMON_BLOOD_BEAST:
                            for (uint32 i10 = 0; i10 < 2; ++i10)
                                DoCast(me, SPELL_SUMMON_BLOOD_BEAST+i10, true);
                            if (Is25ManRaid())
                                for (uint32 i25 = 0; i25 < 3; ++i25)
                                    DoCast(me, SPELL_SUMMON_BLOOD_BEAST_25_MAN+i25, true);
                            Talk(SAY_BLOOD_BEASTS);
                            events.ScheduleEvent(EVENT_SUMMON_BLOOD_BEAST, 40000, 0, PHASE_COMBAT);
                            break;
                        case EVENT_BLOOD_NOVA:
                        {
                            DoCastAOE(SPELL_BLOOD_NOVA_TRIGGER);
                            events.ScheduleEvent(EVENT_BLOOD_NOVA, urand(20000, 25000), 0, PHASE_COMBAT);
                            break;
                        }
                        case EVENT_RUNE_OF_BLOOD:
                            if (me->HasUnitState(UNIT_STATE_CASTING))
                            {
                                events.ScheduleEvent(EVENT_RUNE_OF_BLOOD, 500, 0, PHASE_COMBAT);
                                break;
                            }
                            DoCastVictim(SPELL_RUNE_OF_BLOOD); // Must not be triggered, otherwise DBM won't catch the event
                            events.ScheduleEvent(EVENT_RUNE_OF_BLOOD, urand(20000, 25000), 0, PHASE_COMBAT);
                            break;
                        case EVENT_BOILING_BLOOD:
                            me->CastCustomSpell(SPELL_BOILING_BLOOD, SPELLVALUE_MAX_TARGETS, RAID_MODE<int32>(1, 3, 1, 3), me, true);
                            events.ScheduleEvent(EVENT_BOILING_BLOOD, urand(15000, 20000), 0, PHASE_COMBAT);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            Talk(SAY_BERSERK);
                            break;
                        case EVENT_BOSS_ROOM:
                        {
                            if (me->GetPositionX() < -563.0f)
                                EnterEvadeMode();
                            if (me->GetVictim())
                            {
                                PathGenerator path { me };
                                if (!path.CalculatePath(me->GetVictim()->GetPositionX(), me->GetVictim()->GetPositionY(), me->GetVictim()->GetPositionZ()) ||
                                    path.GetPathType() & (PATHFIND_NOPATH | PATHFIND_INCOMPLETE))
                                {
                                    EnterEvadeMode();
                                    return;
                                }
                            }
                            events.ScheduleEvent(EVENT_BOSS_ROOM, 5000, 0, PHASE_COMBAT);
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();

                EnterEvadeIfOutOfCombatArea(diff); // TODO: check simple way
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_MADE_A_MESS)
                    if (_fallenChampionCastCount < RAID_MODE<uint32>(3, 5, 3, 5))
                        return 1;

                return 0;
            }

            // intro setup
            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case PHASE_INTRO_A:
                    case PHASE_INTRO_H:
                    {
                        if (GameObject* teleporter = GameObject::GetGameObject(*me, instance->GetGuidData(GO_SCOURGE_TRANSPORTER_DEATHBRINGERS)))
                        {
                            instance->HandleGameObject(ObjectGuid::Empty, false, teleporter);
                            teleporter->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                        }

                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        // controls what events will execute
                        events.SetPhase(uint32(action));

                        me->SetHomePosition(deathbringerPos.GetPositionX(), deathbringerPos.GetPositionY(), deathbringerPos.GetPositionZ(), me->GetOrientation());
                        if (action == PHASE_INTRO_A)
                            me->GetMotionMaster()->MovePoint(POINT_SAURFANG, deathbringerPos.GetPositionX(), deathbringerPos.GetPositionY(), deathbringerPos.GetPositionZ());

                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_2, 2000, 0, PHASE_INTRO_A);
                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_3, 19000, 0, PHASE_INTRO_A);

                        events.ScheduleEvent(EVENT_INTRO_HORDE_2, 6000, 0, PHASE_INTRO_H);
                        break;
                    }
                    case ACTION_CONTINUE_INTRO:
                    {
                        if (_introDone)
                            return;
                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_6, 6500+3500, 0, PHASE_INTRO_A);
                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_7, 6500+3500+3000, 0, PHASE_INTRO_A);
                        events.ScheduleEvent(EVENT_INTRO_FINISH,     6500+3500+3000+5000, 0, PHASE_INTRO_A);

                        events.ScheduleEvent(EVENT_INTRO_HORDE_4,  9000, 0, PHASE_INTRO_H);
                        events.ScheduleEvent(EVENT_INTRO_HORDE_9,  9000+10500+18500+17500+4000, 0, PHASE_INTRO_H);
                        events.ScheduleEvent(EVENT_INTRO_HORDE_10, 9000+10500+18500+17500+4000+2000, 0, PHASE_INTRO_H);
                        events.ScheduleEvent(EVENT_INTRO_FINISH,   9000+10500+18500+17500+4000+2000+9000, 0, PHASE_INTRO_H);
                        break;
                    }
                    case ACTION_MARK_OF_THE_FALLEN_CHAMPION:
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 0.0f, true, -SPELL_MARK_OF_THE_FALLEN_CHAMPION))
                        {
                            ++_fallenChampionCastCount;

                            uint32 allowUsing = Is25ManRaid() ? 5 : 3;
                            if (_fallenChampionCastCount > allowUsing)
                                me->GetMap()->SetWorldState(Is25ManRaid() ? WORLDSTATE_IVE_GONE_AND_MADE_A_MESS_25 : WORLDSTATE_IVE_GONE_AND_MADE_A_MESS_10, 0);

                            DoCast(target, SPELL_MARK_OF_THE_FALLEN_CHAMPION, true);
                            me->SetPower(POWER_ENERGY, 0);
                            if (Aura* bloodPower = me->GetAura(SPELL_BLOOD_POWER))
                                bloodPower->RecalculateAmountOfEffects();
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            static uint32 const FightWonValue;

        private:
            uint32 _fallenChampionCastCount;
            bool _introDone;
            bool _frenzied;   // faster than iterating all auras to find Frenzy
            bool _dead;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<boss_deathbringer_saurfangAI>(creature);
        }
};

uint32 const boss_deathbringer_saurfang::boss_deathbringer_saurfangAI::FightWonValue = 100000;


struct npc_high_overlord_saurfang_icc : public ScriptedAI
{
    npc_high_overlord_saurfang_icc(Creature* creature) : ScriptedAI(creature)
    {
        ASSERT(creature->GetVehicleKit());
        _instance = me->GetInstanceScript();

        if (me->GetPositionZ() > 100.0f && !me->IsSummon() && _instance->GetBossState(DATA_DEATHBRINGER_SAURFANG) == DONE && me->GetInstanceScript()->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
        {
            TeleportIn(NPC_SE_APOTHECARY_CANDITH_TOMAS, -529.889099f, 2226.092773f, 539.291870f, 5.660145f)->m_Events.KillEventsByGroup(EVENT_GROUP_POST_NPC);
            TeleportIn(NPC_SE_MORGAN_DAYBLAZE,          -521.246460f, 2231.718750f, 539.290161f, 5.298303f)->m_Events.KillEventsByGroup(EVENT_GROUP_POST_NPC);
        }
    }

    void Reset() override
    {
        _events.Reset();
    }

    bool OnGossipSelect(Player* player, uint32 menuId, uint32 /*gossipListId*/) override
    {
        if (menuId == GOSSIP_MENU_HIGH_OVERLORD_SAURFANG)
        {
            CloseGossipMenuFor(player);
            DoAction(ACTION_START_EVENT);
        }
        return false;
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_START_EVENT:
            {
                // Prevent crashes
                if (_events.IsInPhase(PHASE_INTRO_A) || _events.IsInPhase(PHASE_INTRO_H))
                    return;

                _guardList.clear();
                GetCreatureListWithEntryInGrid(_guardList, me, NPC_SE_KOR_KRON_REAVER, 20.0f);
                _guardList.sort(Trinity::ObjectDistanceOrderPred(me));
                uint32 x = 1;
                for (std::list<Creature*>::iterator itr = _guardList.begin(); itr != _guardList.end(); ++x, ++itr)
                    (*itr)->AI()->SetData(0, x);

                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                Talk(SAY_INTRO_HORDE_1);
                _events.SetPhase(PHASE_INTRO_H);
                _events.ScheduleEvent(EVENT_INTRO_HORDE_1, 6000, 0, PHASE_INTRO_H);
                _events.ScheduleEvent(EVENT_INTRO_HORDE_3, 6000+18000, 0, PHASE_INTRO_H);
                if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_DEATHBRINGER_SAURFANG)))
                    deathbringer->AI()->DoAction(PHASE_INTRO_H);
                break;
            }
            case ACTION_START_OUTRO:
            {
                me->setActive(true);

                uint32 delay = 0;
                Schedule(delay +=  7000, [this]()
                {
                    me->RemoveAurasDueToSpell(SPELL_GRIP_OF_AGONY);
                    me->SetFlying(false);
                    me->ResetInhabitTypeOverride();
                    me->GetMotionMaster()->MoveFall();
                    for (auto&& guard : _guardList)
                    {
                        guard->RemoveAurasDueToSpell(SPELL_GRIP_OF_AGONY);
                        guard->SetFlying(false);
                        guard->ResetInhabitTypeOverride();
                        guard->GetMotionMaster()->MoveFall();
                    }
                });
                Schedule(delay +=  1500, [this]() { me->SetStandState(UNIT_STAND_STATE_KNEEL); });
                Schedule(delay +=  1500, [this]() { Talk(SAY_OUTRO_HORDE_1); });
                Schedule(delay += 10000, [this]()
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(POINT_HORDE_OUTRO_0, -503.591919f, 2211.315430f, 539.292114f);
                });
                Schedule(delay += 19000, [this]() { Talk(SAY_OUTRO_HORDE_3); });
                Schedule(delay +=  7000, [this]()
                {
                    if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_DEATHBRINGER_SAURFANG)))
                    {
                        deathbringer->CastSpell(me, SPELL_RIDE_VEHICLE, true);
                        deathbringer->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        deathbringer->HandleEmoteStateCommand(EMOTE_STATE_DROWNED);
                    }
                });
                Schedule(delay +=  5000, [this]()
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    me->GetMotionMaster()->MovePoint(POINT_HORDE_OUTRO_2, -503.591919f, 2211.315430f, 539.292114f);
                    for (auto&& guard : _guardList)
                    {
                        guard->GetMotionMaster()->MovePoint(0, -550.551758f, 2202.259033f, 539.290771f);
                        guard->DespawnOrUnsummon(3000);
                    }
                });
                break;
            }
            case ACTION_INTERRUPT_INTRO:
                _events.Reset();
                for (auto&& guard : _guardList)
                    guard->DespawnOrUnsummon();
                break;
            case ACTION_RESET_INTRO:
                me->SetFlying(false);
                me->ResetInhabitTypeOverride();
                EnterEvadeMode();
                DespawnHelper(me);
                for (auto&& guard : _guardList)
                {
                    guard->SetFlying(false);
                    guard->ResetInhabitTypeOverride();
                    guard->AI()->EnterEvadeMode();
                    DespawnHelper(guard);
                }
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
            default:
                break;
        }
    }

    void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_GRIP_OF_AGONY)
        {
            me->SetFlying(true);
            me->OverrideInhabitType(INHABIT_AIR);
            me->GetMotionMaster()->MoveTakeoff(POINT_CHOKE, chokePos[0], false);
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type == POINT_MOTION_TYPE)
        {
            switch (pointId)
            {
                case POINT_FIRST_STEP:
                    me->SetWalk(false);
                    Talk(SAY_INTRO_HORDE_3);
                    _events.ScheduleEvent(EVENT_INTRO_HORDE_5, 9000+10500, 0, PHASE_INTRO_H);
                    _events.ScheduleEvent(EVENT_INTRO_HORDE_6, 9000+10500+18500, 0, PHASE_INTRO_H);
                    _events.ScheduleEvent(EVENT_INTRO_HORDE_7, 9000+10500+18500+17500, 0, PHASE_INTRO_H);
                    _events.ScheduleEvent(EVENT_INTRO_HORDE_8, 9000+10500+18500+17500+4000, 0, PHASE_INTRO_H);
                    if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_DEATHBRINGER_SAURFANG)))
                        deathbringer->AI()->DoAction(ACTION_CONTINUE_INTRO);
                    break;
                case POINT_HORDE_OUTRO_0:
                    me->GetMotionMaster()->MovePoint(POINT_HORDE_OUTRO_1, -498.215607f, 2211.315918f, 541.114380f);
                    break;
                case POINT_HORDE_OUTRO_1:
                {
                    Talk(SAY_OUTRO_HORDE_2);
                    uint32 delay = 0;
                    Schedule(delay += 2000, [this]() { me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR); });
                    Schedule(delay += 3000, [this]() { me->SetStandState(UNIT_STAND_STATE_KNEEL); });
                    break;
                }
                case POINT_HORDE_OUTRO_2:
                    me->GetMotionMaster()->MovePoint(POINT_HORDE_OUTRO_3, -548.756409f, 2211.194092f, 539.291870f);
                    break;
                case POINT_HORDE_OUTRO_3:
                {
                    uint32 delay = 0;
                    Schedule(delay += 1000, [this]() { me->SetFacingTo(0.0f); });
                    Schedule(delay += 1000, [this]() { Talk(SAY_OUTRO_HORDE_4); });
                    Schedule(delay += 8000, [this]() { me->GetMotionMaster()->MovePoint(POINT_HORDE_OUTRO_4, -563.7552f, 2211.328f, 538.7848f); });
                    break;
                }
                case POINT_HORDE_OUTRO_4:
                {
                    if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_DEATHBRINGER_SAURFANG)))
                    {
                        deathbringer->ExitVehicle();
                        deathbringer->DespawnOrUnsummon();
                    }
                    me->SetVisible(false);

                    uint32 delay = 0;
                    Schedule(delay +=     1, [this]()
                    {
                        me->SetPhaseMask(PHASEMASK_NORMAL | 64, false);
                        std::list<GameObject*> teleporters;
                        GetGameObjectListWithEntryInGrid(teleporters, me, GO_HORDE_TELEPORTER, 100.0f);
                        me->SetPhaseMask(PHASEMASK_NORMAL, false);
                        for (auto&& teleporter : teleporters)
                            teleporter->SetPhaseMask(PHASEMASK_NORMAL, true);

                        for (auto&& guard : _guardList)
                            guard->Respawn(true);
                    });
                    Schedule(delay +=  1000, [this]()
                    {
                        std::list<GameObject*> teleporters;
                        GetGameObjectListWithEntryInGrid(teleporters, me, GO_HORDE_TELEPORTER, 100.0f);
                        for (auto&& teleporter : teleporters)
                            teleporter->SetGoState(GO_STATE_ACTIVE);
                    });
                    Schedule(delay +=  5000, [this]()
                    {
                        std::list<GameObject*> teleporters;
                        GetGameObjectListWithEntryInGrid(teleporters, me, GO_HORDE_TELEPORTER, 100.0f);
                        for (auto&& teleporter : teleporters)
                            TeleportIn(NPC_SE_HORDE_PEON, teleporter->GetPositionX(), teleporter->GetPositionY(), teleporter->GetPositionZ(), 0.0f, SPELL_SIMPLE_TELEPORT_SMALL);
                    });
                    Schedule(delay += 28000, [this]()
                    {
                        me->SetPhaseMask(PHASEMASK_NORMAL | 64, false);
                        std::list<GameObject*> buildings;
                        GetGameObjectListWithEntryInGrid(buildings, me, GO_HORDE_TENT_1, 100.0f);
                        GetGameObjectListWithEntryInGrid(buildings, me, GO_HORDE_TENT_2, 100.0f);
                        GetGameObjectListWithEntryInGrid(buildings, me, GO_BLACKSMITHS_ANVIL_H, 100.0f);
                        GetGameObjectListWithEntryInGrid(buildings, me, GO_FORGE_H, 100.0f);
                        //GetGameObjectListWithEntryInGrid(buildings, me, GO_BONFIRE, 100.0f);
                        me->SetPhaseMask(PHASEMASK_NORMAL, false);
                        for (auto&& building : buildings)
                            building->SetPhaseMask(PHASEMASK_NORMAL, true);
                    });
                    Schedule(delay +=  9000, [this]()
                    {
                        std::list<GameObject*> teleporters;
                        GetGameObjectListWithEntryInGrid(teleporters, me, GO_HORDE_TELEPORTER, 100.0f);
                        for (auto&& teleporter : teleporters)
                            TeleportIn(teleporter->GetPositionY() > 2210.0f ? NPC_SE_APOTHECARY_CANDITH_TOMAS : NPC_SE_MORGAN_DAYBLAZE, teleporter->GetPositionX(), teleporter->GetPositionY(), teleporter->GetPositionZ(), 0.0f, SPELL_SIMPLE_TELEPORT_SMALL);
                    });
                    Schedule(delay +=  1000, [this]()
                    {
                        me->setActive(false);
                        me->DespawnOrUnsummon();
                    });
                    break;
                }

                case POINT_ALLIANCE_OUTRO_7:
                    me->GetMotionMaster()->MovePoint(POINT_ALLIANCE_OUTRO_8, -498.152405f, 2213.333984f, 541.114075f);
                    break;
                case POINT_ALLIANCE_OUTRO_8:
                {
                    if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_DEATHBRINGER_SAURFANG)))
                        me->SetFacingToObject(deathbringer);
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    uint32 delay = 0;
                    Schedule(delay += 1000, [this]() { Talk(SAY_OUTRO_ALLIANCE_13); });
                    Schedule(delay += 7000, [this]()
                    {
                        if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_DEATHBRINGER_SAURFANG)))
                        {
                            deathbringer->CastSpell(me, SPELL_RIDE_VEHICLE, true);
                            deathbringer->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                            deathbringer->HandleEmoteStateCommand(EMOTE_STATE_DROWNED);
                        }
                    });
                    Schedule(delay += 1000, [this]()
                    {
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        me->GetMotionMaster()->MovePoint(POINT_ALLIANCE_OUTRO_9, -504.248474f, 2219.067871f, 539.285828f);
                    });
                    break;
                }
                case POINT_ALLIANCE_OUTRO_9:
                    me->GetMotionMaster()->MovePoint(POINT_ALLIANCE_OUTRO_10, -523.292725f, 2227.979492f, 539.290283f);
                    break;
                case POINT_ALLIANCE_OUTRO_10:
                    me->GetMotionMaster()->MovePoint(POINT_ALLIANCE_OUTRO_11, -524.304626f, 2229.671387f, 539.290283f);
                    break;
                case POINT_ALLIANCE_OUTRO_11:
                {
                    uint32 delay = 0;
                    Schedule(delay +=  1000, [this]() { me->SetFacingToObject(Varian()); });
                    Schedule(delay +=  1000, [this]()
                    {
                        Talk(SAY_OUTRO_ALLIANCE_15);
                        Varian()->SetFacingToObject(me);
                    });
                    Schedule(delay += 10000, [this]() { Varian()->AI()->Talk(SAY_OUTRO_ALLIANCE_16); });
                    Schedule(delay +=  4000, [this]() { Varian()->HandleEmoteCommand(EMOTE_ONESHOT_TALK); });
                    Schedule(delay +=  4000, [this]() { Varian()->HandleEmoteCommand(EMOTE_ONESHOT_TALK); });
                    Schedule(delay +=  3000, [this]() { Varian()->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE); });
                    Schedule(delay +=  9000, [this]() { Talk(SAY_OUTRO_ALLIANCE_17); });
                    Schedule(delay +=  4000, [this]() { me->GetMotionMaster()->MovePoint(POINT_ALLIANCE_OUTRO_12, -524.042664f, 2247.780762f, 539.290283f); });
                    break;
                }
                case POINT_ALLIANCE_OUTRO_12:
                    if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_DEATHBRINGER_SAURFANG)))
                    {
                        deathbringer->ExitVehicle();
                        deathbringer->DespawnOrUnsummon();
                    }
                    me->DespawnOrUnsummon();
                    break;
                default:
                    break;
            }
        }
        else if (type == WAYPOINT_MOTION_TYPE && pointId == POINT_EXIT)
        {
            std::list<Creature*> guards;
            GetCreatureListWithEntryInGrid(guards, me, NPC_KOR_KRON_GENERAL, 50.0f);
            for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
            {
                (*itr)->SetVisible(false);
                (*itr)->DespawnOrUnsummon();
            }
            me->SetVisible(false);
            me->DespawnOrUnsummon();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);
        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_INTRO_HORDE_1:
                    _instance->HandleGameObject(_instance->GetGuidData(GO_SAURFANG_S_DOOR), true);
                    break;
                case EVENT_INTRO_HORDE_3:
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(POINT_FIRST_STEP, firstStepPos.GetPositionX(), firstStepPos.GetPositionY(), firstStepPos.GetPositionZ());
                    break;
                case EVENT_INTRO_HORDE_5:
                    Talk(SAY_INTRO_HORDE_5);
                    break;
                case EVENT_INTRO_HORDE_6:
                    Talk(SAY_INTRO_HORDE_6);
                    break;
                case EVENT_INTRO_HORDE_7:
                    Talk(SAY_INTRO_HORDE_7);
                    break;
                case EVENT_INTRO_HORDE_8:
                    Talk(SAY_INTRO_HORDE_8);
                    for (std::list<Creature*>::iterator itr = _guardList.begin(); itr != _guardList.end(); ++itr)
                        (*itr)->AI()->DoAction(ACTION_CHARGE);
                    me->GetMotionMaster()->MoveCharge(chargePos[0].GetPositionX(), chargePos[0].GetPositionY(), chargePos[0].GetPositionZ(), 8.5f, POINT_CHARGE);
                    break;
            }
        }
    }

private:
    EventMap _events;
    InstanceScript* _instance;
    std::list<Creature*> _guardList;

    void Schedule(uint32 delay, std::function<void()> const& func)
    {
        me->m_Events.Schedule(delay, func);
    }
    Creature* Varian() const
    {
        return me->FindNearestCreature(NPC_SE_KING_VARIAN_WRYNN, 100.0f);
    }
    Creature* TeleportIn(uint32 entry, float x, float y, float z, float o, uint32 spell = 0, uint32 duration = 0) const
    {
        Creature* creature = me->SummonCreature(entry, x, y, z, o, duration ? TEMPSUMMON_TIMED_DESPAWN : TEMPSUMMON_MANUAL_DESPAWN, Milliseconds(duration));
        if (creature && spell)
            creature->CastSpell(creature, spell, true);
        return creature;
    }
};

struct npc_muradin_bronzebeard_icc : public ScriptedAI
{
    npc_muradin_bronzebeard_icc(Creature* creature) : ScriptedAI(creature)
    {
        _instance = me->GetInstanceScript();

        if (me->GetPositionZ() > 100.0f && _instance->GetBossState(DATA_DEATHBRINGER_SAURFANG) == DONE && me->GetInstanceScript()->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
        {
            TeleportIn(NPC_SE_BRAZIE_GETZ,       -530.619995f, 2226.617920f, 539.291870f, 5.729299f)->m_Events.KillEventsByGroup(EVENT_GROUP_POST_NPC);
            TeleportIn(NPC_SE_SHELY_STEELBOWELS, -526.355042f, 2231.519043f, 539.292114f, 5.654691f)->m_Events.KillEventsByGroup(EVENT_GROUP_POST_NPC);
        }
    }

    void Reset() override
    {
        _events.Reset();
    }


    bool OnGossipSelect(Player* player, uint32 menuId, uint32 /*gossipListId*/) override
    {
        if (menuId == GOSSIP_MENU_MURADIN_BRONZEBEARD)
        {
            CloseGossipMenuFor(player);
            DoAction(ACTION_START_EVENT);
        }
        return false;
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_START_EVENT:
            {
                // Prevent crashes
                if (_events.IsInPhase(PHASE_INTRO_A) || _events.IsInPhase(PHASE_INTRO_H))
                    return;

                _events.SetPhase(PHASE_INTRO_A);
                _guardList.clear();
                GetCreatureListWithEntryInGrid(_guardList, me, NPC_SE_SKYBREAKER_MARINE, 20.0f);
                _guardList.sort(Trinity::ObjectDistanceOrderPred(me));
                uint32 x = 1;
                for (std::list<Creature*>::iterator itr = _guardList.begin(); itr != _guardList.end(); ++x, ++itr)
                    (*itr)->AI()->SetData(0, x);

                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                Talk(SAY_INTRO_ALLIANCE_1);
                _events.ScheduleEvent(EVENT_INTRO_ALLIANCE_4, 32000, 0, PHASE_INTRO_A);
                _instance->HandleGameObject(_instance->GetGuidData(GO_SAURFANG_S_DOOR), true);
                if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_DEATHBRINGER_SAURFANG)))
                    deathbringer->AI()->DoAction(PHASE_INTRO_A);
                break;
            }
            case ACTION_START_OUTRO:
            {
                me->setActive(true);

                uint32 delay = 0;
                Schedule(delay +=  4000, [this]()
                {
                    me->RemoveAurasDueToSpell(SPELL_GRIP_OF_AGONY);
                    me->SetFlying(false);
                    me->ResetInhabitTypeOverride();
                    me->GetMotionMaster()->MoveFall();
                    for (auto&& guard : _guardList)
                    {
                        guard->RemoveAurasDueToSpell(SPELL_GRIP_OF_AGONY);
                        guard->SetFlying(false);
                        guard->ResetInhabitTypeOverride();
                        guard->GetMotionMaster()->MoveFall();
                    }

                    transport = sTransportMgr->CreateTransport(GO_ZEPPELIN_HORDE_THE_MIGHTY_WIND, 0, me->GetMap());
                    if (transport)
                    {
                        if (Creature* saurfang = transport->SummonPassenger(NPC_SE_HIGH_OVERLORD_SAURFANG, { -1.43358433f, -2.35968876f, -17.8489056f, M_PI / 2 }, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            saurfangTransportGUID = saurfang->GetGUID();
                            saurfang->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        }
                        transport->EnableMovement(true);
                    }
                });
                Schedule(delay +=  1500, [this]() { me->SetStandState(UNIT_STAND_STATE_KNEEL); });
                Schedule(delay +=  1500, [this]() { Talk(SAY_OUTRO_ALLIANCE_1); });
                Schedule(delay +=  7000, [this]()
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(POINT_ALLIANCE_OUTRO_0, -498.215607f, 2211.315918f, 541.114380f);
                });
                Schedule(delay +=  7000, [this]() { Talk(SAY_OUTRO_ALLIANCE_2); });
                Schedule(delay += 10000, [this]()
                {
                    me->SetFacingTo(2.3f);
                    me->SetWalk(false);
                });
                Schedule(delay +=   500, [this]() { Talk(SAY_OUTRO_ALLIANCE_3); });
                Schedule(delay +=  3500, [this]() { me->GetMotionMaster()->MovePoint(POINT_ALLIANCE_OUTRO_1, -521.844360f, 2223.625977f, 539.290283f); });
                Schedule(delay += 12000, [this]() { Talk(SAY_OUTRO_ALLIANCE_5); });
                Schedule(delay +=  3000, [this]() { me->HandleEmoteStateCommand(EMOTE_ONESHOT_READY1H); });
                Schedule(delay +=  2000, [this]()
                {
                    static std::pair<float, float> followAngleDist[4] =
                    {
                        std::make_pair( M_PI / 2 + M_PI / 4, 1.5f),
                        std::make_pair(-M_PI / 2 - M_PI / 4, 1.5f),
                        std::make_pair( M_PI / 2 + M_PI / 10, 4.5f),
                        std::make_pair(-M_PI / 2 - M_PI / 10, 4.5f),
                    };
                    uint8 i = 0;
                    for (auto&& guard : _guardList)
                    {
                        Position pos;
                        pos.Relocate(me);
                        pos.RelocateOffset(followAngleDist[i].first, followAngleDist[i].second);
                        guard->HandleEmoteStateCommand(EMOTE_ONESHOT_READY1H);
                        guard->GetMotionMaster()->MovePoint(POINT_ALLIANCE_OUTRO_3, pos);
                        ++i;
                    }
                });
                Schedule(delay +=  9000, [this]()
                {
                    if (Creature* saurfang = ObjectAccessor::GetCreature(*me, saurfangTransportGUID))
                    {
                        saurfangTransportGUID = ObjectGuid::Empty;
                        saurfang->DespawnOrUnsummon();
                    }
                });
                Schedule(delay +=  1000, [this]()
                {
                    transport->EnableMovement(false);
                    if (Creature* saurfang = me->SummonCreature(NPC_SE_HIGH_OVERLORD_SAURFANG, -521.765625f, 2249.008789f, 539.292419f, 4.824901f))
                    {
                        saurfangGUID = saurfang->GetGUID();
                        saurfang->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        saurfang->SetWalk(true);
                        saurfang->GetMotionMaster()->MovePoint(POINT_ALLIANCE_OUTRO_4, -522.099121f, 2232.313477f, 539.291687f);
                    }
                });
                Schedule(delay +=  8000, [this]() { Talk(SAY_OUTRO_ALLIANCE_7); });
                Schedule(delay +=  8000, [this]() { Saurfang()->AI()->Talk(SAY_OUTRO_ALLIANCE_8); });
                Schedule(delay +=  4000, [this]() { Saurfang()->HandleEmoteStateCommand(EMOTE_STATE_READY2H); });
                Schedule(delay +=  6000, [this]() { Talk(SAY_OUTRO_ALLIANCE_9); });
                Schedule(delay +=  9000, [this]() { TeleportIn(NPC_SE_STORMWIND_PORTAL, -529.868286f, 2230.594482f, 539.292053f, 0.0f, SPELL_SIMPLE_TELEPORT, 10000); });
                Schedule(delay +=  4000, [this]()
                {
                    TeleportIn(NPC_SE_KING_VARIAN_WRYNN, -527.552734f, 2230.918457f, 539.291931f, 5.706895f, SPELL_SIMPLE_TELEPORT_SMALL);
                    TeleportIn(NPC_SE_JAINA_PROUDMOORE,  -527.654785f, 2228.423584f, 539.291870f, 0.000000f, SPELL_SIMPLE_TELEPORT_SMALL);
                    Varian()->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, 45899);
                });
                Schedule(delay +=  4000, [this]() { Varian()->AI()->Talk(SAY_OUTRO_ALLIANCE_11); });
                Schedule(delay +=  7000, [this]() { Varian()->SetFacingToObject(Saurfang()); });
                Schedule(delay +=  1000, [this]()
                {
                    me->GetMotionMaster()->MovePoint(POINT_ALLIANCE_OUTRO_5, -528.518372f, 2224.766846f, 539.290405f);

                    static std::pair<float, float> followAngleDist[4] =
                    {
                        std::make_pair( M_PI / 2 + M_PI / 4, 1.5f),
                        std::make_pair(-M_PI / 2 - M_PI / 4, 1.5f),
                        std::make_pair( M_PI / 2 + M_PI / 10, 4.5f),
                        std::make_pair(-M_PI / 2 - M_PI / 10, 4.5f),
                    };
                    uint8 i = 0;
                    for (auto&& guard : _guardList)
                    {
                        Position pos = { -535.665039f, 2223.061768f, 539.290405f, 0.0f };
                        pos.RelocateOffset(followAngleDist[i].first, followAngleDist[i].second);
                        guard->GetMotionMaster()->MovePoint(POINT_ALLIANCE_OUTRO_6, pos);
                        ++i;
                    }
                });
                Schedule(delay +=  4000, [this]() { Saurfang()->HandleEmoteStateCommand(EMOTE_ONESHOT_NONE); });
                Schedule(delay +=  1000, [this]() { Saurfang()->GetMotionMaster()->MovePoint(POINT_ALLIANCE_OUTRO_7, -504.248474f, 2219.067871f, 539.285828f); });
                Schedule(delay += 75000, [this]()
                {
                    transport->EnableMovement(true);
                    Jaina()->AI()->Talk(SAY_OUTRO_ALLIANCE_18);
                });
                Schedule(delay +=  4000, [this]() { Varian()->SetFacingToObject(Jaina()); });
                Schedule(delay +=  1000, [this]() { Varian()->AI()->Talk(SAY_OUTRO_ALLIANCE_19); });
                Schedule(delay +=  3000, [this]() { Jaina()->HandleEmoteCommand(EMOTE_ONESHOT_CRY_JAINA); });
                Schedule(delay +=  3000, [this]() { Jaina()->SetFacingToObject(Varian()); });
                Schedule(delay +=  1000, [this]() { Jaina()->AI()->Talk(SAY_OUTRO_ALLIANCE_20); });
                Schedule(delay +=  2000, [this]() { Jaina()->AI()->Talk(SAY_OUTRO_ALLIANCE_21); });
                Schedule(delay += 10000, [this]() { Varian()->AI()->Talk(SAY_OUTRO_ALLIANCE_22); });
                Schedule(delay += 14000, [this]()
                {
                    me->HandleEmoteStateCommand(EMOTE_ONESHOT_NONE);
                    me->SetFacingToObject(Varian());
                });
                Schedule(delay +=   500, [this]()
                {
                    Talk(SAY_OUTRO_ALLIANCE_23);
                    for (auto&& guard : _guardList)
                    {
                        guard->HandleEmoteStateCommand(EMOTE_ONESHOT_NONE);
                        guard->GetMotionMaster()->MoveTargetedHome();
                    }
                });
                Schedule(delay +=  5500, [this]()
                {
                    if (Creature* varian = Varian())
                    {
                        varian->CastSpell(varian, SPELL_SIMPLE_TELEPORT_SMALL, true);
                        varian->DespawnOrUnsummon(1000);
                    }
                });
                Schedule(delay +=  1000, [this]()
                {
                    if (Creature* jaina = Jaina())
                    {
                        jaina->CastSpell(jaina, SPELL_SIMPLE_TELEPORT_SMALL, true);
                        jaina->DespawnOrUnsummon(1000);
                    }
                });
                Schedule(delay +=  1000, [this]() { me->GetMotionMaster()->MoveTargetedHome(); });
                Schedule(delay +=  4000, [this]()
                {
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    transport->AddObjectToRemoveList();
                    transport = nullptr;

                    me->SetPhaseMask(PHASEMASK_NORMAL | 128, false);
                    std::list<GameObject*> teleporters;
                    GetGameObjectListWithEntryInGrid(teleporters, me, GO_ALLIANCE_TELEPORTER, 100.0f);
                    me->SetPhaseMask(PHASEMASK_NORMAL, false);
                    for (auto&& teleporter : teleporters)
                        teleporter->SetPhaseMask(PHASEMASK_NORMAL, true);
                });
                Schedule(delay +=  1000, [this]()
                {
                    std::list<GameObject*> teleporters;
                    GetGameObjectListWithEntryInGrid(teleporters, me, GO_ALLIANCE_TELEPORTER, 100.0f);
                    for (auto&& teleporter : teleporters)
                        teleporter->SetGoState(GO_STATE_ACTIVE);
                });
                Schedule(delay +=  5000, [this]()
                {
                    std::list<GameObject*> teleporters;
                    GetGameObjectListWithEntryInGrid(teleporters, me, GO_ALLIANCE_TELEPORTER, 100.0f);
                    for (auto&& teleporter : teleporters)
                        TeleportIn(NPC_SE_ALLIANCE_MASON, teleporter->GetPositionX(), teleporter->GetPositionY(), teleporter->GetPositionZ(), 0.0f, SPELL_SIMPLE_TELEPORT_SMALL);
                });
                Schedule(delay += 28000, [this]()
                {
                    me->SetPhaseMask(PHASEMASK_NORMAL | 128, false);
                    std::list<GameObject*> buildings;
                    GetGameObjectListWithEntryInGrid(buildings, me, GO_ALLIANCE_TENT, 100.0f);
                    GetGameObjectListWithEntryInGrid(buildings, me, GO_BLACKSMITHS_ANVIL_A, 100.0f);
                    GetGameObjectListWithEntryInGrid(buildings, me, GO_FORGE_A, 100.0f);
                    GetGameObjectListWithEntryInGrid(buildings, me, GO_ALLIANCE_BANNER, 100.0f);
                    me->SetPhaseMask(PHASEMASK_NORMAL, false);
                    for (auto&& building : buildings)
                        building->SetPhaseMask(PHASEMASK_NORMAL, true);
                });
                Schedule(delay +=  9000, [this]()
                {
                    std::list<GameObject*> teleporters;
                    GetGameObjectListWithEntryInGrid(teleporters, me, GO_ALLIANCE_TELEPORTER, 100.0f);
                    for (auto&& teleporter : teleporters)
                        TeleportIn(teleporter->GetPositionY() > 2210.0f ? NPC_SE_BRAZIE_GETZ : NPC_SE_SHELY_STEELBOWELS, teleporter->GetPositionX(), teleporter->GetPositionY(), teleporter->GetPositionZ(), 0.0f, SPELL_SIMPLE_TELEPORT_SMALL);
                });
                Schedule(delay +=  1000, [this]() { me->setActive(false); });
                break;
            }
            case ACTION_INTERRUPT_INTRO:
                _events.Reset();
                for (auto&& guard : _guardList)
                    guard->DespawnOrUnsummon();
                break;
            case ACTION_RESET_INTRO:
                me->SetFlying(false);
                me->ResetInhabitTypeOverride();
                EnterEvadeMode();
                DespawnHelper(me);
                for (auto&& guard : _guardList)
                {
                    guard->SetFlying(false);
                    guard->ResetInhabitTypeOverride();
                    guard->AI()->EnterEvadeMode();
                    DespawnHelper(guard);
                }
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
            default:
                break;
        }
    }

    void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_GRIP_OF_AGONY)
        {
            me->SetFlying(true);
            me->OverrideInhabitType(INHABIT_AIR);
            me->GetMotionMaster()->MoveTakeoff(POINT_CHOKE, chokePos[0], false);
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type == WAYPOINT_MOTION_TYPE && pointId == POINT_EXIT)
        {
            std::list<Creature*> guards;
            GetCreatureListWithEntryInGrid(guards, me, NPC_ALLIANCE_COMMANDER, 50.0f);
            for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                (*itr)->DespawnOrUnsummon();
            me->DespawnOrUnsummon();
        }
        else if (type == POINT_MOTION_TYPE)
        {
            switch (pointId)
            {
                case POINT_ALLIANCE_OUTRO_0:
                case POINT_ALLIANCE_OUTRO_5:
                    me->SetFacingTo(0.0f);
                    break;
                case POINT_ALLIANCE_OUTRO_1:
                    me->GetMotionMaster()->MovePoint(POINT_ALLIANCE_OUTRO_2, -522.205078f, 2226.709961f, 539.290283f);
                    break;
                default:
                    break;
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);
        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_INTRO_ALLIANCE_4:
                    Talk(SAY_INTRO_ALLIANCE_4);
                    _events.ScheduleEvent(EVENT_INTRO_ALLIANCE_5, 6500, 0, PHASE_INTRO_A);
                    if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_DEATHBRINGER_SAURFANG)))
                        deathbringer->AI()->DoAction(ACTION_CONTINUE_INTRO);
                    break;
                case EVENT_INTRO_ALLIANCE_5:
                    Talk(SAY_INTRO_ALLIANCE_5);
                    for (std::list<Creature*>::iterator itr = _guardList.begin(); itr != _guardList.end(); ++itr)
                        (*itr)->AI()->DoAction(ACTION_CHARGE);
                    me->GetMotionMaster()->MoveCharge(chargePos[0].GetPositionX(), chargePos[0].GetPositionY(), chargePos[0].GetPositionZ(), 8.5f, POINT_CHARGE);
                    break;
            }
        }
    }

private:
    EventMap _events;
    InstanceScript* _instance;
    std::list<Creature*> _guardList;
    Transport* transport = nullptr;
    ObjectGuid saurfangTransportGUID = ObjectGuid::Empty;
    ObjectGuid saurfangGUID = ObjectGuid::Empty;
    void Schedule(uint32 delay, std::function<void()> const& func)
    {
        me->m_Events.Schedule(delay, func);
    }
    Creature* Saurfang() const
    {
        return ObjectAccessor::GetCreature(*me, saurfangGUID);
    }
    Creature* Varian() const
    {
        return me->FindNearestCreature(NPC_SE_KING_VARIAN_WRYNN, 100.0f);
    }
    Creature* Jaina() const
    {
        return me->FindNearestCreature(NPC_SE_JAINA_PROUDMOORE, 100.0f);
    }
    Creature* TeleportIn(uint32 entry, float x, float y, float z, float o, uint32 spell = 0, uint32 duration = 0) const
    {
        Creature* creature = me->SummonCreature(entry, x, y, z, o, duration ? TEMPSUMMON_TIMED_DESPAWN : TEMPSUMMON_MANUAL_DESPAWN, Milliseconds(duration));
        if (creature && spell)
            creature->CastSpell(creature, spell, true);
        return creature;
    }
};

class npc_saurfang_event : public CreatureScript
{
    public:
        npc_saurfang_event() : CreatureScript("npc_saurfang_event") { }

        struct npc_saurfang_eventAI : public ScriptedAI
        {
            npc_saurfang_eventAI(Creature* creature) : ScriptedAI(creature)
            {
                _index = 0;

                if (me->GetInstanceScript()->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                {
                    if (me->GetDBTableGUIDLow() == 191915)
                    {
                        Position pos = { -561.121521f, 2213.168945f, 539.285339f, 0.0f };
                        me->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
                        me->SetHomePosition(pos);
                    }
                    else if (me->GetDBTableGUIDLow() == 192375)
                    {
                        Position pos = { -561.097290f, 2209.641113f, 539.285339f, 0.0f };
                        me->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
                        me->SetHomePosition(pos);
                    }
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                ASSERT(!type && data && data < 6);
                _index = data;
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_GRIP_OF_AGONY)
                {
                    me->SetFlying(true);
                    me->OverrideInhabitType(INHABIT_AIR);
                    me->GetMotionMaster()->MoveTakeoff(POINT_CHOKE, chokePos[_index], false);
                }
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_CHARGE && _index)
                    me->GetMotionMaster()->MoveCharge(chargePos[_index].GetPositionX(), chargePos[_index].GetPositionY(), chargePos[_index].GetPositionZ(), 13.0f, POINT_CHARGE);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT_ALLIANCE_OUTRO_3:
                        me->SetFacingTo(1.687237f);
                        break;
                    case POINT_ALLIANCE_OUTRO_6:
                        me->SetFacingTo(0.0f);
                        break;
                    default:
                        break;
                }
            }

        private:
            uint32 _index;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_saurfang_eventAI>(creature);
        }
};

class npc_saurfang_event_post_npc : public CreatureScript
{
    public:
        npc_saurfang_event_post_npc() : CreatureScript("npc_saurfang_event_post_npc") { }

        struct npc_saurfang_event_post_npcAI : public ScriptedAI
        {
            npc_saurfang_event_post_npcAI(Creature* creature) : ScriptedAI(creature) { }

            void IsSummonedBy(Unit* summoner) override
            {
                float x, y, z, o;
                me->GetPosition(x, y, z, o);

                uint32 delay = 0;
                if (IsWorker())
                {
                    if (y > 2210.0f)
                    {
                        delay = 3000;
                        waypoints.push(Position { -542.152710f, 2221.169678f, 539.290100f });
                        waypoints.push(Position { -529.863220f, 2227.263916f, 539.290100f, 2.403925f });

                        returnDelay = 23000;
                        waypointsBack.push(Position { -536.226563f, 2214.088623f, 539.290100f });
                        waypointsBack.push(Position { -552.795654f, 2215.481201f, 539.290100f });
                        waypointsBack.push(Position { x, y, z, o });

                        me->m_Events.Schedule(2500, EVENT_GROUP_POST_NPC, [this]() { me->PlayDistanceSound(IsAlliance() ? 6290 : 6192); });
                    }
                    else
                    {
                        delay = 5000;
                        waypoints.push(Position { -541.578552f, 2201.894775f, 539.290955f });
                        waypoints.push(Position { -528.811157f, 2223.288574f, 539.290955f });
                        waypoints.push(Position { -524.132568f, 2232.325439f, 539.290955f, 2.345018f });

                        returnDelay = 18000;
                        waypointsBack.push(Position { -533.073792f, 2205.677002f, 539.290955f });
                        waypointsBack.push(Position { -549.517700f, 2202.911377f, 539.290955f });
                        waypointsBack.push(Position { x, y, z, o });

                        me->m_Events.Schedule(4000, EVENT_GROUP_POST_NPC, [this]() { me->PlayDistanceSound(IsAlliance() ? 7094 : 7194); });
                    }
                }
                else
                {
                    delay = 1000;
                    me->SetWalk(true);
                    if (y > 2210.0f)
                    {
                        waypoints.push(Position { -536.951416f, 2223.258057f, 539.290222f });
                        waypoints.push(Position { -529.531982f, 2223.770752f, 539.290222f });
                        if (IsAlliance())
                        {
                            waypoints.push(Position { -527.702576f, 2225.215332f, 539.290222f });
                            waypoints.push(Position { -530.619995f, 2226.617920f, 539.291870f, 5.729299f });
                        }
                        else
                            waypoints.push(Position { -529.889099f, 2226.092773f, 539.291870f, 5.660145f });
                    }
                    else
                    {
                        waypoints.push(Position { -548.614746f, 2202.755615f, 539.291992f });
                        waypoints.push(Position { -544.600037f, 2207.085449f, 539.291992f });
                        waypoints.push(Position { -528.183105f, 2225.423340f, 539.291992f });
                        if (IsAlliance())
                            waypoints.push(Position { -526.355042f, 2231.519043f, 539.292114f, 5.654691f });
                        else
                            waypoints.push(Position { -521.246460f, 2231.718750f, 539.290161f, 5.298303f });
                    }
                }

                me->m_Events.Schedule(delay, EVENT_GROUP_POST_NPC, [this]() { me->GetMotionMaster()->MovePoint(0, GetWaypoints().front()); });
            }

            void MovementInform(uint32 type, uint32 /*pointId*/) override
            {
                if (type != POINT_MOTION_TYPE || GetWaypoints().empty())
                    return;

                Position current = GetWaypoints().front();
                GetWaypoints().pop();
                if (!GetWaypoints().empty())
                {
                    me->GetMotionMaster()->MovePoint(0, GetWaypoints().front());
                    return;
                }

                me->SetFacingTo(current.GetOrientation());

                if (IsWorker())
                {
                    if (returning)
                    {
                        me->DespawnOrUnsummon(me->GetPositionY() > 2210.0f ? 1000 : 3000);
                        return;
                    }

                    me->m_Events.Schedule(2000, [this]() { me->HandleEmoteStateCommand(EMOTE_STATE_WORK); });
                    me->m_Events.Schedule(returnDelay, [this]()
                    {
                        returning = true;
                        me->HandleEmoteStateCommand(EMOTE_ONESHOT_NONE);
                        me->GetMotionMaster()->MovePoint(0, GetWaypoints().front());
                    });
                }
            }

            bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
            {
                if (sender == 10949 && action == 1)
                    player->CLOSE_GOSSIP_MENU();
                return true;
            }

        private:
            bool returning = false;
            uint32 returnDelay = 0;
            std::queue<Position> waypoints;
            std::queue<Position> waypointsBack;

            std::queue<Position>& GetWaypoints() { return returning ? waypointsBack : waypoints; }
            bool IsAlliance() const { return me->GetEntry() == NPC_SE_ALLIANCE_MASON || me->GetEntry() == NPC_SE_SHELY_STEELBOWELS || me->GetEntry() == NPC_SE_BRAZIE_GETZ; }
            bool IsWorker() const { return me->GetEntry() == NPC_SE_ALLIANCE_MASON || me->GetEntry() == NPC_SE_HORDE_PEON; }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_saurfang_event_post_npcAI>(creature);
        }
};

class spell_deathbringer_blood_link : public SpellScriptLoader
{
    public:
        spell_deathbringer_blood_link() : SpellScriptLoader("spell_deathbringer_blood_link") { }

        class spell_deathbringer_blood_link_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deathbringer_blood_link_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BLOOD_LINK_POWER))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_BLOOD_POWER))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                GetHitUnit()->CastCustomSpell(SPELL_BLOOD_LINK_POWER, SPELLVALUE_BASE_POINT0, GetEffectValue(), GetHitUnit(), true);
                PreventHitDefaultEffect(EFFECT_0);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_deathbringer_blood_link_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_deathbringer_blood_link_SpellScript();
        }
};

class spell_deathbringer_blood_link_aura : public SpellScriptLoader
{
    public:
        spell_deathbringer_blood_link_aura() : SpellScriptLoader("spell_deathbringer_blood_link_aura") { }

        class spell_deathbringer_blood_link_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_deathbringer_blood_link_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_MARK_OF_THE_FALLEN_CHAMPION))
                    return false;
                return true;
            }

            void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                if (GetUnitOwner()->GetPowerType() == POWER_ENERGY && GetUnitOwner()->GetPower(POWER_ENERGY) == GetUnitOwner()->GetMaxPower(POWER_ENERGY))
                    if (Creature* saurfang = GetUnitOwner()->ToCreature())
                        saurfang->AI()->DoAction(ACTION_MARK_OF_THE_FALLEN_CHAMPION);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_deathbringer_blood_link_AuraScript::HandlePeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_deathbringer_blood_link_AuraScript();
        }
};

class spell_deathbringer_blood_power : public SpellScriptLoader
{
    public:
        spell_deathbringer_blood_power() : SpellScriptLoader("spell_deathbringer_blood_power") { }

        class spell_deathbringer_blood_power_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deathbringer_blood_power_SpellScript);

            void ModAuraValue()
            {
                if (Aura* aura = GetHitAura())
                    aura->RecalculateAmountOfEffects();
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_deathbringer_blood_power_SpellScript::ModAuraValue);
            }
        };

        class spell_deathbringer_blood_power_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_deathbringer_blood_power_AuraScript);

            void RecalculateHook(AuraEffect const* /*aurEffect*/, float& amount, bool& canBeRecalculated)
            {
                amount = int32(GetUnitOwner()->GetPower(POWER_ENERGY));
                canBeRecalculated = true;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_deathbringer_blood_power_AuraScript::RecalculateHook, EFFECT_0, SPELL_AURA_MOD_SCALE);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_deathbringer_blood_power_AuraScript::RecalculateHook, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_deathbringer_blood_power_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_deathbringer_blood_power_AuraScript();
        }
};

class spell_deathbringer_rune_of_blood : public SpellScriptLoader
{
    public:
        spell_deathbringer_rune_of_blood() : SpellScriptLoader("spell_deathbringer_rune_of_blood") { }

        class spell_deathbringer_rune_of_blood_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deathbringer_rune_of_blood_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BLOOD_LINK_DUMMY))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);  // make this the default handler
                if (GetCaster()->GetPower(POWER_ENERGY) != GetCaster()->GetMaxPower(POWER_ENERGY))
                    GetHitUnit()->CastCustomSpell(SPELL_BLOOD_LINK_DUMMY, SPELLVALUE_BASE_POINT0, GetCaster()->GetMap()->IsHeroic() ? 2 : 1, GetCaster(), true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_deathbringer_rune_of_blood_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_deathbringer_rune_of_blood_SpellScript();
        }
};

class spell_deathbringer_blood_nova : public SpellScriptLoader
{
    public:
        spell_deathbringer_blood_nova() : SpellScriptLoader("spell_deathbringer_blood_nova") { }

        class spell_deathbringer_blood_nova_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deathbringer_blood_nova_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BLOOD_LINK_DUMMY))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);  // make this the default handler
                if (GetCaster()->GetPower(POWER_ENERGY) != GetCaster()->GetMaxPower(POWER_ENERGY))
                    GetHitUnit()->CastCustomSpell(SPELL_BLOOD_LINK_DUMMY, SPELLVALUE_BASE_POINT0, GetCaster()->GetMap()->IsHeroic() ? 4 : 2, GetCaster(), true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_deathbringer_blood_nova_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_deathbringer_blood_nova_SpellScript();
        }
};

class spell_deathbringer_blood_nova_targeting : public SpellScriptLoader
{
    public:
        spell_deathbringer_blood_nova_targeting() : SpellScriptLoader("spell_deathbringer_blood_nova_targeting") { }

        class spell_deathbringer_blood_nova_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deathbringer_blood_nova_targeting_SpellScript);

            bool Load() override
            {
                // initialize variable
                target = nullptr;
                return true;
            }

            void FilterTargetsInitial(std::list<WorldObject*>& targets)
            {
                if (targets.empty())
                    return;

                // select one random target, with preference of ranged targets
                uint32 targetsAtRange = 0;
                uint32 const minTargets = uint32(GetCaster()->GetMap()->Is25ManRaid() ? 10 : 4);
                targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster(), false));

                // get target count at range
                for (std::list<WorldObject*>::iterator itr = targets.begin(); itr != targets.end(); ++itr, ++targetsAtRange)
                    if ((*itr)->GetDistance(GetCaster()) < 12.0f)
                        break;

                // If not enough ranged targets are present just select anyone
                if (targetsAtRange < minTargets)
                    targetsAtRange = uint32(targets.size());

                std::list<WorldObject*>::const_iterator itr = targets.begin();
                std::advance(itr, urand(0, targetsAtRange - 1));
                target = *itr;
                targets.clear();
                targets.push_back(target);
            }

            // use the same target for first and second effect
            void FilterTargetsSubsequent(std::list<WorldObject*>& unitList)
            {
                unitList.clear();
                if (!target)
                    return;

                unitList.push_back(target);
            }

            void HandleForceCast(SpellEffIndex /*effIndex*/)
            {
                GetCaster()->CastSpell(GetHitUnit(), uint32(GetEffectValue()), TRIGGERED_FULL_MASK);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_deathbringer_blood_nova_targeting_SpellScript::FilterTargetsInitial, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_deathbringer_blood_nova_targeting_SpellScript::FilterTargetsSubsequent, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_deathbringer_blood_nova_targeting_SpellScript::HandleForceCast, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
            }

            WorldObject* target;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_deathbringer_blood_nova_targeting_SpellScript();
        }
};

class achievement_ive_gone_and_made_a_mess : public AchievementCriteriaScript
{
    public:
        achievement_ive_gone_and_made_a_mess() : AchievementCriteriaScript("achievement_ive_gone_and_made_a_mess") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            if (target)
                if (Creature* saurfang = target->ToCreature())
                    if (saurfang->AI()->GetData(DATA_MADE_A_MESS))
                        return true;

            return false;
        }
};

/*class spell_deathbringer_boiling_blood : public SpellScriptLoader // TODO: remove
{
    public:
        spell_deathbringer_boiling_blood() : SpellScriptLoader("spell_deathbringer_boiling_blood") { }

        class spell_deathbringer_boiling_blood_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deathbringer_boiling_blood_SpellScript);

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_UNIT;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove(GetCaster()->GetVictim());
                if (targets.empty())
                    return;

                WorldObject* target = Trinity::Containers::SelectRandomContainerElement(targets);
                targets.clear();
                targets.push_back(target);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_deathbringer_boiling_blood_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_deathbringer_boiling_blood_SpellScript();
        }
};*/

class spell_deathbringer_boiling_blood : public SpellScriptLoader
{
    public:
        spell_deathbringer_boiling_blood() : SpellScriptLoader("spell_deathbringer_boiling_blood") { }

        class spell_deathbringer_boiling_blood_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_deathbringer_boiling_blood_AuraScript);

            void OnPeriodic(AuraEffect const* aurEff)
            {
                Unit* caster = GetCaster();
                Unit* target = GetTarget();
                if (!caster || !target)
                    return;

                uint32 absorb = 0;
                for (auto aura : target->GetAuraEffectsByType(SPELL_AURA_SCHOOL_ABSORB))
                    if (aura->GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL)
                        absorb += aura->GetAmount();

                if (absorb)
                {
                    // Calculate approximate amount of damage that will be dealt
                    uint32 damage = std::max(aurEff->GetAmount(), 0);
                    damage = caster->SpellDamageBonusDone(target, GetSpellInfo(), aurEff->GetEffIndex(), damage, DOT, GetStackAmount());
                    damage = target->SpellDamageBonusTaken(caster, GetSpellInfo(), aurEff->GetEffIndex(), damage, DOT, GetStackAmount());
                    if (Unit::IsDamageReducedByArmor(GetSpellInfo()->GetSchoolMask(), GetSpellInfo(), aurEff->GetEffIndex()))
                        damage = caster->CalcArmorReducedDamage(target, damage, GetSpellInfo());

                    if (absorb >= damage)
                        return;
                }

                target->CastCustomSpell(SPELL_BLOOD_LINK_DUMMY, SPELLVALUE_BASE_POINT0, caster->GetMap()->IsHeroic() ? 2 : 1, caster, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_deathbringer_boiling_blood_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_deathbringer_boiling_blood_AuraScript();
        }
};

class npc_bloodbeast : public CreatureScript
{
    public:
        npc_bloodbeast() : CreatureScript("npc_bloodbeast") { }

        struct npc_bloodbeastAI : public ScriptedAI
        {
            npc_bloodbeastAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();

                me->m_Events.Repeated(5000, 5000, [this]
                {
                    if (!me->IsInCombat() || me->isDead())
                        return true;

                    if (!me->GetVictim())
                        return false;

                    PathGenerator path { me };
                    if (!path.CalculatePath(me->GetVictim()->GetPositionX(), me->GetVictim()->GetPositionY(), me->GetVictim()->GetPositionZ()) ||
                        path.GetPathType() & (PATHFIND_NOPATH | PATHFIND_INCOMPLETE))
                    {
                        if (Creature* saurfang = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_DEATHBRINGER_SAURFANG)))
                            saurfang->AI()->EnterEvadeMode();

                        return true;
                    }

                    return false;
                });
            }

            uint32 Jet_Timer;
            bool knockedBack = false;

            void Reset() override
            {
                Jet_Timer = 5000;
                DoCast(me, SPELL_RESISTANT_SKIN, true);
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!knockedBack && me->HasUnitState(UNIT_STATE_JUMPING))
                    knockedBack = true;

                if (knockedBack && !me->HasUnitState(UNIT_STATE_JUMPING))
                {
                    knockedBack = false;
                    DoCast(me, 65256, true); // Self Stun (1 secs)
                }

                if (IsHeroic())
                {
                    if (Jet_Timer <= diff)
                    {
                        DoCast(me, SPELL_SCENT_OF_BLOOD);
                        Jet_Timer = 40000;
                    } else Jet_Timer -= diff;
                }

                DoMeleeAttackIfReady();
            }

            private:
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_bloodbeastAI(creature);
        }
};

class spell_deathbringer_blood_link_bloodbeast : public SpellScriptLoader
{
    public:
        spell_deathbringer_blood_link_bloodbeast() : SpellScriptLoader("spell_deathbringer_blood_link_bloodbeast") { }

        class spell_deathbringer_blood_link_bloodbeast_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_deathbringer_blood_link_bloodbeast_AuraScript);

            void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();
                if (Unit* caster = GetCaster())
                    caster->CastCustomSpell(SPELL_BLOOD_LINK_DUMMY, SPELLVALUE_BASE_POINT0, 3, caster, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_deathbringer_blood_link_bloodbeast_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_deathbringer_blood_link_bloodbeast_AuraScript();
        }
};

class spell_deathbringer_remove_marks : public SpellScriptLoader
{
    public:
        spell_deathbringer_remove_marks() : SpellScriptLoader("spell_deathbringer_remove_marks") { }

        class spell_deathbringer_remove_marks_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deathbringer_remove_marks_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetHitUnit()->RemoveAurasDueToSpell(uint32(GetEffectValue()));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_deathbringer_remove_marks_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_deathbringer_remove_marks_SpellScript();
        }
};

class BloodTargetFilter
{
    public:
        BloodTargetFilter() { }

        bool operator()(WorldObject* target)
        {
            if (Unit* unit = target->ToUnit())
                return unit->HasAura(SPELL_SCENT_OF_BLOOD);
            //if (unit->GetTypeId() != TYPEID_PLAYER)
            //    return false;
            return true;
        }
};

// 72769
class spell_scent_of_blood : public SpellScriptLoader
{
    public:
        spell_scent_of_blood() : SpellScriptLoader("spell_scent_of_blood") { }

        class spell_scent_of_blood_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_scent_of_blood_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(BloodTargetFilter());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_scent_of_blood_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_scent_of_blood_SpellScript();
        }
};

void AddSC_boss_deathbringer_saurfang()
{
    new boss_deathbringer_saurfang();
    RegisterIcecrownCitadelCreatureAI(npc_high_overlord_saurfang_icc);
    RegisterIcecrownCitadelCreatureAI(npc_muradin_bronzebeard_icc);
    new npc_bloodbeast();
    new npc_saurfang_event();
    new npc_saurfang_event_post_npc();
    new spell_deathbringer_blood_link();
    new spell_deathbringer_blood_link_aura();
    new spell_deathbringer_blood_power();
    new spell_deathbringer_rune_of_blood();
    new spell_deathbringer_blood_nova();
    new spell_deathbringer_blood_nova_targeting();
    new spell_deathbringer_boiling_blood();
    new spell_deathbringer_blood_link_bloodbeast();
    new achievement_ive_gone_and_made_a_mess();
    new spell_deathbringer_remove_marks();
    new spell_scent_of_blood();
}
