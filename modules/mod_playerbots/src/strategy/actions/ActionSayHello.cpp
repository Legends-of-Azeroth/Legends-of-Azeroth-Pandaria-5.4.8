#include "ActionSayHello.h"

#include "PlayerbotAI.h"
#include "Player.h"
#include "Log.h"

SayHelloAction::SayHelloAction(PlayerbotAI* ai)
    : Action(ai, "say hello")
{
}

bool SayHelloAction::Execute(Event event)
{
    botAI->GetBot()->Say("Hello !", Language::LANG_UNIVERSAL);
    return true;
}