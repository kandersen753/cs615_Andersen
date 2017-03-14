#!/bin/bash
#SBATCH -n8
#SBATCH -N4
#SBATCH --time=00:05:00
#SBATCH --priority=0
srun pa2p
