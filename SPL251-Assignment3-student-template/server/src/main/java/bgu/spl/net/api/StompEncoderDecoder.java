package bgu.spl.net.api;

import java.nio.charset.StandardCharsets;
import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.impl.stomp.Frame;

public class StompEncoderDecoder implements MessageEncoderDecoder<Frame> {
    private StringBuilder buffer = new StringBuilder();

    @Override
    public Frame decodeNextByte(byte nextByte) {
        if (nextByte == '\u0000') { 
            String message = buffer.toString();
            buffer.setLength(0); 
            return fromString(message); 
        } else {
            buffer.append((char) nextByte);
            return null;
        }
    }

    private Frame fromString (String message){
        String[] lines = message.split("\n");
        String command = lines[0].trim(); 
        ConcurrentHashMap<String, String> headers = new ConcurrentHashMap<>();

        int i = 1;
        while (i < lines.length && !lines[i].trim().isEmpty()) {
            String[] header = lines[i].split(":", 2);
            if (header.length == 2) {
                headers.put(header[0].trim(), header[1].trim());
            }
            i++;
        }

        String body = (i < lines.length) ? lines[i].trim() : "";
        Frame newFrame = new Frame(command, headers, body);
        return newFrame;
    }

    @Override
    public byte[] encode(Frame message) {
        return (message.toString() + '\u0000').getBytes(StandardCharsets.UTF_8);
    }
}
