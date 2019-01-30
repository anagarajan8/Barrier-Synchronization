
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#define MICROSECOND 1000000
#define NUM_THREADS 8
#define NUM_BARRIERS 100

int count;

// Barrier function

void sense_reversing (int *counter, int *sense)
{

	int num_threads = omp_get_num_threads();
	int actualsense = *sense;

#pragma omp critical
	{
		*counter = *counter - 1;
		//printf("Count is %d\n", *counter);
	}

	//for last thread
	if (*counter ==0)
	{
		*counter = num_threads; //Reset count
		*sense = !(actualsense);  //flip sense flag
		//printf("Barrier Ends\n");
		//printf("\n");
	}

	//if not last thread
	else {
		while (*sense == actualsense); //spin on sense flag
	}

}


int main(int argc, char **argv)
{
	char *filename = "sense_reversing.csv";
	FILE *fp;
	fp = fopen(filename, "a+");

	count = atoi(argv[1]);

	int sense = 0;

	// Serial code
//	printf("This is the serial section\n");
	omp_set_num_threads(count);

	#pragma omp parallel
	{
		// Now we're in the parallel section
		int num_threads = omp_get_num_threads();
		int thread_num = omp_get_thread_num();
		struct timeval time_before, time_after;
		//Barrier in action

		for (int j = 0; j < NUM_BARRIERS; j++)
		{
//			printf("Thread %d of %d Starts\n", thread_num, num_threads);
			for (int i = 0; i < 99; ++i);

			gettimeofday(&time_before, NULL);
			sense_reversing(&count, &sense);  //Barrier function called
			gettimeofday(&time_after, NULL);
			long long int deltat = (time_after.tv_sec - time_before.tv_sec) * MICROSECOND + (time_after.tv_usec - time_before.tv_usec);
			fprintf(fp, "%d, %d, %lld\n", j, thread_num, deltat);

//			printf("Barrier wait time for thread %d is %lu\n", thread_num, deltat);

		}

//		printf("Hello World from thread %d of %d.\n", thread_num, num_threads);
	} // implied barrier

	// Resume serial code
//	printf("Back in the serial section again\n");
	fprintf(fp, "\n");
	fclose(fp);
	return 0;
}
