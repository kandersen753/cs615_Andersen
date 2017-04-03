#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


int main(int argc, char* argv[])
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

	//variables used throughout program
	int numBuckets=10;
	std::list<int> buckets[numBuckets];
	int* values;
	int* sortedValues;
	int numbersInFile = 0;
	std::string filePath;
	int maxVal = 0;
	int currentBucketSize;
	int dataIndex = 0;

	//timer variables
	double startTime;
	double endTime;

	//gets filepath from command line
	filePath = argv[1];

	//input stream variable
	std::ifstream fin;

	//opens path to data
	fin.open(filePath.c_str());

	//reads in first value to tell how much data is in file
	fin >> numbersInFile;

	//allocates memory for the full data array
	values = new int [numbersInFile];
	sortedValues = new int [numbersInFile];

	//reads in all data for values
	for (int i=0; i<numbersInFile; i++)
	{
		fin >> values[i];

		//check for max value in data set
		if (maxVal < values[i])
			{
				maxVal = values[i];
			}
	}

	//closes file stream
	fin.close();
	
	startTime = MPI_Wtime();

	//split data into buckets
	//iterates through each value
	for (int i=0; i<numbersInFile; i++)
	{
		//checks which bucket it can fit into
		for (int j=0; j<numBuckets; j++)
		{
			//each side of the and is the minumum value and the maximum value of the current bucket
			if (values[i] >= maxVal*(((float)j)/numBuckets) && values[i] <= maxVal*(((float)j+1)/numBuckets))
			{
				//if the value is between the min/max of the bucket, then it is pushed into that bucket
				buckets[j].push_back(values[i]);
				break;
			}
		}
	}

	//sort each of the buckets
	for (int i=0; i<numBuckets; i++)
	{
		buckets[i].sort();
	}

	//goes through each bucket and applies the numbers in order to the sorted array
	for (int i=0; i<numBuckets; i++)
	{
		//gets size of the current bucket that was returned
		currentBucketSize = buckets[i].size();
		for (int j=0; j<currentBucketSize; j++)
		{
			//copies data then pops data from front
			sortedValues[dataIndex]=buckets[i].front();
			buckets[i].pop_front();
			
			//increments to the next position in the 1d array
			dataIndex++;
		}
	}

	endTime = MPI_Wtime() - startTime;

	printf("Numbers is File:           %d\n", numbersInFile);
	printf("Time taken to sort:   %f\n", endTime);

	//ends program
	MPI_Finalize();
}
