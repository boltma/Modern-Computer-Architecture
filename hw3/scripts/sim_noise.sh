#!/bin/bash
# Program:
#     RRAM noise simulation
Target=NN_out
Unit=MHz
Root=$HOME/gem5/"$Target" # Target dictionary for stats.txt
Gem5=$HOME/gem5/build/ARM/gem5.opt # gem5 dictionary
Script=$HOME/gem5/configs/example/binnet.py # config script dictionary
Noise=0
Log=$Root/log_noise # Log file for all accuracy results

mkdir -p $Root
rm $Log # Remove log if already exists
touch $Log
while [ $Noise -le 40 ]
do
	Acc=`$Gem5 $Script --noise ${Noise}e-1 | grep 'accu' | cut -d ' ' -f 2`
	cp m5out/stats.txt $Root/stats_${Noise}e-1.txt
	echo $Noise $Acc >> $Log
	Noise=`expr $Noise + 2`
done;
echo "finished"
exit 0
