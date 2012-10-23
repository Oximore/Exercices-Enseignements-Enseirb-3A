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
	    
	    try {
		// Wait an user connection 
		// and quit where all users are deconnected
		boolean haveAnUser = false;
		boolean hadAnUser = false;
		
		ArrayList userList;
		while (haveAnUser || !hadAnUser) {
		    userList = servant.list();
		    hadAnUser = hadAnUser || haveAnUser;
		    haveAnUser = (userList.size() > 0);
		    Thread.sleep(1000);
		}
	    }
	    finally {
		UnicastRemoteObject.unexportObject(servant,true);
	    }
	    
        } catch (Exception e) { 
	    System.out.println("HelloServer Exception: " + e.getMessage()); 
	    e.printStackTrace(); 
        } 
    } 
}

