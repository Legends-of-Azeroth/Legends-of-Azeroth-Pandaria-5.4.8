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
#include "LFGMgr.h"
#include "Player.h"
#include "Group.h"
#include "CreatureTextMgr.h"

enum Yells
{
    SAY_PLAYER_TEXT_1     = 0,
    SAY_PLAYER_TEXT_2     = 1,
    SAY_PLAYER_TEXT_3     = 2,
    EMOTE_EARTHEN_ASSAULT = 3,
    EMOTE_SUBMERGED       = 4,
    EMOTE_EMERGE_SOON     = 5
};

enum Spells
{
    // Ahune
    SPELL_AHUNE_SHIELD            = 45954,
    SPELL_SELF_STUN               = 46416,
    SPELL_SUBMERGE                = 37550,
    SPELL_EMERGE                  = 50142,
    SPELL_RESURFACE               = 46402,
    SPELL_COLD_SLAP               = 46145,
    SPELL_SUMMON_HAILSTONE        = 46176,
    SPELL_SUMMON_COLDWEAVE        = 46143,
    SPELL_SUMMON_FROSTWIND        = 46382,
    SPELL_MAKE_BONFIRE            = 45930,
    SPELL_GHOST_DISGUISE          = 46786,
    SPELL_ICE_BOMBARDMENT         = 46396,
    SPELL_SUMMON_LOOT             = 45939,

    // Slippery floor
    SPELL_SLIP                    = 45947,
    SPELL_YOU_SLIPPED             = 45946,

    // Ice Spear
    SPELL_SUMMON_ICE_SPEAR_BUNNY  = 46359,
    SPELL_SUMMON_ICE_SPEAR_OBJECT = 46369,
    SPELL_ICE_SPEAR_VISUAL        = 75498,
    SPELL_ICE_SPEAR_KNOCKBACK     = 46360,

    // [PH] Spank Target Bunny
    SPELL_SPANK_KNOCK_TO          = 46734,

    // Earthen Ring
    SPELL_BEAM_CAST               = 45576,
    SPELL_BEAM                    = 46363,
    SPELL_BEAM_EFFECT             = 46336
};

enum Actions
{
    TYPE_STARTER_GUID  = 100,
    ACTION_START_EVENT = 200
};

enum Events
{
    EVENT_INTRO_1             = 100,
    EVENT_INTRO_2             = 110,
    EVENT_INTRO_3             = 120,
    EVENT_INTRO_DONE          = 130,
    EVENT_EMERGE              = 200,
    EVENT_COLD_SLAP           = 210,
    EVENT_ICE_SPEAR           = 220,
    EVENT_SUMMON_HAILSTONE    = 230,
    EVENT_SUMMON_COLDWEAVE    = 240,
    EVENT_EARTHEN_RING_ATTACK = 250,
    EVENT_SUBMERGE            = 300,
    EVENT_ICE_CORE            = 310,
    EVENT_GHOST_VISUAL        = 320,
    EVENT_GHOST_VISUAL_REMOVE = 330,
    EVENT_EMERGE_SOON         = 340
};

enum Phases
{
    PHASE_AHUNE = 1,
    PHASE_CORE  = 2
};

enum Creatures
{
    NPC_AHUNE                    = 25740,
    NPC_EARTHEN_RING_TOTEM       = 25961,
    NPC_EARTHEN_RING_FLAMECALLER = 25754,
    NPC_FROZEN_CORE              = 25865
};

enum GameObjects
{
    GO_ICE_STONE = 187882,
    GO_ICE_SPEAR = 188077
};

enum LocPoints
{
    AHUNE     = 0,
    ICE_STONE = 1,
};

const Position SpawnLoc[] =
{
    { -97.191f, -212.236f, -1.222f, 1.637f }, // 0 - AHUNE
    { -73.497f, -155.976f, -1.898f, 3.238f }, // 1 - ICE_STONE
};

const Position EarthenRingLoc[] =
{
    { -135.871f, -145.185f, -1.852f, 5.427f }, // Totem A
    { -129.179f, -141.099f, -1.992f, 5.160f }, // Totem B
    { -122.147f, -136.741f, -2.286f, 5.133f }, // Totem C
    { -140.735f, -139.580f, -1.592f, 5.427f }, // Flamecaller A
    { -134.587f, -134.331f, -1.856f, 5.337f }, // Flamecaller B
    { -128.186f, -130.267f, -2.017f, 5.460f }, // Flamecaller C
};

// 25740
class boss_frostlord_ahune : public CreatureScript
{
    public:
        boss_frostlord_ahune() : CreatureScript("boss_frostlord_ahune") { }

        struct boss_frostlord_ahuneAI : public ScriptedAI
        {
            boss_frostlord_ahuneAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            EventMap events;
            SummonList summons;

            void Reset() override
            {
                if (!me->IsAlive())
                    return;

                _isStarted = false;
                _starterGUID = ObjectGuid::Empty;
                _introDone = false;
                _firstPhase = true;
                _earthenRingAttacks = 0;

                _totemCount = 0;
                _totemAGUID = ObjectGuid::Empty;
                _totemBGUID = ObjectGuid::Empty;
                _totemCGUID = ObjectGuid::Empty;
                _flamecallerCount = 0;
                _flamecallerAGUID = ObjectGuid::Empty;
                _flamecallerBGUID = ObjectGuid::Empty;
                _flamecallerCGUID = ObjectGuid::Empty;

                _icyCoreGUID = ObjectGuid::Empty;

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);

                me->SetCorpseDelay(60);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                me->SetVisible(false);
                DoCast(me, SPELL_SUBMERGE, true);
                me->SummonGameObject(GO_ICE_STONE, SpawnLoc[ICE_STONE].GetPositionX(), SpawnLoc[ICE_STONE].GetPositionY(), SpawnLoc[ICE_STONE].GetPositionZ(), 0, { }, 0);

                events.Reset();
                summons.DespawnAll();

                std::list<GameObject*> spears;
                GetGameObjectListWithEntryInGrid(spears, me, GO_ICE_SPEAR, 200.0f);
                for (auto&& spear : spears)
                    spear->Delete();
            }

            void SetGUID(ObjectGuid guid, int32 type) override
            {
                if (type == TYPE_STARTER_GUID)
                    _starterGUID = guid;
            }

            void DoAction(int32 const actionId) override
            {
                if (!_isStarted && actionId == ACTION_START_EVENT)
                {
                    _isStarted = true;
                    events.ScheduleEvent(EVENT_INTRO_1, 1 * IN_MILLISECONDS);

                    for (int i = 0; i < 3; ++i)
                    {
                        me->SummonCreature(NPC_EARTHEN_RING_TOTEM, EarthenRingLoc[i], TEMPSUMMON_MANUAL_DESPAWN);
                        me->SummonCreature(NPC_EARTHEN_RING_FLAMECALLER, EarthenRingLoc[i + 3], TEMPSUMMON_MANUAL_DESPAWN);
                    }
                }
            }

            void AttackStart(Unit* target) override
            {
                if (target && me->Attack(target, true))
                    DoStartNoMovement(target);
            }

            void JustSummoned(Creature* summoned) override
            {
                summons.Summon(summoned);
                switch (summoned->GetEntry())
                {
                    case NPC_EARTHEN_RING_TOTEM:
                        summoned->SetObjectScale(2.0f);
                        switch (_totemCount)
                        {
                            case 0:
                                _totemAGUID = summoned->GetGUID();
                                break;
                            case 1:
                                _totemBGUID = summoned->GetGUID();
                                break;
                            case 2:
                                _totemCGUID = summoned->GetGUID();
                                break;
                            default:
                                return;
                        }
                        ++_totemCount;
                        return;
                    case NPC_EARTHEN_RING_FLAMECALLER:
                        switch (_flamecallerCount)
                        {
                            case 0:
                                _flamecallerAGUID = summoned->GetGUID();
                                break;
                            case 1:
                                _flamecallerBGUID = summoned->GetGUID();
                                break;
                            case 2:
                                _flamecallerCGUID = summoned->GetGUID();
                                break;
                            default:
                                return;
                        }
                        ++_flamecallerCount;
                        return;
                    case NPC_FROZEN_CORE:
                        _icyCoreGUID = summoned->GetGUID();
                        summoned->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                        summoned->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                        summoned->SetHealth(me->GetHealth());
                        summoned->SetReactState(REACT_PASSIVE);
                        break;
                    default:
                        break;
                }
                summoned->SetCorpseDelay(3);
                summoned->SetInCombatWithZone();
            }

            void SummonedCreatureDespawn(Creature* summoned) override
            {
                if (!me->IsAlive())
                    return;

                if (summoned->GetEntry() == NPC_FROZEN_CORE)
                {
                    _icyCoreGUID = ObjectGuid::Empty;
                    me->SetHealth(summoned->GetHealth());
                }
            }

            void SummonedCreatureDies(Creature* summoned, Unit* /*killer*/) override
            {
                if (summoned->GetEntry() == NPC_FROZEN_CORE)
                {
                    _icyCoreGUID = ObjectGuid::Empty;
                    me->DealDamage(me, me->GetHealth());
                }
            }

            void EnterEvadeMode() override
            {
                summons.DespawnAll();
                ScriptedAI::EnterEvadeMode();
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->GetHealth() <= damage)
                    me->ResetPlayerDamageReq();
            }

            void JustDied(Unit* /*killer*/) override
            {
                events.Reset();
                summons.DespawnAll();
                DoCast(me, SPELL_SUMMON_LOOT);

                Map* map = me->GetMap();
                if (map->IsDungeon())
                    if (auto instance = map->ToInstanceMap()->GetInstanceScript())
                        instance->DoFinishLFGDungeon(286);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (_introDone && !UpdateVictim())
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_1:
                            events.ScheduleEvent(EVENT_INTRO_2, 3 * IN_MILLISECONDS);
                            me->SetVisible(true);
                            PlayerSay(_starterGUID, SAY_PLAYER_TEXT_1);
                            break;
                        case EVENT_INTRO_2:
                            events.ScheduleEvent(EVENT_INTRO_3, 3 * IN_MILLISECONDS);
                            PlayerSay(_starterGUID, SAY_PLAYER_TEXT_2);
                            break;
                        case EVENT_INTRO_3:
                            events.ScheduleEvent(EVENT_INTRO_DONE, 2 * IN_MILLISECONDS);
                            PlayerSay(_starterGUID, SAY_PLAYER_TEXT_3);
                            break;
                        case EVENT_INTRO_DONE:
                            _introDone = true;
                            events.ScheduleEvent(EVENT_EMERGE, 1 * IN_MILLISECONDS);
                            me->SetVisible(true);
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->SetInCombatWithZone();
                            Talk(EMOTE_EARTHEN_ASSAULT);
                            break;
                        case EVENT_EMERGE:
                            _earthenRingAttacks = 0;
                            events.SetPhase(PHASE_AHUNE);
                            events.ScheduleEvent(EVENT_COLD_SLAP, 1 * IN_MILLISECONDS, 0, PHASE_AHUNE);
                            events.ScheduleEvent(EVENT_ICE_SPEAR, 7 * IN_MILLISECONDS, 0, PHASE_AHUNE);
                            events.ScheduleEvent(EVENT_SUMMON_HAILSTONE, 2 * IN_MILLISECONDS, 0, PHASE_AHUNE);
                            events.ScheduleEvent(EVENT_SUMMON_COLDWEAVE, 3 * IN_MILLISECONDS, 0, PHASE_AHUNE);
                            events.ScheduleEvent(EVENT_EARTHEN_RING_ATTACK, 10 * IN_MILLISECONDS);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            me->RemoveAurasDueToSpell(SPELL_SELF_STUN);
                            me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                            DoCast(SPELL_AHUNE_SHIELD);
                            DoCast(SPELL_EMERGE);
                            DoCast(SPELL_RESURFACE);
                            break;
                        case EVENT_COLD_SLAP:
                            events.ScheduleEvent(EVENT_COLD_SLAP, 1 * IN_MILLISECONDS, 0, PHASE_AHUNE);
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 9.0f, true, -SPELL_YOU_SLIPPED))
                                DoCast(target, SPELL_COLD_SLAP, true);
                            break;
                        case EVENT_ICE_SPEAR:
                            events.ScheduleEvent(EVENT_ICE_SPEAR, 7 * IN_MILLISECONDS, 0, PHASE_AHUNE);
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                DoCast(target, SPELL_SUMMON_ICE_SPEAR_BUNNY);

                            break;
                        case EVENT_SUMMON_HAILSTONE:
                            DoCast(SPELL_SUMMON_HAILSTONE);
                            break;
                        case EVENT_SUMMON_COLDWEAVE:
                            me->CastSpell(me, SPELL_SUMMON_COLDWEAVE, true);
                            me->CastSpell(me, SPELL_SUMMON_COLDWEAVE, true);
                            if (_firstPhase)
                            {
                                events.ScheduleEvent(EVENT_SUMMON_COLDWEAVE, 6 * IN_MILLISECONDS, 0, PHASE_AHUNE);
                            }
                            else
                            {
                                events.ScheduleEvent(EVENT_SUMMON_COLDWEAVE, 6 * IN_MILLISECONDS, 0, PHASE_AHUNE);
                                me->CastSpell(me, SPELL_SUMMON_FROSTWIND, true);
                            }
                            break;
                        case EVENT_EARTHEN_RING_ATTACK:
                            ++_earthenRingAttacks;
                            EarthenAttack(_totemAGUID, _flamecallerAGUID);
                            if (_earthenRingAttacks > 3)
                                EarthenAttack(_totemBGUID, _flamecallerBGUID);
                            if (_earthenRingAttacks > 5)
                                EarthenAttack(_totemCGUID, _flamecallerCGUID);

                            if (_earthenRingAttacks > 7)
                                events.ScheduleEvent(EVENT_SUBMERGE, 10 * IN_MILLISECONDS, 0, PHASE_AHUNE);
                            else
                                events.ScheduleEvent(EVENT_EARTHEN_RING_ATTACK, 10 * IN_MILLISECONDS, 0, PHASE_AHUNE);

                            break;
                        case EVENT_SUBMERGE:
                            _firstPhase = false;
                            events.SetPhase(PHASE_CORE);
                            events.ScheduleEvent(EVENT_ICE_CORE, 5 * IN_MILLISECONDS, 0, PHASE_CORE);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            me->RemoveAurasDueToSpell(SPELL_AHUNE_SHIELD);
                            me->SummonCreature(NPC_FROZEN_CORE, *me, TEMPSUMMON_TIMED_DESPAWN, 40 * IN_MILLISECONDS);
                            DoCast(SPELL_MAKE_BONFIRE);
                            DoCast(SPELL_SUBMERGE);
                            Talk(EMOTE_SUBMERGED);
                            break;
                        case EVENT_ICE_CORE:
                            events.ScheduleEvent(EVENT_GHOST_VISUAL, 3 * IN_MILLISECONDS, 0, PHASE_CORE);
                            events.ScheduleEvent(EVENT_EMERGE_SOON, 30 * IN_MILLISECONDS, 0, PHASE_CORE);
                            events.ScheduleEvent(EVENT_EMERGE, 35 * IN_MILLISECONDS, 0, PHASE_CORE);
                            if (Creature* totemA = ObjectAccessor::GetCreature(*me, _totemAGUID))
                                totemA->RemoveAurasDueToSpell(SPELL_BEAM);
                            if (Creature* totemB = ObjectAccessor::GetCreature(*me, _totemBGUID))
                                totemB->RemoveAurasDueToSpell(SPELL_BEAM);
                            if (Creature* totemC = ObjectAccessor::GetCreature(*me, _totemCGUID))
                                totemC->RemoveAurasDueToSpell(SPELL_BEAM);

                            DoCast(me, SPELL_SELF_STUN, true);
                            break;
                        case EVENT_GHOST_VISUAL:
                            events.ScheduleEvent(EVENT_GHOST_VISUAL_REMOVE, 2 * IN_MILLISECONDS, 0, PHASE_CORE);
                            if (_icyCoreGUID)
                                if (Creature* icyCore = ObjectAccessor::GetCreature(*me, _icyCoreGUID))
                                    icyCore->CastSpell(me, SPELL_GHOST_DISGUISE);

                            break;
                        case EVENT_GHOST_VISUAL_REMOVE:
                            events.ScheduleEvent(EVENT_GHOST_VISUAL, 4 * IN_MILLISECONDS, 0, PHASE_CORE);
                            if (_icyCoreGUID)
                            {
                                if (Creature* icyCore = ObjectAccessor::GetCreature(*me, _icyCoreGUID))
                                {
                                    icyCore->RemoveAurasDueToSpell(SPELL_GHOST_DISGUISE);
                                    icyCore->CastSpell(icyCore, SPELL_ICE_BOMBARDMENT);
                                }
                            }
                            break;
                        case EVENT_EMERGE_SOON:
                            Talk(EMOTE_EMERGE_SOON);
                            break;
                    }
                }

            }

            void PlayerSay(ObjectGuid playerGUID, uint8 sayText) // TODO: should be CHAT_MSG_SAY, not CHAT_MSG_ADDON
            {
                if (Player* player = ObjectAccessor::GetPlayer(*me, playerGUID))
                    sCreatureTextMgr->SendChat(me, sayText, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_NORMAL, 0, TEAM_OTHER, false, player);
            }

            void EarthenAttack(ObjectGuid totemGUID, ObjectGuid flamecallerGUID)
            {
                if (Creature* flamecaller = ObjectAccessor::GetCreature(*me, flamecallerGUID))
                {
                    if (Creature* totem = ObjectAccessor::GetCreature(*me, totemGUID))
                    {
                        flamecaller->CastSpell(flamecaller, SPELL_BEAM_CAST, false);
                        totem->CastSpell(me, SPELL_BEAM, false);
                        DoCast(SPELL_BEAM_EFFECT);
                    }
                }
            }

            private:
                bool _isStarted;
                ObjectGuid _starterGUID;
                bool _introDone;
                bool _firstPhase;
                uint8 _earthenRingAttacks;

                uint8 _totemCount;
                ObjectGuid _totemAGUID;
                ObjectGuid _totemBGUID;
                ObjectGuid _totemCGUID;
                uint8 _flamecallerCount;
                ObjectGuid _flamecallerAGUID;
                ObjectGuid _flamecallerBGUID;
                ObjectGuid _flamecallerCGUID;

                ObjectGuid _icyCoreGUID;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_frostlord_ahuneAI(creature);
        }
};

// 25985
class npc_ahune_ice_spear : public CreatureScript
{
    public:
        npc_ahune_ice_spear() : CreatureScript("npc_ahune_ice_spear") { }

        struct npc_ahune_ice_spearAI : public ScriptedAI
        {
            npc_ahune_ice_spearAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                _spikeTimer = 2.5 * IN_MILLISECONDS;
                _spiked = false;

                DoCast(me, SPELL_ICE_SPEAR_VISUAL);
                DoCast(me, SPELL_SUMMON_ICE_SPEAR_OBJECT, true);
            }

            void AttackStart(Unit* target) override
            {
                if (target && me->Attack(target, true))
                    DoStartNoMovement(target);
            }

            void UpdateAI(uint32 diff) override
            {
                if (_spikeTimer)
                {
                    if (_spikeTimer <= diff)
                    {
                        if (GameObject* spike = me->FindNearestGameObject(GO_ICE_SPEAR, 10.0f))
                        {
                            if (!_spiked)
                            {
                                _spiked = true;
                                _spikeTimer = 3.5 * IN_MILLISECONDS;
                                if (Unit* target = SelectTarget(SELECT_TARGET_NEAREST, 0, 3.0f, true))
                                    DoCast(target, SPELL_ICE_SPEAR_KNOCKBACK, true);

                                spike->UseDoorOrButton();
                            }
                            else
                            {
                                spike->Delete();
                                me->DespawnOrUnsummon();
                            }
                        }
                    }
                    else
                        _spikeTimer -= diff;
                }
            }

            private:
                uint32 _spikeTimer;
                bool _spiked;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_ahune_ice_spearAI(creature);
        }
};

// 25865
struct npc_frozen_core : public ScriptedAI
{
    npc_frozen_core(Creature* c) : ScriptedAI(c) { }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (me->GetHealth() <= damage)
        {
            me->ResetPlayerDamageReq();
            if (Player* player = me->GetMap()->GetFirstPlayerInInstance())
                player->RewardPersonalLootAndCurrency(me, NPC_AHUNE);
        }
    }

    void UpdateAI(uint32 diff) override { }
};

// 187882
class go_ahune_ice_stone : public GameObjectScript
{
    public:
        go_ahune_ice_stone() : GameObjectScript("go_ahune_ice_stone") { }

        bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 /*action*/)
        {
            player->PlayerTalkClass->ClearMenus();

            if (Creature* ahune = go->FindNearestCreature(NPC_AHUNE, 200.0f, true))
            {
                ahune->AI()->SetGUID(player->GetGUID(), TYPE_STARTER_GUID);
                ahune->AI()->DoAction(ACTION_START_EVENT);

                player->PlayerTalkClass->SendCloseGossip();
                go->Delete();
            }

            return true;
        }
};

// 46320 - Slippery Floor Ambient Periodic
class spell_ahune_slippery_floor : public SpellScript
{
    PrepareSpellScript(spell_ahune_slippery_floor);

    void HandleScript(SpellEffIndex)
    {
        if (Unit* target = GetHitUnit())
        {
            if (target->GetTypeId() == TYPEID_PLAYER && target->isMoving())
            {
                target->CastSpell(target, SPELL_SLIP, true);
                target->CastSpell(target, SPELL_YOU_SLIPPED, true);
            }
        }
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_ahune_slippery_floor::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 46735 - Spank - Force Bunny To Knock You To
class spell_spank_force_bunny_to_knock_you_to : public SpellScript
{
    PrepareSpellScript(spell_spank_force_bunny_to_knock_you_to);

    void HandleScript(SpellEffIndex)
    {
        GetHitUnit()->CastSpell(GetCaster(), SPELL_SPANK_KNOCK_TO, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spank_force_bunny_to_knock_you_to::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

void AddSC_boss_frostlord_ahune()
{
    new boss_frostlord_ahune();
    new npc_ahune_ice_spear();
    new creature_script<npc_frozen_core>("npc_frozen_core");
    new go_ahune_ice_stone();
    new spell_script<spell_ahune_slippery_floor>("spell_ahune_slippery_floor");
    new spell_script<spell_spank_force_bunny_to_knock_you_to>("spell_spank_force_bunny_to_knock_you_to");
};
