# Gnuplot script for plotting linear "proof"

set log xy
set key left top

plot \
"../../benchmarks/linear/serial.dat" title "Serial",\
"../../benchmarks/linear/pthreads.dat" title "Pthreads",\
"../../benchmarks/linear/openmp.dat" title "OpenMP"
"../../benchmarks/linear/mpi.dat" title "MPI"
