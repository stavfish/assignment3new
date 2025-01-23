#include "KeyboardInput.h"
#include "Frame.h"
#include "Threads.h"
#include <iostream>
#include <map>
#include <string>

KeyboardInput::KeyboardInput(ConnectionHandler handler) : connectionHandler(handler) {}

Frame KeyboardInput::logIn(const std::string host, const std::string username, const std::string password) {
    std::map<std::string, std::string> headers = {
        {"accept-version", "1.2"},
        {"host", host},
        {"login", username},
        {"passcode", password}
    };
    return Frame("CONNECT", headers, "");
}

Frame KeyboardInput::join(const std::string channel_name) {
    std::map<std::string, std::string> headers = {
        {"destination", "/" + channel_name},
        {"id", "1"},  // Generate a unique ID for each subscription
        {"receipt", "join-receipt"}
    };
    return Frame("SUBSCRIBE", headers, "");
}

Frame KeyboardInput::exit(const std::string channel_name) {
    std::map<std::string, std::string> headers = {
        {"id", "1"},  // Match the ID used in the join command
        {"receipt", "exit-receipt"}
    };
    return Frame("UNSUBSCRIBE", headers, "");
}

std::vector<Frame> KeyboardInput::report(const std::string file) {
    std::vector<Frame> frames;
    // Parse the JSON file to extract events
    names_and_events parsedEvents = parseEventsFile(file);
    const std::string& channel_name = parsedEvents.channel_name;

    for (const Event& event : parsedEvents.events) {
        std::map<std::string, std::string> headers = {
            {"destination", "/" + channel_name}
        };

        std::string body = "user:" + event.getEventOwnerUser() + "\n" +
                           "city:" + event.get_city() + "\n" +
                           "event name:" + event.get_name() + "\n" +
                           "date time:" + std::to_string(event.get_date_time()) + "\n" +
                           "general information:";

        for (const auto& info : event.get_general_information()) {
            body += "\n  " + info.first + ":" + info.second;
        }

        body += "\n\n" + event.get_description();

        frames.emplace_back("SEND", headers, body);
    }
    return frames;
}

Frame KeyboardInput::summary(const std::string channel_name, const std::string user, const std::string file) {
    // Implementation of summary assumes that the Threads or another class has stored received events
    // For simplicity, stub implementation here
    std::cerr << "Summary generation not yet implemented.\n";
    return Frame("SUMMARY", {}, "");
}

Frame KeyboardInput::logout() {
    std::map<std::string, std::string> headers = {
        {"receipt", "logout-receipt"}
    };
    return Frame("DISCONNECT", headers, "");
}
