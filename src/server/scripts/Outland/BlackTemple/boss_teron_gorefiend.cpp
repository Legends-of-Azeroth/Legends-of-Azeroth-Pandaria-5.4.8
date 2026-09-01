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

/* ScriptData
SDName: Boss_Teron_Gorefiend
SD%Complete: 60
SDComment: Requires Mind Control support for Ghosts.
SDCategory: Black Temple
EndScriptData */

#include "ScriptPCH.h"
#include "black_temple.h"

enum DoomBlossom
{
    // Speech'n'sound
    SAY_INTRO                       = 0,
    SAY_AGGRO                       = 1,
    SAY_SLAY                        = 2,
    SAY_SPELL                       = 3,
    SAY_SPECIAL                     = 4,
    SAY_ENRAGE                      = 5,
    SAY_DEATH                       = 6,

    // Spells
    SPELL_INCINERATE                = 40239,
    SPELL_CRUSHING_SHADOWS          = 40243,
    SPELL_SHADOWBOLT                = 40185,
    SPELL_PASSIVE_SHADOWFORM        = 40326,
    SPELL_SHADOW_OF_DEATH           = 40251,
    SPELL_BERSERK                   = 45078,
    SPELL_ATROPHY                   = 40327, // Shadowy Constructs use this when they get within melee range of a player

    CREATURE_DOOM_BLOSSOM           = 23123,
    CREATURE_SHADOWY_CONSTRUCT      = 23111
};

class npc_doom_blossom : public CreatureScript
{
    public:
        npc_doom_blossom() : CreatureScript("npc_doom_blossom") { }

        struct npc_doom_blossomAI : public ScriptedAI
        {
            npc_doom_blossomAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 CheckTeronTimer;
            uint32 ShadowBoltTimer;
            ObjectGuid TeronGUID;

            void Reset() override
            {
                CheckTeronTimer = 5000;
                ShadowBoltTimer = 12000;
                TeronGUID = ObjectGuid::Empty;
            }

            void JustEngagedWith(Unit* /*who*/) override { }
            void AttackStart(Unit* /*who*/) override { }
            void MoveInLineOfSight(Unit* /*who*/) override { }

            void Despawn()
            {
                me->DealDamage(me, me->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
                me->RemoveCorpse();
            }

            void UpdateAI(uint32 diff) override
            {
                if (CheckTeronTimer <= diff)
                {
                    if (TeronGUID)
                    {
                        DoZoneInCombat();

                        Creature* teron = (Unit::GetCreature(*me, TeronGUID));
                        if (teron && (!teron->IsAlive() || teron->IsInEvadeMode()))
                            Despawn();
                    }
                    else
                        Despawn();

                    CheckTeronTimer = 5000;
                } else CheckTeronTimer -= diff;

                if (ShadowBoltTimer < diff && me->IsInCombat())
                {
                    DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0), SPELL_SHADOWBOLT);
                    ShadowBoltTimer = 10000;
                } else ShadowBoltTimer -= diff;
                return;
            }

            void SetTeronGUID(ObjectGuid guid)
            {
                TeronGUID = guid;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_doom_blossomAI(creature);
        }
};

class npc_shadowy_construct : public CreatureScript
{
    public:
        npc_shadowy_construct() : CreatureScript("npc_shadowy_construct") { }

        struct npc_shadowy_constructAI : public ScriptedAI
        {
            npc_shadowy_constructAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid GhostGUID;
            ObjectGuid TeronGUID;
            uint32 CheckPlayerTimer;
            uint32 CheckTeronTimer;

            void Reset() override
            {
                GhostGUID = ObjectGuid::Empty;
                TeronGUID = ObjectGuid::Empty;

                CheckPlayerTimer = 2000;
                CheckTeronTimer = 5000;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!who || (!who->IsAlive()) || (who->GetGUID() == GhostGUID))
                    return;

                ScriptedAI::MoveInLineOfSight(who);
            }

            /* Comment it out for now. NOTE TO FUTURE DEV: UNCOMMENT THIS OUT ONLY AFTER MIND CONTROL IS IMPLEMENTED
            void DamageTaken(Unit* done_by, uint32& damage) override
            {
                if (done_by->GetGUID() != GhostGUID)
                    damage = 0; // Only the ghost can deal damage.
            }*/

            void CheckPlayers()
            {
                ThreatContainer::StorageType const& threatlist = me->GetThreatManager().getThreatList();
                if (threatlist.empty())
                    return;                                         // No threat list. Don't continue.
                ThreatContainer::StorageType::const_iterator itr = threatlist.begin();
                std::list<Unit*> targets;
                for (; itr != threatlist.end(); ++itr)
                {
                    Unit* unit = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                    if (unit && unit->IsAlive())
                        targets.push_back(unit);
                }
                targets.sort(Trinity::ObjectDistanceOrderPred(me));
                Unit* target = targets.front();
                if (target && me->IsWithinDistInMap(target, me->GetAttackDistance(target)))
                {
                    DoCast(target, SPELL_ATROPHY);
                    me->AI()->AttackStart(target);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (CheckPlayerTimer <= diff)
                {
                    CheckPlayers();
                    CheckPlayerTimer = 3000;
                } else CheckPlayerTimer -= diff;

                if (CheckTeronTimer <= diff)
                {
                    Creature* Teron = (Unit::GetCreature(*me, TeronGUID));
                    if (!Teron || !Teron->IsAlive() || Teron->IsInEvadeMode())
                        me->DealDamage(me, me->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);

                    CheckTeronTimer = 5000;
                } else CheckTeronTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shadowy_constructAI(creature);
        }
};

class boss_teron_gorefiend : public CreatureScript
{
    public:
        boss_teron_gorefiend() : CreatureScript("boss_teron_gorefiend") { }

        struct boss_teron_gorefiendAI : public ScriptedAI
        {
            boss_teron_gorefiendAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint32 IncinerateTimer;
            uint32 SummonDoomBlossomTimer;
            uint32 EnrageTimer;
            uint32 CrushingShadowsTimer;
            uint32 ShadowOfDeathTimer;
            uint32 SummonShadowsTimer;
            uint32 RandomYellTimer;
            uint32 AggroTimer;
            ObjectGuid AggroTargetGUID;
            ObjectGuid GhostGUID; // Player that gets killed by Shadow of Death and gets turned into a ghost
            bool Intro;
            bool Done;

            void Reset() override
            {
                if (instance)
                    instance->SetData(DATA_TERON_GOREFIEND_EVENT, NOT_STARTED);

                IncinerateTimer = 20000 + rand()%11000;
                SummonDoomBlossomTimer = 12000;
                EnrageTimer = 600000;
                CrushingShadowsTimer = 22000;
                SummonShadowsTimer = 60000;
                RandomYellTimer = 50000;

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                // Start off unattackable so that the intro is done properly
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                AggroTimer = 20000;
                AggroTargetGUID = ObjectGuid::Empty;
                Intro = false;
                Done = false;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!Intro && who->GetTypeId() == TYPEID_PLAYER && who->isTargetableForAttack() && me->IsHostileTo(who) && who->isInAccessiblePlaceFor(me))
                {
                    if (me->IsWithinDistInMap(who, VISIBLE_RANGE) && me->IsWithinLOSInMap(who))
                    {
                        if (instance && instance->CheckRequiredBosses(DATA_TERON_GOREFIEND_EVENT, who->ToPlayer()))
                            instance->SetData(DATA_TERON_GOREFIEND_EVENT, IN_PROGRESS);
                        else
                            return;

                        me->GetMotionMaster()->Clear(false);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        Talk(SAY_INTRO);
                        me->HandleEmoteStateCommand(EMOTE_STATE_TALK);
                        AggroTargetGUID = who->GetGUID();
                        Intro = true;
                    }
                }
                if (Done)
                    ScriptedAI::MoveInLineOfSight(who);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                    instance->SetData(DATA_TERON_GOREFIEND_EVENT, DONE);

                Talk(SAY_DEATH);
            }

            float CalculateRandomLocation(float Loc, uint32 radius)
            {
                float coord = Loc;
                switch (urand(0,1))
                {
                    case 0:
                        coord += rand()%radius;
                        break;
                    case 1:
                        coord -= rand()%radius;
                        break;
                }
                return coord;
            }

            void SetThreatList(Creature* blossom)
            {
                if (!blossom) return;

                ThreatContainer::StorageType const& threatlist = me->GetThreatManager().getThreatList();
                ThreatContainer::StorageType::const_iterator i = threatlist.begin();
                for (i = threatlist.begin(); i != threatlist.end(); ++i)
                {
                    Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());
                    if (unit && unit->IsAlive())
                    {
                        float threat = DoGetThreat(unit);
                        blossom->AddThreat(unit, threat);
                    }
                }
            }

            void MindControlGhost()
            {
                /************************************************************************/
                /** NOTE FOR FUTURE DEVELOPER: PROPERLY IMPLEMENT THE GHOST PORTION *****/
                /**  ONLY AFTER TrinIty FULLY IMPLEMENTS MIND CONTROL ABILITIES      *****/
                /**   THE CURRENT CODE IN THIS FUNCTION IS ONLY THE BEGINNING OF    *****/
                /**    WHAT IS FULLY NECESSARY FOR GOREFIEND TO BE 100% COMPLETE    *****/
                /************************************************************************/

                Unit* Ghost = nullptr;
                if (GhostGUID)
                    Ghost = Unit::GetUnit(*me, GhostGUID);
                if (Ghost && Ghost->IsAlive() && Ghost->HasAura(SPELL_SHADOW_OF_DEATH))
                {
                    /*float x,y,z;
                    Ghost->GetPosition(x,y,z);
                    Creature* control = me->SummonCreature(CREATURE_GHOST, x, y, z, 0, TEMPSUMMON_TIMED_DESAWN, 30000ms);
                    if (control)
                    {
                        CAST_PLR(Ghost)->Possess(control);
                        Ghost->DealDamage(Ghost, Ghost->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr,
                    false);
                    }*/
                    for (uint8 i = 0; i < 4; ++i)
                    {
                        Creature* Construct = nullptr;
                        float X = CalculateRandomLocation(Ghost->GetPositionX(), 10);
                        float Y = CalculateRandomLocation(Ghost->GetPositionY(), 10);
                        Construct = me->SummonCreature(CREATURE_SHADOWY_CONSTRUCT, X, Y, Ghost->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 45000ms);
                        if (Construct)
                        {
                            Construct->CastSpell(Construct, SPELL_PASSIVE_SHADOWFORM, true);
                            SetThreatList(Construct);               // Use same function as Doom Blossom to set Threat List.
                            CAST_AI(npc_shadowy_construct::npc_shadowy_constructAI, Construct->AI())->GhostGUID = GhostGUID;
                            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1);
                            if (!target)                             // someone's trying to solo.
                                target = me->GetVictim();

                            if (target)
                                Construct->GetMotionMaster()->MoveChase(target);
                        }
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (Intro && !Done)
                {
                    if (AggroTimer <= diff)
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        Talk(SAY_AGGRO);
                        me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
                        Done = true;
                        if (AggroTargetGUID)
                        {
                            Unit* unit = Unit::GetUnit(*me, AggroTargetGUID);
                            if (unit)
                                AttackStart(unit);

                            DoZoneInCombat();
                        }
                        else
                        {
                            EnterEvadeMode();
                            return;
                        }
                    } else AggroTimer -= diff;
                }

                if (!UpdateVictim() || !Done)
                    return;

                if (SummonShadowsTimer <= diff)
                {
                    //MindControlGhost();

                    for (uint8 i = 0; i < 2; ++i)
                    {
                        Creature* shadow = nullptr;
                        float X = CalculateRandomLocation(me->GetPositionX(), 10);
                        shadow = me->SummonCreature(CREATURE_SHADOWY_CONSTRUCT, X, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 0ms);
                        if (shadow)
                        {
                            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1);
                            if (!target)
                                target = me->GetVictim();

                            if (target)
                                shadow->AI()->AttackStart(target);
                        }
                    }
                    SummonShadowsTimer = 60000;
                } else SummonShadowsTimer -= diff;

                if (SummonDoomBlossomTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    {
                        float X = CalculateRandomLocation(target->GetPositionX(), 20);
                        float Y = CalculateRandomLocation(target->GetPositionY(), 20);
                        float Z = target->GetPositionZ();
                        Z = me->GetMap()->GetHeight(X, Y, Z);
                        Creature* doomBlossom = me->SummonCreature(CREATURE_DOOM_BLOSSOM, X, Y, Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000ms);
                        if (doomBlossom)
                        {
                            doomBlossom->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            doomBlossom->SetFaction(me->GetFaction());
                            doomBlossom->AddThreat(target, 1.0f);
                            CAST_AI(npc_doom_blossom::npc_doom_blossomAI, doomBlossom->AI())->SetTeronGUID(me->GetGUID());
                            target->CombatStart(doomBlossom);
                            SetThreatList(doomBlossom);
                            SummonDoomBlossomTimer = 35000;
                        }
                    }
                } else SummonDoomBlossomTimer -= diff;

                if (IncinerateTimer <= diff)
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1);
                    if (!target)
                        target = me->GetVictim();

                    if (target)
                    {
                        Talk(SAY_SPECIAL);
                        DoCast(target, SPELL_INCINERATE);
                        IncinerateTimer = 20000 + rand()%31 * 1000;
                    }
                } else IncinerateTimer -= diff;

                if (CrushingShadowsTimer <= diff)
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                    if (target && target->IsAlive())
                        DoCast(target, SPELL_CRUSHING_SHADOWS);
                    CrushingShadowsTimer = 10000 + rand()%16 * 1000;
                } else CrushingShadowsTimer -= diff;

                /*** NOTE FOR FUTURE DEV: UNCOMMENT BELOW ONLY IF MIND CONTROL IS FULLY IMPLEMENTED **/
                /*if (ShadowOfDeathTimer <= diff)
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1);

                    if (!target)
                       target = me->GetVictim();

                    if (target && target->IsAlive() && target->GetTypeId() == TYPEID_PLAYER)
                    {
                        DoCast(target, SPELL_SHADOW_OF_DEATH);
                        GhostGUID = target->GetGUID();
                        ShadowOfDeathTimer = 30000;
                        SummonShadowsTimer = 53000; // Make it VERY close but slightly less so that we can check if the aura is still on the player
                    }
                } else ShadowOfDeathTimer -= diff;*/

                if (RandomYellTimer <= diff)
                {
                    Talk(SAY_SPELL);
                    RandomYellTimer = 50000 + rand()%51 * 1000;
                } else RandomYellTimer -= diff;

                if (!me->HasAura(SPELL_BERSERK))
                {
                    if (EnrageTimer <= diff)
                {
                    DoCast(me, SPELL_BERSERK);
                    Talk(SAY_ENRAGE);
                } else EnrageTimer -= diff;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_teron_gorefiendAI(creature);
        }
};

void AddSC_boss_teron_gorefiend()
{
    new npc_doom_blossom();
    new npc_shadowy_construct();
    new boss_teron_gorefiend();
}
