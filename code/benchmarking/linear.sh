#!/bin/bash
#
# Runs the given particle simulation implementation and output
# its running times for increasing number of particles.
#

function print_usage {
    echo "Usage: ${0} <executable> [<first> <increment> <last>)]"
    exit 1
}

if [ $# -gt 0 ]; then
    executable="$1"
    if [ $# -eq 1 ]; then
        first=1000
        last=10000
        increment=1000
    elif [ $# -eq 4 ]; then
        first=$2
        last=$3
        increment=$4
    else
        print_usage
    fi
else
    print_usage
fi

sequence=`seq --separator=' ' $first $increment $last`

echo "# Benchmarks for $executable with n = [$sequence]"
echo "# n     time (s)"

for n in $sequence; do
    printf "%5d\t" $n

    for j in `seq 1 5`; do
        sim_time[$j]=$($executable -n $n | grep -o \
            "simulation time = [0-9.]* seconds" | awk '{print $4}')
    done

    median=$(echo ${sim_time[1]} ${sim_time[2]} ${sim_time[3]} ${sim_time[4]} \
        ${sim_time[5]} | tr " " "\n" | sort | head -n 3 | tail -n 1)

    printf "%f\n" $median
done
