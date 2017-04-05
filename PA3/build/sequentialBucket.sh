#!/bin/bash

#SBATCH -n1
#SBATCH -N1
#SBATCH --time=00:05:00

srun pa3 1000000000  > ../bin/outputs/data100000000.txt
