#!/bin/bash

#SBATCH -n25
#SBATCH -N4
#SBATCH --time=00:10:00

srun pa4 5000 >> ../bin/results.txt