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
#define  MASTER		0

struct Complex
{
	float real;
	float imag;
}complex;

int display_width = 500;
int display_height = 500;

int cal_pixel(struct Complex c);

void writeImage(int* img);

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
	//double startTime = 0.0;
	//double endTime = 0.0;

	//loop counters
	int xVal;
	int yVal;

	int colors[display_height][display_width];


	int onedcolors[display_height*display_width];

	struct Complex c;


	float real_min = -2.0;
	float real_max = 1.0;
	float imag_min = -1.2;
	float imag_max = imag_min+(real_max-real_min)*display_height/display_width;


	float scale_real = (real_max-real_min) / display_width;
	float scale_imag = (imag_max-imag_min) / display_height;


	for (xVal=0; xVal < display_width; xVal++)
		{
			for (yVal=0; yVal < display_height; yVal++)
			{
				c.real = real_min + ((float) xVal*scale_real);
				c.imag = imag_min + ((float) yVal*scale_imag);
				colors[xVal][yVal] = cal_pixel(c);
			}
		}

	int counter = 0;

	for (int x=0; x< display_width; x++)
	{
		for (int y=0; y< display_height; y++, counter++)
		{
			onedcolors[counter] = colors[x][y];
		}
	}
	writeImage(onedcolors);



	
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

void writeImage(int* img)
{
	int row, col;

	FILE* fp = fopen("../bin/image.ppm", "wb");

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