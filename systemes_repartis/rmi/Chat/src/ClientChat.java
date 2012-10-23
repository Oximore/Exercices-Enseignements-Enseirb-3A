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

    public static void main(String args[]) { 
	if (args.length != 4) {
	    System.out.println("Usage: ClientChat myHost myPort serverHost serverPort");
	    System.exit(1);
	}
	
	try { 
	    // System.out.println("Bienvenue");

	    // Argument filter
	    int iArg = 0;
	    String myHost = args[iArg++];
	    String myPort = args[iArg++];
	    String serverHost = args[iArg++];
	    String serverPort = args[iArg++];
	    
	    
	    //	    Chat _chatServer = null;
	    
	    // Look up for the Remote Object Chat from Server RMI registry 
	    String chatUrl = Tools.GetChatUrl(serverHost, serverPort);
	    _chatServer = (Chat)Naming.lookup(chatUrl);
	    _nick = Constantes.DEFAULT_USER_NAME;	    


	    boolean haveToStop = false;
	    BufferedReader in;
	    while (!haveToStop) {
		in = new BufferedReader(new InputStreamReader(System.in));
		try {
		    String msg = in.readLine();
		    String firstWord = msg.trim().split(" ")[0];
		    
		    if (firstWord.equals("/notify")) {
			Notify(msg);
		    } else if (firstWord.equals("/list")) {
			List();
		    } else if (firstWord.equals("/connect")) {
			Connect(msg);	
		    } else if (firstWord.equals("/disconnect")) {
			Disconnect();
			haveToStop = true;
		    } else {
			Send(msg);
		    }
		    
		}
		catch (RemoteException e) { /* ... */} 
		catch(IOException e) { /* ... */ }
	    }
	    	    
	} catch (Exception e) { 
	    System.out.println("HelloServer Exception: " + e.getMessage()); 
	    e.printStackTrace(); 
	
	}
    }



    // TODO : Finir ces fonctions ...

    private static void Notify(String msg) throws RemoteException {
	String cmd[] = msg.split(" ");
	String user = cmd[1]; 
	String message = "";
	for (int i = 2 ; i<cmd.length ; ++i)
	    message += cmd[i];
	message = message.trim();

        _chatServer.send(_nick,user,message);
    }
    
    private static void List() throws RemoteException {
	ArrayList<String> userList = _chatServer.list();

	System.out.println("Les utilisateurs actuellement connectés sont :");
	
	for (int i = 0 ; i < userList.size() ; ++i) {
	    System.out.println("\t" + userList.get(i));
	}
	System.out.println("");
    }
   
    private static void Connect(String msg) throws RemoteException {
	if (!_nick.equals(Constantes.DEFAULT_USER_NAME)){
	    Disconnect();	    
	}
	
	_nick = msg.trim().split(" ")[1];

	// Créer un objet ChatBackImpl
	ChatBackImpl _myChatBackImpl = new ChatBackImpl();
	
	// String myChatBackUrl = /* TODO */ ; // GetChatUrl(host,port);
	// Naming.rebind(myChatBackUrl, myChatBack);
	
	_chatServer.connect(_nick, _myChatBackImpl);
    }
	
    private static void Disconnect() throws RemoteException {
	_chatServer.disconnect(_nick);
	UnicastRemoteObject.unexportObject(_myChatBackImpl,true);
	_nick = Constantes.DEFAULT_USER_NAME;
    }
    
    private static void Send(String message) throws RemoteException {
        _chatServer.send(_nick ,message);    
    }


}