#pragma once
#include <string>
#include <map>
#include <sstream>

class Frame {
private:
    std::string command; 
    std::map<std::string, std::string> headers; 
    std::string body; 

public:
    // Constructor
    Frame(const std::string command, const std::map<std::string, std::string> headers, const std::string body);

    // Getters
    std::string getCommand() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getBody() const;

    // Setters
    void setCommand(const std::string command);
    void setHeaders(const std::map<std::string, std::string> headers);
    void setBody(const std::string body);

    // Convert the frame to a STOMP string 
    std::string toString() const;

    // Parse a STOMP string into a Frame object
    static Frame fromString(const std::string frameString);
};