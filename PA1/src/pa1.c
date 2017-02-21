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


	
	//gets numtasks and taskid
	MPI_Init(&argc, &argv);

	//assigns numtasks and taskid to the world
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

	//gets the hostname
	MPI_Get_processor_name(hostname, &len);
	
	//variables for timing
	double difference;
	double startTime;
	double endTime;

	//loop counters
	int Count;
	int Iteration;

	//number of integers being sent
	for (Count = 1; Count<3000; Count++)
	{
		average = 0;

		//gets start time
		startTime = MPI_Wtime();
		
		//test number for the number of integers
		for (Iteration = 0; Iteration < 100; Iteration++)
		{
			if (taskid == MASTER)
			{
				//allocates an array of size Count
				int* send = (int*)calloc(Count, sizeof(int));

				//sends array size count to the last available node for my total size
			   	MPI_Send(send, Count, MPI_INT, numtasks-1, 0, MPI_COMM_WORLD);

			   	//receives a message back from the last node
			   	MPI_Recv(send, Count, MPI_INT, numtasks-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			   	
			   	//dealloates array
			   	free(send);
			}
			else if (taskid == (numtasks-1))
			{
				//allocates memory for an array of the same size being received
				int* recv = (int*)calloc(Count, sizeof(int));

				//reveives the array then sends it back to the master node
				MPI_Recv(recv, Count, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Send(recv, Count, MPI_INT, 0, 0, MPI_COMM_WORLD);
				free(recv);
			}
			//unused nodes wont have a process running
			else 
			{
				break;
			}
		}
		
		//end time
		endTime = MPI_Wtime();

	   	printf("%d , %f\n", Count, (endTime-startTime)/100);
	}	
	MPI_Finalize();
	return 0;
}

