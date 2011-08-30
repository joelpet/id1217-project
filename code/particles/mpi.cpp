#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "common.h"
#include "grid_hash_set.h"

//
//  benchmarking program
//
int main( int argc, char **argv )
{    
    //
    //  process command line parameters
    //
    if( find_option( argc, argv, "-h" ) >= 0 )
    {
        printf( "Options:\n" );
        printf( "-h to see this help\n" );
        printf( "-n <int> to set the number of particles\n" );
        printf( "-s <int> to set the number of steps in the simulation\n" );
        printf( "-o <filename> to specify the output file name\n" );
        printf( "-f <int> to set the frequency of saving particle coordinates\n" );
        return 0;
    }

    int n = read_int( argc, argv, "-n", 1000 );
    int s = read_int( argc, argv, "-s", NSTEPS );
    int f = read_int( argc, argv, "-f", SAVEFREQ );
    char *savename = read_string( argc, argv, "-o", NULL );

    //
    //  set up MPI
    //
    int n_proc, rank;
    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &n_proc );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );

    //
    //  allocate generic resources
    //
    FILE *fsave = savename && rank == 0 ? fopen( savename, "w" ) : NULL;

    // Define derived data type representing the six doubles in particle_t.
    MPI_Datatype PARTICLE;
    MPI_Type_contiguous( 6, MPI_DOUBLE, &PARTICLE );
    MPI_Type_commit( &PARTICLE );

    //
    //  set up the data partitioning across processors
    //
    int particle_per_proc = (n + n_proc - 1) / n_proc;
    int *partition_offsets = (int*) malloc( (n_proc+1) * sizeof(int) );
    for( int i = 0; i < n_proc+1; i++ )
        partition_offsets[i] = min( i * particle_per_proc, n );

    int *partition_sizes = (int*) malloc( n_proc * sizeof(int) );
    for( int i = 0; i < n_proc; i++ )
        partition_sizes[i] = partition_offsets[i+1] - partition_offsets[i];

    //
    //  allocate storage for local partition
    //
    int nlocal = partition_sizes[rank];

    //
    //  initialize and distribute the particles (that's fine to leave it unoptimized)
    //
    set_size( n );
    particle_t *particles = (particle_t*) malloc( n * sizeof(particle_t) );
    if( rank == 0 ) {
        init_particles( n, particles );
    }
    MPI_Bcast(particles, n, PARTICLE, 0, MPI_COMM_WORLD);

    prtcl::GridHashSet* grid = new prtcl::GridHashSet(n, size, cutoff);

    //
    //  simulate a number of time steps
    //
    double simulation_time = read_timer( );
    for( int step = 0; step < s; step++ )
    {
        // Prepare grid hash set and insert local data.
        grid->clear();
        insert_into_grid(n, particles, grid);

        //
        //  save current step if necessary (slightly different semantics than in other codes)
        //
        if( rank == 0 && fsave && (step % f) == 0 )
            save( fsave, n, particles );

        //
        //  compute all forces
        //
        for (int i = partition_offsets[rank]; i < partition_offsets[rank + 1]; i++)
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
        for (int i = partition_offsets[rank]; i < partition_offsets[rank + 1]; i++)
            move( particles[i] );


        // Now distribute the work we have done to everyone else, before next iteration.
        MPI_Barrier(MPI_COMM_WORLD); // TODO nödvändig?
        particle_t* local = &particles[partition_offsets[rank]];
        MPI_Allgatherv(local, nlocal, PARTICLE, particles, partition_sizes, partition_offsets, PARTICLE, MPI_COMM_WORLD );

    }
    simulation_time = read_timer( ) - simulation_time;

    if( rank == 0 )
        printf("n = %d, steps = %d, savefreq = %d, n_procs = %d, simulation time = %g seconds\n", n, s, f, n_proc, simulation_time);

    //
    //  release resources
    //
    delete grid;
    free( partition_offsets );
    free( partition_sizes );
    free( particles );
    if( fsave )
        fclose( fsave );

    MPI_Finalize( );

    return 0;
}
