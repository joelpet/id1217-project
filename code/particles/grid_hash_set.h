#ifndef PARTICLES_GRID_HASH_SET_H
#define PARTICLES_GRID_HASH_SET_H

#include <deque>
#include <vector>
#include "common.h"

namespace particles {
    class GridHashSet {
        public:
            GridHashSet(int n, double size);
            ~GridHashSet();
            //particle_t& operator() (unsigned i, unsigned j);
            //particle_t const& operator() (unsigned i, unsigned j) const;
            void insert(particle_t &);

        private:
            const int num_rows, num_cols;
            const double grid_size;
            std::vector< std::deque<particle_t *> > * grids;

            unsigned int get_row(particle_t &);
            unsigned int get_col(particle_t &);

    };
}

#endif
