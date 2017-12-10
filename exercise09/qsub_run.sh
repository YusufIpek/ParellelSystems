#!/bin/bash

# Execute job in the queue "std.q" unless you have special requirements.
#$ -q std.q

# The batch system should use the current directory as working directory.
#$ -cwd



# Redirect output stream to this file.
#$ -o output.dat

# Join the error stream to the output stream.
#$ -e error.dat


# Use the parallel environment "openmpi-fillup", which assigns as many processes
# as available on each host. Start 16 MPI processes across an arbitrary number of
# hosts. For each process, SGE will reserve one CPU-core.
#$ -pe openmpi-8perhost 8 

./run_single_node 

#$ -pe openmpi-fillup 16
./run_multiple_nodes 16

#$ -pe openmpi-fillup 32
./run_multiple_nodes 32

#$ -pe openmpi-fillup 64
./run_multiple_nodes 64



