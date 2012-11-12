import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;

//être capable de lancer deux banques servers
//un compte A dans une banque B
//le changer de banque en B'
//on appelle B.move(B',A) qui renvoie un compte A'
//on programme move() en utilisant seulement les fonctions déjà disponibles

public class AccountImpl extends AccountPOA {

    public int Id;
    private int _solde;
  
    public AccountImpl (int id){
	this.Id = id;
	_solde = 0;
    }
    
    public int getId () {
	return this.Id ;
    }
    
    public void deposit (int amount){
  	_solde += amount;
    }
   
    public void withdraw (int amount){
  	_solde -= amount;
    }
    
    public int balance (){
  	return _solde;
    }
} 
