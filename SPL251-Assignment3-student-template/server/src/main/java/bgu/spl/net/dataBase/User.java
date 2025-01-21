package bgu.spl.net.dataBase;

public class User {
    String name;
    String passcode;
    boolean logIn;

    public User(String name, String passcode) {
        this.name = name;
        this.passcode = passcode;
        this.logIn = true;
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
    }

    public void logIn(){
        this.logIn = true;
    }

}
