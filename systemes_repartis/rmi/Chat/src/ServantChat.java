import java.util.HashMap;
import java.rmi.RemoteException;
import java.util.ArrayList;


public class ServantChat implements Chat {
    
    public ServantChat(){
	this.map = new HashMap<String,ChatBack>();

    }

    private HashMap<String,ChatBack> map;


    // Begin Interface Chat
    public void connect(String nick, ChatBack ref) throws RemoteException {
	if (map.get(nick) == null){
	    map.put(nick,ref);
	} 
	else {
	    // TODO ERREUR
	}
    }
    
    public void disconnect(String nick) throws RemoteException {
    } 
    
    public ArrayList<String> list() throws RemoteException {
	return null;
    } 
    
    public void send(String src, String msg) throws RemoteException {
    } 
    
    public void send(String src, String dst, String msg) throws RemoteException {
	ChatBack ref = map.get(dst);
	if (ref != null) {
	    try {
		ref.send(src,msg);
		// System.out.println
	    } catch (RemoteException e) {
		// TODO ERREUR
	    }
	}
	else {
	    // TODO ERREUR
	}
    }
    // End Interface Chat
}