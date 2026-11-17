#ifndef _PLAYERBOT_MAGEAIOBJECTCONTEXT_H
#define _PLAYERBOT_MAGEAIOBJECTCONTEXT_H

#include "AiObjectContext.h"

class PlayerbotAI;

class MageAiObjectContext : public AiObjectContext
{
public:
    MageAiObjectContext(PlayerbotAI* botAI);
};

#endif
