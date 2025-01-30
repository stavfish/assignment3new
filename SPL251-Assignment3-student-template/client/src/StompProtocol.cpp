#include "StompProtocol.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include "event.h"

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
std::vector<Event> StompProtocol::getMessagesPerUser(std::string channelName, std::string userName){
    return database.getMessagesPerUser(channelName,userName);
}
void StompProtocol::addMessage(std::string channelName, std::string messageFrame){
    Event e = Event(messageFrame);
    database.addMessage(channelName,e);
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
                              const std::vector<Event>& reportsByName) {
    // Open the file in overwrite mode
    std::ofstream outFile(filePath, std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    // Write channel information
    outFile << "Channel <" << channelName << ">\n";
    outFile << "Stats:\n";
    outFile << "Total: " << std::to_string(database.getTotalReportsNumber()) << "\n";
    outFile << "active: " << std::to_string(database.getActiveNumber()) << "\n";
    outFile << "forces arrival at scene: " << std::to_string(database.getNumberOfForcesArrival()) << "\n\n";

    // Write report information
    outFile << "Event Reports:\n";

    int reportCounter = 1;
    for ( Event report : reportsByName) {
        std::string SummarizedDescription = report.get_description();
        if(SummarizedDescription.size()>27){
            SummarizedDescription = SummarizedDescription.substr(0,26);
            SummarizedDescription += "...";
        }

        outFile << "Report_" << reportCounter++ << ":\n";
        outFile << "  city:" << report.get_city() << "\n";
        outFile << "  date time:" << std::to_string(report.get_date_time()).substr(0,2)+"/"+std::to_string(report.get_date_time()).substr(2,2)+"/"+std::to_string(report.get_date_time()).substr(4,2)+"  "+ std::to_string(report.get_date_time()).substr(6,2)+":"+std::to_string(report.get_date_time()).substr(8,2)<< "\n";
        outFile << "  event name:" << report.get_name() << "\n";
        outFile << "  summary:" << SummarizedDescription << "\n";
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
void StompProtocol::errorFrameDetected(Frame f){
    if(f.getHeaders().find("message")!=f.getHeaders().end()){
        for(const auto& pair : f.getHeaders()){
            if(pair.first=="message"){
                std::cout<<pair.second<<std::endl;
            }
        }
    }
}
void StompProtocol::addReciptSubscription(std::map<std::string, std::string> headers,std::string command){
    database.addReciptSubscription(headers,command);
}
std::string StompProtocol::reciptSuccseed(std::string reciptId){
    return database.getReciptString(reciptId);
}