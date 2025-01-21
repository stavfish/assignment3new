package bgu.spl.net.srv;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.CopyOnWriteArraySet;
import java.util.concurrent.atomic.AtomicInteger;
import bgu.spl.net.dataBase.User;
import bgu.spl.net.impl.stomp.Frame;

public class ConnectionsImpl<T> implements Connections<T> {

    private ConcurrentMap<String, User> users = new ConcurrentHashMap<>();// userName -> User
    private ConcurrentMap<Integer, ConnectionHandler<T>> clients = new ConcurrentHashMap<>(); // Map to store connection handlers for each client
    private ConcurrentMap<String, CopyOnWriteArraySet<Integer>> topicSubscribers = new ConcurrentHashMap<>();  //topic -> <connectionId>
    private ConcurrentMap<Integer, Map<String, String>> subscriptionMap = new ConcurrentHashMap<>(); //connectionId -> (subscriptionId -> topic)
    private AtomicInteger msgId = new AtomicInteger(1);

    @Override
    public void connect (String userName, String passcode){
        if (!users.containsKey(userName)){
            User newUser = new User(userName, passcode);
            users.put(userName, newUser);
        }
        else {
            User user = users.get(userName);
            user.logIn();
            }
        }
    
    public boolean isConnected(String userName, String passcode) {
        User user = users.get(userName);
        return user.isLogIn();
    }

    public boolean isConnected(int connectionId){
        return clients.containsKey(connectionId);
    }

    public boolean isPasscodeCorrect(String userName, String passcode){
        User user = users.get(userName);
        return user.getPasscode().equals(passcode);
    }

    public void subscribe(String topic, int connectionId, String subscriptionId) { // Subscribe a client to a topic
        if (!topicSubscribers.containsKey(topic)) {
            topicSubscribers.put(topic, new CopyOnWriteArraySet<>());
        }
        topicSubscribers.get(topic).add(connectionId);
        subscriptionMap.get(connectionId).put(subscriptionId, topic);
    }

    public boolean isSubscribed(String destination, int connectionId){
        CopyOnWriteArraySet<Integer> subscribers = topicSubscribers.get(destination);
        return subscribers != null && subscribers.contains(connectionId);  
    }

    public boolean unsubscribe(int connectionId, String subscriptionId) {
        Map<String, String> subscriptions = subscriptionMap.get(connectionId); 
        if (subscriptions == null) {
            return false;
        }
        String topic = subscriptions.remove(subscriptionId);
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

    public void send(String channel, T msg) { // Send the message to all clients subscribed to the topic
        CopyOnWriteArraySet<Integer> subscribers = topicSubscribers.get(channel);
        if (subscribers != null) {
            for (Integer connectionId : subscribers) {
                Map<String, String> subscriptions = subscriptionMap.get(connectionId);
                if (subscriptions != null) {
                    String subscriptionId = null;
                    for (Map.Entry<String, String> entry : subscriptions.entrySet()) {
                        if (entry.getValue().equals(channel)) {
                            subscriptionId = entry.getKey(); 
                            break;
                        }
                    }

                    if (subscriptionId != null) {
                        ConcurrentHashMap<String, String> respondHeaders = new ConcurrentHashMap<>();
                        respondHeaders.put("subscription", subscriptionId);
                        respondHeaders.put("message-id", String.valueOf(msgId.getAndIncrement()));
                        respondHeaders.put("destination", channel);
                        Frame messageFrame = new Frame("MESSAGE", respondHeaders, msg.toString());
                        send(connectionId, (T)messageFrame);
                    }
                }
            }
        }
    }

    public void send(int connectionId, Frame respondFrame){ // Send frame to the specific client
        send(connectionId, (T) respondFrame.toString());
    }  

    public boolean send(int connectionId, T msg) { // Send message to the specific client
        ConnectionHandler<T> handler = clients.get(connectionId);
        if (handler != null) {
            handler.send(msg); 
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

        clients.remove(connectionId);
    }
    
    public void addClient(int connectionId, ConnectionHandler<T> handler) { // Register a new client connection
        clients.put(connectionId, handler);
    }
}
