#!/bin/bash

#SBATCH -n1
#SBATCH -N1
#SBATCH --time=00:10:00

srun pa4 2700 >> ../bin/outputSequentialData.txt