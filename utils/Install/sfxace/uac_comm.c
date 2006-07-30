/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      Decompresses and outputs comment if present.                        */
/*                                                                          */
/* ------------------------------------------------------------------------ */

#include <stdio.h>    // printf()

#include "globals.h"
#include "uac_dcpr.h"
#include "uac_comm.h"

INT  comm_cpr_size=0;
CHAR *comm;

void comment_out(CHAR *top)      // outputs comment if present
{
	INT  i;

	if (head.HEAD_FLAGS & ACE_COMM)
	{                             // comment present?
		if (head.HEAD_TYPE == MAIN_BLK)
		{                          // get begin and size of comment data
			comm = (CHAR*)MCOMM;
			comm_cpr_size = MCOMM_SIZE;
		}
		else
		{
		 comm = (CHAR*)FCOMM;
		 comm_cpr_size = FCOMM_SIZE;
		}                          // limit comment size if too big
		i = sizeof(head) - (INT)(comm - (CHAR*) &head);
		if (comm_cpr_size > i)
			comm_cpr_size = i;
		dcpr_comm(i);              // decompress comment

#ifdef AMIGA
		{
			char *p=comm;
			while (*p)
			{
				if (*p==0x0D)
					*p=0x0A;          // Replace ms-dos line termination
				p++;
			}
		}
#endif

		pipeit("%s\n\n%s\n\n", top, comm); // output comment
	}
}
