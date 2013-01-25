#ifndef FASTFLOW_FARMING_H_
#define FASTFLOW_FARMING_H_

#include <ff/node.hpp>

#include "common.h"

namespace prtcl {

struct ParticlesTask {
	/**
	 * Specifies the interval of this task: [begin, end);
	 */
	size_t begin, end;

	/**
	 * Pointer to the particles to operate on.
	 */
	particle_t* particles;

	/**
	 * A grid hash set representation of the particles.
	 */
	GridHashSet* grid;
};

class ParticlesEmitter: public ff::ff_node {
	size_t m_num_particles;
	GridHashSet* m_grid;
	ParticlesTask* m_template_task;
	size_t m_block_size;

public:
	ParticlesEmitter(particle_t*, size_t, GridHashSet*);
	virtual ~ParticlesEmitter();
	virtual void* svc(void*);
};

class ComputeWorker: public ff::ff_node {
public:
	virtual void* svc(void*);
};

class MoveWorker: public ff::ff_node {
public:
	virtual void* svc(void*);
};

}

#endif /* FASTFLOW_FARMING_H_ */
