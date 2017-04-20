# PA4: "Matrix Multiplication"

# Dependencies, Building, and Running

## Dependency Instructions
These projects depend on the MPI Library and SLIURM Running environment preinstalled in [h1.cse.unr.edu](h1.cse.unr.edu).  There are no instructions included on how to install these suites on your personal machines.
If you are using a Windows machine, you may SSH into your respective UNR accounts through [PuTTY] (http://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html).

You may enter h1 off campus by first logging into your account through ubuntu.cse.unr.edu.  (Please note that keystrokes will be a tiny bit slower, but it's still manageable).
```bash
ssh username@ubuntu.cse.unr.edu
ssh username@h1.cse.unr.edu
```

## Building and Running
There is one option to build this project and that is by a Makefile.  
This option is to use the provided Makefile, which is easier to look at and compile from.

Running the make in a separate directory will allow easy cleanup of the build data, and an easy way to prevent unnecessary data to be added to the git repository.

From the main directory of PA4 to compile
```bash
cd build
make
```


Now to run the program, while still in the build folder, enter the following commands to run the sequential version of the code:
```bash
sbatch seqMatrix.sh
```

To run the parallel version of the program input the following:
```bash
sbatch parallelMatrix.sh
```

To adjust how the parallel version runs, while in the build folder, open parallelMatrix.sh in a text editor.
To adjust how many processors you are running, adjust the 'n' value.  To adjust how many boxes you are runnning on adjust the 'N' value.
Keep in mind that for every interval of 8 on 'n' there must be 1 corresponding 'N'.  i.e. if you want 25 processors, the values would be the following:
'-n25' and '-N4'.  The value on line 7 next to the word pa4, represents the size of the matrix.  If the value entered is 400, the matrix will be of size 400x400.

If you would like addiditional information on the matrix's and their calculations please uncomment lines 136-163, 379, and 384-396


If you would like to adjust the amount of data being processed on the sequential, open the bash file and adjust the srun line.

In order to see the data that is being displayed follow these instructions to get to the appropriate file from the build folder.
```bash
cd ../bin
```
The text file within the bin folder is named "results.txt", open it with any text editor to see what is printed within.


To clean the files you've created, there is an extra target, `clean`. From the PA4 main directory input the following commands to clean all of the executable files
```bash
cd build
make clean
```
