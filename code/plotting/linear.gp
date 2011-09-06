# Gnuplot script for plotting linear "proof"
#
# To be run with cwd set to the same dir as this file resides in.
#

# LaTeX
set terminal eepic color
set output "../../report/plots/linear.tex"

# Misc settings
set log xy
set key left top
set grid xtics ytics
set xlabel "Number of particles (n)"
set ylabel "Time in seconds"


plot \
"../../benchmarks/linear/serial.dat" title "Serial",\
"../../benchmarks/linear/pthreads.dat" title "Pthreads",\
"../../benchmarks/linear/openmp.dat" title "OpenMP",\
"../../benchmarks/linear/mpi.dat" title "MPI"
