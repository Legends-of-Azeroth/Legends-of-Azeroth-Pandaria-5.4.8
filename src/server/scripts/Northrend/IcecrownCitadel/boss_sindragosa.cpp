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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellAuraEffects.h"
#include "icecrown_citadel.h"
#include "VMapFactory.h"

enum Texts
{
    SAY_AGGRO                           = 0, // You are fools to have come to this place! The icy winds of Northrend will consume your souls!
    SAY_UNCHAINED_MAGIC                 = 1, // Suffer, mortals, as your pathetic magic betrays you!
    EMOTE_WARN_BLISTERING_COLD          = 2, // %s prepares to unleash a wave of blistering cold!
    SAY_BLISTERING_COLD                 = 3, // Can you feel the cold hand of death upon your heart?
    SAY_RESPITE_FOR_A_TORMENTED_SOUL    = 4, // Aaah! It burns! What sorcery is this?!
    SAY_AIR_PHASE                       = 5, // Your incursion ends here! None shall survive!
    SAY_PHASE_2                         = 6, // Now feel my master's limitless power and despair!
    EMOTE_WARN_FROZEN_ORB               = 7, // %s fires a frozen orb towards $N!
    SAY_KILL                            = 8, // Perish!
                                             // A flaw of mortality...
    SAY_BERSERK                         = 9, // Enough! I tire of these games!
    SAY_DEATH                           = 10, // Free...at last...
    EMOTE_BERSERK_RAID                  = 11
};

enum Spells
{
    // Sindragosa
    SPELL_SINDRAGOSA_S_FURY     = 70608,
    SPELL_TANK_MARKER           = 71039,
    SPELL_FROST_AURA            = 70084,
    SPELL_PERMAEATING_CHILL     = 70109,
    SPELL_CLEAVE                = 19983,
    SPELL_TAIL_SMASH            = 71077,
    SPELL_FROST_BREATH_P1       = 69649,
    SPELL_FROST_BREATH_P2       = 73061,
    SPELL_UNCHAINED_MAGIC       = 69762,
    SPELL_BACKLASH              = 69770,
    SPELL_ICY_GRIP              = 70117,
    SPELL_ICY_GRIP_JUMP         = 70122,
    SPELL_BLISTERING_COLD       = 70123,
    SPELL_FROST_BEACON          = 70126,
    SPELL_ICE_TOMB_TARGET       = 69712,
    SPELL_ICE_TOMB_DUMMY        = 69675,
    SPELL_ICE_TOMB_UNTARGETABLE = 69700,
    SPELL_ICE_TOMB_DAMAGE       = 70157,
    SPELL_ASPHYXIATION          = 71665,
    SPELL_FROST_BOMB_TRIGGER    = 69846,
    SPELL_FROST_BOMB_VISUAL     = 70022,
    SPELL_BIRTH_NO_VISUAL       = 40031,
    SPELL_FROST_BOMB            = 69845,
    SPELL_MYSTIC_BUFFET         = 70128,
    SPELL_MYSTIC_BUFFET_AURA    = 70127,

    // Spinestalker
    SPELL_BELLOWING_ROAR        = 36922,
    SPELL_CLEAVE_SPINESTALKER   = 40505,
    SPELL_TAIL_SWEEP            = 71370,

    // Rimefang
    SPELL_FROST_BREATH          = 71386,
    SPELL_FROST_AURA_RIMEFANG   = 71387,
    SPELL_ICY_BLAST             = 71376,
    SPELL_ICY_BLAST_AREA        = 71380,

    // Frostwarden Handler
    SPELL_FOCUS_FIRE            = 71350,
    SPELL_ORDER_WHELP           = 71357,
    SPELL_CONCUSSIVE_SHOCK      = 71337,

    // Misc
    SPELL_FROST_INFUSION_CREDIT = 72289,
};

enum Events
{
    // Sindragosa
    EVENT_BERSERK                   = 1,
    EVENT_CLEAVE                    = 2,
    EVENT_TAIL_SMASH                = 3,
    EVENT_FROST_BREATH              = 4,
    EVENT_UNCHAINED_MAGIC           = 5,
    EVENT_ICY_GRIP                  = 6,
    EVENT_BLISTERING_COLD           = 7,
    EVENT_BLISTERING_COLD_YELL      = 8,
    EVENT_AIR_PHASE                 = 9,
    EVENT_ICE_TOMB                  = 10,
    EVENT_FROST_BOMB                = 11,
    EVENT_LAND                      = 12,
    EVENT_AIR_MOVEMENT              = 21,
    EVENT_THIRD_PHASE_CHECK         = 22,
    EVENT_AIR_MOVEMENT_FAR          = 23,
    EVENT_LAND_GROUND               = 24,
    EVENT_CHECK_PLAYERS             = 25,

    // Spinestalker
    EVENT_BELLOWING_ROAR            = 13,
    EVENT_CLEAVE_SPINESTALKER       = 14,
    EVENT_TAIL_SWEEP                = 15,

    // Rimefang
    EVENT_FROST_BREATH_RIMEFANG     = 16,
    EVENT_ICY_BLAST                 = 17,
    EVENT_ICY_BLAST_CAST            = 18,

    // Trash
    EVENT_FROSTWARDEN_ORDER_WHELP   = 19,
    EVENT_CONCUSSIVE_SHOCK          = 20,

    // event groups
    EVENT_GROUP_LAND_PHASE          = 1,
};

enum FrostwingData
{
    DATA_MYSTIC_BUFFET_STACK    = 0,
    DATA_FROSTWYRM_OWNER        = 1,
    DATA_WHELP_MARKER           = 2,
    DATA_LINKED_GAMEOBJECT      = 3,
    DATA_TRAPPED_PLAYER         = 4,
    DATA_ENABLE_ASPHYXIATION    = 5,
};

enum MovementPoints
{
    POINT_FROSTWYRM_FLY_IN  = 1,
    POINT_FROSTWYRM_LAND    = 2,
    POINT_AIR_PHASE         = 3,
    POINT_TAKEOFF           = 4,
    POINT_LAND              = 5,
    POINT_AIR_PHASE_FAR     = 6,
    POINT_LAND_GROUND       = 7,
};

enum Shadowmourne
{
    QUEST_FROST_INFUSION        = 24757,
    ITEM_SHADOW_S_EDGE          = 49888,

    SPELL_FROST_INFUSION        = 72292,
    SPELL_FROST_IMBUED_BLADE    = 72290,
};

Position const RimefangFlyPos      = {4413.309f, 2456.421f, 233.3795f, 2.890186f};
Position const RimefangLandPos     = {4413.309f, 2456.421f, 203.3848f, 2.890186f};
Position const SpinestalkerFlyPos  = {4418.895f, 2514.233f, 230.4864f, 3.396045f};
Position const SpinestalkerLandPos = {4418.895f, 2514.233f, 203.3848f, 3.396045f};
Position const SindragosaSpawnPos  = {4818.700f, 2483.710f, 287.0650f, 3.089233f};
Position const SindragosaFlyPos    = {4475.190f, 2484.570f, 234.8510f, 3.141593f};
Position const SindragosaLandPos   = {4419.080f, 2484.360f, 203.3848f, 3.141593f};
Position const SindragosaAirPos    = {4475.990f, 2484.430f, 247.9340f, M_PI};
Position const SindragosaAirPosFar = {4525.600f, 2485.150f, 245.0820f, M_PI};
Position const SindragosaFlyInPos  = {4419.080f, 2484.360f, 232.5150f, M_PI};

class boss_sindragosa : public CreatureScript
{
    public:
        boss_sindragosa() : CreatureScript("boss_sindragosa") { }

        struct boss_sindragosaAI : public BossAI
        {
            boss_sindragosaAI(Creature* creature) : BossAI(creature, DATA_SINDRAGOSA) { }

            void Reset() override
            {
                BossAI::Reset();
                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_MYSTIC_BUFFET_AURA, true);
                me->ResetLootMode();
                DoCast(me, SPELL_TANK_MARKER, true);
                events.ScheduleEvent(EVENT_BERSERK, 600000);
                events.ScheduleEvent(EVENT_CLEAVE, 10000, EVENT_GROUP_LAND_PHASE);
                events.ScheduleEvent(EVENT_TAIL_SMASH, 20000, EVENT_GROUP_LAND_PHASE);
                events.ScheduleEvent(EVENT_FROST_BREATH, urand(8000, 12000), EVENT_GROUP_LAND_PHASE);
                events.ScheduleEvent(EVENT_UNCHAINED_MAGIC, urand(9000, 14000), EVENT_GROUP_LAND_PHASE);
                events.ScheduleEvent(EVENT_ICY_GRIP, 33500, EVENT_GROUP_LAND_PHASE);
                events.ScheduleEvent(EVENT_AIR_PHASE, 50000);
                events.ScheduleEvent(EVENT_CHECK_PLAYERS, 5000);
                _frostBombCounter = 0;
                _iceTombCounter = 0;
                _mysticBuffetStack = 0;
                _isInAirPhase = false;
                _isThirdPhase = false;

                if (instance->GetData(DATA_SINDRAGOSA_FROSTWYRMS) != 255)
                {
                    me->SetFlying(true);
                    me->OverrideInhabitType(INHABIT_GROUND | INHABIT_AIR);
                }
                else
                    me->OverrideInhabitType(INHABIT_GROUND);

                me->GetMap()->SetWorldState(WORLDSTATE_ALL_YOU_CAN_EAT, 1);
            }

            void JustDied(Unit* killer) override
            {
                BossAI::JustDied(killer);
                Talk(SAY_DEATH);
                DoCast(me, SPELL_FROST_INFUSION_CREDIT);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!instance->CheckRequiredBosses(DATA_SINDRAGOSA, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(LIGHT_S_HAMMER_TELEPORT);
                    return;
                }

                BossAI::JustEngagedWith(who);
                instance->SetBossState(DATA_SINDRAGOSA, IN_PROGRESS);
                DoCast(me, SPELL_FROST_AURA);
                DoCast(me, SPELL_PERMAEATING_CHILL);
                Talk(SAY_AGGRO);
            }

            void JustReachedHome() override
            {
                BossAI::JustReachedHome();
                instance->SetBossState(DATA_SINDRAGOSA, FAIL);
                me->DespawnOrUnsummon();
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_START_FROSTWYRM)
                {
                    if (instance->GetBossState(DATA_SINDRAGOSA_GAUNTLET) != DONE)
                        return;

                    if (TempSummon* summon = me->ToTempSummon())
                        summon->SetTempSummonType(TEMPSUMMON_DEAD_DESPAWN);

                    instance->SetData(DATA_SINDRAGOSA_FROSTWYRMS, 255);
                    if (me->isDead())
                        return;

                    me->setActive(true);
                    me->SetFlying(true);
                    me->OverrideInhabitType(INHABIT_GROUND | INHABIT_AIR);
                    me->SetSpeed(MOVE_RUN, 30.0f / baseMoveSpeed[MOVE_RUN]);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                    me->GetMotionMaster()->MovePoint(POINT_FROSTWYRM_FLY_IN, SindragosaFlyPos);
                    DoCast(me, SPELL_SINDRAGOSA_S_FURY);
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_MYSTIC_BUFFET_STACK)
                    return _mysticBuffetStack;
                return 0xFFFFFFFF;
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT_FROSTWYRM_FLY_IN:
                        me->m_Events.Schedule(1000, [this]()
                        {
                            me->GetMotionMaster()->MoveLand(POINT_FROSTWYRM_LAND, SindragosaLandPos, 8.247422f);
                        });
                        break;
                    case POINT_FROSTWYRM_LAND:
                        me->setActive(false);
                        me->SetFlying(false);
                        me->OverrideInhabitType(INHABIT_GROUND);
                        me->SetHomePosition(SindragosaLandPos);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                        me->SetReactState(REACT_AGGRESSIVE);
                        // Sindragosa enters combat as soon as she lands
                        DoZoneInCombat(me, 100.0f);
                        break;
                    case POINT_TAKEOFF:
                        me->SetFlying(true);
                        me->OverrideInhabitType(INHABIT_GROUND | INHABIT_AIR);
                        events.ScheduleEvent(EVENT_AIR_MOVEMENT, 1);
                        break;
                    case POINT_AIR_PHASE:
                        me->SetFacingTo(M_PI);
                        me->CastCustomSpell(SPELL_ICE_TOMB_TARGET, SPELLVALUE_MAX_TARGETS, RAID_MODE<int32>(2, 5, 2, 6), nullptr);
                        events.ScheduleEvent(EVENT_AIR_MOVEMENT_FAR, 1500);
                        break;
                    case POINT_AIR_PHASE_FAR:
                        me->SetFacingTo(M_PI);
                        break;
                    case POINT_LAND:
                        events.ScheduleEvent(EVENT_LAND_GROUND, 1);
                        break;
                    case POINT_LAND_GROUND:
                    {
                        me->SetFlying(false);
                        me->OverrideInhabitType(INHABIT_GROUND);
                        me->SetReactState(REACT_DEFENSIVE);
                        if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE)
                            me->GetMotionMaster()->MovementExpired();
                        DoStartMovement(me->GetVictim());

                        _isInAirPhase = false;
                        // trigger Asphyxiation
                        EntryCheckPredicate pred(NPC_ICE_TOMB);
                        summons.DoAction(ACTION_TRIGGER_ASPHYXIATION, pred);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/) override
            {
                if (!_isThirdPhase && !HealthAbovePct(35))
                {
                    events.CancelEvent(EVENT_AIR_PHASE);
                    events.CancelEvent(EVENT_ICY_GRIP);
                    events.ScheduleEvent(EVENT_THIRD_PHASE_CHECK, 1000);
                    _isThirdPhase = true;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_ICE_TOMB && _isThirdPhase)
                    summon->AI()->SetData(DATA_ENABLE_ASPHYXIATION, 0);

                if (summon->GetEntry() == NPC_FROST_BOMB)
                {
                    summon->CastSpell(summon, SPELL_FROST_BOMB_VISUAL, true);
                    summon->CastSpell(summon, SPELL_BIRTH_NO_VISUAL, true);
                    summon->m_Events.Schedule(5500, [this, summon]()
                    {
                        summon->CastSpell((Unit*)nullptr, SPELL_FROST_BOMB, false, nullptr, nullptr, me->GetGUID());
                        summon->RemoveAurasDueToSpell(SPELL_FROST_BOMB_VISUAL);
                        summon->DespawnOrUnsummon(2000);
                    });
                }
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                BossAI::SummonedCreatureDespawn(summon);
                if (summon->GetEntry() == NPC_ICE_TOMB)
                    summon->AI()->JustDied(summon);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (uint32 spellId = SPELL_MYSTIC_BUFFET_AURA)
                {
                    if (target->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (spellId == spell->Id)
                    {
                        if (Aura const* mysticBuffet = target->GetAura(spell->Id))
                        {
                            _mysticBuffetStack = std::max<uint8>(_mysticBuffetStack, mysticBuffet->GetStackAmount());

                            if (mysticBuffet->GetStackAmount() > 5)
                                me->GetMap()->SetWorldState(WORLDSTATE_ALL_YOU_CAN_EAT, 0);
                        }

                        return;
                    }
                }

                // Frost Infusion
                if (Player* player = target->ToPlayer())
                {
                    if (Is25ManRaid())
                    if (uint32 spellId = _isThirdPhase ? SPELL_FROST_BREATH_P2 : SPELL_FROST_BREATH_P1)
                    {
                        if (spellId == spell->Id)
                        {
                            Item* shadowsEdge = player->GetWeaponForAttack(BASE_ATTACK, true);
                            if (player->GetQuestStatus(QUEST_FROST_INFUSION) == QUEST_STATUS_INCOMPLETE && shadowsEdge)
                            {
                                if (!player->HasAura(SPELL_FROST_IMBUED_BLADE) && shadowsEdge->GetEntry() == ITEM_SHADOW_S_EDGE)
                                {
                                    if (Aura* infusion = player->GetAura(SPELL_FROST_INFUSION))
                                    {
                                        if (infusion->GetStackAmount() == 3)
                                        {
                                            player->CastSpell(player, SPELL_FROST_IMBUED_BLADE, true);
                                            player->RemoveAura(infusion);
                                        }
                                        else
                                            player->CastSpell(player, SPELL_FROST_INFUSION, true);
                                    }
                                    else
                                        player->CastSpell(player, SPELL_FROST_INFUSION, true);
                                }
                            }

                            return;
                        }
                    }
                }
            }

            void CheckPlayerPositions()
            {
                Map* map = me->GetMap();
                if (map && map->IsDungeon())
                {
                    Map::PlayerList const &PlayerList = map->GetPlayers();

                    if (PlayerList.isEmpty())
                        return;

                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if (i->GetSource())
                            if (i->GetSource()->IsAlive())
                                if (i->GetSource()->GetPositionZ() < 195.0f && i->GetSource()->GetDistance2d(4379.1f, 2485.4f) < 100.0f)
                                    i->GetSource()->TeleportTo(631, 4419.190f, 2484.570f, 205.0f, 3.141593f);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || !CheckInRoom())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            Talk(EMOTE_BERSERK_RAID);
                            Talk(SAY_BERSERK);
                            DoCast(me, SPELL_BERSERK);
                            break;
                        case EVENT_CLEAVE:
                            DoCastVictim(SPELL_CLEAVE);
                            events.ScheduleEvent(EVENT_CLEAVE, urand(15000, 20000), EVENT_GROUP_LAND_PHASE);
                            break;
                        case EVENT_TAIL_SMASH:
                            DoCast(me, SPELL_TAIL_SMASH);
                            events.ScheduleEvent(EVENT_TAIL_SMASH, urand(27000, 32000), EVENT_GROUP_LAND_PHASE);
                            break;
                        case EVENT_FROST_BREATH:
                            DoCastVictim(_isThirdPhase ? SPELL_FROST_BREATH_P2 : SPELL_FROST_BREATH_P1);
                            events.ScheduleEvent(EVENT_FROST_BREATH, urand(20000, 25000), EVENT_GROUP_LAND_PHASE);
                            break;
                        case EVENT_UNCHAINED_MAGIC:
                            Talk(SAY_UNCHAINED_MAGIC);
                            DoCast(me, SPELL_UNCHAINED_MAGIC);
                            events.ScheduleEvent(EVENT_UNCHAINED_MAGIC, urand(30000, 35000), EVENT_GROUP_LAND_PHASE);
                            break;
                        case EVENT_ICY_GRIP:
                            DoCast(me, SPELL_ICY_GRIP);
                            events.ScheduleEvent(EVENT_BLISTERING_COLD, 1000, EVENT_GROUP_LAND_PHASE);

                            // Reset Ice Tomb counter, and schedule Ice Tombs again in phase 3
                            if (_isThirdPhase)
                            {
                                _iceTombCounter = 0;
                                events.ScheduleEvent(EVENT_ICE_TOMB, urand(7000, 10000));
                            }
                            break;
                        case EVENT_BLISTERING_COLD:
                            Talk(EMOTE_WARN_BLISTERING_COLD);
                            DoCast(me, SPELL_BLISTERING_COLD);
                            events.ScheduleEvent(EVENT_BLISTERING_COLD_YELL, 5000, EVENT_GROUP_LAND_PHASE);
                            break;
                        case EVENT_BLISTERING_COLD_YELL:
                            Talk(SAY_BLISTERING_COLD);
                            break;
                        case EVENT_AIR_PHASE:
                        {
                            _isInAirPhase = true;
                            _frostBombCounter = 0;
                            Talk(SAY_AIR_PHASE);
                            me->SetFlying(true);
                            me->OverrideInhabitType(INHABIT_GROUND | INHABIT_AIR);
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();
                            Position pos;
                            pos.Relocate(me);
                            pos.m_positionZ += 25.0f;
                            me->GetMotionMaster()->MoveTakeoff(POINT_TAKEOFF, pos, 12.0f);
                            events.CancelEventGroup(EVENT_GROUP_LAND_PHASE);
                            events.ScheduleEvent(EVENT_FROST_BOMB, 15000);
                            events.ScheduleEvent(EVENT_LAND, 40000);
                            events.ScheduleEvent(EVENT_AIR_PHASE, 110000);
                            break;
                        }
                        case EVENT_AIR_MOVEMENT:
                            me->GetMotionMaster()->MovePoint(POINT_AIR_PHASE, SindragosaAirPos);
                             break;
                        case EVENT_AIR_MOVEMENT_FAR:
                            me->GetMotionMaster()->MovePoint(POINT_AIR_PHASE_FAR, SindragosaAirPosFar);
                            break;
                        case EVENT_ICE_TOMB:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true, -SPELL_ICE_TOMB_DAMAGE))
                            {
                                Talk(EMOTE_WARN_FROZEN_ORB, target);
                                DoCast(target, SPELL_ICE_TOMB_DUMMY, true);

                                ObjectGuid targetGuid = target->GetGUID();
                                me->m_Events.Schedule(500, [this, targetGuid]()
                                {
                                    if (Unit* target = ObjectAccessor::GetUnit(*me, targetGuid))
                                        me->CastSpell(target, SPELL_FROST_BEACON, true);
                                });
                            }

                            _iceTombCounter++;
                            if (_iceTombCounter < 4) // Avoid casting ice tomb more than 4 times between icy grips
                                events.ScheduleEvent(EVENT_ICE_TOMB, urand(16000, 23000));
                            else // We are done with all ice tombs, start icy grip timer
                                events.ScheduleEvent(EVENT_ICY_GRIP, 20000);
                            break;
                        case EVENT_FROST_BOMB:
                        {
                            float destX, destY, destZ;
                            destX = float(rand_norm()) * 75.0f + 4350.0f;
                            destY = float(rand_norm()) * 75.0f + 2450.0f;
                            destZ = 205.0f; // random number close to ground, get exact in next call
                            me->UpdateGroundPositionZ(destX, destY, destZ);
                            me->CastSpell(destX, destY, destZ, SPELL_FROST_BOMB_TRIGGER, false);

                            _frostBombCounter++;
                            if (_frostBombCounter < 4) // Avoid casting Frost Bomb more than 4 times
                                events.ScheduleEvent(EVENT_FROST_BOMB, urand(6000, 7000));
                            break;
                        }
                        case EVENT_LAND:
                        {
                            if (_frostBombCounter < 4)
                            {
                                events.ScheduleEvent(EVENT_LAND, 1000);
                                break;
                            }
                            events.CancelEvent(EVENT_FROST_BOMB);
                            me->GetMotionMaster()->MovePoint(POINT_LAND, SindragosaFlyInPos);
                            break;
                        }
                        case EVENT_LAND_GROUND:
                            events.ScheduleEvent(EVENT_CLEAVE, urand(13000, 15000), EVENT_GROUP_LAND_PHASE);
                            events.ScheduleEvent(EVENT_TAIL_SMASH, urand(19000, 23000), EVENT_GROUP_LAND_PHASE);
                            events.ScheduleEvent(EVENT_FROST_BREATH, urand(10000, 15000), EVENT_GROUP_LAND_PHASE);
                            events.ScheduleEvent(EVENT_UNCHAINED_MAGIC, urand(12000, 17000), EVENT_GROUP_LAND_PHASE);
                            events.ScheduleEvent(EVENT_ICY_GRIP, urand(35000, 40000), EVENT_GROUP_LAND_PHASE);
                            me->GetMotionMaster()->MoveLand(POINT_LAND_GROUND, SindragosaLandPos, 12.0f);
                            break;
                        case EVENT_THIRD_PHASE_CHECK:
                        {
                            if (!_isInAirPhase)
                            {
                                Talk(SAY_PHASE_2);
                                _iceTombCounter = 2; // Set to 2 here, so we get 2 casts until first icy grip
                                events.ScheduleEvent(EVENT_ICE_TOMB, urand(7000, 10000));
                                events.CancelEvent(EVENT_ICY_GRIP);
                                DoCast(me, SPELL_MYSTIC_BUFFET, true);
                            }
                            else
                                events.ScheduleEvent(EVENT_THIRD_PHASE_CHECK, 5000);
                            break;
                        }
                        case EVENT_CHECK_PLAYERS:
                            CheckPlayerPositions();
                            events.ScheduleEvent(EVENT_CHECK_PLAYERS, 3000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            uint8 _frostBombCounter;
            uint8 _iceTombCounter;
            uint8 _mysticBuffetStack;
            bool _isInAirPhase;
            bool _isThirdPhase;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<boss_sindragosaAI>(creature);
        }
};

class npc_ice_tomb : public CreatureScript
{
    public:
        npc_ice_tomb() : CreatureScript("npc_ice_tomb") { }

        struct npc_ice_tombAI : public ScriptedAI
        {
            npc_ice_tombAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
                _trappedPlayerGUID = ObjectGuid::Empty;
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_INSTANTLY_APPEAR_MODEL);
            }

            void Reset() override
            {
                me->ApplySpellImmune(0, IMMUNITY_ID, 49560, true); // after fix - grip redirect not  use mechanic grip
                me->ApplySpellImmune(0, IMMUNITY_ID, 33786, true); // exploit with cyclone
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                me->SetReactState(REACT_PASSIVE);
                _asphyxiationTimer = 20000;
                _asphyxiationTimerEnabled = false;
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                DoCast(me, 26586, true); // Birth
            }

            void SetGUID(ObjectGuid guid, int32 type/* = 0 */) override
            {
                if (type == DATA_TRAPPED_PLAYER)
                {
                    _trappedPlayerGUID = guid;
                    _existenceCheckTimer = 1000;
                }
            }

            void SetData(uint32 type, uint32 /*data*/) override
            {
                if (type == DATA_ENABLE_ASPHYXIATION)
                    _asphyxiationTimerEnabled = true;
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_TRIGGER_ASPHYXIATION)
                    if (Player* player = ObjectAccessor::GetPlayer(*me, _trappedPlayerGUID))
                        player->CastSpell(player, SPELL_ASPHYXIATION, true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveAllGameObjects();
                me->DespawnOrUnsummon(2000);

                if (Player* player = ObjectAccessor::GetPlayer(*me, _trappedPlayerGUID))
                {
                    _trappedPlayerGUID = ObjectGuid::Empty;
                    player->RemoveAurasDueToSpell(SPELL_ICE_TOMB_UNTARGETABLE);
                    player->RemoveAurasDueToSpell(SPELL_ICE_TOMB_DAMAGE);
                    player->RemoveAurasDueToSpell(SPELL_ASPHYXIATION);

                    // set back in combat with sindragosa
                    if (InstanceScript* instance = player->GetInstanceScript())
                        if (Creature* sindragosa = ObjectAccessor::GetCreature(*player, instance->GetGuidData(DATA_SINDRAGOSA)))
                        {
                            sindragosa->SetInCombatWith(player);
                            player->SetInCombatWith(sindragosa);
                        }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!_trappedPlayerGUID)
                    return;

                if (_existenceCheckTimer <= diff)
                {
                    Player* player = ObjectAccessor::GetPlayer(*me, _trappedPlayerGUID);
                    if (!player || player->isDead() || !player->HasAura(SPELL_ICE_TOMB_DAMAGE))
                    {
                        // Remove object
                        JustDied(me);
                        me->DespawnOrUnsummon();
                        return;
                    }
                    _existenceCheckTimer = 1000;
                }
                else
                    _existenceCheckTimer -= diff;

                if (_asphyxiationTimerEnabled)
                {
                    if (_asphyxiationTimer <= diff)
                    {
                        _asphyxiationTimerEnabled = false;

                        if (Player* player = ObjectAccessor::GetPlayer(*me, _trappedPlayerGUID))
                            player->CastSpell(player, SPELL_ASPHYXIATION, true);
                    }
                    else
                        _asphyxiationTimer -= diff;
                }
            }

        private:
            ObjectGuid _trappedPlayerGUID;
            uint32 _existenceCheckTimer;
            bool _asphyxiationTimerEnabled;
            uint32 _asphyxiationTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_ice_tombAI>(creature);
        }
};

class npc_spinestalker : public CreatureScript
{
    public:
        npc_spinestalker() : CreatureScript("npc_spinestalker") { }

        struct npc_spinestalkerAI : public ScriptedAI
        {
            npc_spinestalkerAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

            void InitializeAI() override
            {
                // Increase add count
                if (!me->isDead())
                {
                    _instance->SetData(DATA_SINDRAGOSA_FROSTWYRMS, 1);  // this cannot be in Reset because reset also happens on evade
                    Reset();
                }
            }

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_BELLOWING_ROAR, urand(20000, 25000));
                _events.ScheduleEvent(EVENT_CLEAVE_SPINESTALKER, urand(10000, 15000));
                _events.ScheduleEvent(EVENT_TAIL_SWEEP, urand(8000, 12000));

                if (_instance->GetData(DATA_SPINESTALKER) != 255)
                {
                    me->SetFlying(true);
                    me->OverrideInhabitType(INHABIT_GROUND | INHABIT_AIR);
                }
                else
                    me->OverrideInhabitType(INHABIT_GROUND);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                // Close this door when Rimefang or Spinestalker get infight
                // Server crashes can be ignored in this case, since teleporter to Sindragosa is active
                if (GameObject* sindragosaDoor = _instance->instance->GetGameObject(_instance->GetGuidData(GO_SINDRAGOSA_ENTRANCE_DOOR)))
                    _instance->HandleGameObject(_instance->GetGuidData(GO_SINDRAGOSA_ENTRANCE_DOOR), false, sindragosaDoor);
            }

            void JustAppeared() override
            {
                ScriptedAI::JustAppeared();
                _instance->SetData(DATA_SINDRAGOSA_FROSTWYRMS, 1);  // this cannot be in Reset because reset also happens on evade
            }

            void JustDied(Unit* /*killer*/) override
            {
                _events.Reset();
                _instance->SetData(DATA_SINDRAGOSA_FROSTWYRMS, 0);
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_START_FROSTWYRM)
                {
                    if (_instance->GetBossState(DATA_SINDRAGOSA_GAUNTLET) != DONE)
                        return;

                    if (TempSummon* summon = me->ToTempSummon())
                        summon->SetTempSummonType(TEMPSUMMON_DEAD_DESPAWN);

                    _instance->SetData(DATA_SPINESTALKER, 255);
                    if (me->isDead())
                        return;

                    me->setActive(true);
                    me->SetSpeed(MOVE_RUN, 2.0f);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->SetDefaultMovementType(IDLE_MOTION_TYPE);
                    me->SetWalk(false);
                    me->GetMotionMaster()->MoveIdle();
                    me->StopMoving();
                    me->SetFacingTo(me->GetAngle(&SpinestalkerLandPos));
                    me->GetMotionMaster()->MovePoint(POINT_FROSTWYRM_FLY_IN, SpinestalkerFlyPos);
                    me->SetReactState(REACT_DEFENSIVE);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT_FROSTWYRM_FLY_IN:
                        me->m_Events.Schedule(1000, [this]()
                        {
                            me->GetMotionMaster()->MoveLand(POINT_FROSTWYRM_LAND, SpinestalkerLandPos, 8.247422f);
                        });
                        break;
                    case POINT_FROSTWYRM_LAND:
                        me->setActive(false);
                        me->UpdateSpeed(MOVE_RUN, false);
                        me->SetFlying(false);
                        me->OverrideInhabitType(INHABIT_GROUND);
                        me->SetHomePosition(SpinestalkerLandPos);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BELLOWING_ROAR:
                            DoCast(me, SPELL_BELLOWING_ROAR);
                            _events.ScheduleEvent(EVENT_BELLOWING_ROAR, urand(25000, 30000));
                            break;
                        case EVENT_CLEAVE_SPINESTALKER:
                            DoCastVictim(SPELL_CLEAVE_SPINESTALKER);
                            _events.ScheduleEvent(EVENT_CLEAVE_SPINESTALKER, urand(10000, 15000));
                            break;
                        case EVENT_TAIL_SWEEP:
                            DoCast(me, SPELL_TAIL_SWEEP);
                            _events.ScheduleEvent(EVENT_TAIL_SWEEP, urand(22000, 25000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_spinestalkerAI>(creature);
        }
};

class npc_rimefang : public CreatureScript
{
    public:
        npc_rimefang() : CreatureScript("npc_rimefang_icc") { }

        struct npc_rimefangAI : public ScriptedAI
        {
            npc_rimefangAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

            void InitializeAI() override
            {
                // Increase add count
                if (!me->isDead())
                {
                    _instance->SetData(DATA_SINDRAGOSA_FROSTWYRMS, 1);  // this cannot be in Reset because reset also happens on evade
                    Reset();
                }
            }

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_FROST_BREATH_RIMEFANG, urand(12000, 15000));
                _events.ScheduleEvent(EVENT_ICY_BLAST, urand(30000, 35000));
                _icyBlastCounter = 0;

                if (_instance->GetData(DATA_RIMEFANG) != 255)
                {
                    me->SetFlying(true);
                    me->OverrideInhabitType(INHABIT_GROUND | INHABIT_AIR);
                }
                else
                    me->OverrideInhabitType(INHABIT_GROUND);
            }

            void JustAppeared() override
            {
                ScriptedAI::JustAppeared();
                _instance->SetData(DATA_SINDRAGOSA_FROSTWYRMS, 1);  // this cannot be in Reset because reset also happens on evade
            }

            void JustDied(Unit* /*killer*/) override
            {
                _events.Reset();
                _instance->SetData(DATA_SINDRAGOSA_FROSTWYRMS, 0);
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_START_FROSTWYRM)
                {
                    if (_instance->GetBossState(DATA_SINDRAGOSA_GAUNTLET) != DONE)
                        return;

                    _instance->SetData(DATA_RIMEFANG, 255);
                    if (me->isDead())
                        return;

                    me->setActive(true);
                    me->SetSpeed(MOVE_RUN, 2.0f);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetDefaultMovementType(IDLE_MOTION_TYPE);
                    me->SetWalk(false);
                    me->GetMotionMaster()->MoveIdle();
                    me->StopMoving();
                    me->SetFacingTo(me->GetAngle(&SpinestalkerLandPos));
                    me->GetMotionMaster()->MovePoint(POINT_FROSTWYRM_FLY_IN, RimefangFlyPos);
                    me->SetReactState(REACT_DEFENSIVE);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT_FROSTWYRM_FLY_IN:
                        me->m_Events.Schedule(1000, [this]()
                        {
                            me->GetMotionMaster()->MoveLand(POINT_FROSTWYRM_LAND, RimefangLandPos, 8.247422f);
                        });
                        break;
                    case POINT_FROSTWYRM_LAND:
                        me->setActive(false);
                        me->UpdateSpeed(MOVE_RUN, false);
                        me->SetFlying(false);
                        me->OverrideInhabitType(INHABIT_GROUND);
                        me->SetHomePosition(RimefangLandPos);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    case POINT_AIR_PHASE:
                        _events.ScheduleEvent(EVENT_ICY_BLAST, urand(60000, 70000));
                        _events.ScheduleEvent(EVENT_ICY_BLAST_CAST, 250);
                        break;
                    case POINT_LAND_GROUND:
                        me->SetFlying(false);
                        me->OverrideInhabitType(INHABIT_GROUND);
                        me->SetReactState(REACT_AGGRESSIVE);

                        if (me->IsInCombat())
                            if (Unit* victim = me->SelectVictim())
                                AttackStart(victim);
                        break;
                    default:
                        break;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCast(me, SPELL_FROST_AURA_RIMEFANG, true);

                // Close this door when Rimefang or Spinestalker get infight
                // Server crashes can be ignored in this case, since teleporter to Sindragosa is active
                if (GameObject* sindragosaDoor = _instance->instance->GetGameObject(_instance->GetGuidData(GO_SINDRAGOSA_ENTRANCE_DOOR)))
                    _instance->HandleGameObject(_instance->GetGuidData(GO_SINDRAGOSA_ENTRANCE_DOOR), false, sindragosaDoor);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FROST_BREATH_RIMEFANG:
                            DoCast(me, SPELL_FROST_BREATH);
                            _events.ScheduleEvent(EVENT_FROST_BREATH_RIMEFANG, urand(35000, 40000));
                            break;
                        case EVENT_ICY_BLAST:
                        {
                            _icyBlastCounter = RAID_MODE<uint8>(5, 7, 6, 8);
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();
                            me->SetFlying(true);
                            me->OverrideInhabitType(INHABIT_GROUND | INHABIT_AIR);
                            me->GetMotionMaster()->MoveTakeoff(POINT_AIR_PHASE, RimefangFlyPos);
                            break;
                        }
                        case EVENT_ICY_BLAST_CAST:
                            if (--_icyBlastCounter)
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                {
                                    me->SetFacingToObject(target);
                                    DoCast(target, SPELL_ICY_BLAST);
                                }
                                _events.ScheduleEvent(EVENT_ICY_BLAST_CAST, 3000);
                            }
                            else
                            {
                                Position pos = me->GetPosition();
                                me->UpdateGroundPositionZ(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
                                me->GetMotionMaster()->MoveLand(POINT_LAND_GROUND, pos);
                            }
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            InstanceScript* _instance;
            uint8 _icyBlastCounter;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_rimefangAI>(creature);
        }
};

class npc_sindragosa_trash : public CreatureScript
{
    public:
        npc_sindragosa_trash() : CreatureScript("npc_sindragosa_trash") { }

        struct npc_sindragosa_trashAI : public ScriptedAI
        {
            npc_sindragosa_trashAI(Creature* creature) : ScriptedAI(creature)
            {
                _instance = creature->GetInstanceScript();
            }

            void InitializeAI() override
            {
                _frostwyrmId = (me->GetHomePosition().GetPositionY() < 2484.35f) ? DATA_RIMEFANG : DATA_SPINESTALKER;
                // Increase add count
                if (!me->isDead())
                {
                    if (me->GetEntry() != NPC_FROSTWARDEN_HANDLER)
                        _instance->SetData(_frostwyrmId, 1);  // this cannot be in Reset because reset also happens on evade
                    Reset();
                }
            }

            void Reset() override
            {
                // This is shared AI for handler and whelps
                if (me->GetEntry() == NPC_FROSTWARDEN_HANDLER)
                {
                    _events.ScheduleEvent(EVENT_FROSTWARDEN_ORDER_WHELP, 3000);
                    _events.ScheduleEvent(EVENT_CONCUSSIVE_SHOCK, urand(8000, 10000));
                }

                _isTaunted = false;
            }

            void JustAppeared() override
            {
                ScriptedAI::JustAppeared();

                // Increase add count
                if (me->GetEntry() != NPC_FROSTWARDEN_HANDLER)
                    _instance->SetData(_frostwyrmId, 1);  // this cannot be in Reset because reset also happens on evade
            }

            void JustDied(Unit* /*killer*/) override
            {
                // Decrease add count
                if (me->GetEntry() != NPC_FROSTWARDEN_HANDLER)
                    _instance->SetData(_frostwyrmId, 0);
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_WHELP_MARKER)
                    _isTaunted = data != 0;
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_FROSTWYRM_OWNER)
                    return _frostwyrmId;
                else if (type == DATA_WHELP_MARKER)
                    return uint32(_isTaunted);
                return 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                //if ( me->GetDistance(me->GetVictim()) > 30.0f)
                //    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FROSTWARDEN_ORDER_WHELP:
                            DoCast(me, SPELL_ORDER_WHELP, true);
                            _events.ScheduleEvent(EVENT_FROSTWARDEN_ORDER_WHELP, 3000);
                            break;
                        case EVENT_CONCUSSIVE_SHOCK:
                            DoCast(me, SPELL_CONCUSSIVE_SHOCK);
                            _events.ScheduleEvent(EVENT_CONCUSSIVE_SHOCK, urand(10000, 13000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            InstanceScript* _instance;
            uint32 _frostwyrmId;
            bool _isTaunted; // Frostwing Whelp only
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_sindragosa_trashAI>(creature);
        }
};

class spell_sindragosa_s_fury : public SpellScriptLoader
{
    public:
        spell_sindragosa_s_fury() : SpellScriptLoader("spell_sindragosa_s_fury") { }

        class spell_sindragosa_s_fury_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sindragosa_s_fury_SpellScript);

            bool Load() override
            {
                _targetCount = 0;

                // This script should execute only in Icecrown Citadel
                if (InstanceMap* instance = GetCaster()->GetMap()->ToInstanceMap())
                    if (instance->GetInstanceScript())
                        if (instance->GetScriptId() == sObjectMgr->GetScriptId(ICCScriptName))
                            return true;

                return false;
            }

            void SelectDest()
            {
                if (Position* dest = const_cast<WorldLocation*>(GetExplTargetDest()))
                {
                    float destX = float(rand_norm()) * 75.0f + 4350.0f;
                    float destY = float(rand_norm()) * 75.0f + 2450.0f;
                    float destZ = 205.0f; // random number close to ground, get exact in next call
                    GetCaster()->UpdateGroundPositionZ(destX, destY, destZ);
                    dest->Relocate(destX, destY, destZ);
                }
            }

            void CountTargets(std::list<WorldObject*>& targets)
            {
                _targetCount = targets.size();
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (!GetHitUnit()->IsAlive() || !_targetCount)
                    return;

                // Do not hit GMs in GM mode
                if (GetHitUnit()->ToPlayer() && GetHitUnit()->ToPlayer()->IsGameMaster())
                    return;

                float resistance = float(GetHitUnit()->GetResistance(SpellSchoolMask(GetSpellInfo()->SchoolMask)));
                uint32 minResistFactor = std::min(9u, uint32((resistance / (resistance + 510.0f)) * 10.0f) * 2);
                uint32 randomResist = urand(0, (9 - minResistFactor) * 100) / 100 + 1;

                uint32 damage = (uint32(GetEffectValue() / _targetCount) * randomResist) / 10;

                SpellNonMeleeDamage damageInfo(GetCaster(), GetHitUnit(), GetSpellInfo()->Id, GetSpellInfo()->SchoolMask);
                damageInfo.damage = damage;
                GetCaster()->SendSpellNonMeleeDamageLog(&damageInfo);
                GetCaster()->DealSpellDamage(&damageInfo, false);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_sindragosa_s_fury_SpellScript::SelectDest);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sindragosa_s_fury_SpellScript::CountTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_sindragosa_s_fury_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }

            uint32 _targetCount;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sindragosa_s_fury_SpellScript();
        }
};

class UnchainedMagicTargetSelector
{
    public:
        UnchainedMagicTargetSelector() { }

        bool operator()(WorldObject* object) const
        {
            if (Unit* unit = object->ToUnit())
                return unit->GetPowerType() != POWER_MANA;
            return true;
        }
};

class spell_sindragosa_unchained_magic : public SpellScriptLoader
{
    public:
        spell_sindragosa_unchained_magic() : SpellScriptLoader("spell_sindragosa_unchained_magic") { }

        class spell_sindragosa_unchained_magic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sindragosa_unchained_magic_SpellScript);

            void FilterTargets(std::list<WorldObject*>& unitList)
            {
                unitList.remove_if(UnchainedMagicTargetSelector());
                uint32 maxSize = uint32(GetCaster()->GetMap()->Is25ManRaid() ? 6 : 2);
                if (unitList.size() > maxSize)
                    Trinity::Containers::RandomResizeList(unitList, maxSize);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sindragosa_unchained_magic_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        class spell_sindragosa_unchained_magic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sindragosa_unchained_magic_AuraScript);

            void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                if (eventInfo.GetDamageInfo() && eventInfo.GetDamageInfo()->GetSpellInfo())
                {
                    // Prevent proc from these spells because Blizzard is weird
                    switch (eventInfo.GetDamageInfo()->GetSpellInfo()->Id)
                    {
                        case 33110: // Prayer of Mending
                        case 41635: // Prayer of Mending (Rank 1)
                        case 48110: // Prayer of Mending (Rank 2)
                        case 48111: // Prayer of Mending (Rank 3)
                            PreventDefaultAction();
                            break;
                    }
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_sindragosa_unchained_magic_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sindragosa_unchained_magic_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_sindragosa_unchained_magic_AuraScript();
        }
};

class spell_sindragosa_instability : public SpellScriptLoader
{
    public:
        spell_sindragosa_instability() : SpellScriptLoader("spell_sindragosa_instability") { }

        class spell_sindragosa_instability_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sindragosa_instability_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BACKLASH))
                    return false;
                return true;
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    GetTarget()->CastCustomSpell(SPELL_BACKLASH, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), GetTarget(), true, nullptr, aurEff, GetCasterGUID());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_sindragosa_instability_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sindragosa_instability_AuraScript();
        }
};

class spell_sindragosa_frost_beacon : public SpellScriptLoader
{
    public:
        spell_sindragosa_frost_beacon() : SpellScriptLoader("spell_sindragosa_frost_beacon") { }

        class spell_sindragosa_frost_beacon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sindragosa_frost_beacon_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ICE_TOMB_DAMAGE))
                    return false;
                return true;
            }

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                if (Unit* caster = GetCaster())
                    caster->CastSpell(GetTarget(), SPELL_ICE_TOMB_DAMAGE, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sindragosa_frost_beacon_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sindragosa_frost_beacon_AuraScript();
        }
};

class spell_sindragosa_ice_tomb : public SpellScriptLoader
{
    public:
        spell_sindragosa_ice_tomb() : SpellScriptLoader("spell_sindragosa_ice_tomb") { }

        class spell_sindragosa_ice_tomb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sindragosa_ice_tomb_SpellScript);

            void HandleAfterHit()
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();
                if (!caster || !target)
                    return;

                caster->CastSpell(target, SPELL_ICE_TOMB_DUMMY, true);

                ObjectGuid targetGuid = target->GetGUID();
                caster->m_Events.Schedule(500, [caster, targetGuid]()
                {
                    if (Unit* target = ObjectAccessor::GetUnit(*caster, targetGuid))
                        caster->CastSpell(target, SPELL_FROST_BEACON, true);
                });
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_sindragosa_ice_tomb_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sindragosa_ice_tomb_SpellScript();
        }
};

class spell_sindragosa_ice_tomb_trap : public SpellScriptLoader
{
    public:
        spell_sindragosa_ice_tomb_trap() : SpellScriptLoader("spell_sindragosa_ice_tomb_trap") { }

        class spell_sindragosa_ice_tomb_trap_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sindragosa_ice_tomb_trap_AuraScript);

            void PeriodicTick(AuraEffect const* aurEff)
            {
                PreventDefaultAction();
                if (aurEff->GetTickNumber() != 1)
                    return;

                Unit* unit = GetTarget();
                Unit* caster = GetCaster();

                if (!unit || !caster)
                    return;

                Position pos = unit->GetPosition();

                if (TempSummon* summon = caster->SummonCreature(NPC_ICE_TOMB, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation()))
                {
                    summon->AI()->SetGUID(unit->GetGUID(), DATA_TRAPPED_PLAYER);
                    if (GameObject* go = summon->SummonGameObject(GO_ICE_BLOCK, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 0, { }, 0))
                    {
                        go->SetSpellId(SPELL_ICE_TOMB_DAMAGE);
                        summon->AddGameObject(go);
                    }
                }

                if (Unit* target = GetTarget())
                    if (!target->HasAura(SPELL_ICE_TOMB_UNTARGETABLE))
                        target->CastSpell(target, SPELL_ICE_TOMB_UNTARGETABLE, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sindragosa_ice_tomb_trap_AuraScript::PeriodicTick, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sindragosa_ice_tomb_trap_AuraScript();
        }
};

class spell_sindragosa_ice_tomb_unattackable : public SpellScriptLoader
{
    public:
        spell_sindragosa_ice_tomb_unattackable() : SpellScriptLoader("spell_sindragosa_ice_tomb_unattackable") { }

        class spell_sindragosa_ice_tomb_unattackable_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sindragosa_ice_tomb_unattackable_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                    target->AddUnitState(UNIT_STATE_FOLLOW_FORMATION); // To prevent healing
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                    target->ClearUnitState(UNIT_STATE_FOLLOW_FORMATION);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_sindragosa_ice_tomb_unattackable_AuraScript::OnApply, EFFECT_1, SPELL_AURA_MOD_UNATTACKABLE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectRemove += AuraEffectRemoveFn(spell_sindragosa_ice_tomb_unattackable_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_MOD_UNATTACKABLE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sindragosa_ice_tomb_unattackable_AuraScript();
        }
};

class spell_sindragosa_icy_grip : public SpellScriptLoader
{
    public:
        spell_sindragosa_icy_grip() : SpellScriptLoader("spell_sindragosa_icy_grip") { }

        class spell_sindragosa_icy_grip_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sindragosa_icy_grip_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ICY_GRIP_JUMP))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Unit* unit = GetHitUnit();
                Unit* caster = GetCaster();

                if (unit && caster)
                {
                    if (unit->HasAura(SPELL_FROST_BEACON)) // player with Beacon not gripped
                        return;

                    if (caster->GetTypeId() == TYPEID_UNIT && unit->GetTypeId() == TYPEID_PLAYER)
                        if (caster->GetVictim() != unit) // exclude tank
                            unit->CastSpell(caster, SPELL_ICY_GRIP_JUMP, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sindragosa_icy_grip_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sindragosa_icy_grip_SpellScript();
        }
};

class MysticBuffetTargetFilter
{
    public:
        explicit MysticBuffetTargetFilter(Unit* caster) : _caster(caster) { }

        bool operator()(WorldObject* unit) const
        {
            return !unit->IsWithinLOSInMap(_caster);
        }

    private:
        Unit* _caster;
};

class spell_sindragosa_mystic_buffet : public SpellScriptLoader
{
    public:
        spell_sindragosa_mystic_buffet() : SpellScriptLoader("spell_sindragosa_mystic_buffet") { }

        class spell_sindragosa_mystic_buffet_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sindragosa_mystic_buffet_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(MysticBuffetTargetFilter(GetCaster()));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sindragosa_mystic_buffet_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sindragosa_mystic_buffet_SpellScript();
        }
};

class OrderWhelpTargetSelector
{
    public:
        explicit OrderWhelpTargetSelector(Creature* owner) : _owner(owner) { }

        bool operator()(Creature* creature)
        {
            if (!creature->AI()->GetData(DATA_WHELP_MARKER) && creature->AI()->GetData(DATA_FROSTWYRM_OWNER) == _owner->AI()->GetData(DATA_FROSTWYRM_OWNER))
                return false;
            return true;
        }

    private:
        Creature* _owner;
};

class spell_frostwarden_handler_order_whelp : public SpellScriptLoader
{
    public:
        spell_frostwarden_handler_order_whelp() : SpellScriptLoader("spell_frostwarden_handler_order_whelp") { }

        class spell_frostwarden_handler_order_whelp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_frostwarden_handler_order_whelp_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_FOCUS_FIRE))
                    return false;
                return true;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(Trinity::ObjectTypeIdCheck(TYPEID_PLAYER, false));
                if (targets.empty())
                    return;

                WorldObject* target = Trinity::Containers::SelectRandomContainerElement(targets);
                targets.clear();
                targets.push_back(target);
            }

            void HandleForcedCast(SpellEffIndex effIndex)
            {
                // caster is Frostwarden Handler, target is player, caster of triggered is whelp
                PreventHitDefaultEffect(effIndex);
                std::list<Creature*> unitList;
                GetCreatureListWithEntryInGrid(unitList, GetCaster(), NPC_FROSTWING_WHELP, 150.0f);
                if (Creature* creature = GetCaster()->ToCreature())
                    unitList.remove_if(OrderWhelpTargetSelector(creature));

                if (unitList.empty())
                    return;

                Trinity::Containers::SelectRandomContainerElement(unitList)->CastSpell(GetHitUnit(), uint32(GetEffectValue()), true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_frostwarden_handler_order_whelp_SpellScript::HandleForcedCast, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_frostwarden_handler_order_whelp_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_frostwarden_handler_order_whelp_SpellScript();
        }
};

class spell_frostwarden_handler_focus_fire : public SpellScriptLoader
{
    public:
        spell_frostwarden_handler_focus_fire() : SpellScriptLoader("spell_frostwarden_handler_focus_fire") { }

        class spell_frostwarden_handler_focus_fire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_frostwarden_handler_focus_fire_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetCaster()->GetThreatManager().AddThreat(GetHitUnit(), float(GetEffectValue()));
                GetCaster()->GetAI()->SetData(DATA_WHELP_MARKER, 1);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_frostwarden_handler_focus_fire_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        class spell_frostwarden_handler_focus_fire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_frostwarden_handler_focus_fire_AuraScript);

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                if (Unit* caster = GetCaster())
                {
                    caster->GetThreatManager().AddThreat(GetTarget(), -float(GetSpellInfo()->Effects[EFFECT_1].CalcValue()));
                    caster->GetAI()->SetData(DATA_WHELP_MARKER, 0);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_frostwarden_handler_focus_fire_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_frostwarden_handler_focus_fire_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_frostwarden_handler_focus_fire_AuraScript();
        }
};

class spell_sindragosa_permeating_chill : public SpellScriptLoader
{
    public:
        spell_sindragosa_permeating_chill() : SpellScriptLoader("spell_sindragosa_permeating_chill") { }

        class spell_sindragosa_permeating_chill_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sindragosa_permeating_chill_AuraScript);

            void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                if (eventInfo.GetActionTarget() != GetCaster())
                    PreventDefaultAction();
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_sindragosa_permeating_chill_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sindragosa_permeating_chill_AuraScript();
        }
};

class at_sindragosa_lair : public AreaTriggerScript
{
    public:
        at_sindragosa_lair() : AreaTriggerScript("at_sindragosa_lair") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (player->IsGameMaster())
                return true;

            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetBossState(DATA_SINDRAGOSA_GAUNTLET) != DONE)
                    return true;

                if (!instance->GetData(DATA_SPINESTALKER))
                    if (Creature* spinestalker = ObjectAccessor::GetCreature(*player, instance->GetGuidData(DATA_SPINESTALKER)))
                        spinestalker->AI()->DoAction(ACTION_START_FROSTWYRM);

                if (!instance->GetData(DATA_RIMEFANG))
                    if (Creature* rimefang = ObjectAccessor::GetCreature(*player, instance->GetGuidData(DATA_RIMEFANG)))
                        rimefang->AI()->DoAction(ACTION_START_FROSTWYRM);

                if ((!instance->GetData(DATA_SINDRAGOSA_FROSTWYRMS) && instance->GetBossState(DATA_SINDRAGOSA) != DONE) ||
                    (instance->GetData(DATA_SINDRAGOSA_FROSTWYRMS) == 255 && instance->GetBossState(DATA_SINDRAGOSA) == FAIL))
                {
                    if (player->GetMap()->IsHeroic() && !instance->GetData(DATA_HEROIC_ATTEMPTS))
                        return true;

                    Creature* sindragosa = ObjectAccessor::GetCreature(*player, instance->GetGuidData(DATA_SINDRAGOSA));

                    // Sindragosa is not spawned, so respawn now
                    if (!sindragosa)
                    {
                        player->GetMap()->LoadGrid(SindragosaSpawnPos.GetPositionX(), SindragosaSpawnPos.GetPositionY());
                        if (sindragosa = player->GetMap()->SummonCreature(NPC_SINDRAGOSA, SindragosaSpawnPos))
                        {
                            sindragosa->AI()->DoAction(ACTION_START_FROSTWYRM);
                            return true;
                        }
                    }
                }
            }

            return true;
        }
};

class FrostBombTargetSelector
{
    public:
        FrostBombTargetSelector(Unit* caster, std::list<Creature*> const& collisionList) : _caster(caster), _collisionList(collisionList) { }

        bool operator()(WorldObject* object) const
        {
            if (object->ToUnit()->HasAura(SPELL_ICE_TOMB_DAMAGE))
                return true;

            float oldOrientation = _caster->GetOrientation();
            _caster->SetOrientation(_caster->GetAngle(object));

            float distToTarget = _caster->GetExactDist2dSq(object);

            bool result = false;
            for (std::list<Creature*>::const_iterator itr = _collisionList.begin(); itr != _collisionList.end() && !result; ++itr)
                if ((*itr)->IsAlive() && distToTarget > _caster->GetExactDist2dSq(*itr) && _caster->HasInArc(0.0f, *itr, 2.0f))
                    result = true;

            _caster->SetOrientation(oldOrientation);

            return result;
        }

    private:
        Unit* _caster;
        std::list<Creature*> const& _collisionList;
};

class spell_sindragosa_collision_filter : public SpellScriptLoader
{
    public:
        spell_sindragosa_collision_filter() : SpellScriptLoader("spell_sindragosa_collision_filter") { }

        class spell_sindragosa_collision_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sindragosa_collision_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& unitList)
            {
                std::list<Creature*> tombs;
                GetCreatureListWithEntryInGrid(tombs, GetCaster(), NPC_ICE_TOMB, 200.0f);
                unitList.remove_if (FrostBombTargetSelector(GetCaster(), tombs));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sindragosa_collision_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sindragosa_collision_filter_SpellScript();
        }
};

class spell_sindragosa_tail_smash : public SpellScriptLoader
{
    public:
        spell_sindragosa_tail_smash() : SpellScriptLoader("spell_sindragosa_tail_smash") { }

        class spell_sindragosa_tail_smash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sindragosa_tail_smash_SpellScript);

            void TargetSelect(SpellDestination& dest)
            {
                if (Unit* caster = GetCaster()){
                    Position pos = caster->GetNearPosition(40.0f, M_PI);
                    dest._position = WorldLocation(GetCaster()->GetMapId(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
                }
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_sindragosa_tail_smash_SpellScript::TargetSelect, EFFECT_0, TARGET_DEST_DEST_BACK);
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_sindragosa_tail_smash_SpellScript::TargetSelect, EFFECT_1, TARGET_DEST_DEST_BACK);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sindragosa_tail_smash_SpellScript();
        }
};

void AddSC_boss_sindragosa()
{
    new boss_sindragosa();
    new npc_ice_tomb();
    new npc_spinestalker();
    new npc_rimefang();
    new npc_sindragosa_trash();
    new spell_sindragosa_s_fury();
    new spell_sindragosa_unchained_magic();
    new spell_sindragosa_instability();
    new spell_sindragosa_frost_beacon();
    new spell_sindragosa_ice_tomb();
    new spell_sindragosa_ice_tomb_trap();
    new spell_sindragosa_ice_tomb_unattackable();
    new spell_sindragosa_collision_filter();
    new spell_sindragosa_icy_grip();
    new spell_sindragosa_mystic_buffet();
    new spell_frostwarden_handler_order_whelp();
    new spell_frostwarden_handler_focus_fire();
    new spell_sindragosa_permeating_chill();
    new at_sindragosa_lair();
    new spell_sindragosa_tail_smash();
}
