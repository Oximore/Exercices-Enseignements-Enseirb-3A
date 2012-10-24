// EchoClient.java

import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;

// client program
public class EchoClient
{
    public static void main(String args[])
    {
	org.omg.CORBA.Object objRef;

	try{
	    // create and initialize the ORB
	    ORB orb = ORB.init(args, null);
	    
	    // get the naming service
	    objRef = orb.resolve_initial_references("NameService");
	    NamingContextExt ncRef = NamingContextExtHelper.narrow(objRef);
	    
	    // resolve the object reference from the naming service
	    String name = "echo.echo"; // id.kind
	    objRef = ncRef.resolve_str(name);
	    
	    // convert the object reference into Echo reference
	    Echo echoRef = EchoHelper.narrow(objRef);
	    
	    // remote method invokation
	    org.omg.CORBA.Any lun = org.omg.CORBA.Any();
	    lun.insert_char('j');

	    System.out.println("client: " + lun);
	    //System.out.println("client: " + echoRef.echoString("coucou"));
	    //System.out.println("client: " + echoRef.echoLong(22));
	    
	} catch (Exception e) {
	    System.out.println("ERROR : " + e) ;
	    e.printStackTrace(System.out);
	}
    }
    
}

