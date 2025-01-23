#include <iostream>
#include <string>
#include "Threads.h"
#include "ConnectionHandler.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        return -1;
    }

    std::string host = argv[1];
    short port = std::stoi(argv[2]);

    // Initialize the ConnectionHandler
    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        std::cerr << "Failed to connect to " << host << ":" << port << std::endl;
        return -1;
    }

    // Initialize the Threads class
    Threads threads(connectionHandler);

    // Start threads
    threads.start();

    // Main program loop
    std::string input;
    while (true) {
        std::getline(std::cin, input);

        // Check for exit condition
        if (input == "exit") {
            threads.stop();
            break;
        }

        // Add input to frame queue (handled in Threads)
        threads.addFrame(input);
    }

    // Clean up
    std::cout << "Exiting program..." << std::endl;
    return 0;
}