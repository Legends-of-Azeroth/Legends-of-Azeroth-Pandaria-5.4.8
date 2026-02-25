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

#include "throne_of_thunder.h"
#include "Vehicle.h"
#include <array>

enum eSpells
{
    // Gara'Jal's Soul
    SPELL_LINGERING_PRESENCE                = 136467, // When the spirit of Gara'jal leaves a councillor
    SPELL_POSSESSED                         = 136442, // When the spirit of Gara'jal merges with a councillor (periodic dummy ? Dark Energy maybe)
    SPELL_DARK_POWER                        = 136507, // Damages increase with each cast

    // Soul Fragment
    SPELL_SOUL_FRAGMENT_SELECTOR            = 137645, // Trigger 137641 (AREA_ENTRY ?)
    SPELL_SOUL_FRAGMENT_PERIODIC            = 137641, // Override spells with 137643
    SPELL_SOUL_FRAGMENT_SWITCHER            = 137643, // Trigger 137641
    SPELL_SOUL_FRAGMENT_DUMMY               = 137670, // Maybe visual
    SPELL_SHADOWED_SOUL                     = 137650, // Each time 137641 ticks

    // Frost King Malakk

    // Frigid Assault
    SPELL_FRIGID_ASSAULT                    = 136904, // Apply trigger auras
    SPELL_FRIGID_ASSAULT_STACK              = 136903, // Stun owner when stacks reach 15
    SPELL_FRIGID_ASSAULT_DAMAGES            = 136911, // Damages
    SPELL_FRIGID_ASSAULT_STUN               = 136910, // When 136903 reaches 15 stacks

    // Biting Cold <=> Frostbite
    SPELL_BITING_COLD                       = 136917, // Main damages (select random target, need AuraScript or SpellScript)
    SPELL_BITING_COLD_PERIODIC_DAMAGES      = 136992, // Periodically trigger 136991
    SPELL_BITING_COLD_ALLY_DAMAGES          = 136991, // Periodic damages to allies
    SPELL_BITING_COLD_PERIODIC_DUMMY        = 137579, // Periodic trigger 137580
    SPELL_BITING_COLD_DUMMY                 = 137580, // Visual maybe ?
    // When Malakk has the possessed aura, Biting Cold turns into Frostbite
    SPELL_FROSTBITE                         = 136990, // Main damages (works like Biting Cold)
    SPELL_FROSTBITE_PERIODIC_DAMAGES        = 136922, // Periodic trigger damages to ally (amount must be multiplied to fit stack amount)
    SPELL_FROSTBITE_ALLY_DAMAGES            = 136937, // Damages to allies
    SPELL_FROSTBITE_SCREEN_EFFECT           = 136926, // Screen effect
    SPELL_FROSTBITE_SELECTOR                = 137575, // Selector with mark
    // Is there something that can stack ?
    // Bodyheat triggers when Frostbite hits an ally in Heroic
    SPELL_BODY_HEAT                         = 137084, // Triggers 137085 on expire (AuraScript)
    SPELL_CHILLED_TO_THE_BONE               = 137085, // Prevents player from reducing Frostbite amount of nearby players (or only caster ?)

    // Kazra'jin

    // Reckless Charge
    // SPELL_RECKLESS_CHARGE                = 137117, // Visual on the boss while travelling (launch everything) ("They see me rollin ! FUCK YEAH !")
    SPELL_RECKLESS_CHARGE_GROUND_AT         = 138026, // Visual on the ground while boss is travelling (damage dealer ?)
    SPELL_RECKLESS_CHARGE_SHATTER_GROUND    = 137122, // Final damages + Knock back and visual of rocks appearing around
    SPELL_RECKLESS_CHARGE_UNIQUE_DAMAGES    = 137133, // Damage on unique target (AT ? Kazra'jin ?)
    SPELL_RECKLESS_CHARGE_PRE_PATH          = 000000, // TBF: Visual of dark pools on ground before charge

    SPELL_RECKLESS_CHARGE                   = 137107,
    SPELL_RECKLESS_CHARGE_UNK               = 137124,
    SPELL_RECKLESS_CHARGE_VISUAL            = 137117,
    SPELL_RECKLESS_CHARGE_FACE              = 137121, // Forces facing towards target
    SPELL_RECKLESS_CHARGE_SOUND             = 137131,

    // Need black visual on ground before cast

    // Overload
    // When Kazra'jin is empowered, he overloads after performing a Reckless Charge
    SPELL_OVERLOAD                          = 137149, // Dummy aura to handle the strike back (HandleDummyAuraProc ?) (must add UNIT_STATE_STUN manually)
    SPELL_OVERLOAD_DAMAGES                  = 137151, // Damage spell
    SPELL_OVERLOAD_VISUAL                   = 137163, // Visual on caster of spell that triggered mirror effect
    // In Heroic, Overload turns into Discharge
    SPELL_DISCHARGE                         = 137166, // Periodic dummy ? Why ? (must add UNIT_STATE_STUN manually)
    SPELL_DISCHARGE_DAMAGES                 = 136935, // Damage spell
    SPELL_DISCHARGE_VISUAL                  = 137172, // Visual on all players when spell ticks

    // Sul the Sandcrawler

    SPELL_SAND_BOLT                         = 136189, // Trigger Missile 136190
    SPELL_SAND_BOLT_DAMAGES                 = 136190, // Damages

    // Quicksand (fuckin AT)
    // Quicksand is an AT, but handling the spell with an AT is too complex... we'll use another mechanism,
    SPELL_QUICKSAND_PERIODIC_DAMAGES        = 136860, // Periodic damages to any target within 7 yards : we must handle apply / remove manually
    SPELL_QUICKSAND_AT_VISUAL               = 137572, // Visual
    SPELL_QUICKSAND_AT_VISUAL_INIT          = 136851,
    SPELL_ENSNARED                          = 136878, // Slow player and stacks; when it reaches 5 stacks, player is Entrapped
    SPELL_ENTRAPPED                         = 136857, // Need to prevent second effect... so annoying

    // Sandstorm
    SPELL_SAND_STORM                        = 136894, // Periodic trigger 136899,
    SPELL_SAND_STORM_DAMAGES                = 136899, // Damages
    SPELL_SAND_STORM_DUMMY_AURA             = 136895, // Maybe visual since it has the same duration as 136894

    // High Priestess Mar'li

    // Wrath of the Loa
    SPELL_WRATH_OF_THE_LOA                  = 137344,
    SPELL_WRATH_OF_THE_LOA_DARK             = 137347, // When Possessed

    // Loa Spirit <=> Twisted Fate
    SPELL_BLESSED_LOA_SPIRIT                = 137203, // Handle dummy cause Blizzard messed that up
    SPELL_SUMMON_BLESSED_LOA_SPIRIT         = 137200, // Summons 69480
    SPELL_SHADOWED_LOA_SPIRIT               = 137350, // Handle dummy cause Blizzard messed that up (Possessed version of Loa Spirit)
    SPELL_SUMMON_SHADOWED_LOA_SPIRIT        = 137351, // Summons 69548
    // In Heroic, Shadowed Loa Spirit is replaced with Twisted Fate
    SPELL_TWISTED_FATE_PERIODIC             = 137986, // Periodic trigger 137972,
    SPELL_TWISTED_FATE_DAMAGES              = 137972, // Periodic damages while linked AND while not linked (must update value in handler of 137986)
    SPELL_TWISTED_FATE_DUMMY_AURA           = 137964, // Visual maybe ?
    // First npc of Twisted Fate
    SPELL_TWISTED_FATE                      = 137891, // Launch everything (triggers 137893)
    SPELL_TWISTED_FATE_SUMMON_FIRST         = 137893, // Summon the first Twisted Fate
    SPELL_TWISTED_FATE_FORCE_FIRST          = 137943, // Force cast 137950 on self
    SPELL_TWISTED_FATE_CLONE_FIRST          = 137950, // Clone, Dummy, Size... WHAT ELSE ?
    // Second npc of Twisted Fate
    SPELL_TWISTED_FATE_FORCE_SUMMON_SECOND  = 137962, // Trigger 137963 on the most distant player (fuck target)
    SPELL_TWISTED_FATE_SUMMON_SECOND        = 137963, // Summon the second Twisted Fate
    SPELL_TWISTED_FATE_FORCE_SECOND         = 137964, // Force cast 137965 on self
    SPELL_TWISTED_FATE_CLONE_SECOND         = 137965, // Clone, Dummy, Size... WHAT ELSE ?
    SPELL_TWISTED_FATE_VISUAL               = 137967,

    // Summons
    SPELL_FORTIFIED                         = 136864, // Living Sand : On hit by Sandstorm
    SPELL_TREACHEROUS_GROUND                = 137614, // Living Sand : On emerge from reunited Quicksand
    SPELL_BLESSED_GIFT                      = 137303, // Blessed Loa Spirit, heal 5% of Max Health on weakest councillor
    SPELL_BLESSED_TIME_OUT                  = 137204, // Dummy visual for blizz
    SPELL_MARKED_SOUL                       = 137359, // Shadowed Loa Spirit / Player ? Force Loa to follow player
    SPELL_SHADOWED_GIFT                     = 137390, // Instantly kills player (Shadowed Loa Spirit)
    SPELL_SHADOWED_TIME_OUT                 = 137398, // Dummy visual for blizz
    
    // Visuals
    SPELL_GARA_JALS_SOUL                    = 136423, // NPC Visual
    SPELL_BLESSED_TRANSFORMATION            = 137181, // NPC Visual
    SPELL_SHADOWED_TRANSFORMATION           = 137271, // NPC Visual
    SPELL_GARAJAL_GHOST                     = 000000, // TBF
    
    // Shared
    SPELL_ZERO_POWER                        = 72242,
    SPELL_PERMANENT_FEIGN_DEATH             = 70628,

    // Misc
    SPELL_BLESSING_OF_PROTECTION            = 1022,
    SPELL_ICE_BLOCK                         = 45438,
    SPELL_DIVINE_SHIELD                     = 642,
    SPELL_CAGE_MATCH                        = 138084,
    SPELL_VISUAL_ID_RECKLESS_CHARGE         = 30216,
};

enum eEvents
{
    // Frost King Malakk
    EVENT_FRIGID_ASSAULT = 1,
    EVENT_BITING_COLD,
    EVENT_FROSTBITE,
    EVENT_FROSTBITE_EFF,

    // Kazra'jin
    EVENT_RECKLESS_CHARGE_PRE_PATH,
    EVENT_RECKLESS_CHARGE,

    // Sul the Sandcrawler
    EVENT_SAND_BOLT,
    EVENT_QUICKSAND,
    EVENT_SANDSTORM,

    // High Priestess Mar'li
    EVENT_WRATH_OF_THE_LOA,
    EVENT_WRATH_OF_THE_LOA_DARK,
    EVENT_BLESSED_LOA_SPIRIT,
    EVENT_SHADOWED_LOA_SPIRIT,
    EVENT_TWISTED_FATE,

    // Blessed Loa Spirit
    EVENT_BLESSED_GIFT,
    EVENT_MOVE_COUNCILLOR,

    // Shadowed Loa Spiri
    EVENT_SHADOWED_GIFT,

    // Councillots
    EVENT_INCREASE_POWER,
    EVENT_DARK_POWER,

    // Quicksand Stalker
    EVENT_QUICKSAND_PERIODIC, // This only handles apply of the Quicksand damages aura, which handle the rooting by itself
    EVENT_TRY_MERGE,          // Try merge event is used to merge Quicksand when they are summoend by others Quicksand (only scheduled once)
    EVENT_ACTIVATE_SAND,
    
    // Garajal
    EVENT_SUMMON_SOUL,
    EVENT_REPOSSSESS,
};


enum eActions
{
    // Gara'jal
    ACTION_ENTER_COMBAT         = 0, // Garajal + Garajal's soul
    ACTION_EXIT_COUNCILLOR      = 1, // Garajal's soul
    ACTION_FIGHT_RESET          = 2,
    ACTION_FIGHT_BEGIN          = 3,
    ACTION_COUNCILLOR_DIED      = 4,

    // Councillors
    ACTION_COUNCILLORS_ENTER_COMBAT         = 4, // When one enters combat, everybody else must enter combat
    ACTION_SET_POSSESSED                    = 5, // Initialize the events to possessed phase (called from SpellScript)
    ACTION_SET_UNPOSSESSED                  = 6, // Reset the events to normal (same as for SET_POSSESED)
    ACTION_DARK_POWER                       = 7, // Initialize the Dark Power phase (called from the handler of the periodic dummy aura I assume)

    // Kazra'jin
    ACTION_RESET_DAMAGES                    = 8, // Must be called each time the periodic aura ticks

    // Living Sand
    ACTION_FORTIFY                          = 9,

    // Twisted Fate Helper
    ACTION_TWISTED_FATE_END_FIGHT           = 10, // Used to deallocate memory

    // Twisted Fate (common)
    ACTION_OTHER_TWISTED_FATE_DIED          = 11,

    // Quicksand
    ACTION_CREATE_LIVING_SAND               = 12,
};


enum eMotions
{
    // Gara'jal
    POINT_COUNCILLOR                        = 4343,

    // Kazra'jin
    POINT_RECKLESS_CHARGE_LAND              = 5000, // Position where Kazrajin lands after performing Reckless Charge
    POINT_RECKLESS_CHARGE_PLAYER            = 6714, // Position where Kazrajin rolls to after landing from Reckless Charge

    // Blessed Loa Spirit
    POINT_BLESSED_LOA_SPIRIT_COUNCILLOR     = 9413, // Point to identify the councillor the Blessed Loa Spirit is going toward
    // No need to do a point for the Shadowed Loa Spirit since it will not
    // reach the target but only go 6 yards away from him (so we can use MoveFollow)

    // Twisted Fate (common)
    POINT_MIDDLE                            = 6653, // Point representing the middle point on the line formed by the two Twisted Fate
};


enum eHelperStatus
{
    STATUS_RESET                            = 0,
    STATUS_PROGRESS                         = 1,
    STATUS_DONE                             = 2,
};


enum eDatas
{
    DATA_DARK_POWER_COUNT                   = 0,
    DATA_QUICKSAND_MERGE_COUNT              = 1,
    DATA_SHADOWED_LOA_SPIRIT_TARGET_GUID    = 2, // Accessor to get the GUID of the player the Shadowed Loa Spirit will follow
    DATA_BLESSED_LOA_SPIRIT_TARGET_GUID     = 3, // Accessor to get the GUID of the boss the Blessed Loa Spirit will follow
    DATA_TWISTED_FATE_GUID                  = 4, // Accessor to get the GUID of the twisted fate the caller is linked to
    DATA_TWISTED_FATE_OTHER_DIED            = 5, // Accessor to know if one twisted fate has been unlinked from the other
    DATA_RECKLESS_CHARGE_TARGET_GUID        = 6, // Accessor to get the GUID of the player Kazrajin is targetting for the purpose of Reckless Charge
    DATA_DAMAGES_PAST_SEC                   = 7, // Accessor to get the amount of damages Kazrajin received in the last second
    DATA_RECKLESS_CHARGE_HIT                = 8,
};


enum eTalks
{
    TALK_AGGRO                  = 0,
    TALK_POSSESS                = 1,
    TALK_SPECIAL                = 2,
    EMOTE_POSSESS               = 6,

    // sul
    TALK_SUL_QUICKSAND          = 3,
    TALK_SUL_SLAY               = 4,
    TALK_SUL_DEATH              = 5,
    TALK_SUL_SANDSTORM          = 7,

    // malakk
    TALK_MALAKK_SLAY            = 3,
    TALK_MALAKK_DEATH           = 4,
    TALK_MALAKK_FROSTBITE       = 5,

    // marli
    TALK_MARLI_SLAY             = 3,
    TALK_MARLI_DEATH            = 4,
    TALK_MARLI_BLESSED          = 5,
    TALK_MARLI_SHADOWED         = 7,

    // kazra'jin
    TALK_KAZRAJIN_CHARGE        = 3,
    TALK_KAZRAJIN_SLAY          = 4,
    TALK_KAZRAJIN_DEATH         = 5,
    TALK_KAZRAJIN_OVERLOAD      = 7,

    // Garajal
    TALK_INTRO_1                = 1,
    TALK_INTRO_2,
};

static Creature* GetBossByEntry(uint32 entry, WorldObject* source)
{
    switch (entry)
    {
        case NPC_COUNCIL_FROST_KING_MALAKK:
            return ObjectAccessor::GetCreature(*source, source->GetInstanceScript() ? source->GetInstanceScript()->GetGuidData(NPC_COUNCIL_FROST_KING_MALAKK) : ObjectGuid::Empty);
        case NPC_COUNCIL_KAZRAJIN:
            return ObjectAccessor::GetCreature(*source, source->GetInstanceScript() ? source->GetInstanceScript()->GetGuidData(NPC_COUNCIL_KAZRAJIN) : ObjectGuid::Empty);
        case NPC_COUNCIL_SUL_THE_SANDCRAWLER:
            return ObjectAccessor::GetCreature(*source, source->GetInstanceScript() ? source->GetInstanceScript()->GetGuidData(NPC_COUNCIL_SUL_THE_SANDCRAWLER) : ObjectGuid::Empty);;
        case NPC_COUNCIL_HIGH_PRIESTESS_MARLI:
            return ObjectAccessor::GetCreature(*source, source->GetInstanceScript() ? source->GetInstanceScript()->GetGuidData(NPC_COUNCIL_HIGH_PRIESTESS_MARLI) : ObjectGuid::Empty);;
        default:
            return NULL;
    }
}

// Convenient typedef for the accessors
typedef Creature* (*Accessor)(WorldObject* source);

static const std::vector<uint32> uiBossEntries = { NPC_COUNCIL_FROST_KING_MALAKK, NPC_COUNCIL_KAZRAJIN, NPC_COUNCIL_SUL_THE_SANDCRAWLER, NPC_COUNCIL_HIGH_PRIESTESS_MARLI };

const std::vector<uint32> frostbiteIgnoreSpells =
{
    SPELL_DIVINE_SHIELD,
    SPELL_ICE_BLOCK,
    SPELL_BLESSING_OF_PROTECTION,
};

// Base class for the councillor's AI (only override common functions)
class boss_council_of_elders_baseAI : public ScriptedAI
{
    public:
        boss_council_of_elders_baseAI(Creature* creature) : ScriptedAI(creature)
        {
            events.Reset();
            instance = me->GetInstanceScript();
        }

        TaskScheduler scheduler;
        EventMap events, darkPowerEvents;
        InstanceScript* instance;
        ObjectGuid frostBiteGUID;
        ObjectGuid recklessChargeGUID;
        std::vector<uint32> playerGUIDs;
        bool hasFeign;

        // Override Reset to reset the EventMap in one place and force
        // reset of the fight by sending DoAction to the helper.
        // Note: if one boss reset, every other boss need to reset. Otherwise,
        // it would be a very major fail, meaning that a creature can reset while
        // other close creatures are still in combat.
        void Reset() override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

            events.Reset();
            darkPowerEvents.Reset();

            me->SetPowerType(POWER_MANA);
            me->SetMaxPower(POWER_MANA, 100);
            me->SetPower(POWER_MANA, 0);
            me->setRegeneratingMana(false);
            uiDarkPowerCount        = 0;
            hasFeign = false;
            uiDamageTakenPossessed  = 0;
            frostBiteGUID = ObjectGuid::Empty;
            recklessChargeGUID = ObjectGuid::Empty;

            // if feign death remove
            me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);

            me->SetReactState(REACT_AGGRESSIVE);

            scheduler
                .Schedule(Seconds(2), [this](TaskContext context)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
            });
        }

        // Override JustEngagedWith to send the DoAction to the helper
        void JustEngagedWith(Unit* /*who*/) override 
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

            if (Creature* pGarajal = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_COUNCIL_OF_ELDERS) : ObjectGuid::Empty))
                pGarajal->AI()->DoAction(ACTION_FIGHT_BEGIN);

            Talk(TALK_AGGRO);

            InitStandartEvents();

            switch (me->GetEntry())
            {
                case NPC_SUL_LITHUZ_STONEGAZER:
                case NPC_COUNCIL_HIGH_PRIESTESS_MARLI:
                    me->SetReactState(REACT_DEFENSIVE);
                    break;
                case NPC_COUNCIL_KAZRAJIN:
                    me->PrepareChanneledCast(me->GetOrientation());
                    break;
            }
        }

        void EnterEvadeMode() override
        {
            if (instance)
            {
                instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_QUICKSAND_PERIODIC_DAMAGES);
            }

            CreatureAI::EnterEvadeMode();

            if (Creature* pGarajal = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_COUNCIL_OF_ELDERS) : ObjectGuid::Empty))
                pGarajal->AI()->EnterEvadeMode();

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);

            uint32 corpseDelay = me->GetCorpseDelay();
            uint32 respawnDelay = me->GetRespawnDelay();

            me->SetCorpseDelay(1);
            me->SetRespawnDelay(29);

            me->DespawnOrUnsummon();

            me->SetCorpseDelay(corpseDelay);
            me->SetRespawnDelay(respawnDelay);
        }

        // Override DoAction for the generic actions
        void DoAction(int32 actionId) override 
        {
            switch (actionId)
            {
                case ACTION_COUNCILLORS_ENTER_COMBAT:
                    // The call to AttackStart will be kinda useless for some
                    // boss since they can't move, which will result in useless
                    // call to MoveChase. Anyway, I do not want to recode this
                    // function now.
                    InitStandartEvents();
                    break;
                case ACTION_SET_POSSESSED:
                    Talk(TALK_POSSESS);
                    Talk(EMOTE_POSSESS);
                    InitPossessedEvents();
                    darkPowerEvents.ScheduleEvent(EVENT_INCREASE_POWER, GetPowerTimer());
                    uiDarkPowerCount = 0;
                    break;
                case ACTION_SET_UNPOSSESSED:
                    me->SetPower(POWER_MANA, 0);
                    InitStandartEvents();
                    uiDarkPowerCount = 0;
                    break;
                case ACTION_DARK_POWER:
                    InitDarkPower();
                    uiDarkPowerCount = 0;
                    break;
            }
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage) override
        {
            // Heroic only shit..
            // if (Aura* aura = me->GetAura(SPELL_DISCHARGE))

            // Feign Death
            if (hasFeign)
            {
                damage = 0;
                return;
            }

            // Kazrajin Discharge Incrementation
            if (me->HasAura(SPELL_DISCHARGE))
                uiDamagesDoneInPastSecs += damage;

            if (damage >= me->GetHealth() && !hasFeign)
            {
                hasFeign = true;
                damage = 0;

                // Remove Garajal Spirit
                if (me->GetVehicleKit() && me->GetVehicleKit()->GetPassenger(0))
                    if (Creature* pGarajal = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_GARA_JALS_SOUL) : ObjectGuid::Empty))
                        pGarajal->AI()->DoAction(ACTION_EXIT_COUNCILLOR);

                me->RemoveAllAuras();
                me->SetHealth(1);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->PrepareChanneledCast(me->GetOrientation());
                DoCast(me, SPELL_PERMANENT_FEIGN_DEATH, true);
                JustDied(me);
                return;
            }

            if (!me->HasAura(SPELL_POSSESSED))
                return;

            uiDamageTakenPossessed += damage;

            if (uiDamageTakenPossessed >= (float)(me->GetMaxHealth() * 0.25f))
            {
                // No remove when no other councillor alive
                if (IsACouncillorAlive())
                    if (Creature* pGarajal = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_GARA_JALS_SOUL) : ObjectGuid::Empty))
                        pGarajal->AI()->DoAction(ACTION_EXIT_COUNCILLOR);

                uiDamageTakenPossessed = 0; // Reset in both case to prevent chain call to IsACouncillorAlive
            }
        }

        void RewardCurrencyAndUpdateState()
        {
            Map::PlayerList const &lPlayers = me->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                if (Player * const player = itr->GetSource())
                {
                    player->ModifyCurrency(CURRENCY_TYPE_VALOR_POINTS, 4000);
                }
            }

            instance->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, NPC_GARA_JAL, me);
        }
    
        void JustDied(Unit* killer) override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

            switch (me->GetEntry())
            {
                case NPC_COUNCIL_FROST_KING_MALAKK:
                    Talk(TALK_MALAKK_DEATH);
                    break;
                case NPC_COUNCIL_KAZRAJIN:
                    Talk(TALK_KAZRAJIN_DEATH);
                    break;
                case NPC_COUNCIL_SUL_THE_SANDCRAWLER:
                    Talk(TALK_SUL_DEATH);
                    break;
                case NPC_COUNCIL_HIGH_PRIESTESS_MARLI:
                    Talk(TALK_MARLI_DEATH);
                    break;
                default:
                    break;
            }

            events.Reset();

            if (Creature* pGarajal = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_COUNCIL_OF_ELDERS) : ObjectGuid::Empty))
            {
                if (CreatureAI* pAI = pGarajal->AI())
                {
                    pAI->DoAction(ACTION_COUNCILLOR_DIED);

                    if (me->GetEntry() != NPC_COUNCIL_HIGH_PRIESTESS_MARLI) // in temp case (data sent twice + 1 smth else)
                        me->SetLootRecipient(NULL);
                }
            }
        }
    
        void KilledUnit(Unit* /*victim*/) override
        {
            switch (me->GetEntry())
            {
                case NPC_COUNCIL_FROST_KING_MALAKK:
                    Talk(TALK_MALAKK_SLAY);
                    break;
                case NPC_COUNCIL_KAZRAJIN:
                    Talk(TALK_KAZRAJIN_SLAY);
                    break;
                case NPC_COUNCIL_SUL_THE_SANDCRAWLER:
                    Talk(TALK_SUL_SLAY);
                    break;
                case NPC_COUNCIL_HIGH_PRIESTESS_MARLI:
                    Talk(TALK_MARLI_SLAY);
                    break;
                default:
                    break;
            }
        }

        // Helper function to prevent removing the possessed aura when no
        // other councillor is alive.
        bool IsACouncillorAlive() const
        {
            Creature    *pMalakk   = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_COUNCIL_FROST_KING_MALAKK) : ObjectGuid::Empty),
                        *pKazrajin = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_COUNCIL_KAZRAJIN) : ObjectGuid::Empty),
                        *pSul      = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_COUNCIL_SUL_THE_SANDCRAWLER) : ObjectGuid::Empty),
                        *pMarli    = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_COUNCIL_HIGH_PRIESTESS_MARLI) : ObjectGuid::Empty);

            // Pointers should not be null
            if (!pMalakk || !pKazrajin || !pSul || !pMarli)
                return false;

            switch (me->GetEntry())
            {
                case NPC_COUNCIL_FROST_KING_MALAKK:
                    return (!pKazrajin->HasAura(SPELL_PERMANENT_FEIGN_DEATH) || !pSul->HasAura(SPELL_PERMANENT_FEIGN_DEATH) || !pMarli->HasAura(SPELL_PERMANENT_FEIGN_DEATH));
                case NPC_COUNCIL_KAZRAJIN:
                    return (!pSul->HasAura(SPELL_PERMANENT_FEIGN_DEATH) || !pMarli->HasAura(SPELL_PERMANENT_FEIGN_DEATH) || !pMalakk->HasAura(SPELL_PERMANENT_FEIGN_DEATH));
                case NPC_COUNCIL_SUL_THE_SANDCRAWLER:
                    return (!pMarli->HasAura(SPELL_PERMANENT_FEIGN_DEATH) || !pMalakk->HasAura(SPELL_PERMANENT_FEIGN_DEATH) || !pKazrajin->HasAura(SPELL_PERMANENT_FEIGN_DEATH));
                case NPC_COUNCIL_HIGH_PRIESTESS_MARLI:
                    return (!pMalakk->HasAura(SPELL_PERMANENT_FEIGN_DEATH) || !pKazrajin->HasAura(SPELL_PERMANENT_FEIGN_DEATH) || !pSul->HasAura(SPELL_PERMANENT_FEIGN_DEATH));
                default:
                    return false;
            }
        }
    
        uint32 GetData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_DARK_POWER_COUNT:
                    return uiDarkPowerCount;
                case DATA_DAMAGES_PAST_SEC:
                    return uiDamagesDoneInPastSecs;
                default:
                    return std::find(playerGUIDs.begin(), playerGUIDs.end(), type) == playerGUIDs.end() ? 1 : 0;
            }

            return 0;
        }

    // Use the protected scope, so we do not have to declare again the same attributes
    protected:
        uint32 uiDarkPowerCount; // Internal counter to know the real amount of damages of Dark Power
        // Internal counter to know the amount of damages we have
        // taken since the possession began. When this amount is
        // equal or higher than 25% of our max health point, we
        // get rid of Garajal's Soul.
        uint32 uiDamageTakenPossessed;
        uint32 uiDamagesDoneInPastSecs;

        // Virtual methods used to init the events when fight begin
        // or possession begins / ends. Use pure virtual function to
        // be sure everybody overrides it.
        virtual void InitStandartEvents() = 0;
        virtual void InitPossessedEvents() = 0;

        // Method to init the DarkPower event, so there is no need
        // to repeat the same code each time.
        void InitDarkPower()
        {
            events.Reset();
            darkPowerEvents.Reset();
            darkPowerEvents.ScheduleEvent(EVENT_DARK_POWER, 1000);
        }
    
        uint32 GetPowerTimer() const
        {
            if (Aura* pLingeringPresence = me->GetAura(SPELL_LINGERING_PRESENCE))
            {
                float fReduce   = 1 - (pLingeringPresence->GetStackAmount() / 10);
                uint32 uiTimer  = (2 * IN_MILLISECONDS) * fReduce;
                return uiTimer;
            }
            else
                return 2 * IN_MILLISECONDS;
        }
};
typedef boss_council_of_elders_baseAI CouncilBaseAI;

// Frost King Malakk AI
class boss_frost_king_malakk : public CreatureScript
{
    public:
        boss_frost_king_malakk() : CreatureScript("boss_frost_king_malakk") { }

        struct boss_frost_king_malakkAI : public CouncilBaseAI
        {
            boss_frost_king_malakkAI(Creature* creature) : CouncilBaseAI(creature) { }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                frostBiteGUID = guid;
            }

            ObjectGuid GetGUID(int32 /*type*/) const override
            {
                return frostBiteGUID;
            }

            // No need to override Reset since there is nothing to reset here
            // No need to override JustEngagedWith since there is nothing to do here
            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                darkPowerEvents.Update(diff);
                events.Update(diff);

                switch (darkPowerEvents.ExecuteEvent())
                {
                    case EVENT_DARK_POWER:
                        DoCast(me, SPELL_DARK_POWER, true);
                        ++uiDarkPowerCount;
                        darkPowerEvents.ScheduleEvent(EVENT_DARK_POWER, 1 * IN_MILLISECONDS);
                        break;
                    case EVENT_INCREASE_POWER:
                        me->ModifyPower(POWER_MANA, 3);
                        if (me->GetPower(POWER_MANA) < 100)
                            darkPowerEvents.ScheduleEvent(EVENT_INCREASE_POWER, GetPowerTimer());
                        else
                            DoAction(ACTION_DARK_POWER);
                        break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 uiEventId = events.ExecuteEvent())
                {
                    switch (uiEventId)
                    {
                        case EVENT_FRIGID_ASSAULT:
                            DoCast(me, SPELL_FRIGID_ASSAULT);
                            events.ScheduleEvent(EVENT_FRIGID_ASSAULT, 30 * IN_MILLISECONDS);
                            break;
                        case EVENT_BITING_COLD:
                            DoCastAOE(SPELL_BITING_COLD); // Spell is wierd... handle target selection in SpellScript
                            events.ScheduleEvent(EVENT_BITING_COLD, Is25ManRaid() ? 25 * IN_MILLISECONDS : 45 * IN_MILLISECONDS);
                            break;
                        case EVENT_FROSTBITE:
                            Talk(TALK_SPECIAL);
                            DoCast(me, SPELL_FROSTBITE_SELECTOR, true);
                            events.ScheduleEvent(EVENT_FROSTBITE_EFF, 0.5 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_FROSTBITE, Is25ManRaid() ? 25 * IN_MILLISECONDS : 45 * IN_MILLISECONDS);
                            break;
                        case EVENT_FROSTBITE_EFF:
                            DoCast(me, SPELL_FROSTBITE);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff);
            }

        private:
            void InitStandartEvents() override
            {
                darkPowerEvents.Reset();
                events.Reset();

                events.ScheduleEvent(EVENT_FRIGID_ASSAULT, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BITING_COLD, 15 * IN_MILLISECONDS);
            }

            void InitPossessedEvents() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_FRIGID_ASSAULT, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_FROSTBITE, 15 * IN_MILLISECONDS);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_frost_king_malakkAI(creature);
        }
};

// Kazra'jin AI
class boss_kazrajin : public CreatureScript
{
    public:
        boss_kazrajin() : CreatureScript("boss_kazrajin") { }

        struct boss_kazrajinAI : public CouncilBaseAI
        {
            boss_kazrajinAI(Creature* creature): CouncilBaseAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                SetCombatMovement(false);
            }

            // Override reset to reset the amount of damages received, and the
            // movement flags.
            void Reset() override
            {
                instance = me->GetInstanceScript();
                uiDamagesDoneInPastSecs = 0;
                recklessChargeGUID = ObjectGuid::Empty;
                playerGUIDs.clear();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                CouncilBaseAI::Reset(); // Finalize Reset
            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                recklessChargeGUID = guid;
            }

            ObjectGuid GetGUID(int32 /*type*/) const override
            {
                return recklessChargeGUID;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_DAMAGES_PAST_SEC:
                        uiDamagesDoneInPastSecs = data;
                        break;
                    case DATA_RECKLESS_CHARGE_HIT:
                        playerGUIDs.push_back(data);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                darkPowerEvents.Update(diff);
                events.Update(diff);

                switch (darkPowerEvents.ExecuteEvent())
                {
                    case EVENT_DARK_POWER:
                        DoCast(me, SPELL_DARK_POWER, true);
                        ++uiDarkPowerCount;
                        darkPowerEvents.ScheduleEvent(EVENT_DARK_POWER, 1 * IN_MILLISECONDS);
                        break;
                    case EVENT_INCREASE_POWER:
                        me->ModifyPower(POWER_MANA, 3);
                        if (me->GetPower(POWER_MANA) < 100)
                            darkPowerEvents.ScheduleEvent(EVENT_INCREASE_POWER, GetPowerTimer());
                        else
                            DoAction(ACTION_DARK_POWER);
                        break;
                }

                if (me->HasAura(SPELL_OVERLOAD) || me->HasAura(SPELL_DISCHARGE))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RECKLESS_CHARGE_PRE_PATH:
                        {
                            playerGUIDs.clear();
                            DoCast(me, SPELL_RECKLESS_CHARGE, true);

                            events.ScheduleEvent(EVENT_RECKLESS_CHARGE, 2 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_RECKLESS_CHARGE:
                            events.ScheduleEvent(EVENT_RECKLESS_CHARGE_PRE_PATH, 7 * IN_MILLISECONDS);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                            if (rand() % 10 > 4)
                                Talk(TALK_KAZRAJIN_CHARGE);

                            DoCast(me, SPELL_RECKLESS_CHARGE_VISUAL); // Launch everything
                            // Summon npcs for the visual of Reckless Charge while travelling ?
                            // Handle next part in MovementInform.
                            break;
                    }
                }

                EnterEvadeIfOutOfCombatArea(diff);
            }

            void MovementInform(uint32 /*type*/, uint32 pointId) override
            {
                if (pointId == POINT_RECKLESS_CHARGE_LAND)
                {
                    me->RemoveAura(SPELL_RECKLESS_CHARGE_VISUAL);
                    DoCastAOE(SPELL_RECKLESS_CHARGE_SHATTER_GROUND);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                    if (me->HasAura(SPELL_POSSESSED))
                    {
                        Talk(TALK_SPECIAL);
                        if (IsHeroic())
                            DoCast(me, SPELL_DISCHARGE);
                        else
                        {
                            Talk(TALK_KAZRAJIN_OVERLOAD);
                            DoCast(me, SPELL_OVERLOAD);
                        }

                        events.RescheduleEvent(EVENT_RECKLESS_CHARGE_PRE_PATH, 1000 + rand() % 2000);
                    }
                    else
                        events.RescheduleEvent(EVENT_RECKLESS_CHARGE_PRE_PATH, 1000 + rand() % 2000);
                }
            }

        private:
            // Amount of damages received during past seconds in Heroic,
            // cause spell ticks each second, instead of being a permanent
            // dummy. Fuck blizzard's logic.
            ObjectGuid uiRecklessChargeTargetGUID;
            void InitStandartEvents() override
            {
                darkPowerEvents.Reset();
                events.Reset();

                events.ScheduleEvent(EVENT_RECKLESS_CHARGE_PRE_PATH, urand(3, 4) * IN_MILLISECONDS);
            }

            void InitPossessedEvents() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_RECKLESS_CHARGE_PRE_PATH, urand(3, 4) * IN_MILLISECONDS);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_kazrajinAI(creature);
        }
};

// Sul the Sandcrawler AI
class boss_sul_the_sandcrawler : public CreatureScript
{
    public:
        boss_sul_the_sandcrawler() : CreatureScript("boss_sul_the_sandcrawler") { }

        struct boss_sul_the_sandcrawlerAI : public CouncilBaseAI
        {
            boss_sul_the_sandcrawlerAI(Creature* creature) : CouncilBaseAI(creature) { }

            void AttackStart(Unit* target) override
            {
                if (!target)
                    return;

                if (me->Attack(target, false))
                    DoStartNoMovement(target);
            }

            void CastInterrupted(SpellInfo const* spell) override
            {
                events.RescheduleEvent(EVENT_SAND_BOLT, 6 * IN_MILLISECONDS);

                if (Unit* vict = me->GetVictim())
                    me->GetMotionMaster()->MoveChase(vict);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                darkPowerEvents.Update(diff);
                events.Update(diff);

                switch (darkPowerEvents.ExecuteEvent())
                {
                    case EVENT_DARK_POWER:
                        DoCast(me, SPELL_DARK_POWER, true);
                        ++uiDarkPowerCount;
                        darkPowerEvents.ScheduleEvent(EVENT_DARK_POWER, 1 * IN_MILLISECONDS);
                        break;
                    case EVENT_INCREASE_POWER:
                        me->ModifyPower(POWER_MANA, 3);
                        if (me->GetPower(POWER_MANA) < 100)
                            darkPowerEvents.ScheduleEvent(EVENT_INCREASE_POWER, GetPowerTimer());
                        else
                            DoAction(ACTION_DARK_POWER);
                        break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 uiEventId = events.ExecuteEvent())
                {
                    switch (uiEventId)
                    {
                        case EVENT_SAND_BOLT:
                            me->StopMoving();
                            me->GetMotionMaster()->MovementExpired();

                            if (Unit* vict = me->GetVictim())
                            {
                                if (me->GetExactDist2d(vict) > 45.0f)
                                    DoStartMovement(vict);
                                else
                                    DoStartNoMovement(vict);
                            }

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                DoCast(target, SPELL_SAND_BOLT);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 300.0f, true))
                                DoCast(target, SPELL_SAND_BOLT);

                            events.ScheduleEvent(EVENT_SAND_BOLT, 2.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_QUICKSAND:
                        {
                            Talk(TALK_SUL_QUICKSAND);
                            std::list<Player*> playerList;
                            std::list<Player*> tempList;
                            me->GetPlayerListInGrid(playerList, 150.f);

                            std::copy(std::begin(playerList), std::end(playerList), std::inserter(tempList, tempList.begin()));

                            playerList.remove_if ([this](Player const* player) -> bool
                            {
                                return this->me->GetExactDist2d(player) < 15.0f; // Remove players that are closer than 15 yards
                            });

                            // Pick one of the players in the list if not empty
                            if (!playerList.empty())
                            {
                                if (Player *player = Trinity::Containers::SelectRandomContainerElement<std::list<Player*>>(playerList))
                                    me->SummonCreature(NPC_LIVING_SAND, *player);
                            }
                            else if (!tempList.empty())
                            {
                                if (Player *player = Trinity::Containers::SelectRandomContainerElement<std::list<Player*>>(tempList))
                                    me->SummonCreature(NPC_LIVING_SAND, *player);
                            }

                            events.ScheduleEvent(EVENT_QUICKSAND, urand(20, 25) * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_SANDSTORM:
                            Talk(TALK_SPECIAL);
                            Talk(TALK_SUL_SANDSTORM);
                            DoCastAOE(SPELL_SAND_STORM);
                            events.ScheduleEvent(EVENT_SANDSTORM, 40 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff);
            }

        private:
            void InitStandartEvents() override
            {
                darkPowerEvents.Reset();
                events.Reset();

                events.ScheduleEvent(EVENT_SAND_BOLT, 5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_QUICKSAND, 10 * IN_MILLISECONDS);
            }

            void InitPossessedEvents() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_SAND_BOLT, 5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_QUICKSAND, 7 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SANDSTORM, 10 * IN_MILLISECONDS);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_sul_the_sandcrawlerAI(creature);
        }
};

// High Priestess Mar'li
class boss_high_priestess_marli : public CreatureScript
{
    public:
        boss_high_priestess_marli() : CreatureScript("boss_high_priestess_marli") { }

        struct boss_high_priestess_marli_AI : public CouncilBaseAI
        {
            boss_high_priestess_marli_AI(Creature* creature) : CouncilBaseAI(creature) { }

            // Override Reset() to clean the lists
            void Reset() override
            {
                uiShadowedSpiritPlayerGUIDs.clear();
                uiBlessedLoaSpiritBossGUIDs.clear();

                CouncilBaseAI::Reset();
            }

            void AttackStart(Unit* target) override
            {
                if (!target)
                    return;

                if (me->Attack(target, false))
                    DoStartNoMovement(target);
            }

            void CastInterrupted(SpellInfo const* spell) override
            {
                events.RescheduleEvent(me->HasAura(SPELL_POSSESSED) ? EVENT_WRATH_OF_THE_LOA_DARK : EVENT_WRATH_OF_THE_LOA, 6 * IN_MILLISECONDS);

                if (Unit* vict = me->GetVictim())
                    me->GetMotionMaster()->MoveChase(vict);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                darkPowerEvents.Update(diff);
                events.Update(diff);

                switch (darkPowerEvents.ExecuteEvent())
                {
                    case EVENT_DARK_POWER:
                        DoCast(me, SPELL_DARK_POWER, true);
                        ++uiDarkPowerCount;
                        darkPowerEvents.ScheduleEvent(EVENT_DARK_POWER, 1 * IN_MILLISECONDS);
                        break;
                    case EVENT_INCREASE_POWER:
                        me->ModifyPower(POWER_MANA, 3);
                        if (me->GetPower(POWER_MANA) < 100)
                            darkPowerEvents.ScheduleEvent(EVENT_INCREASE_POWER, GetPowerTimer());
                        else
                            DoAction(ACTION_DARK_POWER);
                        break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 uiEventId = events.ExecuteEvent())
                {
                    switch (uiEventId)
                    {
                        case EVENT_WRATH_OF_THE_LOA:
                            me->StopMoving();
                            me->GetMotionMaster()->MovementExpired();

                            if (Unit* vict = me->GetVictim())
                            {
                                if (me->GetExactDist2d(vict) > 45.0f)
                                    DoStartMovement(vict);
                                else
                                    DoStartNoMovement(vict);
                            }

                            if (me->GetVictim() && !me->GetVictim()->ToPet())
                                DoCast(me->GetVictim(), SPELL_WRATH_OF_THE_LOA);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                DoCast(target, SPELL_WRATH_OF_THE_LOA);

                            events.ScheduleEvent(EVENT_WRATH_OF_THE_LOA, 2.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_BLESSED_LOA_SPIRIT:
                            // Check that we are not the only left councillor (otherwise it would be cheaty)
                            for (uint8 i = 0; i < 2; i++)
                            {
                                if (Creature* pCouncillor = GetBossByEntry(uiBossEntries[i], me))
                                {
                                    if (!pCouncillor->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                                    {
                                        // Cast after having init the list to be sure the guid has been set
                                        DoCast(me, SPELL_BLESSED_LOA_SPIRIT);
                                        events.ScheduleEvent(EVENT_BLESSED_LOA_SPIRIT, urand(20, 28) * IN_MILLISECONDS);


                                        // Get out of the loop and break again; this way, we do not schedule
                                        // the event if there is no other boss than Mar'li alive.
                                        break;
                                    }
                                }
                            }
                            break;
                        case EVENT_WRATH_OF_THE_LOA_DARK:
                            me->StopMoving();

                            if (Unit* vict = me->GetVictim())
                            {
                                if (me->GetExactDist2d(vict) > 45.0f)
                                    DoStartMovement(vict);
                                else
                                    DoStartNoMovement(vict);
                            }

                            if (me->GetVictim() && !me->GetVictim()->ToPet())
                                DoCast(me->GetVictim(), SPELL_WRATH_OF_THE_LOA_DARK);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                DoCast(target, SPELL_WRATH_OF_THE_LOA_DARK);

                            events.ScheduleEvent(EVENT_WRATH_OF_THE_LOA_DARK, 2.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_SHADOWED_LOA_SPIRIT:
                        {
                            Talk(TALK_SPECIAL);
                            DoCast(me, SPELL_SHADOWED_LOA_SPIRIT);
                            events.ScheduleEvent(EVENT_SHADOWED_LOA_SPIRIT, urand(15, 25) * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_TWISTED_FATE:
                            DoCastAOE(SPELL_TWISTED_FATE); // Automatically handle target selection in the SpellScript
                            events.ScheduleEvent(EVENT_TWISTED_FATE, 30 * IN_MILLISECONDS);
                            break;
                    }
                }

                if (!DoSpellAttackIfReady(me->HasAura(SPELL_POSSESSED) ? SPELL_WRATH_OF_THE_LOA_DARK : SPELL_WRATH_OF_THE_LOA))
                    DoMeleeAttackIfReady();

                EnterEvadeIfOutOfCombatArea(diff);
            }

            // Override function to return the GUIDs of the targets for the Loa Spirits.
            ObjectGuid GetGUID(int32 iIndex) const override
            {
                switch (iIndex)
                {
                    case DATA_BLESSED_LOA_SPIRIT_TARGET_GUID:
                        if (uiBlessedLoaSpiritBossGUIDs.empty())
                            return ObjectGuid::Empty;
                        else
                            return uiBlessedLoaSpiritBossGUIDs.back();
                        break;
                    case DATA_SHADOWED_LOA_SPIRIT_TARGET_GUID:
                        if (uiShadowedSpiritPlayerGUIDs.empty())
                            return ObjectGuid::Empty;
                        else
                            return uiShadowedSpiritPlayerGUIDs.back();
                        break;
                    default:
                        return ObjectGuid::Empty;
                }
            }

        private:
            // GUIDs of the players that will be chased by a Shadowed Loa Spirit
            // Always select the last in the list when a Shadowed Loa Spirit
            // is summoned.
            std::list<ObjectGuid> uiShadowedSpiritPlayerGUIDs;

            // GUIDS of the boss that will be chased by a Blessed Loa Spirit.
            // Like above, select the last in the list when a Blessed Loa Spirit
            // is summoned.
            std::list<ObjectGuid> uiBlessedLoaSpiritBossGUIDs;

            void InitStandartEvents() override
            {
                darkPowerEvents.Reset();
                events.Reset();

                events.ScheduleEvent(EVENT_WRATH_OF_THE_LOA, 5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BLESSED_LOA_SPIRIT, 25 * IN_MILLISECONDS);
            }

            void InitPossessedEvents() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_WRATH_OF_THE_LOA_DARK, 5 * IN_MILLISECONDS);
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_TWISTED_FATE, 25 * IN_MILLISECONDS);
                else
                    events.ScheduleEvent(EVENT_SHADOWED_LOA_SPIRIT, 20 * IN_MILLISECONDS);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_high_priestess_marli_AI(creature);
        }
};

// Garajal
class npc_garajal : public CreatureScript
{
    public:
        npc_garajal() : CreatureScript("npc_garajal") { }
        
        struct npc_garajal_AI : public BossAI
        {
            npc_garajal_AI(Creature* creature) : BossAI(creature, DATA_COUNCIL_OF_ELDERS)
            {
                me->SetReactState(REACT_PASSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                events.Reset();
                instance = me->GetInstanceScript();
                introDone = false;
                atEvade   = false;
            }

            TaskScheduler scheduler;
            std::list<ObjectGuid> m_lBossGuids;
            uint32 m_uiDeadCouncillors;
            bool introDone, atEvade;
            bool allowAchiev;

            void Reset() override 
            {
                atEvade = false;
                allowAchiev = true;
            }

            void SetData(uint32 /*type*/, uint32 data) override
            {
                m_uiDeadCouncillors += data;
            }

            uint32 GetData(uint32 type) const override
            {
                return m_uiDeadCouncillors;
            }

            void EnterEvadeMode() override
            {
                if (atEvade)
                    return;

                atEvade = true;
                _EnterEvadeMode();
                ResetFight();

                scheduler.CancelAll();

                if (instance)
                {
                    instance->SetBossState(DATA_COUNCIL_OF_ELDERS, FAIL);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_QUICKSAND_PERIODIC_DAMAGES);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FRIGID_ASSAULT_STACK);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FRIGID_ASSAULT_STUN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BITING_COLD_PERIODIC_DAMAGES);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FROSTBITE_PERIODIC_DAMAGES);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FROSTBITE_SCREEN_EFFECT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHADOWED_SOUL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SOUL_FRAGMENT_PERIODIC);
                }

                BossAI::EnterEvadeMode();
                _DespawnAtEvade();
            }

            void JustDied(Unit* /*killer*/) override
            {
                scheduler.CancelAll();
                _JustDied();

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_QUICKSAND_PERIODIC_DAMAGES);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FRIGID_ASSAULT_STACK);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FRIGID_ASSAULT_STUN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BITING_COLD_PERIODIC_DAMAGES);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FROSTBITE_PERIODIC_DAMAGES);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FROSTBITE_SCREEN_EFFECT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHADOWED_SOUL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SOUL_FRAGMENT_PERIODIC);

                    if (allowAchiev)
                    {
                        for (auto&& itr : instance->instance->GetPlayers())
                            if (Player* target = itr.GetSource())
                                target->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_CAGE_MATCH);
                    }
                }

                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                events.Reset();

                for (auto&& entry : uiBossEntries)
                {
                    if (Creature* councillor = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(entry) : ObjectGuid::Empty))
                    {
                        councillor->LowerPlayerDamageReq(councillor->GetMaxHealth());
                        councillor->RemoveAura(SPELL_PERMANENT_FEIGN_DEATH);

                        if (Player* killer = me->FindNearestPlayer(200.0f))
                            killer->Kill(councillor);
                    }
                }

                if (Player* player = me->GetMap()->GetFirstPlayerInInstance())
                    player->RewardPersonalLootAndCurrency(me);

                instance->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, NPC_GARA_JAL, me);
            }

            void ResetFight()
            {
                if (!me->IsAlive())
                    return;

                FillBossGuids(m_lBossGuids);

                summons.DespawnAll();

                me->SetVisible(true);
                events.Reset();

                for (auto&& uiGuid : m_lBossGuids)
                {
                    if (Creature* pBoss = ObjectAccessor::GetCreature(*me, uiGuid))
                    {
                        if (atEvade)
                            pBoss->AI()->EnterEvadeMode();
                        else
                        {
                            float x, y, z, o;
                            pBoss->GetHomePosition(x, y, z, o);
                            pBoss->NearTeleportTo(x, y, z, o);
                            pBoss->Respawn();
                            pBoss->SetFacingTo(o);
                        }
                    }
                }

                DespawnCreatures();
            }

            void FillBossGuids(std::list<ObjectGuid>&list)
            {
                m_uiDeadCouncillors = 0;

                list.clear();

                list.push_back(instance->GetGuidData(NPC_COUNCIL_FROST_KING_MALAKK));
                list.push_back(instance->GetGuidData(NPC_COUNCIL_HIGH_PRIESTESS_MARLI));
                list.push_back(instance->GetGuidData(NPC_COUNCIL_SUL_THE_SANDCRAWLER));
                list.push_back(instance->GetGuidData(NPC_COUNCIL_KAZRAJIN));
            }

            void FinishFight()
            {
                JustDied(me);
                DespawnCreatures();
            }

            void BeginFight()
            {
                if (instance && instance->GetBossState(DATA_COUNCIL_OF_ELDERS) == IN_PROGRESS)
                    return;

                m_uiDeadCouncillors = 0;

                instance->SetBossState(DATA_COUNCIL_OF_ELDERS, IN_PROGRESS);

                FillBossGuids(m_lBossGuids);

                for (const ObjectGuid uiGuid : m_lBossGuids)
                {
                    if (Creature* pBoss = ObjectAccessor::GetCreature(*me, uiGuid))
                    {
                        if (!pBoss->IsInCombat() && pBoss->AI())
                        {
                            DoZoneInCombat(pBoss, 150.f);
                            pBoss->AI()->DoAction(ACTION_COUNCILLORS_ENTER_COMBAT);
                        }
                    }
                }

                scheduler
                    .Schedule(Milliseconds(5000), [this](TaskContext context)
                {
                    if (HasAnyBehind())
                        allowAchiev = false;

                    if (allowAchiev)
                        context.Repeat(Seconds(1));
                });

                events.ScheduleEvent(EVENT_SUMMON_SOUL, 3 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_FIGHT_BEGIN:
                        BeginFight();
                        break;
                    case ACTION_FIGHT_RESET:
                        ResetFight();
                        break;
                    case ACTION_COUNCILLOR_DIED:
                        ++m_uiDeadCouncillors;

                        if (m_uiDeadCouncillors > 3)
                            FinishFight();
                        break;
                    case ACTION_COUNCIL_INTRO:
                        if (!introDone )
                        {
                            introDone = true;

                            if (instance && instance->GetData(DATA_COUNCIL_EVENT) == DONE)
                                break;

                            if (instance)
                                instance->SetData(DATA_COUNCIL_EVENT, DONE);

                            Talk(TALK_INTRO_1);

                            me->m_Events.Schedule(8200, 1, [this]()
                            {
                                Talk(TALK_INTRO_2);
                            });
                        }
                        break;
                    default:
                        break;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            inline void DespawnCreatures() const
            {
                DespawnCreaturesByEntry(NPC_LIVING_SAND);
                DespawnCreaturesByEntry(NPC_BLESSED_LOA_SPIRIT);
                DespawnCreaturesByEntry(NPC_SHADOWED_LOA_SPIRIT);
                DespawnCreaturesByEntry(NPC_TWISTED_FATE_FIRST);
                DespawnCreaturesByEntry(NPC_TWISTED_FATE_SECOND);
            }

            void DespawnCreaturesByEntry(uint32 uiEntry) const
            {
                std::list<Creature*> minionsList;
                GetCreatureListWithEntryInGrid(minionsList, me, uiEntry, 500.0f);

                for (Creature* pMinion : minionsList)
                    pMinion->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_SUMMON_SOUL)
                    {
                        Talk(0);
                        me->SetVisible(false);

                        if (Creature* pSoul = me->SummonCreature(NPC_GARA_JALS_SOUL, *me))
                            instance->SetData64(NPC_GARA_JALS_SOUL, pSoul->GetGUID());
                    }
                    break;
                }
            }
        private:
            EventMap events;
            InstanceScript* instance;

            bool HasAnyBehind()
            {
                if (instance)
                {
                    for (auto&& itr : instance->instance->GetPlayers())
                        if (Player* target = itr.GetSource())
                            if (target->GetExactDist2d(councilRingCenter.GetPositionX(), councilRingCenter.GetPositionY()) > 40.0f)
                                return true;
                }

                return false;
            }
        };
        
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_garajal_AI(creature);
        }
};

class guidVectorPredicate
{
    public:
        guidVectorPredicate(uint64 guid) : _guid(guid) { }

        bool operator()(uint64 uiGuid) const
        {
            return uiGuid == _guid;
        }
    private:
        ObjectGuid _guid;
};

// Reckless Charge 69453
struct npc_council_reckless_charge : public ScriptedAI
{
    npc_council_reckless_charge(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetDisplayFromModel(1);

        if (Creature* kazrajin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_COUNCIL_KAZRAJIN) : ObjectGuid::Empty))
        {
            me->SetFacingTo(me->GetAngle(kazrajin));

            float dist = kazrajin->GetExactDist2d(me) - 3.0f; // Remove 5 yards to continue rolling
            float x, y;

            uint8 pointCount = ((uint8)dist / 3) + 1;

            for (uint8 i = 0; i < pointCount; ++i)
            {
                GetPositionWithDistInOrientation(kazrajin, dist - (i * 3), Position::NormalizeOrientation(me->GetOrientation() + M_PI), x, y);
                me->SendPlaySpellVisual(SPELL_VISUAL_ID_RECKLESS_CHARGE, x, y, kazrajin->GetMap()->GetHeight(x, y, kazrajin->GetPositionZ() + 0.8f), 2.75f);
            }
        }
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Garajal's Soul 69182
struct npc_garajals_soul : public ScriptedAI
{
    npc_garajals_soul(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    InstanceScript* instance;
    std::list<uint64> possessedTargets;
    uint32 prevCouncillor;
    uint32 fragmentCounter;
    bool firstWaveDone;

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetDisplayId(11686);
        instance = me->GetInstanceScript();
        firstWaveDone = false;
        prevCouncillor = 0;
        fragmentCounter = 2;
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        DoCast(me, SPELL_GARA_JALS_SOUL, true);
        possessedTargets = { NPC_COUNCIL_FROST_KING_MALAKK, NPC_COUNCIL_HIGH_PRIESTESS_MARLI, NPC_COUNCIL_KAZRAJIN, NPC_COUNCIL_SUL_THE_SANDCRAWLER };
        HandlePossessedIntoLeader();
    }

    void HandlePossessedIntoLeader()
    {
        if (possessedTargets.empty())
        {
            possessedTargets = { NPC_COUNCIL_FROST_KING_MALAKK, NPC_COUNCIL_HIGH_PRIESTESS_MARLI, NPC_COUNCIL_SUL_THE_SANDCRAWLER, NPC_COUNCIL_KAZRAJIN };

            // Random at wave end (fucking list issue with shuffle)
            std::vector<uint64> buf(possessedTargets.begin(), possessedTargets.end());
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(buf.begin(), buf.end(), g);            
            possessedTargets.clear();
            std::copy(buf.begin(), buf.end(), std::inserter(possessedTargets, possessedTargets.begin()));

            firstWaveDone = true;
        }

        if (firstWaveDone)
        {
            if (prevCouncillor)
                possessedTargets.erase(std::find(possessedTargets.begin(), possessedTargets.end(), prevCouncillor));

            possessedTargets.remove_if([=](uint64 councillorEntry) { return instance && ObjectAccessor::GetCreature(*me, instance->GetGuidData(councillorEntry)) && ObjectAccessor::GetCreature(*me, instance->GetGuidData(councillorEntry))->HasAura(SPELL_PERMANENT_FEIGN_DEATH); });

            if (possessedTargets.empty())
            {
                events.ScheduleEvent(EVENT_REPOSSSESS, 0.5 * IN_MILLISECONDS);
                return;
            }

            prevCouncillor = Trinity::Containers::SelectRandomContainerElement(possessedTargets);

            // Remove before init next if it`s last element for form list
            if (possessedTargets.size() == 1)
                possessedTargets.clear();
        }
        else // In Begun case
        {
            possessedTargets.remove_if([=](uint64 councillorEntry) { return instance && ObjectAccessor::GetCreature(*me, instance->GetGuidData(councillorEntry)) && ObjectAccessor::GetCreature(*me, instance->GetGuidData(councillorEntry))->HasAura(SPELL_PERMANENT_FEIGN_DEATH); });

            if (possessedTargets.empty())
            {
                events.ScheduleEvent(EVENT_REPOSSSESS, 0.5 * IN_MILLISECONDS);
                return;
            }

            prevCouncillor = possessedTargets.front();
            possessedTargets.pop_front();
        }

        if (Creature* councillor = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(prevCouncillor) : ObjectGuid::Empty))
            Possess(councillor);
    }

    void Possess(Unit* creature)
    {
        DoCast(creature, SPELL_POSSESSED);
        me->EnterVehicle(creature, 0);
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_ENTER_COMBAT:
                // Always possess Malakk first
                HandlePossessedIntoLeader();
                break;
            case ACTION_EXIT_COUNCILLOR:
                // Set Garajal visible again
                DoCast(SPELL_LINGERING_PRESENCE);
                me->ExitVehicle();
            
                // In Heroic, each time Garajal is forced out of a councillor, he leaves
                // a Soul Fragment behind. (In fact there is no npc summoned, just a spell
                // cast).
                if (IsHeroic() && Is25ManRaid())
                {
                    if (Creature* pGarajal = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_COUNCIL_OF_ELDERS) : ObjectGuid::Empty))
                        pGarajal->CastSpell(pGarajal, SPELL_SOUL_FRAGMENT_SELECTOR, true);
                }
                else if (IsHeroic() && fragmentCounter % 2 == 0) // through time in 10 ppl
                {
                    if (Creature* pGarajal = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_COUNCIL_OF_ELDERS) : ObjectGuid::Empty))
                        pGarajal->CastSpell(pGarajal, SPELL_SOUL_FRAGMENT_SELECTOR, true);
                }
            
                fragmentCounter++;
                // Select a new councillor
                HandlePossessedIntoLeader();
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_REPOSSSESS)
                HandlePossessedIntoLeader();
            break;
        }
    }
};

// Living Sand AI
class npc_living_sand : public CreatureScript
{
    public:
        npc_living_sand() : CreatureScript("npc_living_sand") { }

        struct npc_living_sandAI : public ScriptedAI
        {
            npc_living_sandAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

            EventMap events;

            void IsSummonedBy(Unit* summoner) override
            {
                me->AddAura(SPELL_QUICKSAND_AT_VISUAL_INIT, me);
                Initialize();
            }

            void Reset() override { }

            void EnterEvadeMode() override { }
            
            void Initialize()
            {
                me->RemoveAurasDueToSpell(SPELL_FORTIFIED);
                me->SetHealth(me->GetMaxHealth());
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                me->PrepareChanneledCast(me->GetOrientation());
                me->GetMotionMaster()->MovementExpired();
                me->GetMotionMaster()->MoveIdle();
                events.ScheduleEvent(EVENT_QUICKSAND_PERIODIC, 500);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_CREATE_LIVING_SAND:
                        if (!me->HasAura(SPELL_QUICKSAND_AT_VISUAL) && !me->HasAura(SPELL_QUICKSAND_AT_VISUAL_INIT))
                            DoCast(me, SPELL_FORTIFIED, true);
                        else
                        {
                            me->RemoveAura(SPELL_QUICKSAND_AT_VISUAL_INIT, 0, 0, AURA_REMOVE_BY_EXPIRE);
                            me->RemoveAura(SPELL_QUICKSAND_AT_VISUAL, 0, 0, AURA_REMOVE_BY_EXPIRE);
                            me->RemoveAllAuras();
                            events.ScheduleEvent(EVENT_ACTIVATE_SAND, 4000);
                            events.CancelEvent(EVENT_QUICKSAND_PERIODIC);
                        }
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 uiEventId = events.ExecuteEvent())
                {
                    switch (uiEventId)
                    {
                        case EVENT_QUICKSAND_PERIODIC:
                        {
                            std::list<Player*> playerList;
                            me->GetPlayerListInGrid(playerList, 100.f);

                            for (Player *player : playerList)
                            {
                                if (player->GetExactDist2d(me) < (7.4f + me->GetFloatValue(UNIT_FIELD_BOUNDING_RADIUS)))
                                {
                                    if (Aura* aura = player->GetAura(SPELL_QUICKSAND_PERIODIC_DAMAGES, me->GetGUID()))
                                    {
                                        aura->RefreshDuration();
                                    }
                                    else
                                        me->AddAura(SPELL_QUICKSAND_PERIODIC_DAMAGES, player);
                                }
                                else
                                    player->RemoveAura(SPELL_QUICKSAND_PERIODIC_DAMAGES, me->GetGUID(), 0, AURA_REMOVE_BY_EXPIRE);
                                    // Handle the root and cie in the AuraScript
                            }

                            events.ScheduleEvent(EVENT_QUICKSAND_PERIODIC, 500);
                            break;
                        }
                        case EVENT_ACTIVATE_SAND:
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                            if (!me->IsInCombat())
                                DoZoneInCombat(me, 100.f);
                            else if (me->GetVictim())
                                me->GetMotionMaster()->MoveChase(me->GetVictim());
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                AttackStart(target);
                            break;
                    }
                }

                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE) || !UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
            
            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
                {
                    damage = 0;
                    return;
                }

                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    DoCast(me, SPELL_QUICKSAND_AT_VISUAL, true);
                    Initialize();
                }
            }

        private:
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_living_sandAI(creature);
        }
};

// Blessed Loa Spirit 69480, 69491, 69492
struct npc_blessed_loa_spirit : public ScriptedAI
{
    npc_blessed_loa_spirit(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) 
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
    }

    void IsSummonedBy(Unit* summoner) override
    {
        uiTargetGuid = ObjectGuid::Empty;
        me->SetReactState(REACT_PASSIVE);
        me->SetSpeed(MOVE_RUN, 0.85f);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
        events.Reset();
        DoCast(me, SPELL_BLESSED_TRANSFORMATION);
        HandleTargetSelection();

        events.RescheduleEvent(EVENT_BLESSED_GIFT, 20 * IN_MILLISECONDS);
        events.RescheduleEvent(EVENT_MOVE_COUNCILLOR, 500);
    }

    // Override function to be sure there won't be any call to MoveChase (at least in AttackStart)
    void AttackStart(Unit* target) override { }

    void JustDied(Unit* /*killer*/) override
    {
        me->DespawnOrUnsummon(5000);
    }

    void Move()
    {
        if (Creature* pCouncillor = ObjectAccessor::GetCreature(*me, uiTargetGuid))
        {
            if (pCouncillor->IsAlive())
            {
                if (me->GetExactDist2d(pCouncillor) <= 5.0f)
                {
                    DoCast(pCouncillor, SPELL_BLESSED_GIFT);
                    me->DisappearAndDie();
                    return;
                }

                me->GetMotionMaster()->MoveFollow(pCouncillor, 0.0f, me->GetAngle(pCouncillor));
            }
            else
                HandleTargetSelection();
        }
        else
            HandleTargetSelection();

        events.ScheduleEvent(EVENT_MOVE_COUNCILLOR, 0.5 * IN_MILLISECONDS);
    }

    void HandleTargetSelection()
    {
        // Select a target now
        std::list<Creature*> councillors = { ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_COUNCIL_FROST_KING_MALAKK) : ObjectGuid::Empty), ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_COUNCIL_KAZRAJIN) : ObjectGuid::Empty), ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_COUNCIL_SUL_THE_SANDCRAWLER) : ObjectGuid::Empty) };
        councillors.remove_if([](Creature const* pCouncil) -> bool { return pCouncil->HasAura(SPELL_PERMANENT_FEIGN_DEATH); });

        if (councillors.empty())
        {
            me->DespawnOrUnsummon();
            return;
        }

        councillors.sort([](Creature const* first, Creature const* second) -> bool { return first->GetHealthPct() < second->GetHealthPct(); });

        if (councillors.front())
            uiTargetGuid = councillors.front()->GetGUID();
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_MOVE_COUNCILLOR:
                    Move();
                    break;
                case EVENT_BLESSED_GIFT:
                    if (Creature* pCouncillor = ObjectAccessor::GetCreature(*me, uiTargetGuid))
                    {
                        DoCast(me, SPELL_BLESSED_TIME_OUT);
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->MoveJump(*pCouncillor, 42.0f, 42.0f, EVENT_JUMP);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            if (Creature* pCouncillor = ObjectAccessor::GetCreature(*me, uiTargetGuid))
            {
                DoCast(pCouncillor, SPELL_BLESSED_GIFT);
                me->DisappearAndDie();
            }
        }
    }

    private:
        EventMap events;
        InstanceScript* instance;
        ObjectGuid uiTargetGuid; // GUID of the councillor we are moving toward
};

// Shadowed Loa Spirit 69548
struct npc_shadowed_loa_spirit : public ScriptedAI
{
    npc_shadowed_loa_spirit(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()), uiTargetGuid()
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
    }

    // Override function to be sure there won't be any call to MoveChase (at least in AttackStart)
    void AttackStart(Unit* target) override { }

    void IsSummonedBy(Unit* summoner) override
    {
        uiTargetGuid = ObjectGuid::Empty;
        me->SetReactState(REACT_PASSIVE);
        me->SetSpeed(MOVE_RUN, 0.85f);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
        events.Reset();
        DoCast(me, SPELL_SHADOWED_TRANSFORMATION);
        DoCast(me, SPELL_MARKED_SOUL, true);

        events.ScheduleEvent(EVENT_MOVE_COUNCILLOR, 500);
        events.ScheduleEvent(EVENT_SHADOWED_GIFT, 20 * IN_MILLISECONDS);
    }

    void Move()
    {
        if (Unit* target = ObjectAccessor::GetUnit(*me, uiTargetGuid))
        {
            if (target->IsAlive())
            {
                if (me->GetExactDist2d(target) <= 5.0f)
                {
                    DoCast(target, SPELL_SHADOWED_GIFT, true);
                    target->RemoveAurasDueToSpell(SPELL_MARKED_SOUL, me->GetGUID());
                    me->DisappearAndDie();
                    return;
                }

                me->GetMotionMaster()->MoveFollow(target, 0.0f, me->GetAngle(target));
            }
            else
            {
                if (Aura* aura = target->GetAura(SPELL_MARKED_SOUL, me->GetGUID()))
                    aura->Remove(AURA_REMOVE_BY_DEATH);

                DoCast(me, SPELL_MARKED_SOUL, true);
            }
        }
        else
            DoCast(me, SPELL_MARKED_SOUL, true);

        events.ScheduleEvent(EVENT_MOVE_COUNCILLOR, 0.5 * IN_MILLISECONDS);
    }

    void SetGUID(ObjectGuid guid, int32 /*value*/) override
    {
        uiTargetGuid = guid;
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_MOVE_COUNCILLOR:
                    Move();
                    break;
                case EVENT_SHADOWED_GIFT:
                    if (Player* target = ObjectAccessor::GetPlayer(*me, uiTargetGuid))
                    {
                        if (target->IsAlive())
                        {
                            DoCast(me, SPELL_SHADOWED_TIME_OUT);
                            me->GetMotionMaster()->MovementExpired();
                            me->GetMotionMaster()->MoveJump(*target, 42.0f, 42.0f, 5050);
                            return;
                        }
                    }
                    break;
            }
        }
    }

    void MovementInform(uint32 /*type*/, uint32 pointId) override
    {

        if (pointId == 5050)
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, uiTargetGuid))
            {
                DoCast(player, SPELL_SHADOWED_GIFT);
                player->RemoveAurasDueToSpell(SPELL_MARKED_SOUL, me->GetGUID());
                me->DisappearAndDie();
            }
        }

    }

    void JustDied(Unit* killer) override
    {
        if (Player *player = ObjectAccessor::GetPlayer(*me, uiTargetGuid))
            player->RemoveAurasDueToSpell(SPELL_MARKED_SOUL);

        me->DespawnOrUnsummon(5000);
    }

    private:
        EventMap events;
        InstanceScript* instance;
        ObjectGuid uiTargetGuid; // GUID of the councillor we are moving toward
};

// Twisted Fate 69740, 69746
struct npc_twisted_fate : public ScriptedAI
{
    npc_twisted_fate(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript())
    {
        me->SetReactState(REACT_PASSIVE);
    }

    ObjectGuid copyGUID;
    // Add a new TwistedFate_t to the helper
    void IsSummonedBy(Unit* summoner) override
    {
        me->SetWalk(true);

        if (Creature* garajalCouncil = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_COUNCIL_OF_ELDERS) : ObjectGuid::Empty))
            garajalCouncil->AI()->JustSummoned(me);

        if (me->GetEntry() == NPC_TWISTED_FATE_FIRST)
        {
            summoner->CastSpell(summoner, SPELL_TWISTED_FATE_CLONE_FIRST, true);
            DoCast(me, SPELL_TWISTED_FATE_FORCE_SUMMON_SECOND, true);
            DoCast(me, SPELL_TWISTED_FATE_PERIODIC, true);
        }
        else
        {
            summoner->CastSpell(summoner, SPELL_TWISTED_FATE_CLONE_SECOND, true);
            DoCast(me, SPELL_TWISTED_FATE_VISUAL, true);
            DoCast(me, SPELL_TWISTED_FATE_PERIODIC, true);
            
            if (Creature* secondCopy = me->FindNearestCreature(NPC_TWISTED_FATE_FIRST, 150.0f, true))
            {
                copyGUID = secondCopy->GetGUID();
                secondCopy->AI()->SetGUID(me->GetGUID());
                me->GetMotionMaster()->MoveFollow(secondCopy, 0.0f, me->GetAngle(secondCopy));
                secondCopy->GetMotionMaster()->MoveFollow(me, 0.0f, secondCopy->GetAngle(me));
            }
        }
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        copyGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return copyGUID;
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DisappearAndDie();
    }

    void JustSummoned(Creature* summon) override
    {
        copyGUID = summon->GetGUID();
    }

    protected:
        InstanceScript* instance;
        ObjectGuid uiOtherTwistedFateGuid; // Guid of the other twisted fate
    
        // Boolean to control whenever we are in the linked phase or not,
        // and used to compute correctly the amount of damages of the
        // SPELL_TWISTED_FATE_DAMAGES.
        bool bOtherTwistedFateDied;
};

// Possessed 136442
class spell_garajal_possessed : public AuraScript
{
    PrepareAuraScript(spell_garajal_possessed)

    void HandleApply(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_SET_POSSESSED);
    }

    void HandleRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_SET_UNPOSSESSED);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_garajal_possessed::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_garajal_possessed::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Frigid Assault
class spell_malakk_frigid_assault : public SpellScriptLoader
{
    public:
        spell_malakk_frigid_assault() : SpellScriptLoader("spell_malakk_frigid_assault") { }

        class spell_malakk_frigid_assault_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_malakk_frigid_assault_AuraScript)

            void HandleEffectApply(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
            {
                Player *pOwner;
                if (GetOwner() && GetOwner()->ToPlayer())
                {
                    pOwner = GetOwner()->ToPlayer();
                    // Stun when stacks reach 15
                    if (GetStackAmount() == 15)
                    {
                        pOwner->CastSpell(pOwner, SPELL_FRIGID_ASSAULT_STUN, true);
                        Remove(AURA_REMOVE_BY_DEFAULT);
                    }
                }
            }

            void Register() override
            {
                // BTW: Periodic dummy choice was quite surprising...
                // Note: better to use AfterEffectApply, cause stack amount is not yet set to 15 when OnEffectApply is called
                AfterEffectApply += AuraEffectApplyFn(spell_malakk_frigid_assault_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK); // Indeed, it should be AfterEffectReapply
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_malakk_frigid_assault_AuraScript();
        }
};

// Biting Cold 136917
class spell_malakk_biting_cold : public SpellScript
{
    PrepareSpellScript(spell_malakk_biting_cold)

    std::list<WorldObject*> m_targets;

    void HandleEffectHitTarget(SpellEffIndex effIndex)
    {
        // Since spell selects one random target, there is no way we can handle this another way
        if (Unit* pHit = GetHitUnit())
        {
            if (Unit* caster = GetCaster())
            {
                caster->CastSpell(pHit, SPELL_BITING_COLD_PERIODIC_DAMAGES, true);
                caster->CastSpell(pHit, SPELL_BITING_COLD_PERIODIC_DUMMY, true);
            }
        }
    }

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        m_targets = targets;
        targets.remove_if(TankSpecTargetSelector());

        // Max Affect Targets 1 - not need smth more
        if (!targets.empty())
            return;

        if (!m_targets.empty())
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_malakk_biting_cold::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malakk_biting_cold::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Frostbite (Periodic)
class spell_malakk_frostbite_periodic : public SpellScriptLoader
{
    public:
        spell_malakk_frostbite_periodic() : SpellScriptLoader("spell_malakk_frostbite_periodic") { }

        class spell_malakk_frostbite_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_malakk_frostbite_periodic_AuraScript)

            void HandlePeriodic(AuraEffect const* pAuraEffect)
            {
                if (!GetOwner())
                    return;

                // Stack amount can be reduced when players are standing close to the owner
                if (Player *pOwner = GetOwner()->ToPlayer())
                {
                    std::list<Player*> playerList;
                    pOwner->GetPlayerListInGrid(playerList, 4.0f);

                    playerList.remove(pOwner); // Remove self
                    playerList.remove_if ([](Player const* player) -> bool { return player->HasAura(SPELL_CHILLED_TO_THE_BONE); }); // Remove players with Chilled to the Bone

                    uint32 uiReduceAmount = 0;

                    switch (pOwner->GetMap()->GetDifficulty())
                    {
                        // In 25-man raid, the amount is 1 * number of players within 4 yards
                        case RAID_DIFFICULTY_25MAN_HEROIC:
                        case RAID_DIFFICULTY_25MAN_NORMAL:
                            uiReduceAmount = playerList.size() > 4 ? 4 : playerList.size();
                            break;
                        // Otherwise it is 2 * number of players within 4 yards
                        default:
                            uiReduceAmount = playerList.size() > 2 ? 4 : playerList.size() * 2;
                            break;
                    }

                    // And the amount can't be reduced below 1
                    if (GetStackAmount() <= uiReduceAmount) // Indeed, if the update was done in real time, this should never happen
                        SetStackAmount(1);
                    else
                        SetStackAmount(5 - uiReduceAmount);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_malakk_frostbite_periodic_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_malakk_frostbite_periodic_AuraScript();
        }
};

// Frostbite Selector 137575
class spell_malakk_frostbite_selector : public SpellScript
{
    PrepareSpellScript(spell_malakk_frostbite_selector)

    void HandleOnHit(SpellEffIndex eEffIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Unit* target = GetHitUnit())
                caster->AI()->SetGUID(target->GetGUID());
    }

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove whole exclude casters
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if(MeeleSpecTargetSelector());

            // if we have enough players without tanks, meelee
            if (!pList.empty())
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                return;
            }

            // if we haven`t enough players
            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malakk_frostbite_selector::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

class FrostbitePredicate 
{
    public:
        FrostbitePredicate(Creature* const m_caster) : _caster(m_caster) { }
    
        bool operator()(WorldObject* object)
        {
            return object && object->ToUnit() && object->ToUnit()->GetGUID() == _caster->AI()->GetGUID();
        }

    private:
        Creature const* _caster;
};

// Frostbite 136990
class spell_malakk_frostbite : public SpellScript
{
    PrepareSpellScript(spell_malakk_frostbite)

    // Handler to select target (cause TARGET_UNIT_SRC_AREA_ENTRY doesn't work fine)
    void SelectTarget(std::list<WorldObject*>& targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            targets.remove_if(FrostbitePredicate(caster));
    }

    // Handler to apply the visual
    void HandleOnHit(SpellEffIndex eEffIndex)
    {
        if (Unit* pHit = GetHitUnit())
        {
            // Prevent Frostbite dmg Eff and apply if player has smth from defence list
            if (hasAnyDefenceSpells(pHit))
            {
                PreventHitEffect(eEffIndex);
                return;
            }

            if (Unit* caster = GetCaster())
            {
                pHit->CastSpell(pHit, SPELL_FROSTBITE_SCREEN_EFFECT, true);
                caster->CastSpell(pHit, SPELL_FROSTBITE_PERIODIC_DAMAGES, true);

                if (caster->ToCreature()->AI())
                    caster->ToCreature()->AI()->Talk(TALK_MALAKK_FROSTBITE, pHit);
            }
        }
    }

    bool hasAnyDefenceSpells(Unit* target)
    {
        for (auto&& itr : frostbiteIgnoreSpells)
            if (target->HasAura(itr))
                return true;

        return false;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malakk_frostbite::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_malakk_frostbite::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Frostbite (allies damages) 136937
class spell_malakk_frostbite_allies : public SpellScript
{
    PrepareSpellScript(spell_malakk_frostbite_allies)

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && (!target->ToPlayer() || target->ToPlayer() && target->ToPlayer()->IsGameMaster() || target->ToPlayer() && target->ToPlayer()->HasAura(SPELL_CHILLED_TO_THE_BONE)); });
    }

    int32 value;

    bool Load() override
    {
        if (GetCaster())
        {
            value = GetCaster()->GetMap()->IsHeroic() ? 60000 : 22000;
            return true;
        }
        return false;
    }

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        Unit* target = GetHitUnit();
        Unit* caster = GetCaster();

        if (!target || !caster)
            return;

        if (Aura* aura = caster->GetAura(SPELL_FROSTBITE_PERIODIC_DAMAGES))
        {
            SetHitDamage(aura->GetStackAmount() * value);
        }
    }

    void HandleOnCast()
    {
        // Cast Body Heat on heroic
        if (Unit* caster = GetCaster())
            if (caster->GetMap()->IsHeroic())
                caster->CastSpell(caster, SPELL_BODY_HEAT, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_malakk_frostbite_allies::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_malakk_frostbite_allies::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnCast += SpellCastFn(spell_malakk_frostbite_allies::HandleOnCast);
    }
};

// Reckless Charge 137117
class spell_kazrajin_reckless_charge : public SpellScript
{
    PrepareSpellScript(spell_kazrajin_reckless_charge)

    void HandleCast(SpellEffIndex eff_idx)
    {
        if (Creature* pKazrajin = GetCaster()->ToCreature())
        {
            if (Unit* chargeTarget = ObjectAccessor::GetUnit(*pKazrajin, pKazrajin->AI()->GetGUID()))
            {
                pKazrajin->CastSpell(pKazrajin, SPELL_RECKLESS_CHARGE_SOUND, true);
                chargeTarget->RemoveAurasDueToSpell(SPELL_RECKLESS_CHARGE_FACE);

                // Compute position of landing
                float fDist = pKazrajin->GetExactDist2d(chargeTarget) - 3.f; // Remove 5 yards to continue rolling
                float fAngle = pKazrajin->GetAngle(chargeTarget);
                float fX, fY;
                GetPositionWithDistInOrientation(pKazrajin, fDist, fAngle, fX, fY);

                uint8 m_pointCount = ((uint8)fDist / 3) + 1;

                for (uint8 i = 0; i < m_pointCount; ++i)
                {
                    float x, y;
                    GetPositionWithDistInOrientation(pKazrajin, fDist - (i * 3), fAngle, x, y);
                    pKazrajin->CastSpell(x, y, pKazrajin->GetMap()->GetHeight(x, y, pKazrajin->GetPositionZ() + 0.8f), SPELL_RECKLESS_CHARGE_GROUND_AT, true);
                }

                if (pKazrajin->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
                    pKazrajin->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                pKazrajin->GetMotionMaster()->MoveCharge(fX, fY, chargeTarget->GetPositionZ(), 42.f, POINT_RECKLESS_CHARGE_LAND);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_kazrajin_reckless_charge::HandleCast, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

class withinRangePredicate
{
    public:
        withinRangePredicate(Unit* _caster) : caster(_caster) { }

        bool operator()(WorldObject* target) const
        {
            if (target && target->GetExactDist2d(caster) < 9.f)
                return true;
            return false;
        }

    private:
        Unit* caster;
};

class RecklessChargeDistanceOrderPred
{
    public:
        RecklessChargeDistanceOrderPred(const WorldObject* object, bool ascending = true) : m_object(object), m_ascending(ascending) {}
        bool operator() (const WorldObject* a, const WorldObject* b) const
        {
            return m_ascending ? a->GetDistance(m_object) < b->GetDistance(m_object) :
                a->GetDistance(m_object) > b->GetDistance(m_object);
        }
    private:
        const WorldObject* m_object;
        const bool m_ascending;
};

// Reckless Charge Selector 137107
class spell_kazrajin_reckless_charge_targeting : public SpellScript
{
    PrepareSpellScript(spell_kazrajin_reckless_charge_targeting);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector());

        if (GetCaster() && !targets.empty())
        {
            // Try to select farthest victim
            targets.sort(RecklessChargeDistanceOrderPred(GetCaster(), false));
            targets.resize(1);
            return;
        }

        targets.clear();
        targets = m_targets;

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                if (Creature* recklessChargeTrigger = target->SummonCreature(NPC_RECKLESS_CHARGE, *target, TEMPSUMMON_TIMED_DESPAWN, 7 * IN_MILLISECONDS))
                {
                    caster->CastSpell(caster, SPELL_RECKLESS_CHARGE_UNK, true);
                    caster->CastSpell(caster, SPELL_RECKLESS_CHARGE_FACE, true);
                    caster->ClearUnitState(UNIT_STATE_CASTING);
                    caster->AI()->SetGUID(recklessChargeTrigger->GetGUID());
                }
            }
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_kazrajin_reckless_charge_targeting::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_kazrajin_reckless_charge_targeting::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Overload 137149
class spell_kazrajin_overload : public AuraScript
{
    PrepareAuraScript(spell_kazrajin_overload);

    void HandleEffectApply(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (!GetOwner())
            return;

        // Stunned on aura apply
        if (Creature* pOwner = GetOwner()->ToCreature())
        {
            pOwner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            pOwner->SetControlled(true, UNIT_STATE_STUNNED);
        }
    }

    void HandleOnProc(ProcEventInfo& rProcInfo)
    {
        if (!rProcInfo.GetDamageInfo())
            return;

        int32 uiDamages = (int32)rProcInfo.GetDamageInfo()->GetDamage() * 0.4f; // 40% of damages returned to the player

        if (Unit* victim = rProcInfo.GetProcTarget())
        {
            GetUnitOwner()->CastCustomSpell(victim, SPELL_OVERLOAD_DAMAGES, &uiDamages, 0, 0, true);
            victim->CastSpell(GetUnitOwner(), SPELL_OVERLOAD_VISUAL, true);
        }
    }

    void HandleEffectRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (!GetOwner())
            return;

        // Unstunned on aura remove
        if (Creature* pOwner = GetOwner()->ToCreature())
            pOwner->SetControlled(false, UNIT_STATE_STUNNED);
    }

    void Register() override
    {
        // Note: there is no stunning spell, I'm quite sure of that... even if
        // it is really wierd.
        OnEffectApply += AuraEffectApplyFn(spell_kazrajin_overload::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnProc += AuraProcFn(spell_kazrajin_overload::HandleOnProc);
        OnEffectRemove += AuraEffectRemoveFn(spell_kazrajin_overload::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Discharge 137166
class spell_kazrajin_discharge : public AuraScript
{
    PrepareAuraScript(spell_kazrajin_discharge)

    void HandleEffectApply(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (!GetOwner())
            return;

        // Stunned on aura apply
        if (Creature* pOwner = GetOwner()->ToCreature())
            pOwner->SetControlled(true, UNIT_STATE_STUNNED);
    }

    void HandlePeriodic(AuraEffect const* pAuraEffect)
    {
        if (Creature* pOwner = GetOwner()->ToCreature())
        {
            int32 iDamage = (int32)pOwner->AI()->GetData(DATA_DAMAGES_PAST_SEC);
            //int32 pctDamage = CalculatePct(iDamage, GetSpellInfo()->Effects[EFFECT_0].BasePoints);

            /* better approximated ~ 18,5*(100k*0,03)=55k, 6*(100k*0,08)=48k
            possible it`s just should`ve not calculate dmg from dot effects, but we can`t check it*/
            int32 pctDamage = CalculatePct(iDamage, pOwner->GetMap()->Is25ManRaid() ? 3 : 8);
            pOwner->AI()->SetData(DATA_DAMAGES_PAST_SEC, 0);

            pOwner->CastCustomSpell(pOwner, SPELL_DISCHARGE_DAMAGES, &pctDamage, 0, 0, true);
            pOwner->CastSpell(pOwner, SPELL_DISCHARGE_VISUAL, true);
        }
    }

    void HandleEffectRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (!GetOwner())
            return;

        // Unstunned on aura remove
        if (Creature* pOwner = GetOwner()->ToCreature())
            pOwner->SetControlled(false, UNIT_STATE_STUNNED);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_kazrajin_discharge::HandleEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_kazrajin_discharge::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectRemove += AuraEffectRemoveFn(spell_kazrajin_discharge::HandleEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Ensnared / Entrapped (the most fucked up spell ever made in WoW) (handled through Quicksand)
class spell_quicksand_periodic : public SpellScriptLoader
{
    public:
        spell_quicksand_periodic() : SpellScriptLoader("spell_quicksand_periodic") { }

        class spell_quicksand_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_quicksand_periodic_AuraScript)

            void HandlePeriodic(AuraEffect const* pAuraEffect)
            {
                Unit* caster = GetCaster();
                Unit* target = GetOwner()->ToUnit();

                if (!caster || !target)
                    return;

                if (Aura* aura = target->GetAura(SPELL_ENSNARED))
                {
                    if (aura->GetCasterGUID() == caster->GetGUID())
                    {
                        aura->ModStackAmount(1);
                        if (aura->GetStackAmount() >= 5)
                        {
                            caster->AddAura(SPELL_ENTRAPPED, target);
                            aura->Remove();
                        }
                    }
                }
                else if (!target->HasAura(SPELL_ENTRAPPED))
                    caster->AddAura(SPELL_ENSNARED, target);
            }

            void HandleOnRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                Unit* caster = GetCaster();
                Unit* target = GetOwner()->ToUnit();

                if (!caster || !target)
                    return;

                target->RemoveAura(SPELL_ENSNARED, caster->GetGUID());

            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_quicksand_periodic_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                OnEffectRemove += AuraEffectRemoveFn(spell_quicksand_periodic_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_quicksand_periodic_AuraScript();
        }
}; 

// Entrapped
class spell_quicksand_entrapped : public SpellScriptLoader
{
    public:
        spell_quicksand_entrapped() : SpellScriptLoader("spell_quicksand_entrapped") { }

        class spell_quicksand_entrapped_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_quicksand_entrapped_SpellScript)

            void HandleAdditionalSpell(SpellEffIndex eEffIndex)
            {
                PreventHitDefaultEffect(eEffIndex);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_quicksand_entrapped_SpellScript::HandleAdditionalSpell, EFFECT_1, SPELL_EFFECT_203);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_quicksand_entrapped_SpellScript();
        }
};

// Sandstorm
class spell_sul_sandstorm : public SpellScriptLoader
{
    enum : uint32
    {
        SANDSTORM_VISUAL    = 136895
    };

    public:
        spell_sul_sandstorm() : SpellScriptLoader("spell_sul_sandstorm") { }

        class spell_sul_sandstorm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sul_sandstorm_SpellScript)

            void HandleAfterCast()
            {
                if (Unit* caster = GetCaster())
                {
                    caster->AddAura(SANDSTORM_VISUAL, caster);

                    std::list<Creature*> livingSandsList;
                    caster->GetCreatureListWithEntryInGrid(livingSandsList, NPC_LIVING_SAND, 500.0f);

                    for (auto&& itr : livingSandsList)
                        itr->AI()->DoAction(ACTION_CREATE_LIVING_SAND);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_sul_sandstorm_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sul_sandstorm_SpellScript();
        }
};

// Summon Blessed Loa Spirit 137203
class spell_marli_summon_blessed_loa_spirit : public SpellScript
{
    PrepareSpellScript(spell_marli_summon_blessed_loa_spirit)

    void HandleDummy(SpellEffIndex eEffIndex)
    {
        if (Unit* caster = GetCaster())
        {
            caster->CastSpell(caster, urand(SPELL_SUMMON_BLESSED_LOA_SPIRIT, SPELL_SUMMON_BLESSED_LOA_SPIRIT + 2), false);

            if (caster->ToCreature()->AI())
                caster->ToCreature()->AI()->Talk(TALK_MARLI_BLESSED);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_marli_summon_blessed_loa_spirit::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Summon Shadowed Loa Spirit
class spell_marli_summon_shadowed_loa_spirit : public SpellScriptLoader
{
    public:
        spell_marli_summon_shadowed_loa_spirit() : SpellScriptLoader("spell_marli_summon_shadowed_loa_spirit") { }

        class spell_marli_summon_shadowed_loa_spirit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_marli_summon_shadowed_loa_spirit_SpellScript)

            void HandleDummy(SpellEffIndex eEffIndex)
            {
                if (Unit* caster = GetCaster())
                {
                    caster->CastSpell(caster, SPELL_SUMMON_SHADOWED_LOA_SPIRIT, false);

                    if (caster->ToCreature()->AI())
                        caster->ToCreature()->AI()->Talk(TALK_MARLI_SHADOWED);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_marli_summon_shadowed_loa_spirit_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_marli_summon_shadowed_loa_spirit_SpellScript();
        }
};

// Twisted Fate (first) 137891
class spell_marli_twisted_fate_first : public SpellScript
{
    PrepareSpellScript(spell_marli_twisted_fate_first)

    std::list<WorldObject*> m_targets;

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        m_targets = targets;

        // Remove whole exclude meelee
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(CasterSpecTargetSelector());

        if (!targets.empty())
        {
            Trinity::Containers::RandomResizeList(targets, 1);
            return;
        }

        // if not found any caster
        if (!m_targets.empty())
        {
            targets.clear();
            targets.push_back(Trinity::Containers::SelectRandomContainerElement(m_targets));
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_marli_twisted_fate_first::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Twisted Fate (second) 137962
class spell_marli_twisted_fate_second : public SpellScript
{
    PrepareSpellScript(spell_marli_twisted_fate_second)

    std::list<WorldObject*> m_targets;

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        m_targets = targets;

        // Remove whole exclude casters
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        if (!targets.empty())
        {
            // We should try select farthest
            targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster(), false));
            targets.resize(1);
            return;
        }

        // if not found any caster
        if (!m_targets.empty())
        {
            targets.clear();

            m_targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster(), false));
            targets.push_back(m_targets.front());
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_marli_twisted_fate_second::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Twisted Fate (Periodic Damages)
class spell_marli_twisted_fate_damages : public SpellScriptLoader
{
    public:
        spell_marli_twisted_fate_damages() : SpellScriptLoader("spell_marli_twisted_fate_damages") { }

        class spell_marli_twisted_fate_damages_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_marli_twisted_fate_damages_AuraScript)

            void CalculateAmount(AuraEffect const* pAuraEffect, float& ruiAmount, bool &rbCanBeRecalculated)
            {
                if (!GetOwner())
                    return;

                if (Creature* pOwner = GetOwner()->ToCreature())
                {
                    if (Creature* pTwistedFateTarget = ObjectAccessor::GetCreature(*pOwner, pOwner->AI()->GetGUID(DATA_TWISTED_FATE_GUID)))
                    {
                        if (pTwistedFateTarget->IsAlive())
                        {
                            // The max amount is 250000, but it reduces the father the twisted fates are from
                            // each other. Let's say the maximum dist between them is 55 yards. If we want this
                            // to be pseudo linear, we'll say that with each yard between the twisted fates, we'll
                            // reduce this amount by 100 / 55. 
                            float fDist = pOwner->GetExactDist2d(pTwistedFateTarget);
                            ruiAmount = 250000 - CalculatePct(250000, fDist * 100 / 55);
                        }
                    }
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_marli_twisted_fate_damages_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_marli_twisted_fate_damages_AuraScript();
        }
};

// Dark Power
class spell_dark_power : public SpellScriptLoader
{
    public:
        spell_dark_power() : SpellScriptLoader("spell_dark_power") { }
        
        class spell_dark_power_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dark_power_SpellScript)
            
            void HandleHitTarget(SpellEffIndex eEffectIndex)
            {
                if (Unit* unit = GetCaster())
                {
                    if (Creature* caster = unit->ToCreature())
                    {
                        if (CreatureAI* pAI = caster->AI())
                        {
                            uint32 const uiDarkPowerCount = pAI->GetData(DATA_DARK_POWER_COUNT);
                            int32 iCustomValue = GetSpellInfo()->Effects[(uint32)eEffectIndex].BasePoints;

                            // Add 10% for each stack of Dark Power
                            for (uint32 i = 0; i < uiDarkPowerCount; ++i)
                                iCustomValue *= 1.1f;

                            SetHitDamage(iCustomValue);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dark_power_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };
        
        SpellScript* GetSpellScript() const override
        {
            return new spell_dark_power_SpellScript();
        }
};

// Soul Fragment Target Selector 137645
class spell_soul_fragment_target_selector : public SpellScript
{
    PrepareSpellScript(spell_soul_fragment_target_selector)

    std::list<WorldObject*> m_targets;

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        m_targets = targets;
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_SOUL_FRAGMENT_PERIODIC); });

        // Max Affect Targets 1 - not need smth more
        if (!targets.empty())
            return;

        m_targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_SOUL_FRAGMENT_PERIODIC); });

        if (!m_targets.empty())
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);
        }
    }

    void HandleHitTarget(SpellEffIndex eEffectIndex)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_SOUL_FRAGMENT_PERIODIC, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soul_fragment_target_selector::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_soul_fragment_target_selector::HandleHitTarget, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
    }
};

// Soul Fragment Switcher 137643
class spell_soul_fragment_switcher : public SpellScript
{
    PrepareSpellScript(spell_soul_fragment_switcher)

    SpellCastResult CheckCast()
    {
        if (Unit* caster = GetCaster())
            if (Player* target = ObjectAccessor::GetPlayer(*caster, caster->GetTarget()))
                if (!target->HasAura(SPELL_SOUL_FRAGMENT_PERIODIC))
                    return SPELL_CAST_OK;

        return SPELL_FAILED_UNKNOWN;
    }

    void HandleHitTarget(SpellEffIndex eEffIndex)
    {
        if (Unit* caster = GetCaster())
            caster->RemoveAurasDueToSpell(SPELL_SOUL_FRAGMENT_PERIODIC);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_soul_fragment_switcher::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_soul_fragment_switcher::HandleHitTarget, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
    }
};

class distancePredicate
{
    private:
        Unit* caster;
    public:
        distancePredicate(Unit* _caster) : caster(_caster) { }

        bool operator()(WorldObject* target) const
        {
            return target->GetExactDist2d(caster) < 15.f;
        }
};

// Marked Soul 137359
class spell_marked_soul : public SpellScript
{
    PrepareSpellScript(spell_marked_soul);

    void HandleEffectHitTarget(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            caster->ClearUnitState(UNIT_STATE_CASTING);

            if (Unit* target = GetHitUnit())
                caster->AI()->SetGUID(target->GetGUID());
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        std::list<WorldObject*> tempTargets;
        std::copy(std::begin(targets), std::end(targets), std::inserter(tempTargets, tempTargets.begin()));

        targets.remove_if(TankSpecTargetSelector());

        if (!targets.empty())
        {
            if (Unit* caster = GetCaster())
                targets.sort(Trinity::ObjectDistanceOrderPred(caster, false));

            if (targets.size() > 1)
                targets.resize(1);
        }
        else
        {
            std::copy(std::begin(tempTargets), std::end(tempTargets), std::inserter(targets, targets.begin()));

            if (targets.empty())
                return;

            if (Unit* caster = GetCaster())
                targets.sort(Trinity::ObjectDistanceOrderPred(caster, false));

            if (targets.size() > 1)
                targets.resize(1);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_marked_soul::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_marked_soul::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

class correctGuidPredicate
{
    private:
        ObjectGuid casterGuid;
    public:
        correctGuidPredicate(uint64 guid) : casterGuid(guid) { }

        bool operator()(WorldObject* target) const
        {
            if (target)
            {
                return !target->ToUnit()->HasAura(SPELL_MARKED_SOUL, casterGuid);
            }
            return false;
        }
};

class spell_shadowed_gift : public SpellScriptLoader
{
    public:
        spell_shadowed_gift() : SpellScriptLoader("spell_shadowed_gift") { }

        class spell_shadowed_gift_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_shadowed_gift_SpellScript);

            void SelectTargets(std::list<WorldObject*>&targets)
            {
                targets.remove_if([=](WorldObject* target) { return target && (!target->ToPlayer() || target->ToPlayer() && target->ToPlayer()->IsGameMaster()); });

                if (Unit* caster = GetCaster())
                    targets.remove_if (correctGuidPredicate(caster->GetGUID()));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_shadowed_gift_SpellScript::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_shadowed_gift_SpellScript();
        }
};

// Soul Fragment 137641
class spell_council_soul_fragment : public AuraScript
{
    PrepareAuraScript(spell_council_soul_fragment);

    void CalculateTick(AuraEffect const*, int32& damage)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            int32 damageMod = GetSpellInfo()->Effects[EFFECT_1].BasePoints;
            uint32 stackAmount = (owner->GetAura(SPELL_SHADOWED_SOUL) ? owner->GetAura(SPELL_SHADOWED_SOUL)->GetStackAmount() : 0) * (owner->GetMap()->Is25ManRaid() ? 5 : 3);
            int32 newDamage = damageMod + CalculatePct(damageMod, stackAmount);
            damage = newDamage;
        }
    }

    void Register() override
    {
        DoEffectCalcTickAmount += AuraEffectCalcTickAmountFn(spell_council_soul_fragment::CalculateTick, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// Body Heat 137084
class spell_body_heat : public SpellScript
{
    PrepareSpellScript(spell_body_heat);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && target->ToPlayer()->HasAura(SPELL_BODY_HEAT); });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_body_heat::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};
// Body Heat 137084
class spell_body_heat_aura : public AuraScript
{
    PrepareAuraScript(spell_body_heat_aura)

    void HandleRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_CHILLED_TO_THE_BONE, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_body_heat_aura::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Twisted Fate Eff 137972
class spell_twisted_fate_eff : public SpellScript
{
    PrepareSpellScript(spell_twisted_fate_eff);

    void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Creature* secondCopy = ObjectAccessor::GetCreature(*caster, caster->AI()->GetGUID()))
            {
                if (Unit* unit = GetHitUnit())
                {
                    // 80% damage reduction after 30 yards distance
                    float dist = caster->GetExactDist2d(secondCopy);

                    int32 reduction = dist >= 30 ? GetHitDamage() * 0.2 : GetHitDamage() * (1 - ((dist * 2.5) / 100));
                    SetHitDamage(reduction);
                }
            }
            else
                SetHitDamage(100000);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_twisted_fate_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 751. Summoned by 138026 - Reckless Charge
class sat_reckless_charge : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                if (Creature* kazrajin = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_COUNCIL_KAZRAJIN) : ObjectGuid::Empty))
                {
                    // Prevent twice damage
                    if (kazrajin->AI()->GetData(target->GetGUID().GetCounter() + 10))
                    {
                        kazrajin->AI()->SetData(DATA_RECKLESS_CHARGE_HIT, target->GetGUID().GetCounter() + 10);
                        caster->CastSpell(target, SPELL_RECKLESS_CHARGE_UNIQUE_DAMAGES, true);
                    }
                }
            }
        }
    }
};

// Areatrigger 8986
class AreaTrigger_behind_council_of_elders : public AreaTriggerScript
{
    public:
        AreaTrigger_behind_council_of_elders() : AreaTriggerScript("AreaTrigger_behind_council_of_elders") { }
    
        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* garajalCouncil = ObjectAccessor::GetCreature(*player, instance->GetGuidData(DATA_COUNCIL_OF_ELDERS)))
                    garajalCouncil->AI()->DoAction(ACTION_COUNCIL_INTRO);
    
            return false;
        }
};

void AddSC_boss_council_of_elders()
{
    new boss_frost_king_malakk();
    new boss_kazrajin();
    new boss_sul_the_sandcrawler();
    new boss_high_priestess_marli();
    new npc_garajal();
    new creature_script<npc_council_reckless_charge>("npc_council_reckless_charge");
    new creature_script<npc_garajals_soul>("npc_garajals_soul");
    new npc_living_sand();
    new creature_script<npc_blessed_loa_spirit>("npc_blessed_loa_spirit");
    new creature_script<npc_shadowed_loa_spirit>("npc_shadowed_loa_spirit");
    new creature_script<npc_twisted_fate>("npc_twisted_fate");
    new aura_script<spell_garajal_possessed>("spell_garajal_possessed");
    new spell_malakk_frigid_assault();
    new spell_script<spell_malakk_biting_cold>("spell_malakk_biting_cold");
    new spell_script<spell_malakk_frostbite_selector>("spell_malakk_frostbite_selector");
    new spell_script<spell_malakk_frostbite>("spell_malakk_frostbite");
    new spell_malakk_frostbite_periodic();
    new spell_script<spell_malakk_frostbite_allies>("spell_malakk_frostbite_allies");
    new spell_script<spell_kazrajin_reckless_charge>("spell_kazrajin_reckless_charge");
    new spell_script<spell_kazrajin_reckless_charge_targeting>("spell_kazrajin_reckless_charge_targeting");
    new aura_script<spell_kazrajin_overload>("spell_kazrajin_overload");
    new aura_script<spell_kazrajin_discharge>("spell_kazrajin_discharge");
    new spell_quicksand_periodic();
    new spell_quicksand_entrapped();
    new spell_sul_sandstorm();
    new spell_script<spell_marli_summon_blessed_loa_spirit>("spell_marli_summon_blessed_loa_spirit");
    new spell_marli_summon_shadowed_loa_spirit();
    new spell_script<spell_marli_twisted_fate_first>("spell_marli_twisted_fate_first");
    new spell_script<spell_marli_twisted_fate_second>("spell_marli_twisted_fate_second");
    new spell_marli_twisted_fate_damages();
    new spell_dark_power();
    new spell_script<spell_soul_fragment_target_selector>("spell_soul_fragment_target_selector");
    new spell_script<spell_soul_fragment_switcher>("spell_soul_fragment_switcher");
    new spell_script<spell_marked_soul>("spell_marked_soul");
    new spell_shadowed_gift();
    new aura_script<spell_council_soul_fragment>("spell_council_soul_fragment");
    new spell_script<spell_body_heat>("spell_body_heat");
    new aura_script<spell_body_heat_aura>("spell_body_heat_aura");
    new spell_script<spell_twisted_fate_eff>("spell_twisted_fate_eff");
    new atrigger_script<sat_reckless_charge>("sat_reckless_charge");
    new AreaTrigger_behind_council_of_elders();
}

