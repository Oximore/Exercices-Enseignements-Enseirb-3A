import java.rmi.RemoteException;
import java.io.Serializable;


public class ChatBackImpl implements Serializable, ChatBack {
    public ChatBackImpl(){}

    public void send(String src, String msg) throws RemoteException {
	System.out.println(src + ": " + msg);
    }
}