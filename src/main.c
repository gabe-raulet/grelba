#include "elba.h"
#include "elba_error.h"
#include "mpiutil.h"
#include "commgrid.h"
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int myrank, nprocs;
    commgrid_t grid;
    elba_opt_t opt;

    /*
     * Initialize MPI.
     */

    MPI_Init(&argc, &argv);

    /*
     * Initialize CombBLAS grid communicator.
     */

    ELBA_CHECK(commgrid_init(&grid, MPI_COMM_WORLD));
    commgrid_log(grid, stderr);

    /*
     * main() probably will need to know where in the grid it is.
     */

    mpi_info(MPI_COMM_WORLD, &myrank, &nprocs);

    /*
     * Initialize default ELBA parameters.
     */

    elba_opt_init(&opt);

    /*
     * Parse user provided command-line parameters.
     */

    ELBA_CHECK(elba_opt_parse(&opt, myrank, argc, argv));
    elba_opt_log(opt, grid.gridrank, stderr);

    ELBA_CHECK(commgrid_free(&grid));

    MPI_Finalize();

    return 0;
}
