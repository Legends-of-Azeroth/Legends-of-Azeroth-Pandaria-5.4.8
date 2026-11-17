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
SDName: chess_event
SD%Complete: 80
SDComment: Chess AI could use some improvements.
SDCategory: Karazhan
EndScriptData */

#include "ScriptPCH.h"
#include "karazhan.h"
#include "Random.h"

enum
{
    // texts
    EMOTE_LIFT_CURSE                = 0,
    EMOTE_CHEAT                     = 1,

    SOUND_ID_GAME_BEGIN             = 10338,
    SOUND_ID_LOSE_PAWN_PLAYER_1     = 10339,
    SOUND_ID_LOSE_PAWN_PLAYER_2     = 10340,
    SOUND_ID_LOSE_PAWN_PLAYER_3     = 10341,
    SOUND_ID_LOSE_PAWN_MEDIVH_1     = 10342,
    SOUND_ID_LOSE_PAWN_MEDIVH_2     = 10343,
    SOUND_ID_LOSE_PAWN_MEDIVH_3     = 10344,
    SOUND_ID_LOSE_ROOK_PLAYER       = 10345,
    SOUND_ID_LOSE_ROOK_MEDIVH       = 10346,
    SOUND_ID_LOSE_BISHOP_PLAYER     = 10347,
    SOUND_ID_LOSE_BISHOP_MEDIVH     = 10348,
    SOUND_ID_LOSE_KNIGHT_PLAYER     = 10349,
    SOUND_ID_LOSE_KNIGHT_MEDIVH     = 10350,
    SOUND_ID_LOSE_QUEEN_PLAYER      = 10351,
    SOUND_ID_LOSE_QUEEN_MEDIVH      = 10352,
    SOUND_ID_CHECK_PLAYER           = 10353,
    SOUND_ID_CHECK_MEDIVH           = 10354,
    SOUND_ID_WIN_PLAYER             = 10355,
    SOUND_ID_WIN_MEDIVH             = 10356,
    SOUND_ID_CHEAT_1                = 10357,
    SOUND_ID_CHEAT_2                = 10358,
    SOUND_ID_CHEAT_3                = 10359,

    // movement spells
    SPELL_MOVE_GENERIC              = 30012,                    // spell which sends the signal to move - handled in core
    SPELL_MOVE_1                    = 32312,                    // spell which selects AI move square (for short range pieces)
    SPELL_MOVE_2                    = 37388,                    // spell which selects AI move square (for long range pieces)
    // SPELL_MOVE_PAWN              = 37146,                    // individual move spells (used only by controlled npcs)
    // SPELL_MOVE_KNIGHT            = 37144,
    // SPELL_MOVE_QUEEN             = 37148,
    // SPELL_MOVE_ROCK              = 37151,
    // SPELL_MOVE_BISHOP            = 37152,
    // SPELL_MOVE_KING              = 37153,

    // additional movement spells
    SPELL_CHANGE_FACING             = 30284,                    // spell which sends the initial facing request - handled in core
    SPELL_FACE_SQUARE               = 30270,                    // change facing - finalize facing update

    SPELL_MOVE_TO_SQUARE            = 30253,                    // spell which sends the move response from the square to the piece
    SPELL_MOVE_COOLDOWN             = 30543,                    // add some cooldown to movement
    SPELL_MOVE_MARKER               = 32261,                    // white beam visual - used to mark the movement as complete
    SPELL_DISABLE_SQUARE            = 32745,                    // used by the White / Black triggers on the squares when a chess piece moves into place
    SPELL_IS_SQUARE_OCCUPIED        = 39400,                    // cast when a chess piece moves to another square
    SPELL_IS_SQUARE_OCCUPIED_CHECK  = 39399,                    // triggered by 39400; used to check if the square is occupied (if hits a target); Missing in 2.4.3

    // generic spells
    SPELL_IN_GAME                   = 30532,                    // teleport player near the entrance
    SPELL_CONTROL_PIECE             = 30019,                    // control a chess piece
    SPELL_RECENTLY_IN_GAME          = 30529,                    // debuff on player after chess piece uncharm

    SPELL_CHESS_AI_ATTACK_TIMER     = 32226,                    // melee action timer - triggers 32225
    SPELL_ACTION_MELEE              = 32225,                    // handle melee attacks
    SPELL_MELEE_DAMAGE              = 32247,                    // melee damage spell - used by all chess pieces
    // SPELL_AI_SNAPSHOT_TIMER      = 37440,                    // used to trigger spell 32260; purpose and usage unk
    // SPELL_DISABLE_SQUARE_SELF    = 32260,                    // used when a piece moves to another square
    // SPELL_AI_ACTION_TIMER        = 37504,                    // handle some kind of event check. Cast by npc 17459. Currently the way it works is unk
    // SPELL_DISABLE_SQUARE         = 30271,                    // not used
    // SPELL_FIND_ENEMY             = 32303,                    // not used
    // SPELL_MOVE_NEAR_UNIT         = 30417,                    // not used
    // SPELL_GET_EMPTY_SQUARE       = 30418,                    // not used
    // SPELL_FACE_NEARBY_ENEMY      = 37787,                    // not used
    // SPELL_POST_MOVE_FACING       = 38011,                    // not used

    // melee action spells
    SPELL_MELEE_FOOTMAN             = 32227,
    SPELL_MELEE_WATER_ELEM          = 37142,
    SPELL_MELEE_CHARGER             = 37143,
    SPELL_MELEE_CLERIC              = 37147,
    SPELL_MELEE_CONJURER            = 37149,
    SPELL_MELEE_KING_LLANE          = 37150,
    SPELL_MELEE_GRUNT               = 32228,
    SPELL_MELEE_DAEMON              = 37220,
    SPELL_MELEE_NECROLYTE           = 37337,
    SPELL_MELEE_WOLF                = 37339,
    SPELL_MELEE_WARLOCK             = 37345,
    SPELL_MELEE_WARCHIEF_BLACKHAND  = 37348,

    // cheat spells
    SPELL_HAND_OF_MEDIVH_HORDE      = 39338,                    // triggers 39339
    SPELL_HAND_OF_MEDIVH_ALLIANCE   = 39342,                    // triggers 39339
    SPELL_FURY_OF_MEDIVH_HORDE      = 39341,                    // triggers 39343
    SPELL_FURY_OF_MEDIVH_ALLIANCE   = 39344,                    // triggers 39345
    SPELL_FURY_OF_MEDIVH_AURA       = 39383,
    SPELL_FURY_OF_MEDIVH_INFERNO    = 39346,
    // SPELL_FULL_HEAL_HORDE        = 39334,                    // spells are not confirmed (probably removed after 2.4.3)
    // SPELL_FULL_HEAL_ALLIANCE     = 39335,

    // spells used by the chess npcs
    SPELL_HEROISM                   = 37471,                    // human king
    SPELL_SWEEP                     = 37474,
    SPELL_BLOODLUST                 = 37472,                    // orc king
    SPELL_CLEAVE                    = 37476,
    SPELL_HEROIC_BLOW               = 37406,                    // human pawn
    SPELL_SHIELD_BLOCK              = 37414,
    SPELL_VICIOUS_STRIKE            = 37413,                    // orc pawn
    SPELL_WEAPON_DEFLECTION         = 37416,
    SPELL_SMASH                     = 37453,                    // human knight
    SPELL_STOMP                     = 37498,
    SPELL_BITE                      = 37454,                    // orc knight
    SPELL_HOWL                      = 37502,
    SPELL_ELEMENTAL_BLAST           = 37462,                    // human queen
    SPELL_RAIN_OF_FIRE              = 37465,
    SPELL_FIREBALL                  = 37463,                    // orc queen
    // SPELL_POISON_CLOUD           = 37469,
    SPELL_POISON_CLOUD_ACTION       = 37775,                    // triggers 37469 - acts as a target selector spell for orc queen
    SPELL_HEALING                   = 37455,                    // human bishop
    SPELL_HOLY_LANCE                = 37459,
    // SPELL_SHADOW_MEND            = 37456,                    // orc bishop
    SPELL_SHADOW_MEND_ACTION        = 37824,                    // triggers 37456 - acts as a target selector spell for orc bishop
    SPELL_SHADOW_SPEAR              = 37461,
    SPELL_GEYSER                    = 37427,                    // human rook
    SPELL_WATER_SHIELD              = 37432,
    SPELL_HELLFIRE                  = 37428,                    // orc rook
    SPELL_FIRE_SHIELD               = 37434,

    // spells used to transform side trigger when npc dies
    SPELL_TRANSFORM_FOOTMAN         = 39350,
    SPELL_TRANSFORM_CHARGER         = 39352,
    SPELL_TRANSFORM_CLERIC          = 39353,
    SPELL_TRANSFORM_WATER_ELEM      = 39354,
    SPELL_TRANSFORM_CONJURER        = 39355,
    SPELL_TRANSFORM_KING_LLANE      = 39356,
    SPELL_TRANSFORM_GRUNT           = 39357,
    SPELL_TRANSFORM_WOLF            = 39358,
    SPELL_TRANSFORM_NECROLYTE       = 39359,
    SPELL_TRANSFORM_DAEMON          = 39360,
    SPELL_TRANSFORM_WARLOCK         = 39361,
    SPELL_TRANSFORM_BLACKHAND       = 39362,

    // generic npcs
    // NPC_SQUARE_OUTSIDE_B         = 17316,                    // used to check the interior of the board
    // NPC_SQUARE_OUTSIDE_W         = 17317,                    // not used in our script; keep for reference only
    NPC_FURY_MEDIVH_VISUAL          = 22521,                    // has aura 39383

    // misc
    TARGET_TYPE_SELF                = 0,
    TARGET_TYPE_RANDOM              = 1,
    TARGET_TYPE_FRIENDLY            = 2,
    TARGET_TYPE_ADJACENT            = 3,
};

class npc_echo_of_medivh : public CreatureScript
{
    public:
        npc_echo_of_medivh() : CreatureScript("npc_echo_of_medivh") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                if (instance->GetData(TYPE_CHESS) == SPECIAL)
                    player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                return true;
            }

            return false;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                // reset the board
                if (InstanceScript* instance = creature->GetInstanceScript())
                    if (instance->GetData(TYPE_CHESS) == SPECIAL)
                        instance->SetData(TYPE_CHESS, DONE);

                player->CLOSE_GOSSIP_MENU();
            }

            return true;
        }

        struct npc_echo_of_medivhAI : public ScriptedAI
        {
            npc_echo_of_medivhAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                Reset();
            }

            void Reset() override
            {
                cheatTimer = 90000;
                cheatEffectTimer = 0;
                movementTimer = 1000;
                me->SetWalk(true);
            }

            void MoveInLineOfSight(Unit* /*who*/) override { }
            void AttackStart(Unit* /*who*/) override { }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_FURY_MEDIVH_VISUAL)
                {
                    summon->CastSpell(summon, SPELL_FURY_OF_MEDIVH_AURA, true);
                    summon->CastSpell(summon, SPELL_FURY_OF_MEDIVH_INFERNO, true);
                }
            }

            void MovementInform(uint32 type, uint32 point) override
            {
                if (type == POINT_MOTION_TYPE && point)
                    movementTimer = urand(1000, 5000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (movementTimer)
                {
                    if (movementTimer <= diff)
                    {
                        Position dest = { -11092.44f, -1849.69f, 221.07f, 3.79f };
                        dest.RelocateOffset(0.0f, frand(0.0f, 13.0f));
                        dest.RelocateOffset(frand(0.0f, 2 * M_PI), frand(0.0f, 3.0f));
                        me->GetMotionMaster()->MovePoint(1, dest);

                        movementTimer = 0;
                    }
                    else
                        movementTimer -= diff;
                }

                if (!instance || instance->GetData(TYPE_CHESS) != IN_PROGRESS)
                {
                    cheatTimer = 90000;
                    cheatEffectTimer = 0;
                    return;
                }

                if (cheatTimer <= diff)
                {
                    DoPlaySoundToSet(me, RAND(SOUND_ID_CHEAT_1, SOUND_ID_CHEAT_2, SOUND_ID_CHEAT_3));
                    cheatTimer = 90000;
                    cheatEffectTimer = 2500;
                }
                else
                    cheatTimer -= diff;

                if (cheatEffectTimer)
                {
                    if (cheatEffectTimer <= diff)
                    {
                        DoCast(me, urand(0, 1)
                            ? (dynamic_cast<karazhan_accessor*>(instance)->GetPlayerTeam() == ALLIANCE ? SPELL_HAND_OF_MEDIVH_HORDE : SPELL_HAND_OF_MEDIVH_ALLIANCE)
                            : (dynamic_cast<karazhan_accessor*>(instance)->GetPlayerTeam() == ALLIANCE ? SPELL_FURY_OF_MEDIVH_ALLIANCE : SPELL_FURY_OF_MEDIVH_HORDE));

                        Talk(EMOTE_CHEAT);
                        cheatEffectTimer = 0;
                    }
                    else
                        cheatEffectTimer -= diff;
                }
            }

        private:
            InstanceScript* instance;

            uint32 cheatTimer;
            uint32 cheatEffectTimer;
            uint32 movementTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_echo_of_medivhAI(creature);
        }
};

class npc_chess_piece : public CreatureScript
{
    public:
        npc_chess_piece() : CreatureScript("npc_chess_piece") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (!player->HasAura(SPELL_RECENTLY_IN_GAME) && !player->HasAura(SPELL_CONTROL_PIECE) && !creature->HasAura(SPELL_CONTROL_PIECE))
                if (InstanceScript* instance = creature->GetInstanceScript())
                    if (instance->GetData(TYPE_CHESS) != DONE && instance->GetData(TYPE_CHESS) != FAIL && player->GetTeam() == creature->AI()->GetData(1) || dynamic_cast<karazhan_accessor*>(instance)->IsFriendlyGameReady())
                        player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                // start event when used on the king
                if (InstanceScript* instance = creature->GetInstanceScript())
                {
                    if (instance->GetData(TYPE_CHESS) == NOT_STARTED)
                        instance->SetData(TYPE_CHESS, IN_PROGRESS);
                    else if (instance->GetData(TYPE_CHESS) == DONE)
                        instance->SetData(TYPE_CHESS, SPECIAL);

                    // teleport at the entrance and control the chess piece
                    player->CastSpell(player, SPELL_IN_GAME, true);
                    player->CastSpell(creature, SPELL_CONTROL_PIECE, true);
                }

                player->CLOSE_GOSSIP_MENU();
            }

            return true;
        }

        bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, Creature* target) override
        {
            // movement perform spell
            if (spellId == SPELL_MOVE_TO_SQUARE && effIndex == EFFECT_0)
            {
                if (caster->GetTypeId() == TYPEID_UNIT && !caster->HasAura(SPELL_DISABLE_SQUARE) && !caster->HasAura(SPELL_IS_SQUARE_OCCUPIED))
                {
                    caster->CastSpell(caster, SPELL_DISABLE_SQUARE, true);
                    caster->CastSpell(caster, SPELL_IS_SQUARE_OCCUPIED, true);

                    target->CastSpell(target, SPELL_MOVE_COOLDOWN);
                    target->AddCreatureSpellCooldown(SPELL_MOVE_COOLDOWN);
                    target->AI()->SetGUID(caster->GetGUID(), 1);
                }

                return true;
            }
            // generic melee tick
            else if (spellId == SPELL_ACTION_MELEE && effIndex == EFFECT_0)
            {
                if (!target->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_ACTIVE))
                    target->SetFacingTo(target->GetOrientation());

                target->CastSpell(target, target->AI()->GetData(0), true);
                return true;
            }
            // square facing
            else if (spellId == SPELL_FACE_SQUARE && effIndex == EFFECT_0)
            {
                if (caster->GetTypeId() != TYPEID_UNIT)
                    return true;

                if (Creature* square = ObjectAccessor::GetCreature(*target, target->AI()->GetGUID()))
                {
                    if (caster->GetExactDist2d(square) > 8.0f)
                        return true;

                    if (caster->GetEntry() != (square->GetEntry() == NPC_SQUARE_WHITE ? NPC_SQUARE_BLACK : NPC_SQUARE_WHITE) &&
                        caster->GetEntry() != (square->GetEntry() == NPC_SQUARE_WHITE ? NPC_SQUARE_OUTSIDE_BLACK : NPC_SQUARE_OUTSIDE_WHITE))
                        return true;

                    target->SetFacingTo(square->GetAngle(caster));
                    target->AI()->DoAction(0);
                }
                return true;
            }
            // Is Square OCCUPIED (DND)
            else if (spellId == SPELL_IS_SQUARE_OCCUPIED_CHECK)
            {
                //caster->CastSpell(caster, SPELL_DISABLE_SQUARE, true);
                return true;
            }

            return false;
        }

        struct npc_chess_pieceAI : public ScriptedAI
        {
            npc_chess_pieceAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();

                isAttacked = false;
                isUncharming = false;

                switch (me->GetEntry())
                {
                    case NPC_KING_LLANE:                chessPieceInfo = ChessPieceInfo { true,   ALLIANCE, FACTION_ID_CHESS_ALLIANCE,   8.0f,  1,  EMOTE_ONESHOT_YES,      6164,   TARGET_TYPE_RANDOM,     20.0f,    0,    SPELL_HEROISM,              TARGET_TYPE_RANDOM,     10.0f,  180,    SPELL_SWEEP,                SPELL_MELEE_KING_LLANE,         SPELL_TRANSFORM_KING_LLANE, SOUND_ID_WIN_MEDIVH,            SOUND_ID_WIN_MEDIVH,            SOUND_ID_WIN_MEDIVH,            SOUND_ID_WIN_PLAYER,            SOUND_ID_WIN_PLAYER,            SOUND_ID_WIN_PLAYER         }; break;
                    case NPC_WARCHIEF_BLACKHAND:        chessPieceInfo = ChessPieceInfo { true,   HORDE,    FACTION_ID_CHESS_HORDE,      8.0f,  1,  EMOTE_ONESHOT_ROAR,     2694,   TARGET_TYPE_RANDOM,     20.0f,    0,    SPELL_BLOODLUST,            TARGET_TYPE_RANDOM,     10.0f,  180,    SPELL_CLEAVE,               SPELL_MELEE_WARCHIEF_BLACKHAND, SPELL_TRANSFORM_BLACKHAND,  SOUND_ID_WIN_MEDIVH,            SOUND_ID_WIN_MEDIVH,            SOUND_ID_WIN_MEDIVH,            SOUND_ID_WIN_PLAYER,            SOUND_ID_WIN_PLAYER,            SOUND_ID_WIN_PLAYER         }; break;
                    case NPC_HUMAN_CONJURER:            chessPieceInfo = ChessPieceInfo { false,  ALLIANCE, FACTION_ID_CHESS_ALLIANCE,  20.0f,  3,  EMOTE_ONESHOT_YES,      6164,   TARGET_TYPE_RANDOM,     25.0f,  180,    SPELL_RAIN_OF_FIRE,         TARGET_TYPE_RANDOM,     20.0f,  180,    SPELL_ELEMENTAL_BLAST,      SPELL_MELEE_CONJURER,           SPELL_TRANSFORM_CONJURER,   SOUND_ID_LOSE_QUEEN_PLAYER,     SOUND_ID_LOSE_QUEEN_PLAYER,     SOUND_ID_LOSE_QUEEN_PLAYER,     SOUND_ID_LOSE_QUEEN_MEDIVH,     SOUND_ID_LOSE_QUEEN_MEDIVH,     SOUND_ID_LOSE_QUEEN_MEDIVH, }; break;
                    case NPC_ORC_WARLOCK:               chessPieceInfo = ChessPieceInfo { false,  HORDE,    FACTION_ID_CHESS_HORDE,     20.0f,  3,  EMOTE_ONESHOT_YES,      6024,   TARGET_TYPE_RANDOM,     25.0f,  180,    SPELL_POISON_CLOUD_ACTION,  TARGET_TYPE_RANDOM,     20.0f,  180,    SPELL_FIREBALL,             SPELL_MELEE_WARLOCK,            SPELL_TRANSFORM_WARLOCK,    SOUND_ID_LOSE_QUEEN_PLAYER,     SOUND_ID_LOSE_QUEEN_PLAYER,     SOUND_ID_LOSE_QUEEN_PLAYER,     SOUND_ID_LOSE_QUEEN_MEDIVH,     SOUND_ID_LOSE_QUEEN_MEDIVH,     SOUND_ID_LOSE_QUEEN_MEDIVH, }; break;
                    case NPC_HUMAN_CLERIC:              chessPieceInfo = ChessPieceInfo { false,  ALLIANCE, FACTION_ID_CHESS_ALLIANCE,   8.0f,  2,  EMOTE_ONESHOT_BOW,      0,      TARGET_TYPE_FRIENDLY,   25.0f,  360,    SPELL_HEALING,              TARGET_TYPE_RANDOM,     18.0f,   20,    SPELL_HOLY_LANCE,           SPELL_MELEE_CLERIC,             SPELL_TRANSFORM_CLERIC,     SOUND_ID_LOSE_BISHOP_PLAYER,    SOUND_ID_LOSE_BISHOP_PLAYER,    SOUND_ID_LOSE_BISHOP_PLAYER,    SOUND_ID_LOSE_BISHOP_MEDIVH,    SOUND_ID_LOSE_BISHOP_MEDIVH,    SOUND_ID_LOSE_BISHOP_MEDIVH }; break;
                    case NPC_ORC_NECROLYTE:             chessPieceInfo = ChessPieceInfo { false,  HORDE,    FACTION_ID_CHESS_HORDE,      8.0f,  2,  EMOTE_ONESHOT_NONE,     6026,   TARGET_TYPE_FRIENDLY,   25.0f,  360,    SPELL_SHADOW_MEND_ACTION,   TARGET_TYPE_RANDOM,     18.0f,   20,    SPELL_SHADOW_SPEAR,         SPELL_MELEE_NECROLYTE,          SPELL_TRANSFORM_NECROLYTE,  SOUND_ID_LOSE_BISHOP_PLAYER,    SOUND_ID_LOSE_BISHOP_PLAYER,    SOUND_ID_LOSE_BISHOP_PLAYER,    SOUND_ID_LOSE_BISHOP_MEDIVH,    SOUND_ID_LOSE_BISHOP_MEDIVH,    SOUND_ID_LOSE_BISHOP_MEDIVH }; break;
                    case NPC_CONJURED_WATER_ELEMENTAL:  chessPieceInfo = ChessPieceInfo { false,  ALLIANCE, FACTION_ID_CHESS_ALLIANCE,   8.0f,  3,  EMOTE_ONESHOT_NONE,     0,      TARGET_TYPE_RANDOM,      9.0f,  180,    SPELL_GEYSER,               TARGET_TYPE_RANDOM,      9.0f,  180,    SPELL_WATER_SHIELD,         SPELL_MELEE_WATER_ELEM,         SPELL_TRANSFORM_WATER_ELEM, SOUND_ID_LOSE_ROOK_PLAYER,      SOUND_ID_LOSE_ROOK_PLAYER,      SOUND_ID_LOSE_ROOK_PLAYER,      SOUND_ID_LOSE_ROOK_MEDIVH,      SOUND_ID_LOSE_ROOK_MEDIVH,      SOUND_ID_LOSE_ROOK_MEDIVH   }; break;
                    case NPC_SUMMONED_DAEMON:           chessPieceInfo = ChessPieceInfo { false,  HORDE,    FACTION_ID_CHESS_HORDE,      8.0f,  3,  EMOTE_ONESHOT_NONE,     9105,   TARGET_TYPE_RANDOM,      9.0f,  180,    SPELL_HELLFIRE,             TARGET_TYPE_RANDOM,      9.0f,  180,    SPELL_FIRE_SHIELD,          SPELL_MELEE_DAEMON,             SPELL_TRANSFORM_DAEMON,     SOUND_ID_LOSE_ROOK_PLAYER,      SOUND_ID_LOSE_ROOK_PLAYER,      SOUND_ID_LOSE_ROOK_PLAYER,      SOUND_ID_LOSE_ROOK_MEDIVH,      SOUND_ID_LOSE_ROOK_MEDIVH,      SOUND_ID_LOSE_ROOK_MEDIVH   }; break;
                    case NPC_HUMAN_CHARGER:             chessPieceInfo = ChessPieceInfo { false,  ALLIANCE, FACTION_ID_CHESS_ALLIANCE,  15.0f,  4,  EMOTE_ONESHOT_NONE,     729,    TARGET_TYPE_RANDOM,      8.0f,   63,    SPELL_SMASH,                TARGET_TYPE_RANDOM,     10.0f,  100,    SPELL_STOMP,                SPELL_MELEE_CHARGER,            SPELL_TRANSFORM_CHARGER,    SOUND_ID_LOSE_KNIGHT_PLAYER,    SOUND_ID_LOSE_KNIGHT_PLAYER,    SOUND_ID_LOSE_KNIGHT_PLAYER,    SOUND_ID_LOSE_KNIGHT_MEDIVH,    SOUND_ID_LOSE_KNIGHT_MEDIVH,    SOUND_ID_LOSE_KNIGHT_MEDIVH }; break;
                    case NPC_ORC_WOLF:                  chessPieceInfo = ChessPieceInfo { false,  HORDE,    FACTION_ID_CHESS_HORDE,     15.0f,  4,  EMOTE_ONESHOT_NONE,     390,    TARGET_TYPE_RANDOM,      8.0f,   63,    SPELL_BITE,                 TARGET_TYPE_RANDOM,     10.0f,  100,    SPELL_HOWL,                 SPELL_MELEE_WOLF,               SPELL_TRANSFORM_WOLF,       SOUND_ID_LOSE_KNIGHT_PLAYER,    SOUND_ID_LOSE_KNIGHT_PLAYER,    SOUND_ID_LOSE_KNIGHT_PLAYER,    SOUND_ID_LOSE_KNIGHT_MEDIVH,    SOUND_ID_LOSE_KNIGHT_MEDIVH,    SOUND_ID_LOSE_KNIGHT_MEDIVH }; break;
                    case NPC_HUMAN_FOOTMAN:             chessPieceInfo = ChessPieceInfo { false,  ALLIANCE, FACTION_ID_CHESS_ALLIANCE,   8.0f,  5,  EMOTE_ONESHOT_SALUTE,   6164,   TARGET_TYPE_RANDOM,      8.0f,   63,    SPELL_HEROIC_BLOW,          TARGET_TYPE_RANDOM,      8.0f,  180,    SPELL_SHIELD_BLOCK,         SPELL_MELEE_FOOTMAN,            SPELL_TRANSFORM_FOOTMAN,    SOUND_ID_LOSE_PAWN_PLAYER_1,    SOUND_ID_LOSE_PAWN_PLAYER_2,    SOUND_ID_LOSE_PAWN_PLAYER_3,    SOUND_ID_LOSE_PAWN_MEDIVH_1,    SOUND_ID_LOSE_PAWN_MEDIVH_2,    SOUND_ID_LOSE_PAWN_MEDIVH_3 }; break;
                    case NPC_ORC_GRUNT:                 chessPieceInfo = ChessPieceInfo { false,  HORDE,    FACTION_ID_CHESS_HORDE,      8.0f,  5,  EMOTE_ONESHOT_SALUTE,   6025,   TARGET_TYPE_RANDOM,      8.0f,   63,    SPELL_VICIOUS_STRIKE,       TARGET_TYPE_RANDOM,      8.0f,  180,    SPELL_WEAPON_DEFLECTION,    SPELL_MELEE_GRUNT,              SPELL_TRANSFORM_GRUNT,      SOUND_ID_LOSE_PAWN_PLAYER_1,    SOUND_ID_LOSE_PAWN_PLAYER_2,    SOUND_ID_LOSE_PAWN_PLAYER_3,    SOUND_ID_LOSE_PAWN_MEDIVH_1,    SOUND_ID_LOSE_PAWN_MEDIVH_2,    SOUND_ID_LOSE_PAWN_MEDIVH_3 }; break;
                }

                if (SpellInfo const* spellPrimary = sSpellMgr->GetSpellInfo(chessPieceInfo.PrimarySpell))
                    if (SpellInfo const* spellSecondary = sSpellMgr->GetSpellInfo(chessPieceInfo.SecondarySpell))
                        isAlternatingSpells = !spellPrimary->RecoveryTime && !spellSecondary->RecoveryTime;

                Reset();
            }

            void Reset() override
            {
                if (!me->IsVisible())
                {
                    Position curPos = me->GetPosition();

                    me->Relocate(me->GetHomePosition());
                    me->SetVisible(true);
                    me->Relocate(curPos);
                }

                if (!isUncharming)
                {
                    moveTimer = 0;
                    isPrimarySpell = true;
                    currentOrientation = me->GetOrientation();
                }
                moveCommandTimer = facingCommandTimer = 1000;
                isAllowedToCast = spellPrimaryTimer = spellSecondaryTimer = me->HasAura(SPELL_CONTROL_PIECE) ? 0 : 1000;
                ResetMoveChance();
                me->SetWalk(true);
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 0.0f);

                // cancel move timer for player faction npcs or for friendly games
                if (instance)
                {
                    if (dynamic_cast<karazhan_accessor*>(instance)->GetPlayerTeam() == ALLIANCE && me->GetFaction() == FACTION_ID_CHESS_ALLIANCE ||
                        dynamic_cast<karazhan_accessor*>(instance)->GetPlayerTeam() == HORDE && me->GetFaction() == FACTION_ID_CHESS_HORDE ||
                        instance->GetData(TYPE_CHESS) == DONE || instance->GetData(TYPE_CHESS) == SPECIAL)
                        moveCommandTimer = facingCommandTimer = 0;

                    if (instance->GetData(TYPE_CHESS) == SPECIAL)
                        isAllowedToCast = spellPrimaryTimer = spellSecondaryTimer = 0;
                }
            }

            // no default attacking or evading
            void MoveInLineOfSight(Unit* /*who*/) override { }
            void AttackStart(Unit* /*who*/) override { }
            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override { }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->HasAura(SPELL_GAME_OVER))
                {
                    damage = 0;
                    return;
                }

                if (!chessPieceInfo.IsKing)
                    return;

                if (!damage || isAttacked || !instance || attacker->GetTypeId() != TYPEID_UNIT || instance->GetData(TYPE_CHESS) == SPECIAL)
                    return;

                if (Creature* medivh = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ECHO_MEDIVH)))
                {
                    if (dynamic_cast<karazhan_accessor*>(instance)->GetPlayerTeam() == chessPieceInfo.PieceTeam)
                        DoPlaySoundToSet(medivh, SOUND_ID_CHECK_PLAYER);
                    else
                        DoPlaySoundToSet(medivh, SOUND_ID_CHECK_MEDIVH);
                }

                isAttacked = true;
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* square = ObjectAccessor::GetCreature(*me, currentSquareGuid))
                    square->RemoveAllAuras();

                if (!instance)
                    return;

                Creature* medivh = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ECHO_MEDIVH));
                if (!medivh)
                    return;

                if (chessPieceInfo.IsKing && instance->GetData(TYPE_CHESS) == SPECIAL)
                    instance->SetData(TYPE_CHESS, DONE);
                else if (dynamic_cast<karazhan_accessor*>(instance)->GetPlayerTeam() == chessPieceInfo.PieceTeam)
                {
                    if (instance->GetData(TYPE_CHESS) != SPECIAL)
                        DoPlaySoundToSet(medivh, RAND(chessPieceInfo.LoseSoundPlayer1, chessPieceInfo.LoseSoundPlayer2, chessPieceInfo.LoseSoundPlayer3));
                    if (chessPieceInfo.IsKing)
                        instance->SetData(TYPE_CHESS, FAIL);
                }
                else
                {
                    if (instance->GetData(TYPE_CHESS) != SPECIAL)
                        DoPlaySoundToSet(medivh, RAND(chessPieceInfo.LoseSoundMedivh1, chessPieceInfo.LoseSoundMedivh2, chessPieceInfo.LoseSoundMedivh3));
                    if (chessPieceInfo.IsKing)
                    {
                        medivh->AI()->Talk(EMOTE_LIFT_CURSE);
                        instance->SetData(TYPE_CHESS, DONE);
                    }
                }

                me->m_Events.Schedule(10000, [this]()
                {
                    if (me->isDead())
                    {
                        dynamic_cast<karazhan_accessor*>(instance)->DoMoveChessPieceToSides(chessPieceInfo.TransformSpell, chessPieceInfo.Faction);
                        me->SetVisible(false);
                    }
                });

                if (chessPieceInfo.IsKing)
                    dynamic_cast<karazhan_accessor*>(instance)->FinishChessEvent(chessPieceInfo.Faction);
            }

            void OnCharmed(bool apply) override
            {
                if (!apply)
                {
                    isUncharming = true;
                    Reset();
                    isUncharming = false;
                }
            }

            uint32 GetData(uint32 type) const override { return type ? chessPieceInfo.PieceTeam : chessPieceInfo.MeleeSpell; }

            ObjectGuid GetGUID(int32 /* = 0 */) const override { return currentSquareGuid; }

            void SetGUID(ObjectGuid guid, int32 type) override
            {
                // handle move event
                if (type)
                {
                    // clear the current square
                    if (Creature* square = ObjectAccessor::GetCreature(*me, currentSquareGuid))
                        square->RemoveAllAuras();

                    if (chessPieceInfo.MoveSound)
                        me->PlayDistanceSound(chessPieceInfo.MoveSound);

                    currentSquareGuid = guid;
                    moveTimer = 2000;
                }
                // handle encounter start event
                else
                {
                    // reset the variables
                    Reset();
                    currentSquareGuid = guid;
                    isAttacked = false;

                    // ToDo: enable this when the scope of the spell is clear
                    //if (Creature* pStalker = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_WAITING_ROOM_STALKER))
                    //    pStalker->CastSpell(pStalker, SPELL_AI_ACTION_TIMER, true);

                    //DoCast(me, SPELL_AI_SNAPSHOT_TIMER, true);
                    DoCast(me, SPELL_CHESS_AI_ATTACK_TIMER, true);

                    me->m_Events.Schedule(500, [this]()
                    {
                        if (Creature* square = ObjectAccessor::GetCreature(*me, currentSquareGuid))
                        {
                            square->CastSpell(square, SPELL_MOVE_MARKER, true);
                            square->CastSpell(square, SPELL_IS_SQUARE_OCCUPIED, true);
                        }
                        me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK1H);
                    });
                }
            }

            void DoAction(int32 const action) override
            {
                currentOrientation = me->GetOrientation();
            }

            void MovementInform(uint32 type, uint32 point) override
            {
                if (type != POINT_MOTION_TYPE || !point)
                    return;

                // update facing
                if (Unit* target = GetTargetByType(TARGET_TYPE_ADJACENT, 8.0f, 0))
                    DoCast(target, SPELL_CHANGE_FACING);
                else
                    me->SetFacingTo(currentOrientation);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                // do a soft reset when the piece is controlled
                if (caster->GetTypeId() == TYPEID_PLAYER && spell->Id == SPELL_CONTROL_PIECE)
                {
                    me->RestoreFaction();
                    Reset();
                }
                else if (spell->Id == SPELL_MOVE_COOLDOWN && chessPieceInfo.MoveEmote)
                    me->HandleEmoteCommand(chessPieceInfo.MoveEmote);
            }

            // Function which returns a random target by type and range
            Unit* GetTargetByType(uint8 type, float range, float arc = M_PI)
            {
                if (!instance)
                    return nullptr;

                uint32 team = me->GetFaction() == FACTION_ID_CHESS_ALLIANCE ? FACTION_ID_CHESS_HORDE : FACTION_ID_CHESS_ALLIANCE;

                // get friendly list for this type
                if (type == TARGET_TYPE_FRIENDLY)
                    team = me->GetFaction();

                // Get the list of enemies
                std::vector<Creature*> targets;
                for (auto&& guid : dynamic_cast<karazhan_accessor*>(instance)->GetChessPiecesByFaction(team))
                {
                    Creature* target = ObjectAccessor::GetCreature(*me, guid);
                    if (target && target->IsAlive())
                    {
                        // check for specified range targets and angle; Note: to be checked if the angle is right
                        if (!me->IsWithinCombatRange(target, range) || arc > 0 && !me->HasInArc(arc, target))
                            continue;

                        if (type == TARGET_TYPE_ADJACENT)
                            if (Creature* mySquare = ObjectAccessor::GetCreature(*me, currentSquareGuid))
                                if (Creature* theirSquare = ObjectAccessor::GetCreature(*target, target->AI()->GetGUID()))
                                    if (theirSquare->GetEntry() != (mySquare->GetEntry() == NPC_SQUARE_WHITE ? NPC_SQUARE_BLACK : NPC_SQUARE_WHITE))
                                        continue;

                        // skip friendly targets which are at full HP
                        if (type == TARGET_TYPE_FRIENDLY && target->GetHealth() == target->GetMaxHealth())
                            continue;

                        targets.push_back(target);
                    }
                }

                if (targets.empty())
                    return nullptr;

                return Trinity::Containers::SelectRandomContainerElement(targets);
            }

            // Function to get a square as close as possible to the enemy
            Unit* GetMovementSquare()
            {
                if (!instance)
                    return nullptr;

                // this will replace the targeting system of spells SPELL_MOVE_1 and SPELL_MOVE_2

                // get all available squares for movement
                std::list<Creature*> squares;
                GetCreatureListWithEntryInGrid(squares, me, NPC_SQUARE_BLACK, chessPieceInfo.MoveRadius);
                GetCreatureListWithEntryInGrid(squares, me, NPC_SQUARE_WHITE, chessPieceInfo.MoveRadius);

                squares.remove_if([this](Creature* square) { return me->GetExactDist2d(square) > chessPieceInfo.MoveRadius || square->HasAura(SPELL_DISABLE_SQUARE) || square->HasAura(SPELL_IS_SQUARE_OCCUPIED); });

                if (squares.empty())
                    return nullptr;

                // Get the list of enemies
                std::list<Creature*> enemies;

                for (auto&& guid : dynamic_cast<karazhan_accessor*>(instance)->GetChessPiecesByFaction(me->GetFaction() == FACTION_ID_CHESS_ALLIANCE ? FACTION_ID_CHESS_HORDE : FACTION_ID_CHESS_ALLIANCE))
                    if (Creature* target = ObjectAccessor::GetCreature(*me, guid))
                        if (target->IsAlive())
                            enemies.push_back(target);

                if (enemies.empty())
                    return nullptr;

                // Sort the enemies by distance and the squares compared to the distance to the closest enemy
                enemies.sort(Trinity::ObjectDistanceOrderPred(me));
                squares.sort(Trinity::ObjectDistanceOrderPred(enemies.front()));

                return squares.front();
            }

            void DoCastPrimarySpell()
            {
                if (!me->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_ACTIVE))
                    me->SetFacingTo(me->GetOrientation());

                if (Unit* target = GetTargetByType(chessPieceInfo.PrimaryTarget, chessPieceInfo.PrimaryRadius, chessPieceInfo.PrimaryArc / 180.0f * M_PI))
                {
                    DoCast(target, chessPieceInfo.PrimarySpell);
                    SpellInfo const* spell = sSpellMgr->GetSpellInfo(chessPieceInfo.PrimarySpell);
                    me->AddCreatureSpellCooldown(chessPieceInfo.PrimarySpell);
                    me->_AddCreatureSpellCooldown(chessPieceInfo.PrimarySpell, time(nullptr) + spell->GetRecoveryTime() / IN_MILLISECONDS);
                    me->_AddCreatureSpellCooldown(chessPieceInfo.SecondarySpell, time(nullptr) + spell->CategoryRecoveryTime / IN_MILLISECONDS);
                    spellPrimaryTimer = spell->GetRecoveryTime();
                    if (spellSecondaryTimer < int32(spell->CategoryRecoveryTime))
                        spellSecondaryTimer = int32(spell->CategoryRecoveryTime);
                }
            }
            void DoCastSecondarySpell()
            {
                if (!me->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_ACTIVE))
                    me->SetFacingTo(me->GetOrientation());

                if (Unit* target = GetTargetByType(chessPieceInfo.SecondaryTarget, chessPieceInfo.SecondaryRadius, chessPieceInfo.SecondaryArc / 180.0f * M_PI))
                {
                    DoCast(target, chessPieceInfo.SecondarySpell);
                    SpellInfo const* spell = sSpellMgr->GetSpellInfo(chessPieceInfo.SecondarySpell);
                    me->AddCreatureSpellCooldown(chessPieceInfo.SecondarySpell);
                    me->_AddCreatureSpellCooldown(chessPieceInfo.SecondarySpell, time(nullptr) + spell->GetRecoveryTime() / IN_MILLISECONDS);
                    me->_AddCreatureSpellCooldown(chessPieceInfo.PrimarySpell, time(nullptr) + spell->CategoryRecoveryTime / IN_MILLISECONDS);
                    spellSecondaryTimer = spell->GetRecoveryTime();
                    if (spellPrimaryTimer < int32(spell->CategoryRecoveryTime))
                        spellPrimaryTimer = int32(spell->CategoryRecoveryTime);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!instance || instance->GetData(TYPE_CHESS) != IN_PROGRESS && instance->GetData(TYPE_CHESS) != SPECIAL)
                    return;

                // issue move command
                if (moveCommandTimer)
                {
                    if (moveCommandTimer <= diff)
                    {
                        // just update facing if some enemy is near
                        if (!GetTargetByType(TARGET_TYPE_ADJACENT, 8.0f, 0))
                        {
                            // the npc doesn't have a 100% chance to move; also there should be some GCD check in core for this part
                            if (roll_chance_i(moveChance))
                            {
                                ResetMoveChance();

                                // Note: in a normal case the target would be chosen using the spells above
                                // However, because the core doesn't support special targeting, we'll provide explicit target
                                //uint32 moveSpell = SPELL_MOVE_1;
                                //switch (me->GetEntry())
                                //{
                                //    case NPC_HUMAN_CONJURER:
                                //    case NPC_ORC_WARLOCK:
                                //    case NPC_HUMAN_CHARGER:
                                //    case NPC_ORC_WOLF:
                                //        moveSpell = SPELL_MOVE_2;
                                //        break;
                                //}
                                //DoCast(me, moveSpell, true);

                                // workaround which provides specific move target
                                if (Unit* target = GetMovementSquare())
                                {
                                    DoCast(target, SPELL_MOVE_GENERIC, true);
                                    if (chessPieceInfo.MoveSound)
                                        me->PlayDistanceSound(chessPieceInfo.MoveSound);
                                }

                                currentOrientation = me->GetOrientation();
                                moveCommandTimer = facingCommandTimer = 12000;
                            }
                            else
                            {
                                moveCommandTimer = 5000;
                                moveChance += chessPieceInfo.MoveChanceIncrement;
                            }
                        }
                        else
                            ResetMoveChance();
                    }
                    else
                        moveCommandTimer -= diff;
                }

                if (facingCommandTimer)
                {
                    if (facingCommandTimer <= diff)
                    {
                        // If there is a unit directly in front of us - just set facing on a small cooldown
                        if (Unit* target = GetTargetByType(TARGET_TYPE_ADJACENT, 8.0f, M_PI / 8))
                            facingCommandTimer = 1000;
                        // Otherwise - try to find a target around us
                        else if (Unit* target = GetTargetByType(TARGET_TYPE_ADJACENT, 8.0f, 0))
                        {
                            DoCast(target, SPELL_CHANGE_FACING);
                            moveCommandTimer = facingCommandTimer = 1000;
                        }
                    }
                    else
                        facingCommandTimer -= diff;
                }

                // issue spell command
                if (isAllowedToCast)
                {
                    spellPrimaryTimer -= std::min(spellPrimaryTimer, diff);
                    if (isAlternatingSpells)
                    {
                        if (!spellPrimaryTimer)
                        {
                            if (isPrimarySpell)
                                DoCastPrimarySpell();
                            else
                                DoCastSecondarySpell();

                            isPrimarySpell = !isPrimarySpell;
                        }
                    }
                    else
                    {
                        spellSecondaryTimer -= std::min(spellSecondaryTimer, diff);
                        if (!spellPrimaryTimer)
                            DoCastPrimarySpell();
                        if (!spellSecondaryTimer)
                            DoCastSecondarySpell();
                    }
                }

                // finish move timer
                if (moveTimer)
                {
                    if (moveTimer <= diff)
                    {
                        if (Creature* square = ObjectAccessor::GetCreature(*me, currentSquareGuid))
                        {
                            DoCast(square, SPELL_MOVE_MARKER, true);
                            me->GetMotionMaster()->MovePoint(1, square->GetPositionX(), square->GetPositionY(), square->GetPositionZ());
                        }
                        moveTimer = 0;
                    }
                    else
                        moveTimer -= diff;
                }

                //if (!UpdateVictim())
                //    return;
            }

        private:
            InstanceScript* instance;

            ObjectGuid currentSquareGuid;

            uint32 moveTimer;
            uint32 moveCommandTimer;
            uint32 facingCommandTimer;
            uint32 spellPrimaryTimer;
            uint32 spellSecondaryTimer;
            uint32 moveChance; // Constantly incrementing chance to move to make sure pieces don't stay in one spot for too long

            bool isAttacked;
            bool isPrimarySpell;
            bool isAlternatingSpells;
            bool isUncharming;
            bool isAllowedToCast;
            float currentOrientation;

            struct ChessPieceInfo
            {
                bool IsKing;
                Team PieceTeam;
                uint32 Faction;

                float MoveRadius;
                uint32 MoveChanceIncrement;
                Emote MoveEmote;
                uint32 MoveSound;

                uint8 PrimaryTarget;
                float PrimaryRadius;
                float PrimaryArc;
                uint32 PrimarySpell;

                uint8 SecondaryTarget;
                float SecondaryRadius;
                float SecondaryArc;
                uint32 SecondarySpell;

                uint32 MeleeSpell;
                uint32 TransformSpell;

                uint32 LoseSoundPlayer1, LoseSoundPlayer2, LoseSoundPlayer3;
                uint32 LoseSoundMedivh1, LoseSoundMedivh2, LoseSoundMedivh3;
            } chessPieceInfo;

            void ResetMoveChance()
            {
                moveChance = 15;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_chess_pieceAI(creature);
        }
};

class spell_game_in_session : public SpellScriptLoader
{
    public:
        spell_game_in_session() : SpellScriptLoader("spell_game_in_session") { }

        class spell_game_in_session_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_game_in_session_AuraScript);

            bool CheckAreaTarget(Unit* target)
            {
                switch (target->GetEntry())
                {
                    case NPC_ORC_GRUNT:
                    case NPC_ORC_WOLF:
                    case NPC_ORC_WARLOCK:
                    case NPC_ORC_NECROLYTE:
                    case NPC_SUMMONED_DAEMON:
                    case NPC_WARCHIEF_BLACKHAND:
                    case NPC_HUMAN_FOOTMAN:
                    case NPC_HUMAN_CHARGER:
                    case NPC_HUMAN_CONJURER:
                    case NPC_HUMAN_CLERIC:
                    case NPC_CONJURED_WATER_ELEMENTAL:
                    case NPC_KING_LLANE:
                        return false;
                    default:
                        if (target->IsControlledByPlayer())
                            if (InstanceScript* instance = target->GetInstanceScript())
                                return instance->GetData(TYPE_CHESS) == IN_PROGRESS || instance->GetData(TYPE_CHESS) == SPECIAL;
                        return false;
                }
            }
            void Register() override
            {
                DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_game_in_session_AuraScript::CheckAreaTarget);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_game_in_session_AuraScript();
        }
};

void AddSC_chess_event()
{
    new npc_echo_of_medivh();
    new npc_chess_piece();
    new spell_game_in_session();
}
