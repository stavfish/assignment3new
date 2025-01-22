package bgu.spl.net.impl.stomp;

import java.util.function.Supplier;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.api.StompEncoderDecoder;
import bgu.spl.net.api.StompMessagingProtocolImpl;
import bgu.spl.net.srv.Server;

public class StompServer {
    public static void main(String[] args) {
        if (args.length < 2) {
            System.out.println("Usage: StompServer <tpc/reactor> [port]");
            return;
        }

        String mode = args[1]; // Either "tpc" or "reactor"
        int port; // Parse port number
        try {
            port = Integer.parseInt(args[0]); // Parse port number
        } catch (NumberFormatException e) {
            System.out.println("Invalid port number.");
            return;
        }

        // Suppliers for protocol and encoder/decoder
        Supplier<MessagingProtocol<Frame>> protocolFactory = StompMessagingProtocolImpl::new;
        Supplier<MessageEncoderDecoder<Frame>> encoderDecoderFactory = StompEncoderDecoder::new;
        
        if (mode.equalsIgnoreCase("tpc")) {
            // Thread-Per-Client mode
            Server<String> server = Server.threadPerClient(port, protocolFactory, encoderDecoderFactory);
            server.serve();
        } 
        
        else if (mode.equalsIgnoreCase("reactor")) {
            // Reactor mode
            int nThreads = Runtime.getRuntime().availableProcessors(); 
            Server<String> server = Server.reactor(nThreads, port, protocolFactory, encoderDecoderFactory);
            server.serve();
        } 

        else {
            System.out.println("Invalid mode. Use 'tpc' for Thread-Per-Client or 'reactor' for Reactor.");
        }
    }
}
