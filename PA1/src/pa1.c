/******************************************************************************
* FILE: pa1.c
* DESCRIPTION:
*   Ping Pong for timing messages
* AUTHOR: Kurt Andersen
* LAST REVISED: 02/19/17
******************************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define  MASTER		0

int main (int argc, char *argv[])
{
	int   numtasks, taskid, len;
	char hostname[MPI_MAX_PROCESSOR_NAME];


	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	MPI_Get_processor_name(hostname, &len);
	double difference;
	double startTime;
	double endTime;
	int Count;
	int Iteration;
	for (Count = 1; Count<3000; Count++)
	{
		for (Iteration = 0; Iteration < 100; Iteration++)
		{
			if (taskid == MASTER)
			{
				int* send = (int*)calloc(Count, sizeof(int));
				startTime = MPI_Wtime();
			   	MPI_Send(send, Count, MPI_INT, numtasks-1, 0, MPI_COMM_WORLD);
			   	MPI_Recv(send, Count, MPI_INT, numtasks-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			   	endTime = MPI_Wtime();
			   	free(send);
			   	difference = endTime-startTime;
			   	printf("Time difference of loop %d is %f\n", Count, difference);
			}
			else if (taskid == (numtasks-1))
			{
				int* recv = (int*)calloc(Count, sizeof(int));
				MPI_Recv(recv, Count, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Send(recv, Count, MPI_INT, 0, 0, MPI_COMM_WORLD);
				free(recv);
			}
			else 
			{
				break;
			}
		}
	}	
	MPI_Finalize();
	return 0;
}

