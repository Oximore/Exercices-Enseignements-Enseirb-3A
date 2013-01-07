CC=gcc
FLAG=-Wall -I /net/autre/LABRI/shenry/amd_opencl/include/  -I $(CURDIR) -L /net/autre/LABRI/shenry/amd_opencl/lib/x86
LIB=-lamdocl32

EXECS=cholesky cholesky_monokernel 
BINDIR=$(CURDIR)/../bin/



all: $(EXECS)
	echo "Don't forget to use \"source set_env.sh\""


cholesky: cholesky.o util.o 
	$(CC) $(FLAG) $(LIB) $^ -o $(BINDIR)$@  

cholesky_monokernel: cholesky_monokernel.o util.o
	$(CC) $(FLAG) $(LIB) $^ -o $(BINDIR)$@  


%.o: %.c
	$(CC) $(FLAG) $(LIB) -c $< -o $@ 

%.c: %.h

.PHONY: clean mrproper

mrproper: clean
	@-rm $(BINDIR)*

clean:
	@-rm *.o
