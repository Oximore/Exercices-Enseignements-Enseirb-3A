import java.util.HashMap;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.io.Serializable;
import java.rmi.server.UnicastRemoteObject;

public class ServantChat extends UnicastRemoteObject implements Chat, Serializable  {
    
    public ServantChat() throws RemoteException {
	this._map = new HashMap<String,ChatBack>();
	_myName = Constantes.SERVER_NAME;
    }

    private HashMap<String,ChatBack> _map;
    private String _myName;
    
    
    // Begin Interface Chat
    public void connect(String nick, ChatBack ref) throws RemoteException {
	System.out.println("connect : " + nick);
	if (_map.get(nick) == null){
	    send(_myName,"User " + nick + " is online");
	    _map.put(nick,ref);
	    ref.send(_myName, "You are now connected as " + nick + ".");
	} 
	else {
	    ref.send(_myName, "The pseudonym \"" + nick + "\" is already used by an other user");
	    /* TODO ERREUR */
	}
    }
    
    public void disconnect(String nick) throws RemoteException {
	System.out.println("disconnect : " + nick);
	if ( _map.get(nick) != null){
	    _map.remove(nick);
	    send(_myName,"User " + nick + " is offline");
	} 
	else { /* TODO ERREUR */ }
    } 
    
    public ArrayList<String> list() throws RemoteException {
	System.out.println("ask list");
	ArrayList<String> result = new ArrayList<String>();
	for (String user : _map.keySet()){
	    result.add(user);
	}
	return result;
    } 
    
    public void send(String src, String msg) throws RemoteException {
	System.out.println("<" + src + " to All> : " + msg);
	ChatBack chatUser;
	for (String user : _map.keySet()){
	    if (!user.equals(src)) {
		chatUser = _map.get(user);
		try {
		    chatUser.send(src, msg);
		} catch (RemoteException e) {
		    e.printStackTrace();
		}
	    }
	}	
    } 
    
    public void send(String src, String dst, String msg) throws RemoteException {
	System.out.println("<" + src + " to " + dst  + "> : " + msg);
	
	ChatBack ref = _map.get(dst);
	if (ref != null) {
	    try {
		ref.send(src,msg);
	    } catch (RemoteException e) {
		e.printStackTrace();
	    }
	}
	else {
	    ref = _map.get(src);
	    if (ref != null){
		ref.send(_myName, "User \"" + dst + "\" do not exist.\n Try /list to view users list.");
	    }
	    else { /* TODO ERREUR */ }
	}
    }
    // End Interface Chat
}