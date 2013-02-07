#include <algorithm>
#include <math.h>
#include <iomanip>
#include <ff/farm.hpp>
#include <fstream>

#include "fastflow_farming.h"
#include "grid_hash_set.h"

namespace prtcl {

//
// Emitter
//

ParticlesEmitter::ParticlesEmitter(particle_t* particles,
		particle_t* particles_next, size_t num_particles, GridHashSet* grid) :
		m_num_particles(num_particles), m_grid(grid), m_template_task(
				new ParticlesTask()) {
	m_template_task->begin = 0;
	m_template_task->end = 0;
	m_template_task->particles = particles;
	m_template_task->particles_next = particles_next;
	m_template_task->grid = m_grid;

	m_block_size = cache_line_size();
}

ParticlesEmitter::~ParticlesEmitter() {
	delete m_template_task;
}

void* ParticlesEmitter::svc(void*) {
	size_t end = 0;

	while (end < m_num_particles) {
		ParticlesTask* t = new ParticlesTask(*m_template_task);
		t->begin = end;
		end += std::min(m_block_size, (m_num_particles - end));
		t->end = end;

		// Push the task to the emitter's output buffer, which is built upon a
		// SWSR lock-free (wait-free) (un)bounded FIFO queue.
		ff_send_out(t);
	}

	std::swap(m_template_task->particles, m_template_task->particles_next);

	return EOS ;
}

//
// Worker
//

void* SimulatorWorker::svc(void* task) {
	ParticlesTask* t = static_cast<ParticlesTask*>(task);

	for (size_t i = t->begin; i < t->end; ++i) {
		t->particles[i].ax = t->particles[i].ay = 0;

		// Iterate over all neighbors in the surrounding of current particle.
		// This should be constant w.r.t. n.
		prtcl::GridHashSet::surr_iterator neighbors_it;
		prtcl::GridHashSet::surr_iterator neighbors_it_end = t->grid->surr_end(
				t->particles[i]);

		for (neighbors_it = t->grid->surr_begin(t->particles[i]);
				neighbors_it != neighbors_it_end; ++neighbors_it) {
			apply_force(t->particles[i], **neighbors_it);
		}

		t->particles_next[i] = t->particles[i];
		move(t->particles_next[i]);
	}

	return t;
}

//
// Collector
//

ParticlesCollector::ParticlesCollector(char* savename, int n, int* step, int f) :
		m_outfile(savename), m_line_length(
				2 * (ParticlesCollector::COORDINATE_PRECISION + 6) + 2), m_n(n), m_step(
				step), m_frequency(f) {
	m_outfile << n << " " << size << std::endl;
	m_header_offset = m_outfile.tellp();

	m_outfile << std::setiosflags(std::ios::scientific);
	m_outfile << std::setprecision(COORDINATE_PRECISION);
}

ParticlesCollector::~ParticlesCollector() {
	m_outfile.close();
}

void* ParticlesCollector::svc(void* task) {
	ParticlesTask* t = static_cast<ParticlesTask*>(task);

	if (*m_step % m_frequency == 0) {
		int begin_offset = m_header_offset
				+ (*m_step / m_frequency) * m_n * m_line_length
				+ t->begin * m_line_length;
		m_outfile.seekp(begin_offset);

		for (size_t i = t->begin; i < t->end; ++i) {
			m_outfile << std::setw(COORDINATE_PRECISION)
					<< t->particles_next[i].x;
			m_outfile << " ";
			m_outfile << std::setw(COORDINATE_PRECISION)
					<< t->particles_next[i].y;
			m_outfile << std::endl;
		}
	}

	delete t;
	return GO_ON ;
}

}

