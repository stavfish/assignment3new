#pragma once
#include "ConnectionHandler.h"
#include "Frame.h"
#include <vector>
#include <string>

class KeyboardInput {
public:
    explicit KeyboardInput(ConnectionHandler& handler);

    // Log in to the server
    Frame logIn(const std::string& host, const std::string& username, const std::string& password);

    // Join a channel
    Frame join(const std::string& channelName);

    // Exit a channel
    Frame exit(const std::string& channelName);

    // Report events
    std::vector<Frame> report(const std::string& filePath);

    // Generate a summary
    Frame summary(const std::string& channelName, const std::string& user, const std::string& filePath);

    // Log out from the server
    Frame logout();

private:
    ConnectionHandler& connectionHandler;
};
