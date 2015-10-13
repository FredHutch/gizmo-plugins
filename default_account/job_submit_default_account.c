/*****************************************************************************\
 *  job_match_partition.c - Attempt to set partition to match job's account
 *  parameter
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
const char plugin_name[]       	= "Job submit private node plugin";
const char plugin_type[]       	= "job_submit/partition";
const uint32_t plugin_version   = SLURM_VERSION_NUMBER;

/* Get the default account for a user (or NULL if not present) */
/* from job_submit_lua.c */
static char *_get_default_account(uint32_t user_id)
{
	slurmdb_user_rec_t user;

	memset(&user, 0, sizeof(slurmdb_user_rec_t));
	user.uid = user_id;
	if (assoc_mgr_fill_in_user(acct_db_conn,
				   &user, 0, NULL) != SLURM_ERROR) {
		return user.default_acct;
	} else {
		return NULL;
	}
}

/* On job submit, check the user's default account or the account specified
 * on the command line and attempt to set the partition of the job to a
 * partition with a matching name, e.g.:
 *
 * sbatch -A foo_bar .... would set partition to "foo_bar"
 * 
 * */
extern int job_submit(struct job_descriptor *job_desc, uint32_t submit_uid)
{
	ListIterator part_iterator;
	struct part_record *part_ptr;
	struct part_record *top_prio_part = NULL;
    char *account = NULL;
    char *part_req = NULL;

    debug("default_account: starting plugin") ;

	if (job_desc->partition) {	/* job already specified partition */
        debug( "default_account: partition %s set by command line", job_desc->partition );
		return SLURM_SUCCESS;
    }

    /* check job's account */
    debug( "default_account: checking job account" );
    if (job_desc->account){
		debug("default_account: account %s requested", job_desc->account);
		account = job_desc->account ;
    } else {
        /* else check user's default account */
        account = _get_default_account( job_desc->user_id );
        debug( "default_account: got users default account %s", account );
    }

    /* look for partition matching account name */
	part_iterator = list_iterator_create(part_list);
	while ((part_ptr = (struct part_record *) list_next(part_iterator))) {
        debug( "default_account: checking partition %s against %s",
                part_ptr->name, account );
		if (strcmp(part_ptr->name,account) == 0){
            job_desc->partition = xstrdup(account);
            info( "default_account: set partition to %s", job_desc->partition );
            break;
		}
	}
	list_iterator_destroy(part_iterator);

    /* look for default partition if we set partition above */
    if (job_desc->partition != NULL ){
        debug( "default_account: checking for default partition" );
        part_iterator = list_iterator_create(part_list);
        while ((part_ptr = (struct part_record *) list_next(part_iterator))) {
            if (part_ptr->flags & PART_FLAG_DEFAULT){
                debug( "default_account: found default partition %s",
                        part_ptr->name );
                xstrcat(job_desc->partition, ",");
                xstrcat(job_desc->partition, part_ptr->name);
                info( "default_account: set partition to %s",
                        job_desc->partition );
                break;
            }
        }
        list_iterator_destroy(part_iterator);
    }

    if (job_desc->partition == NULL){
        debug( "default_account: no partition set and no match found for %s",
                account
                );
    }

	return SLURM_SUCCESS;
}

extern int job_modify(struct job_descriptor *job_desc,
		      struct job_record *job_ptr, uint32_t submit_uid)
{
	return SLURM_SUCCESS;
}
