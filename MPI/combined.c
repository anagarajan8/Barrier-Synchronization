#include <stdio.h>
#include "mpi.h"
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/time.h>

#define MICROSECOND 1000000

#define NUM_BARRIERS 10
#define NUM_THREADS 4

int num_processes;
int num_threads;
int totalrounds;

int count;

typedef enum {CHAMPION, WINNER, LOSER, WALKOVER} state;

typedef struct process{
	int my_id;
	int opp_id;
	state my_state;
	int sense;
	int thread_signal;
};

struct process my_process;

// SENSE REVERSING BARRIER FOR THREADS


void sense_reversing (int *counter)
{

	// when I tried to directly modify this variable I was getting a deadlock
	// in some runs so defined this local variable

	//for atomic decrement
	#pragma omp critical
	{
		*counter = *counter - 1;
//		printf("Count is %d\n", *counter);
	}

	//for last thread
	if (*counter == 0)
	{
		*counter = num_threads; //Reset count
	//	printf("Process %d count is updated to %d\n", my_process.my_id, *counter);
		my_process.thread_signal = 1;
		arrival();
	//	printf("Barrier Ends\n");
//		printf("\n");
	}

	//if not last thread
	else {
		while (my_process.sense == 0); //spin on sense flag
	}

}




// TOURNAMENT BARRIER FOR PROCESSES

struct process init_process_states(int my_id){

	my_process.my_id = my_id;
	my_process.sense = 0;
	my_process.thread_signal = 0;
	if (my_id == 0){
		my_process.my_state = CHAMPION;
		my_process.opp_id = my_id + 1;
	}
	else if(my_id == num_processes - 1){
		if(my_id % 2 == 0){
			my_process.my_state = WALKOVER;
			my_process.opp_id = -1;
		}
		else if(my_id % 2 == 1){
			my_process.my_state = LOSER;
			my_process.opp_id = my_id - 1;
		}
	}
	else if(my_id % 2 == 0){
		my_process.my_state = WINNER;
		my_process.opp_id = my_id + 1;
	}
	else if(my_id % 2 == 1){
		my_process.my_state = LOSER;
		my_process.opp_id = my_id - 1;
	}

	return my_process;
}

void arrival() {

	int receiver_id, sender_id;
	int tag = 0;
	int my_id;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Request recvd, send;
	MPI_Status status;
	int flag = 0;
	int buff = 1, buff2 = 1;

	int round = 0;

	while (round < totalrounds) {

		int temp_opp_id = my_process.opp_id;
		int temp_my_id  =my_process.my_id;
		if (my_process.my_state == CHAMPION || my_process.my_state == WINNER) {
//		    Receiving message
			MPI_Recv(&buff2, 1, MPI_INT, my_process.opp_id, tag, MPI_COMM_WORLD, &recvd);
		}
		else if (my_process.my_state == LOSER) {
//		    Sending message
			while(my_process.thread_signal == 0);
			MPI_Isend(&buff, 1, MPI_INT, my_process.opp_id, tag, MPI_COMM_WORLD, &send);
			//my_process.sense = 1;
			wakeup();
			return;
		}
//		if WALKOVER no actions required

//		WAKEUP
		if (round == totalrounds - 1) {  //this is only for the champion
			while(my_process.thread_signal == 0);
			wakeup();
			my_process.sense = 1;
//			printf("Flag is reversed\n");
			  //flip sense flag
			return;
		}



		round++;

		if (my_process.my_id % (int)pow(2, round + 1) == pow(2, round)) {
			my_process.my_state = LOSER;
			my_process.opp_id = my_process.my_id - pow(2, round);
		}
		else if (my_process.my_id % (int)pow(2, round + 1) == 0 && my_process.my_id + (int)pow(2, round) >= num_processes  && my_process.my_state != CHAMPION) {
			my_process.my_state = WALKOVER;
		}
		else if (my_process.my_id % (int)pow(2, round + 1) == 0 &&
		         my_process.my_id + (int)pow(2, round) < num_processes) {
			if (my_process.my_state != CHAMPION) {
				my_process.my_state = WINNER;
			}
			my_process.opp_id = my_process.my_id + pow(2, round);
		}
	}
	return;
}


void wakeup() {

	int tag = 0;
	int my_id;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Request recvd, send;
	MPI_Status status;
	int flag = 0;
	int buff = totalrounds;
	int buff2;

	int round = totalrounds;

	if (my_process.my_state == LOSER) {
		MPI_Recv(&buff2, 1, MPI_INT, my_process.opp_id, tag, MPI_COMM_WORLD, &recvd);
		round = buff2;
		if (my_process.sense == 0) {
			my_process.sense = 1;
		}
		//my_process.sense = 0;
		if (my_process.my_id % (int)pow(2, round + 1) == pow(2, round) &&
		    my_process.my_id + (int)pow(2, round) > num_processes ) {

			if (my_process.my_id + (int)pow(2, round) -(int)pow(2, round -1 ) < num_processes)
			{
				my_process.my_state = WINNER;
				my_process.opp_id = my_process.my_id + pow(2, round - 1);
			}
			else {
				my_process.my_state = WALKOVER;
				my_process.opp_id = -1;
			}
		} else {
			my_process.my_state = WINNER;
			my_process.opp_id = my_process.my_id + pow(2, round - 1);
		}

	}

	while (round > 0){
		if(round == 0){
			break;
		}
		buff = 0;
		round--;
		if (my_process.my_state == CHAMPION) {
			buff = round;
			MPI_Isend(&buff, 1, MPI_INT, my_process.opp_id, tag, MPI_COMM_WORLD, &send);
			if (round > 0) {
				my_process.opp_id = my_process.my_id + pow(2, round - 1);
			}
		}

		else if (my_process.my_state == WINNER) {
			buff = round;
			MPI_Isend(&buff, 1, MPI_INT, my_process.opp_id, tag, MPI_COMM_WORLD, &send);
		//	if (my_process.sense == 1) {
	//			my_process.sense = 0;
	//		}
			if (round > 0) {
				my_process.opp_id = my_process.my_id + pow(2, round - 1);
			}
		}

		else if (my_process.my_state == WALKOVER && round > 0){
			if (my_process.my_id + (int)pow(2, round - 1) >= num_processes ) {
				my_process.my_state = WALKOVER;
			} else {
				my_process.my_state = WINNER;
				if (round > 0) {
					my_process.opp_id = my_process.my_id + pow(2, round - 1);
				}
			}
	//		if (my_process.sense == 1) {
	//			my_process.sense = 0;
	//		}
		}

	}

	return;
}


void combined_barrier(int *count, int *thread_num, int my_id){
	my_process = init_process_states(my_id);
//	printf("Thread %d of process %d in barrier with count %d\n", *thread_num, my_process.my_id, *count);
	sense_reversing(count);
	return;
}



int main(int argc, char **argv) {


	MPI_Init(&argc, &argv);
	num_threads = atoi(argv[1]);
	count = num_threads;
	int my_id;
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

	totalrounds = ceil(log(num_processes)/log(2));

	//printf("This is the serial section\n");
	omp_set_num_threads(num_threads);

	for (int k = 0; k < NUM_BARRIERS; ++k) {
		#pragma omp parallel
		{
			num_threads = omp_get_num_threads();
			struct timeval time_before, time_after;
//			printf("Num threads %d\n", num_threads);
			int thread_num = omp_get_thread_num();
			for (int j = 0; j < 9; ++j);

//			printf("Thread %d of process %d Started\n", thread_num, my_id);
			gettimeofday(&time_before, NULL);
			combined_barrier(&count, &thread_num, my_id);
			gettimeofday(&time_after, NULL);
			long deltat = (time_after.tv_sec * MICROSECOND + time_after.tv_usec) -  (time_before.tv_sec * MICROSECOND + time_before.tv_usec);
			printf("%d, %d, %d, %lu\n", k, my_id, thread_num, deltat);
//			printf("Thread %d of process %d Crossed Barrier\n", thread_num, my_id);
		}

		//printf("Back in the serial section again\n");
	}

	MPI_Finalize();

	return 0;
}
