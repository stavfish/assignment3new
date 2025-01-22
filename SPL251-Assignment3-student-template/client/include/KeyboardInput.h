#pragma once

#include "../include/ConnectionHandler.h"
#include <string>

class KeyboardInput
{
private:
    ConnectionHandler connectionHandler;
public:
    KeyboardInput(ConnectionHandler handler);
    bool logIn(const std::string host, const std::string username, const std::string password);
    bool join(const std::string channel_name);
    bool exit(const std::string channel_name);
    bool report(const std::string file);
    bool summary(const std::string channel_name, const std::string user, const std::string file);
    bool logout();

    
};