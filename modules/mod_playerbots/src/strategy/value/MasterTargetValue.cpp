#include "MasterTargetValue.h"

#include "Playerbots.h"

Unit* MasterTargetValue::Calculate() { return botAI->GetGroupMaster(); }
