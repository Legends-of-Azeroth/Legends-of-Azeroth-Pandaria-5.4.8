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

/* ScriptData
SDName: Urom
SD%Complete: 80
SDComment: Is not working SPELL_ARCANE_SHIELD. SPELL_FROSTBOMB has some issues, the damage aura should not stack.
SDCategory: Instance Script
EndScriptData */

#include "ScriptPCH.h"
#include "oculus.h"
#include "Vehicle.h"

enum Spells
{
    SPELL_ARCANE_SHIELD                           = 53813, // Dummy --> Channeled, shields the caster from damage.
    SPELL_EMPOWERED_ARCANE_EXPLOSION              = 51110,
    SPELL_EMPOWERED_ARCANE_EXPLOSION_2            = 59377,
    SPELL_FROSTBOMB                               = 51103, // Urom throws a bomb, hitting its target with the highest aggro which inflict directly 650 frost damage and drops a frost zone on the ground. This zone deals 650 frost damage per second and reduce the movement speed by 35%. Lasts 1 minute.
    SPELL_SUMMON_MENAGERIE                        = 50476, // Summons an assortment of creatures and teleports the caster to safety.
    SPELL_SUMMON_MENAGERIE_2                      = 50495,
    SPELL_SUMMON_MENAGERIE_3                      = 50496,
    SPELL_TELEPORT                                = 51112, // Teleports to the center of Oculus
    SPELL_TELEPORT_VISUAL                         = 51347,
    SPELL_TIME_BOMB                               = 51121, // Deals arcane damage to a random player, and after 6 seconds, deals zone damage to nearby equal to the health missing of the target afflicted by the debuff.
    SPELL_TIME_BOMB_2                             = 59376,
    SPELL_EVOCATE                                 = 51602  // He always cast it on reset or after teleportation
};

enum Yells
{
    SAY_SUMMON_1                                  = 0,
    SAY_SUMMON_2                                  = 1,
    SAY_SUMMON_3                                  = 2,
    SAY_AGGRO                                     = 3,
    EMOTE_ARCANE_EXPLOSION                        = 4,
    SAY_ARCANE_EXPLOSION                          = 5,
    SAY_DEATH                                     = 6,
    SAY_PLAYER_KILL                               = 7
};

enum Creatures
{
    NPC_PHANTASMAL_CLOUDSCRAPER                   = 27645,
    NPC_PHANTASMAL_MAMMOTH                        = 27642,
    NPC_PHANTASMAL_WOLF                           = 27644,

    NPC_PHANTASMAL_AIR                            = 27650,
    NPC_PHANTASMAL_FIRE                           = 27651,
    NPC_PHANTASMAL_WATER                          = 27653,

    NPC_PHANTASMAL_MURLOC                         = 27649,
    NPC_PHANTASMAL_NAGAL                          = 27648,
    NPC_PHANTASMAL_OGRE                           = 27647
};

struct Summons
{
    uint32 entry[4];
};

static Summons Group[] =
{
    { {NPC_PHANTASMAL_CLOUDSCRAPER, NPC_PHANTASMAL_CLOUDSCRAPER, NPC_PHANTASMAL_MAMMOTH, NPC_PHANTASMAL_WOLF}   },
    { {NPC_PHANTASMAL_AIR,          NPC_PHANTASMAL_AIR,          NPC_PHANTASMAL_WATER,   NPC_PHANTASMAL_FIRE}   },
    { {NPC_PHANTASMAL_OGRE,         NPC_PHANTASMAL_OGRE,         NPC_PHANTASMAL_NAGAL,   NPC_PHANTASMAL_MURLOC} }
};

static uint32 TeleportSpells[] =
{
    SPELL_SUMMON_MENAGERIE, SPELL_SUMMON_MENAGERIE_2, SPELL_SUMMON_MENAGERIE_3
};

class boss_urom : public CreatureScript
{
    public:
        boss_urom() : CreatureScript("boss_urom") { }

        struct boss_uromAI : public BossAI
        {
            boss_uromAI(Creature* creature) : BossAI(creature, DATA_UROM)
            {
                Initialize();
                teleported = false;
                platform = 0;

                for (uint8 i = 0; i < 3; ++i)
                    group[i] = i;
                std::random_device rd;
                std::mt19937 g(rd());
                std::shuffle(group, group + 3, g);
            }

            void Initialize()
            {
                x = 0.0f;
                y = 0.0f;
                canCast = false;
                canGoBack = false;

                arcaneShieldPending = false;

                teleportTimer = urand(30000, 35000);
                arcaneExplosionTimer = 9000;
                castArcaneExplosionTimer = 2000;
                frostBombTimer = urand(5000, 8000);
                timeBombTimer = urand(20000, 25000);
            }

            void InitializeAI() override
            {
                BossAI::InitializeAI();
                if (me->IsAlive())
                    me->CastSpell(me, SPELL_EVOCATE);
            }

            void Reset() override
            {
                summons.clear(); // We don't want them to despawn each time Urom evades
                _Reset();
                Initialize();

                me->GetMotionMaster()->MoveIdle();
                me->SetFlying(false);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                me->InterruptSpell(CURRENT_CHANNELED_SPELL);

                StartAttack();
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();
                me->CastSpell(me, SPELL_EVOCATE);
            }

            void AttackStart(Unit* who) override
            {
                if (!who)
                    return;

                if (me->GetPositionZ() > 518.63f)
                    DoStartNoMovement(who);
                else
                    BossAI::AttackStart(who);
            }

            bool CanAIAttack(Unit const* who) const override
            {
                if (Unit* vehicle = who->GetVehicleBase())
                    return vehicle->GetEntry() != NPC_RUBY_DRAKE_VEHICLE && vehicle->GetEntry() != NPC_AMBER_DRAKE_VEHICLE && vehicle->GetEntry() != NPC_EMERALD_DRAKE_VEHICLE;
                return true;
            }

            void JustSummoned(Creature* summon) override
            {
                summons.push_back(summon->GetGUID());
            }

            void SummonedCreatureDies(Creature* summon, Unit* killer) override
            {
                summons.remove(summon->GetGUID());
                if (summons.empty())
                {
                    me->InterruptSpell(CURRENT_CHANNELED_SPELL);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
                    me->CastSpell(me, SPELL_EVOCATE);
                }
            }

            void SetPosition(uint8 i)
            {
                switch (i)
                {
                    case 0:
                        x = me->GetPositionX() + 4;
                        y = me->GetPositionY() - 4;
                        break;
                    case 1:
                        x = me->GetPositionX() + 4;
                        y = me->GetPositionY() + 4;
                        break;
                    case 2:
                        x = me->GetPositionX() - 4;
                        y = me->GetPositionY() + 4;
                        break;
                    case 3:
                        x = me->GetPositionX() - 4;
                        y = me->GetPositionY() - 4;
                        break;
                    default:
                        break;
                }
            }

            void StartAttack()
            {
                if (platform > 2)
                {
                    Talk(SAY_AGGRO);
                    return;
                }

                // summon guards and jump to next platform
                for (uint8 i = 0; i < 4; ++i)
                {
                    SetPosition(i);
                    me->SummonCreature(Group[group[platform]].entry[i], x, y, me->GetPositionZ(), me->GetOrientation());
                }

                teleported = false;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
                Talk(platform);
                DoCast(TeleportSpells[platform]);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_PLAYER_KILL);
            }

            void UpdateAI(uint32 diff) override
            {
                if (arcaneShieldPending)
                {
                    arcaneShieldPending = false;
                    DoCastAOE(SPELL_ARCANE_SHIELD);
                }

                // Let him finish casting teleport no matter what. UpdateVictim() can cause him to exit combat, interrupting the cast
                if (me->IsInCombat() && me->HasUnitState(UNIT_STATE_CASTING))
                    for (auto&& spell : TeleportSpells)
                        if (me->FindCurrentSpellBySpellId(spell))
                            return;

                if (!UpdateVictim())
                    return;

                if (platform < 2)
                    return;

                events.Update(diff);

                if (chasePending)
                {
                    chasePending = false;
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                    if (me->GetVictim())
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                }

                if (teleportTimer <= diff)
                {
                    me->InterruptNonMeleeSpells(false);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();
                    me->SetFlying(true);
                    DoCast(SPELL_TELEPORT);
                    teleportTimer = urand(30000, 35000);
                }
                else
                    teleportTimer -= diff;

                if (canCast && !me->FindCurrentSpellBySpellId(SPELL_EMPOWERED_ARCANE_EXPLOSION))
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                    if (castArcaneExplosionTimer <= diff)
                    {
                        canCast = false;
                        canGoBack = true;
                        me->InterruptNonMeleeSpells(false);
                        DoCastAOE(DUNGEON_MODE(SPELL_EMPOWERED_ARCANE_EXPLOSION, SPELL_EMPOWERED_ARCANE_EXPLOSION_2));
                        castArcaneExplosionTimer = 2000;
                    }
                    else
                        castArcaneExplosionTimer -= diff;
                }

                if (canGoBack)
                {
                    if (arcaneExplosionTimer <= diff)
                    {
                        if (me->GetVictim())
                        {
                            Position pos = me->GetVictim()->GetPosition();

                            me->SetFlying(false);
                            me->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
                            DoCastAOE(SPELL_TELEPORT_VISUAL, true);
                            chasePending = true;
                        }

                        canCast = false;
                        canGoBack = false;
                        arcaneExplosionTimer = 9000;
                    }
                    else
                        arcaneExplosionTimer -= diff;
                }

                if (!me->IsNonMeleeSpellCasted(false, true, true))
                {
                    if (frostBombTimer <= diff)
                    {
                        DoCastVictim(SPELL_FROSTBOMB);
                        frostBombTimer = urand(5000, 8000);
                    }
                    else
                        frostBombTimer -= diff;

                    if (timeBombTimer <= diff)
                    {
                        if (Unit* unit = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(unit, SPELL_TIME_BOMB);

                        timeBombTimer = urand(20000, 25000);
                    }
                    else
                        timeBombTimer -= diff;
                }

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
                DoCast(me, SPELL_DEATH_SPELL, true); // we cast the spell as triggered or the summon effect does not occur
            }

            void LeaveCombat()
            {
                me->RemoveAllAuras();
                me->CombatStop(false);
                me->DeleteThreatList();
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spellInfo) override
            {
                Position home;
                switch (spellInfo->Id)
                {
                    case SPELL_SUMMON_MENAGERIE:
                        home.Relocate(968.66f, 1042.53f, 527.32f, 0.077f);
                        break;
                    case SPELL_SUMMON_MENAGERIE_2:
                        home.Relocate(1164.02f, 1170.85f, 527.321f, 3.66f);
                        break;
                    case SPELL_SUMMON_MENAGERIE_3:
                        home.Relocate(1118.31f, 1080.377f, 508.361f, 4.25f);
                        break;
                    case SPELL_TELEPORT:
                        Talk(EMOTE_ARCANE_EXPLOSION);
                        Talk(SAY_ARCANE_EXPLOSION);
                        canCast = true;
                        return;
                    default:
                        return;
                }
                LeaveCombat();
                me->SetHomePosition(home);
                arcaneShieldPending = true;
                teleported = true;
                ++platform;
            }

        private:
            float x, y;

            bool canCast;
            bool canGoBack;

            bool teleported;
            bool arcaneShieldPending;
            bool chasePending;
            uint8 platform;

            uint8 group[3];

            uint32 teleportTimer;
            uint32 arcaneExplosionTimer;
            uint32 castArcaneExplosionTimer;
            uint32 frostBombTimer;
            uint32 timeBombTimer;

            std::list<uint64> summons;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetOculusAI<boss_uromAI>(creature);
        }
};

void AddSC_boss_urom()
{
    new boss_urom();
}
