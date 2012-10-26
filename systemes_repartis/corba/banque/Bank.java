import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;


public class Bank extends BankOPA { 
	
    private HashMap<int,Account> _comptes;
    private int _compteurComptes;
    private POA _poa;
	
	
    public Bank(POA rootPoa) {
	super();
	_comptes = new HashMap<int,Account>();
	_compteurComptes = 0;
	_poa = rootPoa;
    }
    
    
    // Begin Interface Methodes
    public Account create (org.omg.CORBA.IntHolder id) {
	Account a = new Account(++_compteurComptes);
    	id.value = a.Id;
	_comptes.put(a.Id,a);		

	//	org.omg.CORBA.Object objref = _poa.servant_to_reference(a);
			   
	System.out.println("Creation du compte " + a.Id );
	return a;
    }		
    
    
    public void destroy (Account a){
	/* TODO */
	comptes.remove(a);
    }
    
    Account lookup (int id) {
	return _compte.get(id);
    }
    
    public Account move (Bank target, Account a){
	/* TODO */
    }
    // End Interface Methodes
} 
