#!/bin/bash

#SBATCH -n16
#SBATCH -N2
#SBATCH --time=00:05:00

srun pa4 1200 0 data >> ../bin/results.txt