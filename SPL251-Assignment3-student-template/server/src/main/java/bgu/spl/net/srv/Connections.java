package bgu.spl.net.srv;

public interface Connections<T> {

    void connect (String login, String passcode, int connectionId);

    boolean isConnected(String login, String passcode);

    boolean isConnected(int connectionId);

    boolean isPasscodeCorrect( String login, String passcode);

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);
    
    void disconnect(int connectionId);

    void subscribe(String topic, int connectionId,String subscriptionId);

    boolean unsubscribe(int connectionId, String subscriptionId);

    boolean isSubscribed(String destination, int connectionId);


}
