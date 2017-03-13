/******************************************************************************
* FILE: pa2.c
* DESCRIPTION:
*   Mandelbrot Sequential
* AUTHOR: Kurt Andersen
* LAST REVISED: 03/12/2017
******************************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define  MASTER		0
using namespace std;

struct Complex
{
	float real;
	float imag;
}complex;



int cal_pixel(struct Complex c);

void writeImage(int* img, int display_width, int display_height);

void loadRow(int row, int width, int* data, int* display);

int main (int argc, char *argv[])
{
	int display_width = 500;
	int display_height = 500;

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
	double startTime = 0.0;
	double endTime = 0.0;


	int** colors= new int* [(int)display_width];
	for (int i=0; i<display_width; i++)
	{
		colors[i] = new int [(int)display_height];
	}

	int* oneDcolors;

	struct Complex c;


	float real_min = -2.0;
	float real_max = 1.0;
	float imag_min = -1.2;
	float imag_max = imag_min+(real_max-real_min)*display_height/display_width;


	float scale_real = (real_max-real_min) / display_width;
	float scale_imag = (imag_max-imag_min) / display_height;


	//master tasks
	if (taskid == MASTER)
	{
		//local vars
		int row = 0;
		int count = 0;
		int tracker[numtasks]; /*Tracks what row a process is processing */
		MPI_Status status;
		int* rowBuffer;
		
		
  
  
		//allocate space for arrays
		rowBuffer = new int [display_width];
		oneDcolors = new int [display_width*display_height];
  
		startTime = MPI_Wtime();
		
		//send rows to each process initially
		for (int currentProccess = 1; currentProccess < numtasks; currentProccess++){
				MPI_Send(&row, 1, MPI_INT, currentProccess, 0, MPI_COMM_WORLD);
				tracker[currentProccess] = row;
				row++;
				count++;
  
		}
  
		//handle the returned rows and reassign tasks from the pool
		do{
				//I need data about recieved data
				MPI_Recv(rowBuffer, display_width, MPI_UNSIGNED_CHAR, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD,
				&status);
				count --;
				
				//transfer data recieved into our display array
				loadRow(tracker[status.MPI_SOURCE], display_width, rowBuffer, oneDcolors);

		
				//while we still have unprocessed rows
				//send work to various processes
				if(row < display_height){
					MPI_Send(&row, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
					tracker[status.MPI_SOURCE] = row;
					row++;
					count++;
				} 
				else {
					MPI_Send(&row, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
				}
  
		}while(count > 0);
  
		endTime = MPI_Wtime();
  
		endTime = endTime - startTime;
  
		cout <<  display_height << ',' << display_width << ',' << endTime << ',' << numtasks << endl;
  
		writeImage(oneDcolors, display_width, display_height);
	
	  	delete [] rowBuffer;
	  	delete [] oneDcolors;
	}

	else if (taskid > MASTER)
	{
		//variables for computing rows and 
		int row;
		MPI_Status status;
		int *pxlBuff;
		
		
  
		//allocate space for the buffer we are sending out
		pxlBuff = new int [display_width];
		
		MPI_Recv(&row, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		
		while(status.MPI_TAG == 0){
  
		
		//process a row of pixels
		//calculates y value
		c.imag = imag_min + row * scale_imag;

		//calculates each x value
		for(int pxlCol = 0; pxlCol < display_width; pxlCol++){
			c.real = real_min + pxlCol * scale_real;
			pxlBuff[pxlCol] = cal_pixel(c);
		}
		
		//send off row
		MPI_Send(pxlBuff, display_width, MPI_UNSIGNED_CHAR, MASTER, 2, MPI_COMM_WORLD);
	
		
		MPI_Recv(&row, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		}

		delete [] pxlBuff;;
	}
	
	MPI_Finalize();
	return 0;
}

int cal_pixel(struct Complex c)
{
	int count;
	int max_iter;
	struct Complex z;
	float temp;
	float lengthsq;
	max_iter = 256;
	z.real = 0;
	z.imag = 0;
	count = 0;

	do{
		temp =z.real*z.real - z.imag*z.imag + c.real;
		z.imag = 2*z.real*z.imag + c.imag;
		z.real = temp;
		lengthsq = z.real*z.real + z.imag*z.imag;
		count++;
	} while ((lengthsq < 4.0) && (count < max_iter));
	return count;
}

void writeImage(int* img, int display_width, int display_height)
{
	int row, col;

	FILE* fp = fopen("../../bin/imageP.ppm", "wb");

	fprintf(fp, "P5\n# \n%d %d\n255\n", display_width, display_height);

	for (row =0; row < display_height; row++)
	{
		for (col=0; col < display_width; col++)
		{
			fprintf (fp, "%c", (unsigned char)img[row*display_width+col]);
		}
	}

	fclose(fp);
}

void loadRow(int row, int width, int* data, int* display){
    //calc row offset for 1d array
    int rowoffset = row * width;

    for(int col = 0; col < width; col++){
		display[rowoffset + col] = data[col];
    }

}
