#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
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
        printf( "-n <int> to set number of particles\n" );
        printf( "-o <filename> to specify the output file name\n" );
        return 0;
    }

    int n = read_int( argc, argv, "-n", 1000 );
    char *savename = read_string( argc, argv, "-o", NULL );

    FILE *fsave = savename ? fopen( savename, "w" ) : NULL;

    particle_t *particles = (particle_t*) malloc( n * sizeof(particle_t) );
    set_size( n );
    init_particles( n, particles );

    prtcl::GridHashSet* grid = new prtcl::GridHashSet(n, size, cutoff);
    insert_into_grid(n, particles, grid);

    //
    //  simulate a number of time steps
    //
    double simulation_time = read_timer( );

    #pragma omp parallel
    for( int step = 0; step < 1000; step++ )
    {
        //
        //  compute all forces
        //
        #pragma omp for
        for( int i = 0; i < n; i++ )
        {
            particles[i].ax = particles[i].ay = 0;

            // Iterate over all neighbors in the surrounding of current particle.
            // This should be constant w.r.t. n.
            prtcl::GridHashSet::surr_iterator neighbors_it;
            for (neighbors_it = grid->surr_begin(particles[i]);
                    neighbors_it != grid->surr_end(particles[i]);
                    ++neighbors_it) { 
                apply_force(particles[i], **neighbors_it);
            }
        }
        
        //
        //  move particles
        //
        #pragma omp for
        for( int i = 0; i < n; i++ ) 
            move( particles[i] );

        // Update grid hash set.
        #pragma omp single
        {
            grid->clear();
            insert_into_grid(n, particles, grid);
        }
        
        //
        //  save if necessary
        //
        #pragma omp master
        if( fsave && (step%SAVEFREQ) == 0 )
            save( fsave, n, particles );
    }
    simulation_time = read_timer( ) - simulation_time;
    
    printf("n = %d,\tsimulation time = %g seconds\n", n, simulation_time );
    
    free( particles );
    if( fsave )
        fclose( fsave );
    
    return 0;
}
