##################################################
## General configuration
## =====================

# Every Makefile should contain this line:
SHELL=/bin/sh

# Program for compiling C++ programs; default `g++'. 
CXX=g++

# Program for compiling and linking MPI programs written in C++.
MPCC=mpicxx

# Extra flags to give to the C preprocessor and programs that use it (the C and Fortran compilers). 
CFLAGS=

# Extra flags to give to the C++ compiler (both at compiling and linking)
# Notice: `make CXXFLAGS='-g -O' will replace the value of CXXFLAGS given below
CXXFLAGS=-Wall -Wextra

# Add any custom flags before $(CXXFLAGS) below, that are required for this build
all_cxxflags=$(CXXFLAGS)

# Extra flags to give to compilers when they are supposed to invoke the linker, `ld'. 
LDFLAGS=

##################################################
## Setup files variables
## =====================

# Setup binary files directory
bin_dir=bin

# Source files to compile and link together for the different programs.
serial_srcs=common.cpp serial.cpp
pthreads_srcs=common.cpp pthreads.cpp
openmp_srcs=common.cpp openmp.cpp
mpi_srcs=common.cpp mpi.cpp

# Object file names derived from sources specified above.
serial_objs=$(addprefix $(bin_dir)/,$(serial_srcs:.cpp=.o))
pthreads_objs=$(addprefix $(bin_dir)/,$(pthreads_srcs:.cpp=.o))
openmp_objs=$(addprefix $(bin_dir)/,$(openmp_srcs:.cpp=.o))
mpi_objs=$(addprefix $(bin_dir)/,$(mpi_srcs:.cpp=.o))


##################################################
## Ordinary targets
## ================

# Targets that does not generate a file named same as the target itself.
.PHONY: all clean 

all: serial pthreads openmp mpi
clean:
	rm -f $(bin_dir)/* serial pthreads openmp mpi

# TODO Create a multiple-target recipe for these targets.
serial: $(serial_objs) 
	-mkdir $(bin_dir)
	$(CXX) $(LDFLAGS) $^ -o $@		
pthreads: $(pthreads_objs)
	-mkdir $(bin_dir)
	$(CXX) $(LDFLAGS) $^ -o $@ -lpthread
openmp: $(openmp_objs)
	-mkdir $(bin_dir)
	$(CXX) $(LDFLAGS) $^ -o $@ -fopenmp
mpi: $(mpi_objs)
	-mkdir $(bin_dir)
	$(MPCC) $(LDFLAGS) $^ -o $@

# TODO Create a multiple-target recipe for these targets.
$(bin_dir)/mpi.o: mpi.cpp
	$(MPCC) -c $(CFLAGS) $(all_cxxflags) $< -o $@
$(bin_dir)/openmp.o: openmp.cpp
	$(CXX) -c $(CFLAGS) $(all_cxxflags) $< -o $@ -fopenmp
$(bin_dir)/%.o: %.cpp
	$(CXX) -c $(CFLAGS) $(all_cxxflags) $< -o $@



##################################################
## Notes
## =====

# $@ expands to the file name of the target

