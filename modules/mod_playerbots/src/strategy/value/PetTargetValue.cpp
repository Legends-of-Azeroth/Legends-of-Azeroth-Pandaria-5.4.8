#include "PetTargetValue.h"

#include "Playerbots.h"

Unit* PetTargetValue::Calculate() { return botAI->GetBot()->GetPet(); }
