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
#include "forge_of_souls.h"
#include "../AI/SmartScripts/SmartAI.h"

/*
 * TODO:
 * - Fix model id during unleash soul -> seems DB issue 36503 is missing (likewise 36504 is also missing).
 * - Fix outro npc movement
 */

enum Yells
{
    SAY_FACE_ANGER_AGGRO                          = 0,
    SAY_FACE_DESIRE_AGGRO                         = 1,  // unused on retail?
    SAY_FACE_ANGER_SLAY                           = 2,
    SAY_FACE_DESIRE_SLAY                          = 3,
    SAY_FACE_SORROW_SLAY                          = 4,
    SAY_FACE_ANGER_DEATH                          = 5,
    SAY_FACE_DESIRE_DEATH                         = 6,
    SAY_FACE_SORROW_DEATH                         = 7,
    SAY_FACE_SORROW_UNLEASH_SOUL                  = 8,
    SAY_FACE_ANGER_UNLEASH_SOUL                   = 9,  // unused on retail?
    SAY_FACE_DESIRE_UNLEASH_SOUL                  = 10, // unused on retail?
    SAY_FACE_DESIRE_WAILING_SOUL                  = 11,
    SAY_FACE_ANGER_WAILING_SOUL                   = 12, // unused on retail?
    EMOTE_WAILING_SOUL                            = 13,
    EMOTE_UNLEASH_SOUL                            = 14,
    EMOTE_MIRRORED_SOUL                           = 15,

    SAY_JAINA_SYLVANAS_OUTRO                      = 0,
};

enum Spells
{
    SPELL_PHANTOM_BLAST                           = 68982,
    SPELL_MIRRORED_SOUL                           = 69051,
    SPELL_WELL_OF_SOULS                           = 68820,
    SPELL_UNLEASHED_SOULS                         = 68939,
    SPELL_WAILING_SOULS_STARTING                  = 68912, // Initial spell cast at begining of wailing souls phase
    SPELL_WAILING_SOULS_BEAM                      = 68875, // the beam visual
    SPELL_WAILING_SOULS                           = 68873, // the actual spell
// 68871, 68873, 68875, 68876, 68899, 68912,
// 68899 trigger 68871
};

enum Events
{
    EVENT_NONE                                    = 1,
    EVENT_PHANTOM_BLAST                           = 2,
    EVENT_MIRRORED_SOUL                           = 3,
    EVENT_WELL_OF_SOULS                           = 4,
    EVENT_UNLEASHED_SOULS                         = 5,
    EVENT_WAILING_SOULS                           = 6,
    EVENT_WAILING_SOULS_TICK                      = 7,
    EVENT_FACE_ANGER                              = 8,
};

enum eEnum
{
    DISPLAY_ANGER                                 = 30148,
    DISPLAY_SORROW                                = 30149,
    DISPLAY_DESIRE                                = 30150,
};

enum Misc
{
    WORLDSTATE_THREE_FACED = 7812,
};

struct OutroSpawn
{
    uint32 Entry;
    uint32 WaypointsID;
    Position Pos;
} outroSpawnsAlliance[] =
{
    { NPC_CHAMPION_1_ALLIANCE,  NPC_CHAMPION_1_ALLIANCE * 100 + 0,  { 5600.836f, 2421.359f, 705.9351f, 1.5816120f } },
    { NPC_CHAMPION_1_ALLIANCE,  NPC_CHAMPION_1_ALLIANCE * 100 + 1,  { 5599.373f, 2424.422f, 705.9143f, 1.0713430f } },
    { NPC_CHAMPION_1_ALLIANCE,  NPC_CHAMPION_1_ALLIANCE * 100 + 2,  { 5593.632f, 2428.580f, 705.9351f, 0.3386126f } },
    { NPC_CHAMPION_1_ALLIANCE,  NPC_CHAMPION_1_ALLIANCE * 100 + 3,  { 5598.036f, 2429.377f, 705.9351f, 0.6393744f } },
    { NPC_CHAMPION_1_ALLIANCE,  NPC_CHAMPION_1_ALLIANCE * 100 + 4,  { 5590.505f, 2428.026f, 705.9352f, 0.6883672f } },
    { NPC_CHAMPION_1_ALLIANCE,  NPC_CHAMPION_1_ALLIANCE * 100 + 5,  { 5601.465f, 2426.771f, 705.9351f, 1.1315430f } },
    { NPC_CHAMPION_1_ALLIANCE,  NPC_CHAMPION_1_ALLIANCE * 100 + 6,  { 5596.331f, 2427.121f, 705.9133f, 0.7287790f } },
    { NPC_CHAMPION_1_ALLIANCE,  NPC_CHAMPION_1_ALLIANCE * 100 + 7,  { 5602.589f, 2431.936f, 705.8700f, 0.9412316f } },
    { NPC_CHAMPION_1_ALLIANCE,  NPC_CHAMPION_1_ALLIANCE * 100 + 8,  { 5601.587f, 2418.604f, 705.9351f, 1.5085570f } },
    { NPC_CHAMPION_2_ALLIANCE,  NPC_CHAMPION_2_ALLIANCE * 100 + 0,  { 5588.519f, 2419.411f, 705.8804f, 0.9566764f } },
    { NPC_CHAMPION_2_ALLIANCE,  NPC_CHAMPION_2_ALLIANCE * 100 + 1,  { 5593.937f, 2410.875f, 705.9351f, 1.3139220f } },
    { NPC_CHAMPION_2_ALLIANCE,  NPC_CHAMPION_2_ALLIANCE * 100 + 2,  { 5593.449f, 2415.090f, 705.8518f, 0.8844323f } },
    { NPC_CHAMPION_2_ALLIANCE,  NPC_CHAMPION_2_ALLIANCE * 100 + 3,  { 5585.499f, 2418.229f, 705.9351f, 1.0502980f } },
    { NPC_CHAMPION_3_ALLIANCE,  NPC_CHAMPION_3_ALLIANCE * 100 + 0,  { 5589.813f, 2421.271f, 705.9351f, 0.3307303f } },
    { NPC_CHAMPION_3_ALLIANCE,  NPC_CHAMPION_3_ALLIANCE * 100 + 1,  { 5594.647f, 2417.108f, 705.9351f, 1.3424520f } },
    { NPC_CHAMPION_3_ALLIANCE,  NPC_CHAMPION_3_ALLIANCE * 100 + 2,  { 5594.336f, 2421.619f, 705.9206f, 0.8251035f } },
    { NPC_KORELN,               NPC_KORELN * 100,                   { 5608.396f, 2435.061f, 705.9227f, 0.8172771f } },
    { NPC_ELANDRA,              NPC_ELANDRA * 100,                  { 5604.396f, 2438.163f, 705.9137f, 1.0028980f } },
    { NPC_JAINA_PART2,          NPC_JAINA_PART2 * 100,              { 5607.934f, 2438.370f, 705.9181f, 0.9097379f } },
    { 0,                        0,                                  { 0000.000f, 0000.000f, 000.0000f, 0.0000000f } },
    { 0,                        0,                                  { 0000.000f, 0000.000f, 000.0000f, 0.0000000f } },
},
outroSpawnsHorde[] =
{
    { NPC_KALIRA,               NPC_KALIRA * 100,                   { 5606.255f, 2433.374f, 705.9338f, 1.0411090f } },
    { NPC_CHAMPION_1_HORDE,     NPC_CHAMPION_1_HORDE * 100 + 0,     { 5597.997f, 2429.137f, 705.9351f, 0.6364982f } },
    { NPC_CHAMPION_1_HORDE,     NPC_CHAMPION_1_HORDE * 100 + 1,     { 5601.548f, 2418.365f, 705.9351f, 1.3472840f } },
    { NPC_CHAMPION_1_HORDE,     NPC_CHAMPION_1_HORDE * 100 + 2,     { 5594.192f, 2425.089f, 705.9331f, 0.9594442f } },
    { NPC_CHAMPION_1_HORDE,     NPC_CHAMPION_1_HORDE * 100 + 3,     { 5593.593f, 2428.340f, 705.9351f, 0.6210129f } },
    { NPC_CHAMPION_1_HORDE,     NPC_CHAMPION_1_HORDE * 100 + 4,     { 5590.466f, 2427.786f, 705.9352f, 0.9131427f } },
    { NPC_CHAMPION_1_HORDE,     NPC_CHAMPION_1_HORDE * 100 + 5,     { 5600.965f, 2429.518f, 705.9297f, 0.9370359f } },
    { NPC_CHAMPION_1_HORDE,     NPC_CHAMPION_1_HORDE * 100 + 6,     { 5598.078f, 2421.761f, 705.9333f, 0.8700571f } },
    { NPC_CHAMPION_1_HORDE,     NPC_CHAMPION_1_HORDE * 100 + 7,     { 5601.426f, 2426.531f, 705.9351f, 1.1281410f } },
    { NPC_CHAMPION_1_HORDE,     NPC_CHAMPION_1_HORDE * 100 + 8,     { 5600.797f, 2421.120f, 705.9351f, 1.3751750f } },
    { NPC_CHAMPION_2_HORDE,     NPC_CHAMPION_2_HORDE * 100 + 0,     { 5589.773f, 2421.031f, 705.9351f, 0.4020257f } },
    { NPC_CHAMPION_2_HORDE,     NPC_CHAMPION_2_HORDE * 100 + 1,     { 5592.401f, 2419.364f, 705.9337f, 0.9138597f } },
    { NPC_CHAMPION_2_HORDE,     NPC_CHAMPION_2_HORDE * 100 + 2,     { 5594.608f, 2416.868f, 705.9351f, 1.4242600f } },
    { NPC_CHAMPION_3_HORDE,     NPC_CHAMPION_3_HORDE * 100 + 0,     { 5605.997f, 2428.649f, 705.9272f, 0.8756463f } },
    { NPC_CHAMPION_3_HORDE,     NPC_CHAMPION_3_HORDE * 100 + 1,     { 5586.914f, 2416.940f, 705.9304f, 0.9317115f } },
    { NPC_CHAMPION_3_HORDE,     NPC_CHAMPION_3_HORDE * 100 + 2,     { 5585.460f, 2417.990f, 705.9351f, 1.0245420f } },
    { NPC_CHAMPION_3_HORDE,     NPC_CHAMPION_3_HORDE * 100 + 3,     { 5593.897f, 2410.635f, 705.9351f, 1.2934920f } },
    { NPC_CHAMPION_3_HORDE,     NPC_CHAMPION_3_HORDE * 100 + 4,     { 5591.780f, 2412.860f, 705.9274f, 0.8832204f } },
    { NPC_CHAMPION_3_HORDE,     NPC_CHAMPION_3_HORDE * 100 + 5,     { 5598.737f, 2434.853f, 705.9277f, 0.9360511f } },
    { NPC_LORALEN,              NPC_LORALEN * 100,                  { 5602.723f, 2436.158f, 705.9332f, 0.9804344f } },
    { NPC_SYLVANAS_PART2,       NPC_SYLVANAS_PART2 * 100,           { 5606.318f, 2436.855f, 705.9329f, 0.9050557f } },
};

Position const CrucibleSummonPos = {5672.294f,2520.686f, 713.4386f, 0.9599311f};

class boss_devourer_of_souls : public CreatureScript
{
    public:
        boss_devourer_of_souls() : CreatureScript("boss_devourer_of_souls") { }

        struct boss_devourer_of_soulsAI : public BossAI
        {
            boss_devourer_of_soulsAI(Creature* creature) : BossAI(creature, DATA_DEVOURER_EVENT)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            // wailing soul event
            float beamAngle;
            float beamAngleDiff;
            int8 wailingSoulTick;

            ObjectGuid uiMirroredSoulTarget;

            void Reset() override
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->SetDisplayId(DISPLAY_ANGER);
                me->SetReactState(REACT_AGGRESSIVE);

                events.Reset();
                summons.DespawnAll();

                uiMirroredSoulTarget = ObjectGuid::Empty;
                wasCasting = false;

                if (instance)
                    instance->SetData(DATA_DEVOURER_EVENT, NOT_STARTED);

                me->GetMap()->SetWorldState(WORLDSTATE_THREE_FACED, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance)
                    instance->SetData(DATA_DEVOURER_EVENT, IN_PROGRESS);

                Talk(RAND(SAY_FACE_ANGER_AGGRO,SAY_FACE_DESIRE_AGGRO));

                if (!me->FindNearestCreature(NPC_CRUCIBLE_OF_SOULS, 60)) // Prevent double spawn
                    instance->instance->SummonCreature(NPC_CRUCIBLE_OF_SOULS, CrucibleSummonPos);

                events.ScheduleEvent(EVENT_PHANTOM_BLAST, 5000);
                events.ScheduleEvent(EVENT_MIRRORED_SOUL, 8000);
                events.ScheduleEvent(EVENT_WELL_OF_SOULS, 30000);
                events.ScheduleEvent(EVENT_UNLEASHED_SOULS, 20000);
                events.ScheduleEvent(EVENT_WAILING_SOULS, urand(60000, 70000));
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (uiMirroredSoulTarget && me->HasAura(SPELL_MIRRORED_SOUL))
                {
                    if (Player *player = Unit::GetPlayer(*me, uiMirroredSoulTarget))
                    {
                        if (player->GetAura(SPELL_MIRRORED_SOUL))
                        {
                            int32 mirrorDamage = (damage * 45)/100;
                            me->CastCustomSpell(player, 69034, &mirrorDamage, 0, 0, true);
                            //me->DealDamage(player, (damage * 45)/100, 0, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW);
                        }
                        else
                            uiMirroredSoulTarget = ObjectGuid::Empty;
                    }
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(RAND(SAY_FACE_ANGER_SLAY, SAY_FACE_SORROW_SLAY, SAY_FACE_DESIRE_SLAY));
            }

            void JustDied(Unit* /*killer*/) override
            {
                summons.DespawnAll();

                Talk(RAND(SAY_FACE_ANGER_DEATH, SAY_FACE_SORROW_DEATH, SAY_FACE_DESIRE_DEATH));

                if (instance)
                {
                    TeamId team = TeamId(instance->GetData(DATA_TEAM_IN_INSTANCE));

                    instance->SetData(DATA_DEVOURER_EVENT, DONE);

                    for (auto&& spawn : (team == TEAM_ALLIANCE ? outroSpawnsAlliance : outroSpawnsHorde))
                    {
                        if (!spawn.Entry)
                            continue;

                        if (Creature* summon = me->SummonCreature(spawn.Entry, spawn.Pos, TEMPSUMMON_DEAD_DESPAWN))
                        {
                            // Stopping it here, otherwise waypoint movement will be interrupted in SAI OOC update
                            if (summon->GetAIName() == "SmartAI")
                                CAST_AI(SmartAI, summon->AI())->SetCombatMove(false);

                            if (spawn.WaypointsID)
                                summon->GetMotionMaster()->MovePath(spawn.WaypointsID, false);

                            if (spawn.Entry == NPC_JAINA_PART2 || spawn.Entry == NPC_SYLVANAS_PART2)
                                summon->AI()->Talk(SAY_JAINA_SYLVANAS_OUTRO);

                            if (spawn.Entry == NPC_KALIRA)
                            {
                                summon->HandleEmoteStateCommand(0);
                                summon->SetSheath(SHEATH_STATE_RANGED);
                            }
                        }
                    }
                }
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* /*spell*/) override
            {
                if (wasCasting && !me->HasUnitState(UNIT_STATE_CASTING))
                {
                    wasCasting = false;
                    me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                }
            }

            void SpellHitTarget(Unit* /*target*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_PHANTOM_BLAST/* && (target->GetTypeId() == TYPEID_PLAYER)*/)
                {
                    me->GetMap()->SetWorldState(WORLDSTATE_THREE_FACED, 0);
                    wasCasting = false;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PHANTOM_BLAST:
                            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
                            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, false);
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0))
                                DoCast(target, SPELL_PHANTOM_BLAST);
                            wasCasting = true;
                            events.ScheduleEvent(EVENT_PHANTOM_BLAST, 5000);
                            break;
                        case EVENT_MIRRORED_SOUL:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            {
                                uiMirroredSoulTarget = target->GetGUID();
                                DoCast(target, SPELL_MIRRORED_SOUL);
                                Talk(EMOTE_MIRRORED_SOUL);
                            }
                            events.ScheduleEvent(EVENT_MIRRORED_SOUL, urand(15000, 30000));
                            break;
                        case EVENT_WELL_OF_SOULS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_WELL_OF_SOULS);
                            events.ScheduleEvent(EVENT_WELL_OF_SOULS, 20000);
                            break;
                        case EVENT_UNLEASHED_SOULS:
                            // Interrupt any spell casting
                            me->InterruptNonMeleeSpells(false);
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_UNLEASHED_SOULS);
                            me->SetDisplayId(DISPLAY_SORROW);
                            Talk(RAND(SAY_FACE_ANGER_UNLEASH_SOUL, SAY_FACE_SORROW_UNLEASH_SOUL, SAY_FACE_DESIRE_UNLEASH_SOUL));
                            Talk(EMOTE_UNLEASH_SOUL);
                            events.ScheduleEvent(EVENT_UNLEASHED_SOULS, 30000);
                            events.ScheduleEvent(EVENT_FACE_ANGER, 5000);
                            break;
                        case EVENT_FACE_ANGER:
                            me->SetDisplayId(DISPLAY_ANGER);
                            break;

                        case EVENT_WAILING_SOULS:
                            me->SetDisplayId(DISPLAY_DESIRE);
                            Talk(RAND(SAY_FACE_ANGER_WAILING_SOUL, SAY_FACE_DESIRE_WAILING_SOUL));
                            Talk(EMOTE_WAILING_SOUL);
                            DoCast(me, SPELL_WAILING_SOULS_STARTING);

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            {
                                me->SetFacingToObject(target);
                                DoCast(me, SPELL_WAILING_SOULS_BEAM);
                            }

                            beamAngle = me->GetOrientation();

                            beamAngleDiff = M_PI / 30.0f; // PI / 2 in 15 sec = PI / 30 per tick
                            if (RAND(true,false))
                                beamAngleDiff = -beamAngleDiff;

                            me->InterruptNonMeleeSpells(false);
                            me->SetReactState(REACT_PASSIVE);

                            // Remove any target
                            me->SetTarget(ObjectGuid::Empty);

                            me->GetMotionMaster()->Clear();
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                            wailingSoulTick = 15;
                            events.DelayEvents(18000); // no other events during wailing souls
                            events.ScheduleEvent(EVENT_WAILING_SOULS_TICK, 3000); // first one after 3 secs.
                            break;

                        case EVENT_WAILING_SOULS_TICK:
                            beamAngle += beamAngleDiff;
                            me->SetFacingTo(beamAngle);

                            DoCast(me, SPELL_WAILING_SOULS);

                            if (--wailingSoulTick)
                                events.ScheduleEvent(EVENT_WAILING_SOULS_TICK, 1000);
                            else
                            {
                                me->SetReactState(REACT_AGGRESSIVE);
                                me->SetDisplayId(DISPLAY_ANGER);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                                me->GetMotionMaster()->MoveChase(me->GetVictim());
                                events.ScheduleEvent(EVENT_WAILING_SOULS, urand(60000, 70000));
                            }
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff);
            }

        private:
           bool wasCasting;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_devourer_of_soulsAI(creature);
        }
};

class npc_unleashed_soul : public CreatureScript
{
    public:
        npc_unleashed_soul() : CreatureScript("npc_unleashed_soul") { }

        struct npc_unleashed_soulAI : public ScriptedAI
        {
            npc_unleashed_soulAI(Creature* creature) : ScriptedAI(creature) 
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            //uint32 uiShadowBoltTimer;

            void Reset() override
            {
                //uiShadowBoltTimer = 6 * IN_MILLISECONDS;
                if (instance)
                {
                    if (Creature* Devourer = Unit::GetCreature(*me, instance->GetGuidData(DATA_DEVOURER_EVENT)))
                        if (Devourer->IsAlive())
                            AttackStart(Devourer->GetVictim());
                }
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!UpdateVictim())
                    return;

                //if (uiShadowBoltTimer <= diff)
                //{
                //    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                //         DoCast(target, SPELL_SHADOW_BOLT, true);
                //    uiShadowBoltTimer = 15 * IN_MILLISECONDS;
                //} else uiShadowBoltTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_unleashed_soulAI(creature);
        }
};

void AddSC_boss_devourer_of_souls()
{
    new boss_devourer_of_souls;
    new npc_unleashed_soul;
}
