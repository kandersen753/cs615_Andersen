#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int findMax (int* data, int size);

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
	int maxVal;
	int currentBucketSize;
	int dataIndex = 0;

	//timer variables
	double startTime;
	double checkPoint1;
	double checkPoint2;
	double endTime;
	double overAllTime;

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
	}

	//closes file stream
	fin.close();
	
	startTime = MPI_Wtime();

	//gets the max value in the list
	maxVal = findMax(values, numbersInFile);

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

	///gets time for distributing to buckets
	checkPoint1 = MPI_Wtime() - startTime;

	//sort each of the buckets
	for (int i=0; i<numBuckets; i++)
	{
		buckets[i].sort();
	}

	//gets time to sort data in all buckets
	checkPoint2 = MPI_Wtime() - checkPoint1;

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

	endTime = MPI_Wtime() - checkPoint2;
	overAllTime = MPI_Wtime() - startTime;

	printf("Numbers is File:           %d\n", numbersInFile);
	printf("Time to distribute:        %f\n" , checkPoint1);
	printf("Time to only sort buckets: %f\n", checkPoint2);
	printf("Time to reorganize list:   %f\n", endTime);
	printf("Overall time:              %f\n", overAllTime);

	//ends program
	MPI_Finalize();
}

//finds the maximum value within an array
int findMax (int* data, int size)
{
	//assigns first data value as the mx
	int max = data[0];

	//loops to see what data is the largest, constantly replacing as a larger number comes by
	for (int i=1; i<size; i++)
	{
		//checks if current data is larger than current max
		if (data[i] > max)
		{
			max = data[i];
		}
	}

	//returns maximum value in array
	return max;
}