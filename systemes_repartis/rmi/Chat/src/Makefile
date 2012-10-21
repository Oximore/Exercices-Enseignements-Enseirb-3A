CJ=javac
R=java
JFLAGS=
WFLAGS=-Xlint:unchecked
EXECCLIENT=ClientChat.class
EXECSERVEUR=ServerChat.class
SRC=$(wildcard *.java)
EXECS=$(SRC:.java=.class )


all: $(SRC)
	-rm *.class
	$(CJ) ClientChat.java
	$(CJ) ServerChat.java

#W: $(EXEC)
#	-rm *.class
#	$(CJ) $(WFLAGS) Pair.java

#run: $(EXEC) client
#	$(R) Pair

#client: ClientChat.java ServeurEssaisRepete.java
#	$(CJ) $<

.PHONY: clean clean_dwn mrproper

clean:
	@-rm  *.class *# *~ 

