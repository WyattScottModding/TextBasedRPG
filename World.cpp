#include "World.h"

#include <algorithm>
#include <iostream>
#include <ostream>
#include <string>

#include "Character.h"
#include "TextBasedRPG.h"

using namespace std;

Character *World::player;
std::map<std::string, std::string> World::areas;
std::vector<std::string> World::decisions;
std::vector<std::tuple<ENCOUNTER, std::string>> World::encounters;

void World::PlayGame()
{
    pair<string, string> area = World::GenerateNewArea(player);
    cout << area.second << endl << endl;
    
    while (true)
    {
        cout << "The player now has the following decisions: " << endl;
        vector<string> options = GetDecisionsForArea(area.first, player);

        for (int i = 0; i < options.size(); i++)
        {
            cout << (i + 1) << ". " << options[i] << endl;
        }

        int choice;
        do
        {
            cout << "Enter a number (1-" << options.size() << "): ";
            cin >> choice;

            if (choice < 1 || choice > options.size())
            {
                cout << "Invalid choice" << endl << endl;
            }
            else
            {
                break;
            }
        }
        while (true);

        decisions.push_back(options[choice - 1]);

        cout << endl;

        if (options[choice - 1].find("Travel") != string::npos)
        {
            area = World::GenerateNewArea(player);
            cout << area.second << endl << endl;
        }
        else
        {
            cout << MakeDecision(player, options[choice - 1], area.first) << endl << endl;
        }
    }
}

void World::PlayerCreation()
{
    cout << "What should we call you?" << endl;
    string playerName;
    getline(cin, playerName);

    cout << "What is your gender?" << endl;
    string playerGender;
    getline(cin, playerGender);

    cout << "Select your race:\n";
    cout << "1. Human\n2. Elf\n3. Half-Elf\n4. Gnome\n";
    int raceChoice;
    do
    {
        cout << "Enter a number (1-4): ";
        cin >> raceChoice;

        if (raceChoice < 1 || raceChoice > 4)
        {
            cout << "Invalid Input" << endl << endl;
        }
        else
        {
            break;
        }
    } while (true);
    RACE characterRace = static_cast<RACE>(raceChoice - 1);
    
    cout << "Select your class:\n";
    cout << "1. Wizard\n2. Druid\n3. Monk\n4. Warrior\n5. Archer\n";
    int classChoice;
    do
    {
        cout << "Enter a number (1-5): ";
        cin >> classChoice;

        if (classChoice < 1 || classChoice > 5)
        {
            cout << "Invalid Input" << endl << endl;
        }
        else
        {
            break;
        }
    } while (true);
    CLASS characterClass = static_cast<CLASS>(classChoice - 1);

    Stats stats = { 8, 8, 8, 8, 8, 8};

    int pointsToAssign = 30;
    cout << "You have 30 points to distribute among your stats.  Each stat starts at 8 and cannot exceed 18.\n";

    while (pointsToAssign > 0)
    {
        cout << "Remaining Points: " << pointsToAssign << endl;
        cout << "1. Strength " << stats.Strength << "\n2. Dexterity " << stats.Dexterity << "\n3. Constitution " << stats.Constitution << "\n4. Intelligence " << stats.Intelligence << "\n5. Wisdom " << stats.Wisdom << "\n6. Charisma " << stats.Charisma << endl;

        int statChoice, points;
        cout << "Choose a stat to increase (1-6): ";
        cin >> statChoice;

        if (statChoice < 1 || statChoice > 6)
        {
            cout << "Invalid choice.  Please select a valid stat" << endl;
            continue;
        }

        cout << "Enter poitns to assign (max " << pointsToAssign << "): ";
        cin >> points;

        if (points < 1 || points > pointsToAssign)
        {
            cout << "Invalid point value." << endl;
            continue;
        }

        int *statArray[] = {&stats.Strength, &stats.Dexterity, &stats.Constitution, &stats.Intelligence, &stats.Wisdom, &stats.Charisma};

        if (*statArray[statChoice - 1] + points > 18)
        {
            cout << "Cannot exceed stat cap of 18.\n";
            continue;
        }

        *statArray[statChoice - 1] += points;
        pointsToAssign -= points;

        cout << endl;
    }

    World::player = new Character(playerName, playerGender, characterClass, characterRace, stats);
}

std::pair<std::string, std::string> World::GenerateNewArea(Character* character)
{
    std::string prompt = "Generate a new area for this player: (" + character->CharacterToString() + 
                         ") to explore. Describe the surroundings in detail and include information about how dangerous the area is. " +
                         "Make it engaging and fitting for a Dungeons and Dragons game. " +
                         "You will take the role of a narrator talking about what the player is doing. " +
                         "In your response you must first state the name of the area, followed by a period, then followed by the description." +
                         "Here is a list of existing areas, so you should not generate any area with these names: (";

    for (const auto& a : areas)
    {
        prompt += a.first + ", ";
    }
    
    prompt += ")";
    
    std::string response = TextBasedRPG::AskChatGPT(prompt, 0.9f);

    response = response.substr(0, response.rfind('.') + 1);

    size_t periodPosition = response.find('.');

    pair<string, string> area = make_pair<string, string>(response.substr(0, periodPosition), response.substr(periodPosition + 1));
    areas.insert(area);

    player->Location = area.first;

    return area;
}

std::vector<std::string> World::GetDecisionsForArea(const std::string &area, Character *character)
{
    std::vector<std::string> vec;

    while (vec.size() == 0)
    {
        std::string prompt = "Provide 3-5 distinct and concise options for actions the player can take, separated by a period, question mark, or exclamation point. "
                            "Do not number the options or provide any additional information. These could be dialog options with a friend or enemy, attack options, or movement options. "
                            "If you give the option for the player to move to a new area, you must start the response with the word Travel. "
                            "The player's details are: (" +
                            character->CharacterToString() +
                            "). They are in an area described as: (" +
                            areas[area] +
                            "). Ensure the options are brief and clear.";

    
       vec = World::GetOptionsFromResponse(TextBasedRPG::AskChatGPT(prompt, 0.8f));
    }
    
    return vec;
}

std::string trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\n\r,;/");
    size_t end = str.find_last_not_of(" \t\n\r,;/");

    if (start == std::string::npos || end == std::string::npos)
        return ""; // String is entirely whitespace or empty

    return str.substr(start, end - start + 1);
}

std::vector<std::string> World::GetOptionsFromResponse(const std::string& r)
{
    string response = r;
    vector<string> options;

    while (!response.empty())
    {
        // Find the next period, question mark, or exclamation point
        size_t index = response.find_first_of(".?!");

        if (index == string::npos)
            break;

        string option = trim(response.substr(0, index + 1));

        // Validate the option, ensure it is not empty and has at least a few words
        if (!option.empty() && option.find(' ') != string::npos)
        {
            options.push_back(option);
        }

        if (response.length() > index + 1)
            response = response.substr(index + 1);
        else
        {
            response.clear();
        }

        response = trim(response);
    }

    // Remove duplicate options
    sort(options.begin(), options.end());
    options.erase(unique(options.begin(), options.end()), options.end());

    return options;
}

std::string World::MakeDecision(Character* character, const std::string& option, const std::string& area)
{
    string prompt = "The player (" + character->CharacterToString() +
                    ") has decided to take this option: " + option +
                    " in this area: " + areas[area] +
                    ". What happens to this player next? They can find treasure, find an enemy, find a friend, or a special event can happen based on what the area is like. "
                    " if they are in battle then the action the player decided must occur and the enemy must react. "
                    "Use the player's melee value to determine how much physical damage they do and their mana to determine how much magical damage they do. "
                    "Their items can also come into play as well. "
                    "In your response the first word needs to start with Treasure, Enemy, Friend, Battle, or Event followed by a period."
                    "If the player has found treasure, the treasure item needs to be listed right after the word Treasure. Items can have enchanted abilities, or be plain weapons or armor.";

    string response = trim(TextBasedRPG::AskChatGPT(prompt, 0.85f));

    size_t index = response.find('.');
    string encounterString = response.substr(0, index);
    ENCOUNTER encounter = EVENT;
    response = response.substr(index + 1);

    if (encounterString._Equal("Treasure"))
    {
        character->items.push_back(response);
    }
    else if (encounterString._Equal("Enemy"))
    {
        encounter = ENEMY;
    }
    else if (encounterString._Equal("Friend"))
    {
        encounter = FRIEND;
    }
    else if (encounterString._Equal("Battle"))
    {
        encounter = BATTLE;
    }

    encounters.push_back(make_tuple(encounter, response));

    return response;
    
}