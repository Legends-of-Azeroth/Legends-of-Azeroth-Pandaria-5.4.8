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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "Object.h"
#include "Player.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "Unit.h"
#include "UnitAI.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Spell.h"
#include "timeless_isle.h"
#include "CreatureTextMgr.h"
#include "Vehicle.h"
#include "Language.h"
#include "Guild.h"
#include "Random.h"

enum TimeLostShrine
{
    // Spells
    SPELL_FORTITUDE_OF_NIUZAO           = 147281,
    SPELL_WISDOM_OF_YULON               = 147282,
    SPELL_CHI_JIS_HOPE                  = 147283,
    SPELL_XUENS_STRENGTH                = 147284,
    SPELL_ORDOS_BURNING_SACRIFICE       = 147285,
    SPELL_TIME_LOST_WISDOM              = 147280,
    SPELL_STORM_BLOSSOM_EFF             = 147829,
    SPELL_FIRE_BLOSSOM_EFF              = 147822,
};

uint32 const items[5][2] =
{
    { SPELL_FORTITUDE_OF_NIUZAO,     LANG_SHRINE_BLESSING_OF_NIUZAO },
    { SPELL_WISDOM_OF_YULON,         LANG_SHRINE_BLESSING_OF_YULON  },
    { SPELL_CHI_JIS_HOPE,            LANG_SHRINE_BLESSING_OF_CHI_JI },
    { SPELL_XUENS_STRENGTH,          LANG_SHRINE_BLESSING_OF_XUEN   },
    { SPELL_ORDOS_BURNING_SACRIFICE, LANG_SHRINE_CORRUPTED_BY_ORDOS },
};

class go_time_lost_shrine_ti : public GameObjectScript
{
    public:
        go_time_lost_shrine_ti() : GameObjectScript("go_time_lost_shrine_ti") { }

        void OnLootStateChanged(GameObject* go, uint32 state, Unit* unit) override
        {
            if (!unit || state != GO_ACTIVATED || unit->GetTypeId() != TYPEID_PLAYER)
                return;

            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);

            for (auto&& aura : items)
            {
                if (unit->HasAura(aura[0]))
                    unit->RemoveAurasDueToSpell(aura[0]);
            }

            uint8 choice = urand(1, 5) - 1;
            unit->CastSpell(unit, SPELL_TIME_LOST_WISDOM);
            unit->CastSpell(unit, items[choice][0]);
            unit->ToPlayer()->SendPersonalMessage(items[choice][1], CHAT_MSG_RAID_BOSS_WHISPER, LANG_UNIVERSAL);

            go->m_Events.Schedule(300000, [go]()
            {
                go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
            });
        }
};

class go_gleaming_crane_statue_ti : public GameObjectScript
{
    public:
        go_gleaming_crane_statue_ti() : GameObjectScript("go_gleaming_crane_statue_ti") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (!player->IsWeeklyQuestDone(QUEST_MIST_COVERED_TREASURE_CHEST))
                player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(go), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, GameObject* /*go*/, uint32 /*sender*/, uint32 /*action*/) override
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();

            player->CastSpell(player, 144387, true); // knockback in the air
            player->m_Events.Schedule(6000, 1, [player]()
            {
                player->CastSpell(player, 144385, true);
            });

            return true;
        }
};

enum EmperorMiscActions
{
    ACTION_SELECT_CELESTIAL,
    EVENT_REGROUP,
};

const std::map<uint32, uint32> invCelesialType =
{
    { BOSS_YU_LON, EMPEROR_TALK_INTRO_YULON  },
    { BOSS_NIUZAO, EMPEROR_TALK_INTRO_NIUZAO },
    { BOSS_CHI_JI, EMPEROR_TALK_INTRO_CHIJI  },
    { BOSS_XUEN,   EMPEROR_TALK_INTRO_XUEN   },
};

// Emperor Shaohao - 73303
class npc_emperor_shaohao : public CreatureScript
{
    public:
        npc_emperor_shaohao() : CreatureScript("npc_emperor_shaohao") { }

        struct npc_emperor_shaohaoAI : public ScriptedAI
        {
            npc_emperor_shaohaoAI(Creature* creature) : ScriptedAI(creature) { }

            bool hasTriggered;
            bool firstWaveDone;
            std::list<uint32> celestialSequence;
            EventMap celesialEvents;
            uint32 prevCelesial;

            void Reset() override
            {
                me->setActive(true);
                celesialEvents.Reset();
                firstWaveDone = false;
                prevCelesial = 0;

                celestialSequence = { BOSS_YU_LON, BOSS_NIUZAO, BOSS_CHI_JI, BOSS_XUEN };

                // Random at wave end
                std::vector<uint32> buf(celestialSequence.begin(), celestialSequence.end());
                std::random_device rd;
                std::mt19937 g(rd());
                std::shuffle(buf.begin(), buf.end(), g);
                celestialSequence.clear();
                std::copy(buf.begin(), buf.end(), std::inserter(celestialSequence, celestialSequence.begin()));

                celesialEvents.ScheduleEvent(EVENT_REGROUP, 5.5 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_SELECT_CELESTIAL)
                {
                    if (celestialSequence.empty())
                    {
                        celestialSequence = { BOSS_YU_LON, BOSS_NIUZAO, BOSS_CHI_JI, BOSS_XUEN };

                        // Random at wave end
                        std::vector<uint32> buf(celestialSequence.begin(), celestialSequence.end());
                        std::random_device rd;
                        std::mt19937 g(rd());
                        std::shuffle(buf.begin(), buf.end(), g);
                        celestialSequence.clear();
                        std::copy(buf.begin(), buf.end(), std::inserter(celestialSequence, celestialSequence.begin()));

                        firstWaveDone = true;
                    }

                    if (firstWaveDone)
                    {
                        if (prevCelesial)
                            celestialSequence.erase(std::find(celestialSequence.begin(), celestialSequence.end(), prevCelesial));

                        if (celestialSequence.empty())
                        {
                            celesialEvents.ScheduleEvent(EVENT_REGROUP, 0.5 * IN_MILLISECONDS);
                            return;
                        }

                        prevCelesial = Trinity::Containers::SelectRandomContainerElement(celestialSequence);

                        // Remove before init next if it`s last element for form list
                        if (celestialSequence.size() == 1)
                            celestialSequence.clear();
                    }
                    else // In Begun case
                    {
                        if (celestialSequence.empty())
                        {
                            celesialEvents.ScheduleEvent(EVENT_REGROUP, 0.5 * IN_MILLISECONDS);
                            return;
                        }

                        prevCelesial = celestialSequence.front();
                        celestialSequence.pop_front();
                    }

                    if (Creature* celesial = me->FindNearestCreature(prevCelesial, 500.0f, true))
                    {
                        celesial->GetMotionMaster()->MovePoint(1, _timelessIsleMiddle);
                        me->AI()->Talk(invCelesialType.find(celesial->GetEntry())->second);
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                celesialEvents.Update(diff);

                while (uint32 eventId = celesialEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_REGROUP)
                        DoAction(ACTION_SELECT_CELESTIAL);
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_emperor_shaohaoAI(creature);
        }
};

enum TurtlesEvents
{
    EVENT_GEYSER        = 1,
    EVENT_SHELL_SPINE   = 2,
    EVENT_SNAPPING_BITE = 3,
};

// Chelon - 72045
// Great Turtle Furyshell - 73161
// Elder Great Turtle - 72765
// Great Turtle - 72764
// Great Turtle Hatchling - 72763
struct npc_timeless_turtles : public ScriptedAI
{
    npc_timeless_turtles(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void EnterEvadeMode() override
    {
        if (me->GetEntry() == RARE_CHELON)
        {
            if (GameObject* go = me->FindNearestGameObject(221027, 150.0f))
                go->Respawn();

            me->DespawnOrUnsummon();
        }
        else
            ScriptedAI::EnterEvadeMode();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_GEYSER,        8  * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SHELL_SPINE,   10 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SNAPPING_BITE, 6  * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_GEYSER:
            {
                if (me->GetEntry() == RARE_CHELON || me->GetEntry() == RARE_GREAT_TURTLE_FURYSHELL)
                    me->CastSpell(me->GetVictim(), SPELL_GEYSER_1);
                else if (me->GetEntry() == 72765)
                    me->CastSpell(me->GetVictim(), SPELL_GEYSER_2);
                events.ScheduleEvent(EVENT_GEYSER, 8 * IN_MILLISECONDS);
                break;
            }
            case EVENT_SHELL_SPINE:
            {
                if (me->GetEntry() == RARE_CHELON || me->GetEntry() == RARE_GREAT_TURTLE_FURYSHELL || me->GetEntry() == 72764)
                    me->CastSpell(me->GetVictim(), SPELL_SHELL_SPINE_1);
                else if (me->GetEntry() == 72765)
                    me->CastSpell(me->GetVictim(), SPELL_SHELL_SPINE_2);
                events.ScheduleEvent(EVENT_SHELL_SPINE, 10 * IN_MILLISECONDS);
                break;
            }
            case EVENT_SNAPPING_BITE:
            {
                if (me->GetEntry() == RARE_CHELON || me->GetEntry() == 73161)
                    me->CastSpell(me->GetVictim(), SPELL_SNAPPING_BITE_1);
                else if (me->GetEntry() == 72765)
                    me->CastSpell(me->GetVictim(), SPELL_SNAPPING_BITE_2);
                else if (me->GetEntry() == 72764 || me->GetEntry() == 72763)
                    me->CastSpell(me->GetVictim(), SPELL_SNAPPING_BITE_3);
                events.ScheduleEvent(EVENT_SNAPPING_BITE, 6 * IN_MILLISECONDS);
                break;
            }
            default:
                break;
        }

        DoMeleeAttackIfReady();
    }
};

// Tushui Sentry - 73361
// Huojin Sentry - 73362
// Michi Windblossom - 71940
// Chi-Ro the Skytamer - 71939
// Celestial Guard 73343
// Captain Huan - 73347
class npc_timeless_faction_sentries : public CreatureScript
{
    public:
        npc_timeless_faction_sentries() : CreatureScript("npc_timeless_faction_sentries") { }

        struct npc_timeless_faction_sentriesAI : public ScriptedAI
        {
            npc_timeless_faction_sentriesAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiIncapacitatingShoutTimer;
            uint32 uiMortalStrikeTimer;
            uint32 uiNetTimer;
            uint32 uiSnapKickTimer;

            void Reset() override
            {
                uiIncapacitatingShoutTimer = 9*IN_MILLISECONDS;
                uiMortalStrikeTimer = 4*IN_MILLISECONDS;
                uiNetTimer = 1*IN_MILLISECONDS;
                uiSnapKickTimer = 7*IN_MILLISECONDS;

                if (me->GetEntry() != 71939 && me->GetEntry() != 71940 && me->GetEntry() != 73386 && me->GetEntry() != 73385)
                    me->SetAIAnimKitId(3786);
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (me->GetEntry())
                {
                    case 73361:
                    {
                        if (uiIncapacitatingShoutTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_INCAPACITATING_SHOUT);
                            uiIncapacitatingShoutTimer = 25*IN_MILLISECONDS;
                        }
                        else uiIncapacitatingShoutTimer -= diff;

                        if (uiMortalStrikeTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_MORTAL_STRIKE);
                            uiMortalStrikeTimer = 8*IN_MILLISECONDS;
                        }
                        else uiMortalStrikeTimer -= diff;

                        if (uiNetTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_NET);
                            uiNetTimer = 15*IN_MILLISECONDS;
                        }
                        else uiNetTimer -= diff;

                        if (uiSnapKickTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_SNAP_KICK);
                            uiSnapKickTimer = 20*IN_MILLISECONDS;
                        }
                        else uiSnapKickTimer -= diff;
                        break;
                    }
                    case 73362:
                    {
                        if (uiIncapacitatingShoutTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_INCAPACITATING_SHOUT);
                            uiIncapacitatingShoutTimer = 25*IN_MILLISECONDS;
                        }
                        else uiIncapacitatingShoutTimer -= diff;

                        if (uiMortalStrikeTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_MORTAL_STRIKE);
                            uiMortalStrikeTimer = 8*IN_MILLISECONDS;
                        }
                        else uiMortalStrikeTimer -= diff;

                        if (uiNetTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_NET);
                            uiNetTimer = 15*IN_MILLISECONDS;
                        }
                        else uiNetTimer -= diff;

                        if (uiSnapKickTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_SNAP_KICK);
                            uiSnapKickTimer = 20*IN_MILLISECONDS;
                        }
                        else uiSnapKickTimer -= diff;
                        break;
                    }
                    case 71940:
                    {
                        if (uiMortalStrikeTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_MORTAL_STRIKE);
                            uiMortalStrikeTimer = 8*IN_MILLISECONDS;
                        }
                        else uiMortalStrikeTimer -= diff;

                        if (uiNetTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_NET);
                            uiNetTimer = 15*IN_MILLISECONDS;
                        }
                        else uiNetTimer -= diff;

                        if (uiSnapKickTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_SNAP_KICK);
                            uiSnapKickTimer = 20*IN_MILLISECONDS;
                        }
                        else uiSnapKickTimer -= diff;
                        break;
                    }
                    case 71939:
                    {
                        if (uiMortalStrikeTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_MORTAL_STRIKE);
                            uiMortalStrikeTimer = 8*IN_MILLISECONDS;
                        }
                        else uiMortalStrikeTimer -= diff;

                        if (uiNetTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_NET);
                            uiNetTimer = 15*IN_MILLISECONDS;
                        }
                        else uiNetTimer -= diff;

                        if (uiSnapKickTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_SNAP_KICK);
                            uiSnapKickTimer = 20*IN_MILLISECONDS;
                        }
                        else uiSnapKickTimer -= diff;
                        break;
                    }
                    case 73343:
                    {
                        if (uiIncapacitatingShoutTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_INCAPACITATING_SHOUT);
                            uiIncapacitatingShoutTimer = 25*IN_MILLISECONDS;
                        }
                        else uiIncapacitatingShoutTimer -= diff;

                        if (uiMortalStrikeTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_MORTAL_STRIKE);
                            uiMortalStrikeTimer = 8*IN_MILLISECONDS;
                        }
                        else uiMortalStrikeTimer -= diff;

                        if (uiNetTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_NET);
                            uiNetTimer = 15*IN_MILLISECONDS;
                        }
                        else uiNetTimer -= diff;

                        if (uiSnapKickTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_SNAP_KICK);
                            uiSnapKickTimer = 20*IN_MILLISECONDS;
                        }
                        else uiSnapKickTimer -= diff;
                        break;
                    }
                    case 73347:
                    {
                        if (uiIncapacitatingShoutTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_INCAPACITATING_SHOUT);
                            uiIncapacitatingShoutTimer = 25*IN_MILLISECONDS;
                        }
                        else uiIncapacitatingShoutTimer -= diff;

                        if (uiMortalStrikeTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_MORTAL_STRIKE);
                            uiMortalStrikeTimer = 8*IN_MILLISECONDS;
                        }
                        else uiMortalStrikeTimer -= diff;

                        if (uiNetTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_NET);
                            uiNetTimer = 15*IN_MILLISECONDS;
                        }
                        else uiNetTimer -= diff;

                        if (uiSnapKickTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_SNAP_KICK);
                            uiSnapKickTimer = 20*IN_MILLISECONDS;
                        }
                        else uiSnapKickTimer -= diff;
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_timeless_faction_sentriesAI(creature);
        }
};

// Imperial Python - 73163
// Death Adder - 72841
class npc_timeless_adders : public CreatureScript
{
    public:
        npc_timeless_adders() : CreatureScript("npc_timeless_adders") { }

        struct npc_timeless_addersAI : public ScriptedAI
        {
            npc_timeless_addersAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiHugeFangTimer;

            void Reset() override
            {
                uiHugeFangTimer = 1*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (me->GetEntry())
                {
                    case 73163:
                    {
                        if (uiHugeFangTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_HUGE_FANG_1);
                            uiHugeFangTimer = 1*IN_MILLISECONDS;
                        }
                        else uiHugeFangTimer -= diff;

                        break;
                    }

                    case 72841:
                    {
                        if (uiHugeFangTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_HUGE_FANG_2);
                            uiHugeFangTimer = 1*IN_MILLISECONDS;
                        }
                        else uiHugeFangTimer -= diff;

                        break;
                    }

                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_timeless_addersAI(creature);
        }
};

// Ironfur Steelhorn - 73160
// Ironfur Herdling - 72842
// Ironfur Grazer - 72843
// Ironfur Great Bull - 72844
class npc_timeless_yaks : public CreatureScript
{
    public:
        npc_timeless_yaks() : CreatureScript("npc_timeless_yaks") { }

        struct npc_timeless_yaksAI : public ScriptedAI
        {
            npc_timeless_yaksAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiHeadbuttTimer;
            uint32 uiIronFurTimer;
            uint32 uiOxChargeTimer;
            uint32 uiOxChargeTriggerTimer;
            bool oxCharge;

            void Reset() override
            {
                oxCharge = false;
                uiHeadbuttTimer = 8*IN_MILLISECONDS;
                uiIronFurTimer = 5*IN_MILLISECONDS;
                uiOxChargeTimer = 10*IN_MILLISECONDS;
                uiOxChargeTriggerTimer = 11*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                
                switch (me->GetEntry())
                {
                    case 73160:
                    {
                        if (!oxCharge)
                        {
                            if (uiHeadbuttTimer <= diff)
                            {
                                me->CastSpell(me->GetVictim(), SPELL_HEADBUTT_1);
                                uiHeadbuttTimer = 8*IN_MILLISECONDS;
                            }
                            else uiHeadbuttTimer -= diff;

                            if (uiIronFurTimer <= diff)
                            {
                                me->CastSpell(me, SPELL_IRON_FUR);
                                uiIronFurTimer = 25*IN_MILLISECONDS;
                            }
                            else uiIronFurTimer -= diff;
                        }

                        if (uiOxChargeTimer <= diff)
                        {
                            oxCharge = true;
                            me->CastSpell(SelectTarget(SELECT_TARGET_RANDOM), SPELL_OX_CHARGE);
                            uiOxChargeTimer = 10*IN_MILLISECONDS;
                        }
                        else uiOxChargeTimer -= diff;

                        if (uiOxChargeTriggerTimer <= diff)
                        {
                            me->CastSpell(me, SPELL_OX_CHARGE_TRIGGER);
                            oxCharge = false;
                            uiOxChargeTriggerTimer = 10*IN_MILLISECONDS;
                        }
                        else uiOxChargeTriggerTimer -= diff;

                        break;
                    }

                    case 72842:
                    {
                        if (uiHeadbuttTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_HEADBUTT_2);
                            uiHeadbuttTimer = 8*IN_MILLISECONDS;
                        }
                        else uiHeadbuttTimer -= diff;

                        break;
                    }

                    case 72843:
                    {
                        if (uiHeadbuttTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_HEADBUTT_3);
                            uiHeadbuttTimer = 8*IN_MILLISECONDS;
                        }
                        else uiHeadbuttTimer -= diff;

                        if (uiIronFurTimer <= diff)
                        {
                            me->CastSpell(me, SPELL_IRON_FUR);
                            uiIronFurTimer = 16*IN_MILLISECONDS;
                        }
                        else uiIronFurTimer -= diff;

                        break;
                    }

                    case 72844:
                    {
                        if (!oxCharge)
                        {
                            if (uiIronFurTimer <= diff)
                            {
                                me->CastSpell(me, SPELL_IRON_FUR);
                                uiIronFurTimer = 25*IN_MILLISECONDS;
                            }
                            else uiIronFurTimer -= diff;
                        }

                        if (uiOxChargeTimer <= diff)
                        {
                            oxCharge = true;
                            me->CastSpell(SelectTarget(SELECT_TARGET_RANDOM), SPELL_OX_CHARGE);
                            uiOxChargeTimer = 10*IN_MILLISECONDS;
                        }
                        else uiOxChargeTimer -= diff;

                        if (uiOxChargeTriggerTimer <= diff)
                        {
                            me->CastSpell(me, SPELL_OX_CHARGE_TRIGGER);
                            oxCharge = false;
                            uiOxChargeTriggerTimer = 10*IN_MILLISECONDS;
                        }
                        else uiOxChargeTriggerTimer -= diff;
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_timeless_yaksAI(creature);
        }
};

enum craneEvents
{
    EVENT_GUST_OF_WIND = 1,
    EVENT_HEALING_SONG,
    EVENT_WIND_FEATHER,
};

// Emerald Gander - 73158
// Brilliant Windfeather - 72762
// Windfeather Chick - 71143
// Fishgorged Crane - 72095
// Windfeather Nestkeeper - 72761
class npc_timeless_cranes : public CreatureScript
{
    public:
        npc_timeless_cranes() : CreatureScript("npc_timeless_cranes") { }

        struct npc_timeless_cranesAI : public customCreatureAI
        {
            npc_timeless_cranesAI(Creature* creature) : customCreatureAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override 
            {
                events.ScheduleEvent(EVENT_GUST_OF_WIND, 8 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_HEALING_SONG, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_WIND_FEATHER, 1 * IN_MILLISECONDS);
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
                    switch (me->GetEntry())
                    {
                        case 73158:
                        {
                            ExecuteTargetEvent(SPELL_GUST_OF_WIND, 8 * IN_MILLISECONDS, EVENT_GUST_OF_WIND, eventId, PRIORITY_CHANNELED);
                            ExecuteTargetEvent(SPELL_HEALING_SONG, 18 * IN_MILLISECONDS, EVENT_HEALING_SONG, eventId, PRIORITY_SELF);
                            ExecuteTargetEvent(SPELL_WINDFEATHER, 15 * IN_MILLISECONDS, EVENT_WIND_FEATHER, eventId, PRIORITY_SELF);
                            break;
                        }
                        case 72762:
                        {
                            ExecuteTargetEvent(SPELL_GUST_OF_WIND, 8 * IN_MILLISECONDS, EVENT_GUST_OF_WIND, eventId, PRIORITY_CHANNELED);
                            ExecuteTargetEvent(SPELL_WINDFEATHER, 13 * IN_MILLISECONDS, EVENT_WIND_FEATHER, eventId, PRIORITY_SELF);
                            break;
                        }
                        case 71143:
                        case 72095:
                        {
                            ExecuteTargetEvent(SPELL_WINDFEATHER, 10 * IN_MILLISECONDS, EVENT_WIND_FEATHER, eventId, PRIORITY_SELF);
                            break;
                        }
                        case 72761:
                        {
                            ExecuteTargetEvent(SPELL_HEALING_SONG, 10 * IN_MILLISECONDS, EVENT_HEALING_SONG, eventId, PRIORITY_SELF);
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_timeless_cranesAI(creature);
        }
};

// Gu'chi the Swarmbringer - 72909
// Spotted Swarmer - 72908
class npc_timeless_silkworms : public CreatureScript
{
    public:
        npc_timeless_silkworms() : CreatureScript("npc_timeless_silkworms") { }

        struct npc_timeless_silkwormsAI : public ScriptedAI
        {
            npc_timeless_silkwormsAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiVenomSpitTimer;
            uint32 uiPoisonSpitTimer;
            std::vector<ObjectGuid> swarmers;

            void Reset() override
            {
                uiVenomSpitTimer = 7*IN_MILLISECONDS;
                uiPoisonSpitTimer = urand(7, 12)*IN_MILLISECONDS;

                if (me->GetEntry() == 72909)
                {
                    for (auto&& guid : swarmers)
                        if (Creature* swarmer = ObjectAccessor::GetCreature(*me, guid))
                            swarmer->DespawnOrUnsummon();
                    swarmers.clear();

                    for (uint32 i = 1; i <= 20; i++)
                    {
                        Position sumPos = me->GetPosition();
                        sumPos.RelocateOffset(i * (M_PI * 2 / 20), 10.0f);
                        if (Creature* swarmer = me->SummonCreature(72908, sumPos))
                        {
                            swarmer->GetMotionMaster()->MoveFollow(me, 10.0f, i * (M_PI * 2 / 20));
                            swarmers.push_back(swarmer->GetGUID());
                        }
                    }
                }
            }

            void JustEngagedWith(Unit* who) override
            {
                if (me->GetEntry() == 72909)
                {
                    for (auto&& guid : swarmers)
                        if (Creature* swarmer = ObjectAccessor::GetCreature(*me, guid))
                            swarmer->AI()->AttackStart(who);
                }
            }

            void JustDied(Unit* killer) override
            {
                if (me->GetEntry() == 72909)
                {
                    for (auto&& guid : swarmers)
                        if (Creature* swarmer = ObjectAccessor::GetCreature(*me, guid))
                            swarmer->DespawnOrUnsummon();
                    swarmers.clear();
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (me->IsSummon() && me->GetEntry() == 72908)
                    me->SetHomePosition(me->GetPosition());

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (me->GetEntry())
                {
                    case 72909:
                    {
                        if (uiVenomSpitTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_VENOM_SPIT);
                            uiVenomSpitTimer = 10*IN_MILLISECONDS;
                        }
                        else uiVenomSpitTimer -= diff;
                        break;
                    }
                    case 72908:
                    {
                        if (uiPoisonSpitTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_POISON_SPIT);
                            uiPoisonSpitTimer = urand(7, 12)*IN_MILLISECONDS;
                        }
                        else uiPoisonSpitTimer -= diff;

                        break;
                    }

                    default:
                        break;
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_timeless_silkwormsAI(creature);
        }
};

// Ancient Spineclaw - 72766
// Monstrous Spineclaw - 73166
class npc_timeless_crabs : public CreatureScript
{
    public:
        npc_timeless_crabs() : CreatureScript("npc_timeless_crabs") { }

        struct npc_timeless_crabsAI : public ScriptedAI
        {
            npc_timeless_crabsAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiClawFlurry;

            void Reset() override
            {
                uiClawFlurry = 5*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (me->GetEntry())
                {
                    case 72766:
                    {
                        if (uiClawFlurry <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_CLAW_FLURRY_1);
                            uiClawFlurry = 8*IN_MILLISECONDS;
                        }
                        else uiClawFlurry -= diff;

                        break;
                    }
                    case 73166:
                    {
                        if (uiClawFlurry <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_CLAW_FLURRY_2);
                            uiClawFlurry = 8*IN_MILLISECONDS;
                        }
                        else uiClawFlurry -= diff;

                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_timeless_crabsAI(creature);
        }
};

enum TimelessSpectralsEnum
{
    EVENT_CHI_BURST           = 1,
    EVENT_HEALING_MISTS       = 2,
    EVENT_SPINNING_CRANE_KICK = 3,
};

// Spectral Windwalker - 73021
// Spectral Mistweaver - 73025
// Spectral Brewmaster - 73018
class npc_timeless_spectrals : public CreatureScript
{
    public:
        npc_timeless_spectrals() : CreatureScript("npc_timeless_spectrals") { }

        struct npc_timeless_spectralsAI : public ScriptedAI
        {
            npc_timeless_spectralsAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiSpecSpellTimer;
            bool alementalNear;
            EventMap events;

            void Reset() override
            {
                events.Reset();

                me->CastSpell(me, SPELL_GHOST_VISUAL);

                alementalNear = false;

                if (auto addon = me->GetCreatureAddon())
                    if (addon->emote != 0)
                        me->HandleEmoteStateCommand(addon->emote);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->HandleEmoteStateCommand(0);

                switch (me->GetEntry())
                {
                    case 73021:
                        events.ScheduleEvent(EVENT_CHI_BURST, 6 * IN_MILLISECONDS);
                        break;
                    case 73025:
                        events.ScheduleEvent(EVENT_HEALING_MISTS, 6 * IN_MILLISECONDS);
                        break;
                    case 73018:
                        events.ScheduleEvent(EVENT_SPINNING_CRANE_KICK, 6 * IN_MILLISECONDS);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!alementalNear && me->FindNearestCreature(NPC_SKUNKY_ALEMENTAL, 5.0f))
                {
                    me->CastSpell(me, SPELL_DRUNKEN_STUPOR);
                    alementalNear = true;
                }

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                switch (me->GetEntry())
                {
                    case 73021:
                        me->CastSpell(me->GetVictim(), SPELL_CHI_BURST);
                        events.ScheduleEvent(EVENT_CHI_BURST, urand(6, 8) * IN_MILLISECONDS);
                        break;
                    case 73025:
                        me->CastSpell(me, SPELL_HEALING_MISTS);
                        events.ScheduleEvent(EVENT_HEALING_MISTS, urand(13, 15) * IN_MILLISECONDS);
                        break;
                    case 73018:
                        me->CastSpell(me->GetVictim(), SPELL_SPINNING_CRANE_KICK);
                        events.ScheduleEvent(EVENT_SPINNING_CRANE_KICK, urand(9, 12) * IN_MILLISECONDS);
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_timeless_spectralsAI(creature);
        }
};

enum catEvents
{
    EVENT_DEFENSIVE_LEAP = 1,
    EVENT_SWIPE,
    EVENT_POUNCE_CHARGE,
};

// Crag Stalker - 72807
// Primal Stalker - 72805
// Tsavo'ka - 72808
class npc_timeless_cats : public CreatureScript
{
    public:
        npc_timeless_cats() : CreatureScript("npc_timeless_cats") { }

        struct npc_timeless_catsAI : public customCreatureAI
        {
            npc_timeless_catsAI(Creature* creature) : customCreatureAI(creature) { }

            TaskScheduler scheduler;
            bool pounce;

            void Reset() override
            {
                events.Reset();
                pounce = false;

                if (!me->HasAura(131727) && me->GetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE) == 0)
                    me->GetMotionMaster()->MoveRandom(5.0f);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->HandleEmoteStateCommand(0);
                me->RemoveAura(131727);

                events.ScheduleEvent(EVENT_DEFENSIVE_LEAP, 8 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SWIPE, 2 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_POUNCE_CHARGE, 9 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                pounce = true;

                // prevent cone swipe while in pounce
                scheduler
                    .Schedule(Seconds(5), [this](TaskContext context)
                {
                    pounce = false;
                    events.RescheduleEvent(EVENT_SWIPE, 2 * IN_MILLISECONDS);
                });
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (!pounce)
                        ExecuteTargetEvent(SPELL_RENDING_SWIPE, 10 * IN_MILLISECONDS, EVENT_SWIPE, eventId, PRIORITY_CHANNELED);

                    ExecuteTargetEvent(SPELL_DEFENSIVE_LEAP, 12 * IN_MILLISECONDS, EVENT_DEFENSIVE_LEAP, eventId, PRIORITY_SELF);
                    ExecuteTargetEvent(SPELL_POUNCE_CHARGE, 12 * IN_MILLISECONDS, EVENT_POUNCE_CHARGE, eventId);
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_timeless_catsAI(creature);
        }
};

// Zhu-Gon the Sour - 71919
class npc_timeless_elementals : public CreatureScript
{
    public:
        npc_timeless_elementals() : CreatureScript("npc_timeless_elementals") { }

        struct npc_timeless_elementalsAI : public ScriptedAI
        {
            npc_timeless_elementalsAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiBrewBoltTimer;
            uint32 uiRainOfSkunkyAleTimer;
            uint32 uiSkunkyAleCrashTimer;

            void Reset() override
            {
                uiBrewBoltTimer = 1*IN_MILLISECONDS;
                uiRainOfSkunkyAleTimer = 14*IN_MILLISECONDS;
                uiSkunkyAleCrashTimer = 6*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uiBrewBoltTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_BREW_BOLT);
                    uiBrewBoltTimer = 3*IN_MILLISECONDS;
                }
                else uiBrewBoltTimer -= diff;

                if (uiRainOfSkunkyAleTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_RAIN_OF_SKUNKY_ALE);
                    uiRainOfSkunkyAleTimer = 14*IN_MILLISECONDS;
                }
                else uiRainOfSkunkyAleTimer -= diff;

                if (uiSkunkyAleCrashTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_SKUNKY_ALE_CRASH);
                    uiSkunkyAleCrashTimer = 6*IN_MILLISECONDS;
                }
                else uiSkunkyAleCrashTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_timeless_elementalsAI(creature);
        }
};

// Foreboding Flame - 73162
// Spirit of Jadefire - 72769
class npc_timeless_spirits : public CreatureScript
{
    public:
        npc_timeless_spirits() : CreatureScript("npc_timeless_spirits") { }

        struct npc_timeless_spiritsAI : public ScriptedAI
        {
            npc_timeless_spiritsAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiFireBoltTimer;
            uint32 uiFlameStrikeTimer;

            void Reset() override
            {
                uiFireBoltTimer = 1*IN_MILLISECONDS;
                uiFlameStrikeTimer = 10*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                
                switch (me->GetEntry())
                {
                    case 73162:
                    {
                        if (uiFireBoltTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_SPIRITFIRE_BOLT);
                            uiFireBoltTimer = 5*IN_MILLISECONDS;
                        }
                        else uiFireBoltTimer -= diff;

                        if (uiFlameStrikeTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_SPIRITFLAME_STRIKE);
                            uiFlameStrikeTimer = 12*IN_MILLISECONDS;
                        }
                        else uiFlameStrikeTimer -= diff;

                        break;
                    }
                    case 72769:
                    {
                        if (uiFireBoltTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_JADEFIRE_BOLT);
                            uiFireBoltTimer = 6*IN_MILLISECONDS;
                        }
                        else uiFireBoltTimer -= diff;

                        if (uiFlameStrikeTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_JADEFLAME_STRIKE);
                            uiFlameStrikeTimer = 15*IN_MILLISECONDS;
                        }
                        else uiFlameStrikeTimer -= diff;

                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_timeless_spiritsAI(creature);
        }
};

// Eroded Cliffdweller - 72809
// Golganarr - 72970
class npc_timeless_giants : public CreatureScript
{
    public:
        npc_timeless_giants() : CreatureScript("npc_timeless_giants") { }

        struct npc_timeless_giantsAI : public ScriptedAI
        {
            npc_timeless_giantsAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiBoulderTimer;
            uint32 uiStompTimer;
            uint32 uiFrenzyTimer;
            bool boulderOrStomp;

            void Reset() override
            {
                uiBoulderTimer = 0*IN_MILLISECONDS;
                uiStompTimer = 1*IN_MILLISECONDS;
                uiFrenzyTimer = 9*IN_MILLISECONDS;
                boulderOrStomp = false; // false = Boulder, true = Stomp

                if (me->GetEntry() == RARE_GOLGANARR)
                    Talk(0);
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_EXHAUSTION))
                    return;

                if (!boulderOrStomp && me->GetVictim() && me->GetVictim()->GetDistance2d(me) <= 10.0f)
                    boulderOrStomp = true;

                if (boulderOrStomp && me->GetVictim() && me->GetVictim()->GetDistance2d(me) > 10.0f)
                    boulderOrStomp = false;

                switch (me->GetEntry())
                {
                    case 72809:
                    {
                        if (!boulderOrStomp)
                        {
                            if (uiBoulderTimer <= diff)
                            {
                                me->CastSpell(SelectTarget(SELECT_TARGET_RANDOM), SPELL_BOULDER);
                                uiBoulderTimer = 0*IN_MILLISECONDS;
                            }
                            else uiBoulderTimer -= diff;
                        }
                        else
                        {
                            if (uiStompTimer <= diff)
                            {
                                if (me->HasAura(SPELL_FRENZY))
                                    me->CastSpell(me->GetVictim(), SPELL_FRENZY_STOMP);
                                else
                                    me->CastSpell(me->GetVictim(), SPELL_STOMP);

                                uiStompTimer = 1*IN_MILLISECONDS;
                            }
                            else uiStompTimer -= diff;
                        }
                        if (uiFrenzyTimer <= diff)
                        {
                            me->CastSpell(me, SPELL_FRENZY);
                            uiFrenzyTimer = 16*IN_MILLISECONDS;
                        }
                        else uiFrenzyTimer -= diff;
                        break;
                    }
                    case RARE_GOLGANARR:
                    {
                        if (!boulderOrStomp)
                        {
                            if (uiBoulderTimer <= diff)
                            {
                                me->CastSpell(SelectTarget(SELECT_TARGET_RANDOM), SPELL_BOULDER);
                                uiBoulderTimer = 0*IN_MILLISECONDS;
                            }
                            else uiBoulderTimer -= diff;
                        }
                        else
                        {
                            if (uiStompTimer <= diff)
                            {
                                if (me->HasAura(SPELL_FRENZY))
                                    me->CastSpell(me->GetVictim(), SPELL_FRENZY_STOMP);
                                else
                                    me->CastSpell(me->GetVictim(), SPELL_STOMP);

                                uiStompTimer = 1*IN_MILLISECONDS;
                            }
                            else uiStompTimer -= diff;
                        }
                        if (uiFrenzyTimer <= diff)
                        {
                            me->CastSpell(me, SPELL_FRENZY);
                            uiFrenzyTimer = 16*IN_MILLISECONDS;
                        }
                        else uiFrenzyTimer -= diff;
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_timeless_giantsAI(creature);
        }
};

// Gulp Frog - 72777
// Bufo - 72775
class npc_timeless_frogs : public CreatureScript
{
    public:
        npc_timeless_frogs() : CreatureScript("npc_timeless_frogs") { }

        struct npc_timeless_frogsAI : public ScriptedAI
        {
            npc_timeless_frogsAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->CastSpell(me, SPELL_TOXIC_SKIN);
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_timeless_frogsAI(creature);
        }
};

enum huolonEvents
{
    EVENT_SCHOOL_BREATH = 1,
    EVENT_SCHOOL_BLOSSOM,
};

// Crimsonscale Firestorm - 72876
// Huolon - 73167
class npc_timeless_dragons : public CreatureScript
{
    public:
        npc_timeless_dragons() : CreatureScript("npc_timeless_dragons") { }

        struct npc_timeless_dragonsAI : public customCreatureAI
        {
            npc_timeless_dragonsAI(Creature* creature) : customCreatureAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SCHOOL_BREATH, 4 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SCHOOL_BLOSSOM, 10 * IN_MILLISECONDS);
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
                    switch (me->GetEntry())
                    {
                        case 72876:
                        {
                            ExecuteTargetEvent(SPELL_FLAME_BREATH, 10.5 * IN_MILLISECONDS, EVENT_SCHOOL_BREATH, eventId, PRIORITY_CHANNELED);
                            ExecuteTargetEvent(SPELL_FIRE_BLOSSOM, 12 * IN_MILLISECONDS, EVENT_SCHOOL_BLOSSOM, eventId, PRIORITY_NOT_VICTIM);
                            break;
                        }
                        case 73167:
                        {
                            ExecuteTargetEvent(SPELL_LIGHTNING_BREATH, 10.5 * IN_MILLISECONDS, EVENT_SCHOOL_BREATH, eventId, PRIORITY_CHANNELED);
                            ExecuteTargetEvent(SPELL_STORM_BLOSSOM, 12 * IN_MILLISECONDS, EVENT_SCHOOL_BLOSSOM, eventId, PRIORITY_NOT_VICTIM);
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_timeless_dragonsAI(creature);
        }
};

enum OrdosMinionEvents
{
    EVENT_CAUTERIZE = 1,
    EVENT_CONJURE,
    EVENT_FIRE_STORM_EFF,
    EVENT_GREAT_PYRO,
    EVENT_BLAZING_BLOW,
};

// Blazebound Chanter - 72897
// Eternal Kilnmaster - 72896
// High Priest of Ordos - 72898
// Flintlord Gairan - 73172
// Urdur the Cauterizer - 73173
// Archiereus of Flame - 73666
class npc_ordos_minions : public CreatureScript
{
    public:
        npc_ordos_minions() : CreatureScript("npc_ordos_minions") { }

        struct npc_ordos_minionsAI : public customCreatureAI
        {
            npc_ordos_minionsAI(Creature* creature) : customCreatureAI(creature) { }

            void Reset() override
            {
                events.Reset();

                if (me->GetEntry() == RARE_ARCHIEREUS_OF_FLAME_CLOAK)
                    if (Player* player = me->FindNearestPlayer(50.0f))
                        Talk(0, player);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (me->GetEntry() == RARE_ARCHIEREUS_OF_FLAME_CLOAK)
                    for (auto&& guid : me->GetLootRecipients())
                        if (Player* player = ObjectAccessor::GetPlayer(*me, guid))
                            if (!player->IsDailyQuestDone(QUEST_ARCHIEREUS_OF_FLAME))
                                if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_ARCHIEREUS_OF_FLAME))
                                    player->RewardQuest(quest, 0, player, false);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                switch (me->GetEntry())
                {
                    case 72896:
                    case 73172:
                        me->CastSpell(me->GetVictim(), SPELL_FIERY_CHARGE);
                        break;
                    default:
                        break;
                }

                events.ScheduleEvent(EVENT_CAUTERIZE, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CONJURE, 8 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_FIRE_STORM_EFF, 20 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_GREAT_PYRO, 1 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BLAZING_BLOW, 5 * IN_MILLISECONDS);
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
                    switch (me->GetEntry())
                    {
                        case 72897:
                        {
                            ExecuteTargetEvent(SPELL_CAUTERIZE, 35 * IN_MILLISECONDS, EVENT_CAUTERIZE, eventId, PRIORITY_SELF);
                            ExecuteTargetEvent(SPELL_CONJURE_FLARECORE_GOLEM, 25 * IN_MILLISECONDS, EVENT_CONJURE, eventId, PRIORITY_SELF);
                            ExecuteTargetEvent(SPELL_FIRE_STORM, 25 * IN_MILLISECONDS, EVENT_FIRE_STORM_EFF, eventId, PRIORITY_SELF);
                            ExecuteTargetEvent(SPELL_GREATER_PYROBLAST, 1.5 * IN_MILLISECONDS, EVENT_GREAT_PYRO, eventId);
                            break;
                        }
                        case 72896:
                        case 73174:
                        {
                            ExecuteTargetEvent(SPELL_BLAZING_BLOW, 8 * IN_MILLISECONDS, EVENT_BLAZING_BLOW, eventId, PRIORITY_CHANNELED);
                            ExecuteTargetEvent(SPELL_CONJURE_ETERNAL_KILN, 25 * IN_MILLISECONDS, EVENT_CONJURE, eventId, PRIORITY_SELF);
                            break;
                        }
                        case 72898:
                        {
                            ExecuteTargetEvent(SPELL_CAUTERIZE, 35 * IN_MILLISECONDS, EVENT_CAUTERIZE, eventId, PRIORITY_SELF);
                            ExecuteTargetEvent(RAND(SPELL_CONJURE_FLARECORE_GOLEM, SPELL_CONJURE_ETERNAL_KILN), 25 * IN_MILLISECONDS, EVENT_CONJURE, eventId, PRIORITY_SELF);
                            ExecuteTargetEvent(SPELL_FIRE_STORM, 25 * IN_MILLISECONDS, EVENT_FIRE_STORM_EFF, eventId, PRIORITY_SELF);
                            ExecuteTargetEvent(SPELL_BLAZING_BLOW, 8 * IN_MILLISECONDS, EVENT_BLAZING_BLOW, eventId, PRIORITY_CHANNELED);
                            break;
                        }
                        case 73172:
                        {
                            ExecuteTargetEvent(SPELL_BLAZING_BLOW, 8 * IN_MILLISECONDS, EVENT_BLAZING_BLOW, eventId, PRIORITY_CHANNELED);
                            ExecuteTargetEvent(SPELL_CONJURE_ETERNAL_KILN, 25 * IN_MILLISECONDS, EVENT_CONJURE, eventId, PRIORITY_SELF);
                            break;
                        }
                        case 73173:
                        {
                            ExecuteTargetEvent(SPELL_CAUTERIZE, 35 * IN_MILLISECONDS, EVENT_CAUTERIZE, eventId, PRIORITY_SELF);
                            ExecuteTargetEvent(SPELL_CONJURE_FLARECORE_GOLEM, 25 * IN_MILLISECONDS, EVENT_CONJURE, eventId, PRIORITY_SELF);
                            ExecuteTargetEvent(SPELL_FIRE_STORM, 25 * IN_MILLISECONDS, EVENT_FIRE_STORM_EFF, eventId, PRIORITY_SELF);
                            ExecuteTargetEvent(SPELL_GREATER_PYROBLAST, 1.5 * IN_MILLISECONDS, EVENT_GREAT_PYRO, eventId);
                            break;
                        }
                        
                        case RARE_ARCHIEREUS_OF_FLAME_CLOAK:
                        {
                            ExecuteTargetEvent(SPELL_CAUTERIZE, 35 * IN_MILLISECONDS, EVENT_CAUTERIZE, eventId, PRIORITY_SELF);
                            ExecuteTargetEvent(RAND(SPELL_CONJURE_FLARECORE_GOLEM, SPELL_CONJURE_ETERNAL_KILN), 25 * IN_MILLISECONDS, EVENT_CONJURE, eventId, PRIORITY_SELF);
                            ExecuteTargetEvent(SPELL_FIRE_STORM, 25 * IN_MILLISECONDS, EVENT_FIRE_STORM_EFF, eventId, PRIORITY_SELF);
                            ExecuteTargetEvent(SPELL_BLAZING_BLOW, 8 * IN_MILLISECONDS, EVENT_BLAZING_BLOW, eventId, PRIORITY_CHANNELED);
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ordos_minionsAI(creature);
        }
};

// Eternal Kiln - 73528
class npc_eternal_kiln : public CreatureScript
{
    public:
        npc_eternal_kiln() : CreatureScript("npc_eternal_kiln") { }

        struct npc_eternal_kilnAI : public ScriptedAI
        {
            npc_eternal_kilnAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            bool kilnFire;

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                me->AddUnitState(UNIT_STATE_ROTATING); // Restric movements

                kilnFire = false;
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->DespawnOrUnsummon();
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!kilnFire)
                {
                    if (TempSummon* summon = me->ToTempSummon())
                        if (Unit* summoner = summon->GetSummoner())
                        {
                            summon->CastSpell(summoner, SPELL_KILNFIRE);
                            kilnFire = true;
                        }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_eternal_kilnAI(creature);
        }
};

// Damp Shambler - 72771
// Rock Moss - 73157
// Spelurk - 71864
class npc_damp_shamblers : public CreatureScript
{
    public:
        npc_damp_shamblers() : CreatureScript("npc_damp_shamblers") { }

        struct npc_damp_shamblersAI : public ScriptedAI
        {
            npc_damp_shamblersAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiMurkyCloudTimer;
            uint32 uiRenewingMistsTimer;
            uint32 uiPoisonBreathTimer;

            void Reset() override
            {
                uiMurkyCloudTimer = 5*IN_MILLISECONDS;
                uiRenewingMistsTimer = 10*IN_MILLISECONDS;
                uiPoisonBreathTimer = 8*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (me->GetEntry() == 72896)
                    me->CastSpell(me->GetVictim(), SPELL_FIERY_CHARGE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (me->GetEntry())
                {
                    case 72771:
                    {
                        if (uiMurkyCloudTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_MURKY_CLOUD);
                            uiMurkyCloudTimer = 20*IN_MILLISECONDS;
                        }
                        else uiMurkyCloudTimer -= diff;
                        
                        if (uiRenewingMistsTimer <= diff)
                        {
                            me->CastSpell(me, SPELL_RENEWING_MISTS);
                            uiRenewingMistsTimer = 18*IN_MILLISECONDS;
                        }
                        else uiRenewingMistsTimer -= diff;

                        break;
                    }

                    case 73157:
                    {
                        if (uiMurkyCloudTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_MURKY_CLOUD);
                            uiMurkyCloudTimer = 20*IN_MILLISECONDS;
                        }
                        else uiMurkyCloudTimer -= diff;
                        
                        if (uiRenewingMistsTimer <= diff)
                        {
                            me->CastSpell(me, SPELL_RENEWING_MISTS);
                            uiRenewingMistsTimer = 18*IN_MILLISECONDS;
                        }
                        else uiRenewingMistsTimer -= diff;

                        break;
                    }

                    case 71864:
                    {
                        if (uiMurkyCloudTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_MURKY_CLOUD);
                            uiMurkyCloudTimer = 20*IN_MILLISECONDS;
                        }
                        else uiMurkyCloudTimer -= diff;
                        
                        if (uiRenewingMistsTimer <= diff)
                        {
                            me->CastSpell(me, SPELL_RENEWING_MISTS);
                            uiRenewingMistsTimer = 18*IN_MILLISECONDS;
                        }
                        else uiRenewingMistsTimer -= diff;

                        if (uiPoisonBreathTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_POISON_BREATH);
                            uiPoisonBreathTimer = 13*IN_MILLISECONDS;
                        }
                        else uiPoisonBreathTimer -= diff;

                        break;
                    }

                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_damp_shamblersAI(creature);
        }
};

enum oathguardEvents
{
    EVENT_CRACKING_BLOW = 1,
    EVENT_DEFENSIVE_SHIELD,
};

// Ordon Oathguard - 72892
// Jakur of Ordon - 73169
class npc_ordon_guards : public CreatureScript
{
    public:
        npc_ordon_guards() : CreatureScript("npc_ordon_guards") { }

        struct npc_ordon_guardsAI : public customCreatureAI
        {
            npc_ordon_guardsAI(Creature* creature) : customCreatureAI(creature) { }

            bool oathOfGuardianship;

            void Reset() override
            {
                events.Reset();
                oathOfGuardianship = false;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CRACKING_BLOW, 4 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_DEFENSIVE_SHIELD, 10 * IN_MILLISECONDS);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (!oathOfGuardianship && HealthBelowPct(20))
                {
                    oathOfGuardianship = true;

                    if (!me->HasAura(SPELL_OATH_OF_GUARDIANSHIP))
                        me->CastSpell(me, SPELL_OATH_OF_GUARDIANSHIP);
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
                    ExecuteTargetEvent(SPELL_CRACKING_BLOW, 6 * IN_MILLISECONDS, EVENT_CRACKING_BLOW, eventId, PRIORITY_CHANNELED);
                    ExecuteTargetEvent(SPELL_DEFENSIVE_SHIELD, 25 * IN_MILLISECONDS, EVENT_DEFENSIVE_SHIELD, eventId, PRIORITY_SELF);
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ordon_guardsAI(creature);
        }
};

// Burning Berserker - 72895
// Champion of the Black Flame - 73171
class npc_ordos_warriors : public CreatureScript
{
    public:
        npc_ordos_warriors() : CreatureScript("npc_ordos_warriors") { }

        struct npc_ordos_warriorsAI : public ScriptedAI
        {
            npc_ordos_warriorsAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiForageTimer;
            uint32 uiBlazingCleaveTimer;
            uint32 uiBurningFuryTimer;

            void Reset() override
            {
                uiForageTimer = 5*IN_MILLISECONDS;
                uiBlazingCleaveTimer = 4*IN_MILLISECONDS;
                uiBurningFuryTimer = 10*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->CastSpell(me->GetVictim(), SPELL_FIERY_CHARGE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->IsInCombat() && !me->isMoving())
                {
                    if (uiForageTimer <= diff)
                    {
                        if (roll_chance_i(20))
                            me->CastSpell(me, SPELL_FORAGE);
                        uiForageTimer = 25*IN_MILLISECONDS;
                    }
                    else uiForageTimer -= diff;
                }

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (me->GetEntry())
                {
                    case 72895:
                    {
                        if (uiBlazingCleaveTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_BLAZING_CLEAVE);
                            uiBlazingCleaveTimer = 6*IN_MILLISECONDS;
                        }
                        else uiBlazingCleaveTimer -= diff;

                        if (uiBurningFuryTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_BURNING_FURY);
                            uiBurningFuryTimer = 20*IN_MILLISECONDS;
                        }
                        else uiBurningFuryTimer -= diff;

                        break;
                    }

                    case 73171:
                    {
                        if (uiBlazingCleaveTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_BLAZING_CLEAVE);
                            uiBlazingCleaveTimer = 6*IN_MILLISECONDS;
                        }
                        else uiBlazingCleaveTimer -= diff;

                        if (uiBurningFuryTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_BURNING_FURY);
                            uiBurningFuryTimer = 20*IN_MILLISECONDS;
                        }
                        else uiBurningFuryTimer -= diff;

                        break;
                    }

                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ordos_warriorsAI(creature);
        }
};

// Ordon Fire-Watcher 72894
// Watcher Osu - 73170
class npc_ordon_watchers : public CreatureScript
{
    public:
        npc_ordon_watchers() : CreatureScript("npc_ordon_watchers") { }

        struct npc_ordon_watchersAI : public ScriptedAI
        {
            npc_ordon_watchersAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiFallingFlamesTimer;
            uint32 uiPyroblastTimer;

            void Reset() override
            {
                uiFallingFlamesTimer = 4*IN_MILLISECONDS;
                uiPyroblastTimer = 1*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (me->GetEntry())
                {
                    case 72894:
                    {
                        if (uiFallingFlamesTimer <= diff)
                        {
                            me->CastSpell(me, SPELL_FALLING_FLAMES);
                            uiFallingFlamesTimer = 4*IN_MILLISECONDS;
                        }
                        else uiFallingFlamesTimer -= diff;

                        if (uiPyroblastTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_PYROBLAST);
                            uiPyroblastTimer = 2*IN_MILLISECONDS;
                        }
                        else uiPyroblastTimer -= diff;

                        break;
                    }
                    case 73170:
                    {
                        if (uiFallingFlamesTimer <= diff)
                        {
                            me->CastSpell(me, SPELL_FALLING_FLAMES);
                            uiFallingFlamesTimer = 4*IN_MILLISECONDS;
                        }
                        else uiFallingFlamesTimer -= diff;

                        if (uiPyroblastTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_PYROBLAST);
                            uiPyroblastTimer = 2*IN_MILLISECONDS;
                        }
                        else uiPyroblastTimer -= diff;
                        break;
                    }

                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ordon_watchersAI(creature);
        }
};

// Molten Guardian - 72888 x
// Cinderfall - 73175

enum moltenGuardiansEvents
{
    EVENT_MOLTEN_INFERNO_GUARDIAN = 1,
};

const std::map<uint32, uint32> invMoltenRotationType
{
    { 0, SPELL_MOLTEN_INFERNO_1 },
    { 1, SPELL_MOLTEN_INFERNO_2 },
    { 2, SPELL_MOLTEN_INFERNO_3 },
};

class npc_molten_guards : public CreatureScript
{
    public:
        npc_molten_guards() : CreatureScript("npc_molten_guards") { }

        struct npc_molten_guardsAI : public ScriptedAI
        {
            npc_molten_guardsAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            TaskScheduler scheduler;
            uint8 spellRotation;
            ObjectGuid targetGUID;

            void Reset() override
            {
                spellRotation = 0;
                targetGUID = ObjectGuid::Empty;
                me->SetReactState(REACT_AGGRESSIVE);
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_MOLTEN_INFERNO_GUARDIAN, 6 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_MOLTEN_INFERNO_GUARDIAN)
                    {
                        if (Unit* target = me->GetVictim())
                        {
                            targetGUID = target->GetGUID();
                            me->PrepareChanneledCast(me->GetAngle(target), invMoltenRotationType.find(spellRotation)->second);

                            scheduler
                                .Schedule(Milliseconds(2600), [this](TaskContext context)
                            {
                                // Prevent if evade
                                if (me->IsInCombat())
                                    me->RemoveChanneledCast(targetGUID);
                            });
                        }

                        if (++spellRotation > 2)
                            spellRotation = 0;

                        events.ScheduleEvent(EVENT_MOLTEN_INFERNO_GUARDIAN, 10 * IN_MILLISECONDS);
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_molten_guardsAI(creature);
        }
};

// Leafmender - 73277
// Ashleaf Sprite - 72877
class npc_leaf_sprites : public CreatureScript
{
    public:
        npc_leaf_sprites() : CreatureScript("npc_leaf_sprites") { }

        struct npc_leaf_spritesAI : public ScriptedAI
        {
            npc_leaf_spritesAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiGreathWrathTimer;
            uint32 uiHealingTouchTimer;
            uint32 uiSunWorshipTimer;

            void Reset() override
            {
                uiGreathWrathTimer = 1*IN_MILLISECONDS;
                uiHealingTouchTimer = 10*IN_MILLISECONDS;
                uiSunWorshipTimer = 6*IN_MILLISECONDS;

                if (Creature* trigger = me->FindNearestCreature(72973, 25.0f))
                    me->CastSpell(trigger, 28892);
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (me->GetEntry())
                {
                    case 73277:
                    {
                        if (uiGreathWrathTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_GREAT_WRATH);
                            uiGreathWrathTimer = 2*IN_MILLISECONDS;
                        }
                        else uiGreathWrathTimer -= diff;

                        if (uiHealingTouchTimer <= diff)
                        {
                            me->CastSpell(me, SPELL_HEALING_TOUCH);
                            uiHealingTouchTimer = 15*IN_MILLISECONDS;
                        }
                        else uiHealingTouchTimer -= diff;

                        if (uiSunWorshipTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_SUN_WORSHIP);
                            uiSunWorshipTimer = 6*IN_MILLISECONDS;
                        }
                        else uiSunWorshipTimer -= diff;
                        break;
                    }
                    case 72877:
                    {
                        if (uiGreathWrathTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_GREAT_WRATH);
                            uiGreathWrathTimer = 2*IN_MILLISECONDS;
                        }
                        else uiGreathWrathTimer -= diff;

                        if (uiHealingTouchTimer <= diff)
                        {
                            me->CastSpell(me, SPELL_HEALING_TOUCH);
                            uiHealingTouchTimer = 15*IN_MILLISECONDS;
                        }
                        else uiHealingTouchTimer -= diff;

                        if (uiSunWorshipTimer <= diff)
                        {
                            me->CastSpell(me->GetVictim(), SPELL_SUN_WORSHIP);
                            uiSunWorshipTimer = 6*IN_MILLISECONDS;
                        }
                        else uiSunWorshipTimer -= diff;
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_leaf_spritesAI(creature);
        }
};

enum KarkanosEvents
{
    EVENT_GUSHING_MAW = 1,
};

// Karkanos - 72193
class npc_karkanos : public CreatureScript
{
    public:
        npc_karkanos() : CreatureScript("npc_karkanos") { }

        struct npc_karkanosAI : public ScriptedAI
        {
            npc_karkanosAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_GUSHING_MAW, 3 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_GUSHING_MAW:
                    {
                        me->CastSpell(me->GetVictim(), SPELL_GUSHING_MAW);
                        events.ScheduleEvent(EVENT_GUSHING_MAW, 5 * IN_MILLISECONDS);
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_karkanosAI(creature);
        }
};

// Zesqua - 72245
class npc_zesqua : public CreatureScript
{
    public:
        npc_zesqua() : CreatureScript("npc_zesqua") { }

        struct npc_zesquaAI : public ScriptedAI
        {
            npc_zesquaAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiFuriousSplashTimer;
            uint32 uiWaterBlossomTimer;
            uint32 uiWaterBoltVolleyTimer;

            void Reset() override
            {
                uiFuriousSplashTimer = 7*IN_MILLISECONDS;
                uiWaterBlossomTimer = 10*IN_MILLISECONDS;
                uiWaterBoltVolleyTimer = 3*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uiFuriousSplashTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_FURIOUS_SPLASH);
                    uiFuriousSplashTimer = 12*IN_MILLISECONDS;
                }
                else uiFuriousSplashTimer -= diff;

                if (uiWaterBlossomTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_WATER_BLOSSOM);
                    uiWaterBlossomTimer = 16*IN_MILLISECONDS;
                }
                else uiWaterBlossomTimer -= diff;

                if (uiWaterBoltVolleyTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_WATER_BOLT_VOLLEY);
                    uiWaterBoltVolleyTimer = 3*IN_MILLISECONDS;
                }
                else uiWaterBoltVolleyTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zesquaAI(creature);
        }
};

// Jademist Dancer - 72767
class npc_jademist_dancer : public CreatureScript
{
    public:
        npc_jademist_dancer() : CreatureScript("npc_jademist_dancer") { }

        struct npc_jademist_dancerAI : public ScriptedAI
        {
            npc_jademist_dancerAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiSteamBlastTimer;

            void Reset() override
            {
                uiSteamBlastTimer = 6*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uiSteamBlastTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_STEAM_BLAST);
                    uiSteamBlastTimer = 10*IN_MILLISECONDS;
                }
                else uiSteamBlastTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_jademist_dancerAI(creature);
        }
};

// Cranegnasher - 72049
struct npc_cranegnasher : public ScriptedAI
{
    npc_cranegnasher(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        me->CastSpell(me, SPELL_STEALTH);
        scheduler
            .Schedule(Seconds(3), [=](TaskContext ctx)
            {
                DoCastVictim(SPELL_BITE_MORSEL);
                ctx.Repeat(Seconds(8));
            })
            .Schedule(Seconds(10), [=](TaskContext ctx)
            {
                DoCastVictim(SPELL_CLAW);
                ctx.Repeat(Seconds(10));
            })
            .Schedule(Seconds(12), [=](TaskContext ctx)
            {
                DoCastVictim(SPELL_POUNCE);
                ctx.Repeat(Seconds(18));
            })
            .Schedule(Seconds(16), [=](TaskContext ctx)
            {
                DoCastVictim(SPELL_RAKE);
                ctx.Repeat(Seconds(20));
            });
    }

    void EnterEvadeMode() override
    {
        scheduler.CancelAll();
        ScriptedAI::EnterEvadeMode();
    }

    void JustEngagedWith(Unit* /*who*/) override { }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        scheduler.Update(diff);
        DoMeleeAttackIfReady();
    }

    void JustDied(Unit*) override
    {
        for (auto&& guid : me->GetLootRecipients())
            if (Player* player = ObjectAccessor::GetPlayer(*me, guid))
                if (player->IsAtGroupRewardDistance(me))
                    player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, 73854, 1, 0, me);
    }
};

enum NiceSprite
{
    EVENT_NATURES_WRATH = 1,
    EVENT_STICKY_SAP    = 2,

    SPELL_ASCENDANCE    = 114052,

    NPC_ANGRY_SPRITE    = 71824,
};

// Nice Sprite - 71823
struct npc_nice_sprite : public ScriptedAI
{
    npc_nice_sprite(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void SpellHit(Unit* caster, const SpellInfo* spell) override
    {
        if (spell->Id == SPELL_ASCENDANCE)
        {
            me->UpdateEntry(NPC_ANGRY_SPRITE);
            me->Attack(caster, true);
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_NATURES_WRATH, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_STICKY_SAP, 3 * IN_MILLISECONDS);
    }

    void JustDied(Unit* killer) override
    {
        if (roll_chance_i(20))
            if (Creature* creture = me->SummonCreature(71826, me->GetPosition()))
                creture->Attack(killer, true);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_NATURES_WRATH:
            {
                me->CastSpell(me->GetVictim(), SPELL_NATURE_S_WRATH);
                events.ScheduleEvent(EVENT_NATURES_WRATH, 1 * IN_MILLISECONDS);
                break;
            }
            case EVENT_STICKY_SAP:
            {
                me->CastSpell(me->GetVictim(), SPELL_STICKY_SAP);
                events.ScheduleEvent(EVENT_STICKY_SAP, 2 * IN_MILLISECONDS);
                break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Ordon Candlekeeper - 72875
class npc_ordon_candlekeeper : public CreatureScript
{
    public:
        npc_ordon_candlekeeper() : CreatureScript("npc_ordon_candlekeeper") { }

        struct npc_ordon_candlekeeperAI : public ScriptedAI
        {
            npc_ordon_candlekeeperAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiBreathOfFireTimer;

            void Reset() override
            {
                uiBreathOfFireTimer = 1*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (HealthBelowPct(25))
                    me->CastSpell(me, SPELL_BURNING_SACRIFICE);

                if (uiBreathOfFireTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_BREATH_OF_FIRE);
                    uiBreathOfFireTimer = 2*IN_MILLISECONDS;
                }
                else uiBreathOfFireTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ordon_candlekeeperAI(creature);
        }
};

// Garnia - 73282
class npc_garnia : public CreatureScript
{
    public:
        npc_garnia() : CreatureScript("npc_garnia") { }

        struct npc_garniaAI : public ScriptedAI
        {
            npc_garniaAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiRubyBoltTimer;

            void Reset() override
            {
                uiRubyBoltTimer = 2*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uiRubyBoltTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_RUBY_BOLT);
                    uiRubyBoltTimer = 3*IN_MILLISECONDS;
                }
                else uiRubyBoltTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_garniaAI(creature);
        }
};

// Rattleskew - 72048
class npc_rattleskew : public CreatureScript
{
    public:
        npc_rattleskew() : CreatureScript("npc_rattleskew") { }

        struct npc_rattleskewAI : public ScriptedAI
        {
            npc_rattleskewAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiCurseOfPainTimer;
            uint32 uiDrowningDeathTimer;
            uint32 uiShadowBoltTimer;
            uint32 uiSpikedBoneTimer;

            void Reset() override
            {
                uiCurseOfPainTimer = 2*IN_MILLISECONDS;
                uiDrowningDeathTimer = 5*IN_MILLISECONDS;
                uiShadowBoltTimer = 4*IN_MILLISECONDS;
                uiSpikedBoneTimer = 1*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uiCurseOfPainTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_CURSE_OF_PAIN);
                    uiCurseOfPainTimer = 20*IN_MILLISECONDS;
                }
                else uiCurseOfPainTimer -= diff;

                if (uiDrowningDeathTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_DROWNING_DEATH);
                    uiDrowningDeathTimer = 30*IN_MILLISECONDS;
                }
                else uiDrowningDeathTimer -= diff;

                if (uiShadowBoltTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_SHADOW_BOLT);
                    uiShadowBoltTimer = 8*IN_MILLISECONDS;
                }
                else uiShadowBoltTimer -= diff;

                if (uiSpikedBoneTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_SPIKED_BONE);
                    uiSpikedBoneTimer = 6*IN_MILLISECONDS;
                }
                else uiSpikedBoneTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_rattleskewAI(creature);
        }
};

// Southsea Lookout - 73718
class npc_southsea_lookout : public CreatureScript
{
    public:
        npc_southsea_lookout() : CreatureScript("npc_southsea_lookout") { }

        struct npc_southsea_lookoutAI : public ScriptedAI
        {
            npc_southsea_lookoutAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->CastSpell(me, SPELL_SPY_GLASS);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->RemoveAurasDueToSpell(SPELL_SPY_GLASS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_southsea_lookoutAI(creature);
        }
};

// Playful Water Spirit - 71883
class npc_playful_water_spirit : public CreatureScript
{
    public:
        npc_playful_water_spirit() : CreatureScript("npc_playful_water_spirit") { }

        struct npc_playful_water_spiritAI : public ScriptedAI
        {
            npc_playful_water_spiritAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiBubblePopTimer;
            uint32 uiWaterBoltTimer;
            uint32 uiWaterSpoutTimer;

            void Reset() override
            {
                uiBubblePopTimer = 5*IN_MILLISECONDS;
                uiWaterBoltTimer = 2*IN_MILLISECONDS;
                uiWaterSpoutTimer = 6*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uiBubblePopTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_BUBBLE_POP);
                    uiBubblePopTimer = 30*IN_MILLISECONDS;
                }
                else uiBubblePopTimer -= diff;

                if (uiWaterBoltTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_WATER_BOLT);
                    uiWaterBoltTimer = 3*IN_MILLISECONDS;
                }
                else uiWaterBoltTimer -= diff;

                if (uiWaterSpoutTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_WATER_SPOUT);
                    uiWaterSpoutTimer = 8*IN_MILLISECONDS;
                }
                else uiWaterSpoutTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_playful_water_spiritAI(creature);
        }
};

// Cove Shark - 71986
class npc_cove_shark : public CreatureScript
{
    public:
        npc_cove_shark() : CreatureScript("npc_cove_shark") { }

        struct npc_cove_sharkAI : public ScriptedAI
        {
            npc_cove_sharkAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiSavageBiteimer;
            uint32 uiScentOfBloodTimer;

            void Reset() override
            {
                uiSavageBiteimer = 1*IN_MILLISECONDS;
                uiScentOfBloodTimer = 5*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uiSavageBiteimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_SAVAGE_BITE);
                    uiSavageBiteimer = 5*IN_MILLISECONDS;
                }
                else uiSavageBiteimer -= diff;

                if (uiScentOfBloodTimer <= diff)
                {
                    me->CastSpell(me, SPELL_SCENT_OF_BLOOD);
                    uiScentOfBloodTimer = 15*IN_MILLISECONDS;
                }
                else uiScentOfBloodTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cove_sharkAI(creature);
        }
};

// Stinkbraid - 73704
class npc_stinkbraid : public CreatureScript
{
    public:
        npc_stinkbraid() : CreatureScript("npc_stinkbraid") { }

        struct npc_stinkbraidAI : public ScriptedAI
        {
            npc_stinkbraidAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->RemoveAurasDueToSpell(SPELL_ROTTEN_STENCH);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->CastSpell(me, SPELL_ROTTEN_STENCH);
                me->CastSpell(me->GetVictim(), SPELL_LEAPING_CLEAVE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_stinkbraidAI(creature);
        }
};

// Spectral Pirate - 71987
class npc_spectral_pirate : public CreatureScript
{
    public:
        npc_spectral_pirate() : CreatureScript("npc_spectral_pirate") { }

        struct npc_spectral_pirateAI : public ScriptedAI
        {
            npc_spectral_pirateAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiBlackFogTimer;
            uint32 uiSinisterStrikeTimer;

            void Reset() override
            {
                uiBlackFogTimer = 8 * IN_MILLISECONDS;
                uiSinisterStrikeTimer = 2*IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(0);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uiBlackFogTimer <= diff)
                {
                    me->CastSpell(me->GetVictim(), SPELL_BLACK_FOG);
                    uiBlackFogTimer = 10 * IN_MILLISECONDS;
                }
                else uiBlackFogTimer -= diff;

                if (uiSinisterStrikeTimer <= diff)
                {
                    me->CastSpell(me, SPELL_SINISTER_STRIKE);
                    uiSinisterStrikeTimer = 5*IN_MILLISECONDS;
                }
                else uiSinisterStrikeTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spectral_pirateAI(creature);
        }
};

class npc_hopswift : public CreatureScript
{
    public:
        npc_hopswift() : CreatureScript("npc_hopswift") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->GetSession()->SendShowMailBox(creature->GetGUID());
            return true;
        }
};

enum ATimelessQuestion
{
    GOSSIP_MENU_QUESTION_01        = 15889,
    GOSSIP_MENU_QUESTION_02        = 15921,
    GOSSIP_MENU_QUESTION_03        = 65110,
    GOSSIP_MENU_QUESTION_04        = 65111,
    GOSSIP_MENU_QUESTION_05        = 15933,
    GOSSIP_MENU_QUESTION_06        = 65112,
    GOSSIP_MENU_QUESTION_07        = 65113,
    GOSSIP_MENU_QUESTION_08        = 15910,
    GOSSIP_MENU_QUESTION_09        = 65115,
    GOSSIP_MENU_QUESTION_10        = 65116,
    GOSSIP_MENU_QUESTION_11        = 65117,
    GOSSIP_MENU_QUESTION_12        = 65118,
    GOSSIP_MENU_QUESTION_13        = 65119,
    GOSSIP_MENU_QUESTION_14        = 65120,
    GOSSIP_MENU_QUESTION_15        = 65121,
    GOSSIP_MENU_QUESTION_16        = 65122,
    GOSSIP_MENU_QUESTION_17        = 15919,
    GOSSIP_MENU_QUESTION_18        = 65123,
    GOSSIP_MENU_QUESTION_19        = 65124,
    GOSSIP_MENU_QUESTION_20        = 65125,
    GOSSIP_MENU_QUESTION_21        = 65126,
    GOSSIP_MENU_QUESTION_22        = 65127,
    GOSSIP_MENU_QUESTION_23        = 65128,
    GOSSIP_MENU_QUESTION_24        = 65129,
    GOSSIP_MENU_QUESTION_25        = 65130,
    GOSSIP_MENU_QUESTION_26        = 65131,
    GOSSIP_MENU_QUESTION_27        = 15930,
    GOSSIP_MENU_QUESTION_28        = 65132,
    GOSSIP_MENU_QUESTION_29        = 65133,
    GOSSIP_MENU_QUESTION_30        = 65134,
    GOSSIP_MENU_QUESTION_31        = 65135,
    GOSSIP_MENU_QUESTION_32        = 65136,
    GOSSIP_MENU_QUESTION_33        = 65137,
    GOSSIP_MENU_QUESTION_34        = 65138,
    GOSSIP_MENU_QUESTION_35        = 65139,
    GOSSIP_MENU_QUESTION_36        = 65140,

    GOSSIP_TEXT_QUESTION_01        = 22900,
    GOSSIP_TEXT_QUESTION_02        = 22931,
    GOSSIP_TEXT_QUESTION_03        = 22905,
    GOSSIP_TEXT_QUESTION_04        = 22906,
    GOSSIP_TEXT_QUESTION_05        = 22943,
    GOSSIP_TEXT_QUESTION_06        = 22938,
    GOSSIP_TEXT_QUESTION_07        = 22908,
    GOSSIP_TEXT_QUESTION_08        = 22920,
    GOSSIP_TEXT_QUESTION_09        = 22942,
    GOSSIP_TEXT_QUESTION_10        = 22904,
    GOSSIP_TEXT_QUESTION_11        = 22909,
    GOSSIP_TEXT_QUESTION_12        = 22923,
    GOSSIP_TEXT_QUESTION_13        = 22946,
    GOSSIP_TEXT_QUESTION_14        = 22922,
    GOSSIP_TEXT_QUESTION_15        = 22934,
    GOSSIP_TEXT_QUESTION_16        = 22926,
    GOSSIP_TEXT_QUESTION_17        = 22929,
    GOSSIP_TEXT_QUESTION_18        = 22925,
    GOSSIP_TEXT_QUESTION_19        = 22935,
    GOSSIP_TEXT_QUESTION_20        = 22944,
    GOSSIP_TEXT_QUESTION_21        = 22924,
    GOSSIP_TEXT_QUESTION_22        = 22937,
    GOSSIP_TEXT_QUESTION_23        = 22921,
    GOSSIP_TEXT_QUESTION_24        = 22945,
    GOSSIP_TEXT_QUESTION_25        = 61009,
    GOSSIP_TEXT_QUESTION_26        = 22907,
    GOSSIP_TEXT_QUESTION_27        = 22940,
    GOSSIP_TEXT_QUESTION_28        = 22928,
    GOSSIP_TEXT_QUESTION_29        = 22941,
    GOSSIP_TEXT_QUESTION_30        = 22918,
    GOSSIP_TEXT_QUESTION_31        = 22939,
    GOSSIP_TEXT_QUESTION_32        = 22927,
    GOSSIP_TEXT_QUESTION_33        = 22903,
    GOSSIP_TEXT_QUESTION_34        = 22936,
    GOSSIP_TEXT_QUESTION_35        = 22930,
    GOSSIP_TEXT_QUESTION_36        = 22933,

    GORLOCS                        = 0,
    MURGHOULS                      = 1,
    WOLVAR                         = 2,
    MURLICHES                      = 3,
    DEFECTIVE_ELEKK_TURD           = 0,
    CRYSTAL_DEATH_TRAP             = 1,
    WORTHLESS_ELEKK_DUNG           = 2,
    RADIOACTIVE_BIOHAZARD          = 3,
    GELBIN_MEKKATORQUE             = 0,
    MILLHOUSE_MANASTORM            = 1,
    SICCO_THERMAPLUGG              = 2,
    FIZZCRANK_FULLTHROTTLE         = 3,
    CRIMSON_RING                   = 0,
    EMERALD_CIRCLE                 = 1,
    EARTHEN_RING                   = 2,
    CENARION_CIRCLE                = 3,
    CHENETA                        = 0,
    KRISTINE_DENNY                 = 1,
    TATAI                          = 2,
    DELA_RUNETOTEM                 = 3,
    GOREHOOF                       = 0,
    TOOTHGNASHER                   = 1,
    BLOODHORN                      = 2,
    STEELMAULER                    = 3,
    RED_DRAGONFLIGHT               = 0,
    BLUE_DRAGONFLIGHT              = 1,
    GREEN_DRAGONFLIGHT             = 2,
    BRONZE_DRAGONFLIGHT            = 3,
    ACHERUS                        = 0,
    NAXXANAR                       = 1,
    KOLRAMAS                       = 2,
    NAXXRAMAS                      = 3,
    MAAKA                          = 0,
    GRIMATH                        = 1,
    TALAK                          = 2,
    RAWIRI                         = 3,
    SHAMAN                         = 0,
    STORMCALLER                    = 1,
    ARCHDRUID                      = 2,
    FAR_SEER                       = 3,
    SNAKECOIL_CAVERN               = 0,
    SERPENTINE_BASIN               = 1,
    COILFANG_RESERVOIR             = 2,
    SPIRALFANG_CISTERN             = 3,
    EMMA_HARRINGTON                = 0,
    CALISSA_HARRINGTON             = 1,
    VANESSA_WHITEHALL              = 2,
    KATRINA_WHITEHALL              = 3,
    SHARP_CLAW                     = 0,
    RAZOR_TOOTH                    = 1,
    SHARP_TOOTH                    = 2,
    RAZOR_CLAW                     = 3,
    QUEEN_LIA_GREYMANE             = 0,
    QUEEN_MALIA_GREYMANE           = 1,
    QUEEN_LIRIA_GREYMANE           = 2,
    QUEEN_MIA_GREYMANE             = 3,
    TIFFIN_WINDEMERE_WRYNN         = 0,
    THERESE_ANNE_WRYNN             = 1,
    THERESE_ANGHARAD_WRYNN         = 2,
    TIFFIN_ELLERIAN_WRYNN          = 3,
    TENEBRON_VESPERON_AND_SHADRON  = 0,
    TENEBRON_VESPERON_AND_HALION   = 1,
    THERALION_SHADRON_AND_ABYSSION = 2,
    THERALION_HALION_AND_ABYSSION  = 3,
    AMANTHUL                       = 0,
    NORGANNON                      = 1,
    EONAR                          = 2,
    KHAZGOROTH                     = 3,
    VELEN                          = 0,
    AKAMA                          = 1,
    MARAAD                         = 2,
    NOBUNDO                        = 3,
    KARESH                         = 0,
    XARODI                         = 1,
    KHURAL                         = 2,
    XOROTH                         = 3,
    EREDRUIN                       = 0,
    SAYAAD                         = 1,
    SHIVARRA                       = 2,
    EREDAR                         = 3,
    TERON_GOREFIEND                = 0,
    ARTHAS_MENETHIL                = 1,
    ALEXANDROS_MOGRAINE            = 2,
    KOLTIRA_DEATHWEAVER            = 3,
    FELLARI_SWIFTARROW             = 0,
    HOLIA_SUNSHIELD                = 1,
    YANA_BLOODSPEAR                = 2,
    VALEA_TWINBLADES               = 3,
    SWIFTWING                      = 0,
    SHARPBEAK                      = 1,
    SKYREE                         = 2,
    STORMBEAK                      = 3,
    SHIRVALLAH                     = 0,
    KIMBUL                         = 1,
    MUEHZALA                       = 2,
    SHADRA                         = 3,
    PRINCE_ARTHAS_MENETHIL         = 0,
    KING_LLANE_WRYNN               = 1,
    UTHER_THE_LIGHTBRINGER         = 2,
    KING_TERENAS_MENETHIL_II       = 3,
    MAGHAR                         = 0,
    FELBLOOD                       = 1,
    MOKNATHAL                      = 2,
    FEL_ORC                        = 3,
    BORELA_MIR                     = 0,
    BELAN_SHI                      = 1,
    AVRAL_SHI                      = 2,
    ALENA_MIR                      = 3,
    DRAKAS_FURY                    = 0,
    HELLSCREAMS_FURY               = 1,
    AGGRAS_FURY                    = 2,
    DUROTANS_FURY                  = 3,
    BILLY                          = 0,
    WILLIAM                        = 1,
    GILES                          = 2,
    TOBIAS                         = 3,
    TELDRASSIL                     = 0,
    WELL_OF_ETERNITY               = 1,
    NORDRASSIL                     = 2,
    MOONWELLS                      = 3,
    CORPRETHAR                     = 0,
    ALDURTHAR                      = 1,
    ANGRATHAR                      = 2,
    MORDRETHAR                     = 3,
    ASHANOR                        = 0,
    MISTSILVER                     = 1,
    FEONIR                         = 2,
    MIRADOR                        = 3,
    WHITECLAW_CLAN                 = 0,
    WARSONG_CLAN                   = 1,
    FROSTWOLF_CLAN                 = 2,
    ICEFANG_CLAN                   = 3,
    SCARLET_FEVER                  = 0,
    BLOOD_POX                      = 1,
    CRIMSON_FEVER                  = 2,
    RED_POX                        = 3,
    GALAKROND                      = 0,
    VERANUS                        = 1,
    ZEPTEK                         = 2,
    VYLETONGUE                     = 3,
    PEROTHARN                      = 0,
    GARAXXAS                       = 1,
    XAVIUS                         = 2,
    VYLETONGUE_2                   = 3,

    GOSSIP_INCORRECT               = 22901,
    GOSSIP_CORRECT                 = 22902,

    NPC_A_TIMELESS_QUESTION_CREDIT = 73570,
};

// Senior Historian Evelyna - 73570
class npc_senior_historian_evelyna : public CreatureScript
{
    public:
        npc_senior_historian_evelyna() : CreatureScript("npc_senior_historian_evelyna") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            player->PlayerTalkClass->ClearMenus();
            player->PrepareQuestMenu(creature->GetGUID());

            if (player->GetQuestStatus(33211) == QUEST_STATUS_INCOMPLETE)
            {
                switch (urand(1, 36)) // select random question
                {
                    case 1:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_01, GORLOCS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_01, MURGHOULS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_01, WOLVAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_01, MURLICHES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_01, creature->GetGUID());
                        break;
                    case 2:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_02, DEFECTIVE_ELEKK_TURD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_02, CRYSTAL_DEATH_TRAP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_02, WORTHLESS_ELEKK_DUNG, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_02, RADIOACTIVE_BIOHAZARD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_02, creature->GetGUID());
                        break;
                    case 3:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_03, GELBIN_MEKKATORQUE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_03, MILLHOUSE_MANASTORM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_03, SICCO_THERMAPLUGG, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_03, FIZZCRANK_FULLTHROTTLE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_03, creature->GetGUID());
                        break;
                    case 4:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_04, CRIMSON_RING, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_04, EMERALD_CIRCLE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_04, EARTHEN_RING, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_04, CENARION_CIRCLE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_04, creature->GetGUID());
                        break;
                    case 5:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_05, CHENETA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_05, KRISTINE_DENNY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_05, TATAI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_05, DELA_RUNETOTEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_05, creature->GetGUID());
                        break;
                    case 6:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_06, GOREHOOF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_06, TOOTHGNASHER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_06, BLOODHORN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_06, STEELMAULER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_06, creature->GetGUID());
                        break;
                    case 7:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_07, RED_DRAGONFLIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_07, BLUE_DRAGONFLIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_07, GREEN_DRAGONFLIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_07, BRONZE_DRAGONFLIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_07, creature->GetGUID());
                        break;
                    case 8:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_08, ACHERUS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_08, NAXXANAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_08, KOLRAMAS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_08, NAXXRAMAS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_08, creature->GetGUID());
                        break;
                    case 9:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_09, MAAKA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_09, GRIMATH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_09, TALAK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_09, RAWIRI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_09, creature->GetGUID());
                        break;
                    case 10:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_10, SHAMAN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_10, STORMCALLER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_10, ARCHDRUID, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_10, FAR_SEER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_10, creature->GetGUID());
                        break;
                    case 11:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_11, SNAKECOIL_CAVERN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_11, SERPENTINE_BASIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_11, COILFANG_RESERVOIR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_11, SPIRALFANG_CISTERN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_11, creature->GetGUID());
                        break;
                    case 12:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_12, EMMA_HARRINGTON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_12, CALISSA_HARRINGTON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_12, VANESSA_WHITEHALL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_12, KATRINA_WHITEHALL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_12, creature->GetGUID());
                        break;
                    case 13:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_13, SHARP_CLAW, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_13, RAZOR_TOOTH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_13, SHARP_TOOTH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_13, RAZOR_CLAW, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_13, creature->GetGUID());
                        break;
                    case 14:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_14, QUEEN_LIA_GREYMANE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_14, QUEEN_MALIA_GREYMANE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_14, QUEEN_LIRIA_GREYMANE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_14, QUEEN_MIA_GREYMANE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_14, creature->GetGUID());
                        break;
                    case 15:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_15, TIFFIN_WINDEMERE_WRYNN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_15, THERESE_ANNE_WRYNN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_15, THERESE_ANGHARAD_WRYNN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_15, TIFFIN_ELLERIAN_WRYNN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_15, creature->GetGUID());
                        break;
                    case 16:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_16, TENEBRON_VESPERON_AND_SHADRON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_16, TENEBRON_VESPERON_AND_HALION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_16, THERALION_SHADRON_AND_ABYSSION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_16, THERALION_HALION_AND_ABYSSION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_16, creature->GetGUID());
                        break;
                    case 17:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_17, AMANTHUL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_17, NORGANNON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_17, EONAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_17, KHAZGOROTH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_17, creature->GetGUID());
                        break;
                    case 18:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_18, VELEN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_18, AKAMA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_18, MARAAD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_18, NOBUNDO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_18, creature->GetGUID());
                        break;
                    case 19:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_19, KARESH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_19, XARODI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_19, KHURAL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_19, XOROTH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_19, creature->GetGUID());
                        break;
                    case 20:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_20, EREDRUIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_20, SAYAAD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_20, SHIVARRA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_20, EREDAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_20, creature->GetGUID());
                        break;
                    case 21:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_21, TERON_GOREFIEND, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_21, ARTHAS_MENETHIL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_21, ALEXANDROS_MOGRAINE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_21, KOLTIRA_DEATHWEAVER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_21, creature->GetGUID());
                        break;
                    case 22:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_22, FELLARI_SWIFTARROW, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_22, HOLIA_SUNSHIELD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_22, YANA_BLOODSPEAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_22, VALEA_TWINBLADES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_22, creature->GetGUID());
                        break;
                    case 23:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_23, SWIFTWING, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_23, SHARPBEAK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_23, SKYREE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_23, STORMBEAK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_23, creature->GetGUID());
                        break;
                    case 24:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_24, SHIRVALLAH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_24, KIMBUL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_24, MUEHZALA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_24, SHADRA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_24, creature->GetGUID());
                        break;
                    case 25:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_25, PRINCE_ARTHAS_MENETHIL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_25, KING_LLANE_WRYNN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_25, UTHER_THE_LIGHTBRINGER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_25, KING_TERENAS_MENETHIL_II, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_25, creature->GetGUID());
                        break;
                    case 26:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_26, MAGHAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_26, FELBLOOD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_26, MOKNATHAL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_26, FEL_ORC, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_26, creature->GetGUID());
                        break;
                    case 27:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_27, BORELA_MIR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_27, BELAN_SHI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_27, AVRAL_SHI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_27, ALENA_MIR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_27, creature->GetGUID());
                        break;
                    case 28:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_28, DRAKAS_FURY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_28, HELLSCREAMS_FURY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_28, AGGRAS_FURY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_28, DUROTANS_FURY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_28, creature->GetGUID());
                        break;
                    case 29:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_29, BILLY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_29, WILLIAM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_29, GILES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_29, TOBIAS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_29, creature->GetGUID());
                        break;
                    case 30:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_30, TELDRASSIL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_30, WELL_OF_ETERNITY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_30, NORDRASSIL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_30, MOONWELLS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_30, creature->GetGUID());
                        break;
                    case 31:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_31, CORPRETHAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_31, ALDURTHAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_31, ANGRATHAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_31, MORDRETHAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_31, creature->GetGUID());
                        break;
                    case 32:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_32, ASHANOR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_32, MISTSILVER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_32, FEONIR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_32, MIRADOR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_32, creature->GetGUID());
                        break;
                    case 33:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_33, WHITECLAW_CLAN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_33, WARSONG_CLAN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_33, FROSTWOLF_CLAN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_33, ICEFANG_CLAN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_33, creature->GetGUID());
                        break;
                    case 34:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_34, SCARLET_FEVER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_34, BLOOD_POX, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_34, CRIMSON_FEVER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_34, RED_POX, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_34, creature->GetGUID());
                        break;
                    case 35:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_35, GALAKROND, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_35, VERANUS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_35, ZEPTEK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_35, VYLETONGUE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_35, creature->GetGUID());
                        break;
                    case 36:
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_36, PEROTHARN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_36, GARAXXAS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_36, XAVIUS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_QUESTION_36, VYLETONGUE_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXT_QUESTION_36, creature->GetGUID());
                        break;
                }
            }
            else
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1) // false
                player->SEND_GOSSIP_MENU(GOSSIP_INCORRECT, creature->GetGUID());
            else // true
            {
                player->KilledMonsterCredit(NPC_A_TIMELESS_QUESTION_CREDIT);
                player->SEND_GOSSIP_MENU(GOSSIP_CORRECT, creature->GetGUID());
            }

            return true;
        }
};

// Highwind Albatross - 73531
struct npc_highwind_albatross : public ScriptedAI
{
    npc_highwind_albatross(Creature* creature) : ScriptedAI(creature) { }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (attacker->GetTypeId() != TYPEID_PLAYER)
            return;

        if (damage > me->GetHealth())
            return;

        if (me->GetVehicleKit() && me->GetVehicleKit()->HasEmptySeat(0) && !attacker->IsOnVehicle())
        {
            attacker->EnterVehicle(me, 0);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
            Position pos = me->GetPosition();
            if (me->GetPositionZ() > 65.0f)
                pos.m_positionZ = 240.0f;
            else
                pos.m_positionZ = 80.0f;
            me->GetMotionMaster()->MovePoint(1, pos);
            me->m_Events.Schedule(3000, [this]() { me->GetMotionMaster()->MovePath(me->GetEntry(), true); });
        }
    }

    void PassengerBoarded(Unit* unit, int8 /*seat*/, bool apply) override
    {
        if (!apply)
            me->DespawnOrUnsummon();
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type == POINT_MOTION_TYPE && me->GetVehicleKit() && me->GetVehicleKit()->HasEmptySeat(0))
            me->DespawnOrUnsummon();
    }

    void UpdateAI(uint32 /*diff*/) override { }
    void Reset() override { }
    void JustEngagedWith(Unit* /*who*/) override { }
};

Position karkanosPos = { -1130.133f, - 4900.529f, - 1.579f, 0.0f };

class npc_fin_longpaw : public CreatureScript
{
    public:
        npc_fin_longpaw() : CreatureScript("npc_fin_longpaw") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            player->PlayerTalkClass->ClearMenus();
            if (!creature->FindNearestCreature(RARE_KARKANOS, 50.0f, true))
                player->ADD_GOSSIP_ITEM_DB(60013, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();
            creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                creature->AI()->Talk(0);
                creature->m_Events.Schedule(5000, [creature]()
                {
                    creature->HandleEmoteStateCommand(0);
                    creature->AI()->Talk(1);
                    creature->SummonCreature(RARE_KARKANOS, karkanosPos);
                    creature->DespawnOrUnsummon(5000);
                });
            }

            return true;
        }

        struct npc_fin_longpawAI : public ScriptedAI
        {
            npc_fin_longpawAI(Creature* creature) : ScriptedAI(creature) { }

            void UpdateAI(uint32 /*diff*/) override { }

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            void JustEngagedWith(Unit* /*who*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_fin_longpawAI(creature);
        }
};

// Zarhym - 71876
class npc_zarhym_1 : public CreatureScript
{
    public:
        npc_zarhym_1() : CreatureScript("npc_zarhym_1") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (!player->IsDailyQuestDone(QUEST_ZARHYM_TRACKER))
            player->ADD_GOSSIP_ITEM_DB(60014, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                player->CastSpell(player, 144145);
                creature->DespawnOrUnsummon();
                if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_ZARHYM_TRACKER))
                    player->RewardQuest(quest, 0, player, false);
            }

            return true;
        }

        struct npc_zarhym_1AI : public ScriptedAI
        {
            npc_zarhym_1AI(Creature* creature) : ScriptedAI(creature) { }

            bool intro;

            void UpdateAI(uint32 /*diff*/) override { }
            void JustEngagedWith(Unit* /*who*/) override { }

            void Reset() override
            {
                intro = false;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (intro)
                    return;
                if (who->GetTypeId() != TYPEID_PLAYER)
                    return;
                if (!me->IsWithinDistInMap(who, 15.0f, false))
                    return;

                Talk(0);
                intro = true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zarhym_1AI(creature);
        }
};

enum ZarhymEvents
{
    EVENT_OOO_SAY = 1,
};

// Zarhym - 71869
class npc_zarhym_2 : public CreatureScript
{
    public:
        npc_zarhym_2() : CreatureScript("npc_zarhym_2") { }

        struct npc_zarhym_2AI : public ScriptedAI
        {
            npc_zarhym_2AI(Creature* creature) : ScriptedAI(creature)
            {
                talked = false;
            }

            EventMap events;
            bool talked;

            void UpdateAI(uint32 diff) override
            {
                if (!talked)
                {
                    Talk(0);
                    talked = true;
                }

                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_OOO_SAY:
                    {
                        Talk(1);
                        events.ScheduleEvent(EVENT_OOO_SAY, 25 * IN_MILLISECONDS);
                        break;
                    }
                }
            }

            void Reset() override
            {
                events.Reset();

                if (auto summ = me->ToTempSummon())
                    if (auto owner = summ->GetSummoner())
                        me->GetMotionMaster()->MoveFollow(owner, 3.0f, me->GetFollowAngle());

                events.ScheduleEvent(EVENT_OOO_SAY, 10 * IN_MILLISECONDS);
            }

            void JustEngagedWith(Unit* /*who*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zarhym_2AI(creature);
        }
};

enum DresShipEnums
{
    EVENT_UPDATE_AURAS   = 1,
    EVENT_CANNON_BARRAGE = 2,
    EVENT_DARK_BARRAGE   = 3,
    EVENT_CHECK_PLAYERS  = 4,
    EVENT_BURNING_PITCH  = 5,

    NPC_PIRATE           = 73613,
    NPC_CAPTAIN          = 73547,
    NPC_FOG              = 73557,

    SPELL_CANNON_BARRAGE = 148159,
    SPELL_DARK_BARRAGE   = 148168,
    SPELL_SKYBOX         = 148137,
    SPELL_EERIE_FOG      = 148110,
    SEPLL_BURNING_PITCH  = 148181,
};

// Dread Ship Vazuvius - 73281
struct npc_dread_ship_vazuvius : public ScriptedAI
{
    npc_dread_ship_vazuvius(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        playersAura.clear();
        events.Reset();
        DespawnSummons();
        ApplyAuras();

        // intro
        Position pos{ -132.153f, -4709.59f, -1.270847f, 4.138892f };
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
        if (me->GetPositionZ() < -5.0f)
        {
            me->SummonCreature(NPC_FOG, pos);
            me->GetMotionMaster()->MovePoint(0, pos);
            me->m_Events.Schedule(10000, [this, pos]()
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->SetHomePosition(pos);
            });
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* captain = me->FindNearestCreature(NPC_CAPTAIN, 50.0f, true))
            captain->AI()->Talk(5);

        DespawnSummons();
        RemoveAuras();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetTypeId() != TYPEID_PLAYER)
            return;

        if (me->IsWithinDist(who, 7.0f))
            me->CastSpell(who, 149204);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_CANNON_BARRAGE, 2 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_DARK_BARRAGE, 7 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_BURNING_PITCH, 5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_UPDATE_AURAS, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CHECK_PLAYERS, 20 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (me->GetThreatManager().getThreatList().empty())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_CANNON_BARRAGE:
            {
                me->CastSpell(GetTarget(), SPELL_CANNON_BARRAGE);
                events.ScheduleEvent(EVENT_CANNON_BARRAGE, 10 * IN_MILLISECONDS);
                break;
            }
            case EVENT_DARK_BARRAGE:
            {
                me->CastSpell(GetTarget(), SPELL_DARK_BARRAGE);
                events.ScheduleEvent(EVENT_DARK_BARRAGE, 5 * IN_MILLISECONDS);
                break;
            }
            case EVENT_BURNING_PITCH:
            {
                if (Creature* pirate = me->FindNearestCreature(NPC_PIRATE, 50.0f, true))
                    pirate->CastSpell(GetTarget(), SEPLL_BURNING_PITCH);
                events.ScheduleEvent(EVENT_BURNING_PITCH, 7 * IN_MILLISECONDS);
                break;
            }
            case EVENT_UPDATE_AURAS:
            {
                ApplyAuras();
                events.ScheduleEvent(EVENT_UPDATE_AURAS, 5 * IN_MILLISECONDS);
                break;
            }
            case EVENT_CHECK_PLAYERS:
            {
                if (!me->FindNearestPlayer(150.0f))
                    me->DespawnOrUnsummon();
                events.ScheduleEvent(EVENT_CHECK_PLAYERS, 20 * IN_MILLISECONDS);
                break;
            }
        }
    }

private:
    std::list<ObjectGuid> playersAura;
    Unit* GetTarget()
    {
        std::list<HostileReference*> threatList = me->GetThreatManager().getThreatList();
        HostileReference* ref = Trinity::Containers::SelectRandomContainerElement(threatList);
        Unit* target = Unit::GetUnit(*me, ref->getUnitGuid());
        return target;
    }
    void DespawnSummons()
    {
        std::list<Creature*> summons;
        me->GetCreatureListWithEntryInGrid(summons, NPC_PIRATE, 150.0f);
        me->GetCreatureListWithEntryInGrid(summons, NPC_CAPTAIN, 150.0f);
        me->GetCreatureListWithEntryInGrid(summons, NPC_FOG, 150.0f);
        for (auto&& summon : summons)
            summon->DespawnOrUnsummon();
    }
    void ApplyAuras()
    {
        std::list<Player*> players;
        me->GetPlayerListInGrid(players, 150.0f);
        for (auto&& player : players)
        {
            if (player->GetAreaId() == 6835 || player->GetAreaId() == 6832)
            {
                if (!player->HasAura(SPELL_EERIE_FOG))
                    player->CastSpell(player, SPELL_EERIE_FOG);
                if (!player->HasAura(SPELL_SKYBOX))
                    player->CastSpell(player, SPELL_SKYBOX);
                playersAura.push_back(player->GetGUID());
            }
        }
    }
    void RemoveAuras()
    {
        for (auto&& guid : playersAura)
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, guid))
            {
                player->RemoveAura(SPELL_EERIE_FOG);
                player->RemoveAura(SPELL_SKYBOX);
            }
        }
    }
};

enum EvermawEnum
{
    SPELL_DEVOUR_EFF    = 147961,
    SPELL_DEVOUR        = 147948,
    SPELL_TIME_LOCK_AT  = 147841,
    SPELL_TIME_LOCK_EFF = 147813,
    SPELL_TIME_RIFT_AT  = 147812,
    SPELL_TIME_RIFT_EFF = 147840,
};

// Evermaw - 73279
struct npc_evermaw : public ScriptedAI
{
    npc_evermaw(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        me->UpdateSpeed(MOVE_RUN, true);

        DoCast(me, SPELL_DEVOUR);
        me->GetMotionMaster()->MovePath(me->GetDBTableGUIDLow(), true);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        // Evade Check
        scheduler
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            for (auto&& itr : me->GetThreatManager().getThreatList())
            {
                if (Unit* target = ObjectAccessor::GetUnit(*me, itr->getUnitGuid()))
                {
                    if (target->IsAlive() && me->GetExactDist2d(target) < 100.0f)
                    {
                        context.Repeat(Seconds(1));
                        return;
                    }
                }
            }

            EnterEvadeMode();
        });

        // Spawn Portals
        scheduler
            .Schedule(Seconds(3), [this](TaskContext context)
        {
            for (uint8 i = 0; i < 3; i++)
                DoCast(me, SPELL_TIME_LOCK_AT);

            for (uint8 i = 0; i < 2; i++)
                DoCast(me, SPELL_TIME_RIFT_AT);

            context.Repeat(Seconds(5));
        });
    }

    void EnterEvadeMode() override
    {
        me->CombatStop();
        scheduler.CancelAll(); // not evade to home pos
    }

    void JustDied(Unit* /*killer*/) override
    {
        scheduler.CancelAll();
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

class go_conspicuously_empty_shell : public GameObjectScript
{
    public:
        go_conspicuously_empty_shell() : GameObjectScript("go_conspicuously_empty_shell") { }

        bool OnReportUse(Player* player, GameObject* go) override
        {
            if (!go->FindNearestCreature(RARE_CHELON, 50.0f, true))
                if (Creature* creat = go->SummonCreature(RARE_CHELON, go->GetPosition()))
                    creat->Attack(player, false);

            go->ForcedDespawn();
            return true;
        }
};

class go_challengers_stone : public GameObjectScript
{
    public:
        go_challengers_stone() : GameObjectScript("go_challengers_stone") { }

        void OnLootStateChanged(GameObject* go, uint32 state, Unit* unit) override
        {
            if (!unit || state != GO_ACTIVATED)
                return;

            if (!go->FindNearestCreature(RARE_ARCHIEREUS_OF_FLAME_CLOAK, 50.0f, true))
                if (Creature* creat = go->SummonCreature(RARE_ARCHIEREUS_OF_FLAME_CLOAK, { -267.976f, -4909.930f, 15.944f, 3.20498f }, TEMPSUMMON_DEAD_DESPAWN))
                    creat->Attack(unit, false);
        }
};

class go_crane_nest : public GameObjectScript
{
    public:
        go_crane_nest() : GameObjectScript("go_crane_nest") { }

        void OnLootStateChanged(GameObject* go, uint32 state, Unit* unit) override
        {
            if (!unit || state != GO_ACTIVATED)
                return;

            std::list<Creature*> creatures;
            go->GetCreatureListWithEntryInGrid(creatures, 71143, 20.0f);
            go->GetCreatureListWithEntryInGrid(creatures, 72762, 20.0f);
            go->GetCreatureListWithEntryInGrid(creatures, 72761, 20.0f);
            for (auto&& creature : creatures)
                creature->Attack(unit, false);

            go->m_Events.KillEventsByGroup(1);
            go->m_Events.Schedule(MINUTE * IN_MILLISECONDS, 1, [go]
            {
                if (go->getLootState() == GO_ACTIVATED)
                    go->SetLootState(GO_JUST_DEACTIVATED);
            });
        }
};

class go_zarhyms_body : public GameObjectScript
{
    public:
        go_zarhyms_body() : GameObjectScript("go_zarhyms_body") { }

        void OnLootStateChanged(GameObject* go, uint32 state, Unit* unit) override
        {
            if (!unit || state != GO_ACTIVATED || unit->GetTypeId() != TYPEID_PLAYER)
                return;

            auto group = unit->ToPlayer()->GetGroup();

            if (group)
            {
                for (auto&& itr : group->GetMemberSlots())
                {
                    Player* member = ObjectAccessor::GetPlayer(*unit, itr.guid);
                    if (member && member->GetDistance(unit->GetPosition()) < 30.0f)
                        member->CastSpell(unit, 147081);
                }
            }
            else
                unit->CastSpell(unit, 147081);
        }
};

class go_cursed_grave : public GameObjectScript
{
    public:
        go_cursed_grave() : GameObjectScript("go_cursed_grave") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (!player->HasItemCount(104115, 1, true))
            {
                player->SendGameError(GameError::ERR_USE_LOCKED);
                return false;
            }

            if (go->FindNearestCreature(73281, 150.0f) || go->FindNearestCreature(NPC_CAPTAIN, 150.0f))
            {
                player->SendGameError(GameError::ERR_USE_LOCKED);
                return false;
            }

            player->DestroyItemCount(104115, 1, true);
            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);

            if (Creature* captain = go->SummonCreature(NPC_CAPTAIN, -213.352f, -4715.245f, 13.070f, 4.343475f))
            {
                captain->SetFaction(35);
                captain->AI()->Talk(0);
                captain->m_Events.Schedule(5000,  [captain]() { captain->AI()->Talk(1); });
                captain->m_Events.Schedule(10000, [captain]() { captain->AI()->Talk(2); });
                captain->m_Events.Schedule(15000, [captain]() { captain->AI()->Talk(3); });
                captain->m_Events.Schedule(20000, [captain]()
                {
                    captain->AI()->Talk(4);
                    captain->DespawnOrUnsummon(1000);
                    captain->SummonCreature(73281, -131.9149f, -4710.043f, -37.28537f, 3.77507f);
                });
            }

            go->m_Events.Schedule(3600000, [go]()
            {
                go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
            });
            return true;
        }
};

class go_timeless_chest : public GameObjectScript
{
    public:
        go_timeless_chest() : GameObjectScript("go_timeless_chest") { }

        void OnLootStateChanged(GameObject* go, uint32 state, Unit* unit) override
        {
            if (!unit || state != GO_ACTIVATED || unit->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = unit->ToPlayer();

            // kill credit for quest 33333
            if (go->GetEntry() != 222684 && go->GetEntry() != 222685 && go->GetEntry() != 222687 && go->GetEntry() != 222688 && go->GetEntry() != 223193 && go->GetEntry() != 222689 && go->GetEntry() != 221764 && go->GetEntry() != 222686)
                player->KilledMonsterCredit(73982);

            if (go->GetEntry() == GO_MIST_COVERED_TREASURE_CHEST)
            {
                if (!player->HasAura(144385))
                {
                    player->SendGameError(GameError::ERR_LOOT_CANT_LOOT_THAT);
                    go->ForcedDespawn();
                }
                return;
            }

            // check possibility to reward loot
            auto itr = timelessRareChestsMap.find(go->GetEntry());
            if (itr != timelessRareChestsMap.end())
            {
                if (player->GetQuestStatus(itr->second) != QUEST_STATUS_REWARDED && !player->IsWeeklyQuestDone(itr->second))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(itr->second))
                        player->RewardQuest(quest, 0, player, false);
                    if (GameObjectTemplate const* objectInfo = sObjectMgr->GetGameObjectTemplate(itr->first))
                        if (objectInfo->GetLockId() == 0)
                            player->SendLoot(go->GetGUID(), LOOT_CORPSE);
                }
                else
                {
                    player->SendGameError(GameError::ERR_LOOT_CANT_LOOT_THAT);
                    go->ForcedDespawn();
                    return;
                }
            }

            // start despawn timer to prevent loot stucking
            go->m_Events.KillEventsByGroup(1);
            go->m_Events.Schedule(MINUTE * IN_MILLISECONDS, 1, [go]
            {
                if (go->getLootState() == GO_ACTIVATED)
                    go->SetLootState(GO_JUST_DEACTIVATED);
            });
        }
};

class go_neverending_spritewood : public GameObjectScript
{
    public:
        go_neverending_spritewood() : GameObjectScript("go_neverending_spritewood") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_NEVERENDING_SPRITEWOOD))
                player->RewardQuest(quest, 0, player, false);
            player->CastSpell(player, 144052);
            go->ForcedDespawn();
            return true;
        }
};

class spell_timeless_isle_crane_wings : public SpellScriptLoader
{
    public:
        spell_timeless_isle_crane_wings() : SpellScriptLoader("spell_timeless_isle_crane_wings") { }

        class spell_timeless_isle_crane_wings_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_timeless_isle_crane_wings_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                     caster->CastSpell(caster, 144391, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    caster->CastSpell(caster, 148162, true);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_timeless_isle_crane_wings_AuraScript::OnApply, EFFECT_0, SPELL_AURA_FEATHER_FALL, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_timeless_isle_crane_wings_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_timeless_isle_crane_wings_AuraScript();
        }
};

class spell_timeless_isle_cauterize : public SpellScriptLoader
{
    public:
        spell_timeless_isle_cauterize() : SpellScriptLoader("spell_timeless_isle_cauterize") { }

        class spell_timeless_isle_cauterize_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_timeless_isle_cauterize_AuraScript);

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                GetCaster()->SetMaxHealth(GetCaster()->GetHealthPct() - 1.0f);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_timeless_isle_cauterize_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_timeless_isle_cauterize_AuraScript();
        }
};

class spell_timeless_isle_burning_fury : public SpellScriptLoader
{
    public:
        spell_timeless_isle_burning_fury() : SpellScriptLoader("spell_timeless_isle_burning_fury") { }

        class spell_timeless_isle_burning_fury_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_timeless_isle_burning_fury_AuraScript);

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                GetCaster()->DealDamage(GetCaster(), 50000, NULL, SELF_DAMAGE, SPELL_SCHOOL_MASK_FIRE);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_timeless_isle_burning_fury_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_timeless_isle_burning_fury_AuraScript();
        }
};

class spell_spirit_world : public SpellScriptLoader
{
    public:
        spell_spirit_world() : SpellScriptLoader("spell_spirit_world") { }

        class spell_spirit_world_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_spirit_world_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (WorldObject* owner = GetOwner())
                    owner->SetPhaseMask(2, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (WorldObject* owner = GetOwner())
                {
                    owner->SetPhaseMask(1, true);

                    if (Player* player = owner->ToPlayer())
                    {
                        auto summons = player->GetSummons();
                        for (auto&& summ : summons)
                        {
                            if (summ->GetEntry() == 71869)
                                summ->DespawnOrUnsummon();
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_spirit_world_AuraScript::OnApply, EFFECT_0, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_spirit_world_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_spirit_world_AuraScript();
        }
};

std::map<uint32, std::vector<uint32>> timelessLoot =
{
    { 146259, { 101838, 101830 } },
    { 146265, { 101843, 101835 } },
    { 146243, { 101799, 101861, 101826 } },
    { 146267, { 101839, 101831 } },
    { 146268, { 101869, 101788 } },
    { 146277, { 101807, 101816 } },
    { 146263, { 101840, 101832 } },
    { 146270, { 101867, 101786 } },
    { 146239, { 101791, 101853, 101820 } },
    { 146283, { 101810, 101818 } },
    { 146271, { 101865, 101784 } },
    { 146264, { 101842, 101834 } },
    { 146272, { 101866, 101785 } },
    { 146237, { 101797, 101859, 101825 } },
    { 146275, { 101862, 101781 } },
    { 146279, { 101803, 101813 } },
    { 146261, { 101845, 101837 } },
    { 146236, { 101792, 101854, 101821 } },
    { 148740, { 101805, 101795, 101857, 101849, 101827 } },
    { 148746, { 103689, 103688, 103686, 103690, 103687 } },
    { 146246, { 101800, 101790, 101852, 101848, 101828 } },
    { 146274, { 101868, 101787 } },
    { 146269, { 101863, 101782 } },
    { 146260, { 101841, 101833 } },
    { 146280, { 101802, 101812 } },
    { 146266, { 101844, 101836 } },
    { 146273, { 101864, 101783 } },
    { 146278, { 101806, 101815 } },
    { 146282, { 101808, 101817 } },
    { 146281, { 101804, 101814 } },
    { 146276, { 101801, 101811 } },
    { 146240, { 101793, 101855, 101822 } },
    { 146241, { 101794, 101856, 101823 } },
    { 146238, { 101789, 101851, 101819 } },
    { 146242, { 101796, 101858, 101824 } },
    { 146244, { 101809, 101798, 101860, 101850, 101829 } },
};

class spell_create_timeless_item : public SpellScript
{
    PrepareSpellScript(spell_create_timeless_item);

    void HandleCreateItem(SpellEffIndex effIndex)
    {
        PreventHitEffect(effIndex);

        if (Player* player = GetCaster()->ToPlayer())
        {
            auto itr = timelessLoot.find(GetSpellInfo()->Id);
            if (itr != timelessLoot.end())
            {
                bool itemGived = false;
                uint32 entry = 0;
                for (auto&& itemId : itr->second)
                {
                    auto itemSpec = sObjectMgr->GetItemSpecInfo(itemId, 90);
                    if (!itemSpec)
                        continue;

                    uint32 spec = player->GetLootSpecialization() ? player->GetLootSpecialization() : player->GetTalentSpecialization(player->GetActiveSpec());
                    if (itemSpec->HasSpecializationId(spec))
                    {
                        player->AddItem(itemId, 1);
                        itemGived = true;
                        entry = itemId;
                        break;
                    }
                }

                if (!itemGived)
                {
                    entry = Trinity::Containers::SelectRandomContainerElement(itr->second);
                    player->AddItem(entry, 1);
                }

                if (Guild* guild = player->GetGuild())
                    guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CRAFT_ITEMS_GUILD, entry, 1, 0, nullptr, player);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_create_timeless_item::HandleCreateItem, EFFECT_0, SPELL_EFFECT_CREATE_ITEM_2);
    }
};

std::map<uint32, std::vector<uint32>> timelessCacheLoot =
{
    { 147597, { 101789, 101791, 101792, 101793, 101794, 101796, 101797, 101799, 101819, 101820, 101821, 101822, 101823, 101824, 101825, 101826, 101851, 101853, 101854, 101855, 101856, 101858, 101859, 101861 } },
    { 148099, { 101830, 101831, 101832, 101833, 101834, 101835, 101836, 101837, 101838, 101839, 101840, 101841, 101842, 101843, 101844, 101845 } },
    { 148103, { 101781, 101782, 101783, 101784, 101785, 101786, 101787, 101788, 101862, 101863, 101864, 101865, 101866, 101867, 101868, 101869 } },
    { 148104, { 101801, 101802, 101803, 101804, 101806, 101807, 101808, 101810, 101811, 101812, 101813, 101814, 101815, 101816, 101817, 101818 } },
};

class spell_create_timeless_item_from_cache : public SpellScript
{
    PrepareSpellScript(spell_create_timeless_item_from_cache);

    void HandleCreateItem(SpellEffIndex effIndex)
    {
        PreventHitEffect(effIndex);

        if (Player* player = GetCaster()->ToPlayer())
        {
            std::list<uint32> itemsForSpec;
            auto itr = timelessCacheLoot.find(GetSpellInfo()->Id);
            if (itr != timelessCacheLoot.end())
            {
                for (auto&& itemId : itr->second)
                {
                    auto itemSpec = sObjectMgr->GetItemSpecInfo(itemId, 90);
                    if (!itemSpec)
                        continue;

                    uint32 spec = player->GetLootSpecialization() ? player->GetLootSpecialization() : player->GetTalentSpecialization(player->GetActiveSpec());
                    if (itemSpec->HasSpecializationId(spec))
                        itemsForSpec.push_back(itemId);
                }

                uint32 itemId = 0;
                if (!itemsForSpec.empty())
                    itemId = Trinity::Containers::SelectRandomContainerElement(itemsForSpec);
                else
                    itemId = Trinity::Containers::SelectRandomContainerElement(itr->second);

                player->AddItem(itemId, 1);

                if (Guild* guild = player->GetGuild())
                    guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CRAFT_ITEMS_GUILD, itemId, 1, 0, nullptr, player);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_create_timeless_item_from_cache::HandleCreateItem, EFFECT_0, SPELL_EFFECT_CREATE_ITEM_2);
    }
};

// 148385 - Censer of Eternal Agony
// 148429 - Fire-Watcher's Oath
class spell_bloody_coin_proc : public AuraScript
{
    PrepareAuraScript(spell_bloody_coin_proc);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetProcTarget())
            return false;

        if (eventInfo.GetActionTarget()->HasAura(149624))
            return false;

        return eventInfo.GetProcTarget()->GetTypeId() == TYPEID_PLAYER;
    }

    void Register()
    {
        DoCheckProc += AuraCheckProcFn(spell_bloody_coin_proc::CheckProc);
    }
};

// 148385 - Censer of Eternal Agony
class spell_censer_of_eternal_agony : public AuraScript
{
    PrepareAuraScript(spell_censer_of_eternal_agony);

    bool Load() override
    {
        return GetOwner()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        GetOwner()->ToPlayer()->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_IGNORE_REPUTATION);
        GetOwner()->ToPlayer()->UpdatePvP(true);
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        GetOwner()->ToPlayer()->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_IGNORE_REPUTATION);
        GetOwner()->ToPlayer()->UpdatePvP(false);
    }

    void Register() override
    {
        OnEffectApply  += AuraEffectApplyFn(spell_censer_of_eternal_agony::HandleApply,  EFFECT_0, SPELL_AURA_MOD_FACTION, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_censer_of_eternal_agony::HandleRemove, EFFECT_0, SPELL_AURA_MOD_FACTION, AURA_EFFECT_HANDLE_REAL);
    }
};

enum BookProc
{
    SPELL_TIMELESS_AGILITY   = 147355,
    SPELL_TIMELESS_INTELLECT = 147357,
    SPELL_TIMELESS_STRENGHT  = 147359,
    SPELL_TIMELESS_STAMINA   = 147361,
    SPELL_BOOK_OF_THE_AGES   = 147226,
};

// 147226 - Book of the Ages
class spell_book_of_the_ages : public AuraScript
{
    PrepareAuraScript(spell_book_of_the_ages);

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        Player* player = GetOwner()->ToPlayer();
        if (!player)
            return;

        switch (player->GetTalentSpecialization(player->GetActiveSpec()))
        {
            case SPEC_WARRIOR_PROTECTION:
            case SPEC_PALADIN_PROTECTION:
            case SPEC_DEATH_KNIGHT_BLOOD:
                player->CastSpell(player, SPELL_TIMELESS_STAMINA, true);
                break;
            case SPEC_WARRIOR_ARMS:
            case SPEC_WARRIOR_FURY:
            case SPEC_PALADIN_RETRIBUTION:
            case SPEC_DEATH_KNIGHT_FROST:
            case SPEC_DEATH_KNIGHT_UNHOLY:
                player->CastSpell(player, SPELL_TIMELESS_STRENGHT, true);
                break;
            case SPEC_PALADIN_HOLY:
            case SPEC_PRIEST_DISCIPLINE:
            case SPEC_PRIEST_HOLY:
            case SPEC_PRIEST_SHADOW:
            case SPEC_SHAMAN_ELEMENTAL:
            case SPEC_SHAMAN_RESTORATION:
            case SPEC_MAGE_ARCANE:
            case SPEC_MAGE_FIRE:
            case SPEC_MAGE_FROST:
            case SPEC_WARLOCK_AFFLICTION:
            case SPEC_WARLOCK_DEMONOLOGY:
            case SPEC_WARLOCK_DESTRUCTION:
            case SPEC_DRUID_BALANCE:
            case SPEC_DRUID_RESTORATION:
            case SPEC_MONK_MISTWEAVER:
                player->CastSpell(player, SPELL_TIMELESS_INTELLECT, true);
                break;
            case SPEC_HUNTER_BEAST_MASTERY:
            case SPEC_HUNTER_MARKSMANSHIP:
            case SPEC_HUNTER_SURVIVAL:
            case SPEC_ROGUE_ASSASSINATION:
            case SPEC_ROGUE_COMBAT:
            case SPEC_ROGUE_SUBTLETY:
            case SPEC_SHAMAN_ENHANCEMENT:
            case SPEC_DRUID_FERAL:
            case SPEC_DRUID_GUARDIAN:
            case SPEC_MONK_BREWMASTER:
            case SPEC_MONK_WINDWALKER:
                player->CastSpell(player, SPELL_TIMELESS_AGILITY, true);
                break;
            default:
                player->CastSpell(player, SPELL_TIMELESS_STAMINA, true);
                break;
        }
    }

    void Register()
    {
        OnProc += AuraProcFn(spell_book_of_the_ages::HandleOnProc);
    }
};

std::map<uint32, std::vector<uint32>> embracedTimelessLoot =
{
    { 146259, { 101927, 101919 } },
    { 146265, { 101932, 101924 } },
    { 146243, { 101915, 101948, 101888 } },
    { 146267, { 101928, 101920 } },
    { 146268, { 101877, 101956 } },
    { 146277, { 101896, 101905 } },
    { 146263, { 101929, 101921 } },
    { 146270, { 101875, 101954 } },
    { 146239, { 101909, 101940, 101880 } },
    { 146283, { 101899, 101907 } },
    { 146271, { 101952, 101873 } },
    { 146264, { 101923, 101931 } },
    { 146272, { 101874, 101953 } },
    { 146237, { 101914, 101946, 101886 } },
    { 146275, { 101949, 101870 } },
    { 146279, { 101892, 101902 } },
    { 146261, { 101926, 101934 } },
    { 146236, { 101910, 101941, 101881 } },
    { 148740, { 101936, 101894, 101916, 101944, 101884 } },
    { 146246, { 101917, 101889, 101939, 101879, 101935 } },
    { 146274, { 101955, 101876 } },
    { 146269, { 101950, 101871 } },
    { 146260, { 101922, 101930 } },
    { 146280, { 101901, 101891 } },
    { 146266, { 101925, 101933 } },
    { 146273, { 101951, 101872 } },
    { 146278, { 101904, 101895 } },
    { 146282, { 101897, 101906 } },
    { 146281, { 101893, 101903 } },
    { 146276, { 101890, 101900 } },
    { 146240, { 101882, 101911, 101942 } },
    { 146241, { 101883, 101912, 101943 } },
    { 146238, { 101938, 101878, 101908 } },
    { 146242, { 101913, 101945, 101885 } },
    { 146244, { 101887, 101898, 101937, 101918, 101947 } },
};

class spell_eternitys_embrace : public SpellScript
{
    PrepareSpellScript(spell_eternitys_embrace);

    SpellCastResult CheckCast()
    {
        if (!GetExplTargetItem())
            return SPELL_FAILED_BAD_TARGETS;

        if (!GetExplTargetItem()->GetTemplate()->Spells[0].SpellId)
            return SPELL_FAILED_BAD_TARGETS;

        auto itr = embracedTimelessLoot.find(GetExplTargetItem()->GetTemplate()->Spells[0].SpellId);
        if (itr == embracedTimelessLoot.end())
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void HandleAfterCast()
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            auto itr = embracedTimelessLoot.find(GetExplTargetItem()->GetTemplate()->Spells[0].SpellId);
            if (itr != embracedTimelessLoot.end())
            {
                player->DestroyItemCount(GetExplTargetItem()->GetEntry(), 1, true);

                uint32 entry = 0;
                bool itemGived = false;
                for (auto&& itemId : itr->second)
                {
                    auto itemSpec = sObjectMgr->GetItemSpecInfo(itemId, 90);
                    if (!itemSpec)
                        continue;

                    uint32 spec = player->GetLootSpecialization() ? player->GetLootSpecialization() : player->GetTalentSpecialization(player->GetActiveSpec());
                    if (itemSpec->HasSpecializationId(spec))
                    {
                        player->AddItem(itemId, 1);
                        itemGived = true;
                        entry = itemId;
                        break;
                    }
                }

                if (!itemGived)
                {
                    entry = Trinity::Containers::SelectRandomContainerElement(itr->second);
                    player->AddItem(entry, 1);
                }

                if (Guild* guild = player->GetGuild())
                    guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CRAFT_ITEMS_GUILD, entry, 1, 0, nullptr, player);
            }
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_eternitys_embrace::CheckCast);
        AfterCast += SpellCastFn(spell_eternitys_embrace::HandleAfterCast);
    }
};

// Gulp Frog Toxin 147655
class spell_gulp_frog_toxin : public AuraScript
{
    PrepareAuraScript(spell_gulp_frog_toxin);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetStackAmount() > 9 && GetOwner() && GetOwner()->ToUnit())
            GetOwner()->ToUnit()->CastSpell(GetOwner()->ToUnit(), SPELL_INSTAKILL_TOXIN, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_gulp_frog_toxin::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAPPLY);
    }
};

// Defensive Leap 147650
class spell_cat_defensive_leap : public SpellScript
{
    PrepareSpellScript(spell_cat_defensive_leap);

    void HandleEffectHitTarget(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(0);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_cat_defensive_leap::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_KNOCK_BACK);
    }
};

// Pounce 147649
class spell_timelesscat_pounce : public SpellScript
{
    PrepareSpellScript(spell_timelesscat_pounce);

    void HandleEffectHitTarget(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_POUNCE_STUN, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_timelesscat_pounce::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_JUMP);
    }
};

// Time Rift 147812, Time Lock 147841
class spell_evermaw_time_portals : public SpellScript
{
    PrepareSpellScript(spell_evermaw_time_portals);

    void SelectTargets(SpellDestination& dest)
    {
        // always in water behind caster in cone
        float dist = frand(10.0f, 30.0f);
        float angle = Position::NormalizeOrientation(GetCaster()->GetOrientation() - M_PI + frand(-M_PI / 2, M_PI / 2));
        x = GetCaster()->GetPositionX() + dist * cos(angle);
        y = GetCaster()->GetPositionY() + dist * sin(angle);

        Position newPos = { x, y, GetCaster()->GetPositionZ() - 10.0f + frand(-4.5f, 4.5f), 0 };
        dest.Relocate(newPos);
    }

    private:
        float x, y;

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_evermaw_time_portals::SelectTargets, EFFECT_0, TARGET_DEST_CASTER);
    }
};

class spell_crane_wings : public AuraScript
{
    PrepareAuraScript(spell_crane_wings);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_MIST_COVERED_TREASURE_CHEST))
                caster->RewardQuest(quest, 0, caster, false);

            caster->CastSpell(caster, 148162);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_crane_wings::OnRemove, EFFECT_0, SPELL_AURA_FEATHER_FALL, AURA_EFFECT_HANDLE_REAL);
    }
};

class at_path_to_ordos : public AreaTriggerScript
{
    public:
        at_path_to_ordos() : AreaTriggerScript("at_path_to_ordos") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*at*/) override
        {
            bool hasLegendaryCloack = player->HasItemCount(102245) || player->HasItemCount(102246) || player->HasItemCount(102247) || player->HasItemCount(102248) || player->HasItemCount(102249) || player->HasItemCount(102250);
            if (player->HasAchieved(7536) || hasLegendaryCloack)
            {
                player->CastSpell(player, 149322);
                return true;
            }
            return false;
        }
};

// 1320 Summoned by 148208 - Burning Pitch
class sat_burning_pitch : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, 148208, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(148208);
    }
};

// 1306 Summoned by 147819 - Fire Blossom
class sat_fire_blossom : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, SPELL_FIRE_BLOSSOM_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_FIRE_BLOSSOM_EFF);
    }
};

// 1307 Summoned by 147827 - Storm Blossom
class sat_storm_blossom : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, SPELL_STORM_BLOSSOM_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_STORM_BLOSSOM_EFF);
    }
};

// 1312. Summoned By 147948 - Devour
class sat_evermaw_devour : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && GetCaster() && GetCaster()->isInFront(triggering->ToPlayer(), M_PI / 3);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
            if (Player* target = triggering->ToPlayer())
                caster->CastSpell(target, SPELL_DEVOUR_EFF, true);
    }
};

// 1311. Summoned By 147812 - Time Rift
class sat_evermaw_time_rift : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Unit* caster = GetCaster())
            if (Player* itr = target->ToPlayer())
                caster->CastSpell(itr, SPELL_TIME_RIFT_EFF, true);
    }
};

// 1305. Summoned By 147841 - Time Lock
class sat_evermaw_time_lock : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Unit* caster = GetCaster())
            if (Player* itr = target->ToPlayer())
                caster->CastSpell(itr, SPELL_TIME_LOCK_EFF, true);
    }
};

// 1232. Summoned By 146496 - Sprite Ward
class sat_sprite_ward : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, 146495, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(146495);
    }
};

void AddSC_timeless_isle()
{
    new creature_script<npc_timeless_turtles>("npc_timeless_turtles");
    new npc_timeless_faction_sentries();
    new npc_timeless_adders();
    new npc_timeless_yaks();
    new npc_timeless_cranes();
    new npc_timeless_silkworms();
    new npc_timeless_crabs();
    new npc_timeless_spectrals();
    new npc_timeless_cats();
    new npc_timeless_elementals();
    new npc_timeless_spirits();
    new npc_timeless_giants();
    new npc_timeless_frogs();
    new npc_timeless_dragons();
    new npc_ordos_minions();
    new npc_eternal_kiln();
    new npc_damp_shamblers();
    new npc_ordon_guards();
    new npc_ordos_warriors();
    new npc_ordon_watchers();
    new npc_molten_guards();
    new npc_leaf_sprites();
    new npc_karkanos();
    new npc_zesqua();
    new npc_jademist_dancer();
    new creature_script<npc_cranegnasher>("npc_cranegnasher");
    new creature_script<npc_nice_sprite>("npc_nice_sprite");
    new npc_ordon_candlekeeper();
    new npc_garnia();
    new npc_rattleskew();
    new npc_southsea_lookout();
    new npc_playful_water_spirit();
    new npc_cove_shark();
    new npc_stinkbraid();
    new npc_spectral_pirate();
    new npc_hopswift();
    new npc_emperor_shaohao();
    new npc_senior_historian_evelyna();
    new creature_script<npc_highwind_albatross>("npc_highwind_albatross");
    new npc_fin_longpaw();
    new npc_zarhym_1();
    new npc_zarhym_2();
    new creature_script<npc_dread_ship_vazuvius>("npc_dread_ship_vazuvius");
    new creature_script<npc_evermaw>("npc_evermaw");

    new go_time_lost_shrine_ti();
    new go_gleaming_crane_statue_ti();
    new go_conspicuously_empty_shell();
    new go_challengers_stone();
    new go_crane_nest();
    new go_zarhyms_body();
    new go_cursed_grave();
    new go_timeless_chest();
    new go_neverending_spritewood();

    new spell_timeless_isle_crane_wings();
    new spell_timeless_isle_cauterize();
    new spell_timeless_isle_burning_fury();
    new spell_spirit_world();
    new spell_script<spell_create_timeless_item>("spell_create_timeless_item");
    new spell_script<spell_create_timeless_item_from_cache>("spell_create_timeless_item_from_cache");
    new aura_script<spell_bloody_coin_proc>("spell_bloody_coin_proc");
    new aura_script<spell_censer_of_eternal_agony>("spell_censer_of_eternal_agony");
    new aura_script<spell_book_of_the_ages>("spell_book_of_the_ages");
    new spell_script<spell_eternitys_embrace>("spell_eternitys_embrace");
    new aura_script<spell_gulp_frog_toxin>("spell_gulp_frog_toxin");
    new spell_script<spell_cat_defensive_leap>("spell_cat_defensive_leap");
    new spell_script<spell_timelesscat_pounce>("spell_timelesscat_pounce");
    new spell_script<spell_evermaw_time_portals>("spell_evermaw_time_portals");
    new aura_script<spell_crane_wings>("spell_crane_wings");

    new at_path_to_ordos();

    new atrigger_script<sat_burning_pitch>("sat_burning_pitch");
    new atrigger_script<sat_fire_blossom>("sat_fire_blossom");
    new atrigger_script<sat_storm_blossom>("sat_storm_blossom");
    new atrigger_script<sat_evermaw_devour>("sat_evermaw_devour");
    new atrigger_script<sat_evermaw_time_rift>("sat_evermaw_time_rift");
    new atrigger_script<sat_evermaw_time_lock>("sat_evermaw_time_lock");
    new atrigger_script<sat_sprite_ward>("sat_sprite_ward");
}
