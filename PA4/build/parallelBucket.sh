#!/bin/bash

#SBATCH -n2
#SBATCH -N1
#SBATCH --time=00:10:00
#SBATCH --ntasks-per-node=2
srun pa3 1300000000 > ../bin/2p1_3bil.txt