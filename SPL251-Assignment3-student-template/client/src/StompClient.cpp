#include <iostream>
#include <string>
#include "Threads.h"
#include "ConnectionHandler.h"
#include "json.hpp"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        return -1;
    }

    std::string host = argv[1];
    short port = std::stoi(argv[2]);

    // Initialize the ConnectionHandler
    ConnectionHandler connectionHandler(host, port);
    StompProtocol stompProtocol;
    if (!connectionHandler.connect()) {
        std::cerr << "Failed to connect to " << host << ":" << port << std::endl;
        return -1;
    }

    // Initialize the Threads class
    Threads threads(connectionHandler,stompProtocol);

    // Start threads
    threads.start();

    // Main program loop
    while (threads.isRunning()) {

    }

    // Clean up
    std::cout << "Exiting program..." << std::endl;
    return 0;
}