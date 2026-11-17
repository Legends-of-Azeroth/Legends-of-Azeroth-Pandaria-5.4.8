#include "FollowMasterStrategy.h"

#include "Playerbots.h"

NextAction** FollowMasterStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("follow", 1.0f), nullptr);
}