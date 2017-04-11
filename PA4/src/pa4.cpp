#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
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

	
	int** matrixA;
	int** matrixB;
	int** solutionMatrix;

	int matrixDimension;
	std::string numval = argv[1];
	matrixDimension = atoi(numval.c_str());

	//timer variables
	double startTime = 0;
	double endTime = 0;

	if (numtasks == 1)
	{
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
			}
		}

		//random numbers 0 to 100 for MatrixB
		for (int i=0; i<matrixDimension; i++)
		{
			for (int j=0; j<matrixDimension; j++)
			{
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
			

		}

		else if (taskid > 0)
		{

		}
	}

	//ends program
	MPI_Finalize();
}
