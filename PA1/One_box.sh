#!/bin/bash


make clean
make
#sbatch -n2
#sbatch --time=00:05:00
#sbatch --output=output/oneBox_%A.csv
srun pa1