#!/bin/bash
#
# Runs the serial, pthreads, openmp and mpi implementations and output their
# running times for increasing number of particles.
#

function print_usage {
    echo "Usage: ${0} <executable> [<n_first_exp> <n_last_exp>]"
    exit 1
}

if [ $# -gt 1 ]; then
    executable="$1"
    if [ $# -eq 1 ]; then
        n_first_exp=0
        n_last_exp=8
    elif [ $# -eq 3 ]; then
        n_first_exp=$2
        n_last_exp=$3
    else
        print_usage
    fi
else
    print_usage
fi

let n_first=100*2**$n_first_exp
let n_last=100*2**$n_last_exp
echo "# Benchmarks for $executable with n = [$n_first ... $n_last]"
echo "# n     time (s)"

for i in `seq $n_first_exp $n_last_exp`; do
    let n=100*2**$i
    printf "%5d\t" $n

    for j in `seq 1 5`; do
        sim_time[$j]=$($executable -n $n | grep -o "simulation time = [0-9.]* seconds" | awk '{print $4}')
    done

    median=$(echo ${sim_time[1]} ${sim_time[2]} ${sim_time[3]} ${sim_time[4]} ${sim_time[5]} | tr " " "\n" | sort | head -n 3 | tail -n 1)

    printf "%f\n" $median
done
