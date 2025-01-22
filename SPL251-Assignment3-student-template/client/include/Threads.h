#pragma once

#include "../include/ConnectionHandler.h"
#include <BlockingQueue>//
#include <string>
#include "KeyboardInput.h"
#include "StompProtocol.h"
#include <mutex>

class Threads
{
private:
    std::vector<std::string> frames; 
    KeyboardInput keyboardInput;
    StompProtocol stompProtocol;


public:


};