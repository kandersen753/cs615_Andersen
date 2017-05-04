#!/bin/bash

#SBATCH -n25
#SBATCH -N4
#SBATCH --time=00:05:00

srun pa4 2760 0 data >> ../bin/results.txt