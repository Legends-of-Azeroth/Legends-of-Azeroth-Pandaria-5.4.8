-- QUEST_SPECIAL_FLAGS_project_DAILY_QUEST = 0x01000000

delete
    quest_template_addon,
    quest_template
from quest_template_addon
         inner join quest_template on quest_template.ID = quest_template_addon.ID
where quest_template_addon.SpecialFlags & 0x01000000 > 0;

SET @ENTRY := 40000;

delete from quest_objective where questId >= @ENTRY;
delete from quest_objectives_locale where QuestId >= @ENTRY;
delete from quest_offer_reward where ID >= @ENTRY;
delete from quest_offer_reward_locale where ID >= @ENTRY;
delete from quest_poi where QuestID >= @ENTRY;
delete from quest_poi_points where QuestID >= @ENTRY;
delete from quest_request_items where ID >= @ENTRY;
delete from quest_request_items_locale where ID >= @ENTRY;
delete from quest_template_locale where ID >= @ENTRY;
delete from creature_questender where quest >= @ENTRY;
delete from creature_queststarter where quest >= @ENTRY;
