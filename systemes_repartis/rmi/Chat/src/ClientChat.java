import java.rmi.Naming;
import java.rmi.server.UnicastRemoteObject;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.ArrayList;
import java.rmi.RemoteException;

public class ClientChat {

    private static String _nick = "";
    private static Chat _chatServer = null;
    private static ChatBackImpl _myChatBackImpl = null;

    private static void Usage(){
	System.out.println("Usage:");
	System.out.println("\tClientChat serverHost serverPort");
    }
    
    private static void WelcomeMessage(){ /* System.out.println("Bienvenue"); */ }
    
    public static void main(String args[]) { 
	if (args.length != 2) {
	    Usage();
	    System.exit(1);
	}
	
	try { 
	    // Argument filter
	    int iArg = 0;
	    String serverHost = args[iArg++];
	    String serverPort = args[iArg++];
	    
	    // Look up for the Remote Object Chat from Server RMI registry 
	    String chatUrl = Tools.GetChatUrl(serverHost, serverPort);
	    _chatServer = (Chat)Naming.lookup(chatUrl);
	    _nick = Constantes.DEFAULT_USER_NAME;	    

	    WelcomeMessage();

	    boolean haveToStop = false;
	    BufferedReader in;
	    while (!haveToStop) {
		
		in = new BufferedReader(new InputStreamReader(System.in));
		try {
		    String msg = in.readLine();
		    String firstWord = msg.trim().split(" ")[0];
		    
		    if (firstWord.equals("/help")) {
			Help();
		    } else if (firstWord.equals("/notify")) {
			Notify(msg);
		    } else if (firstWord.equals("/list")) {
			List();
		    } else if (firstWord.equals("/connect")) {
			Connect(msg);	
		    } else if (firstWord.equals("/disconnect")) {
			Disconnect();
		    } else if (firstWord.equals("/quit")) {
			Disconnect();
			haveToStop = true;
		    } else if (firstWord.startsWith("/")){
			System.out.println("Your argument is invalid !");
		    } else {
			Send(msg);
		    }
		}
		catch (RemoteException e) { e.printStackTrace(); } 
		catch(IOException e) { e.printStackTrace(); }
	    }
	    	    
	} catch (Exception e) { 
	    System.out.println("ClientChat Exception: " + e.getMessage()); 
	    e.printStackTrace(); 
	}
	System.exit(0);
    }

    private static void Help(){
	System.out.println("Commandes:");
	System.out.println("\t/connect pseudo");
	System.out.println("\t/disconnect");
	System.out.println("\t/help");
	System.out.println("\t/list");
	System.out.println("\t/notify dst msg");
	System.out.println("\t/quit");
	System.out.println("\tmsg");
    }

    private static void Notify(String msg) throws RemoteException {
	String user = msg.split(" ")[1]; 
	String message = msg.substring(msg.indexOf(user) + user.length()).trim();

        _chatServer.send(_nick,user,message);
    }
    
    private static void List() throws RemoteException {
	ArrayList<String> userList = _chatServer.list();
	System.out.println("Les utilisateurs actuellement connectés sont :");
	for (int i = 0 ; i < userList.size() ; ++i) {
	    System.out.println("\t" + userList.get(i));
	}
    }
   
    private static void Connect(String msg) throws RemoteException {
	if (!_nick.equals(Constantes.DEFAULT_USER_NAME)){
	    Disconnect();	    
	}
	_nick = msg.trim().split(" ")[1];

	// Create an object ChatBackImpl
	ChatBackImpl _myChatBackImpl = new ChatBackImpl();
	_chatServer.connect(_nick, _myChatBackImpl);
    }
	
    private static void Disconnect() throws RemoteException {
	_chatServer.disconnect(_nick);
	_nick = Constantes.DEFAULT_USER_NAME;
    }
    
    private static void Send(String message) throws RemoteException {
        _chatServer.send(_nick ,message);    
    }


}