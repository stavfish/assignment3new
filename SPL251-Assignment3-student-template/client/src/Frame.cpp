#include "Frame.h"

// Constructor
Frame::Frame(const std::string command, const std::map<std::string, std::string> headers, const std::string body)
    : command(command), headers(headers), body(body) {}

// Getters
std::string Frame::getCommand() const {
    return command;
}

std::map<std::string, std::string> Frame::getHeaders() const {
    return headers;
}

std::string Frame::getBody() const {
    return body;
}

// Setters
void Frame::setCommand(const std::string command) {
    this->command = command;
}

void Frame::setHeaders(const std::map<std::string, std::string> headers) {
    this->headers = headers;
}

void Frame::setBody(const std::string body) {
    this->body = body;
}

// Convert the frame to a STOMP string
std::string Frame::toString() const {
    std::ostringstream oss;
    oss << command << "\n";

    // Add headers
    for (const auto& header : headers) {
        oss << header.first << ":" << header.second << "\n";
    }

    // Add a blank line and body
    oss << "\n" << body << "\0"; // Null terminator
    return oss.str();
}

// Parse a STOMP string into a Frame object
Frame Frame::fromString(const std::string frameString) {
    std::istringstream iss(frameString);
    std::string line;

    // Get command
    std::getline(iss, line);
    std::string command = line;

    // Get headers
    std::map<std::string, std::string> headers;
    while (std::getline(iss, line) && !line.empty()) {
        size_t delimiter = line.find(':');
        if (delimiter != std::string::npos) {
            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 1);
            headers[key] = value;
        }
    }

    // Get body
    std::string body;
    std::getline(iss, body, '\0'); // Read until null terminator

    return Frame(command, headers, body);
}