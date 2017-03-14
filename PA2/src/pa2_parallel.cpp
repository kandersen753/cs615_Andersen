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
#define DATA_TAG 0
#define TERMINATOR 1
#define REQUEST_TAG 2
using namespace std;

struct Complex
{
	float real;
	float imag;
}complex;



int cal_pixel(struct Complex c);

void writeImage(unsigned char* img, int display_width, int display_height);

void addData(int row, int width, unsigned char* data, unsigned char* display);


int main (int argc, char *argv[])
{
	int display_width = 5000;
	int display_height = 5000;

	int   numtasks, taskid, len;
	char hostname[MPI_MAX_PROCESSOR_NAME];

	//gets numtasks and taskid
	MPI_Init(&argc, &argv);

	//assigns numtasks and taskid to the world
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

	//gets the hostname
	MPI_Get_processor_name(hostname, &len);

	struct Complex c;

	//sets values for the image orientation
	float real_min = -2.0;
	float real_max = 1.0;
	float imag_min = -1.2;
	float imag_max = imag_min+(real_max-real_min)*display_height/display_width;

	//sets the scale values for the image
	float scale_real = (real_max-real_min) / display_width;
	float scale_imag = (imag_max-imag_min) / display_height;


		//master task
		if(taskid == MASTER)
		{
			//variables for calculations
			unsigned char* disp = new unsigned char [display_height*display_width];
			float delta, start, end;
			int row = 0;
			int count = 0;

			//has total number of tasks, and each offset represents another
			//slave node, and the value stored is the row being calculated
			int rowTracker[numtasks]; 
			MPI_Status status;
			unsigned char* rowBuffer;
			
			
	  
	  
			//allocate space for arrays
			rowBuffer = new unsigned char [display_width];
			disp = new unsigned char [display_height * display_width];
	  
	  		//starts timer
			start = MPI_Wtime();
			
			//initial task assignment
			for (int proc = 1; proc < numtasks; proc++)
			{
					MPI_Send(&row, 1, MPI_INT, proc, DATA_TAG, MPI_COMM_WORLD);
					rowTracker[proc] = row;
					row++;
					count++;
	  
			}
	  
			//as tasks come back, reassign a task to the current slave
			do
			{
					//receive data from slave X
					MPI_Recv(rowBuffer, display_width, MPI_UNSIGNED_CHAR, MPI_ANY_SOURCE, 
								REQUEST_TAG, MPI_COMM_WORLD,&status);
					//decrement number of rows that are out
					count --;
					
					//store slave X's data into the main array
					addData(rowTracker[status.MPI_SOURCE], display_width, rowBuffer, disp);	    
			
					//while there is still unprocessed data, keep sending data out as needed
					if(row < display_height)
					{
						MPI_Send(&row, 1, MPI_INT, status.MPI_SOURCE, DATA_TAG, MPI_COMM_WORLD);
						rowTracker[status.MPI_SOURCE] = row;
						row++;
						count++;
					} 
					else 
					{
						MPI_Send(&row, 1, MPI_INT, status.MPI_SOURCE, TERMINATOR, MPI_COMM_WORLD);
					}
	  
			}while(count > 0);
	  
	  		//all calculations completed
			end = MPI_Wtime();
	  		
	  		//gives overall runtime
			delta = end - start;
	  		
			std::cout <<  display_height << ',' << display_width << ',' << delta << ',' << numtasks << std::endl;
	  
	  		//prints data to image file
			writeImage(disp, display_width, display_height);
		
		  	delete [] rowBuffer;
		  	delete [] disp;
			
		}
			
		//slave tasks
		if(taskid > MASTER)
		{	
			//variables for calculations
			int row;
			MPI_Status status;
			unsigned char *pxlBuff;
			
			
	  
			//allocate space for the buffer we are sending out
			pxlBuff = new unsigned char [display_width];
			
			//receives which row to calculate from master
			MPI_Recv(&row, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			
			while(status.MPI_TAG == DATA_TAG)
			{
				//process a row of pixels
				c.imag = imag_min + row * (imag_max - imag_min)/display_height;

				//process the column values
				for(int pxlCol = 0; pxlCol < display_width; pxlCol++)
				{
					c.real = real_min + pxlCol * (real_max - real_min)/display_width;
		  			
		  			//cast the int being returned as an unsigned char
					pxlBuff[pxlCol] = (unsigned char)cal_pixel(c);
				}
				
				//sends calculated row back to the master node
				MPI_Send(pxlBuff, display_width, MPI_UNSIGNED_CHAR, MASTER, REQUEST_TAG, MPI_COMM_WORLD);
			
				MPI_Recv(&row, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			}
			//deallocates array
			delete [] pxlBuff;
	    }
	
	MPI_Finalize();
	return 0;
}

int cal_pixel(struct Complex c)
{
	//vatialbe declaration for calculations
	int count;
	int max_iter;
	struct Complex z;
	float temp;
	float lengthsq;
	max_iter = 256;
	z.real = 0;
	z.imag = 0;
	count = 0;

	//loops to find the proper value for the current pixel
	do{
		temp =z.real*z.real - z.imag*z.imag + c.real;
		z.imag = 2*z.real*z.imag + c.imag;
		z.real = temp;
		lengthsq = z.real*z.real + z.imag*z.imag;
		count++;
	} while ((lengthsq < 4.0) && (count < max_iter));//loops until the appropriate value is achieved

	//returns the value for the data pixel
	return count;
}

void writeImage(unsigned char* img, int display_width, int display_height)
{
	//varialbles for loops
	int row, col;

	//static printing path
	FILE* fp = fopen("../../bin/PARALLELMANDELBROT.ppm", "wb");

	//adds header data for the image file
	fprintf(fp, "P5\n# \n%d %d\n255\n", display_width, display_height);

	//double nested loop to apply one dimensional while its treated as a 2d array
	for (row =0; row < display_height; row++)
	{
		for (col=0; col < display_width; col++)
		{
			fprintf (fp, "%c", (unsigned char)img[row*display_width+col]);
		}
	}

	//closes file stream
	fclose(fp);
}

void addData(int row, int width, unsigned char* data, unsigned char* display)
{
    //gets the offset for the 1d to treat it as 2d
    int nthRow = row * width;

    //assigns data for the nth row
    for(int col = 0; col < width; col++)
    {
		display[nthRow + col] = data[col];
    }

}
