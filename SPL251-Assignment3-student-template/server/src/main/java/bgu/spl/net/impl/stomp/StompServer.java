package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompEncoderDecoder;
import bgu.spl.net.api.StompMessagingProtocolImpl;
import bgu.spl.net.srv.ConnectionsImpl;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {

    if (args.length < 2) {
        System.out.println("Usage: StompServer <tpc/reactor> [port]");
        return;
    }
    String mode = args[1].toLowerCase();; // Either "tpc" or "reactor"
    int port; 
    try {
        port = Integer.parseInt(args[0]); // Parse port number
    } catch (NumberFormatException e) {
        System.out.println("Invalid port number.");
        return;
    }

    if (mode.equals("tpc")) {
        Server.threadPerClient(
                port,
                () -> new StompMessagingProtocolImpl(), 
                StompEncoderDecoder::new,
                new ConnectionsImpl<>()
        ).serve();
    }
    else if (mode.equals("reactor")) {
        Server.reactor(
                Runtime.getRuntime().availableProcessors(),
                port,
                () -> new StompMessagingProtocolImpl(), 
                StompEncoderDecoder::new,
                new ConnectionsImpl<>()
        ).serve();      
    }
    else {
        System.out.println("Invalid mode. Use 'tpc' for Thread-Per-Client or 'reactor' for Reactor.");
    }
    }
}