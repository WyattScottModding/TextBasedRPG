#pragma once
#include <string>

class TextBasedRPG
{
public:
    static void NewGame();
    
    static std::string ExtractContentFromResponse(const std::string& response);
    static std::string AskChatGPT(const std::string &m, float temperature);
    static std::string GetContext();
    static std::string RemoveString(std::string s, std::string toRemove);
};
