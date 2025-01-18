package bgu.spl.net.srv;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.CopyOnWriteArraySet;

public class ConnectionsImpl<T> implements Connections<T> {

    private ConcurrentMap<Integer, ConnectionHandler<T>> clients = new ConcurrentHashMap<>(); // Map to store connection handlers for each client
    private ConcurrentMap<String, CopyOnWriteArraySet<Integer>> topicSubscribers = new ConcurrentHashMap<>();  // Map to track which clients are subscribed to each topic
    private final ConcurrentMap<Integer, Map<String, String>> subscriptions = new ConcurrentHashMap<>();


    @Override
    public boolean send(int connectionId, T msg) { // Send message to the specific client
        ConnectionHandler<T> handler = clients.get(connectionId);
        if (handler != null) {
            handler.send(msg); 
            return true;
        }
        return false;
    }

    @Override
    public void send(String channel, T msg) { // Send the message to all clients subscribed to the topic
        CopyOnWriteArraySet<Integer> subscribers = topicSubscribers.get(channel);
        if (subscribers != null) {
            for (Integer connectionId : subscribers) {
                send(connectionId, msg); 
            }
        }
    }

    @Override
    public void disconnect(int connectionId) { // Remove the client from all topics it subscribed to
        for (CopyOnWriteArraySet<Integer> subscribers : topicSubscribers.values()) {
            subscribers.remove(connectionId);
        }
        clients.remove(connectionId);
    }
    
    public void addClient(int connectionId, ConnectionHandler<T> handler) { // Register a new client connection
        clients.put(connectionId, handler);
    }

    public void subscribe(String topic, int connectionId) { // Subscribe a client to a topic
        topicSubscribers.computeIfAbsent(topic, k -> new CopyOnWriteArraySet<>()).add(connectionId);
    }

    public boolean unsubscribe(int connectionId, String subscriptionId) {
        CopyOnWriteArraySet<Integer> subscribers = topicSubscribers.get(subscriptionId);
        if (subscribers != null) {
            subscribers.remove(connectionId);
            if (subscribers.isEmpty()) {
                topicSubscribers.remove(subscriptionId);
            }
            return true;
        }
        return false;
    }

    public boolean isConnected(int connectionId) {
        return clients.containsKey(connectionId);
    }

    public boolean isSubscribed(String destination, int connectionId){
        CopyOnWriteArraySet<Integer> subscribers = topicSubscribers.get(destination);
        return subscribers != null && subscribers.contains(connectionId);  
    }


}
