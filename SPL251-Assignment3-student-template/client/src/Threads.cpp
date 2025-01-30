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
    std::thread(&Threads::networkListener, this).detach();
    
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
    queueCV.notify_one(); 
}
void Threads::keyboardListener() {
    std::string input;

    while (running) {
        std::getline(std::cin, input); 

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
                    addFrame(joinFrame.toString()); 
                    stompProtocol.addReciptSubscription(joinFrame.getHeaders(),"join");
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
                    stompProtocol.addReciptSubscription(exitFrame.getHeaders(),"exit");
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
                     }
                }
                

            }
            else if(command == "summary"){
                std::string channelname, user, file;
                iss >> channelname >> user >> file;

                if (channelname.empty() || user.empty() || file.empty()) {
                    throw std::invalid_argument("Usage: summary <channelname> <user> <file>");
                }
                std::vector<Event> reportsByName = stompProtocol.getMessagesPerUser(channelname,user);
                stompProtocol.writeChannelReportToFile(file,channelname,reportsByName);
            }
            else if (command == "logout") {
                std::unordered_map<std::string,int> toLeave= stompProtocol.getAllChannels();
                for (const auto pair :toLeave) {
                    std::string key = pair.first;
                    int value = pair.second;
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
            if (!connectionHandler.getFrameAscii(frame, '\0')) {
                std::cerr << "Failed to fetch frame from socket. Connection might be closed." << std::endl;
                stop();
                return;
            }
            std::unique_lock<std::mutex> lock(networkMutex);
            networkQueue.push(frame);
            std::string frameToProcess = networkQueue.front();
            networkQueue.pop();

            Frame parsedFrame = Frame::fromString(frameToProcess);

            if (parsedFrame.getCommand() == "CONNECTED") {
                std::cout << "Succsesfully logged in" << std::endl;
            }else if(parsedFrame.getCommand() == "RECEIPT") {
                auto headers = parsedFrame.getHeaders();
                auto receiptId = headers.find("receipt-id");
                stompProtocol.addRecipt(receiptId->second,frameToProcess);
                std::cout<<stompProtocol.reciptSuccseed(receiptId->second)<<std::endl;
            } 
            else if(parsedFrame.getCommand() == "MESSAGE"){
                stompProtocol.addMessage(parsedFrame.getHeaders().at("destination").substr(1),parsedFrame.getBody());

            }
            else if(parsedFrame.getCommand() == "DISCONNECT"){
                stompProtocol.disconnect();
                toRun=false;
            }else if(parsedFrame.getCommand() == "ERROR"){
                stompProtocol.errorFrameDetected(parsedFrame);
                toRun=false;
            }
            else {
                std::cerr << "Unhandled frame command: " << parsedFrame.getCommand() << std::endl;
            }
    }

    std::cerr << "Network listener has stopped." << std::endl;
}


void Threads::frameHandler() {
    while (running) {

        std::string frame;
    
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait(lock, [this]() { return !frameQueue.empty() || !running; });
        
            if (!running && frameQueue.empty()) break;

    
            frame = frameQueue.front();
            frameQueue.pop();
        }  
            Frame parsedFrame = Frame::fromString(frame);

            if (!connectionHandler.sendFrameAscii(parsedFrame.toString(), '\0')) {
                std::cerr << "Failed to send frame: " << parsedFrame.toString() << std::endl;
                stop();
            }
    }
}
