#include "../include/ConnectionHandler.h"
#include "Frame.h"
#include <iostream>

StompProtocol::StompProtocol(ConnectionHandler handler) : connectionHandler(handler) {}

bool StompProtocol::send(const Frame& frame) {
    // Convert Frame to a STOMP string
    std::string frameString = frame.toString();

    // Use ConnectionHandler to send the frame
    if (!connectionHandler.sendFrameAscii(frameString, '\0')) {
        std::cerr << "Failed to send frame: " << frameString << std::endl;
        return false;
    }
    return true;
}

Frame StompProtocol::receive() {
    std::string frameString;

    // Use ConnectionHandler to receive the frame as a string
    if (!connectionHandler.getFrameAscii(frameString, '\0')) {
        std::cerr << "Failed to receive frame" << std::endl;
        throw std::runtime_error("Connection error while receiving frame");
    }

    // Convert the raw string into a Frame object
    return Frame::fromString(frameString);
}
