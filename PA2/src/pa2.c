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
#include "PIMFuncs.h"
#define  MASTER		0

struct Complex
{
	float real;
	float imag;
}complex;

int cal_pixel(struct Complex c);

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
	double startTime = 0.0;
	double endTime = 0.0;

	//loop counters
	int xVal;
	int yVal;

	char filePath = "bin/attempt.jpeg";

	float colors[500][500];

	struct Complex c;

	int display_width = 500;
	int display_height = 500;
	
	float real_min = -2.0;
	float real_max = -2.0;
	float imag_min = 2.0;
	float imag_max = 2.0;

	float scale_real = (real_max-real_min) / display_width;
	float scale_imag = (imag_max-imag_min) / display_height;

	for (xVal=0; xVal < display_width; xVal++)
		{
			for (yVal=0; yVal < display_height; yVal++)
			{
				c.real = real_min + ((float) xVal*scale_real);
				c.imag = imag_min + ((float) yVal*scale_imag);
				color[xVal][yVal] = cal_pixel(c);
			}
		}

	bool pim_write_black_and_white(filePath, display_width, display_height, colors);


	
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