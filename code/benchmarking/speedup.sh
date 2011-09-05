#!/bin/bash
#
# Runs the given particle simulation implementations and output its running
# times for increasing number of worker threads/processes/processors.
#

function print_usage {
    echo "Usage: ${0} <executable> [<n_proc_first> <n_proc_last>]"
    exit 1
}

if [ $# -gt 0 ]; then
    executable="$1"
    if [ $# -eq 1 ]; then
        n_proc_first=1
        n_proc_last=8
    elif [ $# -eq 3 ]; then
        n_proc_first=$2
        n_proc_last=$3
    else
        print_usage
    fi
else
    print_usage
fi

num_particles=10000

echo "# Benchmarks for $executable with num_particles = $num_particles, and n_proc = [$n_proc_first ... $n_proc_last]"
echo "# n_proc  time (s)"


for num_proc in `seq $n_proc_first $n_proc_last`; do
    printf "%3d\t\t" $num_proc

    for j in `seq 1 5`; do
        sim_time[$j]=$($executable -n $num_particles -p $num_proc | grep -o "simulation time = [0-9.]* seconds" | awk '{print $4}')
    done

    median=$(echo ${sim_time[1]} ${sim_time[2]} ${sim_time[3]} ${sim_time[4]} ${sim_time[5]} | tr " " "\n" | sort | head -n 3 | tail -n 1)

    printf "%f\n" $median
done

