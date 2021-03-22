#!/bin/bash
# Program:
#     CPU frequency simulation
Target=Frequency_out
Unit=MHz
Root=$HOME/gem5/"$Target" # Target dictionary for stats.txt
Gem5=$HOME/gem5/build/ARM/gem5.debug # gem5 dictionary
Script=$HOME/gem5/configs/example/se.py # config script dictionary 
Bench=$HOME/gem5/tests/test-progs/queens/queens # benchmark path
i=1000 # frequency: MHz
CPUtype=MinorCPU
Log=$Root/log_$CPUtype # Log file for all sim time results

mkdir -p $Root
rm $Log # Remove log if already exists
touch $Log
while [ $i -le 3000 ]                    # Max frequency: 3GHz = 3000MHz
do
	$Gem5 $Script \
		--num-cpus=1 \
		--caches \
		--l2cache \
		--l1d_size=8kB \
		--l1i_size=8kB \
		--l2_size=1MB \
		--l1d_assoc=4 \
		--l1i_assoc=4 \
		--l2_assoc=8 \
		--cacheline_size=64 \
		--cpu-type=$CPUtype \
		--cpu-clock=$i$Unit \
		--cmd=$Bench
	cp m5out/stats.txt $Root/stats_${CPUtype}_$i$Unit.txt
	SimSeconds=`grep 'sim_seconds' m5out/stats.txt | tr -s ' ' | cut -d ' ' -f 2`
	echo $i $SimSeconds >> $Log
	i=`expr $i + 100`
done;
echo "finished"
exit 0
