#pragma once

#include "../include/ConnectionHandler.h"
#include "Frame.h"
#include "Threads.h"
#include "event.h" 
#include <string>
#include <vector>

class KeyboardInput {
private:
    ConnectionHandler connectionHandler; // Handles communication with the server

public:
    KeyboardInput(ConnectionHandler handler);

    // Methods to process user input and generate frames
    Frame logIn(const std::string host, const std::string username, const std::string password);
    Frame join(const std::string channel_name);
    Frame exit(const std::string channel_name);
    std::vector<Frame> report(const std::string file);
    Frame summary(const std::string channel_name, const std::string user, const std::string file);
    Frame logout();
};
