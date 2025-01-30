#pragma once

#include "ClientDataBase.h"
#include <string>
#include "Frame.h"

class StompProtocol {
private:
    ClientDataBase& database;

public:
    // Constructor retrieves the singleton instance
    StompProtocol();
    int joinChannel(const std::string& subscriptionId) const;
    void addRecipt(std::string reciptId,std::string recipt);
    std::string getId(const std::string& subscriptionId) const;
    void leaveChannel(const std::string& subscriptionId) const;
    std::vector<Event> getMessagesPerUser(std::string channelName, std::string userName);
    void addMessage(std::string channelName, std::string messageFrame);
    int getTotalReportsNumber() const;
    int getActiveNumber() const;
    int getNumberOfForcesArrival() const;
    void increaseTotalReports();
    void increaseNumberOfForcesArrival();
    void increaseActiveNumber();
    void writeChannelReportToFile(const std::string& filePath,
                              const std::string& channelName,
                              const std::vector<Event>& reportsByName);
    std::unordered_map<std::string,int> getAllChannels();
    void disconnect();
    void errorFrameDetected(Frame F);
    void addReciptSubscription(std::map<std::string, std::string> headers,std::string command);
    std::string reciptSuccseed(std::string reciptId);
};