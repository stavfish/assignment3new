package bgu.spl.net.srv;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    void subscribe(String topic, int connectionId);

    boolean unsubscribe(int connectionId, String subscriptionId);

    boolean isConnected(int connectionId);

    boolean isSubscribed(String destination, int connectionId);

}
