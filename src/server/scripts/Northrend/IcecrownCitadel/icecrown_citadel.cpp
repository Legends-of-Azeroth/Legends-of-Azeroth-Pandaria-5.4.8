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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "SpellAuraEffects.h"
#include "SmartAI.h"
#include "Group.h"
#include "icecrown_citadel.h"
#include "PassiveAI.h"

// Weekly quest support
//* Deprogramming                (DONE)
//* Securing the Ramparts        (DONE)
//* Residue Rendezvous           (DONE)
//* Blood Quickening             (DONE)
//* Respite for a Tormented Soul

enum Texts
{
    // Highlord Tirion Fordring (at Light's Hammer)
    SAY_TIRION_INTRO_1              = 0,
    SAY_TIRION_INTRO_2              = 1,
    SAY_TIRION_INTRO_3              = 2,
    SAY_TIRION_INTRO_4              = 3,
    SAY_TIRION_INTRO_H_5            = 4,
    SAY_TIRION_INTRO_A_5            = 5,

    // The Lich King (at Light's Hammer)
    SAY_LK_INTRO_1                  = 0,
    SAY_LK_INTRO_2                  = 1,
    SAY_LK_INTRO_3                  = 2,
    SAY_LK_INTRO_4                  = 3,
    SAY_LK_INTRO_5                  = 4,

    // Highlord Bolvar Fordragon (at Light's Hammer)
    SAY_BOLVAR_INTRO_1              = 0,

    // High Overlord Saurfang (at Light's Hammer)
    SAY_SAURFANG_INTRO_1            = 15,
    SAY_SAURFANG_INTRO_2            = 16,
    SAY_SAURFANG_INTRO_3            = 17,
    SAY_SAURFANG_INTRO_4            = 18,

    // Muradin Bronzebeard (at Light's Hammer)
    SAY_MURADIN_INTRO_1             = 13,
    SAY_MURADIN_INTRO_2             = 14,
    SAY_MURADIN_INTRO_3             = 15,

    // Deathbound Ward
    SAY_TRAP_ACTIVATE               = 0,

    // Rotting Frost Giant
    EMOTE_DEATH_PLAGUE_WARNING      = 0,

    // Sister Svalna
    SAY_SVALNA_KILL_CAPTAIN         = 1, // happens when she kills a captain
    SAY_SVALNA_KILL                 = 4,
    SAY_SVALNA_CAPTAIN_DEATH        = 5, // happens when a captain resurrected by her dies
    SAY_SVALNA_DEATH                = 6,
    EMOTE_SVALNA_IMPALE             = 7,
    EMOTE_SVALNA_BROKEN_SHIELD      = 8,

    SAY_CROK_INTRO_1                = 0, // Ready your arms, my Argent Brothers. The Vrykul will protect the Frost Queen with their lives.
    SAY_ARNATH_INTRO_2              = 5, // Even dying here beats spending another day collecting reagents for that madman, Finklestein.
    SAY_CROK_INTRO_3                = 1, // Enough idle banter! Our champions have arrived - support them as we push our way through the hall!
    SAY_SVALNA_EVENT_START          = 0, // You may have once fought beside me, Crok, but now you are nothing more than a traitor. Come, your second death approaches!
    SAY_CROK_COMBAT_WP_0            = 2, // Draw them back to us, and we'll assist you.
    SAY_CROK_COMBAT_WP_1            = 3, // Quickly, push on!
    SAY_CROK_FINAL_WP               = 4, // Her reinforcements will arrive shortly, we must bring her down quickly!
    SAY_SVALNA_RESURRECT_CAPTAINS   = 2, // Foolish Crok. You brought my reinforcements with you. Arise, Argent Champions, and serve the Lich King in death!
    SAY_CROK_COMBAT_SVALNA          = 5, // I'll draw her attacks. Return our brothers to their graves, then help me bring her down!
    SAY_SVALNA_AGGRO                = 3, // Come, Scourgebane. I'll show the master which of us is truly worthy of the title of "Champion"!
    SAY_CAPTAIN_DEATH               = 0,
    SAY_CAPTAIN_RESURRECTED         = 1,
    SAY_CAPTAIN_KILL                = 2,
    SAY_CAPTAIN_SECOND_DEATH        = 3,
    SAY_CAPTAIN_SURVIVE_TALK        = 4,
    SAY_CROK_WEAKENING_GAUNTLET     = 6,
    SAY_CROK_WEAKENING_SVALNA       = 7,
    SAY_CROK_VICTORY                = 9,

    SAY_CAPTAIN_TRANSPORTER         = 5,
    SAY_CROK_TRANSPORTER            = 10,

    // Sindragosas Ward
    SAY_WARD_GAUNTLET_STARTED       = 0,
};

enum Spells
{
    // Rotting Frost Giant
    SPELL_DEATH_PLAGUE              = 72879,
    SPELL_DEATH_PLAGUE_AURA         = 72865,
    SPELL_RECENTLY_INFECTED         = 72884,
    SPELL_DEATH_PLAGUE_KILL         = 72867,
    SPELL_STOMP                     = 64639,
    SPELL_STOMP_H                   = 64652,
    SPELL_ARCTIC_BREATH_SCRIPT      = 72847,
    SPELL_ARCTIC_BREATH             = 72848,

    // Frost Freeze Trap
    SPELL_COLDFLAME_JETS            = 70460,

    // Alchemist Adrianna
    SPELL_HARVEST_BLIGHT_SPECIMEN   = 72155,
    SPELL_HARVEST_BLIGHT_SPECIMEN25 = 72162,

    // Crok Scourgebane
    SPELL_ICEBOUND_ARMOR            = 70714,
    SPELL_SCOURGE_STRIKE            = 71488,
    SPELL_DEATH_STRIKE              = 71489,
    SPELL_TELEPORT_VISUAL_ONLY      = 51347,

    // Sister Svalna
    SPELL_CARESS_OF_DEATH           = 70078,
    SPELL_IMPALING_SPEAR_KILL       = 70196,
    SPELL_REVIVE_CHAMPION           = 70053,
    SPELL_UNDEATH                   = 70089,
    SPELL_IMPALING_SPEAR            = 71443,
    SPELL_AETHER_SHIELD             = 71463,
    SPELL_HURL_SPEAR                = 71466,
    SPELL_DIVINE_SURGE              = 71465,

    // Captain Arnath
    SPELL_DOMINATE_MIND             = 14515,
    SPELL_FLASH_HEAL_NORMAL         = 71595,
    SPELL_POWER_WORD_SHIELD_NORMAL  = 71548,
    SPELL_SMITE_NORMAL              = 71546,
    SPELL_FLASH_HEAL_UNDEAD         = 71782,
    SPELL_POWER_WORD_SHIELD_UNDEAD  = 71780,
    SPELL_SMITE_UNDEAD              = 71778,

    // Captain Brandon
    SPELL_CRUSADER_STRIKE           = 71549,
    SPELL_DIVINE_SHIELD             = 71550,
    SPELL_JUDGEMENT_OF_COMMAND      = 71551,
    SPELL_HAMMER_OF_BETRAYAL        = 71784,

    // Captain Grondel
    SPELL_CHARGE                    = 71553,
    SPELL_MORTAL_STRIKE             = 71552,
    SPELL_SUNDER_ARMOR              = 71554,
    SPELL_CONFLAGRATION             = 71785,

    // Captain Rupert
    SPELL_FEL_IRON_BOMB_NORMAL      = 71592,
    SPELL_MACHINE_GUN_NORMAL        = 71594,
    SPELL_ROCKET_LAUNCH_NORMAL      = 71590,
    SPELL_FEL_IRON_BOMB_UNDEAD      = 71787,
    SPELL_MACHINE_GUN_UNDEAD        = 71788,
    SPELL_ROCKET_LAUNCH_UNDEAD      = 71786,

    // Invisible Stalker (Float, Uninteractible, LargeAOI)
    SPELL_SOUL_MISSILE              = 72585,
};

// Helper defines
// Captain Arnath
#define SPELL_FLASH_HEAL        (IsUndead ? SPELL_FLASH_HEAL_UNDEAD : SPELL_FLASH_HEAL_NORMAL)
#define SPELL_POWER_WORD_SHIELD (IsUndead ? SPELL_POWER_WORD_SHIELD_UNDEAD : SPELL_POWER_WORD_SHIELD_NORMAL)
#define SPELL_SMITE             (IsUndead ? SPELL_SMITE_UNDEAD : SPELL_SMITE_NORMAL)

// Captain Rupert
#define SPELL_FEL_IRON_BOMB     (IsUndead ? SPELL_FEL_IRON_BOMB_UNDEAD : SPELL_FEL_IRON_BOMB_NORMAL)
#define SPELL_MACHINE_GUN       (IsUndead ? SPELL_MACHINE_GUN_UNDEAD : SPELL_MACHINE_GUN_NORMAL)
#define SPELL_ROCKET_LAUNCH     (IsUndead ? SPELL_ROCKET_LAUNCH_UNDEAD : SPELL_ROCKET_LAUNCH_NORMAL)

enum EventTypes
{
    // Highlord Tirion Fordring (at Light's Hammer)
    // The Lich King (at Light's Hammer)
    // Highlord Bolvar Fordragon (at Light's Hammer)
    // High Overlord Saurfang (at Light's Hammer)
    // Muradin Bronzebeard (at Light's Hammer)
    EVENT_TIRION_INTRO_2    = 1,
    EVENT_TIRION_INTRO_3    = 2,
    EVENT_TIRION_INTRO_4    = 3,
    EVENT_TIRION_INTRO_5    = 4,
    EVENT_LK_INTRO_1        = 5,
    EVENT_TIRION_INTRO_6    = 6,
    EVENT_LK_INTRO_2        = 7,
    EVENT_LK_INTRO_3        = 8,
    EVENT_LK_INTRO_4        = 9,
    EVENT_BOLVAR_INTRO_1    = 10,
    EVENT_LK_INTRO_5        = 11,
    EVENT_SAURFANG_INTRO_1  = 12,
    EVENT_TIRION_INTRO_H_7  = 13,
    EVENT_SAURFANG_INTRO_2  = 14,
    EVENT_SAURFANG_INTRO_3  = 15,
    EVENT_SAURFANG_INTRO_4  = 16,
    EVENT_SAURFANG_RUN      = 17,
    EVENT_MURADIN_INTRO_1   = 18,
    EVENT_MURADIN_INTRO_2   = 19,
    EVENT_MURADIN_INTRO_3   = 20,
    EVENT_TIRION_INTRO_A_7  = 21,
    EVENT_MURADIN_INTRO_4   = 22,
    EVENT_MURADIN_INTRO_5   = 23,
    EVENT_MURADIN_RUN       = 24,

    // Frost Freeze Trap
    EVENT_ACTIVATE_TRAP     = 28,

    // Crok Scourgebane
    EVENT_SCOURGE_STRIKE                = 29,
    EVENT_DEATH_STRIKE                  = 30,
    EVENT_HEALTH_CHECK                  = 31,
    EVENT_CROK_INTRO_3                  = 32,
    EVENT_START_PATHING                 = 33,

    // Sister Svalna
    EVENT_ARNATH_INTRO_2                = 34,
    EVENT_SVALNA_START                  = 35,
    EVENT_SVALNA_RESURRECT              = 36,
    EVENT_SVALNA_COMBAT                 = 37,
    EVENT_IMPALING_SPEAR                = 38,
    EVENT_AETHER_SHIELD                 = 39,

    // Captain Arnath
    EVENT_ARNATH_FLASH_HEAL             = 40,
    EVENT_ARNATH_PW_SHIELD              = 41,
    EVENT_ARNATH_SMITE                  = 42,
    EVENT_ARNATH_DOMINATE_MIND          = 43,

    // Captain Brandon
    EVENT_BRANDON_CRUSADER_STRIKE       = 44,
    EVENT_BRANDON_DIVINE_SHIELD         = 45,
    EVENT_BRANDON_JUDGEMENT_OF_COMMAND  = 46,
    EVENT_BRANDON_HAMMER_OF_BETRAYAL    = 47,

    // Captain Grondel
    EVENT_GRONDEL_CHARGE_CHECK          = 48,
    EVENT_GRONDEL_MORTAL_STRIKE         = 49,
    EVENT_GRONDEL_SUNDER_ARMOR          = 50,
    EVENT_GRONDEL_CONFLAGRATION         = 51,

    // Captain Rupert
    EVENT_RUPERT_FEL_IRON_BOMB          = 52,
    EVENT_RUPERT_MACHINE_GUN            = 53,
    EVENT_RUPERT_ROCKET_LAUNCH          = 54,

    // Sindragosas Ward
    EVENT_SUB_WAVE_1                    = 55,
    EVENT_SUB_WAVE_2                    = 56,
    EVENT_UPDATE_CHECK                  = 57,

    // Invisible Stalker (Float, Uninteractible, LargeAOI)
    EVENT_SOUL_MISSILE                  = 58,

    // Rotting Frost Giant
    EVENT_CHECK_POSITION,
    EVENT_DEATH_PLAGUE,
    EVENT_STOMP,
    EVENT_ARCTIC_BREATH,
};

enum DataTypesICC
{
    DATA_DAMNED_KILLS       = 1,

    // Rotting Frost Giant
    DATA_RESCHEDULE_DEATH_PLAGUE,
};

enum Actions
{
    // Sister Svalna
    ACTION_KILL_CAPTAIN         = 1,
    ACTION_START_GAUNTLET       = 2,
    ACTION_RESURRECT_CAPTAINS   = 3,
    ACTION_CAPTAIN_DIES         = 4,
    ACTION_RESET_EVENT          = 5,
};

enum EventIds
{
    EVENT_AWAKEN_WARD_1 = 22900,
    EVENT_AWAKEN_WARD_2 = 22907,
    EVENT_AWAKEN_WARD_3 = 22908,
    EVENT_AWAKEN_WARD_4 = 22909,
    EVENT_GEIST_ALARM_1 = 22869,
    EVENT_GEIST_ALARM_2 = 22870,
};

enum MovementPoints
{
    POINT_LAND  = 1,
    POINT_CROK_FINAL = 12345,
};

const Position SvalnaLandPos = {4356.71f, 2484.33f, 358.5f, 1.571f};

const Position SindragosaGauntletSpawn[18] =
{
    { 4130.71f, 2484.10f, 290.000f, M_PI *  0 / 6 },
    { 4137.93f, 2505.52f, 290.000f, M_PI *  1 / 6 },
    { 4160.64f, 2528.13f, 290.000f, M_PI *  2 / 6 },
    { 4180.81f, 2533.88f, 290.000f, M_PI *  3 / 6 },
    { 4200.92f, 2527.18f, 290.000f, M_PI *  4 / 6 },
    { 4222.23f, 2503.56f, 290.000f, M_PI *  5 / 6 },
    { 4229.40f, 2484.63f, 290.000f, M_PI *  6 / 6 },
    { 4222.01f, 2464.93f, 290.000f, M_PI *  7 / 6 },
    { 4201.55f, 2441.03f, 290.000f, M_PI *  8 / 6 },
    { 4181.29f, 2433.38f, 290.000f, M_PI *  9 / 6 },
    { 4161.86f, 2441.94f, 290.000f, M_PI * 10 / 6 },
    { 4138.78f, 2463.95f, 290.000f, M_PI * 11 / 6 },

    { 4177.00f, 2573.75f, 211.033f, M_PI *  3 / 2 },
    { 4181.00f, 2573.75f, 211.033f, M_PI *  3 / 2 },
    { 4185.00f, 2573.75f, 211.033f, M_PI *  3 / 2 },
    { 4177.00f, 2395.75f, 211.033f, M_PI *  1 / 2 },
    { 4181.00f, 2395.75f, 211.033f, M_PI *  1 / 2 },
    { 4185.00f, 2395.75f, 211.033f, M_PI *  1 / 2 },
};

class FrostwingVrykulSearcher
{
    public:
        FrostwingVrykulSearcher(Creature const* source, float range) : _source(source), _range(range) { }

        bool operator()(Unit* unit)
        {
            if (!unit->IsAlive())
                return false;

            switch (unit->GetEntry())
            {
                case NPC_YMIRJAR_BATTLE_MAIDEN:
                case NPC_YMIRJAR_DEATHBRINGER:
                case NPC_YMIRJAR_FROSTBINDER:
                case NPC_YMIRJAR_HUNTRESS:
                case NPC_YMIRJAR_WARLORD:
                    break;
                default:
                    return false;
            }

            if (!unit->IsWithinDist(_source, _range, false))
                return false;

            return true;
        }

    private:
        Creature const* _source;
        float _range;
};

class FrostwingGauntletRespawner
{
    public:
        void operator()(Creature* creature)
        {
            switch (creature->GetOriginalEntry())
            {
                case NPC_YMIRJAR_BATTLE_MAIDEN:
                case NPC_YMIRJAR_DEATHBRINGER:
                case NPC_YMIRJAR_FROSTBINDER:
                case NPC_YMIRJAR_HUNTRESS:
                case NPC_YMIRJAR_WARLORD:
                    break;
                case NPC_CROK_SCOURGEBANE:
                case NPC_CAPTAIN_ARNATH:
                case NPC_CAPTAIN_BRANDON:
                case NPC_CAPTAIN_GRONDEL:
                case NPC_CAPTAIN_RUPERT:
                    creature->AI()->DoAction(ACTION_RESET_EVENT);
                    break;
                case NPC_SISTER_SVALNA:
                    creature->AI()->DoAction(ACTION_RESET_EVENT);
                    // return, this creature is never dead if event is reset
                    return;
                default:
                    return;
            }

            bool alive = creature->IsAlive();

            if (FirstSpawn)
            {
                if (!alive)
                {
                    creature->Respawn(true);
                    creature->AI()->InitializeAI();
                }
                return;
            }

            uint32 corpseDelay = creature->GetCorpseDelay();
            uint32 respawnDelay = creature->GetRespawnDelay();
            creature->SetCorpseDelay(1);
            creature->SetRespawnDelay(2);

            if (CreatureData const* data = creature->GetCreatureData())
                creature->SetPosition(data->posX, data->posY, data->posZ, data->orientation);
            creature->DespawnOrUnsummon();

            creature->SetCorpseDelay(corpseDelay);
            creature->SetRespawnDelay(respawnDelay);

            if (!alive)
                creature->Respawn(true);
        }

        bool FirstSpawn = false;
};

// at Light's Hammer
class npc_highlord_tirion_fordring_lh : public CreatureScript
{
    public:
        npc_highlord_tirion_fordring_lh() : CreatureScript("npc_highlord_tirion_fordring_lh") { }

        struct npc_highlord_tirion_fordringAI : public ScriptedAI
        {
            npc_highlord_tirion_fordringAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

            void Reset() override
            {
                _events.Reset();
                _theLichKing = ObjectGuid::Empty;
                _bolvarFordragon = ObjectGuid::Empty;
                _factionNPC = ObjectGuid::Empty;
                _damnedKills = 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (_instance->GetData(DATA_LIGHTS_HAMMER_NPCS_LEFT))
                    return;

                if (type == DATA_DAMNED_KILLS && data == 1)
                {
                    if (++_damnedKills == 2)
                    {
                        if (Creature* theLichKing = me->FindNearestCreature(NPC_THE_LICH_KING_LH, 150.0f))
                        {
                            if (Creature* bolvarFordragon = me->FindNearestCreature(NPC_HIGHLORD_BOLVAR_FORDRAGON_LH, 150.0f))
                            {
                                if (Creature* factionNPC = me->FindNearestCreature(_instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE ? NPC_SE_HIGH_OVERLORD_SAURFANG : NPC_SE_MURADIN_BRONZEBEARD, 50.0f))
                                {
                                    me->setActive(true);
                                    _theLichKing = theLichKing->GetGUID();
                                    theLichKing->setActive(true);
                                    _bolvarFordragon = bolvarFordragon->GetGUID();
                                    bolvarFordragon->setActive(true);
                                    _factionNPC = factionNPC->GetGUID();
                                    factionNPC->setActive(true);
                                }
                            }
                        }

                        if (!_bolvarFordragon || !_theLichKing || !_factionNPC)
                            return;

                        Talk(SAY_TIRION_INTRO_1);
                        _events.ScheduleEvent(EVENT_TIRION_INTRO_2, 4000);
                        _events.ScheduleEvent(EVENT_TIRION_INTRO_3, 14000);
                        _events.ScheduleEvent(EVENT_TIRION_INTRO_4, 18000);
                        _events.ScheduleEvent(EVENT_TIRION_INTRO_5, 31000);
                        _events.ScheduleEvent(EVENT_LK_INTRO_1, 35000);
                        _events.ScheduleEvent(EVENT_TIRION_INTRO_6, 51000);
                        _events.ScheduleEvent(EVENT_LK_INTRO_2, 58000);
                        _events.ScheduleEvent(EVENT_LK_INTRO_3, 74000);
                        _events.ScheduleEvent(EVENT_LK_INTRO_4, 86000);
                        _events.ScheduleEvent(EVENT_BOLVAR_INTRO_1, 107000);
                        _events.ScheduleEvent(EVENT_LK_INTRO_5, 113000);

                        if (_instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
                        {
                            _events.ScheduleEvent(EVENT_SAURFANG_INTRO_1, 120000);
                            _events.ScheduleEvent(EVENT_TIRION_INTRO_H_7, 129000);
                            _events.ScheduleEvent(EVENT_SAURFANG_INTRO_2, 139000);
                            _events.ScheduleEvent(EVENT_SAURFANG_INTRO_3, 150000);
                            _events.ScheduleEvent(EVENT_SAURFANG_INTRO_4, 162000);
                            _events.ScheduleEvent(EVENT_SAURFANG_RUN, 170000);
                        }
                        else
                        {
                            _events.ScheduleEvent(EVENT_MURADIN_INTRO_1, 120000);
                            _events.ScheduleEvent(EVENT_MURADIN_INTRO_2, 124000);
                            _events.ScheduleEvent(EVENT_MURADIN_INTRO_3, 127000);
                            _events.ScheduleEvent(EVENT_TIRION_INTRO_A_7, 136000);
                            _events.ScheduleEvent(EVENT_MURADIN_INTRO_4, 144000);
                            _events.ScheduleEvent(EVENT_MURADIN_INTRO_5, 151000);
                            _events.ScheduleEvent(EVENT_MURADIN_RUN, 157000);
                        }
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (_damnedKills != 2)
                    return;

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TIRION_INTRO_2:
                            me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                            break;
                        case EVENT_TIRION_INTRO_3:
                            Talk(SAY_TIRION_INTRO_2);
                            break;
                        case EVENT_TIRION_INTRO_4:
                            me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                            break;
                        case EVENT_TIRION_INTRO_5:
                            Talk(SAY_TIRION_INTRO_3);
                            break;
                        case EVENT_LK_INTRO_1:
                            me->HandleEmoteCommand(EMOTE_ONESHOT_POINT_NO_SHEATHE);
                            if (Creature* theLichKing = ObjectAccessor::GetCreature(*me, _theLichKing))
                                theLichKing->AI()->Talk(SAY_LK_INTRO_1);
                            break;
                        case EVENT_TIRION_INTRO_6:
                            Talk(SAY_TIRION_INTRO_4);
                            break;
                        case EVENT_LK_INTRO_2:
                            if (Creature* theLichKing = ObjectAccessor::GetCreature(*me, _theLichKing))
                                theLichKing->AI()->Talk(SAY_LK_INTRO_2);
                            break;
                        case EVENT_LK_INTRO_3:
                            if (Creature* theLichKing = ObjectAccessor::GetCreature(*me, _theLichKing))
                                theLichKing->AI()->Talk(SAY_LK_INTRO_3);
                            break;
                        case EVENT_LK_INTRO_4:
                            if (Creature* theLichKing = ObjectAccessor::GetCreature(*me, _theLichKing))
                                theLichKing->AI()->Talk(SAY_LK_INTRO_4);
                            break;
                        case EVENT_BOLVAR_INTRO_1:
                            if (Creature* bolvarFordragon = ObjectAccessor::GetCreature(*me, _bolvarFordragon))
                            {
                                bolvarFordragon->AI()->Talk(SAY_BOLVAR_INTRO_1);
                                bolvarFordragon->setActive(false);
                            }
                            break;
                        case EVENT_LK_INTRO_5:
                            if (Creature* theLichKing = ObjectAccessor::GetCreature(*me, _theLichKing))
                            {
                                theLichKing->AI()->Talk(SAY_LK_INTRO_5);
                                theLichKing->setActive(false);
                            }
                            break;
                        case EVENT_SAURFANG_INTRO_1:
                            if (Creature* saurfang = ObjectAccessor::GetCreature(*me, _factionNPC))
                                saurfang->AI()->Talk(SAY_SAURFANG_INTRO_1);
                            break;
                        case EVENT_TIRION_INTRO_H_7:
                            Talk(SAY_TIRION_INTRO_H_5);
                            break;
                        case EVENT_SAURFANG_INTRO_2:
                            if (Creature* saurfang = ObjectAccessor::GetCreature(*me, _factionNPC))
                                saurfang->AI()->Talk(SAY_SAURFANG_INTRO_2);
                            break;
                        case EVENT_SAURFANG_INTRO_3:
                            if (Creature* saurfang = ObjectAccessor::GetCreature(*me, _factionNPC))
                                saurfang->AI()->Talk(SAY_SAURFANG_INTRO_3);
                            break;
                        case EVENT_SAURFANG_INTRO_4:
                            if (Creature* saurfang = ObjectAccessor::GetCreature(*me, _factionNPC))
                                saurfang->AI()->Talk(SAY_SAURFANG_INTRO_4);
                            break;
                        case EVENT_MURADIN_RUN:
                        case EVENT_SAURFANG_RUN:
                            if (Creature* factionNPC = ObjectAccessor::GetCreature(*me, _factionNPC))
                                factionNPC->GetMotionMaster()->MovePath(factionNPC->GetDBTableGUIDLow()*10, false);
                            me->setActive(false);
                            _damnedKills = 3;
                            _instance->SetData(DATA_LIGHTS_HAMMER_NPCS_LEFT, true);
                            break;
                        case EVENT_MURADIN_INTRO_1:
                            if (Creature* muradin = ObjectAccessor::GetCreature(*me, _factionNPC))
                                muradin->AI()->Talk(SAY_MURADIN_INTRO_1);
                            break;
                        case EVENT_MURADIN_INTRO_2:
                            if (Creature* muradin = ObjectAccessor::GetCreature(*me, _factionNPC))
                                muradin->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                            break;
                        case EVENT_MURADIN_INTRO_3:
                            if (Creature* muradin = ObjectAccessor::GetCreature(*me, _factionNPC))
                                muradin->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                            break;
                        case EVENT_TIRION_INTRO_A_7:
                            Talk(SAY_TIRION_INTRO_A_5);
                            break;
                        case EVENT_MURADIN_INTRO_4:
                            if (Creature* muradin = ObjectAccessor::GetCreature(*me, _factionNPC))
                                muradin->AI()->Talk(SAY_MURADIN_INTRO_2);
                            break;
                        case EVENT_MURADIN_INTRO_5:
                            if (Creature* muradin = ObjectAccessor::GetCreature(*me, _factionNPC))
                                muradin->AI()->Talk(SAY_MURADIN_INTRO_3);
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap _events;
            InstanceScript* const _instance;
            ObjectGuid _theLichKing;
            ObjectGuid _bolvarFordragon;
            ObjectGuid _factionNPC;
            uint16 _damnedKills;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_highlord_tirion_fordringAI>(creature);
        }
};

class npc_rotting_frost_giant : public CreatureScript
{
    public:
        npc_rotting_frost_giant() : CreatureScript("npc_rotting_frost_giant") { }

        struct npc_rotting_frost_giantAI : public ScriptedAI
        {
            npc_rotting_frost_giantAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_CHECK_POSITION, 5000);
                _events.ScheduleEvent(EVENT_DEATH_PLAGUE, 8000);
                _events.ScheduleEvent(EVENT_STOMP, urand(3000, 6000));
                _events.ScheduleEvent(EVENT_ARCTIC_BREATH, 12000);

                if (InstanceScript* instance = me->GetInstanceScript())
                    instance->SetData(DATA_ROTTING_FROST_GIANT_STATE, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->setActive(true);
                DoZoneInCombat();

                if (InstanceScript* instance = me->GetInstanceScript())
                    instance->SetData(DATA_ROTTING_FROST_GIANT_STATE, IN_PROGRESS);
            }

            void JustDied(Unit* killer) override
            {
                me->setActive(false);
                _events.Reset();

                auto removePlague = [this]()
                {
                    for (auto&& ref : me->GetMap()->GetPlayers())
                    {
                        if (Player* player = ref.GetSource())
                        {
                            if (player->HasAura(SPELL_DEATH_PLAGUE_AURA))
                            {
                                player->RemoveAurasDueToSpell(SPELL_DEATH_PLAGUE_AURA);
                                player->CastSpell(player, SPELL_RECENTLY_INFECTED, true);
                            }
                        }
                    }
                };
                removePlague();
                me->m_Events.Schedule(5000, removePlague);

                if (InstanceScript* instance = me->GetInstanceScript())
                    instance->SetData(DATA_ROTTING_FROST_GIANT_STATE, DONE);
            }

            void JustReachedHome() override
            {
                me->setActive(false);
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_RESCHEDULE_DEATH_PLAGUE)
                    _events.RescheduleEvent(EVENT_DEATH_PLAGUE, 5000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_POSITION:
                            if (me->GetPositionZ() < 195.0f)
                                if (me->GetVictim())
                                    me->NearTeleportTo(me->GetVictim()->GetPositionX(), me->GetVictim()->GetPositionY(), me->GetVictim()->GetPositionZ() + 1.0f, me->GetOrientation());
                            _events.ScheduleEvent(EVENT_CHECK_POSITION, 5000);
                            break;
                        case EVENT_DEATH_PLAGUE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [this](Unit* u) { return me->IsWithinCombatRange(u, 100.0f) && !u->HasAura(SPELL_RECENTLY_INFECTED) && !u->HasAura(SPELL_DEATH_PLAGUE_AURA); }))
                            {
                                DoCast(target, SPELL_DEATH_PLAGUE);
                                _events.RescheduleEvent(EVENT_DEATH_PLAGUE, 20000);
                            }
                            else
                                _events.RescheduleEvent(EVENT_DEATH_PLAGUE, 1000);
                            break;
                        case EVENT_STOMP:
                            DoCastVictim(RAID_MODE(SPELL_STOMP, SPELL_STOMP_H, SPELL_STOMP, SPELL_STOMP_H));
                            _events.ScheduleEvent(EVENT_STOMP, urand(16000, 19000));
                            break;
                        case EVENT_ARCTIC_BREATH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            {
                                DoCast(target, SPELL_ARCTIC_BREATH_SCRIPT);
                                DoCast(target, SPELL_ARCTIC_BREATH);
                            }
                            _events.ScheduleEvent(EVENT_ARCTIC_BREATH, 25000);
                            break;
                        default:
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        break;
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_rotting_frost_giantAI>(creature);
        }
};

Creature* FindRottingFrostGiant(Unit* source)
{
    if (!source)
        return nullptr;
    if (Creature* giant = source->FindNearestCreature(NPC_ROTTING_FROST_GIANT_10, 500.0f))
        return giant;
    if (Creature* giant = source->FindNearestCreature(NPC_ROTTING_FROST_GIANT_25, 500.0f))
        return giant;
    return nullptr;
}

class spell_frost_giant_death_plague : public SpellScriptLoader
{
    public:
        spell_frost_giant_death_plague() : SpellScriptLoader("spell_frost_giant_death_plague") { }

        class spell_frost_giant_death_plague_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_frost_giant_death_plague_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_DEATH_PLAGUE_AURA));
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();
                if (!caster || !target)
                    return;

                if (Creature* giant = FindRottingFrostGiant(caster))
                    caster->CastSpell(target, SPELL_DEATH_PLAGUE, true, nullptr, nullptr, giant->GetGUID());

                didHit = true;
            }

            void HandleAfterCast()
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                if (didHit)
                {
                    caster->CastSpell(caster, SPELL_RECENTLY_INFECTED, true);
                    if (Creature* giant = FindRottingFrostGiant(caster))
                        giant->AI()->SetData(DATA_RESCHEDULE_DEATH_PLAGUE, 5000);
                }
                else
                {
                    caster->CastSpell(caster, SPELL_DEATH_PLAGUE_KILL, true);
                    if (Creature* giant = FindRottingFrostGiant(caster))
                        giant->AI()->SetData(DATA_RESCHEDULE_DEATH_PLAGUE, 1000);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_frost_giant_death_plague_SpellScript::FilterTargets, EFFECT_ALL, TARGET_UNIT_SRC_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_frost_giant_death_plague_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
                AfterCast += SpellCastFn(spell_frost_giant_death_plague_SpellScript::HandleAfterCast);
            }

        private:
            bool didHit = false;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_frost_giant_death_plague_SpellScript();
        }
};

class spell_frost_giant_stomp : public SpellScriptLoader
{
    public:
        spell_frost_giant_stomp() : SpellScriptLoader("spell_frost_giant_stomp") { }

        class spell_frost_giant_stomp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_frost_giant_stomp_SpellScript);

            void SelectTarget(SpellDestination& dest)
            {
                if (Unit* caster = GetCaster())
                {
                    Position pos = caster->GetFirstCollisionPosition(2.0f, 0.0f);
                    dest._position = WorldLocation(caster->GetMapId(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
                }
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_frost_giant_stomp_SpellScript::SelectTarget, EFFECT_0, TARGET_DEST_CASTER_FRONT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_frost_giant_stomp_SpellScript();
        }
};

class npc_frost_freeze_trap : public CreatureScript
{
    public:
        npc_frost_freeze_trap() : CreatureScript("npc_frost_freeze_trap") { }

        struct npc_frost_freeze_trapAI: public ScriptedAI
        {
            npc_frost_freeze_trapAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case 0:
                        _events.CancelEvent(EVENT_ACTIVATE_TRAP);
                        me->CastStop();
                        me->RemoveAurasDueToSpell(SPELL_COLDFLAME_JETS);
                        break;
                    case 1000:
                    case 11000:
                        _events.ScheduleEvent(EVENT_ACTIVATE_TRAP, uint32(action));
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                _events.Update(diff);

                if (_events.ExecuteEvent() == EVENT_ACTIVATE_TRAP)
                {
                    DoCast(me, SPELL_COLDFLAME_JETS);
                    _events.ScheduleEvent(EVENT_ACTIVATE_TRAP, 22000);
                }
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_frost_freeze_trapAI>(creature);
        }
};

class npc_alchemist_adrianna : public CreatureScript
{
    public:
        npc_alchemist_adrianna() : CreatureScript("npc_alchemist_adrianna") { }

        struct npc_alchemist_adriannaAI : public ScriptedAI
        {
            npc_alchemist_adriannaAI(Creature* creature) : ScriptedAI(creature) { }

            bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
            {
                if (sender == 11034 && action == 0)
                {
                    player->CLOSE_GOSSIP_MENU();
                    if (!me->FindCurrentSpellBySpellId(SPELL_HARVEST_BLIGHT_SPECIMEN) && !me->FindCurrentSpellBySpellId(SPELL_HARVEST_BLIGHT_SPECIMEN25))
                    {
                        if (player->HasAura(SPELL_ORANGE_BLIGHT_RESIDUE) && player->HasAura(SPELL_GREEN_BLIGHT_RESIDUE))
                        {
                            DoCastAOE(SPELL_HARVEST_BLIGHT_SPECIMEN);
                            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                            me->m_Events.Schedule(4000, [this]() { me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER); });
                        }
                    }
                }
                return true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_alchemist_adriannaAI>(creature);
        }
};

class boss_sister_svalna : public CreatureScript
{
    public:
        boss_sister_svalna() : CreatureScript("boss_sister_svalna") { }

        struct boss_sister_svalnaAI : public BossAI
        {
            boss_sister_svalnaAI(Creature* creature) : BossAI(creature, DATA_SISTER_SVALNA), _isEventInProgress(false) { }

            void InitializeAI() override
            {
                if (!me->isDead())
                    Reset();

                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                _Reset();
                me->SetReactState(REACT_DEFENSIVE);
                _isEventInProgress = false;
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_SVALNA_DEATH);

                uint64 delay = 1;
                for (uint32 i = 0; i < 4; ++i)
                {
                    if (Creature* crusader = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_CAPTAIN_ARNATH + i)))
                    {
                        if (crusader->IsAlive() && crusader->GetEntry() == crusader->GetCreatureData()->id)
                        {
                            crusader->m_Events.Schedule(delay, [crusader]() { crusader->AI()->Talk(SAY_CAPTAIN_SURVIVE_TALK); });
                            delay += 6000;
                        }
                    }
                }

                if (Creature* crok = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_CROK_SCOURGEBANE)))
                    crok->m_Events.Schedule(delay + 3000, [crok]() { crok->GetMotionMaster()->MovePoint(POINT_CROK_FINAL, 4335.371094f, 2484.422363f, 358.441711f); });
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                if (Creature* crok = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_CROK_SCOURGEBANE)))
                    crok->AI()->Talk(SAY_CROK_COMBAT_SVALNA);
                events.ScheduleEvent(EVENT_SVALNA_COMBAT, 1);
                events.ScheduleEvent(EVENT_IMPALING_SPEAR, urand(40000, 50000));
                events.ScheduleEvent(EVENT_AETHER_SHIELD, urand(100000, 110000));
                DoCast(SPELL_DIVINE_SURGE);
            }

            void KilledUnit(Unit* victim) override
            {
                switch (victim->GetTypeId())
                {
                    case TYPEID_PLAYER:
                        Talk(SAY_SVALNA_KILL);
                        break;
                    case TYPEID_UNIT:
                        switch (victim->GetEntry())
                        {
                            case NPC_CAPTAIN_ARNATH:
                            case NPC_CAPTAIN_BRANDON:
                            case NPC_CAPTAIN_GRONDEL:
                            case NPC_CAPTAIN_RUPERT:
                                Talk(SAY_SVALNA_KILL_CAPTAIN);
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                me->SetReactState(REACT_PASSIVE);
                me->SetFlying(false);
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_KILL_CAPTAIN:
                        me->CastCustomSpell(SPELL_CARESS_OF_DEATH, SPELLVALUE_MAX_TARGETS, 1, me, true);
                        break;
                    case ACTION_START_GAUNTLET:
                        me->setActive(true);
                        _isEventInProgress = true;
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
                        events.ScheduleEvent(EVENT_SVALNA_START, 25000);
                        break;
                    case ACTION_RESURRECT_CAPTAINS:
                        events.ScheduleEvent(EVENT_SVALNA_RESURRECT, 7000);
                        break;
                    case ACTION_CAPTAIN_DIES:
                        Talk(SAY_SVALNA_CAPTAIN_DEATH);
                        break;
                    case ACTION_RESET_EVENT:
                        me->setActive(false);
                        Reset();
                        break;
                    default:
                        break;
                }
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_HURL_SPEAR && me->HasAura(SPELL_AETHER_SHIELD))
                {
                    me->RemoveAurasDueToSpell(SPELL_AETHER_SHIELD);
                    Talk(EMOTE_SVALNA_BROKEN_SHIELD, caster);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE || pointId != POINT_LAND)
                    return;

                _isEventInProgress = false;
                me->setActive(false);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlying(false);
                DoZoneInCombat(me, 150.0f);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_IMPALING_SPEAR_KILL:
                        me->Kill(target);
                        break;
                    case SPELL_IMPALING_SPEAR:
                        if (TempSummon* summon = target->SummonCreature(NPC_IMPALING_SPEAR, *target))
                        {
                            Talk(EMOTE_SVALNA_IMPALE, target);
                            summon->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, target, false);
                            summon->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1 | 0x4000);
                            // give item
                            //ItemPosCountVec dest;
                            //uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 50307, 1);
                            //if (msg == EQUIP_ERR_OK)
                            //{
                            //    Item* item = player->StoreNewItem(dest, 50307, true);
                            //    player->SendNewItem(item, 1, true, false);
                            //}
                        }
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() && !_isEventInProgress)
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SVALNA_START:
                            Talk(SAY_SVALNA_EVENT_START);
                            break;
                        case EVENT_SVALNA_RESURRECT:
                            Talk(SAY_SVALNA_RESURRECT_CAPTAINS);
                            me->CastSpell(me, SPELL_REVIVE_CHAMPION, false);
                            break;
                        case EVENT_SVALNA_COMBAT:
                            me->SetReactState(REACT_DEFENSIVE);
                            Talk(SAY_SVALNA_AGGRO);
                            break;
                        case EVENT_IMPALING_SPEAR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true, -SPELL_IMPALING_SPEAR))
                            {
                                DoCast(me, SPELL_AETHER_SHIELD);
                                DoCast(target, SPELL_IMPALING_SPEAR);
                            }
                            events.ScheduleEvent(EVENT_IMPALING_SPEAR, urand(20000, 25000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            bool _isEventInProgress;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<boss_sister_svalnaAI>(creature);
        }
};

class npc_crok_scourgebane : public CreatureScript
{
    public:
        npc_crok_scourgebane() : CreatureScript("npc_crok_scourgebane") { }

        struct npc_crok_scourgebaneAI : public npc_escortAI
        {
            npc_crok_scourgebaneAI(Creature* creature) : npc_escortAI(creature),
                _instance(creature->GetInstanceScript()), _respawnTime(creature->GetRespawnDelay()),
                _corpseDelay(creature->GetCorpseDelay())
            {
                SetDespawnAtEnd(false);
                SetDespawnAtFar(false);
                _isEventActive = false;
                _isEventDone = _instance->GetBossState(DATA_SISTER_SVALNA) == DONE;
                _didUnderTenPercentText = false;

                if (!_isEventDone && me->GetDBTableGUIDLow()) // Skip Scourge Transport cutscene spawn
                {
                    me->m_Events.Schedule(1000, [this]()
                    {
                        FrostwingGauntletRespawner respawner;
                        respawner.FirstSpawn = true;
                        Trinity::CreatureWorker<FrostwingGauntletRespawner> worker(me, respawner);
                        me->VisitNearbyGridObject(333.0f, worker);
                    });
                }
            }

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_SCOURGE_STRIKE, urand(7500, 12500));
                _events.ScheduleEvent(EVENT_DEATH_STRIKE, urand(25000, 30000));
                me->SetReactState(REACT_DEFENSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                _isEventActive = false;

                if (_instance)
                    _isEventDone = _instance->GetBossState(DATA_SISTER_SVALNA) == DONE;

                _didUnderTenPercentText = false;
                _wipeCheckTimer = 1000;
                _aliveTrash.clear();
                _currentWPid = 0;
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_START_GAUNTLET)
                {
                    if (_isEventDone || !me->IsAlive())
                        return;

                    _isEventActive = true;
                    _isEventDone = true;
                    // Load Grid with Sister Svalna
                    me->GetMap()->LoadGrid(4356.71f, 2484.33f);
                    if (Creature* svalna = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_SISTER_SVALNA)))
                        svalna->AI()->DoAction(ACTION_START_GAUNTLET);
                    Talk(SAY_CROK_INTRO_1);
                    _events.ScheduleEvent(EVENT_ARNATH_INTRO_2, 7000);
                    _events.ScheduleEvent(EVENT_CROK_INTRO_3, 14000);
                    _events.ScheduleEvent(EVENT_START_PATHING, 37000);
                    me->setActive(true);
                    for (uint32 i = 0; i < 4; ++i)
                        if (Creature* crusader = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_CAPTAIN_ARNATH + i)))
                            crusader->AI()->DoAction(ACTION_START_GAUNTLET);

                    std::list<Creature*> ymirjar;
                    GetCreatureListWithEntryInGrid(ymirjar, me, NPC_YMIRJAR_BATTLE_MAIDEN, 200.0f);
                    GetCreatureListWithEntryInGrid(ymirjar, me, NPC_YMIRJAR_HUNTRESS, 200.0f);
                    GetCreatureListWithEntryInGrid(ymirjar, me, NPC_YMIRJAR_WARLORD, 200.0f);
                    for (auto&& creature : ymirjar)
                        creature->AI()->DoAction(1); // ACTION_APPROACH
                }
                else if (action == ACTION_RESET_EVENT)
                {
                    _isEventActive = false;
                    _isEventDone = _instance->GetBossState(DATA_SISTER_SVALNA) == DONE;
                    me->setActive(false);
                    _aliveTrash.clear();
                    _currentWPid = 0;
                }
            }

            void SetGUID(ObjectGuid guid, int32 type/* = 0*/) override
            {
                if (type == ACTION_VRYKUL_DEATH)
                {
                    _aliveTrash.erase(guid);
                    if (_aliveTrash.empty())
                    {
                        SetEscortPaused(false);
                        if (_currentWPid == 4 && _isEventActive)
                        {
                            _isEventActive = false;
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            me->setActive(false);
                            Talk(SAY_CROK_FINAL_WP);
                            if (Creature* svalna = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_SISTER_SVALNA)))
                                svalna->AI()->DoAction(ACTION_RESURRECT_CAPTAINS);
                        }
                    }
                }
            }

            void WaypointReached(uint32 waypointId) override
            {
                switch (waypointId)
                {
                    // pause pathing until trash pack is cleared
                    case 0:
                        Talk(SAY_CROK_COMBAT_WP_0);
                        if (!_aliveTrash.empty())
                            SetEscortPaused(true);
                        break;
                    case 1:
                        Talk(SAY_CROK_COMBAT_WP_1);
                        if (!_aliveTrash.empty())
                            SetEscortPaused(true);
                        break;
                    case 4:
                        if (_aliveTrash.empty() && _isEventActive)
                        {
                            _isEventActive = false;
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            me->setActive(false);
                            Talk(SAY_CROK_FINAL_WP);
                            if (Creature* svalna = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_SISTER_SVALNA)))
                                svalna->AI()->DoAction(ACTION_RESURRECT_CAPTAINS);
                        }
                        break;
                    default:
                        break;
                }
            }

            void WaypointStart(uint32 waypointId) override
            {
                _currentWPid = waypointId;
                switch (waypointId)
                {
                    case 0:
                    case 1:
                    case 4:
                    {
                        // get spawns by home position
                        float minY = 2600.0f;
                        float maxY = 2650.0f;
                        if (waypointId == 1)
                        {
                            minY -= 50.0f;
                            maxY -= 50.0f;
                            // at waypoints 1 and 2 she kills one captain
                            if (Creature* svalna = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_SISTER_SVALNA)))
                                svalna->AI()->DoAction(ACTION_KILL_CAPTAIN);
                        }
                        else if (waypointId == 4)
                        {
                            minY -= 100.0f;
                            maxY -= 100.0f;
                        }

                        // get all nearby vrykul
                        std::list<Creature*> temp;
                        FrostwingVrykulSearcher check(me, 80.0f);
                        Trinity::CreatureListSearcher<FrostwingVrykulSearcher> searcher(me, temp, check);
                        me->VisitNearbyGridObject(80.0f, searcher);

                        _aliveTrash.clear();
                        for (std::list<Creature*>::iterator itr = temp.begin(); itr != temp.end(); ++itr)
                            if ((*itr)->GetHomePosition().GetPositionY() < maxY && (*itr)->GetHomePosition().GetPositionY() > minY)
                                _aliveTrash.insert((*itr)->GetGUID());
                        break;
                    }
                    // at waypoints 1 and 2 she kills one captain
                    case 2:
                        if (Creature* svalna = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_SISTER_SVALNA)))
                            svalna->AI()->DoAction(ACTION_KILL_CAPTAIN);
                        break;
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                // check wipe
                if (!_wipeCheckTimer)
                {
                    _wipeCheckTimer = 1000;
                    Player* player = nullptr;
                    Trinity::AnyPlayerInObjectRangeCheck check(me, 60.0f);
                    Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, player, check);
                    me->VisitNearbyWorldObject(60.0f, searcher);
                    // wipe
                    if (!player)
                    {
                        damage *= 100;
                        if (damage >= me->GetHealth())
                        {
                            FrostwingGauntletRespawner respawner;
                            Trinity::CreatureWorker<FrostwingGauntletRespawner> worker(me, respawner);
                            me->VisitNearbyGridObject(333.0f, worker);
                        }
                        return;
                    }
                }

                if (HealthBelowPct(10) || damage >= me->GetHealth())
                {
                    if (!_didUnderTenPercentText)
                    {
                        _didUnderTenPercentText = true;
                        if (_isEventActive)
                            Talk(SAY_CROK_WEAKENING_GAUNTLET);
                        else
                            Talk(SAY_CROK_WEAKENING_SVALNA);
                    }

                    damage = 0;
                    DoCast(me, SPELL_ICEBOUND_ARMOR);
                    _events.ScheduleEvent(EVENT_HEALTH_CHECK, 1000);
                }
            }

            void UpdateEscortAI(uint32 diff) override
            {
                if (_wipeCheckTimer <= diff)
                    _wipeCheckTimer = 0;
                else
                    _wipeCheckTimer -= diff;

                if (!UpdateVictim() && !_isEventActive)
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARNATH_INTRO_2:
                            if (Creature* arnath = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_CAPTAIN_ARNATH)))
                                arnath->AI()->Talk(SAY_ARNATH_INTRO_2);
                            break;
                        case EVENT_CROK_INTRO_3:
                            Talk(SAY_CROK_INTRO_3);
                            break;
                        case EVENT_START_PATHING:
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            Start(true, true);
                            break;
                        case EVENT_SCOURGE_STRIKE:
                            DoCastVictim(SPELL_SCOURGE_STRIKE);
                            _events.ScheduleEvent(EVENT_SCOURGE_STRIKE, urand(10000, 14000));
                            break;
                        case EVENT_DEATH_STRIKE:
                            if (HealthBelowPct(20))
                                DoCastVictim(SPELL_DEATH_STRIKE);
                            _events.ScheduleEvent(EVENT_DEATH_STRIKE, urand(5000, 10000));
                            break;
                        case EVENT_HEALTH_CHECK:
                            if (HealthAbovePct(15))
                            {
                                me->RemoveAurasDueToSpell(SPELL_ICEBOUND_ARMOR);
                                _didUnderTenPercentText = false;
                            }
                            else
                            {
                                me->DealHeal(me, me->CountPctFromMaxHealth(5));
                                _events.ScheduleEvent(EVENT_HEALTH_CHECK, 1000);
                            }
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE && pointId == POINT_CROK_FINAL)
                {
                    Talk(SAY_CROK_VICTORY);
                    for (uint32 i = 0; i < 4; ++i)
                        if (Creature* crusader = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_CAPTAIN_ARNATH + i)))
                            crusader->DespawnOrUnsummon(30000);
                    me->DespawnOrUnsummon(30000);
                }
                else
                    npc_escortAI::MovementInform(type, pointId);
            }

            bool CanAIAttack(Unit const* target) const override
            {
                // do not see targets inside Frostwing Halls when we are not there
                return (me->GetPositionY() > 2660.0f) == (target->GetPositionY() > 2660.0f);
            }

        private:
            EventMap _events;
            std::set<uint64> _aliveTrash;
            InstanceScript* _instance;
            uint32 _currentWPid;
            uint32 _wipeCheckTimer;
            uint32 const _respawnTime;
            uint32 const _corpseDelay;
            bool _isEventActive;
            bool _isEventDone;
            bool _didUnderTenPercentText;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_crok_scourgebaneAI>(creature);
        }
};

struct npc_argent_captainAI : public ScriptedAI
{
    public:
        npc_argent_captainAI(Creature* creature) : ScriptedAI(creature), Instance(creature->GetInstanceScript()), _firstDeath(true)
        {
            FollowAngle = PET_FOLLOW_ANGLE;
            FollowDist = PET_FOLLOW_DIST;
            IsUndead = false;
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (_firstDeath)
            {
                _firstDeath = false;
                Talk(SAY_CAPTAIN_DEATH);
            }
            else
                Talk(SAY_CAPTAIN_SECOND_DEATH);
        }

        void KilledUnit(Unit* victim) override
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
                Talk(SAY_CAPTAIN_KILL);
        }

        void DoAction(int32 action) override
        {
            if (action == ACTION_START_GAUNTLET)
            {
                if (Creature* crok = ObjectAccessor::GetCreature(*me, Instance->GetGuidData(DATA_CROK_SCOURGEBANE)))
                {
                    me->SetReactState(REACT_DEFENSIVE);
                    FollowAngle = me->GetAngle(crok) + me->GetOrientation();
                    FollowDist = me->GetDistance2d(crok);
                    me->GetMotionMaster()->MoveFollow(crok, FollowDist, FollowAngle, MOTION_SLOT_IDLE);
                }

                me->setActive(true);
            }
            else if (action == ACTION_RESET_EVENT)
            {
                _firstDeath = true;
            }
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            me->SetHomePosition(*me);
            if (IsUndead)
                DoZoneInCombat();
        }

        bool CanAIAttack(Unit const* target) const override
        {
            // do not see targets inside Frostwing Halls when we are not there
            return (me->GetPositionY() > 2660.0f) == (target->GetPositionY() > 2660.0f);
        }

        void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
        {
            if (IsUndead)
                me->DespawnOrUnsummon();

            // not yet following
            if (me->GetMotionMaster()->GetMotionSlotType(MOTION_SLOT_IDLE) != FOLLOW_MOTION_TYPE)
            {
                ScriptedAI::EnterEvadeMode();
                return;
            }

            if (!_EnterEvadeMode())
                return;

            if (!me->GetVehicle())
            {
                me->GetMotionMaster()->Clear(false);
                if (Creature* crok = ObjectAccessor::GetCreature(*me, Instance->GetGuidData(DATA_CROK_SCOURGEBANE)))
                    me->GetMotionMaster()->MoveFollow(crok, FollowDist, FollowAngle, MOTION_SLOT_IDLE);
            }

            Reset();
        }

        void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
        {
            if (spell->Id == SPELL_REVIVE_CHAMPION && !IsUndead)
            {
                IsUndead = true;
                me->setDeathState(JUST_RESPAWNED);
                uint32 newEntry = 0;
                switch (me->GetEntry())
                {
                    case NPC_CAPTAIN_ARNATH:
                        newEntry = NPC_CAPTAIN_ARNATH_UNDEAD;
                        break;
                    case NPC_CAPTAIN_BRANDON:
                        newEntry = NPC_CAPTAIN_BRANDON_UNDEAD;
                        break;
                    case NPC_CAPTAIN_GRONDEL:
                        newEntry = NPC_CAPTAIN_GRONDEL_UNDEAD;
                        break;
                    case NPC_CAPTAIN_RUPERT:
                        newEntry = NPC_CAPTAIN_RUPERT_UNDEAD;
                        break;
                    default:
                        return;
                }

                Talk(SAY_CAPTAIN_RESURRECTED);
                me->UpdateEntry(newEntry, Instance->GetData(DATA_TEAM_IN_INSTANCE), me->GetCreatureData());
                DoCast(me, SPELL_UNDEATH, true);
                me->SetReactState(REACT_AGGRESSIVE);
                DoZoneInCombat(me, 150.0f);
            }
        }

    protected:
        EventMap Events;
        InstanceScript* Instance;
        float FollowAngle;
        float FollowDist;
        bool IsUndead;

    private:
        bool _firstDeath;
};

class npc_captain_arnath : public CreatureScript
{
    public:
        npc_captain_arnath() : CreatureScript("npc_captain_arnath") { }

        struct npc_captain_arnathAI : public npc_argent_captainAI
        {
            npc_captain_arnathAI(Creature* creature) : npc_argent_captainAI(creature)
            {
            }

            void Reset() override
            {
                Events.Reset();
                Events.ScheduleEvent(EVENT_ARNATH_FLASH_HEAL, urand(4000, 7000));
                Events.ScheduleEvent(EVENT_ARNATH_PW_SHIELD, urand(8000, 14000));
                Events.ScheduleEvent(EVENT_ARNATH_SMITE, urand(3000, 6000));
                if (Is25ManRaid() && IsUndead)
                    Events.ScheduleEvent(EVENT_ARNATH_DOMINATE_MIND, urand(22000, 27000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                Events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARNATH_FLASH_HEAL:
                            if (Creature* target = FindFriendlyCreature())
                                DoCast(target, SPELL_FLASH_HEAL);
                            Events.ScheduleEvent(EVENT_ARNATH_FLASH_HEAL, urand(6000, 9000));
                            break;
                        case EVENT_ARNATH_PW_SHIELD:
                        {
                            std::list<Creature*> targets = DoFindFriendlyMissingBuff(40.0f, SPELL_POWER_WORD_SHIELD);
                            DoCast(Trinity::Containers::SelectRandomContainerElement(targets), SPELL_POWER_WORD_SHIELD);
                            Events.ScheduleEvent(EVENT_ARNATH_PW_SHIELD, urand(15000, 20000));
                            break;
                        }
                        case EVENT_ARNATH_SMITE:
                            DoCastVictim(SPELL_SMITE);
                            Events.ScheduleEvent(EVENT_ARNATH_SMITE, urand(4000, 7000));
                            break;
                        case EVENT_ARNATH_DOMINATE_MIND:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_DOMINATE_MIND);
                            Events.ScheduleEvent(EVENT_ARNATH_DOMINATE_MIND, urand(28000, 37000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            Creature* FindFriendlyCreature() const
            {
                Creature* target = nullptr;
                Trinity::MostHPMissingInRange u_check(me, 60.0f, 0);
                Trinity::CreatureLastSearcher<Trinity::MostHPMissingInRange> searcher(me, target, u_check);
                me->VisitNearbyGridObject(60.0f, searcher);
                return target;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_captain_arnathAI>(creature);
        }
};

class npc_captain_brandon : public CreatureScript
{
    public:
        npc_captain_brandon() : CreatureScript("npc_captain_brandon") { }

        struct npc_captain_brandonAI : public npc_argent_captainAI
        {
            npc_captain_brandonAI(Creature* creature) : npc_argent_captainAI(creature)
            {
            }

            void Reset() override
            {
                Events.Reset();
                Events.ScheduleEvent(EVENT_BRANDON_CRUSADER_STRIKE, urand(6000, 10000));
                Events.ScheduleEvent(EVENT_BRANDON_DIVINE_SHIELD, 500);
                Events.ScheduleEvent(EVENT_BRANDON_JUDGEMENT_OF_COMMAND, urand(8000, 13000));
                if (IsUndead)
                    Events.ScheduleEvent(EVENT_BRANDON_HAMMER_OF_BETRAYAL, urand(25000, 30000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                Events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BRANDON_CRUSADER_STRIKE:
                            DoCastVictim(SPELL_CRUSADER_STRIKE);
                            Events.ScheduleEvent(EVENT_BRANDON_CRUSADER_STRIKE, urand(6000, 12000));
                            break;
                        case EVENT_BRANDON_DIVINE_SHIELD:
                            if (HealthBelowPct(20))
                            {
                                 DoCast(me, SPELL_DIVINE_SHIELD);
                                Events.RescheduleEvent(EVENT_BRANDON_DIVINE_SHIELD, 300000); // 5 minutes
                            }
                            else
                            Events.ScheduleEvent(EVENT_BRANDON_DIVINE_SHIELD, 500);
                            break;
                        case EVENT_BRANDON_JUDGEMENT_OF_COMMAND:
                            DoCastVictim(SPELL_JUDGEMENT_OF_COMMAND);
                            Events.ScheduleEvent(EVENT_BRANDON_JUDGEMENT_OF_COMMAND, urand(8000, 13000));
                            break;
                        case EVENT_BRANDON_HAMMER_OF_BETRAYAL:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_HAMMER_OF_BETRAYAL);
                            Events.ScheduleEvent(EVENT_BRANDON_HAMMER_OF_BETRAYAL, urand(45000, 60000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_captain_brandonAI>(creature);
        }
};

class npc_captain_grondel : public CreatureScript
{
    public:
        npc_captain_grondel() : CreatureScript("npc_captain_grondel") { }

        struct npc_captain_grondelAI : public npc_argent_captainAI
        {
            npc_captain_grondelAI(Creature* creature) : npc_argent_captainAI(creature) { }

            void Reset() override
            {
                Events.Reset();
                Events.ScheduleEvent(EVENT_GRONDEL_CHARGE_CHECK, 500);
                Events.ScheduleEvent(EVENT_GRONDEL_MORTAL_STRIKE, urand(8000, 14000));
                Events.ScheduleEvent(EVENT_GRONDEL_SUNDER_ARMOR, urand(3000, 12000));
                if (IsUndead)
                    Events.ScheduleEvent(EVENT_GRONDEL_CONFLAGRATION, urand(12000, 17000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                Events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GRONDEL_CHARGE_CHECK:
                            if (CanCast(me->GetVictim(), sSpellMgr->GetSpellInfo(SPELL_CHARGE)))
                                DoCastVictim(SPELL_CHARGE);
                            Events.ScheduleEvent(EVENT_GRONDEL_CHARGE_CHECK, 500);
                            break;
                        case EVENT_GRONDEL_MORTAL_STRIKE:
                            DoCastVictim(SPELL_MORTAL_STRIKE);
                            Events.ScheduleEvent(EVENT_GRONDEL_MORTAL_STRIKE, urand(10000, 15000));
                            break;
                        case EVENT_GRONDEL_SUNDER_ARMOR:
                            DoCastVictim(SPELL_SUNDER_ARMOR);
                            Events.ScheduleEvent(EVENT_GRONDEL_SUNDER_ARMOR, urand(5000, 17000));
                            break;
                        case EVENT_GRONDEL_CONFLAGRATION:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_CONFLAGRATION);
                            Events.ScheduleEvent(EVENT_GRONDEL_CONFLAGRATION, urand(10000, 15000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            bool CanCast(Unit* target, SpellInfo const* spell)
            {
                // No target so we can't cast
                if (!target || !spell)
                    return false;

                // Silenced so we can't cast
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
                    return false;

                // Check for power
                int32 powerEntryIndex = -1;
                Powers power = spell->GetPowerType(me, &powerEntryIndex);
                if (me->GetPower(power) < spell->CalcPowerCost(me, spell->GetSchoolMask(), powerEntryIndex))
                    return false;

                // Unit is out of range of this spell
                if (!me->IsInRange(target, spell->GetMinRange(false), spell->GetMaxRange(false))) // long range EVENTAI
                    return false;

                // Spell is on cooldown
                if (me->HasSpellCooldown(spell->Id))
                    return false;

                return true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_captain_grondelAI>(creature);
        }
};

class npc_captain_rupert : public CreatureScript
{
    public:
        npc_captain_rupert() : CreatureScript("npc_captain_rupert") { }

        struct npc_captain_rupertAI : public npc_argent_captainAI
        {
            npc_captain_rupertAI(Creature* creature) : npc_argent_captainAI(creature) { }

            void Reset() override
            {
                Events.Reset();
                Events.ScheduleEvent(EVENT_RUPERT_FEL_IRON_BOMB, urand(15000, 20000));
                Events.ScheduleEvent(EVENT_RUPERT_MACHINE_GUN, urand(25000, 30000));
                Events.ScheduleEvent(EVENT_RUPERT_ROCKET_LAUNCH, urand(10000, 15000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                Events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RUPERT_FEL_IRON_BOMB:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_FEL_IRON_BOMB);
                            Events.ScheduleEvent(EVENT_RUPERT_FEL_IRON_BOMB, urand(15000, 20000));
                            break;
                        case EVENT_RUPERT_MACHINE_GUN:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                                DoCast(target, SPELL_MACHINE_GUN);
                            Events.ScheduleEvent(EVENT_RUPERT_MACHINE_GUN, urand(25000, 30000));
                            break;
                        case EVENT_RUPERT_ROCKET_LAUNCH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                                DoCast(target, SPELL_ROCKET_LAUNCH);
                            Events.ScheduleEvent(EVENT_RUPERT_ROCKET_LAUNCH, urand(10000, 15000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_captain_rupertAI>(creature);
        }
};

class npc_impaling_spear : public CreatureScript
{
    public:
        npc_impaling_spear() : CreatureScript("npc_impaling_spear") { }

        struct npc_impaling_spearAI : public ScriptedAI
        {
            npc_impaling_spearAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                _vehicleCheckTimer = 500;
            }

            void UpdateAI(uint32 diff) override
            {
                if (_vehicleCheckTimer <= diff)
                {
                    _vehicleCheckTimer = 500;
                    if (!me->GetVehicle())
                        me->DespawnOrUnsummon(100);
                }
                else
                    _vehicleCheckTimer -= diff;
            }

            uint32 _vehicleCheckTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_impaling_spearAI(creature);
        }
};

class npc_arthas_teleport_visual : public CreatureScript
{
    public:
        npc_arthas_teleport_visual() : CreatureScript("npc_arthas_teleport_visual") { }

        struct npc_arthas_teleport_visualAI : public NullCreatureAI
        {
            npc_arthas_teleport_visualAI(Creature* creature) : NullCreatureAI(creature), _instance(creature->GetInstanceScript()) { }

            void Reset() override
            {
                _events.Reset();
                if (_instance->GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE &&
                    _instance->GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE &&
                    _instance->GetBossState(DATA_SINDRAGOSA) == DONE)
                    _events.ScheduleEvent(EVENT_SOUL_MISSILE, urand(1000, 6000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (_events.Empty())
                    return;

                _events.Update(diff);

                if (_events.ExecuteEvent() == EVENT_SOUL_MISSILE)
                {
                    DoCastAOE(SPELL_SOUL_MISSILE);
                    _events.ScheduleEvent(EVENT_SOUL_MISSILE, urand(5000, 7000));
                }
            }

        private:
            InstanceScript* _instance;
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            // Distance from the center of the spire
            if (creature->GetExactDist2d(4357.052f, 2769.421f) < 100.0f && creature->GetHomePosition().GetPositionZ() < 315.0f)
                return GetIcecrownCitadelAI<npc_arthas_teleport_visualAI>(creature);

            // Default to no script
            return nullptr;
        }
};

class spell_icc_stoneform : public SpellScriptLoader
{
    public:
        spell_icc_stoneform() : SpellScriptLoader("spell_icc_stoneform") { }

        class spell_icc_stoneform_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_icc_stoneform_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Creature* target = GetTarget()->ToCreature())
                {
                    target->SetReactState(REACT_PASSIVE);
                    target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                    target->HandleEmoteStateCommand(EMOTE_STATE_CUSTOM_SPELL_02);
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Creature* target = GetTarget()->ToCreature())
                {
                    target->SetReactState(REACT_AGGRESSIVE);
                    target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                    target->HandleEmoteStateCommand(0);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_icc_stoneform_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_icc_stoneform_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_icc_stoneform_AuraScript();
        }
};

class spell_icc_sprit_alarm : public SpellScriptLoader
{
    public:
        spell_icc_sprit_alarm() : SpellScriptLoader("spell_icc_sprit_alarm") { }

        class spell_icc_sprit_alarm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icc_sprit_alarm_SpellScript);

            void HandleEvent(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                uint32 wardGUID = 0;
                switch (GetSpellInfo()->Effects[effIndex].MiscValue)
                {
                    case EVENT_AWAKEN_WARD_1: wardGUID = 136112; break;
                    case EVENT_AWAKEN_WARD_2: wardGUID = 136393; break;
                    case EVENT_AWAKEN_WARD_3: wardGUID = 136458; break;
                    case EVENT_AWAKEN_WARD_4: wardGUID = 136627; break;
                    default:
                        return;
                }

                std::list<Creature*> wards;
                GetCaster()->GetCreatureListWithEntryInGrid(wards, NPC_DEATHBOUND_WARD, 150.0f);
                for (auto&& ward : wards)
                {
                    if (ward->GetDBTableGUIDLow() == wardGUID && ward->IsAlive() && ward->HasAura(SPELL_STONEFORM))
                    {
                        ward->AI()->Talk(SAY_TRAP_ACTIVATE);
                        ward->AI()->DoAction(0);
                        break;
                    }
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_icc_sprit_alarm_SpellScript::HandleEvent, EFFECT_2, SPELL_EFFECT_SEND_EVENT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_icc_sprit_alarm_SpellScript();
        }
};

class spell_icc_geist_alarm : public SpellScriptLoader
{
    public:
        spell_icc_geist_alarm() : SpellScriptLoader("spell_icc_geist_alarm") { }

        class spell_icc_geist_alarm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icc_geist_alarm_SpellScript);

            void HandleEvent(SpellEffIndex effIndex)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                PreventHitDefaultEffect(effIndex);

                Position pos = { 4356.851563f, 3002.722412f, 360.514130f, M_PI / 2 };
                for (uint32 i = 0; i < 10; ++i)
                {
                    Position sumPos = pos.GetPosition();
                    sumPos.RelocateOffset(frand(0, M_PI * 2), frand(0, 10.0f));
                    if (Creature* summon = caster->SummonCreature(NPC_VENGEFUL_FLESHREAPER, sumPos, TEMPSUMMON_DEAD_DESPAWN))
                        summon->AI()->DoAction(i);
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_icc_geist_alarm_SpellScript::HandleEvent, EFFECT_2, SPELL_EFFECT_SEND_EVENT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_icc_geist_alarm_SpellScript();
        }
};

class go_icc_trap : public GameObjectScript
{
    public:
        go_icc_trap() : GameObjectScript("go_icc_trap") { }

        void OnLootStateChanged(GameObject* go, uint32 state, Unit* unit) override
        {
            if (state == GO_JUST_DEACTIVATED)
            {
                go->SetRespawnTime(WEEK);
                go->SaveRespawnTime();
            }
        }
};

class spell_icc_harvest_blight_specimen : public SpellScriptLoader
{
    public:
        spell_icc_harvest_blight_specimen() : SpellScriptLoader("spell_icc_harvest_blight_specimen") { }

        class spell_icc_harvest_blight_specimen_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icc_harvest_blight_specimen_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetHitUnit()->RemoveAurasDueToSpell(uint32(GetEffectValue()));
            }

            void HandleQuestComplete(SpellEffIndex effIndex)
            {
                GetHitUnit()->RemoveAurasDueToSpell(uint32(GetEffectValue()));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_icc_harvest_blight_specimen_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
                OnEffectHitTarget += SpellEffectFn(spell_icc_harvest_blight_specimen_SpellScript::HandleQuestComplete, EFFECT_1, SPELL_EFFECT_QUEST_COMPLETE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_icc_harvest_blight_specimen_SpellScript();
        }
};

class AliveCheck
{
    public:
        bool operator()(WorldObject* object) const
        {
            if (Unit* unit = object->ToUnit())
                return unit->IsAlive();
            return false;
        }
};

class spell_svalna_revive_champion : public SpellScriptLoader
{
    public:
        spell_svalna_revive_champion() : SpellScriptLoader("spell_svalna_revive_champion") { }

        class spell_svalna_revive_champion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_svalna_revive_champion_SpellScript);

            void RemoveAliveTarget(std::list<WorldObject*>& targets)
            {
                targets.remove_if(AliveCheck());
                Trinity::Containers::RandomResizeList(targets, 2);
            }

            void Land(SpellEffIndex /*effIndex*/)
            {
                Creature* caster = GetCaster()->ToCreature();
                if (!caster)
                    return;

                caster->SetHomePosition(SvalnaLandPos);
                caster->GetMotionMaster()->MovePoint(POINT_LAND, SvalnaLandPos);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_svalna_revive_champion_SpellScript::RemoveAliveTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
                OnEffectHit += SpellEffectFn(spell_svalna_revive_champion_SpellScript::Land, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_svalna_revive_champion_SpellScript();
        }
};

class spell_svalna_remove_spear : public SpellScriptLoader
{
    public:
        spell_svalna_remove_spear() : SpellScriptLoader("spell_svalna_remove_spear") { }

        class spell_svalna_remove_spear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_svalna_remove_spear_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Creature* target = GetHitCreature())
                {
                    if (Unit* vehicle = target->GetVehicleBase())
                        vehicle->RemoveAurasDueToSpell(SPELL_IMPALING_SPEAR);
                    target->DespawnOrUnsummon(1);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_svalna_remove_spear_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_svalna_remove_spear_SpellScript();
        }
};

class at_icc_saurfang_portal : public AreaTriggerScript
{
    public:
        at_icc_saurfang_portal() : AreaTriggerScript("at_icc_saurfang_portal") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            InstanceScript* instance = player->GetInstanceScript();
            if (!instance || instance->GetBossState(DATA_DEATHBRINGER_SAURFANG) != DONE)
            {
                player->TeleportTo(631, -549.5f, 2211.5f, 539.3f, 0.0f);
                return true;
            }

            player->TeleportTo(631, 4126.35f, 2769.23f, 350.963f, 0.0f);

            if (instance->GetData(DATA_COLDFLAME_JETS) == NOT_STARTED)
            {
                // Process relocation now, to preload the grid and initialize traps
                player->GetMap()->PlayerRelocation(player, 4126.35f, 2769.23f, 350.963f, 0.0f);

                instance->SetData(DATA_COLDFLAME_JETS, IN_PROGRESS);
                std::list<Creature*> traps;
                GetCreatureListWithEntryInGrid(traps, player, NPC_FROST_FREEZE_TRAP, 120.0f);
                traps.sort(Trinity::ObjectDistanceOrderPred(player));
                bool instant = false;
                for (std::list<Creature*>::iterator itr = traps.begin(); itr != traps.end(); ++itr)
                {
                    (*itr)->AI()->DoAction(instant ? 1000 : 11000);
                    instant = !instant;
                }
            }
            return true;
        }
};

class at_icc_shutdown_traps : public AreaTriggerScript
{
    public:
        at_icc_shutdown_traps() : AreaTriggerScript("at_icc_shutdown_traps") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (player->IsGameMaster())
                return true;

            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_COLDFLAME_JETS) != DONE)
                {
                    instance->SetData(DATA_COLDFLAME_JETS, DONE);
                    Summon(instance, NPC_CAPTAIN_ARNATH,   4197.510f, 2770.916f, 351.156f);
                    Summon(instance, NPC_CAPTAIN_BRANDON,  4200.325f, 2770.532f, 351.156f);
                    Summon(instance, NPC_CAPTAIN_GRONDEL,  4200.092f, 2767.734f, 351.156f);
                    Summon(instance, NPC_CAPTAIN_RUPERT,   4197.361f, 2767.497f, 351.156f);
                    if (Creature* crok = Summon(instance, NPC_CROK_SCOURGEBANE, 4198.415f, 2769.15f, 351.156f))
                    {
                        uint32 delay = 0;
                        crok->m_Events.Schedule(delay += 4000, [crok]()
                        {
                            if (Creature* captain = crok->FindNearestCreature(NPC_CAPTAIN_BRANDON, 100.0f))
                                captain->AI()->Talk(SAY_CAPTAIN_TRANSPORTER);
                            crok->GetMotionMaster()->MovePoint(0, 4223.6f, 2769.036f, 350.9629f);
                        });
                        crok->m_Events.Schedule(delay += 7000, [crok]()
                        {
                            if (Creature* captain = crok->FindNearestCreature(NPC_CAPTAIN_GRONDEL, 100.0f))
                                captain->AI()->Talk(SAY_CAPTAIN_TRANSPORTER);
                        });
                        crok->m_Events.Schedule(delay += 4000, [crok]()
                        {
                            crok->AI()->Talk(SAY_CROK_TRANSPORTER);
                            crok->SetFacingTo(3.001966f);
                        });
                        crok->m_Events.Schedule(delay += 6000, [crok]()
                        {
                            crok->GetMotionMaster()->MovePoint(0, 4264.86f, 2769.65f, 349.3839f);
                        });
                        crok->m_Events.Schedule(delay += 500, [crok]()
                        {
                            if (Creature* captain = crok->FindNearestCreature(NPC_CAPTAIN_ARNATH, 100.0f))
                            {
                                captain->GetMotionMaster()->MovePoint(0, 4257.008f, 2778.286f, 349.2144f);
                                captain->DespawnOrUnsummon(7000);
                            }
                            if (Creature* captain = crok->FindNearestCreature(NPC_CAPTAIN_BRANDON, 100.0f))
                            {
                                captain->GetMotionMaster()->MovePoint(0, 4262.357f, 2774.338f, 349.3105f);
                                captain->DespawnOrUnsummon(7500);
                            }
                            if (Creature* captain = crok->FindNearestCreature(NPC_CAPTAIN_GRONDEL, 100.0f))
                            {
                                captain->GetMotionMaster()->MovePoint(0, 4262.432f, 2764.839f, 349.3242f);
                                captain->DespawnOrUnsummon(7500);
                            }
                            if (Creature* captain = crok->FindNearestCreature(NPC_CAPTAIN_RUPERT, 100.0f))
                            {
                                captain->GetMotionMaster()->MovePoint(0, 4255.499f, 2757.24f, 349.1486f);
                                captain->DespawnOrUnsummon(7000);
                            }
                            crok->DespawnOrUnsummon(4500);
                        });
                    }
                }
            }
            return true;
        }

    private:
        Creature* Summon(InstanceScript* instance, uint32 entry, float x, float y, float z)
        {
            Position summonPos = { x, y, z, 0.0f };
            if (Creature* summon = instance->instance->SummonCreature(entry, summonPos))
            {
                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC); // To make sure they don't get into combat and start their usual AI
                summon->HandleEmoteStateCommand(0);
                summon->SetSheath(SHEATH_STATE_MELEE);
                summon->CastSpell(summon, SPELL_TELEPORT_VISUAL_ONLY, true);
                return summon;
            }
            return nullptr;
        }
};

class at_icc_start_blood_quickening : public AreaTriggerScript
{
    public:
        at_icc_start_blood_quickening() : AreaTriggerScript("at_icc_start_blood_quickening") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (player->IsGameMaster())
                return true;

            if (InstanceScript* instance = player->GetInstanceScript())
                if (instance->GetData(DATA_BLOOD_QUICKENING_STATE) == NOT_STARTED)
                    instance->SetData(DATA_BLOOD_QUICKENING_STATE, IN_PROGRESS);
            return true;
        }
};

class at_icc_start_frostwing_gauntlet : public AreaTriggerScript
{
    public:
        at_icc_start_frostwing_gauntlet() : AreaTriggerScript("at_icc_start_frostwing_gauntlet") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (player->IsGameMaster())
                return true;

            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* crok = ObjectAccessor::GetCreature(*player, instance->GetGuidData(DATA_CROK_SCOURGEBANE)))
                    crok->AI()->DoAction(ACTION_START_GAUNTLET);
            return true;
        }
};

class npc_sindragosas_ward : public CreatureScript
{
    public:
        npc_sindragosas_ward() : CreatureScript("npc_sindragosas_ward") { }

        struct npc_sindragosas_wardAI : public BossAI
        {
            npc_sindragosas_wardAI(Creature* creature) : BossAI(creature, DATA_SINDRAGOSA_GAUNTLET)
            {
                _isEventInProgressOrDone = false;
                _spawnCountToBeSummonedInWave = 0;
                _waveNumber = 0;
            }

            void Reset() override
            {
                _Reset();
                _isEventInProgressOrDone = false;
                _spawnCountToBeSummonedInWave = 0;
                _waveNumber = 0;
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_START_GAUNTLET)
                    if (!_isEventInProgressOrDone)
                        if (!IsAnyPlayerOutOfRange())
                            DoZoneInCombat(me, 150.0f);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                _isEventInProgressOrDone = true;
                _spawnCountToBeSummonedInWave = 32;
                _waveNumber = 1;
                DoSummonWave(_waveNumber);
                Talk(SAY_WARD_GAUNTLET_STARTED);
                events.ScheduleEvent(EVENT_SUB_WAVE_1, 10000);
                events.ScheduleEvent(EVENT_SUB_WAVE_2, 25000);
                events.ScheduleEvent(EVENT_UPDATE_CHECK, 5000);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                // Open this door only once, it should be closed, when Sindragosa npcs get infight
                // Server crashes can be ignored in this case, since teleporter to Sindragosa is active now
                if (GameObject* sindragosaDoor = instance->instance->GetGameObject(instance->GetGuidData(GO_SINDRAGOSA_ENTRANCE_DOOR)))
                    instance->HandleGameObject(instance->GetGuidData(GO_SINDRAGOSA_ENTRANCE_DOOR), true, sindragosaDoor);
            }

            void DoSummonWave(uint8 number)
            {
                switch (number)
                {
                    case 1:
                    case 3:
                        me->SummonCreature(NPC_NERUBAR_WEBWEAVER, SindragosaGauntletSpawn[1], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        me->SummonCreature(NPC_NERUBAR_WEBWEAVER, SindragosaGauntletSpawn[4], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        me->SummonCreature(NPC_NERUBAR_WEBWEAVER, SindragosaGauntletSpawn[7], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        me->SummonCreature(NPC_NERUBAR_WEBWEAVER, SindragosaGauntletSpawn[10], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        me->SummonCreature(NPC_NERUBAR_CHAMPION, SindragosaGauntletSpawn[2], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        me->SummonCreature(NPC_NERUBAR_CHAMPION, SindragosaGauntletSpawn[5], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        me->SummonCreature(NPC_NERUBAR_CHAMPION, SindragosaGauntletSpawn[8], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        me->SummonCreature(NPC_NERUBAR_CHAMPION, SindragosaGauntletSpawn[11], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        break;
                    case 2:
                        me->SummonCreature(NPC_FROSTWARDEN_SORCERESS, SindragosaGauntletSpawn[12], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        me->SummonCreature(NPC_FROSTWARDEN_SORCERESS, SindragosaGauntletSpawn[15], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        me->SummonCreature(NPC_FROSTWARDEN_WARRIOR, SindragosaGauntletSpawn[13], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        me->SummonCreature(NPC_FROSTWARDEN_WARRIOR, SindragosaGauntletSpawn[14], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        me->SummonCreature(NPC_FROSTWARDEN_WARRIOR, SindragosaGauntletSpawn[16], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        me->SummonCreature(NPC_FROSTWARDEN_WARRIOR, SindragosaGauntletSpawn[17], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        break;
                    case EVENT_SUB_WAVE_1:
                    case EVENT_SUB_WAVE_2:
                        for (uint8 i = 0; i < 12; i++)
                            me->SummonCreature(NPC_NERUBAR_BROODLING, SindragosaGauntletSpawn[i], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000ms);
                        break;
                    default:
                        break;
                }

                for (auto&& sum : me->GetSummons())
                    sum->AI()->DoZoneInCombat(me, 150.0f);
            }

            bool IsAnyPlayerOutOfRange()
            {
                if (!me->GetMap())
                    return true;

                Map::PlayerList const& playerList = me->GetMap()->GetPlayers();

                if (playerList.isEmpty())
                    return true;

                for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                {
                    if (Player* player = itr->GetSource())
                    {
                        if (player->IsGameMaster())
                            continue;

                        if (player->IsAlive() && player->GetExactDist2d(4181.27979f, 2483.64990f) > 60.0f)
                            return true;
                    }
                }

                return false;
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
            {
                _spawnCountToBeSummonedInWave--;
                summon->DespawnOrUnsummon(30000);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                // This one should never happen, if summoned creature despawns alive, reset!
                if (summon->IsAlive())
                {
                    EnterEvadeMode();
                    return;
                }

                summons.Despawn(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || !_isEventInProgressOrDone)
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SUB_WAVE_1:
                            DoSummonWave(EVENT_SUB_WAVE_1);
                            break;
                        case EVENT_SUB_WAVE_2:
                            DoSummonWave(EVENT_SUB_WAVE_2);
                            break;
                        case EVENT_UPDATE_CHECK:
                        {
                            if (_spawnCountToBeSummonedInWave <= 5)
                            {
                                if (summons.size() < _spawnCountToBeSummonedInWave)
                                    _spawnCountToBeSummonedInWave = summons.size();

                                if (!_spawnCountToBeSummonedInWave)
                                {
                                    switch (_waveNumber)
                                    {
                                        case 1:
                                            _spawnCountToBeSummonedInWave += 30;
                                            break;
                                        case 2:
                                            _spawnCountToBeSummonedInWave += 32;
                                            break;
                                        case 3:
                                            me->Kill(me);
                                            return;
                                    }

                                    _waveNumber++;
                                    DoSummonWave(_waveNumber);
                                    events.ScheduleEvent(EVENT_SUB_WAVE_1, 10000);
                                    events.ScheduleEvent(EVENT_SUB_WAVE_2, 25000);
                                }
                            }

                            if (IsAnyPlayerOutOfRange())
                            {
                                EnterEvadeMode();
                                return;
                            }

                            events.ScheduleEvent(EVENT_UPDATE_CHECK, 5000);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

        private:
            bool _isEventInProgressOrDone;
            uint32 _spawnCountToBeSummonedInWave;
            uint8 _waveNumber;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_sindragosas_wardAI>(creature);
        }
};

enum SindragosaGauntletEnums
{
    // Common
    SPELL_GAUNTLET_WEB_BEAM         = 69986,
    POINT_GAUNTLET_DESCEND          = 1,
    POINT_GAUNTLET_SURROUND         = 2,

    // Frostwarden Warrior
    SPELL_GAUNTLET_FROSTBLADE       = 71323,
    // Frostwarden Sorceress
    SPELL_GAUNTLET_ICE_TOMB         = 71330,
    SPELL_GAUNTLET_FROST_NOVA       = 71320,
    SPELL_GAUNTLET_FROSTBOLT        = 71318,
    // Nerub'ar Champion
    SPELL_GAUNTLET_RUSH             = 71801,
    // Nerub'ar Webweaver
    SPELL_GAUNTLET_CRYPT_SCARABS    = 71326,
    SPELL_GAUNTLET_WEB              = 71327,
    SPELL_GAUNTLET_WEB_GUARD        = 11920,

    EVENT_GAUNTLET_FROSTBLADE = 1,
    EVENT_GAUNTLET_ICE_TOMB,
    EVENT_GAUNTLET_FROST_NOVA,
    EVENT_GAUNTLET_RUSH,
    EVENT_GAUNTLET_WEB,
};

class npc_icc_sindragosa_gauntlet_frostwarden : public CreatureScript
{
    public:
        npc_icc_sindragosa_gauntlet_frostwarden() : CreatureScript("npc_icc_sindragosa_gauntlet_frostwarden") { }

        struct npc_icc_sindragosa_gauntlet_frostwardenAI : public ScriptedAI
        {
            npc_icc_sindragosa_gauntlet_frostwardenAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                events.Reset();
                if (IsWarrior())
                {
                    events.ScheduleEvent(EVENT_GAUNTLET_FROSTBLADE, urand(5000, 15000));
                }
                else if (IsSorceress())
                {
                    if (Is25ManRaid())
                        events.ScheduleEvent(EVENT_GAUNTLET_ICE_TOMB, urand(15000, 20000));
                    events.ScheduleEvent(EVENT_GAUNTLET_FROST_NOVA, urand(10000, 15000));
                    autoCastTimer.SetInterval(2500);
                }
            }

            void JustEngagedWith(Unit* who) override
            {
                DoZoneInCombat(nullptr, 150.0f);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                me->m_Events.Schedule(2000, [this]()
                {
                    if (!me->IsInCombat())
                        DoZoneInCombat(nullptr, 150.0f);
                });
            }

            void AttackStart(Unit* victim) override
            {
                if (!IsSorceress())
                    ScriptedAI::AttackStart(victim);
                else if (victim && me->Attack(victim, false))
                    casterMovement.Chase(victim);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                if (IsSorceress())
                    autoCastTimer.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (IsSorceress())
                    casterMovement.Update(diff);

                // no 'while' on purpose
                switch (events.ExecuteEvent())
                {
                    case EVENT_GAUNTLET_FROSTBLADE:
                        DoCast(me, SPELL_GAUNTLET_FROSTBLADE);
                        events.ScheduleEvent(EVENT_GAUNTLET_FROSTBLADE, urand(10000, 25000));
                        break;
                    case EVENT_GAUNTLET_ICE_TOMB:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 20.0f))
                            DoCast(target, SPELL_GAUNTLET_ICE_TOMB);
                        events.ScheduleEvent(EVENT_GAUNTLET_ICE_TOMB, urand(20000, 25000));
                        break;
                    case EVENT_GAUNTLET_FROST_NOVA:
                        DoCastAOE(SPELL_GAUNTLET_FROST_NOVA);
                        events.ScheduleEvent(EVENT_GAUNTLET_FROST_NOVA, urand(10000, 20000));
                        break;
                    default:
                        if (IsSorceress() && autoCastTimer.Passed())
                        {
                            DoCastVictim(SPELL_GAUNTLET_FROSTBOLT);
                            autoCastTimer.Reset();
                        }
                        break;
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_GAUNTLET_ICE_TOMB).Spell(SPELL_GAUNTLET_FROSTBOLT);
            IntervalTimer autoCastTimer;

            bool IsWarrior()   const { return me->GetEntry() == NPC_FROSTWARDEN_WARRIOR;   }
            bool IsSorceress() const { return me->GetEntry() == NPC_FROSTWARDEN_SORCERESS; }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_icc_sindragosa_gauntlet_frostwardenAI(creature);
        }
};

class npc_icc_sindragosa_gauntlet_nerubar : public CreatureScript
{
    public:
        npc_icc_sindragosa_gauntlet_nerubar() : CreatureScript("npc_icc_sindragosa_gauntlet_nerubar") { }

        struct npc_icc_sindragosa_gauntlet_nerubarAI : public ScriptedAI
        {
            npc_icc_sindragosa_gauntlet_nerubarAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            }

            void Reset() override
            {
                events.Reset();
                if (IsBroodling())
                {
                }
                else if (IsChampion())
                {
                    events.ScheduleEvent(EVENT_GAUNTLET_RUSH, 1);
                }
                else if (IsWebweaver())
                {
                    events.ScheduleEvent(EVENT_GAUNTLET_WEB, urand(10000, 15000));
                    autoCastTimer.SetInterval(2500);
                }
            }

            void JustEngagedWith(Unit* who) override
            {
                DoZoneInCombat(nullptr, 150.0f);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetFacingTo(me->GetAngle(4181.25f, 2484.0f));
                me->m_Events.Schedule(urand(1, 5000), [this]()
                {
                    Position pos = me->GetPosition();
                    pos.m_positionZ = 211.033f;
                    me->GetMotionMaster()->MoveLand(POINT_GAUNTLET_DESCEND, pos);

                    DoCast(me, SPELL_GAUNTLET_WEB_BEAM);
                });
            }

            void AttackStart(Unit* victim) override
            {
                if (!IsWebweaver())
                    ScriptedAI::AttackStart(victim);
                else if (victim && me->Attack(victim, false))
                    casterMovement.Chase(victim);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                    return;

                if (pointId == POINT_GAUNTLET_DESCEND)
                {
                    me->OverrideInhabitType(INHABIT_GROUND);
                    me->InterruptSpell(CURRENT_CHANNELED_SPELL);
                    me->SetHomePosition(*me);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->SetAnimTier(AnimTier::Ground);

                    if (IsBroodling())
                    {
                        me->m_Events.Schedule(2000, [this]()
                        {
                            if (!me->IsInCombat())
                                DoZoneInCombat(nullptr, 150.0f);
                        });
                    }
                    else
                    {
                        me->m_Events.Schedule(250, [this]
                        {
                            if (me->IsInCombat())
                                return;

                            Position pos = me->GetPosition();
                            pos.RelocateOffset(0, 30.0f);
                            me->GetMotionMaster()->MovePoint(POINT_GAUNTLET_SURROUND, pos);
                        });
                    }
                }
                else if (pointId == POINT_GAUNTLET_SURROUND)
                {
                    if (!me->IsInCombat())
                        DoZoneInCombat(nullptr, 150.0f);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (IsWebweaver())
                    autoCastTimer.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (IsWebweaver())
                    casterMovement.Update(diff);

                // no 'while' on purpose
                switch (events.ExecuteEvent())
                {
                    case EVENT_GAUNTLET_RUSH:
                        DoCastVictim(SPELL_GAUNTLET_RUSH);
                        events.ScheduleEvent(EVENT_GAUNTLET_RUSH, 500);
                        break;
                    case EVENT_GAUNTLET_WEB:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true, -SPELL_GAUNTLET_WEB_GUARD))
                            DoCast(target, SPELL_GAUNTLET_WEB);
                        events.ScheduleEvent(EVENT_GAUNTLET_WEB, urand(10000, 20000));
                        break;
                    default:
                        if (IsWebweaver() && autoCastTimer.Passed())
                        {
                            DoCastVictim(SPELL_GAUNTLET_CRYPT_SCARABS);
                            autoCastTimer.Reset();
                        }
                        break;
                }

                if (!IsWebweaver() || casterMovement.IsUnableToCast())
                    DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_GAUNTLET_CRYPT_SCARABS);
            IntervalTimer autoCastTimer;

            bool IsBroodling() const { return me->GetEntry() == NPC_NERUBAR_BROODLING; }
            bool IsChampion()  const { return me->GetEntry() == NPC_NERUBAR_CHAMPION;  }
            bool IsWebweaver() const { return me->GetEntry() == NPC_NERUBAR_WEBWEAVER; }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_icc_sindragosa_gauntlet_nerubarAI(creature);
        }
};

class at_icc_start_sindragosa_gauntlet : public AreaTriggerScript
{
    public:
        at_icc_start_sindragosa_gauntlet() : AreaTriggerScript("at_icc_start_sindragosa_gauntlet") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (player->IsGameMaster())
                return true;

            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* ward = ObjectAccessor::GetCreature(*player, instance->GetGuidData(DATA_SINDRAGOSA_GAUNTLET)))
                    ward->AI()->DoAction(ACTION_START_GAUNTLET);

            return true;
        }
};

namespace
{
    struct strength_of_wrynn_aura : AuraScript
    {
        PrepareAuraScript(strength_of_wrynn_aura)

        strength_of_wrynn_aura(const PetAura &pet_aura) : pet_aura(&pet_aura) { }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(strength_of_wrynn_aura::applied, EFFECT_FIRST_FOUND, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectApplyFn(strength_of_wrynn_aura::removed, EFFECT_FIRST_FOUND, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        }

        void applied(const AuraEffect *, AuraEffectHandleModes)
        {
            GetTarget()->AddPetAura(pet_aura);
        }

        void removed(const AuraEffect *, AuraEffectHandleModes)
        {
            GetTarget()->RemovePetAura(pet_aura);
        }

        const PetAura *pet_aura;
    };

    struct strength_of_wrynn : SpellScriptLoader
    {
        strength_of_wrynn() : SpellScriptLoader("spell_icc_strength_of_wrynn"),
            pet_aura(0, 73828 /* Strength of Wrynn 30*/, false, 0) { }

        AuraScript *GetAuraScript() const override
        {
            return new strength_of_wrynn_aura(pet_aura);
        }

        PetAura pet_aura;
    };
}

class spell_icc_soul_missile : public SpellScriptLoader
{
    public:
        spell_icc_soul_missile() : SpellScriptLoader("spell_icc_soul_missile") { }

        class spell_icc_soul_missile_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icc_soul_missile_SpellScript);

            void RelocateDest()
            {
                static Position const offset = {0.0f, 0.0f, 200.0f, 0.0f};
                const_cast<WorldLocation*>(GetExplTargetDest())->RelocateOffset(offset);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_icc_soul_missile_SpellScript::RelocateDest);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_icc_soul_missile_SpellScript();
        }
};

namespace
{
    struct hellscreams_warsong_aura : AuraScript
    {
        PrepareAuraScript(hellscreams_warsong_aura)

        hellscreams_warsong_aura(const PetAura &pet_aura) : pet_aura(&pet_aura) { }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(hellscreams_warsong_aura::applied, EFFECT_FIRST_FOUND, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectApplyFn(hellscreams_warsong_aura::removed, EFFECT_FIRST_FOUND, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        }

        void applied(const AuraEffect *, AuraEffectHandleModes)
        {
            GetTarget()->AddPetAura(pet_aura);
        }

        void removed(const AuraEffect *, AuraEffectHandleModes)
        {
            GetTarget()->RemovePetAura(pet_aura);
        }

        const PetAura *pet_aura;
    };

    struct hellscreams_warsong : SpellScriptLoader
    {
        hellscreams_warsong() : SpellScriptLoader("spell_icc_hellscreams_warsong"),
            pet_aura(0, 73822 /* Hellscream's Warsong 30*/, false, 0)
        {
        }

        AuraScript *GetAuraScript() const override
        {
            return new hellscreams_warsong_aura(pet_aura);
        }

        PetAura pet_aura;
    };
}

class go_inconspicuous_lever : public GameObjectScript
{
    public:
        go_inconspicuous_lever() : GameObjectScript("go_inconspicuous_lever") { }

        struct go_inconspicuous_leverAI : public GameObjectAI
        {
            go_inconspicuous_leverAI(GameObject* go) : GameObjectAI(go) { }

            void OnStateChanged(uint32 state, Unit* unit) override
            {
                if (state != GO_JUST_DEACTIVATED)
                    return;

                if (InstanceScript* instance = me->GetInstanceScript())
                {
                    if (instance->GetData(DATA_COLDFLAME_JETS) == IN_PROGRESS)
                    {
                        std::list<Creature*> traps;
                        GetCreatureListWithEntryInGrid(traps, me, NPC_FROST_FREEZE_TRAP, 120.0f);
                        for (std::list<Creature*>::iterator itr = traps.begin(); itr != traps.end(); ++itr)
                            (*itr)->AI()->DoAction(0);

                        instance->SetData(DATA_COLDFLAME_JETS, DONE);
                    }
                }
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new go_inconspicuous_leverAI(go);
        }
};

enum PutricideTrapEnums
{
    SPELL_GIANT_INSECT_SWARM        = 70475,
    SPELL_SUMMON_PLAGUED_INSECTS    = 70484,
    SPELL_LEAP_TO_A_RANDOM_LOCATION = 70485,
    SPELL_FLESH_EATING_BITE         = 72967,

    EVENT_SPAWN_INSECT_TOP = 1,
    EVENT_SPAWN_INSECT_BOTTOM,
    EVENT_END_TRAP,
};

class at_icc_putricide_trap : public AreaTriggerScript
{
    public:
        at_icc_putricide_trap() : AreaTriggerScript("at_icc_putricide_trap") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (player->IsGameMaster())
                return false;

            if (InstanceScript* instance = player->GetInstanceScript())
                if (instance->GetData(DATA_PUTRICIDE_TRAP) == NOT_STARTED)
                    if (Creature* trap = GetClosestCreatureWithEntry(player, NPC_PUTRICIDE_TRAP, 100.0f))
                        trap->AI()->DoAction(0);

            return true;
        }
};

class npc_putricide_trap : public CreatureScript
{
    public:
        npc_putricide_trap() : CreatureScript("npc_putricide_trap") { }

        struct npc_putricide_trapAI : public ScriptedAI
        {
            npc_putricide_trapAI(Creature* creature) : ScriptedAI(creature), instance(me->GetInstanceScript()) { }

            void Reset() override
            {
                memset(spawnerGuids, 0, sizeof(spawnerGuids));
            }

            void DoAction(int32 action) override
            {
                if (instance->GetData(DATA_PUTRICIDE_TRAP) != NOT_STARTED)
                    return;

                instance->SetData(DATA_PUTRICIDE_TRAP, IN_PROGRESS);
                if (instance->GetData(DATA_PUTRICIDE_TRAP) != IN_PROGRESS)
                    return;

                events.Reset();
                events.ScheduleEvent(EVENT_SPAWN_INSECT_TOP, 1000);
                events.ScheduleEvent(EVENT_SPAWN_INSECT_BOTTOM, 2000);
                events.ScheduleEvent(EVENT_END_TRAP, 40000);

                DoCastAOE(SPELL_GIANT_INSECT_SWARM, true);

                std::list<Creature*> spawners;
                GetCreatureListWithEntryInGrid(spawners, me, NPC_INVISIBLE_STALKER, 100.0f);
                if (spawners.size() == 4)
                {
                    spawners.sort(Trinity::ObjectDistanceOrderPred(me));
                    uint32 i = 0;
                    for (auto&& spawner : spawners)
                        spawnerGuids[i++] = spawner->GetGUID();
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SPAWN_INSECT_TOP:
                        case EVENT_SPAWN_INSECT_BOTTOM:
                            for (uint32 i = uint32(eventId == EVENT_SPAWN_INSECT_TOP ? 0 : 2); i < uint32(eventId == EVENT_SPAWN_INSECT_TOP ? 2 : 4); ++i)
                                if (Creature* spawner = ObjectAccessor::GetCreature(*me, spawnerGuids[i]))
                                    spawner->CastSpell(spawner, SPELL_SUMMON_PLAGUED_INSECTS, true);
                            events.ScheduleEvent(eventId, 2000);
                            break;
                        case EVENT_END_TRAP:
                            events.Reset();
                            me->RemoveAurasDueToSpell(SPELL_GIANT_INSECT_SWARM);
                            me->RemoveAllDynObjects();
                            instance->SetData(DATA_PUTRICIDE_TRAP, DONE);
                            break;
                    }
                }
            }

        private:
            InstanceScript* instance;
            EventMap events;
            ObjectGuid spawnerGuids[4];
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_putricide_trapAI>(creature);
        }
};

class npc_flesh_eating_insect : public CreatureScript
{
    public:
        npc_flesh_eating_insect() : CreatureScript("npc_flesh_eating_insect") { }

        struct npc_flesh_eating_insectAI : public ScriptedAI
        {
            npc_flesh_eating_insectAI(Creature* creature) : ScriptedAI(creature) { }

            void IsSummonedBy(Unit* summoner) override
            {
                DoCastAOE(SPELL_LEAP_TO_A_RANDOM_LOCATION, true);
                me->m_Events.Schedule(1500, [this]()
                {
                    me->ToTempSummon()->SetTempSummonType(TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT);
                    if (!me->IsInCombat())
                        DoZoneInCombat(nullptr, 20.0f);
                });
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!me->GetCurrentSpell(CURRENT_MELEE_SPELL))
                    DoCastVictim(SPELL_FLESH_EATING_BITE);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_flesh_eating_insectAI>(creature);
        }
};

class spell_icc_leap_to_a_random_location : public SpellScriptLoader
{
    public:
        spell_icc_leap_to_a_random_location() : SpellScriptLoader("spell_icc_leap_to_a_random_location") { }

        class spell_icc_leap_to_a_random_location_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icc_leap_to_a_random_location_SpellScript);

            void TargetSelect(SpellDestination& dest)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                dest._position.m_positionZ = caster->GetPositionZ();
                //caster->UpdateGroundPositionZ(dest._position.GetPositionX(), dest._position.GetPositionY(), dest._position.m_positionZ, 0, 100.0f);
                caster->UpdateGroundPositionZ(dest._position.GetPositionX(), dest._position.GetPositionY(), dest._position.m_positionZ);
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_icc_leap_to_a_random_location_SpellScript::TargetSelect, EFFECT_0, TARGET_DEST_DEST_RANDOM);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_icc_leap_to_a_random_location_SpellScript();
        }
};

enum EmpoweringOrbEnums
{
    SPELL_SIPHON_ESSENCE            = 70299,
    SPELL_ORB_CONTROLLER_ACTIVATE   = 70293,
    SPELL_BLOOD_ORB_CAST_VISUAL     = 72099,
    SPELL_BLOOD_ORB_STATE_VISUAL    = 72100,

    NPC_DARKFALLEN_ADVISOR          = 37571,
    NPC_DARKFALLEN_BLOOD_KNIGHT     = 37595,
    NPC_DARKFALLEN_NOBLE            = 37663,
    NPC_DARKFALLEN_ARCHMAGE         = 37664,
    NPC_EMPOWERING_ORB_CONTROLLER   = 36934,
    NPC_EMPOWERING_ORB_VISUAL       = 38463,

    GO_EMPOWERING_BLOOD_ORB         = 201741,

    EVENT_STORE_TRASH = 1,
    EVENT_CHECK_CHANNELER,
};

uint32 const empoweringOrbTrashEntries[] = { NPC_DARKFALLEN_ADVISOR, NPC_DARKFALLEN_BLOOD_KNIGHT, NPC_DARKFALLEN_NOBLE, NPC_DARKFALLEN_ARCHMAGE, NPC_DARKFALLEN_BLOOD_KNIGHT, NPC_DARKFALLEN_ARCHMAGE };

class npc_icc_empowering_orb_controller : public CreatureScript
{
    public:
        npc_icc_empowering_orb_controller() : CreatureScript("npc_icc_empowering_orb_controller") { }

        struct npc_icc_empowering_orb_controllerAI : public ScriptedAI
        {
            npc_icc_empowering_orb_controllerAI(Creature* creature) : ScriptedAI(creature), instance(me->GetInstanceScript()) { }

            void Reset() override
            {
                memset(trashGuids, 0, sizeof(trashGuids));
                events.Reset();
                events.ScheduleEvent(EVENT_STORE_TRASH, 1000);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_ORB_CONTROLLER_ACTIVATE)
                {
                    DoCast(caster, SPELL_EMPOWERED_BLOOD_NPC, true);
                    if (Creature* visual = ObjectAccessor::GetCreature(*me, visualGuid))
                    {
                        visual->CastStop();
                        visual->RemoveAurasDueToSpell(SPELL_BLOOD_ORB_STATE_VISUAL);
                    }
                    if (GameObject* orb = ObjectAccessor::GetGameObject(*me, orbGuid))
                        orb->SetGoState(GO_STATE_ACTIVE);
                }
                else if (spell->Id == SPELL_EMPOWERED_BLOOD)
                {
                    me->m_Events.Schedule(30000, [this]()
                    {
                        if (GameObject* orb = ObjectAccessor::GetGameObject(*me, orbGuid))
                        {
                            orb->SetRespawnTime(WEEK);
                            orb->SaveRespawnTime();
                        }
                    });
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STORE_TRASH:
                        {
                            bool missing = false;
                            if (!visualGuid)
                            {
                                Creature* visual = nullptr;
                                if ((visual = me->FindNearestCreature(NPC_EMPOWERING_ORB_VISUAL, 20.0f, true)) ||
                                    (visual = me->FindNearestCreature(NPC_EMPOWERING_ORB_VISUAL, 20.0f, false)))
                                    visualGuid = visual->GetGUID();
                                else
                                    missing = true;
                            }
                            if (!orbGuid)
                            {
                                if (GameObject* orb = me->FindNearestGameObject(GO_EMPOWERING_BLOOD_ORB, 20.0f))
                                    orbGuid = orb->GetGUID();
                                else
                                    missing = true;
                            }
                            for (uint32 i = 0; i < uint32(Is25ManRaid() ? 6 : 4); ++i)
                            {
                                if (trashGuids[i])
                                    continue;
                                std::list<Creature*> trash;
                                GetCreatureListWithEntryInGrid(trash, me, empoweringOrbTrashEntries[i], 20.0f);
                                trash.remove_if([](Creature* c) { return c->GetPositionZ() > 400.0f; });
                                if (trash.size() == 2)
                                    trashGuids[i] = (i < 4 ? trash.front() : trash.back())->GetGUID();
                                else if (trash.size() == 1)
                                    trashGuids[i] = trash.front()->GetGUID();
                                else
                                    missing = true;
                            }
                            if (missing)
                                events.ScheduleEvent(EVENT_STORE_TRASH, 5000);
                            else
                            {
                                // Select a creature farthest from Advisor to serve as channeler
                                std::list<Creature*> trash;
                                for (auto&& guid : trashGuids)
                                    if (guid)
                                        if (Creature* c = ObjectAccessor::GetCreature(*me, guid))
                                            trash.push_back(c);
                                if (trash.size() > 1)
                                    trash.sort(Trinity::ObjectDistanceOrderPred(trash.front()));
                                channelerGuid = trash.back()->GetGUID();

                                events.ScheduleEvent(EVENT_CHECK_CHANNELER, 1);
                            }
                            break;
                        }
                        case EVENT_CHECK_CHANNELER:
                        {
                            events.ScheduleEvent(EVENT_CHECK_CHANNELER, 1000);
                            if (me->HasAura(SPELL_SIPHON_ESSENCE))
                                break;

                            Creature* visual = ObjectAccessor::GetCreature(*me, visualGuid);
                            if (!visual)
                                break;

                            GameObject* orb = ObjectAccessor::GetGameObject(*me, orbGuid);
                            if (!orb)
                                break;

                            //visual->CastSpell(visual, SPELL_BLOOD_ORB_CAST_VISUAL);

                            std::list<Creature*> availableTrash;
                            bool inCombat = false;
                            for (uint32 i = 0; i < uint32(Is25ManRaid() ? 6 : 4); ++i)
                            {
                                if (Creature* trash = ObjectAccessor::GetCreature(*me, trashGuids[i]))
                                {
                                    if (trash->IsAlive())
                                    {
                                        availableTrash.push_back(trash);
                                        inCombat |= trash->IsInCombat();
                                    }
                                }
                            }

                            Creature* channeler = ObjectAccessor::GetCreature(*me, channelerGuid);
                            if (!channeler || channeler->isDead())
                            {
                                if (orb->GetGoState() == GO_STATE_READY)
                                    orb->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

                                if (!availableTrash.empty())
                                    break;

                                visual->CastStop();
                                visual->RemoveAurasDueToSpell(SPELL_BLOOD_ORB_STATE_VISUAL);
                                events.Reset();

                                if (me->GetPositionZ() < 370.0f)
                                    instance->HandleGameObject(instance->GetGuidData(GO_CRIMSON_HALL_DOOR), true);
                                break;
                            }
                            if (me->GetPositionZ() < 370.0f)
                                instance->HandleGameObject(instance->GetGuidData(GO_CRIMSON_HALL_DOOR), false);

                            if (!inCombat)
                                break;

                            for (auto&& trash : availableTrash)
                                if (!trash->IsInCombat())
                                    DoZoneInCombat(trash, 100.0f);

                            if (orb->GetGoState() == GO_STATE_ACTIVE)
                                break;

                            if (channeler && channeler->IsAlive())
                            {
                                channeler->CastStop();
                                channeler->CastSpell(channeler, SPELL_SIPHON_ESSENCE);
                            }
                            break;
                        }
                    }
                }
            }

        private:
            InstanceScript* instance;
            EventMap events;
            ObjectGuid trashGuids[6];
            ObjectGuid visualGuid = ObjectGuid::Empty;
            ObjectGuid orbGuid = ObjectGuid::Empty;
            ObjectGuid channelerGuid = ObjectGuid::Empty;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_icc_empowering_orb_controllerAI>(creature);
        }
};

enum UnsealedChestQuestgiversEnum
{
    SPELL_MOGRAINE_FORGE                = 70366,
    SPELL_FORGE_FLAME_BLUE              = 70367,
    SPELL_MOGRAINE_FORGE_BEAM           = 70471,
    SPELL_SHADOWS_EDGE_AURA             = 70504,
    SPELL_SUMMON_SHADOWS_EDGE_BUNNY     = 70508,
    SPELL_BLUE_EXPLOSION                = 70509,
    SPELL_SUMMON_AXE_BUNNY              = 70526,
    SPELL_MASTER_SUMMON_AXE             = 70528,
    SPELL_SUMMON_SHADOWS_EDGE_AXE       = 70529,
    SPELL_MOGRAINE_FORGE_GOSSIP         = 71231,
    SPELL_PULL_SHADOWS_EDGE             = 71310,
    SPELL_MOGRAINE_AXE_TIMER_AURA       = 71385,
    SPELL_DISARM_MOGRAINE               = 71389,
    SPELL_MOGRAINE_FORGE_BEAM_II        = 72209,
    SPELL_SUMMON_SHADOWMOURNE_BUNNY     = 72212,
    SPELL_SUMMON_SHADOWMOURNE_AXE       = 72213,
    SPELL_SHADOWMOURNE_QUEST_ACCEPT     = 72411,
    SPELL_SUMMON_ALEXANDROS             = 72468,
    SPELL_ALEXANDROS_SPAWN              = 72469,
    NPC_RUNEFORGE_BUNNY                 = 37702,
    NPC_SHADOWS_EDGE_BUNNY              = 37801,
    NPC_SHADOWS_EDGE_AXE_BUNNY          = 37814,
    NPC_SHADOWS_EDGE_MODEL              = 38191,
    NPC_SHADOWMOURNE_BUNNY              = 38527,
    NPC_SHADOWMOURNE_AXE                = 38528,
    NPC_SHADOWMOURNE_AXE_MODEL          = 38529,
    NPC_HIGHLORD_ALEXANDROS_MOGRAINE    = 38610,
    ITEM_SHADOWS_EDGE_MONSTER_OFFHAND   = 50257,
    ITEM_SHADOWMOURNE_MONSTER_OFFHAND   = 50815,
    QUEST_THE_SACRED_AND_THE_CORRUPT    = 24545,
    QUEST_SHADOWMOURNE                  = 24549,
    QUEST_SHADOWS_EDGE                  = 24743,
    QUEST_EMPOWERMENT                   = 24912,
    QUEST_MOGRAINES_REUNION             = 24915,
    QUEST_JAINAS_LOCKET                 = 24916,
    QUEST_MURADINS_LAMENT               = 24917,
    QUEST_SYLVANAS_VENGEANCE            = 24918,
    QUEST_THE_LIGHTBRINGERS_REDEMPTION  = 24919,
    EVENT_GROUP_BLUE_EXPLOSION          = 1,
};

class npc_icc_unsealed_chest_questgiver : public CreatureScript
{
    public:
        npc_icc_unsealed_chest_questgiver() : CreatureScript("npc_icc_unsealed_chest_questgiver") { }

        bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, Creature* target) override
        {
            if (spellId == SPELL_MOGRAINE_FORGE && caster->GetTypeId() == TYPEID_PLAYER)
            {
                if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_THE_SACRED_AND_THE_CORRUPT))
                {
                    OnQuestReward(caster->ToPlayer(), target, quest, 0);
                    return true;
                }
            }
            return false;
        }

        bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 opt) override
        {
            if (!creature->HasFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
                return false;

            uint32 delay = 0;
            switch (creature->GetEntry())
            {
                case NPC_HIGHLORD_DARION_MOGRAINE_QUEST:
                    switch (quest->GetQuestId())
                    {
                        case QUEST_THE_SACRED_AND_THE_CORRUPT:
                        {
                            if (Creature* bunny = creature->FindNearestCreature(NPC_RUNEFORGE_BUNNY, 20.0f))
                                bunny->SetStandState(UNIT_STAND_STATE_DEAD); // Probably incorrect, but results in better visual alignment

                            creature->SetWalk(true);
                            creature->m_Events.Schedule(delay +=  2000, [creature]() { creature->AI()->Talk(0); });
                            creature->m_Events.Schedule(delay +=  3000, [creature]() { creature->GetMotionMaster()->MovePoint(0, -67.514404f, 2161.194580f, 30.654169f); }); // not from sniff
                            creature->m_Events.Schedule(delay +=  2750, [creature]() { creature->SetFacingTo(5.846046f); }); // not from sniff
                            creature->m_Events.Schedule(delay +=  2000, [creature]()
                            {
                                creature->AI()->Talk(1);
                                creature->CastSpell(creature, SPELL_MOGRAINE_FORGE_BEAM);
                            });
                            creature->m_Events.Schedule(delay +=  6000, [creature]()
                            {
                                if (Creature* bunny = creature->FindNearestCreature(NPC_RUNEFORGE_BUNNY, 20.0f))
                                {
                                    bunny->m_Events.Repeated(0, 900, EVENT_GROUP_BLUE_EXPLOSION, [bunny]() { bunny->CastSpell(bunny, SPELL_BLUE_EXPLOSION); return false; });
                                    bunny->CastSpell(bunny, SPELL_SUMMON_SHADOWS_EDGE_BUNNY, true);
                                }
                                if (Creature* bunny = creature->FindNearestCreature(NPC_SHADOWS_EDGE_BUNNY, 20.0f))
                                {
                                    bunny->SetVisible(false);
                                    bunny->CastSpell(bunny, SPELL_SHADOWS_EDGE_AURA, true);
                                }
                            });
                            creature->m_Events.Schedule(delay +=  5000, [creature]()
                            {
                                creature->GetMotionMaster()->MovePoint(0, creature->GetHomePosition());
                                if (Creature* bunny = creature->FindNearestCreature(NPC_RUNEFORGE_BUNNY, 20.0f))
                                    bunny->m_Events.KillEventsByGroup(EVENT_GROUP_BLUE_EXPLOSION);
                            });
                            creature->m_Events.Schedule(delay +=  3500, [creature]() { creature->SetFacingTo(creature->GetHomePosition().GetOrientation()); });
                            creature->m_Events.Schedule(delay +=  2000, [creature]()
                            {
                                creature->AI()->Talk(2);
                                if (Creature* bunny = creature->FindNearestCreature(NPC_RUNEFORGE_BUNNY, 20.0f))
                                    bunny->CastSpell(bunny, SPELL_FORGE_FLAME_BLUE);
                                if (Creature* bunny = creature->FindNearestCreature(NPC_SHADOWS_EDGE_BUNNY, 20.0f))
                                {
                                    bunny->SetWalk(true);
                                    bunny->GetMotionMaster()->MovePoint(0, -71.016014f, 2158.688232f, bunny->GetPositionZ()/*30.654173f*/); // not from sniff
                                    bunny->SetVisible(true);
                                }
                            });
                            creature->m_Events.Schedule(delay +=  5000, [creature]()
                            {
                                creature->AI()->Talk(3);
                                if (Creature* bunny = creature->FindNearestCreature(NPC_RUNEFORGE_BUNNY, 20.0f))
                                    bunny->SetStandState(UNIT_STAND_STATE_STAND);
                                if (Creature* bunny = creature->FindNearestCreature(NPC_SHADOWS_EDGE_BUNNY, 20.0f))
                                    bunny->SetObjectScale(1.5f);
                            });
                            creature->m_Events.Schedule(delay +=  4000, [creature]()
                            {
                                creature->CastSpell(creature, SPELL_SUMMON_SHADOWS_EDGE_AXE, true);
                                if (Creature* axe = creature->FindNearestCreature(NPC_SHADOWS_EDGE_AXE_BUNNY, 20.0f))
                                    axe->UpdateEntry(NPC_SHADOWS_EDGE_MODEL); // I have no idea how else to force model of another creature onto a trigger, and there are no SPELL_AURA_TRANSFORM auras for this
                            });
                            creature->m_Events.Schedule(delay +=  3000, [creature]()
                            {
                                if (Creature* bunny = creature->FindNearestCreature(NPC_SHADOWS_EDGE_BUNNY, 20.0f))
                                    bunny->SetObjectScale(0.1f);
                            });
                            creature->m_Events.Schedule(delay +=  1000, [creature]()
                            {
                                if (Creature* bunny = creature->FindNearestCreature(NPC_SHADOWS_EDGE_BUNNY, 20.0f))
                                    bunny->DespawnOrUnsummon();
                            });
                            creature->m_Events.Schedule(delay +=   500, [creature]() { creature->CastSpell(creature, SPELL_PULL_SHADOWS_EDGE); });
                            creature->m_Events.Schedule(delay +=   500, [creature]()
                            {
                                if (Creature* axe = creature->FindNearestCreature(/*NPC_SHADOWS_EDGE_AXE_BUNNY*/NPC_SHADOWS_EDGE_MODEL, 20.0f))
                                    axe->EnterVehicle(creature);
                            });
                            creature->m_Events.Schedule(delay +=   500, [creature]()
                            {
                                creature->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, ITEM_SHADOWS_EDGE_MONSTER_OFFHAND);
                                if (Creature* axe = creature->FindNearestCreature(/*NPC_SHADOWS_EDGE_AXE_BUNNY*/NPC_SHADOWS_EDGE_MODEL, 20.0f))
                                    axe->DespawnOrUnsummon();
                                creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                                SetQuestGiverFlag(creature, true);
                                creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            });

                            SetQuestGiverFlag(creature, false);
                            creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_SPELLCLICK);
                            return false;
                        }
                        case QUEST_SHADOWS_EDGE:
                        {
                            creature->m_Events.Schedule(delay +=  1000, [creature]() { creature->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, EQUIP_UNEQUIP); });
                            return false;
                        }
                        case QUEST_EMPOWERMENT:
                        {
                            if (Creature* bunny = creature->FindNearestCreature(NPC_RUNEFORGE_BUNNY, 20.0f))
                                bunny->SetStandState(UNIT_STAND_STATE_DEAD); // Probably incorrect, but results in better visual alignment

                            creature->SetWalk(true);
                            creature->m_Events.Schedule(delay +=  2000, [creature]() { creature->AI()->Talk(13); });
                            creature->m_Events.Schedule(delay +=  2000, [creature]() { creature->GetMotionMaster()->MovePoint(0, -67.514404f, 2161.194580f, 30.654169f); }); // not from sniff
                            creature->m_Events.Schedule(delay +=  2750, [creature]() { creature->SetFacingTo(5.846046f); }); // not from sniff
                            creature->m_Events.Schedule(delay +=  2000, [creature]()
                            {
                                creature->AI()->Talk(14);
                                creature->CastSpell(creature, SPELL_MOGRAINE_FORGE_BEAM_II);
                            });
                            creature->m_Events.Schedule(delay +=  6000, [creature]()
                            {
                                if (Creature* bunny = creature->FindNearestCreature(NPC_RUNEFORGE_BUNNY, 20.0f))
                                {
                                    bunny->m_Events.Repeated(0, 900, EVENT_GROUP_BLUE_EXPLOSION, [bunny]() { bunny->CastSpell(bunny, SPELL_BLUE_EXPLOSION); return false; });
                                    bunny->CastSpell(bunny, SPELL_SUMMON_SHADOWMOURNE_BUNNY, true);
                                }
                                if (Creature* bunny = creature->FindNearestCreature(NPC_SHADOWMOURNE_BUNNY, 20.0f))
                                {
                                    bunny->SetVisible(false);
                                    bunny->CastSpell(bunny, SPELL_SHADOWS_EDGE_AURA, true);
                                }
                            });
                            creature->m_Events.Schedule(delay +=  5000, [creature]()
                            {
                                creature->GetMotionMaster()->MovePoint(0, creature->GetHomePosition());
                                if (Creature* bunny = creature->FindNearestCreature(NPC_RUNEFORGE_BUNNY, 20.0f))
                                    bunny->m_Events.KillEventsByGroup(EVENT_GROUP_BLUE_EXPLOSION);
                            });
                            creature->m_Events.Schedule(delay +=  2750, [creature]() { creature->SetFacingTo(creature->GetHomePosition().GetOrientation()); });
                            creature->m_Events.Schedule(delay +=  2000, [creature]()
                            {
                                creature->AI()->Talk(15);
                                if (Creature* bunny = creature->FindNearestCreature(NPC_RUNEFORGE_BUNNY, 20.0f))
                                    bunny->CastSpell(bunny, SPELL_FORGE_FLAME_BLUE);
                                if (Creature* bunny = creature->FindNearestCreature(NPC_SHADOWMOURNE_BUNNY, 20.0f))
                                {
                                    bunny->SetWalk(true);
                                    bunny->GetMotionMaster()->MovePoint(0, -71.016014f, 2158.688232f, bunny->GetPositionZ()/*30.654173f*/); // not from sniff
                                    bunny->SetVisible(true);
                                }
                            });
                            creature->m_Events.Schedule(delay +=  7000, [creature]()
                            {
                                if (Creature* bunny = creature->FindNearestCreature(NPC_RUNEFORGE_BUNNY, 20.0f))
                                    bunny->SetStandState(UNIT_STAND_STATE_STAND);
                                if (Creature* bunny = creature->FindNearestCreature(NPC_SHADOWMOURNE_BUNNY, 20.0f))
                                    bunny->SetObjectScale(1.5f);
                            });
                            creature->m_Events.Schedule(delay +=  1000, [creature]() { creature->AI()->Talk(16); });
                            creature->m_Events.Schedule(delay +=  2500, [creature]()
                            {
                                creature->CastSpell(creature, SPELL_SUMMON_SHADOWMOURNE_AXE, true);
                                if (Creature* axe = creature->FindNearestCreature(NPC_SHADOWMOURNE_AXE, 20.0f))
                                    axe->UpdateEntry(NPC_SHADOWMOURNE_AXE_MODEL); // I have no idea how else to force model of another creature onto a trigger, and there are no SPELL_AURA_TRANSFORM auras for this
                            });
                            creature->m_Events.Schedule(delay +=   500, [creature]() { creature->AI()->Talk(17); });
                            creature->m_Events.Schedule(delay +=  4500, [creature]() { creature->CastSpell(creature, SPELL_PULL_SHADOWS_EDGE); });
                            creature->m_Events.Schedule(delay +=   500, [creature]()
                            {
                                if (Creature* bunny = creature->FindNearestCreature(NPC_SHADOWMOURNE_BUNNY, 20.0f))
                                    bunny->SetObjectScale(0.1f);
                                if (Creature* axe = creature->FindNearestCreature(/*NPC_SHADOWMOURNE_AXE*/NPC_SHADOWMOURNE_AXE_MODEL, 20.0f))
                                    axe->EnterVehicle(creature);
                            });
                            creature->m_Events.Schedule(delay +=   500, [creature]()
                            {
                                creature->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, ITEM_SHADOWMOURNE_MONSTER_OFFHAND);
                                if (Creature* axe = creature->FindNearestCreature(/*NPC_SHADOWMOURNE_AXE*/NPC_SHADOWMOURNE_AXE_MODEL, 20.0f))
                                    axe->DespawnOrUnsummon();
                                creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            });
                            creature->m_Events.Schedule(delay +=  1000, [creature]()
                            {
                                if (Creature* bunny = creature->FindNearestCreature(NPC_SHADOWMOURNE_BUNNY, 20.0f))
                                    bunny->DespawnOrUnsummon();
                                creature->AI()->Talk(18);
                                SetQuestGiverFlag(creature, true);
                                creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            });

                            SetQuestGiverFlag(creature, false);
                            creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_SPELLCLICK);
                            return false;
                        }
                        case QUEST_SHADOWMOURNE:
                        {
                            creature->m_Events.Schedule(delay +=  1000, [creature]() { creature->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, EQUIP_UNEQUIP); });
                            return false;
                        }
                        case QUEST_MOGRAINES_REUNION:
                            break; // Scripted below
                        default:
                            return false;
                    }
                    creature->m_Events.Schedule(delay += 5000, [creature]()
                    {
                        creature->CastSpell(creature, SPELL_SUMMON_ALEXANDROS, true);
                        if (Creature* alexandros = creature->FindNearestCreature(NPC_HIGHLORD_ALEXANDROS_MOGRAINE, 20.0f))
                        {
                            alexandros->CastSpell(alexandros, 72469, true);
                            uint32 delay = 0;
                            alexandros->m_Events.Schedule(delay += 10000, [alexandros]() { alexandros->AI()->Talk(0); });
                            alexandros->m_Events.Schedule(delay +=  7000, [alexandros]() { alexandros->AI()->Talk(1); });
                            alexandros->m_Events.Schedule(delay += 14000, [alexandros]() { alexandros->AI()->Talk(2); });
                            alexandros->m_Events.Schedule(delay += 12000, [alexandros]() { alexandros->AI()->Talk(3); });
                            alexandros->m_Events.Schedule(delay += 10000, [alexandros]() { alexandros->AI()->Talk(4); });
                            alexandros->m_Events.Schedule(delay +=  8000, [alexandros]() { alexandros->AI()->Talk(5); });
                            alexandros->m_Events.Schedule(delay +=  8000, [alexandros]() { alexandros->AI()->Talk(6); });
                            alexandros->m_Events.Schedule(delay += 19000, [alexandros]() { alexandros->DespawnOrUnsummon(); });
                        }
                    });
                    creature->m_Events.Schedule(delay += 6000, [creature]()
                    {
                        if (Creature* alexandros = creature->FindNearestCreature(NPC_HIGHLORD_ALEXANDROS_MOGRAINE, 20.0f))
                            creature->SetFacingToObject(alexandros);
                        creature->AI()->Talk(4);
                    });
                    creature->m_Events.Schedule(delay += 18000, [creature]() { creature->AI()->Talk(11); });
                    creature->m_Events.Schedule(delay += 55000, [creature]() { creature->AI()->Talk(12); });
                    creature->m_Events.Schedule(delay +=  7000, [creature]() { creature->SetFacingTo(creature->GetOriginalOrientation()); });
                    creature->m_Events.Schedule(delay +=  1000, [creature]() { SetQuestGiverFlag(creature, true); });
                    break;
                case NPC_LADY_JAINA_PROUDMOORE_QUEST:
                    if (quest->GetQuestId() != QUEST_JAINAS_LOCKET)
                        return false;
                    creature->m_Events.Schedule(delay +=  4000, [creature, player]() { creature->AI()->Talk(0, player); });
                    creature->m_Events.Schedule(delay +=  4000, [creature, player]() { creature->AI()->Talk(1, player); });
                    creature->m_Events.Schedule(delay +=  6000, [creature, player]() { creature->AI()->Talk(2, player); });
                    creature->m_Events.Schedule(delay +=  4000, [creature, player]() { creature->AI()->Talk(3, player); });
                    creature->m_Events.Schedule(delay += 11000, [creature, player]() { creature->AI()->Talk(4, player); });
                    creature->m_Events.Schedule(delay +=  6000, [creature, player]() { creature->AI()->Talk(5, player); });
                    creature->m_Events.Schedule(delay +=  5000, [creature, player]() { SetQuestGiverFlag(creature, true); });
                    break;
                case NPC_MURADIN_BRONZEBEARD_QUEST:
                    if (quest->GetQuestId() != QUEST_MURADINS_LAMENT)
                        return false;
                    creature->m_Events.Schedule(delay +=  3000, [creature, player]() { creature->AI()->Talk(0, player); });
                    creature->m_Events.Schedule(delay +=  4000, [creature, player]() { creature->AI()->Talk(1, player); });
                    creature->m_Events.Schedule(delay +=  5000, [creature, player]() { creature->AI()->Talk(2, player); });
                    creature->m_Events.Schedule(delay +=  5000, [creature, player]() { creature->AI()->Talk(3, player); });
                    creature->m_Events.Schedule(delay +=  6000, [creature, player]() { creature->AI()->Talk(4, player); });
                    creature->m_Events.Schedule(delay +=  7000, [creature, player]() { creature->AI()->Talk(5, player); });
                    creature->m_Events.Schedule(delay +=  8000, [creature, player]() { creature->AI()->Talk(6, player); });
                    creature->m_Events.Schedule(delay +=  6000, [creature, player]() { creature->AI()->Talk(7, player); });
                    creature->m_Events.Schedule(delay +=  5000, [creature, player]() { SetQuestGiverFlag(creature, true); });
                    break;
                case NPC_UTHER_THE_LIGHTBRINGER_QUEST:
                    if (quest->GetQuestId() != QUEST_THE_LIGHTBRINGERS_REDEMPTION)
                        return false;
                    creature->m_Events.Schedule(delay +=  3000, [creature, player]() { creature->AI()->Talk(0, player); });
                    creature->m_Events.Schedule(delay +=  4000, [creature, player]() { creature->AI()->Talk(1, player); });
                    creature->m_Events.Schedule(delay +=  8000, [creature, player]() { creature->AI()->Talk(2, player); });
                    creature->m_Events.Schedule(delay +=  8000, [creature, player]() { creature->AI()->Talk(3, player); });
                    creature->m_Events.Schedule(delay +=  9000, [creature, player]() { creature->AI()->Talk(4, player); });
                    creature->m_Events.Schedule(delay += 12000, [creature, player]() { creature->AI()->Talk(5, player); });
                    creature->m_Events.Schedule(delay +=  6000, [creature, player]() { creature->AI()->Talk(6, player); });
                    creature->m_Events.Schedule(delay +=  7000, [creature, player]() { creature->AI()->Talk(7, player); });
                    creature->m_Events.Schedule(delay +=  6000, [creature, player]() { creature->AI()->Talk(8, player); });
                    creature->m_Events.Schedule(delay +=  5000, [creature, player]() { SetQuestGiverFlag(creature, true); });
                    break;
                case NPC_LADY_SYLVANAS_WINDRUNNER_QUEST:
                    if (quest->GetQuestId() != QUEST_SYLVANAS_VENGEANCE)
                        return false;
                    creature->m_Events.Schedule(delay +=  3000, [creature, player]() { creature->AI()->Talk(0, player); });
                    creature->m_Events.Schedule(delay +=  4000, [creature, player]() { creature->AI()->Talk(1, player); });
                    creature->m_Events.Schedule(delay +=  8000, [creature, player]() { creature->AI()->Talk(2, player); });
                    creature->m_Events.Schedule(delay +=  7000, [creature, player]() { creature->AI()->Talk(3, player); });
                    creature->m_Events.Schedule(delay +=  8000, [creature, player]() { creature->AI()->Talk(4, player); });
                    creature->m_Events.Schedule(delay +=  8000, [creature, player]() { creature->AI()->Talk(5, player); });
                    creature->m_Events.Schedule(delay +=  3000, [creature, player]() { creature->AI()->Talk(6, player); });
                    creature->m_Events.Schedule(delay +=  5000, [creature, player]() { SetQuestGiverFlag(creature, true); });
                    break;
                default:
                    return false;
            }

            SetQuestGiverFlag(creature, false);
            return false;
        }

    private:
        static void SetQuestGiverFlag(Creature* source, bool on)
        {
            std::list<Creature*> npcs;
            GetCreatureListWithEntryInGrid(npcs, source, NPC_HIGHLORD_DARION_MOGRAINE_QUEST, 200.0f);
            GetCreatureListWithEntryInGrid(npcs, source, NPC_LADY_JAINA_PROUDMOORE_QUEST, 200.0f);
            GetCreatureListWithEntryInGrid(npcs, source, NPC_MURADIN_BRONZEBEARD_QUEST, 200.0f);
            GetCreatureListWithEntryInGrid(npcs, source, NPC_UTHER_THE_LIGHTBRINGER_QUEST, 200.0f);
            GetCreatureListWithEntryInGrid(npcs, source, NPC_LADY_SYLVANAS_WINDRUNNER_QUEST, 200.0f);
            for (auto&& npc : npcs)
                npc->ApplyModFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER, on);
        }
};

enum AshenVerdictRingsEnum
{
    RING_CLASS_TANK = 0,
    RING_CLASS_MELEE,
    RING_CLASS_CASTER,
    RING_CLASS_HEALER,
    RING_CLASS_STRENGTH,
    RING_CLASS_MAX,

    RING_RANK_BASIC = 0,
    RING_RANK_GREATER,
    RING_RANK_UNMATCHED,
    RING_RANK_ENDLESS,
    RING_RANK_MAX,

    QUEST_TANK_RING_FLAG        = 24808,
    QUEST_HEALER_RING_FLAG      = 24809,
    QUEST_MELEE_RING_FLAG       = 24810,
    QUEST_CASTER_RING_FLAG      = 24811,
    QUEST_STRENGTH_RING_FLAG    = 25238,
};

uint32 AshenVerdictRingItemIDs[5][4] =
{
    { 50375, 50388, 50403, 50404 }, // Tank
    { 50376, 50387, 50401, 50402 }, // Melee
    { 50377, 50384, 50397, 50398 }, // Caster
    { 50378, 50386, 50399, 50400 }, // Healer
    { 52569, 52570, 52571, 52572 }, // Strength
};

uint32 AshenVerdictRingQuestIDs[5][4] =
{
    { 24815, 24827, 24834, 24835 }, // Tank
    { 24815, 24826, 24832, 24833 }, // Melee
    { 24815, 24828, 24823, 24829 }, // Caster
    { 24815, 24825, 24830, 24831 }, // Healer
    { 24815, 25239, 25240, 25242 }, // Strength
};

uint32 AshenVerdictRingFlagQuestIDs[5] =
{
    QUEST_TANK_RING_FLAG,
    QUEST_MELEE_RING_FLAG,
    QUEST_CASTER_RING_FLAG,
    QUEST_HEALER_RING_FLAG,
    QUEST_STRENGTH_RING_FLAG,
};

bool CheckAshenVerdictRingRetrieveAvailability(Player* player)
{
    // Check if the player has flag quest rewarded
    bool classFound = false;
    for (uint8 c = 0; c < RING_CLASS_MAX && !classFound; ++c)
        if (player->IsQuestRewarded(AshenVerdictRingFlagQuestIDs[c]))
            classFound = true;

    if (!classFound)
        return false;

    // Check if the player currently has no rings
    for (uint8 c = 0; c < RING_CLASS_MAX; ++c)
        for (uint8 r = 0; r < RING_RANK_MAX; ++r)
            if (player->HasItemCount(AshenVerdictRingItemIDs[c][r], 1, true))
                return false;

    // Check if the player has at least any first rank quest done
    for (uint8 c = 0; c < RING_CLASS_MAX; ++c)
        if (player->IsQuestRewarded(AshenVerdictRingQuestIDs[c][0]))
            return true;

    return false;
}

class npc_icc_ormus_the_penitent : public CreatureScript
{
    public:
        npc_icc_ormus_the_penitent() : CreatureScript("npc_icc_ormus_the_penitent") { }

        struct npc_icc_ormus_the_penitentAI : public ScriptedAI
        {
            npc_icc_ormus_the_penitentAI(Creature* creature) : ScriptedAI(creature) { }

            bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
            {
                if (sender != 10998 || action != 0)
                    return false;

                if (!CheckAshenVerdictRingRetrieveAvailability(player))
                    return false;

                player->CLOSE_GOSSIP_MENU();

                // Find class of owned ring
                int8 ownedClass = -1;
                for (uint8 c = 0; c < RING_CLASS_MAX && ownedClass == -1; ++c)
                    if (player->IsQuestRewarded(AshenVerdictRingFlagQuestIDs[c]))
                        ownedClass = c;

                if (ownedClass == -1)
                    return false;

                // Find max owned ring rank
                int8 ownedRank = -1;
                for (uint8 c = 0; c < RING_CLASS_MAX && ownedRank < RING_RANK_MAX - 1; ++c)
                    for (int8 r = RING_RANK_MAX - 1; r > ownedRank; --r)
                        if (player->IsQuestRewarded(AshenVerdictRingQuestIDs[c][r]))
                            ownedRank = r;

                if (ownedRank == -1)
                    return false;

                player->AddItem(AshenVerdictRingItemIDs[ownedClass][ownedRank], 1);
                return true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_icc_ormus_the_penitentAI>(creature);
        }
};

class cond_icc_ashen_verdict_ring_retrieve : public ConditionScript
{
    public:
        cond_icc_ashen_verdict_ring_retrieve() : ConditionScript("cond_icc_ashen_verdict_ring_retrieve") { }

        bool OnConditionCheck(const Condition* /*condition*/, ConditionSourceInfo& sourceInfo) override
        {
            if (!sourceInfo.mConditionTargets[0])
                return false;

            if (Player* player = sourceInfo.mConditionTargets[0]->ToPlayer())
                return CheckAshenVerdictRingRetrieveAvailability(player);

            return false;
        }
};

class at_icc_check_lich_king_availability : public AreaTriggerScript
{
    public:
        at_icc_check_lich_king_availability() : AreaTriggerScript("at_icc_check_lich_king_availability") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                instance->GetData(DATA_CHECK_LICH_KING_AVAILABILITY);

            return true;
        }
};

enum LichKingWhispers
{
    SPELL_LICH_KING_WHISPER_EVENT       = 71391,
    SPELL_LK_WHISPER_SUPPRESSION_AURA   = 71392,
};

class spell_icc_soul_feast : public SpellScriptLoader
{
    public:
        spell_icc_soul_feast() : SpellScriptLoader("spell_icc_soul_feast") { }

        class spell_icc_soul_feast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icc_soul_feast_SpellScript);

            void HandleHitTarget(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetHitPlayer())
                {
                    if (player->HasAura(SPELL_LK_WHISPER_SUPPRESSION_AURA))
                        return;

                    if (InstanceScript* instance = player->GetInstanceScript())
                        if (instance->GetBossState(DATA_THE_LICH_KING) == DONE)
                            return;

                    player->CastSpell(player, SPELL_LICH_KING_WHISPER_EVENT, true);
                    player->CastSpell(player, SPELL_LK_WHISPER_SUPPRESSION_AURA, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_icc_soul_feast_SpellScript::HandleHitTarget, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_icc_soul_feast_SpellScript();
        }
};

void AddSC_icecrown_citadel()
{
    new npc_highlord_tirion_fordring_lh();
    new npc_rotting_frost_giant();
    new spell_frost_giant_death_plague();
    new spell_frost_giant_stomp();
    new npc_frost_freeze_trap();
    new npc_alchemist_adrianna();
    new boss_sister_svalna();
    new npc_crok_scourgebane();
    new npc_captain_arnath();
    new npc_captain_brandon();
    new npc_captain_grondel();
    new npc_captain_rupert();
    new npc_impaling_spear();
    new npc_arthas_teleport_visual();
    new spell_icc_stoneform();
    new spell_icc_sprit_alarm();
    new spell_icc_geist_alarm();
    new go_icc_trap();
    new spell_icc_harvest_blight_specimen();
    new spell_trigger_spell_from_caster("spell_svalna_caress_of_death", SPELL_IMPALING_SPEAR_KILL);
    new spell_svalna_revive_champion();
    new spell_svalna_remove_spear();
    new at_icc_saurfang_portal();
    new at_icc_shutdown_traps();
    new at_icc_start_blood_quickening();
    new at_icc_start_frostwing_gauntlet();
    new npc_sindragosas_ward();
    new npc_icc_sindragosa_gauntlet_frostwarden();
    new npc_icc_sindragosa_gauntlet_nerubar();
    new at_icc_start_sindragosa_gauntlet();
    new strength_of_wrynn;
    new spell_icc_soul_missile();
    new hellscreams_warsong;
    new go_inconspicuous_lever();
    new at_icc_putricide_trap();
    new npc_putricide_trap();
    new npc_flesh_eating_insect();
    new spell_icc_leap_to_a_random_location();
    new npc_icc_empowering_orb_controller();
    new npc_icc_unsealed_chest_questgiver();
    new npc_icc_ormus_the_penitent();
    new cond_icc_ashen_verdict_ring_retrieve();
    new at_icc_check_lich_king_availability();
    new spell_icc_soul_feast();
}
