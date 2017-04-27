#!/bin/bash

#SBATCH -n9
#SBATCH -N2
#SBATCH --time=00:10:00

srun pa4 6 1 data >> ../bin/results.txt