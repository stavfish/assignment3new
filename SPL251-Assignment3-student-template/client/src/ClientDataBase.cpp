#include "ClientDataBase.h"
#include <mutex>
#include <c++/10/bits/algorithmfwd.h>
#include <iostream>
#include <vector>

// Initialize the singleton instance pointer to null
ClientDataBase* ClientDataBase::instance = nullptr;

// Singleton accessor
ClientDataBase& ClientDataBase::getInstance() {
    if (!instance) {
        instance = new ClientDataBase();
    }
    return *instance;
}
// Constructor
ClientDataBase::ClientDataBase() : userSubscriptions(),recipts(),channelMessages(), nextSubscriptionId(1),totalReportsNumber(0),numberOfActive(0),numberOfForcesArrival(0) {}

// Adds a user and assigns a unique subscription ID
int ClientDataBase::insertSubscription(std::string subscription) {
    if (!subscriptionExists(subscription)) {
        userSubscriptions[subscription] = nextSubscriptionId;
        nextSubscriptionId++;
        channelMessages[subscription] = std::vector<std::string>();
        return nextSubscriptionId-1;
    } else {
        return -1;
    }
}
bool ClientDataBase::subscriptionExists( std::string valueToFind)  {
    if(userSubscriptions.find(valueToFind)!=userSubscriptions.end()){
        return true;
    }
    else{
        return false;
    }
}
std::string ClientDataBase:: getId(std::string subscriptionId){
    if(userSubscriptions.find(subscriptionId)==userSubscriptions.end()){
        return std::to_string(-1);
    }
    else{
        return std::to_string(userSubscriptions[subscriptionId]);
    }
}
void ClientDataBase::leaveChannel(std::string subscriptionId){
    userSubscriptions.erase(subscriptionId);
}
void ClientDataBase::addRecipt(std::string reciptId, std::string recipt){
    recipts[reciptId] = recipt;
    std::cout << "Recipt: "  << recipt << "was added to recipts map" << std::endl;
}
void ClientDataBase::addMessage(std::string channelName, std::string messageFrame){
    if(channelMessages.find(channelName)!=channelMessages.end()){
            int newDateTime = extractDateTime(messageFrame);

    // Find the correct position to insert the new string
        auto it = channelMessages[channelName].begin();
        for (; it != channelMessages[channelName].end(); ++it) {
            if (newDateTime < extractDateTime(*it)) {
            break;
            }
        }

    // Insert the new string at the correct position
    channelMessages[channelName].insert(it, messageFrame);
    }
}
int ClientDataBase::extractDateTime (std::string& str) {
    const std::string key = "date time:";
    size_t pos = str.find(key);
    if (pos != std::string::npos) {
        pos += key.length();
        size_t endPos = str.find_first_of(" \n", pos); 
        std::string dateTimeStr = str.substr(pos, endPos - pos);
        return std::stoi(dateTimeStr); 
    }
    return -1; 
}
std::vector<std::string> ClientDataBase::getMessagesPerUser(std::string channelName, std::string userName){
        std::vector<std::string> result;
        auto it = channelMessages.find(channelName);
        if (it != channelMessages.end()) {
            const auto& messages = it->second;
            // Filter messages where user header matches the given username
            for (const auto& message : messages) {
            if (extractUser(message) == userName) {
                result.push_back(message);
            }
            if(result.empty()){
                std :: cerr << "This name has no report in the desired channel"<< std::endl;
            }
        }
    
        }
        else{
            std :: cerr << "You are not registered to this channel"<< std::endl;
        }
        return result;
}
std::string ClientDataBase::extractUser(const std::string& str) {
    const std::string header = "user:";
    size_t pos = str.find(header);
    if (pos != std::string::npos) {
        pos += header.length(); 
        size_t endPos = str.find(" ", pos); 
        return str.substr(pos, endPos - pos); 
    }
    return ""; 
}
int ClientDataBase::getTotalReportsNumber(){
    return totalReportsNumber;
}
int ClientDataBase::getActiveNumber(){
    return numberOfActive;
}
int ClientDataBase::getNumberOfForcesArrival(){
    return numberOfForcesArrival;
}
void ClientDataBase::increaseTotalReports(){
    totalReportsNumber++;
}
void ClientDataBase::increaseNumberOfForcesArrival(){
    numberOfForcesArrival++;
}
void ClientDataBase::increaseActiveNumber(){
    numberOfActive++;
}
 std::unordered_map<std::string,int> ClientDataBase:: getUserSubscriptions(){
    return userSubscriptions;
 }
 void ClientDataBase::cleanData(){
    userSubscriptions = std::unordered_map<std::string, int>();
    recipts = std::unordered_map<std::string, std::string>();
    channelMessages = std::unordered_map<std::string, std::vector<std::string>>();
    nextSubscriptionId=1;
    totalReportsNumber=0;
    numberOfActive=0;
    numberOfForcesArrival=0;
 }


