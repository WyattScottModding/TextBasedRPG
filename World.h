#pragma once
#include <map>

#include "Character.h"

enum ENCOUNTER
{
    TREASURE,
    ENEMY,
    FRIEND,
    BATTLE,
    TRAVEL,
    EVENT
};


class World
{
public:
    static Character *player;
    static std::map<std::string, std::string> areas;
    static std::vector<std::string> decisions;
    static std::vector<std::tuple<ENCOUNTER, std::string>> encounters;
public:
    static void PlayGame();
    static void PlayerCreation();

    static std::pair<std::string, std::string> GenerateNewArea(Character *character);
    static std::vector<std::string> GetDecisionsForArea(const std::string &area, Character *character);
    static std::vector<std::string> GetOptionsFromResponse(const std::string &response);
    static std::string MakeDecision(Character *character, const std::string &option, const std::string& area);
};
