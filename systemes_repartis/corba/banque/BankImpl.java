import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;
import java.util.HashMap;

import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;


public class BankImpl extends BankPOA { 
    
    private HashMap<String,Account> _comptes;
    private int _compteurComptes;
    private POA _poa;
    
    
    public BankImpl(POA rootPoa) {
	super();
	_comptes = new HashMap<String,Account>();
	_compteurComptes = 0;
	_poa = rootPoa;
    }
    
    
    // Begin Interface Methodes
    public Account create (org.omg.CORBA.IntHolder id) {
	Account a = (Account) new AccountImpl(++_compteurComptes);
    	id.value = a.getId();
	_comptes.put(("" + a.getId()),a);		
	//	org.omg.CORBA.Object objref = _poa.servant_to_reference(a);
	System.out.println("Creation du compte " + a.getId() );
	return a;
    }		
    
    public void destroy (Account a){
	_comptes.remove(a);
    }
    
    public Account lookup (int id) {
	return _comptes.get("" + id);
    }
    
    public Account move (Bank target, Account a){
	int amount = a.balance();
	org.omg.CORBA.IntHolder newId = new org.omg.CORBA.IntHolder();
	Account newAccount = target.create(newId);
	newAccount.deposit(amount);
	this.destroy(a);
	return newAccount;
    }
    // End Interface Methodes
} 
