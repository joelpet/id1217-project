#ifndef PARTICLES_GRID_HASH_SET_H
#define PARTICLES_GRID_HASH_SET_H

#include <deque>
#include <vector>
#include "common.h"

namespace prtcl {
    class GridHashSet {
        public:
            friend class surr_iterator;

            class surr_iterator : std::iterator_traits<particle_t *> {
                friend class GridHashSet;

                public:
                    surr_iterator();
                    surr_iterator(GridHashSet *, particle_t &, bool end = false);
                    //surr_iterator & operator=(const surr_iterator &); // Assignment constructor
                    //surr_iterator(const surr_iterator &); // Copy-constructor
                    ~surr_iterator();
                    surr_iterator & operator++();
                    pointer operator*() const;
                    bool operator==(surr_iterator const &);
                    bool operator!=(surr_iterator const &);
                private:
                    GridHashSet * grid;
                    bool reached_end;
                    int r, c;
                    int top_left_row, top_left_col, bottom_right_row, bottom_right_col;
                    std::deque<particle_t *>::iterator particles_it;
                    std::deque<particle_t *>::iterator particles_it_end;

                    surr_iterator& ff_to_end();
            };

            GridHashSet(int n, double size, double cutoff_radius);
            ~GridHashSet();
            //particle_t& operator() (unsigned row, unsigned col);
            //particle_t const& operator() (unsigned row, unsigned col) const;
            void clear();
            void insert(particle_t &);
            std::deque<particle_t *>::iterator grid_particles_begin(particle_t &);
            std::vector<std::deque<particle_t *>*>::iterator grids_begin();
            std::vector<std::deque<particle_t *>*>::iterator grids_end();
            surr_iterator surr_begin(particle_t &);
            surr_iterator surr_end(particle_t &);
            int get_row(particle_t &);
            int get_col(particle_t &);

        private:
            int num_rows;
            int num_cols;
            double grid_size;
            int num_surrounding_grids;
            std::vector<std::deque<particle_t *>*>* grids;

            void first_surr_index(int&, int&, particle_t&);
            void last_surr_index(int&, int&, particle_t&);
            int get_index(int row, int col);
            int get_index(particle_t &);

    };
}

#endif
