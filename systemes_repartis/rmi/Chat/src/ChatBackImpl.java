import java.rmi.RemoteException;
import java.io.Serializable;
import java.rmi.server.UnicastRemoteObject;

public class ChatBackImpl extends UnicastRemoteObject implements Serializable, ChatBack {
    public ChatBackImpl() throws RemoteException {}

    public void send(String src, String msg) throws RemoteException {
	System.out.println("\"" + src + "\" : " + msg);
    }
}