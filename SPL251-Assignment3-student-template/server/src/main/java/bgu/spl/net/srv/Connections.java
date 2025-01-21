package bgu.spl.net.srv;

import bgu.spl.net.impl.stomp.Frame;

public interface Connections<T> {

    void connect (String login, String passcode);

    boolean isConnected(String login, String passcode);

    boolean isConnected(int connectionId);

    boolean isPasscodeCorrect( String login, String passcode);

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void send(int connectionId, Frame respondFrame);
    
    void disconnect(int connectionId);

    void subscribe(String topic, int connectionId,String subscriptionId);

    boolean unsubscribe(int connectionId, String subscriptionId);


    boolean isSubscribed(String destination, int connectionId);


}
