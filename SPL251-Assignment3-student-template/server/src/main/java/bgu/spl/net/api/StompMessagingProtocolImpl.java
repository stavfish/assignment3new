package bgu.spl.net.api;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.impl.stomp.Frame;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<String> {
    private int connectionId;
    private Connections<String> connections;
    private boolean shouldTerminate = false;

    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
    }

    @Override
    public void process(String message) {
        try {
            Frame frame = Frame.fromString(message);
            handleCommand(frame);
        } 
        catch (IllegalArgumentException e) {
            sendError("Invalid STOMP frame: " + e.getMessage())
            ;
        }
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    private void handleCommand(Frame frame) {
        String command = frame.getCommand();
        Map<String, String> headers = frame.getHeaders();
        String body = frame.getBody();

        switch (command) {
            case "CONNECT":
                handleConnect(headers);
                break;
            case "SUBSCRIBE":
                handleSubscribe(headers);
                break;
            case "UNSUBSCRIBE":
                handleUnsubscribe(headers);
                break;
            case "SEND":
                handleSend(headers, body);
                break;
            case "DISCONNECT":
                handleDisconnect(headers);
                break;
            default:
                sendError("Unknown command: " + command);
        }
    }

    private void handleConnect(Map<String, String> headers) {
        String login = headers.get("login");
        String passcode = headers.get("passcode");
        
        if (login == null || passcode == null) {
             sendError("Missing login or passcode in CONNECT");
             return;
        } 
        
        else if (connections.isConnected(connectionId)){
            sendError("Client is already login.");
            return;
        }

        else if (connections.isConnected(login, passcode)) { 
            sendError("User is already login.");
            return;
        }

        else if (!connections.isPasscodeCorrect(login, passcode)){
            sendError("Invalid Passcode.");
            return;
        }

        ConcurrentHashMap<String, String> respondHeaders = new ConcurrentHashMap<>();
        respondHeaders.put("version", "1.2");
        Frame respondFrame = new Frame("CONNECTED", respondHeaders, null);
        connections.connect(login,passcode);
        connections.send(connectionId, respondFrame);
    }

    private void handleSend(Map<String, String> headers, String body) {

        String destination = headers.get("destination");
        if (destination == null) {
            sendError("Missing destination in SEND");
            return;
        } 

        else if (!connections.isSubscribed(destination, connectionId)) {
            sendError("Client is not subscribed to the topic: " + destination);
            return;
        }
        connections.send(destination, body); 
        
    }

    private void handleSubscribe(Map<String, String> headers) {
        String destination = headers.get("destination");
        String subscriptionId = headers.get("id");
        if (destination == null || subscriptionId == null) {
            sendError("Missing destination or id in SUBSCRIBE");
            return;
        } 
        else if (connections.isSubscribed(destination,connectionId)){
            sendError("Client is already subscribed to the topic: " + destination);
            return;
        }

        connections.subscribe(destination, connectionId, subscriptionId);
    }
    
    private void handleUnsubscribe(Map<String, String> headers) {
        String subscriptionId = headers.get("id");
        
        if (subscriptionId == null) {
            sendError("Missing subscription ID in UNSUBSCRIBE");
            return;
        }
        else if (!connections.isSubscribed(subscriptionId, connectionId)){
            sendError("Client is not Subscribed. " );
            return;
        }

        boolean success = connections.unsubscribe(connectionId, subscriptionId); 
    
        if (!success) {
            sendError("Failed to unsubscribe. Subscription ID not found: " + subscriptionId);
        }
     }

     private void handleDisconnect(Map<String, String> headers) {
        String receipt = headers.get("receipt");

        if (!connections.isConnected(connectionId)) { 
            sendError("Client is already disconnected.");
        }
        else {
        connections.disconnect(connectionId);
        ConcurrentHashMap<String, String> respondHeaders = new ConcurrentHashMap<>();
        respondHeaders.put("receipt-id", receipt);
        Frame respondFrame = new Frame("DISCONNECT", respondHeaders, null);
        connections.send(connectionId, respondFrame);
        shouldTerminate = true;
        }
    }

         
    private void sendError( String errorMessage) {
        ConcurrentHashMap<String, String> respondHeaders = new ConcurrentHashMap<>();
        respondHeaders.put("receipt-id","message-12345");
        respondHeaders.put("message",errorMessage);
        Frame respondFrame = new Frame("ERROR", respondHeaders, null);
        connections.send(connectionId, respondFrame);
        shouldTerminate = true;
    }
    
}

