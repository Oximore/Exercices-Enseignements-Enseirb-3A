public class Tools {

    public static String GetChatUrl(String serverHost, String serverPort) {
	return "rmi://" + serverHost + ":" + serverPort + "/Chat";// + Constantes.CHAT_NAME; 
    }
}