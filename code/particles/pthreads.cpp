#include "common.h"
#include "grid_hash_set.h"
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

//
//  global variables
//
int n, s, f, n_threads;
particle_t * particles;
prtcl::GridHashSet* grid;
FILE *fsave;
pthread_barrier_t barrier;

//
//  check that pthreads routine call was successful
//
#define P( condition ) {if( (condition) != 0 ) { printf( "\n FAILURE in %s, line %d\n", __FILE__, __LINE__ );exit( 1 );}}

//
//  This is where the action happens
//
void *thread_routine( void *pthread_id )
{
    int thread_id = *(int*)pthread_id;

    int particles_per_thread = (n + n_threads - 1) / n_threads;
    int first = min(  thread_id    * particles_per_thread, n );
    int last  = min( (thread_id+1) * particles_per_thread, n );
    
    //
    //  simulate a number of time steps
    //
    for( int step = 0; step < s; step++ )
    {
        //
        //  compute forces
        //
        for( int i = first; i < last; i++ )
        {
            particles[i].ax = particles[i].ay = 0;

            // Iterate over all neighbors in the surrounding of current particle.
            // This should be constant w.r.t. n.
            prtcl::GridHashSet::surr_iterator neighbors_it;
            for (neighbors_it = grid->surr_begin(particles[i]);
                    neighbors_it != grid->surr_end(particles[i]);
                    ++neighbors_it) { 
                //printf("\t\tneighbor (%f, %f) in grid [%d][%d]\n", (*neighbors_it)->x, (*neighbors_it)->y, grid->get_row(**neighbors_it), grid->get_col(**neighbors_it));

                apply_force(particles[i], **neighbors_it);
            }
        }
        
        pthread_barrier_wait( &barrier );
        
        //
        //  move particles
        //
        for( int i = first; i < last; i++ ) 
            move( particles[i] );
        
        pthread_barrier_wait( &barrier );
        

        // Update grid hash set (thread 0).
        if (thread_id == 0) {
            grid->clear();
            insert_into_grid(n, particles, grid);
        }

        pthread_barrier_wait( &barrier );

        //
        //  save if necessary
        //
        if( thread_id == 0 && fsave && (step % f) == 0 )
            save( fsave, n, particles );
    }
    
    return NULL;
}

//
//  benchmarking program
//
int main( int argc, char **argv )
{    
    //
    //  process command line
    //
    if( find_option( argc, argv, "-h" ) >= 0 )
    {
        printf( "Options:\n" );
        printf( "-h to see this help\n" );
        printf( "-n <int> to set the number of particles\n" );
        printf( "-s <int> to set the number of steps in the simulation\n" );
        printf( "-p <int> to set the number of threads\n" );
        printf( "-o <filename> to specify the output file name\n" );
        printf( "-f <int> to the the frequency of saving particle coordinates\n" );
        return 0;
    }
    
    n = read_int( argc, argv, "-n", 1000 );
    s = read_int( argc, argv, "-s", NSTEPS );
    f = read_int( argc, argv, "-f", SAVEFREQ );
    n_threads = read_int( argc, argv, "-p", 2 );
    char *savename = read_string( argc, argv, "-o", NULL );
    
    //
    //  allocate resources
    //
    fsave = savename ? fopen( savename, "w" ) : NULL;

    particles = (particle_t*) malloc( n * sizeof(particle_t) );
    set_size( n );
    init_particles( n, particles );

    grid = new prtcl::GridHashSet(n, size, cutoff);
    insert_into_grid(n, particles, grid);

    pthread_attr_t attr;
    P( pthread_attr_init( &attr ) );
    P( pthread_barrier_init( &barrier, NULL, n_threads ) );

    int *thread_ids = (int *) malloc( n_threads * sizeof( int ) );
    for( int i = 0; i < n_threads; i++ ) 
        thread_ids[i] = i;

    pthread_t *threads = (pthread_t *) malloc( n_threads * sizeof( pthread_t ) );
    
    //
    //  do the parallel work
    //
    double simulation_time = read_timer( );
    for( int i = 1; i < n_threads; i++ ) 
        P( pthread_create( &threads[i], &attr, thread_routine, &thread_ids[i] ) );
    
    thread_routine( &thread_ids[0] );
    
    for( int i = 1; i < n_threads; i++ ) 
        P( pthread_join( threads[i], NULL ) );
    simulation_time = read_timer( ) - simulation_time;
    
    printf( "n = %d, n_threads = %d, simulation time = %g seconds\n", n, n_threads, simulation_time );
    
    //
    //  release resources
    //
    P( pthread_barrier_destroy( &barrier ) );
    P( pthread_attr_destroy( &attr ) );
    free( thread_ids );
    free( threads );
    free( particles );
    if( fsave )
        fclose( fsave );
    
    return 0;
}
