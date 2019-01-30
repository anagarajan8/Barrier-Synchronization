#include <stdio.h>
#include "mpi.h"
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>

#define MICROSECOND 1000000
#define NUM_BARRIERS 100

int num_processes;
int totalrounds;

void dissemination() //Count for testing only. no need of args since we're not storing count or anything
{  
	int receiver_id, sender_id;
	int tag = 0;
	int my_id;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Request recvd, send;
	MPI_Status status;
	int flag = 0;
	int buff, buff2;
	buff = 1;

    for (int i = 0; i < totalrounds; i++)
    {
//	    formula for circular message passing from videos -> the %NUM_THREADS is for circular passing
	    receiver_id = (my_id + (int) pow(2,i)) % num_processes;

//	    Sending message
        MPI_Isend(&buff, 1, MPI_INT, receiver_id, tag, MPI_COMM_WORLD, &send);
	    MPI_Recv(&buff2, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
		
	//    printf("received at %d from %d\n", my_id, status.MPI_SOURCE);
    }
//	printf("Barrier Ends for process %d\n", my_id);
}




int main(int argc, char **argv)
{
	int my_id;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	struct timeval time_before, time_after;

	totalrounds = ceil(log(num_processes)/log(2));

//Testing the barrier

    for (int i = 0; i<NUM_BARRIERS; i++)
    {
	    for (int j = 0; j < 999; ++j);
	    gettimeofday(&time_before, NULL);
        dissemination();
//		printf("Process %d Passed Barrier\n\n", my_id);
	    gettimeofday(&time_after, NULL);
	    long deltat = (time_after.tv_sec * MICROSECOND + time_after.tv_usec) -  (time_before.tv_sec * MICROSECOND + time_before.tv_usec);
	    printf("%d, %d, %lu\n", i, my_id, deltat);
//	    printf("Barrier wait time for process %d is %lu\n", my_id, deltat);
    }

//	printf("Hello World from processes %d of %d\n", my_id, num_processes);

	MPI_Finalize();
	return 0;
}



