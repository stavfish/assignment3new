#include "KeyboardInput.h"
#include "Frame.h"
#include <map>
#include <iostream>
#include <vector>

// Constructor
KeyboardInput::KeyboardInput(ConnectionHandler& handler) : connectionHandler(handler) {
    std::cout << "KeyboardInput initialized.\n";
}

// Log in
Frame KeyboardInput::logIn(const std::string& host, const std::string& username, const std::string& password) {
    std::map<std::string, std::string> headers = {
        {"accept-version", "1.2"},
        {"host", "stomp.cs.bgu.ac.il"},
        {"login", username},
        {"passcode", password}
    };
    return Frame("CONNECT", headers,"");
}

// Join a channel
Frame KeyboardInput::join(const std::string& channelName) {
    std::map<std::string, std::string> headers = {
        {"destination", "/" + channelName},
        {"id", "1"},  // Unique ID for subscription
        {"receipt", "join-receipt"}
    };
    return Frame("SUBSCRIBE", headers, "");
}

// Exit a channel
Frame KeyboardInput::exit(const std::string& channelName) {
    std::map<std::string, std::string> headers = {
        {"id", "1"},  // Match the ID used in the join command
        {"receipt", "exit-receipt"}
    };
    return Frame("UNSUBSCRIBE", headers, "");
}

// Report events (example using a file path to parse events)
std::vector<Frame> KeyboardInput::report(const std::string& filePath) {
    std::vector<Frame> frames;

    // This is a stub implementation. Replace with actual file parsing.
    std::cout << "Parsing events from file: " << filePath << std::endl;

    // Example frame creation
    std::map<std::string, std::string> headers = {
        {"destination", "/exampleChannel"}
    };
    frames.push_back(Frame("SEND", headers, "Event data"));

    return frames;
}

// Generate a summary
Frame KeyboardInput::summary(const std::string& channelName, const std::string& user, const std::string& filePath) {
    std::cout << "Generating summary for channel: " << channelName << " and user: " << user << std::endl;

    // Placeholder implementation for the summary frame
    return Frame("SUMMARY", {}, "Generated summary content");
}

// Log out
Frame KeyboardInput::logout() {
    std::map<std::string, std::string> headers = {
        {"receipt", "logout-receipt"}
    };
    return Frame("DISCONNECT", headers, "");
}
