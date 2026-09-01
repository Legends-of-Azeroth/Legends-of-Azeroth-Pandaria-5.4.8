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
#include "utgarde_pinnacle.h"

enum Spells
{
    SPELL_BANE                      = 48294,
    SPELL_BANE_H                    = 59301,
    SPELL_BANE_TRIGGERED            = 59302,
    SPELL_DARK_SLASH                = 48292,
    SPELL_FETID_ROT                 = 48291,
    SPELL_FETID_ROT_H               = 59300,
    SPELL_SCREAMS_OF_THE_DEAD       = 51750,
    SPELL_SPIRIT_BURST              = 48529,
    SPELL_SPIRIT_BURST_H            = 59305,
    SPELL_SPIRIT_STRIKE             = 48423,
    SPELL_SPIRIT_STRIKE_H           = 59304,
    SPELL_ANCESTORS_VENGEANCE       = 16939,

    SPELL_CHANNEL_SPIRIT_TO_YMIRON  = 48316,
    SPELL_CHANNEL_YMIRON_TO_SPIRIT  = 48307,

    SPELL_SPIRIT_FOUNT              = 48380,
    SPELL_SPIRIT_FOUNT_H            = 59320
};

enum Yells
{
    SAY_AGGRO                       = 0,
    SAY_SLAY                        = 1,
    SAY_SUMMON_BJORN                = 2,
    SAY_SUMMON_HALDOR               = 3,
    SAY_SUMMON_RANULF               = 4,
    SAY_SUMMON_TORGYN               = 5,
    SAY_DEATH                       = 6
};

enum Creatures
{
    NPC_BJORN                       = 27303,
    NPC_BJORN_VISUAL                = 27304,
    NPC_HALDOR                      = 27307,
    NPC_HALDOR_VISUAL               = 27310,
    NPC_RANULF                      = 27308,
    NPC_RANULF_VISUAL               = 27311,
    NPC_TORGYN                      = 27309,
    NPC_TORGYN_VISUAL               = 27312,
    NPC_SPIRIT_FOUNT                = 27339,
    NPC_AVENGING_SPIRIT             = 27386
};

struct ActiveBoatStruct
{
    uint32 npc;
    uint32 say;
    float MoveX, MoveY, MoveZ, SpawnX, SpawnY, SpawnZ, SpawnO;
};

static ActiveBoatStruct ActiveBoat[4] =
{
    { NPC_BJORN_VISUAL,  SAY_SUMMON_BJORN,  404.379f, -335.335f, 104.756f, 413.594f, -335.408f, 107.995f, 3.157f },
    { NPC_HALDOR_VISUAL, SAY_SUMMON_HALDOR, 380.813f, -335.069f, 104.756f, 369.994f, -334.771f, 107.995f, 6.232f },
    { NPC_RANULF_VISUAL, SAY_SUMMON_RANULF, 381.546f, -314.362f, 104.756f, 370.841f, -314.426f, 107.995f, 6.232f },
    { NPC_TORGYN_VISUAL, SAY_SUMMON_TORGYN, 404.310f, -314.761f, 104.756f, 413.992f, -314.703f, 107.995f, 3.157f }
};

class boss_ymiron : public CreatureScript
{
    public:
        boss_ymiron() : CreatureScript("boss_ymiron") { }

        struct boss_ymironAI : public ScriptedAI
        {
            boss_ymironAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                instance = creature->GetInstanceScript();

                for (int i = 0; i < 4; ++i)
                    _activeOrder[i] = i;
                for (int i = 0; i < 3; ++i)
                {
                    int r = i + (rand() % (4 - i));
                    int temp = _activeOrder[i];
                    _activeOrder[i] = _activeOrder[r];
                    _activeOrder[r] = temp;
                }

                _activeGhostGUID = ObjectGuid::Empty;
                _bjornSpiritGUID = ObjectGuid::Empty;
            }

            InstanceScript* instance;
            SummonList summons;

            void Reset() override
            {
                _isWalking = false;
                _isPause = false;
                _isActiveWithBjorn = false;
                _isActiveWithHaldor = false;
                _isActiveWithRanulf = false;
                _isActiveWithTorgyn = false;

                summons.DespawnAll();

                // Apply DK's Death Grip immune
                me->ApplySpellImmune(0, IMMUNITY_ID, 49560, true);

                if (instance)
                    instance->SetData(DATA_KING_YMIRON_EVENT, NOT_STARTED);

                _fetidRotTimer = urand(8 * IN_MILLISECONDS, 13 * IN_MILLISECONDS);
                _baneTimer = urand(18 * IN_MILLISECONDS, 23 * IN_MILLISECONDS);
                _darkSlashTimer = urand(28 * IN_MILLISECONDS, 33 * IN_MILLISECONDS);
                _ancestorsVengeanceTimer = DUNGEON_MODE(60, 45) * IN_MILLISECONDS;
                _pauseTimer = 0;

                _abilityBjornTimer  = 0;
                _abilityHaldorTimer = 0;
                _abilityRanulfTimer = 0;
                _abilityTorgynTimer = 0;

                _activeBoatNumber = 0;
                _healthAmountModifier = 1;
                _healthAmountMultipler = DUNGEON_MODE(20, 25);

                DespawnBoatGhosts(_activeGhostGUID);
                DespawnBoatGhosts(_bjornSpiritGUID);
                _activeGhostGUID = ObjectGuid::Empty;
                _bjornSpiritGUID = ObjectGuid::Empty;

                me->HandleEmoteStateCommand(EMOTE_STATE_SIT_CHAIR_HIGH);
                me->GetMap()->SetWorldState(WORLDSTATE_KINGS_BANE, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                me->HandleEmoteStateCommand(EMOTE_ONESHOT_NONE);

                if (instance)
                    instance->SetData(DATA_KING_YMIRON_EVENT, IN_PROGRESS);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SpellHitTarget(Unit* target, const SpellInfo* spell) override
            {
                if (target && target->GetTypeId() == TYPEID_PLAYER && spell->Id == SPELL_BANE_TRIGGERED)
                    me->GetMap()->SetWorldState(WORLDSTATE_KINGS_BANE, 0);
            }

            void UpdateAI(uint32 diff) override
            {
                if (me->IsInCombat() && me->GetPositionX() > 445.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                if (_isWalking)
                {
                    if (_pauseTimer <= diff)
                    {
                        Talk(ActiveBoat[_activeOrder[_activeBoatNumber]].say);

                        DoCast(me, SPELL_CHANNEL_YMIRON_TO_SPIRIT);
                        if (Creature* ghost = me->SummonCreature(ActiveBoat[_activeOrder[_activeBoatNumber]].npc, ActiveBoat[_activeOrder[_activeBoatNumber]].SpawnX, ActiveBoat[_activeOrder[_activeBoatNumber]].SpawnY, ActiveBoat[_activeOrder[_activeBoatNumber]].SpawnZ, ActiveBoat[_activeOrder[_activeBoatNumber]].SpawnO, TEMPSUMMON_CORPSE_DESPAWN, 0ms))
                        {
                            _activeGhostGUID = ghost->GetGUID();
                            ghost->CastSpell(me, SPELL_CHANNEL_SPIRIT_TO_YMIRON, true);
                            ghost->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            ghost->SetFlying(true);
                            switch (_activeOrder[_activeBoatNumber])
                            {
                                case 0:
                                    _isActiveWithBjorn  = true;
                                    break;
                                case 1:
                                    _isActiveWithHaldor = true;
                                    break;
                                case 2:
                                    _isActiveWithRanulf = true;
                                    break;
                                case 3:
                                    _isActiveWithTorgyn = true;
                                    break;
                            }
                        }

                        _pauseTimer = 3 * IN_MILLISECONDS;
                        _isPause = true;
                        _isWalking = false;
                    }
                    else
                        _pauseTimer -= diff;

                    return;
                }
                else if (_isPause)
                {
                    if (_pauseTimer <= diff)
                    {
                        _abilityBjornTimer = 5 * IN_MILLISECONDS;
                        _abilityHaldorTimer = 5 * IN_MILLISECONDS;
                        _abilityRanulfTimer = 5 * IN_MILLISECONDS;
                        _abilityTorgynTimer = 5 * IN_MILLISECONDS;

                        _isPause = false;
                        _pauseTimer = 0;
                    }
                    else
                        _pauseTimer -= diff;

                    return;
                }

                if (!UpdateVictim())
                    return;

                if (!_isPause)
                {
                    // Ymiron abilities
                    if (_baneTimer <= diff)
                    {
                        DoCast(me, DUNGEON_MODE(SPELL_BANE, SPELL_BANE_H));
                        _baneTimer = urand(20 * IN_MILLISECONDS, 25 * IN_MILLISECONDS);
                    }
                    else
                        _baneTimer -= diff;

                    if (_fetidRotTimer <= diff)
                    {
                        if (Unit* victim = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(victim, DUNGEON_MODE(SPELL_FETID_ROT, SPELL_FETID_ROT_H));

                        _fetidRotTimer = urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS);
                    }
                    else
                        _fetidRotTimer -= diff;

                    if (_darkSlashTimer <= diff)
                    {
                        DoCastVictim(SPELL_DARK_SLASH);
                        _darkSlashTimer = urand(30 * IN_MILLISECONDS, 35 * IN_MILLISECONDS);
                    }
                    else
                        _darkSlashTimer -= diff;

                    if (_ancestorsVengeanceTimer <= diff)
                    {
                        DoCast(me, SPELL_ANCESTORS_VENGEANCE);
                        _ancestorsVengeanceTimer = DUNGEON_MODE(urand(60, 65), urand(45, 50)) * IN_MILLISECONDS;
                    }
                    else
                        _ancestorsVengeanceTimer -= diff;

                    // Ghosts abilities
                    if (_isActiveWithBjorn && _abilityBjornTimer <= diff)
                    {
                        if (Creature* spirit = me->SummonCreature(NPC_SPIRIT_FOUNT, 385.0f + rand() % 10, -330.0f + rand() % 10, 104.756f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, Milliseconds(3 * MINUTE * IN_MILLISECONDS)))
                        {
                            spirit->SetSpeed(MOVE_RUN, 0.4f);
                            spirit->CastSpell(spirit, DUNGEON_MODE(SPELL_SPIRIT_FOUNT, SPELL_SPIRIT_FOUNT_H));
                            spirit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            spirit->SetDisplayId(11686);
                            spirit->GetMotionMaster()->MoveChase(me->GetVictim());
                            _bjornSpiritGUID = spirit->GetGUID();
                        }
                        _isActiveWithBjorn = false;
                    }
                    else
                        _abilityBjornTimer -= diff;

                    if (_isActiveWithHaldor && _abilityHaldorTimer <= diff)
                    {
                        DoCastVictim(DUNGEON_MODE(SPELL_SPIRIT_STRIKE, SPELL_SPIRIT_STRIKE_H));
                        _abilityHaldorTimer = 5 * IN_MILLISECONDS;
                    }
                    else
                        _abilityHaldorTimer -= diff;

                    if (_isActiveWithRanulf && _abilityRanulfTimer <= diff)
                    {
                        DoCast(me, DUNGEON_MODE(SPELL_SPIRIT_BURST, SPELL_SPIRIT_BURST_H));
                        _abilityRanulfTimer = 10 * IN_MILLISECONDS;
                    }
                    else
                        _abilityRanulfTimer -= diff;

                    if (_isActiveWithTorgyn && _abilityTorgynTimer <= diff)
                    {
                        float x, y, z;
                        x = me->GetPositionX() - 5;
                        y = me->GetPositionY() - 5;
                        z = me->GetPositionZ();
                        for (uint8 i = 0; i < 4; ++i)
                        {
                            if (Creature* spirit = me->SummonCreature(NPC_AVENGING_SPIRIT, x + rand() % 10, y + rand() % 10, z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, Milliseconds(30 * IN_MILLISECONDS)))
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                {
                                    spirit->AddThreat(target, 0.0f);
                                    spirit->SetFaction(14);
                                    spirit->AI()->AttackStart(target);
                                }
                            }
                        }
                        _abilityTorgynTimer = 15 * IN_MILLISECONDS;
                    }
                    else
                        _abilityTorgynTimer -= diff;

                    if (me->HealthBelowPct(100 - _healthAmountMultipler * _healthAmountModifier))
                    {
                        uint8 order = _healthAmountModifier - 1;
                        ++_healthAmountModifier;

                        me->InterruptNonMeleeSpells(true);
                        DoCast(me, SPELL_SCREAMS_OF_THE_DEAD);
                        me->GetMotionMaster()->Clear();
                        me->StopMoving();
                        me->AttackStop();
                        me->GetMotionMaster()->MovePoint(0, ActiveBoat[_activeOrder[order]].MoveX, ActiveBoat[_activeOrder[order]].MoveY, ActiveBoat[_activeOrder[order]].MoveZ);

                        DespawnBoatGhosts(_activeGhostGUID);
                        DespawnBoatGhosts(_bjornSpiritGUID);

                        _isActiveWithBjorn  = false;
                        _isActiveWithHaldor = false;
                        _isActiveWithRanulf = false;
                        _isActiveWithTorgyn = false;

                        _baneTimer = urand(20 * IN_MILLISECONDS, 22 * IN_MILLISECONDS);
                        _fetidRotTimer += 3 * IN_MILLISECONDS;
                        _darkSlashTimer += 3 * IN_MILLISECONDS;
                        _ancestorsVengeanceTimer += 8 * IN_MILLISECONDS;

                        _activeBoatNumber = order;
                        _isWalking = true;
                        _pauseTimer = 3 * IN_MILLISECONDS;
                        return;
                    }
                    DoMeleeAttackIfReady();
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                DespawnBoatGhosts(_activeGhostGUID);
                DespawnBoatGhosts(_bjornSpiritGUID);

                if (instance)
                    instance->SetData(DATA_KING_YMIRON_EVENT, DONE);
            }

            void DespawnBoatGhosts(ObjectGuid creatureGUID)
            {
                if (creatureGUID)
                    if (Creature* despawnCreature = ObjectAccessor::GetCreature(*me, creatureGUID))
                        despawnCreature->DisappearAndDie();

                creatureGUID = ObjectGuid::Empty;
            }

            private:
                bool _isWalking;
                bool _isPause;
                bool _isActiveWithBjorn;
                bool _isActiveWithHaldor;
                bool _isActiveWithRanulf;
                bool _isActiveWithTorgyn;

                uint8 _activeOrder[4];
                uint8 _activeBoatNumber;

                uint32 _fetidRotTimer;
                uint32 _baneTimer;
                uint32 _darkSlashTimer;
                uint32 _ancestorsVengeanceTimer;

                uint32 _abilityBjornTimer;
                uint32 _abilityHaldorTimer;
                uint32 _abilityRanulfTimer;
                uint32 _abilityTorgynTimer;

                uint32 _pauseTimer;
                uint32 _healthAmountModifier;
                uint32 _healthAmountMultipler;

                ObjectGuid _activeGhostGUID;
                ObjectGuid _bjornSpiritGUID;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_ymironAI(creature);
        }
};

void AddSC_boss_ymiron()
{
    new boss_ymiron();
}
