#include "Frame.h"
#include <iostream>

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

    for (const auto& header : headers) {
        oss << header.first << ":" << header.second << "\n";
    }

    oss << "\n" << body << '\0';
    return oss.str();
}

Frame Frame::fromString(const std::string frameString) {
    std::istringstream iss(frameString);
    std::string line;

    // Get command
    if (!std::getline(iss, line) || line.empty()) {
        throw std::invalid_argument("Invalid frame: Missing command.");
    }
    std::string command = line;

    std::map<std::string, std::string> headers;
    while (std::getline(iss, line) && !line.empty()) {
        size_t delimiter = line.find(':');
        if (delimiter == std::string::npos) {
            throw std::invalid_argument("Invalid frame: Malformed header: " + line);
        }
        std::string key = line.substr(0, delimiter);
        std::string value = line.substr(delimiter+1);
        headers[key] = value;
    }

    // Get body
    std::string body;
    std::getline(iss, body, '\0'); // Read until null terminator
    return Frame(command, headers, body);
}

