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
#include "CombatAI.h"
#include "Player.h"
#include "Vehicle.h"
#include "oculus.h"

#define HAS_ESSENCE(a) ((a)->HasItemCount(ITEM_EMERALD_ESSENCE,1) || (a)->HasItemCount(ITEM_AMBER_ESSENCE,1) || (a)->HasItemCount(ITEM_RUBY_ESSENCE,1))

enum GossipNPCs
{
    GOSSIP_MENU_VERDISA                 = 9573,
    GOSSIP_MENU_ETERNOS                 = 9574,
    GOSSIP_MENU_BELGARISTRASZ           = 9575,

    ITEM_EMERALD_ESSENCE                = 37815,
    ITEM_AMBER_ESSENCE                  = 37859,
    ITEM_RUBY_ESSENCE                   = 37860
};

enum Drakes
{
/*
 * Ruby Drake (27756)
 * (summoned by spell Ruby Essence (37860) --> Call Amber Drake (49462) --> Summon 27756)
 */
    SPELL_RIDE_RUBY_DRAKE_QUE           = 49463,          // Apply Aura: Periodic Trigger, Interval: 3 seconds --> 49464
    SPELL_RUBY_DRAKE_SADDLE             = 49464,          // Allows you to ride on the back of an Amber Drake. --> Dummy
    SPELL_RUBY_SEARING_WRATH            = 50232,          // (60 yds) - Instant - Breathes a stream of fire at an enemy dragon, dealing 6800 to 9200 Fire damage and then jumping to additional dragons within 30 yards. Each jump increases the damage by 50%. Affects up to 5 total targets
    SPELL_RUBY_EVASIVE_AURA             = 50248,          // Instant - Allows the Ruby Drake to generate Evasive Charges when hit by hostile attacks and spells.
    SPELL_RUBY_EVASIVE_CHARGES          = 50241,
    SPELL_RUBY_EVASIVE_MANEUVERS        = 50240,          // Instant - 5 sec. cooldown - Allows your drake to dodge all incoming attacks and spells. Requires Evasive Charges to use. Each attack or spell dodged while this ability is active burns one Evasive Charge. Lasts 30 sec. or until all charges are exhausted.
    // you do not have acces to until you kill Mage-Lord Urom
    SPELL_RUBY_MARTYR                   = 50253,          // Instant - 10 sec. cooldown - Redirect all harmful spells cast at friendly drakes to yourself for 10 sec.

/*
 * Amber Drake (27755)
 * (summoned by spell Amber Essence (37859) --> Call Amber Drake (49461) --> Summon 27755)
 */
    SPELL_RIDE_AMBER_DRAKE_QUE          = 49459,          // Apply Aura: Periodic Trigger, Interval: 3 seconds --> 49460
    SPELL_AMBER_DRAKE_SADDLE            = 49460,          // Allows you to ride on the back of an Amber Drake. --> Dummy
    SPELL_AMBER_SHOCK_CHARGE            = 49836,
    SPELL_AMBER_SHOCK_LANCE             = 49840,          // (60 yds) - Instant - Deals 4822 to 5602 Arcane damage and detonates all Shock Charges on an enemy dragon. Damage is increased by 6525 for each detonated.
    SPELL_AMBER_STOP_TIME               = 49838,          // Instant - 1 min cooldown - Halts the passage of time, freezing all enemy dragons in place for 10 sec. This attack applies 5 Shock Charges to each affected target.
    // you do not have access to until you kill the Mage-Lord Urom.
    SPELL_AMBER_TEMPORAL_RIFT           = 49592,          // (60 yds) - Channeled - Channels a temporal rift on an enemy dragon for 10 sec. While trapped in the rift, all damage done to the target is increased by 100%. In addition, for every 15, 000 damage done to a target affected by Temporal Rift, 1 Shock Charge is generated.

/*
 * Emerald Drake (27692)
 * (summoned by spell Emerald Essence (37815) --> Call Emerald Drake (49345) --> Summon 27692)
 */
    SPELL_RIDE_EMERALD_DRAKE_QUE        = 49427,         // Apply Aura: Periodic Trigger, Interval: 3 seconds --> 49346
    SPELL_EMERALD_DRAKE_SADDLE          = 49346,         // Allows you to ride on the back of an Amber Drake. --> Dummy
    SPELL_EMERALD_LEECHING_POISON       = 50328,         // (60 yds) - Instant - Poisons the enemy dragon, leeching 1300 to the caster every 2 sec. for 12 sec. Stacks up to 3 times.
    SPELL_EMERALD_TOUCH_THE_NIGHTMARE   = 50341,         // (60 yds) - Instant - Consumes 30% of the caster's max health to inflict 25, 000 nature damage to an enemy dragon and reduce the damage it deals by 25% for 30 sec.
    // you do not have access to until you kill the Mage-Lord Urom

    SPELL_FLIGHT                        = 50296,
    SPELL_SOAR                          = 50325,
    SPELL_DRAKE_FLAG_VISUAL             = 53797,
    SPELL_SIMPLE_TELEPORT               = 12980,

    // Misc
    POINT_LAND                          = 2,
    POINT_TAKE_OFF                      = 3,
    ACTION_GPS                          = 1,
};

enum DrakeEvents
{
    EVENT_WELCOME = 1,
    EVENT_ABILITIES,
    EVENT_SPECIAL_ATTACK,
    EVENT_LOW_HEALTH,
    EVENT_RESET_LOW_HEALTH,
    EVENT_TAKE_OFF
};

enum Says
{
    SAY_VAROS                         = 0,
    SAY_UROM                          = 1,
    SAY_SPECIAL                       = 2,
    SAY_BELGARISTRASZ                 = 0,
    SAY_DRAKES_TAKEOFF                = 0,

    WHISPER_DRAKES_WELCOME            = 1,
    WHISPER_DRAKES_ABILITIES          = 2,
    WHISPER_DRAKES_SPECIAL            = 3,
    WHISPER_DRAKES_LOWHEALTH          = 4,
    WHISPER_DRAKES_GPS_CONSTRUCTS     = 5,
    WHISPER_DRAKES_GPS_CONSTRUCT      = 6,
    WHISPER_DRAKES_GPS_VAROS          = 7,
    WHISPER_DRAKES_GPS_UROM           = 8,
    WHISPER_DRAKES_GPS_EREGOS         = 9,
};

class npc_verdisa_beglaristrasz_eternos : public CreatureScript
{
    public:
        npc_verdisa_beglaristrasz_eternos() : CreatureScript("npc_verdisa_beglaristrasz_eternos") { }

        struct npc_verdisa_beglaristrasz_eternosAI : public ScriptedAI
        {
            npc_verdisa_beglaristrasz_eternosAI(Creature* creature) : ScriptedAI(creature) { }

            void StoreEssence(Player* player, uint32 itemId)
            {
                /// @todo: should be handled by spell, but not found in dbc (49450 and other?)
                uint32 count = 1;
                ItemPosCountVec dest;
                uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, count);
                if (msg == EQUIP_ERR_OK)
                    if (Item* item = player->StoreNewItem(dest, itemId, true))
                        player->SendNewItem(item, count, true, true);
            }

            void RemoveEssence(Player* player, uint32 itemId)
            {
                player->DestroyItemCount(itemId, 1, true, false);
            }

            bool OnGossipSelect(Player* player, uint32 menuId, uint32 action) override
            {
                switch (menuId)
                {
                    case GOSSIP_MENU_VERDISA:
                        if (action >= 1 && action <= 3)
                        {
                            if (action == 2)
                                RemoveEssence(player, ITEM_AMBER_ESSENCE);
                            else if (action == 3)
                                RemoveEssence(player, ITEM_RUBY_ESSENCE);

                            StoreEssence(player, ITEM_EMERALD_ESSENCE);
                            break;
                        }
                        return true;
                    case GOSSIP_MENU_ETERNOS:
                        if (action >= 1 && action <= 3)
                        {
                            if (action == 2)
                                RemoveEssence(player, ITEM_EMERALD_ESSENCE);
                            else if (action == 3)
                                RemoveEssence(player, ITEM_RUBY_ESSENCE);

                            StoreEssence(player, ITEM_AMBER_ESSENCE);
                            break;
                        }
                        return true;
                    case GOSSIP_MENU_BELGARISTRASZ:
                        if (action <= 2)
                        {
                            if (action == 1)
                                RemoveEssence(player, ITEM_AMBER_ESSENCE);
                            else if (action == 2)
                                RemoveEssence(player, ITEM_EMERALD_ESSENCE);

                            StoreEssence(player, ITEM_RUBY_ESSENCE);
                            break;
                        }
                        return true;
                    default:
                        return true;
                }
                player->PlayerTalkClass->SendCloseGossip();
                return true;
            }

            void MovementInform(uint32 /*type*/, uint32 pointId) override
            {
                if (pointId != POINT_MOVE_OUT)
                    return;

                // When Belgaristraz finish his moving say grateful text
                if (me->GetEntry() == NPC_BELGARISTRASZ)
                    Talk(SAY_BELGARISTRASZ);

                // The gossip flag should activate when Drakos die and not from DB
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetOculusAI<npc_verdisa_beglaristrasz_eternosAI>(creature);
        }
};

class npc_image_belgaristrasz : public CreatureScript
{
    public:
        npc_image_belgaristrasz() : CreatureScript("npc_image_belgaristrasz") { }

        struct npc_image_belgaristraszAI : public ScriptedAI
        {
            npc_image_belgaristraszAI(Creature* creature) : ScriptedAI(creature) { }

            void IsSummonedBy(Unit* summoner) override
            {
                DoCast(me, SPELL_SIMPLE_TELEPORT, true);

                ObjectGuid summonerGUID = summoner->GetGUID();
                if (summoner->GetEntry() == NPC_VAROS)
                {
                    uint32 delay = 0;
                    me->m_Events.Schedule(delay += 6000, [this, summonerGUID]
                    {
                        me->HandleEmoteStateCommand(EMOTE_STATE_TALK);
                        Talk(SAY_VAROS, ObjectAccessor::GetPlayer(*me, summonerGUID));
                    });
                    me->m_Events.Schedule(delay += 6000, [this]
                    {
                        me->HandleEmoteStateCommand(0);
                        me->DespawnOrUnsummon(60000);
                    });
                }

                if (summoner->GetEntry() == NPC_UROM)
                {
                    uint32 delay = 0;
                    me->m_Events.Schedule(delay += 6000, [this, summonerGUID]
                    {
                        me->HandleEmoteStateCommand(EMOTE_STATE_TALK);
                        Talk(SAY_UROM, ObjectAccessor::GetPlayer(*me, summonerGUID));
                    });
                    me->m_Events.Schedule(delay += 7000, [this, summonerGUID]
                    {
                        Talk(SAY_SPECIAL, ObjectAccessor::GetPlayer(*me, summonerGUID));
                    });
                    me->m_Events.Schedule(delay += 6000, [this]
                    {
                        me->HandleEmoteStateCommand(0);
                        me->DespawnOrUnsummon(60000);
                    });
                }

                if (summoner->GetEntry() == NPC_EREGOS)
                {
                    uint32 delay = 0;
                    me->m_Events.Schedule(delay += 6000, [this] { me->HandleEmoteStateCommand(EMOTE_STATE_TALK); });
                    me->m_Events.Schedule(delay += 6000, [this] { me->HandleEmoteStateCommand(0); });
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetOculusAI<npc_image_belgaristraszAI>(creature);
        }
};

class npc_ruby_emerald_amber_drake : public CreatureScript
{
    public:
        npc_ruby_emerald_amber_drake() : CreatureScript("npc_ruby_emerald_amber_drake") { }

        struct npc_ruby_emerald_amber_drakeAI : public VehicleAI
        {
            npc_ruby_emerald_amber_drakeAI(Creature* creature) : VehicleAI(creature)
            {
                Initialize();
                _instance = creature->GetInstanceScript();
            }

            void Initialize()
            {
                _healthWarning = true;
            }

            void Reset() override
            {
                _events.Reset();
                Initialize();
            }

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetFacingToObject(summoner);

                if (summoner->ToPlayer())
                    summoner->ToPlayer()->UnsummonPetTemporaryIfAny();

                switch (me->GetEntry())
                {
                    case NPC_RUBY_DRAKE_VEHICLE:
                        me->GetMap()->SetWorldState(WORLDSTATE_RUBY_VOID, 0);
                        me->CastSpell(summoner, SPELL_RIDE_RUBY_DRAKE_QUE, true);
                        break;
                    case NPC_EMERALD_DRAKE_VEHICLE:
                        me->GetMap()->SetWorldState(WORLDSTATE_EMERALD_VOID, 0);
                        me->CastSpell(summoner, SPELL_RIDE_EMERALD_DRAKE_QUE, true);
                        break;
                    case NPC_AMBER_DRAKE_VEHICLE:
                        me->GetMap()->SetWorldState(WORLDSTATE_AMBER_VOID, 0);
                        me->CastSpell(summoner, SPELL_RIDE_AMBER_DRAKE_QUE, true);
                        break;
                    default:
                        return;
                }

                me->SetFlying(true);
                me->GetMotionMaster()->MoveLand(POINT_LAND, summoner->GetPosition());
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->GetMotionMaster()->MoveFall();
                me->DespawnOrUnsummon(5000);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == EFFECT_MOTION_TYPE && pointId == POINT_LAND)
                    me->SetFlying(false); // Needed this for proper animation after spawn, the summon in air fall to ground bug leave no other option for now, if this isn't used the drake will only walk on move.
            }

            void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply) override
            {
                if (passenger->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (!me->IsAlive())
                    return;

                if (apply)
                {
                    passenger->CastSpell(passenger, SPELL_DRAKE_FLAG_VISUAL, true);
                    DoCast(me, SPELL_FLIGHT, true);
                    DoCast(me, SPELL_SOAR, true);

                    if (_instance->GetBossState(DATA_VAROS) != DONE)
                        _events.ScheduleEvent(EVENT_WELCOME, 10 * IN_MILLISECONDS);

                    else if (_instance->GetBossState(DATA_UROM) == DONE)
                        _events.ScheduleEvent(EVENT_SPECIAL_ATTACK, 10 * IN_MILLISECONDS);
                }
                else
                {
                    passenger->RemoveAurasDueToSpell(SPELL_DRAKE_FLAG_VISUAL);
                    me->RemoveAurasDueToSpell(SPELL_FLIGHT);
                    me->RemoveAurasDueToSpell(SPELL_SOAR);

                    _events.Reset();
                    _events.ScheduleEvent(EVENT_TAKE_OFF, 2 * IN_MILLISECONDS);

                    if (passenger->ToPlayer())
                        passenger->ToPlayer()->ResummonPetTemporaryUnSummonedIfAny();
                }
            }

            void DoAction(int32 action) override
            {
                if (action != ACTION_GPS)
                    return;

                if (_instance->GetBossState(DATA_EREGOS) == DONE)
                    return;
                else if (_instance->GetBossState(DATA_UROM) == DONE)
                    Talk(WHISPER_DRAKES_GPS_EREGOS, ObjectAccessor::GetPlayer(*me, me->GetCreatorGUID()));
                else if (_instance->GetBossState(DATA_VAROS) == DONE)
                    Talk(WHISPER_DRAKES_GPS_UROM, ObjectAccessor::GetPlayer(*me, me->GetCreatorGUID()));
                else if (!_instance->GetData(DATA_CENTRIFUGUE_CONSTRUCTS))
                    Talk(WHISPER_DRAKES_GPS_VAROS, ObjectAccessor::GetPlayer(*me, me->GetCreatorGUID()));
                else if (_instance->GetData(DATA_CENTRIFUGUE_CONSTRUCTS) == 1)
                    Talk(WHISPER_DRAKES_GPS_CONSTRUCT, ObjectAccessor::GetPlayer(*me, me->GetCreatorGUID()));
                else
                    Talk(WHISPER_DRAKES_GPS_CONSTRUCTS, ObjectAccessor::GetPlayer(*me, me->GetCreatorGUID()));
            }

            void UpdateAI(uint32 diff) override
            {
                if (_healthWarning)
                {
                    if (me->GetHealthPct() <= 40.0f)
                        _events.ScheduleEvent(EVENT_LOW_HEALTH, 0);
                }

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WELCOME:
                            Talk(WHISPER_DRAKES_WELCOME, ObjectAccessor::GetPlayer(*me, me->GetCreatorGUID()));
                            _events.ScheduleEvent(EVENT_ABILITIES, 5 * IN_MILLISECONDS);
                            break;
                        case EVENT_ABILITIES:
                            Talk(WHISPER_DRAKES_ABILITIES, ObjectAccessor::GetPlayer(*me, me->GetCreatorGUID()));
                            break;
                        case EVENT_SPECIAL_ATTACK:
                            Talk(WHISPER_DRAKES_SPECIAL, ObjectAccessor::GetPlayer(*me, me->GetCreatorGUID()));
                            break;
                        case EVENT_LOW_HEALTH:
                            Talk(WHISPER_DRAKES_LOWHEALTH, ObjectAccessor::GetPlayer(*me, me->GetCreatorGUID()));
                            _healthWarning = false;
                            _events.ScheduleEvent(EVENT_RESET_LOW_HEALTH, 25000);
                            break;
                        case EVENT_RESET_LOW_HEALTH:
                            _healthWarning = true;
                            break;
                        case EVENT_TAKE_OFF:
                        {
                            me->DespawnOrUnsummon(2050);
                            me->SetSpeed(MOVE_RUN, 1.0f, true);
                            Talk(SAY_DRAKES_TAKEOFF);
                            float const angle = me->GetOrientation() + (M_PI * 100 / 180); // 100 degrees to the left
                            float const pitch = M_PI * 50 / 180; // 50 degrees up
                            float const distance = 50.0f;
                            float x, y, z;
                            me->GetPosition(x, y, z);
                            x += distance * cos(pitch) * cos(angle);
                            y += distance * cos(pitch) * sin(angle);
                            z += distance * sin(pitch);
                            me->SetFlying(true);
                            me->GetMotionMaster()->MoveTakeoff(POINT_TAKE_OFF, Position { x, y, z });
                            break;
                        }
                        default:
                            break;
                    }
                }
            };

        private:
            InstanceScript* _instance;
            EventMap _events;
            bool _healthWarning;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetOculusAI<npc_ruby_emerald_amber_drakeAI>(creature);
        }
};

class go_nexus_portal : public GameObjectScript
{
    public:
        go_nexus_portal() : GameObjectScript("go_nexus_portal") { }

        bool OnGossipHello(Player* player, GameObject* /*go*/)
        {
            player->TeleportTo(player->GetMapId(), 985.33f, 1057.04f, 359.97f, 0.0f);
            return false;
        }
};

// 49345 - Call Emerald Drake
// 49461 - Call Amber Drake
// 49462 - Call Ruby Drake
class spell_oculus_call_ruby_emerald_amber_drake : public SpellScriptLoader
{
    public:
        spell_oculus_call_ruby_emerald_amber_drake() : SpellScriptLoader("spell_oculus_call_ruby_emerald_amber_drake") { }

        class spell_oculus_call_ruby_emerald_amber_drake_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_oculus_call_ruby_emerald_amber_drake_SpellScript);

            void SetDest(SpellDestination& dest)
            {
                Position const offset = { 0.0f, 0.0f, 8.0f, 0.0f };
                dest.RelocateOffset(offset);
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_oculus_call_ruby_emerald_amber_drake_SpellScript::SetDest, EFFECT_0, TARGET_DEST_CASTER_FRONT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_oculus_call_ruby_emerald_amber_drake_SpellScript();
        }
};

// 49427 - Ride Emerald Drake Que
// 49459 - Ride Amber Drake Que
// 49463 - Ride Ruby Drake Que
class spell_oculus_ride_ruby_emerald_amber_drake_que : public SpellScriptLoader
{
    public:
        spell_oculus_ride_ruby_emerald_amber_drake_que() : SpellScriptLoader("spell_oculus_ride_ruby_emerald_amber_drake_que") { }

        class spell_oculus_ride_ruby_emerald_amber_drake_que_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_oculus_ride_ruby_emerald_amber_drake_que_AuraScript);

            void HandlePeriodic(AuraEffect const* aurEff)
            {
                // caster of the triggered spell is wrong for an unknown reason, handle it here correctly
                PreventDefaultAction();
                if (Unit* caster = GetCaster())
                    GetTarget()->CastSpell(caster, GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_oculus_ride_ruby_emerald_amber_drake_que_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_oculus_ride_ruby_emerald_amber_drake_que_AuraScript();
        }
};

// 50240 - Evasive Maneuvers
class spell_oculus_evasive_maneuvers : public SpellScriptLoader
{
    public:
        spell_oculus_evasive_maneuvers() : SpellScriptLoader("spell_oculus_evasive_maneuvers") { }

        class spell_oculus_evasive_maneuvers_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_oculus_evasive_maneuvers_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_RUBY_EVASIVE_CHARGES))
                    return false;
                return true;
            }

            void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
            {
                PreventDefaultAction();
                GetTarget()->RemoveAuraFromStack(SPELL_RUBY_EVASIVE_CHARGES);
                if (!GetTarget()->HasAura(SPELL_RUBY_EVASIVE_CHARGES))
                    Remove(AURA_REMOVE_BY_DEFAULT);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_oculus_evasive_maneuvers_AuraScript::HandleProc, EFFECT_2, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_oculus_evasive_maneuvers_AuraScript();
        }
};

// 49840 - Shock Lance
class spell_oculus_shock_lance : public SpellScriptLoader
{
    public:
        spell_oculus_shock_lance() : SpellScriptLoader("spell_oculus_shock_lance") { }

        class spell_oculus_shock_lance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_oculus_shock_lance_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_AMBER_SHOCK_CHARGE))
                    return false;
                return true;
            }

            void CalcDamage()
            {
                int32 damage = GetHitDamage();
                if (Unit* target = GetHitUnit())
                    if (AuraEffect const* shockCharges = target->GetAuraEffect(SPELL_AMBER_SHOCK_CHARGE, EFFECT_0, GetCaster()->GetGUID()))
                    {
                        damage += shockCharges->GetAmount();
                        shockCharges->GetBase()->Remove();
                    }

                SetHitDamage(damage);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_oculus_shock_lance_SpellScript::CalcDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_oculus_shock_lance_SpellScript();
        }
};

// 49838 - Stop Time
class spell_oculus_stop_time : public SpellScriptLoader
{
    public:
        spell_oculus_stop_time() : SpellScriptLoader("spell_oculus_stop_time") { }

        class spell_oculus_stop_time_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_oculus_stop_time_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_AMBER_SHOCK_CHARGE))
                    return false;
                return true;
            }

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                Unit* target = GetTarget();
                for (uint32 i = 0; i < 5; ++i)
                    caster->CastSpell(target, SPELL_AMBER_SHOCK_CHARGE, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_oculus_stop_time_AuraScript::Apply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_oculus_stop_time_AuraScript();
        }
};

// 49592 - Temporal Rift
class spell_oculus_temporal_rift : public SpellScriptLoader
{
    public:
        spell_oculus_temporal_rift() : SpellScriptLoader("spell_oculus_temporal_rift") { }

        class spell_oculus_temporal_rift_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_oculus_temporal_rift_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_AMBER_SHOCK_CHARGE))
                    return false;
                return true;
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();
                int32 amount = aurEff->GetAmount() + eventInfo.GetDamageInfo()->GetDamage();

                if (amount >= 15000)
                {
                    if (Unit* caster = GetCaster())
                        caster->CastSpell(GetTarget(), SPELL_AMBER_SHOCK_CHARGE, true);
                    amount -= 15000;
                }

                const_cast<AuraEffect*>(aurEff)->SetAmount(amount);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_oculus_temporal_rift_AuraScript::HandleProc, EFFECT_2, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_oculus_temporal_rift_AuraScript();
        }
};

// 50341 - Touch the Nightmare
class spell_oculus_touch_the_nightmare : public SpellScriptLoader
{
    public:
        spell_oculus_touch_the_nightmare() : SpellScriptLoader("spell_oculus_touch_the_nightmare") { }

        class spell_oculus_touch_the_nightmare_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_oculus_touch_the_nightmare_SpellScript);

            void HandleDamageCalc(SpellEffIndex /*effIndex*/)
            {
                SetHitDamage(int32(GetCaster()->CountPctFromMaxHealth(30)));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_oculus_touch_the_nightmare_SpellScript::HandleDamageCalc, EFFECT_2, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_oculus_touch_the_nightmare_SpellScript();
        }
};

// 50344 - Dream Funnel
class spell_oculus_dream_funnel : public SpellScriptLoader
{
    public:
        spell_oculus_dream_funnel() : SpellScriptLoader("spell_oculus_dream_funnel") { }

        class spell_oculus_dream_funnel_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_oculus_dream_funnel_AuraScript);

            void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, float& amount, bool& canBeRecalculated)
            {
                if (Unit* caster = GetCaster())
                    amount = int32(caster->CountPctFromMaxHealth(5));

                canBeRecalculated = false;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_oculus_dream_funnel_AuraScript::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_oculus_dream_funnel_AuraScript::HandleEffectCalcAmount, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_oculus_dream_funnel_AuraScript();
        }
};

// 53389 - GPS (Greater drake Positioning System)
class spell_oculus_gps : public SpellScriptLoader
{
    public:
        spell_oculus_gps() : SpellScriptLoader("spell_oculus_gps") { }

        class spell_oculus_gps_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_oculus_gps_AuraScript);

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                if (Unit* caster = GetCaster())
                    if (caster->IsAIEnabled)
                        caster->GetAI()->DoAction(ACTION_GPS);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_oculus_gps_AuraScript::HandleTriggerSpell, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_oculus_gps_AuraScript();
        }
};

class cond_loot_cache_of_the_ley_guardian : public ConditionScript
{
    public:
        cond_loot_cache_of_the_ley_guardian() : ConditionScript("cond_loot_cache_of_the_ley_guardian") { }

        bool OnConditionCheck(const Condition* /*condition*/, ConditionSourceInfo& sourceInfo) override
        {
            if (!sourceInfo.mConditionTargets[0])
                return false;

            if (Player* player = sourceInfo.mConditionTargets[0]->ToPlayer())
                if (InstanceScript* instance = player->GetInstanceScript())
                    if (CacheEligibilityCheckAccessor* accessor = dynamic_cast<CacheEligibilityCheckAccessor*>(instance))
                        return accessor->IsPlayerEligibleForCache(player->GetGUID());

            return false;
        }
};

void AddSC_oculus()
{
    new npc_verdisa_beglaristrasz_eternos();
    new npc_image_belgaristrasz();
    new npc_ruby_emerald_amber_drake();
    new go_nexus_portal();
    new spell_oculus_call_ruby_emerald_amber_drake();
    new spell_oculus_ride_ruby_emerald_amber_drake_que();
    new spell_oculus_evasive_maneuvers();
    new spell_oculus_shock_lance();
    new spell_oculus_stop_time();
    new spell_oculus_temporal_rift();
    new spell_oculus_touch_the_nightmare();
    new spell_oculus_dream_funnel();
    new spell_oculus_gps();
    new cond_loot_cache_of_the_ley_guardian();
}
