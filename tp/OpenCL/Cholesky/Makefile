CC=gcc
FLAG=-Wall -I /net/autre/LABRI/shenry/amd_opencl/include/  -I $(CURDIR) -L /net/autre/LABRI/shenry/amd_opencl/lib/x86
LIB=-lamdocl32

EXECS=cholesky cholesky_monokernel test




all: $(EXECS)
	echo "Don't forget to use \"source set_env.sh\""


cholesky: cholesky.o 
	$(CC) $(FLAG) $(LIB) $^ -o $@  

cholesky_monokernel: cholesky_monokernel.o
	$(CC) $(FLAG) $(LIB) $^ -o $@  

test: test.o
	$(CC) $(FLAG) $(LIB) $^ -o $@  


%.o: %.c
	$(CC) $(FLAG) $(LIB) -c $< -o $@ 

%.c: %.h

.PHONY: clean mrproper

mrproper: clean
	@-rm $(EXECS)

clean:
	@-rm *.o
