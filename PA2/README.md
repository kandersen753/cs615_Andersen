# PA2: "Mandelbrot Set"

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

With this project I have 2 seperate makefiles for each Sequential and Parallel programs 

\
### Makefile Instructions
The makefile works as expected and must be updated with new files added in.


From the main directory of PA2 to compile Parallel
```bash
cd build/parallel
make
```

From the main directory of PA2 to compile Sequential
```bash
cd build/sequential
make
```

Now you can run one of two bash scripts.  Each bash script is located with the appropriate compiled file. In order to run the parallel program from the PA2 main directiory input the following...
```bash
cd build/parallel
sbatch dynamic.sh
```

In order to run the sequential version of the code, input the following commands from the PA2 main directory.
```bash
cd build/sequentail
sbatch sequential.sh
```

These two scripts will output a file of 5000x5000 pixels to an image in the bin folder.  They will be named appropriately to the program that executed it. i.e. "sequentailmandelbrot.ppm"

To clean the files you've created, there is an extra target, `clean`. From the PA2 main directory input the following commands to clean all of the executable files
```bash
cd build/parallel
make clean
cd ..
cd sequentail
make clean
```
