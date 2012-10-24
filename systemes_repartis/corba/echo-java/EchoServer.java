// EchoServer.java

import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;
import java.util.Properties;

// object implementation (servant)
class EchoImpl extends EchoPOA {

    org.omg.CORBA.Any echoAny (org.omg.CORBA.Any x) {
	System.out.println("server: (any)" + (x.type()) x );  
	return x;
    }

    /*
    public String echoString(String msg) {
	System.out.println("server: " + msg);
	return msg;
    }    

    public int echoLong(int l) {
	System.out.println("server: (int)" + l);
	return l;
    }*/
    

}

// server program
public class EchoServer {
    
    public static void main(String args[]) {

	org.omg.CORBA.Object objRef;

	try{
	    
	    // create and initialize the ORB
	    ORB orb = ORB.init(args, null);
	    
	    // get reference to rootpoa & activate the POAManager
	    POA rootpoa = POAHelper.narrow(orb.resolve_initial_references("RootPOA"));
	    rootpoa.the_POAManager().activate();

	    // get the naming service
	    objRef = orb.resolve_initial_references("NameService");
	    NamingContextExt ncRef = NamingContextExtHelper.narrow(objRef);
	    
	    // get object reference from the servant
	    EchoImpl echoImpl = new EchoImpl();
	    objRef = rootpoa.servant_to_reference(echoImpl);
	    
	    // bind the object reference in the naming service
	    String name = "echo.echo"; // id.kind
	    NameComponent path[] = ncRef.to_name(name);
	    ncRef.rebind(path, objRef);
	    
	    // wait for invocations from clients
	    System.out.println("EchoServer ready and waiting...");
	    orb.run(); // blocking...
	} 
	
	catch (Exception e) {
	    System.err.println("ERROR: " + e);
	    e.printStackTrace(System.out);
	}
	
    }
}
