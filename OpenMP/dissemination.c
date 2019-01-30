#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define MICROSECOND 1000000
#define NUM_THREADS 4
#define NUM_BARRIERS 100

#define ROUNDS_I_GUESS NUM_THREADS

//int totalrounds = ceil(log(NUM_THREADS)/log(2));
//int messagearray[ROUNDS_I_GUESS][NUM_THREADS];
int totalrounds;
int num_threads;
int ** messagearray;
//Array to store messages for all threads  in each rounds.

void dissemination(int **messagearray)
{
	int send_id;
	int thread_num = omp_get_thread_num();

	for (int i =0; i < totalrounds; i++)
	{
//		formula for circular message passing from videos -> the %NUM_THREADS is for circular passing
		send_id = (thread_num + (int)pow(2,i)) % num_threads;
//		this is for send
		messagearray[i][send_id] = 1;    //Sending message to a peer
//		this is for receive
//		waiting to receive. while loop will exit as soon as it gets a 1 from its expecting peer
		while (messagearray[i][thread_num] == 0);

//		reset this back to 0 for the next round
		messagearray[i][thread_num] = 0;
	}
}

int main(int argc, char **argv) {

	char *filename = "dissemination.csv";
	FILE *fp;
	fp = fopen(filename, "a+");

	num_threads = atoi(argv[1]);
	totalrounds = ceil(log(num_threads) / log(2));

	messagearray = (int **) malloc(totalrounds * sizeof(int *));
	for (int i=0; i < totalrounds; i++)
		messagearray[i] = (int *)malloc(num_threads * sizeof(int));

	//	we need to initialize this array to zero initially
	for (int i = 0; i < totalrounds; i++) {
		for (int j = 0; j < num_threads; j++) {
			messagearray[i][j] = 0;
		}
	}


	// Serial code
	//printf("This is the serial section\n");
	omp_set_num_threads(num_threads);

#pragma omp parallel
	{
		// Now we're in the parallel section
		int num_threads = omp_get_num_threads();
		int thread_num = omp_get_thread_num();
		struct timeval time_before, time_after;

		for (int i = 0; i < NUM_BARRIERS; i++) {
			for (int j = 0; j < 9999999; ++j);
			//printf("Thread %d of %d Started\n", thread_num, num_threads);
			gettimeofday(&time_before, NULL);
			dissemination(messagearray);
			gettimeofday(&time_after, NULL);
			long long deltat = (time_after.tv_sec - time_before.tv_sec) * MICROSECOND + (time_after.tv_usec - time_before.tv_usec);
			fprintf(fp, "%d, %d, %llu\n", i, thread_num, deltat);

			//printf("Barrier wait time for thread %d is %lu\n", thread_num, deltat);
		}
		//printf("Hello World from thread %d of %d.\n", thread_num, num_threads);
	}

	// Resume serial code
	//printf("Back in the serial section again\n");
	for (int i = 0; i < totalrounds; i++){
		int *currentIntPtr = messagearray[i];
		free(currentIntPtr);
	}
//	free(messagearray);
	fprintf(fp, "\n");
	fclose(fp);

	return 0;
}
