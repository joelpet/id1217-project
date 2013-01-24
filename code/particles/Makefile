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
CFLAGS=-g

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
serial_srcs=common.cpp serial.cpp grid_hash_set.cpp
pthreads_srcs=common.cpp pthreads.cpp grid_hash_set.cpp
openmp_srcs=common.cpp openmp.cpp grid_hash_set.cpp
mpi_srcs=common.cpp mpi.cpp grid_hash_set.cpp
fastflow_srcs =	common.cpp fastflow.cpp grid_hash_set.cpp fastflow_farming.cpp

# Object file names derived from sources specified above.
serial_objs=$(addprefix $(bin_dir)/,$(serial_srcs:.cpp=.o))
pthreads_objs=$(addprefix $(bin_dir)/,$(pthreads_srcs:.cpp=.o))
openmp_objs=$(addprefix $(bin_dir)/,$(openmp_srcs:.cpp=.o))
mpi_objs=$(addprefix $(bin_dir)/,$(mpi_srcs:.cpp=.o))
fastflow_objs=$(addprefix $(bin_dir)/,$(fastflow_srcs:.cpp=.o))

# Executables
executables = serial pthreads openmp mpi fastflow

##################################################
## Ordinary targets
## ================

# Targets that does not generate a file named same as the target itself.
.PHONY: all clean 

all: $(executables)
clean:
	rm -f $(bin_dir)/* $(executables)
$(bin_dir):
	-mkdir $@

serial: $(bin_dir) $(serial_objs)
	$(CXX) $(LDFLAGS) $(filter %.o, $^) -o $@
pthreads: $(bin_dir) $(pthreads_objs)
	$(CXX) $(LDFLAGS) $(filter %.o, $^) -o $@ -lpthread
openmp: $(bin_dir) $(openmp_objs)
	$(CXX) $(LDFLAGS) $(filter %.o, $^) -o $@ -fopenmp
mpi: $(bin_dir) $(mpi_objs)
	$(MPCC) $(LDFLAGS) $(filter %.o, $^) -o $@
fastflow: $(bin_dir) $(fastflow_objs)
	$(CXX) $(LDFLAGS) $(filter %.o, $^) -o $@ -lpthread

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

