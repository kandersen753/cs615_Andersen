#!/bin/bash
#SBATCH -n2
#SBATCH --time=00:05:00
#SBATCH --output=output/oneBox_%A.csv
srun build/pa1