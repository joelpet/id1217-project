#include <algorithm>
#include <ff/farm.hpp>

#include "fastflow_farming.h"
#include "grid_hash_set.h"

namespace prtcl {

//
// Particles emitter
//

ParticlesEmitter::ParticlesEmitter(particle_t* particles, size_t num_particles,
		GridHashSet* grid) :
		m_num_particles(num_particles), m_grid(grid), m_template_task(
				new ParticlesTask()) {
	m_template_task->begin = 0;
	m_template_task->end = 0;
	m_template_task->particles = particles;
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

	return EOS ;
}

//
// Force compute worker
//

void* ComputeWorker::svc(void* task) {
	ParticlesTask* t = static_cast<ParticlesTask*>(task);

	for (size_t i = t->begin; i < t->end; ++i) {
		t->particles[i].ax = t->particles[i].ay = 0;

		// Iterate over all neighbors in the surrounding of current particle.
		// This should be constant w.r.t. n.
		prtcl::GridHashSet::surr_iterator neighbors_it;

		for (neighbors_it = t->grid->surr_begin(t->particles[i]);
				neighbors_it != t->grid->surr_end(t->particles[i]);
				++neighbors_it) {
			apply_force(t->particles[i], **neighbors_it);
		}
	}

	delete t;

	return GO_ON ;
}

//
// Move particles worker
//

void* MoveWorker::svc(void* task) {
	ParticlesTask* t = static_cast<ParticlesTask*>(task);

	for (size_t i = t->begin; i < t->end; ++i) {
		move(t->particles[i]);
	}

	delete t;

	// I finished processing the current task, I give you no result to be
	// delivered onto the output stream, but please keep me alive ready to
	// receive another input task. (GO_ON)
	return GO_ON ;
}

}

