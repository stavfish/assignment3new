#include "KeyboardInput.h"
#include "Frame.h"
#include <map>
#include <iostream>
#include <vector>
#include <event.h>

// Constructor
KeyboardInput::KeyboardInput(ConnectionHandler& handler,int recipt1) : connectionHandler(handler),reciptMaker(recipt1) {
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
Frame KeyboardInput::join(const std::string& channelName,int subscription_id) {
    std::map<std::string, std::string> headers ={
        {"destination", "/" + channelName},
        {"id", std::to_string(subscription_id)},  // Unique ID for subscription
        {"receipt", std::to_string(reciptMaker)}
    };
    reciptMaker++;
    return Frame("SUBSCRIBE", headers, "");
}
// Exit a channel
Frame KeyboardInput::exit(const std::string& channelName, const std::string& subscriptionId) {
    std::map<std::string, std::string> headers = {
        {"id", subscriptionId},  // Match the ID used in the join command
        {"receipt", std::to_string(reciptMaker)}
    };
    reciptMaker++;
    return Frame("UNSUBSCRIBE", headers, "");
}

// Report events (example using a file path to parse events)
std::vector<Frame> KeyboardInput::report(const std::string& filePath,const std::string& userName) {
    std::vector<Frame> frames;

    names_and_events nne = parseEventsFile(filePath);
    std::cout << "Parsing events from file: " << filePath << std::endl;

   for(Event e:nne.events){
    e.setEventOwnerUser(userName);
    std::map<std::string, std::string> headers = {
        {"destination", "/" + nne.channel_name},
    };
    std::string body = "user:"+e.getEventOwnerUser()+"\n"+"city:"+e.get_city()+"\n"+"event name:" + e.get_name() + "\n" +"date time:"+ std::to_string(e.get_date_time()) + "\n" + "general information: ...\n" +"  active:"+ e.get_general_information().at("active")+"\n" +"  forces_arrival_at_scene:" + e.get_general_information().at("forces_arrival_at_scene")+"\n" + "description:\n" + e.get_description() ;
    frames.push_back(Frame("SEND", headers,body));
   }
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
        {"receipt", std::to_string(reciptMaker)}
    };
    reciptMaker++;
    return Frame("DISCONNECT", headers, "");
}
