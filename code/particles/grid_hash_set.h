#ifndef PARTICLES_GRID_HASH_SET_H
#define PARTICLES_GRID_HASH_SET_H

#include "common.h"
#include <deque>
#include <vector>

namespace particles {
    class GridHashSet {
        public:
            GridHashSet(int n);
            ~GridHashSet();
            //particle_t& operator() (unsigned i, unsigned j);
            //particle_t const& operator() (unsigned i, unsigned j) const;
            void insert(particle_t &);
            void test();

        private:
            const int num_rows, num_cols;
            std::vector< std::deque<const particle_t *> > grids;
            unsigned int get_row(particle_t &);
            unsigned int get_col(particle_t &);

    };
}

#endif
