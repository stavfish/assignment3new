#include "../include/ConnectionHandler.h"
#include "../include/Frame.h"
#include <iostream>
#include <stdexcept> // For std::runtime_error

class StompProtocol {
public:
    // Constructor: Use a reference to ConnectionHandler to avoid copying
    explicit StompProtocol(ConnectionHandler& handler) : connectionHandler(handler) {}

    // Send a frame
    bool send(const Frame& frame) {
        // Convert Frame to a STOMP string
        std::string frameString = frame.toString();

        // Use ConnectionHandler to send the frame
        if (!connectionHandler.sendFrameAscii(frameString, '\0')) {
            std::cerr << "Failed to send frame: " << frameString << std::endl;
            return false;
        }
        return true;
    }

    // Receive a frame
    Frame receive() {
        std::string frameString;
        std::cout << "Frame is: " << frameString << std::endl;
        // Use ConnectionHandler to receive the frame as a string
        if (!connectionHandler.getFrameAscii(frameString, '\0')) {
            std::cerr << "Failed to receive frame" << std::endl;
            throw std::runtime_error("Connection error while receiving frame");
        }

        // Convert the raw string into a Frame object
        return Frame::fromString(frameString);
    }

private:
    ConnectionHandler& connectionHandler; // Reference to avoid copying
};
