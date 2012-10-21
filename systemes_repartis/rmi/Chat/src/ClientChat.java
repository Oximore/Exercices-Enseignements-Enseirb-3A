


public static class ClientChat {

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
	    
	    // Look up for the Remote Object Chat from Server RMI registry 
	    String chatUrl = GetChatUrl(serverHost, serverPort);
	    Chat chatServer = (Chat)Naming.lookup(chatUrl);
	    String nick = Constantes.DEFAULT_USER_NAME;	    


	    boolean haveToStop = false;
	    BufferedReader in;
	    while (!haveToStop) {
		in = new BufferedReader(new InputStreamReader(System.in));
		try {
		    String msg = in.readLine();
		    String firstWord = msg.tip().Split()[0];

		    switch (firstWord) {
		    case ("/notify") :
			Notify(chatServer, nick, msg);
			break;
		    case ("/list") :
			List(chatServer);
			break;
		    case ("/connect") :
			Connect(chatServer, ref nick, msg);	
			break;
		    case ("/disconnect") :
			Disconnect(chatServer, nick);
			haveToStop = true;
			break;
		    default:
			Send(chatServer, nick, msg);
		    }
		    
		} catch(IOException e) { /* ... */ }
		
	    }


	} catch (Exception e) { 
	    System.out.println("HelloServer Exception: " + e.getMessage()); 
	    e.printStackTrace(); 
	}
    }




    // TODO : Finir ces fonctions ...

    private void Notify(Chat chat, String nick, String msg){
	String cmd[] = msg.Split(' ');
	String user = cmd[1]; 
	String message = String.empty();
	for (int i = 2 ; i<cmd.length ; ++i)
	    message += cmd[i];
	message = message.trim();

        chat.send(nick,user,message);
    }
    
    private void List(Chat chat) {
	ArrayList<String> userList = chat.list();

	System.out.println("Les utilisateurs actuellement connectés sont :");
	foreach (user in userList) {
	    System.out.println("\t" + user);
	}
	System.out.println("");
    }
   
    private void Connect(Chat chat, ref String nick, String msg) {
	
	// Créer un objet ChatBackImpl
	ChatBackImpl myChatBack = new ChatBackImpl();
	//String myChatBackUrl = /* TODO */ ; // GetChatUrl(host, port); 
	//Naming.rebind(myChatBackUrl, myChatBack);
	
	chat.connect(nick, myChatBack);
    }
	
    private void Disconnect(Chat chat, ref String nick){
	chat.disconnect(nick);
	nick = Constantes.DEFAULT_USER_NAME;
    }
    
    private void Send(Chat chat, String nick, String msg) {
        chat.send(nick ,message);    
    }


}