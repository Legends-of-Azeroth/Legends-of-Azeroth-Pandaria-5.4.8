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
#include "../AI/SmartScripts/SmartAI.h"
#include "Random.h"

enum ElwynnForest
{
    SPELL_FIRE_EXTINGUISHER_AURA            = 80209,

    /// Infantries vs. Wolfs
    // Texts
    INFANTRY_HELP_YELL                      = 0,
    INFANTRY_COMBAT_YELL                    = 1,
    // Creatures
    NPC_BLACKROCK_BATTLE_WORG               = 49871,
    NPC_STORMWIND_INFANTRY                  = 49869,
    // Spells
    WORG_GROWL                              = 2649,
    // Misc
    WORG_FIGHTING_FACTION                   = 232,
    WORG_FACTION_RESTORE                    = 32,
    AI_HEALTH_MIN                           = 85,
    INFANTRY_YELL_CHANCE                    = 10,

    /// Brother Paxton
    // Spells
    SPELL_FORTITUDE                         = 13864,
    SPELL_PENANCE                           = 66097,
    SPELL_FLASH_HEAL                        = 38588,
    SPELL_RENEW                             = 8362,
    SPELL_REVIVE                            = 93799,
    // Texts
    BROTHER_PAXTON_TEXT                     = 0,
    BROTHER_PAXTON_TEXT_PLAYER              = 1,
    // Events
    BROTHER_PAXTON_HEAL_EVENT_01            = 0,
    BROTHER_PAXTON_HEAL_EVENT_02            = 1,
    BROTHER_PAXTON_HEAL_EVENT_03            = 2,
    BROTHER_PAXTON_HEAL_EVENT_04            = 3,
    HEAL_EVENT_01_STEP_01                   = 1,
    HEAL_EVENT_01_STEP_02                   = 2,
    HEAL_EVENT_01_STEP_03                   = 3,
    HEAL_EVENT_01_STEP_04                   = 4,
    HEAL_EVENT_02_STEP_01                   = 5,
    HEAL_EVENT_02_STEP_02                   = 6,
    HEAL_EVENT_02_STEP_03                   = 7,
    HEAL_EVENT_02_STEP_04                   = 8,
    HEAL_EVENT_03_STEP_01                   = 9,
    HEAL_EVENT_03_STEP_02                   = 10,
    HEAL_EVENT_03_STEP_03                   = 11,
    HEAL_EVENT_03_STEP_04                   = 12,
    HEAL_EVENT_04_STEP_01                   = 13,
    HEAL_EVENT_04_STEP_02                   = 14,
    HEAL_EVENT_04_STEP_03                   = 15,
    HEAL_EVENT_04_STEP_04                   = 16,
    HEAL_EVENT_RESET                        = 17,

    /// Blackrock Spy
    // Texts
    TEXT_BLACKROCK_SPY_COMBAT               = 0,

    /// Blackrock Invader
    // Texts
    TEXT_BLACKROCK_INVADER_COMBAT           = 0,

    /// Goblin Assassin
    // Texts
    ASSASSIN_SAY_COMBAT                     = 0,

    /// Hogger + End Event
    // Texts
    SAY_AGGRO                               = 0,
    SAY_HELP                                = 1,
    SAY_GRR                                 = 2,
    SAY_NO                                  = 3,
    SAY_EATING                              = 4,
    SAY_STUNNED                             = 5,
    SAY_NO_HURT                             = 6,
    // Spells
    SPELL_TELEPORT_VISUAL_ONLY              = 64446,
    SPELL_VICIOUS_SLICE                     = 87337,
    SPELL_EATING                            = 87351,
    SPELL_UPSET_STOMACH                     = 87352,
    SPELL_SUMMON_MINIONS                    = 87366,
    SPELL_BLOODY_STRIKE                     = 87359,
    // Creatures
    NPC_HOGGER                              = 448,
    NPC_DUMAS                               = 46940,
    NPC_ANDROMATH                           = 46941,
    NPC_JONATHAN                            = 46942,
    NPC_RAGAMUFFIN                          = 46943,
    NPC_TRIGGER_MEAT                        = 45979,
    // Phases
    PHASE_EATING                            = 1,
    PHASE_EATING_ENDED                      = 2,
    PHASE_FINAL                             = 3,
    // Events
    EVENT_CAST_VICIOUS_SLICE                = 1,
    EVENT_HALF_HP_ONCE                      = 2,
    EVENT_CAST_BLOODY_STRIKE                = 3,
    EVENT_BACK_TO_FIGHT                     = 4,
    EVENT_SUMMON_GUARDS                     = 5,
    EVENT_SUMMON_KIDS                       = 6,
    EVENT_SAY_GRR                           = 7,
    EVENT_SAY_NO                            = 8,
    // Hogger Minion Spells
    SPELL_ADVENTURERS_RUSH                  = 87402,

    /// Alliance Way
    // Texts
    SAY_INTRO                               = 0,
    SAY_SPECIAL_1                           = 1,
    SAY_SPECIAL_2                           = 2,
    SAY_SPECIAL_3                           = 3,
    SAY_SPECIAL_4                           = 4,
    SAY_SPECIAL_5                           = 5,
    SAY_SPECIAL_6                           = 6,
    SAY_SPECIAL_7                           = 7,
    SAY_SPECIAL_8                           = 8,
    SAY_SPECIAL_9                           = 9,
    SAY_SPECIAL_10                          = 10,
    SAY_SPECIAL_11                          = 11,
    SAY_SPECIAL_12                          = 12,
    SAY_SPECIAL_13                          = 13,
    SAY_SPECIAL_14                          = 14,
    SAY_SPECIAL_15                          = 15,
    SAY_SPECIAL_16                          = 16,
    SAY_SPECIAL_17                          = 17,
    SAY_SPECIAL_18                          = 18,
    SAY_SPECIAL_19                          = 19,
    // Quests
    QUEST_ALLIANCE_WAY                      = 30988,
    QUEST_AN_OLD_PIT_FIGHTER                = 30989,
    // Creatures
    NPC_KING_VARIAN_WRYNN                   = 29611,
    NPC_VARIAN_WRYNN_ALLIANCE_WAY           = 61796,
    NPC_AYISA_ALLIANCE_WAY                  = 61792,
    NPC_JO_JO_ALLIANCE_WAY                  = 61793,
    NPC_WALK_WITH_VARIAN_CREDIT             = 61798,
    NPC_FIGHT_WITH_VARIAN_CREDIT            = 61824,
    // Spells
    SPELL_SUMMON_VARIAN                     = 120352,
    SPELL_SUMMON_AYSA_AND_JOJO              = 120344,
    SPELL_SPYING                            = 92857,
    SPELL_SNEAKING                          = 93046,
    SPELL_SPYGLASS                          = 80676,
    SPELL_VARIAN_GET_PUNCHED_SCENE          = 120568, // SPELL_EFFECT_186 not implemented in core
    // Actions
    ACTION_AN_OLD_PIT_FIGHTER               = 1
};

const Position AysaJojoStandPos[2]
{
    { -8331.30f, 290.36f, 156.83f, 5.99f }, // Ayisa
    { -8313.47f, 277.04f, 156.83f, 2.26f },
};

const Position VarianPath[9]
{
    { -8363.68f, 241.96f, 155.31f, 1.50f },
    { -8358.11f, 258.80f, 155.34f, 1.25f },
    { -8343.70f, 269.41f, 155.34f, 0.63f },
    { -8325.33f, 246.29f, 155.34f, 5.39f },
    { -8277.68f, 283.86f, 155.34f, 0.67f },
    { -8315.63f, 331.37f, 155.34f, 2.25f },
    { -8339.31f, 312.58f, 155.34f, 3.76f },
    { -8334.64f, 307.08f, 157.34f, 5.34f },
    { -8323.51f, 293.86f, 156.83f, 5.34f },
};

const Position VarianHomePath[3]
{
    { -8338.44f, 274.53f, 157.34f, 3.78f },
    { -8362.87f, 254.89f, 155.34f, 3.78f },
    { -8363.05f, 232.23f, 157.07f, 2.23f },
};

struct npc_stormwind_infantry : public ScriptedAI
{
    npc_stormwind_infantry(Creature* creature) : ScriptedAI(creature)
    {
        me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, Emote::EMOTE_STATE_READY1H);
    }

    uint32 tSeek, cYell,tYell;

    void Reset() override
    {
        me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, Emote::EMOTE_STATE_READY1H);
        tSeek = urand(1 * TimeConstants::IN_MILLISECONDS, 2 * TimeConstants::IN_MILLISECONDS);
        cYell = urand(0, 100);
        tYell = urand(5 * TimeConstants::IN_MILLISECONDS, 60 * TimeConstants::IN_MILLISECONDS);
    }

    void DamageTaken(Unit* who, uint32& damage) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            me->GetThreatManager().resetAllAggro();
            who->AddThreat(me, 1.0f);
            me->AddThreat(who, 1.0f);
            me->AI()->AttackStart(who);
        }
        else if (who->IsPet())
        {
            me->GetThreatManager().resetAllAggro();
            me->AddThreat(who, 1.0f);
            me->AI()->AttackStart(who);
        }

        if (who->GetEntry() == ElwynnForest::NPC_BLACKROCK_BATTLE_WORG && me->HealthBelowPct(ElwynnForest::AI_HEALTH_MIN))
        {
            damage = 0;
            me->AddThreat(who, 1.0f);
            me->AI()->AttackStart(who);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;
        else
        {
            if (tYell <= diff)
            {
                if (cYell < ElwynnForest::INFANTRY_YELL_CHANCE)
                {
                    Talk(ElwynnForest::INFANTRY_COMBAT_YELL);
                    tYell = urand(10 * TimeConstants::IN_MILLISECONDS, 120 * TimeConstants::IN_MILLISECONDS);
                }
                else
                    tYell = urand(10 * TimeConstants::IN_MILLISECONDS, 120 * TimeConstants::IN_MILLISECONDS);
            }
            else
            {
                tYell -= diff;
                DoMeleeAttackIfReady();
            }
        }
    }
};

struct npc_blackrock_battle_worg : public ScriptedAI
{
    npc_blackrock_battle_worg(Creature* creature) : ScriptedAI(creature) { }

    uint32 tSeek, tGrowl;

    void Reset() override
    {
        tSeek = urand(1 * TimeConstants::IN_MILLISECONDS, 2 * TimeConstants::IN_MILLISECONDS);
        tGrowl = urand(8 * TimeConstants::IN_MILLISECONDS + 500, 10 * TimeConstants::IN_MILLISECONDS);
        me->SetFaction(ElwynnForest::WORG_FACTION_RESTORE);
    }

    void DamageTaken(Unit* who, uint32& damage) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            me->GetThreatManager().resetAllAggro();
            who->AddThreat(me, 1.0f);
            me->AddThreat(who, 1.0f);
            me->AI()->AttackStart(who);
        }
        else if (who->IsPet())
        {
            me->GetThreatManager().resetAllAggro();
            me->AddThreat(who, 1.0f);
            me->AI()->AttackStart(who);
        }

        if (who->GetEntry() == ElwynnForest::NPC_STORMWIND_INFANTRY && me->HealthBelowPct(ElwynnForest::AI_HEALTH_MIN))
        {
            damage = 0;
            me->AddThreat(who, 1.0f);
            me->AI()->AttackStart(who);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (tSeek <= diff)
        {
            if ((me->IsAlive()) && (!me->IsInCombat() && (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) <= 1.0f)))
                if (Creature* enemy = me->FindNearestCreature(ElwynnForest::NPC_STORMWIND_INFANTRY, 1.0f, true))
                {
                    me->SetFaction(ElwynnForest::WORG_FIGHTING_FACTION);
                    me->AI()->AttackStart(enemy);
                    tSeek = urand(1 * TimeConstants::IN_MILLISECONDS, 2 * TimeConstants::IN_MILLISECONDS);
                }
        }
        else
            tSeek -= diff;

        if (UpdateVictim())
        {
            if (tGrowl <=diff)
            {
                DoCast(me->GetVictim(), ElwynnForest::WORG_GROWL);
                tGrowl = urand(8 * TimeConstants::IN_MILLISECONDS + 500, 10 * TimeConstants::IN_MILLISECONDS);
            }
            else
            {
               tGrowl -= diff;
               DoMeleeAttackIfReady();
            }
        }
        else
        {
            me->SetFaction(ElwynnForest::WORG_FACTION_RESTORE);
            return;
        }
    }
};

struct npc_brother_paxton : public ScriptedAI
{
    npc_brother_paxton(Creature *c) : ScriptedAI(c) { }

    EventMap _events;

    uint32 _cooldownTimer;

    bool _cooldown;

    void Reset() override
    {
        _events.Reset();

        me->SetFlag(UNIT_FIELD_FLAGS, UnitFlags::UNIT_FLAG_IMMUNE_TO_PC | UnitFlags::UNIT_FLAG_IMMUNE_TO_NPC);
        DoCast(me, ElwynnForest::SPELL_FORTITUDE);
        me->InitializeReactState();

        _cooldown = false;
        _cooldownTimer = 0;
    }

    void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
    {
        return;
    }

    void MoveInLineOfSight(Unit* p_Who) override
    {
        if (me->GetDistance(p_Who) < 15.0f)
        {
            if (p_Who && p_Who->GetTypeId() == TYPEID_PLAYER && !p_Who->HasAura(ElwynnForest::SPELL_FORTITUDE) && !_cooldown)
            {
               if (roll_chance_i(30))
               {
                    _cooldown = true;
                    me->CastSpell(p_Who, ElwynnForest::SPELL_FORTITUDE);
                    me->CastSpell(p_Who, ElwynnForest::SPELL_RENEW, true);
                    Talk(ElwynnForest::BROTHER_PAXTON_TEXT_PLAYER, p_Who);
                }
            }
        }
    }

    void JustEngagedWith(Unit* /*p_Who*/) override
    {
        return;
    }

    void MovementInform(uint32 p_Type, uint32 p_ID) override
    {
        if (p_Type == 2 && p_ID == 5 || p_ID == 14 || p_ID == 22 || p_ID == 33 || p_ID == 40 || p_ID == 51 || p_ID == 60 || p_ID == 62)
        {
            switch(urand(ElwynnForest::BROTHER_PAXTON_HEAL_EVENT_01, ElwynnForest::BROTHER_PAXTON_HEAL_EVENT_04))
            {
                case ElwynnForest::BROTHER_PAXTON_HEAL_EVENT_01:
                    _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_01_STEP_01, 500ms);
                    break;
                case ElwynnForest::BROTHER_PAXTON_HEAL_EVENT_02:
                    _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_02_STEP_01, 500ms);
                    break;
                case ElwynnForest::BROTHER_PAXTON_HEAL_EVENT_03:
                    _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_03_STEP_01, 500ms);
                    break;
                case ElwynnForest::BROTHER_PAXTON_HEAL_EVENT_04:
                    _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_04_STEP_01, 500ms);
                    break;
            }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (_cooldownTimer <= diff)
        {
            _cooldown = false;
            _cooldownTimer = 20 * TimeConstants::IN_MILLISECONDS;
        }
        else _cooldownTimer -= diff;

        _events.Update(diff);

        switch (_events.ExecuteEvent())
        {
            case ElwynnForest::HEAL_EVENT_01_STEP_01:
                me->SetReactState(ReactStates::REACT_PASSIVE);

                if (Creature* stormwind_infantry = me->FindNearestCreature(ElwynnForest::NPC_STORMWIND_INFANTRY, 6.0f))
                    stormwind_infantry->AI()->Talk(ElwynnForest::INFANTRY_HELP_YELL);
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_01_STEP_02, 1s);
                break;
            case ElwynnForest::HEAL_EVENT_01_STEP_02:
                if (Creature* stormwind_infantry = me->FindNearestCreature(ElwynnForest::NPC_STORMWIND_INFANTRY, 6.0f))
                    me->SetFacingTo(me->GetAngle(stormwind_infantry));
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_01_STEP_03, 2s);
                break;
            case ElwynnForest::HEAL_EVENT_01_STEP_03:
                Talk(ElwynnForest::BROTHER_PAXTON_TEXT);
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_01_STEP_04, 1s);
                break;
            case ElwynnForest::HEAL_EVENT_01_STEP_04:
                if (Creature* stormwind_infantry = me->FindNearestCreature(ElwynnForest::NPC_STORMWIND_INFANTRY, 6.0f))
                    me->CastSpell(stormwind_infantry, ElwynnForest::SPELL_PENANCE);
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_RESET, 4s);
                break;
            case ElwynnForest::HEAL_EVENT_02_STEP_01:
                me->SetReactState(ReactStates::REACT_PASSIVE);

                if (Creature* stormwind_infantry = me->FindNearestCreature(ElwynnForest::NPC_STORMWIND_INFANTRY, 6.0f))
                    stormwind_infantry->AI()->Talk(ElwynnForest::INFANTRY_HELP_YELL);
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_02_STEP_02, 1s);
                break;
            case HEAL_EVENT_02_STEP_02:
                if (Creature* stormwind_infantry = me->FindNearestCreature(ElwynnForest::NPC_STORMWIND_INFANTRY, 6.0f))
                    me->SetFacingTo(me->GetAngle(stormwind_infantry));
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_02_STEP_03, 2s);
                break;
            case ElwynnForest::HEAL_EVENT_02_STEP_03:
                Talk(ElwynnForest::BROTHER_PAXTON_TEXT);
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_02_STEP_04, 1s);
                break;
            case ElwynnForest::HEAL_EVENT_02_STEP_04:
                if (Creature* stormwind_infantry = me->FindNearestCreature(ElwynnForest::NPC_STORMWIND_INFANTRY, 6.0f))
                    me->CastSpell(stormwind_infantry, ElwynnForest::SPELL_FLASH_HEAL);
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_RESET, 4s);
                break;
            case ElwynnForest::HEAL_EVENT_03_STEP_01:
                me->SetReactState(ReactStates::REACT_PASSIVE);

                if (Creature* stormwind_infantry = me->FindNearestCreature(ElwynnForest::NPC_STORMWIND_INFANTRY, 6.0f))
                    stormwind_infantry->AI()->Talk(ElwynnForest::INFANTRY_HELP_YELL);
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_03_STEP_02, 1s);
                break;
            case ElwynnForest::HEAL_EVENT_03_STEP_02:
                if (Creature* stormwind_infantry = me->FindNearestCreature(ElwynnForest::NPC_STORMWIND_INFANTRY, 6.0f))
                    me->SetFacingTo(me->GetAngle(stormwind_infantry));
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_03_STEP_03, 2s);
                break;
            case ElwynnForest::HEAL_EVENT_03_STEP_03:
                Talk(ElwynnForest::BROTHER_PAXTON_TEXT);
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_03_STEP_04, 1s);
                break;
            case ElwynnForest::HEAL_EVENT_03_STEP_04:
                if (Creature* stormwind_infantry = me->FindNearestCreature(ElwynnForest::NPC_STORMWIND_INFANTRY, 6.0f))
                    me->CastSpell(stormwind_infantry, ElwynnForest::SPELL_RENEW);
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_RESET, 4s);
                break;
            case ElwynnForest::HEAL_EVENT_04_STEP_01:
                me->SetReactState(ReactStates::REACT_PASSIVE);

                if (Creature* stormwind_infantry = me->FindNearestCreature(ElwynnForest::NPC_STORMWIND_INFANTRY, 6.0f))
                    stormwind_infantry->AI()->Talk(ElwynnForest::INFANTRY_HELP_YELL);
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_04_STEP_02, 1s);
                break;
            case ElwynnForest::HEAL_EVENT_04_STEP_02:
                if (Creature* stormwind_infantry = me->FindNearestCreature(ElwynnForest::NPC_STORMWIND_INFANTRY, 6.0f))
                    me->SetFacingTo(me->GetAngle(stormwind_infantry));
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_04_STEP_03, 2s);
                break;
            case ElwynnForest::HEAL_EVENT_04_STEP_03:
                Talk(ElwynnForest::BROTHER_PAXTON_TEXT);
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_04_STEP_04, 1s);
                break;
            case ElwynnForest::HEAL_EVENT_04_STEP_04:
                if (Creature* stormwind_infantry = me->FindNearestCreature(ElwynnForest::NPC_STORMWIND_INFANTRY, 6.0f))
                    me->CastSpell(stormwind_infantry, ElwynnForest::SPELL_REVIVE);
                _events.ScheduleEvent(ElwynnForest::HEAL_EVENT_RESET, 4s);
                break;
            case ElwynnForest::HEAL_EVENT_RESET:
                me->ClearInCombat();
                Reset();
                break;
            default:
                break;
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct npc_blackrock_spy : public ScriptedAI
{
    npc_blackrock_spy(Creature* creature) : ScriptedAI(creature)
    {
        CastSpying();
    }

    void CastSpying()
    {
        GetCreature(-8868.88f, -99.1016f);
        GetCreature(-8936.5f, -246.743f);
        GetCreature(-8922.44f, -73.9883f);
        GetCreature(-8909.68f, -40.0247f);
        GetCreature(-8834.85f, -119.701f);
        GetCreature(-9022.08f, -163.965f);
        GetCreature(-8776.55f, -79.158f);
        GetCreature(-8960.08f, -63.767f);
        GetCreature(-8983.12f, -202.827f);
    }

    void GetCreature(float X, float Y)
    {
        if (me->GetHomePosition().GetPositionX() == X && me->GetHomePosition().GetPositionY() == Y)
            if (!me->IsInCombat() && !me->HasAura(SPELL_SPYING))
                DoCast(me, SPELL_SPYING);

        CastSpyglass();
    }

    void CastSpyglass()
    {
        Spyglass(-8868.88f, -99.1016f, -8936.5f, -246.743f, -8922.44f, -73.9883f, -8909.68f, -40.0247f, -8834.85f,
            -119.701f, -9022.08f, -163.965f, -8776.55f, -79.158f, -8960.08f, -63.767f, -8983.12f, -202.827f);
    }

    void Spyglass(float X1, float Y1, float X2, float Y2, float X3, float Y3, float X4, float Y4, float X5, float Y5,
        float X6, float Y6, float X7, float Y7, float X8, float Y8, float X9, float Y9)
    {
        if ((me->GetHomePosition().GetPositionX() != X1 && me->GetHomePosition().GetPositionY() != Y1) &&
            (me->GetHomePosition().GetPositionX() != X2 && me->GetHomePosition().GetPositionY() != Y2) &&
            (me->GetHomePosition().GetPositionX() != X3 && me->GetHomePosition().GetPositionY() != Y3) &&
            (me->GetHomePosition().GetPositionX() != X4 && me->GetHomePosition().GetPositionY() != Y4) &&
            (me->GetHomePosition().GetPositionX() != X5 && me->GetHomePosition().GetPositionY() != Y5) &&
            (me->GetHomePosition().GetPositionX() != X6 && me->GetHomePosition().GetPositionY() != Y6) &&
            (me->GetHomePosition().GetPositionX() != X7 && me->GetHomePosition().GetPositionY() != Y7) &&
            (me->GetHomePosition().GetPositionX() != X8 && me->GetHomePosition().GetPositionY() != Y8) &&
            (me->GetHomePosition().GetPositionX() != X9 && me->GetHomePosition().GetPositionY() != Y9))
            if (me->GetHomePosition().GetPositionX() == me->GetPositionX() && me->GetHomePosition().GetPositionY() == me->GetPositionY())
                if (!me->IsInCombat() && !me->HasAura(SPELL_SPYGLASS))
                    DoCast(me, SPELL_SPYGLASS);
    }

    void JustEngagedWith(Unit* who) override
    {
        if (who && who->GetTypeId() == TypeID::TYPEID_PLAYER)
            if (roll_chance_i(50))
                Talk(ElwynnForest::TEXT_BLACKROCK_SPY_COMBAT, who);
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        CastSpyglass();

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct npc_blackrock_invader : public ScriptedAI
{
    npc_blackrock_invader(Creature* creature) : ScriptedAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        if (who && who->GetTypeId() == TypeID::TYPEID_PLAYER)
            if (roll_chance_i(50))
                Talk(ElwynnForest::TEXT_BLACKROCK_INVADER_COMBAT, who);
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct npc_goblin_assassin : public ScriptedAI
{
    npc_goblin_assassin(Creature* creature) : ScriptedAI(creature)
    {
        if (!me->IsInCombat() && !me->HasAura(SPELL_SPYING))
            DoCast(SPELL_SNEAKING);
    }

    void JustEngagedWith(Unit* who) override
    {
        if (who && who->GetTypeId() == TypeID::TYPEID_PLAYER)
            if (roll_chance_i(50))
                Talk(ASSASSIN_SAY_COMBAT, who);
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// King Varian Wrynn 29611
class npc_king_varian_wrynn : public CreatureScript
{
    public:
        npc_king_varian_wrynn() : CreatureScript("npc_king_varian_wrynn") { }

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_ALLIANCE_WAY)
                player->CastSpell(creature, SPELL_SUMMON_VARIAN, true);

            return false;
        }

        struct npc_king_varian_wrynnAI : public SmartAI
        {
            npc_king_varian_wrynnAI(Creature* creature) : SmartAI(creature) { }

            void JustEngagedWith(Unit* who) override
            {
                SmartAI::JustEngagedWith(who);
            }

            void UpdateAI(uint32 diff) override
            {
                SmartAI::UpdateAI(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_king_varian_wrynnAI(creature);
        }
};

// Varian Wrynn Alliance Way 61796
class npc_varian_wrynn_alliance_way_quest : public CreatureScript
{
    public:
        npc_varian_wrynn_alliance_way_quest() : CreatureScript("npc_varian_wrynn_alliance_way_quest") { }

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_AN_OLD_PIT_FIGHTER)
                player->CastSpell(creature, SPELL_SUMMON_VARIAN, true);

            return false;
        }

        struct npc_varian_wrynn_alliance_way_questAI : public ScriptedAI
        {
            npc_varian_wrynn_alliance_way_questAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid summonerGUID;
            uint32 delay;

            void IsSummonedBy(Unit* summoner) override
            {
                summonerGUID = summoner->GetGUID();
                me->SetFacingToObject(summoner);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                me->SetPhaseMask(2, true);

                if (summoner->ToPlayer() && summoner->ToPlayer()->GetQuestStatus(QUEST_ALLIANCE_WAY) == QUEST_STATUS_REWARDED)
                {
                    DoAction(ACTION_AN_OLD_PIT_FIGHTER);
                    return;
                }

                delay = 0;
                me->m_Events.Schedule(delay += 1500, 17, [this]()
                {
                    Movement::MoveSplineInit init(me);
                    for (auto itr : VarianPath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                    init.SetWalk(true);
                    init.Launch();

                    me->m_Events.Schedule(delay += me->GetSplineDuration(), 16, [this]()
                    {
                        me->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() + M_PI));
                        me->DespawnOrUnsummon(1 * IN_MILLISECONDS);

                        if (Player* owner = ObjectAccessor::GetPlayer(*me, summonerGUID))
                        {
                            owner->KilledMonsterCredit(NPC_WALK_WITH_VARIAN_CREDIT);

                            delay = 0;
                            owner->m_Events.Schedule(delay += 1000, 1, [owner]()
                            {
                                owner->SetPhaseMask(1, true);
                            });
                        }
                    });

                    delay = 0;
                    me->m_Events.Schedule(delay += 2000, 1, [this]()
                    {
                        Talk(SAY_INTRO);
                    });

                    me->m_Events.Schedule(delay += 5000, 2, [this]()
                    {
                        Talk(SAY_SPECIAL_1);
                    });

                    me->m_Events.Schedule(delay += 5000, 3, [this]()
                    {
                        Talk(SAY_SPECIAL_2);
                    });

                    me->m_Events.Schedule(delay += 7000, 4, [this]()
                    {
                        Talk(SAY_SPECIAL_3);
                    });

                    me->m_Events.Schedule(delay += 9000, 5, [this]()
                    {
                        Talk(SAY_SPECIAL_4);
                    });

                    me->m_Events.Schedule(delay += 6500, 6, [this]()
                    {
                        Talk(SAY_SPECIAL_5);
                    });

                    me->m_Events.Schedule(delay += 10000, 7, [this]()
                    {
                        Talk(SAY_SPECIAL_6);
                    });

                    me->m_Events.Schedule(delay += 9000, 8, [this]()
                    {
                        Talk(SAY_SPECIAL_7);
                    });

                    me->m_Events.Schedule(delay += 6000, 9, [this]()
                    {
                        Talk(SAY_SPECIAL_8);
                    });

                    me->m_Events.Schedule(delay += 6000, 10, [this]()
                    {
                        Talk(SAY_SPECIAL_9);
                    });

                    me->m_Events.Schedule(delay += 5000, 11, [this]()
                    {
                        Talk(SAY_SPECIAL_10);
                    });

                    me->m_Events.Schedule(delay += 7000, 12, [this]()
                    {
                        Talk(SAY_SPECIAL_11);
                    });

                    me->m_Events.Schedule(delay += 9000, 13, [this]()
                    {
                        Talk(SAY_SPECIAL_12);
                    });

                    me->m_Events.Schedule(delay += 13000, 14, [this]()
                    {
                        Talk(SAY_SPECIAL_13);
                    });
                });
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_AN_OLD_PIT_FIGHTER)
                {
                    delay = 0;
                    me->m_Events.Schedule(delay += 1500, 18, [this]()
                    {
                        Talk(SAY_SPECIAL_14);
                    });

                    me->m_Events.Schedule(delay += 11000, 20, [this]()
                    {
                        me->SetFaction(16);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                        Talk(SAY_SPECIAL_15);
                    });

                    me->m_Events.Schedule(delay += 9300, 21, [this]()
                    {
                        Talk(SAY_SPECIAL_16);
                    });

                    me->m_Events.Schedule(delay += 6000, 22, [this]()
                    {
                        Talk(SAY_SPECIAL_17);
                    });

                    me->m_Events.Schedule(delay += 12000, 23, [this]()
                    {
                        Talk(SAY_SPECIAL_18);
                    });

                    me->m_Events.Schedule(delay += 2900, 24, [this]()
                    {
                        if (Player* owner = ObjectAccessor::GetPlayer(*me, summonerGUID))
                        {
                            owner->KilledMonsterCredit(NPC_FIGHT_WITH_VARIAN_CREDIT);
                            owner->CombatStop();
                        }

                        me->CombatStop();
                        me->SetFaction(35);
                    });

                    me->m_Events.Schedule(delay += 3500, 25, [this]()
                    {
                        Talk(SAY_SPECIAL_19);
                    });

                    me->m_Events.Schedule(delay += 4000, 26, [this]()
                    {
                        Movement::MoveSplineInit init(me);
                        for (auto itr : VarianHomePath)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                        init.SetUncompressed();
                        init.Launch();

                        delay = 0;
                        me->m_Events.Schedule(delay += me->GetSplineDuration(), 27, [this]()
                        {
                            if (Player* owner = ObjectAccessor::GetPlayer(*me, summonerGUID))
                                owner->SetPhaseMask(1, true);

                            me->DespawnOrUnsummon();
                        });
                    });
                }
            }

            void DamageTaken(Unit* who, uint32& damage) override
            {
                damage = 0;
                me->HandleEmoteCommand(EMOTE_ONESHOT_DODGE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_varian_wrynn_alliance_way_questAI(creature);
        }
};

// Ayisa and Jo Jo Alliance way 61792, 61793
class npc_ayisa_jojo_alliance_way_quest : public CreatureScript
{
    public:
        npc_ayisa_jojo_alliance_way_quest() : CreatureScript("npc_ayisa_jojo_alliance_way_quest") { }

        struct npc_ayisa_jojo_alliance_way_questAI : public ScriptedAI
        {
            npc_ayisa_jojo_alliance_way_questAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid summonerGUID;
            uint32 delay;
            uint8 point;
            bool follow;

            void IsSummonedBy(Unit* summoner) override
            {
                point = 0;
                summonerGUID = summoner->GetGUID();
                me->SetPhaseMask(2, true);
                me->DespawnOrUnsummon(2 * MINUTE * IN_MILLISECONDS + 30 * IN_MILLISECONDS);
                follow = true;

                if (summoner->ToPlayer() && summoner->ToPlayer()->GetQuestStatus(QUEST_ALLIANCE_WAY) == QUEST_STATUS_REWARDED)
                {
                    DoAction(ACTION_AN_OLD_PIT_FIGHTER);
                    follow = false;
                    return;
                }

                if (me->GetEntry() == NPC_AYISA_ALLIANCE_WAY)
                {
                    delay = 0;
                    me->m_Events.Schedule(delay += 300, 1, [this]()
                    {
                        follow = false;
                    });
                    
                    me->m_Events.Schedule(delay += 6000, 2, [this]()
                    {
                        Talk(SAY_INTRO);
                    });

                    me->m_Events.Schedule(delay += 46000, 3, [this]()
                    {
                        Talk(SAY_SPECIAL_1);
                    });

                    me->m_Events.Schedule(delay += 51000, 4, [this]()
                    {
                        Talk(SAY_SPECIAL_2);
                    });
                }
                
                if (me->GetEntry() == NPC_JO_JO_ALLIANCE_WAY)
                {
                    delay = 0;
                    me->m_Events.Schedule(delay += 600, 1, [this]()
                    {
                        follow = false;
                    });
                }
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_AN_OLD_PIT_FIGHTER)
                {
                    delay = 0;
                    me->m_Events.Schedule(delay += 11000, 19, [this]()
                    {
                        Movement::MoveSplineInit init(me);
                        point = me->GetEntry() == NPC_AYISA_ALLIANCE_WAY ? 0 : 1;
                        init.MoveTo(AysaJojoStandPos[point].GetPositionX(), AysaJojoStandPos[point].GetPositionY(), AysaJojoStandPos[point].GetPositionZ());

                        init.Launch();

                        me->m_Events.Schedule(delay += 19000, 21, [this]()
                        {
                            if (me->GetEntry() == NPC_AYISA_ALLIANCE_WAY)
                                if (Unit* owner = ObjectAccessor::GetUnit(*me, summonerGUID))
                                    Talk(SAY_SPECIAL_3, owner);
                        });

                        me->m_Events.Schedule(delay += 10800, 22, [this]()
                        {
                            follow = true;
                        });

                        delay = 0;
                        me->m_Events.Schedule(delay += me->GetSplineDuration(), 20, [this]()
                        {
                            me->SetFacingTo(AysaJojoStandPos[point].GetOrientation());
                        });
                    });
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (follow)
                    if (Unit* owner = ObjectAccessor::GetUnit(*me, summonerGUID))
                        me->GetMotionMaster()->MoveFollow(owner, 2.5f, me->GetAngle(owner));
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ayisa_jojo_alliance_way_questAI(creature);
        }
};

// Summon Varian alliance way 120352
class spell_summ_varian_alliance_way : public SpellScript
{
    PrepareSpellScript(spell_summ_varian_alliance_way);

    void HandleAfterCast()
    {
        if (Player* pCaster = GetCaster()->ToPlayer())
        {
            pCaster->SetPhaseMask(3, true);
            pCaster->CastSpell(pCaster, SPELL_SUMMON_AYSA_AND_JOJO, true);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_summ_varian_alliance_way::HandleAfterCast);
    }
};

struct npc_hogger : public ScriptedAI
{
    npc_hogger(Creature* creature) : ScriptedAI(creature)
    {
        if (Creature* trigger_meat = me->FindNearestCreature(ElwynnForest::NPC_TRIGGER_MEAT, 30))
            trigger_meat_guid = trigger_meat->GetGUID();
    }

    void Reset() override
    {
        me->SetReactState(REACT_AGGRESSIVE);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
        phase = 0;
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        if (urand(0, 9) < 3)
            Talk(ElwynnForest::SAY_AGGRO);

        events.ScheduleEvent(ElwynnForest::EVENT_CAST_VICIOUS_SLICE, 3s);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (!phase && !HealthAbovePct(50))
        {
            Talk(ElwynnForest::SAY_HELP);
            me->CastSpell(me, ElwynnForest::SPELL_SUMMON_MINIONS, true);
            me->AttackStop();
            me->SetReactState(REACT_PASSIVE);
            phase = ElwynnForest::PHASE_EATING;
            events.CancelEvent(ElwynnForest::EVENT_CAST_VICIOUS_SLICE);
            events.ScheduleEvent(ElwynnForest::EVENT_HALF_HP_ONCE, 4s);
        }
        else if (phase == ElwynnForest::PHASE_EATING && me->HasAura(ElwynnForest::SPELL_UPSET_STOMACH))
        {
            Talk(ElwynnForest::SAY_STUNNED);
            phase = ElwynnForest::PHASE_EATING_ENDED;
            events.CancelEvent(ElwynnForest::EVENT_CAST_BLOODY_STRIKE);
            events.RescheduleEvent(ElwynnForest::EVENT_BACK_TO_FIGHT, 10s);
        }

        if (damage >= me->GetHealth() && phase != ElwynnForest::PHASE_FINAL)
        {
            Talk(ElwynnForest::SAY_NO_HURT);
            me->RemoveAllAuras();
            me->AttackStop();
            attacker->AttackStop();
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
            me->SetWalk(true);
            me->GetMotionMaster()->MovePoint(1, -10136.9f, 670.009f, 36.03682f);
            damage = me->GetHealth() - 1;
            phase = ElwynnForest::PHASE_FINAL;
            events.Reset();

            if (Player* player = me->GetLootRecipient())
                player->RewardPlayerAndGroupAtKill(me, false);
        }
        else if (phase == ElwynnForest::PHASE_FINAL)
        {
            attacker->AttackStop();
            damage = me->GetHealth() - 1;
        }
    }

    void MovementInform(uint32 /*type*/, uint32 waypoint_id) override
    {
        switch (waypoint_id)
        {
            case 0:
                me->CastSpell(me, ElwynnForest::SPELL_EATING, true);
                events.ScheduleEvent(ElwynnForest::EVENT_CAST_BLOODY_STRIKE, 0);
                events.ScheduleEvent(ElwynnForest::EVENT_BACK_TO_FIGHT, 15s);
                break;
            case 1:
                events.ScheduleEvent(ElwynnForest::EVENT_SUMMON_GUARDS, 1s);
                break;
        }
    }

    void SetData(uint32 /*type*/, uint32 /*data*/) override
    {
        me->CastSpell(me, ElwynnForest::SPELL_TELEPORT_VISUAL_ONLY, true);
        me->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);
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
                case ElwynnForest::EVENT_CAST_VICIOUS_SLICE:
                    if (Unit* victim = me->GetVictim())
                        me->CastSpell(victim, ElwynnForest::SPELL_VICIOUS_SLICE, false);

                    events.ScheduleEvent(ElwynnForest::EVENT_CAST_VICIOUS_SLICE, 12s);
                    break;
                case ElwynnForest::EVENT_HALF_HP_ONCE:
                {
                    Talk(ElwynnForest::SAY_EATING);
                    float trigger_x, trigger_y;

                    if (Creature* trigger_meat = ObjectAccessor::GetCreature(*me, trigger_meat_guid))
                    {
                        trigger_meat->GetPosition(trigger_x, trigger_y);
                        const float angle = me->GetAngle(trigger_x, trigger_y);
                        const float x = trigger_meat->GetPositionX() - 2 * cos(angle);
                        const float y = trigger_meat->GetPositionY() - 2 * sin(angle);
                        const float z = trigger_meat->GetMap()->GetHeight(x, y, trigger_meat->GetPositionZ());
                        me->GetMotionMaster()->MovePoint(0, x, y, z);
                    }
                    break;
                }
                case ElwynnForest::EVENT_CAST_BLOODY_STRIKE:
                    if (Creature* meatTrigger = ObjectAccessor::GetCreature(*me, trigger_meat_guid))
                        me->CastSpell(meatTrigger, ElwynnForest::SPELL_BLOODY_STRIKE, false);

                    events.ScheduleEvent(ElwynnForest::EVENT_CAST_BLOODY_STRIKE, 3s);
                    break;
                case ElwynnForest::EVENT_BACK_TO_FIGHT:
                    me->SetReactState(REACT_AGGRESSIVE);
                    phase = ElwynnForest::PHASE_EATING_ENDED;
                    events.CancelEvent(ElwynnForest::EVENT_CAST_BLOODY_STRIKE);
                    events.ScheduleEvent(ElwynnForest::EVENT_CAST_VICIOUS_SLICE, 3s);
                    break;
                case ElwynnForest::EVENT_SUMMON_GUARDS:
                    me->SummonCreature(ElwynnForest::NPC_JONATHAN, -10128.3f, 656.465f, 36.05443f, 2.045435f, TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(ElwynnForest::NPC_DUMAS, -10132.9f, 653.561f, 36.05033f, 2.020999f, TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(ElwynnForest::NPC_ANDROMATH, -10123.0f, 656.875f, 36.05113f, 1.971813f, TEMPSUMMON_MANUAL_DESPAWN);
                    events.ScheduleEvent(ElwynnForest::EVENT_SUMMON_KIDS, 4s);
                    break;
                case ElwynnForest::EVENT_SUMMON_KIDS:
                {
                    if (Creature* ragamuffin_1 = me->SummonCreature(ElwynnForest::NPC_RAGAMUFFIN, -10130.9f, 653.302f, 36.05013f, 1.652422f, TEMPSUMMON_TIMED_DESPAWN, 7500ms))
                        ragamuffin_1->AI()->SetData(0, 1);

                    if (Creature* ragamuffin_2 = me->SummonCreature(ElwynnForest::NPC_RAGAMUFFIN, -10122.5f, 660.198f, 36.03663f, 2.837752f, TEMPSUMMON_TIMED_DESPAWN, 6500ms))
                        ragamuffin_2->AI()->SetData(0, 2);

                    events.ScheduleEvent(ElwynnForest::EVENT_SAY_GRR, 8s + 500ms);
                    break;
                }
                case ElwynnForest::EVENT_SAY_GRR:
                    Talk(ElwynnForest::SAY_GRR);

                    if (Creature* jonathan = me->FindNearestCreature(ElwynnForest::NPC_JONATHAN, 10))
                        me->SetFacingToObject(jonathan);

                    events.ScheduleEvent(ElwynnForest::EVENT_SAY_NO, 15s);
                    break;
                case ElwynnForest::EVENT_SAY_NO:
                    Talk(ElwynnForest::SAY_NO);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap events;
    ObjectGuid trigger_meat_guid;
    uint8 phase = 0;
};

struct npc_minion_of_hogger : public ScriptedAI
{
    npc_minion_of_hogger(Creature* creature) : ScriptedAI(creature) { }

    void JustDied(Unit* /*killer*/) override
    {
        DoCastAOE(ElwynnForest::SPELL_ADVENTURERS_RUSH);
    }

    void IsSummonedBy(Unit* summoner) override
    {
        if (summoner->GetVictim())
            me->AI()->AttackStart(summoner->GetVictim());
    }
};

// 26391
class quest_extinguishing_hope : public QuestScript
{
public:
    quest_extinguishing_hope() : QuestScript("quest_extinguishing_hope") {}

    void OnQuestStatusChange(Player* player, Quest const* /*quest*/, QuestStatus /*oldStatus*/, QuestStatus newStatus) override
    {
        if (newStatus == QUEST_STATUS_INCOMPLETE)
            player->CastSpell(player, SPELL_FIRE_EXTINGUISHER_AURA, true);
        else if (newStatus == QUEST_STATUS_REWARDED || newStatus == QUEST_STATUS_NONE)
            player->RemoveAurasDueToSpell(SPELL_FIRE_EXTINGUISHER_AURA);
    }
};

void AddSC_elwynn_forest()
{

    RegisterCreatureAI(npc_stormwind_infantry);
    new creature_script<npc_blackrock_battle_worg>("npc_blackrock_battle_worg");
    RegisterCreatureAI(npc_brother_paxton);
    RegisterCreatureAI(npc_blackrock_spy);
    new creature_script<npc_goblin_assassin>("npc_goblin_assassin");
    new creature_script<npc_blackrock_invader>("npc_blackrock_invader");
    new npc_king_varian_wrynn();
    new npc_varian_wrynn_alliance_way_quest();
    new npc_ayisa_jojo_alliance_way_quest();
    new spell_script<spell_summ_varian_alliance_way>("spell_summ_varian_alliance_way");
    new creature_script<npc_hogger>("npc_hogger");
    new creature_script<npc_minion_of_hogger>("npc_minion_of_hogger");
    new quest_extinguishing_hope();
}
