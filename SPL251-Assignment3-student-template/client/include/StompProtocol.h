#pragma once

#include "../include/ConnectionHandler.h"
#include <string>

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
    ConnectionHandler connectionHandler;
public:
    StompProtocol(ConnectionHandler handler);
    bool connect(const std::string host, const std::string username, const std::string password);
    bool join(const std::string channel_name);
    bool exit(const std::string channel_name);

};
