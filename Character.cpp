#include "Character.h"

#include <random>
#include <curl/curl.h>

using namespace std;

Character::Character()
{
    RandomlySelectClassAndRace();
    InitializeAttributes();

    stats = { 8, 8, 8, 8, 8, 8};
    
    RandomlyAllocateStats();

    this->Name = RandomFirstName() + " " + RandomLastName();
}

Character::Character(std::string Name, std::string Gender, CLASS characterClass, RACE characterRace, Stats stats)
{
    this->Name = Name;
    this->Gender = Gender;
    this->characterClass = characterClass;
    this->characterRace = characterRace;
    this->stats = stats;

    InitializeAttributes();
}

void Character::RandomlySelectClassAndRace()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> classDistrib(0, 4); // For the range of classes
    characterClass = static_cast<CLASS>(classDistrib(gen));

    uniform_int_distribution<> raceDistrib(0, 3); // For the range of races
    characterRace = static_cast<RACE>(raceDistrib(gen));

    uniform_int_distribution<> genderDistrib(0, 2); // For the range of genders
    Gender = genderDistrib(gen) == 0 ? "Male" : "Female";
}

void Character::RandomlyAllocateStats()
{
    int remainingPoints = 30;

    if (characterClass == WIZARD)
    {
        stats.Intelligence += 4;
        remainingPoints -= 4;
    }
    else if (characterClass == WARRIER)
    {
        stats.Strength += 4;
        remainingPoints -= 4;
    }
    else if (characterClass == MONK || characterClass == DRUID)
    {
        stats.Wisdom += 4;
        remainingPoints -= 4;
    }
    

    if (characterRace == ELF)
    {
        stats.Intelligence += 4;
        remainingPoints -= 4;
    }
    else if (characterRace == HALF_ELF)
    {
        stats.Intelligence += 2;
        stats.Charisma += 2;
        remainingPoints -= 4;
    }
    else if (characterRace == HUMAN)
    {
        stats.Charisma += 4;
        remainingPoints -= 4;
    }
    else if (characterRace == GNOME)
    {
        stats.Wisdom += 4;
        remainingPoints -= 4;
    }

    // Randomly distribute the rest of the points
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, 10);

    while (remainingPoints > 0)
    {
        int statIndex = distrib(gen) % 6; // Chose one of the 6 stats
        int maxIncrease = 18 - GetStatValue(statIndex);

        int pointsToAdd = min(remainingPoints, maxIncrease);
        AddPointsToStat(statIndex, pointsToAdd);
        remainingPoints -= pointsToAdd;
    }
}

// Helper function to get a stat value by index
int Character::GetStatValue(int statIndex) {
    switch (statIndex) {
    case 0: return stats.Strength;
    case 1: return stats.Dexterity;
    case 2: return stats.Constitution;
    case 3: return stats.Intelligence;
    case 4: return stats.Wisdom;
    case 5: return stats.Charisma;
    default: return 8; // Default value for unknown stat
    }
}

void Character::AddPointsToStat(int statIndex, int points) {
    switch (statIndex) {
    case 0: stats.Strength += points; break;
    case 1: stats.Dexterity += points; break;
    case 2: stats.Constitution += points; break;
    case 3: stats.Intelligence += points; break;
    case 4: stats.Wisdom += points; break;
    case 5: stats.Charisma += points; break;
    }
}


void Character::InitializeAttributes()
{
    if (characterRace == HUMAN)
    {
        health = 50;
        mana = 50;
        meleeDamage = 6;
    }
    else if (characterRace == ELF)
    {
        health = 30;
        mana = 70;
        meleeDamage = 8;
    }
    else if (characterRace == HALF_ELF)
    {
        health = 40;
        mana = 60;
        meleeDamage = 7;
    }
    else if (characterRace == GNOME)
    {
        health = 20;
        mana = 80;
        meleeDamage = 3;
    }

    health += static_cast<float>(stats.Constitution);
    meleeDamage += static_cast<float>(stats.Strength);

    if (characterClass == WIZARD)
    {
        mana += 50;
    }
    else if (characterClass == WARRIER)
    {
        health += 30;
        meleeDamage +=2;
    }
}

std::string Character::CharacterToString() const
{
    string s = "Character Name: " + this->Name
    + ". Gender: " + this->Gender
    + ". Is at the location: " + this->Location
    + ". Class: " + ClassToString()
    + ". Race: " + RaceToString()
    + ". Health: " + to_string(this->health)
    + ". Mana: " + to_string(this->mana)
    + ". Melee Damage: " + to_string(this->meleeDamage)
    + ". Stats: Strength = " + to_string(this->stats.Strength)
    + ", Dexterity = " + to_string(this->stats.Dexterity)
    + ", Constitution = " + to_string(this->stats.Constitution)
    + ", Intelligence = " + to_string(this->stats.Intelligence)
    + ", Wisdom = " + to_string(this->stats.Wisdom)
    + ", Charisma = " + to_string(this->stats.Charisma)
    + ". Items = ";

    for (int i = 0; i < this->items.size(); i++)
    {
        s += items[i];

        if (i < this->items.size() - 1)
            s += ",";
    }

    return s;
}

std::string Character::CharacterToStringFormatted() const
{
    string s = "Character Name: " + this->Name + ". \n"
    + "Gender: " + this->Gender + ". \n"
    + "Is at the location: " + this->Location + ". \n"
    + "Class: " + ClassToString() + ". \n"
    + "Race: " + RaceToString() + ". \n"
    + "Health: " + to_string(this->health) + ". \n"
    + "Mana: " + to_string(this->mana) + ". \n"
    + "Melee Damage: " + to_string(this->meleeDamage) + ". \n"
    + "Stats: Strength = " + to_string(this->stats.Strength)
    + ", Dexterity = " + to_string(this->stats.Dexterity)
    + ", Constitution = " + to_string(this->stats.Constitution)
    + ", Intelligence = " + to_string(this->stats.Intelligence)
    + ", Wisdom = " + to_string(this->stats.Wisdom)
    + ", Charisma = " + to_string(this->stats.Charisma) + ". \n"
    + "Items = ";

    for (int i = 0; i < this->items.size(); i++)
    {
        s += items[i];

        if (i < this->items.size() - 1)
            s += ",";
    }

    return s;
}

std::string Character::ClassToString() const
{
    switch (characterClass)
    {
    case WIZARD:
        return "WIZARD";
    case DRUID:
        return "DRUID";
    case MONK:
        return "MONK";
    case WARRIER:
        return "WARRIER";
    case ARCHER:
        return "ARCHER";
    }
    return "";
}

std::string Character::RaceToString() const
{
    switch (characterRace)
    {
    case HUMAN:
        return "HUMAN";
    case ELF:
        return "ELF";
    case HALF_ELF:
        return "HALF_ELF";
    case GNOME:
        return "GNOME";
    }
    return "";
}

std::string Character::RandomFirstName()
{
    // List of possible first names
    std::vector<std::string> firstNames = {
        "Aelar", "Thalindra", "Aric", "Lirael", "Bren", 
        "Eldrin", "Kaelen", "Mira", "Dorian", "Liora"
    };
        
    // Randomly select a first name
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, firstNames.size() - 1);
    return firstNames[dist(gen)];
}

std::string Character::RandomLastName()
{
    // List of possible last names
    std::vector<std::string> lastNames = {
        "Silverleaf", "Stormrider", "Brightblade", "Darkwater", "Ironfoot",
        "Moonshadow", "Fireheart", "Stonefist", "Dawnwhisper", "Thornbriar"
    };
        
    // Randomly select a last name
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, lastNames.size() - 1);
    return lastNames[dist(gen)];
}