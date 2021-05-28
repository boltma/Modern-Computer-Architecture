#!/bin/bash
# Program:
#     CPU L1 cache simulation
Target=Cache_out
Root=$HOME/gem5/"$Target" # Target dictionary for stats.txt
Gem5=$HOME/gem5/build/X86/gem5.opt # gem5 dictionary
Script=$HOME/gem5/configs/hw2/run.py # config script dictionary 
CPUtype=o3
Log=$Root/log_$CPUtype # Log file for all sim time results

mkdir -p $Root
rm $Log # Remove log if already exists
touch $Log
for size in 1kB 2kB 4kB 8kB 16kB 32kB 64kB 128kB 256kB 512kB 1MB
do
	$Gem5 $Script $CPUtype --l1d_size=$size 
	cp m5out/stats.txt $Root/stats_${CPUtype}_$size.txt
	ipc=`grep 'ipc' m5out/stats.txt | sed -n 3p | tr -s ' ' | cut -d ' ' -f 2`
	echo $size $ipc >> $Log
done;
echo "finished"
exit 0
