#pragma once

#include "../include/ConnectionHandler.h"
#include "KeyboardInput.h"
#include "StompProtocol.h"
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>

class Threads {
private:
    std::queue<std::string> frameQueue; // Queue for frames to handle
    std::mutex queueMutex;              // Mutex to synchronize access to the queue
    std::condition_variable queueCV;    // Condition variable for thread signaling
    KeyboardInput keyboardInput;        // Handles keyboard input
    StompProtocol stompProtocol;        // Handles STOMP protocol
    bool running;                       // Tracks the running state of threads

    // Thread methods
    void keyboardListener();            // Listens for keyboard input
    void networkListener();             // Listens for network messages
    void frameHandler();                // Handles frames from the queue

public:
    Threads(ConnectionHandler& handler);

    // Starts the threads
    void start();

    // Stops the threads
    void stop();

    // Adds a frame to the queue
    void addFrame(const std::string& frame);
};
