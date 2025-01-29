#pragma once

#include "ClientDataBase.h"
#include <string>

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
    std::vector<std::string> getMessagesPerUser(std::string channelName, std::string userName);
    void addMessage(std::string channelName, std::string messageFrame);
    int getTotalReportsNumber() const;
    int getActiveNumber() const;
    int getNumberOfForcesArrival() const;
    void increaseTotalReports();
    void increaseNumberOfForcesArrival();
    void increaseActiveNumber();
    void writeChannelReportToFile(const std::string& filePath,
                              const std::string& channelName,
                              const std::vector<std::string>& reportsByName);
    std::unordered_map<std::string,int> getAllChannels();
    void disconnect();
};