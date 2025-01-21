package bgu.spl.net.api;

import java.nio.charset.StandardCharsets;

public class StompEncoderDecoder implements MessageEncoderDecoder<String> {
    private StringBuilder buffer = new StringBuilder();

    @Override
    public String decodeNextByte(byte nextByte) {
        //end massage
        if (nextByte == '\0'){
            if (buffer.length() == 0) { //empty messages
                return null;
            }
            String massage = buffer.toString();
            buffer = new StringBuilder();
            return massage;
        }
        else {
            buffer.append((char) nextByte);
            return null;
        }
    }

    @Override
    public byte[] encode(String message) {
         return (message + '\0').getBytes(StandardCharsets.UTF_8);
    }
    
    
}
