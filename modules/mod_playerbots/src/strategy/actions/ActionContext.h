#ifndef _PLAYERBOT_ACTIONCONTEXT_H
#define _PLAYERBOT_ACTIONCONTEXT_H

#include "AttackActions.h"
#include "ActionSayHello.h"
#include "AutoMaintenanceOnLevelupAction.h"
#include "ChooseTargetActions.h"
#include "FollowActions.h"
#include "GenericActions.h"
#include "PositionAction.h"
#include "RtiAction.h"
#include "MovementActions.h"
#include "RandomBotUpdateAction.h"
#include "ReachTargetActions.h"
#include "StayActions.h"

class PlayerbotAI;
class ActionContext : public NamedObjectContext<Action>
{
public:
    ActionContext()
    {
        creators["say hello"] = &ActionContext::hello;
        creators["move random"] = &ActionContext::move_random;

        creators["follow"] = &ActionContext::follow;
        creators["flee"] = &ActionContext::flee;
        creators["move from group"] = &ActionContext::move_from_group;
        creators["flee to master"] = &ActionContext::flee_to_master;
        creators["guard"] = &ActionContext::guard;
        creators["runaway"] = &ActionContext::runaway;
        creators["stay"] = &ActionContext::stay;
        creators["sit"] = &ActionContext::sit;

        creators["random bot update"] = &ActionContext::random_bot_update;
        creators["mark rti"] = &ActionContext::mark_rti;
        creators["attack rti target"] = &ActionContext::attack_rti_target;
        creators["auto maintenance on levelup"] = &ActionContext::auto_maintenance_on_levelup;

        creators["tank face"] = &ActionContext::tank_face;
        creators["tank assist"] = &ActionContext::tank_assist;
        creators["set facing"] = &ActionContext::set_facing;
        creators["drop target"] = &ActionContext::drop_target;
        creators["dps assist"] = &ActionContext::dps_assist;
        creators["dps aoe"] = &ActionContext::dps_aoe;
        creators["attack"] = &ActionContext::melee;
        creators["melee"] = &ActionContext::melee;
        creators["attack anything"] = &ActionContext::attack_anything;

        creators["toggle pet spell"] = &ActionContext::toggle_pet_spell;
        creators["pet attack"] = &ActionContext::pet_attack;

        creators["reach spell"] = &ActionContext::ReachSpell;
        creators["reach melee"] = &ActionContext::ReachMelee;
        creators["reach party member to heal"] = &ActionContext::reach_party_member_to_heal;
        creators["reach party member to resurrect"] = &ActionContext::reach_party_member_to_resurrect;

        //creators["invite nearby"] = &ActionContext::invite_nearby;
        //creators["invite guild"] = &ActionContext::invite_guild;
    }

private:
    static Action* hello(PlayerbotAI* botAI) { return new SayHelloAction(botAI); }
    static Action* move_random(PlayerbotAI* botAI) { return new MoveRandomAction(botAI); }

    static Action* follow(PlayerbotAI* botAI) { return new FollowAction(botAI); }
    static Action* flee(PlayerbotAI* botAI) { return new FleeAction(botAI); }
    static Action* move_from_group(PlayerbotAI* botAI) { return new MoveFromGroupAction(botAI); }
    static Action* flee_to_master(PlayerbotAI* botAI) { return new FleeToMasterAction(botAI); }
    static Action* guard(PlayerbotAI* botAI) { return new GuardAction(botAI); }
    static Action* stay(PlayerbotAI* botAI) { return new StayAction(botAI); }
    static Action* sit(PlayerbotAI* botAI) { return new SitAction(botAI); }
    static Action* runaway(PlayerbotAI* botAI) { return new RunAwayAction(botAI); }

    static Action* random_bot_update(PlayerbotAI* botAI) { return new RandomBotUpdateAction(botAI); }
    static Action* mark_rti(PlayerbotAI* botAI) { return new MarkRtiAction(botAI); }
    static Action* attack_rti_target(PlayerbotAI* botAI) { return new AttackRtiTargetAction(botAI); }
    static Action* auto_maintenance_on_levelup(PlayerbotAI* botAI) { return new AutoMaintenanceOnLevelupAction(botAI); }

    static Action* tank_face(PlayerbotAI* botAI) { return new TankFaceAction(botAI); }
    static Action* tank_assist(PlayerbotAI* botAI) { return new TankAssistAction(botAI); }
    static Action* set_facing(PlayerbotAI* botAI) { return new SetFacingTargetAction(botAI); }
    static Action* drop_target(PlayerbotAI* botAI) { return new DropTargetAction(botAI); }
    static Action* dps_assist(PlayerbotAI* botAI) { return new DpsAssistAction(botAI); }
    static Action* dps_aoe(PlayerbotAI* botAI) { return new DpsAoeAction(botAI); }
    static Action* melee(PlayerbotAI* botAI) { return new MeleeAction(botAI); }
    static Action* attack_anything(PlayerbotAI* botAI) { return new AttackAnythingAction(botAI); }

    static Action* toggle_pet_spell(PlayerbotAI* ai) { return new TogglePetSpellAutoCastAction(ai); }
    static Action* pet_attack(PlayerbotAI* ai) { return new PetAttackAction(ai); }

    static Action* ReachSpell(PlayerbotAI* botAI) { return new ReachSpellAction(botAI); }
    static Action* ReachMelee(PlayerbotAI* botAI) { return new ReachMeleeAction(botAI); }
    static Action* reach_party_member_to_heal(PlayerbotAI* botAI) { return new ReachPartyMemberToHealAction(botAI); }
    static Action* reach_party_member_to_resurrect(PlayerbotAI* botAI) { return new ReachPartyMemberToResurrectAction(botAI); }
};

#endif
