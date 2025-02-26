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

/**
  QUEST ID: 25407 "Forces of Nature: Wisps"
  QUEST ID: 25409 "Forces of Nature: Hyppogryphs"
  QUEST ID: 25410 "Forces of Nature: Treants"
**/

enum ForcesOfNatureQuests
{
    NPC_FERALAS_WISP                                  = 40079,
    NPC_FRAYFEATHER_HIPPOGRYPH                        = 5300,
    NPC_FRAYFEATHER_STAGWING                          = 5304,
    NPC_HORDE_POACHER                                 = 40069,
    NPC_FORCES_01_HIPPOGRYPH_KILL_CREDIT_BUNNY        = 40072,
    NPC_FORCES_03_TREANT_KILL_CREDIT_BUNNY            = 40090,

    SPELL_RALLY_WILD_WISP                             = 74738,
    SPELL_RALLY_WILD_HIPPOGRYFF                       = 74674,
    SPELL_RALLY_WILD_TREANT                           = 74756,

    SPELL_FORCE_PLAYER_TO_SUMMON_HORDE_PATCHER        = 74728,

    // Feralas wisp
    SPELL_WISP_DETONATE                               = 6237,

    // Frayfeather hippogryph & Frayfeather stagwing
    SPELL_AIR_BLAST                                   = 81181,
    SPELL_FEATHERLIGHT                                = 81179,
    SPELL_WING_BUFFET                                 = 32914,

    // Wandering Forest Walker
    SPELL_REGROWTH                                    = 16561,
    SPELL_FORCE_OF_NATURE                             = 6913,

    // Horde poacher
    SPELL_NET                                         = 6533,
    SPELL_AIMED_SHOT                                  = 78841,
    SPELL_SHOT                                        = 6660,

    EVENT_CHECK_FOR_PLAYER                            = 1,
    EVENT_CHECK_ATTACK                                = 2,
    EVENT_START_FLYING                                = 3,
    EVENT_DESPAWN_PART_00                             = 4,
    EVENT_START_FIGHT                                 = 5,
    EVENT_CHECK_FOR_HEALTH                            = 6,
    EVENT_COOLDOWN_00                                 = 7,
    EVENT_COOLDOWN_01                                 = 8,
    EVENT_COOLDOWN_02                                 = 9,

    HORDE_POACHER_TEXT_01                             = 0,

    QUEST_FORCES_OF_NATURE_WISPS                      = 25407
};

/********************\
 * npc_feralas_wisp *
\********************/

struct npc_feralas_wisp : public ScriptedAI
{
    npc_feralas_wisp(Creature* creature) : ScriptedAI(creature) { }

    EventMap _events;
    bool _isFollowing;
    ObjectGuid _playerGUID;

    void Reset() override
    {
        _isFollowing = false;
        _playerGUID = ObjectGuid::Empty;
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (!_isFollowing)
        {
            if (Player* player = caster->ToPlayer())
            {
                if (spell->Id == SPELL_RALLY_WILD_WISP)
                {
                    _isFollowing = true;
                    _playerGUID = player->GetGUID();
                    me->setActive(true);
                    player->KilledMonsterCredit(NPC_FERALAS_WISP);
                    me->GetMotionMaster()->Clear();
                    me->SetHover(true);
                    me->SetFloatValue(UNIT_FIELD_HOVER_HEIGHT, frand(1.8f, 2.2f));
                    me->GetMotionMaster()->MoveFollow(player, frand(8.0f, 12.0f), frand(2.14f, 4.14f));

                    _events.ScheduleEvent(EVENT_CHECK_FOR_PLAYER, 1000);
                }
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CHECK_FOR_PLAYER:
                {
                    Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID);

                    if (!player)
                        DoCastSelf(SPELL_WISP_DETONATE);
                    else if (player->GetQuestStatus(QUEST_FORCES_OF_NATURE_WISPS) == QUEST_STATUS_REWARDED)
                        DoCastSelf(SPELL_WISP_DETONATE);
                    else if (player->GetQuestStatus(QUEST_FORCES_OF_NATURE_WISPS) == QUEST_STATUS_NONE)
                        DoCastSelf(SPELL_WISP_DETONATE);
                    else if (me->GetDistance2d(player) > 250.0f)
                        DoCastSelf(SPELL_WISP_DETONATE);
                    else
                        _events.ScheduleEvent(EVENT_CHECK_FOR_PLAYER, 1000);
                }
            }
        }
    }
};

/**************************\
 * npc_feralas_hippogryph *
\**************************/

struct npc_feralas_hippogryph : public ScriptedAI
{
    npc_feralas_hippogryph(Creature* creature) : ScriptedAI(creature) { }

    EventMap _events;
    ObjectGuid _playerGUID;
    bool _hippogryph_called;

    uint32 _timer_spell_air_blast;
    uint32 _timer_spell_featherlight;
    uint32 _timer_spell_wing_buffet;

    void Reset() override
    {
        _playerGUID = ObjectGuid::Empty;
        _hippogryph_called = false;

        _timer_spell_air_blast = urand(2000, 3000);
        _timer_spell_featherlight = urand(5000, 9000);
        _timer_spell_wing_buffet = urand(3000, 11000);
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (!_hippogryph_called)
        {
            if (Player* player = caster->ToPlayer())
            {
                if (spell->Id == SPELL_RALLY_WILD_HIPPOGRYFF)
                {
                    _playerGUID = player->GetGUID();
                    _hippogryph_called = true;
                    me->GetMotionMaster()->MoveIdle();
                    me->SetFaction(35);
                    me->SetFacingTo(me->GetAngle(player->GetPositionX(), player->GetPositionY()));
                    player->KilledMonsterCredit(NPC_FORCES_01_HIPPOGRYPH_KILL_CREDIT_BUNNY);

                    _events.ScheduleEvent(EVENT_CHECK_ATTACK, 2000);
                    _events.ScheduleEvent(EVENT_START_FLYING, 3000);
                }
            }
        }
    }

    void JustEngagedWith(Unit* /*victim*/) override
    {
        if (_hippogryph_called)
        {
            me->AttackStop();
            me->SetReactState(REACT_PASSIVE);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_START_FLYING:
                {
                    me->GetMotionMaster()->MovePoint(0, me->m_positionX, me->m_positionY, me->m_positionZ + 100.0f);
                    _events.ScheduleEvent(EVENT_DESPAWN_PART_00, 5000);
                    break;
                }

                case EVENT_DESPAWN_PART_00:
                {
                    me->DespawnOrUnsummon(10);
                    break;
                }

                case EVENT_CHECK_ATTACK:
                {
                    if (urand(0, 100) <= 66)
                        if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
                            me->CastSpell(player, SPELL_FORCE_PLAYER_TO_SUMMON_HORDE_PATCHER, true);
                    break;
                }
            }
        }

        if (UpdateVictim())
        {
            if (me->GetEntry() == NPC_FRAYFEATHER_HIPPOGRYPH)
            {
                if (_timer_spell_air_blast <= diff)
                {
                    DoCastVictim(SPELL_AIR_BLAST);
                    _timer_spell_air_blast = urand(12000, 13000);
                }
                else
                    _timer_spell_air_blast -= diff;
            }

            if (_timer_spell_featherlight <= diff)
            {
                DoCastSelf(SPELL_FEATHERLIGHT);
                _timer_spell_featherlight = urand(22000, 27000);
            }
            else
                _timer_spell_featherlight -= diff;

            if (me->GetEntry() == NPC_FRAYFEATHER_STAGWING)
            {
                if (_timer_spell_wing_buffet <= diff)
                {
                    DoCastVictim(SPELL_WING_BUFFET);
                    _timer_spell_wing_buffet = urand(17000, 19000);
                }
                else
                    _timer_spell_wing_buffet -= diff;
            }

            DoMeleeAttackIfReady();
        }
    }
};

/***************************************\
 * npc_feralas_wandering_forest_walker *
\***************************************/

struct npc_feralas_wandering_forest_walker : public ScriptedAI
{
    npc_feralas_wandering_forest_walker(Creature* creature) : ScriptedAI(creature) { }

    EventMap _events;
    ObjectGuid _playerGUID;
    bool _treant_called;

    uint32 _timer_spell_regrowth;
    uint32 _timer_spell_force_of_nature;

    void Reset() override
    {
        _playerGUID = ObjectGuid::Empty;
        _treant_called = false;

        _timer_spell_regrowth = urand(2000, 3000);
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (!_treant_called)
        {
            if (Player* player = caster->ToPlayer())
            {
                if (spell->Id == SPELL_RALLY_WILD_TREANT)
                {
                    _playerGUID = player->GetGUID();
                    _treant_called = true;
                    me->GetMotionMaster()->Clear();
                    me->SetFacingTo(me->GetAngle(player->GetPositionX(), player->GetPositionY()));
                    player->KilledMonsterCredit(NPC_FORCES_03_TREANT_KILL_CREDIT_BUNNY);

                    _events.ScheduleEvent(EVENT_CHECK_ATTACK, 2000);
                    _events.ScheduleEvent(EVENT_DESPAWN_PART_00, 2000 + 500);
                }
            }
        }
    }

    void JustEngagedWith(Unit* /*victim*/) override
    {
        if (_treant_called)
        {
            me->AttackStop();
            me->SetReactState(REACT_PASSIVE);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DESPAWN_PART_00:
                {
                    me->DespawnOrUnsummon(10);
                    break;
                }

                case EVENT_CHECK_ATTACK:
                {
                    if (urand(0, 100) <= 66)
                        if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
                            me->CastSpell(player, SPELL_FORCE_PLAYER_TO_SUMMON_HORDE_PATCHER, true);
                    break;
                }
            }
        }

        if (UpdateVictim())
        {
            if (_timer_spell_force_of_nature <= diff)
            {
                DoCastSelf(SPELL_FORCE_OF_NATURE);
                _timer_spell_force_of_nature = urand(25000, 28000);
            }
            else
                _timer_spell_force_of_nature -= diff;

            if (!me->HealthAbovePct(40))
            {
                if (_timer_spell_regrowth <= diff)
                {
                    DoCastSelf(SPELL_REGROWTH);
                    _timer_spell_regrowth = urand(22000, 26000);
                }
                else
                    _timer_spell_regrowth -= diff;
            }

            DoMeleeAttackIfReady();
        }
    }
};

/*****************************\
 * npc_feralas_horde_poacher *
\*****************************/

struct npc_feralas_horde_poacher : public ScriptedAI
{
    npc_feralas_horde_poacher(Creature* creature) : ScriptedAI(creature) { }

    EventMap _events;
    ObjectGuid _playerGUID;
    bool _cd_spell_net;
    bool _cd_spell_shot;
    bool _cd_spell_aimed_shot;
    bool _talk_done;

    void Reset() override
    {
        _playerGUID = ObjectGuid::Empty;
        _cd_spell_net = false;
        _cd_spell_shot = false;
        _cd_spell_aimed_shot = false;
        _talk_done = false;
        me->SetFaction(14);
        me->SetReactState(REACT_AGGRESSIVE);
    }

    void IsSummonedBy(Unit* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            _playerGUID = player->GetGUID();
            _events.ScheduleEvent(EVENT_START_FIGHT, 500);

            if (me->GetDistance(player) < 20.0f)
                me->CastSpell(player, SPELL_NET, true);
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/) override
    {
        if (me->GetHealthPct() < 30.0f)
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
            {
                if (!_talk_done)
                    Talk(HORDE_POACHER_TEXT_01);

                _talk_done = true;
                me->AttackStop();
                me->GetMotionMaster()->MoveFleeing(player, 5000);
                _events.RescheduleEvent(EVENT_CHECK_FOR_HEALTH, 5000);
            }
        }
    }

    void EnterEvadeMode() override
    {
        if (_playerGUID)
            me->DespawnOrUnsummon(10);
    };

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        if (!UpdateVictim())
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_START_FIGHT:
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
                    {
                        SpellCastNet(player);
                        me->SetWalk(true);
                        me->Attack(player, true);
                    }
                    break;
                }

                case EVENT_COOLDOWN_00:
                {
                    _cd_spell_net = false;
                    break;
                }

                case EVENT_COOLDOWN_01:
                {
                    _cd_spell_aimed_shot = false;
                    break;
                }

                case EVENT_COOLDOWN_02:
                {
                    _cd_spell_shot = false;
                    break;
                }

                case EVENT_CHECK_FOR_HEALTH:
                {
                    _events.RescheduleEvent(EVENT_CHECK_ATTACK, 10);
                    break;
                }

                case EVENT_CHECK_ATTACK:
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
                    {
                        if (!(_cd_spell_net || _cd_spell_aimed_shot || _cd_spell_shot))
                        {
                            if (me->GetDistance(player) > 35.0f)
                                me->Attack(player, false);
                            else if (me->GetDistance(player) > 25.0f)
                                SpellCastAimedShot(player);
                            else if (me->GetDistance(player) > 5.0f)
                                SpellCastShot(player);
                            else
                                me->Attack(player, true);
                            return;
                        }
                    }

                    _events.ScheduleEvent(EVENT_CHECK_ATTACK, 1000);
                    break;
                }
            }
        }

        DoMeleeAttackIfReady();
    }

    void SpellCastNet(Player* player)
    {
        if (!_cd_spell_net)
        {
            me->CastSpell(player, SPELL_NET, true);
            _cd_spell_net = true;
            _events.ScheduleEvent(EVENT_COOLDOWN_00, 3000);
        }
    }

    void SpellCastAimedShot(Player* player)
    {
        if (!_cd_spell_aimed_shot)
        {
            me->CastSpell(player, SPELL_AIMED_SHOT, true);
            _cd_spell_aimed_shot = true;
            _events.ScheduleEvent(EVENT_COOLDOWN_01, 11000);
        }
    }

    void SpellCastShot(Player* player)
    {
        if (!_cd_spell_aimed_shot)
        {
            me->CastSpell(player, SPELL_SHOT, true);
            _cd_spell_shot = true;
            _events.ScheduleEvent(EVENT_COOLDOWN_02, 5000);
        }
    }
};

enum ForceOfNatureTypes
{
    NPC_TREANT_CREDIT           = 40090,
    NPC_GIANT_CREDIT            = 43347,
    NPC_WANDERING_FOREST_WALKER = 7584,
    NPC_MOUNTAIN_GIANT          = 40026,

    ACTION_GIANT_CALLED = 0,
};

// Mountain Giant 40026
struct npc_feralas_mountain_giant : public CreatureAI
{
    npc_feralas_mountain_giant(Creature* creature) : CreatureAI(creature) { }

    TaskScheduler scheduler;
    bool hasCredit;

    void Reset() override
    {
        hasCredit = false;
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_GIANT_CALLED && !hasCredit)
        {
            hasCredit = true;

            scheduler
                .Schedule(Seconds(3), [this](TaskContext /*context*/)
            {
                // Move away!
                Position pos = me->GetNearPosition(10.0f, frand(0.0f, 2 * M_PI));
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(0, pos);
                me->DespawnOrUnsummon(me->GetSplineDuration());
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Rally Wild Treant 74756
class spell_feralas_rally_wild_treant : public SpellScript
{
    PrepareSpellScript(spell_feralas_rally_wild_treant);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
            if (Creature* target = GetHitCreature())
                if (target->GetEntry() == NPC_WANDERING_FOREST_WALKER)
                    caster->KilledMonsterCredit(NPC_TREANT_CREDIT);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_feralas_rally_wild_treant::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Rally Wild Mountain Giant 80995
class spell_feralas_rally_wild_mountain_giant : public SpellScript
{
    PrepareSpellScript(spell_feralas_rally_wild_mountain_giant);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            if (Creature* target = GetHitCreature())
            {
                target->AI()->DoAction(ACTION_GIANT_CALLED);

                if (target->GetEntry() == NPC_MOUNTAIN_GIANT)
                    caster->KilledMonsterCredit(NPC_GIANT_CREDIT);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_feralas_rally_wild_mountain_giant::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

void AddSC_feralas()
{
    new creature_script<npc_feralas_wisp>("npc_feralas_wisp");
    new creature_script<npc_feralas_hippogryph>("npc_feralas_hippogryph");
    new creature_script<npc_feralas_wandering_forest_walker>("npc_feralas_wandering_forest_walker");
    new creature_script<npc_feralas_horde_poacher>("npc_feralas_horde_poacher");
    new creature_script<npc_feralas_mountain_giant>("npc_feralas_mountain_giant");
    new spell_script<spell_feralas_rally_wild_treant>("spell_feralas_rally_wild_treant");
    new spell_script<spell_feralas_rally_wild_mountain_giant>("spell_feralas_rally_wild_mountain_giant");
}
