#!/bin/bash

MAX = 1000000
STEP = 10

for ((i=100; i<=$MAX; i*=$STEP))
do
	TEST = $(squeue -o"%.18i %.9p %.20j %.8u %.8T %.10M %.91 %.6D %R")

	while [ [ "$TEST" =~ "kandersen-"]]
	do
		sleep 1s
		TEST = $(squeue -o"%.18i %.9p %.20j %.8u %.8T %.10M %.91 %.6D %R")
	done

	sbatch sequentialBucket.sh $i

	sleep 1s
done			