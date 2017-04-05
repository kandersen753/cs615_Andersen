<<<<<<< HEAD
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

	
	int* values;

	std::string filePath;
	int maxVal = 0;
	int currentBucketSize;
	int dataIndex = 0;

	int numValues;
	std::string numval = argv[1];
	numValues = atoi(numval.c_str());

	//timer variables
	double startTime = 0;
	double endTime = 0;

	if (numtasks ==1)
	{
		//variables used throughout program
		int numBuckets=10;
		std::vector<int> buckets[numBuckets];

		//allocate memory for main array
		values = new int [numValues];
		int* sortedValues = new int [numValues];

		for (int i=0; i<numValues; i++)
		{
			values[i] = random()%1000;
			if (maxVal < values[i])
			{
				maxVal = values[i];
			}
		}

		
		startTime = MPI_Wtime();

		//split data into buckets
		//iterates through each value
		for (int i=0; i<numValues; i++)
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
			std::sort(buckets[i].begin(), buckets[i].end());
		}

		//goes through each bucket and applies the numbers in order to the sorted array
		for (int i=0; i<numBuckets; i++)
		{
			//gets size of the current bucket that was returned
			currentBucketSize = buckets[i].size();
			for (int j=0; j<currentBucketSize; j++)
			{
				//copies data then pops data from front
				sortedValues[dataIndex]=buckets[i][j];
				
				
				//increments to the next position in the 1d array
				dataIndex++;
			}
		}
		endTime = MPI_Wtime() - startTime;
		printf("Numbers is File:           %d\n", numValues);
		printf("Time taken to sort:   %f\n", endTime);
		delete[] sortedValues;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//parallel code
	else
	{
		if (taskid == 0)
		{
			//allocate memory for buckets
			int numBuckets=numtasks;
			std::vector<int> buckets[numBuckets];
			MPI_Status status;
			int* dataToSend[numBuckets];
			int arraySizeToSend;
			int arraySizeToReceive;
			int* dataToReceive[numBuckets];
			std::vector<int> finalBucket;
			MPI_Request request;

			//allocate memory for main array
			values = new int [numValues/numtasks+(numValues%numtasks)];

			//loop to send data to last processor back to first
			for (int j=numtasks-1; j>=0; j--)
			{
				if (j!= 0)
				{
					//loops to generate values
					for (int i=0; i<numValues/numtasks; i++)
					{
						values[i] = random()%1000;	
					}
				}
				else if (j == 0)
				{
					//loops to generate values
					for (int i=0; i<numValues/numtasks+(numValues%numtasks); i++)
					{
						values[i] = random()%1000;	
					}
				}

				//send data to the jth processor
				if (j>0)
				{
					MPI_Send(values, numValues/numtasks, MPI_INT, j, 0, MPI_COMM_WORLD);
				}
			}

			//wait for all processors to get data to sort
			MPI_Barrier(MPI_COMM_WORLD);

			startTime = MPI_Wtime();

			//distribute data on this processor to buckets
			for (int i=0; i<numValues/numtasks; i++)
			{
				//checks which bucket it can fit into
				for (int j=0; j<numBuckets; j++)
				{
					//each side of the and is the minumum value and the maximum value of the current bucket
					if (values[i] >= 1000.0*(((float)j)/numBuckets) && values[i] <= 1000.0*(((float)j+1)/numBuckets))
					{
						//if the value is between the min/max of the bucket, then it is pushed into that bucket
						buckets[j].push_back(values[i]);
						break;
					}
				}
			}


			//loop to send bucket data to each other processor
			for (int i=0; i<numtasks; i++)
			{
				//allocate memory to send arrays instead of vector
				dataToSend[i] = new int [buckets[i].size()];

				//if not current process send data
				if (i != taskid)
				{
					//copy data from vector to array
					for (int j=0; j<(int)buckets[i].size(); j++)
					{
						dataToSend[i][j] = buckets[i][j];
					}					
					//first send size of array that will be coming
					arraySizeToSend = buckets[i].size();

					MPI_Isend(&arraySizeToSend, 1, MPI_INT, i, taskid*10+1, MPI_COMM_WORLD, &request);

					//send array
					MPI_Isend(&dataToSend[i][0], buckets[i].size(), MPI_INT, i, taskid, MPI_COMM_WORLD, &request);

				}
			}

			//copy data from bucket to final sorting array
			for (unsigned int i=0; i<buckets[taskid].size(); i++)
			{
				finalBucket.push_back(buckets[taskid][i]); 
			}

			//receive data from all sources and move to main vector for sorting
			for (int i=0; i<numtasks; i++)
			{
				//ensure not this task
				if (taskid != i)
				{

					//receive array size
					MPI_Recv(&arraySizeToReceive, 1, MPI_INT, i, i*10+1, MPI_COMM_WORLD, &status);

					//allocate for incoming array
					dataToReceive[i] = new int [arraySizeToReceive];

					//store data for incoming array

					MPI_Recv(&dataToReceive[i][0], arraySizeToReceive, MPI_INT, i, i, MPI_COMM_WORLD, &status);


					//push data from received array to sortable bucket
					for (int j=0; j<arraySizeToReceive; j++)
					{
						finalBucket.push_back(dataToReceive[i][j]);
					}
				}
			}

			//sort final vector
			std::sort(finalBucket.begin(), finalBucket.end());

			MPI_Barrier(MPI_COMM_WORLD);



			endTime = MPI_Wtime() - startTime;
			printf("Numbers is File:           %d\n", numValues);
			printf("Time taken to sort:   %f\n", endTime);
			delete[] values;
		}
		else if (taskid > 0)
		{
			int numBuckets=numtasks;
			std::vector<int> buckets[numBuckets];
			MPI_Status status;
			int* dataToSend[numBuckets];
			int arraySizeToSend;
			int arraySizeToReceive;
			int* dataToReceive[numBuckets];
			std::vector<int> finalBucket;
			MPI_Request request;

			//allocate memory for main array
			values = new int [numValues/numtasks];

			//receive data from master
			MPI_Recv(values, numValues/numtasks, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

			MPI_Barrier(MPI_COMM_WORLD);

			//organize data to buckets
			for (int i=0; i<numValues/numtasks; i++)
			{
				//checks which bucket it can fit into
				for (int j=0; j<numBuckets; j++)
				{
					//each side of the and is the minumum value and the maximum value of the current bucket
					if (values[i] >= 1000.0*(((float)j)/numBuckets) && values[i] <= 1000.0*(((float)j+1)/numBuckets))
					{
						//if the value is between the min/max of the bucket, then it is pushed into that bucket
						buckets[j].push_back(values[i]);
						break;
					}
				}
			}

			//loop to send bucket data to each other processor
			for (int i=0; i<numtasks; i++)
			{
				//allocate memory to send arrays instead of vector
				dataToSend[i] = new int [buckets[i].size()];

				//if not current process send data
				if (i != taskid)
				{
					//copy data from vector to array
					for (int j=0; j<(int)buckets[i].size(); j++)
					{
						dataToSend[i][j] = buckets[i][j];
					}					
					//first send size of array that will be coming
					arraySizeToSend = buckets[i].size();
					MPI_Isend(&arraySizeToSend, 1, MPI_INT, i, taskid*10+1, MPI_COMM_WORLD, &request);

					//send array
					MPI_Isend(&dataToSend[i][0], buckets[i].size(), MPI_INT, i, taskid, MPI_COMM_WORLD, &request);

				}
			}

			//copy data from bucket to final sorting array
			for (int i=0; i<(int)buckets[taskid].size(); i++)
			{
				finalBucket.push_back(buckets[taskid][i]); 
			}

			//receive data from all sources and move to main vector for sorting
			for (int i=0; i<numtasks; i++)
			{
				//ensure not this task
				if (taskid != i)
				{
					//receive array size
					MPI_Recv(&arraySizeToReceive, 1, MPI_INT, i, i*10+1, MPI_COMM_WORLD, &status);

					//allocate for incoming array
					dataToReceive[i] = new int [arraySizeToReceive];

					//store data for incoming array
					MPI_Recv(&dataToReceive[i][0], arraySizeToReceive, MPI_INT, i, i, MPI_COMM_WORLD, &status);

					//push data from received array to sortable bucket
					for (int j=0; j<arraySizeToReceive; j++)
					{
						finalBucket.push_back(dataToReceive[i][j]);
					}
				}
			}

			//sort final vector
			std::sort(finalBucket.begin(), finalBucket.end());

			MPI_Barrier(MPI_COMM_WORLD);

			delete[] values;

		}
	}

	//ends program
	MPI_Finalize();
}
=======
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
>>>>>>> ee3807918e7af453d218b07b9436f0594ea2ddea
