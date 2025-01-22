#pragma once

#include "../include/ConnectionHandler.h"
#include <string>

class StompProtocol
{
private:
    ConnectionHandler connectionHandler;
public:
    StompProtocol(ConnectionHandler handler);
    bool send(Frame frame);
    Frame recive();
};
