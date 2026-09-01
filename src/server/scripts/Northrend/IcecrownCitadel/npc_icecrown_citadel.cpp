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

#include "ScriptPCH.h"
#include "icecrown_citadel.h"

enum Spells
{
    //## Ancient Skeletal Soldier ##
    SPELL_SHIELD_BASH             = 70964,

    //## Deathbound Ward ##
    SPELL_SABER_LASH              = 71021,
    SPELL_DISRUPTING_SHOUT        = 71022,

    //## Nerubar Broodkeeper ##
    SPELL_CRYPT_SCARABS           = 70965,
    SPELL_DARK_MENDING            = 71020,
    SPELL_WEB_WRAP                = 70980,
    SPELL_WEB_WRAP_STUN           = 71010,
    SPELL_WEB_BEAM                = 69887,
    ACTION_DESCEND                = 1,
    DATA_DESCENDED                = 1,
    POINT_DESCEND                 = 1,
    EVENT_WEB_WRAP                = 1,
    EVENT_DARK_MENDING,

    //## Servant of the Throne ##
    SPELL_GLACIAL_BLAST           = 71029,
    POINT_DESCEND_2               = 2,

    //## The Damned ##
    SPELL_BONE_FLURRY             = 70960,
    SPELL_SHATTERED_BONES         = 70961,
    SPELL_SHATTERED_BONES_VISUAL  = 70963,
    ACTION_ASSAULT                = 1,

    // ## Deathspeaker Servant ##
    SPELL_CHAOS_BOLT_10M          = 69576,
    SPELL_CONSUMING_SHADOWS       = 69405,
    SPELL_CURSE_OF_AGONY_10M      = 69404,
    NPC_INVISIBLE_STALKER_SERVANT = 36848,
    NPC_RISEN_DEATHSPEAKER_SERVANT= 36844,
    SPELL_VOID_ZONE_VISUAL        = 69422,
    SPELL_RAISE_DEAD              = 69431,
    EVENT_CONSUMING_SHADOWS       = 1,
    EVENT_CURSE_OF_AGONE,

    // ## Deathspeaker Disciple ##
    SPELL_SHADOW_BOLT_DEATHSPEAKER= 69387,
    SPELL_DARK_BLESSING           = 69391,
    SPELL_SHADOW_MEND_10M         = 69389,
    EVENT_DARK_BLESSING           = 1,
    EVENT_SHADOW_MEND,

    //## Deathspeaker Attendant ##
    SPELL_SHADOW_NOVA_10M         = 69355,
    EVENT_SHADOW_NOVA             = 1,

    //## Deathspeaker Zealot ##
    SPELL_SHADOW_CLEAVE           = 69492,

    //## Deathspeaker High Priest ##
    SPELL_DARK_RECKONING          = 69483,
    SPELL_DARKRECKONING_EFFECT    = 69482,
    SPELL_AURA_OF_DARKNESS        = 69491,
    EVENT_DARKRECKONING           = 1,
    EVENT_DARKRECKONING_EFFECT,

    //## Valkyr Herald ##
    SPELL_SEVERED_ESSENCE_10M     = 71906,

    //## Blighted Abomination ##
    SPELL_CLEAVE                  = 40504,
    SPELL_PLAGUE_CLOUD            = 71150,
    SPELL_SCOURGE_HOOK            = 71140,

    //## Vengeful Fleshreaper ##
    SPELL_LEAPING_FACE_MAUL       = 71164,
    SPELL_DEVOUR_HUMANOID         = 71163,
    SAY_ALERT                     = 0,
    POINT_ALERT                   = 100,

    //## Plague Scientist ##
    SPELL_PLAGUE_BLAST            = 73079,
    SPELL_PLAGUE_STREAM           = 69871,
    SPELL_COMBOBULATING_SPRAY     = 71103,

    //## Pustulating Horror ##
    SPELL_BLIGHT_BOMB             = 71088,
    SPELL_BUBBLING_PUS_10M        = 71089,

    //## Decaying Colossus ##
    SPELL_MASSIVE_STOMP_10M       = 71114,

    //## Darkfallen Archmage ##
    SPELL_AMPLIFY_MAGIC_10M       = 70408,
    SPELL_BLAST_WAVE_10M          = 70407,
    SPELL_FIREBALL_10M            = 70409,
    SPELL_POLYMORPH_SPIDER        = 70410,
    SPELL_POLYMORPH_SPIDER_ALLY   = 72106,
    SPELL_SIPHON_ESSENCE          = 70299,

    //## Darkfallen Blood Knight ##
    SPELL_VAMPIRIC_AURA           = 71736,
    SPELL_UNHOLY_STRIKE           = 70437,
    SPELL_BLOOD_MIRROR_DAMAGE     = 70445,
    SPELL_BLOOD_MIRROR_BUFF       = 70451,
    SPELL_BLOOD_MIRROR_DUMMY      = 70450,

    //## Darkfallen Noble ##
    SPELL_CHAINS_OF_SHADOW        = 70645,
    SPELL_SHADOW_BOLT_10M         = 72960,

    //## Npc Vampiric Fiend ##
    SPELL_DISEASE_CLOUD           = 41290,
    SPELL_LEECHING_ROT            = 70671,

    //## Darkfallen Advisor ##
    SPELL_LICH_SLAP_10M           = 72057,
    SPELL_SHROUD_OF_PROTECTION    = 72065,
    SPELL_SHROUD_OF_SPELL_WARDING = 72066,

    //## Darkfallen Commander ##
    SPELL_VAMPIRE_RUSH_10M        = 70449,
    SPELL_BATTLE_SHOUT            = 70750,

    //## Darkfallen Lieutenant ##
    SPELL_VAMPIRIC_CURSE          = 70423,
    SPELL_REND_FLESH_10M          = 70435,

    //## Darkfallen Tactician ##
    SPELL_SHADOWSTEP              = 70431,
    SPELL_BLOOD_SAP               = 70432,

    //## Ymirjar Deathbringer ##
    SPELL_BANISH                  = 71298,
    SPELL_DEATHS_EMBRACE_10M      = 71299,
    SPELL_SHADOW_BOLT_YMIRJAR_10M = 71296,
    SPELL_SUMMON_YMIRJAR          = 71303,
    SPELL_AWAKEN_YMIRJAR_FALLEN   = 71302,
    EVENT_BANISH                  = 1,
    EVENT_DEATHS_EMBRACE,
    EVENT_SUMMON_YMIRJAR,

    //## Ymirjar Frostbinder ##
    SPELL_ARCTIC_CHILL            = 71270,
    SPELL_FROZEN_ORB              = 71274,
    SPELL_FROZEN_ORB_PRIMER       = 71287,
    SPELL_FROZEN_ORB_MISSILE      = 71285,
    SPELL_TWISTED_WINDS           = 71306,
    SPELL_SPIRIT_STREAM           = 69929,
    EVENT_FROZEN_ORB              = 1,
    EVENT_TWISTED_WINDS,

    //## Ymirjar Battle-Maiden ##
    SPELL_ADRENALINE_RUSH         = 71258,
    SPELL_BARBARIC_STRIKE         = 71257,
    ACTION_APPROACH               = 1,
    POINT_DOWNSTAIRS              = 1,
    POINT_FINAL,

    //## Ymirjar Huntress ##
    SPELL_ICE_TRAP                = 71249,
    SPELL_RAPID_SHOT              = 71251,
    SPELL_SHOOT                   = 71253,
    SPELL_VOLLEY                  = 71252,
    SPELL_SUMMON_WARHAWK          = 71705,
    EVENT_ICE_TRAP                = 1,
    EVENT_RAPID_SHOT,
    EVENT_VOLLEY,

    //## Ymirjar Warlord ##
    SPELL_WHIRLWIND               = 41056,

};

enum npcs
{
    NPC_SERVANT_OF_THE_THRONE    = 36724,
    NPC_ANCIENT_SKELETAL_SOLDIER = 37012,
    NPC_THE_DAMNED               = 37011,
    NPC_NERUBAR_BROODKEEPER      = 36725,
    NPC_DEATHSPEAKER_SERVANT     = 36805,
    NPC_DEATHSPEAKER_ZEALOT      = 36808,
    NPC_DEATHSPEAKER_DISCIPLE    = 36807,
    NPC_DEATHSPEAKER_ATTENDANT   = 36811,
    NPC_DEATHSPEAKER_HIGH_PRIEST = 36829,
    NPC_VALKYR_HERALD            = 37098,
    NPC_BLIGHTED_ABOMINATION     = 37022,
    NPC_PUSTULATING_HORROR       = 10404,
    NPC_PLAGUE_SCIENTIST         = 37023,
    NPC_DECAYING_COLOSSUS        = 36880,
    NPC_DARKFALLEN_ARCHMAGE      = 37664,
    NPC_DARKFALLEN_BLOOD_KNIGHT  = 37595,
    NPC_DARKFALLEN_NOBLE         = 37663,
    NPC_VAMPIRIC_FIEND           = 37901,
    NPC_DARKFALLEN_ADVISOR       = 37571,
    NPC_DARKFALLEN_COMMANDER     = 37662,
    NPC_DARKFALLEN_TACTICIAN     = 37666,
    NPC_DARKFALLEN_LIEUTENANT    = 37665,
    NPC_WARHAWK                  = 38154,

    //Empowering Orb Visual Stalker 38463
};

//#########  Ancient Skeletal Soldier ##############
class npc_ancient_skeletal_soldier_icc : public CreatureScript
{
    public:
        npc_ancient_skeletal_soldier_icc() : CreatureScript("npc_ancient_skeletal_soldier_icc") { }

        struct npc_ancient_skeletal_soldier_iccAI : public ScriptedAI
        {
            npc_ancient_skeletal_soldier_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiBASH_Timer;

            void Reset() override
            {
                m_uiBASH_Timer = 5000;
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_DESCENDED)
                    return descended;

                return 0;
            }

            void DoAction(int32 action) override
            {
                if (action != ACTION_DESCEND || descended)
                    return;

                descended = true;

                float dist;
                switch (me->GetDBTableGUIDLow())
                {
                    case 136113:
                    case 136114:
                    case 136115:
                    case 136116:
                        dist = 20.0f;
                        break;
                    default:
                        return;
                }

                Position pos;
                pos.Relocate(me);
                pos.RelocateOffset(0.0f, dist);
                //me->UpdateAllowedPositionZ(pos.GetPositionX(), pos.GetPositionY(), pos.m_positionZ, 5.0f, 100.0f);
                me->UpdateAllowedPositionZ(pos.GetPositionX(), pos.GetPositionY(), pos.m_positionZ);
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(0, pos);
                me->SetHomePosition(pos);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE && pointId == POINT_DESCEND && !finishedMovement)
                {
                    finishedMovement = true;
                    me->SetWalk(false);
                }
            }

            void AttackStart(Unit* victim) override
            {
                if (!finishedMovement)
                {
                    finishedMovement = true;
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();
                    me->SetWalk(false);
                }
                ScriptedAI::AttackStart(victim);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiBASH_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_SHIELD_BASH);
                    m_uiBASH_Timer = 10000;
                }
                else
                   m_uiBASH_Timer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            bool descended = false;
            bool finishedMovement = false;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_ancient_skeletal_soldier_iccAI>(creature);
        }
};

//#########  Deathbound Ward ##############
class npc_deathbound_ward_icc : public CreatureScript
{
    public:
        npc_deathbound_ward_icc() : CreatureScript("npc_deathbound_ward_icc") { }

        struct npc_deathbound_ward_iccAI : public ScriptedAI
        {
            npc_deathbound_ward_iccAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                ScriptedAI::InitializeAI();

                DoCast(me, SPELL_STONEFORM, true);
            }

            uint32 m_uiDisrupting_Shout_Timer;
            uint32 m_uiSaber_Lash_Timer;

            void Reset() override
            {
                m_uiDisrupting_Shout_Timer = 10000;
                m_uiSaber_Lash_Timer = 7000;

                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                if (awakened)
                    me->RemoveAurasDueToSpell(SPELL_STONEFORM);
            }

            void DoAction(int32 action) override
            {
                awakened = true;
                me->RemoveAurasDueToSpell(SPELL_STONEFORM);
                me->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
                me->GetMotionMaster()->InitDefault();
                //if (Unit* target = me->SelectNearestTarget(150.0f))
                //    AttackStart(target);
            }

            void JustReachedHome() override
            {
                if (awakened)
                    me->RemoveAurasDueToSpell(SPELL_STONEFORM);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiDisrupting_Shout_Timer <= diff)
                {
                    DoCastAOE(SPELL_DISRUPTING_SHOUT);
                    m_uiDisrupting_Shout_Timer = 15000;
                }
                else
                    m_uiDisrupting_Shout_Timer -= diff;

                if (m_uiSaber_Lash_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_SABER_LASH);
                    m_uiSaber_Lash_Timer = 7000;
                }
                else
                    m_uiSaber_Lash_Timer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            bool awakened = false;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_deathbound_ward_iccAI>(creature);
        }
};

//#########  Nerubar Broodkeeper ##############
class npc_nerubar_broodkeeper_icc : public CreatureScript
{
    public:
        npc_nerubar_broodkeeper_icc() : CreatureScript("npc_nerubar_broodkeeper_icc") { }

        struct npc_nerubar_broodkeeper_iccAI : public ScriptedAI
        {
            npc_nerubar_broodkeeper_iccAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_WEB_WRAP, urand(5000, 8000));
                events.ScheduleEvent(EVENT_DARK_MENDING, urand(5000, 8000));
                autoCastTimer.SetInterval(2500);
            }

            void AttackStart(Unit* victim) override
            {
                if (victim && me->Attack(victim, false))
                    casterMovement.Chase(victim);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_DESCENDED)
                    return descended;

                return 0;
            }

            void DoAction(int32 action) override
            {
                if (action != ACTION_DESCEND || descended)
                    return;

                descended = true;

                Position pos = me->GetPosition();
                //me->UpdateGroundPositionZ(pos.GetPositionX(), pos.GetPositionY(), pos.m_positionZ, -10.0f, 200.0f);
                me->UpdateGroundPositionZ(pos.GetPositionX(), pos.GetPositionY(), pos.m_positionZ);
                me->GetMotionMaster()->MoveLand(POINT_DESCEND, pos);

                DoCast(me, SPELL_WEB_BEAM);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == EFFECT_MOTION_TYPE && pointId == POINT_DESCEND)
                {
                    me->OverrideInhabitType(INHABIT_GROUND);
                    me->InterruptSpell(CURRENT_CHANNELED_SPELL);
                    me->SetHomePosition(*me);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->SetAnimTier(AnimTier::Ground);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                autoCastTimer.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                casterMovement.Update(diff);

                // no 'while' on purpose
                switch (events.ExecuteEvent())
                {
                    case EVENT_WEB_WRAP:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [](Unit* u) { return u->GetTypeId() == TYPEID_PLAYER && !u->HasAura(SPELL_WEB_WRAP) && !u->HasAura(SPELL_WEB_WRAP_STUN); }))
                            DoCast(target, SPELL_WEB_WRAP);
                        events.ScheduleEvent(EVENT_WEB_WRAP, urand(5000, 8000));
                        break;
                    case EVENT_DARK_MENDING:
                    {
                        std::list<Creature*> allies;
                        GetCreatureListWithEntryInGrid(allies, me, NPC_NERUBAR_BROODKEEPER, 35.0f);
                        GetCreatureListWithEntryInGrid(allies, me, NPC_THE_DAMNED, 35.0f);
                        GetCreatureListWithEntryInGrid(allies, me, NPC_ANCIENT_SKELETAL_SOLDIER, 35.0f);
                        GetCreatureListWithEntryInGrid(allies, me, NPC_SERVANT_OF_THE_THRONE, 35.0f);
                        allies.remove_if([](Creature* ally) { return ally->isDead() || ally->GetHealth() == ally->GetMaxHealth(); });
                        if (!allies.empty())
                        {
                            allies.sort(Trinity::HealthPctOrderPred());
                            DoCast(allies.front(), SPELL_DARK_MENDING);
                        }
                        events.ScheduleEvent(EVENT_DARK_MENDING, urand(5000, 8000));
                        break;
                    }
                    default:
                        if (autoCastTimer.Passed())
                        {
                            DoCastVictim(SPELL_CRYPT_SCARABS);
                            autoCastTimer.Reset();
                        }
                        break;
                }

                if (casterMovement.IsUnableToCast())
                    DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_CRYPT_SCARABS);
            IntervalTimer autoCastTimer;
            bool descended = false;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_nerubar_broodkeeper_iccAI>(creature);
        }
};

class spell_web_wrap_root : public SpellScriptLoader
{
    public:
        spell_web_wrap_root() : SpellScriptLoader("spell_web_wrap_root") { }

        class spell_web_wrap_root_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_web_wrap_root_AuraScript);

            void HandleRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (Unit* target = GetTarget())
                    target->CastSpell(target, SPELL_WEB_WRAP_STUN, true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_web_wrap_root_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_MOD_ROOT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_web_wrap_root_AuraScript();
        }
};

//#########  Servant of the Throne ##############
class npc_servant_of_the_throne_icc : public CreatureScript
{
    public:
        npc_servant_of_the_throne_icc() : CreatureScript("npc_servant_of_the_throne_icc") { }

        struct npc_servant_of_the_throne_iccAI : public ScriptedAI
        {
            npc_servant_of_the_throne_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiGlacial_Blast_Timer;

            void Reset() override
            {
                m_uiGlacial_Blast_Timer = 1000;
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_DESCENDED)
                    return descended;

                return 0;
            }

            void DoAction(int32 action) override
            {
                if (action != ACTION_DESCEND || descended)
                    return;

                descended = true;

                float dist;
                switch (me->GetDBTableGUIDLow())
                {
                    case 136133:
                    case 136140:
                        dist = 20.0f;
                        break;
                    case 508443:
                    {
                        me->SetHomePosition(Position { -210.0191f, 2210.979f, 35.23346f });
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(POINT_DESCEND, Position { -237.349f, 2211.522f, 42.65228f });
                        return;
                    }
                    default:
                        return;
                }

                Position pos;
                pos.Relocate(me);
                pos.RelocateOffset(0.0f, dist);
                //me->UpdateAllowedPositionZ(pos.GetPositionX(), pos.GetPositionY(), pos.m_positionZ, 5.0f, 100.0f);
                me->UpdateAllowedPositionZ(pos.GetPositionX(), pos.GetPositionY(), pos.m_positionZ);
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(POINT_DESCEND, pos);
                me->SetHomePosition(pos);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE && pointId == POINT_DESCEND)
                {
                    if (me->GetDBTableGUIDLow() == 93499)
                    {
                        me->GetMotionMaster()->MovePoint(POINT_DESCEND_2, Position { -210.0191f, 2210.979f, 35.23346f });
                    }
                    else
                    {
                        finishedMovement = true;
                        me->SetWalk(false);
                    }
                }
                if (type == POINT_MOTION_TYPE && pointId == POINT_DESCEND_2)
                {
                    finishedMovement = true;
                    me->SetWalk(false);
                }
            }

            void AttackStart(Unit* victim) override
            {
                if (!finishedMovement)
                {
                    finishedMovement = true;
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();
                    me->SetWalk(false);
                }
                if (victim && me->Attack(victim, false))
                    casterMovement.Chase(victim);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                casterMovement.Update(diff);

                if (m_uiGlacial_Blast_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                           DoCast(target, SPELL_GLACIAL_BLAST);
                    m_uiGlacial_Blast_Timer = 6000;
                }
                else
                   m_uiGlacial_Blast_Timer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_GLACIAL_BLAST);
            bool descended = false;
            bool finishedMovement = false;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_servant_of_the_throne_iccAI>(creature);
        }
};

class spell_glacial_blast : public SpellScriptLoader
{
    public:
        spell_glacial_blast() : SpellScriptLoader("spell_glacial_blast") { }

        class spell_glacial_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_glacial_blast_SpellScript);

            void TargetSelect(SpellDestination& destination)
            {
                Unit* caster = GetCaster();
                WorldLocation const* dest = GetExplTargetDest();
                if (!caster || !dest)
                    return;

                Position pos = caster->GetPosition();
                destination._position = WorldLocation(caster->GetMapId(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
                for (int32 dist = GetSpellInfo()->Effects[EFFECT_0].CalcValue(); dist > 0; dist -= 5)
                    caster->MovePositionToFirstCollision(destination._position, 5.0f, caster->GetRelativeAngle(dest));
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_glacial_blast_SpellScript::TargetSelect, EFFECT_0, TARGET_DEST_TARGET_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_glacial_blast_SpellScript();
        }
};

//#########  The Damned ##############
class npc_the_damned_icc : public CreatureScript
{
    public:
        npc_the_damned_icc() : CreatureScript("npc_the_damned_icc") { }

        struct npc_the_damned_iccAI : public ScriptedAI
        {
            npc_the_damned_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiBone_Flurry_Timer;

            void Reset() override
            {
                m_uiBone_Flurry_Timer = 20000;
            }

            void JustDied(Unit * killer) override
            {
                DoCastAOE(SPELL_SHATTERED_BONES);
                DoCast(me, SPELL_SHATTERED_BONES_VISUAL);
                DoCast(me, SPELL_SHATTERED_BONES_VISUAL);
                DoCast(me, SPELL_SHATTERED_BONES_VISUAL);
                DoCast(me, SPELL_SHATTERED_BONES_VISUAL);

                if (me->GetDBTableGUIDLow() == 136222 || me->GetDBTableGUIDLow() == 136223)
                    if (Creature* tirion = me->FindNearestCreature(NPC_HIGHLORD_TIRION_FORDRING_LH, 300.0f))
                        tirion->AI()->SetData(1, 1);
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_ASSAULT && (me->GetDBTableGUIDLow() == 582291 || me->GetDBTableGUIDLow() == 582292) && !assaulted)
                {
                    assaulted = true;
                    me->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
                    me->GetMotionMaster()->InitDefault();
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == WAYPOINT_MOTION_TYPE && pointId == 6 && (me->GetDBTableGUIDLow() == 582291 || me->GetDBTableGUIDLow() == 582292))
                {
                    me->SetDefaultMovementType(IDLE_MOTION_TYPE);
                    me->GetMotionMaster()->InitDefault();
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiBone_Flurry_Timer <= diff)
                {
                    DoCast(me, SPELL_BONE_FLURRY);
                    me->ClearUnitState(UNIT_STATE_CASTING);
                    m_uiBone_Flurry_Timer = urand(20000, 30000);
                }
                else
                    m_uiBone_Flurry_Timer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            bool assaulted = false;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_the_damned_iccAI>(creature);
        }
};

//#########  Deathspeaker Servant 10 man ##############
class npc_deathspeaker_servant_10man_icc : public CreatureScript
{
    public:
        npc_deathspeaker_servant_10man_icc() : CreatureScript("npc_deathspeaker_servant_10man_icc") { }

        struct npc_deathspeaker_servant_10man_iccAI : public ScriptedAI
        {
            npc_deathspeaker_servant_10man_iccAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_CONSUMING_SHADOWS, urand(3000, 5000));
                events.ScheduleEvent(EVENT_CURSE_OF_AGONE, urand(3000, 5000));
                autoCastTimer.SetInterval(3000);
            }

            void AttackStart(Unit* victim) override
            {
                if (victim && me->Attack(victim, false))
                    casterMovement.Chase(victim);
            }

            void JustDied(Unit* killer) override
            {
                if (me->GetEntry() == NPC_RISEN_DEATHSPEAKER_SERVANT)
                    return;

                me->m_Events.Schedule(1000, [this]()
                {
                    if (Creature* stalker = me->SummonCreature(NPC_INVISIBLE_STALKER_SERVANT, *me))
                    {
                        stalker->CastSpell(stalker, SPELL_VOID_ZONE_VISUAL, true);
                        stalker->m_Events.Schedule(5000, [stalker]()
                        {
                            if (Creature* risen = stalker->SummonCreature(NPC_RISEN_DEATHSPEAKER_SERVANT, *stalker))
                            {
                                risen->CastSpell(risen, SPELL_RAISE_DEAD, true);
                                risen->CastSpell(risen, 20480); // Birth, 3.5 secs cast
                                risen->m_Events.Schedule(5000, [risen]()
                                {
                                    risen->AI()->DoZoneInCombat(risen, 150.0f);
                                });
                            }
                            stalker->DespawnOrUnsummon(3000);
                        });
                    }
                });
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                autoCastTimer.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                casterMovement.Update(diff);

                // no 'while' on purpose
                switch (events.ExecuteEvent())
                {
                    case EVENT_CONSUMING_SHADOWS:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            DoCast(target, SPELL_CONSUMING_SHADOWS);
                        events.ScheduleEvent(EVENT_CONSUMING_SHADOWS, urand(8000, 15000));
                        break;
                    case EVENT_CURSE_OF_AGONE:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 25.0f))
                            DoCast(target, SPELL_CURSE_OF_AGONY_10M);
                        events.ScheduleEvent(EVENT_CURSE_OF_AGONE, urand(15000, 20000));
                        break;
                    default:
                        if (autoCastTimer.Passed())
                        {
                            DoCastVictim(SPELL_CHAOS_BOLT_10M);
                            autoCastTimer.Reset();
                        }
                        break;
                }

                if (casterMovement.IsUnableToCast())
                    DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_CHAOS_BOLT_10M);
            IntervalTimer autoCastTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_deathspeaker_servant_10man_iccAI>(creature);
        }
};

//#########  Deathspeaker Disciple 10 man ##############
class npc_deathspeaker_disciple_10man_icc : public CreatureScript
{
    public:
        npc_deathspeaker_disciple_10man_icc() : CreatureScript("npc_deathspeaker_disciple_10man_icc") { }

        struct npc_deathspeaker_disciple_10man_iccAI : public ScriptedAI
        {
            npc_deathspeaker_disciple_10man_iccAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_DARK_BLESSING, urand(3000, 5000));
                events.ScheduleEvent(EVENT_SHADOW_MEND, urand(5000, 10000));
                autoCastTimer.SetInterval(2500);
            }

            void AttackStart(Unit* victim) override
            {
                if (victim && me->Attack(victim, false))
                    casterMovement.Chase(victim);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                autoCastTimer.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                casterMovement.Update(diff);

                // no 'while' on purpose
                switch (events.ExecuteEvent())
                {
                    case EVENT_DARK_BLESSING:
                        DoCast(me, SPELL_DARK_BLESSING);
                        events.ScheduleEvent(EVENT_DARK_BLESSING, urand(8000, 15000));
                        break;
                    case EVENT_SHADOW_MEND:
                        DoCast(me, SPELL_SHADOW_MEND_10M);
                        events.ScheduleEvent(EVENT_SHADOW_MEND, urand(8000, 10000));
                        break;
                    default:
                        if (autoCastTimer.Passed())
                        {
                            DoCastVictim(SPELL_SHADOW_BOLT_DEATHSPEAKER);
                            autoCastTimer.Reset();
                        }
                        break;
                }

                if (casterMovement.IsUnableToCast())
                    DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_SHADOW_BOLT_DEATHSPEAKER);
            IntervalTimer autoCastTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_deathspeaker_disciple_10man_iccAI>(creature);
        }
};

//#########  Deathspeaker Attendant 10 man ##############
class npc_deathspeaker_attendant_10man_icc : public CreatureScript
{
    public:
        npc_deathspeaker_attendant_10man_icc() : CreatureScript("npc_deathspeaker_attendant_10man_icc") { }

        struct npc_deathspeaker_attendant_10man_iccAI : public ScriptedAI
        {
            npc_deathspeaker_attendant_10man_iccAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_SHADOW_NOVA, urand(5000, 10000));
                autoCastTimer.SetInterval(2500);
            }

            void AttackStart(Unit* victim) override
            {
                if (victim && me->Attack(victim, false))
                    casterMovement.Chase(victim);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                autoCastTimer.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                casterMovement.Update(diff);

                // no 'while' on purpose
                switch (events.ExecuteEvent())
                {
                    case EVENT_SHADOW_NOVA:
                        DoCastAOE(SPELL_SHADOW_NOVA_10M);
                        events.ScheduleEvent(EVENT_SHADOW_NOVA, urand(5000, 10000));
                        break;
                    default:
                        if (autoCastTimer.Passed())
                        {
                            DoCastVictim(SPELL_SHADOW_BOLT_DEATHSPEAKER);
                            autoCastTimer.Reset();
                        }
                        break;
                }

                if (casterMovement.IsUnableToCast())
                    DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_SHADOW_BOLT_DEATHSPEAKER);
            IntervalTimer autoCastTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_deathspeaker_attendant_10man_iccAI>(creature);
        }
};

//#########  Deathspeaker Zealot ##############
class npc_deathspeaker_zealot_icc : public CreatureScript
{
    public:
        npc_deathspeaker_zealot_icc() : CreatureScript("npc_deathspeaker_zealot_icc") { }

        struct npc_deathspeaker_zealot_iccAI : public ScriptedAI
        {
            npc_deathspeaker_zealot_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiCLEAVE_Timer;

            void Reset() override
            {
                m_uiCLEAVE_Timer = 6000;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiCLEAVE_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_SHADOW_CLEAVE);
                    m_uiCLEAVE_Timer = 6000;
                }
                else
                    m_uiCLEAVE_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_deathspeaker_zealot_iccAI>(creature);
        }
};

//#########  Deathspeaker High Priest ##############
//class npc_deathspeaker_high_priest_icc : public CreatureScript
//{
//    public:
//        npc_deathspeaker_high_priest_icc() : CreatureScript("npc_deathspeaker_high_priest_icc") { }
//
//        struct npc_deathspeaker_high_priest_iccAI : public ScriptedAI
//        {
//            npc_deathspeaker_high_priest_iccAI(Creature* creature) : ScriptedAI(creature) { }
//
//            uint32 m_uiRECKONING_Timer;
//            uint32 m_uiAURA_Timer;
//
//            void Reset() override
//            {
//                m_uiRECKONING_Timer = 2000;
//                m_uiAURA_Timer = 1000;
//            }
//
//            void UpdateAI(uint32 diff) override
//            {
//                if (!UpdateVictim())
//                    return;
//
//                if (m_uiAURA_Timer <= diff)
//                {
//                    DoCast(me, SPELL_AURA_OF_DARKNESS);
//                    m_uiAURA_Timer = urand(12000, 18000);
//                }
//                else
//                    m_uiAURA_Timer -= diff;
//
//                if (m_uiRECKONING_Timer <= diff)
//                {
//                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
//                        DoCast(target, SPELL_DARK_RECKONING);
//                    m_uiRECKONING_Timer = urand(10000, 12000);
//                }
//                else
//                    m_uiRECKONING_Timer -= diff;
//
//                DoMeleeAttackIfReady();
//            }
//        };
//
//        CreatureAI* GetAI(Creature* creature) const override
//        {
//            return GetIcecrownCitadelAI<npc_deathspeaker_high_priest_iccAI>(creature);
//        }
//};

class npc_deathspeaker_high_priest_icc : public CreatureScript
{
public:
    npc_deathspeaker_high_priest_icc() : CreatureScript("npc_deathspeaker_high_priest_icc") { }
    struct npc_deathspeaker_high_priest_iccAI: public ScriptedAI
    {
        npc_deathspeaker_high_priest_iccAI(Creature *c) : ScriptedAI(c)
        {
        }
        EventMap events;

        ObjectGuid targetGuid;

        void Reset() override
        {
            targetGuid = ObjectGuid::Empty;
            events.Reset();
        }

        void JustEngagedWith(Unit* who) override
        {
            events.ScheduleEvent(EVENT_DARKRECKONING, 10000);
            events.ScheduleEvent(EVENT_DARKRECKONING_EFFECT, 19000);
            DoCast(me, SPELL_AURA_OF_DARKNESS);
        }

        void UpdateAI(uint32 diff) override
        {
             //Return since we have no target
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_DARKRECKONING:
                        if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        {
                            targetGuid = pTarget->GetGUID();
                            DoCast(pTarget, SPELL_DARK_RECKONING);
                        }

                        events.RescheduleEvent(EVENT_DARKRECKONING, 20000);
                        break;
                    case EVENT_DARKRECKONING_EFFECT:
                        if (targetGuid)
                            if (Unit *target = me->GetUnit((*me), targetGuid))
                                if (me->IsValidAttackTarget(target))
                                    DoCast(target, SPELL_DARKRECKONING_EFFECT);

                        events.RescheduleEvent(EVENT_DARKRECKONING_EFFECT, 20000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetIcecrownCitadelAI<npc_deathspeaker_high_priest_iccAI>(creature);
    }
};

//#########  Valkyr Herald 10 man ##############
class npc_valkyr_herald_10man_icc : public CreatureScript
{
    public:
        npc_valkyr_herald_10man_icc() : CreatureScript("npc_valkyr_herald_10man_icc") { }

        struct npc_valkyr_herald_10man_iccAI : public ScriptedAI
        {
            npc_valkyr_herald_10man_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiESSENCE_Timer;

            void Reset() override
            {
                m_uiESSENCE_Timer = 1000;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiESSENCE_Timer <= diff)
                {
                    DoCast(me, SPELL_SEVERED_ESSENCE_10M);
                    m_uiESSENCE_Timer = urand(2000, 8000);
                }
                else
                    m_uiESSENCE_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_valkyr_herald_10man_iccAI>(creature);
        }
};

//#########  Blighted Abomination ##############
class npc_blighted_abomination_icc : public CreatureScript
{
    public:
        npc_blighted_abomination_icc() : CreatureScript("npc_blighted_abomination_icc") { }

        struct npc_blighted_abomination_iccAI : public ScriptedAI
        {
            npc_blighted_abomination_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiCLOUD_Timer;
            uint32 m_uiCLEAVE_Timer;
            uint32 m_uiHOOK_Timer;

            void Reset() override
            {
                m_uiCLOUD_Timer = 10000;
                m_uiCLEAVE_Timer = 4000;
                m_uiHOOK_Timer = 5000;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiCLOUD_Timer <= diff)
                {
                    DoCastAOE(SPELL_PLAGUE_CLOUD);
                    m_uiCLOUD_Timer = 20000;
                }
                else
                    m_uiCLOUD_Timer -= diff;

                if (m_uiHOOK_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_SCOURGE_HOOK);
                    m_uiHOOK_Timer = urand(12000, 20000);
                }
                else
                    m_uiHOOK_Timer -= diff;

                if (m_uiCLEAVE_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_CLEAVE);
                    m_uiCLEAVE_Timer = 6000;
                }
                else
                    m_uiCLEAVE_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_blighted_abomination_iccAI>(creature);
        }
};

//#########  Vengeful Fleshreaper ##############
Position const tubeGeistWaypoints[] =
{
    { 4432.42f, 3080.32f, 372.853f },
    { 4432.06f, 3041.54f, 372.788f },
    { 4370.74f, 3042.32f, 372.798f }, // 2 - jump
    { 4342.69f, 3041.52f, 372.792f },
    { 4281.32f, 3041.55f, 372.779f },
    { 4280.76f, 3079.92f, 372.870f },
    { 4281.32f, 3041.55f, 372.779f },
    { 4342.69f, 3041.52f, 372.792f }, // 7 - jump
    { 4370.74f, 3042.32f, 372.798f },
    { 4432.06f, 3041.54f, 372.788f },
};
#define MAX_TUBE_GEIST_WAYPOINTS (sizeof(tubeGeistWaypoints) / sizeof (tubeGeistWaypoints[0]))

Position const fleshreaperWaypoints[] =
{
    { 4356.92f, 2974.64f, 360.511f, 6.24785f },
    { 4372.77f, 2969.23f, 360.510f, 5.79625f },
    { 4385.85f, 2958.79f, 357.733f, 5.40749f },
    { 4392.94f, 2943.05f, 354.336f, 4.86950f },
    { 4391.34f, 2925.73f, 351.125f, 4.46895f },
    { 4381.76f, 2911.12f, 351.084f, 3.95846f },
    { 4366.01f, 2901.66f, 351.099f, 3.46758f },
    { 4348.46f, 2902.25f, 351.099f, 2.95707f },
    { 4331.11f, 2910.75f, 351.088f, 2.41908f },
    { 4321.93f, 2924.94f, 351.098f, 1.92035f },
    { 4320.50f, 2941.73f, 354.116f, 1.45304f },
    { 4326.74f, 2957.97f, 357.516f, 1.01321f },
    { 4339.47f, 2968.77f, 360.308f, 0.54983f },
};
#define MAX_FLESHREAPER_WAYPOINTS (sizeof(fleshreaperWaypoints) / sizeof (fleshreaperWaypoints[0]))

class npc_vengeful_fleshreapert_icc : public CreatureScript
{
    public:
        npc_vengeful_fleshreapert_icc() : CreatureScript("npc_vengeful_fleshreapert_icc") { }

        struct npc_vengeful_fleshreapert_iccAI : public ScriptedAI
        {
            npc_vengeful_fleshreapert_iccAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                if (me->GetPositionZ() > 370)
                {
                    tubeGeist = true;

                    uint32 wp = 0;
                    float closestDist = 1000.0f;
                    for (uint32 i = 0; i < MAX_TUBE_GEIST_WAYPOINTS; ++i)
                    {
                        float dist = me->GetExactDist(&tubeGeistWaypoints[i]);
                        if (closestDist > dist)
                        {
                            closestDist = dist;
                            wp = i;
                        }
                    }
                    wp = (wp + 1) % MAX_TUBE_GEIST_WAYPOINTS;
                    me->m_Events.Schedule(me->GetPositionX() > 4356.0f ? urand(5000, 10000) : 1000, [this, wp]() // add some randomization to avoid geists mirroring each other
                    {
                        me->GetMotionMaster()->MovePoint(wp, tubeGeistWaypoints[wp]);
                    });
                }
                else if (!me->ToTempSummon())
                {
                    // Randomize own waypoints (no sniffed info)
                    for (auto&& pos : fleshreaperWaypoints)
                    {
                        Position wp;
                        wp.Relocate(pos);
                        wp.RelocateOffset(M_PI / 2, frand(-5.0f, 5.0f));
                        //me->UpdateGroundPositionZ(wp.GetPositionX(), wp.GetPositionY(), wp.m_positionZ, 10.0f, 100.0f);
                        me->UpdateGroundPositionZ(wp.GetPositionX(), wp.GetPositionY(), wp.m_positionZ);
                        runWaypoints.push_back(wp);
                    }
                    for (uint32 i = 0; i < MAX_FLESHREAPER_WAYPOINTS; ++i)
                    {
                        float distNominal = fleshreaperWaypoints[i].GetExactDist(&fleshreaperWaypoints[(i + 1) % MAX_FLESHREAPER_WAYPOINTS]);
                        float distReal = runWaypoints[i].GetExactDist(&runWaypoints[(i + 1) % MAX_FLESHREAPER_WAYPOINTS]);
                        float time = distNominal / me->GetSpeed(MOVE_RUN);
                        runWaypoints[i].SetOrientation((distReal / time) / baseMoveSpeed[MOVE_RUN]);
                    }
                    currentWaypoint = 0;
                    me->SetSpeed(MOVE_RUN, runWaypoints[currentWaypoint].GetOrientation());
                    me->GetMotionMaster()->MovePoint(currentWaypoint, runWaypoints[currentWaypoint]);
                }

                ScriptedAI::InitializeAI();
            }

            uint32 m_uiJUMP_Timer;

            void Reset() override
            {
                m_uiJUMP_Timer = tubeGeist ? 1 : urand(3000, 20000);
            }

            void DoAction(int32 action) override
            {
                float x, y;
                me->GetNearPoint2D(x, y, frand(10.0f, 20.0f), me->GetOrientation());
                me->GetMotionMaster()->MovePoint(POINT_ALERT, x, y, me->GetPositionZ());
                if (!action)
                    Talk(SAY_ALERT);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                    return;

                if (pointId == POINT_ALERT)
                {
                    if (!me->IsInCombat())
                        DoZoneInCombat(nullptr, 100.0f);
                    return;
                }

                if (tubeGeist)
                {
                    uint32 wp = (pointId + 1) % MAX_TUBE_GEIST_WAYPOINTS;
                    if (pointId == 2 || pointId == 7)
                        me->GetMotionMaster()->MoveJump(tubeGeistWaypoints[wp].GetPositionX(), tubeGeistWaypoints[wp].GetPositionY(), tubeGeistWaypoints[wp].GetPositionZ(), me->GetExactDist2d(&tubeGeistWaypoints[wp]) * 10.0f / 17.5f, 17.5f, wp);
                    else
                        me->GetMotionMaster()->MovePoint(wp, tubeGeistWaypoints[wp]);
                }
                else if (currentWaypoint >= 0 && !me->IsInCombat())
                {
                    currentWaypoint = (pointId + 1) % runWaypoints.size();
                    me->SetSpeed(MOVE_RUN, runWaypoints[currentWaypoint].GetOrientation());
                    me->GetMotionMaster()->MovePoint(currentWaypoint, runWaypoints[currentWaypoint]);
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                if (!_EnterEvadeMode())
                    return;

                Reset();

                if (currentWaypoint >= 0)
                    me->GetMotionMaster()->MovePoint(currentWaypoint, runWaypoints[currentWaypoint]);
                else
                    me->GetMotionMaster()->MoveTargetedHome();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiJUMP_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, -5.0f))
                    {
                        if (tubeGeist)
                        {
                            tubeGeist = false;
                            me->SetHomePosition(*target);
                        }
                        DoCast(target, SPELL_LEAPING_FACE_MAUL);
                    }
                    m_uiJUMP_Timer = urand(20000,40000);
                }
                else
                    m_uiJUMP_Timer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            bool tubeGeist = false;
            std::vector<Position> runWaypoints;
            int32 currentWaypoint = -1;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_vengeful_fleshreapert_iccAI>(creature);
        }
};

//#########  Plague Scientist ##############
class npc_plague_scientist_icc : public CreatureScript
{
    public:
        npc_plague_scientist_icc() : CreatureScript("npc_plague_scientist_icc") { }

        struct npc_plague_scientist_iccAI : public ScriptedAI
        {
            npc_plague_scientist_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiBLAST_Timer;
            uint32 m_uiSTREAM_Timer;
            uint32 m_uiSTREAM_OUT_Timer;
            uint32 m_uiSPRAY_Timer;

            void Reset() override
            {
                m_uiBLAST_Timer = 2000;
                m_uiSTREAM_Timer = urand(8000, 12000);
                m_uiSTREAM_OUT_Timer = 24000;
                m_uiSPRAY_Timer = urand(5000, 8000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiSTREAM_Timer <= diff)
                {
                    if (m_uiSTREAM_OUT_Timer > diff)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            DoCast(target, SPELL_PLAGUE_STREAM);
                        m_uiSTREAM_OUT_Timer -= diff;
                    }
                    else
                    {
                        me->CastStop(SPELL_PLAGUE_STREAM);
                        m_uiSTREAM_OUT_Timer = urand(15000, 20000);
                    }
                }
                else
                    m_uiSTREAM_Timer -= diff;

                if (m_uiSPRAY_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_COMBOBULATING_SPRAY);
                    m_uiSPRAY_Timer = urand(8000, 12000);
                }
                else
                    m_uiSPRAY_Timer -= diff;

                if (m_uiBLAST_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_PLAGUE_BLAST);
                    m_uiBLAST_Timer = urand(2000, 3000);
                }
                else
                    m_uiBLAST_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_plague_scientist_iccAI>(creature);
        }
};

//#########  Pustulating Horror 10 man ##############
class npc_pustulating_horror_10man_icc : public CreatureScript
{
    public:
        npc_pustulating_horror_10man_icc() : CreatureScript("npc_pustulating_horror_10man_icc") { }

        struct npc_pustulating_horror_10man_iccAI : public ScriptedAI
        {
            npc_pustulating_horror_10man_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiPUS_Timer;

            void Reset() override
            {
                m_uiPUS_Timer = 2000;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (HealthBelowPct(15))
                {
                    DoCastAOE(SPELL_BLIGHT_BOMB);
                    m_uiPUS_Timer = 5000;
                }

                if (m_uiPUS_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_NEAREST, 0))
                        DoCast(target, SPELL_BUBBLING_PUS_10M);
        if (HealthAbovePct(15))
                        m_uiPUS_Timer = urand(15000, 25000);
                }
                else
                    m_uiPUS_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_pustulating_horror_10man_iccAI>(creature);
        }
};

//#########  Decaying Colossus 10 man ##############
class npc_decaying_colossus_10man_icc : public CreatureScript
{
    public:
        npc_decaying_colossus_10man_icc() : CreatureScript("npc_decaying_colossus_10man_icc") { }

        struct npc_decaying_colossus_10man_iccAI : public ScriptedAI
        {
            npc_decaying_colossus_10man_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiSTOMP_Timer;

            void Reset() override
            {
                m_uiSTOMP_Timer = 5000;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiSTOMP_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_MASSIVE_STOMP_10M);
                    m_uiSTOMP_Timer = urand(15000, 25000);
                }
                else
                    m_uiSTOMP_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_decaying_colossus_10man_iccAI>(creature);
        }
};

void DarkfallenTrashScene(Creature* me, Creature* transformSource = nullptr)
{
    if (!me->IsAlive() || me->IsInCombat())
        return;

    Creature* transformed = nullptr;
    switch (transformSource ? 0 : urand(0, 6))
    {
        case 0:
            me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
            if (transformSource && transformSource != me)
                return;
            break;
        case 1: me->HandleEmoteCommand(EMOTE_ONESHOT_TALK); break;
        case 2: me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION); break;
        case 3: me->HandleEmoteCommand(EMOTE_ONESHOT_YES); break;
        case 4: me->HandleEmoteCommand(EMOTE_ONESHOT_NO); break;
        case 5: me->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION); break;
        case 6:
        {
            std::list<Creature*> trash;
            GetCreatureListWithEntryInGrid(trash, me, NPC_DARKFALLEN_BLOOD_KNIGHT, 20.0f);
            GetCreatureListWithEntryInGrid(trash, me, NPC_DARKFALLEN_NOBLE, 20.0f);
            GetCreatureListWithEntryInGrid(trash, me, NPC_DARKFALLEN_ARCHMAGE, 20.0f);
            GetCreatureListWithEntryInGrid(trash, me, NPC_DARKFALLEN_TACTICIAN, 20.0f);
            trash.remove_if([](Creature* c) { return c->GetPositionZ() < 400.0f; });
            trash.remove(me);
            if (!trash.empty())
                if (transformed = Trinity::Containers::SelectRandomContainerElement(trash))
                    me->CastSpell(transformed, SPELL_POLYMORPH_SPIDER_ALLY);
            break;
        }
        default:
            break;
    }

    me->m_Events.Schedule(3000, [me, transformed]()
    {
        if (!me->IsAlive() || me->IsInCombat())
            return;

        std::list<Creature*> trash;
        GetCreatureListWithEntryInGrid(trash, me, NPC_DARKFALLEN_BLOOD_KNIGHT, 20.0f);
        GetCreatureListWithEntryInGrid(trash, me, NPC_DARKFALLEN_NOBLE, 20.0f);
        GetCreatureListWithEntryInGrid(trash, me, NPC_DARKFALLEN_ARCHMAGE, 20.0f);
        GetCreatureListWithEntryInGrid(trash, me, NPC_DARKFALLEN_TACTICIAN, 20.0f);
        trash.remove_if([](Creature* c) { return c->GetPositionZ() < 400.0f; });
        trash.remove(transformed ? transformed : me);
        if (trash.empty())
            return;

        if (transformed)
        {
            for (auto&& creature : trash)
                DarkfallenTrashScene(creature, me);
        }
        else if (Creature* creature = Trinity::Containers::SelectRandomContainerElement(trash))
            DarkfallenTrashScene(creature);
    });
}

//#########  Darkfallen Archmage 10 man ##############
class npc_darkfallen_archmage_10man_icc : public CreatureScript
{
    public:
        npc_darkfallen_archmage_10man_icc() : CreatureScript("npc_darkfallen_archmage_10man_icc") { }

        struct npc_darkfallen_archmage_10man_iccAI : public ScriptedAI
        {
            npc_darkfallen_archmage_10man_iccAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                ScriptedAI::InitializeAI();

                if (me->GetDBTableGUIDLow() == 71062)
                    DarkfallenTrashScene(me);
            }

            uint32 m_uiAMPLIFY_Timer;
            uint32 m_uiBLAST_Timer;
            uint32 m_uiFIREBALL_Timer;
            uint32 m_uiPOLYMORPH_Timer;

            void Reset() override
            {
                m_uiAMPLIFY_Timer = urand(10000, 15000);
                m_uiBLAST_Timer = urand(8000, 10000);
                m_uiFIREBALL_Timer = 2000;
                m_uiPOLYMORPH_Timer = urand(9000, 12000);
            }

            void JustEngagedWith(Unit* who) override
            {
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
            }

            void AttackStart(Unit* victim) override
            {
                if (victim && me->Attack(victim, false))
                    casterMovement.Chase(victim);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasAura(SPELL_SIPHON_ESSENCE))
                    return;

                casterMovement.Update(diff);

                if (m_uiAMPLIFY_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_AMPLIFY_MAGIC_10M);
                    m_uiAMPLIFY_Timer = urand(15000, 20000);
                }
                else
                    m_uiAMPLIFY_Timer -= diff;

                if (m_uiPOLYMORPH_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_POLYMORPH_SPIDER);
                    m_uiPOLYMORPH_Timer = urand(15000, 18000);
                }
                else
                    m_uiPOLYMORPH_Timer -= diff;

                if (m_uiFIREBALL_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_FIREBALL_10M);
                    m_uiFIREBALL_Timer = urand(3000, 4000);
                }
                else
                    m_uiFIREBALL_Timer -= diff;

                if (m_uiBLAST_Timer <= diff)
                {
                    DoCastAOE(SPELL_BLAST_WAVE_10M);
                    m_uiBLAST_Timer = urand(10000, 20000);
                }
                else
                    m_uiBLAST_Timer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_AMPLIFY_MAGIC_10M).Spell(SPELL_FIREBALL_10M);
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_darkfallen_archmage_10man_iccAI>(creature);
        }
};

//#########  Darkfallen Blood Knight ##############
class npc_darkfallen_blood_knight_icc : public CreatureScript
{
    public:
        npc_darkfallen_blood_knight_icc() : CreatureScript("npc_darkfallen_blood_knight_icc") { }

        struct npc_darkfallen_blood_knight_iccAI : public ScriptedAI
        {
            npc_darkfallen_blood_knight_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiAURA_Timer;
            uint32 m_uiSTRIKE_Timer;
            uint32 m_uiMIRROR_Timer;

            void Reset() override
            {
                m_uiAURA_Timer = urand(12000, 15000);
                m_uiSTRIKE_Timer = urand(2000, 3000);
                m_uiMIRROR_Timer = urand(4000, 5000);
            }

            void JustEngagedWith(Unit* who) override
            {
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasAura(SPELL_SIPHON_ESSENCE))
                    return;

                if (m_uiSTRIKE_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_UNHOLY_STRIKE);
                    m_uiSTRIKE_Timer = urand(3000, 4000);
                }
                else
                    m_uiSTRIKE_Timer -= diff;

                if (m_uiAURA_Timer <= diff)
                {
                    DoCast(me, SPELL_VAMPIRIC_AURA);
                    m_uiAURA_Timer = urand(12000, 15000);
                }
                else
                    m_uiAURA_Timer -= diff;

                if (m_uiMIRROR_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                    {
                        DoCastAOE(SPELL_BLOOD_MIRROR_DUMMY);
                        target->CastSpell(me, SPELL_BLOOD_MIRROR_DAMAGE, true);
                        me->CastSpell(target, SPELL_BLOOD_MIRROR_BUFF, true);
                        m_uiMIRROR_Timer = urand(32000, 37000);
                    }
                }
                else
                    m_uiMIRROR_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_darkfallen_blood_knight_iccAI>(creature);
        }
};

//#########  Darkfallen Noble 10 man ##############
class npc_darkfallen_noble_10man_icc : public CreatureScript
{
    public:
        npc_darkfallen_noble_10man_icc() : CreatureScript("npc_darkfallen_noble_10man_icc") { }

        struct npc_darkfallen_noble_10man_iccAI : public ScriptedAI
        {
            npc_darkfallen_noble_10man_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiCHAINS_Timer;
            uint32 m_uiBOLT_Timer;
            uint32 m_uiFIEND_Timer;

            void Reset() override
            {
                m_uiCHAINS_Timer = urand(2000, 4000);
                m_uiBOLT_Timer = urand(3000, 5000);
                m_uiFIEND_Timer = 15000;
            }

            void JustEngagedWith(Unit* who) override
            {
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
            }

            void AttackStart(Unit* victim) override
            {
                if (victim && me->Attack(victim, false))
                    casterMovement.Chase(victim);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasAura(SPELL_SIPHON_ESSENCE))
                    return;

                casterMovement.Update(diff);

                if (m_uiBOLT_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_SHADOW_BOLT_10M);
                    m_uiBOLT_Timer = urand(4000, 5000);
                }
                else
                    m_uiBOLT_Timer -= diff;

                if (m_uiCHAINS_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_CHAINS_OF_SHADOW);
                    m_uiCHAINS_Timer = urand(20000, 25000);
                }
                else
                    m_uiCHAINS_Timer -= diff;

                if (m_uiFIEND_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        if (me->SummonCreature(NPC_VAMPIRIC_FIEND,target->GetPositionX(),target->GetPositionY(),target->GetPositionZ()))
                            m_uiFIEND_Timer = 60000;
                }
                else
                    m_uiFIEND_Timer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_SHADOW_BOLT_10M);
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_darkfallen_noble_10man_iccAI>(creature);
        }
};

 //#########  Npc Vampiric Fiend ##############
class npc_vampiric_fiend_icc : public CreatureScript
{
    public:
        npc_vampiric_fiend_icc() : CreatureScript("npc_vampiric_fiend_icc") { }

        struct npc_vampiric_fiend_iccAI : public ScriptedAI
        {
            npc_vampiric_fiend_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiLEECHING_Timer;

            void Reset() override
            {
                m_uiLEECHING_Timer = 10000;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCast(me, SPELL_DISEASE_CLOUD);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiLEECHING_Timer <= diff)
                {
                    DoCastAOE(SPELL_LEECHING_ROT);
                    me->m_Events.Schedule(3000, [this]() { me->SetVisible(false); });
                    me->DespawnOrUnsummon(13000);
                    m_uiLEECHING_Timer = 20000;
                }
                else
                    m_uiLEECHING_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_vampiric_fiend_iccAI>(creature);
        }
};

//#########  Darkfallen Advisor 10 man ##############
class npc_darkfallen_advisor_10man_icc : public CreatureScript
{
    public:
        npc_darkfallen_advisor_10man_icc() : CreatureScript("npc_darkfallen_advisor_10man_icc") { }

        struct npc_darkfallen_advisor_10man_iccAI : public ScriptedAI
        {
            npc_darkfallen_advisor_10man_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiLICH_Timer;
            uint32 m_uiPROTECTION_Timer;

            void Reset() override
            {
                m_uiLICH_Timer = urand(2000, 5000);
                m_uiPROTECTION_Timer = urand(10000, 15000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiLICH_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_LICH_SLAP_10M);
                    m_uiLICH_Timer = 10000;
                }
                else
                    m_uiLICH_Timer -= diff;

                if (m_uiPROTECTION_Timer <= diff)
                {
                    if (Unit* target = urand(0, 1) ? SelectTarget(SELECT_TARGET_RANDOM, 0) : DoSelectLowestHpFriendly(40.0f))
                    {
                        DoCast(target,SPELL_SHROUD_OF_PROTECTION);
                        DoCast(target,SPELL_SHROUD_OF_SPELL_WARDING);
                        m_uiPROTECTION_Timer = urand(15000, 20000);
                    }
                }
                else
                    m_uiPROTECTION_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_darkfallen_advisor_10man_iccAI>(creature);
        }
};

//#########  Darkfallen Commander 10 man ##############
class npc_darkfallen_commander_10man_icc : public CreatureScript
{
    public:
        npc_darkfallen_commander_10man_icc() : CreatureScript("npc_darkfallen_commander_10man_icc") { }

        struct npc_darkfallen_commander_10man_iccAI : public ScriptedAI
        {
            npc_darkfallen_commander_10man_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiRUSH_Timer;
            uint32 m_uiSHOUT_Timer;

            void Reset() override
            {
                m_uiRUSH_Timer = urand(4000, 8000);
                m_uiSHOUT_Timer = urand(8000, 10000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiSHOUT_Timer <= diff)
                {
                    DoCast(me, SPELL_BATTLE_SHOUT);
                    m_uiSHOUT_Timer = urand(15000, 20000);
                }
                else
                    m_uiSHOUT_Timer -= diff;

                if (m_uiRUSH_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_VAMPIRE_RUSH_10M);
                    m_uiRUSH_Timer = urand(10000, 15000);
                }
                else
                    m_uiRUSH_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_darkfallen_commander_10man_iccAI>(creature);
        }
};

//#########  Darkfallen Lieutenant 10M ##############
class npc_darkfallen_lieutenant_10man_icc : public CreatureScript
{
    public:
        npc_darkfallen_lieutenant_10man_icc() : CreatureScript("npc_darkfallen_lieutenant_10man_icc") { }

        struct npc_darkfallen_lieutenant_10man_iccAI : public ScriptedAI
        {
            npc_darkfallen_lieutenant_10man_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiREND_Timer;
            uint32 m_uiCURSE_Timer;

            void Reset() override
            {
                m_uiREND_Timer = urand(1000, 2000);
                m_uiCURSE_Timer = urand(8000, 15000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiREND_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_REND_FLESH_10M);
                    m_uiREND_Timer = 25000;
                }
                else
                    m_uiREND_Timer -= diff;

                if (m_uiCURSE_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_VAMPIRIC_CURSE);
                    m_uiCURSE_Timer = urand(10000, 20000);
                }
                else
                    m_uiCURSE_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_darkfallen_lieutenant_10man_iccAI>(creature);
        }
};

//#########  Darkfallen Tactician ##############
class npc_darkfallen_tactician_icc : public CreatureScript
{
    public:
        npc_darkfallen_tactician_icc() : CreatureScript("npc_darkfallen_tactician_icc") { }

        struct npc_darkfallen_tactician_iccAI : public ScriptedAI
        {
            npc_darkfallen_tactician_iccAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 m_uiSHADOWSTEP_Timer;
            uint32 m_uiSAP_Timer;
            uint32 m_uiSTRIKE_Timer;

            void Reset() override
            {
                m_uiSHADOWSTEP_Timer = urand(1000, 2000);
                m_uiSAP_Timer = urand(5000, 15000);
                m_uiSTRIKE_Timer = urand(2000, 3000);
            }

            void JustEngagedWith(Unit* who) override
            {
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiSTRIKE_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_UNHOLY_STRIKE);
                    m_uiSTRIKE_Timer = 6000;
                }
                else
                    m_uiSTRIKE_Timer -= diff;

                if (m_uiSAP_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_BLOOD_SAP);
                    m_uiSAP_Timer = urand(15000, 25000);
                }
                else
                    m_uiSAP_Timer -= diff;

                if (m_uiSHADOWSTEP_Timer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_SHADOWSTEP);
                    m_uiSHADOWSTEP_Timer = urand(15000, 20000);
                }
                else
                    m_uiSHADOWSTEP_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_darkfallen_tactician_iccAI>(creature);
        }
};

//#########  Ymirjar Deathbringer 10 man ##############
class npc_ymirjar_deathbringer_10man_icc : public CreatureScript
{
    public:
        npc_ymirjar_deathbringer_10man_icc() : CreatureScript("npc_ymirjar_deathbringer_10man_icc") { }

        struct npc_ymirjar_deathbringer_10man_iccAI : public ScriptedAI
        {
            npc_ymirjar_deathbringer_10man_iccAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                ScriptedAI::InitializeAI();

                if (me->IsAlive())
                    me->SetFullHealth();
            }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_BANISH, urand(5000, 10000));
                events.ScheduleEvent(EVENT_DEATHS_EMBRACE, urand(10000, 15000));
                events.ScheduleEvent(EVENT_SUMMON_YMIRJAR, 1);
                autoCastTimer.SetInterval(2500);
            }

            void JustEngagedWith(Unit* who) override
            {
                me->InterruptSpell(CURRENT_CHANNELED_SPELL);
                channelCooldown = 60000;
            }

            void JustReachedHome() override
            {
                channelCooldown = 10000;
            }

            void AttackStart(Unit* victim) override
            {
                if (victim && me->Attack(victim, true))
                    casterMovement.Chase(victim);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_SUMMON_YMIRJAR)
                    for (uint8 i = 0; i < 5; ++i)
                        DoCastAOE(SPELL_AWAKEN_YMIRJAR_FALLEN, true);
            }

            void JustSummoned(Creature* summon) override
            {
                summon->m_Events.Schedule(1, [summon]() { summon->ToTempSummon()->SetTempSummonType(TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT); });
                summon->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_INSTANTLY_APPEAR_MODEL);
                summon->CastSpell(summon, 26586, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->IsInCombat() && me->IsAlive())
                {
                    if (channelCooldown > diff)
                    {
                        channelCooldown -= diff;
                        return;
                    }
                    else
                        channelCooldown = 0;

                    if (!me->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                    {
                        Position pos;
                        pos.Relocate(me);
                        pos.RelocateOffset(0.0f, 20.0f, 15.0f);
                        me->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_SPIRIT_STREAM, false);
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);
                autoCastTimer.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                casterMovement.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_BANISH:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            DoCast(target, SPELL_BANISH);
                        events.ScheduleEvent(EVENT_BANISH, urand(15000, 25000));
                        break;
                    case EVENT_DEATHS_EMBRACE:
                        DoCast(me, SPELL_DEATHS_EMBRACE_10M);
                        events.ScheduleEvent(EVENT_DEATHS_EMBRACE, urand(15000, 25000));
                        break;
                    case EVENT_SUMMON_YMIRJAR:
                        DoCastAOE(SPELL_SUMMON_YMIRJAR);
                        break;
                    default:
                        if (autoCastTimer.Passed())
                        {
                            DoCastVictim(SPELL_SHADOW_BOLT_YMIRJAR_10M);
                            autoCastTimer.Reset();
                        }
                        break;
                }
            }

        private:
            EventMap events;
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_SHADOW_BOLT_YMIRJAR_10M);
            IntervalTimer autoCastTimer;
            uint32 channelCooldown = 0;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_ymirjar_deathbringer_10man_iccAI>(creature);
        }
};

//#########  Ymirjar Frostbinder ##############
class npc_ymirjar_frostbinder_icc : public CreatureScript
{
    public:
        npc_ymirjar_frostbinder_icc() : CreatureScript("npc_ymirjar_frostbinder_icc") { }

        struct npc_ymirjar_frostbinder_iccAI : public ScriptedAI
        {
            npc_ymirjar_frostbinder_iccAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                ScriptedAI::InitializeAI();

                if (me->IsAlive())
                    me->SetFullHealth();
            }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_FROZEN_ORB, 4000);
                events.ScheduleEvent(EVENT_TWISTED_WINDS, urand(20000, 25000));
            }

            void JustEngagedWith(Unit* who) override
            {
                me->InterruptSpell(CURRENT_CHANNELED_SPELL);
                me->SetHover(true);
                DoCast(me, SPELL_ARCTIC_CHILL, true);
                channelCooldown = 60000;
            }

            void JustReachedHome() override
            {
                me->SetHover(false);
                channelCooldown = 10000;
            }

            void JustDied(Unit* killer) override
            {
                me->SetHover(false);
            }

            void AttackStart(Unit* victim) override
            {
                if (victim && me->Attack(victim, true))
                    casterMovement.Chase(victim);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_FROZEN_ORB_PRIMER)
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        DoCast(target, SPELL_FROZEN_ORB_MISSILE, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->IsInCombat() && me->IsAlive())
                {
                    if (channelCooldown > diff)
                    {
                        channelCooldown -= diff;
                        return;
                    }
                    else
                        channelCooldown = 0;

                    if (!me->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                    {
                        Position pos;
                        pos.Relocate(me);
                        pos.RelocateOffset(0.0f, 20.0f, 15.0f);
                        me->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_SPIRIT_STREAM, false);
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                casterMovement.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_FROZEN_ORB:
                        DoCastAOE(SPELL_FROZEN_ORB, true);
                        events.ScheduleEvent(EVENT_FROZEN_ORB, 12000);
                        break;
                    case EVENT_TWISTED_WINDS:
                        DoCastAOE(SPELL_TWISTED_WINDS, true);
                        events.ScheduleEvent(EVENT_TWISTED_WINDS, urand(20000, 25000));
                        break;
                    default:
                        break;
                }
            }

        private:
            EventMap events;
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_FROZEN_ORB);
            uint32 channelCooldown = 0;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_ymirjar_frostbinder_iccAI>(creature);
        }
};

struct YmirjarMovementInfo
{
    uint32 GuidLow;
    uint32 Delay;
    Position StartPosition;
};
YmirjarMovementInfo ymirjarMovementInfo[] =
{
    // Ymirjar Battle-Maiden
    { 77653,     0, { 4348.401f, 2552.156f, 355.0806f, M_PI / 2 } },
    { 77654,     0, { 4367.549f, 2551.667f, 355.6538f, M_PI / 2 } },
    { 77659, 15000, { 4348.182f, 2537.260f, 358.5151f, M_PI / 2 } },
    { 77660, 15000, { 4367.374f, 2536.133f, 358.5151f, M_PI / 2 } },
    // Ymirjar Huntress
    { 77649,     0, { 4352.708f, 2556.744f, 353.9358f, M_PI / 2 } },
    { 77650,     0, { 4363.049f, 2556.310f, 353.9358f, M_PI / 2 } },
    { 77657, 15000, { 4363.653f, 2542.996f, 357.9427f, M_PI / 2 } },
    { 77658, 15000, { 4351.962f, 2543.756f, 357.3703f, M_PI / 2 } },
    // Ymirjar Warlord
    { 78004,     0, { 4356.827f, 2544.576f, 357.0536f, M_PI / 2 } },
};

struct npc_ymirjar_icc_base : public ScriptedAI
{
    npc_ymirjar_icc_base(Creature* creature) : ScriptedAI(creature) { }

    void InitializeAI() override
    {
        ScriptedAI::InitializeAI();

        if (!me->IsAlive())
            return;

        me->SetFullHealth();

        me->m_Events.Schedule(1, [this]()
        {
            if (YmirjarMovementInfo const* info = GetMovementInfo())
                me->NearTeleportTo(info->StartPosition.GetPositionX(), info->StartPosition.GetPositionY(), info->StartPosition.GetPositionZ(), info->StartPosition.GetOrientation());

            if (me->GetEntry() == NPC_YMIRJAR_HUNTRESS)
                me->m_Events.Schedule(1000, [this]() { DoCastAOE(SPELL_SUMMON_WARHAWK, true); });
        });
    }

    void JustAppeared() override
    {
        ScriptedAI::JustAppeared();
        if (me->GetEntry() == NPC_YMIRJAR_HUNTRESS)
            me->m_Events.Schedule(1000, [this]() { DoCastAOE(SPELL_SUMMON_WARHAWK, true); });
    }

    void JustSummoned(Creature* summon) override
    {
        if (summon->GetEntry() == NPC_WARHAWK)
        {
            summon->m_Events.Schedule(1000, [this, summon]()
            {
                if (!summon->IsInCombat())
                    summon->GetMotionMaster()->MoveFollow(me, 3.0f, me->GetPositionX() > 4357.0f ? 2 * M_PI / 6 : -2 * M_PI / 6);
            });
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        me->m_Events.Schedule(1000, [this, pointId]()
        {
            switch (pointId)
            {
                case POINT_DOWNSTAIRS:
                    me->GetMotionMaster()->MovePoint(POINT_FINAL, me->GetHomePosition());
                    break;
                case POINT_FINAL:
                    me->SetFacingTo(me->GetHomePosition().GetOrientation());
                    break;
                default:
                    break;
            }
        });
    }

    void DoAction(int32 action) override
    {
        if (action != ACTION_APPROACH || approached || !me->IsAlive() || me->IsInCombat())
            return;

        approached = true;
        if (YmirjarMovementInfo const* info = GetMovementInfo())
        {
            if (me->GetPositionY() > 2568.5f)
                return;

            me->SetWalk(false);

            if (info->Delay)
                me->m_Events.Schedule(info->Delay, [this]() { StartMovement(); });
            else
                StartMovement();
        }
    }

    bool CanAIAttack(Unit const* target) const override
    {
        // do not see targets inside Frostwing Halls when we are not there
        return (me->GetPositionY() > 2660.0f) == (target->GetPositionY() > 2660.0f) && ScriptedAI::CanAIAttack(target);
    }

protected:
    YmirjarMovementInfo const* GetMovementInfo() const
    {
        for (auto&& info : ymirjarMovementInfo)
            if (info.GuidLow == me->GetDBTableGUIDLow())
                return &info;

        return nullptr;
    }

    void StartMovement()
    {
        if (!me->IsAlive() || me->IsInCombat())
            return;

        YmirjarMovementInfo const* info = GetMovementInfo();
        if (!info)
            return;

        Position home = me->GetHomePosition();
        Position pos;
        pos.Relocate(info->StartPosition);
        float dist = home.GetPositionY() - pos.GetPositionY();
        float stairsDist = 2568.5f - pos.GetPositionY();
        float percent = stairsDist / dist;
        float angle = pos.GetAngle(&home);
        pos.m_positionX += (home.GetPositionX() - pos.GetPositionX()) * percent;
        pos.m_positionY += (home.GetPositionY() - pos.GetPositionY()) * percent;
        pos.m_positionZ = 351.15f;
        me->GetMotionMaster()->MovePoint(POINT_DOWNSTAIRS, pos);
    }

private:
    bool approached = false;
};

//#########  Ymirjar Battle-Maiden ##############
class npc_ymirjar_battlemaiden_icc : public CreatureScript
{
    public:
        npc_ymirjar_battlemaiden_icc() : CreatureScript("npc_ymirjar_battlemaiden_icc") { }

        struct npc_ymirjar_battlemaiden_iccAI : public npc_ymirjar_icc_base
        {
            npc_ymirjar_battlemaiden_iccAI(Creature* creature) : npc_ymirjar_icc_base(creature) { }

            uint32 m_uiRUSH_Timer;
            uint32 m_uiSTRIKE_Timer;

            void Reset() override
            {
                m_uiRUSH_Timer = urand(10000, 15000);
                m_uiSTRIKE_Timer = urand(1000, 5000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiSTRIKE_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_BARBARIC_STRIKE);
                    m_uiSTRIKE_Timer = urand(2000, 3000);
                }
                else
                    m_uiSTRIKE_Timer -= diff;

                if (m_uiRUSH_Timer <= diff)
                {
                    DoCast(me, SPELL_ADRENALINE_RUSH);
                    m_uiRUSH_Timer = urand(15000, 25000);
                }
                else
                    m_uiRUSH_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_ymirjar_battlemaiden_iccAI>(creature);
        }
};

//#########  Ymirjar Huntress ##############
class npc_ymirjar_huntress_icc : public CreatureScript
{
    public:
        npc_ymirjar_huntress_icc() : CreatureScript("npc_ymirjar_huntress_icc") { }

        struct npc_ymirjar_huntress_iccAI : public npc_ymirjar_icc_base
        {
            npc_ymirjar_huntress_iccAI(Creature* creature) : npc_ymirjar_icc_base(creature) { }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_ICE_TRAP, urand(5000, 15000));
                events.ScheduleEvent(EVENT_RAPID_SHOT, urand(10000, 15000));
                events.ScheduleEvent(EVENT_VOLLEY, urand(5000, 15000));
                autoCastTimer.SetInterval(2500);
            }

            void AttackStart(Unit* victim) override
            {
                if (victim && me->Attack(victim, true))
                    casterMovement.Chase(victim);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                autoCastTimer.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_ICE_TRAP:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(target, SPELL_ICE_TRAP);
                        events.ScheduleEvent(EVENT_ICE_TRAP, urand(30000, 35000));
                        break;
                    case EVENT_RAPID_SHOT:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, -10.0f))
                            DoCastAOE(SPELL_RAPID_SHOT);
                        events.ScheduleEvent(EVENT_RAPID_SHOT, urand(15000, 25000));
                        break;
                    case EVENT_VOLLEY:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(target, SPELL_VOLLEY);
                        events.ScheduleEvent(EVENT_VOLLEY, urand(10000, 15000));
                        break;
                    default:
                        if (autoCastTimer.Passed() && me->GetVictim() && !me->IsWithinMeleeRange(me->GetVictim()))
                        {
                            DoCastVictim(SPELL_SHOOT);
                            autoCastTimer.Reset();
                        }
                        break;
                }

                if (me->GetVictim() && me->IsWithinMeleeRange(me->GetVictim()))
                    DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_SHOOT);
            IntervalTimer autoCastTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_ymirjar_huntress_iccAI>(creature);
        }
};

//#########  Ymirjar Warlord ##############
class npc_ymirjar_warlord_icc : public CreatureScript
{
    public:
        npc_ymirjar_warlord_icc() : CreatureScript("npc_ymirjar_warlord_icc") { }

        struct npc_ymirjar_warlord_iccAI : public npc_ymirjar_icc_base
        {
            npc_ymirjar_warlord_iccAI(Creature* creature) : npc_ymirjar_icc_base(creature) { }

            uint32 m_uiWHIRLWIND_Timer;

            void Reset() override
            {
                m_uiWHIRLWIND_Timer = urand(5000, 15000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_uiWHIRLWIND_Timer <= diff)
                {
                    DoCastAOE(SPELL_WHIRLWIND);
                    m_uiWHIRLWIND_Timer = urand(12000, 20000);
                }
                else
                    m_uiWHIRLWIND_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_ymirjar_warlord_iccAI>(creature);
        }
};

class spell_deathspeaker_divine_reckoning : public SpellScriptLoader
{
public:
    spell_deathspeaker_divine_reckoning() : SpellScriptLoader("spell_deathspeaker_divine_reckoning") { }

    class spell_deathspeaker_divine_reckoning_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_deathspeaker_divine_reckoning_AuraScript);

        void OnPeriodic(AuraEffect const* aurEff)
        {
            if (Unit* caster = GetCaster())
            {
                CustomSpellValues values;
                values.AddSpellMod(SPELLVALUE_BASE_POINT0, aurEff->GetAmount());
                caster->CastCustomSpell(GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, values, GetTarget());
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_deathspeaker_divine_reckoning_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_deathspeaker_divine_reckoning_AuraScript();
    }
};

class at_nerubar_trigger : public AreaTriggerScript
{
    public:
        at_nerubar_trigger(bool first, char const* name) : AreaTriggerScript(name), _first(first) { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            std::list<Creature*> nerubar;
            GetCreatureListWithEntryInGrid(nerubar, player, NPC_NERUBAR_BROODKEEPER, 200.0f);
            for (auto&& creature : nerubar)
                if (!creature->AI()->GetData(DATA_DESCENDED) && (creature->GetPositionX() > -235.0f) == _first)
                    creature->AI()->DoAction(ACTION_DESCEND);

            if (!_first)
                return true;

            std::list<Creature*> skeletons;
            GetCreatureListWithEntryInGrid(skeletons, player, NPC_SERVANT_OF_THE_THRONE, 200.0f);
            GetCreatureListWithEntryInGrid(skeletons, player, NPC_ANCIENT_SKELETAL_SOLDIER, 200.0f);
            for (auto&& creature : skeletons)
                if (!creature->AI()->GetData(DATA_DESCENDED) && creature->GetPositionX() > -265.0f && creature->GetPositionX() < -195.0f)
                    creature->AI()->DoAction(ACTION_DESCEND);

            return true;
        }

    private:
        bool _first;
};

class at_icc_start_the_damned_assault : public AreaTriggerScript
{
    public:
        at_icc_start_the_damned_assault() : AreaTriggerScript("at_icc_start_the_damned_assault") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            std::list<Creature*> damned;
            GetCreatureListWithEntryInGrid(damned, player, NPC_THE_DAMNED, 300.0f);
            for (auto&& creature : damned)
                creature->AI()->DoAction(ACTION_ASSAULT);

            return true;
        }
};

void AddSC_icecrown_citadel_trashmobs()
{
    new npc_ancient_skeletal_soldier_icc();
    new npc_deathbound_ward_icc();
    new npc_nerubar_broodkeeper_icc();
    new spell_web_wrap_root();
    new npc_servant_of_the_throne_icc();
    new spell_glacial_blast();
    new npc_the_damned_icc();
    new npc_deathspeaker_servant_10man_icc();
    new npc_deathspeaker_disciple_10man_icc();
    new npc_deathspeaker_attendant_10man_icc();
    new npc_deathspeaker_zealot_icc();
    new npc_deathspeaker_high_priest_icc();
    new npc_valkyr_herald_10man_icc();
    new npc_blighted_abomination_icc();
    new npc_vengeful_fleshreapert_icc();
    //new npc_plague_scientist_icc();
    new npc_pustulating_horror_10man_icc();
    new npc_decaying_colossus_10man_icc();
    new npc_darkfallen_archmage_10man_icc();
    new npc_darkfallen_blood_knight_icc();
    new npc_darkfallen_noble_10man_icc();
    new npc_vampiric_fiend_icc();
    new npc_darkfallen_advisor_10man_icc();
    new npc_darkfallen_commander_10man_icc();
    new npc_darkfallen_lieutenant_10man_icc();
    new npc_darkfallen_tactician_icc();
    new npc_ymirjar_deathbringer_10man_icc();
    new npc_ymirjar_frostbinder_icc();
    new npc_ymirjar_battlemaiden_icc();
    new npc_ymirjar_huntress_icc();
    new npc_ymirjar_warlord_icc();
    //new spell_deathspeaker_divine_reckoning();
    new at_nerubar_trigger(true, "at_nerubar_trigger_first");
    new at_nerubar_trigger(false, "at_nerubar_trigger_second");
    new at_icc_start_the_damned_assault();
}
