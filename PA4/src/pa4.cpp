#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <math.h>
#include "mpi.h"


int main(int argc, char* argv[])
{
	srand(0);
	int   numtasks, taskid, len;
	char hostname[MPI_MAX_PROCESSOR_NAME];

	//gets numtasks and taskid
	MPI_Init(&argc, &argv);

	//assigns numtasks and taskid to the world
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

	//gets the hostname
	MPI_Get_processor_name(hostname, &len);

	


	int matrixDimension;
	std::string numval = argv[1];
	matrixDimension = atoi(numval.c_str());

	//timer variables
	double startTime = 0;
	double endTime = 0;

	if (numtasks == 1)
	{
		int** matrixA;
		int** matrixB;
		int** solutionMatrix;

		//allocate memory for main arrays
		matrixA = new int* [matrixDimension];
		matrixB = new int* [matrixDimension];
		solutionMatrix = new int* [matrixDimension];

		//allocate memory for all 3 matrices
		for (int i=0; i<matrixDimension; i++)
		{
			matrixA[i] = new int [matrixDimension];
			matrixB[i] = new int [matrixDimension];
			solutionMatrix[i] = new int [matrixDimension];
		}
		

		//random numbers 0 to 100 for matrixA
		for (int i=0; i<matrixDimension; i++)
		{
			for (int j=0; j<matrixDimension; j++)
			{
				matrixA[i][j] = (random()%100) + 1;
				matrixB[i][j] = (random()%100) + 1;
			}
		}


		startTime = MPI_Wtime();

		//find solution for multiplying both matrices
		for (int i=0; i<matrixDimension; i++)
		{
			for (int j=0; j<matrixDimension; j++)
			{
				solutionMatrix[i][j] = 0;
				for (int k=0; k<matrixDimension; k++)
				{
					solutionMatrix[i][j] += matrixA[i][k] * matrixB[k][j];
				}
			}
		}

		endTime = MPI_Wtime();

		std::cout << "Matrix Dimensions = " << matrixDimension << std::endl;
		std::cout << "RunTime = " << endTime-startTime << std::endl;
		

		
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//parallel code
	else
	{
		if (taskid == 0)
		{
			int* matrixA;
			int* matrixB;
			int subMatrixDimension = matrixDimension/(sqrt(numtasks));
			int numSubMatrixPerRow = sqrt(numtasks);
			int* dataSenderA;
			int* dataSenderB;
			int* subMatrixA;
			int* subMatrixB;
			int* subMatrixC;
			int dataSenderOffset = 0;
			int currentRowSection = 0;
			int currentColSection = 0;
			MPI_Status status;
			int myColPosition = taskid%numSubMatrixPerRow;
			int myRowPosition = taskid/numSubMatrixPerRow;
			int matrixADestination;
			int matrixBDestination;
			int matrixAReceiver;
			int matrixBreceiver;

			//allocate memory for main arrays
			matrixA = new int [matrixDimension*matrixDimension];
			matrixB = new int [matrixDimension*matrixDimension];
			dataSenderA = new int [subMatrixDimension*subMatrixDimension];
			dataSenderB = new int [subMatrixDimension*subMatrixDimension];
			subMatrixA = new int[subMatrixDimension*subMatrixDimension];
			subMatrixB = new int [subMatrixDimension*subMatrixDimension];
			subMatrixC = new int [subMatrixDimension*subMatrixDimension];

			//random numbers 0 to 100 for matrixA
			for (int i=0; i<matrixDimension*matrixDimension; i++)
			{
				matrixA[i] = (random()%100) + 1;
				matrixB[i] = (random()%100) + 1;
			}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			//reference to navigate array appropriately
			std::cout << "Matrix A" << std::endl;
			//test print A
			for (int i=0; i<matrixDimension; i++)
			{
				for (int j=0; j<matrixDimension; j++)
				{
					std::cout << matrixA[j+i*matrixDimension] << " ";
				}
				std::cout << std::endl;
			}

			std::cout << std::endl << "Matrix B" << std::endl;
			//test print B
			for (int i=0; i<matrixDimension; i++)
			{
				for (int j=0; j<matrixDimension; j++)
				{
					std::cout << matrixB[j+i*matrixDimension] << " ";
				}
				std::cout << std::endl;
			}

			std::cout << std::endl;
			*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			//initialize all data in the solution sub matrix to 0
			for (int i=0; i<subMatrixDimension*subMatrixDimension; i++)
			{
				subMatrixC[i] = 0;
			}
			

			startTime = MPI_Wtime();
			//loops to distribute to each sub matrix
			//current col section represents which column of sub matrices we are looking at, while current row section
			//is the row of sub matrices we are looking at, so 1,1 would be one row down, and one column in
			for (int currentDistribution=0; currentDistribution<numtasks; currentDistribution++, currentColSection++)
			{
				dataSenderOffset = 0;

				//determines which row section we are working with
				if(currentDistribution%numSubMatrixPerRow == 0 && currentDistribution > 0)
				{
					currentRowSection++;
					currentColSection = 0;
				}

				//loops to move data for the current submatrix to a new 1D array to send off to sub processes
				for (int currentRow=0; currentRow<subMatrixDimension; currentRow++)
				{
					for (int currentCol=0; currentCol<subMatrixDimension; currentCol++, dataSenderOffset++)
					{
						dataSenderA[dataSenderOffset] = matrixA[currentCol+(currentRow*matrixDimension)+(currentRowSection*matrixDimension*subMatrixDimension)+currentColSection*subMatrixDimension];
						dataSenderB[dataSenderOffset] = matrixB[currentCol+(currentRow*matrixDimension)+(currentRowSection*matrixDimension*subMatrixDimension)+currentColSection*subMatrixDimension];
					}
				}

				//send data to individual processes
				//keep data for master process
				if (currentDistribution == 0)
				{
					//copies data from stuff supposed to be sent to the masters data
					for (int i=0; i<subMatrixDimension*subMatrixDimension; i++)
					{
						subMatrixA[i] = dataSenderA[i];
						subMatrixB[i] = dataSenderB[i];
					}
				}
				//send data to other processes
				else
				{
					//sends sub matrix A values, then sends sub matrix B values to the specific 
					MPI_Send(dataSenderA, subMatrixDimension*subMatrixDimension, MPI_INT, currentDistribution, 1, MPI_COMM_WORLD);
					MPI_Send(dataSenderB, subMatrixDimension*subMatrixDimension, MPI_INT, currentDistribution, 2, MPI_COMM_WORLD);
				}

			}

			//waits for all data to be sent to each process, and all processes have received their data
			MPI_Barrier(MPI_COMM_WORLD);


			//pass data for initial setup for cannons algorithm
			//send then receive A matrix first
			if (taskid - (myRowPosition*numSubMatrixPerRow) - myRowPosition < 0)
			{
				//checks to see if piece of data needs to reset to the right side of matrix
				matrixADestination = taskid - myRowPosition + numSubMatrixPerRow;
			}
			else
			{
				//no reset is needed, stays in same row
				matrixADestination = taskid - myRowPosition;
			}

			//find where im receiving from
			if (taskid + myRowPosition >= myRowPosition*numSubMatrixPerRow + numSubMatrixPerRow)
			{
				matrixAReceiver = taskid + myRowPosition - numSubMatrixPerRow;
			}
			else
			{
				matrixAReceiver = taskid + myRowPosition;
			}

			//sends data for the A submatrix, then receives new data for the new submatrix
			if (taskid != matrixADestination)
			{			
				MPI_Sendrecv_replace(subMatrixA, subMatrixDimension*subMatrixDimension, MPI_INT, matrixADestination, 1, matrixAReceiver, 1, MPI_COMM_WORLD, &status);
			}			

			//barrier is used here since anysource is used.  This restricts it so only 1 message is getting sent to this processor
			MPI_Barrier(MPI_COMM_WORLD);

			//send and reverive B matrix
			if (taskid - (myColPosition*numSubMatrixPerRow) <= 0 && myColPosition != 0)
			{
				//if a wrap around is needed for the columns, resets it back up to the proper position
				matrixBDestination = taskid - (myColPosition*numSubMatrixPerRow) + (numSubMatrixPerRow*numSubMatrixPerRow);
				
			}
			else if (taskid - (myColPosition*numSubMatrixPerRow) < 0 && myColPosition == 0)
			{
				//if a wrap around is needed for the columns, resets it back up to the proper position
				matrixBDestination = taskid - (myColPosition*numSubMatrixPerRow) + (numSubMatrixPerRow*numSubMatrixPerRow);
				
			}
			else
			{
				//no reset is needed
				matrixBDestination = taskid - (myColPosition*numSubMatrixPerRow);
			}

			//find where B is being received from
			if (taskid + (myColPosition*numSubMatrixPerRow) >= numtasks)
			{
				matrixBreceiver = taskid + (myColPosition*numSubMatrixPerRow) - (numSubMatrixPerRow*numSubMatrixPerRow);
			}
			else
			{
				matrixBreceiver = taskid + (myColPosition*numSubMatrixPerRow);
			}

			//sends data for the B submatrix, then receives new data for the new submatrix
			if (taskid != matrixBDestination)
			{
				MPI_Sendrecv_replace(subMatrixB, subMatrixDimension*subMatrixDimension, MPI_INT, matrixBDestination, 2, matrixBreceiver, 2, MPI_COMM_WORLD, &status);
			}			

			//barrier is used here since anysource is used.  This restricts it so only 1 message is getting sent to this processor
			MPI_Barrier(MPI_COMM_WORLD);

			//loop to do all calculations and add current sub matrix solution to the solution sub matrix C
			//loops enough for the amount of needed shifts
			for (int i = 0; i<numSubMatrixPerRow; i++)
			{
				//triple nest to do matrix multiplication for the current sub matrix set of a and b
				for (int j=0; j<subMatrixDimension; j++)
				{
					for (int k=0; k<subMatrixDimension; k++)
					{
						for (int l=0; l<subMatrixDimension; l++)
						{
							subMatrixC[k+j*subMatrixDimension] += subMatrixA[l+j*subMatrixDimension] * subMatrixB[k+l*subMatrixDimension];
						}
					}
				}

				//current sub matrix calculated now calculate where each submatrix will get sent next
				//send then receive A matrix first
				//1 is used because each row shift is only 1 displacement in the processor mesh
				if (taskid - (myRowPosition*numSubMatrixPerRow) - 1 < 0)
				{
					//checks to see if piece of data needs to reset to the right side of matrix
					matrixADestination = taskid - 1 + numSubMatrixPerRow;
				}
				else
				{
					//no reset is needed, stays in same row
					matrixADestination = taskid - 1;
				}

				//find where im receiving from
				if (taskid + 1 >= myRowPosition*numSubMatrixPerRow + numSubMatrixPerRow)
				{
					matrixAReceiver = taskid + 1 - numSubMatrixPerRow;
				}
				else
				{
					matrixAReceiver = taskid + 1;
				}

				//sends data for the A submatrix, then receives new data for the new submatrix
				MPI_Sendrecv_replace(subMatrixA, subMatrixDimension*subMatrixDimension, MPI_INT, matrixADestination, 1, matrixAReceiver, 1, MPI_COMM_WORLD, &status);
				
				//barrier is used here since anysource is used.  This restricts it so only 1 message is getting sent to this processor
				MPI_Barrier(MPI_COMM_WORLD);

				//send and reverive B matrix
				if (taskid - numSubMatrixPerRow <= 0 && myColPosition != 0)
				{
					//if a wrap around is needed for the columns, resets it back up to the proper position
					matrixBDestination = taskid - numSubMatrixPerRow + (numSubMatrixPerRow*numSubMatrixPerRow);
				}
				else if(taskid - numSubMatrixPerRow < 0 && myColPosition == 0)
				{
					//if a wrap around is needed for the columns, resets it back up to the proper position
					matrixBDestination = taskid - numSubMatrixPerRow + (numSubMatrixPerRow*numSubMatrixPerRow);
				}
				else
				{
					//no reset is needed
					matrixBDestination = taskid - numSubMatrixPerRow;
				}

				//find where B is being received from
				if (taskid + numSubMatrixPerRow >= numtasks)
				{
					matrixBreceiver = taskid + numSubMatrixPerRow - (numSubMatrixPerRow*numSubMatrixPerRow);
				}
				else
				{
					matrixBreceiver = taskid + numSubMatrixPerRow;
				}

				//sends data for the B submatrix, then receives new data for the new submatrix
				MPI_Sendrecv_replace(subMatrixB, subMatrixDimension*subMatrixDimension, MPI_INT, matrixBDestination, 2, matrixBreceiver, 2, MPI_COMM_WORLD, &status);

				//barrier is used here since anysource is used.  This restricts it so only 1 message is getting sent to this processor
				MPI_Barrier(MPI_COMM_WORLD);

				//reset counters for when data comes back
				currentRowSection = 0;
				currentColSection = 0;
			}


			for (int i=0; i<numtasks; i++)
			{
				//std::cout << "sub matrix solution # " << i << std::endl;
				if (i!=taskid)
				{
					MPI_Recv(subMatrixC, subMatrixDimension*subMatrixDimension, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
				}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				/*
				for (int j=0; j<subMatrixDimension*subMatrixDimension; j++)
				{
					if (j%subMatrixDimension == 0 && j != 0)
					{
						std::cout << std::endl;
					}
					std::cout << subMatrixC[j] << " ";
				}
				std::cout << std::endl;
				*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}

			endTime = MPI_Wtime() - startTime;
			std::cout << std::endl << "Total Run Time: " << endTime << std::endl;
			std::cout << "Matrix Dimensions: " << matrixDimension << std::endl;
			std::cout << "Number of Processors: " << numtasks << std::endl;
		}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//subprocessors start here
		else if (taskid > 0)
		{
			//variable declaration for all calculations
			int* subMatrixA;
			int* subMatrixB;
			int* subMatrixC;
			int subMatrixDimension = matrixDimension/(sqrt(numtasks));
			int numSubMatrixPerRow = sqrt(numtasks);
			MPI_Status status;
			int myColPosition = taskid%numSubMatrixPerRow;
			int myRowPosition = taskid/numSubMatrixPerRow;
			int matrixADestination = 0;
			int matrixBDestination = 0;
			int matrixAReceiver;
			int matrixBreceiver;

			//allocate enough space in memory for each sub matrix
			subMatrixA = new int [subMatrixDimension*subMatrixDimension];
			subMatrixB = new int [subMatrixDimension*subMatrixDimension];
			subMatrixC = new int [subMatrixDimension*subMatrixDimension];

			//initializes solution sub matrix data to all 0;
			for (int i=0; i<subMatrixDimension*subMatrixDimension; i++)
			{
				subMatrixC[i] = 0;
			}

			//receives data from the master for the initial sub matrix data
			MPI_Recv(subMatrixA, subMatrixDimension*subMatrixDimension, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
			MPI_Recv(subMatrixB, subMatrixDimension*subMatrixDimension, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);

			//waits until all subprocesses have received their data
			MPI_Barrier(MPI_COMM_WORLD);

			//pass data for initial setup for cannons algorithm
			//send then receive A matrix first
			if (taskid - (myRowPosition*numSubMatrixPerRow) - myRowPosition < 0)
			{
				//checks to see if piece of data needs to reset to the right side of matrix
				matrixADestination = taskid - myRowPosition + numSubMatrixPerRow;
			}
			else
			{
				//no reset is needed, stays in same row
				matrixADestination = taskid - myRowPosition;
			}

			//find where im receiving from
			if (taskid + myRowPosition >= myRowPosition*numSubMatrixPerRow + numSubMatrixPerRow)
			{
				matrixAReceiver = taskid + myRowPosition - numSubMatrixPerRow;
			}
			else
			{
				matrixAReceiver = taskid + myRowPosition;
			}

			//sends data for the A submatrix, then receives new data for the new submatrix
			if (taskid != matrixADestination)
			{
				MPI_Sendrecv_replace(subMatrixA, subMatrixDimension*subMatrixDimension, MPI_INT, matrixADestination, 1, matrixAReceiver, 1, MPI_COMM_WORLD, &status);
			}

			//Ensure all processes are at the same point, to keep synchonous running for safety
			MPI_Barrier(MPI_COMM_WORLD);

			//send and reverive B matrix
			if (taskid - (myColPosition*numSubMatrixPerRow) <= 0 && myColPosition != 0)
			{
				//if a wrap around is needed for the columns, resets it back up to the proper position
				matrixBDestination = taskid - (myColPosition*numSubMatrixPerRow) + (numSubMatrixPerRow*numSubMatrixPerRow);
				
			}
			else if (taskid - (myColPosition*numSubMatrixPerRow) < 0 && myColPosition == 0)
			{
				//if a wrap around is needed for the columns, resets it back up to the proper position
				matrixBDestination = taskid - (myColPosition*numSubMatrixPerRow) + (numSubMatrixPerRow*numSubMatrixPerRow);
				
			}
			else
			{
				//no reset is needed
				matrixBDestination = taskid - (myColPosition*numSubMatrixPerRow);
			}

			//find where B is being received from
			if (taskid + (myColPosition*numSubMatrixPerRow) >= numtasks)
			{
				matrixBreceiver = taskid + (myColPosition*numSubMatrixPerRow) - (numSubMatrixPerRow*numSubMatrixPerRow);
			}
			else
			{
				matrixBreceiver = taskid + (myColPosition*numSubMatrixPerRow);
			}

			//sends data for the B submatrix, then receives new data for the new submatrix
			if (taskid != matrixBDestination)
			{
				MPI_Sendrecv_replace(subMatrixB, subMatrixDimension*subMatrixDimension, MPI_INT, matrixBDestination, 2, matrixBreceiver, 2, MPI_COMM_WORLD, &status);
			}

			//barrier is used here since anysource is used.  This restricts it so only 1 message is getting sent to this processor
			MPI_Barrier(MPI_COMM_WORLD);

			//loop to do all calculations and add current sub matrix solution to the solution sub matrix C
			//loops enough for the amount of needed shifts
			for (int i = 0; i<numSubMatrixPerRow; i++)
			{
				//triple nest to do matrix multiplication for the current sub matrix set of a and b
				for (int j=0; j<subMatrixDimension; j++)
				{
					for (int k=0; k<subMatrixDimension; k++)
					{
						for (int l=0; l<subMatrixDimension; l++)
						{
							subMatrixC[k+j*subMatrixDimension] += subMatrixA[l+j*subMatrixDimension] * subMatrixB[k+l*subMatrixDimension];
						}
					}
				}

				//current sub matrix calculated now calculate where each submatrix will get sent next
				//send then receive A matrix first
				//1 is used because each row shift is only 1 displacement in the processor mesh
				if (taskid - (myRowPosition*numSubMatrixPerRow) - 1 < 0)
				{
					//checks to see if piece of data needs to reset to the right side of matrix
					matrixADestination = taskid - 1 + numSubMatrixPerRow;
				}
				else
				{
					//no reset is needed, stays in same row
					matrixADestination = taskid - 1;
				}

				//find where im receiving from
				if (taskid + 1 >= myRowPosition*numSubMatrixPerRow + numSubMatrixPerRow)
				{
					matrixAReceiver = taskid + 1 - numSubMatrixPerRow;
				}
				else
				{
					matrixAReceiver = taskid + 1;
				}

				//sends data for the A submatrix, then receives new data for the new submatrix
				MPI_Sendrecv_replace(subMatrixA, subMatrixDimension*subMatrixDimension, MPI_INT, matrixADestination, 1, matrixAReceiver, 1, MPI_COMM_WORLD, &status);
				
				//barrier is used here since anysource is used.  This restricts it so only 1 message is getting sent to this processor
				MPI_Barrier(MPI_COMM_WORLD);

				//send and reverive B matrix
				if (taskid - numSubMatrixPerRow <= 0 && myColPosition != 0)
				{
					//if a wrap around is needed for the columns, resets it back up to the proper position
					matrixBDestination = taskid - numSubMatrixPerRow + (numSubMatrixPerRow*numSubMatrixPerRow);
				}
				else if(taskid - numSubMatrixPerRow < 0 && myColPosition == 0)
				{
					//if a wrap around is needed for the columns, resets it back up to the proper position
					matrixBDestination = taskid - numSubMatrixPerRow + (numSubMatrixPerRow*numSubMatrixPerRow);
				}
				else
				{
					//no reset is needed
					matrixBDestination = taskid - numSubMatrixPerRow;
				}

				//find where B is being received from
				if (taskid + numSubMatrixPerRow >= numtasks)
				{
					matrixBreceiver = taskid + numSubMatrixPerRow - (numSubMatrixPerRow*numSubMatrixPerRow);
				}
				else
				{
					matrixBreceiver = taskid + numSubMatrixPerRow;
				}

				//sends data for the B submatrix, then receives new data for the new submatrix
				MPI_Sendrecv_replace(subMatrixB, subMatrixDimension*subMatrixDimension, MPI_INT, matrixBDestination, 2, matrixBreceiver, 2, MPI_COMM_WORLD, &status);

				//barrier is used here since anysource is used.  This restricts it so only 1 message is getting sent to this processor
				MPI_Barrier(MPI_COMM_WORLD);

			}

			//sends solution to the master
			MPI_Send(subMatrixC, subMatrixDimension*subMatrixDimension, MPI_INT, 0, 0, MPI_COMM_WORLD);

		}
	}
	//ends program
	MPI_Finalize();
}
