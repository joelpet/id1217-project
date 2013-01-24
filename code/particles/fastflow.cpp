#include <assert.h>
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
		return 0;
	}

	int n = read_int(argc, argv, "-n", 1000);
	int s = read_int(argc, argv, "-s", NSTEPS);
	int f = read_int(argc, argv, "-f", SAVEFREQ);

	char *savename = read_string(argc, argv, "-o", NULL);
	FILE *fsave = savename ? fopen(savename, "w") : NULL;

	particle_t *particles = (particle_t*) malloc(n * sizeof(particle_t));
	set_size(n);
	init_particles(n, particles);
	size_t num_cores = ff_numCores();

	prtcl::GridHashSet* grid = new prtcl::GridHashSet(n, size, cutoff);

	double simulation_time = read_timer();

	for (int step = 0; step < s; ++step) {
		grid->clear();
		insert_into_grid(n, particles, grid);

		// TODO init everything outside for and just run_and_wait_end() inside
		ff::ff_farm<> force_farm;

		ff::ff_node* emitter = new prtcl::ParticlesEmitter(particles, n, *grid);
		std::vector<ff::ff_node*> workers;

		for (size_t i = 0; i < num_cores; ++i) {
			workers.push_back(new prtcl::ComputeWorker());
		}

		force_farm.add_emitter(emitter);
		force_farm.add_workers(workers);

		if (force_farm.run_and_wait_end() < 0) {
			ff::error("Something went wrong when computing forces.");
			return -1;
		}

		ff::ff_farm<> move_farm;

		ff::ff_node* move_emitter = new prtcl::ParticlesEmitter(particles, n,
				*grid);
		std::vector<ff::ff_node*> move_workers;

		for (size_t i = 0; i < num_cores; ++i) {
			move_workers.push_back(new prtcl::MoveWorker());
		}

		move_farm.add_emitter(move_emitter);
		move_farm.add_workers(move_workers);

		if (move_farm.run_and_wait_end() < 0) {
			ff::error("Something went wrong when moving particles.");
			return -1;
		}

		//  save if necessary
		if (fsave && (step % f) == 0) {
			save(fsave, n, particles);
		}
	}

	simulation_time = read_timer() - simulation_time;

	printf("n = %d, steps = %d, savefreq = %d, simulation time = %g seconds\n",
			n, s, f, simulation_time);

	delete grid;
	free(particles);
	if (fsave)
		fclose(fsave);

	return 0;
}
