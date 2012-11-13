import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;

public class Script
{
    public static void main(String args[])
    {
	org.omg.CORBA.Object objRef;
	Bank bank1;
	Bank bank2;
	Account account1;
	Account account2;
	Account account3;
	int id1;
	int id2;
	org.omg.CORBA.IntHolder id;

	try{
	    // create and initialize the ORB
	    ORB orb = ORB.init(args, null);
	    
	    // get the naming service
	    objRef = orb.resolve_initial_references("NameService");
	    NamingContextExt ncRef = NamingContextExtHelper.narrow(objRef);
	    
	    // resolve the object reference from the naming service
	    objRef = ncRef.resolve_str("bank.bank1");
	    bank1 = BankHelper.narrow(objRef);

	    objRef = ncRef.resolve_str("bank.bank2");
	    bank2 = BankHelper.narrow(objRef);

	    id = new org.omg.CORBA.IntHolder();	    
	    
	    /************************************************/
	    // Création d'un compte dans la banque 1
	    account1 = bank1.create(id);
	    assert (account1 != null);
	    
	    id1 = id.value;
	    assert(id1 == account1.getId());
	    	    
	    assert(account1.balance() == 0);
	    
	    System.out.println("Création du compte numéro " + account1.getId() + " dans la banque 1."); 
	    System.out.println("Solde du compte " + account1.getId() + " : " + account1.balance());
	    
	    
	    // Dépot de 500 sur ce comtpe
	    System.out.println("Depot de 500 sur le compte numéro " + account1.getId() + " dans la banque 1."); 
	    account1.deposit(500);
	    account1.balance();
	    assert(account1.balance() == 500);
	    System.out.println("Solde du compte " + account1.getId() + " : " + account1.balance());

	    // Retrait de 400 sur ce compte
	    System.out.println("Retrait de 400 sur le compte numéro " + account1.getId() + " dans la banque 1."); 
	    account1.withdraw(400);
	    account1.balance();
	    assert(account1.balance() == 100);
	    System.out.println("Solde du compte " + account1.getId() + " : " + account1.balance());

	   
	    account2 = bank1.lookup(id1);
	    assert(account2 != null);
	    account2.balance();
	    assert(account2.balance() == 100);
	   


	    // Déplacement de ce compte dans la banque 2
	    System.out.println("Déplacement du compte " + account1.getId() + " de la banque 1 vers la banque 2"  );
	    account3 = bank1.move(bank2,account1);
	    assert(account3 != null);
	    
	    assert(account3.balance() == 100);
	    System.out.println("Solde du compte " + account3.getId() + " : " + account3.balance());
	    
	    

	    

	    /************************************************/
	    	    
	} catch (Exception e) {
	    System.out.println("ERROR : " + e) ;
	    e.printStackTrace(System.out);
	}
    }
    

}

