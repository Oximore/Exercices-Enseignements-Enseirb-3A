import java.util.HashMap;

public class ServantChat implements Chat {
    
    public ServantChat(){
	this.map = new HashMap<String,ChatBack>();

    }

    private HashMap<String,ChatBack> map;


    // Begin Interface Chat
    void connect(String nick, ChatBack ref) throws RemoteException {
	if (map.get(nick) == null){
	    map.put(nick,ref);
	} 
	else {
	    // TODO ERREUR
	}
    }
    
    //void disconnect(String nick) throws RemoteException; 
    //ArrayList<String> list() throws RemoteException; 
    //void send(String src, String msg) throws RemoteException; 
    
    void send(String src, String dst, String msg) throws RemoteException {
	ChatBack ref = map.get(dst);
	if (ref != null) {
	    try {
		ref.send(src,msg);
		// System.out.println
	    } catch () {
		// TODO ERREUR
	    }
	}
	else {
	    // TODO ERREUR
	}
    }
    // End Interface Chat
}