import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;

//être capable de lancer deux banques servers
//un compte A dans une banque B
//le changer de banque en B'
//on appelle B.move(B',A) qui renvoie un compte A'
//on programme move() en utilisant seulement les fonctions déjà disponibles

public class Account
{
  private int solde;
  public String nom;
  
  public Account (nom){
  	solde=0;
  	this.nom=nom;
  }
  public void deposit (int amount){
  	solde+=amount;
  }
  
  
  public void withdraw (int amount){
  	solde-=amount;
  }
  
  
  public int balance (){
  	return solde;
  }
} 
