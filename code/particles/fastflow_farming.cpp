#include <algorithm>
#include <ff/farm.hpp>

#include "fastflow_farming.h"
#include "grid_hash_set.h"

namespace prtcl {

//
// Particles emitter
//

ParticlesEmitter::ParticlesEmitter(particle_t* particles, size_t num_particles,
		GridHashSet& grid) :
		m_num_particles(num_particles), m_grid(grid), m_template_task(
				new ParticlesTask()) {
	m_template_task->begin = 0;
	m_template_task->end = 0;
	m_template_task->particles = particles;
	m_template_task->grid = &m_grid;

	m_block_size = (num_particles + 1) / ff_numCores();
}

void* ParticlesEmitter::svc(void*) {
	size_t end = 0;

	while (end < m_num_particles) {
		ParticlesTask* t = new ParticlesTask(*m_template_task);
		t->begin = end;
		end += std::min(m_block_size, (m_num_particles - end));
		t->end = end;

		ff_send_out(t);
	}

	return EOS ;
}

//
// Force compute worker
//

void* ComputeWorker::svc(void* task) {
	ParticlesTask* t = (ParticlesTask*) task;

	for (size_t i = t->begin; i < t->end; ++i) {
		particle_t& particle = t->particles[i];
		particle.ax = particle.ay = 0;

		// Iterate over all neighbors in the surrounding of current particle.
		// This should be constant w.r.t. n.
		prtcl::GridHashSet::surr_iterator neighbors_it;

		for (neighbors_it = t->grid->surr_begin(particle);
				neighbors_it != t->grid->surr_end(particle); ++neighbors_it) {
			particle_t& other = **neighbors_it;
			apply_force(particle, other);
		}
	}

	return EOS ;
}

//
// Move particles worker
//

void* MoveWorker::svc(void* task) {
	ParticlesTask* t = (ParticlesTask*) task;

	for (size_t i = t->begin; i < t->end; ++i) {
		move(t->particles[i]);
	}

	return EOS ;
}

}

