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
			int* solutionMatrix;
			int rowCounter=0;
			int subMatrixDimension = matrixDimension/(sqrt(numtasks));
			int numSubMatrixPerRow = sqrt(numtasks);
			int* dataSenderA;
			int* dataSenderB;
			int* masterData;
			int dataSenderOffset = 0;
			int currentRowSection = 0;
			int currentColSection = 0;

			std::cout << "sub Dimension = " << subMatrixDimension << std::endl;

			//allocate memory for main arrays
			matrixA = new int [matrixDimension*matrixDimension];
			matrixB = new int [matrixDimension*matrixDimension];
			solutionMatrix = new int [matrixDimension*matrixDimension];
			dataSenderA = new int [subMatrixDimension*subMatrixDimension];
			dataSenderB = new int [subMatrixDimension*subMatrixDimension];
			masterData = new int[subMatrixDimension*subMatrixDimension];

			//random numbers 0 to 100 for matrixA
			for (int i=0; i<matrixDimension*matrixDimension; i++)
			{
				matrixA[i] = (random()%100) + 1;
				matrixB[i] = (random()%100) + 1;
			}

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
				std::cout << currentRowSection << "  " << currentColSection << std::endl;
				for (int currentRow=0; currentRow<subMatrixDimension; currentRow++)
				{
					for (int currentCol=0; currentCol<subMatrixDimension; currentCol++, dataSenderOffset++)
					{
						dataSenderA[dataSenderOffset] = matrixA[currentCol+(currentRow*matrixDimension)+(currentRowSection*matrixDimension*subMatrixDimension)+currentColSection*subMatrixDimension];
						dataSenderB[dataSenderOffset] = matrixB[currentCol+(currentRow*matrixDimension)+(currentRowSection*matrixDimension*subMatrixDimension)+currentColSection*subMatrixDimension];
					}
				}

				//test print sub matrices
				std::cout << "subMatrixA# " << currentDistribution << std::endl;
				for (int i=0; i<subMatrixDimension; i++)
				{
					
					for (int j=0; j<subMatrixDimension; j++)
					{
						std::cout << dataSenderA[j+i*subMatrixDimension] << " ";
					}
					std::cout << std::endl;
				}
				std::cout << std::endl;
				std::cout << "subMatrixB# " << currentDistribution << std::endl;
				for (int i=0; i<subMatrixDimension; i++)
				{
					
					for (int j=0; j<subMatrixDimension; j++)
					{
						std::cout << dataSenderB[j+i*subMatrixDimension] << " ";
					}
					std::cout << std::endl;
				}
				std::cout << std::endl;

			}
		}

		else if (taskid > 0)
		{

		}
	}

	//ends program
	MPI_Finalize();
}
