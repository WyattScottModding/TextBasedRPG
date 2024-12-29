#pragma once
#include <string>
#include <vector>

enum CLASS
{
    WIZARD,
    DRUID,
    MONK,
    WARRIER,
    ARCHER
};

enum RACE
{
    HUMAN,
    ELF,
    HALF_ELF,
    GNOME
};

struct Stats
{
    int Strength;
    int Dexterity;
    int Constitution;
    int Intelligence;
    int Wisdom;
    int Charisma;
};

class Character
{
public:
    std::string Name;
    std::string Gender;
    std::string Location;

    CLASS characterClass;
    RACE characterRace;

    Stats stats;

    float health;
    float mana;
    float meleeDamage;

    std::vector<std::string> items;

public:
    Character();
    Character(std::string Name, std::string Gender, CLASS characterClass, RACE characterRace, Stats stats);

    void RandomlySelectClassAndRace();
    void RandomlyAllocateStats();
    int GetStatValue(int statIndex);
    void AddPointsToStat(int statIndex, int points);
    void InitializeAttributes();
    
    std::string CharacterToString() const;
    std::string CharacterToStringFormatted() const;
    std::string ClassToString() const;
    std::string RaceToString() const;

    std::string RandomFirstName();
    std::string RandomLastName();
};
