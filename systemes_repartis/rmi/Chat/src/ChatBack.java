/* ChatBack interface by aurelien.esnard@labri.fr */

import java.rmi.Remote; 
import java.rmi.RemoteException; 

public interface ChatBack extends Remote { 
    void send(String src, String msg) throws RemoteException; 
}