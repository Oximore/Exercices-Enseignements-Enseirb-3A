CC=mpicc
GF=-W -Wall
TEST=test_code
TEST_SRC=$(TEST).c $(TEST).h


all: poisson_sequentiel poisson_mpi


move:
	./move.sh


poisson_sequentiel: poisson_sequentiel.c $(TEST_SRC)
	$(CC) $(GF) $< $(TEST).c -o $@	

poisson_mpi: poisson_mpi.c $(TEST_SRC)
	$(CC) $(GF) $< $(TEST).c -o $@	




.PHONY: clean 

clean:
	-@rm *~ *.o *#