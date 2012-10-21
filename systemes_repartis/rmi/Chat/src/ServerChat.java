import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public static class ServerChat extends UnicastRemoteObject {
    public HelloServer() throws RemoteException {
	super();
    }
    
    public static void main(String args[]) { 
	if (args.length != 2) {
	    System.out.println("Usage: ServerChat host port");
	    System.exit(1);
	}
	
	try { 
	    Chat servant = (Chat) new ServantChat();

	    // Bind this object to the local RMI registry
	    String host = args[0];
	    String port = args[1];
	    String url = GetChatUrl(host, port); 
	    Naming.rebind(url, servant);
	    
        } catch (Exception e) { 
	    System.out.println("HelloServer Exception: " + e.getMessage()); 
	    e.printStackTrace(); 
        } 
    } 
    

}

