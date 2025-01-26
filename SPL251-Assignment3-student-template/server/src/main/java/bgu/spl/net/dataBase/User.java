package bgu.spl.net.dataBase;

public class User {
    String name;
    String passcode;
    boolean logIn;
    int connectionId;

    public User(String name, String passcode,int connectionId ) {
        this.name = name;
        this.passcode = passcode;
        this.logIn = true;
        this.connectionId = connectionId;
    }

    public String getUserName(){
        return this.name;
    }

    public String getPasscode() {
        return passcode;
    }

    public boolean isLogIn(){
        return logIn;
    }

    public void logOut(){
        this.logIn = false;
        this.connectionId = -1;
    }

    public void logIn(int connectionId){
        this.logIn = true;
        this.connectionId = connectionId;
    }

    public int getConnectionId(){
        return this.connectionId;
    } 

}
