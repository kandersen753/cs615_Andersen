#!/bin/bash

#SBATCH -n1
#SBATCH -N1
#SBATCH --time=00:10:00

srun pa4 1100 >> ../bin/outputSequentialData.txt
