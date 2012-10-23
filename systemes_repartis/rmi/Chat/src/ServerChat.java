import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;

public class ServerChat {

    private static void Usage(){
	System.out.println("Usage:");
	System.out.println("\tServerChat port");
    }

    public static void main(String args[]) { 
	if (args.length != 1) {
	    Usage();
	    System.exit(1);
	}
	
	try { 
	    // Argument Filter
	    int iArg = 0;
	    String host = "localhost";
	    String port = args[iArg++];
	    
	    Chat servant = (Chat) new ServantChat();
	    	    
	    // Bind this object to the local RMI registry
	    String url = Tools.GetChatUrl(host, port); 
	    Naming.rebind(url, servant);
	    
	    // To realse server : 
	    // UnicastRemoteObject.unexportObject(servant,true);
	    	    
        } catch (Exception e) { 
	    System.out.println("HelloServer Exception: " + e.getMessage()); 
	    e.printStackTrace(); 
        } 
    } 
}

