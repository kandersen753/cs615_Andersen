#!/bin/bash
#sbatch make clean
#sbatch make
srun -n2 --time=00:05:00 pa1 > output/oneBox.txt