import java.rmi.RemoteException;

public class ChatBackImpl implements ChatBack {
    public ChatBackImpl(){}

    public void send(String src, String msg) throws RemoteException {
	System.out.println(src + ": " + msg);
    }
}