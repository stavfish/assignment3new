#include "Threads.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <string>

// Constructor with proper member initialization
Threads::Threads(ConnectionHandler& handler)
    : connectionHandler(handler), // Initialize reference
      keyboardInput(handler),     // Initialize KeyboardInput
      frameQueue(),               // Initialize queue
      queueMutex(),               // Initialize mutex
      queueCV(),                  // Initialize condition variable
      running(false) {            // Initialize running flag
    std::cout << "Threads initialized.\n";
}

void Threads::start() {
    std::cout << "Threads started.\n";
    running = true;

    // Start the keyboard and network listeners
    std::thread(&Threads::keyboardListener, this).detach();
    std::thread(&Threads::networkListener, this).detach();
    std::thread(&Threads::frameHandler, this).detach();
}

void Threads::stop() {
    std::cout << "Threads stopped.\n";
    running = false;

    // Notify all threads to stop waiting
    queueCV.notify_all();
}

bool Threads::isRunning(){
    return running;
}
void Threads::addFrame(const std::string& frame) {
    std::unique_lock<std::mutex> lock(queueMutex);
    frameQueue.push(frame);
    queueCV.notify_one(); // Notify the frame handler
    std::cout << "Frame added: " << frame << "\n";
}
void Threads::keyboardListener() {
    std::string input;

    while (running) {
        std::getline(std::cin, input); // Get user input

        std::istringstream iss(input);
        std::string command;
        iss >> command;

        try {
            if (command == "login") {
                std::string host, username, password;
                iss >> host >> username >> password;

                if (host.empty() || username.empty() || password.empty()) {
                    throw std::invalid_argument("Usage: login <host> <username> <password>");
                }

                Frame loginFrame = keyboardInput.logIn(host, username, password);
                addFrame(loginFrame.toString());
            } else if (command == "join") {
                // Handle join command
            } else if (command == "logout") {
                // Handle logout command
            } else {
                std::cerr << "Error: Unknown command \"" << command << "\"." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}



void Threads::networkListener() {
    while (running) {
        try {
            std::string rawFrame;
            if (!connectionHandler.getFrameAscii(rawFrame, '\0')) {
                std::cerr << "Connection closed while receiving frame." << std::endl;
                stop();
                return;
            }
            
        }
        catch (const std::exception& e) {
            std::cerr << "Error in networkListener: " << e.what() << std::endl;
            stop();
        }
    }
}

void Threads::frameHandler() {
    while (running) {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCV.wait(lock, [this]() { return !frameQueue.empty() || !running; });

        if (!running && frameQueue.empty()) break;

        std::string frame = frameQueue.front();
        frameQueue.pop();

        // Process the frame (sending via connectionHandler)
        Frame parsedFrame = Frame::fromString(frame);
        std::cout<<frame<<std::endl;
        if (!connectionHandler.sendFrameAscii(parsedFrame.toString(), '\0')) {
            std::cerr << "Failed to send frame: " << parsedFrame.toString() << std::endl;
        }
    }
}
