
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

