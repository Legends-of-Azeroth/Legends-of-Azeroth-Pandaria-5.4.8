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
SDName: Boss_Muru
SD%Complete: 80
SDComment: all sounds, black hole effect triggers to often (46228) */

#include "ScriptPCH.h"
#include "sunwell_plateau.h"

// Muru & Entropius's spells
enum Spells
{
    SPELL_ENRAGE                = 26662,

    // Muru's spells
    SPELL_NEGATIVE_ENERGY       = 46009, //(this trigger 46008)
    SPELL_DARKNESS              = 45999,
    SPELL_OPEN_ALL_PORTALS      = 46177,
    SPELL_OPEN_PORTAL           = 45977,
    SPELL_OPEN_PORTAL_2         = 45976,
    SPELL_SUMMON_BERSERKER      = 46037,
    SPELL_SUMNON_FURY_MAGE      = 46038,
    SPELL_SUMMON_VOID_SENTINEL  = 45988,
    SPELL_SUMMON_ENTROPIUS      = 46217,

    // Entropius's spells
    SPELL_DARKNESS_E            = 46269,
    SPELL_BLACKHOLE             = 46282,
    SPELL_NEGATIVE_ENERGY_E     = 46284,
    SPELL_ENTROPIUS_SPAWN       = 46223,

    // Shadowsword Berserker's spells
    SPELL_FLURRY                = 46160,
    SPELL_DUAL_WIELD            = 29651,

    // Shadowsword Fury Mage's spells
    SPELL_FEL_FIREBALL          = 46101,
    SPELL_SPELL_FURY            = 46102,

    // Void Sentinel's spells
    SPELL_SHADOW_PULSE          = 46087,
    SPELL_VOID_BLAST            = 46161,

    // Void Spawn's spells
    SPELL_SHADOW_BOLT_VOLLEY    = 46082,

    // Dark Fiend Spells
    SPELL_DARKFIEND_AOE         = 45944,
    SPELL_DARKFIEND_VISUAL      = 45936,
    SPELL_DARKFIEND_SKIN        = 45934,

    // Black Hole Spells
    SPELL_BLACKHOLE_SPAWN       = 46242,
    SPELL_BLACKHOLE_GROW        = 46228
};

enum BossTimers
{
    TIMER_DARKNESS              = 0,
    TIMER_HUMANOIDES            = 1,
    TIMER_PHASE                 = 2,
    TIMER_SENTINEL              = 3
};

float DarkFiends[8][4] =
{
    { 1819.9f,    609.80f,    69.74f,    1.94f },
    { 1829.39f,   617.89f,    69.73f,    2.61f },
    { 1801.98f,   633.62f,    69.74f,    5.71f },
    { 1830.88f,   629.99f,    69.73f,    3.52f },
    { 1800.38f,   621.41f,    69.74f,    0.22f },
    { 1808.3f ,   612.45f,    69.73f,    1.02f },
    { 1823.9f ,   639.69f,    69.74f,    4.12f },
    { 1811.85f,   640.46f,    69.73f,    4.97f }
};

float Humanoides[6][5] =
{
    { NPC_FURY_MAGE, 1780.16f,    666.83f,    71.19f,    5.21f },
    { NPC_FURY_MAGE, 1847.93f,    600.30f,    71.30f,    2.57f },
    { NPC_BERSERKER, 1779.97f,    660.64f,    71.19f,    5.28f },
    { NPC_BERSERKER, 1786.2f ,    661.01f,    71.19f,    4.51f },
    { NPC_BERSERKER, 1845.17f,    602.63f,    71.28f,    2.43f },
    { NPC_BERSERKER, 1842.91f,    599.93f,    71.23f,    2.44f }
};

uint32 enrageTimer = 600000;

class boss_entropius : public CreatureScript
{
    public:
        boss_entropius() : CreatureScript("boss_entropius") { }

        struct boss_entropiusAI : public ScriptedAI
        {
            boss_entropiusAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            SummonList summons;
            uint32 blackHoleSummonTimer;

            void Reset() override
            {
                blackHoleSummonTimer = 15000;
                DoCastAOE(SPELL_NEGATIVE_ENERGY_E, false);

                summons.DespawnAll();

                if (instance)
                    instance->SetData(DATA_MURU_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCastAOE(SPELL_NEGATIVE_ENERGY_E, true);
                DoCast(me, SPELL_ENTROPIUS_SPAWN, false);

                if (instance)
                    instance->SetData(DATA_MURU_EVENT, IN_PROGRESS);
            }

            void JustSummoned(Creature* summon) override
            {
                switch (summon->GetEntry())
                {
                    case NPC_DARK_FIENDS:
                        summon->CastSpell(summon,SPELL_DARKFIEND_VISUAL,false);
                        break;
                    case NPC_DARKNESS:
                        summon->AddUnitState(UNIT_STATE_STUNNED);
                        float x, y, z, o;
                        summon->GetHomePosition(x, y, z, o);
                        me->SummonCreature(NPC_DARK_FIENDS, x, y, z, o, TEMPSUMMON_CORPSE_DESPAWN, 0ms);
                        break;
                }
                summon->AI()->AttackStart(SelectTarget(SELECT_TARGET_RANDOM,0, 50, true));
                summons.Summon(summon);
            }

            void JustDied(Unit* /*killer*/) override
            {
                summons.DespawnAll();

                if (instance)
                    instance->SetData(DATA_MURU_EVENT, DONE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (enrageTimer < diff && !me->HasAura(SPELL_ENRAGE, 0))
                {
                    DoCast(me, SPELL_ENRAGE, false);
                } else enrageTimer -= diff;

                if (blackHoleSummonTimer <= diff)
                {
                    Unit* random = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
                    if (!random)
                        return;

                    DoCast(random, SPELL_DARKNESS_E, false);

                    random = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
                    if (!random)
                        return;

                    random->CastSpell(random, SPELL_BLACKHOLE, false);
                    blackHoleSummonTimer = 15000;
                } else blackHoleSummonTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_entropiusAI(creature);
        }
};

class boss_muru : public CreatureScript
{
    public:
        boss_muru() : CreatureScript("boss_muru") { }

        struct boss_muruAI : public ScriptedAI
        {
            boss_muruAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                instance = creature->GetInstanceScript();
                SetCombatMovement(false);
            }

            InstanceScript* instance;
            SummonList summons;
            uint32 timer[4];
            uint8 phase;
            bool DarkFiend;

            void Reset() override
            {
                DarkFiend = false;
                phase = 1;

                enrageTimer = 600000;
                timer[TIMER_DARKNESS] = 45000;
                timer[TIMER_HUMANOIDES] = 10000;
                timer[TIMER_PHASE] = 2000;
                timer[TIMER_SENTINEL] = 31500;

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetVisible(true);

                summons.DespawnAll();

                if (instance)
                    instance->SetData(DATA_MURU_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance->GetData(DATA_KALECGOS_EVENT) != DONE || instance->GetData(DATA_BRUTALLUS_EVENT) != DONE || instance->GetData(DATA_FELMYST_EVENT) != DONE)
                {
                    instance->DoCastSpellOnPlayers(SPELL_ELEPORT_TO_APEX_POINT);
                    EnterEvadeMode();
                    return;
                }
                if (instance->GetData(DATA_EREDAR_TWINS_EVENT) != DONE)
                {
                    instance->DoCastSpellOnPlayers(SPELL_TELEPORT_TO_WITCHS_SANCTUM);
                    EnterEvadeMode();
                    return;
                }

                DoCastAOE(SPELL_NEGATIVE_ENERGY,false);

                if (instance)
                    instance->SetData(DATA_MURU_EVENT, IN_PROGRESS);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage > me->GetHealth() && phase == 1)
                {
                    damage = 0;
                    phase = 2;
                    me->RemoveAllAuras();
                    DoCast(me, SPELL_OPEN_ALL_PORTALS, false);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
                if (phase > 1 && phase < 4)
                    damage = 0;
            }

            void JustSummoned(Creature* summon) override
            {
                switch (summon->GetEntry())
                {
                    case BOSS_ENTROPIUS:
                        me->SetVisible(false);
                        break;
                    case NPC_DARK_FIENDS:
                        summon->CastSpell(summon,SPELL_DARKFIEND_VISUAL,false);
                        break;
                }
                summon->AI()->AttackStart(SelectTarget(SELECT_TARGET_RANDOM,0, 50, true));
                summons.Summon(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (phase == 3)
                {
                    if (timer[TIMER_PHASE] <= diff)
                    {
                        if (!instance)
                            return;
                        switch (instance->GetData(DATA_MURU_EVENT))
                        {
                            case NOT_STARTED:
                                Reset();
                                break;
                            case DONE:
                                phase = 4;
                                me->DisappearAndDie();
                                break;
                        }
                        timer[TIMER_PHASE] = 3000;
                    } else timer[TIMER_PHASE] -= diff;
                    return;
                }

                if (enrageTimer < diff && !me->HasAura(SPELL_ENRAGE, 0))
                {
                    DoCast(me, SPELL_ENRAGE, false);
                } else enrageTimer -= diff;

                for (uint8 i = 0; i < 4; ++i)
                {
                    if (timer[i] <= diff)
                    {
                        switch (i)
                        {
                            case TIMER_DARKNESS:
                                if (!DarkFiend)
                                {
                                    DoCastAOE(SPELL_DARKNESS, false);
                                    timer[TIMER_DARKNESS] = 3000;
                                    DarkFiend = true;
                                }
                                else
                                {
                                    DarkFiend = false;
                                    for (uint8 j = 0; j < 8; ++j)
                                        me->SummonCreature(NPC_DARK_FIENDS, DarkFiends[j][0], DarkFiends[j][1], DarkFiends[j][2], DarkFiends[j][3], TEMPSUMMON_CORPSE_DESPAWN, 0ms);
                                    timer[TIMER_DARKNESS] = 42000;
                                }
                                break;
                            case TIMER_HUMANOIDES:
                                for (uint8 j = 0; j < 6; ++j)
                                    me->SummonCreature(uint32(Humanoides[j][0]), Humanoides[j][1], Humanoides[j][2], Humanoides[j][3], Humanoides[j][4], TEMPSUMMON_CORPSE_DESPAWN, 0ms);
                                timer[TIMER_HUMANOIDES] = 60000;
                                break;
                            case TIMER_PHASE:
                                me->RemoveAllAuras();
                                DoCast(me, SPELL_SUMMON_ENTROPIUS, false);
                                timer[TIMER_PHASE] = 3000;
                                phase = 3;
                                return;
                            case TIMER_SENTINEL:
                                DoCastAOE(SPELL_OPEN_PORTAL_2, false);
                                timer[TIMER_SENTINEL] = 30000;
                                break;
                        }
                        break;
                    }
                }

                // Timer
                for (uint8 i = 0; i < 4; ++i)
                {
                    if (i != TIMER_PHASE)timer[i] -= diff;
                    else if (phase == 2) timer[i] -= diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_muruAI(creature);
        }
};

class npc_muru_portal : public CreatureScript
{
    public:
        npc_muru_portal() : CreatureScript("npc_muru_portal") { }

        struct npc_muru_portalAI : public ScriptedAI
        {
            npc_muru_portalAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                instance = creature->GetInstanceScript();
                SetCombatMovement(false);
            }

            InstanceScript* instance;
            SummonList summons;
            uint32 summonTimer;
            bool summonSentinel;
            bool inAction;

            void Reset() override
            {
                summonTimer = 5000;

                inAction = false;
                summonSentinel = false;

                me->AddUnitState(UNIT_STATE_STUNNED);

                summons.DespawnAll();
            }

            void JustSummoned(Creature* summon) override
            {
                if (instance)
                    if (Player* Target = Unit::GetPlayer(*me, instance->GetGuidData(DATA_PLAYER_GUID)))
                        summon->AI()->AttackStart(Target);

                summons.Summon(summon);
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                float x,y,z,o;
                me->GetHomePosition(x,y,z,o);
                DoTeleportTo(x,y,z);
                inAction = true;
                switch (spell->Id)
                {
                    case SPELL_OPEN_ALL_PORTALS:
                        DoCastAOE(SPELL_OPEN_PORTAL, false);
                        break;
                    case SPELL_OPEN_PORTAL_2:
                        DoCastAOE(SPELL_OPEN_PORTAL, false);
                        summonSentinel = true;
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!summonSentinel)
                {
                    if (inAction && instance && instance->GetData(DATA_MURU_EVENT) == NOT_STARTED)
                        Reset();
                    return;
                }
                if (summonTimer <= diff)
                {
                    DoCastAOE(SPELL_SUMMON_VOID_SENTINEL, false);
                    summonTimer = 5000;
                    summonSentinel = false;
                } else summonTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_muru_portalAI(creature);
        }
};

class npc_dark_fiend : public CreatureScript
{
    public:
        npc_dark_fiend() : CreatureScript("npc_dark_fiend") { }

        struct npc_dark_fiendAI : public ScriptedAI
        {
            npc_dark_fiendAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 waitTimer;
            bool inAction;

            void Reset() override
            {
                waitTimer = 2000;
                inAction = false;

                me->AddUnitState(UNIT_STATE_STUNNED);
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                for (uint8 i = 0; i < 3; ++i)
                    if (spell->Effects[i].Effect == 38)
                        me->DisappearAndDie();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (waitTimer <= diff)
                {
                    if (!inAction)
                    {
                        me->ClearUnitState(UNIT_STATE_STUNNED);
                        DoCastAOE(SPELL_DARKFIEND_SKIN, false);
                        AttackStart(SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true));
                        inAction = true;
                        waitTimer = 500;
                    }
                    else
                    {

                        if (me->IsWithinDist(me->GetVictim(), 5))
                        {
                            DoCastAOE(SPELL_DARKFIEND_AOE, false);
                            me->DisappearAndDie();
                        }
                        waitTimer = 500;
                    }
                } else waitTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dark_fiendAI(creature);
        }
};

class npc_void_sentinel : public CreatureScript
{
    public:
        npc_void_sentinel() : CreatureScript("npc_void_sentinel") { }

        struct npc_void_sentinelAI : public ScriptedAI
        {
            npc_void_sentinelAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 pulseTimer;
            uint32 voidBlastTimer;

            void Reset() override
            {
                pulseTimer = 3000;
                voidBlastTimer = 45000; //is this a correct timer?

                float x,y,z,o;
                me->GetHomePosition(x,y,z,o);
                DoTeleportTo(x,y,71);
            }

            void JustDied(Unit* /*killer*/) override
            {
                for (uint8 i = 0; i < 8; ++i)
                    me->SummonCreature(NPC_VOID_SPAWN, me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(), float(rand()%6), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 180000ms);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (pulseTimer <= diff)
                {
                    DoCastAOE(SPELL_SHADOW_PULSE, true);
                    pulseTimer = 3000;
                } else pulseTimer -= diff;

                if (voidBlastTimer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_VOID_BLAST, false);
                    voidBlastTimer = 45000;
                } else voidBlastTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_void_sentinelAI(creature);
        }
};


class npc_blackhole : public CreatureScript
{
    public:
        npc_blackhole() : CreatureScript("npc_blackhole") { }

        struct npc_blackholeAI : public ScriptedAI
        {
            npc_blackholeAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint32 despawnTimer;
            uint32 spellTimer;
            uint8 phase;
            uint8 needForAHack;

            void Reset() override
            {
                despawnTimer = 15000;
                spellTimer = 5000;
                phase = 0;

                me->AddUnitState(UNIT_STATE_STUNNED);
                DoCastAOE(SPELL_BLACKHOLE_SPAWN, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (spellTimer <= diff)
                {
                    Unit* Victim = Unit::GetUnit(*me, instance ? instance->GetGuidData(DATA_PLAYER_GUID) : ObjectGuid::Empty);
                    if (DATA_PLAYER_GUID>0)
                    switch (needForAHack)
                    {
                        case 0:
                            me->ClearUnitState(UNIT_STATE_STUNNED);
                            DoCastAOE(SPELL_BLACKHOLE_GROW, false);
                            if (Victim)
                                AttackStart(Victim);
                            spellTimer = 700;
                            needForAHack = 2;
                            break;
                        case 1:
                            me->AddAura(SPELL_BLACKHOLE_GROW, me);
                            needForAHack = 2;
                            spellTimer = 600;
                            break;
                        case 2:
                            spellTimer = 400;
                            needForAHack = 3;
                            me->RemoveAura(SPELL_BLACKHOLE_GROW, 1);
                            break;
                        case 3:
                            spellTimer = urand(400,900);
                            needForAHack = 1;
                            if (Unit* temp = me->GetVictim())
                            {
                                if (temp->GetPositionZ() > 73 && Victim)
                                    AttackStart(Victim);
                            } 
                            else
                                return;
                    }
                } else spellTimer -= diff;

                if (despawnTimer <= diff)
                    me->DisappearAndDie();
                else despawnTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_blackholeAI(creature);
        }
};

void AddSC_boss_muru()
{
    new boss_muru();
    new boss_entropius();
    new npc_muru_portal();
    new npc_dark_fiend();
    new npc_void_sentinel();
    new npc_blackhole();
}
