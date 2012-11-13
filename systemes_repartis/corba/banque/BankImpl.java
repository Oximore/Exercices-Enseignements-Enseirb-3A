import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;
import java.util.HashMap;

import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;


public class BankImpl extends BankPOA{ 
    
    public String Name;

    private HashMap<String,Account> _comptes;
    private int _compteurComptes;
    private POA _poa;
    private NamingContextExt _ncRef;
    
    public BankImpl(POA rootPoa, NamingContextExt ncRef, String name) {
	super();
	Name = name;
	_comptes = new HashMap<String,Account>();
	_compteurComptes = 0;
	_poa = rootPoa;
	_ncRef = ncRef;
    }
    
    
    // Begin Interface Methodes
    public Account create (org.omg.CORBA.IntHolder id) {
	Account res = null;
	AccountImpl ai = new AccountImpl(++_compteurComptes);
    	id.value = ai.getId();
	
	try
	    {
		org.omg.CORBA.Object objRef = _poa.servant_to_reference(ai); 
		_ncRef.rebind(_ncRef.to_name("bank." + Name + ai.getId()), objRef);
		
		res = AccountHelper.narrow(objRef);

		_comptes.put(("" + res.getId()),res);		
		System.out.println("Creation du compte " + res.getId() );
	    }
	catch(Exception in) {
	    System.out.println("invalid name"); /* TODO */ 
	}
	
	return res;
    }		
    
    public void destroy (Account a){
	/* TODO : suppression dans le serveur de nom */	
	System.out.println("Suppression du compte " + a.getId()+ " de la banque " + Name);
	_comptes.remove(a);
    }
    
    public Account lookup (int id) {
	System.out.println("Recherche du compte id=" + id + " dans la banque " + Name);
	return _comptes.get("" + id);
    }
    
    public Account move (Bank target, Account a){
	System.out.println("Creation du compte " + a.getId()+ " dans la banque " + Name );
	int amount = a.balance();
	org.omg.CORBA.IntHolder newId = new org.omg.CORBA.IntHolder();
	Account newAccount = target.create(newId);
	newAccount.deposit(amount);
	this.destroy(a);
	return newAccount;
    }
    // End Interface Methodes
} 
