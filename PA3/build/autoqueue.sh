#!/bin/bash

TEST = $(squeue -o"%.18i %.9p %.20j %.8u %.8T %.10M %.91 %.6D %R")

while [ [ "$TEST" =~ "kandersen-"]]
do
	sleep 1s
	TEST = $(squeue -o"%.18i %.9p %.20j %.8u %.8T %.10M %.91 %.6D %R")
done

sbatch sequentialBucket.sh ../bin/inputs/100.txt

sleep 1s

TEST = $(squeue -o"%.18i %.9p %.20j %.8u %.8T %.10M %.91 %.6D %R")

while [ [ "$TEST" =~ "kandersen-"]]
do
	sleep 1s
	TEST = $(squeue -o"%.18i %.9p %.20j %.8u %.8T %.10M %.91 %.6D %R")
done

sbatch sequentialBucket.sh ../bin/inputs/1000.txt

sleep 1s

TEST = $(squeue -o"%.18i %.9p %.20j %.8u %.8T %.10M %.91 %.6D %R")

while [ [ "$TEST" =~ "kandersen-"]]
do
	sleep 1s
	TEST = $(squeue -o"%.18i %.9p %.20j %.8u %.8T %.10M %.91 %.6D %R")
done

sbatch sequentialBucket.sh ../bin/inputs/10000.txt

sleep 1s

TEST = $(squeue -o"%.18i %.9p %.20j %.8u %.8T %.10M %.91 %.6D %R")

while [ [ "$TEST" =~ "kandersen-"]]
do
	sleep 1s
	TEST = $(squeue -o"%.18i %.9p %.20j %.8u %.8T %.10M %.91 %.6D %R")
done

sbatch sequentialBucket.sh ../bin/inputs/100000.txt

sleep 1s

TEST = $(squeue -o"%.18i %.9p %.20j %.8u %.8T %.10M %.91 %.6D %R")

while [ [ "$TEST" =~ "kandersen-"]]
do
	sleep 1s
	TEST = $(squeue -o"%.18i %.9p %.20j %.8u %.8T %.10M %.91 %.6D %R")
done

sbatch sequentialBucket.sh ../bin/inputs/1000000.txt

sleep 1s			