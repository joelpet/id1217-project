#include "grid_hash_set.h"
#include <cassert>
#include <cmath>
#include <deque>
#include <iostream>
#include <vector>

namespace prtcl {

/**
 * Constructs a new grid hash set with the given number of particles and
 * the specified size.
 */
GridHashSet::GridHashSet(int n, double size, double cutoff_radius) :
		num_rows(ceil(sqrt(n))), num_cols(ceil(sqrt(n))), grid_size(
				size / num_rows), num_surrounding_grids(
				ceil(cutoff_radius / grid_size)) {

	grids = new std::vector<std::deque<particle_t *>*>(num_rows * num_cols);

	std::vector<std::deque<particle_t *>*>::iterator it;
	for (it = grids->begin(); it != grids->end(); ++it) {
		*it = new std::deque<particle_t *>();
	}
}

/**
 * Deconstructs this grid hash set.
 */
GridHashSet::~GridHashSet() {

	std::vector<std::deque<particle_t *>*>::iterator it;
	for (it = grids->begin(); it != grids->end(); ++it) {
		delete *it;
	}

	delete grids;
}

/**
 * Removes all particle pointers from this grid hash set.
 */
void GridHashSet::clear() {
	std::vector<std::deque<particle_t *>*>::iterator it;
	for (it = grids->begin(); it != grids->end(); ++it) {
		(*it)->clear();
	}
}

/**
 * Inserts pointer to the given particle p into this grid hash set.
 */
void GridHashSet::insert(particle_t & p) {
	(*grids)[get_index(p)]->push_back(&p);
}

/**
 * Returns an iterator to the beginning of the sequence of particles that
 * the given particle belongs to, including itself.
 */
std::deque<particle_t *>::iterator GridHashSet::grid_particles_begin(
		particle_t & p) {
	return (*grids)[get_index(p)]->begin();
}

std::vector<std::deque<particle_t *>*>::iterator GridHashSet::grids_begin() {
	return grids->begin();
}

std::vector<std::deque<particle_t *>*>::iterator GridHashSet::grids_end() {
	return grids->end();
}

GridHashSet::surr_iterator GridHashSet::surr_begin(particle_t & p) {
	return surr_iterator(this, p);
}

GridHashSet::surr_iterator GridHashSet::surr_end(particle_t & p) {
	return surr_iterator(this, p).ff_to_end();
}

/**
 * Stores the top left surrounding row and column of p in tlr and tlc.
 */
void GridHashSet::first_surr_index(int& tlr, int& tlc, particle_t& p) {
	tlr = max(get_row(p) - num_surrounding_grids, 0);
	tlc = max(get_col(p) - num_surrounding_grids, 0);
}

void GridHashSet::last_surr_index(int& brr, int& brc, particle_t& p) {
	brr = min(get_row(p) + num_surrounding_grids, num_rows - 1);
	brc = min(get_col(p) + num_surrounding_grids, num_cols - 1);
}

int GridHashSet::get_index(int row, int col) {
	return row * num_cols + col;
}

/**
 * Returns the internal vector index where p should reside.
 */
int GridHashSet::get_index(particle_t & p) {
	return get_index(get_row(p), get_col(p));
}

/**
 * Returns the row index where p should reside.
 */
int GridHashSet::get_row(particle_t & p) {
	int row = static_cast<int>(p.y / grid_size);
	assert(row < num_rows);
	return row;
}

/**
 * Returns the column index where p should reside.
 */
int GridHashSet::get_col(particle_t & p) {
	int col = static_cast<int>(p.x / grid_size);
	assert(col < num_cols);
	return col;
}

/*
 * Surrounding iterator.
 */

/**
 * Constructs an empty (useless) surrounding iterator.
 */
GridHashSet::surr_iterator::surr_iterator() {
}

/**
 * Constructs a surroudning iterator and initializes row and col bounds.
 */
GridHashSet::surr_iterator::surr_iterator(GridHashSet* grid, particle_t& p,
		bool end) :
		grid(grid), reached_end(end) {

	// Store the start and end surrounding indices.
	grid->first_surr_index(top_left_row, top_left_col, p);
	grid->last_surr_index(bottom_right_row, bottom_right_col, p);

	// Initialize internal outer "iterator".
	r = top_left_row;
	c = top_left_col;

	// Initialize the deque particle iterator.
	particles_it = grid->grids->at(grid->get_index(r, c))->begin();
	particles_it_end = grid->grids->at(grid->get_index(r, c))->end();

	advance_outer_iterator();
}

/**
 * Advances the outer iterator one.
 *
 * If the end is reached during this operation, the corresponding flag is set.
 */
GridHashSet::surr_iterator::~surr_iterator() {
}

/**
 * Prefix ++ operator.
 *
 * Guarantees that this iterator will never be in a state where it will
 * reference a deque iterator that is exhausted.
 */
GridHashSet::surr_iterator & GridHashSet::surr_iterator::operator++() {
	// Increment the inner dequeu (particles) iterator.
	++particles_it;

	advance_outer_iterator();

	return *this;
}

GridHashSet::surr_iterator::pointer GridHashSet::surr_iterator::operator*() const {
	// Dereference the internal current deque<particle_t*> iterator.
	return *particles_it;
}

bool GridHashSet::surr_iterator::operator==(surr_iterator const & other) {
	// Either both have reached end or r, c and particles iterator match.
	return reached_end == other.reached_end
			&& (reached_end
					|| (r == other.r && c == other.c
							&& particles_it == other.particles_it));
}

bool GridHashSet::surr_iterator::operator!=(surr_iterator const & other) {
	return !((*this) == other);
}

/**
 * Fast forwards this iterator to the end and returns a reference to itself.
 */
void GridHashSet::surr_iterator::advance_outer_iterator() {
	// While inner deque iterator points nowhere useful.
	while ((particles_it == particles_it_end || *particles_it == NULL)
			&& !reached_end) {
		// Move to next grid.
		if (++c > bottom_right_col) {
			c = top_left_col;
			++r;
		}

		if (r > bottom_right_row) {
			reached_end = true;
			return;
		} else {
			// Update internal deque (particles) iterators.
			particles_it = grid->grids->at(grid->get_index(r, c))->begin();
			particles_it_end = grid->grids->at(grid->get_index(r, c))->end();
		}
	}
}

GridHashSet::surr_iterator& GridHashSet::surr_iterator::ff_to_end() {
	// Go one step further than the last grid.
	r = bottom_right_row + 1;
	c = top_left_col;

	// Get the deque (particle) iterator from last grid.
	reached_end = true;

	return (*this);
}

}

