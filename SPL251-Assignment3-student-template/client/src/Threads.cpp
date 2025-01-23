#include "Threads.h"
#include "../include/ConnectionHandler.h"

Threads::Threads(ConnectionHandler& handler)
    : keyboardInput(handler), stompProtocol(handler), running(true) {}

void Threads::start() {
    std::thread(&Threads::keyboardListener, this).detach();
    std::thread(&Threads::networkListener, this).detach();
    std::thread(&Threads::frameHandler, this).detach();
}

void Threads::stop() {
    std::unique_lock<std::mutex> lock(queueMutex);
    running = false;
    queueCV.notify_all(); // Wake up all waiting threads
}

void Threads::addFrame(const std::string& frame) {
    std::unique_lock<std::mutex> lock(queueMutex);
    frameQueue.push(frame);
    queueCV.notify_one(); // Signal that a new frame is available
}

void Threads::keyboardListener() {
    while (running) {
        // Example: Simulate keyboard input handling
        // This would invoke KeyboardInput methods like logIn, join, etc.
        std::string input;
        std::getline(std::cin, input);

        // Example: Process input into a frame and add to the queue
        if (!input.empty()) {
            Frame frame = keyboardInput.logIn("stomp.cs.bgu.ac.il", "user", "pass"); // Example
            addFrame(frame.toString());
        }
    }
}

void Threads::networkListener() {
    while (running) {
        try {
            Frame frame = stompProtocol.receive();
            std::cout << "Received frame: " << frame.toString() << std::endl;
        } catch (const std::exception& e) {
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

        // Send frame via stompProtocol
        Frame parsedFrame = Frame::fromString(frame);
        if (!stompProtocol.send(parsedFrame)) {
            std::cerr << "Failed to send frame: " << frame << std::endl;
        }
    }
}
