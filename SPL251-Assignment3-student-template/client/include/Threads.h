#pragma once

#include "ConnectionHandler.h"
#include "KeyboardInput.h"
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>

class Threads {
public:
    explicit Threads(ConnectionHandler& handler);
    void start();
    void stop();
    bool isRunning();
    void addFrame(const std::string &frame);

private:
    ConnectionHandler& connectionHandler; // Connection handler reference
    KeyboardInput keyboardInput;          // Handles keyboard input
    std::queue<std::string> frameQueue;   // Queue for frames
    std::mutex queueMutex;                // Mutex for thread safety
    std::condition_variable queueCV;      // Condition variable for signaling
    bool running;                         // Tracks running state

    // Private methods for thread tasks
    void keyboardListener();              // Listens for keyboard input
    void networkListener();               // Listens for network messages
    void frameHandler();                  // Handles frames from the queue
};
