#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "core_allvars.h"
#include "core_mymalloc.h"
#include "core_io_tree.h"

#include "io/read_tree_lhalo_binary.h"
#include "io/read_tree_consistentrees_ascii.h"

#ifdef HDF5
#include "io/read_tree_lhalo_hdf5.h"
#include "io/read_tree_genesis_standard_hdf5.h"
#endif

int setup_forests_io(struct params *run_params, struct forest_info *forests_info,
                     const int ThisTask, const int NTasks)
{
    int status = EXIT_FAILURE;/* initialize to FAIL  */
    const int firstfile = run_params->FirstFile;
    const int lastfile = run_params->LastFile;
    const enum Valid_TreeTypes TreeType = run_params->TreeType;

    switch (TreeType)
        {
#ifdef HDF5
        case lhalo_hdf5:
            status = setup_forests_io_lht_hdf5(forests_info, firstfile, lastfile, ThisTask, NTasks, run_params);
            break;

        case genesis_standard_hdf5:
            (void) firstfile, (void) lastfile;
            status = setup_forests_io_genesis_hdf5(forests_info, ThisTask, NTasks, run_params);
            break;
#endif

        case lhalo_binary:
            status = setup_forests_io_lht_binary(forests_info, firstfile, lastfile, ThisTask, NTasks, run_params);
            break;

        case consistent_trees_ascii:
            (void) firstfile, (void) lastfile;
            status = setup_forests_io_ctrees(forests_info, ThisTask, NTasks, run_params);
            break;

        default:
            fprintf(stderr, "Your tree type has not been included in the switch statement for function ``%s`` in file ``%s``.\n", __FUNCTION__, __FILE__);
            fprintf(stderr, "Please add it there.\n");
            return INVALID_OPTION_IN_PARAMS;
        }


    return status;
}


/* This routine is to be called after *ALL* forests have been processed */
void cleanup_forests_io(enum Valid_TreeTypes TreeType, struct forest_info *forests_info)
{
    /* Don't forget to free the open file handle */
    switch (TreeType) {
#ifdef HDF5
    case lhalo_hdf5:
        break;

    /* case genesis_standard_hdf5: */
    /*     break; */
#endif

    case lhalo_binary:
        cleanup_forests_io_lht_binary(forests_info);
        break;

    case consistent_trees_ascii:

        /* because consistent trees can only be cleaned up after *ALL* forests
           have been processed (and not on a `per file` basis)
         */
        cleanup_forests_io_ctrees(forests_info);
        break;

    default:
        fprintf(stderr, "Your tree type has not been included in the switch statement for function ``%s`` in file ``%s``.\n", __FUNCTION__, __FILE__);
        fprintf(stderr, "Please add it there.\n");
        ABORT(EXIT_FAILURE);

    }

    return;
}

int64_t load_forest(struct params *run_params, const int forestnr, struct halo_data **halos, struct forest_info *forests_info)
{

    int64_t nhalos;
    const enum Valid_TreeTypes TreeType = run_params->TreeType;

    switch (TreeType) {

#ifdef HDF5
    case lhalo_hdf5:
        nhalos = load_forest_hdf5(forestnr, halos, forests_info);
        break;

    case genesis_standard_hdf5:
        nhalos = load_forest_genesis_hdf5(forestnr, halos, forests_info, run_params);
        break;
#endif

    case lhalo_binary:
        nhalos = load_forest_lht_binary(forestnr, halos, forests_info);
        break;

    case consistent_trees_ascii:
        nhalos = load_forest_ctrees(forestnr, halos, forests_info, run_params);
        break;

    default:
        fprintf(stderr, "Your tree type has not been included in the switch statement for ``%s`` in ``%s``.\n",
                __FUNCTION__, __FILE__);
        fprintf(stderr, "Please add it there.\n");
        ABORT(EXIT_FAILURE);
    }

    return nhalos;
}
