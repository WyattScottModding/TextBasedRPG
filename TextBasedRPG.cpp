#include "TextBasedRPG.h"
#define CURL_STATICLIB
#include <iostream>
#include <ostream>
#include <curl/curl.h>

#include "World.h"

using namespace std;

int main(int argc, char* argv[])
{
    TextBasedRPG::NewGame();
    return 0;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void TextBasedRPG::NewGame()
{
    World::PlayerCreation();
    cout << endl;
    cout << "Added the player: \n" << World::player->CharacterToStringFormatted() << endl << endl;
    World::PlayGame();
}

std::string TextBasedRPG::ExtractContentFromResponse(const std::string& response)
{
    size_t choicesStart = response.find("\"choices\":");
    if (choicesStart != std::string::npos)
    {
        size_t messageStart = response.find("\"message\":", choicesStart);
        if (messageStart != std::string::npos)
        {
            size_t contentStart = response.find("\"content\":", messageStart);
            if (contentStart != std::string::npos)
            {
                contentStart += 11;
                size_t contentEnd = response.find(" \"", contentStart);
                if (contentEnd != std::string::npos)
                {
                    return response.substr(contentStart, contentEnd - contentStart);
                }
            }
        }
    }
    return "Error: Content not found in API response: " + response;
}

std::string TextBasedRPG::AskChatGPT(const std::string &m, float temperature)
{
    string message = RemoveString(m, "\"");
    message = RemoveString(message, "\\n");
    message = RemoveString(message, "\\");
    
    CURL* curl = curl_easy_init();
    std::string readBuffer;

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        std::string jsonPayload = R"({
            "model": "gpt-3.5-turbo",
            "messages": [
                {"role": "system", "content": "You are a dungeons and dragons game master. Every character has health and melee damage they can deal during battle.  Here is what has happened to far: )" + TextBasedRPG::GetContext() + R"("},
                {"role": "user", "content": ")" + message + R"("}
            ],
            "temperature": )" + std::to_string(temperature) + R"(,
            "top_p": 0.9,
            "max_tokens": 250,
            "frequency_penalty": 0.5,
            "presence_penalty": 0.6
        })";

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Authorization: Bearer API_KEY");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res == CURLE_OK)
        {
            string response = ExtractContentFromResponse(readBuffer);
            response = RemoveString(response, "\\n");
            response = RemoveString(response, "\"");
            response = response.substr(0, response.rfind('.') + 1);

            return response;
        }
        else
        {
            cout << "Json: " << jsonPayload << endl;
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
            return "Error: Failed to contact the API.";
        }
    }
    return "Error: Failed to contact the API.";
}

std::string TextBasedRPG::RemoveString(std::string s, std::string toRemove)
{
    size_t pos = 0;
    while ((pos = s.find(toRemove)) != std::string::npos)
    {
        s.erase(pos, toRemove.length());
    }

    return s;
}

std::string TextBasedRPG::GetContext()
{
    int levelsOfContext = 3;

    string context = "";

    if (World::decisions.size() > 0)
    {
        context = "The player has already made decisions to get to this point.  They are in order of occurrence: ";

        for (int i = max(World::decisions.size() - levelsOfContext, 0); i < World::decisions.size(); i++)
        {
            context += "Decision " + to_string(i + 1) + ": " + World::decisions[i] + ".";
        }

        context += " Talk about how their last decision impacts other characters or the enviornment.";
        context += " If all of the enemies have been defeated, the player should consider moving forward, resting, or looting the fallen enemies.";
        context += " If any specific creatures were here and they didn't die or go away, they should still be here in your response.";
    }

    if (World::encounters.size() > 0)
    {
        context += "The player has already had encounters to get to this point.  They are in order of occurrence: ";

        for (int i = max(World::encounters.size() - levelsOfContext, 0); i < World::encounters.size(); i++)
        {
            ENCOUNTER encounter = get<0> (World::encounters[i]);
            string encounterString = "A special event has occured! ";

            switch (encounter)
            {
            case TREASURE:
                encounterString = "The player has found Treasure, they can try to open it if they wish, but it may or may not be trapped. ";
                break;
            case ENEMY:
                encounterString = "The player has found an enemy, they can flee or chose to engage. If the enemy doesn't know the player is there, then a suprise attack can be powerful.  ";
                break;
            case FRIEND:
                encounterString = "The player has found a friend, the player should not attack this person since they will not attack back.  The player can chose to talk with them or go somwhere else.  ";
                break;
            case BATTLE:
                encounterString = "THe player is currently in battle and must continue to fight or flee. ";
                break;
            }

            
            context += encounterString + " The player made the decision: " + get<1>(World::encounters[i]) + ".";
        }
    }

    context = RemoveString(context, "\"");
    context = RemoveString(context, "\n");
    context = RemoveString(context, "\\");
    context = RemoveString(context, "'");
    
    return context;
}