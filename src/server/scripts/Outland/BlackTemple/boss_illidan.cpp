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

#include "PassiveAI.h"
#include "ScriptPCH.h"
#include "black_temple.h"

enum Say
{
    // Akama
    SAY_AKAMA_BEWARE                = 0,
    SAY_AKAMA_LEAVE                 = 1,
    // Illidan
    SAY_ILLIDAN_MINION              = 0,
    SAY_ILLIDAN_KILL                = 1,
    SAY_ILLIDAN_TAKEOFF             = 2,
    SAY_ILLIDAN_SUMMONFLAMES        = 3,
    SAY_ILLIDAN_EYE_BLAST           = 4,
    SAY_ILLIDAN_MORPH               = 5,
    SAY_ILLIDAN_ENRAGE              = 6,
    SAY_ILLIDAN_TAUNT               = 7,
    // Maiev Shadowsong
    SAY_MAIEV_SHADOWSONG_TAUNT      = 0,
    // Flame of Azzinoth
    EMOTE_AZZINOTH_GAZE             = 0
};

enum Spells
{
    // Normal Form
    SPELL_SHEAR                     =   37335, // 41032 is bugged, cannot be block/dodge/parry// Reduces Max. Health by 60% for 7 seconds. Can stack 19 times. 1.5f second cast
    SPELL_FLAME_CRASH               =   40832,
    SPELL_DRAW_SOUL                 =   40904,
    SPELL_PARASITIC_SHADOWFIEND     =   41917,
    SPELL_PARASITIC_SHADOWFIEND2    =   41914,
    SPELL_SUMMON_PARASITICS         =   41915,
    SPELL_AGONIZING_FLAMES          =   40932,
    SPELL_ENRAGE                    =   40683,
    // Flying (Phase 2)
    SPELL_THROW_GLAIVE              =   39635,
    SPELL_THROW_GLAIVE2             =   39849, // Animation for the spell above
    SPELL_GLAIVE_RETURNS            =   39873,
    SPELL_FIREBALL                  =   40598,
    SPELL_DARK_BARRAGE              =   40585,
    // Demon Form
    SPELL_DEMON_TRANSFORM_1         =   40511,
    SPELL_DEMON_TRANSFORM_2         =   40398,
    SPELL_DEMON_TRANSFORM_3         =   40510,
    SPELL_DEMON_FORM                =   40506,
    SPELL_SHADOW_BLAST              =   41078,
    SPELL_FLAME_BURST               =   41126,
    SPELL_FLAME_BURST_EFFECT        =   41131, // The actual damage. Have each player cast it on itself (workaround)
    // Other Illidan spells
    SPELL_KNEEL                     =   39656, // Before beginning encounter, this is how he appears (talking to skully).
    SPELL_SHADOW_PRISON             =   40647,
    SPELL_DEATH                     =   41220,
    SPELL_BERSERK                   =   45078,
    SPELL_DUAL_WIELD                =   42459,
    SPELL_SUMMON_MAIEV              =   40403,
    // Phase Normal spells
    SPELL_FLAME_CRASH_EFFECT        =   40836,
    SPELL_SUMMON_SHADOWDEMON        =   41117,
    SPELL_SHADOWFIEND_PASSIVE       =   41913,
    SPELL_SHADOW_DEMON_PASSIVE      =   41079,
    SPELL_CONSUME_SOUL              =   41080,
    SPELL_PARALYZE                  =   41083,
    SPELL_PURPLE_BEAM               =   39123,
    // Phase Flight spells
    SPELL_AZZINOTH_CHANNEL          =   39857, // Glaives cast it on Flames. Not sure if this is the right spell.
    SPELL_EYE_BLAST_TRIGGER         =   40017,
    SPELL_EYE_BLAST                 =   39908,
    SPELL_BLAZE_EFFECT              =   40610,
    SPELL_BLAZE_SUMMON              =   40637,
    SPELL_DEMON_FIRE                =   40029,
    SPELL_FLAME_BLAST               =   40631,
    SPELL_CHARGE                    =   41581,
    SPELL_FLAME_ENRAGE              =   45078,
    // Akama spells
    SPELL_AKAMA_DOOR_CHANNEL        =   41268,
    SPELL_DEATHSWORN_DOOR_CHANNEL   =   41269,
    SPELL_AKAMA_DOOR_FAIL           =   41271, // Not sure where this is really used...
    SPELL_HEALING_POTION            =   40535,
    SPELL_CHAIN_LIGHTNING           =   40536,
    // Maiev spells
    SPELL_CAGE_TRAP_DUMMY           =   40761, // Put this in DB for cage trap GO.
    SPELL_CAGED                     =   40695,
    SPELL_CAGE_TRAP_SUMMON          =   40694, // Summons a Cage Trap GO (bugged) on the ground along with a Cage Trap Disturb Trigger mob (working)
    SPELL_CAGE_TRAP_BEAM            =   40713,
    SPELL_TELEPORT_VISUAL           =   41232,
    SPELL_SHADOW_STRIKE             =   40685,
    SPELL_THROW_DAGGER              =   41152,
    SPELL_FAN_BLADES                =   39954  // bugged visual
};

enum Misc
{
    FLAME_ENRAGE_DISTANCE           = 30,
    FLAME_CHARGE_DISTANCE           = 50,

    EQUIP_ID_MAIN_HAND              = 32837,
    EQUIP_ID_OFF_HAND               = 32838,
};

/**** Creature Summon and Recognition IDs ****/
enum CreatureEntry
{
    EMPTY                       =     0,
    AKAMA                       = 22990,
    ILLIDAN_STORMRAGE           = 22917,
    BLADE_OF_AZZINOTH           = 22996,
    FLAME_OF_AZZINOTH           = 22997,
    MAIEV_SHADOWSONG            = 23197,
    SHADOW_DEMON                = 23375,
    DEMON_FIRE                  = 23069,
    FLAME_CRASH                 = 23336,
    ILLIDAN_DOOR_TRIGGER        = 23412,
    SPIRIT_OF_OLUM              = 23411,
    SPIRIT_OF_UDALO             = 23410,
    ILLIDARI_ELITE              = 23226,
    PARASITIC_SHADOWFIEND       = 23498,
    CAGE_TRAP_TRIGGER           = 23292,
};

/*** Phase Names ***/
enum PhaseIllidan
{
    PHASE_ILLIDAN_NULL          = 0,
    PHASE_NORMAL                = 1,
    PHASE_FLIGHT                = 2,
    PHASE_NORMAL_2              = 3,
    PHASE_DEMON                 = 4,
    PHASE_NORMAL_MAIEV          = 5,
    PHASE_TALK_SEQUENCE         = 6,
    PHASE_FLIGHT_SEQUENCE       = 7,
    PHASE_TRANSFORM_SEQUENCE    = 8,
    PHASE_ILLIDAN_MAX           = 9,
}; // Maiev uses the same phase

enum PhaseAkama
{
    PHASE_AKAMA_NULL            = 0,
    PHASE_CHANNEL               = 1,
    PHASE_WALK                  = 2,
    PHASE_TALK                  = 3,
    PHASE_FIGHT_ILLIDAN         = 4,
    PHASE_FIGHT_MINIONS         = 5,
    PHASE_RETURN                = 6,
};

enum EventIllidan
{
    EVENT_NULL                  = 0,
    EVENT_BERSERK               = 1,
    // normal phase
    EVENT_TAUNT                 = 2,
    EVENT_SHEAR                 = 3,
    EVENT_FLAME_CRASH           = 4,
    EVENT_PARASITIC_SHADOWFIEND = 5,
    EVENT_PARASITE_CHECK        = 6,
    EVENT_DRAW_SOUL             = 7,
    EVENT_AGONIZING_FLAMES      = 8,
    EVENT_TRANSFORM_NORMAL      = 9,
    EVENT_ENRAGE                = 10,
    // flight phase
    EVENT_FIREBALL              = 2,
    EVENT_DARK_BARRAGE          = 3,
    EVENT_EYE_BLAST             = 4,
    EVENT_MOVE_POINT            = 5,
    // demon phase
    EVENT_SHADOW_BLAST          = 2,
    EVENT_FLAME_BURST           = 3,
    EVENT_SHADOWDEMON           = 4,
    EVENT_TRANSFORM_DEMON       = 5,
    // sequence phase
    EVENT_TALK_SEQUENCE         = 2,
    EVENT_FLIGHT_SEQUENCE       = 2,
    EVENT_TRANSFORM_SEQUENCE    = 2,
};

enum EventMaiev
{
    EVENT_MAIEV_NULL            = 0,
    EVENT_MAIEV_STEALTH         = 1,
    EVENT_MAIEV_TAUNT           = 2,
    EVENT_MAIEV_SHADOW_STRIKE   = 3,
    EVENT_MAIEV_THROW_DAGGER    = 4,
    EVENT_MAIEV_TRAP            = 4,
};

static const EventIllidan MaxTimer[9] =
{
    EVENT_NULL,
    EVENT_DRAW_SOUL,
    EVENT_MOVE_POINT,
    EVENT_TRANSFORM_NORMAL,
    EVENT_TRANSFORM_DEMON,
    EVENT_ENRAGE,
    EVENT_TALK_SEQUENCE,
    EVENT_FLIGHT_SEQUENCE,
    EVENT_TRANSFORM_SEQUENCE
};

struct Yells
{
    uint32 sound;
    int32 text;
    uint32 creature, timer, emote;
    bool TalkText;
};

static const Yells Conversation[22] =
{
    { 11463,  8, ILLIDAN_STORMRAGE, 8000,  0,   true  },
    { 0,     -1, ILLIDAN_STORMRAGE, 5000,  396, true  },
    { 11389,  2, AKAMA,             7000,  25,  true  },
    { 0,     -1, AKAMA,             5000,  66,  true  },
    { 11464,  9, ILLIDAN_STORMRAGE, 8000,  396, true  },
    { 11380,  3, AKAMA,             3000,  22,  true  },
    { 0,     -1, AKAMA,             2000,  15,  true  },
    { 11466, 10, ILLIDAN_STORMRAGE, 3000,  406, true  },
    { 0,     -1, EMPTY,             1000,  0,   true  },
    { 0,     -1, EMPTY,             0,     0,   false }, // 9
    { 11476, 11, ILLIDAN_STORMRAGE, 8000,  0,   true  },
    { 11491,  0, MAIEV_SHADOWSONG,  8000,  5,   true  },
    { 11477, 12, ILLIDAN_STORMRAGE, 5000,  1,   true  },
    { 11492,  1, MAIEV_SHADOWSONG,  8000,  15,  true  },
    { 11470, 13, ILLIDAN_STORMRAGE, 1000,  0,   false }, // 14
    { 11496,  2, MAIEV_SHADOWSONG,  6000,  0,   true  }, // 15
    { 11478, 14, ILLIDAN_STORMRAGE, 30000, 65,  true  }, // Emote dead for now. Kill him later
    { 11497,  3, MAIEV_SHADOWSONG,  9000,  0,   true  },
    { 11498, -1, MAIEV_SHADOWSONG,  5000,  0,   true  },
    { 11498, -1, EMPTY,             1000,  0,   true  }, // 19 Maiev disappear
    { 11387,  4, AKAMA,             8000,  0,   true  },
    { 0,     -1, EMPTY,             1000,  0,   false } // 21
};

struct Locations
{
    float x, y, z;
};

static const Locations HoverPosition[4]=
{
    { 657.0f, 340.0f, 355.0f },
    { 657.0f, 275.0f, 355.0f },
    { 705.0f, 275.0f, 355.0f },
    { 705.0f, 340.0f, 355.0f }
};

static const Locations GlaivePosition[4]=
{
    { 695.105f, 305.303f, 354.256f },
    { 659.338f, 305.303f, 354.256f }, // the distance between two glaives is 36
    { 700.105f, 305.303f, 354.256f },
    { 664.338f, 305.303f, 354.256f }
};

static const Locations EyeBlast[2]=
{
    { 677.0f, 350.0f, 354.0f }, // start point, pass through glaive point
    { 677.0f, 260.0f, 354.0f }
};

static const Locations AkamaWP[13]=
{
    { 770.01f, 304.50f, 312.29f }, // Bottom of the first stairs, at the doors
    { 780.66f, 304.50f, 319.74f }, // Top of the first stairs
    { 790.13f, 319.68f, 319.76f }, // Bottom of the second stairs (left from the entrance)
    { 787.17f, 347.38f, 341.42f }, // Top of the second stairs
    { 781.34f, 350.31f, 341.44f }, // Bottom of the third stairs
    { 762.60f, 361.06f, 353.60f }, // Top of the third stairs
    { 756.35f, 360.52f, 353.27f }, // Before the door-thingy
    { 743.82f, 342.21f, 353.00f }, // Somewhere further
    { 732.69f, 305.13f, 353.00f }, // In front of Illidan - (8)
    { 738.11f, 365.44f, 353.00f }, // in front of the door-thingy (the other one!)
    { 792.18f, 366.62f, 341.42f }, // Down the first flight of stairs
    { 796.84f, 304.89f, 319.76f }, // Down the second flight of stairs
    { 782.01f, 304.55f, 319.76f }  // Final location - back at the initial gates. This is where he will fight the minions! (12)
};
// 755.762f, 304.0747f, 312.1769f -- This is where Akama should be spawned
static const Locations SpiritSpawns[2]=
{
    { 755.5426f, 309.9156f, 312.2129f },
    { 755.5426f, 298.7923f, 312.0834f }
};

struct Animation // For the demon transformation
{
    uint32 aura, unaura, timer, size, displayid, phase;
    bool equip;
};

// Other defines
#define CENTER_X            676.740f
#define CENTER_Y            305.297f
#define CENTER_Z            353.192f

static const Animation DemonTransformation[10]=
{
    { SPELL_DEMON_TRANSFORM_1, 0,                       1000, 0,          0,     6, true  },
    { SPELL_DEMON_TRANSFORM_2, SPELL_DEMON_TRANSFORM_1, 4000, 0,          0,     6, true  },
    { 0,                       0,                       3000, 1073741824, 21322, 6, false }, // stunned, cannot cast demon form
    { SPELL_DEMON_TRANSFORM_3, SPELL_DEMON_TRANSFORM_2, 3500, 0,          0,     6, false },
    { SPELL_DEMON_FORM,        SPELL_DEMON_TRANSFORM_3, 0,    0,          0,     4, false },
    { SPELL_DEMON_TRANSFORM_1, 0,                       1000, 0,          0,     6, false },
    { SPELL_DEMON_TRANSFORM_2, SPELL_DEMON_TRANSFORM_1, 4000, 0,          0,     6, false },
    { 0,                       SPELL_DEMON_FORM,        3000, 1069547520, 21135, 6, false },
    { SPELL_DEMON_TRANSFORM_3, SPELL_DEMON_TRANSFORM_2, 3500, 0,          0,     6, true  },
    { 0,                       SPELL_DEMON_TRANSFORM_3, 0,    0,          0,     8, true  }
};

#define GOSSIP_ITEM 8638

class npc_flame_of_azzinoth : public CreatureScript
{
    public:
        npc_flame_of_azzinoth() : CreatureScript("npc_flame_of_azzinoth") { }

        struct flame_of_azzinothAI : public ScriptedAI
        {
            flame_of_azzinothAI(Creature* creature) : ScriptedAI(creature) 
            {
                me->ApplySpellImmune(0, IMMUNITY_ID, 40611, true); // Blaze (damage self)
            }

            uint32 FlameBlastTimer;
            uint32 CheckTimer;
            ObjectGuid GlaiveGUID;

            void Reset() override
            {
                FlameBlastTimer = 15000;
                CheckTimer = 5000;
                GlaiveGUID = ObjectGuid::Empty;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoZoneInCombat();
            }

            void ChargeCheck()
            {
                Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 200, false);
                if (target && (!me->IsWithinCombatRange(target, FLAME_CHARGE_DISTANCE)))
                {
                    me->AddThreat(target, 5000000.0f);
                    AttackStart(target);
                    DoCast(target, SPELL_CHARGE);
                    Talk(EMOTE_AZZINOTH_GAZE);
                }
            }

            void EnrageCheck()
            {
                if (Creature* glaive = ObjectAccessor::GetCreature(*me, GlaiveGUID))
                {
                    if (!me->IsWithinDistInMap(glaive, FLAME_ENRAGE_DISTANCE))
                    {
                        glaive->InterruptNonMeleeSpells(true);
                        DoCast(me, SPELL_FLAME_ENRAGE, true);
                        DoResetThreat();
                        Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                        if (target && target->IsAlive())
                        {
                            me->AddThreat(me->GetVictim(), 5000000.0f);
                            AttackStart(me->GetVictim());
                        }
                    }
                    else if (!me->HasAura(SPELL_AZZINOTH_CHANNEL))
                    {
                        glaive->CastSpell(me, SPELL_AZZINOTH_CHANNEL, false);
                        me->RemoveAurasDueToSpell(SPELL_FLAME_ENRAGE);
                    }
                }
            }

            void SetGlaiveGUID(ObjectGuid guid)
            {
                GlaiveGUID = guid;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (FlameBlastTimer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_BLAZE_SUMMON, true); // appear at victim
                    DoCast(me->GetVictim(), SPELL_FLAME_BLAST);
                    FlameBlastTimer = 15000; // 10000 is official-like?
                    DoZoneInCombat(); // in case someone is revived
                } else FlameBlastTimer -= diff;

                if (CheckTimer <= diff)
                {
                    ChargeCheck();
                    EnrageCheck();
                    CheckTimer = 1000;
                } else CheckTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new flame_of_azzinothAI(creature);
        }
};

/************************************** Illidan's AI ***************************************/
class boss_illidan_stormrage : public CreatureScript
{
    public:
        boss_illidan_stormrage() : CreatureScript("boss_illidan_stormrage") { }

        struct boss_illidan_stormrageAI : public ScriptedAI
        {
            boss_illidan_stormrageAI(Creature* creature) : ScriptedAI(creature), Summons(me)
            {
                instance = creature->GetInstanceScript();
                me->CastSpell(me, SPELL_DUAL_WIELD, true);
                //DoCast(me, SPELL_DUAL_WIELD, true);
                AkamaGUID = ObjectGuid::Empty;
            }

            InstanceScript* instance;
            Creature* temp;
            PhaseIllidan Phase;
            EventIllidan Event;
            uint32 Timer[EVENT_ENRAGE + 1];
            uint32 TalkCount;
            uint32 TransformCount;
            uint32 FlightCount;
            uint32 HoverPoint;
            ObjectGuid AkamaGUID;
            ObjectGuid MaievGUID;
            ObjectGuid FlameGUID[2];
            ObjectGuid GlaiveGUID[2];
            SummonList Summons;

            void Reset() override;

            void JustSummoned(Creature* summon) override;

            void SummonedCreatureDespawn(Creature* summon) override
            {
                if (summon->GetCreatureTemplate()->Entry == FLAME_OF_AZZINOTH)
                {
                    for (uint8 i = 0; i < 2; ++i)
                        if (summon->GetGUID() == FlameGUID[i])
                            FlameGUID[i] = ObjectGuid::Empty;

                    if (!FlameGUID[0] && !FlameGUID[1] && Phase != PHASE_ILLIDAN_NULL)
                    {
                        me->InterruptNonMeleeSpells(true);
                        EnterPhase(PHASE_FLIGHT_SEQUENCE);
                    }
                }
                Summons.Despawn(summon);
            }

            void MovementInform(uint32 type, uint32 /*pointId*/) override
            {
                me->m_Events.Schedule(1000, [this, type]()
                {
                    if (type == EFFECT_MOTION_TYPE)
                    {
                        me->StopMoving();
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveIdle();
                    }

                    if (Phase != PHASE_FLIGHT && Phase != PHASE_FLIGHT_SEQUENCE)
                        return;

                    if (FlightCount == 7) //change hover point
                    {
                        if (me->GetVictim())
                            me->SetFacingToObject(me->GetVictim());
                        EnterPhase(PHASE_FLIGHT);
                    }
                    else // handle flight sequence
                        Timer[EVENT_FLIGHT_SEQUENCE] = 1000;
                });
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->setActive(true);
                DoZoneInCombat();
            }

            void AttackStart(Unit* who) override
            {
                if (!who || Phase >= PHASE_TALK_SEQUENCE)
                    return;

                if (Phase == PHASE_FLIGHT || Phase == PHASE_DEMON)
                    AttackStartNoMove(who);
                else
                    ScriptedAI::AttackStart(who);
            }

            void MoveInLineOfSight(Unit* ) override { }

            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                if (!instance)
                    return;

                instance->SetData(DATA_ILLIDAN_STORMRAGE_EVENT, DONE); // Completed

                for (uint8 i = DATA_GAMEOBJECT_ILLIDAN_DOOR_R; i < DATA_GAMEOBJECT_ILLIDAN_DOOR_L + 1; ++i)
                    instance->HandleGameObject(instance->GetGuidData(i), true);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(SAY_ILLIDAN_KILL);
            }

            void DamageTaken(Unit* done_by, uint32& damage) override
            {
                if (damage >= me->GetHealth() && done_by != me)
                    damage = 0;
                if (done_by->GetGUID() == MaievGUID)
                    done_by->AddThreat(me, -(3*(float)damage)/4); // do not let maiev tank him
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_GLAIVE_RETURNS) // Re-equip our warblades!
                {
                    if (!me->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID))
                        SetEquipmentSlots(false, EQUIP_ID_MAIN_HAND, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                    else
                        SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_ID_OFF_HAND, EQUIP_NO_CHANGE);
                    me->SetSheath(SHEATH_STATE_MELEE);
                }
            }

            void DeleteFromThreatList(uint64 TargetGUID)
            {
                for (std::list<HostileReference*>::const_iterator itr = me->GetThreatManager().getThreatList().begin(); itr != me->GetThreatManager().getThreatList().end(); ++itr)
                {
                    if ((*itr)->getUnitGuid() == TargetGUID)
                    {
                        (*itr)->removeReference();
                        break;
                    }
                }
            }

            void TalkText(uint32 count)
            {
                Timer[EVENT_TALK_SEQUENCE] = Conversation[count].timer;

                Creature* creature = nullptr;
                if (Conversation[count].creature == ILLIDAN_STORMRAGE)
                    creature = me;
                else if (Conversation[count].creature == AKAMA)
                    creature = (Unit::GetCreature(*me, AkamaGUID));
                else if (Conversation[count].creature == MAIEV_SHADOWSONG)
                    creature = (Unit::GetCreature(*me, MaievGUID));

                if (creature)
                {
                    if (Conversation[count].emote)
                        creature->HandleEmoteCommand(Conversation[count].emote); // Make the Creature do some animation!
                    if (Conversation[count].text >= 0)
                        creature->AI()->Talk(Conversation[count].text); // Have the Creature yell out some text
                    if (Conversation[count].sound)
                        DoPlaySoundToSet(creature, Conversation[count].sound); // Play some sound on the creature
                }
            }

            void EnterPhase(PhaseIllidan NextPhase)
            {
                DoZoneInCombat();
                switch (NextPhase)
                {
                    case PHASE_NORMAL:
                    case PHASE_NORMAL_2:
                    case PHASE_NORMAL_MAIEV:
                        AttackStart(me->GetVictim());
                        Timer[EVENT_TAUNT] = 32000;
                        Timer[EVENT_SHEAR] = 10000 + rand()%15 * 1000;
                        Timer[EVENT_FLAME_CRASH] = 20000;
                        Timer[EVENT_PARASITIC_SHADOWFIEND] = 25000;
                        Timer[EVENT_PARASITE_CHECK] = 0;
                        Timer[EVENT_DRAW_SOUL] = 30000;
                        if (NextPhase == PHASE_NORMAL)
                            break;
                        Timer[EVENT_AGONIZING_FLAMES] = 35000;
                        Timer[EVENT_TRANSFORM_NORMAL] = 60000;
                        if (NextPhase == PHASE_NORMAL_2)
                            break;
                        Timer[EVENT_ENRAGE] = 30000 + rand() % 10 * 1000;
                        break;
                    case PHASE_FLIGHT:
                        Timer[EVENT_FIREBALL] = 1000;
                        if (!(rand()%4))
                            Timer[EVENT_DARK_BARRAGE] = 10000;
                        Timer[EVENT_EYE_BLAST] = 10000 + rand() % 15 * 1000;
                        Timer[EVENT_MOVE_POINT] = 20000 + rand() % 20 * 1000;
                        break;
                    case PHASE_DEMON:
                        Timer[EVENT_SHADOW_BLAST] = 1000;
                        Timer[EVENT_FLAME_BURST] = 10000;
                        Timer[EVENT_SHADOWDEMON] = 30000;
                        Timer[EVENT_TRANSFORM_DEMON] = 60000;
                        AttackStart(me->GetVictim());
                        break;
                    case PHASE_TALK_SEQUENCE:
                        Timer[EVENT_TALK_SEQUENCE] = 100;
                        me->RemoveAllAuras();
                        me->InterruptNonMeleeSpells(false);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                        me->GetMotionMaster()->Clear(false);
                        me->AttackStop();
                        break;
                    case PHASE_FLIGHT_SEQUENCE:
                        if (Phase == PHASE_NORMAL) //lift off
                        {
                            FlightCount = 1;
                            Timer[EVENT_FLIGHT_SEQUENCE] = 1;
                            me->RemoveAllAuras();
                            me->InterruptNonMeleeSpells(false);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            me->GetMotionMaster()->Clear(false);
                            me->AttackStop();
                        }
                        else // land
                        {
                            FlightCount = 7; // Could be not 7 if adds were killed too fast
                            Timer[EVENT_FLIGHT_SEQUENCE] = 2000;
                            me->GetMotionMaster()->Clear(false); // Otherwise if MovementInform happens within the next 2 seconds the phase will be set back to PHASE_FLIGHT
                        }
                        break;
                    case PHASE_TRANSFORM_SEQUENCE:
                        if (Phase == PHASE_DEMON)
                            Timer[EVENT_TRANSFORM_SEQUENCE] = 500;
                        else
                        {
                            TransformCount = 0;
                            Timer[EVENT_TRANSFORM_SEQUENCE] = 500;
                            Talk(SAY_ILLIDAN_MORPH);
                        }
                        me->GetMotionMaster()->Clear();
                        me->AttackStop();
                        break;
                    default:
                        break;
                }
                if (MaievGUID)
                {
                    if (Creature* maiev = ObjectAccessor::GetCreature(*me, MaievGUID))
                        if (maiev->IsAlive())
                            maiev->AI()->DoAction(NextPhase);
                }
                Phase = NextPhase;
                Event = EVENT_NULL;
            }

            void CastEyeBlast()
            {
                me->InterruptNonMeleeSpells(false);

                Talk(SAY_ILLIDAN_EYE_BLAST);

                float distx, disty, dist[2];
                for (uint8 i = 0; i < 2; ++i)
                {
                    distx = EyeBlast[i].x - HoverPosition[HoverPoint].x;
                    disty = EyeBlast[i].y - HoverPosition[HoverPoint].y;
                    dist[i] = distx * distx + disty * disty;
                }
                Locations initial = EyeBlast[dist[0] < dist[1] ? 0 : 1];
                for (uint8 i = 0; i < 2; ++i)
                {
                    distx = GlaivePosition[i].x - HoverPosition[HoverPoint].x;
                    disty = GlaivePosition[i].y - HoverPosition[HoverPoint].y;
                    dist[i] = distx * distx + disty * disty;
                }
                Locations final = GlaivePosition[dist[0] < dist[1] ? 0 : 1];

                final.x = 2 * final.x - initial.x;
                final.y = 2 * final.y - initial.y;

                Creature* trigger = me->SummonCreature(23069, initial.x, initial.y, initial.z, 0, TEMPSUMMON_TIMED_DESPAWN, 13000ms);
                if (!trigger)
                    return;

                trigger->SetSpeed(MOVE_WALK, 3);
                trigger->SetWalk(true);
                trigger->GetMotionMaster()->MovePoint(0, final.x, final.y, final.z);

                //trigger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetTarget(trigger->GetGUID());
                DoCast(trigger, SPELL_EYE_BLAST);
            }

            void SummonFlamesOfAzzinoth()
            {
                Talk(SAY_ILLIDAN_SUMMONFLAMES);

                for (uint8 i = 0; i < 2; ++i)
                {
                    if (Creature* glaive = ObjectAccessor::GetCreature(*me, GlaiveGUID[i]))
                    {
                        Creature* flame = me->SummonCreature(FLAME_OF_AZZINOTH, GlaivePosition[i+2].x, GlaivePosition[i+2].y, GlaivePosition[i+2].z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000ms);
                        if (flame)
                        {
                            flame->SetFaction(me->GetFaction()); // Just in case the database has it as a different faction
                            flame->SetMeleeDamageSchool(SPELL_SCHOOL_FIRE);
                            FlameGUID[i] = flame->GetGUID(); // Record GUID in order to check if they're dead later on to move to the next phase
                            CAST_AI(npc_flame_of_azzinoth::flame_of_azzinothAI, flame->AI())->SetGlaiveGUID(GlaiveGUID[i]);
                            glaive->CastSpell(flame, SPELL_AZZINOTH_CHANNEL, false); // Glaives do some random Beam type channel on it.
                        }
                    }
                }
            }
            void SummonMaiev()
            {
                //DoCast(me, SPELL_SHADOW_PRISON, true);
                // Spell causes client crash!
                if (instance)
                {
                    Map::PlayerList const& PlayerList = instance->instance->GetPlayers();
                    if (!PlayerList.isEmpty())
                        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                            if (Player* player = itr->GetSource())
                                me->AddAura(SPELL_SHADOW_PRISON, player);
                }
                DoCast(me, 40403, true);
                if (!MaievGUID) // If Maiev cannot be summoned, reset the encounter and post some errors to the console.
                {
                    EnterEvadeMode();
                    TC_LOG_ERROR("misc", "SD2 ERROR: Unable to summon Maiev Shadowsong (entry: 23197). Check your database to see if you have the proper SQL for Maiev Shadowsong (entry: 23197)");
                }
            }

            void HandleTalkSequence();

            void HandleFlightSequence()
            {
                switch (FlightCount)
                {
                    case 1: // lift off
                        me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                        me->OverrideInhabitType(INHABIT_AIR);
                        me->SetFlying(true);
                        me->UpdateMovementFlags();
                        Talk(SAY_ILLIDAN_TAKEOFF);
                        Timer[EVENT_FLIGHT_SEQUENCE] = 3000;
                        break;
                    case 2: // move to center
                        me->GetMotionMaster()->MoveTakeoff(0, Position { CENTER_X + 5, CENTER_Y, CENTER_Z }); //+5, for SPELL_THROW_GLAIVE bug
                        Timer[EVENT_FLIGHT_SEQUENCE] = 0;
                        break;
                    case 3: // throw one glaive
                        {
                            uint8 i = 1;
                            Creature* glaive = me->SummonCreature(BLADE_OF_AZZINOTH, GlaivePosition[i].x, GlaivePosition[i].y, GlaivePosition[i].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0ms);
                            if (glaive)
                            {
                                GlaiveGUID[i] = glaive->GetGUID();
                                glaive->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                //glaive->SetDisplayId(11686); // idk how it must works, but glaive shoud not be invisible
                                glaive->SetFaction(me->GetFaction());
                                DoCast(glaive, SPELL_THROW_GLAIVE2);
                            }
                        }
                        Timer[EVENT_FLIGHT_SEQUENCE] = 700;
                        break;
                    case 4: // throw another
                        SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                        {
                            uint8 i = 0;
                            Creature* glaive = me->SummonCreature(BLADE_OF_AZZINOTH, GlaivePosition[i].x, GlaivePosition[i].y, GlaivePosition[i].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0ms);
                            if (glaive)
                            {
                                GlaiveGUID[i] = glaive->GetGUID();
                                glaive->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                //glaive->SetDisplayId(11686); // idk how it must works, but glaive shoud not be invisible
                                glaive->SetFaction(me->GetFaction());
                                DoCast(glaive, SPELL_THROW_GLAIVE, true);
                            }
                        }
                        Timer[EVENT_FLIGHT_SEQUENCE] = 5000;
                        break;
                    case 5: // summon flames
                        SummonFlamesOfAzzinoth();
                        Timer[EVENT_FLIGHT_SEQUENCE] = 3000;
                        break;
                    case 6: // fly to hover point
                        me->GetMotionMaster()->MoveTakeoff(0, Position { HoverPosition[HoverPoint].x, HoverPosition[HoverPoint].y, HoverPosition[HoverPoint].z });
                        Timer[EVENT_FLIGHT_SEQUENCE] = 0;
                        break;
                    case 7: // return to center
                        me->GetMotionMaster()->MoveTakeoff(0, Position { CENTER_X, CENTER_Y, CENTER_Z });
                        Timer[EVENT_FLIGHT_SEQUENCE] = 0;
                        break;
                    case 8: // glaive return
                        for (uint8 i = 0; i < 2; ++i)
                        {
                            if (GlaiveGUID[i])
                            {
                                Unit* glaive = Unit::GetUnit(*me, GlaiveGUID[i]);
                                if (glaive)
                                {
                                    glaive->CastSpell(me, SPELL_GLAIVE_RETURNS, false); // Make it look like the Glaive flies back up to us
                                    glaive->SetDisplayId(11686); // disappear but not die for now
                                }
                            }
                        }
                        Timer[EVENT_FLIGHT_SEQUENCE] = 2000;
                        break;
                    case 9: // land
                        me->ResetInhabitTypeOverride();
                        me->SetFlying(false);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                        for (uint8 i = 0; i < 2; ++i)
                        {
                            if (GlaiveGUID[i])
                            {
                                Unit* glaive = Unit::GetUnit(*me, GlaiveGUID[i]);
                                {
                                    glaive->SetVisible(false);
                                    glaive->setDeathState(JUST_DIED); // Despawn the Glaive
                                }
                                GlaiveGUID[i] = ObjectGuid::Empty;
                            }
                        }
                        Timer[EVENT_FLIGHT_SEQUENCE] = 2000;
                        break;
                    case 10: // attack
                        me->m_Events.Schedule(1000, [this]()
                        {
                            DoResetThreat();
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                            SetEquipmentSlots(false, EQUIP_ID_MAIN_HAND, EQUIP_ID_OFF_HAND, EQUIP_NO_CHANGE);
                            me->SetSheath(SHEATH_STATE_MELEE);
                            EnterPhase(PHASE_NORMAL_2);
                        });
                        break;
                    default:
                        break;
                }
                ++FlightCount;
            }

            void HandleTransformSequence()
            {
                if (DemonTransformation[TransformCount].unaura)
                    me->RemoveAurasDueToSpell(DemonTransformation[TransformCount].unaura);

                if (DemonTransformation[TransformCount].aura)
                    DoCast(me, DemonTransformation[TransformCount].aura, true);

                if (DemonTransformation[TransformCount].displayid)
                    me->SetDisplayId(DemonTransformation[TransformCount].displayid); // It's morphin time!

                if (DemonTransformation[TransformCount].equip)
                {
                    // Requip warglaives if needed
                    SetEquipmentSlots(false, EQUIP_ID_MAIN_HAND, EQUIP_ID_OFF_HAND, EQUIP_NO_CHANGE);
                    me->SetSheath(SHEATH_STATE_MELEE);
                }
                else
                {
                    // Unequip warglaives if needed
                    SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                }

                switch (TransformCount)
                {
                    case 2:
                        DoResetThreat();
                        break;
                    case 4:
                        EnterPhase(PHASE_DEMON);
                        break;
                    case 7:
                        DoResetThreat();
                        break;
                    case 9:
                        if (MaievGUID)
                            EnterPhase(PHASE_NORMAL_MAIEV); // Depending on whether we summoned Maiev, we switch to either phase 5 or 3
                        else
                            EnterPhase(PHASE_NORMAL_2);
                        break;
                    default:
                        break;
                }
                if (Phase == PHASE_TRANSFORM_SEQUENCE)
                    Timer[EVENT_TRANSFORM_SEQUENCE] = DemonTransformation[TransformCount].timer;
                ++TransformCount;
            }

            void UpdateAI(uint32 diff) override
            {
                if ((!UpdateVictim()) && Phase < PHASE_TALK_SEQUENCE)
                    return;

                Event = EVENT_NULL;
                for (int32 i = 1; i <= MaxTimer[Phase]; ++i)
                {
                    if (Timer[i]) // Event is enabled
                    {
                        if (Timer[i] <= diff)
                        {
                            if (!Event) // No event with higher priority
                                Event = (EventIllidan)i;
                        }
                        else Timer[i] -= diff;
                    }
                }

                switch (Phase)
                {
                    case PHASE_NORMAL:
                        if (HealthBelowPct(65))
                            EnterPhase(PHASE_FLIGHT_SEQUENCE);
                        break;
                    case PHASE_NORMAL_2:
                        if (HealthBelowPct(30))
                            EnterPhase(PHASE_TALK_SEQUENCE);
                        break;
                    case PHASE_NORMAL_MAIEV:
                        if (HealthBelowPct(1))
                            EnterPhase(PHASE_TALK_SEQUENCE);
                        break;
                    case PHASE_TALK_SEQUENCE:
                        if (Event == EVENT_TALK_SEQUENCE)
                            HandleTalkSequence();
                        break;
                    case PHASE_FLIGHT_SEQUENCE:
                        if (Event == EVENT_FLIGHT_SEQUENCE)
                            HandleFlightSequence();
                        break;
                    case PHASE_TRANSFORM_SEQUENCE:
                        if (Event == EVENT_TRANSFORM_SEQUENCE)
                            HandleTransformSequence();
                        break;
                    default:
                        break;
                }

                if (me->IsNonMeleeSpellCasted(false))
                    return;

                if (Phase == PHASE_NORMAL || Phase == PHASE_NORMAL_2 || (Phase == PHASE_NORMAL_MAIEV && !me->HasAura(SPELL_CAGED)))
                {
                    switch (Event)
                    {
                        // PHASE_NORMAL
                        case EVENT_BERSERK:
                            Talk(SAY_ILLIDAN_ENRAGE);
                            DoCast(me, SPELL_BERSERK, true);
                            Timer[EVENT_BERSERK] = 5000;//The buff actually lasts forever.
                            break;
                        case EVENT_TAUNT:
                            Talk(SAY_ILLIDAN_TAUNT);
                            Timer[EVENT_TAUNT] = 25000 + rand()%10000;
                            break;
                        case EVENT_SHEAR:
                            // no longer exists in 3.0f.2
                            //DoCast(me->GetVictim(), SPELL_SHEAR);
                            Timer[EVENT_SHEAR] = 25000 + (rand()%16 * 1000);
                            break;
                        case EVENT_FLAME_CRASH:
                            DoCast(me->GetVictim(), SPELL_FLAME_CRASH);
                            Timer[EVENT_FLAME_CRASH] = 30000 + rand()%10000;
                            break;
                        case EVENT_PARASITIC_SHADOWFIEND:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 200, true))
                                DoCast(target, SPELL_PARASITIC_SHADOWFIEND, true);
                            Timer[EVENT_PARASITIC_SHADOWFIEND] = 35000 + rand() % 10000;
                            break;
                        }
                        case EVENT_PARASITE_CHECK:
                            Timer[EVENT_PARASITE_CHECK] = 0;
                            break;
                        case EVENT_DRAW_SOUL:
                            //DoCast(me->GetVictim(), SPELL_DRAW_SOUL);
                            DoCastAOE(SPELL_DRAW_SOUL);
                            Timer[EVENT_DRAW_SOUL] = 50000 + rand()%10000;
                            break;
                            //PHASE_NORMAL_2
                        case EVENT_AGONIZING_FLAMES:
                            DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0), SPELL_AGONIZING_FLAMES);
                            Timer[EVENT_AGONIZING_FLAMES] = 0;
                            break;
                        case EVENT_TRANSFORM_NORMAL:
                            EnterPhase(PHASE_TRANSFORM_SEQUENCE);
                            break;
                            //PHASE_NORMAL_MAIEV
                        case EVENT_ENRAGE:
                            DoCast(me, SPELL_ENRAGE);
                            Timer[EVENT_ENRAGE] = 0;
                            break;
                        default:
                            break;
                    }
                    DoMeleeAttackIfReady();
                }

                if (Phase == PHASE_FLIGHT)
                {
                    switch (Event)
                    {
                        case EVENT_FIREBALL:
                            DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0), SPELL_FIREBALL);
                            Timer[EVENT_FIREBALL] = 3000;
                            break;
                        case EVENT_DARK_BARRAGE:
                            DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0), SPELL_DARK_BARRAGE);
                            Timer[EVENT_DARK_BARRAGE] = 0;
                            break;
                        case EVENT_EYE_BLAST:
                            CastEyeBlast();
                            Timer[EVENT_EYE_BLAST] = 0;
                            break;
                        case EVENT_MOVE_POINT:
                            Phase = PHASE_FLIGHT_SEQUENCE;
                            Timer[EVENT_FLIGHT_SEQUENCE] = 0;//do not start Event when changing hover point
                            HoverPoint += (rand()%3 + 1);
                            if (HoverPoint > 3)
                                HoverPoint -= 4;
                            me->GetMotionMaster()->MovePoint(0, HoverPosition[HoverPoint].x, HoverPosition[HoverPoint].y, HoverPosition[HoverPoint].z);
                            break;
                        default:
                            break;
                    }
                }

                if (Phase == PHASE_DEMON)
                {
                    switch (Event)
                    {
                        case EVENT_SHADOW_BLAST:
                            me->GetMotionMaster()->Clear(false);
                            if (!me->IsWithinDistInMap(me->GetVictim(), 50)||!me->IsWithinLOSInMap(me->GetVictim()))
                                me->GetMotionMaster()->MoveChase(me->GetVictim(), 30);
                            else
                                me->GetMotionMaster()->MoveIdle();
                            DoCast(me->GetVictim(), SPELL_SHADOW_BLAST);
                            Timer[EVENT_SHADOW_BLAST] = 4000;
                            break;
                        case EVENT_SHADOWDEMON:
                            if (instance && instance->instance->GetPlayersCountExceptGMs() > 1) // not use this if solo run
                                DoCast(me, SPELL_SUMMON_SHADOWDEMON);
                            //for (int i = 0; i < 4; i++)
                            //{
                            //    temp = DoSpawnCreature(SHADOW_DEMON, 0, 0, 0, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000ms);
                            //    if (temp)
                            //    {
                            //        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 200, true))
                            //            temp->AI()->AttackStart(target);
                            //    }
                            //}
                            Timer[EVENT_SHADOWDEMON] = 0;
                            Timer[EVENT_FLAME_BURST] += 10000;
                            break;
                        case EVENT_FLAME_BURST:
                            DoCast(me, SPELL_FLAME_BURST);
                            Timer[EVENT_FLAME_BURST] = 15000;
                            break;
                        case EVENT_TRANSFORM_DEMON:
                            EnterPhase(PHASE_TRANSFORM_SEQUENCE);
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_illidan_stormrageAI(creature);
        }
};

/********************************** End of Illidan AI ******************************************/

/******* Functions and vars for Akama's AI ******/
class boss_maiev_shadowsong : public CreatureScript
{
    public:
        boss_maiev_shadowsong() : CreatureScript("boss_maiev_shadowsong") { }

        struct boss_maievAI : public ScriptedAI
        {
            boss_maievAI(Creature* creature) : ScriptedAI(creature) { };

            ObjectGuid IllidanGUID;
            PhaseIllidan Phase;
            EventMaiev Event;
            uint32 Timer[5];
            uint32 MaxTimer;

            void Reset() override
            {
                MaxTimer = 0;
                Phase = PHASE_NORMAL_MAIEV;
                IllidanGUID = ObjectGuid::Empty;
                Timer[EVENT_MAIEV_STEALTH] = 0;
                Timer[EVENT_MAIEV_TAUNT] = 22000 + rand() % 21 * 1000;
                Timer[EVENT_MAIEV_SHADOW_STRIKE] = 30000;
                SetEquipmentSlots(false, 44850, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 2, 32326);
            }

            void JustEngagedWith(Unit* /*who*/) override { }
            void MoveInLineOfSight(Unit* /*who*/) override { }
            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override { }

            void GetIllidanGUID(ObjectGuid guid)
            {
                IllidanGUID = guid;
            }

            void DamageTaken(Unit* done_by, uint32& damage) override
            {
                if (done_by->GetGUID() != IllidanGUID)
                    damage = 0;
                else
                {
                    if (Creature* illidan = ObjectAccessor::GetCreature(*me, IllidanGUID))
                        if (illidan->GetVictim() == me)
                            damage = me->CountPctFromMaxHealth(10);

                    if (damage >= me->GetHealth())
                        damage = 0;
                }
            }

            void AttackStart(Unit* who) override
            {
                if (!who || Timer[EVENT_MAIEV_STEALTH])
                    return;

                if (Phase == PHASE_TALK_SEQUENCE)
                    AttackStartNoMove(who);
                else if (Phase == PHASE_DEMON || Phase == PHASE_TRANSFORM_SEQUENCE)
                {
                    if (Creature* illidan = ObjectAccessor::GetCreature(*me, IllidanGUID))
                        if (me->IsWithinDistInMap(illidan, 25))
                            BlinkToPlayer(); // Do not let dread aura hurt her.
                    AttackStartNoMove(who);
                }
                else
                    ScriptedAI::AttackStart(who);
            }

            void DoAction(int32 action) override
            {
                if (action > PHASE_ILLIDAN_NULL && action < PHASE_ILLIDAN_MAX)
                    EnterPhase(PhaseIllidan(action));
            }

            void EnterPhase(PhaseIllidan NextPhase)//This is in fact Illidan's phase.
            {
                switch (NextPhase)
                {
                    case PHASE_TALK_SEQUENCE:
                        if (Timer[EVENT_MAIEV_STEALTH])
                        {
                            me->SetFullHealth();
                            me->SetVisible(true);
                            Timer[EVENT_MAIEV_STEALTH] = 0;
                        }
                        me->InterruptNonMeleeSpells(false);
                        me->GetMotionMaster()->Clear(false);
                        me->AttackStop();
                        me->SetTarget(IllidanGUID);
                        MaxTimer = 0;
                        break;
                    case PHASE_TRANSFORM_SEQUENCE:
                        MaxTimer = 4;
                        Timer[EVENT_MAIEV_TAUNT] += 10000;
                        Timer[EVENT_MAIEV_THROW_DAGGER] = 2000;
                        break;
                    case PHASE_DEMON:
                        break;
                    case PHASE_NORMAL_MAIEV:
                        MaxTimer = 4;
                        Timer[EVENT_MAIEV_TAUNT] += 10000;
                        Timer[EVENT_MAIEV_TRAP] = 22000;
                        break;
                    default:
                        break;
                }
                if (Timer[EVENT_MAIEV_STEALTH])
                    MaxTimer = 1;
                Phase = NextPhase;
            }

            void BlinkTo(float x, float y, float z)
            {
                me->AttackStop();
                me->InterruptNonMeleeSpells(false);
                me->GetMotionMaster()->Clear(false);
                DoTeleportTo(x, y, z);
                DoCast(me, SPELL_TELEPORT_VISUAL, true);
            }

            void BlinkToPlayer()
            {
                if (Creature* illidan = ObjectAccessor::GetCreature(*me, IllidanGUID))
                {
                    Unit* target = CAST_AI(boss_illidan_stormrage::boss_illidan_stormrageAI, illidan->AI())->SelectTarget(SELECT_TARGET_RANDOM, 0);

                    if (!target || !me->IsWithinDistInMap(target, 80) || illidan->IsWithinDistInMap(target, 20))
                    {
                        uint8 pos = rand() % 4;
                        BlinkTo(HoverPosition[pos].x, HoverPosition[pos].y, HoverPosition[pos].z);
                    }
                    else
                    {
                        float x, y, z;
                        target->GetPosition(x, y, z);
                        BlinkTo(x, y, z);
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if ((!UpdateVictim())
                    && !Timer[EVENT_MAIEV_STEALTH])
                    return;

                Event = EVENT_MAIEV_NULL;
                for (uint8 i = 1; i <= MaxTimer; ++i)
                    if (Timer[i])
                    {
                        if (Timer[i] <= diff)
                            Event = (EventMaiev)i;
                        else Timer[i] -= diff;
                    }

                    switch (Event)
                    {
                        case EVENT_MAIEV_STEALTH:
                        {
                            me->SetFullHealth();
                            me->SetVisible(true);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            Timer[EVENT_MAIEV_STEALTH] = 0;
                            BlinkToPlayer();
                            EnterPhase(Phase);
                            break;
                        }
                        case EVENT_MAIEV_TAUNT:
                        {
                            Talk(SAY_MAIEV_SHADOWSONG_TAUNT);
                            Timer[EVENT_MAIEV_TAUNT] = 22000 + rand() % 21 * 1000;
                            break;
                        }
                        case EVENT_MAIEV_SHADOW_STRIKE:
                            DoCast(me->GetVictim(), SPELL_SHADOW_STRIKE);
                            Timer[EVENT_MAIEV_SHADOW_STRIKE] = 60000;
                            break;
                        case EVENT_MAIEV_TRAP:
                            if (Phase == PHASE_NORMAL_MAIEV)
                            {
                                BlinkToPlayer();
                                DoCast(me, SPELL_CAGE_TRAP_SUMMON);
                                Timer[EVENT_MAIEV_TRAP] = 22000;
                            }
                            else
                            {
                                if (!me->IsWithinDistInMap(me->GetVictim(), 40))
                                    me->GetMotionMaster()->MoveChase(me->GetVictim(), 30);
                                DoCast(me->GetVictim(), SPELL_THROW_DAGGER);
                                Timer[EVENT_MAIEV_THROW_DAGGER] = 2000;
                            }
                            break;
                        default:
                            break;
                    }

                    if (HealthBelowPct(50))
                    {
                        me->SetVisible(false);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        if (Creature* illidan = ObjectAccessor::GetCreature(*me, IllidanGUID))
                            CAST_AI(boss_illidan_stormrage::boss_illidan_stormrageAI, illidan->AI())->DeleteFromThreatList(me->GetGUID());
                        me->AttackStop();
                        Timer[EVENT_MAIEV_STEALTH] = 60000; //reappear after 1 minute
                        MaxTimer = 1;
                    }

                    if (Phase == PHASE_NORMAL_MAIEV)
                        DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_maievAI(creature);
        }
};

class npc_akama_illidan : public CreatureScript
{
    public:
        npc_akama_illidan() : CreatureScript("npc_akama_illidan") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            if (action == GOSSIP_ACTION_INFO_DEF) // Time to begin the Event
            {
                player->CLOSE_GOSSIP_MENU();
                //CAST_AI(npc_akama_illidan::npc_akama_illidanAI, creature->AI())->EnterPhase(PHASE_CHANNEL);
                InstanceScript* instance = creature->GetInstanceScript();

                if (!instance || instance->GetData(DATA_ILLIDAN_GATE_OPENED) != DONE)
                {
                    ((npc_akama_illidanAI*)creature->AI())->EnterPhase(PHASE_CHANNEL);
                }
                else
                {
                    ((npc_akama_illidanAI*)creature->AI())->Phase = PHASE_CHANNEL;
                    ((npc_akama_illidanAI*)creature->AI())->EnterPhase(PHASE_WALK);
                }
            }
            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            InstanceScript* instance = creature->GetInstanceScript();

            if (instance && instance->GetData(DATA_ILLIDAN_STORMRAGE_EVENT) == NOT_STARTED && instance->CheckRequiredBosses(DATA_ILLIDAN_STORMRAGE_EVENT, player))
                player->ADD_GOSSIP_ITEM_DB(GOSSIP_ITEM, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

            player->SEND_GOSSIP_MENU(10465, creature->GetGUID());
            return true;
        }

        struct npc_akama_illidanAI : public ScriptedAI
        {
            npc_akama_illidanAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                JustCreated = true;
            }
            bool JustCreated;
            InstanceScript* instance;

            PhaseAkama Phase;
            bool Event;
            uint32 Timer;

            ObjectGuid IllidanGUID;
            ObjectGuid ChannelGUID;
            ObjectGuid SpiritGUID[2];
            ObjectGuid GateGUID;
            ObjectGuid DoorGUID[2];

            uint32 ChannelCount;
            uint32 WalkCount;
            uint32 TalkCount;
            uint32 Check_Timer;

            void Reset() override
            {
                WalkCount = 0;
                if (instance)
                {
                    instance->SetData(DATA_ILLIDAN_STORMRAGE_EVENT, NOT_STARTED);

                    IllidanGUID = instance->GetGuidData(DATA_ILLIDAN_STORMRAGE);
                    GateGUID = instance->GetGuidData(DATA_GAMEOBJECT_ILLIDAN_GATE);
                    DoorGUID[0] = instance->GetGuidData(DATA_GAMEOBJECT_ILLIDAN_DOOR_R);
                    DoorGUID[1] = instance->GetGuidData(DATA_GAMEOBJECT_ILLIDAN_DOOR_L);

                    if (JustCreated) // close all doors at create
                    {
                        instance->HandleGameObject(GateGUID, false);

                        for (uint8 i = 0; i < 2; ++i)
                            instance->HandleGameObject(DoorGUID[i], false);
                    }
                    else // open all doors, raid wiped
                    {
                        instance->HandleGameObject(GateGUID, true);
                        WalkCount = 1; // skip first wp
                        for (uint8 i = 0; i < 2; ++i)
                            instance->HandleGameObject(DoorGUID[i], true);
                    }
                }
                else
                {
                    IllidanGUID = ObjectGuid::Empty;
                    GateGUID = ObjectGuid::Empty;
                    DoorGUID[0] = ObjectGuid::Empty;
                    DoorGUID[1] = ObjectGuid::Empty;
                }

                ChannelGUID = ObjectGuid::Empty;
                SpiritGUID[0] = ObjectGuid::Empty;
                SpiritGUID[1] = ObjectGuid::Empty;

                Phase = PHASE_AKAMA_NULL;
                Timer = 0;

                ChannelCount = 0;
                TalkCount = 0;
                Check_Timer = 5000;

                KillAllElites();

                me->SetUInt32Value(UNIT_FIELD_NPC_FLAGS, 0); // Database sometimes has strange values..
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                me->setActive(false);
                me->SetVisible(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            // Do not call reset in Akama's evade mode, as this will stop him from summoning minions after he kills the first bit
            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                //me->InterruptNonMeleeSpells(true);
                me->RemoveAllAuras();
                me->DeleteThreatList();
                me->CombatStop(true);
            }

            void JustEngagedWith(Unit* /*who*/) override { }
            void MoveInLineOfSight(Unit* /*who*/) override { }

            void MovementInform(uint32 type, uint32 /*pointId*/) override
            {
                if (type == POINT_MOTION_TYPE)
                    Timer = 1;
            }

            void DamageTaken(Unit* done_by, uint32& damage) override
            {
                if (damage > me->GetHealth() || done_by->GetGUID() != IllidanGUID)
                    damage = 0;
            }

            void KillAllElites()
            {
                ThreatContainer::StorageType const& threatList = me->GetThreatManager().getThreatList();
                std::vector<Unit*> eliteList;
                for (ThreatContainer::StorageType::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
                {
                    Unit* unit = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                    if (unit && unit->GetEntry() == ILLIDARI_ELITE)
                        eliteList.push_back(unit);
                }
                for (std::vector<Unit*>::const_iterator itr = eliteList.begin(); itr != eliteList.end(); ++itr)
                    (*itr)->setDeathState(JUST_DIED);
                EnterEvadeMode();
            }

            void BeginTalk()
            {
                if (!instance)
                    return;

                instance->SetData(DATA_ILLIDAN_STORMRAGE_EVENT, IN_PROGRESS);

                if (instance)
                {
                    DoorGUID[0] = instance->GetGuidData(DATA_GAMEOBJECT_ILLIDAN_DOOR_R);
                    DoorGUID[1] = instance->GetGuidData(DATA_GAMEOBJECT_ILLIDAN_DOOR_L);
                }

                for (uint8 i = 0; i < 2; ++i)
                    instance->HandleGameObject(DoorGUID[i], false);

                if (Creature* illidan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ILLIDAN_STORMRAGE)))
                {
                    illidan->RemoveAurasDueToSpell(SPELL_KNEEL);
                    me->SetInFront(illidan);
                    illidan->SetInFront(me);
                    me->GetMotionMaster()->MoveIdle();
                    illidan->GetMotionMaster()->MoveIdle();
                    CAST_AI(boss_illidan_stormrage::boss_illidan_stormrageAI, illidan->AI())->AkamaGUID = me->GetGUID();
                    CAST_AI(boss_illidan_stormrage::boss_illidan_stormrageAI, illidan->AI())->EnterPhase(PHASE_TALK_SEQUENCE);
                }
            }

            void BeginChannel()
            {
                me->setActive(true);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                if (!JustCreated)
                    return;

                float x, y, z;
                // if (GameObject* gate = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(DATA_GO_ILLIDAN_GATE))) // not work with new invis system
                //     gate->GetPosition(x, y, z);
                GateGUID = instance->GetGuidData(DATA_GAMEOBJECT_ILLIDAN_GATE);
                if (GameObject* obj = instance->instance->GetGameObject(GateGUID))
                    obj->GetPosition(x, y, z);
                else
                    return; //if door not spawned, don't crash server

                if (Creature* Channel = me->SummonCreature(ILLIDAN_DOOR_TRIGGER, x, y, z+5, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 360000ms))
                //if (Creature* Channel = me->SummonCreature(ILLIDAN_DOOR_TRIGGER, 771.637f, 304.536f, 315.156f, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 360000ms))
                {
                    ChannelGUID = Channel->GetGUID();
                    Channel->SetDisplayId(11686); // Invisible but spell visuals can still be seen.
                    DoCast(Channel, SPELL_AKAMA_DOOR_FAIL);
                }

                for (uint8 i = 0; i < 2; ++i)
                    if (Creature* Spirit = me->SummonCreature(i ? SPIRIT_OF_OLUM : SPIRIT_OF_UDALO, SpiritSpawns[i].x, SpiritSpawns[i].y, SpiritSpawns[i].z, 0, TEMPSUMMON_TIMED_DESPAWN, 20000ms))
                    {
                        Spirit->SetVisible(false);
                        SpiritGUID[i] = Spirit->GetGUID();
                    }
            }

            void BeginWalk()
            {
                me->SetWalk(false);
                me->SetSpeed(MOVE_RUN, 1.0f);
                me->GetMotionMaster()->MovePoint(0, AkamaWP[WalkCount].x, AkamaWP[WalkCount].y, AkamaWP[WalkCount].z);
            }

            void EnterPhase(PhaseAkama NextPhase)
            {
                if (!instance)
                    return;
                switch (NextPhase)
                {
                    case PHASE_CHANNEL:
                        BeginChannel();
                        Timer = 5000;
                        ChannelCount = 0;
                        break;
                    case PHASE_WALK:
                        if (Phase == PHASE_CHANNEL)
                            WalkCount = 0;
                        else if (Phase == PHASE_TALK)
                        {
                            if (Creature* illidan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ILLIDAN_STORMRAGE)))
                                CAST_AI(boss_illidan_stormrage::boss_illidan_stormrageAI, illidan->AI())->DeleteFromThreatList(me->GetGUID());
                            EnterEvadeMode();
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            ++WalkCount;
                        }
                        JustCreated = false;
                        BeginWalk();
                        Timer = 0;
                        break;
                    case PHASE_TALK:
                        if (Phase == PHASE_WALK)
                        {
                            BeginTalk();
                            Timer = 0;
                        }
                        else if (Phase == PHASE_FIGHT_ILLIDAN)
                        {
                            Timer = 1;
                            TalkCount = 0;
                        }
                        break;
                    case PHASE_FIGHT_ILLIDAN:
                        if (Creature* illidan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ILLIDAN_STORMRAGE)))
                        {
                            me->AddThreat(illidan, 10000000.0f);
                            me->GetMotionMaster()->MoveChase(illidan);
                        }
                        Timer = 30000; //chain lightning
                        break;
                    case PHASE_FIGHT_MINIONS:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        Timer = 10000 + rand()%6000;//summon minion
                        break;
                    case PHASE_RETURN:
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        KillAllElites();
                        WalkCount = 0;
                        BeginWalk();
                        Timer = 1;
                        break;
                    default:
                        break;
                }
                Phase = NextPhase;
                Event = false;
            }

            void HandleTalkSequence()
            {
                switch (TalkCount)
                {
                    case 0:
                        if (Creature* illidan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ILLIDAN_STORMRAGE)))
                        {
                            CAST_AI(boss_illidan_stormrage::boss_illidan_stormrageAI, illidan->AI())->Timer[EVENT_TAUNT] += 30000;
                            illidan->AI()->Talk(SAY_ILLIDAN_MINION);
                        }
                        Timer = 8000;
                        break;
                    case 1:
                        Talk(SAY_AKAMA_LEAVE);
                        Timer = 3000;
                        break;
                    case 2:
                        EnterPhase(PHASE_WALK);
                        break;
                }
                ++TalkCount;
            }

            void HandleChannelSequence()
            {
                Unit* Channel = nullptr, *Spirit[2];
                if (ChannelCount <= 5)
                {
                    Channel = Unit::GetUnit(*me, ChannelGUID);
                    Spirit[0] = Unit::GetUnit(*me, SpiritGUID[0]);
                    Spirit[1] = Unit::GetUnit(*me, SpiritGUID[1]);
                    if (!Channel || !Spirit[0] || !Spirit[1])
                        return;
                }

                switch (ChannelCount)
                {
                    case 0: // channel failed
                        me->InterruptNonMeleeSpells(true);
                        Timer = 2000;
                        break;
                    case 1: // spirit appear
                        Spirit[0]->SetVisible(true);
                        Spirit[1]->SetVisible(true);
                        Timer = 2000;
                        break;
                    case 2: // spirit help
                        DoCast(Channel, SPELL_AKAMA_DOOR_CHANNEL);
                        Spirit[0]->CastSpell(Channel, SPELL_DEATHSWORN_DOOR_CHANNEL,false);
                        Spirit[1]->CastSpell(Channel, SPELL_DEATHSWORN_DOOR_CHANNEL,false);
                        Timer = 5000;
                        break;
                    case 3: //open the gate
                        me->InterruptNonMeleeSpells(true);
                        Spirit[0]->InterruptNonMeleeSpells(true);
                        Spirit[1]->InterruptNonMeleeSpells(true);
                        if (instance)
                            instance->HandleGameObject(GateGUID, true);
                        Timer = 2000;
                        break;
                    case 4:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                        Timer = 2000;
                        break;
                    case 5:
                        Talk(SAY_AKAMA_BEWARE);
                        Channel->setDeathState(JUST_DIED);
                        Spirit[0]->SetVisible(false);
                        Spirit[1]->SetVisible(false);
                        Timer = 3000;
                        break;
                    case 6:
                        EnterPhase(PHASE_WALK);
                        instance->SetData(DATA_ILLIDAN_GATE_OPENED,3);
                        break;
                    default:
                        break;
                }
                ++ChannelCount;
            }

            void HandleWalkSequence()
            {
                switch (WalkCount)
                {
                    case 6:
                        for (uint8 i = 0; i < 2; ++i)
                            if (instance)
                                instance->HandleGameObject(DoorGUID[i], true);
                        break;
                    case 8:
                        if (Phase == PHASE_WALK)
                            EnterPhase(PHASE_TALK);
                        else
                            EnterPhase(PHASE_FIGHT_ILLIDAN);
                        break;
                    case 12:
                        EnterPhase(PHASE_FIGHT_MINIONS);
                        break;
                }

                if (Phase == PHASE_WALK)
                {
                    Timer = 0;
                    ++WalkCount;
                    me->GetMotionMaster()->MovePoint(WalkCount, AkamaWP[WalkCount].x, AkamaWP[WalkCount].y, AkamaWP[WalkCount].z);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->IsVisible())
                {
                    if (Check_Timer <= diff)
                    {
                        if (instance && instance->GetData(DATA_ILLIDARI_COUNCIL_EVENT) == DONE)
                            me->SetVisible(true);

                        Check_Timer = 5000;
                    } else Check_Timer -= diff;
                }

                if (me->IsVisible())
                {
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }

                Event = false;
                if (Timer)
                {
                    if (Timer <= diff)
                        Event = true;
                    else Timer -= diff;
                }

                if (Event)
                {
                    switch (Phase)
                    {
                        case PHASE_CHANNEL:
                            if (JustCreated)
                                HandleChannelSequence();
                            else{
                                EnterPhase(PHASE_WALK);
                            }
                            break;
                        case PHASE_TALK:
                            HandleTalkSequence();
                            break;
                        case PHASE_WALK:
                        case PHASE_RETURN:
                            HandleWalkSequence();
                            break;
                        case PHASE_FIGHT_ILLIDAN:
                            {
                                Creature* illidan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ILLIDAN_STORMRAGE));
                                if (illidan && illidan->HealthBelowPct(90))
                                    EnterPhase(PHASE_TALK);
                                else
                                {
                                    DoCast(me->GetVictim(), SPELL_CHAIN_LIGHTNING);
                                    Timer = 30000;
                                }
                            }
                            break;
                        case PHASE_FIGHT_MINIONS:
                            {
                                float x, y, z;
                                me->GetPosition(x, y, z);
                                Creature* elite = me->SummonCreature(ILLIDARI_ELITE, x+rand()%10, y+rand()%10, z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30000ms);
                                //Creature* elite = me->SummonCreature(ILLIDARI_ELITE, x, y, z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30000ms);
                                if (elite)
                                {
                                    elite->AI()->AttackStart(me);
                                    elite->AddThreat(me, 1000000.0f);
                                    AttackStart(elite);
                                    me->AddThreat(elite, 1000000.0f);
                                }
                                Timer = 10000 + rand() % 6000;
                                if (Creature* illidan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ILLIDAN_STORMRAGE)))
                                    if (illidan->HealthBelowPct(10))
                                        EnterPhase(PHASE_RETURN);
                            }
                            break;
                        default:
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                if (HealthBelowPct(20))
                    DoCast(me, SPELL_HEALING_POTION);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_akama_illidanAI(creature);
        }
};

void boss_illidan_stormrage::boss_illidan_stormrageAI::Reset()
{
    if (instance)
        instance->SetData(DATA_ILLIDAN_STORMRAGE_EVENT, NOT_STARTED);

    if (AkamaGUID)
    {
        if (Creature* akama = ObjectAccessor::GetCreature(*me, AkamaGUID))
        {
            if (!akama->IsAlive())
                akama->Respawn();
            else
            {
                if (CAST_AI(npc_akama_illidan::npc_akama_illidanAI, akama->AI()))
                {
                    CAST_AI(npc_akama_illidan::npc_akama_illidanAI, akama->AI())->EnterEvadeMode();
                    akama->GetMotionMaster()->MoveTargetedHome();
                    CAST_AI(npc_akama_illidan::npc_akama_illidanAI, akama->AI())->Reset();
                }
            }
        }
        AkamaGUID = ObjectGuid::Empty;
    }

    MaievGUID = ObjectGuid::Empty;
    for (uint8 i = 0; i < 2; ++i)
    {
        FlameGUID[i] = ObjectGuid::Empty;
        GlaiveGUID[i] = ObjectGuid::Empty;
    }

    Phase = PHASE_ILLIDAN_NULL;
    Event = EVENT_NULL;
    Timer[EVENT_BERSERK] = 1500000;

    HoverPoint = 0;
    TalkCount = 0;
    FlightCount = 0;
    TransformCount = 0;

    me->SetDisplayId(21135);
    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
    me->ResetInhabitTypeOverride();
    me->SetFlying(false);
    me->setActive(false);
    Summons.DespawnAll();
}

void boss_illidan_stormrage::boss_illidan_stormrageAI::JustSummoned(Creature* summon)
{
    Summons.Summon(summon);
    switch (summon->GetEntry())
    {
        case PARASITIC_SHADOWFIEND:
            {
                if (Phase == PHASE_TALK_SEQUENCE)
                {
                    summon->SetVisible(false);
                    summon->setDeathState(JUST_DIED);
                    return;
                }
                Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 999, true);
                if (!target || target->HasAura(SPELL_PARASITIC_SHADOWFIEND)
                    || target->HasAura(SPELL_PARASITIC_SHADOWFIEND2))
                    target = SelectTarget(SELECT_TARGET_RANDOM, 0, 999, true);
                if (target)
                    summon->AI()->AttackStart(target);
            }
            break;
        case SHADOW_DEMON:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 999, true)) // only on players.
            {
                summon->AddThreat(target, 5000000.0f);
                summon->AI()->AttackStart(target);
            }
            break;
        case MAIEV_SHADOWSONG:
            {
                summon->SetVisible(false); // Leave her invisible until she has to talk
                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                MaievGUID = summon->GetGUID();
                CAST_AI(boss_maiev_shadowsong::boss_maievAI, summon->AI())->GetIllidanGUID(me->GetGUID());
                summon->AI()->DoAction(PHASE_TALK_SEQUENCE);
            }
            break;
        case FLAME_OF_AZZINOTH:
                summon->AI()->AttackStart(summon->SelectNearestTarget(999));
            break;
        default:
            break;
    }
}

void boss_illidan_stormrage::boss_illidan_stormrageAI::HandleTalkSequence()
{
    switch (TalkCount)
    {
        case 0:
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            break;
        case 8:
            // Equip our warglaives!
            SetEquipmentSlots(false, EQUIP_ID_MAIN_HAND, EQUIP_ID_OFF_HAND, EQUIP_NO_CHANGE);
            me->SetSheath(SHEATH_STATE_MELEE);
            me->SetWalk(false);
            break;
        case 9:
            if (Creature* akama = ObjectAccessor::GetCreature(*me, AkamaGUID))
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                me->AddThreat(akama, 100.0f);
                CAST_AI(npc_akama_illidan::npc_akama_illidanAI, akama->AI())->EnterPhase(PHASE_FIGHT_ILLIDAN);
                EnterPhase(PHASE_NORMAL);
            }
            break;
        case 10:
            SummonMaiev();
            break;
        case 11:
            if (Creature* maiev = ObjectAccessor::GetCreature(*me, MaievGUID))
            {
                maiev->SetVisible(true); // Maiev is now visible
                maiev->CastSpell(maiev, SPELL_TELEPORT_VISUAL, true); // onoz she looks like she teleported!
                maiev->SetInFront(me); // Have her face us
                me->SetInFront(maiev); // Face her, so it's not rude =P
                maiev->GetMotionMaster()->MoveIdle();
                me->GetMotionMaster()->MoveIdle();
            }
            break;
        case 14:
            if (Creature* maiev = ObjectAccessor::GetCreature(*me, MaievGUID))
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                maiev->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                maiev->AddThreat(me, 10000000.0f); // Have Maiev add a lot of threat on us so that players don't pull her off if they damage her via AOE
                maiev->AI()->AttackStart(me); // Force Maiev to attack us.
                EnterPhase(PHASE_NORMAL_MAIEV);
            }
            break;
        case 15:
            DoCast(me, SPELL_DEATH); // Animate his kneeling + stun him
            Summons.DespawnAll();
            break;
        case 17:
            if (Creature* akama = ObjectAccessor::GetCreature(*me, AkamaGUID))
            {
                if (!me->IsWithinDistInMap(akama, 15))
                {
                    float x, y, z;
                    me->GetPosition(x, y, z);
                    x += 10; y += 10;
                    akama->GetMotionMaster()->Clear(false);
                    //akama->GetMotionMaster()->MoveIdle();
                    akama->NearTeleportTo(x, y, z, 0.0f);//Illidan must not die until Akama arrives.
                    akama->GetMotionMaster()->MoveChase(me);
                }
            }
            break;
        case 19: // Make Maiev leave
            if (Creature* maiev = ObjectAccessor::GetCreature(*me, MaievGUID))
            {
                maiev->CastSpell(maiev, SPELL_TELEPORT_VISUAL, true);
                maiev->setDeathState(JUST_DIED);
                me->SetStandState(UNIT_STAND_STATE_DEAD);
            }
            break;
        case 21: // Kill ourself.
            me->DealDamage(me, me->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
            break;
        default:
            break;
    }
    if (Phase == PHASE_TALK_SEQUENCE)
        TalkText(TalkCount); // This function does most of the talking
    ++TalkCount;
}

class npc_cage_trap_trigger : public CreatureScript
{
    public:
        npc_cage_trap_trigger() : CreatureScript("npc_cage_trap_trigger") { }

        struct cage_trap_triggerAI : public ScriptedAI
        {
            cage_trap_triggerAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid IllidanGUID;
            uint32 DespawnTimer;

            bool Active;
            bool SummonedBeams;

            void Reset() override
            {
                IllidanGUID = ObjectGuid::Empty;

                Active = false;
                SummonedBeams = false;

                DespawnTimer = 0;

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!Active)
                    return;

                if (who && (who->GetTypeId() != TYPEID_PLAYER))
                {
                    if (who->GetEntry() == ILLIDAN_STORMRAGE) // Check if who is Illidan
                    {
                        if (!IllidanGUID && me->IsWithinDistInMap(who, 3) && (!who->HasAura(SPELL_CAGED)))
                        {
                            IllidanGUID = who->GetGUID();
                            who->CastSpell(who, SPELL_CAGED, true);
                            DespawnTimer = 5000;
                            if (who->HasAura(SPELL_ENRAGE))
                                who->RemoveAurasDueToSpell(SPELL_ENRAGE); // Dispel his enrage
                            //if (GameObject* CageTrap = instance->instance->GetGameObject(instance->GetData64(CageTrapGUID)))
                            //    CageTrap->SetLootState(GO_JUST_DEACTIVATED);
                        }
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (DespawnTimer)
                {
                    if (DespawnTimer <= diff)
                        me->DealDamage(me, me->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
                    else DespawnTimer -= diff;
                }

                //if (IllidanGUID && !SummonedBeams)
                //{
                //    if (Unit* Illidan = Unit::GetUnit(*me, IllidanGUID)
                //    {
                //        //TODO: Find proper spells and properly apply 'caged' Illidan effect
                //    }
                //}
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new cage_trap_triggerAI(creature);
        }
};

class gameobject_cage_trap : public GameObjectScript
{
    public:
        gameobject_cage_trap() : GameObjectScript("gameobject_cage_trap") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            float x, y, z;
            player->GetPosition(x, y, z);

            // Grid search for nearest live Creature of entry 23304 within 10 yards
            if (Creature* pTrigger = go->FindNearestCreature(23304, 10.0f))
                CAST_AI(npc_cage_trap_trigger::cage_trap_triggerAI, pTrigger->AI())->Active = true;
            go->SetGoState(GO_STATE_ACTIVE);
            return true;
        }
};

class npc_shadow_demon : public CreatureScript
{
    public:
        npc_shadow_demon() : CreatureScript("npc_shadow_demon") { }

        struct shadow_demonAI : public ScriptedAI
        {
            shadow_demonAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid TargetGUID;

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoZoneInCombat();
            }

            void Reset() override
            {
                TargetGUID = ObjectGuid::Empty;
                DoCast(me, SPELL_SHADOW_DEMON_PASSIVE, true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Unit* target = Unit::GetUnit(*me, TargetGUID))
                    target->RemoveAurasDueToSpell(SPELL_PARALYZE);
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!UpdateVictim()) return;

                if (me->GetVictim()->GetTypeId() != TYPEID_PLAYER) return; // Only cast the below on players.

                if (!me->GetVictim()->HasAura(SPELL_PARALYZE))
                {
                    TargetGUID = me->GetVictim()->GetGUID();
                    me->AddThreat(me->GetVictim(), 10000000.0f);
                    DoCast(me->GetVictim(), SPELL_PURPLE_BEAM, true);
                    DoCast(me->GetVictim(), SPELL_PARALYZE, true);
                }
                // Kill our target if we're very close.
                if (me->IsWithinDistInMap(me->GetVictim(), 3))
                    DoCast(me->GetVictim(), SPELL_CONSUME_SOUL);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new shadow_demonAI(creature);
        }
};

class npc_blade_of_azzinoth : public CreatureScript
{
    public:
        npc_blade_of_azzinoth() : CreatureScript("npc_blade_of_azzinoth") { }

        struct blade_of_azzinothAI : public NullCreatureAI
        {
            blade_of_azzinothAI(Creature* creature) : NullCreatureAI(creature) { }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_THROW_GLAIVE2 || spell->Id == SPELL_THROW_GLAIVE)
                    me->SetDisplayId(21431); // appear when hit by Illidan's glaive
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new blade_of_azzinothAI(creature);
        }
};

class npc_parasitic_shadowfiend : public CreatureScript
{
    public:
        npc_parasitic_shadowfiend() : CreatureScript("npc_parasitic_shadowfiend") { }

        // Shadowfiends interact with Illidan, setting more targets in Illidan's hashmap
        struct npc_parasitic_shadowfiendAI : public ScriptedAI
        {
            npc_parasitic_shadowfiendAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            ObjectGuid IllidanGUID;
            uint32 CheckTimer;

            void Reset() override
            {
                if (instance)
                    IllidanGUID = instance->GetGuidData(DATA_ILLIDAN_STORMRAGE);
                else
                    IllidanGUID = ObjectGuid::Empty;

                CheckTimer = 5000;
                DoCast(me, SPELL_SHADOWFIEND_PASSIVE, true);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoZoneInCombat();
            }

            void DoMeleeAttackIfReady()
            {
                if (me->isAttackReady() && me->IsWithinMeleeRange(me->GetVictim()))
                {
                    if (!me->GetVictim()->HasAura(SPELL_PARASITIC_SHADOWFIEND)
                        && !me->GetVictim()->HasAura(SPELL_PARASITIC_SHADOWFIEND2))
                    {
                        if (Creature* illidan = Unit::GetCreature(*me,IllidanGUID))//summon only in 1. phase
                            if (CAST_AI(boss_illidan_stormrage::boss_illidan_stormrageAI, illidan->AI())->Phase == PHASE_NORMAL)
                                me->CastSpell(me->GetVictim(), SPELL_PARASITIC_SHADOWFIEND2, true, 0, 0, IllidanGUID); //do not stack
                    }
                    me->AttackerStateUpdate(me->GetVictim());
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->GetVictim())
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 999, true))
                        AttackStart(target);
                    else
                    {
                        me->SetVisible(false);
                        me->setDeathState(JUST_DIED);
                        return;
                    }
                }

                if (CheckTimer <= diff)
                {
                    Creature* illidan = ObjectAccessor::GetCreature(*me, IllidanGUID);
                    if (!illidan || illidan->IsInEvadeMode())
                    {
                        me->SetVisible(false);
                        me->setDeathState(JUST_DIED);
                        return;
                    } else CheckTimer = 5000;
                } else CheckTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_parasitic_shadowfiendAI(creature);
        }
};

void AddSC_boss_illidan()
{
    new boss_illidan_stormrage();
    new npc_akama_illidan();
    new boss_maiev_shadowsong();
    new npc_flame_of_azzinoth();
    new npc_blade_of_azzinoth();
    new gameobject_cage_trap();
    new npc_cage_trap_trigger();
    new npc_shadow_demon();
    new npc_parasitic_shadowfiend();
}
