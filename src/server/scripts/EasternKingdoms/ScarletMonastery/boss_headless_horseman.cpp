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

#include "scarlet_monastery.h"
#include "LFGMgr.h"
#include "Player.h"
#include "Group.h"
#include "SpellInfo.h"
#include "CreatureTextMgr.h"

enum Yells
{
    SAY_PLAYER_TEXT_0           = 12510,
    SAY_PLAYER_TEXT_1           = 12511,
    SAY_PLAYER_TEXT_2           = 12512,
    SAY_PLAYER_TEXT_3           = 12513,

    SAY_ENTRANCE                = 0,
    SAY_REJOINED                = 1,
    SAY_CONFLAGRATION           = 2,
    SAY_SPROUTING_PUMPKINS      = 3,
    SAY_PLAYER_DEATH            = 4,
    SAY_DEATH                   = 5,

    SAY_LOST_HEAD               = 0,
};

enum Misc
{
    TYPE_STARTER_GUID           = 100,

    ACTION_SWITCH_PHASE         = 1,
};

enum Entries
{
    NPC_HEAD                    = 23775,
    NPC_PULSING_PUMPKIN         = 23694,
    NPC_PUMPKIN_FIEND           = 23545,
    NPC_SIR_THOMAS              = 23904,

    GO_PUMPKIN_SHRINE           = 186267,
    GO_LOOSELY_TURNED_SOIL      = 186314,
};

enum Spells
{
    SPELL_SUMMON_PUMPKIN                             = 42277,
    SPELL_PUMPKIN_LIFE_CYCLE                         = 42280,
    SPELL_SPROUTING                                  = 42281,
    SPELL_SPROUT_BODY                                = 42285,
    SPELL_HEADLESS_HORSEMAN_SUMMON_PUMPKIN_PULSE     = 42290,
    SPELL_HEADLESS_HORSEMAN_PUMPKIN_LIFE_CYCLE_DEATH = 42291,
    SPELL_HEADLESS_HORSEMAN_PUMPKIN_AURA             = 42294,
    SPELL_HORSEMANS_SUMMON                           = 42394,
    SPELL_SUMMON_PUMPKIN_MISSILE                     = 42552,
    SPELL_SUMMON_PUMPKIN_BURST_DELAY                 = 52236, // 8s trigger 42394

    SPELL_IMMUNED                                    = 42556,
    SPELL_BODY_REGEN                                 = 42403, // regenerate health plus model change to unhorsed
    SPELL_CONFUSED                                   = 43105,
    SPELL_HEAL_BODY                                  = 43306, // heal to 100%
    SPELL_CLEAVE                                     = 42587,
    SPELL_WHIRLWIND                                  = 43116,
    SPELL_CONFLAGRATION                              = 42380,
    SPELL_BURNING                                    = 42971,

    SPELL_FLYING_HEAD                                = 42399, // flying head visual
    SPELL_HEAD                                       = 42413, // horseman head visual
    SPELL_HEAD_LANDS                                 = 42400,
    //SPELL_CREATE_PUMPKIN_TREATS                      = 42754,
    SPELL_RHYME_BIG                                  = 42909,
};

std::vector<uint32> randomLaugh = { 11965, 11975, 11976 };
std::vector<uint32> text = { SAY_PLAYER_TEXT_0, SAY_PLAYER_TEXT_1, SAY_PLAYER_TEXT_2, SAY_PLAYER_TEXT_3 };

#define GOSSIP_OPTION_MENU 8891

struct boss_headless_horseman : public ScriptedAI
{
    boss_headless_horseman(Creature* creature) : ScriptedAI(creature), _summons(me) { }

    void SetGUID(ObjectGuid guid, int32 type) override
    {
        if (type == TYPE_STARTER_GUID)
            _starterGUID = guid;
    }

    void Reset() override
    {
        if (me->GetInstanceScript())
        {
            if (!me->GetMap()->ToInstanceMap()->IsLFGMap())
                return;

            if (Player* player = me->GetMap()->GetFirstPlayerInInstance())
                if (sLFGMgr->GetDungeon(player->GetGroup() ? player->GetGroup()->GetGUID() : player->GetGUID()) != LFG_DUNGEON_HEADLESS_HORSEMAN)
                    return;
        }

        _summons.DespawnAll();

        me->SetVisible(false);
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);

        me->SetSpeed(MOVE_RUN, 2.0f, true);
        me->SetCorpseDelay(75);

        introCount = 0;
        phase = 0;
        _starterGUID = ObjectGuid::Empty;

        introTimer = 1*IN_MILLISECONDS;
        laughTimer = 5*IN_MILLISECONDS;
        cleaveTimer = 3*IN_MILLISECONDS;
        summonTimer = 1*IN_MILLISECONDS;
        conflagTimer = 4*IN_MILLISECONDS;

        me->SummonGameObject(GO_PUMPKIN_SHRINE, 1099.74f, 612.901f, 2.16763f, 6.14356f, { 0, 0, 0.00518764f, -0.999987f }, 0);
        DoCast(me, SPELL_HEAD, true);
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type != WAYPOINT_MOTION_TYPE || id != 5)
            return;

        me->m_Events.Schedule(1000, [this]
        {
            phase = 1;
            float x, y, z, o;
            me->GetRespawnPosition(x, y, z, &o);
            me->SetHomePosition(x, y, z, o);
            me->SetReactState(REACT_AGGRESSIVE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
            DoZoneInCombat(me, 100.0f);
        });
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoZoneInCombat(me, 100.0f);
    }

    void KilledUnit(Unit* victim) override
    {
        if (!victim->ToPlayer())
            return;

        Talk(SAY_PLAYER_DEATH);
    }

    void JustSummoned(Creature* summon) override
    {
        _summons.Summon(summon);

        if (summon->GetEntry() == NPC_PULSING_PUMPKIN)
        {
            summon->NearTeleportTo(summon->GetPositionX(), summon->GetPositionY(), summon->GetPositionZ() + 0.5f, summon->GetOrientation());
            summon->CastSpell(summon, SPELL_PUMPKIN_LIFE_CYCLE, true);
            summon->CastSpell(summon, SPELL_HEADLESS_HORSEMAN_SUMMON_PUMPKIN_PULSE, true);
            summon->CastSpell(summon, SPELL_HEADLESS_HORSEMAN_PUMPKIN_AURA, true);
            summon->CastSpell(summon, SPELL_SPROUTING);
            summon->SetReactState(REACT_PASSIVE);
            return;
        }

        summon->SetInCombatWithZone();
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(SAY_DEATH);
        _summons.DespawnAll();

        // TODO: check kill boss == DONE
        // TODO: unhack
        Map* map = me->GetMap();
        if (map && map->IsDungeon())
        {
            Map::PlayerList const& players = map->GetPlayers();
            if (!players.isEmpty())
                if (players.begin()->GetSource()->GetGroup())
                    sLFGMgr->FinishDungeon(players.begin()->GetSource()->GetGroup()->GetGUID(), 285, map);
        }
        DoCast(me, SPELL_BURNING, true);
        me->SummonCreature(NPC_SIR_THOMAS, 1118.27f, 617.449f, 1.2174f, 4.42471f, TEMPSUMMON_TIMED_DESPAWN, Milliseconds(60*IN_MILLISECONDS));
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (phase > 3)
        {
            me->RemoveAllAuras();
            return;
        }

        if (me->HasAura(SPELL_BODY_REGEN))
        {
            damage = 0;
            return;
        }

        if (damage >= me->GetHealth())
        {
            damage = me->GetHealth() - 1;

            DoCast(me, SPELL_IMMUNED, true);
            DoCast(me, SPELL_BODY_REGEN, true);
            DoCast(me, SPELL_CONFUSED, true);

            Position randomPos = me->GetRandomNearPosition(20.0f);

            if (Creature* head = me->SummonCreature(NPC_HEAD, randomPos))
            {
                head->AI()->SetData(0, phase);

                switch (phase)
                {
                    case 2: head->SetHealth(uint32(head->GetMaxHealth() * 2 / 3)); break;
                    case 3: head->SetHealth(uint32(head->GetMaxHealth() / 3)); break;
                }
            }

            me->RemoveAurasDueToSpell(SPELL_HEAD);
        }
    }

    void SpellHit(Unit* /*target*/, SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_HORSEMANS_SUMMON)
            Talk(SAY_SPROUTING_PUMPKINS);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_SWITCH_PHASE:
                me->RemoveAllAuras();
                DoCast(me, SPELL_HEAL_BODY, true);
                DoCast(me, SPELL_HEAD, true);

                ++phase;
                if (phase > 3)
                    me->DealDamage(me, me->GetHealth());
                else
                    Talk(SAY_REJOINED);

                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (phase == 0)
            return;

        if (me->HasAura(SPELL_BODY_REGEN))
        {
            if (me->IsFullHealth())
            {
                me->RemoveAllAuras();
                DoCast(me, SPELL_HEAD, true);

                if (Creature* head = me->FindNearestCreature(NPC_HEAD, 250.0f, true))
                {
                    head->SetFullHealth();
                    head->RemoveAllAuras();
                    head->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    head->DespawnOrUnsummon(3000);
                    head->CastSpell(me, SPELL_FLYING_HEAD, true);
                }
            }
            else if (!me->HasAura(SPELL_WHIRLWIND) && me->GetHealthPct() > 10.0f)
                DoCast(me, SPELL_WHIRLWIND, true);

            return;
        }

        if (laughTimer <= diff)
        {
            DoPlaySoundToSet(me, randomLaugh[rand()%3]);
            laughTimer = urand(11 * IN_MILLISECONDS, 22 * IN_MILLISECONDS);
        }
        else
            laughTimer -= diff;

        if (me->HasReactState(REACT_PASSIVE))
            return;

        if (cleaveTimer <= diff)
        {
            DoCastVictim(SPELL_CLEAVE);
            cleaveTimer = urand(2 * IN_MILLISECONDS, 6 * IN_MILLISECONDS);
        }
        else
            cleaveTimer -= diff;

        switch (phase)
        {
            case 2:
                if (conflagTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 30.0f, true))
                        DoCast(target, SPELL_CONFLAGRATION);
                    Talk(SAY_CONFLAGRATION);
                    conflagTimer = urand(8 * IN_MILLISECONDS, 12 * IN_MILLISECONDS);
                }
                else
                    conflagTimer -= diff;
                break;
            case 3:
                if (summonTimer <= diff)
                {
                    DoCast(me, SPELL_SUMMON_PUMPKIN_BURST_DELAY, true);
                    summonTimer = 15*IN_MILLISECONDS;
                }
                else
                    summonTimer -= diff;
                break;
        }

        DoMeleeAttackIfReady();
    }

    void PlayerSay(uint32 sayText)
    {
        if (Player* player = ObjectAccessor::GetPlayer(*me, _starterGUID))
            player->Say(player->GetSession()->GetTrinityString(sayText), LANG_UNIVERSAL);
    }

    private:
        SummonList _summons;
        uint8 phase;
        uint8 introCount;
        uint32 introTimer;
        uint32 laughTimer;
        uint32 cleaveTimer;
        uint32 summonTimer;
        uint32 conflagTimer;
        ObjectGuid _starterGUID;
};

struct npc_horseman_head : public ScriptedAI
{
    npc_horseman_head(Creature* creature) : ScriptedAI(creature)
    {
        me->SetDisplayId(21908);
        me->SetReactState(REACT_PASSIVE);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED | UNIT_FLAG_NOT_SELECTABLE);
        me->GetMotionMaster()->MoveRandom(30.0f);
        DoCast(me, SPELL_HEAD, true);
        DoCast(me, SPELL_HEAD_LANDS, true);
        Talk(SAY_LOST_HEAD);
        _despawn = false;
    }

    void SetData(uint32 /*type*/, uint32 data) override
    {
        _phase = data;
    }

    void DamageTaken(Unit* /*attacker*/, uint32 &damage) override
    {
        if (_despawn)
        {
            damage = 0;
            return;
        }

        int32 healthPct;

        switch (_phase)
        {
            case 1: healthPct = 66; break;
            case 2: healthPct = 33; break;
            default: healthPct = 1; break;
        }

        if (me->HealthBelowPctDamaged(healthPct, damage) || damage >= me->GetHealth())
        {
            damage = 0;
            me->RemoveAllAuras();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            me->DespawnOrUnsummon(3000);
            _despawn = true;

            if (me->ToTempSummon())
                if (Unit* horseman = me->ToTempSummon()->GetSummoner())
                {
                    if (_phase < 3)
                        DoCast(horseman, SPELL_FLYING_HEAD, true);
                    horseman->ToCreature()->AI()->DoAction(ACTION_SWITCH_PHASE);
                }
        }
    }

private:
    uint8 _phase;
    bool _despawn;
};

class go_pumpkin_shrine : public GameObjectScript
{
    public:
        go_pumpkin_shrine() : GameObjectScript("go_pumpkin_shrine") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (go->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
                player->PrepareQuestMenu(go->GetGUID());

            player->ADD_GOSSIP_ITEM_DB(GOSSIP_OPTION_MENU, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF)
            {
                if (Unit* horseman = go->GetOwner())
                {
                    uint32 delay = 0;
                    horseman->ToCreature()->AI()->SetGUID(player->GetGUID(), TYPE_STARTER_GUID);
                    horseman->m_Events.Schedule(delay += 1000, [horseman] { CAST_AI(boss_headless_horseman, horseman->GetAI())->PlayerSay(text[0]); });
                    horseman->m_Events.Schedule(delay += 3000, [horseman] { CAST_AI(boss_headless_horseman, horseman->GetAI())->PlayerSay(text[1]); });
                    horseman->m_Events.Schedule(delay += 3000, [horseman] { CAST_AI(boss_headless_horseman, horseman->GetAI())->PlayerSay(text[2]); });
                    horseman->m_Events.Schedule(delay += 3000, [horseman] { CAST_AI(boss_headless_horseman, horseman->GetAI())->PlayerSay(text[3]); });
                    horseman->m_Events.Schedule(delay +=  100, [horseman]
                    {
                        horseman->ToCreature()->AI()->Talk(SAY_ENTRANCE);
                        horseman->CastSpell(horseman, SPELL_RHYME_BIG, true);
                        horseman->GetMotionMaster()->MovePath(2368200, false);
                        horseman->SetVisible(true);
                    });
                    horseman->m_Events.Schedule(delay += 4000, [horseman]
                    {
                        if (GameObject* go = horseman->FindNearestGameObject(GO_LOOSELY_TURNED_SOIL, 100.0f))
                            go->ForcedDespawn();
                    });
                    go->ForcedDespawn();
                }
            }
            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
            player->CLOSE_GOSSIP_MENU();
            return true;
        }
};

class spell_sprouting : public SpellScript
{
    PrepareSpellScript(spell_sprouting);

    void HandleEffectHitTarget(SpellEffIndex effIndex)
    {
        if (Creature* pumpkin = GetHitCreature())
        {
            pumpkin->RemoveAurasDueToSpell(SPELL_PUMPKIN_LIFE_CYCLE);
            pumpkin->RemoveAurasDueToSpell(SPELL_HEADLESS_HORSEMAN_PUMPKIN_AURA);
            pumpkin->CastSpell(pumpkin, SPELL_HEADLESS_HORSEMAN_PUMPKIN_LIFE_CYCLE_DEATH, true);
            pumpkin->CastSpell(pumpkin, SPELL_SPROUT_BODY, true);
            pumpkin->SetReactState(REACT_AGGRESSIVE);
            pumpkin->SetInCombatWithZone();
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sprouting::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

void AddSC_boss_headless_horseman()
{
    new creature_script<boss_headless_horseman>("boss_headless_horseman");
    new creature_script<npc_horseman_head>("npc_horseman_head");
    new go_pumpkin_shrine();
    new spell_script<spell_sprouting>("spell_sprouting");
}
