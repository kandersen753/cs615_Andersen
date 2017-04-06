#!/bin/bash

#SBATCH -n1
#SBATCH -N1
#SBATCH --time=00:10:00

srun pa3 1000000000 > ../bin/S1bil.txt