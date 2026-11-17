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

#include"ScriptPCH.h"
#include"Spell.h"
#include"blackwing_descent.h"

//todo: реализовать reroure power, low health, корректный double attack
enum ScriptTexts
{
    // Chimaeron
    SAY_AGGRO             = 0,
    SAY_MASSACRE          = 1,
    SAY_OFFLINE           = 2,
    SAY_ONLINE            = 3,
    // Bile O Tron 800
    SAY_BILE_O_TRON_START = 0,
};

enum Spells
{
    // Chimaeron
    SPELL_BERSERK                   = 26662,
    SPELL_FAST_ASLEEP               = 82706,
    SPELL_DOUBLE_ATTACK_DUMMY       = 88826,
    SPELL_DOUBLE_ATTACK             = 82882,
    SPELL_FEUD                      = 88872,
    SPELL_BREAK                     = 82881,
    SPELL_MASSACRE                  = 82848,
    SPELL_CAUSTIC_SLIME_MISSILE     = 82913,
    SPELL_CAUSTIC_SLIME_TARGET      = 82871,
    SPELL_CAUSTIC_SLIME             = 82935,
    SPELL_MORTALITY                 = 82890,
    SPELL_MORTALITY_SELF            = 82934,

    // Bile O Tron 800
    SPELL_FINKLES_MIXTURE           = 82705,
    SPELL_REROUTE_POWER             = 88861,
    SPELL_SYSTEMS_FAILURE           = 88853,
    SPELL_FINKLES_MIXTURE_VISUAL    = 91106,

    SPELL_LOW_HEALTH                = 89084,

    SPELL_SHADOW_WHIP               = 91304,
    SPELL_MOCKING_SHADOWS           = 91307,
};

enum Events
{
    EVENT_CAUSTIC_SLIME     = 1,
    EVENT_BREAK             = 2,
    EVENT_MASSACRE          = 3,
    EVENT_FEUD              = 4,
    EVENT_ACTIVATE_BUFF     = 5,
    EVENT_FEUD_OFF          = 6,
    EVENT_BERSERK           = 7,
    EVENT_NEXT_WAYPOINT     = 8,
    EVENT_DOUBLE_ATTACK     = 9,
};

enum Actions
{
    ACTION_BILE_O_TRON_START   = 1,
    ACTION_BILE_O_TRON_ONLINE  = 2,
    ACTION_BILE_O_TRON_OFFLINE = 3,
    ACTION_BILE_O_TRON_RESET   = 4,
};

Position const bilePos[6] =
{
    {-135.79f,  15.56f,  73.16f, 4.64f},
    {-129.17f, -10.48f,  73.07f, 5.63f},
    {-106.18f, -18.53f,  72.79f, 1.55f},
    {-77.95f,   0.702f,  73.09f, 1.50f},
    {-77.46f,   31.038f, 73.17f, 4.48f},
    {-120.42f,  34.49f,  72.05f, 4.11f},
};

const Position chimaeronnefariusspawnPos = {-112.16f, 42.80f, 80.14f, 4.73f}; 

class boss_chimaeron : public CreatureScript
{
    public:
        boss_chimaeron() : CreatureScript("boss_chimaeron") { }

        struct boss_chimaeronAI : public BossAI
        {
            boss_chimaeronAI(Creature* creature) : BossAI(creature, DATA_CHIMAERON)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                DoCast(me, SPELL_FAST_ASLEEP);
            }

            uint8 massacrecount; 
            uint8 stage;
            uint8 uiVictims;
            bool bFeud;

            void Reset() override
            {
                _Reset();

                instance->SetData(DATA_BILE_O_TRON_800, 0);
                if (Creature* bileotron800 = Unit::GetCreature(*me, instance->GetGuidData(DATA_BILE_O_TRON_800)))
                    bileotron800->RemoveAurasDueToSpell(SPELL_FINKLES_MIXTURE);
                massacrecount = 0;
                stage = 0;
                uiVictims = 0;
                bFeud = false;

                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 10);
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 10);

                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false); 
                me->m_extraAttacks = 0;
                me->SetReactState(REACT_DEFENSIVE);
                events.Reset();
                summons.DespawnAll();
                me->RemoveAllAuras();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MORTALITY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BREAK);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }

                me->GetMap()->SetWorldState(WORLDSTATE_FULL_OF_SOUND_AND_FURY, 1);
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                if (Creature* pBileotron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_BILE_O_TRON_800)))
                    pBileotron->AI()->DoAction(ACTION_BILE_O_TRON_RESET);
                DoCast(SPELL_FAST_ASLEEP);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    if (++uiVictims > 2)
                        me->GetMap()->SetWorldState(WORLDSTATE_FULL_OF_SOUND_AND_FURY, 0);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_BERSERK, 7.5 * MINUTE * IN_MILLISECONDS);

                events.ScheduleEvent(EVENT_BREAK, 5000);
                events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 10000);
                events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 15000);
                events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 20000);
                events.ScheduleEvent(EVENT_MASSACRE, 25000);

                Talk(SAY_AGGRO);

                if (instance)
                {
                    instance->SetData(DATA_BILE_O_TRON_800, 1);

                    if (Creature* bileotron800 = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_BILE_O_TRON_800)))
                        bileotron800->AI()->DoCast(SPELL_FINKLES_MIXTURE);

                    instance->SetBossState(DATA_CHIMAERON, IN_PROGRESS);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }

                if (IsHeroic())
                    if (Creature* pNefarius = me->SummonCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, chimaeronnefariusspawnPos))
                        pNefarius->AI()->DoAction(ACTION_CHIMAERON_INTRO);

                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                me->RemoveAllAuras();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_BILE_O_TRON_800, 0);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MORTALITY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BREAK);

                    if (Creature* pBileotron800 = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_BILE_O_TRON_800)))
                    {
                        pBileotron800->RemoveAurasDueToSpell(SPELL_FINKLES_MIXTURE);
                        pBileotron800->AI()->DoAction(ACTION_BILE_O_TRON_RESET);
                    }
                }

                if (Creature* pNefarius = me->SummonCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f))
                    pNefarius->AI()->DoAction(ACTION_CHIMAERON_DEATH);
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_SHADOW_WHIP)
                {
                    me->InterruptNonMeleeSpells(false);
                    me->RemoveAurasDueToSpell(SPELL_FEUD);
                    events.ScheduleEvent(EVENT_BREAK, 5000);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HealthBelowPct(20) && stage == 0 && !bFeud)
                {
                    stage = 1;
                    me->InterruptNonMeleeSpells(false);
                    events.CancelEvent(EVENT_CAUSTIC_SLIME);
                    events.CancelEvent(EVENT_MASSACRE);
                    events.CancelEvent(EVENT_FEUD);
                    if (Creature* pNefarius = me->FindNearestCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, 200.0f))
                        pNefarius->AI()->DoAction(ACTION_CHIMAERON_LOW);
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                    DoCast(me, SPELL_MORTALITY);
                    DoCast(me, SPELL_MORTALITY_SELF);
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) && !bFeud)
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BREAK:
                        {
                            uint32 attTimer = me->getAttackTimer(BASE_ATTACK);
                            if (attTimer < 1000)
                            {
                                attTimer = 1000;
                                me->setAttackTimer(BASE_ATTACK, 1000);
                            }
                            DoCast(me->GetVictim(), SPELL_BREAK);
                            DoCast(me, SPELL_DOUBLE_ATTACK_DUMMY, true);
                            events.ScheduleEvent(EVENT_DOUBLE_ATTACK, attTimer);
                            events.ScheduleEvent(EVENT_BREAK, 15000);
                            break;
                        }
                        case EVENT_DOUBLE_ATTACK:
                            me->RemoveAurasDueToSpell(SPELL_DOUBLE_ATTACK_DUMMY);
                            DoCast(me, SPELL_DOUBLE_ATTACK, true);
                            break;
                        case EVENT_MASSACRE:
                            me->InterruptNonMeleeSpells(false);
                            events.CancelEvent(EVENT_BREAK);
                            me->RemoveAurasDueToSpell(SPELL_DOUBLE_ATTACK_DUMMY);
                            me->m_extraAttacks = 0;
                            Talk(SAY_MASSACRE);
                            DoCast(SPELL_MASSACRE);
                            massacrecount++;
                            if (massacrecount == (IsHeroic()? 2: 3))
                            {
                                massacrecount = 0;
                                events.ScheduleEvent(EVENT_FEUD, 5000);
                            }
                            else
                            {
                                events.ScheduleEvent(EVENT_BREAK, 10000);
                                events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 19000);
                                events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 24000);
                                events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 29000);
                                events.ScheduleEvent(EVENT_MASSACRE, 34000);
                            }
                            break;
                        case EVENT_FEUD:
                            bFeud = true;
                            Talk(SAY_OFFLINE);
                            instance->SetData(DATA_BILE_O_TRON_800, 0);
                            if (Creature* pBileotron800 = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_BILE_O_TRON_800)))
                                pBileotron800->AI()->DoAction(ACTION_BILE_O_TRON_OFFLINE);
                            DoCast(me, SPELL_FEUD);
                            if (IsHeroic())
                                if (Creature* pNefarius = me->FindNearestCreature(NPC_LORD_VICTOR_NEFARIUS_HEROIC, 200.0f))
                                    pNefarius->AI()->DoAction(ACTION_CHIMAERON_FEUD);
                            events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 15000);
                            events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 20000);
                            events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 25000);
                            events.ScheduleEvent(EVENT_FEUD_OFF, 26000);
                            break;
                        case EVENT_FEUD_OFF:
                            bFeud = false;
                            me->RemoveAurasDueToSpell(SPELL_FEUD);
                            massacrecount++;
                            Talk(SAY_ONLINE);
                            if (Creature* pBileotron800 = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_BILE_O_TRON_800)))
                                pBileotron800->AI()->DoAction(ACTION_BILE_O_TRON_ONLINE);
                            if (!me->HealthBelowPct(20))
                            {
                                DoCast(SPELL_MASSACRE);
                                events.ScheduleEvent(EVENT_BREAK, 10000);
                                events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 15000);
                                events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 20000);
                                events.ScheduleEvent(EVENT_CAUSTIC_SLIME, 25000);
                                events.ScheduleEvent(EVENT_MASSACRE, 34000);
                            }
                            break;
                        case EVENT_CAUSTIC_SLIME:
                        {
                            uint32 num = RAID_MODE(2, 5, 2, 5);
                            me->CastCustomSpell(SPELL_CAUSTIC_SLIME_TARGET, SPELLVALUE_MAX_TARGETS, num, me, true);
                            break;
                        }
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            break;
                    }
                }
                if (!me->HasAura(SPELL_FEUD))
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_chimaeronAI>(creature);
        }
};

#define GOSSIP_MENU_1 16565
#define GOSSIP_OPTION_1 "I suppose you'll be needing a key for this cage? Wait, don't tell me. The horrific gibbering monster behind me ate it, right?"
#define GOSSIP_OPTION_1_RU "Я полагаю, тебе нужен ключ от этой клетки? Погоди, ничего не говори. Невероятно ужасный монстр позади меня съел его, не так ли?"
#define GOSSIP_MENU_2 16593
#define GOSSIP_OPTION_2 "You were trapped, as I recall. This situation seems oddly similar."
#define GOSSIP_OPTION_2_RU "Ты оказался в ловушке, как я понимаю. Эта ситуация кажется подозрительно похожей."
#define GOSSIP_MENU_3 16594
#define GOSSIP_OPTION_3 "Gnomes in Lava Suits, for example."
#define GOSSIP_OPTION_3_RU "Гнома в лава-костюме, например."
#define GOSSIP_MENU_4 16595
#define GOSSIP_OPTION_4 "No, I, uh, haven't seen it. You were saying?"
#define GOSSIP_OPTION_4_RU "Нет, я, э-э, не видел его. Так что ты говорил?"
#define GOSSIP_MENU_5 16596
#define GOSSIP_OPTION_5 "Restrictions? What restrictions?"
#define GOSSIP_OPTION_5_RU "Условия? Что за условия?"
#define GOSSIP_MENU_6 16597

class npc_finkle_einhorn : public CreatureScript
{
    public:
        npc_finkle_einhorn() : CreatureScript("npc_finkle_einhorn") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            bool ru = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU;
          
            InstanceScript* instance = creature->GetInstanceScript();
            if (!instance)
                return false;
            if (instance->GetData(DATA_CHIMAERON) == IN_PROGRESS || instance->GetData(DATA_BILE_O_TRON_800) == 1)
                return false;
            player->ADD_GOSSIP_ITEM(0, ru ? GOSSIP_OPTION_1_RU : GOSSIP_OPTION_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(GOSSIP_MENU_1, creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            bool ru = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU;
        
            InstanceScript* instance = creature->GetInstanceScript();
            if (!instance)
                return false;
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF+1:
                    player->ADD_GOSSIP_ITEM(0, ru ? GOSSIP_OPTION_2_RU : GOSSIP_OPTION_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                    player->SEND_GOSSIP_MENU(GOSSIP_MENU_2, creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
                    player->ADD_GOSSIP_ITEM(0, ru ? GOSSIP_OPTION_3_RU : GOSSIP_OPTION_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                    player->SEND_GOSSIP_MENU(GOSSIP_MENU_3, creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+3:
                    player->ADD_GOSSIP_ITEM(0, ru ? GOSSIP_OPTION_4_RU : GOSSIP_OPTION_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                    player->SEND_GOSSIP_MENU(GOSSIP_MENU_4, creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+4:
                    player->ADD_GOSSIP_ITEM(0, ru ? GOSSIP_OPTION_5_RU : GOSSIP_OPTION_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                    player->SEND_GOSSIP_MENU(GOSSIP_MENU_5, creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+5:
                    player->SEND_GOSSIP_MENU(GOSSIP_MENU_6, creature->GetGUID());
                    instance->SetData(DATA_BILE_O_TRON_800, 1);
                    if (Creature* pBileotron = ObjectAccessor::GetCreature(*creature, instance->GetGuidData(DATA_BILE_O_TRON_800)))
                        pBileotron->AI()->DoAction(ACTION_BILE_O_TRON_START);
                    break;
            }
            return true;
        }
};

class npc_bile_o_tron_800 : public CreatureScript
{
    public:
        npc_bile_o_tron_800() : CreatureScript("npc_bile_o_tron_800") { }

        struct npc_bile_o_tron_800AI : public ScriptedAI
        {
            npc_bile_o_tron_800AI(Creature* creature) : ScriptedAI(creature)
            {
                creature->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;
            uint8 uiWaypoint;

            void Reset() override
            {
                events.Reset();
                uiWaypoint = 0;
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_BILE_O_TRON_START:
                        Talk(SAY_BILE_O_TRON_START);
                        DoCast(me, SPELL_FINKLES_MIXTURE_VISUAL, true);
                        me->GetMotionMaster()->MovePoint(1, bilePos[0]);
                        break;
                    case ACTION_BILE_O_TRON_ONLINE:
                        instance->SetData(DATA_BILE_O_TRON_800, 1);
                        DoCast(me, SPELL_FINKLES_MIXTURE, true);
                        break;
                    case ACTION_BILE_O_TRON_OFFLINE:
                        instance->SetData(DATA_BILE_O_TRON_800, 0);
                        me->RemoveAurasDueToSpell(SPELL_FINKLES_MIXTURE);
                        break;
                    case ACTION_BILE_O_TRON_RESET:
                        events.Reset();
                        me->RemoveAllAuras();
                        me->GetMotionMaster()->MoveTargetedHome();
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 /*pointId*/) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (uiWaypoint >= 5)
                    uiWaypoint = 0;
                else
                    uiWaypoint++;
                
                events.ScheduleEvent(EVENT_NEXT_WAYPOINT, 1000);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_NEXT_WAYPOINT:
                            DoCast(me, SPELL_FINKLES_MIXTURE_VISUAL, true);
                            me->GetMotionMaster()->MovePoint(1, bilePos[uiWaypoint]);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_bile_o_tron_800AI>(creature);
        }
};

class spell_chimaeron_caustic_slime_target : public SpellScriptLoader
{
    public:
        spell_chimaeron_caustic_slime_target() : SpellScriptLoader("spell_chimaeron_caustic_slime_target") { }


        class spell_chimaeron_caustic_slime_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_chimaeron_caustic_slime_target_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), SPELL_CAUSTIC_SLIME_MISSILE, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_chimaeron_caustic_slime_target_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_chimaeron_caustic_slime_target_SpellScript();
        }
};

class spell_chimaeron_finkles_mixture : public SpellScriptLoader
{
    public:
        spell_chimaeron_finkles_mixture() : SpellScriptLoader("spell_chimaeron_finkles_mixture") { }

        class spell_chimaeron_finkles_mixture_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_chimaeron_finkles_mixture_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, float& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
                if (!GetTarget())
                    return;

                uint32 curHealth = GetTarget()->GetHealth();

                if (curHealth >= 10000)
                {
                    absorbAmount = dmgInfo.GetDamage() - curHealth + 1;
                }
            }

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                if (!GetTarget())
                    return;

                if (GetTarget()->GetHealth() < 10000)
                    GetTarget()->CastSpell(GetTarget(), SPELL_LOW_HEALTH, true);
                else
                {
                    if (GetTarget()->HasAura(SPELL_LOW_HEALTH))
                        GetTarget()->RemoveAurasDueToSpell(SPELL_LOW_HEALTH);
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_chimaeron_finkles_mixture_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_chimaeron_finkles_mixture_AuraScript::Absorb, EFFECT_1);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_chimaeron_finkles_mixture_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_chimaeron_finkles_mixture_AuraScript();
        }
};

void AddSC_boss_chimaeron()
{
    new boss_chimaeron();
    new npc_finkle_einhorn();
    new npc_bile_o_tron_800();
    new spell_chimaeron_caustic_slime_target();
    new spell_chimaeron_finkles_mixture();
}
