#!/bin/bash

#SBATCH -n1
#SBATCH -N1
#SBATCH --time=00:05:00

srun pa3 ../bin/inputs/1000000000.txt  > ../bin/outputs/data1000000000.txt