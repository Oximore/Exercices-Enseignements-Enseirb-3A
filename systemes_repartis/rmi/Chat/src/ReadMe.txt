Mini-projet : Création d'un chat à l'aide de Java RMI

Participants :
    Benjamin Lux
    Gabrielle Schmidt
    Pierre-Yves Caneill

Sujet :
    http://www.labri.fr/perso/esnard/Teaching/SysRep/RMI/TP2.txt

Git : 
    https://github.com/Oximore/Exercices-Enseignements-Enseirb-3A/tree/master/systemes_repartis/rmi/Chat



Compiler le projet:
    $ make

Lancer le projet:
    $ rmiregistery <port>
    java ServerChat <port>

Lancer un client:
    $ java ClientChat [<host>,localhost] <port>

Nettoyer le répertoire:
    $ make clean


Remarques :
    ServantChat n'est pas thread safe.
    Même si dans la pratique avec peu d'opérations par seconde cela n'est pas un problème certaines opérations peuvent en théorie entrainer des bugs. 
    Exemple : deux utilisateurs veulents se conncecter au même moment avec le même pseudo.
    Remarque : si la contrainte "au même moment" disparai : aucun problème.
