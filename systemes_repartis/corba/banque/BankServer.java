import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;
import java.util.Properties;

// server program
public class BankServer {
    
    public static void main(String args[]) {

	org.omg.CORBA.Object objRef;

	try{
	    // Argument filter
	    String bankName = args[0];
	    
	    // create and initialize the ORB
	    String orbArgs[] = new String [args.length-1];
	    for (int i = 0 ; i<orbArgs.length ; ++i){
		orbArgs[i] = args[i+1];		
	    }

	    ORB orb = ORB.init(orbArgs, null);
	    
	    // get reference to rootpoa & activate the POAManager
	    POA rootpoa = POAHelper.narrow(orb.resolve_initial_references("RootPOA"));
	    rootpoa.the_POAManager().activate();

	    // get the naming service
	    objRef = orb.resolve_initial_references("NameService");
	    NamingContextExt ncRef = NamingContextExtHelper.narrow(objRef);
	    
	    // get object reference from the servant
	    BankImpl bank = new BankImpl(rootpoa, ncRef, bankName);
	    objRef = rootpoa.servant_to_reference(bank);

	    // bind the object reference in the naming service
	    String name = "bank." + bankName;
	    NameComponent path[] = ncRef.to_name(name);
	    ncRef.rebind(path, objRef);
	    
	    // wait for invocations from clients
	    System.out.println("BankServer ready and waiting...");
	    orb.run(); // blocking...
	} 
	
	catch (Exception e) {
	    System.err.println("ERROR: " + e);
	    e.printStackTrace(System.out);
	}
	
    }
}

