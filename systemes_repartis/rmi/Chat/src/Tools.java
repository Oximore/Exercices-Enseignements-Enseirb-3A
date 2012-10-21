public static class Tools {

    public static string GetChatUrl(string serverHost, string serverPort) {
	return "rmi://" + serverHost + ":" + serverPort + "/" + Constantes.CHAT_NAME; 
    }


}