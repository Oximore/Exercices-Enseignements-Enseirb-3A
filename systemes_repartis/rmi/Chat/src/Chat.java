/* Chat interface by aurelien.esnard@labri.fr */

import java.rmi.Remote; 
import java.rmi.RemoteException; 
import java.util.ArrayList;

public interface Chat extends Remote { 
    void connect(String nick, ChatBack ref) throws RemoteException; 
    void disconnect(String nick) throws RemoteException; 
    ArrayList<String> list() throws RemoteException; 
    void send(String src, String msg) throws RemoteException; 
    void send(String src, String dst, String msg) throws RemoteException; 
}
