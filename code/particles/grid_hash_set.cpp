#include "grid_hash_set.h"
#include <cmath>
#include <deque>
#include <iostream>
#include <vector>

namespace particles {

    /**
     * Constructs a new grid hash set with the specified size.
     *
     * TODO What's worse; num_grids > n OR average_particle_count > 1 ?
     */
    GridHashSet::GridHashSet(int n) : num_rows(ceil(sqrt(n))), num_cols(ceil(sqrt(n))) { 
        // Resize the vector to contain exactly our number of grids.
        grids.resize(num_rows * num_cols);

        std::cout << "Rows: " << num_rows << std::endl;
        std::cout << "Cols: " << num_cols << std::endl;
        std::cout << "Grids: " << grids.size() << std::endl;

        // Initialize all deques. 
        //std::vector< std::deque<particle_t *> >::iterator it;
        //for (it = grids.begin(); it < grids.end(); it++) {
            //it();
        //}
    }

    GridHashSet::~GridHashSet() {
        // TODO free resources
    }

    void GridHashSet::insert(particle_t & p) {
        std::cout << "Inserting particle at (" << p.x << ", " << p.y << ")" << std::endl;

        grids[get_row(p) * num_cols + get_col(p)].push_back(&p);
    }

    unsigned int GridHashSet::get_row(particle_t & p) {
        // TODO implement
        return 0;
    }

    unsigned int GridHashSet::get_col(particle_t & p) {
        // TODO implement
        return 0;
    }

}

