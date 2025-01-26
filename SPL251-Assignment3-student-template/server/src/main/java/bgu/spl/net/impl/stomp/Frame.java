package bgu.spl.net.impl.stomp;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class Frame {

    private String command;
    private ConcurrentHashMap <String, String> headers; 
    private String body;

    public Frame(String command, ConcurrentHashMap<String, String> headers, String body) {
        this.command = command;
        this.headers = headers;
        this.body = body;
    }

    // Getters and Setters
    public String getCommand() {
        return command;
    }

    public void setCommand(String command) {
        this.command = command;
    }

    public Map<String, String> getHeaders() {
        return headers;
    }

    public void setHeaders(ConcurrentHashMap <String, String> headers) {
        this.headers = headers;
    }

    public String getBody() {
        return body;
    }

    public void setBody(String body) {
        this.body = body;
    }

    public String toString(){
        StringBuilder sb = new StringBuilder();
        sb.append(command).append("\n"); 

        // Add headers
        for (Map.Entry<String, String> entry : headers.entrySet()) {
            sb.append(entry.getKey()).append(":").append(entry.getValue()).append("\n");
        }

        sb.append("\n"); 
        sb.append(body).append("\u0000"); 
        return sb.toString();
    }
}
