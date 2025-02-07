package bgu.spl.net.api;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import bgu.spl.net.impl.stomp.Frame;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<Frame> {
    private int connectionId;
    private Connections<Frame> connections;
    private boolean shouldTerminate = false;


    @Override
    public void start(int connectionId, Connections<Frame> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
    }

    @Override
    public void process(Frame message) {
        try {
            handleCommand(message);
        } 
        catch (IllegalArgumentException e) {
            sendError("Invalid STOMP frame: " + e.getMessage(), message)
            ;
        }
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    private void handleCommand(Frame frame) {
        String command = frame.getCommand();
        if (frame.getHeaders() == null){
            sendError("Missing headers in Frame " + command, frame);
             return;
        }
        
        switch (command) {
            case "CONNECT":
                handleConnect(frame);
                break;
            case "SUBSCRIBE":
                handleSubscribe(frame);
                break;
            case "UNSUBSCRIBE":
                handleUnsubscribe(frame);
                break;
            case "SEND":
                handleSend(frame);
                break;
            case "DISCONNECT":
                handleDisconnect(frame);
                break;
            default:
                sendError("Unknown error. ",frame);
        }
    }

    private void handleConnect(Frame frame) {
        Map<String, String> headers = frame.getHeaders();
        String login = headers.get("login");
        String passcode = headers.get("passcode");
  
        if (login == null || passcode == null) {
             sendError("Missing login or passcode in CONNECT", frame);
             return;
        } 

        else if (connections.isConnected(login, passcode)) { 
            sendError("User is already login.", frame);
            return;
        }

        else if (!connections.isPasscodeCorrect(login, passcode)){
            sendError("Invalid Passcode.", frame);
            return;
        }

        ConcurrentHashMap<String, String> respondHeaders = new ConcurrentHashMap<>();
        respondHeaders.put("version", "1.2");
        Frame respondFrame = new Frame("CONNECTED", respondHeaders, null);
        connections.connect(login,passcode,connectionId);
        connections.send(connectionId, respondFrame);
    }

    private void handleSend(Frame frame) {
        Map<String, String> headers = frame.getHeaders();
        String body = frame.getBody();
        if (body == null){
            sendError("Missing body in Send", frame);
             return;
        }
        String destination = headers.get("destination");
        if (destination == null) {
            sendError("Missing destination in SEND", frame);
            return;
        } 
        if (destination.startsWith("/")) {
            destination = destination.substring(1);
        }

        else if (!connections.isSubscribed(destination, connectionId)) {
            sendError("Client:"+ connectionId+ " is not subscribed to the topic: " + destination, frame);
            return;
        }
        ConcurrentHashMap<String, String> respondHeaders = new ConcurrentHashMap<>();
        respondHeaders.put("subscription", "null");
        respondHeaders.put("message-id", "null");
        respondHeaders.put("destination", "/"+destination);
        Frame messageFrame = new Frame("MESSAGE", respondHeaders, body);
        connections.send(destination, messageFrame); 
        
    }

    private void handleSubscribe(Frame frame) {
        Map<String, String> headers = frame.getHeaders();
        String destination = headers.get("destination");
        String subscriptionId = headers.get("id");
        String receipt = headers.get("receipt");
        if (destination == null || subscriptionId == null || receipt == null) {
            sendError("Missing destination or id or receipt in SUBSCRIBE", frame);
            return;
        } 
        if (destination.startsWith("/")) {
            destination = destination.substring(1);
        }

        else if (connections.isSubscribed(destination,connectionId)){
            sendError("Client is already subscribed to the topic: " + destination, frame);
            return;
        }

        connections.subscribe(destination, connectionId, subscriptionId);
        ConcurrentHashMap<String, String> respondHeaders = new ConcurrentHashMap<>();
        respondHeaders.put("receipt-id", receipt);
        Frame respondFrame = new Frame("RECEIPT", respondHeaders, null);
        connections.send(connectionId, respondFrame);
    }
    
    private void handleUnsubscribe(Frame frame) {
        Map<String, String> headers = frame.getHeaders();
        String subscriptionId = headers.get("id");
        String receipt = headers.get("receipt");
        
        if (subscriptionId == null || receipt == null) {
            sendError("Missing subscription ID in UNSUBSCRIBE", frame);
            return;
        }
    
        boolean success = connections.unsubscribe(connectionId, subscriptionId); 
    
        if (!success) {
            sendError("Failed to unsubscribe. Subscription ID not found: " + subscriptionId, frame);
        }

        ConcurrentHashMap<String, String> respondHeaders = new ConcurrentHashMap<>();
        respondHeaders.put("receipt-id", receipt);
        Frame messageFrame = new Frame("RECEIPT", respondHeaders, null);
        connections.send(connectionId, messageFrame); 

     }

     private void handleDisconnect(Frame frame) {
        Map<String, String> headers = frame.getHeaders();
        String receipt = headers.get("receipt");

        if (!connections.isConnected(connectionId)) { 
            sendError("Client is already disconnected.", frame);
        }
        else {
        ConcurrentHashMap<String, String> respondHeaders = new ConcurrentHashMap<>();
        respondHeaders.put("receipt-id", receipt);
        Frame respondFrame = new Frame("RECEIPT", respondHeaders, null);
        connections.send(connectionId, respondFrame);
        connections.disconnect(connectionId);
        shouldTerminate = true;
        }
    }
  
    private void sendError(String errorMessage, Frame relatedFrame) {
        ConcurrentHashMap<String, String> respondHeaders = new ConcurrentHashMap<>();

        if (relatedFrame != null && relatedFrame.getHeaders().containsKey("receipt")) {
            respondHeaders.put("receipt-id", relatedFrame.getHeaders().get("receipt"));
        }
        respondHeaders.put("message",errorMessage);

        String body = "The message:\n" + (relatedFrame != null ? relatedFrame.toString() : "");

        Frame respondFrame = new Frame("ERROR", respondHeaders, body);
        connections.send(connectionId, respondFrame);
        shouldTerminate = true;
        connections.disconnect(connectionId); 
    }
}

