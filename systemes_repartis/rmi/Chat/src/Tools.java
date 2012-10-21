public class Tools {

    public static String GetChatUrl(String serverHost, String serverPort) {
	return "rmi://" + serverHost + ":" + serverPort + "/" + Constantes.CHAT_NAME; 
    }
}