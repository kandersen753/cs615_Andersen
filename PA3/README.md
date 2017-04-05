# PA3: "Bucket Sort"

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

From the main directory of PA3 to compile
```bash
cd build
make
```


Now to run the program, while still in the build folder, enter the following commands:
```bash
sbatch sequentialBucket.sh
```
The current setup of the script will run pa3 as a sequential program on 1 processor.  Right now it is limited to 100 data items to be sorted.  This is the smallest set of data I sort.  To change the file you want to run from, go into the script file, and add an extra 0 to the .txt files.  Doing the following instructions will make the code run 1000 data points instead of 100.

```bash
change from this:
srun pa3 ../bin/inputs/100.txt  > ../bin/outputs/data100.txt
to:
srun pa3 ../bin/inputs/1000.txt  > ../bin/outputs/data1000.txt
```


To clean the files you've created, there is an extra target, `clean`. From the PA2 main directory input the following commands to clean all of the executable files
```bash
cd build
make clean
```
