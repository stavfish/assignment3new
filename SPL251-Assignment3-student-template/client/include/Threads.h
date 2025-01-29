#pragma once

#include "ConnectionHandler.h"
#include "KeyboardInput.h"
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "StompProtocol.h"

class Threads {
public:
    explicit Threads(ConnectionHandler& handler,StompProtocol& protocol);
    void start();
    void stop();
    bool isRunning();
    void addFrame(const std::string &frame);
    void addNetworkFrame();

private:
    ConnectionHandler& connectionHandler;
    StompProtocol& stompProtocol;  // Connection handler reference
    KeyboardInput keyboardInput;          // Handles keyboard input
    std::queue<std::string> frameQueue;   // Queue for frames
    std::mutex queueMutex;                // Mutex for thread safety
    std::condition_variable queueCV;      // Condition variable for signaling
    bool running;
    std::mutex networkMutex;
    std::condition_variable networkCV;
    std::queue<std::string> networkQueue;
    std::string userName;                        // Tracks running state

    // Private methods for thread tasks
    void keyboardListener();
    // Listens for keyboard input
    void networkListener();               // Listens for network messages
    void frameHandler();                  // Handles frames from the queue
};
