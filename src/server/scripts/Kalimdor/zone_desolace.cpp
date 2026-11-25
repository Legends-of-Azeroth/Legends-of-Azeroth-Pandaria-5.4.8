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
SDName: Desolace
SD%Complete: 100
SDComment: Quest support: 5561, 5581
SDCategory: Desolace
EndScriptData */

/* ContentData
npc_aged_dying_ancient_kodo
go_demon_portal
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "Player.h"
#include "SpellInfo.h"
#include "GameObjectAI.h"

enum DyingKodo
{
    SAY_SMEED_HOME                  = 0,

    QUEST_KODO                      = 5561,

    NPC_SMEED                       = 11596,
    NPC_AGED_KODO                   = 4700,
    NPC_DYING_KODO                  = 4701,
    NPC_ANCIENT_KODO                = 4702,
    NPC_TAMED_KODO                  = 11627,

    SPELL_KODO_KOMBO_ITEM           = 18153,
    SPELL_KODO_KOMBO_PLAYER_BUFF    = 18172,
    SPELL_KODO_KOMBO_DESPAWN_BUFF   = 18377,
    SPELL_KODO_KOMBO_GOSSIP         = 18362

};

class npc_aged_dying_ancient_kodo : public CreatureScript
{
public:
    npc_aged_dying_ancient_kodo() : CreatureScript("npc_aged_dying_ancient_kodo") { }

    

    struct npc_aged_dying_ancient_kodoAI : public ScriptedAI
    {
        npc_aged_dying_ancient_kodoAI(Creature* creature) : ScriptedAI(creature) { }

        void MoveInLineOfSight(Unit* who) override
        {
            if (who->GetEntry() == NPC_SMEED && me->IsWithinDistInMap(who, 10.0f) && !me->HasAura(SPELL_KODO_KOMBO_GOSSIP))
            {
                me->GetMotionMaster()->Clear();
                DoCast(me, SPELL_KODO_KOMBO_GOSSIP, true);
                if (Creature* smeed = who->ToCreature())
                    smeed->AI()->Talk(SAY_SMEED_HOME);
            }
        }

        void SpellHit(WorldObject* caster, SpellInfo const* spell) override
        {
                Unit* unitCaster = caster ? caster->ToUnit() : nullptr;
                if (!unitCaster)
                    return;

            if (spell->Id == SPELL_KODO_KOMBO_ITEM)
            {
                if (!(unitCaster->HasAura(SPELL_KODO_KOMBO_PLAYER_BUFF) || me->HasAura(SPELL_KODO_KOMBO_DESPAWN_BUFF))
                    && (me->GetEntry() == NPC_AGED_KODO || me->GetEntry() == NPC_DYING_KODO || me->GetEntry() == NPC_ANCIENT_KODO))
                {
                    unitCaster->CastSpell(unitCaster, SPELL_KODO_KOMBO_PLAYER_BUFF, true);
                    DoCast(me, SPELL_KODO_KOMBO_DESPAWN_BUFF, true);

                    me->UpdateEntry(NPC_TAMED_KODO);
                    me->GetMotionMaster()->MoveFollow(unitCaster, PET_FOLLOW_DIST, me->GetFollowAngle());
                }
            }
            else if (spell->Id == SPELL_KODO_KOMBO_GOSSIP)
            {
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                me->DespawnOrUnsummon(60000);
            }
        }
                bool OnGossipHello(Player* player) override
            {

            if (player->HasAura(SPELL_KODO_KOMBO_PLAYER_BUFF) && me->HasAura(SPELL_KODO_KOMBO_DESPAWN_BUFF))
            {
                player->TalkedToCreature(me->GetEntry(), ObjectGuid::Empty);
                player->RemoveAurasDueToSpell(SPELL_KODO_KOMBO_PLAYER_BUFF);
            }

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(me), me->GetGUID());
            return true;
    
            }
        };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_aged_dying_ancient_kodoAI(creature);
    }
};

/*######
## go_demon_portal
######*/

enum DemonPortal
{
    NPC_DEMON_GUARDIAN           = 11937,

    QUEST_PORTAL_OF_THE_LEGION_H = 5581,
    QUEST_PORTAL_OF_THE_LEGION_A = 14374,

    EVENT_UPDATE_STATE           = 1,
};

class npc_demon_guardian : public CreatureScript
{
    public:
        npc_demon_guardian() : CreatureScript("npc_demon_guardian") { }

        struct npc_demon_guardianAI : public ScriptedAI
        {
            npc_demon_guardianAI(Creature* creature) : ScriptedAI(creature) { }

            void JustDied(Unit* /*killer*/) override
            {
                if (GameObject* portal = ObjectAccessor::GetGameObject(*me, me->GetOwnerGUID()))
                {
                    portal->SetRespawnTime(portal->GetGOData()->spawntimesecs);
                    portal->UpdateObjectVisibility();
                    portal->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_demon_guardianAI(creature);
        }
};

class go_demon_portal : public GameObjectScript
{
    public:
        go_demon_portal() : GameObjectScript("go_demon_portal") { }

        
        struct go_demon_portalAI : public GameObjectAI
        {
            go_demon_portalAI(GameObject* go) : GameObjectAI(go) { }

            bool OnGossipHello(Player* player, bool isUse) override
            {

                if (player->GetQuestStatus(QUEST_PORTAL_OF_THE_LEGION_H) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_PORTAL_OF_THE_LEGION_A) == QUEST_STATUS_INCOMPLETE)
                {
                    ObjectGuid goGuid = me->GetGUID();
                    if (Creature* guardian = me->SummonCreature(NPC_DEMON_GUARDIAN, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0ms))
                    {
                        guardian->AI()->AttackStart(player);
                        guardian->SetOwnerGUID(goGuid);

                        me->SetGoState(GO_STATE_ACTIVE);
                        me->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

                        uint32 delay = 0;
                        guardian->m_Events.Schedule(delay += 5000, [guardian, goGuid]()
                        {
                            if (GameObject* portal = ObjectAccessor::GetGameObject(*guardian, goGuid))
                                portal->SetGoState(GO_STATE_READY);
                        });
                    }
                }

                return true;
        
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new go_demon_portalAI(go);
        }
};

void AddSC_desolace()
{
    new npc_aged_dying_ancient_kodo();
    new npc_demon_guardian();
    new go_demon_portal();
}
