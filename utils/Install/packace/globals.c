/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      Global variable declarations                                        */
/*                                                                          */
/* ------------------------------------------------------------------------ */

#include "os.h"

#include "acestruc.h"
#include "unace.h"

#include <string.h>
#include <stdio.h>

//-------- Ace sign
const char *acesign = "**ACE**";

//-------- Version string for program
const char *version="UNACE v1.1    public version\n";

#ifdef AMIGA
//-------- Version string for VERSION program
static char *_version="$VER: Unace Amiga 1.1  "__AMIGADATE__"\n\n";
#endif

//-------- header buffer and pointers
thead head;

tmhead *t_mhead = (tmhead *) & head;
tfhead *t_fhead = (tfhead *) & head;

//-------- buffers
ULONG *buf_rd        =0;
CHAR  *buf           =0;
CHAR  *buf_wr        =0;
UCHAR *readbuf       =0;

//-------- decompressor variables
SHORT rpos           =0,
      dcpr_do        =0,
      dcpr_do_max    =0,
      blocksize      =0,
      dcpr_dic       =0,
      dcpr_oldnum    =0,
      bits_rd        =0,
      dcpr_frst_file =0;
USHORT dcpr_code_mn[1 << maxwd_mn],
       dcpr_code_lg[1 << maxwd_lg];
UCHAR dcpr_wd_mn[maxcode + 2],
      dcpr_wd_lg[maxcode + 2],
      wd_svwd[svwd_cnt];
ULONG dcpr_dpos      =0,
      cpr_dpos2      =0,
      dcpr_dicsiz    =0,
      dcpr_dican     =0,
      dcpr_size      =0,
      dcpr_olddist[4]={0,0,0,0},
      code_rd        =0;

CHAR *dcpr_text      =0;

//-------- quicksort
USHORT sort_org[maxcode + 2];
UCHAR sort_freq[(maxcode + 2) * 2];

//-------- file handling
CHAR aname[PATH_MAX];
INT  archan,
     wrhan;
#if !defined(__EMX__) && !defined(__OS2__)
FILE *farchan = NULL;
#endif
LONG skipsize=0;

//-------- structures for archive handling
struct tadat adat;

//-------- flags
INT  f_err      =0,
     f_ovrall   =0,
     f_allvol_pr=0,
     f_curpas   =0,
     f_criterr  =0;


void resetglobals(void)
{
	t_mhead = (tmhead *) & head;
	t_fhead = (tfhead *) & head;

	buf_rd        =0;
	buf           =0;
	buf_wr        =0;
	readbuf       =0;

	rpos           =0;
	dcpr_do        =0;
	dcpr_do_max    =0;
	blocksize      =0;
	dcpr_dic       =0;
	dcpr_oldnum    =0;
	bits_rd        =0;
	dcpr_frst_file =0;

	memset(&dcpr_code_mn, 0, sizeof(dcpr_code_mn));
	memset(&dcpr_code_lg, 0, sizeof(dcpr_code_lg));
	memset(&dcpr_wd_mn, 0, sizeof(dcpr_wd_mn));
	memset(&dcpr_wd_lg, 0, sizeof(dcpr_wd_lg));
	memset(&wd_svwd, 0, sizeof(wd_svwd));
	dcpr_dpos      =0;
	cpr_dpos2      =0;
	dcpr_dicsiz    =0;
	dcpr_dican     =0;
	dcpr_size      =0;

	memset(&dcpr_olddist, 0, sizeof(dcpr_olddist));

	code_rd        =0;
	dcpr_text      =0;

	memset(&sort_org, 0, sizeof(sort_org));
	memset(&sort_freq, 0, sizeof(sort_freq));

	archan=0;
	wrhan=0;
	skipsize=0;

	f_err      =0;
	f_ovrall   =0;
	f_allvol_pr=0;
	f_curpas   =0;
	f_criterr  =0;

}
