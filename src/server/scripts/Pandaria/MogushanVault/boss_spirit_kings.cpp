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

#include "mogu_shan_vault.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"

enum eSpells
{
    // Quiang
    SPELL_FLANKING_ORDERS          = 117910, // Also when vanquished
    SPELL_MASSIVE_ATTACKS          = 117921,
    SPELL_ANNIHILATE               = 117948,
    SPELL_IMPERVIOUS_SHIELD        = 117961, // Heroic

    // Subetai
    SPELL_PILLAGE_CHARGE           = 118047, // Also when vanquished
    SPELL_PILLAGE                  = 118049,
    SPELL_PILLAGE_AURA             = 118048,
    SPELL_VOLLEY_VISUAL            = 118100,
    SPELL_VOLLEY_1                 = 118094,
    SPELL_VOLLEY_2                 = 118105,
    SPELL_VOLLEY_3                 = 118106,
    SPELL_RAIN_OF_ARROWS           = 118122,
    SPELL_PINNED_DOWN              = 118145,
    SPELL_PINNED_DOWN_DOT          = 118135,
    SPELL_PINNING_ARROW_VISUAL     = 118141,
    SPELL_SLEIGHT_OF_HAND          = 118162, // Heroic
    SPELL_ROBBED_BLIND             = 118163, // Heroic

    // Zian
    SPELL_UNDYING_SHADOWS          = 117506, // Also when vanquished
    SPELL_FIXATE                   = 118303,
    SPELL_UNDYING_SHADOW_DOT       = 117514,
    SPELL_COALESCING_SHADOW_DOT    = 117539,

    SPELL_SHADOW_BLAST             = 117628,
    SPELL_CHARGED_SHADOWS          = 117685,
    SPELL_SHIELD_OF_DARKNESS       = 117697, // Heroic
    SPELL_DARKNESS                 = 117701, // Heroic

    // Meng
    SPELL_MADDENING_SHOUT          = 117708, // Also when vanquished
    SPELL_CRAZED                   = 117737,
    SPELL_COWARDICE                = 117756,
    SPELL_COWARDICE_DOT            = 117829,
    SPELL_CRAZY_THOUGHT            = 117833,
    SPELL_DELIRIOUS                = 117837, // Heroic

    // Shared
    SPELL_INACTIVE                 = 118205,
    SPELL_INACTIVE_STUN            = 118319,
    SPELL_KING_BERSERK             = 120207,
    SPELL_ACTIVATION_VISUAL        = 118212,

    // Flanking Mogu
    SPELL_GHOST_VISUAL             = 117904,
    SPELL_TRIGGER_ATTACK           = 117917,

    // Achievements
    SPELL_GETTING_HOT_IN_HERE      = 128545,
};

enum eEvents
{
    // Controler
    EVENT_CHECK_WIPE               = 1,

    // Quiang
    EVENT_FLANKING_MOGU            = 2,
    EVENT_MASSIVE_ATTACK           = 3,
    EVENT_ANNIHILATE               = 4,
    EVENT_ANNIHILATE_CANCEL        = 5,

    // Subetai
    EVENT_PILLAGE                  = 6,
    EVENT_VOLLEY_1                 = 7,
    EVENT_VOLLEY_2                 = 8,
    EVENT_VOLLEY_3                 = 9,
    EVENT_VOLLEY_CANCEL            = 10,
    EVENT_RAIN_OF_ARROWS           = 11,

    // Zian
    EVENT_UNDYING_SHADOWS          = 12,
    EVENT_SHADOW_BLAST             = 13,
    EVENT_CHARGED_SHADOWS          = 14,

    // Meng
    EVENT_MADDENING_SHOUT          = 15,
    EVENT_CRAZED                   = 16,
    EVENT_CRAZY_THOUGHT            = 17,

    // Heroic
    EVENT_IMPERVIOUS_SHIELD        = 18,
    EVENT_DELIRIOUS                = 19,
    EVENT_SLEIGHT_OF_HAND          = 20,
    EVENT_SHIELD_OF_DARKNESS       = 21,

    // Misc
    EVENT_GETTING_HOT_IN_HERE      = 22,
    EVENT_EVADE_CONTROLLER         = 23,
    EVENT_BERSERK                  = 24,
};

enum equipId
{
    EQUIP_QIANG_POLEARM            = 86777,
    EQUIP_SUBETAI_SWORD            = 80283,
    EQUIP_SUBETAI_BOW              = 60790,
    EQUIP_ZIAN_STAFF               = 76369,
    EQUIP_FLANKING_MOGU_SWORD      = 68838,
    EQUIP_FLANKING_MOGU_SHIELD     = 32375,
};

enum qiangTalk
{
    QIANG_AGGRO                    = 0,
    QIANG_DEATH                    = 1,
    QIANG_INTRO                    = 2,
    QIANG_SLAY                     = 3,
    QIANG_SPELL                    = 4,
    QIANG_FLANKING_ORDERS_ANN      = 5,
};

enum subetaiTalk
{
    SUBETAI_AGGRO                  = 0,
    SUBETAI_DEATH                  = 1,
    SUBETAI_INTRO                  = 2,
    SUBETAI_SLAY                   = 3,
    SUBETAI_SPELL                  = 4,
    SUBRTAI_PILLAGE_ANN            = 5,
};

enum zianTalk
{
    ZIAN_AGGRO                     = 0,
    ZIAN_DEATH                     = 1,
    ZIAN_INTRO                     = 2,
    ZIAN_SLAY                      = 3,
    ZIAN_SPELL                     = 4,
    ZIAN_UNDYING_SHADOWS_ANN       = 5,
};

enum mengTalk
{
    MENG_AGGRO                     = 0,
    MENG_DEATH_CRAZED              = 1,
    MENG_DEATH_COWARDICE           = 2,
    MENG_INTRO                     = 3,
    MENG_SLAY                      = 4,
    MENG_SPELL                     = 5,
    MENG_MADDENING_SHOUT_ANN       = 6,
};

enum Misc
{
    MAX_FLANKING_MOGU       = 48,
    SPIRIT_KINGS_LOOT_ID    = 60709,
};

Position flankingPos[MAX_FLANKING_MOGU] =
{
    { 4195.25f, 1590.85f, 438.841f, 0.12350f },
    { 4194.99f, 1586.06f, 438.841f, 0.26639f },
    { 4193.99f, 1581.04f, 438.840f, 0.39482f },
    { 4195.19f, 1576.21f, 438.840f, 0.52845f },
    { 4196.93f, 1571.99f, 438.839f, 0.64866f },
    { 4200.06f, 1568.31f, 438.839f, 0.77664f },
    { 4203.17f, 1564.49f, 438.837f, 0.90416f },
    { 4206.52f, 1561.59f, 438.837f, 1.01808f },
    { 4209.94f, 1558.79f, 438.836f, 1.12821f },
    { 4213.91f, 1556.73f, 438.837f, 1.23942f },
    { 4217.99f, 1556.02f, 438.839f, 1.34239f },
    { 4222.20f, 1555.52f, 438.839f, 1.44913f },
    { 4226.90f, 1555.01f, 438.839f, 1.56812f },
    { 4231.69f, 1555.00f, 438.839f, 1.68814f },
    { 4236.49f, 1556.31f, 438.838f, 1.81247f },
    { 4241.28f, 1558.22f, 438.837f, 1.94289f },
    { 4245.75f, 1561.11f, 438.837f, 2.07869f },
    { 4250.17f, 1563.56f, 438.837f, 2.20908f },
    { 4253.08f, 1568.12f, 438.839f, 2.34491f },
    { 4255.07f, 1573.32f, 438.840f, 2.48865f },
    { 4257.26f, 1579.05f, 438.840f, 2.66189f },
    { 4257.36f, 1584.80f, 438.841f, 2.82356f },
    { 4258.93f, 1591.29f, 438.841f, 3.03247f },
    { 4259.81f, 1597.36f, 438.841f, 3.21968f },
    { 4265.56f, 1603.04f, 438.842f, 3.35247f },
    { 4264.61f, 1608.74f, 438.843f, 3.49683f },
    { 4263.33f, 1613.64f, 438.841f, 3.62029f },
    { 4260.61f, 1618.46f, 438.842f, 3.75534f },
    { 4257.08f, 1622.41f, 438.845f, 3.88442f },
    { 4253.70f, 1625.77f, 438.845f, 4.00111f },
    { 4250.06f, 1629.08f, 438.844f, 4.12039f },
    { 4245.74f, 1631.66f, 438.843f, 4.24225f },
    { 4241.14f, 1634.23f, 438.846f, 4.36825f },
    { 4236.42f, 1635.51f, 438.847f, 4.48507f },
    { 4231.43f, 1635.73f, 438.849f, 4.60476f },
    { 4226.52f, 1635.65f, 438.851f, 4.72422f },
    { 4221.75f, 1635.25f, 438.851f, 4.84165f },
    { 4217.01f, 1633.81f, 438.853f, 4.96304f },
    { 4211.88f, 1632.07f, 438.853f, 5.09767f },
    { 4207.12f, 1629.80f, 438.851f, 5.22899f },
    { 4202.68f, 1626.57f, 438.851f, 5.36565f },
    { 4198.73f, 1623.16f, 438.850f, 5.49605f },
    { 4195.37f, 1619.05f, 438.849f, 5.62901f },
    { 4192.82f, 1614.57f, 438.849f, 5.75861f },
    { 4190.84f, 1609.64f, 438.847f, 5.89364f },
    { 4190.58f, 1604.28f, 438.850f, 6.02825f },
    { 4192.61f, 1600.68f, 438.844f, 6.11362f },
    { 4195.38f, 1595.63f, 438.841f, 6.25653f }
};

Position QiangTheMerciless = { 4226.33f, 1626.28f, 438.856f, 4.72348f };
Position SubetaiTheSwift = { 4257.35f, 1591.36f, 438.841f, 3.13526f };
Position ZianOfTheEndlessShadow = { 4226.97f, 1558.32f, 438.804f, 1.58495f };
Position MengTheDemented = { 4198.78f, 1590.29f, 438.841f, 6.26345f };

uint32 volleySpells[3] =
{
    SPELL_VOLLEY_1,
    SPELL_VOLLEY_2,
    SPELL_VOLLEY_3
};

// 60984 - Controller
class boss_spirit_kings_controller : public CreatureScript
{
    public:
        boss_spirit_kings_controller() : CreatureScript("boss_spirit_kings_controler") { }

        struct boss_spirit_kings_controllerAI : public BossAI
        {
            boss_spirit_kings_controllerAI(Creature* creature) : BossAI(creature, DATA_SPIRIT_KINGS) { }

            ObjectGuid flankingGuid[MAX_FLANKING_MOGU];
            std::vector<uint32> spiritKingsEntry;
            bool fightInProgress;
            bool _introZianDone;
            bool _introSubetaiDone;
            bool _introMengDone;
            uint8 vanquishedCount;
            uint8 nextSpirit;

            void InitializeAI() override
            {
                spiritKingsEntry.resize(4, 0);
                spiritKingsEntry[0] = NPC_QIANG;

                if (instance && instance->GetData(DATA_SPIRIT_KINGS_STATE) != DONE)
                {
                    if (instance->GetData(DATA_LFR) && !instance->GetData(DATA_LFR_SECOND_QUARTER))
                        return;

                    me->SummonCreature(NPC_LOREWALKER_CHO, ChoPastGarajalSpawn, TEMPSUMMON_MANUAL_DESPAWN);
                }

                Reset();
            }

            void Reset() override
            {
                fightInProgress = false;
                _introZianDone = false;
                _introSubetaiDone = false;
                _introMengDone = false;
                vanquishedCount = 0;
                nextSpirit = 0;

                for (uint8 i = 0; i < MAX_FLANKING_MOGU; ++i)
                    flankingGuid[i] = ObjectGuid::Empty;

                me->SetReactState(REACT_PASSIVE);
                instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MADDENING_SHOUT);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PINNED_DOWN);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PINNED_DOWN_DOT);
            }

            void spawnFlankingMogu(uint8 moguNumber)
            {
                if (moguNumber >= MAX_FLANKING_MOGU)
                    return;

                if (Creature* flankingMogu = me->SummonCreature(NPC_FLANKING_MOGU, flankingPos[moguNumber].GetPositionX(), flankingPos[moguNumber].GetPositionY(), 438.80f, flankingPos[moguNumber].GetOrientation()))
                {
                    flankingGuid[moguNumber] = flankingMogu->GetGUID();
                    flankingMogu->SetReactState(REACT_PASSIVE);
                    flankingMogu->AddAura(SPELL_GHOST_VISUAL, flankingMogu);
                    flankingMogu->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 0, uint32(EQUIP_FLANKING_MOGU_SWORD));
                    flankingMogu->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, uint32(EQUIP_FLANKING_MOGU_SHIELD));
                }
            }

            void DoAction(int32 actionId) override
            {
                if (!instance)
                    return;

                switch (actionId)
                {
                    case ACTION_ENTER_COMBAT:
                    {
                        if (fightInProgress)
                            break;

                        for (uint8 i = 0; i < MAX_FLANKING_MOGU; ++i)
                            spawnFlankingMogu(i);

                        std::vector<uint32> spiritKingsSeq = { NPC_ZIAN, NPC_MENG, NPC_SUBETAI };

                        for (uint32 i_dat = DATA_SPIRIT_SECOND; i_dat < DATA_SPIRIT_FOUR + 1; i_dat++)
                        {
                            if (instance)
                                instance->SetData(i_dat, spiritKingsSeq.back());

                            spiritKingsSeq.pop_back();
                        }
                        

                        for (uint32 i = 1; i < 4; ++i)
                            spiritKingsEntry[i] = instance->GetData(DATA_SPIRIT_KINGS_STATE + i);

                        if (Creature* incomingSpirit = instance->instance->GetCreature(instance->GetGuidData(spiritKingsEntry[1])))
                            me->AddAura(SPELL_ACTIVATION_VISUAL, incomingSpirit);

                        fightInProgress = true;
                        events.ScheduleEvent(EVENT_BERSERK, 10 * MINUTE * IN_MILLISECONDS);
                        break;
                    }
                    case ACTION_FLANKING_MOGU:
                    {
                        uint8 moguBegin = urand(0, 47);
                        uint8 middleMogu = (moguBegin + 3) > 47 ? (moguBegin + 3) - 48 : (moguBegin + 3);
                        float orientation = 0.0f;

                        if (Creature* flankingMogu = instance->instance->GetCreature(flankingGuid[middleMogu]))
                            orientation = flankingMogu->GetOrientation();

                        for (uint8 i = moguBegin; i < moguBegin + 6; ++i)
                        {
                            uint8 j = i;

                            if (j >= 48)
                                j -= 48;

                            if (Creature* flankingMogu = instance->instance->GetCreature(flankingGuid[j]))
                            {
                                flankingMogu->RemoveAurasDueToSpell(SPELL_GHOST_VISUAL);
                                flankingMogu->HandleEmoteCommand(EMOTE_ONESHOT_SHOUT);

                                float x = 0.0f, y = 0.0f;
                                GetPositionWithDistInOrientation(flankingMogu, 80.0f, orientation, x, y);

                                uint32 delay = 0;
                                ObjectGuid moguGUID = flankingMogu->GetGUID();

                                me->m_Events.Schedule(delay += 2000, 20, [this, moguGUID, x, y]()
                                {
                                    if (Creature* flankingMogu = ObjectAccessor::GetCreature(*me, moguGUID))
                                    {
                                        flankingMogu->AddAura(SPELL_TRIGGER_ATTACK, flankingMogu);

                                        Movement::MoveSplineInit init(flankingMogu);
                                        init.MoveTo(x, y, me->GetPositionZ());
                                        init.Launch();

                                        flankingMogu->DespawnOrUnsummon(flankingMogu->GetSplineDuration());
                                    }
                                });

                                spawnFlankingMogu(j); // Spawn a new one to remplace the old one
                            }
                        }
                        break;
                    }
                    case ACTION_ACTIVATE_SPIRIT:
                    {
                        if (IsHeroic())
                            nextSpirit = vanquishedCount + 1;

                        if (nextSpirit < 4)
                        {
                            if (!IsHeroic())
                                nextSpirit = vanquishedCount;

                            if (Creature* spirit = ObjectAccessor::GetCreature(*me, instance->GetGuidData(spiritKingsEntry[nextSpirit])))
                            {
                                spirit->AI()->DoAction(ACTION_ENTER_COMBAT);

                                if (nextSpirit < 3)
                                {
                                    nextSpirit++;

                                    if (Creature* incomingSpirit = instance->instance->GetCreature(instance->GetGuidData(spiritKingsEntry[nextSpirit])))
                                        me->AddAura(SPELL_ACTIVATION_VISUAL, incomingSpirit);
                                }
                            }
                        }
                        break;
                    }
                    case ACTION_SPIRIT_KILLED:
                    {
                        nextSpirit = ++vanquishedCount;

                        if (nextSpirit >= 4)
                        {
                            if (instance)
                            {
                                instance->SetBossState(DATA_SPIRIT_KINGS, DONE);
                                instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                            }

                            summons.DespawnEntry(NPC_FLANKING_MOGU);
                            summons.DespawnEntry(NPC_UNDYING_SHADOW);
                            me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                            events.Reset();

                            for (auto&& entry : spiritKingsEntry)
                            {
                                if (Creature* spirit = instance->instance->GetCreature(instance->GetGuidData(entry)))
                                {
                                    spirit->LowerPlayerDamageReq(spirit->GetMaxHealth());
                                    spirit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                    spirit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                    spirit->RemoveAura(SPELL_INACTIVE);

                                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MADDENING_SHOUT);

                                    if (Unit* killer = spirit->AI()->SelectTarget(SELECT_TARGET_TOPAGGRO))
                                        killer->Kill(spirit);
                                }
                            }

                            if (Player* player = me->GetMap()->GetFirstPlayerInInstance())
                                player->RewardPersonalLootAndCurrency(me, SPIRIT_KINGS_LOOT_ID);
                        }
                        else
                        {
                            // In heroic difficulty, next spirit has been activated before current spirit is killed
                            if (!IsHeroic())
                                DoAction(ACTION_ACTIVATE_SPIRIT);
                        }
                        break;
                    }
                    case ACTION_SAY_INTRO_FOR_NEXT_SPIRIT_IF_ANY:
                    {
                        if (vanquishedCount > 2)
                            break;

                        if (Creature* incomingSpirit = instance->instance->GetCreature(instance->GetGuidData(spiritKingsEntry[vanquishedCount + 1])))
                        {
                            switch (incomingSpirit->GetEntry())
                            {
                                case NPC_ZIAN:
                                    if (_introZianDone)
                                        break;

                                    incomingSpirit->AI()->Talk(ZIAN_INTRO);
                                    _introZianDone = true;
                                    break;
                                case NPC_SUBETAI:
                                    if (_introSubetaiDone)
                                        break;

                                    incomingSpirit->AI()->Talk(SUBETAI_INTRO);
                                    _introSubetaiDone = true;
                                    break;
                                case NPC_MENG:
                                    if (_introMengDone)
                                        break;

                                    incomingSpirit->AI()->Talk(MENG_INTRO);
                                    _introMengDone = true;
                                    break;
                            }
                        }
                        break;
                    }
                    case ACTION_FIRST_FIGHT:
                    {
                        if (instance)
                        {
                            instance->SetBossState(DATA_SPIRIT_KINGS, IN_PROGRESS);
                            events.ScheduleEvent(EVENT_CHECK_WIPE, 2.5 * IN_MILLISECONDS);
                        }
                        break;
                    }
                    case ACTION_SPECIAL_DESPAWN_UNDYING_SHADOWS:
                    {
                        summons.DespawnEntry(NPC_UNDYING_SHADOW);
                        break;
                    }
                }
            }

            void JustDied(Unit* /*killer*/) override 
            {
                _JustDied();

                if (instance)
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PINNED_DOWN);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!fightInProgress)
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_WIPE:
                        {
                            if (instance && instance->IsWipe(45.0f, me))
                            {
                                for (auto&& itr : spiritKingsEntry)
                                    if (Creature* spirit = ObjectAccessor::GetCreature(*me, instance->GetGuidData(itr)))
                                        spirit->AI()->EnterEvadeMode();

                                instance->SetBossState(DATA_SPIRIT_KINGS, FAIL);
                                instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MADDENING_SHOUT);
                                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PINNED_DOWN);

                                Reset();
                                summons.DespawnAll();
                                events.Reset();
                            }
                            events.ScheduleEvent(EVENT_CHECK_WIPE, 1 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_BERSERK:
                            for (auto&& itr : spiritKingsEntry)
                                if (Creature* spirit = ObjectAccessor::GetCreature(*me, instance->GetGuidData(itr)))
                                    spirit->CastSpell(spirit, SPELL_BERSERK, true);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_spirit_kings_controllerAI(creature);
        }
};

// 60701 - Zian of the Endless Shadow
// 60708 - Meng the Demented
// 60709 - Qiang the Merciless
// 60710 - Subetai the Swift
class boss_spirit_kings : public CreatureScript
{
    public:
        boss_spirit_kings() : CreatureScript("boss_spirit_kings") { }

        struct boss_spirit_kingsAI : public ScriptedAI
        {
            boss_spirit_kingsAI(Creature* creature) : ScriptedAI(creature), summons(creature) { }

            EventMap afterEvents, events;
            InstanceScript* instance;
            SummonList summons;
            std::list<uint64> AchievPlayerGUIDs;

            bool vanquished;
            bool _introQiangDone;
            bool lowLife;
            bool aggroDone;
            bool InAchievement;

            uint8 shadowCount;
            uint8 maxShadowCount;
            ObjectGuid targetGuid;
            float targetAngle;

            void InitializeAI() override
            {
                me->setActive(true);
                instance = me->GetInstanceScript();
                me->CastSpell(me, SPELL_INACTIVE, true);
                me->CastSpell(me, SPELL_INACTIVE_STUN, true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                Reset();
            }

            void Reset() override
            {
                events.Reset();
                afterEvents.Reset();

                shadowCount = 0;
                maxShadowCount = 3;
                targetGuid = ObjectGuid::Empty;

                vanquished = false;
                _introQiangDone = false;
                aggroDone = false;
                lowLife = false;
                InAchievement = false;

                AchievPlayerGUIDs.clear();

                summons.DespawnAll();

                events.Reset();
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetSpeed(MOVE_RUN, 1.42f, true);
                me->SetSpeed(MOVE_WALK, 0.88f, true);

                switch (me->GetEntry())
                {
                    case NPC_QIANG:
                        if (instance && instance->GetData(DATA_SPIRIT_KINGS_STATE) < DONE)
                        {
                            instance->SetBossState(DATA_SPIRIT_KINGS, NOT_STARTED);
                            instance->SetData(DATA_SPIRIT_KINGS, NOT_STARTED);
                        }
                        else
                        {
                            me->GetMotionMaster()->MovePath(me->GetDBTableGUIDLow(), true);
                            me->AI()->DoAction(ACTION_FIRST_FIGHT);
                        }
                        break;
                    case NPC_SUBETAI:
                        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                        me->CastSpell(me, SPELL_INACTIVE, true);
                        me->CastSpell(me, SPELL_INACTIVE_STUN, true);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        break;
                    default:
                        me->CastSpell(me, SPELL_INACTIVE, true);
                        me->CastSpell(me, SPELL_INACTIVE_STUN, true);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        break;
                }

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            Creature* GetControler()
            {
                if (instance)
                    return instance->instance->GetCreature(instance->GetGuidData(NPC_SPIRIT_GUID_CONTROLLER));
                else
                    return NULL;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (me->IsWithinDistInMap(who, 50.0f, false) && !_introQiangDone && me->GetEntry() == NPC_QIANG)
                {
                    _introQiangDone = true;
                    Talk(QIANG_INTRO);
                }
                else if (me->IsWithinDistInMap(who, 20.0f, false) && !me->IsInCombat())
                {
                    if (me->CanStartAttack(who, false))
                        AttackStart(who);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            // For get this achievement raid should be full (10 or 25, not less) @ by wowhead
            void ReceiveEmote(Player* player, uint32 uiTextEmote) override
            {
                if (InAchievement)
                {
                    if (uiTextEmote == TEXT_EMOTE_DANCE && player->HasAura(SPELL_PILLAGE_AURA) && !HasPlayerDance(player->GetGUID()))
                        AchievPlayerGUIDs.push_back(player->GetGUID());

                    uint32 pSize = Is25ManRaid() ? 25 : 10;

                    if (AchievPlayerGUIDs.size() == pSize)
                        DoCast(me, SPELL_GETTING_HOT_IN_HERE);
                }
            }

            // Check and not add player`s that already had dance
            bool HasPlayerDance(uint64 pGuid)
            {
                for (auto&& pItr : AchievPlayerGUIDs)
                    if (pGuid == pItr)
                        return true;

                return false;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (!aggroDone)
                {
                    aggroDone = true;
                    if (instance)
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                    switch (me->GetEntry())
                    {
                        case NPC_ZIAN:
                            Talk(ZIAN_AGGRO);
                            afterEvents.ScheduleEvent(EVENT_UNDYING_SHADOWS, 30000);
                            events.ScheduleEvent(EVENT_SHADOW_BLAST, 6000);
                            events.ScheduleEvent(EVENT_CHARGED_SHADOWS, 12000);
                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_SHIELD_OF_DARKNESS, 50000);
                            SetEquipmentSlots(false, EQUIP_ZIAN_STAFF, 0, EQUIP_NO_CHANGE);
                            break;
                        case NPC_MENG:
                            Talk(MENG_AGGRO);
                            afterEvents.ScheduleEvent(EVENT_MADDENING_SHOUT, 25000);
                            events.ScheduleEvent(EVENT_CRAZED, 1000);
                            events.ScheduleEvent(EVENT_CRAZY_THOUGHT, 10000);
                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_DELIRIOUS, 50000);
                            me->RemoveAurasDueToSpell(SPELL_CRAZED);
                            me->RemoveAurasDueToSpell(SPELL_COWARDICE);
                            me->SetPowerType(POWER_RAGE);
                            me->SetPower(POWER_RAGE, 0);
                            break;
                        case NPC_QIANG:
                            if (Creature* controler = GetControler())
                                controler->AI()->DoAction(ACTION_FIRST_FIGHT);

                            Talk(QIANG_AGGRO);
                            afterEvents.ScheduleEvent(EVENT_FLANKING_MOGU, 25000);
                            events.ScheduleEvent(EVENT_MASSIVE_ATTACK, 5 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_ANNIHILATE, 10 * IN_MILLISECONDS);
                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_IMPERVIOUS_SHIELD, 50000);
                            if (_introQiangDone)
                                me->RemoveAurasDueToSpell(SPELL_ACTIVATION_VISUAL);
                            else
                                me->AddAura(SPELL_ACTIVATION_VISUAL, me);
                            SetEquipmentSlots(false, EQUIP_QIANG_POLEARM, 0, EQUIP_NO_CHANGE);
                            break;
                        case NPC_SUBETAI:
                            Talk(SUBETAI_AGGRO);
                            afterEvents.ScheduleEvent(EVENT_PILLAGE, 25 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_VOLLEY_1, 5 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_RAIN_OF_ARROWS, 15 * IN_MILLISECONDS);
                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_SLEIGHT_OF_HAND, 50000);
                            SetEquipmentSlots(false, EQUIP_SUBETAI_SWORD, EQUIP_SUBETAI_SWORD, EQUIP_SUBETAI_BOW);
                            break;
                    }
                    events.ScheduleEvent(EVENT_EVADE_CONTROLLER, 1000);
                }
                me->RemoveAurasDueToSpell(SPELL_ACTIVATION_VISUAL);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_ENTER_COMBAT:
                        if (!me->IsInCombat())
                            if (Player* victim = me->SelectNearestPlayerNotGM(50.0f))
                                AttackStart(victim);
                            // No Break
                    case ACTION_FIRST_FIGHT:
                        me->RemoveAurasDueToSpell(SPELL_INACTIVE);
                        me->RemoveAurasDueToSpell(SPELL_INACTIVE_STUN);
                        me->RemoveAurasDueToSpell(SPELL_ACTIVATION_VISUAL);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        me->SetFaction(14);
                        break;
                    case ACTION_PILLAGE_ACHIEVEMENT:
                        InAchievement = true;
                        events.ScheduleEvent(EVENT_GETTING_HOT_IN_HERE, 15 * IN_MILLISECONDS); // MaxDuration of Aura
                        break;
                }
            }

            void RegeneratePower(Powers /*power*/, int32& value)
            {
                // Any regeneration for Meng, except with Crazed or Cowardice
                if (me->GetEntry() == NPC_MENG)
                    value = 0;
            }

            void SpellHit(Unit* caster, const SpellInfo* spell) override
            {
                // Remove only on hit mass dispell or shattering throw
                if ((me->HasAura(SPELL_IMPERVIOUS_SHIELD) || me->HasAura(SPELL_SHIELD_OF_DARKNESS)) && (spell->Id == 32375 || spell->Id == 64382))
                {
                    me->RemoveAurasDueToSpell(SPELL_IMPERVIOUS_SHIELD);
                    me->RemoveAurasDueToSpell(SPELL_SHIELD_OF_DARKNESS);
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->HealthBelowPctDamaged(30, damage) && IsHeroic() && !lowLife)
                {
                    // Triggered once only
                    lowLife = true;
                    if (Creature* controler = GetControler())
                        controler->AI()->DoAction(ACTION_ACTIVATE_SPIRIT);
                }

                if (me->HealthBelowPctDamaged(5, damage) && !vanquished)
                {
                    vanquished = true;
                    damage = 0;

                    me->InterruptNonMeleeSpells(false, SPELL_ANNIHILATE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->AddAura(SPELL_INACTIVE, me);
                    me->SetReactState(REACT_PASSIVE);
                    me->GetThreatManager().resetAllAggro();
                    me->CombatStop();
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();

                    if (instance)
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    if (Creature* controler = GetControler())
                        controler->AI()->DoAction(ACTION_SPIRIT_KILLED);

                    // We reschedule only the vanquished spell
                    events.Reset();
                    afterEvents.Reset();

                    switch (me->GetEntry())
                    {
                        case NPC_QIANG:
                            Talk(QIANG_DEATH);
                            // He calls for Flanking Orders every 75 seconds while defeated
                            afterEvents.ScheduleEvent(EVENT_FLANKING_MOGU, 75000);
                            break;
                        case NPC_SUBETAI:
                            Talk(SUBETAI_DEATH);
                            afterEvents.ScheduleEvent(EVENT_PILLAGE, 30000);
                            break;
                        case NPC_ZIAN:
                            Talk(ZIAN_DEATH);
                            shadowCount = 0;

                            if (Creature* controler = GetControler())
                                controler->AI()->DoAction(ACTION_SPECIAL_DESPAWN_UNDYING_SHADOWS);

                            afterEvents.ScheduleEvent(EVENT_UNDYING_SHADOWS, 30000);
                            break;
                        case NPC_MENG:
                            if (me->HasAura(SPELL_CRAZED))
                                Talk(MENG_DEATH_CRAZED);
                            else
                                Talk(MENG_DEATH_COWARDICE);

                            afterEvents.ScheduleEvent(EVENT_MADDENING_SHOUT, 75000);

                            me->RemoveAurasDueToSpell(SPELL_CRAZED);
                            me->RemoveAurasDueToSpell(SPELL_COWARDICE);

                            break;
                    }
                }
                else if (me->HealthBelowPctDamaged(10, damage))
                {
                    if (Creature* controler = GetControler())
                        controler->AI()->DoAction(ACTION_SAY_INTRO_FOR_NEXT_SPIRIT_IF_ANY);
                }

                if (me->HasAura(SPELL_COWARDICE))
                {
                    int32 percentage = me->GetPower(POWER_RAGE) / 10;
                    int32 bp = int32(CalculatePct(damage, percentage));

                    bp /= 5;

                    me->CastCustomSpell(attacker, SPELL_COWARDICE_DOT, &bp, NULL, NULL, true);
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                if (!me->IsAlive())
                    return;

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    if (instance->GetBossState(DATA_SPIRIT_KINGS) != FAIL)
                        instance->SetBossState(DATA_SPIRIT_KINGS, FAIL);

                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MADDENING_SHOUT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PINNED_DOWN);
                }

                ScriptedAI::EnterEvadeMode();

                summons.DespawnAll();
                uint32 corpseDelay = me->GetCorpseDelay();
                uint32 respawnDelay = me->GetRespawnDelay();

                me->SetCorpseDelay(1);
                me->SetRespawnDelay(29);

                me->DespawnOrUnsummon();

                me->SetCorpseDelay(corpseDelay);
                me->SetRespawnDelay(respawnDelay);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                {
                    switch (me->GetEntry())
                    {
                        case NPC_ZIAN:
                            Talk(ZIAN_SLAY);
                            break;
                        case NPC_MENG:
                            Talk(MENG_SLAY);
                            break;
                        case NPC_QIANG:
                            Talk(QIANG_SLAY);
                            break;
                        case NPC_SUBETAI:
                            Talk(SUBETAI_SLAY);
                            break;
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                while (uint32 eventId = afterEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FLANKING_MOGU:
                        {
                            if (Creature* controler = GetControler())
                            {
                                me->CastSpell(me, SPELL_FLANKING_ORDERS, false);
                                
                                // In heroic could be 2 waves
                                uint8 count = IsHeroic() ? 2 : 1;

                                for (uint8 i = 0; i < count; ++i)
                                    controler->AI()->DoAction(ACTION_FLANKING_MOGU);

                                Talk(QIANG_FLANKING_ORDERS_ANN);
                                Talk(QIANG_SPELL);
                            }

                            if (vanquished)
                                afterEvents.ScheduleEvent(EVENT_FLANKING_MOGU, 75000);
                            else
                                afterEvents.ScheduleEvent(EVENT_FLANKING_MOGU, 40000);

                            break;
                        }
                        case EVENT_PILLAGE:
                        {
                            Talk(SUBRTAI_PILLAGE_ANN);
                            Talk(SUBETAI_SPELL);

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, PillagePredicate(me)))
                                me->CastSpell(target, SPELL_PILLAGE_CHARGE, false);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                                me->CastSpell(target, SPELL_PILLAGE_CHARGE, false);

                            afterEvents.ScheduleEvent(EVENT_PILLAGE, 37500);
                            break;
                        }
                        case EVENT_MADDENING_SHOUT:
                        {
                            me->CastSpell(me, SPELL_MADDENING_SHOUT, false);
                            Talk(MENG_SPELL);
                            Talk(MENG_MADDENING_SHOUT_ANN);
                            afterEvents.ScheduleEvent(EVENT_MADDENING_SHOUT, 47000);
                            break;
                        }
                        case EVENT_UNDYING_SHADOWS:
                        {
                            // While in Cowardice personality, Zian is Feared.
                            if (shadowCount < maxShadowCount) // Max 3 undying shadow during the fight
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                                {
                                    if (Creature* controler = GetControler())
                                        controler->CastSpell(target, SPELL_UNDYING_SHADOWS, false);

                                    shadowCount++;
                                    Talk(ZIAN_UNDYING_SHADOWS_ANN);
                                    Talk(ZIAN_SPELL);
                                }
                            }

                            afterEvents.ScheduleEvent(EVENT_UNDYING_SHADOWS, 45000);
                            break;
                        }
                    }
                }

                afterEvents.Update(diff);
                events.Update(diff);

                if (!vanquished)
                {
                    if (!UpdateVictim())
                        return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        // Qiang
                        case EVENT_MASSIVE_ATTACK:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_MASSIVE_ATTACKS, false);

                            events.ScheduleEvent(EVENT_MASSIVE_ATTACK, 5 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_ANNIHILATE:
                        {
                            if (Unit* victim = me->GetVictim())
                            {
                                targetGuid = victim->GetGUID();
                                me->PrepareChanneledCast(me->GetAngle(victim), SPELL_ANNIHILATE);
                            }

                            events.ScheduleEvent(EVENT_ANNIHILATE_CANCEL, 3500);
                            events.ScheduleEvent(EVENT_ANNIHILATE, urand(15000, 20000));
                            events.RescheduleEvent(EVENT_MASSIVE_ATTACK, 8500);
                            break;
                        }
                        case EVENT_ANNIHILATE_CANCEL:
                        {
                            me->RemoveChanneledCast(targetGuid);
                            break;
                        }
                        case EVENT_IMPERVIOUS_SHIELD:
                        {
                            DoCast(me, SPELL_IMPERVIOUS_SHIELD);

                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_IMPERVIOUS_SHIELD, urand(45000, 50000));
                            break;
                        }
                        // Subetai
                        case EVENT_VOLLEY_1:
                        case EVENT_VOLLEY_2:
                        case EVENT_VOLLEY_3:
                        {
                            if (eventId == EVENT_VOLLEY_1)
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                {
                                    targetAngle = me->GetAngle(target);
                                    targetGuid = target->GetGUID();
                                }
                                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, DpsSpecTargetSelector()))
                                {
                                    targetAngle = me->GetAngle(target);
                                    targetGuid = target->GetGUID();
                                }
                                else if (Unit* target = me->GetVictim())
                                {
                                    targetAngle = me->GetAngle(target);
                                    targetGuid = target->GetGUID();
                                }

                                events.ScheduleEvent(EVENT_VOLLEY_CANCEL, 3200);
                            }

                            me->PrepareChanneledCast(targetAngle, volleySpells[eventId - EVENT_VOLLEY_1]);
                            events.ScheduleEvent(eventId == EVENT_VOLLEY_3 ? EVENT_VOLLEY_1 : eventId + 1, eventId == EVENT_VOLLEY_3 ? 40 * IN_MILLISECONDS : 50);
                            break;
                        }
                        case EVENT_VOLLEY_CANCEL:
                        {
                            me->RemoveChanneledCast(targetGuid);
                            break;
                        }
                        case EVENT_RAIN_OF_ARROWS:
                        {
                            if (hasEnoughCastersInRange())
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                    DoCast(target, SPELL_RAIN_OF_ARROWS);
                                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                    DoCast(target, SPELL_RAIN_OF_ARROWS);
                            }
                            else
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, MeeleSpecTargetSelector()))
                                    DoCast(target, SPELL_RAIN_OF_ARROWS);
                                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                    DoCast(target, SPELL_RAIN_OF_ARROWS);
                            }

                            events.ScheduleEvent(EVENT_RAIN_OF_ARROWS, 45000);
                            break;
                        }
                        case EVENT_SLEIGHT_OF_HAND:
                        {
                            DoCast(me, SPELL_SLEIGHT_OF_HAND);

                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_SLEIGHT_OF_HAND, urand(45000, 55000));
                            break;
                        }
                        // Zian
                        case EVENT_SHADOW_BLAST:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM,  0, 50.0f, true))
                                me->CastSpell(target, SPELL_SHADOW_BLAST, false);

                            events.ScheduleEvent(EVENT_SHADOW_BLAST, urand(5000, 6000));
                            break;
                        }
                        case EVENT_CHARGED_SHADOWS:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM,  0, 50.0f, true))
                                me->CastSpell(target, SPELL_CHARGED_SHADOWS, false);

                            events.ScheduleEvent(EVENT_CHARGED_SHADOWS, 12000);
                            break;
                        }
                        case EVENT_SHIELD_OF_DARKNESS:
                        {
                            DoCast(me, SPELL_SHIELD_OF_DARKNESS);

                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_SHIELD_OF_DARKNESS, urand(45000, 55000));
                            break;
                        }
                        // Meng
                        case EVENT_CRAZED:
                        {
                            me->CastSpell(me, SPELL_CRAZED, false);
                            break;
                        }
                        case EVENT_CRAZY_THOUGHT:
                        {
                            me->CastSpell(me, SPELL_CRAZY_THOUGHT, false);
                            events.ScheduleEvent(EVENT_CRAZY_THOUGHT, 15000);
                            break;
                        }
                        case EVENT_DELIRIOUS:
                        {
                            me->CastSpell(me, SPELL_DELIRIOUS, true);
                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_DELIRIOUS, urand(45000, 55000));
                            break;
                        }
                        case EVENT_GETTING_HOT_IN_HERE:
                            InAchievement = false;
                            AchievPlayerGUIDs.clear();
                            break;
                        case EVENT_EVADE_CONTROLLER:
                            if (Creature* controller = GetControler())
                                if (me->GetExactDist2d(controller) > 45.0f)
                                    EnterEvadeMode();

                            events.ScheduleEvent(EVENT_EVADE_CONTROLLER, 1000);
                            break;
                    }
                }

                if (me->GetEntry() == NPC_QIANG)
                    return;

                if (!vanquished)
                    DoMeleeAttackIfReady();
            }

            private:
                struct PillagePredicate 
                {
                    public:
                        PillagePredicate(Creature const* me) : _me(me) { }
                        bool operator()(Unit const* target) const
                        {
                            return target && target->GetTypeId() == TYPEID_PLAYER && target->GetExactDist2d(_me) > 20.5f;
                        }

                    private:
                        Creature const* _me;
                };

                bool hasEnoughCastersInRange()
                {
                    std::list<Player*> pCasterList;
                    GetPlayerListInGrid(pCasterList, me, 200.0f);
                    pCasterList.remove_if(MeeleSpecTargetSelector());

                    uint32 modeCount = Is25ManRaid() ? 7 : 3;

                    return pCasterList.size() > modeCount;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_spirit_kingsAI(creature);
        }
};

// 60958 - Pinning Arrow
class npc_pinning_arrow : public CreatureScript
{
    public:
        npc_pinning_arrow() : CreatureScript("npc_pinning_arrow") { }

        struct npc_pinning_arrowAI : public ScriptedAI
        {
            npc_pinning_arrowAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            ObjectGuid playerGuid;

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                playerGuid = ObjectGuid::Empty;
            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                DoZoneInCombat(me, 150.0f);
                playerGuid = guid;

                if (Player* player = ObjectAccessor::FindPlayer(playerGuid))
                {
                    me->EnterVehicle(player);
                    me->AddAura(SPELL_PINNING_ARROW_VISUAL, me);
                }
            }

            void KilledUnit(Unit* victim) override
            {
                me->DespawnOrUnsummon();
                victim->RemoveAurasDueToSpell(SPELL_PINNED_DOWN_DOT);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Player* player = ObjectAccessor::FindPlayer(playerGuid))
                {
                    player->RemoveAurasDueToSpell(SPELL_PINNED_DOWN_DOT);
                    player->RemoveAurasDueToSpell(SPELL_PINNED_DOWN);
                }

                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (me->HasUnitState(UNIT_STATE_UNATTACKABLE))
                    me->ClearUnitState(UNIT_STATE_UNATTACKABLE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_pinning_arrowAI(creature);
        }
};

#define PHASE_UNDYING_SHADOW    true
#define PHASE_COALESCING_SHADOW false

// 60731 - Undying Shadows
class npc_undying_shadow : public CreatureScript
{
    public:
        npc_undying_shadow() : CreatureScript("npc_undying_shadow") { }

        struct npc_undying_shadowAI : public ScriptedAI
        {
            npc_undying_shadowAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            bool phase;
            uint32 switchPhaseTimer;
            float scale;
            ObjectGuid targetGuid;

            void Reset() override
            {
                me->CastSpell(me, SPELL_UNDYING_SHADOW_DOT, true);
                DoZoneInCombat();

                targetGuid = ObjectGuid::Empty;

                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                {
                    me->CastSpell(target, SPELL_FIXATE, true);
                    me->GetMotionMaster()->MoveChase(target);
                    targetGuid = target->GetGUID();
                }
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                {
                    me->CastSpell(target, SPELL_FIXATE, true);
                    me->GetMotionMaster()->MoveChase(target);
                    targetGuid = target->GetGUID();
                }
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                {
                    me->CastSpell(target, SPELL_FIXATE, true);
                    me->GetMotionMaster()->MoveChase(target);
                    targetGuid = target->GetGUID();
                }

                switchPhaseTimer = 0;
                scale = me->GetFloatValue(OBJECT_FIELD_SCALE);

                phase = PHASE_UNDYING_SHADOW;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (phase == PHASE_UNDYING_SHADOW)
                {
                    if (damage >= me->GetHealth())
                    {
                        if (Unit* target = ObjectAccessor::FindPlayer(targetGuid))
                            target->RemoveAurasDueToSpell(SPELL_FIXATE);

                        me->StopMoving();
                        me->PrepareChanneledCast(me->GetOrientation());
                        me->RemoveAurasDueToSpell(SPELL_UNDYING_SHADOW_DOT);
                        me->AddAura(SPELL_COALESCING_SHADOW_DOT, me);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        phase = PHASE_COALESCING_SHADOW;
                        switchPhaseTimer = 60000;
                        damage = 0;
                        targetGuid = ObjectGuid::Empty;
                    }
                }
                else
                    damage = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (switchPhaseTimer)
                {
                    if (switchPhaseTimer <= diff)
                    {
                        me->RemoveAurasDueToSpell(SPELL_COALESCING_SHADOW_DOT);
                        me->AddAura(SPELL_UNDYING_SHADOW_DOT, me);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        phase = PHASE_UNDYING_SHADOW;
                        switchPhaseTimer = 0;
                        DoZoneInCombat();
                        me->SetHealth(me->GetMaxHealth());
                        me->SetObjectScale(scale);
                        me->SetReactState(REACT_AGGRESSIVE);

                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                        {
                            me->CastSpell(target, SPELL_FIXATE, true);
                            targetGuid = target->GetGUID();
                            me->RemoveChanneledCast(targetGuid);
                        }
                        else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                        {
                            me->CastSpell(target, SPELL_FIXATE, true);
                            targetGuid = target->GetGUID();
                            me->RemoveChanneledCast(targetGuid);
                        }
                        else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        {
                            me->CastSpell(target, SPELL_FIXATE, true);
                            targetGuid = target->GetGUID();
                            me->RemoveChanneledCast(targetGuid);
                        }
                    }
                    else
                    {
                        uint32 pct = uint32(float(switchPhaseTimer) / 30000.0f * 100.0f);
                        me->SetObjectScale(CalculatePct(scale, pct));

                        if (Unit* target = ObjectAccessor::FindPlayer(targetGuid))
                            target->RemoveAurasDueToSpell(SPELL_FIXATE);

                        switchPhaseTimer -= diff;
                    }
                }
                else if (targetGuid)
                {
                    if (Unit* target = ObjectAccessor::FindPlayer(targetGuid))
                        me->GetMotionMaster()->MoveChase(target);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_undying_shadowAI(creature);
        }
};

// Massive Attack - 117921
class spell_massive_attacks : public SpellScriptLoader
{
    public:
        spell_massive_attacks() : SpellScriptLoader("spell_massive_attacks") { }

        class spell_massive_attacks_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_massive_attacks_SpellScript);

            uint8 targetsCount;

            bool Load() override
            {
                targetsCount = 1;
                return true;
            }

            void CheckTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if([&](WorldObject* t) { return !t->ToPlayer() || t->ToPlayer()->IsGameMaster(); });
                targetsCount = targets.size();
            }

            void RecalculateDamage(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster())
                    return;
                Difficulty diff = GetCaster()->GetInstanceScript()->instance->GetDifficulty();

                int32 dmg = GetHitDamage();
                switch (diff)
                {
                    case RAID_DIFFICULTY_10MAN_NORMAL:
                        dmg = 800000;
                        break;
                    case RAID_DIFFICULTY_10MAN_HEROIC:
                        dmg = 1100000;
                        break;
                    case RAID_DIFFICULTY_25MAN_NORMAL:
                        dmg = 2000000;
                        break;
                    case RAID_DIFFICULTY_25MAN_HEROIC:
                        dmg = 2800000;
                        break;
                    case RAID_DIFFICULTY_25MAN_LFR:
                        dmg = 1300000;
                        break;
                }
                dmg /= targetsCount;
                SetHitDamage(dmg);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_massive_attacks_SpellScript::CheckTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_WITH_OFFSET);
                OnEffectHitTarget += SpellEffectFn(spell_massive_attacks_SpellScript::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_massive_attacks_SpellScript();
        }
};

// Volley (1) - 118094, Volley (2) - 118105 and Volley (3) - 118106
class spell_volley : public SpellScriptLoader
{
    public:
        spell_volley() : SpellScriptLoader("spell_volley") { }

        class spell_volley_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_volley_SpellScript);

            void HandleDummyLaunch(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                float coneAngle = 0.0f;

                switch (GetSpellInfo()->Id)
                {
                    case SPELL_VOLLEY_1:
                        coneAngle = M_PI / (1.5f);
                        break;
                    case SPELL_VOLLEY_2:
                        coneAngle = M_PI / 4;
                        break;
                    case SPELL_VOLLEY_3:
                        coneAngle = M_PI / 6;
                        break;
                }

                float startAngle = caster->GetOrientation() - (coneAngle / 2);
                float maxAngle = caster->GetOrientation() + (coneAngle / 2);

                for (float actualAngle = startAngle; actualAngle <= maxAngle; actualAngle += 0.1f)
                {
                    float x = 0.0f, y = 0.0f;
                    GetPositionWithDistInOrientation(caster, 100.0f, actualAngle, x, y);

                    caster->CastSpell(x, y, caster->GetPositionZ(), SPELL_VOLLEY_VISUAL, true);
                }
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_volley_SpellScript::HandleDummyLaunch, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_volley_SpellScript();
        }
};

// Pinned Down - 118145
class spell_pinned_down : public SpellScript
{
    PrepareSpellScript(spell_pinned_down);

    void HandleAfterHit()
    {
        if (Unit* target = GetHitUnit())
            if (Creature* pinningArrow = target->SummonCreature(NPC_PINNING_ARROW, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN))
                pinningArrow->AI()->SetGUID(target->GetGUID());
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_pinned_down::HandleAfterHit);
    }
};

// Pinned Down - 118145
class spell_pinned_down_aura : public AuraScript
{
    PrepareAuraScript(spell_pinned_down_aura);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED | UNIT_FLAG_STUNNED);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED | UNIT_FLAG_STUNNED);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pinned_down_aura::OnApply, EFFECT_0, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_pinned_down_aura::OnRemove, EFFECT_0, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
    }
};

// Maddening Shout - 117708
class spell_maddening_shout : public SpellScriptLoader
{
    public:
        spell_maddening_shout() : SpellScriptLoader("spell_maddening_shout") { }

        class spell_maddening_shout_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_maddening_shout_AuraScript);

            void OnAbsorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& /*absorbAmount*/)
            {
                Unit* attacker = dmgInfo.GetAttacker();
                if (!attacker || attacker == GetTarget() || !attacker->GetCharmerOrOwnerPlayerOrPlayerItself())
                    PreventDefaultAction();
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* owner = GetOwner()->ToPlayer())
                    owner->SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_UNK1);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetOwner()->ToPlayer())
                {
                    owner->RemoveByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_UNK1);

                    // Stop attack only if our target has normal faction
                    if (Unit* target = ObjectAccessor::GetUnit(*owner, owner->GetTarget()))
                        if (target->HasAura(SPELL_MADDENING_SHOUT))
                            return;

                    owner->AttackStop();
                }
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_maddening_shout_AuraScript::OnAbsorb, EFFECT_0);
                OnEffectApply  += AuraEffectApplyFn(spell_maddening_shout_AuraScript::OnApply, EFFECT_1, SPELL_AURA_MOD_FACTION, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_maddening_shout_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_MOD_FACTION, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_maddening_shout_AuraScript();
        }
};

// Cowardice - 117756 and Crazed - 117737
class spell_crazed_cowardice : public SpellScriptLoader
{
    public:
        spell_crazed_cowardice() : SpellScriptLoader("spell_crazed_cowardice") { }

        class spell_crazed_cowardice_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_crazed_cowardice_AuraScript);

            void HandlePeriodic(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();

                if (Unit* caster = GetCaster())
                {
                    if (auto const aura = GetAura())
                    {
                        caster->EnergizeBySpell(caster, aura->GetId(), 10, POWER_RAGE);

                        if (aura->GetEffect(1))
                        {
                            if (aura->GetId() == SPELL_CRAZED)
                                aura->GetEffect(1)->ChangeAmount(caster->GetPower(POWER_RAGE) / 5);
                            else
                                aura->GetEffect(1)->ChangeAmount(caster->GetPower(POWER_RAGE) / 10);

                            aura->SetNeedClientUpdateForTargets();
                        }

                        if (caster->GetPower(POWER_RAGE) >= 1000)
                        {
                            caster->SetPower(POWER_RAGE, 0);
                            caster->CastSpell(caster, aura->GetId() == SPELL_CRAZED ? SPELL_COWARDICE : SPELL_CRAZED, true);
                            aura->Remove();
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_crazed_cowardice_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_crazed_cowardice_AuraScript();
        }

        class spell_crazed_cowardice_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_crazed_cowardice_SpellScript);

            float runSpeed;
            float walkSpeed;

            bool Load() override
            {
                // Values from DB
                runSpeed = 1.42857f;
                walkSpeed = 0.888888f;

                return GetCaster();
            }

            void HandleOnHit()
            {
                if (Unit* caster = GetCaster())
                {
                    if (GetSpellInfo()->Id == SPELL_COWARDICE)
                    {
                        caster->SetSpeed(MOVE_RUN, 0.5f, true);
                        caster->SetSpeed(MOVE_WALK, 0.5f, true);
                        caster->GetMotionMaster()->MoveFleeing(caster);
                    }
                    else
                    {
                        if (caster->GetMotionMaster()->GetCurrentMovementGeneratorType() == FLEEING_MOTION_TYPE)
                            caster->GetMotionMaster()->MovementExpired();

                        caster->SetSpeed(MOVE_RUN, runSpeed, true);
                        caster->SetSpeed(MOVE_WALK, walkSpeed, true);

                        if (Unit* target = caster->GetAI()->SelectTarget(SELECT_TARGET_TOPAGGRO))
                        {
                            caster->SetTarget(target->GetGUID());
                            caster->GetAI()->AttackStart(target);
                        }
                    }
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_crazed_cowardice_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_crazed_cowardice_SpellScript();
        }
};

// Crazy Thought - 117833
class spell_crazy_thought : public SpellScriptLoader
{
    public:
        spell_crazy_thought() : SpellScriptLoader("spell_crazy_thought") { }

        class spell_crazy_thought_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_crazy_thought_SpellScript);

            void HandleEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    caster->EnergizeBySpell(caster, GetSpellInfo()->Id, 100, POWER_RAGE);

                    if (auto const cowardice = caster->GetAura(SPELL_COWARDICE))
                    {
                        if (cowardice->GetEffect(1))
                        {
                            cowardice->GetEffect(1)->ChangeAmount(caster->GetPower(POWER_RAGE) / 10);
                            cowardice->SetNeedClientUpdateForTargets();
                        }
                    }
                    else if (auto const crazed = caster->GetAura(SPELL_CRAZED))
                    {
                        if (crazed->GetEffect(1))
                        {
                            crazed->GetEffect(1)->ChangeAmount(caster->GetPower(POWER_RAGE) / 5);
                            crazed->SetNeedClientUpdateForTargets();
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_crazy_thought_SpellScript::HandleEffect, EFFECT_0, SPELL_EFFECT_ENERGIZE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_crazy_thought_SpellScript();
        }
};

// Coalescing Shadow - 117558
class spell_coalescing_shadow : public SpellScriptLoader
{
    public:
        spell_coalescing_shadow() : SpellScriptLoader("spell_coalescing_shadow") { }

        class spell_coalescing_shadow_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_coalescing_shadow_SpellScript);

            void CorrectRange(std::list<WorldObject*>& targets)
            {
                targets.clear();

                float const maxDist = 10.0f * GetCaster()->GetFloatValue(OBJECT_FIELD_SCALE);

                Map::PlayerList const& players = GetCaster()->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    if (Player* player = itr->GetSource())
                        if (player->GetExactDist2d(GetCaster()->GetPositionX(), GetCaster()->GetPositionY()) < maxDist)
                            targets.push_back(player);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_coalescing_shadow_SpellScript::CorrectRange, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_coalescing_shadow_SpellScript();
        }
};

// Pillage Charge 118047
class spell_pillage_charge : public SpellScriptLoader
{
    public:
        spell_pillage_charge() : SpellScriptLoader("spell_pillage_charge") { }

        class spell_pillage_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pillage_charge_SpellScript);

            void HandleChargeEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                    caster->CastSpell(caster, SPELL_PILLAGE, false);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_pillage_charge_SpellScript::HandleChargeEffect, EFFECT_0, SPELL_EFFECT_CHARGE_DEST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pillage_charge_SpellScript();
        }
};

// Impervious Shield 117961
class spell_impervious_shield : public SpellScriptLoader
{
    public:
        spell_impervious_shield() : SpellScriptLoader("spell_impervious_shield") { }

        class spell_impervious_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_impervious_shield_AuraScript);

            void HandleProc(ProcEventInfo&)
            {
                if (Unit* caster = GetCaster())
                    caster->CastSpell(caster, SPELL_MASSIVE_ATTACKS, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_impervious_shield_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_impervious_shield_AuraScript();
        }
};

// Sleight of Hand 118162
class spell_sleight_of_hand : public AuraScript
{
    PrepareAuraScript(spell_sleight_of_hand);

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        if (Unit* caster = GetCaster())
            if (caster->HasAuraType(SPELL_AURA_MOD_STUN))
                PreventDefaultAction();
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, false);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sleight_of_hand::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        OnEffectApply += AuraEffectApplyFn(spell_sleight_of_hand::OnApply, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_sleight_of_hand::OnRemove, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Pillage 118049
class spell_pillage : public SpellScriptLoader
{
    public:
        spell_pillage() : SpellScriptLoader("spell_pillage") { }

        class spell_pillage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pillage_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* caster = GetCaster())
                    if (caster->ToCreature())
                        caster->ToCreature()->AI()->DoAction(ACTION_PILLAGE_ACHIEVEMENT);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_pillage_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pillage_SpellScript();
        }
};

// Shield of Darkness 117697
class spell_shield_of_darkness : public AuraScript
{
    PrepareAuraScript(spell_shield_of_darkness);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (InstanceScript* instance = owner->GetInstanceScript())
                SetCharges(instance->instance->Is25ManRaid() ? 10 : 5);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_shield_of_darkness::OnApply, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_boss_spirit_kings()
{
    new boss_spirit_kings_controller();
    new boss_spirit_kings();
    new npc_pinning_arrow();
    new npc_undying_shadow();
    new spell_massive_attacks();
    new spell_volley();
    new spell_script<spell_pinned_down>("spell_pinned_down");
    new aura_script<spell_pinned_down_aura>("spell_pinned_down_aura");
    new spell_maddening_shout();
    new spell_crazed_cowardice();
    new spell_crazy_thought();
    new spell_coalescing_shadow();
    new spell_pillage_charge();
    new spell_impervious_shield();
    new aura_script<spell_sleight_of_hand>("spell_sleight_of_hand");
    new spell_pillage();
    new aura_script<spell_shield_of_darkness>("spell_shield_of_darkness");
}
