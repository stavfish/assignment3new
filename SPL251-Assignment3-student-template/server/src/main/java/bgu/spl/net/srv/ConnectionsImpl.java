package bgu.spl.net.srv;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.CopyOnWriteArraySet;
import java.util.concurrent.atomic.AtomicInteger;

import bgu.spl.net.dataBase.User;
import bgu.spl.net.impl.stomp.Frame;

public class ConnectionsImpl<T> implements Connections<Frame> {

    private AtomicInteger connectionId = new AtomicInteger(0);///added
    private AtomicInteger msgId = new AtomicInteger(1);
    private ConcurrentMap<String, User> users = new ConcurrentHashMap<>();// userName -> User
    private ConcurrentMap<Integer, ConnectionHandler<Frame>> clients = new ConcurrentHashMap<>(); // Map to store connection handlers for each client
    private ConcurrentMap<String, CopyOnWriteArraySet<Integer>> topicSubscribers = new ConcurrentHashMap<>();  //topic -> <connectionId>
    private ConcurrentMap<Integer, Map<String, String>> subscriptionMap = new ConcurrentHashMap<>(); //connectionId -> (subscriptionId -> topic)


    public int generateNewConnectionId() {
        return connectionId.incrementAndGet();
    }

    @Override
    public void connect (String userName, String passcode, int connectionId){
        User user = users.get(userName);
        if (user == null){
            User newUser = new User(userName, passcode, connectionId);
            users.put(userName, newUser);
        }
        else {
            user.logIn(connectionId);
            }
    }
    
    public boolean isConnected(String userName, String passcode) {
        User user = users.get(userName);
        return user != null && user.isLogIn();
    }

    public boolean isConnected(int connectionId){
        return clients.containsKey(connectionId);
    }

    public boolean isPasscodeCorrect(String userName, String passcode){
        User user = users.get(userName);
        if (user == null){
            return true;
        }
        return user.getPasscode().equals(passcode);
    }

    public void subscribe(String topic, int connectionId, String subscriptionId) { // Subscribe a client to a topic
        if (topic.startsWith("/")) {
            topic = topic.substring(1);
        }

        if (!topicSubscribers.containsKey(topic)) {
            topicSubscribers.put(topic, new CopyOnWriteArraySet<>());
        }
        topicSubscribers.get(topic).add(connectionId);
        if (!subscriptionMap.containsKey(connectionId)){
            subscriptionMap.put(connectionId, new ConcurrentHashMap<>());  
        }
        subscriptionMap.get(connectionId).put(subscriptionId, topic);        
    }

    public boolean isSubscribed(String destination, int connectionId){
        if (destination.startsWith("/")) {
            destination = destination.substring(1);
        }

        CopyOnWriteArraySet<Integer> subscribers = topicSubscribers.get(destination);
        return subscribers != null && subscribers.contains(connectionId);  
    }

    public boolean unsubscribe(int connectionId, String subscriptionId) {
        Map<String, String> subscriptions = subscriptionMap.get(connectionId); 
        if (subscriptions == null) {
            return false;
        }
        String topic = subscriptions.remove(subscriptionId);
        if (topic.startsWith("/")) {
            topic = topic.substring(1);
        }
        if (topic == null) {
            return false;
        }
        CopyOnWriteArraySet<Integer> subscribers = topicSubscribers.get(topic);
        if (subscribers != null) {
            subscribers.remove(connectionId);
            if (subscribers.isEmpty()) {
                topicSubscribers.remove(topic);
            }
        }
        return true;
    }

    public void send(String channel, Frame frame) { // Send the frame to all clients subscribed to the topic
        if (channel.startsWith("/")) {
            channel = channel.substring(1);
        }
        CopyOnWriteArraySet<Integer> subscribers = topicSubscribers.get(channel);
        if (subscribers != null) {
            for (Integer connectionId : subscribers) {
                Map<String, String> headers = frame.getHeaders();
                headers.replace("subscription", getSubscriptionId(connectionId,channel));
                headers.replace("message-id", String.valueOf(msgId.getAndIncrement()));
                send(connectionId, frame);
            }
        }
    }

    private String getSubscriptionId(int connectionId, String topic){
        Map<String, String> subscriptions = subscriptionMap.get(connectionId);
        if (subscriptions != null) {
            for (Map.Entry<String, String> entry : subscriptions.entrySet()) {
                if (entry.getValue().equals(topic)) {
                    return entry.getKey(); 
                }
            }
        }
        return null; 
    }

    public boolean send(int connectionId, Frame frame) { // Send message to the specific client
        ConnectionHandler<Frame> handler = clients.get(connectionId);
        if (handler != null) {
            handler.send(frame); 
            System.out.println("server send - " + frame.getCommand() + frame.getHeaders() + frame.getBody());
            return true;
        }
        return false;
    }

    @Override
    public void disconnect(int connectionId) { // Remove the client from all topics it subscribed to  
        Map<String, String> subscriptions = subscriptionMap.remove(connectionId);
        if (subscriptions != null) {
            for (String topic : subscriptions.values()) {
                CopyOnWriteArraySet<Integer> subscribers = topicSubscribers.get(topic);
                if (subscribers != null) {
                    subscribers.remove(connectionId);
                    if (subscribers.isEmpty()) {
                        topicSubscribers.remove(topic);
                    }
                }
            }
        }
        for (User user : users.values()) {
            if (user.getConnectionId() == connectionId) {
                user.logOut();
                break;
            }
        }
        clients.remove(connectionId);
    }
    
    public void addClient(int connectionId, ConnectionHandler<Frame> handler) { // Register a new client connection
        clients.put(connectionId, handler);
    }

    public void sendError(int connectionId, String errorMessage) {
        ConcurrentHashMap<String, String> respondHeaders = new ConcurrentHashMap<>();
        respondHeaders.put("message",errorMessage);
        Frame respondFrame = new Frame("ERROR", respondHeaders, null);
        send(connectionId, respondFrame);
        disconnect(connectionId);
    }
}
