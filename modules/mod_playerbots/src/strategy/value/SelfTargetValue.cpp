#include "SelfTargetValue.h"

#include "Playerbots.h"

Unit* SelfTargetValue::Calculate() { return botAI->GetBot(); }