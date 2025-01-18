package bgu.spl.net.api;

import java.util.HashMap;
import java.util.Map;

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
    public String process(String message) {//////////string instead of void 
        String[] lines = message.split("\n");
        String command = lines[0].trim(); 
        Map<String, String> headers = new HashMap<>();

        int i = 1;
        while (i < lines.length && !lines[i].trim().isEmpty()) {
            String[] header = lines[i].split(":", 2);
            if (header.length == 2) {
                headers.put(header[0].trim(), header[1].trim());
            }
            i++;
        }

        String body = (i < lines.length) ? lines[i].trim() : "";
        handleCommand(command, headers, body);
        return null;
}


    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    private void handleCommand(String command, Map<String, String> headers, String body) {
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
                handleDisconnect();
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

        else if (connections.isConnected(connectionId)) { 
            sendError("Client is already connected.");
            return;
        }

        else if (!connections.isSubscribed(login, connectionId)){
            sendError("Client is not subscribe.");
            return;
        }

        else {
             connections.send(connectionId, "CONNECTED\nversion:1.2\n\n\u0000");
        }
    }

    private void handleSubscribe(Map<String, String> headers) {
        String destination = headers.get("destination");
        if (destination == null) {
            sendError("Missing destination in SUBSCRIBE");
        } 
        else if(connections.isSubscribed(destination, connectionId)){
            sendError("Client is already subscribed to the topic: " + destination);
            return;
        }
        else {
            connections.subscribe(destination, connectionId);
            connections.send(connectionId, "SUBSCRIBED to " + destination);
        }
    }

    private void handleSend(Map<String, String> headers, String body) {

        String destination = headers.get("destination");
        if (destination == null) {
            sendError("Missing destination in SEND");
        } 

        else if (!connections.isSubscribed(destination, connectionId)) {
            sendError("Client is not subscribed to the topic: " + destination);
            return;
        }
        
        else {
            connections.send(destination, body); 
        }
    }

    private void handleDisconnect() {
        if (!connections.isConnected(connectionId)) { 
            sendError("Client is already disconnected.");
        }
        else {
        connections.disconnect(connectionId);
        shouldTerminate = true;
        }
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
         
    private void sendError(String errorMessage) {
        connections.send(connectionId, "ERROR\nmessage:" + errorMessage + "\n\n\u0000");
        shouldTerminate = true;
    }
    
}

