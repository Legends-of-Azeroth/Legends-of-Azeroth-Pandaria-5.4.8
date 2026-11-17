#include "ActiveSpellValue.h"

#include "Playerbots.h"

uint32 ActiveSpellValue::Calculate()
{
    Player* bot = botAI->GetBot();
    for (uint8 type = CURRENT_MELEE_SPELL; type <= CURRENT_CHANNELED_SPELL; ++type)
    {
        if (Spell* spell = bot->GetCurrentSpell((CurrentSpellTypes)type))
        {
            if (spell->m_spellInfo)
            {
                return spell->m_spellInfo->Id;
            }
        }
    }

    return 0;
}
