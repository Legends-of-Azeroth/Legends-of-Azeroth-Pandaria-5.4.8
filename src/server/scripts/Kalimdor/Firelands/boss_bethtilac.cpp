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
#include "firelands.h"

enum Spells
{
    SPELL_EMBER_FLARE_1             = 98934, // 1 phase
    SPELL_EMBER_FLARE_2             = 99859, // 2 phase
    SPELL_METEOR_BURN_DUMMY         = 98534,
    SPELL_VENOM_RAIN                = 99333,
    SPELL_SPIDERWEB_FILAMENT_UP     = 97196,
    SPELL_SPIDERWEB_FILAMENT_DOWN   = 98149,
    SPELL_SPIDERWEB_FILAMENT_PET    = 97182,
    SPELL_SPIDERWEB_FILAMENT_SUMMON = 98148,
    SPELL_SPIDERWEB_FILAMENT_ANY    = 98623,
    SPELL_CREEPER                   = 98858,
    SPELL_TEMPERAMENT               = 98958,
    SPELL_SMOLDERING_DEVASTATION    = 99052,
    SPELL_BURNING_ACID              = 98471,
    SPELL_FIERY_WEB_SPIN            = 97202,
    SPELL_BOILING_SPATTER           = 99463,
    SPELL_FIXATE                    = 99526,
    SPELL_FIXATE_SELF               = 99599,
    SPELL_FIXATE_BROODLING          = 99999,
    SPELL_LEECH_VENOM               = 99411,
    SPELL_VOLATILE_BURST            = 99990,
    SPELL_THE_WIDOW_KISS            = 99476,
    SPELL_FRENZY                    = 99497,
    SPELL_ZERO_POWER                = 96301,
    SPELL_CONSUME_DRONE             = 99304,
    SPELL_ENERGIZE                  = 99211, // Drone
    SPELL_VOLATILE_POISON           = 99276,
};

enum Adds
{
    NPC_WEB_RIP                 = 53450, // 99071
    NPC_WEB_RIP_1               = 53474, // 99591
    NPC_CINDERWEB_SPINNER       = 52524, // 98858, 98958
    NPC_CINDERWEB_DRONE         = 52581, // 99211
    NPC_CINDERWEB_SPIDERLING    = 52447,
    NPC_ENGORGED_BROODLING      = 53745, // 99982 
    NPC_SPIDERWEB_FILAMENT      = 53082, // 98149 
    NPC_CRACKLING_FLAME         = 53433, // 98570
    NPC_SPIDERLING_STALKER      = 53178,
    NPC_STICKY_WEBBING          = 53492,
    NPC_STICKY_WEBBING_1        = 53490, // 99219
};

enum Events
{
    EVENT_CHECK_HIGH            = 1,
    EVENT_EMBER_FLARE           = 2,
    EVENT_FILAMENT              = 3,
    EVENT_ENERGY                = 4,
    EVENT_BURNING_ACID          = 5,
    EVENT_FIERY_WEB_SPIN        = 6,
    EVENT_THE_WIDOW_KISS        = 7,
    EVENT_FRENZY                = 8,
    EVENT_GO_DOWN               = 9,
    EVENT_ENERGY_2              = 10,
    EVENT_BOILING_SPATTER       = 11,
    EVENT_SUMMON_DRONE          = 12,
    EVENT_SUMMON_SPIDERLING     = 13,
    EVENT_SUMMON_BROODLING_1    = 14,
    EVENT_SUMMON_SPIDERLING_1   = 15,
    EVENT_CHECK_DRONE           = 16,
    EVENT_CHECK_SPIDERLING      = 17,
    EVENT_CHECK_TARGET          = 18,
    EVENT_FIXATE                = 19,
    EVENT_FIXATE_OFF            = 20,
};

enum Other
{
    POINT_HIGH  = 1,
    DATA_PHASE  = 2, // don't change that! Related with TargetedMovementGenerator
    POINT_DOWN  = 3,
    POINT_HIGH2 = 4,
};

#define LEVEL_HEIGHT 100.0f
#define GROUND_HEIGHT 74.05f

const Position highPos = { 75.68f, 397.199f, 110.0f, 3.63f };
const Position addsPos[9] = 
{
    { 24.691f,  298.297f, 81.54f,    0.0f  }, // spiderling
    { 134.570f, 360.037f, 85.420f,   0.0f  }, // spiderling
    { 86.338f,  442.826f, 76.72f,    0.0f  }, // spiderling
    { 11.773f,  479.865f, 78.51f,    0.0f  }, // Drone
    { 48.428f,  376.85f,  106.8785f, 0.0f  }, // Spiderweb Filament
    { 41.594f,  378.043f, 74.05f,    0.0f  }, // center of the room
    { 55.301f,  375.729f, 111.0f,    0.94f }, 
    { 75.372f,  374.039f, 111.0f,    2.18f },
    { 59.665f,  406.437f, 111.0f,    5.02f }
};

struct PositionSelector 
{
    public:
        PositionSelector(bool b, uint32 spellId) : _b(b), _spellId(spellId) { }

        bool operator()(Unit const* target) const
        {
            if (_spellId && target->HasAura(_spellId))
                return false;

            return _b? (target->GetPositionZ() < 100.0f): (target->GetPositionZ() > 100.0f); 
        }

    private:
        bool _b;
        uint32 _spellId;

};

class boss_bethtilac : public CreatureScript
{
    public:
        boss_bethtilac() : CreatureScript("boss_bethtilac") { }

        struct boss_bethtilacAI : public BossAI
        {
            boss_bethtilacAI(Creature* creature) : BossAI(creature, DATA_BETHTILAC)
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
                me->setActive(true);
            }

            uint8 uiPhase;
            uint8 uiCount;
            uint8 uiSide;

            void Reset() override
            {
                _Reset();
                DespawnCreatures(NPC_CINDERWEB_SPINNER);
                DespawnCreatures(NPC_SPIDERWEB_FILAMENT);

                DoCast(me, SPELL_ZERO_POWER, true);
                me->SetReactState(REACT_PASSIVE);
                //me->SetAnimTier(AnimTier::Fly);
                //me->SetUInt32Value(UNIT_FIELD_BYTES_2, 1);
                me->GetMap()->SetWorldState(WORLDSTATE_DEATH_FROM_ABOVE, 1);
                uiPhase = 0;
                uiCount = 0;
                uiSide = 0;
                me->SetMaxPower(POWER_MANA, 9000);
                me->SetPower(POWER_MANA, 9000);

                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetAnimTier(AnimTier::Hover);

                float hx, hy, hz, ho;
                me->GetHomePosition(hx, hy, hz, ho);
                me->NearTeleportTo(hx, hy, hz, ho);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                uiPhase = 0;
                uiCount = 0;
                uiSide = 0;
                instance->SetBossState(DATA_BETHTILAC, IN_PROGRESS);
                me->GetMotionMaster()->MovePoint(POINT_HIGH, highPos);
                DoZoneInCombat();
                me->RemoveAurasByType(SPELL_AURA_MOD_TAUNT);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE)
                {
                    if (pointId == POINT_HIGH)
                    {
                        me->SetFacingTo(highPos.GetOrientation());
                        me->SetAnimTier(AnimTier::Ground);
                        me->RemoveAurasByType(SPELL_AURA_MOD_TAUNT);
                        //me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                        //me->SetUInt32Value(UNIT_FIELD_BYTES_2, 0);
                        me->SetReactState(REACT_AGGRESSIVE);
                        events.ScheduleEvent(EVENT_CHECK_HIGH, 5000);
                        events.ScheduleEvent(EVENT_FILAMENT, 6000);
                        events.ScheduleEvent(EVENT_SUMMON_DRONE, 60000);
                        events.ScheduleEvent(EVENT_SUMMON_SPIDERLING, 30000);
                        events.ScheduleEvent(EVENT_EMBER_FLARE, urand(7000, 8000));
                        events.ScheduleEvent(EVENT_ENERGY, 1000);
                    }
                    else if (pointId == POINT_HIGH2)
                        me->SetFacingTo(highPos.GetOrientation());
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_PHASE)
                    return uiPhase;

                return 0;
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                DespawnCreatures(NPC_CINDERWEB_SPINNER);
                DespawnCreatures(NPC_SPIDERWEB_FILAMENT);

                AddSmoulderingAura(me);
            }
            
            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || !CheckInArea(diff, 5764))
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->GetVictim() && !PositionSelector(uiPhase, 0)(me->GetVictim()))
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, PositionSelector(uiPhase, 0));
                    if (uiPhase == 0) // above
                    {
                        if (target)
                        {
                            AttackStart(target);
                            me->GetMotionMaster()->MoveChase(target);
                        }
                        else
                        {
                            me->AttackStop();
                            if (me->GetExactDist(&highPos) > 5)
                                me->GetMotionMaster()->MovePoint(POINT_HIGH2, highPos);
                        }
                    }
                    else // below
                    {
                        if (target)
                        {
                            AttackStart(target);
                            me->GetMotionMaster()->MoveChase(target);
                        }
                        else
                        {
                            me->AttackStop();
                            if (me->GetExactDist(&addsPos[5]) > 5)
                                me->GetMotionMaster()->MovePoint(0, addsPos[5]);
                        }
                    }
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_HIGH:
                        {
                            //if (!me->GetVictim() || !me->IsWithinMeleeRange(me->GetVictim()))
                            std::list<Player*> PlayerList;
                            PlayerPositionCheck checker(true);
                            Trinity::PlayerListSearcher<PlayerPositionCheck> searcher(me, PlayerList, checker);
                            me->VisitNearbyWorldObject(300.0f, searcher);
                            if (PlayerList.size() == 0)
                                DoCastAOE(SPELL_VENOM_RAIN);
                            events.ScheduleEvent(EVENT_CHECK_HIGH, 5000);
                            break;
                        }
                        case EVENT_EMBER_FLARE:
                            if (uiPhase == 0)
                            {
                                std::list<Player*> PlayerList;
                                PlayerPositionCheck checker(true);
                                Trinity::PlayerListSearcher<PlayerPositionCheck> searcher(me, PlayerList, checker);
                                me->VisitNearbyWorldObject(300.0f, searcher);
                                if (PlayerList.size() > 0)
                                    DoCastAOE(SPELL_EMBER_FLARE_1);
                            }
                            else
                            {
                                DoCastAOE(SPELL_EMBER_FLARE_2);
                            }
                            
                            events.ScheduleEvent(EVENT_EMBER_FLARE, urand(6000, 7000)); 
                            break;
                        case EVENT_FILAMENT:
                            for (uint8 i = 0; i < RAID_MODE(2, 4, 2, 4); i++)
                                if (Creature* pFilament = me->SummonCreature(NPC_SPIDERWEB_FILAMENT, addsPos[4].GetPositionX() + irand(-8, 8), addsPos[4].GetPositionY() + irand(-8, 8), addsPos[4].GetPositionZ(), 0.0f))
                                    pFilament->SetCanFly(true);
                            events.ScheduleEvent(EVENT_FILAMENT, urand(20000, 25000));
                            break;
                        case EVENT_SUMMON_DRONE:
                            if (Creature* pDrone = me->SummonCreature(NPC_CINDERWEB_DRONE, addsPos[3]))
                                //pDrone->GetMotionMaster()->MovePoint(0, addsPos[5]);
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, PositionSelector(true, 0)))
                                    pDrone->GetMotionMaster()->MoveChase(target);
                            events.ScheduleEvent(EVENT_SUMMON_DRONE, 60000);
                            break;
                        case EVENT_SUMMON_SPIDERLING:
                            uiSide = urand(0, 2);
                            for (uint8 i = 0; i < 8; i++)
                                events.ScheduleEvent(EVENT_SUMMON_SPIDERLING_1, i * 500);
                            if (IsHeroic())
                                for (uint8 i = 0; i < RAID_MODE(1, 2, 1, 2); ++i)
                                    events.ScheduleEvent(EVENT_SUMMON_BROODLING_1, i * 1000);
                            events.ScheduleEvent(EVENT_SUMMON_SPIDERLING, 30000);
                            break;
                        case EVENT_SUMMON_SPIDERLING_1:
                            if (Creature* pSpiderling = me->SummonCreature(NPC_CINDERWEB_SPIDERLING, addsPos[uiSide]))
                                pSpiderling->GetMotionMaster()->MovePoint(0, addsPos[5]);
                            break;
                        case EVENT_SUMMON_BROODLING_1:
                            if (Creature* pBroodling = me->SummonCreature(NPC_ENGORGED_BROODLING, addsPos[uiSide]))
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, PositionSelector(true, 0)))
                                {
                                    pBroodling->CastSpell(target, SPELL_FIXATE_BROODLING, true);
                                    pBroodling->ClearUnitState(UNIT_STATE_CASTING);
                                    pBroodling->GetMotionMaster()->MoveFollow(target, 0.0f, 0.0f);
                                }
                            break;
                        case EVENT_ENERGY_2:
                            //me->SetPower(POWER_MANA, 9000);
                            break;
                        case EVENT_ENERGY:
                        {
                            uint32 energy = me->GetPower(POWER_MANA);
                            if (energy > 0)
                                me->SetPower(POWER_MANA, energy - 100);
                            if ((energy - 100) == 0)
                            {
                                uiCount++;
                                if (uiCount < 3)
                                {
                                    events.RescheduleEvent(EVENT_FILAMENT, 23000);
                                    events.RescheduleEvent(EVENT_CHECK_HIGH, 12000);
                                    events.RescheduleEvent(EVENT_EMBER_FLARE, 14000);
                                    events.ScheduleEvent(EVENT_ENERGY, 10000);
                                    events.ScheduleEvent(EVENT_ENERGY_2, 8000);
                                }
                                else
                                {
                                    events.CancelEvent(EVENT_ENERGY);
                                    events.CancelEvent(EVENT_FILAMENT);
                                    events.CancelEvent(EVENT_CHECK_HIGH);
                                    events.CancelEvent(EVENT_SUMMON_DRONE);
                                    events.CancelEvent(EVENT_SUMMON_SPIDERLING);
                                    events.RescheduleEvent(EVENT_EMBER_FLARE, 15000);
                                    events.ScheduleEvent(EVENT_GO_DOWN, 10000);
                                }
                                
                                DoCastAOE(SPELL_SMOLDERING_DEVASTATION);
                            }
                            else
                                events.ScheduleEvent(EVENT_ENERGY, 1000);
                            break;
                        }
                        case EVENT_GO_DOWN:
                        {
                            uiPhase = 1;
                            me->GetMotionMaster()->MoveJump(addsPos[5].GetPositionX(), addsPos[5].GetPositionY(), addsPos[5].GetPositionZ(), 40.0f, 40.0f);
                            events.ScheduleEvent(EVENT_FRENZY, 10000);
                            events.ScheduleEvent(EVENT_THE_WIDOW_KISS, 32000);
                            Map::PlayerList const &PlayerList = instance->instance->GetPlayers();
                            if (!PlayerList.isEmpty())
                            {
                                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                    if (Player* player = i->GetSource())
                                        if (player->GetPositionZ() > 100.0f)
                                            player->NearTeleportTo(addsPos[5].GetPositionX(), addsPos[5].GetPositionY(), addsPos[5].GetPositionZ(), 0.0f);
                            }
                            break;
                        }
                        case EVENT_FRENZY:
                            DoCast(me, SPELL_FRENZY);
                            events.ScheduleEvent(EVENT_FRENZY, 7000);
                            break;
                        case EVENT_THE_WIDOW_KISS:
                            DoCastVictim(SPELL_THE_WIDOW_KISS);
                            events.ScheduleEvent(EVENT_THE_WIDOW_KISS, 34000);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            void DespawnCreatures(uint32 entry)
            {
                std::list<Creature*> creatures;
                GetCreatureListWithEntryInGrid(creatures, me, entry, 1000.0f);

                if (creatures.empty())
                   return;

                for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                     (*iter)->DespawnOrUnsummon();
            }

            class PlayerPositionCheck
            {
                public:
                    PlayerPositionCheck(bool bRev): _bRev(bRev) { }
                    bool operator()(Player* u)
                    {
                        return _bRev? (u->GetPositionZ() > 100.0f): (u->GetPositionZ() < 100.0f);
                    }

                private:
                    bool _bRev;
            };
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_bethtilacAI>(creature);
        }
};

class npc_bethtilac_spiderweb_filament : public CreatureScript
{
    public:
        npc_bethtilac_spiderweb_filament() : CreatureScript("npc_bethtilac_spiderweb_filament") { }

        struct npc_bethtilac_spiderweb_filamentAI : public ScriptedAI
        {
            npc_bethtilac_spiderweb_filamentAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                SetCombatMovement(false);
            }

            InstanceScript* instance;

            void JustSummoned(Creature* summon) override
            {
                if (me->IsInCombat())
                    DoZoneInCombat(summon);
            }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                if (clicker->GetTypeId() == TYPEID_PLAYER)
                {
                    uint32 i = urand(6, 8);
                    clicker->NearTeleportTo(addsPos[i].GetPositionX(), addsPos[i].GetPositionY(), addsPos[i].GetPositionZ(), addsPos[i].GetOrientation());
                }
                if (me->GetOwner() && (me->GetOwner()->GetEntry() == NPC_SPIDERWEB_FILAMENT || me->GetOwner()->GetEntry() == NPC_BETHTILAC))
                    me->GetOwner()->ToCreature()->DespawnOrUnsummon();
                me->DespawnOrUnsummon();
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (summoner->GetEntry() == NPC_BETHTILAC)
                {
                    if (Creature* pSpinner = me->SummonCreature(NPC_CINDERWEB_SPINNER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation()))
                    {
                        pSpinner->SetCanFly(true);
                        pSpinner->SetDisableGravity(true);
                        pSpinner->SetAnimTier(AnimTier::Hover);
                        DoCast(pSpinner, SPELL_SPIDERWEB_FILAMENT_ANY, true);
                        //pSpinner->CastSpell(pSpinner, SPELL_CREEPER, true);
                        pSpinner->CastSpell(pSpinner, SPELL_TEMPERAMENT, true);
                        float z = pSpinner->GetMap()->GetHeight(pSpinner->GetPositionX(), pSpinner->GetPositionY(), pSpinner->GetPositionZ() - 5.0f, true);
                        //float z = GROUND_HEIGHT;
                        pSpinner->GetMotionMaster()->MovePoint(0, pSpinner->GetPositionX(), pSpinner->GetPositionY(), z + 5.0f); 
                    }
                }
                else if (summoner->GetEntry() == NPC_SPIDERWEB_FILAMENT)
                {
                    DoCast(me, SPELL_SPIDERWEB_FILAMENT_DOWN, true);
                    float z = me->GetMap()->GetHeight(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 5.0f, true, 100.0f);
                    //float z = GROUND_HEIGHT;
                    me->GetMotionMaster()->MovePoint(POINT_DOWN, me->GetPositionX(), me->GetPositionY(), z); 
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE)
                    if (pointId == POINT_DOWN)
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (instance->GetBossState(DATA_BETHTILAC) != IN_PROGRESS)
                    me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_bethtilac_spiderweb_filamentAI>(creature);
        }
};

class npc_bethtilac_cinderweb_spinner : public CreatureScript
{
    public:
        npc_bethtilac_cinderweb_spinner() : CreatureScript("npc_bethtilac_cinderweb_spinner") { }

        struct npc_bethtilac_cinderweb_spinnerAI : public ScriptedAI
        {
            npc_bethtilac_cinderweb_spinnerAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                bTaunted = false;
                owner = NULL;
                instance = me->GetInstanceScript();
            }

            bool bTaunted;
            Unit* owner;
            InstanceScript* instance;
            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_BURNING_ACID, urand(7000, 15000));
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_FIERY_WEB_SPIN, urand(5000, 15000));
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (summoner)
                    owner = summoner;
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->SetCanFly(false);
                me->SetDisableGravity(false);

                /*if (bTaunted)
                    return;

                bTaunted = true;

                if (owner && owner->GetEntry() == NPC_SPIDERWEB_FILAMENT)
                    if (Creature* pFilament = owner->SummonCreature(NPC_SPIDERWEB_FILAMENT, owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 15000ms))
                    {
                        pFilament->SetCanFly(true);
                        owner->CastSpell(pFilament, SPELL_SPIDERWEB_FILAMENT_ANY, true);
                    }*/
            }

            void SpellHit(Unit* caster, const SpellInfo* spell) override
            {
                if (bTaunted)
                    return;

                if (spell->HasEffect(SPELL_EFFECT_ATTACK_ME) || spell->HasAura(SPELL_AURA_MOD_TAUNT))
                {
                    bTaunted = true;
                    me->SetReactState(REACT_AGGRESSIVE);
                    if (owner && owner->GetEntry() == NPC_SPIDERWEB_FILAMENT)
                        if (Creature* pFilament = owner->SummonCreature(NPC_SPIDERWEB_FILAMENT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 15000ms))
                        {
                            pFilament->SetCanFly(true);
                            owner->CastSpell(pFilament, SPELL_SPIDERWEB_FILAMENT_ANY, true);
                        }

                    events.CancelEvent(EVENT_FIERY_WEB_SPIN);
                    me->InterruptNonMeleeSpells(false);
                    me->RemoveAura(SPELL_SPIDERWEB_FILAMENT_ANY);
                    //me->RemoveAura(SPELL_CREEPER);
                    me->RemoveAura(SPELL_TEMPERAMENT);
                    me->SetCanFly(false);
                    me->SetDisableGravity(false);
                    me->SetAnimTier(AnimTier::Ground);
                    AttackStart(caster);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(DATA_BETHTILAC) != IN_PROGRESS)
                    me->DespawnOrUnsummon();

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BURNING_ACID:
                            DoCastAOE(SPELL_BURNING_ACID);
                            events.ScheduleEvent(EVENT_BURNING_ACID, urand(7000, 15000));
                            break;
                        case EVENT_FIERY_WEB_SPIN:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, PositionSelector(true, SPELL_FIERY_WEB_SPIN)))
                                DoCast(target, SPELL_FIERY_WEB_SPIN);
                            events.ScheduleEvent(EVENT_FIERY_WEB_SPIN, urand(25000, 30000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_bethtilac_cinderweb_spinnerAI>(creature);
        }
};

class npc_bethtilac_cinderweb_drone : public CreatureScript
{
    public:
        npc_bethtilac_cinderweb_drone() : CreatureScript("npc_bethtilac_cinderweb_drone") { }

        struct npc_bethtilac_cinderweb_droneAI : public ScriptedAI
        {
            npc_bethtilac_cinderweb_droneAI(Creature* creature) : ScriptedAI(creature)
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
                bClimbed = false;
            }

            EventMap events;
            bool bClimbed;

            void Reset() override
            {
                events.Reset();
                DoCast(me, SPELL_ZERO_POWER, true);
                me->SetPowerType(POWER_MANA);
                me->SetMaxPower(POWER_MANA, 90);

                if (bClimbed)
                    return;

                me->SetPower(POWER_MANA, 90);
                DoCast(me, SPELL_ENERGIZE, true);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_BURNING_ACID, urand(7000, 15000));
                events.ScheduleEvent(EVENT_BOILING_SPATTER, urand(14000, 20000));
                events.ScheduleEvent(EVENT_CHECK_TARGET, 2000);
                events.ScheduleEvent(EVENT_CHECK_SPIDERLING, 3000);
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_FIXATE, urand(12000, 15000));
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (me->GetPositionZ() < 100)
                    me->GetMap()->SetWorldState(WORLDSTATE_DEATH_FROM_ABOVE, 0);

                for (uint8 i = 0; i < 3; ++i)
                    DoCast(me, SPELL_CREATE_CHITINOUS_FRAGMENT, true);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE && pointId == POINT_HIGH)
                {
                    me->SetAnimTier(AnimTier::Ground);
                    me->SetReactState(REACT_AGGRESSIVE);
                }
            }

            bool CanAIAttack(Unit const* who) const override
            {
                return PositionSelector(!bClimbed, 0)(who);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (!bClimbed && !me->GetPower(POWER_MANA))
                {
                    bClimbed = true;
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    me->SetCanFly(true);
                    me->SetDisableGravity(true);
                    me->SetAnimTier(AnimTier::Hover);
                    me->RemoveAurasDueToSpell(SPELL_ENERGIZE);
                    float z = std::max(110.0f, me->GetMap()->GetHeight(me->GetPositionX(), me->GetPositionY(), MAX_HEIGHT, true, MAX_HEIGHT * 2));
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(POINT_HIGH, me->GetPositionX(), me->GetPositionY(), z);
                    me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), z, me->GetOrientation());
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->GetVictim() && !PositionSelector(!bClimbed, 0)(me->GetVictim()))
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, PositionSelector(!bClimbed, 0));
                    if (bClimbed) // above
                    {
                        if (target)
                        {
                            AttackStart(target);
                            me->GetMotionMaster()->MoveChase(target);
                        }
                        else
                        {
                            me->AttackStop();
                            Position pos = me->GetHomePosition();
                            if (me->GetExactDist(&pos) > 5)
                                me->GetMotionMaster()->MovePoint(0, pos);
                        }
                    }
                    else // below
                    {
                        if (target)
                        {
                            AttackStart(target);
                            me->GetMotionMaster()->MoveChase(target);
                        }
                        else
                        {
                            me->AttackStop();
                        }
                    }
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FIXATE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, PositionSelector(true, 0)))
                            {
                                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                                DoResetThreat();
                                DoCast(me, SPELL_FIXATE_SELF, true);
                                DoCast(target, SPELL_FIXATE, true);
                                me->AddThreat(target, 1000000.0f);
                                AttackStart(target);
                                events.ScheduleEvent(EVENT_FIXATE_OFF, 10000);
                            }
                            break;
                        case EVENT_FIXATE_OFF:
                            DoResetThreat();
                            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                            break;
                        case EVENT_BOILING_SPATTER:
                            DoCast(me, SPELL_BOILING_SPATTER);
                            events.ScheduleEvent(EVENT_BOILING_SPATTER, urand(15000, 20000));
                            break;
                        case EVENT_BURNING_ACID:
                            DoCastAOE(SPELL_BURNING_ACID);
                            events.ScheduleEvent(EVENT_BURNING_ACID, urand(10000, 15000));
                            break;
                        case EVENT_CHECK_TARGET:
                            if (me->GetVictim())
                                if (bClimbed ? (me->GetVictim()->GetPositionZ() < 100.0f) : (me->GetVictim()->GetPositionZ() > 100.0f))
                                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, PositionSelector(!bClimbed, 0)))
                                        AttackStart(target);

                            events.ScheduleEvent(EVENT_CHECK_TARGET, 2000);
                            break;
                        case EVENT_CHECK_SPIDERLING:
                            if (Creature* pSpiderling = me->FindNearestCreature(NPC_CINDERWEB_SPIDERLING, 8.0f))
                                DoCast(pSpiderling, SPELL_CONSUME_DRONE, true);
                            events.ScheduleEvent(EVENT_CHECK_SPIDERLING, 3000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_bethtilac_cinderweb_droneAI>(creature);
        }
};

class npc_bethtilac_cinderweb_spiderling : public CreatureScript
{
    public:
        npc_bethtilac_cinderweb_spiderling() : CreatureScript("npc_bethtilac_cinderweb_spiderling") { }

        struct npc_bethtilac_cinderweb_spiderlingAI : public ScriptedAI
        {
            npc_bethtilac_cinderweb_spiderlingAI(Creature* creature) : ScriptedAI(creature)
            {
                pDrone = NULL;
                me->SetReactState(REACT_PASSIVE);
            }

            Unit* pDrone;
            EventMap events;

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CHECK_DRONE, 2000);
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
                        case EVENT_CHECK_DRONE:
                            if (!pDrone || !pDrone->IsAlive() || !pDrone->HasAura(SPELL_ENERGIZE))
                            {
                                std::list<Creature*> drones;
                                GetCreatureListWithEntryInGrid(drones, me, NPC_CINDERWEB_DRONE, 500.0f);
                                drones.remove_if(Trinity::UnitAuraCheck(false, SPELL_ENERGIZE));
                                if (drones.size() > 2)
                                    drones.sort(Trinity::DistanceCompareOrderPred(me));
                                if (drones.size())
                                {
                                    pDrone = drones.front();;
                                    me->GetMotionMaster()->MovementExpired(false);
                                    me->GetMotionMaster()->MoveFollow(pDrone, 0.0f, 0.0f);
                                }
                                else
                                {
                                    pDrone = NULL;
                                    if (!me->isMoving())
                                    {
                                        me->GetMotionMaster()->MovementExpired(false);
                                        me->GetMotionMaster()->MovePoint(0, addsPos[5]);
                                    }
                                }
                            }
                            events.ScheduleEvent(EVENT_CHECK_DRONE, 2000);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_bethtilac_cinderweb_spiderlingAI>(creature);
        }
};

class npc_bethtilac_engorged_broodling : public CreatureScript
{
    public:
        npc_bethtilac_engorged_broodling() : CreatureScript("npc_bethtilac_engorged_broodling") { }

        struct npc_bethtilac_engorged_broodlingAI : public ScriptedAI
        {
            npc_bethtilac_engorged_broodlingAI(Creature* creature) : ScriptedAI(creature)
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
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, 2.0f);
            }

            bool bBurst;

            void JustEngagedWith(Unit* /*who*/) override
            {
                bBurst = false;
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!bBurst && me->SelectNearestPlayer(3.0f))
                {
                    bBurst = true;
                    me->StopMoving();
                    me->GetMotionMaster()->MovementExpired(false);
                    me->SetReactState(REACT_PASSIVE);
                    DoCastAOE(SPELL_VOLATILE_BURST);
                    DoCast(me, SPELL_VOLATILE_POISON, true);
                    me->DespawnOrUnsummon(30000);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_bethtilac_engorged_broodlingAI>(creature);
        }
};

class PositionCheck
{
    public:
        PositionCheck(bool b) : _b(b) { }

        bool operator()(WorldObject* unit)
        {
            return _b? (unit->GetPositionZ() > 100.0f): (unit->GetPositionZ() < 100.0f);
        }

        private:
            bool _b;
};

class spell_bethtilac_smoldering_devastation : public SpellScriptLoader
{
    public:
        spell_bethtilac_smoldering_devastation() : SpellScriptLoader("spell_bethtilac_smoldering_devastation") { }

        class spell_bethtilac_smoldering_devastation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_bethtilac_smoldering_devastation_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                targets.remove_if(PositionCheck(false));
            }

            void HandleAfterCast()
            {
                if (!GetCaster())
                    return;

                std::list<Creature*> creatureList;
                GetCaster()->GetCreatureListWithEntryInGrid(creatureList, NPC_DULL_CHITINOUS_FOCUS, 100.0f);
                if (!creatureList.empty())
                {
                    for (std::list<Creature*>::const_iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
                    {
                        if (Creature* pFocus = (*itr)->ToCreature())
                        {
                            pFocus->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            pFocus->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            pFocus->CastSpell(pFocus, SPELL_TRANSFORM_CHARGED_CHITINOUS_FOCUS, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_bethtilac_smoldering_devastation_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_bethtilac_smoldering_devastation_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_bethtilac_smoldering_devastation_SpellScript();
        }
};

class spell_bethtilac_ember_flare : public SpellScriptLoader
{
    public:
        spell_bethtilac_ember_flare() : SpellScriptLoader("spell_bethtilac_ember_flare") { }

        class spell_bethtilac_ember_flare_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_bethtilac_ember_flare_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                targets.remove_if(PositionCheck(false));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_bethtilac_ember_flare_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }

            private:

        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_bethtilac_ember_flare_SpellScript();
        }
};

class spell_bethtilac_burning_acid : public SpellScriptLoader
{
    public:
        spell_bethtilac_burning_acid() : SpellScriptLoader("spell_bethtilac_burning_acid") { }

        class spell_bethtilac_burning_acid_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_bethtilac_burning_acid_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                targets.remove_if(PositionCheck(true));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_bethtilac_burning_acid_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }

            private:

        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_bethtilac_burning_acid_SpellScript();
        }
};

void AddSC_boss_bethtilac()
{
    new boss_bethtilac();
    new npc_bethtilac_spiderweb_filament();
    new npc_bethtilac_cinderweb_spinner();
    new npc_bethtilac_cinderweb_drone();
    new npc_bethtilac_cinderweb_spiderling();
    new npc_bethtilac_engorged_broodling();
    new spell_bethtilac_smoldering_devastation();
    new spell_bethtilac_ember_flare();
    new spell_bethtilac_burning_acid();
}
