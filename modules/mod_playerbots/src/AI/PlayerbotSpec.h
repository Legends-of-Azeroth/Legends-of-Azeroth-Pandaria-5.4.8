#ifndef _PLAYERBOT_PLAYERBOTSPEC_H
#define _PLAYERBOT_PLAYERBOTSPEC_H

using uint32 = unsigned int;

class Player;
class PlayerBotSpec
{
public:
    // 
    static bool IsTank(Player* player, bool bySpec = false);
    static bool IsHeal(Player* player, bool bySpec = false);
    static bool IsDps(Player* player, bool bySpec = false);
    static bool IsRanged(Player* player, bool bySpec = false);
    static bool IsMelee(Player* player, bool bySpec = false);
    static bool IsCaster(Player* player, bool bySpec = false);
    static bool IsCombo(Player* player, bool bySpec = false);
    static bool IsRangedDps(Player* player, bool bySpec = false);
    static uint32 GetGroupTankNum(Player* player);
    static bool IsMainTank(Player* player);
    static bool IsAssistTank(Player* player);
    static bool IsAssistTankOfIndex(Player* bot, Player* player, int index);
    static unsigned int GetSpectab(Player* player);
};

#endif