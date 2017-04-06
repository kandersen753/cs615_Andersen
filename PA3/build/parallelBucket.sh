#!/bin/bash

#SBATCH -n17
#SBATCH -N3
#SBATCH --time=00:10:00

srun pa3 1300000000 > ../bin/17p1_3bil.txt