#include "StompProtocol.h"
#include "Frame.h"
#include <iostream>

StompProtocol::StompProtocol(ConnectionHandler handler) : connectionHandler(handler) {}

bool StompProtocol::connect(const std::string host, const std::string username, const std::string password)
{
    // Create the CONNECT frame
    std::map<std::string, std::string> headers = {
        {"accept-version", "1.2"},
        {"host", host},
        {"login", username},
        {"passcode", password}
    };
    Frame connectFrame("CONNECT", headers, "");

    // Send the frame
    if (!connectionHandler.sendFrameAscii(connectFrame.toString(), '\0'))
    {
        std::cerr << "Could not connect to server" << std::endl;
        return false;
    }

    // Receive the server's response
    std::string response;
    if (!connectionHandler.getFrameAscii(response, '\0'))
    {
        std::cerr << "Could not receive server response" << std::endl;
        return false;
    }

    // Parse the server's response as a Frame
    Frame responseFrame = Frame::fromString(response);

    if (responseFrame.getCommand() == "CONNECTED")
    {
        std::cout << "Login successful" << std::endl;
        return true;
    }
    else if (responseFrame.getCommand() == "ERROR")
    {
        std::string errorMessage = responseFrame.getHeaders()["message"];

        if (errorMessage == "Client is already login.")
        {
            std::cerr << "The client is already logged in, log out before trying again" << std::endl;
        }
        else if (errorMessage == "User is already login.")
        {
            std::cerr << "User already logged in" << std::endl;
        }
        else if (errorMessage == "Invalid Passcode.")
        {
            std::cerr << "Wrong password" << std::endl;
        }
        else
        {
            std::cerr << "Error received: " << errorMessage << std::endl;
        }
        return false;
    }

    return false;
}

bool StompProtocol::join(const std::string channel_name){
    // Create the SUBSCIBE frame
    static int subscriptionId = 1;
    static int receiptId = 1;      

    std::map<std::string, std::string> headers = {
        {"destination", channel_name},
        {"id", std::to_string(subscriptionId++) },
        {"receipt", std::to_string(receiptId++)}//אין בSERVER
    };
    Frame subscribeFrame("SUBSCRIBE", headers, "");

    // Send the frame
    if (!connectionHandler.sendFrameAscii(subscribeFrame.toString(), '\0'))
    {
        std::cerr << "Could not send SUBSCRIBE frame" << std::endl;
        return false;
    }

    // Receive the server's response
    std::string response;
    if (!connectionHandler.getFrameAscii(response, '\0'))
    {
        std::cerr << "Could not receive server response" << std::endl;
        return false;
    }

    // Parse the server's response as a Frame
     Frame responseFrame = Frame::fromString(response);

    if (responseFrame.getCommand() == "RECEIPT")
    {
         std::cout << "Joined channel " << channel_name << std::endl;
        return true;
    }
    else if (responseFrame.getCommand() == "ERROR")
    {
        std::string errorMessage = responseFrame.getHeaders()["message"];
        if (errorMessage.contain("Client is already subscribed to the topic"))
        {
            std::cerr << "The client is already subscribed" << std::endl;
        }
        else
        {
            std::cerr << "Error received: " << errorMessage << std::endl;
        }
        return false;
    }

    return false;

}