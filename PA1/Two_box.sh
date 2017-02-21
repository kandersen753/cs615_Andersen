#!/bin/bash
#SBATCH -n2
#SBATCH -N2
#SBATCH --ntasks-per-node=1
#SBATCH --time=00:05:00
#SBATCH --output=output/TwoBox_%A.csv
srun build/pa1