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
#include "GameEventMgr.h"
#include "Transport.h"
#include "Random.h"

// Darkmoon Deathmatch
enum DarkmoonDeathmatchData
{
    GAME_EVENT_DARKMOON_DEATHMATCH_ANNOUNCE           = 82,
    GAME_EVENT_DARKMOON_DEATHMATCH_RUN                = 83,

    NPC_DARKMOON_DEATHMATCH_ANNOUNCER                 = 55402,

    GO_DARKMOON_DEATHMATCH_CHEST                      = 209620,

    SPELL_ENTER_DEATHMATCH                            = 108919,
    SPELL_EXIT_DEATHMATCH                             = 108923
};

enum Yells
{
    SAY_ANNOUNCE                                      = 0,
    SAY_RUN                                           = 1,
    SAY_END                                           = 2
};

class npc_darkmoon_deathmatch_announcer : public CreatureScript
{
    public:
        npc_darkmoon_deathmatch_announcer() : CreatureScript("npc_darkmoon_deathmatch_announcer") { }

        struct npc_darkmoon_deathmatch_announcerAI : public ScriptedAI
        {
            npc_darkmoon_deathmatch_announcerAI(Creature* creature) : ScriptedAI(creature)
            {
                chestGuid = ObjectGuid::Empty;
                winnerGuid = ObjectGuid::Empty;
                chestNeedsRespawn = false;
            }

            void UpdateAI(uint32 diff) override
            {
                if (chestNeedsRespawn && !chestGuid)
                {
                    if (GameObject* chest = GetClosestGameObjectWithEntry(me, GO_DARKMOON_DEATHMATCH_CHEST, 100))
                    {
                        chestGuid = chest->GetGUID();
                        chest->Respawn();
                    }
                }
            }

            void sOnGameEvent(bool start, uint16 eventId) override
            {
                if (!start)
                    return;

                if (eventId == GAME_EVENT_DARKMOON_DEATHMATCH_ANNOUNCE)
                    Talk(SAY_ANNOUNCE);
                else if (eventId == GAME_EVENT_DARKMOON_DEATHMATCH_RUN)
                {
                    Talk(SAY_RUN);
                    if (GameObject* chest = ObjectAccessor::GetGameObject(*me, chestGuid))
                        chest->Respawn();
                    else
                    {
                        chestGuid = ObjectGuid::Empty;
                        chestNeedsRespawn = true;
                    }
                }
            }

            void SetWinner(ObjectGuid winnerGuid)
            {
                if (Player* player = ObjectAccessor::GetPlayer(*me, winnerGuid))
                    Talk(SAY_END, player);
                sGameEventMgr->StopEvent(GAME_EVENT_DARKMOON_DEATHMATCH_RUN);
            }

        private:
            ObjectGuid chestGuid;
            ObjectGuid winnerGuid;
            bool chestNeedsRespawn;
        };
        
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_darkmoon_deathmatch_announcerAI(creature);
        }
};

enum MoonfangSpells
{
    SPELL_FREE_YOUR_MIND        = 145111,
    SPELL_FREE_YOUR_MIND_EFF    = 145112,
    SPELL_MOONFANG_CURSE        = 144590,
    SPELL_MOONFANG_TEARS        = 144702,
    SPELL_MOONFANG_TEARS_EFF    = 144579,
    SPELL_FANGS_OF_THE_MOON     = 144700,
    SPELL_CALL_THE_PACK         = 144602,
    SPELL_CALL_THE_PACK_EFF     = 144604,
    SPELL_LEAP_FOR_THE_KILL     = 144546,
    SPELL_LEAP_FOR_THE_KILL_EFF = 144535,
};

enum MoonfangEvents
{
    EVENT_MOONFANG_CURSE = 1,
    EVENT_FANGS_OF_THE_MOON,
    EVENT_CALL_THE_PACK,
    EVENT_LEAP_FOR_THE_KILL,
    EVENT_MOONFANG_TEARS,
};

// Moonfang 71992
struct npc_darkmoon_moonfang : public customCreatureAI
{
    npc_darkmoon_moonfang(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_MOONFANG_CURSE, 19s + 500ms);
        events.ScheduleEvent(EVENT_FANGS_OF_THE_MOON, 8s + 500ms);
        events.ScheduleEvent(EVENT_MOONFANG_TEARS, 11s);
        events.ScheduleEvent(EVENT_CALL_THE_PACK, 25s);
        events.ScheduleEvent(EVENT_LEAP_FOR_THE_KILL, 15s);
    }

    void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
    {
        ScriptedAI::EnterEvadeMode();
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
            ExecuteTargetEvent(SPELL_MOONFANG_CURSE, 21 * IN_MILLISECONDS, EVENT_MOONFANG_CURSE, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_CALL_THE_PACK, 32 * IN_MILLISECONDS, EVENT_CALL_THE_PACK, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_FANGS_OF_THE_MOON, 12.5 * IN_MILLISECONDS, EVENT_FANGS_OF_THE_MOON, eventId);
            ExecuteTargetEvent(SPELL_LEAP_FOR_THE_KILL, 16 * IN_MILLISECONDS, EVENT_LEAP_FOR_THE_KILL, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_MOONFANG_TEARS, 20 * IN_MILLISECONDS, EVENT_MOONFANG_TEARS, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

class go_darkmoon_treasure_chest : public GameObjectScript
{
    public:
        go_darkmoon_treasure_chest() : GameObjectScript("go_darkmoon_treasure_chest")
        {
            lastLooter = ObjectGuid::Empty;
        }

        void OnLootStateChanged(GameObject* go, uint32 state, Unit* unit) override
        {
            if (state == GO_ACTIVATED && unit)
                lastLooter = unit->GetGUID();

            if (state == GO_JUST_DEACTIVATED)
                if (Creature* announcer = GetClosestCreatureWithEntry(go, NPC_DARKMOON_DEATHMATCH_ANNOUNCER, 100, true))
                    if (npc_darkmoon_deathmatch_announcer::npc_darkmoon_deathmatch_announcerAI* ai = CAST_AI(npc_darkmoon_deathmatch_announcer::npc_darkmoon_deathmatch_announcerAI, announcer->GetAI()))
                        ai->SetWinner(lastLooter);
        }

    private:
        ObjectGuid lastLooter;
};

class spell_darkmoon_deathmatch : public SpellScriptLoader
{
    public:
        spell_darkmoon_deathmatch() : SpellScriptLoader("spell_darkmoon_deathmatch") { }

        class spell_darkmoon_deathmatch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_darkmoon_deathmatch_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                if (caster->GetPositionY() < 6402.79f)
                    caster->CastSpell(caster, SPELL_ENTER_DEATHMATCH);
                else
                    caster->CastSpell(caster, SPELL_EXIT_DEATHMATCH);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_darkmoon_deathmatch_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_darkmoon_deathmatch_SpellScript();
        }
};

std::vector<std::pair<uint32, Position>> passengers
{
    { 68233, { 6.57632f, -3.6821f,  1.201352f, 4.0477900f } },
    { 68238, { -0.1620f, 7.45662f,  1.201352f, 0.0058666f } },
    { 68231, { -6.6299f, 3.93165f,  1.201352f, 1.1105790f } },
    { 68239, { 6.56192f, 4.00892f,  1.201351f, 5.2554440f } },
    { 68232, { -6.3160f, -3.8700f,  1.201352f, 2.0703380f } },
    { 68238, { 0.27098f, -7.2955f,  1.201352f, 3.1815390f } },
};

class transport_merry_go_round : public TransportScript
{
    public:
        transport_merry_go_round() : TransportScript("transport_merry_go_round") { }

        void OnCreate(Transport* transport) override
        {
            for (auto&& passenger : passengers)
            {
                Map* map = transport->GetMap();
                Creature* creature = new Creature();
                if (!creature->Create(map->GenerateLowGuid<HighGuid::Unit>(), map, PHASEMASK_NORMAL, passenger.first, 0, 0, passenger.second.GetPositionX(), passenger.second.GetPositionY(), passenger.second.GetPositionZ(), passenger.second.GetOrientation()))
                {
                    delete creature;
                    return;
                }

                creature->SetRespawnTime(0);

                float x, y, z, o;
                creature->GetPosition(x, y, z, o);

                creature->SetTransport(transport);
                creature->m_movementInfo.transport.guid = transport->GetGUID();
                creature->m_movementInfo.transport.pos.Relocate(x, y, z, o);
                transport->CalculatePassengerPosition(x, y, z, &o);
                creature->Relocate(x, y, z, o);

                bool spawned = false;
                if (creature->IsInWorld())
                {
                    creature->UpdateObjectVisibility(true);
                    spawned = true;
                }
                else if (creature->GetMap()->AddToMap(creature))
                    spawned = true;
                else
                    spawned = false;

                if (creature->IsPositionValid() && spawned)
                    transport->AddPassenger(creature);
                else if (!creature->GetMap()->AddToMap(creature))
                    delete creature;
            }
        }
};

// 134815 - WHEE!
class spell_whee_trigger : public SpellScript
{
    PrepareSpellScript(spell_whee_trigger);

    enum
    {
        SPELL_WHEE                  = 46668,
        ITEM_DARKMOON_RIDE_TICKET   = 81055,
        MAX_DURATION                = HOUR * IN_MILLISECONDS,
    };

    void HandleHit()
    {
        if (Player* player = GetHitUnit()->ToPlayer())
        {
            if (Aura* aura = player->GetAura(SPELL_WHEE))
            {
                int32 duration = aura->GetDuration();
                duration += MINUTE * IN_MILLISECONDS;
                duration = std::min(duration, int32(MAX_DURATION));
                aura->SetMaxDuration(duration);
                aura->SetDuration(duration);
            }
            else if (player->HasItemCount(ITEM_DARKMOON_RIDE_TICKET))
            {
                player->CastSpell(player, SPELL_WHEE, true);
                player->DestroyItemCount(ITEM_DARKMOON_RIDE_TICKET, 1, true);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_whee_trigger::HandleHit);
    }
};

// Free your Mind 145111
class spell_darkmoon_free_your_mind : public SpellScript
{
    PrepareSpellScript(spell_darkmoon_free_your_mind);

    void HandleAfterCast()
    {
        if (roll_chance_i(50))
            GetCaster()->CastSpell(GetCaster(), SPELL_FREE_YOUR_MIND_EFF, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_darkmoon_free_your_mind::HandleAfterCast);
    }
};

// Moonfang Tears 144702
class spell_darkmoon_moonfang_tears : public SpellScript
{
    PrepareSpellScript(spell_darkmoon_moonfang_tears);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[effIdx].BasePoints, true);
    }

    void CountTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;

        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && GetCaster() && target->ToPlayer()->GetExactDist2d(GetCaster()) < 12.5f; });

        if (targets.size() >= 3)
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (targets.size() > 3)
            Trinity::Containers::RandomResizeList(targets, 3);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_darkmoon_moonfang_tears::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_darkmoon_moonfang_tears::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Moonfang Curse 144590
class spell_darkmoon_moonfang_curse : public SpellScript
{
    PrepareSpellScript(spell_darkmoon_moonfang_curse);

    std::list<WorldObject*> copyTargets;

    void CountTargets(std::list<WorldObject*>& targets)
    {
        uint32 targetsCount = targets.size() % 2 == 0 ? targets.size() / 2 : (targets.size() - 1) / 2;

        if (targets.size() > targetsCount)
            Trinity::Containers::RandomResizeList(targets, targetsCount);

        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_darkmoon_moonfang_curse::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_darkmoon_moonfang_curse::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_darkmoon_moonfang_curse::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_darkmoon_moonfang_curse::CopyTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

void AddSC_darkmoon_island()
{
    new npc_darkmoon_deathmatch_announcer();
    new creature_script<npc_darkmoon_moonfang>("npc_darkmoon_moonfang");
    new go_darkmoon_treasure_chest();
    new spell_darkmoon_deathmatch();
    new transport_merry_go_round();
    new spell_script<spell_whee_trigger>("spell_whee_trigger");
    new spell_script<spell_darkmoon_free_your_mind>("spell_darkmoon_free_your_mind");
    new spell_script<spell_darkmoon_moonfang_tears>("spell_darkmoon_moonfang_tears");
    new spell_script<spell_darkmoon_moonfang_curse>("spell_darkmoon_moonfang_curse");
}
