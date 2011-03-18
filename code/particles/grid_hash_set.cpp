#include "grid_hash_set.h"
#include <cassert>
#include <cmath>
#include <deque>
#include <iostream>
#include <vector>

namespace particles {

    /**
     * Constructs a new grid hash set with the given number of particles and
     * the specified size.
     *
     * TODO What's worse; num_grids > n OR average_particle_count > 1 ?
     * Change initialization of num_rows, num_cols accordingly.
     */
    GridHashSet::GridHashSet(int n, double size) : num_rows(ceil(sqrt(n))),
        num_cols(ceil(sqrt(n))), grid_size(size / num_rows) { 

        grids = new std::vector< std::deque<particle_t *> >(num_rows * num_cols);

        std::cout << "Rows: " << num_rows << std::endl;
        std::cout << "Cols: " << num_cols << std::endl;
        std::cout << "Grids: " << grids->size() << std::endl;
        std::cout << "Grid size: " << grid_size << std::endl;

    }

    /**
     * Deconstructs this grid hash set.
     */
    GridHashSet::~GridHashSet() {
        delete grids;
    }

    /**
     * Inserts the given particle p into this grid hash set.
     */
    void GridHashSet::insert(particle_t & p) {
        std::cout << "Inserting particle at (" << p.x << ", " << p.y;
        std::cout << ") into row " << get_row(p) << " and column ";
        std::cout << get_col(p) << "." << std::endl;

        (*grids)[get_row(p) * num_cols + get_col(p)].push_back(&p);
    }

    /**
     * Returns the row index where p should reside.
     */
    unsigned int GridHashSet::get_row(particle_t & p) {
        assert(floor(p.y / grid_size) < num_rows);
        return floor(p.y / grid_size);
    }

    /**
     * Returns the column index where p should reside.
     */
    unsigned int GridHashSet::get_col(particle_t & p) {
        assert(floor(p.x / grid_size) < num_cols);
        return floor(p.x / grid_size);
    }

}

