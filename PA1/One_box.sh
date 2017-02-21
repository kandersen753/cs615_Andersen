#!/bin/bash
#sbatch make clean
#sbatch make
#sbatch --output=output/oneBox_%A.csv
srun -n2 --time=00:05:00 pa1