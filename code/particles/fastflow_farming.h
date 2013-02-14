#ifndef FASTFLOW_FARMING_H_
#define FASTFLOW_FARMING_H_

#include <boost/dynamic_bitset.hpp>
#include <ff/node.hpp>
#include <fstream>

#include "common.h"

namespace prtcl {

struct ParticlesTask {
	/**
	 * Task tag.
	 */
	size_t tag;

	/**
	 * Simulation step index.
	 */
	size_t step;

	/**
	 * Specifies the interval of this task: [begin, end);
	 */
	size_t begin, end;

	/**
	 * Pointer to the particles to operate on.
	 */
	particle_t* particles;

	/**
	 * Pointer to the particles that will be used in next step.
	 */
	particle_t* particles_next;

	/**
	 * A grid hash set representation of the particles.
	 */
	GridHashSet* grid;
};

class ParticlesEmitter: public ff::ff_node {
	typedef std::vector<ParticlesTask*> TaskVector;

	size_t m_num_particles;
	GridHashSet* m_grid;
	ParticlesTask* m_template_task;
	size_t m_block_size;
	size_t m_num_steps;
	size_t m_step_counter;
	size_t m_tag_counter;
	boost::dynamic_bitset<> m_tasks_awaiting;
	TaskVector m_tasks;

public:
	ParticlesEmitter(particle_t*, particle_t*, size_t, int, GridHashSet*);
	virtual ~ParticlesEmitter();
	virtual int svc_init();
	virtual void* svc(void*);
};

class SimulatorWorker: public ff::ff_node {
public:
	virtual void* svc(void*);
};

class ParticlesCollector: public ff::ff_node {
	static const int COORDINATE_PRECISION = 8;

	std::ofstream m_outfile;
	long m_header_offset;
	int m_line_length;

	int m_n;
	int m_frequency;

public:
	ParticlesCollector(char* savename, int n, int f);
	virtual ~ParticlesCollector();

	virtual void* svc(void*);
};

}

#endif /* FASTFLOW_FARMING_H_ */
