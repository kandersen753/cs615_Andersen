# PA1: "Ping Pong"

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

\
### Makefile Instructions
The makefile works as expected and must be updated with new files added in.

```bash
cd build
make
cd ..
```

Now you can run one of 3 bash scripts, enter each as following in order to run it
Each bash script is limited to a 5 minute time limit, so if for whatever reason the program continues to execute, it will automatically be cut off at 5 minutes.  Each script will also run through sending 1-3000 integers to the other processor 100 times for each number of ints.  The program then takes the average to get a resonable number.

```bash
sbatch One_box.sh
sbatch Two_box.sh
sbatch Timing.sh
```

Each of these will output data to a file followed by the job number it was assigned by the cluster.
These output files can be found within the output folder.

To clean the files you've created, there is an extra target, `clean`.
```bash
cd build
make clean
```
