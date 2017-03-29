#!/bin/bash
#SBATCH -n15
#SBATCH -N2
#SBATCH --time=00:05:00
#SBATCH --priority=0
srun pa2p
