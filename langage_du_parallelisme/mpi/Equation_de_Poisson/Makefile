CC=mpicc
GF=-W -Wall
TEST=test_code
TEST_SRC=$(TEST).c $(TEST).h


all: regle


regle:
	mpicc -W -Wall poisson_mpi.c test_code.c -o poisson_mpi	

run:
	mpiexec -np 6 ./poisson_mpi 0.01 12 10 3 2


#poisson_sequentiel poisson_mpi


move:
	./move.sh


poisson_sequentiel: poisson_sequentiel.c $(TEST_SRC)
	$(CC) $(GF) $< $(TEST).c -o $@	

poisson_mpi: poisson.c $(TEST_SRC)
	$(CC) $(GF) $< $(TEST).c -o $@	




.PHONY: clean 

clean:
	rm poisson_mpi 
#	-@rm *~ *.o *#