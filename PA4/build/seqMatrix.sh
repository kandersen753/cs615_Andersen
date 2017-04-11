#!/bin/bash

#SBATCH -n1
#SBATCH -N1
#SBATCH --time=00:10:00

srun pa4 1500 > ../bin/1500x1500seq.txt