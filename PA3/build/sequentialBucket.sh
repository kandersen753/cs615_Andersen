#!/bin/bash

#SBATCH -n1
#SBATCH -N1
#SBATCH --time=00:05:00

srun pa3 ../bin/inputs/100.txt  > ../bin/outputs/data100.txt
