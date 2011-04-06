# Gnuplot script for plotting linear "proof"

set log xy
plot "../../benchmarks/linear/serial.dat" title "Serial", "../../benchmarks/linear/pthreads.dat" title "Pthreads"
