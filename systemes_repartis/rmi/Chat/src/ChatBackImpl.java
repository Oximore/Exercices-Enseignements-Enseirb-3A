public class ChatBackImpl implements ChatBack {
    public ServantChat(){}

    void send(String src, String msg) throws RemoteException {
	System.out.println(src + ": " + msg);
    }
}