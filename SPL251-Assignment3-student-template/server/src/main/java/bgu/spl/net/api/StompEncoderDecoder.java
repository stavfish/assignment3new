package bgu.spl.net.api;

import java.nio.charset.StandardCharsets;

import bgu.spl.net.impl.stomp.Frame;

public class StompEncoderDecoder implements MessageEncoderDecoder<Frame> {
    private StringBuilder buffer = new StringBuilder();

    @Override
    public Frame decodeNextByte(byte nextByte) {
        if (nextByte == '\0') { 
            String message = buffer.toString();
            buffer.setLength(0); 
            return Frame.fromString(message); 
        } else {
            buffer.append((char) nextByte);
            return null;
        }
    }

    @Override
    public byte[] encode(Frame message) {
        return (message.toString() + '\0').getBytes(StandardCharsets.UTF_8);
    }
    
    
}
