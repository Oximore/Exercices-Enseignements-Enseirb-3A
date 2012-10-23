import java.util.HashMap;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.io.Serializable;
import java.rmi.server.UnicastRemoteObject;

public class ServantChat extends UnicastRemoteObject implements Chat, Serializable  {
    
    public ServantChat() throws RemoteException {
	this._map = new HashMap<String,ChatBack>();
	//this.IsAlive = true;
    }

    private HashMap<String,ChatBack> _map;
    //private boolean IsAlive = false;

    // Begin Interface Chat
    public void connect(String nick, ChatBack ref) throws RemoteException {
	System.out.println("connect : " + nick);
	if (_map.get(nick) == null){
	    _map.put(nick,ref);
	} 
	else {
	    // se deco ?
	    // TODO ERREUR
	}
    }
    
    public void disconnect(String nick) throws RemoteException {
	System.out.println("disconnect : " + nick);
	if ( _map.get(nick) != null){
	    _map.remove(nick);
	} 
	else {
	    // TODO ERREUR
	}
    } 
    
    public ArrayList<String> list() throws RemoteException {
	System.out.println("give list");
	ArrayList<String> result = new ArrayList<String>();
	for (String user : _map.keySet()){
	    result.add(user);
	}	
	return result;
    } 
    
    public void send(String src, String msg) throws RemoteException {
	System.out.println("send : " + msg + " from " + src);
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
	System.out.println("send : " + msg + " from " + src + " to " + dst);
	
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