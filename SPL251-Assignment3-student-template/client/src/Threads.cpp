#include "Threads.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <string>
#include "ConnectionHandler.h"
#include <condition_variable>
#include <mutex>
#include <queue>
#include "StompProtocol.h"

// Constructor with proper member initialization
Threads::Threads(ConnectionHandler& handler,StompProtocol& protocol)
    : connectionHandler(handler),
      stompProtocol(protocol), // Initialize reference
      keyboardInput(handler,1),     // Initialize KeyboardInput
      frameQueue(),               // Initialize queue
      queueMutex(),               // Initialize mutex
      queueCV(),                  // Initialize condition variable
      running(false),
      networkMutex(),
      networkCV(),
      networkQueue(),
      userName()
       {            // Initialize running flag
    std::cout << "Threads initialized.\n";
}

void Threads::start() {
    std::cout << "Threads started.\n";
    running = true;

    std::thread(&Threads::keyboardListener, this).detach();
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
                std::thread(&Threads::networkListener, this).detach();
                addFrame(loginFrame.toString());
                userName = username;
            } else if (command == "join") {
                std::string channelName;
                iss >> channelName;

                if (channelName.empty()) {
                throw std::invalid_argument("Usage: join <channel_name>");
                }
                int subscription_id = stompProtocol.joinChannel(channelName);
                if(subscription_id == -1){
                    std::cout << "Already subscribed to: " << channelName << std::endl;
                }else{
                    Frame joinFrame = keyboardInput.join(channelName,subscription_id);
                    addFrame(joinFrame.toString()); // Add frame to the queue for sending
                }
            
            }
            else if(command == "exit"){
                std::string channelName;
                iss >> channelName;

                if (channelName.empty()) {
                throw std::invalid_argument("Usage: exit <channel_name>");
                }
                std::string subscription_id = stompProtocol.getId(channelName);
                if(subscription_id == "-1"){
                    std::cout << "You are not subscribed to: " << channelName << " therefor you can not leave it" << std::endl;
                }else{
                    Frame exitFrame = keyboardInput.exit(channelName,subscription_id);
                    addFrame(exitFrame.toString()); // Add frame to the queue for sending
                    stompProtocol.leaveChannel(channelName);
                }
            }
            else if(command == "report"){
                std::string file;
                iss >> file;
                
                if (file.empty()) {
                throw std::invalid_argument("Usage: report <file>");
                }
                else{
                    std::vector<Frame> frames = keyboardInput.report(file,userName);
                    for (Frame f:frames){
                        addFrame(f.toString());
                        stompProtocol.increaseTotalReports();
                        auto it = f.getHeaders().find(" active");
                        if(it!=f.getHeaders().end()){
                            if(it->second=="true"){
                                stompProtocol.increaseActiveNumber();
                            }
                        }
                        auto it2 = f.getHeaders().find(" forces_arrival_at_scene");
                        if(it2!=f.getHeaders().end()){
                            if(it2->second=="true"){
                                stompProtocol.increaseNumberOfForcesArrival();
                            }
                        }


                    }
                }

            }
            else if(command == "summary"){
                std::string channelname, user, file;
                iss >> channelname >> user >> file;

                if (channelname.empty() || user.empty() || file.empty()) {
                    throw std::invalid_argument("Usage: summary <channelname> <user> <file>");
                }
                std::vector<std::string> reportsByName = stompProtocol.getMessagesPerUser(channelname,user);
                stompProtocol.writeChannelReportToFile(file,channelname,reportsByName);
            }
            else if (command == "logout") {
                std::unordered_map<std::string,int> toLeave= stompProtocol.getAllChannels();
                for (const auto& pair : toLeave) {
                    const auto& key = pair.first;
                    const auto& value = pair.second;
                    Frame exitFrame = keyboardInput.exit(key,std::to_string(value));
                    addFrame(exitFrame.toString()); 
                    stompProtocol.leaveChannel(key);
                }
                Frame logoutFrame = keyboardInput.logout();
                addFrame(logoutFrame.toString());

            } else {
                std::cerr << "Error: Unknown command \"" << command << "\"." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}




void Threads::networkListener() {
    bool toRun =true;
    while (toRun) {
            std::string frame;
            std::cerr << "Reciving a new frame --------------------" << std::endl;
            if (!connectionHandler.getFrameAscii(frame, '\0')) {
                std::cerr << "Failed to fetch frame from socket. Connection might be closed." << std::endl;
                stop(); // Gracefully stop the threads if the connection is closed
                return;
            }
            std::unique_lock<std::mutex> lock(networkMutex);
            networkQueue.push(frame);
            std::cerr << "Added frame to networkQueue: " << frame << std::endl;
            std::string frameToProcess = networkQueue.front();
            networkQueue.pop();
        // Process the frame
        try {
            Frame parsedFrame = Frame::fromString(frameToProcess);

            // Debug log
            std::cout << "Received frame - Command: " << parsedFrame.getCommand() << std::endl;

            // Example: Handle specific commands
            if (parsedFrame.getCommand() == "CONNECTED") {
                std::cout << "Message body: " << parsedFrame.getBody() << std::endl;
            }else if(parsedFrame.getCommand() == "RECEIPT") {
                auto headers = parsedFrame.getHeaders();
                auto receiptId = headers.find("receipt-id");
                stompProtocol.addRecipt(receiptId->second,frameToProcess);
            } 
            else if(parsedFrame.getCommand() == "MESSAGE"){
                std::string destination;
                auto it = parsedFrame.getHeaders().find("destination");
                if (it != parsedFrame.getHeaders().end()) {
                             destination = it->second;
                             destination.erase(0,1);

                }
                stompProtocol.addMessage(destination,parsedFrame.toString());
            }
            else if(parsedFrame.getCommand() == "DISCONNECT"){
                stompProtocol.disconnect();
                toRun=false;
            }
            else {
                std::cerr << "Unhandled frame command: " << parsedFrame.getCommand() << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "Error processing frame: " << e.what() << std::endl;
        }
    }

    std::cerr << "Network listener has stopped." << std::endl;
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
        if (!connectionHandler.sendFrameAscii(parsedFrame.toString(), '\0')) {
            std::cerr << "Failed to send frame: " << parsedFrame.toString() << std::endl;
            stop();
        }
    }
}
