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

#include "PetAI.h"
#include "Errors.h"
#include "Pet.h"
#include "Player.h"
#include "DBCStores.h"
#include "Spell.h"
#include "ObjectAccessor.h"
#include "SpellMgr.h"
#include "Creature.h"
#include "World.h"
#include "Util.h"
#include "Group.h"
#include "SpellInfo.h"
#include "MoveSpline.h"

enum
{
    SPELL_SEDUCTION         = 6358,
    SPELL_MESMERIZE         = 115268,
    SPELL_SUFFERING         = 17735,
    SPELL_SHADOW_SHIELD     = 115232,
    SPELL_VOID_SHIELD       = 115236,
};

int PetAI::Permissible(const Creature* creature)
{
    if (creature->IsPet())
        return PERMIT_BASE_SPECIAL;

    return PERMIT_BASE_NO;
}

PetAI::PetAI(Creature* c) : CreatureAI(c), i_tracker(TIME_INTERVAL_LOOK)
{
    UpdateAllies();

    if (Unit* owner = me->GetOwner())
        if (owner->GetTypeId() == TYPEID_PLAYER && owner->GetClass() == CLASS_MAGE)
            m_pureCaster = true;
    switch (me->GetEntry())
    {
        case 416:   // Imp
        case 58959: // Fel Imp
            m_pureCaster = true;
            break;
        default:
            break;
    }

    if (!me->IsPet())
        me->SetReactState(REACT_ASSIST);

    // this is for warlock greenfire questline.
    if (me->GetOwner() && me->GetOwner()->ToPlayer() && me->GetOwner()->ToPlayer()->GetQuestStatus(32295) == QUEST_STATUS_INCOMPLETE)
    {
        switch (me->GetEntry())
        {
            case 1860: // voidwalker
                me->GetOwner()->ToPlayer()->KilledMonsterCredit(67945);
                break;
            case 1863: // succubus
                me->GetOwner()->ToPlayer()->KilledMonsterCredit(67944);
                break;
        }
    }
}

PetAI::~PetAI()
{
    ResetDelayedSpellIfNeed();
}

bool PetAI::_needToStop()
{
    // This is needed for charmed creatures, as once their target was reset other effects can trigger threat
    if (me->IsCharmed() && me->GetVictim() == me->GetCharmer())
        return true;

    return !me->IsValidAttackTarget(me->GetVictim());
}

void PetAI::_stopAttack()
{
    if (!me->IsAlive())
    {
        TC_LOG_DEBUG("misc", "Creature stoped attacking cuz his dead [guid=%u]", me->GetGUID().GetCounter());
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MoveIdle();
        me->CombatStop();
        me->GetThreatManager().RemoveMeFromThreatLists();

        return;
    }

    me->AttackStop();
    me->InterruptNonMeleeSpells(false);
    me->SendMeleeAttackStop(); // Should stop pet's attack button from flashing
    me->GetCharmInfo()->SetIsCommandAttack(false);
    ClearCharmInfoFlags();
    HandleReturnMovement();
}

bool PetAI::IsTargetReachable(Unit const* target) const
{
    Unit* owner = me->GetOwner();
    if (!owner)
        return true;

    if (!owner->IsWithinDistInMap(target, 45.0f))
        return false;

    if (owner->IsMounted())
        return false;

    float attackDist = GetSpellAttackRange();
    if (attackDist)
    {
        if (!me->IsWithinDistInMap(target, attackDist))
            return false;

        if (!me->IsWithinLOSInMap(target))
            return false;
    }
    else
    {
        if (!me->IsWithinDistInMap(target, 40.0f))
            return false;

        PathGenerator p{ me };
        p.CalculatePath(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
        if (p.GetPathType() & (PATHFIND_NOPATH | PATHFIND_INCOMPLETE))
            return false;

        auto& path = p.GetPath();
        uint32 size = path.size();
        if (size < 2)
            return false;
        float dist = 0.0f;
        for (uint32 i = 1; i < size; ++i)
        {
            auto& begin = path[i - 1];
            auto& end = path[i];

            Position begPos{ begin.x, begin.y, begin.z };
            Position endPos{ end.x, end.y, end.z };
            dist += begPos.GetExactDist(&endPos);
        }
        float distMax = me->GetExactDist(target);
        distMax *= std::abs(me->GetPositionZ() - target->GetPositionZ()) > 10.0f ? 1.1f : 1.3f;
        if (dist > distMax)
            return false;
    }
    return true;
}

void PetAI::UpdateAI(uint32 diff)
{
    if (!me->IsAlive() || !me->GetCharmInfo())
        return;

    if (me->HasReactState(REACT_ASSIST))
        m_assist.Update(diff);

    Unit* owner = me->GetCharmerOrOwner();

    if (m_updateAlliesTimer <= diff)
        // UpdateAllies self set update timer
        UpdateAllies();
    else
        m_updateAlliesTimer -= diff;

    RestoreLastActionIfNeed();

    if (TryToCastDelayedSpell())
        return;

    if (me->GetVictim() && me->GetVictim()->IsAlive())
    {
        // is only necessary to stop casting, the pet must not exit combat
        if (me->GetVictim()->HasBreakableByDamageCrowdControlAura(me))
        {
            me->InterruptNonMeleeSpells(false);
            return;
        }

        if (_needToStop())
        {
            TC_LOG_DEBUG("misc", "Pet AI stopped attacking [guid=%u]", me->GetGUID().GetCounter());
            _stopAttack();
            return;
        }

        // Check before attacking to prevent pets from leaving stay position
        if (me->GetCharmInfo()->HasCommandState(COMMAND_STAY))
        {
            if (me->GetCharmInfo()->IsCommandAttack() || (me->GetCharmInfo()->IsAtStay() && me->IsWithinMeleeRange(me->GetVictim())))
                if (!m_pureCaster)
                    DoMeleeAttackIfReady();
        }
        else if (!m_pureCaster)
            DoMeleeAttackIfReady();
    }
    else
    {
        if (me->HasReactState(REACT_AGGRESSIVE) || me->GetCharmInfo()->IsAtStay())
        {
            // Every update we need to check targets only in certain cases
            // Aggressive - Allow auto select if owner or pet don't have a target
            // Stay - Only pick from pet or owner targets / attackers so targets won't run by
            //   while chasing our owner. Don't do auto select.
            // All other cases (ie: defensive) - Targets are assigned by AttackedBy(), OwnerAttackedBy(), OwnerAttacked(), etc.
            Unit* nextTarget = SelectNextTarget(me->HasReactState(REACT_AGGRESSIVE));

            if (nextTarget)
                AttackStart(nextTarget);
            else
                HandleReturnMovement();
        }
        else
            HandleReturnMovement();
    }

    // Autocast (casted only in combat or persistent spells in any state)
    if (!me->HasUnitState(UNIT_STATE_CASTING))
    {
        typedef std::vector<std::pair<Unit*, Spell*> > TargetSpellList;
        TargetSpellList targetSpellStore;

        for (uint8 i = 0; i < me->GetPetAutoSpellSize(); ++i)
        {
            uint32 spellID = me->GetPetAutoSpellOnPos(i);
            if (!spellID)
                continue;

            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellID);
            if (!spellInfo)
                continue;

            if (me->GetCharmInfo() && me->GetCharmInfo()->GetGlobalCooldownMgr().HasGlobalCooldown(spellInfo))
                continue;

            if (Pet* pet = me->ToPet())
            {
                if (pet->IsTemporary())
                {
                    /*
                    auto const* spells = dbc::GetSpecializetionSpells(pet->GetSpecializationId());
                    if (!spells)
                        continue;
                    if (std::find(spells->begin(), spells->end(), spellID) == spells->end())
                    */
                        continue;
                }
            }

            if (spellInfo->IsPositive())
            {
                if (spellInfo->CanBeUsedInCombat())
                {
                    // check spell cooldown
                    if (me->HasSpellCooldown(spellInfo->Id))
                        continue;

                    // Check if we're in combat or commanded to attack
                    if (!me->IsInCombat() && !me->GetCharmInfo()->IsCommandAttack())
                        continue;
                }

                Spell* spell = new Spell(me, spellInfo, TRIGGERED_NONE, ObjectGuid::Empty);
                bool spellUsed = false;

                // Some spells can target enemy or friendly (DK Ghoul's Leap)
                // Check for enemy first (pet then owner)
                Unit* target = me->getAttackerForHelper();
                if (!target && owner)
                    target = owner->getAttackerForHelper();

                if (target)
                {
                    if (CanAttack(target) && spell->CanAutoCast(target))
                    {
                        targetSpellStore.push_back(std::make_pair(target, spell));
                        spellUsed = true;
                    }
                }

                if (spellInfo->HasEffect(SPELL_EFFECT_JUMP_DEST))
                    continue; // Pets must only jump to target

                // No enemy, check friendly
                if (!spellUsed)
                {
                    for (std::set<ObjectGuid>::const_iterator tar = m_AllySet.begin(); tar != m_AllySet.end(); ++tar)
                    {
                        Unit* ally = ObjectAccessor::GetUnit(*me, *tar);

                        //only buff targets that are in combat, unless the spell can only be cast while out of combat
                        if (!ally)
                            continue;

                        if (spell->CanAutoCast(ally))
                        {
                            targetSpellStore.push_back(std::make_pair(ally, spell));
                            spellUsed = true;
                            break;
                        }
                    }
                }

                // No valid targets at all
                if (!spellUsed)
                    delete spell;
            }
            else if (me->GetVictim() && CanAttack(me->GetVictim()) && spellInfo->CanBeUsedInCombat())
            {
                Spell* spell = new Spell(me, spellInfo, TRIGGERED_NONE, ObjectGuid::Empty);
                if (spell->CanAutoCast(me->GetVictim()))
                    targetSpellStore.push_back(std::make_pair(me->GetVictim(), spell));
                else
                    delete spell;
            }
        }

        //found units to cast on to
        if (!targetSpellStore.empty())
        {
            uint32 index = urand(0, targetSpellStore.size() - 1);

            Spell* spell  = targetSpellStore[index].second;
            Unit*  target = targetSpellStore[index].first;

            targetSpellStore.erase(targetSpellStore.begin() + index);

            SpellCastTargets targets;
            targets.SetUnitTarget(target);

            if (!me->HasInArc(M_PI, target))
            {
                me->SetInFront(target);
                if (target && target->GetTypeId() == TYPEID_PLAYER)
                    me->SendUpdateToPlayer(target->ToPlayer());

                if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                    me->SendUpdateToPlayer(owner->ToPlayer());
            }

            spell->prepare(&targets);
        }

        // deleted cached Spell objects
        for (TargetSpellList::const_iterator itr = targetSpellStore.begin(); itr != targetSpellStore.end(); ++itr)
            delete itr->second;
    }
}

void PetAI::UpdateAllies()
{
    m_updateAlliesTimer = 10 * IN_MILLISECONDS;                 // update friendly targets every 10 seconds, lesser checks increase performance

    Unit* owner = me->GetCharmerOrOwner();
    if (!owner)
        return;

    Group* group = NULL;
    if (Player* player = owner->ToPlayer())
        group = player->GetGroup();

    //only pet and owner/not in group->ok
    if (m_AllySet.size() == 2 && !group)
        return;

    //owner is in group; group members filled in already (no raid -> subgroupcount = whole count)
    if (group && !group->isRaidGroup() && m_AllySet.size() == (group->GetMembersCount() + 2))
        return;

    m_AllySet.clear();
    m_AllySet.insert(me->GetGUID());
    if (group)                                              //add group
    {
        for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* Target = itr->GetSource();
            if (!Target || !group->SameSubGroup(owner->ToPlayer(), Target))
                continue;

            if (Target->GetGUID() == owner->GetGUID())
                continue;

            m_AllySet.insert(Target->GetGUID());
        }
    }
    else                                                    //remove group
        m_AllySet.insert(owner->GetGUID());
}

float PetAI::GetSpellAttackRange() const
{
    for (uint8 i = 0; i < me->GetPetAutoSpellSize(); ++i)
    {
        uint32 spellId = me->GetPetAutoSpellOnPos(i);
        if (!spellId)
            continue;
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
        if (!spellInfo)
            continue;

        if (spellInfo->IsPositive())
            continue;

        return std::floor(spellInfo->GetMaxRange(false, me));
    }
    return 0.0f;
}

void PetAI::KilledUnit(Unit* victim)
{
    // Called from Unit::Kill() in case where pet or owner kills something
    // if owner killed this victim, pet may still be attacking something else
    if (me->GetVictim() && me->GetVictim() != victim)
        return;

    // Clear target just in case. May help problem where health / focus / mana
    // regen gets stuck. Also resets attack command.
    // Can't use _stopAttack() because that activates movement handlers and ignores
    // next target selection
    me->AttackStop();
    me->InterruptNonMeleeSpells(false);
    me->SendMeleeAttackStop();  // Stops the pet's 'Attack' button from flashing

    // Before returning to owner, see if there are more things to attack
    if (Unit* nextTarget = SelectNextTarget(false))
        AttackStart(nextTarget);
    else
        HandleReturnMovement(); // Return
}

void PetAI::AttackStart(Unit* target)
{
    // Overrides Unit::AttackStart to correctly evaluate Pet states

    // Check all pet states to decide if we can attack this target
    if (!CanAttack(target))
        return;

    // Only chase if not commanded to stay or if stay but commanded to attack
    DoAttack(target, (!me->GetCharmInfo()->HasCommandState(COMMAND_STAY) || me->GetCharmInfo()->IsCommandAttack()));
}

void PetAI::OwnerAttackedBy(Unit* attacker)
{
    // Called when owner takes damage. This function helps keep pets from running off
    //  simply due to owner gaining aggro.

    if (!attacker)
        return;

    if (me->IsPet())
        for (auto&& spell : { SPELL_SUFFERING, SPELL_SEDUCTION, SPELL_MESMERIZE })
            if (me->ToPet()->IsAutoCastEnabled(spell))
                TryToCastSpell(attacker, spell);

    // Passive pets don't do anything
    if (me->HasReactState(REACT_PASSIVE))
        return;

    if (!IsTargetReachable(attacker))
        return;

    // Prevent pet from disengaging from current target
    if (me->GetVictim() && me->GetVictim()->IsAlive())
        return;

    // Continue to evaluate and attack if necessary
    AttackStart(attacker);
}

void PetAI::OwnerAttacked(Unit* target)
{
    // Called when owner attacks something. Allows defensive pets to know
    //  that they need to assist

    // Target might be NULL if called from spell with invalid cast targets
    if (!target)
        return;

    // Passive pets don't do anything
    if (me->HasReactState(REACT_PASSIVE))
        return;

    // Prevent pet from disengaging from current target
    if (me->GetVictim() && me->GetVictim()->IsAlive())
        return;

    // Continue to evaluate and attack if necessary
    AttackStart(target);
}

Unit* PetAI::SelectNextTarget(bool allowAutoSelect) const
{
    // Provides next target selection after current target death.
    // This function should only be called internally by the AI
    // Targets are not evaluated here for being valid targets, that is done in _CanAttack()
    // The parameter: allowAutoSelect lets us disable aggressive pet auto targeting for certain situations

    // Passive pets don't do next target selection
    if (me->HasReactState(REACT_PASSIVE))
        return NULL;

    // Check pet attackers first so we don't drag a bunch of targets to the owner
    if (Unit* myAttacker = me->getAttackerForHelper())
        if (!myAttacker->HasBreakableByDamageCrowdControlAura())
            return myAttacker;

    // Not sure why we wouldn't have an owner but just in case...
    if (!me->GetCharmerOrOwner())
        return NULL;

    // Check owner attackers
    if (Unit* ownerAttacker = me->GetCharmerOrOwner()->getAttackerForHelper())
        if (!ownerAttacker->HasBreakableByDamageCrowdControlAura())
            return ownerAttacker;

    // Check owner victim
    // 3.0.2 - Pets now start attacking their owners victim in defensive mode as soon as the hunter does
    if (Unit* ownerVictim = me->GetCharmerOrOwner()->GetVictim())
            return ownerVictim;

    // Neither pet or owner had a target and aggressive pets can pick any target
    // To prevent aggressive pets from chain selecting targets and running off, we
    //  only select a random target if certain conditions are met.
    if (me->HasReactState(REACT_AGGRESSIVE) && allowAutoSelect)
    {
        if (!me->GetCharmInfo()->IsReturning() || me->GetCharmInfo()->IsFollowing() || me->GetCharmInfo()->IsAtStay())
            if (Unit* nearTarget = me->ToCreature()->SelectNearestHostileUnitInAggroRange(true))
                return nearTarget;
    }

    // Default - no valid targets
    return NULL;
}

void PetAI::HandleReturnMovement()
{
    if (me->HasUnitState(UNIT_STATE_CASTING))
        return;

    // Handles moving the pet back to stay or owner

    // Prevent activating movement when under control of spells
    // such as "Eyes of the Beast"
    if (me->IsCharmed())
        return;

    if (me->GetCharmInfo()->HasCommandState(COMMAND_STAY) || me->GetCharmInfo()->HasCommandState(COMMAND_MOVE_TO))
    {
        if (!me->GetCharmInfo()->IsAtStay() && !me->GetCharmInfo()->IsReturning() && !me->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CONTROLLED))
        {
            // Return to previous position where stay was clicked
            float x, y, z;

            me->GetCharmInfo()->GetStayPosition(x, y, z);
            ClearCharmInfoFlags();
            me->GetCharmInfo()->SetIsReturning(true);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MovePoint(me->GetGUID().GetCounter(), x, y, z);
        }
    }
    else // COMMAND_FOLLOW
    {
        if (!me->GetCharmInfo()->IsFollowing() && !me->GetCharmInfo()->IsReturning() && !me->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CONTROLLED))
        {
            ClearCharmInfoFlags();
            me->GetCharmInfo()->SetIsReturning(true);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveFollow(me->GetCharmerOrOwner(), PET_FOLLOW_DIST, me->GetFollowAngle());
        }
    }
}

void PetAI::DoAttack(Unit* target, bool chase)
{
    // Handles attack with or without chase and also resets flags
    // for next update / creature kill

    float dist = 0.0f;
    if (m_pureCaster)
    {
        dist = GetSpellAttackRange();
        if (!dist)
            return;
    }

    if (me->Attack(target, true))
    {
        if (Unit* owner = me->GetOwner())
            owner->SetInCombatWith(target);

        // Play sound to let the player know the pet is attacking something it picked on its own
        if (me->HasReactState(REACT_AGGRESSIVE) && !me->GetCharmInfo()->IsCommandAttack())
            me->SendPetAIReaction(me->GetGUID());

        if (m_pureCaster && me->HasReactState(REACT_ASSIST) && me->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW) && !me->GetCharmInfo()->IsCommandAttack() && me->HasAura(63090)) // Glyph of Water Elemental
        {
            me->GetMotionMaster()->MoveFollow(me->GetOwner(), PET_FOLLOW_DIST, me->GetFollowAngle());
        }
        else if (chase)
        {
            bool oldCmdAttack = me->GetCharmInfo()->IsCommandAttack(); // This needs to be reset after other flags are cleared
            ClearCharmInfoFlags();
            me->GetCharmInfo()->SetIsCommandAttack(oldCmdAttack); // For passive pets commanded to attack so they will use spells
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveChase(target, dist);
        }
        else // (Stay && ((Aggressive || Defensive) && In Melee Range)))
        {
            ClearCharmInfoFlags();
            me->GetCharmInfo()->SetIsAtStay(true);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveIdle();
        }
    }
}

void PetAI::MovementInform(uint32 moveType, uint32 data)
{
    // Receives notification when pet reaches stay or follow owner
    switch (moveType)
    {
        case POINT_MOTION_TYPE:
        {
            // Pet is returning to where stay was clicked. data should be
            // pet's GUIDLow since we set that as the waypoint ID
            if (data == me->GetGUID().GetCounter() && me->GetCharmInfo()->IsReturning())
            {
                ClearCharmInfoFlags();
                me->GetCharmInfo()->SetIsAtStay(true);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();
            }
            break;
        }
        case FOLLOW_MOTION_TYPE:
        {
            // If data is owner's GUIDLow then we've reached follow point,
            // otherwise we're probably chasing a creature
            if (me->GetCharmerOrOwner() && me->GetCharmInfo() && data == me->GetCharmerOrOwner()->GetGUID().GetCounter() && me->GetCharmInfo()->IsReturning())
            {
                ClearCharmInfoFlags();
                me->GetCharmInfo()->SetIsFollowing(true);
            }
            break;
        }
        default:
            break;
    }
}

bool PetAI::CanAttack(Unit* target)
{
    // Evaluates wether a pet can attack a specific target based on CommandState, ReactState and other flags
    // IMPORTANT: The order in which things are checked is important, be careful if you add or remove checks

    // Hmmm...
    if (!target)
        return false;

    if (Unit* owner = me->GetOwner())
        if (owner->IsMounted())
            return false;

    if (!target->IsAlive())
    {
        // Clear target to prevent getting stuck on dead targets
        me->AttackStop();
        me->InterruptNonMeleeSpells(false);
        me->SendMeleeAttackStop();
        return false;
    }

    // Passive - passive pets can attack if told to
    if (me->HasReactState(REACT_PASSIVE))
        return me->GetCharmInfo()->IsCommandAttack();

    // CC - mobs under crowd control can be attacked if owner commanded
    if (target->HasBreakableByDamageCrowdControlAura())
        return me->GetCharmInfo()->IsCommandAttack();

    // Returning - pets ignore attacks only if owner clicked follow
    if (me->GetCharmInfo()->IsReturning())
        return !me->GetCharmInfo()->IsCommandFollow();

    // Stay - can attack if target is within range or commanded to
    if (me->GetCharmInfo()->HasCommandState(COMMAND_STAY))
        return (me->IsWithinMeleeRange(target) || me->GetCharmInfo()->IsCommandAttack());

    //  Pets attacking something (or chasing) should only switch targets if owner tells them to
    if (me->GetVictim() && me->GetVictim() != target)
    {
        // Check if our owner selected this target and clicked "attack"
        Unit* ownerTarget = NULL;
        if (Player* owner = me->GetCharmerOrOwner()->ToPlayer())
            ownerTarget = owner->GetSelectedUnit();
        else
            ownerTarget = me->GetCharmerOrOwner()->GetVictim();

        if (ownerTarget && me->GetCharmInfo()->IsCommandAttack())
            return (target->GetGUID() == ownerTarget->GetGUID());
    }

    // Follow
    if (me->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW))
        return !me->GetCharmInfo()->IsReturning();

    // default, though we shouldn't ever get here
    return false;
}

void PetAI::ReceiveEmote(Player* player, uint32 emote)
{
    if (me->GetOwnerGUID() && me->GetOwnerGUID() == player->GetGUID())
        switch (emote)
        {
            case TEXT_EMOTE_COWER:
                if (me->IsPet() && me->ToPet()->IsPetGhoul())
                    me->HandleEmoteCommand(/*EMOTE_ONESHOT_ROAR*/EMOTE_ONESHOT_OMNICAST_GHOUL);
                break;
            case TEXT_EMOTE_ANGRY:
                if (me->IsPet() && me->ToPet()->IsPetGhoul())
                    me->HandleEmoteCommand(/*EMOTE_ONESHOT_COWER*/EMOTE_STATE_STUN);
                break;
            case TEXT_EMOTE_GLARE:
                if (me->IsPet() && me->ToPet()->IsPetGhoul())
                    me->HandleEmoteCommand(EMOTE_STATE_STUN);
                break;
            case TEXT_EMOTE_SOOTHE:
                if (me->IsPet() && me->ToPet()->IsPetGhoul())
                    me->HandleEmoteCommand(EMOTE_ONESHOT_OMNICAST_GHOUL);
                break;
        }
}

void PetAI::SpellRequiresMovement(Unit* target, Spell * spell)
{
    if (m_delayedSpell)
    {
        m_delayedSpell->finish();
        delete m_delayedSpell;
        m_delayedSpell = nullptr;
    }
    ChaseUntilSpellCasted(target, spell);
}

void PetAI::OnPetCommand(CommandStates)
{
    ResetDelayedSpellIfNeed();
    m_lastAction.NeedToRestore = false;
}

void PetAI::ClearCharmInfoFlags()
{
    // Quick access to set all flags to FALSE

    CharmInfo* ci = me->GetCharmInfo();

    if (ci)
    {
        ci->SetIsAtStay(false);
        ci->SetIsCommandAttack(false);
        ci->SetIsCommandFollow(false);
        ci->SetIsFollowing(false);
        ci->SetIsReturning(false);
    }
}

void PetAI::AttackedBy(Unit* attacker)
{
    // Called when pet takes damage. This function helps keep pets from running off
    //  simply due to gaining aggro.

    if (!attacker)
        return;

    // Doesn't help I guess
    if (!me->GetCharmInfo())
        return;

    if (me->IsPet())
        for (auto&& spell : { SPELL_SHADOW_SHIELD, SPELL_VOID_SHIELD })
            if (me->ToPet()->IsAutoCastEnabled(spell))
                TryToCastSpell(me, spell);

    // Passive pets don't do anything
    if (me->HasReactState(REACT_PASSIVE))
        return;

    // Prevent pet from disengaging from current target
    if (me->GetVictim() && me->GetVictim()->IsAlive())
        return;

    if (!IsTargetReachable(attacker))
        return;

    // Continue to evaluate and attack if necessary
    AttackStart(attacker);
}

void PetAI::TryToCastSpell(Unit* target, uint32 spell)
{
    if (auto const* spellInfo = sSpellMgr->GetSpellInfo(spell))
    {
        Spell* spell = new Spell{ me, spellInfo, TRIGGERED_NONE };
        if (spell->CheckPetCast(target) != SPELL_CAST_OK)
            delete spell;
        else
        {
            SpellCastTargets targets;
            targets.SetUnitTarget(target);
            spell->prepare(&targets);
        }
    }
}

void PetAI::ChaseUntilSpellCasted(Unit* target, Spell* spell)
{
    CharmInfo* charmInfo = me->GetCharmInfo();

    // For the fuck !IsInWorld() is needed? Target can be out of world?
    if (!target || !target->IsInWorld() || !charmInfo)
    {
        spell->finish(false);
        delete spell;
        return;
    }

    switch (charmInfo->GetCommandState())
    {
        case COMMAND_STAY:
            m_lastAction.StayPosition.Relocate(me->GetPosition());
            m_lastAction.MovingToStayPosition = false;
            m_lastAction.Type = LastActionType::Stay;
            break;
        case COMMAND_FOLLOW:
            if (charmInfo->IsCommandAttack())
            {
                if (me->GetVictim())
                    m_lastAction.AttackingGuid = me->GetVictim()->GetGUID();
                m_lastAction.Type = LastActionType::Attack;
            }
            else
            {
                m_lastAction.Type = LastActionType::Follow;
            }
            break;
        default:
            break;
    }

    charmInfo->SetIsCommandAttack(false);
    charmInfo->SetIsAtStay(false);
    charmInfo->SetIsFollowing(false);
    charmInfo->SetIsCommandFollow(false);
    charmInfo->SetIsReturning(false);
    me->AttackStop();
    me->GetMotionMaster()->MoveChase(target, MELEE_RANGE);
    m_delayedSpell = spell;
    m_delayedSpellTarget = target->GetGUID();
}

void PetAI::ResetDelayedSpellIfNeed()
{
    if (m_delayedSpell)
    {
        m_delayedSpell->finish(false);
        delete m_delayedSpell;
        m_delayedSpell = nullptr;
        m_lastAction.NeedToRestore = false;
    }
}

bool PetAI::TryToCastDelayedSpell()
{
    if (!m_delayedSpell)
        return false;

    Unit* chaseTarget = ObjectAccessor::GetUnit(*me, m_delayedSpellTarget);
    if (!chaseTarget || !chaseTarget->IsAlive())
        return false;

    SpellCastResult result = m_delayedSpell->CheckPetCast(chaseTarget);

    switch (result)
    {
        case SPELL_CAST_OK:
        {
            auto targets = m_delayedSpell->m_targets;
            m_delayedSpell->prepare(&targets);
            m_delayedSpell = nullptr;

            m_lastAction.NeedToRestore = true;

            CharmInfo *charmInfo = me->GetCharmInfo();
            // We cast some spell that causes movement, need SetIsAtStay(false) or pet will hang in PetAI::HandleReturnMovement
            charmInfo->SetIsAtStay(me->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE);
            charmInfo->SetIsCommandAttack(false);
            charmInfo->SetIsCommandFollow(false);
            charmInfo->SetIsFollowing(false);
            charmInfo->SetIsReturning(false);
            break;
        }
        case SPELL_FAILED_OUT_OF_RANGE:
        case SPELL_FAILED_LINE_OF_SIGHT:
            break;
        default:
            return false;
    }
    return true;
}

void PetAI::RestoreLastActionIfNeed()
{
    if (!m_lastAction.NeedToRestore)
        return;

    if (me->HasUnitState(UNIT_STATE_CASTING))
        return;

    CharmInfo* charmInfo = me->GetCharmInfo();
    if (!charmInfo)
        return;

    switch (m_lastAction.Type)
    {
        case LastActionType::Stay:
        {
            if (!m_lastAction.MovingToStayPosition)
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != IDLE_MOTION_TYPE)
                    return;

                me->GetMotionMaster()->MovePoint(0, m_lastAction.StayPosition);
                m_lastAction.MovingToStayPosition = true;
            }
            else
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != IDLE_MOTION_TYPE)
                    return;

                me->AttackStop();
                me->InterruptNonMeleeSpells(false);
                me->StopMoving();
                me->GetMotionMaster()->Clear(false);
                me->GetMotionMaster()->MoveIdle();

                charmInfo->SetIsCommandAttack(false);
                charmInfo->SetIsAtStay(true);
                charmInfo->SetIsCommandFollow(false);
                charmInfo->SetIsFollowing(false);
                charmInfo->SetIsReturning(false);
                charmInfo->SetCommandState(COMMAND_STAY);
                charmInfo->SaveStayPosition();
                m_lastAction.MovingToStayPosition = false;
            }
            break;
        }
        case LastActionType::Follow:
        {
            if (!me->movespline->Finalized())
                return;

            me->AttackStop();
            me->InterruptNonMeleeSpells(false);
            if (Unit* owner = me->GetOwner())
            {
                me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle());

                charmInfo->SetIsCommandAttack(false);
                charmInfo->SetIsAtStay(false);
                charmInfo->SetIsReturning(true);
                charmInfo->SetIsCommandFollow(true);
                charmInfo->SetIsFollowing(false);
                charmInfo->SetCommandState(COMMAND_FOLLOW);
            }
            break;
        }
        case LastActionType::Attack:
            if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == EFFECT_MOTION_TYPE)
                return;

            if (Unit* target = ObjectAccessor::GetUnit(*me, m_lastAction.AttackingGuid))
            {
                charmInfo->SetIsCommandAttack(true);
                charmInfo->SetIsAtStay(false);
                charmInfo->SetIsFollowing(false);
                charmInfo->SetIsCommandFollow(false);
                charmInfo->SetIsReturning(false);
                AttackStart(target);
            }
            m_lastAction.AttackingGuid.Clear();
            break;
        default:
            break;
    }

    m_lastAction.NeedToRestore = false;
}
