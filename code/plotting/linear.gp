# Gnuplot script for plotting linear "proof"
#
# To be run with cwd set to the same dir as this file resides in.
#

# LaTeX
#set terminal eepic color
#set output "../../report/plots/linear.tex"

# PDF
set terminal pdfcairo

# Misc settings
#set log xy
set key left top
set grid xtics ytics
set xlabel "Number of particles"
set ylabel "Time (s)"


plot \
"../../benchmarks/linear/pthreads.dat" title "Pthreads",\
"../../benchmarks/linear/openmp.dat" title "OpenMP",\
"../../benchmarks/linear/fastflow.dat" title "Fastflow",\
"../../benchmarks/linear/serial.dat" title "Serial"

#"../../benchmarks/linear/mpi.dat" title "MPI",\
