#!/bin/bash

#SBATCH -n1
#SBATCH -N1
#SBATCH --time=00:10:00

srun pa3 1300000000 > ../bin/s1_3bil.txt