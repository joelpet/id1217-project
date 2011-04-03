#!/bin/bash
#
# Runs the serial, pthreads, openmp and mpi implementations and output their
# running times for increasing number of particles.
#

echo "# Benchmarks for ./serial"
echo "# n     time (s)"

for i in `seq 0 8`; do
    let n=100*2**$i
    for j in `seq 1 5`; do
        sim_time[$j]=$(../particles/serial -n $n | grep -o "simulation time = [0-9.]* seconds" | awk '{print $4}')
    done

    median=$(echo ${sim_time[1]} ${sim_time[2]} ${sim_time[3]} ${sim_time[4]} ${sim_time[5]} | tr " " "\n" | sort | head -n 3 | tail -n 1)

    printf "%5d\t%f\n" $n $median

done
