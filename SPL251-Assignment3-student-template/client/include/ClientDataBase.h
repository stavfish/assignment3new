#include <unordered_map>
#include <string>
#include <mutex>
#include <stdexcept>
#include <vector>
#include "event.h"

class ClientDataBase {
private:
    static ClientDataBase* instance;
    std::unordered_map< std::string,int> userSubscriptions;
    std::unordered_map<std::string,std ::string> recipts;
    std::unordered_map<std::string, std::vector<Event>> channelMessages; 
    std::map<std::string,std::map<std::string, std::string>>  receiptToSubscriptions;
    int nextSubscriptionId;
    int totalReportsNumber;
    int numberOfActive;
    int numberOfForcesArrival;

public:
    ClientDataBase(); // Constructor
    static ClientDataBase& getInstance();
    int insertSubscription(std::string subscriptionId);
    bool subscriptionExists( std::string valueToFind) ;
    void addRecipt(std::string reciptId, std::string recipt);
    std::string getId(std::string valueToFind);
    void leaveChannel(std::string subscriptionId);
    void addMessage(std::string channelName, Event eventToAdd);
    std::vector<Event> getMessagesPerUser(std::string channelName, std::string userName);
    int extractDateTime(std::string& str);
    std::string extractUser(const std::string& str);
    int getTotalReportsNumber();
    int getActiveNumber();
    int getNumberOfForcesArrival();
    void increaseTotalReports();
    void increaseNumberOfForcesArrival();
    void increaseActiveNumber();
    std::unordered_map<std::string,int> getUserSubscriptions();
    void cleanData();
    void addReciptSubscription(std::map<std::string, std::string>  headers,std::string command);
    std::string getReciptString(std::string);
};
