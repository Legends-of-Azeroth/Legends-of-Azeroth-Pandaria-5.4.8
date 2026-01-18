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
SDName: Eversong_Woods
SD%Complete: 100
SDComment: Quest support: 8488, 8490
SDCategory: Eversong Woods
EndScriptData */

/* ContentData
npc_apprentice_mirveda
npc_infused_crystal
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"

enum UnexpectedResults
{
    // Quest
    QUEST_UNEXPECTED_RESULT         = 8488,

    // Creatures
    NPC_GHARZUL                     = 15958,
    NPC_ANGERSHADE                  = 15656
};

class npc_apprentice_mirveda : public CreatureScript
{
public:
    npc_apprentice_mirveda() : CreatureScript("npc_apprentice_mirveda") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_UNEXPECTED_RESULT)
        {
            CAST_AI(npc_apprentice_mirveda::npc_apprentice_mirvedaAI, creature->AI())->Summon = true;
            CAST_AI(npc_apprentice_mirveda::npc_apprentice_mirvedaAI, creature->AI())->PlayerGUID = player->GetGUID();
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_apprentice_mirvedaAI(creature);
    }

    struct npc_apprentice_mirvedaAI : public ScriptedAI
    {
        npc_apprentice_mirvedaAI(Creature* creature) : ScriptedAI(creature), Summons(me) { }

        uint32 KillCount;
        ObjectGuid PlayerGUID;
        bool Summon;
        SummonList Summons;

        void Reset() override
        {
            KillCount = 0;
            PlayerGUID = ObjectGuid::Empty;
            Summons.DespawnAll();
            Summon = false;
        }

        void JustEngagedWith(Unit* /*who*/) override { }

        void JustSummoned(Creature* summoned) override
        {
            summoned->AI()->AttackStart(me);
            Summons.Summon(summoned);
        }

        void SummonedCreatureDespawn(Creature* summoned) override
        {
            Summons.Despawn(summoned);
            ++KillCount;
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (PlayerGUID)
                if (Player* player = ObjectAccessor::FindPlayer(PlayerGUID))
                    player->FailQuest(QUEST_UNEXPECTED_RESULT);
        }

        void UpdateAI(uint32 /*diff*/) override
        {
            if (KillCount >= 3 && PlayerGUID)
                if (Player* player = ObjectAccessor::FindPlayer(PlayerGUID))
                    if (player->GetQuestStatus(QUEST_UNEXPECTED_RESULT) == QUEST_STATUS_INCOMPLETE)
                    {
                        player->CompleteQuest(QUEST_UNEXPECTED_RESULT);
                        Reset();
                    }

            if (Summon)
            {
                me->SummonCreature(NPC_GHARZUL,    8749.505f, -7132.595f, 35.31983f, 3.816502f, TEMPSUMMON_CORPSE_DESPAWN, 4000);
                me->SummonCreature(NPC_ANGERSHADE, 8755.38f, -7131.521f, 35.30957f, 3.816502f,  TEMPSUMMON_CORPSE_DESPAWN, 4000);
                me->SummonCreature(NPC_ANGERSHADE, 8753.199f, -7125.975f, 35.31986f, 3.816502f, TEMPSUMMON_CORPSE_DESPAWN, 4000);
                Summon = false;
            }
        }
    };
};

/*######
## npc_infused_crystal
######*/

enum InfusedCrystal
{
    // Quest
    QUEST_POWERING_OUR_DEFENSES     = 8490,

    // Says
    EMOTE                           = 0,

    // Creatures
    NPC_ENRAGED_WRAITH              = 17086
};

struct Location
{
    float x, y, z;
};

static Location SpawnLocations[] =
{
    {8270.68f, -7188.53f, 139.619f},
    {8284.27f, -7187.78f, 139.603f},
    {8297.43f, -7193.53f, 139.603f},
    {8303.5f, -7201.96f, 139.577f},
    {8273.22f, -7241.82f, 139.382f},
    {8254.89f, -7222.12f, 139.603f},
    {8278.51f, -7242.13f, 139.162f},
    {8267.97f, -7239.17f, 139.517f}
};

class npc_infused_crystal : public CreatureScript
{
public:
    npc_infused_crystal() : CreatureScript("npc_infused_crystal") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_infused_crystalAI(creature);
    }

    struct npc_infused_crystalAI : public ScriptedAI
    {
        npc_infused_crystalAI(Creature* creature) : ScriptedAI(creature)
        {
            SetCombatMovement(false);
        }

        uint32 EndTimer;
        uint32 WaveTimer;
        bool Completed;
        bool Progress;
        ObjectGuid PlayerGUID;

        void Reset() override
        {
            EndTimer = 0;
            Completed = false;
            Progress = false;
            PlayerGUID = ObjectGuid::Empty;
            WaveTimer = 0;
        }

        void MoveInLineOfSight(Unit* who) override

        {
            if (!Progress && who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 10.0f))
            {
                if (who->ToPlayer()->GetQuestStatus(QUEST_POWERING_OUR_DEFENSES) == QUEST_STATUS_INCOMPLETE)
                {
                    PlayerGUID = who->GetGUID();
                    WaveTimer = 1000;
                    EndTimer = 60000;
                    Progress = true;
                }
            }
        }

        void JustSummoned(Creature* summoned) override
        {
            summoned->AI()->AttackStart(me);
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (PlayerGUID && !Completed)
                if (Player* player = ObjectAccessor::GetPlayer(*me, PlayerGUID))
                    player->FailQuest(QUEST_POWERING_OUR_DEFENSES);
        }

        void UpdateAI(uint32 diff) override
        {
            if (EndTimer < diff && Progress)
            {
                Talk(EMOTE);
                Completed = true;
                if (PlayerGUID)
                    if (Player* player = ObjectAccessor::GetPlayer(*me, PlayerGUID))
                        player->CompleteQuest(QUEST_POWERING_OUR_DEFENSES);

                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->RemoveCorpse();
            } else EndTimer -= diff;

            if (WaveTimer < diff && !Completed && Progress)
            {
                uint32 ran1 = rand()%8;
                uint32 ran2 = rand()%8;
                uint32 ran3 = rand()%8;
                me->SummonCreature(NPC_ENRAGED_WRAITH, SpawnLocations[ran1].x, SpawnLocations[ran1].y, SpawnLocations[ran1].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000);
                me->SummonCreature(NPC_ENRAGED_WRAITH, SpawnLocations[ran2].x, SpawnLocations[ran2].y, SpawnLocations[ran2].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000);
                me->SummonCreature(NPC_ENRAGED_WRAITH, SpawnLocations[ran3].x, SpawnLocations[ran3].y, SpawnLocations[ran3].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000);
                WaveTimer = 30000;
            } else WaveTimer -= diff;
        }
    };
};

// Mass Charm 33384
class spell_lorthemar_mass_charm : public SpellScript
{
    PrepareSpellScript(spell_lorthemar_mass_charm);

    void CountTargets(std::list<WorldObject*>& targets)
    {
        uint32 targetsCount = targets.size() % 2 == 0 ? targets.size() / 2 : (targets.size() - 1) / 2;

        if (targets.size() > targetsCount)
            Trinity::Containers::RandomResizeList(targets, targetsCount);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lorthemar_mass_charm::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

void AddSC_eversong_woods()
{
    new npc_apprentice_mirveda();
    new npc_infused_crystal();
    new spell_script<spell_lorthemar_mass_charm>("spell_lorthemar_mass_charm");
}
