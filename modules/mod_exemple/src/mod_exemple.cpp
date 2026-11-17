/*
** Made by Atidote https://github.com/Atidot3
*/

#include "Common.h"
#include "SpellDefines.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "World.h"
#include "Chat.h"

class exemple_announce : public PlayerScript
{
public:
    exemple_announce() : PlayerScript("exemple_announce") {}

    void OnLogin(Player* player) override
    {
        // Announce Module
        ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00exemple announce |rmodule.");
    }

};


extern void AddSC_mod_exemple()
{
    new exemple_announce();
}
