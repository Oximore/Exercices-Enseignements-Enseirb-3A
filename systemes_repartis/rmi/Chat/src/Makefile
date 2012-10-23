SRC=$(wildcard *.java)

all: $(SRC)
	-rm *.class
	javac ClientChat.java
	javac ServerChat.java
	rmic ServantChat
	rmic ChatBackImpl


.PHONY: clean mrproper

clean:
	@-rm  *.class *# *~ 

mrproper:
	class
