#include "StompProtocol.h"
#include <iostream>
#include <stdexcept>
#include <fstream>

// Constructor retrieves the singleton instance
StompProtocol::StompProtocol() : database(ClientDataBase::getInstance()) {}

// Handles user login
int StompProtocol::joinChannel(const std::string& subscriptionId) const {
    return database.insertSubscription(subscriptionId);
}
void StompProtocol::addRecipt(std::string reciptId, std::string recipt){
    database.addRecipt(reciptId,recipt);
}
std::string StompProtocol::getId(const std::string& subscriptionId) const{
    return database.getId(subscriptionId);
}
void StompProtocol::leaveChannel(const std::string& subscriptionId) const{
    database.leaveChannel(subscriptionId);
}
std::vector<std::string> StompProtocol::getMessagesPerUser(std::string channelName, std::string userName){
    return database.getMessagesPerUser(channelName,userName);
}
void StompProtocol::addMessage(std::string channelName, std::string messageFrame){
    database.addMessage(channelName,messageFrame);
}
int StompProtocol::getTotalReportsNumber() const{
    return database.getTotalReportsNumber();
}
int StompProtocol::getActiveNumber() const{
    return database.getActiveNumber();
}
int StompProtocol::getNumberOfForcesArrival() const{
    return database.getNumberOfForcesArrival();
}
void StompProtocol::increaseTotalReports(){
    database.increaseTotalReports();
}
void StompProtocol::increaseNumberOfForcesArrival(){
    database.increaseNumberOfForcesArrival();
}
void StompProtocol::increaseActiveNumber(){
    database.increaseActiveNumber();
}
void StompProtocol::writeChannelReportToFile(const std::string& filePath,
                              const std::string& channelName,
                              const std::vector<std::string>& reportsByName) {
    // Open the file in overwrite mode
    std::ofstream outFile(filePath, std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    // Write channel information
    outFile << "Channel <" << channelName << ">\n";
    outFile << "Stats:\n";
    outFile << "Total: " << getTotalReportsNumber() << "\n";
    outFile << "active: " << getActiveNumber() << "\n";
    outFile << "forces arrival at scene: " << getNumberOfForcesArrival() << "\n\n";

    // Write report information
    outFile << "Event Reports:\n";

    int reportCounter = 1;
    for (const auto& report : reportsByName) {
        outFile << "Report_" << reportCounter++ << ":\n";
        outFile << "    " << report << "\n";
    }

    // Close the file
    outFile.close();
    std::cout << "File written successfully: " << filePath << std::endl;
}
std::unordered_map<std::string,int> StompProtocol::getAllChannels(){
    return database.getUserSubscriptions();
}
void StompProtocol::disconnect(){
    database.cleanData();
}