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
	
	//variables for timing
	//double startTime = 0.0;
	//double endTime = 0.0;

	//loop counters
	int xVal;
	int yVal;

	//2d array to store data
	int** colors= new int* [(int)display_width];
	for (int i=0; i<display_width; i++)
	{
		colors[i] = new int [(int)display_height];
	}

	//1d array to transfer 2d data to
	int* oneDcolors = new int [(int)(display_height*display_width)];

	struct Complex c;

	//values for image orientation
	float real_min = -2.0;
	float real_max = 1.0;
	float imag_min = -1.2;
	float imag_max = imag_min+(real_max-real_min)*display_height/display_width;

	//values for image scaling
	float scale_real = (real_max-real_min) / display_width;
	float scale_imag = (imag_max-imag_min) / display_height;

	//double nests loop to calculate the data for each pixel sequentially
	for (xVal=0; xVal < display_width; xVal++)
		{
			for (yVal=0; yVal < display_height; yVal++)
			{
				c.real = real_min + ((float) xVal*scale_real);
				c.imag = imag_min + ((float) yVal*scale_imag);
				colors[xVal][yVal] = cal_pixel(c);
			}
		}

	//loop counter for 1d
	int counter = 0;

	//loops to transfer data to 1d array
	for (int x=0; x< display_width; x++)
	{
		for (int y=0; y< display_height; y++, counter++)
		{
			oneDcolors[counter] = colors[x][y];
		}
	}

	//prints array
	writeImage(oneDcolors, display_width, display_height);
	
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

void writeImage(int* img, int display_width, int display_height)
{
	//varialbles for loops
	int row, col;

	//static printing path
	FILE* fp = fopen("../../bin/SEQUENTIALMANDELBROT.ppm", "wb");

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
