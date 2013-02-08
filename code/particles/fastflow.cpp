#include <assert.h>
#include <algorithm>
#include <ff/farm.hpp>
#include <ff/node.hpp>
#include <ff/pipeline.hpp>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "fastflow_farming.h"
#include "grid_hash_set.h"

//
//  benchmarking program
//
int main(int argc, char **argv) {
	if (find_option(argc, argv, "-h") >= 0) {
		printf("Options:\n");
		printf("-h to see this help\n");
		printf("-n <int> to set the number of particles\n");
		printf("-s <int> to set the number of steps in the simulation\n");
		printf("-o <filename> to specify the output file name\n");
		printf(
				"-f <int> to set the frequency of saving particle coordinates (e.g. each ten's step)");
		printf("-p <int> to set the number of processors to employ");
		return 0;
	}

	int n = read_int(argc, argv, "-n", 1000);
	int s = read_int(argc, argv, "-s", NSTEPS);
	int f = read_int(argc, argv, "-f", SAVEFREQ);
	char *savename = read_string(argc, argv, "-o", NULL);
	size_t p = read_int(argc, argv, "-p", ff_numCores());
	size_t num_workers = max(1, p - (savename ? 2 : 1));

	particle_t* particles = new particle_t[n];
	particle_t* particles_next = new particle_t[n];

	set_size(n);
	init_particles(n, particles);
	int step;

	prtcl::GridHashSet grid(n, size, cutoff);

	double simulation_time = read_timer();

	ff::ff_farm<> simulator_farm;
	prtcl::ParticlesEmitter emitter(particles, particles_next, n, &grid);
	std::vector<ff::ff_node*> workers;
	prtcl::ParticlesCollector* collector = NULL;

	for (size_t i = 0; i < num_workers; ++i) {
		workers.push_back(new prtcl::SimulatorWorker());
	}

	simulator_farm.add_emitter(&emitter);
	simulator_farm.add_workers(workers);

	if (savename) {
		collector = new prtcl::ParticlesCollector(savename, n, &step, f);
		simulator_farm.add_collector(collector);
	}

	for (step = 0; step < s; ++step) {
		grid.clear();
		insert_into_grid(n, particles, &grid);

		if (simulator_farm.run_and_wait_end() < 0) {
			ff::error("Something went wrong when computing forces.\n");
			return -1;
		}

		std::swap(particles, particles_next);
	}

	simulation_time = read_timer() - simulation_time;

	printf(
			"n = %d, steps = %d, savefreq = %d, simulation time = %g seconds, num_workers = %d\n",
			n, s, f, simulation_time, simulator_farm.getNWorkers());

	if (collector) {
		delete collector;
	}

	delete[] particles;
	delete[] particles_next;

	return 0;
}
