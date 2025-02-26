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

#include "throne_of_thunder.h"
#include "CreatureTextMgr.h"

enum sSpells
{
    SPELL_FOCUSED_LIGHTNING_TARGET      = 139203,
    SPELL_FOCUSED_LIGHTNING_AOE         = 139209,
    SPELL_FOCUSED_LIGHTNING             = 139206,
    SPELL_FOCUSED_LIGHTNING_DAMAGE      = 139210,
    SPELL_FOCUSED_LIGHTNING_DETONATION  = 139211,
    SPELL_FROST_BOLT                    = 136952,
    SPELL_SPEAR_DISARM                  = 137066,
    SPELL_SPEAR_PULSE_AOE               = 137058,
    SPELL_BERSERKING                    = 137096,
    SPELL_THROW_SPEAR                   = 136986,
    SPELL_RETRIEVE_SPEAR_JUMP           = 137070,
    SPELL_RETRIEVE_SPEAR_STUN           = 137072,
    SPELL_SPEAR_SPIN                    = 137077,
    SPELL_STORM_WEAPON                  = 139319,
    SPELL_STUN                          = 35865,
    SPELL_STORM_ENERGY                  = 139559,
    SPELL_ERUPTION                      = 138655,
    SPELL_WOUNDING_STRIKE               = 140049,
    SPELL_FROST_BOLT_2                  = 139231,
    SPELL_STORM_WEAPON_2                = 139218,
    SPELL_FOCUSING_LIGHTNING_SUMM       = 139206,
    SPELL_DARK_WINDS                    = 139485, // Aura on players
    SPELL_DARK_WINDS_AT_1               = 139499,
    SPELL_DARK_WINDS_AT_2               = 139535, // long
    SPELL_DARK_WINDS_AT_3               = 140781, // weigh
    SPELL_INVISIBLE                     = 132243, // should be, but duration is wrong 139885
    SPELL_ANCIENT_VENOM                 = 139888,
    SPELL_TORMENT                       = 139550,
    SPELL_SPIRITFIRE_BEAM               = 139895,
    SPELL_CRUSH_ARMOR                   = 33661,
    SPELL_SPIRIT_LANTERN                = 139364,
    SPELL_SPIRIT_LIGHT                  = 139461,
    SPELL_CALL_FLAMES                   = 138607,
    SPELL_ERUPTION_2                    = 138652,
    SPELL_FIERY_CORE                    = 138610, // stuck till 5 for each success cast flames or eruption
    SPELL_MOLTEN_BARRAGE                = 138651, // at upon 5 stack
    SPELL_FROST_BULWARK                 = 138668,
    SPELL_GLACIAL_FREEZE_TOTEM          = 138690,
    SPELL_GLACIAL_FREEZE_EFF            = 138687, // using by totem
    SPELL_GLACIAL_FREEZE                = 138678,
    SPELL_VISIONS_OF_GRANDEUR           = 139205,
    SPELL_VISIONS_OF_DEMISE             = 140115,
    SPELL_MARK_OF_THE_PROPHET           = 140400,
    SPELL_SEAL_OF_THE_LOA               = 139212,
    SPELL_MARK_OF_THE_LOA               = 139213, // triggered from auto-attack
    SPELL_LIGHT_OF_THE_LOA              = 139228,
    SPELL_JUDGMENT_OF_THE_LOA           = 139223, // dmg depend of stacks 139213
    SPELL_SUMMON_SHADOWY_MINIONS        = 138730, // by Garajal
    SPELL_SHADOWY_MINION_COSMETIC       = 138719,
    SPELL_BELL_CHANNEL                  = 140579,
    SPELL_CONJURE_ELEMENTALS            = 140636,
    SPELL_CHOKING_SANDS                 = 138742,
    SPELL_SAND_BOLT                     = 138739,
    SPELL_CONJURE_ELEMENTALS_LEFT       = 140635,
    SPELL_CONJURE_ELEMENTALS_RIGHT      = 140634,
    SPELL_BERSERKER_FRENZY              = 138427,
    SPELL_BLOODLETTING                  = 138693,
    SPELL_ZANDALARI_WARCRY              = 134856,
    SPELL_CLEAVE                        = 140414,
    SPELL_STRENGTH_OF_THE_LOA           = 140422,
    SPELL_SHADOW_BOLT                   = 138966,
    SPELL_STORMCLOUD                    = 139900,
    SPELL_CHAIN_LIGHTNING               = 139903,
    SPELL_BRIDGE_VIS_S                  = 139853, // Boss visual
    SPELL_BRIDGE_VISUAL_STRIKE          = 139854,
    SPELL_TELEPORT_DEPTH                = 139852,  // Tortos tele spell
    SPELL_WATERSPOUT_BACK               = 139158,
    SPELL_WATERSPOUT_POOL_VISUAL        = 139149,
    SPELL_WATERSPOUT_TELEPORT           = 139165,
    SPELL_WATERSPOUT_BACK_TRIGGERED     = 139159,
    SPELL_WATERSPOUT_AURA               = 140809,
    SPELL_SLASHING_TALONS               = 136753,
    SPELL_SONIC_SCREECH                 = 136751,
    SPELL_SHALE_SHARDS                  = 140616,
    SPELL_FUNGI_SPORES                  = 140620,
    SPELL_GROW                          = 140626,
    SPELL_CHOCKING_MISTS                = 140682,
    SPELL_CORROSIVE_BREATH              = 140684,
    SPELL_SONIC_CALL                    = 140600,
    SPELL_CRYSTAL_BARBS                 = 140619,
    SPELL_CRUSH_ARMOR_2                 = 140618,
    SPELL_FUNGAL_EXPLOSION              = 140596,
    SPELL_FUNGAL_EXPLOSION_EFF          = 140598,
    SPELL_ETERNAK_PRISON                = 140629,
    SPELL_SIPHON_LIFE                   = 140630,
    SPELL_LIGHTNING_NOVA                = 140628,
    SPELL_WEB_SPRAY                     = 139498,
    SPELL_LUCIFERASE                    = 134470,
    SPELL_LUCIFERASE_EFF                = 134472,
    SPELL_DEVOURED                      = 134415,
    SPELL_OOZE_EXPLOSION                = 136841,
    SPELL_LEI_SHEN_GIFT                 = 138201,
    SPELL_CONDUCTIVE_SHIELD             = 140296,
    SPELL_ZAP                           = 140299,
    SPELL_LIGHTNING_BURST               = 138196,
    SPELL_LIGHTNING_STORM               = 138187,
    SPELL_LIGHTNING_STORM_EFF           = 138234,
    SPELL_TRIUMPHANT_RUSH               = 139803,
    SPELL_WILD_SMASH                    = 139797,
    SPELL_TRIUMPHANT_ROAR               = 139815,
    SPELL_LEAPING_RUSH                  = 121190,
    SPELL_CARNIVOUROUS_BITE             = 122962,
    SPELL_SIT                           = 139814,
    SPELL_LIGHTNING_LASH                = 139771,
    SPELL_COSMIC_STRIKE                 = 139772,
    SPELL_SIPHON_ESSENCE_VISUAL         = 139590,
    SPELL_LONG_RANGE_RITUAL_LIGHTNING   = 137994, // if target longer than 30y
    SPELL_SHORT_RANGE_RITUAL_LIGHTNING  = 137995, // if target less than 30y
    SPELL_SHADOW_NOVA                   = 137998,
    SPELL_FIRE_BLAST                    = 138099,
    SPELL_INTERRUPT_SHIELD              = 140021,
    SPELL_SHOCKWAVE                     = 139215,
    SPELL_MUCK_SPLIT                    = 139309,
    SPELL_FOUL_VENOM                    = 139310,
    SPELL_PUTRIFY                       = 139316,
    SPELL_PUTRIFY_PROC_EFF              = 139317,
    SPELL_FRENZIED_CONSUMPTION          = 139311,
    SPELL_INFECTED_BITE                 = 139314,
    SPELL_ROT                           = 139315,
    SPELL_GNAWED_UPON                   = 134668,
    SPELL_CHOKING_GAS_EFF               = 139470,
    SPELL_ABRASIVE_RADULA               = 134414,
    SPELL_COCOON                        = 112844,
    SPELL_CORPSE_WEB_SPAWN              = 134483,
    SPELL_DISPLACEMENT                  = 139110,
};

enum eEvents
{
    EVENT_ZERK = 1,
    EVENT_SPEAR_THROW,
    EVENT_RETRIEVE_SPEAR,
    EVENT_STUN,
    EVENT_STORM_ENERGY,
    EVENT_PULSE,
    EVENT_FIXATE_CHECK,
    EVENT_FROST_BOLT,
    EVENT_WOUNDING_STRIKE,
    EVENT_FOCUSING_LIGHTNING,
    EVENT_ANCIENT_VENOM,
    EVENT_TORMENT,
    EVENT_CRUSH_ARMOR,
    EVENT_SPIRITFIRE_BEAM,
    EVENT_SPIRIT_REACTION,
    EVENT_CALL_OF_FLAMES,
    EVENT_ERUPTION,
    EVENT_FROST_BULWARK,
    EVENT_GLACIAL_FREEZE_TOTEM,
    EVENT_VISIONS_OF_DEMISE,
    EVENT_MARK_OF_THE_PROPHET,
    EVENT_LIGHT_OF_THE_LOA,
    EVENT_JUDGMENT_OF_THE_LOA,
    EVENT_SAND_BOLT,
    EVENT_CHOKING_SANDS,
    EVENT_BLOODLETTING,
    EVENT_CLEAVE,
    EVENT_STRENGTH_OF_THE_LOA,
    EVENT_NEAR_TRIBES_ALIVE,
    EVENT_SHADOW_BOLT,
    EVENT_STORMCLOUD,
    EVENT_CHAIN_LIGHTNING,
    EVENT_DESTROY_BRIDGE,
    EVENT_LEI_SHEN_SEND_CINEMATIC,
    EVENT_LEI_SHEN_I_1,
    EVENT_LEI_SHEN_I_2,
    EVENT_LEI_SHEN_VISUAL_CAST,
    EVENT_LEI_SHEN_DESTROY_BRIDGE,
    EVENT_LEI_SHEN_TELEPORT_PLAYERS,
    EVENT_SLASHING_TALONS,
    EVENT_SONIC_SCREECH,
    EVENT_SHALE_SHARDS,
    EVENT_FUNGI_SPORES,
    EVENT_GROW,
    EVENT_CHOCKING_MISTS,
    EVENT_CORROSIVE_BREATH,
    EVENT_CRYSTAL_BARBS,
    EVENT_ETERNAL_PRISON,
    EVENT_SIPHON_LIFE,
    EVENT_LIGHTNING_NOVA,
    EVENT_WEB_SPRAY,
    EVENT_LUCIFERASE,
    EVENT_LEI_SHEN_GIFT,
    EVENT_CONDUCTIVE_SHIELD,
    EVENT_LIGHTNING_STORM,
    EVENT_LIGHTNING_BURST,
    EVENT_TRIUMPHANT_RUSH,
    EVENT_WILD_SMASH,
    EVENT_TRIUMPHANT_ROAR,
    EVENT_LEAPING_RUSH,
    EVENT_CARNIVOROUS_BITE,
    EVENT_LIGHTNING_LASH,
    EVENT_COSMIC_STRIKE,
    EVENT_RANGE_RITUAL_LIGHTNING,
    EVENT_SHADOW_NOVA,
    EVENT_FIRE_BLAST,
    EVENT_SHOCKWAVE,
    EVENT_MUCK_SPLIT,
    EVENT_FOUL_VENOM,
    EVENT_FRENZIED_CONSUMPTION,
    EVENT_INFECTED_BITE,
    EVENT_MOVE_RANDOM,
    EVENT_GNAWED_UPON,
};

enum eYells
{
    TALK_INTRO,
    TALK_SPECIAL_1,
    TALK_SPECIAL_2,
    TALK_SPECIAL_3,
    TALK_SPECIAL_4,
};

std::vector<uint32> trashTribes =
{
    NPC_GURUBASHI_BERSERKER,
    NPC_FARRAKI_SAND_CONJURER,
    NPC_DRAKKARI_FROST_WARDEN,
    NPC_ZANDALARI_HIGH_PRIEST,
    NPC_AMANISHI_FLAME_CHANTER
};

const std::map<uint32, uint32> invMatchYellType =
{
    { 554631, TALK_INTRO     },
    { 554634, TALK_SPECIAL_1 },
    { 554238, TALK_SPECIAL_2 },
    { 554246, TALK_SPECIAL_3 },
};

class npc_zandalari_spearshaper : public CreatureScript
{
    public:
        npc_zandalari_spearshaper() : CreatureScript("npc_zandalari_spearshaper") { }

        struct npc_zandalari_spearshaperAI : public ScriptedAI
        {
            npc_zandalari_spearshaperAI(Creature* creature) : ScriptedAI(creature), summons(me) 
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            }

            EventMap events;
            SummonList summons;

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
                m_triggerGuid = ObjectGuid::Empty;

                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            }

            ObjectGuid m_triggerGuid;

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                
                if (summon->GetEntry() == NPC_SPEAR)
                {
                    m_triggerGuid = summon->GetGUID();
                    summon->CastSpell(summon, SPELL_SPEAR_PULSE_AOE, true);
                }
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_ZERK, 6000 + rand() % 5000);
                events.ScheduleEvent(EVENT_SPEAR_THROW, 10000 + rand() % 7000);
            }

            void JumpToSpear()
            {
                Position pos;

                if (Creature* pSpear = ObjectAccessor::GetCreature(*me, m_triggerGuid))
                {
                    pos = pSpear->GetPosition();
                    me->GetMotionMaster()->Clear(false);
                    me->GetMotionMaster()->MoveJump(pos, 10.f, 20.f, 1948);

                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == 1948)
                {
                    DoCast(me, SPELL_RETRIEVE_SPEAR_STUN, true);

                    if (Creature* pSpear = ObjectAccessor::GetCreature(*me, m_triggerGuid))
                        pSpear->DespawnOrUnsummon();

                    me->RemoveAurasDueToSpell(SPELL_SPEAR_DISARM);

                    DoCast(me, SPELL_SPEAR_SPIN, true);
                    me->ClearUnitState(UNIT_STATE_CASTING);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_SPEAR_SPIN))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ZERK:
                            DoCast(SPELL_BERSERKING);
                            events.ScheduleEvent(EVENT_ZERK, 8000 + rand() % 5000);
                            break;
                        case EVENT_SPEAR_THROW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                DoCast(target, SPELL_THROW_SPEAR);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 44.0f, true))
                                DoCast(target, SPELL_THROW_SPEAR);

                            events.ScheduleEvent(EVENT_RETRIEVE_SPEAR, 5000);
                            events.ScheduleEvent(EVENT_SPEAR_THROW, 25000 + rand() % 8000);
                            break;
                        case EVENT_RETRIEVE_SPEAR:
                            JumpToSpear();
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_spearshaperAI(creature);
        }
};

class npc_zandalari_stormcaller : public CreatureScript
{
    public:
        npc_zandalari_stormcaller() : CreatureScript("npc_zandalari_stormcaller") { }

        struct npc_zandalari_stormcallerAI : public ScriptedAI
        {
            npc_zandalari_stormcallerAI(Creature* creature) : ScriptedAI(creature) 
            { 
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                if (me->GetEntry() != NPC_ZANDALARI_STORMCALLER)
                    me->UpdateEntry(NPC_ZANDALARI_STORMCALLER);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (me->GetEntry() == NPC_ZANDALARI_STORMCALLER)
                    DoCast(SPELL_STORM_WEAPON);

                events.ScheduleEvent(EVENT_STUN, 5000);
                events.ScheduleEvent(EVENT_STORM_ENERGY, 5000 + rand() % 4000);
            }

            void JustDied(Unit* killer) override
            {
                if (!me->GetDBTableGUIDLow())
                    return;

                if (InstanceScript* instance = me->GetInstanceScript())
                {
                    if (Creature* jinRokh = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_JINROKH)))
                        jinRokh->AI()->DoAction(ACTION_START_INTRO);

                    instance->HandleGameObject(instance->GetGuidData(GO_JIN_ROKH_PREDOOR), true, NULL);
                }
            }

            void DoStun()
            {
                std::list<HostileReference*> threatList = me->GetThreatManager().getThreatList();
                std::list<Unit*> spellTargets;

                for (auto&& ref : threatList)
                {
                    if (Unit* target = ObjectAccessor::GetUnit(*me, ref->getUnitGuid()))
                        spellTargets.push_back(target);
                }

                uint32 m_uiMaxTargets = me->GetMap()->Is25ManRaid() ? 4 : 1;

                if (!spellTargets.empty())
                {
                    uint32 m_size = spellTargets.size();

                    if (m_size > m_uiMaxTargets)
                        Trinity::Containers::RandomResizeList(spellTargets, m_uiMaxTargets);

                    for (auto&& unit : spellTargets)
                        unit->CastSpell(unit, SPELL_STUN, true, 0, 0, me->GetGUID());
                }          
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
                        case EVENT_STORM_ENERGY:
                            DoCast(SPELL_STORM_ENERGY);
                            events.ScheduleEvent(EVENT_STORM_ENERGY, 4000 + rand() % 2500);
                            break;
                        case EVENT_STUN:
                            DoStun();
                            events.ScheduleEvent(EVENT_STUN, 6000 + rand() % 3000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_stormcallerAI(creature);
        }
};

class npc_focused_lightning_trash : public CreatureScript
{
    public:
        npc_focused_lightning_trash() : CreatureScript("npc_focused_lightning_trash") { }

        struct npc_focused_lightningAI : public ScriptedAI
        {
            npc_focused_lightningAI(Creature* creature) : ScriptedAI(creature)
            {
                Initialize();
            }

            ObjectGuid m_targetGuid;
            EventMap m_mEvents;

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                m_targetGuid = guid;
            }

            ObjectGuid GetGUID(int32 /*type*/) const override
            {
                return m_targetGuid;
            }

            void Initialize()
            {
                m_mEvents.ScheduleEvent(EVENT_PULSE, 500);
                m_mEvents.ScheduleEvent(EVENT_FIXATE_CHECK, 1500);
                m_targetGuid = ObjectGuid::Empty;
                me->AddAura(SPELL_FOCUSED_LIGHTNING_VISUAL, me);

                DoCast(SPELL_FOCUSED_LIGHTNING_TARGET);
            }

            void GetFixatedPlayerOrGetNewIfNeeded()
            {
                std::list<Player*> players;
                GetPlayerListInGrid(players, me, 200.f);

                for (auto&& itr : players)
                {
                    if (itr->HasAura(SPELL_FOCUSED_LIGHTNING_FIXATE, me->GetGUID()))
                    {
                        m_targetGuid = itr->GetGUID();
                        return;
                    }
                }

                if (!players.empty())
                {
                    if (Player* player = Trinity::Containers::SelectRandomContainerElement(players))
                    {
                        DoCast(player, SPELL_FOCUSED_LIGHTNING_FIXATE, true);
                        m_targetGuid = player->GetGUID();
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                m_mEvents.Update(diff);

                while (uint32 eventId = m_mEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FIXATE_CHECK:
                            if (Unit* target = ObjectAccessor::GetPlayer(*me, m_targetGuid))
                                me->GetMotionMaster()->MoveFollow(target, 0.f, 0.f);
                            else if (Player* target = me->FindNearestPlayer(150.0f))
                            {
                                GetFixatedPlayerOrGetNewIfNeeded();
                                me->GetMotionMaster()->MoveFollow(target, 0.f, 0.f);
                            }
                        
                            m_mEvents.ScheduleEvent(EVENT_FIXATE_CHECK, 500);
                            break;
                        case EVENT_PULSE:
                            DoCast(me, SPELL_FOCUSED_LIGHTNING_AOE, true);
                            m_mEvents.ScheduleEvent(EVENT_PULSE, 500);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_focused_lightningAI(creature);
        }
};

// Zandalari Water-Binder 69455
class npc_zandalari_water_binder : public CreatureScript
{
    public:
        npc_zandalari_water_binder() : CreatureScript("npc_zandalari_water_binder") { }
    
        struct npc_zandalari_water_binderAI : public ScriptedAI
        {
            npc_zandalari_water_binderAI(Creature* creature) : ScriptedAI(creature) { }
    
            EventMap events;
    
            void Reset() override
            {
                events.Reset();
            }

            void AttackStart(Unit* target) override
            {
                if (!target)
                    return;

                if (me->Attack(target, false))
                    DoStartNoMovement(target);
            }

            void CastInterrupted(SpellInfo const* spell) override
            {
                events.RescheduleEvent(EVENT_FROST_BOLT, 6 * IN_MILLISECONDS);

                if (Unit* vict = me->GetVictim())
                    me->GetMotionMaster()->MoveChase(vict);
            }
    
            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_FROST_BOLT, 1000);
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
                    if (eventId == EVENT_FROST_BOLT)
                    {
                        me->StopMoving();

                        if (Unit* vict = me->GetVictim())
                        {
                            if (me->GetExactDist2d(vict) > 39.0f)
                                DoStartMovement(vict);
                            else
                                DoStartNoMovement(vict);
                        }

                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                            DoCast(target, SPELL_FROST_BOLT);
                        else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 44.0f, true))
                            DoCast(target, SPELL_FROST_BOLT);

                        events.ScheduleEvent(EVENT_FROST_BOLT, 1000);
                    }
                    break;
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_water_binderAI(creature);
        }
};

// Zandalari Blade Initiate 70230
class npc_zandalari_blade_initiate : public CreatureScript
{
    public:
        npc_zandalari_blade_initiate() : CreatureScript("npc_zandalari_blade_initiate") { }
    
        struct npc_zandalari_blade_initiateAI : public ScriptedAI
        {
            npc_zandalari_blade_initiateAI(Creature* creature) : ScriptedAI(creature) { }
    
            EventMap events;
    
            void Reset() override
            {
                events.Reset();
            }
    
            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_WOUNDING_STRIKE, urand(3.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
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
                    if (eventId == EVENT_WOUNDING_STRIKE)
                    {
                        if (Unit* target = me->GetVictim())
                            DoCast(target, SPELL_WOUNDING_STRIKE);

                        events.ScheduleEvent(EVENT_WOUNDING_STRIKE, urand(12 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                    }
                    break;
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_blade_initiateAI(creature);
        }
};

// Zandalari Stormcaller Trash 69390
class npc_zandalari_stormcaller_2 : public CreatureScript
{
    public:
        npc_zandalari_stormcaller_2() : CreatureScript("npc_zandalari_stormcaller_2") { }
    
        struct npc_zandalari_stormcaller_2AI : public ScriptedAI
        {
            npc_zandalari_stormcaller_2AI(Creature* creature) : ScriptedAI(creature) 
            { 
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            }
    
            EventMap events;
            bool hasTriggered;
    
            void Reset() override
            {
                events.Reset();
                hasTriggered = false;
            }
    
            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_FROST_BOLT, urand(3.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FOCUSING_LIGHTNING, 15 * IN_MILLISECONDS);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (HealthBelowPct(60) && !hasTriggered)
                {
                    hasTriggered = true;
                    DoCast(me, SPELL_STORM_WEAPON_2);
                }
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
                        case EVENT_FROST_BOLT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                DoCast(target, SPELL_FROST_BOLT);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 44.0f, true))
                                DoCast(target, SPELL_FROST_BOLT);

                            events.ScheduleEvent(EVENT_FROST_BOLT, 10.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_FOCUSING_LIGHTNING:
                            DoCast(me, SPELL_FOCUSING_LIGHTNING_SUMM);
                            events.ScheduleEvent(EVENT_FOCUSING_LIGHTNING, urand(12 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
                            break;
                    }
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_stormcaller_2AI(creature);
        }
};

// Ancient Python 70448
class npc_ancient_python : public CreatureScript
{
    public:
        npc_ancient_python() : CreatureScript("npc_ancient_python") { }
    
        struct npc_ancient_pythonAI : public ScriptedAI
        {
            npc_ancient_pythonAI(Creature* creature) : ScriptedAI(creature) { }
    
            EventMap events;
    
            void Reset() override
            {
                events.Reset();
                me->AddAura(SPELL_INVISIBLE, me);
            }
    
            void JustEngagedWith(Unit* who) override
            {
                me->RemoveAurasDueToSpell(SPELL_INVISIBLE);
                events.ScheduleEvent(EVENT_ANCIENT_VENOM, urand(3.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
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
                    if (eventId == EVENT_ANCIENT_VENOM)
                    {
                        if (Unit* target = me->GetVictim())
                            DoCast(target, SPELL_ANCIENT_VENOM);

                        events.ScheduleEvent(EVENT_ANCIENT_VENOM, urand(15 * IN_MILLISECONDS, 28 * IN_MILLISECONDS));
                    }
                    break;
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ancient_pythonAI(creature);
        }
};

// Tormented Spirit 70341
class npc_tormented_spirit : public CreatureScript
{
    public:
        npc_tormented_spirit() : CreatureScript("npc_tormented_spirit") { }
    
        struct npc_tormented_spiritAI : public ScriptedAI
        {
            npc_tormented_spiritAI(Creature* creature) : ScriptedAI(creature) { }
    
            EventMap events;
    
            void Reset() override
            {
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 35.0f);
                events.Reset();
            }
    
            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_TORMENT, 2.5 * IN_MILLISECONDS);
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
                    if (eventId == EVENT_TORMENT)
                    {
                        DoCast(me, SPELL_TORMENT);
                        events.ScheduleEvent(EVENT_TORMENT, 8 * IN_MILLISECONDS);
                    }
                    break;
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_tormented_spiritAI(creature);
        }
};

// Soul-Fed Construct 70308
class npc_soul_fed_construct : public CreatureScript
{
    public:
        npc_soul_fed_construct() : CreatureScript("npc_soul_fed_construct") { }
    
        struct npc_soul_fed_constructAI : public ScriptedAI
        {
            npc_soul_fed_constructAI(Creature* creature) : ScriptedAI(creature) 
            { 
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            }
    
            EventMap events;
    
            void Reset() override
            {
                events.Reset();
            }
    
            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_CRUSH_ARMOR, urand(2 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SPIRITFIRE_BEAM, 5 * IN_MILLISECONDS);
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
                        case EVENT_CRUSH_ARMOR:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_CRUSH_ARMOR);
                        
                            events.ScheduleEvent(EVENT_CRUSH_ARMOR, urand(12 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
                            break;
                        case EVENT_SPIRITFIRE_BEAM:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                DoCast(target, SPELL_SPIRITFIRE_BEAM);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                DoCast(target, SPELL_SPIRITFIRE_BEAM);

                            events.ScheduleEvent(EVENT_SPIRITFIRE_BEAM, 12 * IN_MILLISECONDS);
                            break;
                    }
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_soul_fed_constructAI(creature);
        }
};

// Spirit Flayer 70246
class npc_spirit_flayer : public CreatureScript
{
    public:
        npc_spirit_flayer() : CreatureScript("npc_spirit_flayer") { }
    
        struct npc_spirit_flayerAI : public ScriptedAI
        {
            npc_spirit_flayerAI(Creature* creature) : ScriptedAI(creature) { }

            bool sDelay;
            float x, y;
    
            void Reset() override
            {
                sDelay = false;
                DoCast(me, SPELL_SPIRIT_LANTERN);
            }
    
            void EnterEvadeMode() override
            {
                if (!_EnterEvadeMode())
                    return;
    
                Reset();
            }
    
            void UpdateAI(uint32 diff) override
            {
                if (!sDelay)
                {
                    if (Player* itr = me->FindNearestPlayer(2.5f))
                    {
                        sDelay = true;
                        DoCast(me, SPELL_SPIRIT_LIGHT, true);

                        me->m_Events.Schedule(2000, 1, [this]()
                        {
                            sDelay = false;
                        });
                    }
                }
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spirit_flayerAI(creature);
        }
};

// Stormbringer Drazkil 70445
class npc_stormbringer_drazkil : public CreatureScript
{
    public:
        npc_stormbringer_drazkil() : CreatureScript("npc_stormbringer_drazkil") { }
    
        struct npc_stormbringer_drazkilAI : public ScriptedAI
        {
            npc_stormbringer_drazkilAI(Creature* creature) : ScriptedAI(creature) 
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                instance = me->GetInstanceScript();
            }
    
            EventMap events;
            InstanceScript* instance;
    
            void Reset() override
            {
                events.Reset();
            }
    
            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(2 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_STORMCLOUD, 6 * IN_MILLISECONDS);
            }

            void JustDied(Unit* killer) override
            {
                if (instance)
                {
                    instance->SetData(DATA_PRE_HORIDON_EVENT, DONE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STORMCLOUD);
                }
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
                        case EVENT_STORMCLOUD:
                            DoCast(me, SPELL_STORMCLOUD);
                            events.ScheduleEvent(EVENT_STORMCLOUD, urand(15 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                            break;
                        case EVENT_CHAIN_LIGHTNING:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                DoCast(target, SPELL_CHAIN_LIGHTNING);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                DoCast(target, SPELL_CHAIN_LIGHTNING);
                        
                            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 10.5 * IN_MILLISECONDS);
                            break;
                    }
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_stormbringer_drazkilAI(creature);
        }
};

struct tribeAI : public ScriptedAI
{
    tribeAI(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    InstanceScript* instance;

    void InitializeAI() override
    {
        // Delay for sure that warlord was spawned
        me->m_Events.Schedule(1500, [this]()
        {
            if (Creature* warlord = me->FindNearestCreature(NPC_ZANDALARI_WARLORD, 25.0f, true))
                DoCast(me, SPELL_BELL_CHANNEL);
        });

        Reset();
    }

    void Reset() override
    {
        events.Reset();
    }

    ObjectGuid GetLowestFriendlyGUID()
    {
        std::list<Creature*> tmpTargets;

        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_FARRAKI_SAND_CONJURER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DRAKKARI_FROST_WARDEN, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_AMANISHI_FLAME_CHANTER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_ZANDALARI_WARLORD, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_ZANDALARI_HIGH_PRIEST, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_ZANDALARI_PROPHET, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_GURUBASHI_BERSERKER, 80.0f);

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
    }
};

// Amani`shi Flame Chanter 69909
class npc_amanishi_flame_chanter : public CreatureScript
{
    public:
        npc_amanishi_flame_chanter() : CreatureScript("npc_amanishi_flame_chanter") { }
    
        struct npc_amanishi_flame_chanterAI : public tribeAI
        {
            npc_amanishi_flame_chanterAI(Creature* creature) : tribeAI(creature) { }
    
            void AttackStart(Unit* target) override
            {
                if (!target)
                    return;
    
                if (me->Attack(target, false))
                    DoStartNoMovement(target);
            }
    
            void CastInterrupted(SpellInfo const* spell) override
            {
                events.RescheduleEvent(EVENT_CALL_OF_FLAMES, 6 * IN_MILLISECONDS);
                events.RescheduleEvent(EVENT_ERUPTION, 8.5 * IN_MILLISECONDS);
    
                if (Unit* vict = me->GetVictim())
                    me->GetMotionMaster()->MoveChase(vict);
            }
    
            void JustEngagedWith(Unit* who) override
            {
                me->InterruptNonMeleeSpells(true, SPELL_BELL_CHANNEL);
                events.ScheduleEvent(EVENT_CALL_OF_FLAMES, 1.5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_ERUPTION, 8.5 * IN_MILLISECONDS);
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
                        case EVENT_CALL_OF_FLAMES:
                            me->StopMoving();

                            if (Unit* vict = me->GetVictim())
                            {
                                if (me->GetExactDist2d(vict) > 45.0f)
                                    DoStartMovement(vict);
                                else
                                    DoStartNoMovement(vict);
                            }
                        
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(target, SPELL_CALL_FLAMES);
                        
                            events.ScheduleEvent(EVENT_CALL_OF_FLAMES, 2.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_ERUPTION:
                            me->StopMoving();

                            if (Unit* vict = me->GetVictim())
                            {
                                if (me->GetExactDist2d(vict) > 45.0f)
                                    DoStartMovement(vict);
                                else
                                    DoStartNoMovement(vict);
                            }
                        
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                DoCast(target, SPELL_ERUPTION_2);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(target, SPELL_ERUPTION_2);
                        
                            events.ScheduleEvent(EVENT_ERUPTION, urand(10.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                    }
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_amanishi_flame_chanterAI(creature);
        }
};

// Drakkari Frost Warden 69910
class npc_drakkari_frost_warden : public CreatureScript
{
    public:
        npc_drakkari_frost_warden() : CreatureScript("npc_drakkari_frost_warden") { }
    
        struct npc_drakkari_frost_wardenAI : public tribeAI
        {
            npc_drakkari_frost_wardenAI(Creature* creature) : tribeAI(creature) { }
    
            void JustEngagedWith(Unit* who) override
            {
                me->InterruptNonMeleeSpells(true, SPELL_BELL_CHANNEL);
                events.ScheduleEvent(EVENT_FROST_BULWARK, 7.5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_GLACIAL_FREEZE_TOTEM, 5 * IN_MILLISECONDS);
            }
    
            void CastInterrupted(SpellInfo const* spell) override
            {
                events.RescheduleEvent(EVENT_FROST_BULWARK, 8 * IN_MILLISECONDS);
            }

            void JustSummoned(Creature* summon) override
            {
                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                summon->AddAura(SPELL_GLACIAL_FREEZE, summon);
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
                        case EVENT_FROST_BULWARK:
                            if (Unit* target = ObjectAccessor::GetUnit(*me, GetLowestFriendlyGUID()))
                                DoCast(target, SPELL_FROST_BULWARK);

                            events.ScheduleEvent(EVENT_FROST_BULWARK, 15 * IN_MILLISECONDS);
                            break;
                        case EVENT_GLACIAL_FREEZE_TOTEM:
                            DoCast(me, SPELL_GLACIAL_FREEZE_TOTEM);
                            events.ScheduleEvent(EVENT_GLACIAL_FREEZE_TOTEM, urand(12 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
                            break;
                    }
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_drakkari_frost_wardenAI(creature);
        }
};

// Farraki Sand Conjurer 69899
class npc_farraki_sand_conjurer : public CreatureScript
{
    public:
        npc_farraki_sand_conjurer() : CreatureScript("npc_farraki_sand_conjurer") { }
    
        struct npc_farraki_sand_conjurerAI : public tribeAI
        {
            npc_farraki_sand_conjurerAI(Creature* creature) : tribeAI(creature), summons(me)  { }

            SummonList summons;
    
            void AttackStart(Unit* target) override
            {
                if (!target)
                    return;
    
                if (me->Attack(target, false))
                    DoStartNoMovement(target);
            }
    
            void CastInterrupted(SpellInfo const* spell) override
            {
                events.RescheduleEvent(EVENT_SAND_BOLT, 7 * IN_MILLISECONDS);
                events.RescheduleEvent(EVENT_CHOKING_SANDS, 8.5 * IN_MILLISECONDS);
    
                if (Unit* vict = me->GetVictim())
                    me->GetMotionMaster()->MoveChase(vict);
            }
    
            void JustEngagedWith(Unit* who) override
            {
                me->InterruptNonMeleeSpells(true, SPELL_BELL_CHANNEL);
                DoCast(me, SPELL_CONJURE_ELEMENTALS, true);
                events.RescheduleEvent(EVENT_SAND_BOLT, 1.5 * IN_MILLISECONDS);
                events.RescheduleEvent(EVENT_CHOKING_SANDS, 8.5 * IN_MILLISECONDS);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (Unit* target = me->GetVictim())
                {
                    summon->GetMotionMaster()->MoveChase(target);
                    summon->Attack(target, true);
                }
            }

            void EnterEvadeMode() override
            {
                summons.DespawnAll();
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
                        case EVENT_SAND_BOLT:
                            me->StopMoving();

                            if (Unit* vict = me->GetVictim())
                            {
                                if (me->GetExactDist2d(vict) > 45.0f)
                                    DoStartMovement(vict);
                                else
                                    DoStartNoMovement(vict);
                            }
                        
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(target, SPELL_SAND_BOLT);
                        
                            events.ScheduleEvent(EVENT_SAND_BOLT, 2.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_CHOKING_SANDS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                DoCast(target, SPELL_CHOKING_SANDS);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(target, SPELL_CHOKING_SANDS);
                        
                            events.ScheduleEvent(EVENT_CHOKING_SANDS, urand(10.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                    }
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_farraki_sand_conjurerAI(creature);
        }
};

// Zandalari High Priest 69906
class npc_zandalari_high_priest : public CreatureScript
{
    public:
        npc_zandalari_high_priest() : CreatureScript("npc_zandalari_high_priest") { }
    
        struct npc_zandalari_high_priestAI : public tribeAI
        {
            npc_zandalari_high_priestAI(Creature* creature) : tribeAI(creature) { }
    
            void JustEngagedWith(Unit* who) override
            {
                me->InterruptNonMeleeSpells(true, SPELL_BELL_CHANNEL);
                events.ScheduleEvent(EVENT_LIGHT_OF_THE_LOA, 7.5 * IN_MILLISECONDS);
            }

            void CastInterrupted(SpellInfo const* spell) override
            {
                events.RescheduleEvent(EVENT_LIGHT_OF_THE_LOA, 8 * IN_MILLISECONDS);
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
                    if (eventId == EVENT_LIGHT_OF_THE_LOA)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, GetLowestFriendlyGUID()))
                            DoCast(target, SPELL_LIGHT_OF_THE_LOA);

                        events.ScheduleEvent(EVENT_LIGHT_OF_THE_LOA, 10 * IN_MILLISECONDS);
                    }
                    break;
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_high_priestAI(creature);
        }
};

// Gurubashi Berserker 69905, 69916
class npc_gurubashi_berserker : public CreatureScript
{
    public:
        npc_gurubashi_berserker() : CreatureScript("npc_gurubashi_berserker") { }
    
        struct npc_gurubashi_berserkerAI : public ScriptedAI
        {
            npc_gurubashi_berserkerAI(Creature* creature) : ScriptedAI(creature) { }
    
            EventMap events;
            bool hasTriggered;
    
            void Reset() override
            {
                events.Reset();
                hasTriggered = false;
            }
    
            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_BLOODLETTING, urand(4.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
            }
    
            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (HealthBelowPct(60) && !hasTriggered)
                {
                    hasTriggered = true;
    
                    // Some Berserks have it like default state
                    if (!me->HasAura(SPELL_BERSERKER_FRENZY))
                        DoCast(me, SPELL_BERSERKER_FRENZY);
                }
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
                    if (eventId == EVENT_BLOODLETTING)
                    {
                        if (Unit* target = me->GetVictim())
                            DoCast(target, SPELL_BLOODLETTING);

                        events.ScheduleEvent(EVENT_BLOODLETTING, urand(8.5 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
                    }
                    break;
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_gurubashi_berserkerAI(creature);
        }
};

// Zandalari Prophet 70557
class npc_zandalari_prophet : public CreatureScript
{
    public:
        npc_zandalari_prophet() : CreatureScript("npc_zandalari_prophet") { }
    
        struct npc_zandalari_prophetAI : public ScriptedAI
        {
            npc_zandalari_prophetAI(Creature* creature) : ScriptedAI(creature) { }
    
            EventMap events;
            bool hasTriggered;
    
            void Reset() override
            {
                events.Reset();
                hasTriggered = false;
            }
    
            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_VISIONS_OF_DEMISE, urand(4.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_MARK_OF_THE_PROPHET, 10 * IN_MILLISECONDS);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (HealthBelowPct(60) && !hasTriggered && !me->HasUnitState(UNIT_STATE_CASTING))
                {
                    hasTriggered = true;
                    DoCast(me, SPELL_VISIONS_OF_GRANDEUR);
                }
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
                        case EVENT_VISIONS_OF_DEMISE:
                            DoCast(me, SPELL_VISIONS_OF_DEMISE);
                            events.ScheduleEvent(EVENT_VISIONS_OF_DEMISE, 15 * IN_MILLISECONDS);
                            break;
                        case EVENT_MARK_OF_THE_PROPHET:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector(SPELL_MARK_OF_THE_PROPHET)))
                                DoCast(target, SPELL_MARK_OF_THE_PROPHET);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector(SPELL_MARK_OF_THE_PROPHET)))
                                DoCast(target, SPELL_MARK_OF_THE_PROPHET);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true, -SPELL_MARK_OF_THE_PROPHET))
                                DoCast(target, SPELL_MARK_OF_THE_PROPHET);
                        
                            events.ScheduleEvent(EVENT_MARK_OF_THE_PROPHET, 10 * IN_MILLISECONDS);
                            break;
                    }
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_prophetAI(creature);
        }
};

// Zandalari Prelate 69927
class npc_zandalari_prelate : public CreatureScript
{
    public:
        npc_zandalari_prelate() : CreatureScript("npc_zandalari_prelate") { }
    
        struct npc_zandalari_prelateAI : public ScriptedAI
        {
            npc_zandalari_prelateAI(Creature* creature) : ScriptedAI(creature) 
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            }
    
            EventMap events;
    
            void Reset() override
            {
                events.Reset();
            }
    
            void JustEngagedWith(Unit* who) override
            {
                DoCast(me, SPELL_SEAL_OF_THE_LOA);
                events.ScheduleEvent(EVENT_LIGHT_OF_THE_LOA, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_JUDGMENT_OF_THE_LOA, urand(12 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
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
                        case EVENT_JUDGMENT_OF_THE_LOA:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_JUDGMENT_OF_THE_LOA);

                            events.ScheduleEvent(EVENT_JUDGMENT_OF_THE_LOA, urand(12 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                            break;
                        case EVENT_LIGHT_OF_THE_LOA:
                            if (Unit* target = ObjectAccessor::GetUnit(*me, GetLowestFriendlyGUID()))
                                DoCast(target, SPELL_LIGHT_OF_THE_LOA);

                            events.ScheduleEvent(EVENT_LIGHT_OF_THE_LOA, 15 * IN_MILLISECONDS);
                            break;
                    }
                }
    
                DoMeleeAttackIfReady();
            }

            private:
                ObjectGuid GetLowestFriendlyGUID()
                {
                    std::list<Creature*> tmpTargets;

                    GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_ZANDALARI_PRELATE, 80.0f);
                    GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_ZANDALARI_PROPHET, 80.0f);

                    if (tmpTargets.empty())
                        return ObjectGuid::Empty;

                    tmpTargets.sort(Trinity::HealthPctOrderPred());

                    if (Creature* lowestTarget = tmpTargets.front())
                        return lowestTarget->GetGUID();

                    return ObjectGuid::Empty;
                }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_prelateAI(creature);
        }
};

// Zandalari Warlord 69911
class npc_zandalari_warlord : public CreatureScript
{
    public:
        npc_zandalari_warlord() : CreatureScript("npc_zandalari_warlord") { }
    
        struct npc_zandalari_warlordAI : public ScriptedAI
        {
            npc_zandalari_warlordAI(Creature* creature) : ScriptedAI(creature) 
            {
                hasTriggered = false;
                instance = me->GetInstanceScript();
            }
    
            EventMap events;
            InstanceScript* instance;
            uint32 pctHealth;
            ObjectGuid targetGUID;
            bool hasTriggered;
    
            void Reset() override
            {
                events.Reset();
                pctHealth = 66;
                targetGUID = ObjectGuid::Empty;
            }
    
            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_CLEAVE, urand(4.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));

                if (Creature* garajal = me->FindNearestCreature(NPC_GARAJAL_THE_SPIRITBINDER_TRASH, 30.0f, true))
                {
                    if (hasTriggered)
                        return;

                    hasTriggered = true;

                    garajal->AI()->Talk(TALK_SPECIAL_4);
                    garajal->SetVisible(false);
                    garajal->DespawnOrUnsummon(5 * IN_MILLISECONDS);
                }
            }

            void JustDied(Unit* killer) override
            {
                if (instance)
                    instance->SetData(DATA_PRE_COUNCIL_EVENT_2, DONE);
            }
    
            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (HealthBelowPct(pctHealth) && pctHealth > 0)
                {
                    pctHealth -= 33;
                    DoCast(me, SPELL_ZANDALARI_WARCRY, true);
                }
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
                        case EVENT_CLEAVE:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_CLEAVE);
                        
                            events.ScheduleEvent(EVENT_CLEAVE, urand(4.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                            break;
                        case EVENT_STRENGTH_OF_THE_LOA:
                            if (Unit* target = me->GetVictim())
                                targetGUID = target->GetGUID();

                            me->PrepareChanneledCast(me->GetOrientation(), SPELL_STRENGTH_OF_THE_LOA);

                            me->m_Events.Schedule(4500, [this]()
                            {
                                me->RemoveChanneledCast(targetGUID);
                            });
                            break;
                    }
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_warlordAI(creature);
        }
};

class BehindTribePredicate 
{
    public:
        BehindTribePredicate(Unit* const m_caster) : _caster(m_caster) { }
    
        bool operator()(Creature* target)
        {
            return target && target->GetPositionX() > _caster->GetPositionX();
        }
    
    private:
        Unit const* _caster;
};

// Gara`jal Spiritbinder 70056
class npc_garajal_spirit_binder_event : public CreatureScript
{
    public:
        npc_garajal_spirit_binder_event() : CreatureScript("npc_garajal_spirit_binder_event") { }
    
        struct npc_garajal_spirit_binder_eventAI : public ScriptedAI
        {
            npc_garajal_spirit_binder_eventAI(Creature* creature) : ScriptedAI(creature) { }
    
            EventMap nonCombatEvents;
            InstanceScript* instance;
            std::list<Creature*> tribesList;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();

                // Delay for sure that creature was spawned
                me->m_Events.Schedule(1500, [this]()
                {
                    // Skip if near warlord cuz we should only talk in combat start
                    if (me->FindNearestCreature(NPC_ZANDALARI_WARLORD, 30.0f, true))
                        return;

                    // Skip if both events done
                    if (instance && instance->GetData(DATA_PRE_COUNCIL_EVENT_1) == DONE)
                        return;

                    GetCreatureListWithEntryInGrid(tribesList, me, NPC_AMANISHI_FLAME_CHANTER, 35.0f);
                    GetCreatureListWithEntryInGrid(tribesList, me, NPC_ZANDALARI_HIGH_PRIEST, 35.0f);
                    GetCreatureListWithEntryInGrid(tribesList, me, NPC_DRAKKARI_FROST_WARDEN, 35.0f);
                    GetCreatureListWithEntryInGrid(tribesList, me, NPC_FARRAKI_SAND_CONJURER, 35.0f);
                    GetCreatureListWithEntryInGrid(tribesList, me, NPC_GURUBASHI_BERSERKER, 35.0f);
                    GetCreatureListWithEntryInGrid(tribesList, me, NPC_GURUBASHI_BERSERKER_2, 35.0f);

                    // Remove creatures that behind us
                    tribesList.remove_if(BehindTribePredicate(me));

                    nonCombatEvents.ScheduleEvent(EVENT_NEAR_TRIBES_ALIVE, 1.5 * IN_MILLISECONDS);
                });
            }

            uint32 GetData(uint32 type) const override
            {
                for (auto&& itr : tribesList)
                    if (itr->GetGUID().GetCounter() == type)
                        return 1;

                return 0;
            }
    
            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_NEAR_TRIBES_ALIVE)
                    {
                        if (HasAnyNearTribeIsAlve())
                            nonCombatEvents.ScheduleEvent(EVENT_NEAR_TRIBES_ALIVE, 1 * IN_MILLISECONDS);
                        else
                        {
                            if (me->GetDBTableGUIDLow() == 554246 && instance)
                                instance->SetData(DATA_PRE_COUNCIL_EVENT_1, DONE);

                            me->SetVisible(true);
                            Talk(invMatchYellType.find(me->GetDBTableGUIDLow())->second);
                            DoCast(me, SPELL_SUMMON_SHADOWY_MINIONS);
                        }
                    }
                    break;
                }
            }

            private:
                bool HasAnyNearTribeIsAlve()
                {
                    for (auto&& itr : tribesList)
                        if (itr->IsAlive())
                            return true;

                    return false;
                }

        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_garajal_spirit_binder_eventAI(creature);
        }
};

// Shadowed Voodoo Spirit 70060
class npc_shadowed_voodoo_spirit : public CreatureScript
{
    public:
        npc_shadowed_voodoo_spirit() : CreatureScript("npc_shadowed_voodoo_spirit") { }
    
        struct npc_shadowed_voodoo_spiritAI : public tribeAI
        {
            npc_shadowed_voodoo_spiritAI(Creature* creature) : tribeAI(creature)  { }
    
            void AttackStart(Unit* target) override
            {
                if (!target)
                    return;
    
                if (me->Attack(target, false))
                    DoStartNoMovement(target);
            }
    
            void CastInterrupted(SpellInfo const* spell) override
            {
                events.RescheduleEvent(EVENT_SHADOW_BOLT, 4.5 * IN_MILLISECONDS);
    
                if (Unit* vict = me->GetVictim())
                    me->GetMotionMaster()->MoveChase(vict);
            }
    
            void JustEngagedWith(Unit* who) override
            {
                events.RescheduleEvent(EVENT_SHADOW_BOLT, 1 * IN_MILLISECONDS);
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
                    if (eventId == EVENT_SHADOW_BOLT)
                    {
                        me->StopMoving();

                        if (Unit* vict = me->GetVictim())
                        {
                            if (me->GetExactDist2d(vict) > 45.0f)
                                DoStartMovement(vict);
                            else
                                DoStartNoMovement(vict);
                        }

                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            DoCast(target, SPELL_SHADOW_BOLT);

                        events.ScheduleEvent(EVENT_SHADOW_BOLT, 2.5 * IN_MILLISECONDS);
                    }
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shadowed_voodoo_spiritAI(creature);
        }
};

// Lei Shen - tortos intro 70437
class npc_lei_shen_tortos : public CreatureScript
{
    public:
        npc_lei_shen_tortos() : CreatureScript("npc_lei_shen_tortos") { }
    
        struct npc_lei_shen_tortosAI : public ScriptedAI
        {
            npc_lei_shen_tortosAI(Creature* creature) : ScriptedAI(creature) { }
    
            InstanceScript* instance;
            EventMap events;
            bool introDone;
            uint32 bridgeDestroy;

            void InitializeAI() override
            {
                introDone = false;
                instance = me->GetInstanceScript();
                me->SetVisible(false);
                bridgeDestroy = 0;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_DESTROY_BRIDGE)
                    return bridgeDestroy;

                return 0;
            }
    
            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_START_INTRO && !introDone && instance)
                {
                    introDone = true;

                    me->SetReactState(REACT_PASSIVE);
                    events.ScheduleEvent(EVENT_LEI_SHEN_SEND_CINEMATIC, 100);
                }
            }
    
            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
    
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DESTROY_BRIDGE:
                            if (GameObject* bridge = me->FindNearestGameObject(GO_TORTOS_BRIDGE, 300.0f))
                                bridge->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);

                            me->setActive(false);
                            break;
                        case EVENT_LEI_SHEN_SEND_CINEMATIC:
                            events.CancelEvent(EVENT_DESTROY_BRIDGE);
                        
                            if (Creature* bridgeTrigger = me->SummonCreature(NPC_BRIDGE_TRIGGER, pMidBridge, TEMPSUMMON_TIMED_DESPAWN, 60000))
                            {
                                for (auto&& itr : instance->instance->GetPlayers())
                                {
                                    if (Player* player = itr.GetSource())
                                    {
                                        if (player->IsAlive() && !player->IsGameMaster())
                                        {
                                            Position playerPos = bridgeTrigger->GetRandomNearPosition(16.f);
                                            playerPos.m_positionZ = 154.f;
                        
                                            player->NearTeleportTo(playerPos.GetPositionX(), playerPos.GetPositionY(), playerPos.GetPositionZ(), player->GetAngle(me));
                        
                                            if (Aura* slowfall = player->AddAura(130, player))
                                            {
                                                slowfall->SetMaxDuration(3 * MINUTE * IN_MILLISECONDS);
                                                slowfall->SetDuration(3 * MINUTE * IN_MILLISECONDS);
                                            }
                        
                                            player->SendCinematicStart(CINEMATIC_TORTOS_INTRO);
                                        }
                                    }
                                }
                            }
                        
                            if (instance)
                                instance->SetData(DATA_TORTOS_EVENT, DONE);
                        
                            events.ScheduleEvent(EVENT_LEI_SHEN_I_1, 5900); // at 6 seconds.
                            break;
                        case EVENT_LEI_SHEN_I_1:
                            DoCast(me, SPELL_BRIDGE_VIS_S); // into without storm
                            me->SetVisible(true);
                            Talk(TALK_INTRO);
                            me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                            events.ScheduleEvent(EVENT_LEI_SHEN_I_2, 8000); // at 14 seconds.
                            break;
                        case EVENT_LEI_SHEN_I_2:
                            Talk(TALK_SPECIAL_1);
                            me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                            events.ScheduleEvent(EVENT_LEI_SHEN_VISUAL_CAST, 8000); // at 22 seconds.
                            break;
                        
                        case EVENT_LEI_SHEN_VISUAL_CAST:
                            bridgeDestroy = 1;
                            DoCast(me, SPELL_BRIDGE_VIS_S);
                            events.ScheduleEvent(EVENT_LEI_SHEN_DESTROY_BRIDGE, 4000); // at 26 seconds.
                            break;
                        case EVENT_LEI_SHEN_DESTROY_BRIDGE:
                        {
                            if (GameObject* bridge = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_TORTOS_BRIDGE) : ObjectGuid::Empty))
                                bridge->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
                        
                            events.ScheduleEvent(EVENT_LEI_SHEN_TELEPORT_PLAYERS, 16000); // at 41 seconds.
                        }
                        break;
                        case EVENT_LEI_SHEN_TELEPORT_PLAYERS:
                            me->GetMap()->LoadGrid(6041.180f, 5100.50f);
                            DoCast(me, SPELL_TELEPORT_DEPTH);
                            me->SetVisible(false);
                            break;
                        }
                }
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lei_shen_tortosAI(creature);
        }
};

// Waterspout Trigger 70147
struct npc_waterspout_trigger : public ScriptedAI
{
    npc_waterspout_trigger(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        me->m_Events.Schedule(1000, [this]()
        {
            if (me->HasAura(SPELL_WATERSPOUT_POOL_VISUAL))
            {
                me->RemoveAurasDueToSpell(SPELL_WATERSPOUT_AURA);

                scheduler.Schedule(Milliseconds(500), [this](TaskContext context)
                {
                    if (!me->HasAura(SPELL_WATERSPOUT_BACK_TRIGGERED))
                        if (Player* itr = me->FindNearestPlayer(7.15f))
                            DoCast(me, SPELL_WATERSPOUT_BACK_TRIGGERED);

                    context.Repeat(Milliseconds(500));
                });
            }
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Greater Cave Bat 69351
struct npc_greater_cave_bat : public customCreatureAI
{
    npc_greater_cave_bat(Creature* creature) : customCreatureAI(creature) 
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SLASHING_TALONS, 2.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SONIC_SCREECH, 9.5 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_SLASHING_TALONS, urand(10.5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS), EVENT_SLASHING_TALONS, eventId);
            ExecuteTargetEvent(SPELL_SONIC_SCREECH, urand(13.5 * IN_MILLISECONDS, 19 * IN_MILLISECONDS), EVENT_SONIC_SCREECH, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Shale Stalker 70587
struct npc_shale_stalker : public customCreatureAI
{
    npc_shale_stalker(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHALE_SHARDS, urand(1.5 * IN_MILLISECONDS, 3.5 * IN_MILLISECONDS));
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
            ExecuteTargetEvent(SPELL_SHALE_SHARDS, 3 * IN_MILLISECONDS, EVENT_SHALE_SHARDS, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Fungal Growth 70153
struct npc_fungal_growth_2 : public customCreatureAI
{
    npc_fungal_growth_2(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FUNGI_SPORES, urand(4.5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_GROW, 8 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_FUNGI_SPORES, urand(9 * IN_MILLISECONDS, 12 * IN_MILLISECONDS), EVENT_FUNGI_SPORES, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_GROW, 8 * IN_MILLISECONDS, EVENT_GROW, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Mist Lurker 70594
struct npc_mist_lurker : public customCreatureAI
{
    npc_mist_lurker(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_CHOCKING_MISTS, 5.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CORROSIVE_BREATH, 9.5 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_CHOCKING_MISTS, urand(11 * IN_MILLISECONDS, 17 * IN_MILLISECONDS), EVENT_CHOCKING_MISTS, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_CORROSIVE_BREATH, 12.5 * IN_MILLISECONDS, EVENT_CORROSIVE_BREATH, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Cavern Burrower 70589
struct npc_cavern_burrower : public customCreatureAI
{
    npc_cavern_burrower(Creature* creature) : customCreatureAI(creature) 
    { 
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
        me->HandleEmoteCommand(EMOTE_ONESHOT_SUBMERGE);

        DoCast(me, SPELL_SONIC_CALL);
        events.ScheduleEvent(EVENT_CRYSTAL_BARBS, 5.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CRUSH_ARMOR, 2.5 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_CRYSTAL_BARBS, urand(5.5 * IN_MILLISECONDS, 7.5 * IN_MILLISECONDS), EVENT_CRYSTAL_BARBS, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_CRUSH_ARMOR_2, urand(8 * IN_MILLISECONDS, 15.5 * IN_MILLISECONDS), EVENT_CRUSH_ARMOR, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Mysterious Mushroom 70545
struct npc_mysterious_mushroom : public CreatureAI
{
    npc_mysterious_mushroom(Creature* creature) : CreatureAI(creature) { }

    void Reset() override
    {
        DoCast(me, SPELL_FUNGAL_EXPLOSION); // AreaTrigger
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Eternal Guardian 70586
struct npc_eternal_guardian : public customCreatureAI
{
    npc_eternal_guardian(Creature* creature) : customCreatureAI(creature) 
    { 
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
    }

    InstanceScript* instance;
    ObjectGuid nearBellGUID;

    void InitializeAI() override
    {
        instance = me->GetInstanceScript();
        nearBellGUID = ObjectGuid::Empty;

        // Delay for load
        me->m_Events.Schedule(1500, [this]()
        {
            if (GameObject* go = me->FindNearestGameObject(GO_MOGU_BELL, 30.0f))
                nearBellGUID = go->GetGUID();
        });
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_ETERNAL_PRISON, 2.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_LIGHTNING_NOVA, 5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SIPHON_LIFE, urand(11 * IN_MILLISECONDS, 13.5 * IN_MILLISECONDS));
    }

    void JustDied(Unit* killer) override
    {
        if (instance && instance->GetData(DATA_MEGERA_EVENT) != DONE)
        {
            instance->SetData(DATA_MEGERA_EVENT, IN_PROGRESS);

            if (instance->GetData(DATA_MEGERA_EVENT) >= 3)
                if (Creature* megaera = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_MEGAERA)))
                    megaera->AI()->DoAction(ACTION_MEGAERA_EMERGE);
        }

        if (GameObject* bell = ObjectAccessor::GetGameObject(*me, nearBellGUID))
            bell->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);
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
            ExecuteTargetEvent(SPELL_ETERNAK_PRISON, urand(11 * IN_MILLISECONDS, 17 * IN_MILLISECONDS), EVENT_ETERNAL_PRISON, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_LIGHTNING_NOVA, 5.5 * IN_MILLISECONDS, EVENT_LIGHTNING_NOVA, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_SIPHON_LIFE, urand(12 * IN_MILLISECONDS, 20 * IN_MILLISECONDS), EVENT_SIPHON_LIFE, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Corpse Spider 68248
struct npc_corpse_spider : public customCreatureAI
{
    npc_corpse_spider(Creature* creature) : customCreatureAI(creature) { }

    void InitializeAI() override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
    }

    void Reset() override
    {
        events.Reset(); // Need set inverted anim state
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_WEB_SPRAY, urand(4.5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (pointId == 0)
        {
            me->SetAnimTier(AnimTier::Ground);
            me->OverrideInhabitType(INHABIT_GROUND);
            me->UpdateMovementFlags();

            me->SetHomePosition(*me);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
        }
    }

    void JustDied(Unit* killer) override
    {
        if (me->GetInstanceScript())
            me->GetInstanceScript()->DoRemoveAurasDueToSpellOnPlayers(SPELL_COCOON);
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
            ExecuteTargetEvent(SPELL_WEB_SPRAY, urand(9 * IN_MILLISECONDS, 21.5 * IN_MILLISECONDS), EVENT_WEB_SPRAY, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Corpse Spider Web 68249
struct npc_corpse_spider_web : public customCreatureAI
{
    npc_corpse_spider_web(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        DoCast(me, SPELL_CORPSE_WEB_SPAWN, true);
    }
};

// Bow Fly Swarm 68222
struct npc_bow_fly_swarm : public customCreatureAI
{
    npc_bow_fly_swarm(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_LUCIFERASE, urand(3 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
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
            ExecuteTargetEvent(SPELL_LUCIFERASE, urand(3 * IN_MILLISECONDS, 12 * IN_MILLISECONDS), EVENT_LUCIFERASE, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Gastropod 68220
struct npc_gastropod : public CreatureAI
{
    npc_gastropod(Creature* creature) : CreatureAI(creature) 
    { 
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
    }

    void Reset() override
    {
        me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 0.0f);
        me->SetSpeed(MOVE_RUN, 0.4f);
    }

    void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType damageType) override
    {
        if (!victim)
            return;

        if (victim->HasAura(SPELL_DEVOURED))
        {
            if (Unit* newTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 90.0f, true))
                me->FixateOnTarget(newTarget->GetGUID());

            return;
        }

        // Instant Kill Pet
        if (victim && victim->ToPet())
            DoCast(victim, SPELL_DEVOURED, true);
    }

    void JustEngagedWith(Unit* who) override
    {
        DoCast(me, SPELL_ABRASIVE_RADULA, true);
        me->FixateOnTarget(who->GetGUID());
    }

    void UpdateAI(uint32 /*diff*/) override 
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Roaming Fog - 68313
struct npc_roaming_fog : public customCreatureAI
{
    npc_roaming_fog(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        me->SetWalk(true);
        events.Reset();
        events.ScheduleEvent(EVENT_MOVE_RANDOM, urand(3000, 8000));

        scheduler
            .SetValidator([this] { return !me->IsInCombat(); })
            .Schedule(Seconds(3), [this](TaskContext context)
        {
            Position position = me->GetRandomPoint(CenterDurumu, 40.0f);
            me->GetMotionMaster()->MovePoint(0, position);

            context.Repeat(Milliseconds(me->GetSplineDuration()));
        });
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->SetWalk(false);
        scheduler.CancelAll();
        // todo: fix damage
        //events.ScheduleEvent(EVENT_GNAWED_UPON, urand(3 * IN_MILLISECONDS, 7.5 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        // todo: fix damage
        //while (uint32 eventId = events.ExecuteEvent())
        //{
        //    ExecuteTargetEvent(SPELL_GNAWED_UPON, urand(8 * IN_MILLISECONDS, 19 * IN_MILLISECONDS), EVENT_GNAWED_UPON, eventId);
        //    break;
        //}

        DoMeleeAttackIfReady();
    }
};

// Quivering Blob 69383
struct npc_quivering_blob : public CreatureAI
{
    npc_quivering_blob(Creature* creature) : CreatureAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        // path here if summoned by event
    }

    void JustDied(Unit* killer) override
    {
        DoCast(me, SPELL_OOZE_EXPLOSION);
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Thunder Lord 69821
struct npc_thunder_lord : public customCreatureAI
{
    npc_thunder_lord(Creature* creature) : customCreatureAI(creature) 
    { 
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_CONDUCTIVE_SHIELD);
        events.ScheduleEvent(EVENT_LEI_SHEN_GIFT, urand(8 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_CONDUCTIVE_SHIELD, 20 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_LEI_SHEN_GIFT, urand(18 * IN_MILLISECONDS, 28 * IN_MILLISECONDS), EVENT_LEI_SHEN_GIFT, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_CONDUCTIVE_SHIELD, 20 * IN_MILLISECONDS, EVENT_CONDUCTIVE_SHIELD, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Lightning Guardian 69834
struct npc_lightning_guardian : public customCreatureAI
{
    npc_lightning_guardian(Creature* creature) : customCreatureAI(creature) 
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        isCaster = true;
    }

    void Reset() override
    {
        events.Reset();
    }

    void AttackStart(Unit* target) override
    {
        if (!target)
            return;

        if (me->Attack(target, false))
            DoStartNoMovement(target);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_CONDUCTIVE_SHIELD, 10 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_LIGHTNING_BURST, 1.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_LIGHTNING_STORM, 11 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_LIGHTNING_BURST, 2 * IN_MILLISECONDS, EVENT_LIGHTNING_BURST, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_CONDUCTIVE_SHIELD, 20 * IN_MILLISECONDS, EVENT_CONDUCTIVE_SHIELD, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_LIGHTNING_STORM, 19 * IN_MILLISECONDS, EVENT_LIGHTNING_STORM, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Manchu 70202
struct npc_manchu : public customCreatureAI
{
    npc_manchu(Creature* creature) : customCreatureAI(creature) 
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        instance = me->GetInstanceScript();
    }

    InstanceScript* instance;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        if (Creature* ironQon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_IRON_QON) : ObjectGuid::Empty))
            ironQon->AI()->Talk(0);

        me->CallForHelp(35.0f);
        events.ScheduleEvent(EVENT_WILD_SMASH, urand(4.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_TRIUMPHANT_RUSH, 12.5 * IN_MILLISECONDS);
    }

    void JustDied(Unit* killer) override
    {
        if (Creature* ironQon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_IRON_QON) : ObjectGuid::Empty))
            ironQon->AI()->Talk(1);
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
            ExecuteTargetEvent(SPELL_WILD_SMASH, urand(8.5 * IN_MILLISECONDS, 17.5 * IN_MILLISECONDS), EVENT_WILD_SMASH, eventId, PRIORITY_CHANNELED);
            ExecuteTargetEvent(SPELL_TRIUMPHANT_RUSH, 12.5 * IN_MILLISECONDS, EVENT_TRIUMPHANT_RUSH, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Weishen 70205
struct npc_weishen : public customCreatureAI
{
    npc_weishen(Creature* creature) : customCreatureAI(creature) 
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        instance = me->GetInstanceScript();
    }

    InstanceScript* instance;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        if (Creature* ironQon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_IRON_QON) : ObjectGuid::Empty))
            ironQon->AI()->Talk(2);

        me->CallForHelp(35.0f);
        events.ScheduleEvent(EVENT_TRIUMPHANT_ROAR, 10.5 * IN_MILLISECONDS);
    }

    void JustDied(Unit* killer) override
    {
        if (Creature* ironQon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_IRON_QON) : ObjectGuid::Empty))
            ironQon->AI()->Talk(3);
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
            ExecuteTargetEvent(SPELL_TRIUMPHANT_ROAR, 10.5 * IN_MILLISECONDS, EVENT_TRIUMPHANT_ROAR, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Untrained Quilen 70206, 70209
struct npc_untrained_quilen : public customCreatureAI
{
    npc_untrained_quilen(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->RemoveAurasDueToSpell(SPELL_SIT);
        events.ScheduleEvent(EVENT_CARNIVOROUS_BITE, urand(2.5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
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
            ExecuteTargetEvent(SPELL_CARNIVOUROUS_BITE, urand(8.5 * IN_MILLISECONDS, 20 * IN_MILLISECONDS), EVENT_CARNIVOROUS_BITE, eventId);
            ExecuteTargetEvent(SPELL_LEAPING_RUSH, urand(12.5 * IN_MILLISECONDS, 25 * IN_MILLISECONDS), EVENT_LEAPING_RUSH, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Shanze Celestial Shaper 70240
struct npc_shanze_celestial_shaper : public customCreatureAI
{
    npc_shanze_celestial_shaper(Creature* creature) : customCreatureAI(creature) 
    { 
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
    }

    void Reset() override
    {
        // delay for init 
        me->m_Events.Schedule(1500, [this]()
        {
            // Should select nearest
            if (Creature* celestialStalker = me->FindNearestCreature(NPC_CONSORT_CONTROLLER, 30.0f, true))
                DoCast(celestialStalker, SPELL_SIPHON_ESSENCE_VISUAL);
        });

        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->InterruptNonMeleeSpells(true, SPELL_SIPHON_ESSENCE_VISUAL);
        events.ScheduleEvent(EVENT_LIGHTNING_LASH, urand(2.5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_COSMIC_STRIKE, urand(8 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
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
            ExecuteTargetEvent(SPELL_COSMIC_STRIKE, urand(8.5 * IN_MILLISECONDS, 20 * IN_MILLISECONDS), EVENT_COSMIC_STRIKE, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_LIGHTNING_LASH, urand(2.5 * IN_MILLISECONDS, 3 * IN_MILLISECONDS), EVENT_LIGHTNING_LASH, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Dark Ritualist 69702
struct npc_dark_ritualist : public customCreatureAI
{
    npc_dark_ritualist(Creature* creature) : customCreatureAI(creature) 
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        transformID = 0;
    }

    uint32 transformID;

    void Reset() override
    {
        events.Reset();

        if (transformID)
            DoCast(me, transformID, true);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->CallForHelp(30.0f);
        me->RemoveAurasDueToSpell(SPELL_INTERRUPT_SHIELD);
        events.ScheduleEvent(EVENT_RANGE_RITUAL_LIGHTNING, urand(2.5 * IN_MILLISECONDS, 12.5 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_SHADOW_NOVA, urand(15 * IN_MILLISECONDS, 35 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_FIRE_BLAST, urand(15 * IN_MILLISECONDS, 40 * IN_MILLISECONDS));
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_RITUALIST)
            transformID = data;
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
            ExecuteTargetEvent(urand(0, 1) ? SPELL_LONG_RANGE_RITUAL_LIGHTNING : SPELL_SHORT_RANGE_RITUAL_LIGHTNING, urand(5.5 * IN_MILLISECONDS, 19.5 * IN_MILLISECONDS), EVENT_RANGE_RITUAL_LIGHTNING, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_SHADOW_NOVA, urand(15 * IN_MILLISECONDS, 35 * IN_MILLISECONDS), EVENT_SHADOW_NOVA, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_FIRE_BLAST, urand(15 * IN_MILLISECONDS, 40 * IN_MILLISECONDS), EVENT_FIRE_BLAST, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Ritual Guard 70179
struct npc_ritual_guard : public customCreatureAI
{
    npc_ritual_guard(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHOCKWAVE, urand(5.5 * IN_MILLISECONDS, 12.5 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_SHADOW_NOVA, urand(15 * IN_MILLISECONDS, 35 * IN_MILLISECONDS));
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
            ExecuteTargetEvent(SPELL_SHOCKWAVE, urand(8.5 * IN_MILLISECONDS, 15.5 * IN_MILLISECONDS), EVENT_SHOCKWAVE, eventId);
            ExecuteTargetEvent(SPELL_SHADOW_NOVA, urand(13 * IN_MILLISECONDS, 25 * IN_MILLISECONDS), EVENT_SHADOW_NOVA, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Muckbat 70232
struct npc_muckbat : public customCreatureAI
{
    npc_muckbat(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_MUCK_SPLIT, urand(3.5 * IN_MILLISECONDS, 12.5 * IN_MILLISECONDS));
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
            ExecuteTargetEvent(SPELL_MUCK_SPLIT, urand(6 * IN_MILLISECONDS, 15 * IN_MILLISECONDS), EVENT_MUCK_SPLIT, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Skittering Spiderling 70227
struct npc_skittering_spiderling : public customCreatureAI
{
    npc_skittering_spiderling(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FOUL_VENOM, urand(3.5 * IN_MILLISECONDS, 12.5 * IN_MILLISECONDS));
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
            ExecuteTargetEvent(SPELL_FOUL_VENOM, urand(6 * IN_MILLISECONDS, 15 * IN_MILLISECONDS), EVENT_FOUL_VENOM, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Rotting Scavenger 70224
struct npc_rotting_scavenger : public customCreatureAI
{
    npc_rotting_scavenger(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FRENZIED_CONSUMPTION, urand(8.5 * IN_MILLISECONDS, 19 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_INFECTED_BITE, urand(2.5 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
    }

    void JustDied(Unit* killer) override
    {
        DoCast(me, SPELL_ROT, true);
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
            ExecuteTargetEvent(SPELL_FRENZIED_CONSUMPTION, urand(12 * IN_MILLISECONDS, 20 * IN_MILLISECONDS), EVENT_FRENZIED_CONSUMPTION, eventId, PRIORITY_CHANNELED);
            ExecuteTargetEvent(SPELL_INFECTED_BITE, urand(6.5 * IN_MILLISECONDS, 7.5 * IN_MILLISECONDS), EVENT_INFECTED_BITE, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

class spell_focused_lightning_aoe_trash : public SpellScript
{
    PrepareSpellScript(spell_focused_lightning_aoe_trash);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && (!target->ToPlayer() || target->ToPlayer() && target->ToPlayer()->IsGameMaster()); });
    }

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* unit = GetHitUnit())
            {
                if (unit->HasAura(SPELL_FOCUSED_LIGHTNING_FIXATE, caster->GetGUID()))
                {
                    caster->CastSpell(unit, SPELL_FOCUSED_LIGHTNING_DETONATION, true);
                    caster->Kill(caster);
                    return;
                }

                caster->CastSpell(unit, SPELL_FOCUSED_LIGHTNING_DAMAGE, true);
            }
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_focused_lightning_aoe_trash::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_focused_lightning_aoe_trash::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class spell_storm_weapon : public AuraScript
{
    PrepareAuraScript(spell_storm_weapon);

    void HandleOnApply(AuraEffect const* eff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster())
            if (caster->ToCreature())
                caster->ToCreature()->UpdateEntry(NPC_CRAZED_STORMCALLER);
    }

    void HandleOnRemove(AuraEffect const* eff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster())
            if (caster->ToCreature())
                caster->ToCreature()->UpdateEntry(NPC_ZANDALARI_STORMCALLER);
    }

    void HandleOnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        if (Unit* caster = GetCaster())
        {
            if (caster->ToCreature() && caster->ToCreature()->GetVictim())
                caster->CastSpell(caster->ToCreature()->GetVictim(), 139220, true);
        }
    }


    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_storm_weapon::HandleOnApply, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_storm_weapon::HandleOnRemove, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectProc += AuraEffectProcFn(spell_storm_weapon::HandleOnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

class spell_storm_energy : public SpellScript
{
    PrepareSpellScript(spell_storm_energy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return sSpellMgr->GetSpellInfo(SPELL_STORM_ENERGY);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && (!target->ToPlayer() || target->ToPlayer() && target->ToPlayer()->IsGameMaster()); });

        if (Unit* caster = GetCaster())
        {
            uint32 m_maxSize = caster->GetMap()->Is25ManRaid() ? 3 : 1;

            if (targets.size() > m_maxSize)
                Trinity::Containers::RandomResizeList(targets, m_maxSize);
        }
    }

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* target = GetHitUnit())
            if (Unit* caster = GetCaster())
                caster->AddAura(GetSpellInfo()->Effects[0].BasePoints, target);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_storm_energy::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_storm_energy::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class spell_storm_weapon_proc : public SpellScriptLoader
{
    public:
        spell_storm_weapon_proc() : SpellScriptLoader("spell_storm_weapon_proc") { }

        class spell_storm_weapon_proc_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_storm_weapon_proc_SpellScript);

            void SelectTarget(WorldObject*&target)
            {
                Unit* caster = GetCaster();

                if (caster && caster->ToCreature())
                {
                    if (Unit* victim = caster->ToCreature()->GetVictim())
                    {
                        target = victim;
                    }
                }
            }

            void Register() override
            {
                OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_storm_weapon_proc_SpellScript::SelectTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_storm_weapon_proc_SpellScript();
        }
};


class spell_storm_weapon_aura : public SpellScriptLoader
{
    public:
        spell_storm_weapon_aura() : SpellScriptLoader("spell_storm_weapon_aura") { }

        class spell_storm_weapon_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_storm_weapon_aura_AuraScript);

            void HandleOnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                if (Unit* caster = GetCaster())
                {
                    if (caster->ToCreature() && caster->ToCreature()->GetVictim())
                        caster->CastSpell(caster->ToCreature()->GetVictim(), 139220, true);
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_storm_weapon_aura_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_storm_weapon_aura_AuraScript();
        }
};

class inRangePredicate
{
    private:
        Unit* caster;
    public:
        inRangePredicate(Unit* _caster) : caster(_caster) { }

        bool operator()(WorldObject* target) const
        {
            return target->GetExactDist2d(caster) < 15.1f;
        }
};

// Call of Flames 138607 / Eruption 138652
class spell_flame_chanters : public SpellScript
{
    PrepareSpellScript(spell_flame_chanters);

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_FIERY_CORE, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_flame_chanters::HandleAfterCast);
    }
};

// Fiery Core 138610
class spell_fiery_core : public AuraScript
{
    PrepareAuraScript(spell_fiery_core);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetStackAmount() > 4)
        {
            if (Unit*  owner = GetUnitOwner())
            {
                owner->CastSpell(owner, SPELL_MOLTEN_BARRAGE, true);
                owner->RemoveAurasDueToSpell(SPELL_FIERY_CORE);
            }
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_fiery_core::OnApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAPPLY);
    }
};

// Judgment of the Loa 139223
class spell_judgment_of_the_loa : public SpellScript
{
    PrepareSpellScript(spell_judgment_of_the_loa);

    void AddExtraDamageEff()
    {
        if (Unit* target = GetHitUnit())
            if (Aura* aur = target->GetAura(SPELL_MARK_OF_THE_LOA))
                SetHitDamage(GetHitDamage() + aur->GetStackAmount() * GetHitDamage());
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_judgment_of_the_loa::AddExtraDamageEff);
    }
};

// Conjure Elementals 140636
class spell_conjure_elementals : public SpellScript
{
    PrepareSpellScript(spell_conjure_elementals)

    void HandleEffectHitTarget(SpellEffIndex effectIndex)
    {
        if (Unit* caster = GetCaster())
        {
            caster->CastSpell(caster, SPELL_CONJURE_ELEMENTALS_LEFT, true);
            caster->CastSpell(caster, SPELL_CONJURE_ELEMENTALS_RIGHT, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_conjure_elementals::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class SummonShadowyMinionsPredicate 
{
    public:
        SummonShadowyMinionsPredicate(Creature* const m_caster) : _caster(m_caster) { }
    
        bool operator()(WorldObject* object)
        {
            if (_caster->AI()->GetData(object->GetGUID().GetCounter()))
                return false;

            return true;
        }
    
    private:
        Creature const* _caster;
};

// Summon Shadowy Minions 138730
class spell_summon_shadowy_minions : public SpellScript
{
    PrepareSpellScript(spell_summon_shadowy_minions);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            targets.remove_if(SummonShadowyMinionsPredicate(caster));
    }

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->SummonCreature(NPC_SHADOWED_VOODOO_SPIRIT, *target, TEMPSUMMON_MANUAL_DESPAWN);
    }

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            caster->SetVisible(false);
            caster->DespawnOrUnsummon(6 * IN_MILLISECONDS);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_summon_shadowy_minions::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_summon_shadowy_minions::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
        AfterCast += SpellCastFn(spell_summon_shadowy_minions::HandleAfterCast);
    }
};

// Call Lightning 139853
class spell_leishen_call_lightning : public AuraScript
{
    PrepareAuraScript(spell_leishen_call_lightning);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            if (owner->AI()->GetData(TYPE_DESTROY_BRIDGE))
                owner->CastSpell(pMidBridge.GetPositionX() + frand(-22.0f, 22.0f), pMidBridge.GetPositionY() + frand(-22.0f, 22.0f), pMidBridge.GetPositionZ(), SPELL_BRIDGE_VISUAL_STRIKE, true);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_leishen_call_lightning::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Waterspout back 139158
class spell_waterspout_back : public AuraScript
{
    PrepareAuraScript(spell_waterspout_back);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            owner->CastSpell(owner, SPELL_WATERSPOUT_TELEPORT, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_waterspout_back::OnRemove, EFFECT_1, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
    }
};

class SiphonLifePredicate 
{
    public:
        SiphonLifePredicate(Unit* const m_owner) : _owner(m_owner) { }
    
        bool operator()(WorldObject* object)
        {
            return object && object->GetExactDist2d(_owner) < 15.0f;
        }
    
    private:
        Unit const* _owner;
};


// Siphon Life 140630
class spell_eternal_siphon_life : public SpellScript
{
    PrepareSpellScript(spell_eternal_siphon_life);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
            targets.remove_if(SiphonLifePredicate(caster));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_eternal_siphon_life::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Sonic Call 140600
class spell_sonic_call : public SpellScript
{
    PrepareSpellScript(spell_sonic_call);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_SHALE_STALKER; });
    }

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Creature* target = GetHitCreature())
                if (Unit* attacker = caster->getAttackerForHelper())
                    target->AI()->AttackStart(attacker);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sonic_call::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_sonic_call::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Luciferase 134470
class spell_luciferase : public SpellScript
{
    PrepareSpellScript(spell_luciferase);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            if (InstanceScript* instance = caster->GetInstanceScript())
            {
                uint32 targetsCount = instance->instance->Is25ManRaid() ? 3 : 1;

                if (targets.size() > targetsCount)
                    Trinity::Containers::RandomResizeList(targets, targetsCount);
            }
        }
    }

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_LUCIFERASE_EFF, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_luciferase::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_luciferase::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Devoured 134415
class spell_devoured : public AuraScript
{
    PrepareAuraScript(spell_devoured);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* owner = GetOwner()->ToUnit())
                caster->Kill(owner);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_devoured::OnRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// Conductive Shield 140296
class spell_conductive_shield : public AuraScript
{
    PrepareAuraScript(spell_conductive_shield);

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Unit* target = eventInfo.GetActionTarget())
                owner->CastSpell(target, SPELL_ZAP, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_conductive_shield::HandleOnProc);
    }
};

// Triumphant Roar 139815
class spell_triumphant_roar : public SpellScript
{
    PrepareSpellScript(spell_triumphant_roar);

    std::list<WorldObject*> m_targets;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && (target->GetEntry() != NPC_UNTRAINED_QUILEN && target->GetEntry() != NPC_UNTRAINED_QUILEN_2 || !target->ToUnit()->IsInCombat()); });
        m_targets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_triumphant_roar::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_triumphant_roar::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_triumphant_roar::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_triumphant_roar::CopyTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_triumphant_roar::CopyTargets, EFFECT_4, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_triumphant_roar::CopyTargets, EFFECT_5, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Cosmic Strike Summon 139774
class spell_cosmic_strike_summon : public SpellScript
{
    PrepareSpellScript(spell_cosmic_strike_summon);

    void SelectTargets(SpellDestination& dest)
    {
        Position newPos = { dest._position.GetPositionX(), dest._position.GetPositionY(), frand(152.2f, 154.2f), dest._position.GetOrientation() };
        dest.Relocate(newPos);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_cosmic_strike_summon::SelectTargets, EFFECT_1, TARGET_DEST_CASTER);
    }
};

class RitualLightningPredicate 
{
    public:
        RitualLightningPredicate(Unit* const caster, const uint32 spell_id) : _caster(caster), _spell_id(spell_id) { }
    
        bool operator()(WorldObject* object)
        {
            return object && object->ToPlayer() && _spell_id == SPELL_LONG_RANGE_RITUAL_LIGHTNING ? _caster->GetExactDist2d(object->ToPlayer()) < 30.0f : _caster->GetExactDist2d(object->ToPlayer()) > 30.0f;
        }
    
    private:
        Unit const* _caster;
        uint32 const _spell_id;
};

// Range Ritual Lightning 137994, 137995
class spell_range_ritual_lightning : public SpellScript
{
    PrepareSpellScript(spell_range_ritual_lightning);

    std::list<WorldObject*> m_targets;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
            targets.remove_if(RitualLightningPredicate(caster, GetSpellInfo()->Id));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_range_ritual_lightning::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Putrify 139316
class spell_putrify_proc : public AuraScript
{
    PrepareAuraScript(spell_putrify_proc);

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Unit* target = eventInfo.GetProcTarget())
            if (Unit* owner = GetOwner()->ToUnit())
                owner->CastSpell(target, SPELL_PUTRIFY_PROC_EFF, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_putrify_proc::HandleOnProc);
    }
};

// Cocoon 112844
class spell_corpse_spider_cocoon : public SpellScript
{
    PrepareSpellScript(spell_corpse_spider_cocoon);

    std::list<WorldObject*> m_targets;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        uint32 targetCount = GetCaster()->GetMap()->Is25ManRaid() ? 7 : 3;

        if (targets.size() > targetCount)
            Trinity::Containers::RandomResizeList(targets, targetCount);

        m_targets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_corpse_spider_cocoon::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_corpse_spider_cocoon::CopyTargets, EFFECT_1, TARGET_UNIT_CONE_ENEMY_104);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_corpse_spider_cocoon::CopyTargets, EFFECT_2, TARGET_UNIT_CONE_ENEMY_104);
    }
};

// Displace 139110
class spell_throne_of_thunder_displace : public AuraScript
{
    PrepareAuraScript(spell_throne_of_thunder_displace);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* owner = GetOwner()->ToPlayer())
        {
            Movement::MoveSplineInit init(owner);

            init.Path().push_back(G3D::Vector3(owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ() + 8.0f));

            for (auto&& itr : displacementPath)
                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

            init.SetUncompressed();
            init.SetFly();
            init.SetWalk(false);
            init.SetVelocity(20.0f);
            init.Launch();

            owner->m_Events.Schedule(owner->GetSplineDuration(), [=]()
            {
                owner->RemoveAurasDueToSpell(SPELL_DISPLACEMENT);
            });
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_throne_of_thunder_displace::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

// Ancient Mogu Bell 218723
class go_ancient_mogu_bell : public GameObjectScript
{
    public:
        go_ancient_mogu_bell() : GameObjectScript("go_ancient_mogu_bell") { }

        void OnGameObjectStateChanged(GameObject* go, uint32 state)
        {
            if (state == GO_STATE_ACTIVE && go->GetInstanceScript())
                if (Creature* bellAnnouncer = go->FindNearestCreature(NPC_SLG_GENERIC_MOP, 50.0f, true))
                    bellAnnouncer->AI()->Talk(go->GetInstanceScript()->GetData(DATA_MEGERA_EVENT) - 1);
        }
};
// Displacement Pad 218417
class go_displacement_pad : public GameObjectScript
{
    public:
        go_displacement_pad() : GameObjectScript("go_displacement_pad") { }
    
        bool OnGossipHello(Player* player, GameObject* go)
        {
            if (player->GetInstanceScript() && player->GetInstanceScript()->GetBossState(DATA_LEI_SHEN) != IN_PROGRESS)
                player->CastSpell(player, SPELL_DISPLACEMENT, true);

            return true;
        }
};


// Areatrigger 8940
// Players should die from AT, not drop damage, cuz for example priest could use levitation and don`t die
class AreaTrigger_under_horridon_way : public AreaTriggerScript
{
    public:
        AreaTrigger_under_horridon_way() : AreaTriggerScript("AreaTrigger_under_horridon_way") { }
    
        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_PRE_HORIDON_EVENT) == DONE)
                    return true;

                player->CastSpell(player, SPELL_DARK_WINDS, true);

                if (instance->GetData(DATA_PRE_HORIDON_EVENT) < DONE)
                    instance->SetData(DATA_PRE_HORIDON_EVENT, SPECIAL);
            }

            return false;
        }
};

// Areatrigger 8896
class AreaTrigger_under_lei_shen : public AreaTriggerScript
{
    public:
        AreaTrigger_under_lei_shen() : AreaTriggerScript("AreaTrigger_under_lei_shen") { }
    
        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_LEI_SHEN_EVENT) == DONE || instance->GetData(DATA_LEI_SHEN_EVENT) == IN_PROGRESS)
                    return true;

                instance->SetData(DATA_LEI_SHEN_EVENT, IN_PROGRESS);
            }
    
            return false;
        }
};

// 845, 847, 893. Summoned by 139499, 139535, 140781 - Dark Winds
class sat_dark_winds : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringUpdate(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            float x, y;
            GetPositionWithDistInOrientation(itr, 130.0f, 1.61f, x, y);
            temPos = { x, y, itr->GetPositionZ(), 0.0f };

            if (itr->IsAlive() && !itr->HasForcedMovement())
                itr->SendApplyMovementForce(true, temPos, itr->GetPositionX() > 5460.0f ? 5.8f : 3.8f);
            else if (!itr->IsAlive() && itr->HasForcedMovement())
                itr->SendApplyMovementForce(false, temPos);
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->SendApplyMovementForce(false, temPos);
    }

    private:
        Position temPos;
};

// 889. Summoned by 140596 - Fungal Explosion
class sat_fungal_explosion : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer();
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                caster->CastSpell(caster, SPELL_FUNGAL_EXPLOSION_EFF, true);

                if (caster->ToCreature())
                    caster->ToCreature()->DespawnOrUnsummon(1 * IN_MILLISECONDS);
            }
        }
    }
};

// 786. Summoned by 138187 - Lightning Storm
class sat_lightning_storm : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, SPELL_LIGHTNING_STORM_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_LIGHTNING_STORM_EFF);
    }
};

// 844. Summoned By 139477 - Choking Gas
class sat_choking_gas : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
            if (Player* target = triggering->ToPlayer())
                caster->CastSpell(target, SPELL_CHOKING_GAS_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                target->RemoveAura(SPELL_CHOKING_GAS_EFF, caster->GetGUID(), 0, AURA_REMOVE_BY_EXPIRE);
    }
};

// 590. Summoned By 134414 - Abrasive Radula
class sat_abrasive_radula : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && GetCaster() && GetCaster()->isInFront(triggering->ToPlayer(), M_PI / 3);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
            if (Player* target = triggering->ToPlayer())
                if (!target->HasAura(SPELL_DEVOURED))
                    caster->CastSpell(target, SPELL_DEVOURED, true);
    }
};

// 592. Summoned By 134483 - Corpse Spider Web
class sat_corpse_spider_web : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* triggered = target->ToPlayer())
            if (Creature* spider = triggered->FindNearestCreature(NPC_CORPSE_SPIDER, 20.0f, true))
                spider->GetMotionMaster()->MovePoint(0, *triggered);
    }
};

void AddSC_throne_of_thunder()
{
    new npc_zandalari_spearshaper();
    new npc_zandalari_stormcaller();
    new npc_focused_lightning_trash();
    new npc_zandalari_water_binder();
    new npc_zandalari_blade_initiate();
    new npc_zandalari_stormcaller_2();
    new npc_ancient_python();
    new npc_tormented_spirit();
    new npc_soul_fed_construct();
    new npc_spirit_flayer();
    new npc_stormbringer_drazkil();
    new npc_amanishi_flame_chanter();
    new npc_drakkari_frost_warden();
    new npc_farraki_sand_conjurer();
    new npc_zandalari_high_priest();
    new npc_gurubashi_berserker();
    new npc_zandalari_prophet();
    new npc_zandalari_prelate();
    new npc_zandalari_warlord();
    new npc_garajal_spirit_binder_event();
    new npc_shadowed_voodoo_spirit();
    new npc_lei_shen_tortos();
    new creature_script<npc_waterspout_trigger>("npc_waterspout_trigger");
    new creature_script<npc_greater_cave_bat>("npc_greater_cave_bat");
    new creature_script<npc_shale_stalker>("npc_shale_stalker");
    new creature_script<npc_fungal_growth_2>("npc_fungal_growth_2");
    new creature_script<npc_mist_lurker>("npc_mist_lurker");
    new creature_script<npc_cavern_burrower>("npc_cavern_burrower");
    new creature_script<npc_mysterious_mushroom>("npc_mysterious_mushroom");
    new creature_script<npc_eternal_guardian>("npc_eternal_guardian");
    new creature_script<npc_corpse_spider>("npc_corpse_spider");
    new creature_script<npc_corpse_spider_web>("npc_corpse_spider_web");
    new creature_script<npc_bow_fly_swarm>("npc_bow_fly_swarm");
    new creature_script<npc_gastropod>("npc_gastropod");
    new creature_script<npc_roaming_fog>("npc_roaming_fog");
    new creature_script<npc_quivering_blob>("npc_quivering_blob");
    new creature_script<npc_thunder_lord>("npc_thunder_lord");
    new creature_script<npc_lightning_guardian>("npc_lightning_guardian");
    new creature_script<npc_manchu>("npc_manchu");
    new creature_script<npc_weishen>("npc_weishen");
    new creature_script<npc_untrained_quilen>("npc_untrained_quilen");
    new creature_script<npc_shanze_celestial_shaper>("npc_shanze_celestial_shaper");
    new creature_script<npc_dark_ritualist>("npc_dark_ritualist");
    new creature_script<npc_ritual_guard>("npc_ritual_guard");
    new creature_script<npc_muckbat>("npc_muckbat");
    new creature_script<npc_skittering_spiderling>("npc_skittering_spiderling");
    new creature_script<npc_rotting_scavenger>("npc_rotting_scavenger");

    new spell_script<spell_focused_lightning_aoe_trash>("spell_focused_lightning_aoe_trash");
    new aura_script<spell_storm_weapon>("spell_storm_weapon");
    new spell_script<spell_storm_energy>("spell_storm_energy");
    new spell_storm_weapon_aura();
    new spell_script<spell_flame_chanters>("spell_flame_chanters");
    new aura_script<spell_fiery_core>("spell_fiery_core");
    new spell_script<spell_judgment_of_the_loa>("spell_judgment_of_the_loa");
    new spell_script<spell_conjure_elementals>("spell_conjure_elementals");
    new spell_script<spell_summon_shadowy_minions>("spell_summon_shadowy_minions");
    new aura_script<spell_leishen_call_lightning>("spell_leishen_call_lightning");
    new aura_script<spell_waterspout_back>("spell_waterspout_back");
    new spell_script<spell_eternal_siphon_life>("spell_eternal_siphon_life");
    new spell_script<spell_sonic_call>("spell_sonic_call");
    new spell_script<spell_luciferase>("spell_luciferase");
    new aura_script<spell_devoured>("spell_devoured");
    new aura_script<spell_conductive_shield>("spell_conductive_shield");
    new spell_script<spell_triumphant_roar>("spell_triumphant_roar");
    new spell_script<spell_cosmic_strike_summon>("spell_cosmic_strike_summon");
    new spell_script<spell_range_ritual_lightning>("spell_range_ritual_lightning");
    new aura_script<spell_putrify_proc>("spell_putrify_proc");
    new spell_script<spell_corpse_spider_cocoon>("spell_corpse_spider_cocoon");
    new aura_script<spell_throne_of_thunder_displace>("spell_throne_of_thunder_displace");
    new go_ancient_mogu_bell();
    new go_displacement_pad();
    new AreaTrigger_under_horridon_way();
    new AreaTrigger_under_lei_shen();
    new atrigger_script<sat_dark_winds>("sat_dark_winds");
    new atrigger_script<sat_fungal_explosion>("sat_fungal_explosion");
    new atrigger_script<sat_lightning_storm>("sat_lightning_storm");
    new atrigger_script<sat_choking_gas>("sat_choking_gas");
    new atrigger_script<sat_abrasive_radula>("sat_abrasive_radula");
    new atrigger_script<sat_corpse_spider_web>("sat_corpse_spider_web");
}