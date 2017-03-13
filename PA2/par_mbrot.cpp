#include "PIMFuncs.h"
#include "mpi.h"
#include <cstdlib>
#include <iostream>
#include <cstring>

#define MASTER 0
#define DATA_TAG 0
#define TERMINATE_TAG 1
#define REQ_TAG 2


struct complex{
    float real;
    float imag;
};

struct window{
    int width;
    int height;
};



/**
 * Function: calcPixel
 * About: Calulates the value of one pixel and returns it as an
 *       int descripbing the number of iterations used to perform 
 *	 the calculation per the requirements of the mandelbrot set
 *
 */
int calcPixel(complex c);

/**
  * Function: loadRow
  * About: transfers a row of processed pixels into the proper
  *        location the display grid for later printing
  */
void loadRow(int row, int width, unsigned char* data, unsigned char* display);

/**
  * Function: loadMultipleRows
  * About: Loads a specified number of rows into our 
  *
  */
void loadMultipleRows(int row, window w, int localNumRows, unsigned char* data, unsigned char* display);




int main(int argc, char* argv[]){

    //variables
    complex c;
    window w;
    unsigned char* disp;
    float real_min = -2.0, real_max = 2.0, imag_min = -2.0, imag_max = 2.0;
    int numprocs, myid;
    double start, end, delta; /*For timings*/
//    bool colorFlag; 
    bool staticFlag = false;
    

    //setup parallel components
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    
    /*****************
    General Processing
    ******************/
    
    //check for height and width args
    if(argc < 4){
        std::cout << "Please pass width, height and output as arguments: eg. <executiable> 640 480 image.pgm" << std::endl;
	      return 0;
    } else if (argc == 5 && !strcmp(argv[4],"-s")){
        staticFlag = true;
    }  

    //assign width and height to window variable
    w.width = atoi(argv[1]);
    w.height = atoi(argv[2]);
    
    
    /***************************
    Sequential Processing Option
    ****************************/
    if( numprocs == 1){

		//allocate space for disp
		disp = new unsigned char [w.height * w.width];
		
		start = MPI_Wtime();

		for(int pxlRow = 0; pxlRow < w.height; pxlRow++){
			for(int pxlCol = 0; pxlCol < w.width; pxlCol++){
				c.imag = imag_min + pxlRow * (imag_max - imag_min)/w.height;
				c.real = real_min + pxlCol * (real_max - real_min)/w.width;

				disp[(w.width * pxlRow) + pxlCol] = (unsigned char)calcPixel(c);
			}
			
		}

		end = MPI_Wtime();

		delta = end - start;
		
		std::cout <<  w.height << ',' << w.width << ',' << delta << std::endl;

		pim_write_black_and_white("../logs/sequential_mandelbrot.pgm", w.width, w.height, disp);

		delete [] disp;
    
    }


    /**************************
    Parallel Programming Option
    ***************************/

    else{
	

      /**************************
      Static Task Assignment
      **************************/
		if (staticFlag == true){

		//
      	//MASTER Sends and assigns tasks
      	//
      
      
			if(myid == MASTER){
				//local vars
				unsigned char * rowBuffer;
				int row, proc;
				int rowsPerTask;
				int count = 0;
				int rowTracker[numprocs];
				MPI_Status status;

				//calulate number of rows per task
				rowsPerTask = w.height/(numprocs-1);
			  
				//allocate space for arrays
				rowBuffer = new unsigned char [w.width * rowsPerTask]; //some adjustments needed
				disp = new unsigned char [w.height * w.width]; 


					start = MPI_Wtime();
				
				//send and recieve all rows broken up by size/numTasks
				for(row = 0, proc=1; proc < numprocs; proc++, row += rowsPerTask){
					MPI_Send(&row, 1, MPI_INT, proc, DATA_TAG, MPI_COMM_WORLD);
					rowTracker[proc] = row;
					count++;
				}

				do{
					MPI_Recv(rowBuffer, (w.width*rowsPerTask), MPI_UNSIGNED_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
					count--;

					loadMultipleRows(rowTracker[status.MPI_SOURCE], w, rowsPerTask, rowBuffer, disp);
					if(status.MPI_SOURCE == 2){
						pim_write_black_and_white("partial.pgm", w.width, 500, rowBuffer);
					}
					
					
				} while (count > 0);
				
				end = MPI_Wtime();
		  
				delta = end - start;

				//std::cout <<  w.height << ',' << w.width << ',' << delta << ',' << numprocs << std::endl;
		  
				pim_write_black_and_white(argv[3], w.width, w.height, disp);
				
			}
		  
			//
			// SLAVES
			//
			
			if(myid > MASTER){
				int startingRow, numRows;
				int localRow = 0; //necessary for loading calulated pixels at the proper location
				MPI_Status status;
				unsigned char *pxlBuff;
				
				//store the number of rows our program expects to process
				numRows = w.height/(numprocs - 1);
				

		  
				//allocate space for the buffer we are sending out
				pxlBuff = new unsigned char [w.width*numRows];
				
				MPI_Recv(&startingRow, 1, MPI_INT, MASTER, DATA_TAG, MPI_COMM_WORLD, &status);
				
				std::cout << "Hello from: " << myid << ". I'm incharge of " << startingRow << std::endl;
				
				
				for(int pxlRow = startingRow; pxlRow < numRows; pxlRow++, localRow++){
					c.imag = imag_min + pxlRow * (imag_max - imag_min)/w.height;
					
					for(int pxlCol = 0; pxlCol < w.width; pxlCol++){
						c.real = real_min + pxlCol * (real_max - real_min)/w.width;

						pxlBuff[(w.width * localRow) + pxlCol] = (unsigned char)calcPixel(c);
					}
				}


			
				
				//send off row
				MPI_Send(pxlBuff, (w.width*numRows), MPI_UNSIGNED_CHAR, MASTER, REQ_TAG, MPI_COMM_WORLD);
      	
			}
      	}
      
      

      /***********************
      Dynmaic Task Assignment
      ***********************/
		else{      
      	
		
			//
			//MASTER Sends and assigns tasks
			//
		  
		  
			if(myid == MASTER){
				//local vars
				int row = 0;
				int count = 0;
				int rowTracker[numprocs]; /*Tracks what row a process is processing */
				MPI_Status status;
				unsigned char* rowBuffer;
				
				
		  
		  
				//allocate space for arrays
				rowBuffer = new unsigned char [w.width];
				disp = new unsigned char [w.height * w.width];
		  
				start = MPI_Wtime();
				
				//send rows to each process initially
				for (int proc = 1; proc < numprocs; proc++){
						MPI_Send(&row, 1, MPI_INT, proc, DATA_TAG, MPI_COMM_WORLD);
						rowTracker[proc] = row;
						row++;
						count++;
		  
				}
		  
				//handle the returned rows and reassign tasks from the pool
				do{
						//I need data about recieved data
						MPI_Recv(rowBuffer, w.width, MPI_UNSIGNED_CHAR, MPI_ANY_SOURCE, REQ_TAG, MPI_COMM_WORLD,
						&status);
						count --;
						
						//transfer data recieved into our display array
						loadRow(rowTracker[status.MPI_SOURCE], w.width, rowBuffer, disp);	    
				
						//while we still have unprocessed rows
						//send work to various processes
						if(row < w.height){
							MPI_Send(&row, 1, MPI_INT, status.MPI_SOURCE, DATA_TAG, MPI_COMM_WORLD);
							rowTracker[status.MPI_SOURCE] = row;
							row++;
							count++;
						} 
						else {
							MPI_Send(&row, 1, MPI_INT, status.MPI_SOURCE, TERMINATE_TAG, MPI_COMM_WORLD);
						}
		  
				}while(count > 0);
		  
				end = MPI_Wtime();
		  
				delta = end - start;
		  
				std::cout <<  w.height << ',' << w.width << ',' << delta << ',' << numprocs << std::endl;
		  
				pim_write_black_and_white(argv[3], w.width, w.height, disp);
			
			  	delete [] rowBuffer;
			  	delete [] disp;
				
			}
		  
			//
			// SLAVES
			//
			
			if(myid > MASTER){
				int row;
				MPI_Status status;
				unsigned char *pxlBuff;
				
				
		  
				//allocate space for the buffer we are sending out
				pxlBuff = new unsigned char [w.width];
				
				MPI_Recv(&row, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				
				while(status.MPI_TAG == DATA_TAG){
		  
				
				//process a row of pixels
				c.imag = imag_min + row * (imag_max - imag_min)/w.height;
				for(int pxlCol = 0; pxlCol < w.width; pxlCol++){
					c.real = real_min + pxlCol * (real_max - real_min)/w.width;
		  
					pxlBuff[pxlCol] = (unsigned char)calcPixel(c);
				}
				
				//send off row
				MPI_Send(pxlBuff, w.width, MPI_UNSIGNED_CHAR, MASTER, REQ_TAG, MPI_COMM_WORLD);
			
				
				MPI_Recv(&row, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				}

				delete [] pxlBuff;
			}
		
	    }
      
    }


    MPI_Finalize();

    return 0;
}

int calcPixel(complex c){
    //variables
    complex z;
    int cnt, max_iter;
    float temp, lengthSq;

    //initialize
    z.real = 0.0;
    z.imag = 0.0;
    max_iter = 256; //darkest pixel possible

    for(cnt = 0; ((lengthSq < 4.0) && (cnt < max_iter)); cnt++){
	//hold the calculations for Zk+1 real in a temp  
	//so we can use Zk real for z.imag
	temp = (z.real*z.real) - (z.imag*z.imag) + c.real;
	z.imag = 2 * z.real * z.imag + c.imag;
	z.real = temp;

	lengthSq = z.real * z.real + z.imag * z.imag;
    }
   
   return cnt;

}

void loadRow(int row, int width,  unsigned char* data, unsigned char* display){
    //calc row offset for 1d array
    int rowoffset = row * width;

    for(int col = 0; col < width; col++){
		display[rowoffset + col] = data[col];
    }

}

void loadMultipleRows(int row, window w, int localNumRows, unsigned char* data, unsigned char* display){
	int localRowOffset = 0;
	
	for(int rowoffset = row; localRowOffset < localNumRows && rowoffset < w.height ; rowoffset++, localRowOffset++){
		std::cout << rowoffset << std::endl;
		for(int col = 0; col < w.width; col++){
			display[rowoffset*w.width + col] = data[localRowOffset*w.width + col];
		}
	}
}

