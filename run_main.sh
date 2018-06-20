#!/bin/bash
#SBATCH --ntasks 100
#SBATCH --time 1:00:00
#SBATCH --qos bearchal03
#SBATCH --reservation BearChallenge
#SBATCH --output=task.out

module purge
module load bluebear
module load bear-apps/2018a
module load iomkl/2018a

mpicc main.c -o main

mpirun main
