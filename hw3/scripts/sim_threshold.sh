#!/bin/bash
# Program:
#     NN sparsity simulation
Target=NN_out
Unit=MHz
Root=$HOME/gem5/"$Target" # Target dictionary for stats.txt
Gem5=$HOME/gem5/build/ARM/gem5.opt # gem5 dictionary
Script=$HOME/gem5/configs/example/simple_cnn.py # config script dictionary
Threshold=0 
Log=$Root/log_threshold # Log file for all sim time & accuracy results

mkdir -p $Root
rm $Log # Remove log if already exists
touch $Log
while [ $Threshold -le 15 ]
do
	Acc=`$Gem5 $Script --threshold ${Threshold}e-2 | grep 'accu\|Num-zero' | cut -d ' ' -f 2`
	cp m5out/stats.txt $Root/stats_${Threshold}e-2.txt
	SimSeconds=`grep 'sim_seconds' m5out/stats.txt | tr -s ' ' | cut -d ' ' -f 2`
	echo $Threshold $Acc $SimSeconds >> $Log
	Threshold=`expr $Threshold + 1`
done;
echo "finished"
exit 0
