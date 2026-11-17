#ifndef _SAY_HELLO_ACTION_H_
#define _SAY_HELLO_ACTION_H_

#include "Action.h"

class PlayerbotAI;
class SayHelloAction : public Action
{
public:
    SayHelloAction(PlayerbotAI* ai);

    bool Execute(Event event) override;
};

#endif