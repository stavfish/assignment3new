#include <unordered_map>
#include <string>
#include <mutex>
#include <stdexcept>
#include <vector>

class ClientDataBase {
private:
    static ClientDataBase* instance;
    std::unordered_map< std::string,int> userSubscriptions;
    std::unordered_map<std::string,std ::string> recipts;
    std::unordered_map<std::string, std::vector<std::string>> channelMessages; 
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
    void addMessage(std::string channelName, std::string messageFrame);
    std::vector<std::string> getMessagesPerUser(std::string channelName, std::string userName);
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
};
