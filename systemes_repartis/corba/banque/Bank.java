import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;


public class Bank extends BankOPA
{ 
	
	private ArrayList<Account> comptes;
	
  	public Bank(){
  		super();
  	}
  	
  	public Account create (String nom){
	  	Account a = new Account(nom);
	  	comptes.add(a);
	  	System.out.println("Creation d'un nouveau compte :" + nom);
	  	return a;
  	}		
  
  	
	public void destroy (Account a){
		System.out.println("Suppression du compte :" + a.nom);
		comptes.remove(a);
		System.out.println("Compte supprime");
	}
  	
  
  
  	public Account move (Bank target, Account a){
  
 	}
} 
