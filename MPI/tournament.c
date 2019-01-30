#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>

#define MICROSECOND 1000000

#define NUM_BARRIERS 100

int num_processes;
int totalrounds;

typedef enum {CHAMPION, WINNER, LOSER, WALKOVER} state;

typedef struct process{
	int my_id;
	int opp_id;
	state my_state;
	int sense;
};

struct process my_process;

struct process init_process_states(int my_id){

	my_process.my_id = my_id;
	my_process.sense = 0;
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

//	printf("Process id: %d \t Opp_id: %d \t my_state: %d\n", my_process.my_id, my_process.opp_id, my_process.my_state);

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
//	for (int i = 0; i < totalrounds; i++) {
//	for (int i = 0; i < 2; i++) {

	while (round < totalrounds) {

		int temp_opp_id = my_process.opp_id;
		int temp_my_id  =my_process.my_id;
		if (my_process.my_state == CHAMPION || my_process.my_state == WINNER) {
//		    Receiving message
			MPI_Recv(&buff2, 1, MPI_INT, my_process.opp_id, tag, MPI_COMM_WORLD, &recvd);
		}
		else if (my_process.my_state == LOSER) {
//		    Sending message
			MPI_Isend(&buff, 1, MPI_INT, my_process.opp_id, tag, MPI_COMM_WORLD, &send);
			my_process.sense = 1;
//			while (my_process.sense == 1) {
				wakeup();
//			}
			return;
		}
//		if WALKOVER no actions required


//		WAKEUP
		if (round == totalrounds - 1) {
			wakeup();
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
		my_process.sense = 0;
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


		if (my_process.sense == 1) {
			my_process.sense = 0;
		}

	}

	while (round > 0){

		if(round == 0){
			break;
		}
		buff = 0;

		round--;

		if (my_process.my_state == CHAMPION)
		{
			buff = round;
			MPI_Isend(&buff, 1, MPI_INT, my_process.opp_id, tag, MPI_COMM_WORLD, &send);
			if (round > 0) {
				my_process.opp_id = my_process.my_id + pow(2, round - 1);
			}
		}

		else if (my_process.my_state == WINNER) {
			buff = round;
			MPI_Isend(&buff, 1, MPI_INT, my_process.opp_id, tag, MPI_COMM_WORLD, &send);
			if (my_process.sense == 1) {
				my_process.sense = 0;
			}
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
			if (my_process.sense == 1) {
				my_process.sense = 0;
			}
		}

	}

	return;
}


void tournament_barrier(){
	arrival();
	return;
}


int main(int argc, char **argv)
{
	int my_id;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	struct timeval time_before, time_after;

	totalrounds = ceil(log(num_processes) / log(2));

//	my_process = init_process_states(my_id);

	for (int i = 0; i<NUM_BARRIERS; i++)
	{
		my_process = init_process_states(my_id);
		for (int j = 0; j < 999; ++j);

		gettimeofday(&time_before, NULL);
    	tournament_barrier();
		gettimeofday(&time_after, NULL);
		long deltat = (time_after.tv_sec * MICROSECOND + time_after.tv_usec) -  (time_before.tv_sec * MICROSECOND + time_before.tv_usec);
		printf("%d, %d, %lu\n", i, my_id, deltat);

//		printf("Process %d Passed Barrier\n\n", my_id);
	}

	MPI_Finalize();
	return 0;
}



