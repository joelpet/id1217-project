#include <assert.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "grid_hash_set.h"

//
//  benchmarking program
//
int main( int argc, char **argv )
{    
    if( find_option( argc, argv, "-h" ) >= 0 )
    {
        printf( "Options:\n" );
        printf( "-h to see this help\n" );
        printf( "-n <int> to set the number of particles\n" );
        printf( "-o <filename> to specify the output file name\n" );
        return 0;
    }
    
    int n = read_int( argc, argv, "-n", 1000 );

    char *savename = read_string( argc, argv, "-o", NULL );
    
    FILE *fsave = savename ? fopen( savename, "w" ) : NULL;
    particle_t *particles = (particle_t*) malloc( n * sizeof(particle_t) );
    set_size( n );
    init_particles( n, particles );

    particles::GridHashSet* grid = new particles::GridHashSet(n, size, cutoff);
    insert_into_grid(n, particles, grid);


    //
    //  simulate a number of time steps
    //
    double simulation_time = read_timer( );
    for( int step = 0; step < NSTEPS; step++ )
    {
        //
        //  compute forces
        //

        // Iterate over all grids in the hash grid set.
        std::vector<std::deque<particle_t *>*>::iterator grids_it;
        for (grids_it = grid->grids_begin(); grids_it != grid->grids_end(); ++grids_it) {
            //printf("for grid at %p:\n", &grids_it);

            // Iterate over all particles in the current grid. In general, only 1.
            std::deque<particle_t *>::iterator particles_it;
            for (particles_it = (*grids_it)->begin(); particles_it != (*grids_it)->end(); ++particles_it) {
                //printf("\tparticle x,y=(%f, %f), ax,ay=(%f, %f) in grid [%d][%d]\n", (*particles_it)->x, (*particles_it)->y, (*particles_it)->ax, (*particles_it)->ay, grid->get_row(**particles_it), grid->get_col(**particles_it));

                (*particles_it)->ax = 0;
                (*particles_it)->ay = 0;

                // Iterate over all neighbors in the surrounding of current particle.
                // This should be constant w.r.t. n.
                particles::GridHashSet::surr_iterator neighbors_it;
                for (neighbors_it = grid->surr_begin(**particles_it);
                        neighbors_it != grid->surr_end(**particles_it);
                        ++neighbors_it) { 
                    //printf("\t\tneighbor (%f, %f) in grid [%d][%d]\n", (*neighbors_it)->x, (*neighbors_it)->y, grid->get_row(**neighbors_it), grid->get_col(**neighbors_it));


                    apply_force(**particles_it, **neighbors_it);
                }

            }

        }

        //
        //  move particles
        //
        for( int i = 0; i < n; i++ ) {
            move( particles[i] );
        }

        // Update grid hash set.
        grid->clear();
        insert_into_grid(n, particles, grid);

        
        //
        //  save if necessary
        //
        if( fsave && (step%SAVEFREQ) == 0 )
            save( fsave, n, particles );
    }
    simulation_time = read_timer( ) - simulation_time;
    
    printf( "n = %d, simulation time = %g seconds\n", n, simulation_time );
    
    delete grid;
    free( particles );
    if( fsave )
        fclose( fsave );
    
    return 0;
}
