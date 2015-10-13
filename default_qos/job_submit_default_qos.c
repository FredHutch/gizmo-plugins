/*****************************************************************************\
 *
 *  default_qos.c - Attempt to set QOS to match partition
 *
 *****************************************************************************
 *  FIXME: copyright?
 *
\*****************************************************************************/

#if HAVE_CONFIG_H
#  include "config.h"
#  if STDC_HEADERS
#    include <string.h>
#  endif
#  if HAVE_SYS_TYPES_H
#    include <sys/types.h>
#  endif /* HAVE_SYS_TYPES_H */
#  if HAVE_UNISTD_H
#    include <unistd.h>
#  endif
#  if HAVE_INTTYPES_H
#    include <inttypes.h>
#  else /* ! HAVE_INTTYPES_H */
#    if HAVE_STDINT_H
#      include <stdint.h>
#    endif
#  endif /* HAVE_INTTYPES_H */
#else /* ! HAVE_CONFIG_H */
#  include <sys/types.h>
#  include <unistd.h>
#  include <stdint.h>
#  include <string.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>

#include "slurm/slurm_errno.h"
#include "src/common/slurm_xlator.h"
#include "src/slurmctld/slurmctld.h"

#include "src/common/assoc_mgr.h"


/*
 * These variables are required by the generic plugin interface.  If they
 * are not found in the plugin, the plugin loader will ignore it.
 *
 * plugin_name - a string giving a human-readable description of the
 * plugin.  There is no maximum length, but the symbol must refer to
 * a valid string.
 *
 * plugin_type - a string suggesting the type of the plugin or its
 * applicability to a particular form of data or method of data handling.
 * If the low-level plugin API is used, the contents of this string are
 * unimportant and may be anything.  SLURM uses the higher-level plugin
 * interface which requires this string to be of the form
 *
 *	<application>/<method>
 *
 * where <application> is a description of the intended application of
 * the plugin (e.g., "auth" for SLURM authentication) and <method> is a
 * description of how this plugin satisfies that application.  SLURM will
 * only load authentication plugins if the plugin_type string has a prefix
 * of "auth/".
 *
 * plugin_version   - specifies the version number of the plugin.
 * min_plug_version - specifies the minumum version number of incoming
 *                    messages that this plugin can accept
 */
const char plugin_name[]       	= "Job submit default QOS plugin";
const char plugin_type[]       	= "job_submit/qos";
const uint32_t plugin_version   = SLURM_VERSION_NUMBER;



/* If QOS isn't set, attempt to set the QOS to a QOS matching the partition
 * name
 *
 * sbatch -p foo file.sh
 *
 * would attempt to set the QOS to "foo"
 * 
 * */
extern int job_submit(struct job_descriptor *job_desc, uint32_t submit_uid)
{
    slurmdb_qos_rec_t *qos_ptr = NULL ;
	ListIterator qos_iterator;
    List qos_list = NULL;
    int matched = 0;

    debug("default_qos: starting plugin") ;

	if (job_desc->qos) {	/* job already specified qos */
        debug( "default_qos: qos %s set by command line", job_desc->qos );
		return SLURM_SUCCESS;
    }
	if (!job_desc->partition) {
        /* job partition is default- let default handle it... */
        debug( "default_qos: no partition set- using defaults" );
		return SLURM_SUCCESS;
    }

    debug( "default_qos: requested partition \"%s\"", 
            job_desc->partition
         );

    /* Iterate through list of configured QOS's and locate a match
     * if no match is found, leave blank and let ctld figure out which
     * qos to use and if it has permission
     */

    assoc_mgr_lock_t locks = {
        NO_LOCK, NO_LOCK, READ_LOCK, NO_LOCK, NO_LOCK, NO_LOCK
    }; 
    assoc_mgr_lock(&locks);

    /* look for qos matching partition name  */
	qos_iterator = list_iterator_create( assoc_mgr_qos_list );

	while (( qos_ptr = list_next(qos_iterator) )) {
        debug( "default_qos: checking for QOS name \"%s\" ", qos_ptr->name );
        if (strcmp( job_desc->partition,qos_ptr->name  ) == 0)
        {
            debug( "default_qos: found qos %s matching %s",
                    qos_ptr->name,
                    job_desc->partition
                    );
            job_desc->qos = xstrdup( qos_ptr->name );
            matched = 1;
            info( "default_qos: set job qos to %s", job_desc->qos );
            break;
        }

	}
	list_iterator_destroy(qos_iterator);
    assoc_mgr_unlock(&locks);

    if( matched == 0 )
    {
        info(
            "default_qos: no matching qos found for partition %s",
            job_desc->partition
            );
    }

	return SLURM_SUCCESS;
}

extern int job_modify(struct job_descriptor *job_desc,
		      struct job_record *job_ptr, uint32_t submit_uid)
{
	return SLURM_SUCCESS;
}
