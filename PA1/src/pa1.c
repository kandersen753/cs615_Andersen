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

	for (Count = 1; Count<10; Count++)
	{
		if (taskid == MASTER)
		{
			int* vals = (int*)calloc(Count, sizeof(int));
			startTime = MPI_Wtime();
		   	MPI_Send(vals, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		   	MPI_Recv(vals, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		   	endTime = MPI_Wtime();
		   	difference = endTime-startTime;
		   	printf("Time difference of loop %d is %f\n", Count, difference);
		   	free(send);
		}
		else if (taskid == 1)
		{
			MPI_Recv(vals, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Send(vals, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}
	}	
	MPI_Finalize();

}
