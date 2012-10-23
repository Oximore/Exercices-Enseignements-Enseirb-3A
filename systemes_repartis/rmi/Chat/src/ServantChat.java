import java.util.HashMap;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.io.Serializable;


public class ServantChat implements Chat, Serializable  {
    
    public ServantChat(){
	this._map = new HashMap<String,ChatBack>();

    }

    private HashMap<String,ChatBack> _map;

    // Begin Interface Chat
    public void connect(String nick, ChatBack ref) throws RemoteException {
	if (_map.get(nick) == null){
	    _map.put(nick,ref);
	    // System.out.println("New pseudo : " + nick);
	} 
	else {
	    // se deco ?
	    // TODO ERREUR
	}
    }
    
    public void disconnect(String nick) throws RemoteException {
	if ( _map.get(nick) != null){
	    _map.remove(nick);
	} 
	else {
	    // TODO ERREUR
	}
    } 
    
    public ArrayList<String> list() throws RemoteException {
	ArrayList<String> result = new ArrayList<String>();
	for (String user : _map.keySet()){
	    result.add(user);
	}	
	return result;
    } 
    
    public void send(String src, String msg) throws RemoteException {
	ChatBack chatUser;
	for (String user : _map.keySet()){
	    if (!user.equals(src)) {
		chatUser = _map.get(user);
		try {
		chatUser.send(src, msg);
		} catch (RemoteException e) {
		    // TODO ERREUR
		}
	    }
	}	
    } 
    
    public void send(String src, String dst, String msg) throws RemoteException {
	ChatBack ref = _map.get(dst);
	if (ref != null) {
	    try {
		ref.send(src,msg);
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