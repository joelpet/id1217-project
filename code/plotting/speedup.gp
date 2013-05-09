# Gnuplot script for plotting speedup graph.
#
# To be run with cwd set to the same dir as this file resides in.
#

# LaTeX
#set terminal eepic color
#set output "../../report/plots/speedup.tex"

# PDF
set terminal pdfcairo

# Misc settings
set key right center
set grid xtics ytics
#set yrange [0:80]
set xlabel "Number of threads"
set ylabel "Time (s)"


plot \
"../../benchmarks/speedup/pthreads.dat" title "Pthreads",\
"../../benchmarks/speedup/openmp.dat" title "OpenMP",\
"../../benchmarks/speedup/fastflow.dat" title "Fastflow",\
"../../benchmarks/speedup/serial.dat" title "Serial"

#"../../benchmarks/speedup/mpi.dat" title "MPI"
