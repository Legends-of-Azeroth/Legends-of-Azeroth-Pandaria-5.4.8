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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Vehicle.h"
#include "Creature.h"
#include "brawlers_guild.h"
#include "Random.h"

/* @Note:
 In Patch 5.1 was added brawlers guild with 7 ranks.
 In Patch 5.3 it was expand to 8 and 9td ranks
 */

enum miscData
{
    SPELL_GUARD_AREATRIGGER                     = 134909,
    SPELL_BRAWLERS_PURSE                        = 134565, // reward for each boss (bonus roll type)
    SPELL_U_ARE_NOT_UR_FICKING_LEGPLATES_ACHIEV = 134548,
    SPELL_THROW_ROTTEN_APPLE                    = 135445,
    SPELL_THROW_ROTTEN_WATERMELON               = 135446,
    SPELL_THROW_ROTTEN_BANANA                   = 135447,
    SPELL_YOU_STINK                             = 135451,

    // Creature Spells
    SPELL_CHOMP_CHOMP_CHOMP                     = 135342,
    SPELL_VOLATILE_FLAMES_ALIANCE               = 134740, // it`s diff for aliance and horde, cuz have spell_target_pos in db for each zone
    SPELL_VOLATILE_FLAMES_HORDE                 = 133430,
    SPELL_LAVA_BURST                            = 133293,
    SPELL_VOLATILE_FLAME_EFF                    = 133303,
    SPELL_VOLATILE_FLAME_AT                     = 133294,
    SPELL_FIRE_LINE_AURA                        = 125404,
    SPELL_LUMBERING_CHARGE_MARK                 = 136335,
    SPELL_LUMBERING_CHARGE                      = 134527,
    SPELL_LUMBERING_CHARGE_EFF                  = 134526,
    SPELL_NAUGHTY_SECRETS                       = 133288,
    SPELL_HEATED_POKERS                         = 133286,
    SPELL_SLIPPY                                = 134536,
    SPELL_PECK                                  = 134537,
    SPELL_LIGHTNING_BOLT                        = 136321,
    SPELL_STORM_OF_CLOUD                        = 135234,
    SPELL_SUMMON_TWISTER                        = 132670,
    SPELL_LIGHTNING_FURY                        = 132673,
    SPELL_TWISTER_AURA                          = 132669,
    SPELL_THROW_DYNAMITE                        = 133213,
    SPELL_GOBLIN_DEVICE                         = 133227, // trigger 133219 in 3s with script eff
    SPELL_GOBLIN_LAND_MINES                     = 133219,
    SPELL_GOBLIN_LAND_MINE_SUMM                 = 133220,
    SPELL_NET_TOSS                              = 133308,
    SPELL_EXPLOSION                             = 133236,
    SPELL_DASH                                  = 44531,
    SPELL_SPLIT                                 = 133302,
    SPELL_FIREBALL                              = 136333,
    SPELL_FLAME_BUFFET                          = 135237,
    SPELL_FIREWALL_EFFECT                       = 132661,
    SPELL_FIREWALL                              = 132666,
    SPELL_FIREWALL_AT_1                         = 132662,
    SPELL_FIREWALL_AT_2                         = 132663,
    SPELL_FIREWALL_AT_3                         = 132664,
    SPELL_FIREWALL_AT_4                         = 132665,
    SPELL_PYROBLAST                             = 136334,
    SPELL_DEVASTATING_THRUST                    = 134777,
    SPELL_RAGEFUL_SPIRIT                        = 134534,
    SPELL_COLLISION_EFF                         = 133254, // on player, is player between point of charge and encounter
    SPELL_COLLISION                             = 133255, // on encounter, if it doesn`t hit wall
    SPELL_CHARGING                              = 133260,
    SPELL_CHARGE_S                              = 133253, // only for DBM
    SPELL_EARTHLIVING_SEED                      = 134743,
    SPELL_EARTHLIVING_FLOURISH                  = 134748,
    SPELL_SOLAR_BEAM                            = 129888,
    SPELL_WRATH                                 = 134741,
    SPELL_ILLUSIONIST                           = 133129,
    SPELL_ARCANE_BARRAGE                        = 133123,
    SPELL_MECHANO_KICK                          = 133169,
    SPELL_SHOCK_FIELD_EFF                       = 133156, // damage aura
    SPELL_SHOCK_FIELD_SELECTOR                  = 133159,
    SPELL_SHOCK_FIELD_UNK                       = 133150, 
    SPELL_SHOCK_FIELD_AT                        = 133065,
    SPELL_ELECTRIC_DYNAMITE_EFF                 = 133576,
    SPELL_SHRINK_RAY_AFTERMATH                  = 133306,
    SPELL_PHOTOPLASM_BUSTER_RAY                 = 133357,
    SPELL_ELECTRIC_DYNAMITE                     = 133363, // areatrigger on throw
    SPELL_MEGAFANTASTIC_DISCOMBOBUMORPHNATOR    = 133362,
    SPELL_FLAMES_OF_FALLEN_GLORY_EFF            = 133289,
    SPELL_FLAMES_OF_FALLEN_GLORY                = 133310,
    SPELL_SHADOWY_BLAST                         = 133332,
    SPELL_DARKZONE                              = 133346, // should be cast to trigger
    SPELL_RARK_BARRAGE                          = 133344,
    SPELL_BERSERK                               = 47008,
    SPELL_TORRENT                               = 124935,
    SPELL_RAIN_DANCE                            = 124860,
    SPELL_LEPEROUS_SPEW                         = 133157,
    SPELL_DETERMINED_SHUFFLE                    = 133171,
    SPELL_IZUS_RAVENS                           = 134784,
    SPELL_DISORIENTING_SHRIEK                   = 134795,
    SPELL_THUNDERSTORM                          = 79797,
    SPELL_FERAL_SPIRIT                          = 136194,
    SPELL_SHADOW_STRIKES                        = 126209,
    SPELL_CHAIN_LIGHTNING                       = 139963,
    SPELL_TOUGHT_LUCK                           = 134624,
    SPELL_SHIELD_WALLER                         = 134650,
    SPELL_HOOF_STOMP                            = 134623,
    SPELL_DARK_PROTECTION                       = 133499,
    SPELL_SHADOW_BOLT                           = 133655,
    SPELL_SUMMON_GHOST                          = 133465,
    SPELL_SPOTTED                               = 133427,
    SPELL_REVENGE_OF_DEPARTED                   = 133467,
    SPELL_SHIELDED                              = 133249,
    SPELL_HAPHAZARD_HEALTH_LINK                 = 133304,
    SPELL_GOBLIN_ROCKET_BARRAGE                 = 133212,
    SPELL_EMERGENCY_TELEPORT                    = 133162,
    SPELL_GNOMISH_DEATH_RAY                     = 133172,
    SPELL_ZOT                                   = 133038,
    SPELL_CONJURE_UNSTABLE_MINES                = 133015,
    SPELL_PROTECTED_BY_UNSTABLE_MINES           = 133018,
    SPELL_UNSTABLE_MINE_AURA                    = 133016,
    SPELL_EXPLOSION_MINE                        = 133017,
    SPELL_STRANGE_FEELING_EFF                   = 134851,
    SPELL_MEATBALL_RAGE                         = 134911,
    SPELL_OCULAR_STATIS                         = 140849,
    SPELL_OCULAR_STASIS_TELEPORT                = 143006,
    SPELL_HEXAGON_AREA_1                        = 140822, // required polygon model
    SPELL_HEXAGON_AREA_2                        = 140823,
    SPELL_HEXAGON_AREA_3                        = 140824,
    SPELL_HEXAGON_AREA_4                        = 140825,
    SPELL_HEXAGON_AREA_5                        = 140826,
    SPELL_HEXAGON_AREA_6                        = 140827,
    SPELL_HEXAGON_MASTER                        = 140828,

    // Non Ranked
    SPELL_POWERFUL_BITE                         = 135340,
    SPELL_BETTER_STRONGER_FASTER                = 142795,
    SPELL_MECHA_ARMOR_ALPHA                     = 141558,
    SPELL_MECHA_ARMOR_BETA                      = 142756,
    SPELL_MECHA_ARMOR_GAMMA                     = 142777,
    SPELL_MECHA_ARMOR_DELTA                     = 142778,
    SPELL_MECHA_ARMOR_EPSILON                   = 142779,
    SPELL_MECHA_ARMOR_OMEGA                     = 142780,
    SPELL_STASIS_BEAM                           = 142769,
    SPELL_CHOMP_CHOMP_CHOMP_MECHA               = 142788,
    SPELL_GET_OFF                               = 134772,
    SPELL_SUMMON_POWER_CRYSTALS                 = 133398, // has a selector for unk trigger
    SPELL_SUMMON_POWER_CRYSTALS_MISSLE          = 133399,
    SPELL_POWER_BLAST                           = 133400,
    SPELL_SONG_OF_FLOOT                         = 140982,
    SPELL_CANTATA_OF_FLOOTING                   = 140983,
    SPELL_THROW_LUCKYDO_NT_LAUNCHER             = 140969,
    SPELL_THROW_LUCKYDO                         = 140962,
    SPELL_THROW_LUCKYDONT                       = 140967,
    SPELL_THROW_BAD_LUCKYDO                     = 141020, // same as luckydon`t but not rnd
    SPELL_LUCKYDO_AT                            = 140971,
    SPELL_LUCKYDONT_AT                          = 140975,
    SPELL_LUCKYDOOM                             = 140954,
    SPELL_GOOD_LUCKYDO                          = 140955,
    SPELL_OVERCHARGING_TESLA_COIL_VISUAL        = 141065,
    SPELL_GNOMISH_TESLA_COIL_VISUAL             = 141047,
    SPELL_OVERCHARGED                           = 141059, // link from tesla coil
    SPELL_MOSTLY_ACCURATE_ROCKET                = 141359,
    SPELL_MALFUNCTIONIC                         = 141229,
    SPELL_CHAIN_LIGHTNING_VISUAL                = 141335,
    SPELL_LEFT_HOOK                             = 140868, // this boss required sniffs! there using vehicle for strafe in 3 pos
    SPELL_RIGHT_HOOK                            = 140862,
    SPELL_DODGE_LEFT                            = 140859,
    SPELL_DODGE_RIGHT                           = 140860,
    SPELL_TOSS_A_BOOM_BOOM                      = 140894,
    SPELL_TOSS_A_BOOM_BOOM_AT                   = 140891,
    SPELL_HOZEN_BOOMBSTICK_BACK_PACK            = 140895,
    SPELL_SUMMON_BOOM_BOX                       = 140912,
    SPELL_BOOM_BOX_VISUAL                       = 140930,
    SPELL_SUMMON_FUSE                           = 140918,

    // Misc
    SPELL_ANGRY_ENTHUSIAST_SELECTOR             = 135379,
    SPELL_ANGRY_LOSER                           = 135372,
    SPELL_ACHIEV_BOTTLE_SERVICE                 = 136314,
    SPELL_ACHIEV_HATERS_GONNA_HATE              = 135947,
    SPELL_ACE_KIT                               = 135965,
    SPELL_ACE_JET                               = 135966,
    SPELL_HOVER_ANIM_STATE                      = 90766,
    SPELL_BRAWLERS_GUILD_UNIT_FRAME             = 142875,
    SPELL_QUEUED_FOR_BRAWL                      = 132639,

    // Achiev Spells
    ACHIEVEMENT_FIRST_RULE_OF_BRAWLERS_GUILD   = 7948,
    ACHIEVEMENT_FIRST_RULE_OF_BRAWLERS_GUILD_A = 7947,
};

enum Events
{
    EVENT_INIT_BATTLE = 1,
    EVENT_CHOMP_CHOMP_CHOMP,
    EVENT_REMOVE_CHOMP,
    EVENT_FIRE_LINE,
    EVENT_VOLATILE_FLAMES,
    EVENT_LAVA_BURST,
    EVENT_MOVE,
    EVENT_CHECK,
    EVENT_LUMBERING_CHARGE,
    EVENT_HEATED_POKETS,
    EVENT_NAUGHTY_SECRETS,
    EVENT_LIGHTNING_BOLT,
    EVENT_STORM_OF_CLOUD,
    EVENT_SUMMON_TWISTER,
    EVENT_THROW_DYMANITE,
    EVENT_GOBLIN_LAND_MINE,
    EVENT_NET_TOSS,
    EVENT_EXPLOSION,
    EVENT_DASH,
    EVENT_SPLIT,
    EVENT_FIREBALL,
    EVENT_PYROBLAST,
    EVENT_SPECIAL,
    EVENT_DEVASTATING_THRUST,
    EVENT_FLAME_WALL,
    EVENT_COLLISION,
    EVENT_COLLISION_LAUNCH,
    EVENT_ARCANE_BARRAGE,
    EVENT_EARTHLIVING_SEED,
    EVENT_SOLAR_BEAM,
    EVENT_WRATH,
    EVENT_MOVE_FIELD,
    EVENT_DYNAMITE,
    EVENT_DISCOMBOBUMORPHNATOR,
    EVENT_BUSTER_RAY,
    EVENT_DARKZONE,
    EVENT_SHADOWY_BLAST,
    EVENT_DARK_BARRAGE,
    EVENT_DARK_BARRAGE_REMOVE,
    EVENT_TORRENT,
    EVENT_RAIN_DANCE,
    EVENT_BERKSERK,
    EVENT_SPEW,
    EVENT_SHUFFLE,
    EVENT_RAVENS,
    EVENT_SHRIEK,
    EVENT_THUNDERSTORM,
    EVENT_FERAL_SPIRIT,
    EVENT_SHADOW_STRIKES,
    EVENT_CHAIN_LIGHTNING,
    EVENT_SHIELD_WALLER,
    EVENT_HOOF_STOMP,
    EVENT_SHADOW_BOLT,
    EVENT_SUMMON_GHOST,
    EVENT_RESTORE_SHIELD,
    EVENT_TELEPORT,
    EVENT_ZOT,
    EVENT_RAGE,
    EVENT_YELL,
    EVENT_STASIS_BEAM,
    EVENT_BETTER_STRONGER_FASTER,
    EVENT_POWERFUL_BITE,
    EVENT_SONG_OF_FLOOT,
    EVENT_CANTATA_OF_FLOOTING,
    EVENT_THROW_LUCKYDO_NT,
    EVENT_THROW_BAD_LUCKYDO,
    EVENT_MOSTLY_ACCURATE_ROCKET,
    EVENT_VISUAL_CHARGING,
    EVENT_SUMMON_BLINGTRON,
};

enum MiscData
{
    ITEM_SACK_OF_COINS                = 98100,
    ITEM_BRAWLERS_PURSE               = 92718,
    ITEM_MODIFIED_CHOMPING_APPARATUS  = 97983,

    QUEST_MODIFIED_CHOMPING_APPARATUS = 32846,
};

struct brawlers_guild_encounter_typeAI : public ScriptedAI
{
    brawlers_guild_encounter_typeAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

    SummonList summons;
    EventMap events, nonCombatEvents;
    ObjectGuid summonerGUID, targetGUID;
    ObjectGuid challengeCardGUID;
    bool hasTele, hasYell;

    void IsSummonedBy(Unit* summoner) override
    {
        //Talk(TALK_BATTLE_INIT);
        summonerGUID = summoner->GetGUID();
        challengeCardGUID = ObjectGuid::Empty;

        if (me->GetEntry() != NPC_BO_BOBBLE)
        {
            me->SetVisible(false);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
        }

        summoner->CastSpell(me, SPELL_BRAWLERS_GUILD_UNIT_FRAME, true);
        summoner->RemoveAurasDueToSpell(SPELL_QUEUED_FOR_BRAWL);
        nonCombatEvents.ScheduleEvent(EVENT_INIT_BATTLE, 3 * IN_MILLISECONDS);
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        challengeCardGUID = guid;
    }

    // In situation when ability cast, that use PreapreChanneledCast was interrupted (stun, fear etc. exclude silence)
    // Without this encounter`ll broken
    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        switch (me->GetEntry())
        {
            case NPC_BRUCE:
            case NPC_VIAN_THE_VOLATILE:
            case NPC_GOREDOME:
            case NPC_IXX:
                if (caster->ToPlayer() && (HasAnyControlEffects() || spell->HasEffect(SPELL_EFFECT_KNOCK_BACK)))
                {
                    me->RemoveChanneledCast(targetGUID);

                    // same feature was on draenor, when u did interrupt or stun creature at cast.
                    // not sure that it was in pandaria, but it should work same for this situation
                    if (spell->GetMaxDuration())
                        events.RescheduleEvent(1, spell->GetMaxDuration() + 1.5 * IN_MILLISECONDS);
                }
                break;
            case NPC_CRUSH:
                if (caster->ToPlayer() && (HasAnyControlEffects() || spell->HasEffect(SPELL_EFFECT_KNOCK_BACK)))
                {
                    me->RemoveChanneledCast(targetGUID);

                    if (spell->GetMaxDuration())
                        events.RescheduleEvent(1, spell->GetMaxDuration() + 6.5 * IN_MILLISECONDS);
                }
                break;
            default:
                break;
        }
    }

    bool HasAnyControlEffects()
    {
        for (auto&& cList : controlMechanicType)
            if (me->HasAuraType(cList))
                return true;

        return false;
    }

    void Unsummoned() override
    {
        summons.DespawnAll();
        me->RemoveAllAreasTrigger();

        // In case if player was removed from map
        if (Creature* queueController = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
            queueController->AI()->DoAction(ACTION_FORCE_DESPAWNED);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
        summons.DespawnAll();
        me->RemoveAllAreasTrigger();

        if (me->GetEntry() == NPC_SLEAZY || me->GetEntry() == NPC_FLEASY || me->GetEntry() == NPC_GREAZY || me->GetEntry() == NPC_WHEEZY || me->GetEntry() == NPC_BO_BOBBLE) // leprognomes shouldn`t be here exclude one
        {
            me->DespawnOrUnsummon();
            return;
        }

        if (Creature* queueController = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
        {
            // Joke our brawler
            if (Player* target = ObjectAccessor::GetPlayer(*me, summonerGUID))
                queueController->AI()->Talk(TALK_BRAWLER_DEFEAT, target);

            queueController->AI()->DoAction(ACTION_WIPE);
        }
    }

    void KilledUnit(Unit* victim) override 
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
        {
            summons.DespawnAll();

            if (Creature* queueController = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
            {
                queueController->AI()->Talk(TALK_BRAWLER_DEFEAT, victim);
                queueController->AI()->DoAction(ACTION_WIPE);
            }
        }
    }

    void JustDied(Unit* killer) override
    {
        // Yells on Death
        switch (me->GetEntry())
        {
            case NPC_VIAN_THE_VOLATILE:
            case NPC_KIRRAWK:
            case NPC_FRAN:
            case NPC_RIDDOH:
            case NPC_SANORIAK:
            case NPC_DOMINIKA_THE_ILLUSIONIST:
            case NPC_DEEKEN:
            case NPC_FJOLL:
            case NPC_AKAMA:
            case NPC_SMASH_HOOFSTOMP:
            case NPC_BO_BOBBLE:
            case NPC_BATTLETRON:
            case NPC_MEATBALL:
            case NPC_BLINGTRON_3000:
                Talk(TALK_DEATH);
                break;
            case NPC_DARK_SUMMONER:
                Talk(TALK_DEATH);
                killer->RemoveAurasDueToSpell(SPELL_GUARDIANS_LIGHT);
                break;
        }

        // dodge hook if it was called by not quezy
        for (auto&& lepro : Leprognomes)
        {
            if (me->GetEntry() == lepro)
            {
                if (Creature* sQueazy = me->FindNearestCreature(NPC_QUEAZY, 100.0f, true))
                    sQueazy->AI()->DoAction(ACTION_LEPRO_DEATH);
                return;
            }
        }

        if (me->GetEntry() == NPC_BO_BOBBLE) // not credit him
            return;

        summons.DespawnAll();
        me->RemoveAllAreasTrigger();

        if (Creature* queueController = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
        {
            // Congratulation by arena Holder
            if (Player* arenaMember = ObjectAccessor::GetPlayer(*me, summonerGUID))
                queueController->AI()->Talk(TALK_BRAWLER_WON, arenaMember);

            queueController->AI()->DoAction(ACTION_WIPE);
        }

        Player* player = killer->GetAffectingPlayer();
        if (!player)
            if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
                player = target->GetAffectingPlayer();
        if (player)
        {
            player->RemoveAurasDueToSpell(SPELL_BRAWLERS_GUILD_UNIT_FRAME);

            // Congratulation our winner!
            std::list<Creature*> enthusiastList;
            GetCreatureListWithEntryInGrid(enthusiastList, me, NPC_BRAWL_ENTHUSIAST, 100.0f);
            GetCreatureListWithEntryInGrid(enthusiastList, me, NPC_BRAWL_ENTHUSIAST_A, 100.0f);

            uint32 enthusiastNum = urand(2, 3);
            if (enthusiastList.size() > enthusiastNum)
                Trinity::Containers::RandomResizeList(enthusiastList, enthusiastNum);

            for (auto&& itr : enthusiastList)
                itr->AI()->Talk(TALK_INTR0, player);

            // With low chance any of enthusiasts should became angry and attack player
            if (roll_chance_i(30))
                DoCast(me, SPELL_ANGRY_ENTHUSIAST_SELECTOR);

            // Reward by first achiev if it`s Bruce
            if (me->GetEntry() == NPC_BRUCE)
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_U_ARE_NOT_UR_FICKING_LEGPLATES_ACHIEV);

            // Hatters gonna hate Achiev conditions
            if (Aura* stink = player->GetAura(SPELL_YOU_STINK))
            {
                if (stink->GetStackAmount() > 9)
                    player->CastSpell(player, SPELL_ACHIEV_HATERS_GONNA_HATE, true);
            }

            // Reward Rep 250 for each won (not by challenge card of course)
            if (!challengeCardGUID)
                player->SetReputation(player->GetTeam() == HORDE ? FACTION_BRAWLGAR_ARENA : FACTION_BIZMO_BRAWLPUB, player->GetReputation(player->GetTeam() == HORDE ? FACTION_BRAWLGAR_ARENA : FACTION_BIZMO_BRAWLPUB) + 250);

            player->CastSpell(player, SPELL_BRAWLERS_PURSE, true); // NYI
            player->AddItem(ITEM_BRAWLERS_PURSE, 1); // temp

            if (me->GetEntry() == NPC_BRUCE && challengeCardGUID && !player->HasItemCount(ITEM_MODIFIED_CHOMPING_APPARATUS, 1, true) && player->GetQuestStatus(QUEST_MODIFIED_CHOMPING_APPARATUS) != QUEST_STATUS_REWARDED)
                player->AddItem(ITEM_MODIFIED_CHOMPING_APPARATUS, 1); // reward special item for challenge card by defeat to bruce from card.

            switch (me->GetEntry())
            {
                case NPC_MECHA_BRUCE:
                    player->KilledMonsterCredit(NPC_MECHA_BRUCE_CREDIT);
                    break;
                case NPC_GRANDPA_GRUMPLEFLOOT:
                    player->KilledMonsterCredit(NPC_GRANDPA_CREDIT);
                    break;
                case NPC_BLINGTRON_3000:
                    player->KilledMonsterCredit(NPC_BLINGTRON_CREDIT);
                    break;
                case NPC_MASTER_BOOM_BOOM:
                    player->KilledMonsterCredit(NPC_BOOM_BOOM_CREDIT);
                    break;
            }

            Position wPos = me->GetMapId() == 1043 ? hordeWipePos : WipePos;
            player->RemoveAurasDueToSpell(SPELL_RING_OF_VALOR_IN_THE_ARENA);
            player->TeleportTo(player->GetMapId(), wPos.GetPositionX(), wPos.GetPositionY(), wPos.GetPositionZ(), wPos.GetOrientation(), TELE_TO_NOT_UNSUMMON_PET);
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        if (summon->GetEntry() == NPC_BLAT_SPLITED)
        {
            if (Unit* vict = me->GetVictim())
            {
                summon->Attack(vict, true);
                summon->GetMotionMaster()->MoveChase(vict);
            }
        }
    }

    bool IsHordeArena()
    {
        return me->GetMapId() == 1043;
    }
};

class BrawlersArenaPredicate
{
    public:
        BrawlersArenaPredicate(Creature* _eCreature) : eCreature(_eCreature) { }

        bool operator()(WorldObject* object) const
        {
            return object && object->ToPlayer() && object->ToPlayer()->HasAura(SPELL_RING_OF_VALOR_IN_THE_ARENA);
        }
    private:
        Creature* eCreature;
};

struct brawlers_miscAI : public ScriptedAI
{
    brawlers_miscAI(Creature* creature) : ScriptedAI(creature) { }

    void KilledUnit(Unit* /*victim*/) override { }
};

// Bizmo 68323
struct npc_brawlers_guild_bizmo : public ScriptedAI
{
    npc_brawlers_guild_bizmo(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap berserkerEvents;
    std::vector<uint32> playersInQueue;
    std::vector<ObjectGuid> challengeCardGUIDs;
    ObjectGuid currentChampionGUID;
    ObjectGuid currentEncounterGUID;
    bool hasQueueStarted;

    void InitializeAI() override
    {
        DoCast(me, SPELL_HOVER_ANIM_STATE);

        me->OverrideInhabitType(INHABIT_AIR);
        me->SetAnimTier(AnimTier::Hover);
        me->UpdateMovementFlags();

        // Fly around arena perimetr
        scheduler
            .Schedule(Seconds(2), [this](TaskContext /*context*/)
        {
            Movement::MoveSplineInit init(me);
            for (auto&& itr : bizmoPath)
                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

            init.SetFly();
            init.SetCyclic();
            init.Launch();
        });

        Reset();
    }

    void Reset() override
    {
        me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 44634);
        playersInQueue.clear();
        challengeCardGUIDs.clear();
        currentChampionGUID = ObjectGuid::Empty;
        hasQueueStarted = false;
        berserkerEvents.Reset();
    }

    void SetGUID(ObjectGuid guid, int32 type) override
    {
        if (type == TYPE_CHALLENGE_CARD && std::find(challengeCardGUIDs.begin(), challengeCardGUIDs.end(), guid) == challengeCardGUIDs.end())
            challengeCardGUIDs.push_back(guid);
    }

    ObjectGuid GetGUID(int32 type) const override
    {
        uint64 i = 0; // default

        for (auto&& itr : playersInQueue)
        {
            if ((uint32)type != itr)
                i++;
            else
                break;
        }

        return ObjectGuid(i);
    }

    uint32 GetData(uint32 type) const override
    {
        if (std::find(playersInQueue.begin(), playersInQueue.end(), type) != playersInQueue.end())
            return 1;

        return 0;
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_WIPE:
                if (Creature* encounter = ObjectAccessor::GetCreature(*me, currentEncounterGUID))
                    encounter->DespawnOrUnsummon();

                berserkerEvents.Reset();

                // Remove from challenge card queue
                if (std::find(challengeCardGUIDs.begin(), challengeCardGUIDs.end(), currentChampionGUID) != challengeCardGUIDs.end())
                    challengeCardGUIDs.erase(std::find(challengeCardGUIDs.begin(), challengeCardGUIDs.end(), currentChampionGUID));

                if (Player* target = ObjectAccessor::GetPlayer(*me, currentChampionGUID))
                {
                    target->RemoveAurasDueToSpell(SPELL_BRAWLERS_GUILD_UNIT_FRAME);

                    // Tele out of arena if alive, respawn in 5s if dead
                    if (target->IsAlive())
                    {
                        Position wPos = target->GetMapId() == 1043 ? hordeWipePos : WipePos;
                        target->TeleportTo(target->GetMapId(), wPos.GetPositionX(), wPos.GetPositionY(), wPos.GetPositionZ(), wPos.GetOrientation(), TELE_TO_NOT_UNSUMMON_PET);
                    }
                    else
                    {
                        target->m_Events.Schedule(5000, [=]()
                        {
                            target->ResurrectPlayer(10.0f);
                            target->SpawnCorpseBones();

                            Position wPos = target->GetMapId() == 1043 ? hordeWipePos : WipePos;
                            target->TeleportTo(target->GetMapId(), wPos.GetPositionX(), wPos.GetPositionY(), wPos.GetPositionZ(), wPos.GetOrientation(), TELE_TO_NOT_UNSUMMON_PET);
                        });
                    }
                }

                if (!hasQueueStarted)
                {
                    hasQueueStarted = true;
                    scheduler
                        .Schedule(Seconds(5), [this](TaskContext /*context*/)
                    {
                        hasQueueStarted = false;
                        SetData(TYPE_QUEUE_NEXT, 1);
                    });
                }
                break;
            case ACTION_FORCE_DESPAWNED:
                if (hasQueueStarted)
                    break;

                // Remove from challenge card queue
                if (std::find(challengeCardGUIDs.begin(), challengeCardGUIDs.end(), currentChampionGUID) != challengeCardGUIDs.end())
                    challengeCardGUIDs.erase(std::find(challengeCardGUIDs.begin(), challengeCardGUIDs.end(), currentChampionGUID));

                hasQueueStarted = true;
                scheduler
                    .Schedule(Seconds(5), [this](TaskContext /*context*/)
                {
                    hasQueueStarted = false;
                    SetData(TYPE_QUEUE_NEXT, 1);
                });
                break;
        }
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_IN_QUEUE && std::find(playersInQueue.begin(), playersInQueue.end(), data) == playersInQueue.end())
        {
            playersInQueue.push_back(data);

            // Send players to arena if queue is empty btw :: do nothing if have anyone on arena
            if (playersInQueue.size() > 1)
                return;

            if (ObjectAccessor::GetPlayer(*me, currentChampionGUID))
                return;

            scheduler
                .Schedule(Seconds(5), [this](TaskContext /*context*/)
            {
                SetData(TYPE_QUEUE_NEXT, 1);
            });
            return;
        }

        if (type == TYPE_QUEUE_NEXT) // send new one join to arena
        {
            currentChampionGUID = ObjectGuid::Empty;

            for (auto&& itr : playersInQueue)
            {
                // try found new challenger
                if (Player* target = ObjectAccessor::GetPlayer(*me, ObjectGuid(HighGuid::Player, itr)))
                {
                    currentChampionGUID = target->GetGUID();

                    uint32 currentRep = target->GetReputation(target->GetTeam() == HORDE ? FACTION_BRAWLGAR_ARENA : FACTION_BIZMO_BRAWLPUB);
                    // Hello word!
                    for (auto&& itr : reputationYellType)
                    {
                        if (currentRep >= itr.second[0] && currentRep < itr.second[1])
                        {
                            Talk(itr.first, target);
                            break;
                        }
                    }

                    // Challenge Card queue
                    if (std::find(challengeCardGUIDs.begin(), challengeCardGUIDs.end(), target->GetGUID()) != challengeCardGUIDs.end())
                    {
                        if (uint32 challengeCardEntry = getAnyChallengeCard(target))
                        {
                            uint32 encounterAction = invChallengeCardsType.find(challengeCardEntry)->second;

                            // tele to arena & remove from temp queue & summon encounter depend of rank (btw per value)
                            if (uint32 EncounterEntry = EncounterMatchType.find(encounterAction)->second)
                            {
                                Position spawnPos = EncounterSpawnPos;

                                // Some encounters required not default spawn pos
                                switch (EncounterEntry)
                                {
                                    case NPC_BATTLETRON:
                                        spawnPos = AlianceCenterOfRectangle;
                                        break;
                                    case NPC_BLINGTRON_3000:
                                        spawnPos = blingtronSpawnPosA;
                                        break;
                                }

                                if (TempSummon* encounter = target->SummonCreature(EncounterEntry, spawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    currentEncounterGUID = encounter->GetGUID();
                                    encounter->AI()->SetGUID(target->GetGUID()); // send info about card.
                                }

                                target->DestroyItemCount(challengeCardEntry, 1, true);

                                berserkerEvents.ScheduleEvent(EVENT_BERKSERK, 2 * MINUTE * IN_MILLISECONDS + 2 * IN_MILLISECONDS);
                            }

                            target->RemoveAurasDueToSpell(SPELL_RING_OF_VALOR_IN_THE_ARENA);
                            target->CastSpell(target, SPELL_RING_OF_VALOR_IN_THE_ARENA, true);
                        }
                        else
                            DoAction(ACTION_WIPE);
                    }
                    else if (currentRep <= 6750) // temp, now available only 7 ranks
                    {
                        auto&& encounter = reputationEncounterType.find(currentRep);

                        // tele to arena & remove from temp queue & summon encounter depend of rank (btw per value)
                        if (uint32 EncounterEntry = EncounterMatchType.find(encounter->second)->second)
                        {
                            Position spawnPos = EncounterSpawnPos;

                            // Only Battletron summon here
                            if (EncounterEntry == NPC_BATTLETRON)
                                spawnPos = AlianceCenterOfRectangle;

                            if (TempSummon* encounter = target->SummonCreature(EncounterEntry, spawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                                currentEncounterGUID = encounter->GetGUID();

                            berserkerEvents.ScheduleEvent(EVENT_BERKSERK, 2 * MINUTE * IN_MILLISECONDS + 2 * IN_MILLISECONDS);
                        }

                        target->RemoveAurasDueToSpell(SPELL_RING_OF_VALOR_IN_THE_ARENA);
                        target->CastSpell(target, SPELL_RING_OF_VALOR_IN_THE_ARENA, true);

                        // Berserker
                    }
                    else
                        DoAction(ACTION_WIPE);

                    if (std::find(playersInQueue.begin(), playersInQueue.end(), itr) != playersInQueue.end())
                        playersInQueue.erase(std::find(playersInQueue.begin(), playersInQueue.end(), itr));
                    break;
                }
                else if (std::find(playersInQueue.begin(), playersInQueue.end(), itr) != playersInQueue.end())
                    playersInQueue.erase(std::find(playersInQueue.begin(), playersInQueue.end(), itr)); // remove not founded from queue
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
        berserkerEvents.Update(diff);

        while (uint32 eventId = berserkerEvents.ExecuteEvent())
        {
            if (eventId == EVENT_BERKSERK)
                DoAction(ACTION_WIPE);
            break;
        }
    }

    private:
        uint32 getAnyChallengeCard(Player* cardHolder)
        {
            std::list<uint32> challengeCards;

            for (auto&& itr : invChallengeCardsType)
                if (cardHolder->HasItemCount(itr.first, 1))
                    challengeCards.push_back(itr.first);

            if (challengeCards.empty())
                return 0;

            return Trinity::Containers::SelectRandomContainerElement(challengeCards);
        }
};

// Brawl`gar Arena Grunt 67267, Bizmos Brawlpub Bouncer 68408
class npc_brawlers_guild_brawlgar_arena_grunt : public CreatureScript
{
    public:
        npc_brawlers_guild_brawlgar_arena_grunt() : CreatureScript("npc_brawlers_guild_brawlgar_arena_grunt") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 1:
                    if (!player->HasAchieved(player->GetTeam() == HORDE ? ACHIEVEMENT_FIRST_RULE_OF_BRAWLERS_GUILD : ACHIEVEMENT_FIRST_RULE_OF_BRAWLERS_GUILD_A, false))
                    {
                        creature->AI()->Talk(TALK_DEATH); // need invite
                        break;
                    }

                    // Queue player to arena
                    player->CastSpell(player, SPELL_QUEUED_FOR_BRAWL, true);

                    if (Creature* arenaHolder = player->FindNearestCreature(creature->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                        arenaHolder->AI()->SetData(TYPE_IN_QUEUE, player->GetGUID().GetCounter());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 2:
                    // Get our Position in queue
                    if (Creature* arenaHolder = player->FindNearestCreature(creature->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    {
                        uint32 queuePos = (uint32)arenaHolder->AI()->GetGUID(player->GetGUID().GetCounter());
                        uint32 queueText = queuePos > 9 ? 724016 : queueGossipType.find(queuePos)->second;
                        player->SEND_GOSSIP_MENU(queueText, creature->GetGUID());
                        return false;

                    }
                    break;
                case GOSSIP_ACTION_INFO_DEF + 3:
                    // Queue player to arena
                    player->CastSpell(player, SPELL_QUEUED_FOR_BRAWL, true);

                    if (Creature* arenaHolder = player->FindNearestCreature(creature->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    {
                        arenaHolder->AI()->SetGUID(player->GetGUID(), TYPE_CHALLENGE_CARD);
                        arenaHolder->AI()->SetData(TYPE_IN_QUEUE, player->GetGUID().GetCounter());
                    }
                    break;
            }

            player->CLOSE_GOSSIP_MENU();
    
            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            // if player in queue - send gossip with queue sequence, if not - queue to arena
            if (Creature* arenaHolder = player->FindNearestCreature(creature->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
            {
                if (!arenaHolder->AI()->GetData(player->GetGUID().GetCounter()))
                {
                    player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                    // Challenge Cards
                    if (HasAnyChallengeCard(player))
                        player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                }
                else
                    player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            }

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool HasAnyChallengeCard(Player* cardHolder)
        {
            for (auto&& itr : invChallengeCardsType)
                if (cardHolder->HasItemCount(itr.first, 1))
                    return true;

            return false;
        }

        struct npc_brawlers_guild_brawlgar_arena_gruntAI : public ScriptedAI
        {
            npc_brawlers_guild_brawlgar_arena_gruntAI(Creature* creature) : ScriptedAI(creature) { }

            TaskScheduler scheduler;
            uint32 queueYell;
    
            void Reset() override
            {
                DoCast(me, SPELL_GUARD_AREATRIGGER);
                queueYell = 0;
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_IN_QUEUE)
                    return queueYell;

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_IN_QUEUE)
                {
                    queueYell = data;

                    // Not yell so much often
                    scheduler
                        .Schedule(Seconds(8), [this](TaskContext /*context*/)
                    {
                        queueYell = 0;
                    });
                }
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_brawlers_guild_brawlgar_arena_gruntAI(creature);
        }
};

// Boss Bazzelflange 67260
struct npc_brawlers_guild_boss_bazzelflange : public ScriptedAI
{
    npc_brawlers_guild_boss_bazzelflange(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap berserkerEvents;
    std::vector<uint32> playersInQueue;
    std::vector<ObjectGuid> challengeCardGUIDs;
    ObjectGuid currentChampionGUID;
    ObjectGuid currentEncounterGUID;
    bool hasQueueStarted;

    void InitializeAI() override
    {
        DoCast(me, SPELL_ACE_KIT);
        DoCast(me, SPELL_ACE_JET);
        DoCast(me, SPELL_HOVER_ANIM_STATE);

        me->OverrideInhabitType(INHABIT_AIR);
        me->SetAnimTier(AnimTier::Hover);
        me->UpdateMovementFlags();

        // Fly around arena perimetr
        scheduler
            .Schedule(Seconds(2), [this](TaskContext /*context*/)
        {
            Movement::MoveSplineInit init(me);
            for (auto&& itr : BazzelflangePath)
                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

            init.SetFly();
            init.SetCyclic();
            init.Launch();
        });

        Reset();
    }

    void Reset() override
    {
        playersInQueue.clear();
        challengeCardGUIDs.clear();
        currentChampionGUID = ObjectGuid::Empty;
        hasQueueStarted = false;
        berserkerEvents.Reset();
    }

    void SetGUID(ObjectGuid guid, int32 type) override
    {
        if (type == TYPE_CHALLENGE_CARD && std::find(challengeCardGUIDs.begin(), challengeCardGUIDs.end(), guid) == challengeCardGUIDs.end())
            challengeCardGUIDs.push_back(guid);
    }

    ObjectGuid GetGUID(int32 type) const override
    {
        uint64 i = 0; // default

        for (auto&& itr : playersInQueue)
        {
            if ((uint32)type != itr)
                i++;
            else
                break;
        }

        return ObjectGuid(i);
    }

    uint32 GetData(uint32 type) const override
    {
        if (std::find(playersInQueue.begin(), playersInQueue.end(), type) != playersInQueue.end())
            return 1;

        return 0;
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_WIPE:
                if (Creature* encounter = ObjectAccessor::GetCreature(*me, currentEncounterGUID))
                    encounter->DespawnOrUnsummon();

                berserkerEvents.Reset();

                // Remove from challenge card queue
                if (std::find(challengeCardGUIDs.begin(), challengeCardGUIDs.end(), currentChampionGUID) != challengeCardGUIDs.end())
                    challengeCardGUIDs.erase(std::find(challengeCardGUIDs.begin(), challengeCardGUIDs.end(), currentChampionGUID));

                if (Player* target = ObjectAccessor::GetPlayer(*me, currentChampionGUID))
                {
                    target->RemoveAurasDueToSpell(SPELL_BRAWLERS_GUILD_UNIT_FRAME);

                    // Tele out of arena if alive, respawn in 5s if dead
                    if (target->IsAlive())
                    {
                        Position wPos = target->GetMapId() == 1043 ? hordeWipePos : WipePos;
                        target->TeleportTo(target->GetMapId(), wPos.GetPositionX(), wPos.GetPositionY(), wPos.GetPositionZ(), wPos.GetOrientation(), TELE_TO_NOT_UNSUMMON_PET);
                    }
                    else
                    {
                        target->m_Events.Schedule(5000, [=]()
                        {
                            target->ResurrectPlayer(10.0f);
                            target->SpawnCorpseBones();

                            Position wPos = target->GetMapId() == 1043 ? hordeWipePos : WipePos;
                            target->TeleportTo(target->GetMapId(), wPos.GetPositionX(), wPos.GetPositionY(), wPos.GetPositionZ(), wPos.GetOrientation(), TELE_TO_NOT_UNSUMMON_PET);
                        });
                    }
                }

                if (!hasQueueStarted)
                {
                    hasQueueStarted = true;
                    scheduler
                        .Schedule(Seconds(5), [this](TaskContext /*context*/)
                    {
                        hasQueueStarted = false;
                        SetData(TYPE_QUEUE_NEXT, 1);
                    });
                }
                break;
            case ACTION_FORCE_DESPAWNED:
                if (hasQueueStarted)
                    break;

                // Remove from challenge card queue
                if (std::find(challengeCardGUIDs.begin(), challengeCardGUIDs.end(), currentChampionGUID) != challengeCardGUIDs.end())
                    challengeCardGUIDs.erase(std::find(challengeCardGUIDs.begin(), challengeCardGUIDs.end(), currentChampionGUID));

                hasQueueStarted = true;
                scheduler
                    .Schedule(Seconds(5), [this](TaskContext /*context*/)
                {
                    hasQueueStarted = false;
                    SetData(TYPE_QUEUE_NEXT, 1);
                });
                break;
        }
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_IN_QUEUE && std::find(playersInQueue.begin(), playersInQueue.end(), data) == playersInQueue.end())
        {
            playersInQueue.push_back(data);

            // Send players to arena if queue is empty btw :: do nothing if have anyone on arena
            if (playersInQueue.size() > 1)
                return;

            if (ObjectAccessor::GetPlayer(*me, currentChampionGUID))
                return;

            scheduler
                .Schedule(Seconds(5), [this](TaskContext /*context*/)
            {
                SetData(TYPE_QUEUE_NEXT, 1);
            });
            return;
        }

        if (type == TYPE_QUEUE_NEXT) // send new one join to arena
        {
            currentChampionGUID = ObjectGuid::Empty;

            for (auto&& itr : playersInQueue)
            {
                // try found new challenger
                if (Player* target = ObjectAccessor::GetPlayer(*me, ObjectGuid(HighGuid::Player, itr)))
                {
                    currentChampionGUID = target->GetGUID();

                    uint32 currentRep = target->GetReputation(target->GetTeam() == HORDE ? FACTION_BRAWLGAR_ARENA : FACTION_BIZMO_BRAWLPUB);
                    // Hello word!
                    for (auto&& itr : reputationYellType)
                    {
                        if (currentRep >= itr.second[0] && currentRep < itr.second[1])
                        {
                            Talk(itr.first, target);
                            break;
                        }
                    }

                    // Challenge Card queue
                    if (std::find(challengeCardGUIDs.begin(), challengeCardGUIDs.end(), target->GetGUID()) != challengeCardGUIDs.end())
                    {
                        if (uint32 challengeCardEntry = getAnyChallengeCard(target))
                        {
                            uint32 encounterAction = invChallengeCardsType.find(challengeCardEntry)->second;

                            // tele to arena & remove from temp queue & summon encounter depend of rank (btw per value)
                            if (uint32 EncounterEntry = EncounterMatchType.find(encounterAction)->second)
                            {
                                Position spawnPos = hordeEncounterSpawnPos;

                                // Some encounters required not default spawn pos
                                switch (EncounterEntry)
                                {
                                    case NPC_BATTLETRON:
                                        spawnPos = HordeCenterOfRectangle;
                                        break;
                                    case NPC_BLINGTRON_3000:
                                        spawnPos = blingtronSpawnPos;
                                        break;
                                }

                                if (TempSummon* encounter = target->SummonCreature(EncounterEntry, spawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    currentEncounterGUID = encounter->GetGUID();
                                    encounter->AI()->SetGUID(target->GetGUID()); // send info about card.
                                }

                                target->DestroyItemCount(challengeCardEntry, 1, true);

                                berserkerEvents.ScheduleEvent(EVENT_BERKSERK, 2 * MINUTE * IN_MILLISECONDS + 2 * IN_MILLISECONDS);
                            }

                            target->RemoveAurasDueToSpell(SPELL_RING_OF_VALOR_IN_THE_ARENA);
                            target->CastSpell(target, SPELL_RING_OF_VALOR_IN_THE_ARENA, true);
                        }
                        else
                            DoAction(ACTION_WIPE);
                    }
                    else if (currentRep <= 6750) // temp, now available only 7 ranks
                    {
                        auto&& encounter = reputationEncounterType.find(currentRep);

                        // tele to arena & remove from temp queue & summon encounter depend of rank (btw per value)
                        if (uint32 EncounterEntry = EncounterMatchType.find(encounter->second)->second)
                        {
                            Position spawnPos = hordeEncounterSpawnPos;

                            // Only Battletron summon here
                            if (EncounterEntry == NPC_BATTLETRON)
                                spawnPos = HordeCenterOfRectangle;

                            if (TempSummon* encounter = target->SummonCreature(EncounterEntry, spawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                                currentEncounterGUID = encounter->GetGUID();

                            berserkerEvents.ScheduleEvent(EVENT_BERKSERK, 2 * MINUTE * IN_MILLISECONDS + 2 * IN_MILLISECONDS);
                        }

                        target->RemoveAurasDueToSpell(SPELL_RING_OF_VALOR_IN_THE_ARENA);
                        target->CastSpell(target, SPELL_RING_OF_VALOR_IN_THE_ARENA, true);
                    }
                    else
                        DoAction(ACTION_WIPE);

                    if (std::find(playersInQueue.begin(), playersInQueue.end(), itr) != playersInQueue.end())
                        playersInQueue.erase(std::find(playersInQueue.begin(), playersInQueue.end(), itr));
                    break;
                }
                else if (std::find(playersInQueue.begin(), playersInQueue.end(), itr) != playersInQueue.end())
                        playersInQueue.erase(std::find(playersInQueue.begin(), playersInQueue.end(), itr)); // remove not founded from queue
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
        berserkerEvents.Update(diff);

        while (uint32 eventId = berserkerEvents.ExecuteEvent())
        {
            if (eventId == EVENT_BERKSERK)
                DoAction(ACTION_WIPE);
            break;
        }
    }

    private:
        uint32 getAnyChallengeCard(Player* cardHolder)
        {
            std::list<uint32> challengeCards;

            for (auto&& itr : invChallengeCardsType)
                if (cardHolder->HasItemCount(itr.first, 1))
                    challengeCards.push_back(itr.first);

            if (challengeCards.empty())
                return 0;

            return Trinity::Containers::SelectRandomContainerElement(challengeCards);
        }
};

// Bruce 67262
struct npc_brawlers_guild_bruce : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_bruce(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void Reset() override
    {
        events.Reset();
        targetGUID = ObjectGuid::Empty;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_CHOMP_CHOMP_CHOMP, urand(5 * IN_MILLISECONDS, 9.5 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_BRUCE_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CHOMP_CHOMP_CHOMP:
                    if (Unit* target = me->GetVictim())
                    {
                        targetGUID = target->GetGUID();
                        me->PrepareChanneledCast(me->GetAngle(target), SPELL_CHOMP_CHOMP_CHOMP);
                    }

                    events.ScheduleEvent(EVENT_CHOMP_CHOMP_CHOMP, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                    events.ScheduleEvent(EVENT_REMOVE_CHOMP, 3 * IN_MILLISECONDS);
                    break;
                case EVENT_REMOVE_CHOMP:
                    me->RemoveChanneledCast(targetGUID);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Vian the Volatile 67579
struct npc_brawlers_guild_vian_the_volatile : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_vian_the_volatile(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    uint32 volatileData, lineStep;

    void Reset() override
    {
        events.Reset();
        volatileData = 1;
        lineStep = 6;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTR0);
        events.ScheduleEvent(EVENT_VOLATILE_FLAMES, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_LAVA_BURST, 4 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_FIRE_LINE, urand(7 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
    }

    uint32 GetData(uint32 type) const override
    {
        switch (type)
        {
            case TYPE_VOLATILE_FLAMES:
                return volatileData;
            case TYPE_FIRELINE:
                return lineStep;
        }

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        switch (type)
        {
            case TYPE_VOLATILE_FLAMES:
                volatileData = data;
                break;
            case TYPE_FIRELINE:
                lineStep = data;
                break;
        }
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_VIAN_REMOVE_FIRELINE)
            me->RemoveChanneledCast(targetGUID);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_VIAN_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_VOLATILE_FLAMES:
                    DoCast(me, IsHordeArena() ? SPELL_VOLATILE_FLAMES_HORDE : SPELL_VOLATILE_FLAMES_ALIANCE);
                    events.ScheduleEvent(EVENT_VOLATILE_FLAMES, urand(12 * IN_MILLISECONDS, 17 * IN_MILLISECONDS));
                    break;
                case EVENT_LAVA_BURST:
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        events.RescheduleEvent(EVENT_LAVA_BURST, 5 * IN_MILLISECONDS);
                        break;
                    }

                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_LAVA_BURST);

                    events.ScheduleEvent(EVENT_LAVA_BURST, urand(8 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                    break;
                case EVENT_FIRE_LINE:
                    if (Unit* vict = me->GetVictim())
                    {
                        targetGUID = vict->GetGUID();
                        me->PrepareChanneledCast(me->GetAngle(vict), SPELL_FIRE_LINE);
                    }

                    lineStep = 6;
                    events.ScheduleEvent(EVENT_FIRE_LINE, urand(14 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

Position AlianceArenaGeodata()
{
    return { AlianceCenterOfRectangle.GetPositionX() + frand(-24.0f, 24.0f), AlianceCenterOfRectangle.GetPositionY() + frand(-17.0f, 17.0f), AlianceCenterOfRectangle.GetPositionZ(), AlianceCenterOfRectangle.GetOrientation() };
}

Position HordeArenaGeodata()
{
    return { HordeCenterOfRectangle.GetPositionX() + frand(-19.5f, 19.5f), HordeCenterOfRectangle.GetPositionY() + frand(-18.0f, 18.0f), HordeCenterOfRectangle.GetPositionZ(), HordeCenterOfRectangle.GetOrientation() };
}

// Volatile Flame 68379
struct npc_brawlers_guild_volatile_flame : public brawlers_miscAI
{
    npc_brawlers_guild_volatile_flame(Creature* creature) : brawlers_miscAI(creature) { }

    EventMap events;

    void Reset() override
    {
        me->SetDisplayId(11686);
        DoCast(me, SPELL_VOLATILE_FLAME_AT, true);

        if (Creature* vian = me->FindNearestCreature(NPC_VIAN_THE_VOLATILE, 100.0f, true))
        {
            if (uint32 volatileMovement = vian->AI()->GetData(TYPE_VOLATILE_FLAMES))
            {
                if (volatileMovement > 1)
                    me->GetMotionMaster()->MovePoint(0, me->GetMap()->GetId() == 1043 ? HordeArenaGeodata() : AlianceArenaGeodata());
                else
                    vian->AI()->SetData(TYPE_VOLATILE_FLAMES, 2);
            }
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (pointId == 0)
            events.ScheduleEvent(EVENT_MOVE, 100);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_MOVE)
                me->GetMotionMaster()->MovePoint(0, me->GetMap()->GetId() == 1043 ? HordeArenaGeodata() : AlianceArenaGeodata());
            break;
        }
    }
};

// Fire Line 67773
struct npc_brawlers_guild_fire_line : public brawlers_miscAI
{
    npc_brawlers_guild_fire_line(Creature* creature) : brawlers_miscAI(creature) { }

    EventMap events;
    float x, y;
    uint32 step;

    void Reset() override
    {
        x = 0.0f;
        y = 0.0f;
        me->SetDisplayId(11686);
        events.Reset();
        DoCast(me, SPELL_FIRE_LINE_AURA);

        if (Creature* vian = me->FindNearestCreature(NPC_VIAN_THE_VOLATILE, 100.0f, true))
            step = vian->AI()->GetData(TYPE_FIRELINE);

        if (step < 27)
            events.ScheduleEvent(EVENT_FIRE_LINE, 500);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_FIRE_LINE)
            {
                GetPositionWithDistInOrientation(me, (float)step, me->GetOrientation(), x, y);
                float z = me->GetPositionZ();
                me->UpdateAllowedPositionZ(x, y, z);
                me->CastSpell(x, y, z, SPELL_FIRE_LINE, true);
                step += 7;

                if (Creature* vian = me->FindNearestCreature(NPC_VIAN_THE_VOLATILE, 100.0f, true))
                    vian->AI()->SetData(TYPE_FIRELINE, step);
            }
            break;
        }
    }
};

// Goredome 68257
struct npc_brawlers_guild_goredome : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_goredome(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    ObjectGuid prevLumberingGUID;
    uint32 prevLumberingLow;

    void Reset() override
    {
        events.Reset();
        prevLumberingGUID = ObjectGuid::Empty;
        targetGUID = ObjectGuid::Empty;
        prevLumberingLow = 0;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_LUMBERING_CHARGE, 3 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->SetDisplayId(11686);
        summon->CastSpell(summon, SPELL_LUMBERING_CHARGE_MARK, true);
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_LUMBERING)
            return prevLumberingLow;

        return 0;
    }

    void MovementInform(uint32 /*type*/, uint32 pointId) override
    {
        if (pointId == EVENT_CHARGE)
        {
            DoCast(me, SPELL_LUMBERING_CHARGE_EFF);

            if (Creature* lumbering = ObjectAccessor::GetCreature(*me, prevLumberingGUID))
                lumbering->DespawnOrUnsummon();

            me->RemoveChanneledCast(targetGUID);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_GOREDOME_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_LUMBERING_CHARGE)
            {
                if (Unit* vict = me->GetVictim())
                {
                    if (Unit* LumberingTarget = me->SummonCreature(NPC_LUMBERING_CHARGE_TARGET, *vict, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        prevLumberingGUID = LumberingTarget->GetGUID();
                        prevLumberingLow = LumberingTarget->GetGUID().GetCounter();
                        targetGUID = vict->GetGUID();
                        me->PrepareChanneledCast(me->GetAngle(LumberingTarget), SPELL_LUMBERING_CHARGE);
                    }
                }

                events.ScheduleEvent(EVENT_LUMBERING_CHARGE, urand(6 * IN_MILLISECONDS, 7.5 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Dungeon Master Vishas 67572
struct npc_brawlers_guild_dungeon_master_vishas : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_dungeon_master_vishas(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTR0);
        events.ScheduleEvent(EVENT_HEATED_POKETS, 9 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_NAUGHTY_SECRETS, 23 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_VISHAS_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_NAUGHTY_SECRETS:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_NAUGHTY_SECRETS);

                    Talk(TALK_SPECIAL_2);
                    events.ScheduleEvent(EVENT_HEATED_POKETS, urand(20 * IN_MILLISECONDS, 24 * IN_MILLISECONDS));
                    break;
                case EVENT_HEATED_POKETS:
                    Talk(TALK_SPECIAL);
                    DoCast(me, SPELL_HEATED_POKERS);
                    events.ScheduleEvent(EVENT_HEATED_POKETS, 25 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Dippy 68255
struct npc_brawlers_guild_dippy : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_dippy(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_SLIPPY);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_DIPPY_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        if (Unit* victim = me->GetVictim())
            if (victim->IsWithinMeleeRange(me) && !me->HasUnitState(UNIT_STATE_CASTING))
                DoCast(victim, SPELL_PECK);
    }
};

// Kirrawk 67269
struct npc_brawlers_guild_kirrawk : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_kirrawk(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTR0);
        events.ScheduleEvent(EVENT_STORM_OF_CLOUD, 1.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_LIGHTNING_BOLT, 4.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SUMMON_TWISTER, 10 * IN_MILLISECONDS);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_TWISTER)
        {
            Talk(TALK_SPECIAL);

            if (Unit* vict = me->GetVictim())
                DoCast(vict, SPELL_LIGHTNING_FURY);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_KIRRAWK_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_LIGHTNING_BOLT:
                    if (Unit* victim = me->GetVictim())
                        DoCast(victim, SPELL_LIGHTNING_BOLT);

                    events.ScheduleEvent(EVENT_LIGHTNING_BOLT, 4.5 * IN_MILLISECONDS);
                    break;
                case EVENT_STORM_OF_CLOUD:
                    DoCast(me, SPELL_STORM_OF_CLOUD);
                    events.ScheduleEvent(EVENT_STORM_OF_CLOUD, 15 * IN_MILLISECONDS);
                    break;
                case EVENT_SUMMON_TWISTER:
                    DoCast(me, SPELL_SUMMON_TWISTER);
                    events.ScheduleEvent(EVENT_SUMMON_TWISTER, 10 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Twister 67270
struct npc_brawlers_guild_twister : public CreatureAI
{
    npc_brawlers_guild_twister(Creature* creature) : CreatureAI(creature) { }

    EventMap events;

    void Reset() override
    {
        me->SetDisplayId(11686);
        DoCast(me, SPELL_TWISTER_AURA);
        me->GetMotionMaster()->MovePoint(0, me->GetMap()->GetId() == 1043 ? HordeArenaGeodata() : AlianceArenaGeodata());
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (pointId == 0)
            events.ScheduleEvent(EVENT_MOVE, 100);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_MOVE)
                me->GetMotionMaster()->MovePoint(0, me->GetMap()->GetId() == 1043 ? HordeArenaGeodata() : AlianceArenaGeodata());
            break;
        }
    }
};

// Fran 67525
struct npc_brawlers_guild_fran : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_fran(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    bool fakeDeath, riddohDeath;

    void Reset() override
    {
        SetCombatMovement(false);
        fakeDeath = false;
        riddohDeath = false;
        events.Reset();

        if (TempSummon* riddoh = me->SummonCreature(NPC_RIDDOH, *me, TEMPSUMMON_MANUAL_DESPAWN))
            me->EnterVehicle(riddoh);
    }

    void JustEngagedWith(Unit* who) override
    {
        Talk(TALK_INTR0);
        events.ScheduleEvent(EVENT_THROW_DYMANITE, urand(3 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
        targetGUID = who->GetGUID();
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_RIDDOH_DEATH:
                if (fakeDeath)
                {
                    if (Unit* pVictim = ObjectAccessor::GetUnit(*me, targetGUID))
                        JustDied(pVictim);
                }
                else
                {
                    riddohDeath = true;
                    events.Reset();
                    me->ExitVehicle();
                }
                break;
            case ACTION_NET_TOSS:
                events.ScheduleEvent(EVENT_GOBLIN_LAND_MINE, 1 * IN_MILLISECONDS);
                break;
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (!riddohDeath && me->GetHealth() < damage && !fakeDeath)
        {
            damage = 0;

            if (Creature* riddoh = me->FindNearestCreature(NPC_RIDDOH, 100.0f, true))
                riddoh->CastSpell(riddoh, SPELL_FRENZIED_RIDDOH, true);

            me->RemoveAllAuras();
            me->ClearAllReactives();
            fakeDeath = true;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            me->SetReactState(REACT_PASSIVE);
            me->SetStandState(UNIT_STAND_STATE_DEAD);
            me->AttackStop();
            events.Reset();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_FRAN_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_THROW_DYMANITE:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_THROW_DYNAMITE);

                    events.ScheduleEvent(EVENT_THROW_DYMANITE, urand(6 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                    break;
                case EVENT_GOBLIN_LAND_MINE:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_GOBLIN_DEVICE);
                    break;
            }
        }

        if (riddohDeath && me->HasUnitState(UNIT_STATE_CASTING))
            if (Unit* vict = me->GetVictim())
                DoCast(vict, SPELL_THROW_DYNAMITE);

        DoMeleeAttackIfReady();
    }
};

// Riddoh 67524
struct npc_brawlers_guild_riddoh : public brawlers_miscAI
{
    npc_brawlers_guild_riddoh(Creature* creature) : brawlers_miscAI(creature) { }

    EventMap events;
    ObjectGuid ownerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        ownerGUID = summoner->GetGUID();
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_NET_TOSS, urand(6 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(TALK_DEATH);

        if (Creature* Fran = ObjectAccessor::GetCreature(*me, ownerGUID))
            Fran->AI()->DoAction(ACTION_RIDDOH_DEATH);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_NET_TOSS)
            {
                if (Unit* vict = me->GetVictim())
                    DoCast(vict, SPELL_NET_TOSS);

                if (Creature* fran = me->FindNearestCreature(NPC_FRAN, 100.0f, true))
                    fran->AI()->DoAction(ACTION_NET_TOSS);

                events.ScheduleEvent(EVENT_NET_TOSS, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Goblin Land Mine 67539
struct npc_brawlers_guild_goblin_land_mine : public brawlers_miscAI
{
    npc_brawlers_guild_goblin_land_mine(Creature* creature) : brawlers_miscAI(creature) { }

    EventMap events;

    void Reset() override
    {
        me->SetDisplayId(6271);
        events.ScheduleEvent(EVENT_EXPLOSION, 5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_EXPLOSION)
            {
                DoCast(me, SPELL_EXPLOSION);
                me->DespawnOrUnsummon(2 * IN_MILLISECONDS);
            }
            break;
        }
    }
};

// King Kulaka 68254
struct npc_brawlers_guild_king_kulaka : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_king_kulaka(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_DASH, urand(6 * IN_MILLISECONDS, 11 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_KULAKA_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_DASH)
            {
                DoCast(me, SPELL_DASH);
                events.ScheduleEvent(EVENT_DASH, urand(12 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Blat 67594
struct npc_brawlers_guild_blat : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_blat(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SPLIT, urand(4 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        if (Unit* target = me->GetVictim())
        {
            summons.Summon(summon);
            summon->SetInCombatWith(target);
            target->SetInCombatWith(summon);
            summon->AddThreat(target, 0.0f);

            if (summon->AI())
                summon->AI()->AttackStart(target);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_BLAT_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_SPLIT)
            {
                DoCast(me, SPELL_SPLIT);
                events.ScheduleEvent(EVENT_SPLIT, urand(4 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Sanoriak 67268
struct npc_brawlers_guild_sanoriak : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_sanoriak(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    TaskScheduler scheduler;
    uint32 wallCalled;
    float x, y;
    bool allowCast;

    void Reset() override
    {
        events.Reset();
        wallCalled = SPELL_FIREWALL_AT_4;
        allowCast = true;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTR0);
        events.ScheduleEvent(EVENT_FIREBALL, urand(4 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_PYROBLAST, 10 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SPECIAL, urand(8 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_FLAME_WALL, 5 * IN_MILLISECONDS);
    }

    void AttackStart(Unit* target) override
    {
        if (!target)
            return;

        if (me->Attack(target, false))
            DoStartNoMovement(target);
    }

    void CastInterrupted(SpellInfo const* /*spell*/) override
    {
        if (Unit* vict = me->GetVictim())
        {
            allowCast = false;
            me->GetMotionMaster()->MoveChase(vict);

            scheduler
                .Schedule(Seconds(4), [this](TaskContext /*context*/)
            {
                allowCast = true;
            });
        }
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_CREATE_FLAME_WALL)
        {
            if (Unit* target = me->GetVictim())
            {
                x = target->GetPositionX();
                y = target->GetPositionY();
                me->CastSpell(x, y, me->GetPositionZ(), wallCalled, true);
                wallCalled--;

                scheduler
                    .SetValidator([this] { return wallCalled >= SPELL_FIREWALL_AT_1; })
                    .Schedule(Seconds(1), [this](TaskContext context)
                {
                    me->CastSpell(x, y, me->GetPositionZ(), wallCalled, true);
                    wallCalled--;
                    context.Repeat(Seconds(1));
                });
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                    me->Attack(target, true);

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_SANORIAK_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING) || !allowCast)
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_FLAME_WALL:
                scheduler.CancelAll();
                wallCalled = SPELL_FIREWALL_AT_4;

                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_FIREWALL);

                events.ScheduleEvent(EVENT_FLAME_WALL, 16 * IN_MILLISECONDS);
                break;
                case EVENT_FIREBALL:
                    if (Unit* vict = me->GetVictim())
                    {
                        if (me->GetExactDist2d(vict) > 35.0f)
                            DoStartMovement(vict);
                        else
                            DoStartNoMovement(vict);

                        DoCast(vict, SPELL_FIREBALL);
                    }

                    events.ScheduleEvent(EVENT_FIREBALL, 3.5 * IN_MILLISECONDS);
                    break;
                case EVENT_PYROBLAST:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_PYROBLAST);

                    events.ScheduleEvent(EVENT_PYROBLAST, 10 * IN_MILLISECONDS);
                    break;
                case EVENT_SPECIAL:
                    Talk(TALK_SPECIAL);

                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_FLAME_BUFFET);

                    events.ScheduleEvent(EVENT_SPECIAL, urand(12 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Ixx 68253
struct npc_brawlers_guild_ixx : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_ixx(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void Reset() override
    {
        events.Reset();
        targetGUID = ObjectGuid::Empty;
        me->setRegeneratingHealth(false);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->SetHealth((uint32)(me->GetMaxHealth() * 0.2f));
        events.ScheduleEvent(EVENT_DEVASTATING_THRUST, 5 * IN_MILLISECONDS);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_DEVASTATING_THRUST)
            me->RemoveChanneledCast(targetGUID);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_IXX_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_DEVASTATING_THRUST)
            {
                if (Unit* target = me->GetVictim())
                {
                    targetGUID = target->GetGUID();
                    me->PrepareChanneledCast(me->GetAngle(target), SPELL_DEVASTATING_THRUST);
                }

                events.ScheduleEvent(EVENT_DEVASTATING_THRUST, urand(7 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Mazhareen 68251
struct npc_brawlers_guild_mazhareen : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_mazhareen(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_RAGEFUL_SPIRIT);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (Aura* rage = me->GetAura(SPELL_RAGEFUL_SPIRIT))
            rage->RecalculateAmountOfEffects();
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_MAZHAREEN_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        DoMeleeAttackIfReady();
    }
};

// Crush 67540
struct npc_brawlers_guild_crush : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_crush(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    float x, y, z;
    Position pos;
    uint32 notAllowCollisionCast;

    void Reset() override
    {
        events.Reset();
        targetGUID = ObjectGuid::Empty;
        notAllowCollisionCast = 1;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_COLLISION, 6 * IN_MILLISECONDS);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        switch (pointId)
        {
            case EVENT_JUMP:
                if (Unit* pVictim = ObjectAccessor::GetUnit(*me, targetGUID))
                {
                    DoCast(pVictim, SPELL_CHARGE_S);
                    me->SetFacingTo(me->GetAngle(pVictim));
                }

                events.ScheduleEvent(EVENT_COLLISION_LAUNCH, 1.5 * IN_MILLISECONDS);
                break;
            case EVENT_CHARGE:
                if (GetData(TYPE_CHARGING))
                    DoCast(me, SPELL_COLLISION);

                me->RemoveChanneledCast(targetGUID);
                break;
        }
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_CHARGING)
            return notAllowCollisionCast;

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_CHARGING)
            notAllowCollisionCast = data;
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_CRUSH_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_COLLISION:
                    if (Unit* vict = me->GetVictim())
                    {
                        targetGUID = vict->GetGUID();
                        me->PrepareChanneledCast(me->GetOrientation());
                    }
                    me->GetMotionMaster()->MoveJump(me->GetHomePosition(), 20.0f, 15.0f, EVENT_JUMP);
                    events.ScheduleEvent(EVENT_COLLISION, urand(15 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
                    break;
                case EVENT_COLLISION_LAUNCH:
                    if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                    {
                        notAllowCollisionCast = 1;
                        DoCast(me, SPELL_CHARGING);
                        GetPositionWithDistInOrientation(me, me->GetExactDist2d(target), me->GetAngle(target), x, y);
                        z = me->GetPositionZ();
                        me->UpdateAllowedPositionZ(x, y, z);
                        me->GetMotionMaster()->MoveCharge(x, y, z, 35.0f, EVENT_CHARGE);
                    }
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Leona Earthwind 68377
struct npc_brawlers_guild_leona_earthwind : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_leona_earthwind(Creature* creature) : brawlers_guild_encounter_typeAI(creature) 
    {
        SetCombatMovement(false);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_WRATH, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SOLAR_BEAM, 6.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_EARTHLIVING_SEED, 10.5 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        if (Unit* target = me->GetVictim())
        {
            summons.Summon(summon);
            summon->SetInCombatWith(target);
            target->SetInCombatWith(summon);
            summon->AddThreat(target, 0.0f);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                    me->Attack(target, true);

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_LEONA_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_WRATH:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_WRATH);

                    events.ScheduleEvent(EVENT_WRATH, 2.5 * IN_MILLISECONDS);
                    break;
                case EVENT_SOLAR_BEAM:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SOLAR_BEAM);

                    events.ScheduleEvent(EVENT_SOLAR_BEAM, 12.5 * IN_MILLISECONDS);
                    break;
                case EVENT_EARTHLIVING_SEED:
                    DoCast(me, SPELL_EARTHLIVING_SEED);
                    events.ScheduleEvent(EVENT_EARTHLIVING_SEED, 16 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Dominika the Illusionist 67485
struct npc_brawlers_guild_dominika_the_illusionist : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_dominika_the_illusionist(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTR0);
        DoCast(me, SPELL_ILLUSIONIST);
        events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(3 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        if (Unit* target = me->GetVictim())
        {
            summons.Summon(summon);
            summon->SetInCombatWith(target);
            target->SetInCombatWith(summon);
            summon->AddThreat(target, 0.0f);
            
            if (summon->AI())
                summon->AI()->AttackStart(target);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_DOMINIKA_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_ARCANE_BARRAGE)
            {
                if (Unit* vict = me->GetVictim())
                    DoCast(vict, SPELL_ARCANE_BARRAGE);

                events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(3 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Dominika Illusion 67489
struct npc_brawlers_guild_dominika_illusion : public brawlers_miscAI
{
    npc_brawlers_guild_dominika_illusion(Creature* creature) : brawlers_miscAI(creature) { }

    EventMap events;

    void Reset() override
    {
        SetCombatMovement(false);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(1 * IN_MILLISECONDS, 2 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_ARCANE_BARRAGE)
            {
                if (Unit* vict = me->GetVictim())
                    DoCast(vict, SPELL_ARCANE_BARRAGE);

                events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(3 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
            }
            break;
        }
    }
};

// Deeken 67451
struct npc_brawlers_guild_deeken : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_deeken(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTR0);

        float x, y, z;
        Position rightFieldPos = me->GetMapId() == 1043 ? lowRightField : lowRightFieldBizmo;
        uint8 posRightHigh = me->GetMapId() == 1043 ? 11 : 9;
        uint8 posLeftHigh= me->GetMapId() == 1043 ? 11 : 12;
        float angleDiff = me->GetMapId() == 1043 ? 40.0f : 44.0f;
        // Generate Static Field (up from right angle)
        for (uint8 i = 0; i < posRightHigh; i++)
        {
            x = rightFieldPos.GetPositionX() + 4.0f * i * cos(rightFieldPos.GetOrientation());
            y = rightFieldPos.GetPositionY() + 4.0f * i * sin(rightFieldPos.GetOrientation());
            z = me->GetPositionZ();
            me->UpdateAllowedPositionZ(x, y, z);
            me->SummonCreature(NPC_SHOCK_FIELD, x, y, z, M_PI, TEMPSUMMON_MANUAL_DESPAWN);
        }

        // Generate Static Field (left from right angle)
        for (uint8 i = 1; i < posLeftHigh; i++)
        {
            x = rightFieldPos.GetPositionX() + 4.0f * i * cos(Position::NormalizeOrientation(rightFieldPos.GetOrientation() + M_PI / 2));
            y = rightFieldPos.GetPositionY() + 4.0f * i * sin(Position::NormalizeOrientation(rightFieldPos.GetOrientation() + M_PI / 2));
            z = me->GetPositionZ();
            me->UpdateAllowedPositionZ(x, y, z);
            me->SummonCreature(NPC_SHOCK_FIELD, x, y, z, M_PI / 2, TEMPSUMMON_MANUAL_DESPAWN);
        }

        // Generate Static Field (up from left angle)
        for (uint8 i = 0; i < posRightHigh; i++)
        {
            x = (rightFieldPos.GetPositionX() + angleDiff * cos(Position::NormalizeOrientation(rightFieldPos.GetOrientation() + M_PI / 2))) + 4.0f * i * cos(rightFieldPos.GetOrientation());
            y = (rightFieldPos.GetPositionY() + angleDiff * sin(Position::NormalizeOrientation(rightFieldPos.GetOrientation() + M_PI / 2))) + 4.0f * i * sin(rightFieldPos.GetOrientation());
            z = me->GetPositionZ();
            me->UpdateAllowedPositionZ(x, y, z);
            me->SummonCreature(NPC_SHOCK_FIELD, x, y, z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
        }

        if (me->GetMapId() != 1043)
            angleDiff -= 8.0f;

        // Generate Static Field (left from up right angle)
        for (uint8 i = 1; i < posLeftHigh; i++)
        {
            x = (rightFieldPos.GetPositionX() + angleDiff * cos(rightFieldPos.GetOrientation())) + 4.0f * i * cos(Position::NormalizeOrientation(rightFieldPos.GetOrientation() + M_PI / 2));
            y = (rightFieldPos.GetPositionY() + angleDiff * sin(rightFieldPos.GetOrientation())) + 4.0f * i * sin(Position::NormalizeOrientation(rightFieldPos.GetOrientation() + M_PI / 2));
            z = me->GetPositionZ();
            me->UpdateAllowedPositionZ(x, y, z);
            me->SummonCreature(NPC_SHOCK_FIELD, x, y, z, (3 * M_PI) / 2, TEMPSUMMON_MANUAL_DESPAWN);
        }

        events.ScheduleEvent(EVENT_MOVE_FIELD, 2 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);

                    // Announce
                    if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                        arenaHolder->AI()->Talk(TALK_DEEKEN_ANN);
                }
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_MOVE_FIELD)
            {
                std::list<uint64> candidateFieldGUIDS;
                std::list<Creature*> fieldsList;
                GetCreatureListWithEntryInGrid(fieldsList, me, NPC_SHOCK_FIELD, 100.0f);

                fieldsList.remove_if([=](Creature* target) { return target && target->AI()->GetData(TYPE_FIELD_MOVING); });

                if (Creature* anyField = Trinity::Containers::SelectRandomContainerElement(fieldsList))
                {
                    fieldsList.remove_if([=](Creature* target) { return target && target->GetOrientation() != anyField->GetOrientation(); }); // any triggers of same lines has equal
                    fieldsList.remove_if([=](Creature* target) { return target && target->GetExactDist2d(anyField) > 8.0f; }); // remove farthest

                    uint32 fieldCount = urand(0, 2);

                    if (fieldCount && fieldsList.size() > fieldCount)
                        Trinity::Containers::RandomResizeList(fieldsList, fieldCount);

                    anyField->AI()->DoAction(ACTION_MOVE_SHOCK_FIELD);

                    if (fieldCount)
                    {
                        for (auto&& itr : fieldsList)
                            itr->AI()->DoAction(ACTION_MOVE_SHOCK_FIELD);
                    }
                }

                events.ScheduleEvent(EVENT_MOVE_FIELD, 1 * IN_MILLISECONDS);
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Shock Field 67459
struct npc_brawlers_shock_field : public brawlers_miscAI
{
    npc_brawlers_shock_field(Creature* creature) : brawlers_miscAI(creature) { }

    TaskScheduler scheduler;
    bool isReachDestination;
    uint32 isMoving;
    float x, y, z;

    void Reset() override
    {
        isReachDestination = false;
        isMoving = 0;

        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext /*context*/)
        {
            DoCast(me, SPELL_SHOCK_FIELD_AT);
        });
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_FIELD_MOVING)
            isMoving = data;
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_FIELD_MOVING)
            return isMoving;

        return 0;
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_MOVE_SHOCK_FIELD)
        {
            DoCast(me, SPELL_SHOCK_FIELD_SELECTOR);
            isMoving = 1;
            GetPositionWithDistInOrientation(me, 40.0f, isReachDestination ? Position::NormalizeOrientation(me->GetOrientation() - M_PI) : me->GetOrientation(), x, y);
            z = me->GetPositionZ();
            me->UpdateAllowedPositionZ(x, y, z);
            me->GetMotionMaster()->MovePoint(0, x, y, z);

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext /*context*/)
            {
                isReachDestination = true;
                isMoving = 0;

                if (Creature* dekeen = me->FindNearestCreature(NPC_DEEKEN, 100.0f, true))
                    dekeen->RemoveAurasDueToSpell(SPELL_SHOCK_FIELD_SELECTOR, me->GetGUID());
            });
        }
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Millie Watt 67591
struct npc_brawlers_guild_millie_watt : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_millie_watt(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void InitializeAI() override
    {
        SetCombatMovement(false);
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTR0);
        DoCast(me, SPELL_SHRINK_RAY_AFTERMATH);
        events.ScheduleEvent(EVENT_BUSTER_RAY, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_DYNAMITE, 7.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_DISCOMBOBUMORPHNATOR, urand(20 * IN_MILLISECONDS, 21 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                    me->Attack(target, false);

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_MILLIE_WATT_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DYNAMITE:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_ELECTRIC_DYNAMITE);

                    events.ScheduleEvent(EVENT_DYNAMITE, 7.5 * IN_MILLISECONDS);
                    break;
                case EVENT_DISCOMBOBUMORPHNATOR:
                    me->InterruptNonMeleeSpells(false);
                    Talk(TALK_SPECIAL_2);

                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_MEGAFANTASTIC_DISCOMBOBUMORPHNATOR);

                    events.ScheduleEvent(EVENT_DISCOMBOBUMORPHNATOR, urand(20 * IN_MILLISECONDS, 21 * IN_MILLISECONDS));
                    break;
                case EVENT_BUSTER_RAY:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_PHOTOPLASM_BUSTER_RAY);

                    events.ScheduleEvent(EVENT_BUSTER_RAY, 6 * IN_MILLISECONDS);
                    break;
            }
        }
    }
};

// Fjoll 67596
struct npc_brawlers_guild_fjoll : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_fjoll(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTR0);
        DoCast(me, SPELL_FLAMES_OF_FALLEN_GLORY);
        events.ScheduleEvent(EVENT_DARKZONE, 15 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_DARK_BARRAGE, urand(8 * IN_MILLISECONDS, 11 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_SHADOWY_BLAST, 19 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_FJOLL_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DARKZONE:
                    if (Unit* vict = me->GetVictim())
                        me->CastSpell(vict->GetPositionX(), vict->GetPositionY(), vict->GetPositionZ(), SPELL_DARKZONE, false);

                    events.ScheduleEvent(EVENT_DARKZONE, 15 * IN_MILLISECONDS);
                    break;
                case EVENT_DARK_BARRAGE:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_RARK_BARRAGE);

                    events.ScheduleEvent(EVENT_DARK_BARRAGE, urand(8 * IN_MILLISECONDS, 11 * IN_MILLISECONDS));
                    break;
                case EVENT_SHADOWY_BLAST:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_SHADOWY_BLAST);

                    events.ScheduleEvent(EVENT_SHADOWY_BLAST, 19 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Proboskus 68252
struct npc_brawlers_guild_proboskus : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_proboskus(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_RAIN_DANCE, 10 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_TORRENT, urand(5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_BERKSERK, 60 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_PROBKOSUS_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_RAIN_DANCE:
                    me->InterruptNonMeleeSpells(true);
                    DoCast(me, SPELL_RAIN_DANCE);
                    events.ScheduleEvent(EVENT_RAIN_DANCE, 20 * IN_MILLISECONDS);
                    break;
                case EVENT_TORRENT:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_TORRENT);

                    events.ScheduleEvent(EVENT_TORRENT, urand(9 * IN_MILLISECONDS, 15.5 * IN_MILLISECONDS));
                    break;
                case EVENT_BERKSERK:
                    DoCast(me, SPELL_BERSERK);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Lepregnomes 67515, 67514, 67513, 67511, 67516
struct npc_brawlers_guild_lepregnomes : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_lepregnomes(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    std::vector<ObjectGuid> LeproListGUID;
    bool fakeDeath;
    uint32 cLepro, leproOwnerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetVisible(false);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
        nonCombatEvents.ScheduleEvent(EVENT_INIT_BATTLE, 3 * IN_MILLISECONDS);

        fakeDeath = false;
        cLepro = 0;
        leproOwnerGUID = ObjectGuid::Empty;
        challengeCardGUID = ObjectGuid::Empty;

        if (me->GetEntry() == NPC_QUEAZY)
        {
            summonerGUID = summoner->GetGUID();

            for (auto&& leproList : Leprognomes)
                if (TempSummon* leprognome = me->SummonCreature(leproList, me->GetPositionX() + frand(-1.5f, 1.5f), me->GetPositionY() + frand(-2.5f, 2.5f), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                    LeproListGUID.push_back(leprognome->GetGUID());
        }
        else if (Creature* queazy = me->FindNearestCreature(NPC_QUEAZY, 100.0f, true))
        {
            targetGUID.Set(queazy->AI()->GetGUID());
            summonerGUID = targetGUID;
        }
    }

    void Reset() override
    {
        events.Reset();
    }

    ObjectGuid GetGUID(int32 type) const override
    {
        return me->GetEntry() == NPC_QUEAZY ? summonerGUID : ObjectGuid::Empty;
    }

    void JustEngagedWith(Unit* who) override
    {
        if (me->GetEntry() == NPC_SLEAZY)
            Talk(TALK_INTR0);

        events.ScheduleEvent(EVENT_SPEW, urand(2 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_SHUFFLE, 10 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_LEPRO_DEATH:
                if (++cLepro > 4)
                    DoAction(ACTION_LEPRO_DEATH_ENCOUNTER);
                break;
            case ACTION_LEPRO_DEATH_ENCOUNTER:
                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                    JustDied(target);
                break;
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (me->GetEntry() == NPC_QUEAZY && me->GetHealth() < damage && !fakeDeath)
        {
            damage = 0;
            me->RemoveAllAuras();
            me->ClearAllReactives();
            fakeDeath = true;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            me->SetReactState(REACT_PASSIVE);
            me->SetStandState(UNIT_STAND_STATE_DEAD);
            me->AttackStop();
            events.Reset();
            DoAction(ACTION_LEPRO_DEATH);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }
                else if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID)) // for queazy
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                if (me->GetEntry() == NPC_QUEAZY)
                {
                    // Announce
                    if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                        arenaHolder->AI()->Talk(TALK_LEPREGNOMES_ANN);
                }
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SPEW:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_LEPEROUS_SPEW);

                    events.ScheduleEvent(EVENT_SPEW, urand(3 * IN_MILLISECONDS, 3.5 * IN_MILLISECONDS));
                    break;
                case EVENT_SHUFFLE:
                    DoCast(me, SPELL_DETERMINED_SHUFFLE);
                    events.ScheduleEvent(EVENT_SHUFFLE, 10 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Yikkan Izu 68256
struct npc_brawlers_guild_yikkan_izu : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_yikkan_izu(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void HandleSummonRavens()
    {
        for (uint8 i = 0; i < 4; ++i)
            me->SummonCreature(NPC_IZUS_RAVEN, me->GetPositionX() + frand(-3.0f, 3.0f), me->GetPositionY() + frand(-4.0f, 4.0f), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
    }

    void JustEngagedWith(Unit* who) override
    {
        HandleSummonRavens();
        events.ScheduleEvent(EVENT_RAVENS, urand(10 * IN_MILLISECONDS, 12.5 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_SHRIEK, 7.5 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        if (Unit* target = me->GetVictim())
            summon->Attack(target, false);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, BrawlersArenaPredicate(me)))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_YIKKAN_IZU_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_RAVENS:
                    HandleSummonRavens();
                    events.ScheduleEvent(EVENT_RAVENS, urand(10 * IN_MILLISECONDS, 12.5 * IN_MILLISECONDS));
                    break;
                case EVENT_SHRIEK:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_DISORIENTING_SHRIEK);

                    events.ScheduleEvent(EVENT_SHRIEK, urand(16 * IN_MILLISECONDS, 21.5 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Izus Raven 68268
struct npc_brawlers_guild_izus_raven : public CreatureAI
{
    npc_brawlers_guild_izus_raven(Creature* creature) : CreatureAI(creature) { }

    void JustDied(Unit* killer) override
    {
        DoCast(me, SPELL_FALLEN_KIN);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Akama 67588
struct npc_brawlers_guild_akama : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_akama(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        Talk(TALK_INTR0);
        events.ScheduleEvent(EVENT_FERAL_SPIRIT, 8 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SHADOW_STRIKES, 2.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_THUNDERSTORM, urand(14 * IN_MILLISECONDS, 39 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 5 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        if (Unit* target = me->GetVictim())
            summon->Attack(target, false);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, BrawlersArenaPredicate(me)))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_AKAMA_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SHADOW_STRIKES:
                    DoCast(me, SPELL_SHADOW_STRIKES);
                    events.ScheduleEvent(EVENT_SHADOW_STRIKES, urand(28 * IN_MILLISECONDS, 35 * IN_MILLISECONDS));
                    break;
                case EVENT_FERAL_SPIRIT:
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        events.RescheduleEvent(EVENT_FERAL_SPIRIT, 2 * IN_MILLISECONDS);
                        break;
                    }
                    Talk(TALK_SPECIAL);
                    DoCast(me, SPELL_FERAL_SPIRIT);
                    events.ScheduleEvent(EVENT_FERAL_SPIRIT, urand(19 * IN_MILLISECONDS, 21 * IN_MILLISECONDS));
                    break;
                case EVENT_CHAIN_LIGHTNING:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_CHAIN_LIGHTNING, false);

                    events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(7 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
                    break;
                case EVENT_THUNDERSTORM:
                    DoCast(me, SPELL_THUNDERSTORM);
                    events.ScheduleEvent(EVENT_THUNDERSTORM, urand(14 * IN_MILLISECONDS, 39 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Smash Hoofstomp 68305
struct npc_brawlers_guild_smash_hoofstomp : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_smash_hoofstomp(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTR0);

        // Summon Gnomes on Shoulders
        for (uint8 i = 0; i < 2; i++)
            if (Creature* luckyGoblin = me->SummonCreature(NPC_GOOD_LUCK_GOBLIN, *me, TEMPSUMMON_MANUAL_DESPAWN))
                luckyGoblin->EnterVehicle(me, i);

        events.ScheduleEvent(EVENT_HOOF_STOMP, 6 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SHIELD_WALLER, 9 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, BrawlersArenaPredicate(me)))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_HOOFSTOMP_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_HOOF_STOMP:
                    DoCast(me, SPELL_HOOF_STOMP);
                    events.ScheduleEvent(EVENT_HOOF_STOMP, 23 * IN_MILLISECONDS);
                    break;
                case EVENT_SHIELD_WALLER:
                    DoCast(me, SPELL_SHIELD_WALLER);
                    events.ScheduleEvent(EVENT_HOOF_STOMP, 26 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Good Luck Goblin 68306
struct npc_brawlers_guild_good_luck_goblin : public ScriptedAI
{
    npc_brawlers_guild_good_luck_goblin(Creature* creature) : ScriptedAI(creature) { }

    void JustDied(Unit* killer) override
    {
        DoCast(me, SPELL_TOUGHT_LUCK);

        if (Creature* hoofstomp = me->FindNearestCreature(NPC_SMASH_HOOFSTOMP, 100.0f, true))
            hoofstomp->AI()->Talk(TALK_SPECIAL_2);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Unguloxx 68250
struct npc_brawlers_guild_unguloxx : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_unguloxx(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, BrawlersArenaPredicate(me)))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_UNGULOXX_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Max Megablast, Bo Bobble 67488, 67487
struct npc_brawlers_guild_gg_ingeenering : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_gg_ingeenering(Creature* creature) : brawlers_guild_encounter_typeAI(creature) 
    {
        hasinit = false;
    }

    bool hasinit;

    void Reset() override
    {
        hasTele = false;
        hasYell = false;
        SetCombatMovement(false);
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        targetGUID = who->GetGUID();

        if (me->GetEntry() == NPC_MAX_MEGABLAST && !hasinit)
        {
            hasinit = true;
            Talk(TALK_INTR0);

            auto& generatorPos = me->GetMapId() == 1043 ? ShieldGeneratorHorde : ShieldGeneratorAliance;
            for (auto&& itr : generatorPos)
                me->SummonCreature(NPC_SHIELD_GENERATOR, itr, TEMPSUMMON_MANUAL_DESPAWN);

            events.ScheduleEvent(EVENT_TELEPORT, 20 * IN_MILLISECONDS);
        }
        else if (Creature* max = me->FindNearestCreature(NPC_MAX_MEGABLAST, 100.0f, true))
        {
            max->CastSpell(me, SPELL_HAPHAZARD_HEALTH_LINK, true);
            DoCast(max, SPELL_HAPHAZARD_HEALTH_LINK, true);
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        switch (summon->GetEntry())
        {
            case NPC_SHIELD_GENERATOR:
                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                break;
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(me->GetEntry() == NPC_BO_BOBBLE ? 50 : 25) && me->GetEntry() == NPC_BO_BOBBLE && !hasYell)
        {
            hasYell = true;
            Talk(TALK_SPECIAL);
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
            hasTele = false;
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (me->GetEntry() == NPC_MAX_MEGABLAST)
                {
                    if (Creature* bobble = me->SummonCreature(NPC_BO_BOBBLE, me->GetPositionX() + frand(4.0f, 8.0f), me->GetPositionY() + frand(-6.5f, -4.0f), me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                        {
                            me->Attack(target, true);
                            bobble->Attack(target, true);
                        }
                        else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, BrawlersArenaPredicate(me)))
                        {
                            me->Attack(target, true);
                            bobble->Attack(target, true);
                        }
                    }

                    // Announce
                    if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                        arenaHolder->AI()->Talk(TALK_GG_ENGINEERING_ANN);
                }
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_TELEPORT)
            {
                Talk(me->GetEntry() == NPC_MAX_MEGABLAST ? TALK_DEATH : TALK_INTR0);

                if (me->FindNearestCreature(NPC_SHIELD_GENERATOR, 100.0f, true)) // only if generator allowed
                    hasTele = true;

                me->InterruptNonMeleeSpells(true);
                DoCast(me, SPELL_EMERGENCY_TELEPORT);
                events.ScheduleEvent(EVENT_TELEPORT, urand(16 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
            }
            break;
        }

        if (Unit* victim = me->GetVictim())
            if (!me->HasUnitState(UNIT_STATE_CASTING) && !hasTele)
                DoCast(victim, me->GetEntry() == NPC_MAX_MEGABLAST ? SPELL_GOBLIN_ROCKET_BARRAGE : SPELL_GNOMISH_DEATH_RAY);
    }
};

// Dark Summoner 67678
struct npc_brawlers_guild_dark_summoner : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_dark_summoner(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        Talk(TALK_INTR0);

        if (Player* target = ObjectAccessor::GetPlayer(*me, summonerGUID)) // Announce
            Talk(TALK_SPECIAL, target);

        who->CastSpell(who, SPELL_GUARDIANS_LIGHT, true);
        DoCast(me, SPELL_DARK_PROTECTION);
        events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_SUMMON_GHOST, 5.5 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        if (Unit* target = me->GetVictim())
            summon->Attack(target, false);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_GHOST_DEATH)
        {
            if (Player* target = ObjectAccessor::GetPlayer(*me, summonerGUID))
                Talk(TALK_SPECIAL_2, target);

            me->RemoveAurasDueToSpell(SPELL_DARK_PROTECTION);
            events.ScheduleEvent(EVENT_RESTORE_SHIELD, 19 * IN_MILLISECONDS);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, BrawlersArenaPredicate(me)))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_DARK_SUMMONER_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SHADOW_BOLT:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_SHADOW_BOLT, false);

                    events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                    break;
                case EVENT_SUMMON_GHOST:
                    DoCast(me, SPELL_SUMMON_GHOST);
                    events.ScheduleEvent(EVENT_SUMMON_GHOST, urand(13 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
                    break;
                case EVENT_RESTORE_SHIELD:
                    if (Player* target = ObjectAccessor::GetPlayer(*me, summonerGUID))
                        Talk(TALK_SPECIAL_3, target);

                    me->RemoveAurasDueToSpell(SPELL_REVENGE_OF_DEPARTED);
                    DoCast(me, SPELL_DARK_PROTECTION);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Tormented Ghost 67664
struct npc_brawlers_guild_tormented_ghost : public CreatureAI
{
    npc_brawlers_guild_tormented_ghost(Creature* creature) : CreatureAI(creature) { }

    ObjectGuid targetGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        targetGUID = summoner->GetGUID();
    }

    void JustDied(Unit* killer) override
    {
        if (Creature* dSummoner = ObjectAccessor::GetCreature(*me, targetGUID))
        {
            if (!dSummoner->HasAura(SPELL_REVENGE_OF_DEPARTED))
            {
                dSummoner->CastSpell(dSummoner, SPELL_REVENGE_OF_DEPARTED);
                dSummoner->AI()->DoAction(ACTION_GHOST_DEATH);
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Battletron 67424
struct npc_brawlers_guild_battletron : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_battletron(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    std::vector<ObjectGuid> BattleMinesGUIDs;
    uint32 mDeathCount;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
        SetCombatMovement(false);
        events.Reset();
        BattleMinesGUIDs.clear();
        mDeathCount = 0;
    }

    void JustEngagedWith(Unit* who) override
    {
        Talk(TALK_INTR0);
        DoCast(me, SPELL_PROTECTED_BY_UNSTABLE_MINES);
        DoCast(me, SPELL_CONJURE_UNSTABLE_MINES);
        events.ScheduleEvent(EVENT_ZOT, urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->CastSpell(summon, SPELL_UNSTABLE_MINE_AURA, true);
        summon->SetDisplayId(11686);
        summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
        BattleMinesGUIDs.push_back(summon->GetGUID());
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_BATTLEMINE_DEATH)
        {
            mDeathCount++;

            if (mDeathCount > 5)
            {
                for (auto&& mItr : BattleMinesGUIDs)
                {
                    if (Creature* battlemine = ObjectAccessor::GetCreature(*me, mItr))
                        battlemine->DespawnOrUnsummon();
                }

                BattleMinesGUIDs.clear();
                mDeathCount = 0;
                DoCast(me, SPELL_CONJURE_UNSTABLE_MINES);
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                    me->Attack(target, true);
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, BrawlersArenaPredicate(me)))
                    me->Attack(target, true);

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_BATTLETRON_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_ZOT)
            {
                if (Unit* victim = me->GetVictim())
                {
                    DoCast(victim, SPELL_ZOT);
                    events.ScheduleEvent(EVENT_ZOT, 5 * IN_MILLISECONDS);
                }
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

//// current range to Ertan
//float GetCirclePathAround(uint64 casterGUID, uint64 BattleTronGUID)
//{
//    if (Unit* m_caster = ObjectAccessor::GetUnit(casterGUID))
//        if (Unit* BattleTron = ObjectAccessor::GetUnit(BattleTronGUID))
//            return m_caster->GetExactDist2d(BattleTron);
//
//    return 0.0f;
//}

// Approximate this
//Position GetBattleMineSpawnPos(float m_ori, uint64 casterGUID, float m_dist = 3.5f)
//{
//    Unit* caster = ObjectAccessor::FindUnit(casterGUID);
//
//    if (!caster)
//        return{ 0.0f, 0.0f, 0.0f, 0.0f };
//
//    float x, y;
//    GetPositionWithDistInOrientation(caster, m_dist, m_ori, x, y);
//
//    Position pos = { x, y, caster->GetPositionZ(), 0.0f };
//
//    // At spawn vortex should look at Battletron
//    return{ x, y, caster->GetPositionZ(), pos.GetAngle(caster) };
//}

// Unstable Mine 67422
struct npc_brawlers_guild_unstable_mine : public CreatureAI
{
    npc_brawlers_guild_unstable_mine(Creature* creature) : CreatureAI(creature) { }

    ObjectGuid targetGUID;
    EventMap events;
    float x, y;

    void IsSummonedBy(Unit* summoner) override
    {
        targetGUID = summoner->GetGUID();
        x = 0.0f; y = 0.0f;
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (pointId == 0)
        {
            me->StopMoving();
            me->GetMotionMaster()->Clear();

            if (Unit* summoner = ObjectAccessor::GetUnit(*me, targetGUID))
            {
                Movement::MoveSplineInit init(me);
                for (uint8 i = 1; i < 13; ++i)
                {
                    x = summoner->GetPositionX() + 4.5f * cos(Position::NormalizeOrientation(me->GetAngle(summoner->GetPositionX(), summoner->GetPositionY()) + (i * M_PI / 6) - M_PI));
                    y = summoner->GetPositionY() + 4.5f * sin(Position::NormalizeOrientation(me->GetAngle(summoner->GetPositionX(), summoner->GetPositionY()) + (i * M_PI / 6) - M_PI));
                    init.Path().push_back(G3D::Vector3(x, y, me->GetPositionZ()));
                }

                init.SetWalk(true);
                init.SetFly();
                init.SetUncompressed();
                init.SetCyclic();
                init.Launch();
            }
        }
    }

    void JustDied(Unit* killer) override
    {
        DoCast(me, SPELL_EXPLOSION_MINE);

        if (Creature* BattleTron = ObjectAccessor::GetCreature(*me, targetGUID))
            BattleTron->AI()->DoAction(ACTION_BATTLEMINE_DEATH);

        me->DespawnOrUnsummon(600);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Meatball 67573
struct npc_brawlers_guild_meatball : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_meatball(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    uint32 yellTimer, yellType;

    void Reset() override
    {
        events.Reset();
        yellTimer = 2;
        yellType = TALK_SPECIAL;
    }

    void JustEngagedWith(Unit* who) override
    {
        Talk(TALK_INTR0);
        events.ScheduleEvent(EVENT_RAGE, 1 * MINUTE * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_YELL, yellTimer * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, BrawlersArenaPredicate(me)))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_MEATBALL_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_RAGE:
                    Talk(TALK_SPECIAL_4);
                    DoCast(me, SPELL_MEATBALL_RAGE);
                    break;
                case EVENT_YELL:
                    yellTimer *= 3;
                    Talk(yellType);
                    yellType++;

                    if (yellType <= TALK_SPECIAL_3)
                        events.ScheduleEvent(EVENT_YELL, yellTimer * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Mecha-Bruce 71081
struct npc_brawlers_guild_mecha_bruce : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_mecha_bruce(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void Reset() override
    {
        DoCast(me, SPELL_MECHA_ARMOR_ALPHA);
        events.Reset();
        targetGUID = ObjectGuid::Empty;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_CHOMP_CHOMP_CHOMP, 2.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_POWERFUL_BITE, 10 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_BETTER_STRONGER_FASTER, 9 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_STASIS_BEAM, 16 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_MECHA_BRUCE_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BETTER_STRONGER_FASTER:
                    DoCast(me, SPELL_BETTER_STRONGER_FASTER);
                    events.ScheduleEvent(EVENT_BETTER_STRONGER_FASTER, 28 * IN_MILLISECONDS);
                    break;
                case EVENT_STASIS_BEAM:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_STASIS_BEAM);

                    events.ScheduleEvent(EVENT_STASIS_BEAM, 25 * IN_MILLISECONDS);
                    break;
                case EVENT_CHOMP_CHOMP_CHOMP:
                    if (Unit* target = me->GetVictim())
                    {
                        targetGUID = target->GetGUID();
                        me->PrepareChanneledCast(me->GetAngle(target), SPELL_CHOMP_CHOMP_CHOMP_MECHA);
                    }

                    events.ScheduleEvent(EVENT_CHOMP_CHOMP_CHOMP, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                    events.ScheduleEvent(EVENT_REMOVE_CHOMP, 2 * IN_MILLISECONDS);
                    break;
                case EVENT_REMOVE_CHOMP:
                    me->RemoveChanneledCast(targetGUID);
                    break;
                case EVENT_POWERFUL_BITE:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_POWERFUL_BITE);

                    events.ScheduleEvent(EVENT_POWERFUL_BITE, 11 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Grandpa Grumplefloot 70678
struct npc_brawlers_guild_grandpa_grumplefloot : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_grandpa_grumplefloot(Creature* creature) : brawlers_guild_encounter_typeAI(creature) { }

    void Reset() override
    {
        events.Reset();
        targetGUID = ObjectGuid::Empty;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_DEATH);
        events.ScheduleEvent(EVENT_SONG_OF_FLOOT, 3 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CANTATA_OF_FLOOTING, 10 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_THROW_LUCKYDO_NT, 2 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_THROW_BAD_LUCKYDO, 8 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->CastSpell(summon, summon->GetEntry() == NPC_LUCKYDO ? SPELL_LUCKYDO_AT : SPELL_LUCKYDONT_AT, true);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    me->Attack(target, true);
                    me->GetMotionMaster()->MoveChase(target);
                }

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_GRANDPA_GRUMPLEFLOOT_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SONG_OF_FLOOT:
                    Talk(TALK_INTR0);

                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SONG_OF_FLOOT);

                    events.ScheduleEvent(EVENT_SONG_OF_FLOOT, 7.5 * IN_MILLISECONDS);
                    break;
                case EVENT_CANTATA_OF_FLOOTING:
                    DoCast(me, SPELL_CANTATA_OF_FLOOTING);
                    events.ScheduleEvent(EVENT_CANTATA_OF_FLOOTING, 13.5 * IN_MILLISECONDS);
                    break;
                case EVENT_THROW_LUCKYDO_NT:
                    DoCast(me, SPELL_THROW_LUCKYDO_NT_LAUNCHER);
                    events.ScheduleEvent(EVENT_THROW_LUCKYDO_NT, 2 * IN_MILLISECONDS);
                    break;
                case EVENT_THROW_BAD_LUCKYDO:
                    DoCast(me, SPELL_THROW_BAD_LUCKYDO);
                    events.ScheduleEvent(EVENT_THROW_BAD_LUCKYDO, 10 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Blingtron 3000 70740
struct npc_brawlers_guild_blingtron_3000 : public brawlers_guild_encounter_typeAI
{
    npc_brawlers_guild_blingtron_3000(Creature* creature) : brawlers_guild_encounter_typeAI(creature) 
    {
        hasInit = false;
    }

    bool hasInit;

    void Reset() override
    {
        SetCombatMovement(false);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTR0);
        events.ScheduleEvent(EVENT_MOSTLY_ACCURATE_ROCKET,  2 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_VISUAL_CHARGING, 7.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SUMMON_BLINGTRON, 0.5 * IN_MILLISECONDS);

        // Summon gold pile & tesla coil
        if (!hasInit)
        {
            hasInit = true;
            me->SummonCreature(NPC_GNOMISH_TESLA_COIL, me->GetMapId() == 1043 ? teslaCoilSpawnPos : teslaCoilSpawnPosA, TEMPSUMMON_MANUAL_DESPAWN);

            float x = me->GetPositionX() + 2.0f * cos(Position::NormalizeOrientation(me->GetHomePosition().GetOrientation() + M_PI / 6));
            float y = me->GetPositionY() + 2.0f * sin(Position::NormalizeOrientation(me->GetHomePosition().GetOrientation() + M_PI / 6));
            me->SummonCreature(NPC_PILE_OF_GOLD, x, y, me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN);
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        if (summon->GetEntry() != NPC_BLINGTRON_2000)
            return;

        if (Unit* target = me->GetVictim())
        {
            summons.Summon(summon);
            summon->SetInCombatWith(target);
            target->SetInCombatWith(summon);
            summon->AddThreat(target, 0.0f);

            if (summon->AI())
                summon->AI()->AttackStart(target);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_BATTLE)
            {
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                    me->Attack(target, true);

                // Announce
                if (Creature* arenaHolder = me->FindNearestCreature(me->GetMapId() == 1043 ? NPC_BOSS_BAZZELFLANGE : NPC_BIZMO, 100.0f, true))
                    arenaHolder->AI()->Talk(TALK_BLINGTRON_3000_ANN);
            }
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_MOSTLY_ACCURATE_ROCKET:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_MOSTLY_ACCURATE_ROCKET);

                    events.ScheduleEvent(EVENT_MOSTLY_ACCURATE_ROCKET, urand(5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                    break;
                case EVENT_SUMMON_BLINGTRON:
                    if (Creature* goldPile = me->FindNearestCreature(NPC_PILE_OF_GOLD, 30.0f, true))
                        me->SummonCreature(NPC_BLINGTRON_2000, *goldPile, TEMPSUMMON_MANUAL_DESPAWN);

                    events.ScheduleEvent(EVENT_SUMMON_BLINGTRON, 10 * IN_MILLISECONDS);
                    break;
                case EVENT_VISUAL_CHARGING:
                    ClearBlingtrons();
                    DoCast(me, SPELL_CHAIN_LIGHTNING_VISUAL);
                    events.ScheduleEvent(EVENT_VISUAL_CHARGING, 9 * IN_MILLISECONDS);
                    break;
            }
        }
    }

    private:
        void ClearBlingtrons()
        {
            std::list<Creature*> blingtrons_2000;
            GetCreatureListWithEntryInGrid(blingtrons_2000, me, NPC_BLINGTRON_2000, 100.0f);

            for (auto&& itr : blingtrons_2000)
                itr->AI()->SetData(TYPE_BLINGTRON_CHARGED, 0);
        }
};

// Gnomish Tesla Coil 70743
struct npc_brawlers_guild_gnomish_tesla_coil : public ScriptedAI
{
    npc_brawlers_guild_gnomish_tesla_coil(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    bool hasClicked;

    void Reset() override
    {
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        hasClicked = false;
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        damage = 0;
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_CHARGE_CONDUIT && !hasClicked)
        {
            hasClicked = true;
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

            DoCast(me, SPELL_OVERCHARGED);

            scheduler
                .Schedule(Seconds(9), [this](TaskContext /*context*/)
            {
                Reset();
            });
        }
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        clicker->CastSpell(me, SPELL_OVERCHARGING_TESLA_COIL_VISUAL, false);
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Blingtron 2000 70802
struct npc_brawlers_guild_blingtron_2000 : public CreatureAI
{
    npc_brawlers_guild_blingtron_2000(Creature* creature) : CreatureAI(creature) { }

    bool hasDefeat;
    uint32 isCharged;

    void Reset() override
    {
        hasDefeat = false;
        isCharged = 0;
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (hasDefeat)
            damage = 0;

        if (damage >= me->GetHealth() && !hasDefeat)
        {
            damage = 0;
            hasDefeat = true;
            DoCast(me, SPELL_MALFUNCTIONIC, true);
        }
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_BLINGTRON_CHARGED)
            return isCharged;

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_BLINGTRON_CHARGED)
            isCharged = data;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Card Traider Leila 70752, Card Traider Ami 70751
class npc_brawlers_guild_card_traider : public CreatureScript
{
    public:
        npc_brawlers_guild_card_traider() : CreatureScript("npc_brawlers_guild_card_traider") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

                player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                player->GetSession()->SendListInventory(creature->GetGUID());

            return false;
        }
};

// Join to Arena 132633
class spell_brawlers_guild_join_to_arena : public AuraScript
{
    PrepareAuraScript(spell_brawlers_guild_join_to_arena);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player* owner = GetOwner()->ToPlayer())
        {
            switch (owner->GetTeam())
            {
                case ALLIANCE:
                    owner->ToPlayer()->TeleportTo(owner->GetMapId(), ArenaGizmoPos.GetPositionX(), ArenaGizmoPos.GetPositionY(), ArenaGizmoPos.GetPositionZ(), ArenaGizmoPos.GetOrientation(), TELE_TO_NOT_UNSUMMON_PET);
                    break;
                case HORDE:
                    owner->ToPlayer()->TeleportTo(owner->GetMapId(), RingOfValor.GetPositionX(), RingOfValor.GetPositionY(), RingOfValor.GetPositionZ(), RingOfValor.GetOrientation(), TELE_TO_NOT_UNSUMMON_PET);
                    break;
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_brawlers_guild_join_to_arena::OnAuraEffectApply, EFFECT_0, SPELL_AURA_426, AURA_EFFECT_HANDLE_REAL);
    }
};

// FireLine 133607
class spell_brawlers_guild_fire_line : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_fire_line);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (caster->GetEntry() == NPC_VIAN_THE_VOLATILE)
                caster->AI()->DoAction(ACTION_VIAN_REMOVE_FIRELINE);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_brawlers_guild_fire_line::HandleAfterCast);
    }
};

// Lumbering Charge 134527
class spell_brawlers_guild_lumbering_charge : public AuraScript
{
    PrepareAuraScript(spell_brawlers_guild_lumbering_charge);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetCaster() && GetCaster()->ToCreature())
            if (uint32 lumberingLow = GetCaster()->ToCreature()->AI()->GetData(TYPE_LUMBERING))
                if (Unit* lumbering = ObjectAccessor::GetUnit(*GetCaster(), ObjectGuid(HighGuid::Unit, NPC_LUMBERING_CHARGE_TARGET, lumberingLow)))
                    GetCaster()->GetMotionMaster()->MoveCharge(lumbering->GetPositionX(), lumbering->GetPositionY(), lumbering->GetPositionZ(), 20.0f, EVENT_CHARGE);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_brawlers_guild_lumbering_charge::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Twister Aura 132668
class spell_brawlers_guild_twister_aura : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_twister_aura);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Creature* kirrawk = caster->FindNearestCreature(NPC_KIRRAWK, 100.0f, true))
                kirrawk->AI()->DoAction(ACTION_TWISTER);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_brawlers_guild_twister_aura::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Goblin Land Mines 133219
class spell_brawlers_guild_goblin_land_mines : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_goblin_land_mines);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            x = 0.0f; y = 0.0f;
            // make symob C spawn (simply 3pi/4 to -pi/4)
            for (uint8 i = 0; i < 6; i++)
            {
                GetPositionWithDistInOrientation(caster, 2.0f, Position::NormalizeOrientation(caster->GetOrientation() + (M_PI / 4)*i), x, y);
                caster->CastSpell(x, y, caster->GetPositionZ(), SPELL_GOBLIN_LAND_MINE_MISSLE, true);
            }
        }
    }

    private:
        float x, y;

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_brawlers_guild_goblin_land_mines::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Devastating Thrust 134777
class spell_brawlers_guild_devastating_thrust : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_devastating_thrust);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_DEVASTATING_THRUST);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_brawlers_guild_devastating_thrust::HandleAfterCast);
    }
};

// Illusionist 133119, 133290
class spell_brawlers_guild_illusionist : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_illusionist);

    void SelectTargets(SpellDestination& dest)
    {
        dest.Relocate(GetCaster()->GetMap()->GetId() == 1043 ? HordeArenaGeodata() : AlianceArenaGeodata());
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_brawlers_guild_illusionist::SelectTargets, EFFECT_0, TARGET_DEST_DEST_RANDOM);
    }
};

// Rain Dance 124860
class spell_brawlers_guild_rain_dance : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_rain_dance);

    void SelectTargets(SpellDestination& dest)
    {
        dest.Relocate(GetCaster()->GetMap()->GetId() == 1043 ? HordeArenaGeodata() : AlianceArenaGeodata());
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_brawlers_guild_rain_dance::SelectTargets, EFFECT_0, TARGET_DEST_CASTER_RANDOM);
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_brawlers_guild_rain_dance::SelectTargets, EFFECT_2, TARGET_DEST_CASTER_RANDOM);
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_brawlers_guild_rain_dance::SelectTargets, EFFECT_3, TARGET_DEST_CASTER_RANDOM);
    }
};

// Fallen kin 134789
class spell_brawlers_guild_fallen_kin : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_fallen_kin);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Unit* target = GetHitUnit())
            if (target->GetAura(SPELL_FALLEN_KIN) && target->GetAura(SPELL_FALLEN_KIN)->GetStackAmount() > 4)
                target->CastSpell(target, SPELL_FEATHERED_FURY, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_brawlers_guild_fallen_kin::HandleEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Emergency Teleport 133162
class spell_brawlers_guild_emergency_teleport : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_emergency_teleport);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        PreventDefaultEffect(effIndex);

        if (Unit* caster = GetCaster())
        {
            if (Creature* sGenerator = caster->FindNearestCreature(NPC_SHIELD_GENERATOR, 100.0f, true))
            {
                caster->GetMotionMaster()->MoveJump(sGenerator->GetPositionX(), sGenerator->GetPositionY(), sGenerator->GetPositionZ(), 45.0f, 10.0f, EVENT_JUMP);
                sGenerator->CastSpell(sGenerator, SPELL_SHIELD_GENERATOR_ACTIVATION, false);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_brawlers_guild_emergency_teleport::HandleEffectHit, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
    }
};

// Energy Shielding 133154
class spell_brawlers_guild_energy_shielding : public AuraScript
{
    PrepareAuraScript(spell_brawlers_guild_energy_shielding);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->RemoveAurasDueToSpell(SPELL_SHIELD_GENERATOR_ACTIVATION);
            owner->RemoveAllAreasTrigger();
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_brawlers_guild_energy_shielding::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Goblin Rocket Barrage 133212
class spell_brawlers_guild_goblin_rocket_barrage : public AuraScript
{
    PrepareAuraScript(spell_brawlers_guild_goblin_rocket_barrage);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (ObjectGuid targetGUID = owner->GetTarget())
                if (Unit* target = ObjectAccessor::GetUnit(*owner, targetGUID))
                    owner->CastSpell(target, SPELL_GOBLIN_ROCKET_BARRAGE_MISSLE, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_brawlers_guild_goblin_rocket_barrage::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Conjure Unstable Mines 133015
class spell_brawlers_guild_conjure_unstable_mines : public AuraScript
{
    PrepareAuraScript(spell_brawlers_guild_conjure_unstable_mines);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (TempSummon* mine = owner->SummonCreature(NPC_BATTLE_MINE, *owner, TEMPSUMMON_MANUAL_DESPAWN))
            {
                GetPositionWithDistInOrientation(owner, 4.5f, owner->GetOrientation(), x, y);
                mine->GetMotionMaster()->MovePoint(0, x, y, owner->GetPositionZ() + 3.5f);
            }
        }
    }

    private:
        float x, y;

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_brawlers_guild_conjure_unstable_mines::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// We should select mines between battletron and player, it`ll work like ur spell missle throw her (nice logic)
class UnstableMinesProcPredicate 
{
    public:
        UnstableMinesProcPredicate(Unit* const m_caster, Unit* const m_target) : _caster(m_caster), _target(m_target) { }

        bool operator()(WorldObject* object)
        {
            return object && object->ToCreature() && (!object->ToCreature()->IsInBetween(_caster, _target, 1.0f));
        }

    private:
        Unit const* _caster;
        Unit const* _target;
};

// Protected by Unstable Mines 133018
class spell_brawlers_guild_protected_by_unstable_mines : public AuraScript
{
    PrepareAuraScript(spell_brawlers_guild_protected_by_unstable_mines);

    void OnProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = caster->GetVictim())
            {
                std::list<Creature*> MinesInRange;
                GetCreatureListWithEntryInGrid(MinesInRange, caster, NPC_BATTLE_MINE, 20.0f);
                MinesInRange.remove_if(UnstableMinesProcPredicate(caster, target));

                if (MinesInRange.empty())
                    return;

                if (Creature* Mine = *MinesInRange.begin())
                    Mine->AI()->JustDied(target);
            }
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_brawlers_guild_protected_by_unstable_mines::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// Ring of Valor - Teleport In 132629
class spell_brawlers_guild_ring_of_valor_teleport_in : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_ring_of_valor_teleport_in);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        PreventHitEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_brawlers_guild_ring_of_valor_teleport_in::HandleEffectHit, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
    }
};

// Flamewall 132666
class spell_brawlers_guild_flamewall : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_flamewall);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_CREATE_FLAME_WALL);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_brawlers_guild_flamewall::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Rageful Spirit 134534
class spell_brawlers_guild_rageful_spirit : public AuraScript
{
    PrepareAuraScript(spell_brawlers_guild_rageful_spirit);

    float currentMod;

    bool Load() override
    {
        currentMod = 1.5f;
        return true;
    }

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, float& amount, bool& canBeRecalculated)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (currentMod < (100.0f - owner->GetHealthPct()))
                currentMod = (100.0f - owner->GetHealthPct()) * 1.5f;
        }

        amount = currentMod;
        canBeRecalculated = true;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_brawlers_guild_rageful_spirit::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_brawlers_guild_rageful_spirit::HandleEffectCalcAmount, EFFECT_1, SPELL_AURA_MOD_HASTE);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_brawlers_guild_rageful_spirit::HandleEffectCalcAmount, EFFECT_2, SPELL_AURA_MOD_HASTE_RATING_PCT);
    }
};

// Dark Barrage 133344
class spell_brawlers_guild_dark_barrage : public AuraScript
{
    PrepareAuraScript(spell_brawlers_guild_dark_barrage);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        PreventDefaultAction();

        if (Unit* caster = GetCaster())
            if (Unit* target = GetOwner()->ToUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_brawlers_guild_dark_barrage::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Angry Enthusiast Selector 135379
class spell_brawlers_guild_angry_enthusiast_selector : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_angry_enthusiast_selector);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
            target->AI()->Talk(TALK_SPECIAL_3);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_BRAWL_ENTHUSIAST && target->GetEntry() != NPC_BRAWL_ENTHUSIAST_A; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_brawlers_guild_angry_enthusiast_selector::HandleEffectHit, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_brawlers_guild_angry_enthusiast_selector::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// Bottled 135376
class spell_brawlers_guild_bottled : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_bottled);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            if (Creature* target = GetHitCreature())
                if (target->HasAura(SPELL_ANGRY_LOSER))
                    caster->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACHIEV_BOTTLE_SERVICE);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_brawlers_guild_bottled::HandleEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Summon Ghost 133465
class spell_brawlers_guild_summon_ghost : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_summon_ghost);

    void SelectTargets(SpellDestination& dest)
    {
        dest.Relocate(GetCaster()->GetMap()->GetId() == 1043 ? HordeArenaGeodata() : AlianceArenaGeodata());
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_brawlers_guild_summon_ghost::SelectTargets, EFFECT_0, TARGET_DEST_DEST_RANDOM);
    }
};

// Better Stronger Faster 142795
class spell_brawlers_guild_better_stronger_faster : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_better_stronger_faster);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<uint32> mechaArmors = { SPELL_MECHA_ARMOR_BETA, SPELL_MECHA_ARMOR_GAMMA, SPELL_MECHA_ARMOR_DELTA, SPELL_MECHA_ARMOR_EPSILON, SPELL_MECHA_ARMOR_OMEGA };
            mechaArmors.remove_if([=](uint32 armorId) { return !caster->HasAura(armorId); });

            if (mechaArmors.empty())
            {
                caster->RemoveAurasDueToSpell(SPELL_MECHA_ARMOR_ALPHA);
                caster->CastSpell(caster, SPELL_MECHA_ARMOR_BETA, true); // default
                return;
            }

            uint32 newArmorId = *mechaArmors.begin();

            if (newArmorId == SPELL_MECHA_ARMOR_OMEGA) // no more
                return;

            caster->RemoveAurasDueToSpell(SPELL_MECHA_ARMOR_BETA);
            caster->RemoveAurasDueToSpell(SPELL_MECHA_ARMOR_GAMMA);
            caster->RemoveAurasDueToSpell(SPELL_MECHA_ARMOR_DELTA);
            caster->RemoveAurasDueToSpell(SPELL_MECHA_ARMOR_EPSILON);

            if (newArmorId < SPELL_MECHA_ARMOR_GAMMA)
                newArmorId = SPELL_MECHA_ARMOR_GAMMA;
            else
                newArmorId++;

            caster->CastSpell(caster, newArmorId, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_brawlers_guild_better_stronger_faster::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Throw Luckydo(nt) Launcher 140969
class spell_brawlers_guild_luckydo_nt_launcher : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_luckydo_nt_launcher);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, urand(0, 1) ? GetSpellInfo()->Effects[effIndex].BasePoints : GetSpellInfo()->Effects[EFFECT_1].BasePoints);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_brawlers_guild_luckydo_nt_launcher::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Overcharged 141059
class spell_brawlers_guild_overcharged : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_overcharged);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_BLINGTRON_2000 && target->GetEntry() != NPC_BLINGTRON_3000; });
    }

    void FilterDamageTarget(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_brawlers_guild_overcharged::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_brawlers_guild_overcharged::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Overcharged 141059
class spell_brawlers_guild_overcharged_aura : public AuraScript
{
    PrepareAuraScript(spell_brawlers_guild_overcharged_aura);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            if (owner->GetEntry() == NPC_BLINGTRON_2000)
                owner->Kill(owner);
            else
            {
                owner->AI()->Talk(TALK_SPECIAL);
                owner->CastSpell(owner, SPELL_MALFUNCTIONIC, true);
            }
        }
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_brawlers_guild_overcharged_aura::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Chain Lightning Visual 141335
class spell_brawlers_guild_chain_lightning_visual : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_chain_lightning_visual);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
        {
            if (target->GetEntry() == NPC_GNOMISH_TESLA_COIL) // not charging back
                return;

            target->AI()->SetData(TYPE_BLINGTRON_CHARGED, 1);
            target->CastSpell(target, SPELL_CHAIN_LIGHTNING_VISUAL, true);
        }
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_BLINGTRON_2000 && target->GetEntry() != NPC_GNOMISH_TESLA_COIL; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && target->ToCreature()->AI()->GetData(TYPE_BLINGTRON_CHARGED); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_brawlers_guild_chain_lightning_visual::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_brawlers_guild_chain_lightning_visual::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Overcharging Tesla Coil 141065
class spell_brawlers_guild_overcharging_tesla_coil : public SpellScript
{
    PrepareSpellScript(spell_brawlers_guild_overcharging_tesla_coil);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
            target->AI()->DoAction(ACTION_CHARGE_CONDUIT);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_brawlers_guild_overcharging_tesla_coil::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 609. Summoned by 134909 - Guard Area Trigger
class sat_brawlers_guild_guard_area_trigger : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && GetCaster() && GetCaster()->ToCreature() && GetCaster()->ToCreature()->GetWaypointPath() && !GetCaster()->ToCreature()->AI()->GetData(TYPE_IN_QUEUE);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            if (Creature* caster = GetCaster()->ToCreature())
            {
                caster->AI()->SetData(TYPE_IN_QUEUE, 1);
                caster->AI()->Talk(TALK_INTR0, target);
            }
        }
    }
};

// 548. Summoned by 133294 - Volatile Flame
class sat_brawlers_guild_volatile_flame : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(target, SPELL_VOLATILE_FLAME_EFF, true);
    }
};

// 497. Summoned by 132662 - Flame Wall
class sat_brawlers_guild_flame_wall_inner : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, SPELL_FIREWALL_EFFECT, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_FIREWALL_EFFECT);
    }
};

// 498, 499, 500. Summoned by 132663, 132664, 132665 - Flame Wall
class sat_brawlers_guild_flame_wall_outer : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && GetAreaTrigger() && triggering->ToPlayer()->HasInArc( (5 * M_PI) / 3, GetAreaTrigger());
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, SPELL_FIREWALL_EFFECT, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_FIREWALL_EFFECT);
    }
};

// 543. Summoned by 133260 - Charging
class sat_brawlers_guild_charging : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive();
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
        {
            if (GetCaster() && GetCaster()->ToCreature())
            {
                GetCaster()->CastSpell(itr, SPELL_COLLISION_EFF, true);
                GetCaster()->ToCreature()->AI()->SetData(TYPE_CHARGING, 0);
            }
        }
    }
};

// 526. Summoned by 133065 - Shock Field
class sat_brawlers_guild_shock_field : public IAreaTriggerAura
{
    void OnInit() override
    {
        GetAreaTrigger()->AttachToTarget();
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(target, SPELL_SHOCK_FIELD_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_SHOCK_FIELD_EFF);
    }
};

// 551. Summoned by 133363 - Electric Dynamite
class sat_brawlers_guild_electric_dynamite : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            if (itr->HasAura(SPELL_MEGAFANTASTIC_DISCOMBOBUMORPHNATOR))
            {
                itr->RemoveAurasDueToSpell(SPELL_MEGAFANTASTIC_DISCOMBOBUMORPHNATOR);
                itr->CastSpell(itr, SPELL_ELECTRIC_DYNAMITE_EFF, true);

                if (Creature* Millie = itr->FindNearestCreature(NPC_MILLIE_WATT, 100.0f, true))
                    Millie->AI()->Talk(TALK_SPECIAL);
            }
            else
                itr->CastSpell(itr, SPELL_ELECTRIC_DYNAMITE_EFF, true);
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_ELECTRIC_DYNAMITE_EFF);
    }
};

// 546. Summoned by 133292 - Flames of Fallen Glory
class sat_brawlers_guild_flames_of_fallen_glory : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, SPELL_FLAMES_OF_FALLEN_GLORY_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_FLAMES_OF_FALLEN_GLORY_EFF);
    }
};

// 542. Summoned by 133137 - Activate Shield Generator
class sat_brawlers_guild_activate_shield_generator : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToUnit() && triggering->ToUnit()->IsAlive() && triggering->GetEntry() != NPC_SHIELD_GENERATOR;
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* target = triggering->ToUnit())
            target->CastSpell(target, SPELL_SHIELDED, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Unit* target = triggering->ToUnit())
            target->RemoveAurasDueToSpell(SPELL_SHIELDED);
    }
};

// 608. Summoned by 134883 - Strange Feeling
class sat_brawlers_guild_strange_feeling : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive();
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
            itr->CastSpell(itr, SPELL_STRANGE_FEELING_EFF, true);
    }
};

// 552. Summoned by 133426 - Guardians Light
class sat_brawlers_guild_guardians_light : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToCreature() && triggering->ToCreature()->GetEntry() == NPC_TORMENTED_GHOST && GetCaster() && GetCaster()->HasInArc(M_PI / 12, triggering->ToCreature());
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Creature* target = triggering->ToCreature())
            if (Unit* caster = GetCaster())
                caster->CastSpell(target, SPELL_SPOTTED, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Creature* target = triggering->ToCreature())
            target->RemoveAurasDueToSpell(SPELL_SPOTTED);
    }
};

// 921. Summoned by 140971, 140975 - Luckydo, Luckydon`t
class sat_brawlers_guild_luckydo : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive();
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                caster->CastSpell(itr, SPELL_GOOD_LUCKYDO, true);

                if (caster->ToCreature())
                    caster->ToCreature()->DespawnOrUnsummon(1 * IN_MILLISECONDS);
            }
        }
    }
};

// 922. Summoned by 140975 - Luckydon`t
class sat_brawlers_guild_luckydont : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive();
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                caster->CastSpell(itr, SPELL_LUCKYDOOM, true);

                if (caster->ToCreature())
                    caster->ToCreature()->DespawnOrUnsummon(1 * IN_MILLISECONDS);
            }
        }
    }
};

class cond_brawlers_guild_far_sight_totem : public ConditionScript
{
    public:
        cond_brawlers_guild_far_sight_totem() : ConditionScript("cond_brawlers_guild_far_sight_totem") { }

        bool OnConditionCheck(const Condition* cond, ConditionSourceInfo& source) override
        {
            if (source.mConditionTargets[0])
                if (Player* target = source.mConditionTargets[0]->ToPlayer())
                    return target->HasAura(SPELL_RING_OF_VALOR_IN_THE_ARENA);

            return false;
        }
};

void AddSC_brawlers_guild()
{
    new creature_script<npc_brawlers_guild_bizmo>("npc_brawlers_guild_bizmo");
    new npc_brawlers_guild_brawlgar_arena_grunt();
    new creature_script<npc_brawlers_guild_boss_bazzelflange>("npc_brawlers_guild_boss_bazzelflange");

    new creature_script<npc_brawlers_guild_bruce>("npc_brawlers_guild_bruce");
    new creature_script<npc_brawlers_guild_vian_the_volatile>("npc_brawlers_guild_vian_the_volatile");
    new creature_script<npc_brawlers_guild_volatile_flame>("npc_brawlers_guild_volatile_flame");
    new creature_script<npc_brawlers_guild_fire_line>("npc_brawlers_guild_fire_line");
    new creature_script<npc_brawlers_guild_goredome>("npc_brawlers_guild_goredome");
    new creature_script<npc_brawlers_guild_dungeon_master_vishas>("npc_brawlers_guild_dungeon_master_vishas");
    new creature_script<npc_brawlers_guild_dippy>("npc_brawlers_guild_dippy");
    new creature_script<npc_brawlers_guild_kirrawk>("npc_brawlers_guild_kirrawk");
    new creature_script<npc_brawlers_guild_twister>("npc_brawlers_guild_twister");
    new creature_script<npc_brawlers_guild_fran>("npc_brawlers_guild_fran");
    new creature_script<npc_brawlers_guild_riddoh>("npc_brawlers_guild_riddoh");
    new creature_script<npc_brawlers_guild_goblin_land_mine>("npc_brawlers_guild_goblin_land_mine");
    new creature_script<npc_brawlers_guild_king_kulaka>("npc_brawlers_guild_king_kulaka");
    new creature_script<npc_brawlers_guild_blat>("npc_brawlers_guild_blat");
    new creature_script<npc_brawlers_guild_sanoriak>("npc_brawlers_guild_sanoriak");
    new creature_script<npc_brawlers_guild_ixx>("npc_brawlers_guild_ixx");
    new creature_script<npc_brawlers_guild_mazhareen>("npc_brawlers_guild_mazhareen");
    new creature_script<npc_brawlers_guild_crush>("npc_brawlers_guild_crush");
    new creature_script<npc_brawlers_guild_leona_earthwind>("npc_brawlers_guild_leona_earthwind");
    new creature_script<npc_brawlers_guild_dominika_the_illusionist>("npc_brawlers_guild_dominika_the_illusionist");
    new creature_script<npc_brawlers_guild_dominika_illusion>("npc_brawlers_guild_dominika_illusion");
    new creature_script<npc_brawlers_guild_deeken>("npc_brawlers_guild_deeken");
    new creature_script<npc_brawlers_shock_field>("npc_brawlers_shock_field");
    new creature_script<npc_brawlers_guild_millie_watt>("npc_brawlers_guild_millie_watt");
    new creature_script<npc_brawlers_guild_fjoll>("npc_brawlers_guild_fjoll");
    new creature_script<npc_brawlers_guild_proboskus>("npc_brawlers_guild_proboskus");
    new creature_script<npc_brawlers_guild_lepregnomes>("npc_brawlers_guild_lepregnomes");
    new creature_script<npc_brawlers_guild_yikkan_izu>("npc_brawlers_guild_yikkan_izu");
    new creature_script<npc_brawlers_guild_izus_raven>("npc_brawlers_guild_izus_raven");
    new creature_script<npc_brawlers_guild_akama>("npc_brawlers_guild_akama");
    new creature_script<npc_brawlers_guild_smash_hoofstomp>("npc_brawlers_guild_smash_hoofstomp");
    new creature_script<npc_brawlers_guild_good_luck_goblin>("npc_brawlers_guild_good_luck_goblin");
    new creature_script<npc_brawlers_guild_unguloxx>("npc_brawlers_guild_unguloxx");
    new creature_script<npc_brawlers_guild_gg_ingeenering>("npc_brawlers_guild_gg_ingeenering");
    new creature_script<npc_brawlers_guild_dark_summoner>("npc_brawlers_guild_dark_summoner");
    new creature_script<npc_brawlers_guild_tormented_ghost>("npc_brawlers_guild_tormented_ghost");
    new creature_script<npc_brawlers_guild_battletron>("npc_brawlers_guild_battletron");
    new creature_script<npc_brawlers_guild_unstable_mine>("npc_brawlers_guild_unstable_mine");
    new creature_script<npc_brawlers_guild_meatball>("npc_brawlers_guild_meatball");
    new creature_script<npc_brawlers_guild_mecha_bruce>("npc_brawlers_guild_mecha_bruce");
    new creature_script<npc_brawlers_guild_grandpa_grumplefloot>("npc_brawlers_guild_grandpa_grumplefloot");
    new creature_script<npc_brawlers_guild_blingtron_3000>("npc_brawlers_guild_blingtron_3000");
    new creature_script<npc_brawlers_guild_gnomish_tesla_coil>("npc_brawlers_guild_gnomish_tesla_coil");
    new creature_script<npc_brawlers_guild_blingtron_2000>("npc_brawlers_guild_blingtron_2000");
    new npc_brawlers_guild_card_traider();

    new aura_script<spell_brawlers_guild_join_to_arena>("spell_brawlers_guild_join_to_arena");
    new spell_script<spell_brawlers_guild_fire_line>("spell_brawlers_guild_fire_line");
    new aura_script<spell_brawlers_guild_lumbering_charge>("spell_brawlers_guild_lumbering_charge");
    new spell_script<spell_brawlers_guild_twister_aura>("spell_brawlers_guild_twister_aura");
    new spell_script<spell_brawlers_guild_goblin_land_mines>("spell_brawlers_guild_goblin_land_mines");
    new spell_script<spell_brawlers_guild_devastating_thrust>("spell_brawlers_guild_devastating_thrust");
    new spell_script<spell_brawlers_guild_illusionist>("spell_brawlers_guild_illusionist");
    new spell_script<spell_brawlers_guild_rain_dance>("spell_brawlers_guild_rain_dance");
    new spell_script<spell_brawlers_guild_fallen_kin>("spell_brawlers_guild_fallen_kin");
    new spell_script<spell_brawlers_guild_emergency_teleport>("spell_brawlers_guild_emergency_teleport");
    new aura_script<spell_brawlers_guild_energy_shielding>("spell_brawlers_guild_energy_shielding");
    new aura_script<spell_brawlers_guild_goblin_rocket_barrage>("spell_brawlers_guild_goblin_rocket_barrage");
    new aura_script<spell_brawlers_guild_conjure_unstable_mines>("spell_brawlers_guild_conjure_unstable_mines");
    new aura_script<spell_brawlers_guild_protected_by_unstable_mines>("spell_brawlers_guild_protected_by_unstable_mines");
    new spell_script<spell_brawlers_guild_ring_of_valor_teleport_in>("spell_brawlers_guild_ring_of_valor_teleport_in");
    new spell_script<spell_brawlers_guild_flamewall>("spell_brawlers_guild_flamewall");
    new aura_script<spell_brawlers_guild_rageful_spirit>("spell_brawlers_guild_rageful_spirit");
    new aura_script<spell_brawlers_guild_dark_barrage>("spell_brawlers_guild_dark_barrage");
    new spell_script<spell_brawlers_guild_angry_enthusiast_selector>("spell_brawlers_guild_angry_enthusiast_selector");
    new spell_script<spell_brawlers_guild_bottled>("spell_brawlers_guild_bottled");
    new spell_script<spell_brawlers_guild_summon_ghost>("spell_brawlers_guild_summon_ghost");
    new spell_script<spell_brawlers_guild_better_stronger_faster>("spell_brawlers_guild_better_stronger_faster");
    new spell_script<spell_brawlers_guild_luckydo_nt_launcher>("spell_brawlers_guild_luckydo_nt_launcher");
    new spell_script<spell_brawlers_guild_overcharged>("spell_brawlers_guild_overcharged");
    new aura_script<spell_brawlers_guild_overcharged_aura>("spell_brawlers_guild_overcharged_aura");
    new spell_script<spell_brawlers_guild_chain_lightning_visual>("spell_brawlers_guild_chain_lightning_visual");
    new spell_script<spell_brawlers_guild_overcharging_tesla_coil>("spell_brawlers_guild_overcharging_tesla_coil");
    new atrigger_script<sat_brawlers_guild_guard_area_trigger>("sat_brawlers_guild_guard_area_trigger");
    new atrigger_script<sat_brawlers_guild_volatile_flame>("sat_brawlers_guild_volatile_flame");
    new atrigger_script<sat_brawlers_guild_flame_wall_inner>("sat_brawlers_guild_flame_wall_inner");
    new atrigger_script<sat_brawlers_guild_flame_wall_outer>("sat_brawlers_guild_flame_wall_outer");
    new atrigger_script<sat_brawlers_guild_charging>("sat_brawlers_guild_charging");
    new atrigger_script<sat_brawlers_guild_shock_field>("sat_brawlers_guild_shock_field");
    new atrigger_script<sat_brawlers_guild_electric_dynamite>("sat_brawlers_guild_electric_dynamite");
    new atrigger_script<sat_brawlers_guild_flames_of_fallen_glory>("sat_brawlers_guild_flames_of_fallen_glory");
    new atrigger_script<sat_brawlers_guild_activate_shield_generator>("sat_brawlers_guild_activate_shield_generator");
    new atrigger_script<sat_brawlers_guild_strange_feeling>("sat_brawlers_guild_strange_feeling");
    new atrigger_script<sat_brawlers_guild_guardians_light>("sat_brawlers_guild_guardians_light");
    new atrigger_script<sat_brawlers_guild_luckydo>("sat_brawlers_guild_luckydo");
    new atrigger_script<sat_brawlers_guild_luckydont>("sat_brawlers_guild_luckydont");
    new cond_brawlers_guild_far_sight_totem();
}