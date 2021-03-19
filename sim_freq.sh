#!/bin/bash
# Program:
#     CPU frequency simulation
Target=Frequency_out       
Unit=MHz
Root=/home/lhh/gem5-stable/"$Target"     # Target dictionary for stats.txt
Gem5=/home/lhh/gem5-stable/build/ARM/gem5.opt # gem5 dictionary
Script=/home/lhh/gem5-stable/configs/example/se.py  # config script dictionary 
Bench=queens                                 # benchmark name
i=1000 # frequency: MHz
CPUtype=timing 

mkdir $Root
while [ $i != 3000 ]                    # Max frequency: 3GHz = 3000MHz
do
    # Add your code here for gem5 simulation
done;
echo "finished"
exit 0