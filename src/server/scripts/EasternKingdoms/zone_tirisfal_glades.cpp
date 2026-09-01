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
#include "Object.h"
#include "Player.h"
#include "Random.h"

enum TirisfalGlades
{
    NPC_ARADNE                              = 50372,
    NPC_RISEN_RECRUIT                       = 50414,
    NPC_DEATHKNELL_GRAVE_TARGET             = 50373,
    NPC_MINDLESS_ZOMBIE                     = 1501,

    SPELL_RAISE_UNDEAD                      = 93446,
    SPELL_SUMMON_RISEN_DEAD                 = 93447,
    SPELL_DIRT_EXPLOSION                    = 89199,
    SPELL_CHURNING_DIRT                     = 92788,
    SPELL_ENRAGE                            = 44427,
    SPELL_RISEN_DEAD_TRANSFORM              = 93460,

    ARADNE_MOVEMENT_POINT_01                = 4,
    ARADNE_MOVEMENT_POINT_02                = 12,
    ARADNE_MOVEMENT_POINT_03                = 15,
    ARADNE_MOVEMENT_POINT_04                = 20,
    ARADNE_MOVEMENT_POINT_05                = 26,
    ARADNE_MOVEMENT_POINT_06                = 31,
    ARADNE_MOVEMENT_POINT_07                = 33,
    ARADNE_MOVEMENT_POINT_08                = 37,

    ARADNE_PHASE_00                         = 0,
    ARADNE_PHASE_01                         = 1,
    ARADNE_PHASE_02                         = 2,
    ARADNE_PHASE_03                         = 3,
    ARADNE_PHASE_04                         = 4,

    GRAVE_TARGET_PHASE_00                   = 0,
    GRAVE_TARGET_PHASE_01                   = 1,
    GRAVE_TARGET_PHASE_02                   = 2,
    GRAVE_TARGET_PHASE_03                   = 3,
    GRAVE_TARGET_PHASE_04                   = 4,

    RISEN_DEAD_PHASE_00                     = 0,
    RISEN_DEAD_PHASE_01                     = 1,
    RISEN_DEAD_PHASE_02                     = 2,
    RISEN_DEAD_PHASE_03                     = 3,
    RISEN_DEAD_PHASE_04                     = 4,

    ARADNE_TEXT_01                          = 0,

    RISEN_DEAD_TEXT_01                      = 0,
    RISEN_DEAD_TEXT_02                      = 1,
    RISEN_DEAD_TEXT_03                      = 2,
    RISEN_DEAD_TEXT_04                      = 3,
    RISEN_DEAD_TEXT_05                      = 4,
    RISEN_DEAD_TEXT_06                      = 5,
    RISEN_DEAD_TEXT_07                      = 6,
    RISEN_DEAD_TEXT_08                      = 7,
    RISEN_DEAD_TEXT_09                      = 8,
    RISEN_DEAD_TEXT_10                      = 9,
    RISEN_DEAD_TEXT_11                      = 10,

    RISEN_DEAD_PATH_01                      = 50374*100,
    RISEN_DEAD_PATH_02                      = 50374*100+01,

    RISEN_DEAD_VISUAL_EQUIP_WEAPON_01       = 10614,
    RISEN_DEAD_VISUAL_EQUIP_WEAPON_02       = 1984,

    UNDERTAKER_MORDO_PHASE_00               = 0,
    UNDERTAKER_MORDO_PHASE_01               = 1,
    UNDERTAKER_MORDO_PHASE_02               = 2,
    UNDERTAKER_MORDO_PHASE_03               = 3,
    UNDERTAKER_MORDO_PHASE_04               = 4,
    UNDERTAKER_MORDO_PHASE_05               = 5,
    UNDERTAKER_MORDO_PHASE_06               = 6,
    UNDERTAKER_MORDO_PHASE_07               = 7,
    UNDERTAKER_MORDO_PHASE_08               = 8,
    UNDERTAKER_MORDO_PHASE_09               = 9,

    UNDERTAKER_MORDO_TEXT_01                = 0,
    UNDERTAKER_MORDO_TEXT_02                = 1,

    RISEN_RECRUIT_TEXT_01                   = 0,

    RISEN_RECRUIT_PATH_01                   = 50414*100,
    RISEN_RECRUIT_PATH_02                   = 50414*100+01,

    MINDLESS_ZOMBIE_ATTACK_EVENT_STEP_01    = 1,
    MINDLESS_ZOMBIE_ATTACK_EVENT_STEP_02    = 2,

    QUEST_THE_SHADOW_GRAVE                  = 28608,
    SPELL_SUMMON_DARNELL                    = 91576,

    AREA_THE_DEATHKNELL_GRAVES              = 5692,
    AREA_SHADOW_GRAVE                       = 2117,

    ITEM_THICK_EMBALMING_FLUID              = 64582,
    ITEM_CORPSE_STITCHING_TWINE             = 64581,

    DARNELL_TEXT_01                         = 0,
    DARNELL_TEXT_02                         = 1,
    DARNELL_TEXT_03                         = 2,
    DARNELL_TEXT_04                         = 3,
    DARNELL_TEXT_05                         = 4,
    DARNELL_TEXT_06                         = 5,
    DARNELL_TEXT_07                         = 6,
    DARNELL_TEXT_08                         = 7,
    DARNELL_TEXT_09                         = 8,
    DARNELL_TEXT_10                         = 9,

    DARNELL_MOVE_POINT_01                   = 1,
    DARNELL_MOVE_POINT_02                   = 2,
    DARNELL_MOVE_POINT_03                   = 3,
    DARNELL_MOVE_POINT_04                   = 4,
    DARNELL_MOVE_POINT_05                   = 5,
    DARNELL_MOVE_POINT_06                   = 6,
    DARNELL_MOVE_POINT_07                   = 7,
    DARNELL_MOVE_POINT_08                   = 8,

    DARNELL_PATH_00                         = 0,
    DARNELL_PATH_01                         = 1,
    DARNELL_PATH_02                         = 2,
    DARNELL_PATH_03                         = 3,
    DARNELL_PATH_04                         = 4,
    DARNELL_PATH_05                         = 5,
    DARNELL_PATH_06                         = 6,
    DARNELL_PATH_07                         = 7,
    DARNELL_PATH_08                         = 8,

    DARNELL_MOVE_EVENT_01                   = 1,
    DARNELL_MOVE_EVENT_02                   = 2,
    DARNELL_MOVE_EVENT_03                   = 3,
    DARNELL_MOVE_EVENT_04                   = 4,

    DARNELL_METHOD_00                        = 0,
    DARNELL_METHOD_01                        = 1,
    DARNELL_METHOD_02                        = 2,
    DARNELL_METHOD_03                        = 3,
    DARNELL_METHOD_04                        = 4,
    DARNELL_METHOD_05                        = 5,

    DARNELL_PHASE_00                        = 0,
    DARNELL_PHASE_01                        = 1,

    COUNTER_0                               = 0,
    COUNTER_10                              = 10,

    NPC_MARSHAL_REDPATH                     = 49230,
    NPC_LILIAN_VOSS                         = 38895,
    NPC_VALDRED_MORAY                       = 49231,

    MARSHAL_REDPATH_PATH                    = 49230*100,
    LILIAN_VOSS_PATH                        = 38895*100,
    VALDRED_MORAY_PATH                      = 49231*100,

    MARSHAL_REDPATH_TEXT                    = 0,
    LILIAN_VOSS_TEXT                        = 0,
    VALDRED_MORAY_TEXT                      = 0,

    QUEST_THE_WAKENING                      = 24960,

    GOSSIP_MENU_MARSHAL_REDPATH             = 12486,
    GOSSIP_MENU_LILIAN_VOSS                 = 12484,
    GOSSIP_MENU_VALDRED_MORAY               = 12489,

    MARSHAL_REDPATH_PHASE_00                = 0,
    MARSHAL_REDPATH_PHASE_01                = 1,
    MARSHAL_REDPATH_PHASE_02                = 2,
    MARSHAL_REDPATH_PHASE_03                = 3,
    MARSHAL_REDPATH_PHASE_04                = 4,

    LILIAN_VOSS_PHASE_00                    = 0,
    LILIAN_VOSS_PHASE_01                    = 1,
    LILIAN_VOSS_PHASE_02                    = 2,
    LILIAN_VOSS_PHASE_03                    = 3,
    LILIAN_VOSS_PHASE_04                    = 4,
    LILIAN_VOSS_PHASE_05                    = 5,

    VALDRED_MORAY_PHASE_00                  = 0,
    VALDRED_MORAY_PHASE_01                  = 1,
    VALDRED_MORAY_PHASE_02                  = 2,
    VALDRED_MORAY_PHASE_03                  = 3,
    VALDRED_MORAY_PHASE_04                  = 4,

    NPC_CAPTURED_PUDDLEJUMPER               = 38923,
    NPC_CAPTURED_ORACLE                     = 39078,
    NPC_PUDDLEJUMPER                        = 1543,
    NPC_MINOR_ORACLE                        = 1544,

    SPELL_MURLOC_LEACH                      = 73108,
    SPELL_SUMMON_PUDDLEJUMPER               = 73110,
    SPELL_SUMMON_ORACLE                     = 73434,
    SPELL_VILE_FIN_GUARDIAN_AURA            = 94266,
    SPELL_ROPE_BEAM                         = 84167,

    SPELL_LEAPING_RUSH                      = 75002,
    SPELL_LIGHTNING_BOLT                    = 9532,
    SPELL_LIGHTNING_SHIELD                  = 12550,

    NPC_CAPTURE_MURLOC_CREDIT               = 38923,
    NPC_DELIVERED_MURLOC_CREDIT             = 38887,

    QUEST_EVER_SO_LONELY                    = 24974,

    TEXT_CAPTURE_READY                      = 0,

    EVENT_CHECK_PLAYER                      = 999
};

struct npc_aradne : public ScriptedAI
{
    npc_aradne(Creature *c) : ScriptedAI(c) { }

    uint32 m_timer;
    uint32 m_phase;

    void Reset() override
    {
        m_timer = 0;
        m_phase = TirisfalGlades::ARADNE_PHASE_00;
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == MovementGeneratorType::WAYPOINT_MOTION_TYPE)
        {
            switch (id)
            {
                case TirisfalGlades::ARADNE_MOVEMENT_POINT_01:
                    m_phase = TirisfalGlades::ARADNE_PHASE_01;
                    m_timer = uint32(0.1) * TimeConstants::IN_MILLISECONDS;
                    break;
                case TirisfalGlades::ARADNE_MOVEMENT_POINT_02:
                    m_phase = TirisfalGlades::ARADNE_PHASE_01;
                    m_timer = uint32(0.1) * TimeConstants::IN_MILLISECONDS;
                    break;
                case TirisfalGlades::ARADNE_MOVEMENT_POINT_03:
                    m_phase = TirisfalGlades::ARADNE_PHASE_01;
                    m_timer = uint32(0.1) * TimeConstants::IN_MILLISECONDS;
                    break;
                case TirisfalGlades::ARADNE_MOVEMENT_POINT_04:
                    m_phase = TirisfalGlades::ARADNE_PHASE_01;
                    m_timer = uint32(0.1) * TimeConstants::IN_MILLISECONDS;
                    break;
                case TirisfalGlades::ARADNE_MOVEMENT_POINT_05:
                    m_phase = TirisfalGlades::ARADNE_PHASE_01;
                    m_timer = uint32(0.1) * TimeConstants::IN_MILLISECONDS;
                    break;
                case TirisfalGlades::ARADNE_MOVEMENT_POINT_06:
                    m_phase = TirisfalGlades::ARADNE_PHASE_01;
                    m_timer = uint32(0.1) * TimeConstants::IN_MILLISECONDS;
                    break;
                case TirisfalGlades::ARADNE_MOVEMENT_POINT_07:
                    m_phase = TirisfalGlades::ARADNE_PHASE_01;
                    m_timer = uint32(0.1) * TimeConstants::IN_MILLISECONDS;
                    break;
                case TirisfalGlades::ARADNE_MOVEMENT_POINT_08:
                    m_phase = TirisfalGlades::ARADNE_PHASE_01;
                    m_timer = uint32(0.1) * TimeConstants::IN_MILLISECONDS;
                    break;
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_timer <= diff)
        {
            m_timer = 1 * TimeConstants::IN_MILLISECONDS;
            StartAnimation();
        }
        else m_timer -= diff;
    }

    void StartAnimation()
    {
        switch (m_phase)
        {
            case TirisfalGlades::ARADNE_PHASE_01:
                if (Creature* npc = me->FindNearestCreature(TirisfalGlades::NPC_DEATHKNELL_GRAVE_TARGET, 15.0f))
                    me->SetFacingToObject(npc);

                m_timer = uint32(0.5) * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::ARADNE_PHASE_02;
                break;

            case TirisfalGlades::ARADNE_PHASE_02:
                if (Creature* npc = me->FindNearestCreature(TirisfalGlades::NPC_DEATHKNELL_GRAVE_TARGET, 15.0f))
                    me->CastSpell(npc, TirisfalGlades::SPELL_RAISE_UNDEAD, false);

                m_timer = 2 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::ARADNE_PHASE_03;
                break;
            case TirisfalGlades::ARADNE_PHASE_03:
                Talk(TirisfalGlades::ARADNE_TEXT_01);
                m_timer = uint32(0.5) * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::ARADNE_PHASE_04;
                break;
            default:
                m_timer = 0;
                m_phase = TirisfalGlades::ARADNE_PHASE_00;
                break;
        }
    }
};

struct npc_deathknell_grave_target : public ScriptedAI
{
    npc_deathknell_grave_target(Creature *c) : ScriptedAI(c) { }

    uint32 m_timer;
    uint32 m_phase;

    void Reset() override
    {
        m_timer = 0;
        m_phase = TirisfalGlades::GRAVE_TARGET_PHASE_00;
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (spell->Id == TirisfalGlades::SPELL_RAISE_UNDEAD)
        {
            m_timer = uint32(0.1) * TimeConstants::IN_MILLISECONDS;
            m_phase = TirisfalGlades::GRAVE_TARGET_PHASE_01;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_timer <= diff)
        {
            m_timer = 1 * TimeConstants::IN_MILLISECONDS;
            StartAnimation();
        }
        else m_timer -= diff;
    }

    void StartAnimation()
    {
        switch (m_phase)
        {
            case TirisfalGlades::GRAVE_TARGET_PHASE_01:
                DoCast(me, TirisfalGlades::SPELL_DIRT_EXPLOSION);
                DoCast(me, TirisfalGlades::SPELL_CHURNING_DIRT);
                m_phase = TirisfalGlades::GRAVE_TARGET_PHASE_02;
                break;
            case TirisfalGlades::GRAVE_TARGET_PHASE_02:
                me->CastSpell(me, SPELL_SUMMON_RISEN_DEAD, true);
                m_phase = TirisfalGlades::GRAVE_TARGET_PHASE_03;
                m_timer = 3 * TimeConstants::IN_MILLISECONDS;
                break;
            case TirisfalGlades::GRAVE_TARGET_PHASE_03:
                me->RemoveAllAuras();
                m_phase = TirisfalGlades::GRAVE_TARGET_PHASE_04;
                break;
            case TirisfalGlades::GRAVE_TARGET_PHASE_04:
                m_timer = 0;
                m_phase = TirisfalGlades::GRAVE_TARGET_PHASE_00;
                break;
        }
    }
};

struct npc_risen_dead : public ScriptedAI
{
    npc_risen_dead(Creature *c) : ScriptedAI(c) { }

    uint32 m_timer;
    uint32 m_phase;
    uint32 m_WishToBeDead;

    void Reset() override
    {
        m_timer = 2 * TimeConstants::IN_MILLISECONDS;
        m_phase = TirisfalGlades::RISEN_DEAD_PHASE_00;
        m_WishToBeDead = urand(0, 100);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_timer <= diff)
        {
            m_timer = 1 * TimeConstants::IN_MILLISECONDS;
            StartAnimation();
        }
        else m_timer -= diff;
    }

    void StartAnimation()
    {
        if (m_WishToBeDead < 33)
            ShowDeadScene();
        else if (m_WishToBeDead < 66)
            ShowLifeScene();
        else
            ShowRunningScene();
    }

    void ShowDeadScene()
    {
        switch (m_phase)
        {
            case TirisfalGlades::RISEN_DEAD_PHASE_00:
                me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_QUESTION);
                m_timer = uint32(0.2) * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::RISEN_DEAD_PHASE_01;
                break;
            case TirisfalGlades::RISEN_DEAD_PHASE_01:
                if (Creature* aradne = me->FindNearestCreature(TirisfalGlades::NPC_ARADNE, 20.0f))
                    Talk(TirisfalGlades::RISEN_DEAD_TEXT_08, aradne);

                m_timer = 5 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::RISEN_DEAD_PHASE_02;
                break;
            case TirisfalGlades::RISEN_DEAD_PHASE_02:
                Talk(TirisfalGlades::RISEN_DEAD_TEXT_03);
                m_timer = 5 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::RISEN_DEAD_PHASE_03;
                break;
            case TirisfalGlades::RISEN_DEAD_PHASE_03:
                Talk(TirisfalGlades::RISEN_DEAD_TEXT_02);
                Talk(TirisfalGlades::RISEN_DEAD_TEXT_09);
                me->Kill(me);
                m_timer = 5 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::RISEN_DEAD_PHASE_04;
                break;
            case TirisfalGlades::RISEN_DEAD_PHASE_04:
                m_timer = 0;
                me->DespawnOrUnsummon();
                break;
        }
    }

    void ShowRunningScene()
    {
        switch (m_phase)
        {
            case TirisfalGlades::RISEN_DEAD_PHASE_00:
                me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_QUESTION);
                m_timer = uint32(0.2) * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::RISEN_DEAD_PHASE_01;
                break;
            case TirisfalGlades::RISEN_DEAD_PHASE_01:
                Talk(TirisfalGlades::RISEN_DEAD_TEXT_01);
                m_timer = 5 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::RISEN_DEAD_PHASE_02;
                break;
            case TirisfalGlades::RISEN_DEAD_PHASE_02:
                Talk(TirisfalGlades::RISEN_DEAD_TEXT_04);
                m_timer = 5 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::RISEN_DEAD_PHASE_03;
                break;
            case TirisfalGlades::RISEN_DEAD_PHASE_03:
                Talk(TirisfalGlades::RISEN_DEAD_TEXT_07);
                Talk(TirisfalGlades::RISEN_DEAD_TEXT_10);
                me->UpdateEntry(NPC_MINDLESS_ZOMBIE);
                me->CastSpell(me, TirisfalGlades::SPELL_ENRAGE, true);
                me->GetMotionMaster()->MovePath(RISEN_DEAD_PATH_01, false);
                m_timer = 10 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::RISEN_DEAD_PHASE_04;
                break;
            case TirisfalGlades::RISEN_DEAD_PHASE_04:
                m_timer = 0;
                me->DespawnOrUnsummon();
                break;
        }
    }

    void ShowLifeScene()
    {
        switch (m_phase)
        {
            case TirisfalGlades::RISEN_DEAD_PHASE_00:
                me->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
                m_timer = uint32(0.2) * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::RISEN_DEAD_PHASE_01;
                break;
            case TirisfalGlades::RISEN_DEAD_PHASE_01:
                Talk(TirisfalGlades::RISEN_DEAD_TEXT_01);
                m_timer = 5 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::RISEN_DEAD_PHASE_02;
                break;
            case TirisfalGlades::RISEN_DEAD_PHASE_02:
                Talk(TirisfalGlades::RISEN_DEAD_TEXT_06);
                m_timer = 5 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::RISEN_DEAD_PHASE_03;
                break;
            case TirisfalGlades::RISEN_DEAD_PHASE_03:
                Talk(TirisfalGlades::RISEN_DEAD_TEXT_05);
                Talk(TirisfalGlades::RISEN_DEAD_TEXT_11);
                SetEquipmentSlots(false, RISEN_DEAD_VISUAL_EQUIP_WEAPON_01, RISEN_DEAD_VISUAL_EQUIP_WEAPON_02);
                me->GetMotionMaster()->MovePath(RISEN_DEAD_PATH_02, false);
                m_timer = 40 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::RISEN_DEAD_PHASE_04;
                break;
            case TirisfalGlades::RISEN_DEAD_PHASE_04:
                m_timer = 0;
                me->DespawnOrUnsummon();
                break;
        }
    }
};

struct npc_undertaker_mordo : public ScriptedAI
{
    npc_undertaker_mordo(Creature *c) : ScriptedAI(c) { }

    uint32 m_timer;
    uint32 m_phase;
    Creature* m_npc;

    void Reset() override
    {
        m_timer = urand(30, 45) * TimeConstants::IN_MILLISECONDS;
        m_phase = TirisfalGlades::UNDERTAKER_MORDO_PHASE_00;
        m_npc = nullptr;
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_timer <= diff)
        {
            m_timer = 1 * TimeConstants::IN_MILLISECONDS;
            StartAnimation();
        }
        else m_timer -= diff;
    }

    void StartAnimation()
    {
        switch (m_phase)
        {
            case TirisfalGlades::UNDERTAKER_MORDO_PHASE_00:
                if (Creature* npc_trigger = me->FindNearestCreature(TirisfalGlades::NPC_DEATHKNELL_GRAVE_TARGET, 20.0f))
                    if (m_npc = me->SummonCreature(TirisfalGlades::NPC_RISEN_RECRUIT, npc_trigger->GetPositionX(), npc_trigger->GetPositionY(), npc_trigger->GetPositionZ(), 1.0f, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, Milliseconds(46 * TimeConstants::IN_MILLISECONDS)))
                        m_npc->GetMotionMaster()->MovePath(TirisfalGlades::RISEN_RECRUIT_PATH_01, false);

                m_timer = 5 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::UNDERTAKER_MORDO_PHASE_01;
                break;
            case TirisfalGlades::UNDERTAKER_MORDO_PHASE_01:
                Talk(UNDERTAKER_MORDO_TEXT_01);
                //me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_TALK);
                m_timer = 3 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::UNDERTAKER_MORDO_PHASE_02;
                break;
            case TirisfalGlades::UNDERTAKER_MORDO_PHASE_02:
                //if (m_npc && m_npc->IsAlive())
                {
                    //m_npc->SetStandState(UnitStandStateType::UNIT_STAND_STATE_KNEEL);
                    //m_npc->SetFacingToObject(me);
                }
                m_timer = 1 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::UNDERTAKER_MORDO_PHASE_03;
                break;
            case TirisfalGlades::UNDERTAKER_MORDO_PHASE_03:
                //me->m_Events.Schedule(4 * TimeConstants::IN_MILLISECONDS, [this]() { me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, Emote::EMOTE_STATE_USE_STANDING); });
                //me->m_Events.Schedule(6 * TimeConstants::IN_MILLISECONDS, [this]() { me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, Emote::EMOTE_STATE_NONE); });
                m_timer = uint32(7.5) * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::UNDERTAKER_MORDO_PHASE_04;
                break;
            case TirisfalGlades::UNDERTAKER_MORDO_PHASE_04:
                //me->HandleEmoteCommand(Emote::EMOTE_ONESHOT_POINT);
                m_timer = uint32(3.5) * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::UNDERTAKER_MORDO_PHASE_05;
                break;
            case TirisfalGlades::UNDERTAKER_MORDO_PHASE_05:
                Talk(UNDERTAKER_MORDO_TEXT_02);
                //me->HandleEmoteCommandWithDelay(1 * TimeConstants::IN_MILLISECONDS, Emote::EMOTE_ONESHOT_TALK);
                m_timer = uint32(0.5) * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::UNDERTAKER_MORDO_PHASE_06;
                break;
            case TirisfalGlades::UNDERTAKER_MORDO_PHASE_06:
                //if (m_npc && m_npc->IsAlive())
                {
                    //m_npc->AddAura(TirisfalGlades::SPELL_RISEN_DEAD_TRANSFORM, m_npc);
                    //m_npc->CastSpell(m_npc, TirisfalGlades::SPELL_RISEN_DEAD_TRANSFORM);
                    //m_npc->SetDisplayId(36775);
                    //m_npc->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
                }
                m_timer = 2 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::UNDERTAKER_MORDO_PHASE_07;
                break;
            case TirisfalGlades::UNDERTAKER_MORDO_PHASE_07:
                //if (m_npc && m_npc->IsAlive())
                {
                    //m_npc->HandleEmoteCommand(Emote::EMOTE_ONESHOT_TALK);
                    //m_npc->AI()->Talk(RISEN_RECRUIT_TEXT_01);
                }
                m_timer = 3 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::UNDERTAKER_MORDO_PHASE_08;
                break;
            case TirisfalGlades::UNDERTAKER_MORDO_PHASE_08:
                //if (m_npc && m_npc->IsAlive())
                    //m_npc->HandleEmoteCommand(Emote::EMOTE_ONESHOT_SALUTE);
                m_timer = 2 * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::UNDERTAKER_MORDO_PHASE_09;
                break;
            case TirisfalGlades::UNDERTAKER_MORDO_PHASE_09:
                //if (m_npc && m_npc->IsAlive())
                    //m_npc->GetMotionMaster()->MovePath(TirisfalGlades::RISEN_RECRUIT_PATH_02, false);
                m_timer = urand(15, 45) * TimeConstants::IN_MILLISECONDS;
                m_phase = TirisfalGlades::UNDERTAKER_MORDO_PHASE_00;
                break;
        }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == TirisfalGlades::QUEST_THE_SHADOW_GRAVE)
            player->CastSpell(player, TirisfalGlades::SPELL_SUMMON_DARNELL);
    }
};

struct npc_mindless_zombie : public ScriptedAI
{
    npc_mindless_zombie(Creature *c) : ScriptedAI(c) { }

    EventMap events;
    bool _crazed;

    void Reset() override
    {
        me->RemoveAllAuras();
        _crazed = false;
        me->GetMotionMaster()->MoveRandom(15.f);

        if (!_crazed && roll_chance_i(20))
        {
            _crazed = true;
            events.ScheduleEvent(TirisfalGlades::MINDLESS_ZOMBIE_ATTACK_EVENT_STEP_01, urand(28, 57) * TimeConstants::IN_MILLISECONDS);
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        events.Update(diff);

        if (!me->IsInCombat())
        {
            if (Player* player = me->SelectNearestPlayer(15.0f))
            {
                if (!player->IsInCombat())
                {
                    while (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
                            case TirisfalGlades::MINDLESS_ZOMBIE_ATTACK_EVENT_STEP_01:
                            {
                                me->SetFacingToObject(player);
                                me->CastSpell(me, TirisfalGlades::SPELL_ENRAGE, true);
                                me->SetWalk(false);
                                me->GetMotionMaster()->MoveFollow(player, 1.0f, 0.0f);
                                events.ScheduleEvent(TirisfalGlades::MINDLESS_ZOMBIE_ATTACK_EVENT_STEP_02, urand(4, 6) * TimeConstants::IN_MILLISECONDS);
                                break;
                            }
                            case TirisfalGlades::MINDLESS_ZOMBIE_ATTACK_EVENT_STEP_02:
                            {
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveRandom(15.f);
                                me->RemoveAllAuras();
                                break;
                            }
                        }
                    }
                }
            }
        }

        DoMeleeAttackIfReady();
    }
};

enum class PlaceDescription : uint8
{
    Unknown                                 = 0,
    Outsite                                 = 1,
    Entrance                                = 2,
    Stairs_1                                = 3,
    Stairs_2                                = 4,
    Ground                                  = 5
};

struct npc_darnell : public ScriptedAI
{
    npc_darnell(Creature *c) : ScriptedAI(c) { }

    void Reset() override
    {
        m_timer = urand(30, 45) * TimeConstants::IN_MILLISECONDS;
        m_phase = TirisfalGlades::DARNELL_PHASE_00;
        m_method = TirisfalGlades::DARNELL_METHOD_00;

        if (Unit* npc = me->GetCharmerOrOwner())
        {
            m_player = npc->ToPlayer();
            if (m_player)
            {
                if (m_player->GetQuestStatus(TirisfalGlades::QUEST_THE_SHADOW_GRAVE) == QuestStatus::QUEST_STATUS_INCOMPLETE)
                {
                    m_method = TirisfalGlades::DARNELL_METHOD_01;
                    m_phase = TirisfalGlades::DARNELL_PHASE_01;
                    m_timer = 1 * TimeConstants::IN_MILLISECONDS;
                    m_counter = TirisfalGlades::COUNTER_0;
                    m_path = TirisfalGlades::DARNELL_PATH_00;
                    m_FoundGround = false;
                    m_ItemsFound = false;
                    m_OldPosition = m_player->GetPositionAlternate();
                    m_player_pos = m_player->GetPositionAlternate();
                    m_player_area = m_player->GetAreaId();
                    Talk(TirisfalGlades::DARNELL_TEXT_01, m_player);
                }
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (!CheckPlayerValid())
            return;

        if (type == MovementGeneratorType::POINT_MOTION_TYPE)
            m_arrived = true;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetGUID() != m_player->GetGUID())
            return;

        m_player_pos = who->GetPositionAlternate();
        m_player_area = who->GetAreaId();

        if (m_method == TirisfalGlades::DARNELL_METHOD_01 && me->GetMotionMaster()->GetCurrentMovementGeneratorType() != MovementGeneratorType::FOLLOW_MOTION_TYPE)
            if (m_arrived)
                if (me->GetDistance(m_player) < 4.0f)
                    me->GetMotionMaster()->MoveFollow(m_player, 0.0f, 0.0f);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!CheckPlayerValid())
            return;

        if (m_timer <= diff)
        {
            m_timer = 1 * TimeConstants::IN_MILLISECONDS;
            StartAnimation();
        }
        else m_timer -= diff;
    }

    void StartAnimation()
    {
        if (m_FoundGround)
        {
            SearchOnGround();
            return;
        }

        switch (GetPlaceDescription())
        {
            case PlaceDescription::Outsite:
                InviteToFollow();
                break;
            case PlaceDescription::Entrance:
                InviteToFollowDeeper1();
                break;
            case PlaceDescription::Stairs_1:
                InviteToFollowDeeper2();
                break;
            case PlaceDescription::Stairs_2:
                InviteToFollowToGround();
                break;
            case PlaceDescription::Ground:
                SearchOnGround();
                break;
        }
    }

    void InviteToFollow()
    {
        m_counter++;

        if (GetMovedPlayerDistance() > 1.0f)
            m_counter = TirisfalGlades::COUNTER_0;

        if (m_counter >= TirisfalGlades::COUNTER_10)
        {
            Talk(TirisfalGlades::DARNELL_TEXT_02, m_player);
            me->GetMotionMaster()->MovePoint(TirisfalGlades::DARNELL_MOVE_POINT_01, 1665.368896f, 1662.722656f, 141.850983f);
            m_path = TirisfalGlades::DARNELL_PATH_01;
            m_arrived = false;
            m_counter = TirisfalGlades::COUNTER_0;
        }
    }

    void InviteToFollowDeeper1()
    {
        m_counter++;

        if (GetMovedPlayerDistance() > 1.0f)
            m_counter = TirisfalGlades::COUNTER_0;

        if (m_counter >= TirisfalGlades::COUNTER_10)
        {
            Talk(TirisfalGlades::DARNELL_TEXT_03);
            me->GetMotionMaster()->MovePoint(TirisfalGlades::DARNELL_MOVE_POINT_02, 1642.761963f, 1662.547729f, 132.477753f);
            m_path = TirisfalGlades::DARNELL_PATH_02;
            m_arrived = false;
            m_counter = TirisfalGlades::COUNTER_0;
        }
    }

    void InviteToFollowDeeper2()
    {
        m_counter++;

        if (GetMovedPlayerDistance() > 1.0f)
            m_counter = TirisfalGlades::COUNTER_0;

        if (m_counter >= TirisfalGlades::COUNTER_10)
        {
            Talk(TirisfalGlades::DARNELL_TEXT_03);
            me->GetMotionMaster()->MovePoint(TirisfalGlades::DARNELL_MOVE_POINT_03, 1642.498779f, 1677.809937f, 126.932129f);
            m_path = TirisfalGlades::DARNELL_PATH_03;
            m_arrived = false;
            m_counter = TirisfalGlades::COUNTER_0;
        }
    }

    void InviteToFollowToGround()
    {
        m_counter++;

        if (GetMovedPlayerDistance() > 1.0f)
            m_counter = TirisfalGlades::COUNTER_0;

        if (m_counter >= TirisfalGlades::COUNTER_10)
        {
            Talk(TirisfalGlades::DARNELL_TEXT_03);
            me->GetMotionMaster()->MovePoint(TirisfalGlades::DARNELL_MOVE_POINT_04, 1656.714478f, 1678.538330f, 120.718788f);
            m_path = TirisfalGlades::DARNELL_PATH_04;
            m_arrived = false;
            m_counter = TirisfalGlades::COUNTER_0;
        }
    }

    void SearchOnGround()
    {
        if (CheckPlayerFoundItems())
        {
            if (m_ItemsFound == false)
            {
                m_ItemsFound = true;
                Talk(TirisfalGlades::DARNELL_TEXT_10);
                m_timer = 10 * TimeConstants::IN_MILLISECONDS;
                return;
            }
            else
                me->DespawnOrUnsummon();
                return;
        }

        switch (m_method)
        {
            case TirisfalGlades::DARNELL_METHOD_02:
                MoveToCenter();
                break;
            case TirisfalGlades::DARNELL_METHOD_03:
                MoveToRandomCorner();
                break;
            case TirisfalGlades::DARNELL_METHOD_04:
                SearchingOnCorner();
                break;
            case TirisfalGlades::DARNELL_METHOD_05:
                break;
            default:
                m_method = TirisfalGlades::DARNELL_METHOD_02;
                break;
        }
    }

    void MoveToCenter()
    {
        if (m_path != TirisfalGlades::DARNELL_PATH_08)
        {
            me->GetMotionMaster()->MovePoint(TirisfalGlades::DARNELL_MOVE_POINT_08, 1664.128052f, 1679.201294f, 120.530205f);
            m_arrived = false;
            m_path = TirisfalGlades::DARNELL_PATH_08;
        }
        else if (m_arrived == false) { }
        else
        {
            m_method = TirisfalGlades::DARNELL_METHOD_03;
        }
    }

    void MoveToRandomCorner()
    {
        if (m_path == TirisfalGlades::DARNELL_PATH_08)
        {
            switch (urand(TirisfalGlades::DARNELL_MOVE_EVENT_01, TirisfalGlades::DARNELL_MOVE_EVENT_04))
            {
                case TirisfalGlades::DARNELL_MOVE_EVENT_01:
                    me->GetMotionMaster()->MovePoint(TirisfalGlades::DARNELL_MOVE_POINT_04, 1663.849609f, 1694.495239f, 120.719284f);
                    m_arrived = false;
                    m_path = TirisfalGlades::DARNELL_PATH_04;
                    break;
                case TirisfalGlades::DARNELL_MOVE_EVENT_02:
                    me->GetMotionMaster()->MovePoint(TirisfalGlades::DARNELL_MOVE_POINT_05, 1672.939331f, 1668.029541f, 120.719307f);
                    m_arrived = false;
                    m_path = TirisfalGlades::DARNELL_PATH_05;
                    break;
                case TirisfalGlades::DARNELL_MOVE_EVENT_03:
                    me->GetMotionMaster()->MovePoint(TirisfalGlades::DARNELL_MOVE_POINT_06, 1656.963379f, 1667.456299f, 120.719093f);
                    m_arrived = false;
                    m_path = TirisfalGlades::DARNELL_PATH_06;
                    break;
                case TirisfalGlades::DARNELL_MOVE_EVENT_04:
                    me->GetMotionMaster()->MovePoint(TirisfalGlades::DARNELL_MOVE_POINT_07, 1656.098999f, 1688.312866f, 120.719093f);
                    m_arrived = false;
                    m_path = TirisfalGlades::DARNELL_PATH_07;
                    break;
            }
        }
        else if (m_arrived == false) { }
        else
        {
            m_method = TirisfalGlades::DARNELL_METHOD_04;
        }
    }

    void SearchingOnCorner()
    {
        Talk(urand(TirisfalGlades::DARNELL_TEXT_04, TirisfalGlades::DARNELL_TEXT_09), m_player);
        m_timer = 6 * TimeConstants::IN_MILLISECONDS;
        m_method = TirisfalGlades::DARNELL_METHOD_02;
    }

    bool CheckPlayerFoundItems()
    {
        if (m_player->HasItemCount(TirisfalGlades::ITEM_THICK_EMBALMING_FLUID) && m_player->HasItemCount(TirisfalGlades::ITEM_CORPSE_STITCHING_TWINE))
            return true;

        return false;
    }

    bool CheckPlayerValid()
    {
        if (!m_player->IsInWorld() ||
            m_player->isDead() ||
            m_player->GetQuestStatus(TirisfalGlades::QUEST_THE_SHADOW_GRAVE) != QuestStatus::QUEST_STATUS_INCOMPLETE ||
            (m_player->GetAreaId() != TirisfalGlades::AREA_THE_DEATHKNELL_GRAVES && m_player->GetAreaId() != TirisfalGlades::AREA_SHADOW_GRAVE))
        {
            me->DespawnOrUnsummon();
            return false;
        }

        return true;
    }

    PlaceDescription GetPlaceDescription()
    {
        switch (m_player_area)
        {
            case TirisfalGlades::AREA_THE_DEATHKNELL_GRAVES:
                return PlaceDescription::Outsite;
            case TirisfalGlades::AREA_SHADOW_GRAVE:
            {
                if (m_player_pos.GetPositionZ() < 123.0)
                {
                    m_FoundGround = true;
                    return PlaceDescription::Ground;
                }

                if (m_player_pos.GetPositionZ() < 127.0)
                    return PlaceDescription::Stairs_2;

                if (m_player_pos.GetPositionZ() < 133.0)
                    return PlaceDescription::Stairs_1;

                return PlaceDescription::Entrance;
            }
            default:
                return PlaceDescription::Unknown;
        }
    }

    float GetMovedPlayerDistance()
    {
        float dist = m_player->GetDistance(m_OldPosition);
        m_OldPosition = m_player->GetPositionAlternate();
        return dist;
    }

private:
    bool m_arrived;
    uint32 m_counter;
    Position m_OldPosition;
    Position m_player_pos;
    uint32 m_player_area;
    uint8 m_path;
    uint8 m_method;
    uint32 m_timer;
    uint32 m_phase;
    Player* m_player;
    bool m_FoundGround;
    bool m_ItemsFound;
};

class npc_marshal_redpath : public CreatureScript
{
public:
    npc_marshal_redpath() : CreatureScript("npc_marshal_redpath") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        uint32 id = player->PlayerTalkClass->GetGossipMenu().GetMenuId();

        if (player->GetQuestStatus(TirisfalGlades::QUEST_THE_WAKENING) == QUEST_STATUS_INCOMPLETE && id == TirisfalGlades::GOSSIP_MENU_MARSHAL_REDPATH)
        {
            player->PlayerTalkClass->SendCloseGossip();
            CAST_AI(npc_marshal_redpathAI, creature->AI())->StartAnimation(player);
            return false;
        }

        return false;
    }

    struct npc_marshal_redpathAI : public ScriptedAI
    {
        npc_marshal_redpathAI(Creature *c) : ScriptedAI(c) { }

        uint32 m_timer;
        uint32 m_phase;
        Player* m_player;

        void Reset() override
        {
            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
            m_timer = 0;
            m_phase = TirisfalGlades::MARSHAL_REDPATH_PHASE_00;
            m_player = NULL;
        }

        void StartAnimation(Player* player)
        {
            if (!m_phase)
            {
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                m_player = player;
                m_phase = TirisfalGlades::MARSHAL_REDPATH_PHASE_01;
                m_timer = 100;
            }
        }

        void UpdateAI(const uint32 diff) override
        {
            if (m_timer <= diff)
            {
                m_timer = 1 * IN_MILLISECONDS;
                StartVisual();
            }
            else m_timer -= diff;
        }

        void StartVisual()
        {
            switch (m_phase)
            {
                case TirisfalGlades::MARSHAL_REDPATH_PHASE_01:
                    me->SetFacingToObject(m_player);
                    Talk(MARSHAL_REDPATH_TEXT, m_player);
                    m_timer = 4 * IN_MILLISECONDS;
                    m_phase = TirisfalGlades::MARSHAL_REDPATH_PHASE_02;
                    break;
                case TirisfalGlades::MARSHAL_REDPATH_PHASE_02:
                    if (m_player)
                        m_player->KilledMonsterCredit(TirisfalGlades::NPC_MARSHAL_REDPATH);

                    m_timer = 4 * IN_MILLISECONDS;
                    m_phase = TirisfalGlades::MARSHAL_REDPATH_PHASE_03;
                    break;
                case TirisfalGlades::MARSHAL_REDPATH_PHASE_03:
                    me->GetMotionMaster()->MovePath(TirisfalGlades::MARSHAL_REDPATH_PATH, false);
                    m_timer = 10 * IN_MILLISECONDS;
                    m_phase = TirisfalGlades::MARSHAL_REDPATH_PHASE_04;
                    break;
                case TirisfalGlades::MARSHAL_REDPATH_PHASE_04:
                    me->DespawnOrUnsummon();
                    m_timer = 0;
                    m_phase = TirisfalGlades::MARSHAL_REDPATH_PHASE_00;
                    break;
            }
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_marshal_redpathAI(pCreature);
    }
};

class npc_lilian_voss : public CreatureScript
{
public:
    npc_lilian_voss() : CreatureScript("npc_lilian_voss") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        uint32 id = player->PlayerTalkClass->GetGossipMenu().GetMenuId();

        if (player->GetQuestStatus(TirisfalGlades::QUEST_THE_WAKENING) == QUEST_STATUS_INCOMPLETE && id == TirisfalGlades::GOSSIP_MENU_LILIAN_VOSS)
        {
            player->PlayerTalkClass->SendCloseGossip();
            CAST_AI(npc_lilian_vossAI, creature->AI())->StartAnimation(player);
            return false;
        }

        return false;
    }

    struct npc_lilian_vossAI : public ScriptedAI
    {
        npc_lilian_vossAI(Creature *c) : ScriptedAI(c) { }

        uint32 m_timer;
        uint32 m_phase;
        Player* m_player;

        void Reset() override
        {
            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
            m_timer = 0;
            m_phase = TirisfalGlades::LILIAN_VOSS_PHASE_00;
            m_player = NULL;
        }

        void StartAnimation(Player* player)
        {
            if (!m_phase)
            {
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                m_player = player;
                m_phase = TirisfalGlades::LILIAN_VOSS_PHASE_01;
                m_timer = 100;
            }
        }

        void UpdateAI(const uint32 diff) override
        {
            if (m_timer <= diff)
            {
                m_timer = 1 * IN_MILLISECONDS;
                StartVisual();
            }
            else m_timer -= diff;
        }

        void StartVisual()
        {
            switch (m_phase)
            {
                case TirisfalGlades::LILIAN_VOSS_PHASE_01:
                    me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, Emote::EMOTE_STATE_NONE);
                    m_timer = 500;
                    m_phase = TirisfalGlades::LILIAN_VOSS_PHASE_02;
                    break;
                case TirisfalGlades::LILIAN_VOSS_PHASE_02:
                    me->SetFacingToObject(m_player);
                    Talk(LILIAN_VOSS_TEXT);
                    m_timer = 4 * IN_MILLISECONDS;
                    m_phase = TirisfalGlades::LILIAN_VOSS_PHASE_03;
                    break;
                case TirisfalGlades::LILIAN_VOSS_PHASE_03:
                    if (m_player)
                        m_player->KilledMonsterCredit(TirisfalGlades::NPC_LILIAN_VOSS);

                    m_timer = 5 * IN_MILLISECONDS;
                    m_phase = TirisfalGlades::LILIAN_VOSS_PHASE_04;
                    break;
                case TirisfalGlades::LILIAN_VOSS_PHASE_04:
                    me->GetMotionMaster()->MovePath(TirisfalGlades::LILIAN_VOSS_PATH, false);
                    m_timer = 10 * IN_MILLISECONDS;
                    m_phase = TirisfalGlades::LILIAN_VOSS_PHASE_05;
                    break;
                case TirisfalGlades::LILIAN_VOSS_PHASE_05:
                    me->DespawnOrUnsummon();
                    m_timer = 0;
                    m_phase = TirisfalGlades::LILIAN_VOSS_PHASE_00;
                    break;
            }
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_lilian_vossAI(pCreature);
    }
};

class npc_valdred_moray : public CreatureScript
{
public:
    npc_valdred_moray() : CreatureScript("npc_valdred_moray") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        uint32 id = player->PlayerTalkClass->GetGossipMenu().GetMenuId();

        if (player->GetQuestStatus(TirisfalGlades::QUEST_THE_WAKENING) == QUEST_STATUS_INCOMPLETE && id == TirisfalGlades::GOSSIP_MENU_VALDRED_MORAY)
        {
            player->PlayerTalkClass->SendCloseGossip();
            CAST_AI(npc_valdred_morayAI, creature->AI())->StartAnimation(player);
            return false;
        }

        return false;
    }

    struct npc_valdred_morayAI : public ScriptedAI
    {
        npc_valdred_morayAI(Creature *c) : ScriptedAI(c) { }

        uint32 m_timer;
        uint32 m_phase;
        Player* m_player;

        void Reset() override
        {
            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
            m_timer = 0;
            m_phase = TirisfalGlades::VALDRED_MORAY_PHASE_00;
            m_player = NULL;
        }

        void StartAnimation(Player* player)
        {
            if (!m_phase)
            {
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                m_player = player;
                m_phase = TirisfalGlades::VALDRED_MORAY_PHASE_01;
                m_timer = 100;
            }
        }

        void UpdateAI(const uint32 diff) override
        {
            if (m_timer <= diff)
            {
                m_timer = 1 * IN_MILLISECONDS;
                StartVisual();
            }
            else m_timer -= diff;
        }

        void StartVisual()
        {
            switch (m_phase)
            {
                case TirisfalGlades::VALDRED_MORAY_PHASE_01:
                    me->SetFacingToObject(m_player);
                    Talk(VALDRED_MORAY_TEXT, m_player);
                    m_timer = 4 * IN_MILLISECONDS;
                    m_phase = TirisfalGlades::VALDRED_MORAY_PHASE_02;
                    break;
                case TirisfalGlades::VALDRED_MORAY_PHASE_02:
                    if (m_player)
                        m_player->KilledMonsterCredit(TirisfalGlades::NPC_VALDRED_MORAY);

                    m_timer = 4 * IN_MILLISECONDS;
                    m_phase = TirisfalGlades::VALDRED_MORAY_PHASE_03;
                    break;
                case TirisfalGlades::VALDRED_MORAY_PHASE_03:
                    me->GetMotionMaster()->MovePath(TirisfalGlades::VALDRED_MORAY_PATH, false);
                    m_timer = 6 * IN_MILLISECONDS;
                    m_phase = TirisfalGlades::VALDRED_MORAY_PHASE_04;
                    break;
                case TirisfalGlades::VALDRED_MORAY_PHASE_04:
                    me->DespawnOrUnsummon();
                    m_timer = 0;
                    m_phase = TirisfalGlades::VALDRED_MORAY_PHASE_00;
                    break;
            }
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_valdred_morayAI(pCreature);
    }
};

struct npc_vile_fin_puddlejumper : public ScriptedAI
{
    npc_vile_fin_puddlejumper(Creature* creature) : ScriptedAI(creature) { }

    ObjectGuid m_playerGUID;

    void Reset() override
    {
        uint64 m_playerGUID = 0;
    }

    void JustEngagedWith(Unit* who) override
    {
        me->CastSpell(who, TirisfalGlades::SPELL_LEAPING_RUSH);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    { 
        if (Player* player = attacker->ToPlayer())
        {
            if (me->GetHealthPct() <= 15.0f)
            {
                Talk(TirisfalGlades::TEXT_CAPTURE_READY);
                me->GetMotionMaster()->MoveIdle();
                me->GetMotionMaster()->MoveFleeing(player);
            }
        }
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (caster->ToPlayer())
        {
            if (spell->Id == TirisfalGlades::SPELL_MURLOC_LEACH)
            {
                me->CastSpell(caster, TirisfalGlades::SPELL_SUMMON_PUDDLEJUMPER);
                me->DespawnOrUnsummon(500);
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (UpdateVictim())
            DoMeleeAttackIfReady();
    }
};

struct npc_vile_fin_minor_oracle : public ScriptedAI
{
    npc_vile_fin_minor_oracle(Creature* creature) : ScriptedAI(creature) { }

    ObjectGuid m_playerGUID;
    uint32 m_lightning_bolt_timer;
    uint32 m_lightning_shield_timer;

    void Reset() override
    {
        uint64 m_playerGUID = 0;
        m_lightning_bolt_timer = urand(uint32(3.4), uint32(4.7)) * IN_MILLISECONDS;
        me->CastSpell(me, TirisfalGlades::SPELL_LIGHTNING_SHIELD);
        m_lightning_shield_timer = 10 * MINUTE * IN_MILLISECONDS;
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (Player* player = attacker->ToPlayer())
        {
            if (me->GetHealthPct() <= 15.0f)
            {
                Talk(TirisfalGlades::TEXT_CAPTURE_READY);
                me->GetMotionMaster()->MoveIdle();
                me->GetMotionMaster()->MoveFleeing(player);
            }
        }
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (caster->ToPlayer())
        {
            if (spell->Id == TirisfalGlades::SPELL_MURLOC_LEACH)
            {
                me->CastSpell(caster, TirisfalGlades::SPELL_SUMMON_ORACLE);
                me->DespawnOrUnsummon(500);
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_lightning_shield_timer <= diff)
        {
            me->CastSpell(me, TirisfalGlades::SPELL_LIGHTNING_SHIELD);
            m_lightning_shield_timer = 10 * MINUTE * IN_MILLISECONDS;
        }
        else
            m_lightning_shield_timer -= diff;

        if (UpdateVictim())
        {
            if (m_lightning_bolt_timer <= diff)
            {
                DoCastVictim(TirisfalGlades::SPELL_LIGHTNING_BOLT);
                m_lightning_bolt_timer = urand(uint32(3.4), uint32(4.7)) * IN_MILLISECONDS;
            }
            else
                m_lightning_bolt_timer -= diff;

            DoMeleeAttackIfReady();
        }
    }
};

class spell_murloc_leash : public SpellScript
{
    PrepareSpellScript(spell_murloc_leash);

    SpellCastResult CheckRequirement()
    {
        std::list<uint32>targets;
        targets.push_back(TirisfalGlades::NPC_CAPTURED_PUDDLEJUMPER);
        targets.push_back(TirisfalGlades::NPC_CAPTURED_ORACLE);

        if (Player* player = GetCaster()->ToPlayer())
        {
            std::list<Creature*>npcs = player->FindNearestCreatures(targets, 10.0f);

            if (npcs.size() > 0)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
        }

        if (Unit* unit = GetExplTargetUnit())
        {
            if (unit->GetEntry() == TirisfalGlades::NPC_PUDDLEJUMPER || unit->GetEntry() == TirisfalGlades::NPC_MINOR_ORACLE)
            {
                if (unit->GetHealthPct() < 80.0f)
                    return SPELL_CAST_OK;
            }
            else
                return SPELL_FAILED_BAD_TARGETS;
        }

        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
    }

    void CheckTarget(WorldObject*& target)
    {
        if (Creature* npc = target->ToCreature())
            if (npc->GetEntry() == TirisfalGlades::NPC_PUDDLEJUMPER || npc->GetEntry() == TirisfalGlades::NPC_MINOR_ORACLE)
                if (npc->GetHealthPct() < 80.0f)
                    return;

        target = NULL;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_murloc_leash::CheckRequirement);
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_murloc_leash::CheckTarget, EFFECT_0, TARGET_UNIT_TARGET_ANY);
    }
};

struct npc_captured_vile_fin_puddlejumper : public ScriptedAI
{
    npc_captured_vile_fin_puddlejumper(Creature* creature) : ScriptedAI(creature) { }

    EventMap m_events;
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        m_events.Reset();
        m_events.ScheduleEvent(TirisfalGlades::EVENT_CHECK_PLAYER, 1 * IN_MILLISECONDS);
        uint64 m_playerGUID = 0;
    }

    void IsSummonedBy(Unit* summoner) override
    { 
        if (Player* player = summoner->ToPlayer())
        {
            m_playerGUID = player->GetGUID();
            player->KilledMonsterCredit(TirisfalGlades::NPC_CAPTURE_MURLOC_CREDIT);
            me->GetMotionMaster()->MoveFollow(player, 0.0f, 0.0f);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            player->CastSpell(me, TirisfalGlades::SPELL_ROPE_BEAM);
        }
    }
};

struct npc_captured_vile_fin_minor_oracle : public ScriptedAI
{
    npc_captured_vile_fin_minor_oracle(Creature* creature) : ScriptedAI(creature) { }

    EventMap m_events;
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        m_events.Reset();
        m_events.ScheduleEvent(TirisfalGlades::EVENT_CHECK_PLAYER, 1 * IN_MILLISECONDS);
        uint64 m_playerGUID = 0;
    }

    void IsSummonedBy(Unit* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            m_playerGUID = player->GetGUID();
            player->KilledMonsterCredit(TirisfalGlades::NPC_CAPTURE_MURLOC_CREDIT);
            me->GetMotionMaster()->MoveFollow(player, 0.0f, 0.0f);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            player->CastSpell(me, TirisfalGlades::SPELL_ROPE_BEAM);
        }
    }
};

struct npc_sedrick_calston : public ScriptedAI
{
    npc_sedrick_calston(Creature* creature) : ScriptedAI(creature) { }

    EventMap m_events;

    void Reset() override
    {
        m_events.Reset();
        m_events.ScheduleEvent(TirisfalGlades::EVENT_CHECK_PLAYER, 1 * IN_MILLISECONDS);
    }

    void UpdateAI(const uint32 diff) override
    {
        m_events.Update(diff);

        while (uint32 eventId = m_events.ExecuteEvent())
        {
            switch (eventId)
            {
                case TirisfalGlades::EVENT_CHECK_PLAYER:
                {
                    if (Player* player = me->FindNearestPlayer(7.0f))
                    {
                        if (player->GetQuestStatus(TirisfalGlades::QUEST_EVER_SO_LONELY) == QUEST_STATUS_INCOMPLETE)
                        {
                            std::list<uint32>targets;
                            targets.push_back(TirisfalGlades::NPC_CAPTURED_PUDDLEJUMPER);
                            targets.push_back(TirisfalGlades::NPC_CAPTURED_ORACLE);
                            std::list<Creature*> npcs = me->FindNearestCreatures(targets, 10.0f);

                            if (npcs.size() > 0)
                            {
                                player->RemoveAura(TirisfalGlades::SPELL_VILE_FIN_GUARDIAN_AURA);
                                player->KilledMonsterCredit(TirisfalGlades::NPC_DELIVERED_MURLOC_CREDIT);

                                for (std::list<Creature*>::iterator itr = npcs.begin(); itr != npcs.end(); ++itr)
                                    (*itr)->DespawnOrUnsummon(1);
                            }
                        }
                    }

                    m_events.ScheduleEvent(TirisfalGlades::EVENT_CHECK_PLAYER, 1 * IN_MILLISECONDS);
                    break;
                }
            }
        }

        if (!UpdateVictim())
            return;
        else
            DoMeleeAttackIfReady();
    }
};

void AddSC_tirisfal_glades()
{
    new creature_script<npc_aradne>("npc_aradne");
    new creature_script<npc_deathknell_grave_target>("npc_deathknell_grave_target");
    new creature_script<npc_risen_dead>("npc_risen_dead");
    new creature_script<npc_undertaker_mordo>("npc_undertaker_mordo");
    new creature_script<npc_mindless_zombie>("npc_mindless_zombie");
    new creature_script<npc_darnell>("npc_darnell");
    new npc_marshal_redpath();
    new npc_lilian_voss();
    new npc_valdred_moray();
    new creature_script<npc_vile_fin_puddlejumper>("npc_vile_fin_puddlejumper");
    new creature_script<npc_vile_fin_minor_oracle>("npc_vile_fin_minor_oracle");
    new spell_script<spell_murloc_leash>("spell_murloc_leash");
    new creature_script<npc_captured_vile_fin_puddlejumper>("npc_captured_vile_fin_puddlejumper");
    new creature_script<npc_captured_vile_fin_minor_oracle>("npc_captured_vile_fin_minor_oracle");
    new creature_script<npc_sedrick_calston>("npc_sedrick_calston");
}
