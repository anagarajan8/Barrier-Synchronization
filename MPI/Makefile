OMPFLAGS = -fopenmp
OMPLIBS = -lgomp

#CC = gcc
CC = mpicc
CPPFLAGS = -g -Wall -std=c99
LDFLAGS = -g -Wall $(OMPFLAGS) -std=c99
LDLIBS = $(OMPLIBS) -lm

MPICC = mpicc
MPICH = /usr/lib64/openmpi/1.4-gcc
CFLAGS = -I$(MPICH)/include

all: dissemination_MPI tournament combined
# count_barrier


dissemination_MPI: dissemination_MPI.o
	$(MPICC) -o $@ $(LDFLAGS) $^ $(LDLIBS) $(OMPLIBS)

dissemination_MPI.o: dissemination_MPI.c
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $(OMPFLAGS) $^

tournament: tournament.o
	$(MPICC) -o $@ $(LDFLAGS) $^ $(LDLIBS) $(OMPLIBS)

tournament.o: tournament.c
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $(OMPFLAGS) $^

combined: combined.c
	$(MPICC) -o $@ $(LDFLAGS) $(OMPFLAGS) $^ $(LDLIBS) $(OMPLIBS)

combined.o: combined.c
	$(CC) -c -o $@ $(CPPFLAGS) $(OMPFLAGS) $(CFLAGS) $(OMPLIBS) $^



clean:
	rm -f *.o dissemination_MPI tournament combined
