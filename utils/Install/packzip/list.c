/*---------------------------------------------------------------------------

  list.c

  This file contains the non-ZipInfo-specific listing routines for UnZip.

  Contains:  list_files()
             get_time_stamp()   [optional feature]
             ratio()
             fnprint()

  ---------------------------------------------------------------------------*/


#define UNZIP_INTERNAL
#include "unzip.h"
#ifdef WINDLL
#  ifdef POCKET_UNZIP
#    include "wince/intrface.h"
#  else
#    include "windll/windll.h"
#  endif
#endif


#ifdef TIMESTAMP
   static int  fn_is_dir   OF((__GPRO));
#endif

#ifndef WINDLL
   static ZCONST char Far CompFactorStr[] = "%c%d%%";
   static ZCONST char Far CompFactor100[] = "100%%";

#ifdef OS2_EAS
   static ZCONST char Far HeadersS[]  =
     "  Length     EAs   ACLs    Date   Time    Name";
   static ZCONST char Far HeadersS1[] =
     " --------    ---   ----    ----   ----    ----";
#else
   static ZCONST char Far HeadersS[]  = "  Length     Date   Time    Name";
   static ZCONST char Far HeadersS1[] = " --------    ----   ----    ----";
#endif

   static ZCONST char Far HeadersL[]  =
     " Length   Method    Size  Ratio   Date   Time   CRC-32    Name";
   static ZCONST char Far HeadersL1[] =
     "--------  ------  ------- -----   ----   ----   ------    ----";
   static ZCONST char Far *Headers[][2] =
     { {HeadersS, HeadersS1}, {HeadersL, HeadersL1} };

   static ZCONST char Far CaseConversion[] =
     "%s (\"^\" ==> case\n%s   conversion)\n";
   static ZCONST char Far LongHdrStats[] =
     "%8lu  %-7s%8lu %4s  %02u-%02u-%02u %02u:%02u  %08lx %c";
   static ZCONST char Far LongFileTrailer[] =
     "--------          -------  ---                       \
     -------\n%8lu         %8lu %4s                            %u file%s\n";
#ifdef OS2_EAS
   static ZCONST char Far ShortHdrStats[] =
     "%9lu %6lu %6lu  %02u-%02u-%02u %02u:%02u  %c";
   static ZCONST char Far ShortFileTrailer[] = " --------  -----  -----       \
            -------\n%9lu %6lu %6lu                   %u file%s\n";
   static ZCONST char Far OS2ExtAttrTrailer[] =
     "%ld file%s %ld bytes of OS/2 extended attributes attached.\n";
   static ZCONST char Far OS2ACLTrailer[] =
     "%ld file%s %ld bytes of access control lists attached.\n";
#else
   static ZCONST char Far ShortHdrStats[] =
     "%9lu  %02u-%02u-%02u %02u:%02u  %c";
   static ZCONST char Far ShortFileTrailer[] =
     " --------                   -------\n%9lu                   %u file%s\n";
#endif /* ?OS2_EAS */
#endif /* !WINDLL */





/*************************/
/* Function list_files() */
/*************************/

int list_files(__G)    /* return PK-type error code */
    __GDEF
{
    int do_this_file=FALSE, cfactor, error, error_in_archive=PK_COOL;
#ifndef WINDLL
    char sgn, cfactorstr[10];
    int longhdr=(uO.vflag>1);
#endif
    int date_format;
    unsigned j, methnum, members=0;
#ifdef USE_EF_UT_TIME
    iztimes z_utime;
    struct tm *t;
#endif
    unsigned yr, mo, dy, hh, mm;
    ulg csiz, tot_csize=0L, tot_ucsize=0L;
#ifdef OS2_EAS
    ulg ea_size, tot_easize=0L, tot_eafiles=0L;
    ulg acl_size, tot_aclsize=0L, tot_aclfiles=0L;
#endif
    min_info info;
    char methbuf[8];
    static ZCONST char dtype[]="NXFS";  /* see zi_short() */
    static ZCONST char Far method[NUM_METHODS+1][8] =
        {"Stored", "Shrunk", "Reduce1", "Reduce2", "Reduce3", "Reduce4",
         "Implode", "Token", "Defl:#", "EnhDefl", "ImplDCL", "Unk:###"};

extern int files;


/*---------------------------------------------------------------------------
    Unlike extract_or_test_files(), this routine confines itself to the cen-
    tral directory.  Thus its structure is somewhat simpler, since we can do
    just a single loop through the entire directory, listing files as we go.

    So to start off, print the heading line and then begin main loop through
    the central directory.  The results will look vaguely like the following:

 Length   Method    Size  Ratio   Date   Time   CRC-32    Name ("^" ==> case
--------  ------  ------- -----   ----   ----   ------    ----   conversion)
   44004  Implode   13041  71%  11-02-89 19:34  8b4207f7  Makefile.UNIX
    3438  Shrunk     2209  36%  09-15-90 14:07  a2394fd8 ^dos-file.ext
   16717  Defl:X     5252  69%  11-03-97 06:40  1ce0f189  WHERE
--------          -------  ---                            -------
   64159            20502  68%                            3 files
  ---------------------------------------------------------------------------*/


    G.pInfo = &info;
    date_format = DATE_FORMAT;

	files = 0;

#ifndef WINDLL
    if (uO.qflag < 2) {
        if (uO.L_flag)
            Info(slide, 0, ((char *)slide, LoadFarString(CaseConversion),
              LoadFarStringSmall(Headers[longhdr][0]),
              LoadFarStringSmall2(Headers[longhdr][1])));
        else
            Info(slide, 0, ((char *)slide, "%s\n%s\n",
               LoadFarString(Headers[longhdr][0]),
               LoadFarStringSmall(Headers[longhdr][1])));
    }
#endif /* !WINDLL */

    for (j = 0; j++ < (unsigned)G.ecrec.total_entries_central_dir;) {

        if (readbuf(__G__ G.sig, 4) == 0)
            return PK_EOF;
        if (strncmp(G.sig, central_hdr_sig, 4)) {  /* just to make sure */
            Info(slide, 0x401, ((char *)slide, LoadFarString(CentSigMsg), j));
            Info(slide, 0x401, ((char *)slide, LoadFarString(ReportMsg)));
            return PK_BADERR;   /* sig not found */
        }
        /* process_cdir_file_hdr() sets pInfo->hostnum, pInfo->lcflag, ...: */
        if ((error = process_cdir_file_hdr(__G)) != PK_COOL)
            return error;       /* only PK_EOF defined */

        /*
         * We could DISPLAY the filename instead of storing (and possibly trun-
         * cating, in the case of a very long name) and printing it, but that
         * has the disadvantage of not allowing case conversion--and it's nice
         * to be able to see in the listing precisely how you have to type each
         * filename in order for unzip to consider it a match.  Speaking of
         * which, if member names were specified on the command line, check in
         * with match() to see if the current file is one of them, and make a
         * note of it if it is.
         */

        if ((error = do_string(__G__ G.crec.filename_length, DS_FN)) !=
             PK_COOL)   /*  ^--(uses pInfo->lcflag) */
        {
            error_in_archive = error;
            if (error > PK_WARN)   /* fatal:  can't continue */
                return error;
        }
        if (G.extra_field != (uch *)NULL) {
            free(G.extra_field);
            G.extra_field = (uch *)NULL;
        }
        if ((error = do_string(__G__ G.crec.extra_field_length, EXTRA_FIELD))
            != 0)
        {
            error_in_archive = error;
            if (error > PK_WARN)      /* fatal */
                return error;
        }
        if (!G.process_all_files) {   /* check if specified on command line */
            unsigned i;

            do_this_file = FALSE;
            for (i = 0; i < G.filespecs; i++)
                if (match(G.filename, G.pfnames[i], uO.C_flag)) {
                    do_this_file = TRUE;
                    break;       /* found match, so stop looping */
                }
            if (do_this_file) {  /* check if this is an excluded file */
                for (i = 0; i < G.xfilespecs; i++)
                    if (match(G.filename, G.pxnames[i], uO.C_flag)) {
                        do_this_file = FALSE;  /* ^-- ignore case in match */
                        break;
                    }
            }
        }
        /*
         * If current file was specified on command line, or if no names were
         * specified, do the listing for this file.  Otherwise, get rid of the
         * file comment and go back for the next file.
         */

        if (G.process_all_files || do_this_file) {

#ifdef OS2DLL
            /* this is used by UzpFileTree() to allow easy processing of lists
             * of zip directory contents */
            if (G.processExternally) {
                if ((G.processExternally)(G.filename, &G.crec))
                    break;
                ++members;
            } else {
#endif
#ifdef OS2_EAS
            {
                uch *ef_ptr = G.extra_field;
                int ef_size, ef_len = G.crec.extra_field_length;
                ea_size = acl_size = 0;

                while (ef_len >= EB_HEADSIZE) {
                    ef_size = makeword(&ef_ptr[EB_LEN]);
                    switch (makeword(&ef_ptr[EB_ID])) {
                        case EF_OS2:
                            ea_size = makelong(&ef_ptr[EB_HEADSIZE]);
                            break;
                        case EF_ACL:
                            acl_size = makelong(&ef_ptr[EB_HEADSIZE]);
                            break;
                    }
                    ef_ptr += (ef_size + EB_HEADSIZE);
                    ef_len -= (ef_size + EB_HEADSIZE);
                }
            }
#endif
#ifdef USE_EF_UT_TIME
            if (G.extra_field &&
#ifdef IZ_CHECK_TZ
                G.tz_is_valid &&
#endif
                (ef_scan_for_izux(G.extra_field, G.crec.extra_field_length, 1,
                                  G.crec.last_mod_dos_datetime, &z_utime, NULL)
                 & EB_UT_FL_MTIME))
            {
                TIMET_TO_NATIVE(z_utime.mtime)   /* NOP unless MSC 7.0, Mac */
                t = localtime(&(z_utime.mtime));
            } else
                t = (struct tm *)NULL;
            if (t != (struct tm *)NULL) {
                mo = (unsigned)(t->tm_mon + 1);
                dy = (unsigned)(t->tm_mday);
                yr = (unsigned)(t->tm_year % 100);
                hh = (unsigned)(t->tm_hour);
                mm = (unsigned)(t->tm_min);
            } else
#endif /* USE_EF_UT_TIME */
            {
                yr = ((((unsigned)(G.crec.last_mod_dos_datetime >> 25) & 0x7f)
                       + 80) % (unsigned)100);
                mo = ((unsigned)(G.crec.last_mod_dos_datetime >> 21) & 0x0f);
                dy = ((unsigned)(G.crec.last_mod_dos_datetime >> 16) & 0x1f);
                hh = (((unsigned)G.crec.last_mod_dos_datetime >> 11) & 0x1f);
                mm = (((unsigned)G.crec.last_mod_dos_datetime >> 5) & 0x3f);
            }
            /* permute date so it displays according to nat'l convention
             * ('methnum' is not yet set, it is used as temporary buffer) */
            switch (date_format) {
                case DF_YMD:
                    methnum = (unsigned)mo;
                    mo = yr; yr = dy; dy = (ush)methnum;
                    break;
                case DF_DMY:
                    methnum = (unsigned)mo;
                    mo = dy; dy = (ush)methnum;
            }

            csiz = G.crec.csize;
            if (G.crec.general_purpose_bit_flag & 1)
                csiz -= 12;   /* if encrypted, don't count encryption header */
            if ((cfactor = ratio(G.crec.ucsize, csiz)) < 0) {
#ifndef WINDLL
                sgn = '-';
#endif
                cfactor = (-cfactor + 5) / 10;
            } else {
#ifndef WINDLL
                sgn = ' ';
#endif
                cfactor = (cfactor + 5) / 10;
            }

            methnum = MIN(G.crec.compression_method, NUM_METHODS);
            zfstrcpy(methbuf, method[methnum]);
            if (methnum == DEFLATED) {
                methbuf[5] = dtype[(G.crec.general_purpose_bit_flag>>1) & 3];
            } else if (methnum >= NUM_METHODS) {
                sprintf(&methbuf[4], "%03u", G.crec.compression_method);
            }

#if 0       /* GRR/Euro:  add this? */
#if defined(DOS_FLX_OS2_W32) || defined(UNIX)
            for (p = G.filename;  *p;  ++p)
                if (!isprint(*p))
                    *p = '?';  /* change non-printable chars to '?' */
#endif /* DOS_FLX_OS2_W32 || UNIX */
#endif /* 0 */

#ifdef WINDLL
            /* send data to application for formatting and printing */
            (*G.lpUserFunctions->SendApplicationMessage)(G.crec.ucsize, csiz,
              (ush)cfactor, mo, dy, yr, hh, mm,
              (char)(G.pInfo->lcflag ? '^' : ' '),
              (LPSTR)fnfilter(G.filename, slide), (LPSTR)methbuf, G.crec.crc32,
              (char)((G.crec.general_purpose_bit_flag & 1) ? 'E' : ' '));
#else /* !WINDLL */
            if (cfactor == 100)
                sprintf(cfactorstr, LoadFarString(CompFactor100));
            else
                sprintf(cfactorstr, LoadFarString(CompFactorStr), sgn, cfactor);
            if (longhdr)
                Info(slide, 0, ((char *)slide, LoadFarString(LongHdrStats),
                  G.crec.ucsize, methbuf, csiz, cfactorstr, mo, dy,
                  yr, hh, mm, G.crec.crc32, (G.pInfo->lcflag? '^':' ')));
            else
#ifdef OS2_EAS
                Info(slide, 0, ((char *)slide, LoadFarString(ShortHdrStats),
                  G.crec.ucsize, ea_size, acl_size,
                  mo, dy, yr, hh, mm, (G.pInfo->lcflag? '^':' ')));
#else
                Info(slide, 0, ((char *)slide, LoadFarString(ShortHdrStats),
                  G.crec.ucsize,
                  mo, dy, yr, hh, mm, (G.pInfo->lcflag? '^':' ')));
#endif
				/* fnprint(__G);*/
				files++;
#endif /* ?WINDLL */

            if ((error = do_string(__G__ G.crec.file_comment_length,
                                   QCOND? DISPL_8 : SKIP)) != 0)
            {
                error_in_archive = error;  /* might be just warning */
                if (error > PK_WARN)       /* fatal */
                    return error;
            }
            tot_ucsize += G.crec.ucsize;
            tot_csize += csiz;
            ++members;
#ifdef OS2_EAS
            if (ea_size) {
                tot_easize += ea_size;
                ++tot_eafiles;
            }
            if (acl_size) {
                tot_aclsize += acl_size;
                ++tot_aclfiles;
            }
#endif
#ifdef OS2DLL
            } /* end of "if (G.processExternally) {...} else {..." */
#endif
        } else {        /* not listing this file */
            SKIP_(G.crec.file_comment_length)
        }
    } /* end for-loop (j: files in central directory) */

/*---------------------------------------------------------------------------
    Print footer line and totals (compressed size, uncompressed size, number
    of members in zipfile).
  ---------------------------------------------------------------------------*/

    if (uO.qflag < 2
#ifdef OS2DLL
                     && !G.processExternally
#endif
                                            ) {
        if ((cfactor = ratio(tot_ucsize, tot_csize)) < 0) {
#ifndef WINDLL
            sgn = '-';
#endif
            cfactor = (-cfactor + 5) / 10;
        } else {
#ifndef WINDLL
            sgn = ' ';
#endif
            cfactor = (cfactor + 5) / 10;
        }
#ifdef WINDLL
        /* pass the totals back to the calling application */
        G.lpUserFunctions->TotalSizeComp = tot_csize;
        G.lpUserFunctions->TotalSize = tot_ucsize;
        G.lpUserFunctions->CompFactor = cfactor;
        G.lpUserFunctions->NumMembers = members;

#else /* !WINDLL */
        if (cfactor == 100)
            sprintf(cfactorstr, LoadFarString(CompFactor100));
        else
            sprintf(cfactorstr, LoadFarString(CompFactorStr), sgn, cfactor);
        if (longhdr) {
            Info(slide, 0, ((char *)slide, LoadFarString(LongFileTrailer),
              tot_ucsize, tot_csize, cfactorstr, members, members==1? "":"s"));
#ifdef OS2_EAS
            if (tot_easize || tot_aclsize)
                Info(slide, 0, ((char *)slide, "\n"));
            if (tot_eafiles && tot_easize)
                Info(slide, 0, ((char *)slide, LoadFarString(OS2ExtAttrTrailer),
                  tot_eafiles, tot_eafiles == 1? " has" : "s have a total of",
                  tot_easize));
            if (tot_aclfiles && tot_aclsize)
                Info(slide, 0, ((char *)slide, LoadFarString(OS2ACLTrailer),
                  tot_aclfiles, tot_aclfiles == 1? " has" : "s have a total of",
                  tot_aclsize));
#endif /* OS2_EAS */
        } else
#ifdef OS2_EAS
            Info(slide, 0, ((char *)slide, LoadFarString(ShortFileTrailer),
              tot_ucsize, tot_easize, tot_aclsize, members, members == 1?
              "" : "s"));
#else
            Info(slide, 0, ((char *)slide, LoadFarString(ShortFileTrailer),
              tot_ucsize, members, members == 1? "" : "s"));
#endif /* OS2_EAS */
#endif /* ?WINDLL */
    }

/*---------------------------------------------------------------------------
    Double check that we're back at the end-of-central-directory record.
  ---------------------------------------------------------------------------*/

    if (readbuf(__G__ G.sig, 4) == 0)  /* disk error? */
        return PK_EOF;
    if (strncmp(G.sig, end_central_sig, 4)) {   /* just to make sure again */
        Info(slide, 0x401, ((char *)slide, LoadFarString(EndSigMsg)));
        error_in_archive = PK_WARN;   /* didn't find sig */
    }
    if (members == 0 && error_in_archive <= PK_WARN)
        error_in_archive = PK_FIND;

    return error_in_archive;

} /* end function list_files() */





#ifdef TIMESTAMP

/************************/
/* Function fn_is_dir() */
/************************/

static int fn_is_dir(__G)    /* returns TRUE if G.filename is directory */
    __GDEF
{
    extent fn_len = strlen(G.filename);
    register char   endc;

    return  fn_len > 0 &&
            ((endc = G.filename[fn_len-1]) == '/' ||
             (G.pInfo->hostnum == FS_FAT_ && !strchr(G.filename, '/') &&
              endc == '\\'));
}





/*****************************/
/* Function get_time_stamp() */
/*****************************/

int get_time_stamp(__G__ last_modtime, nmember)  /* return PK-type error code */
    __GDEF
    time_t *last_modtime;
    unsigned *nmember;
{
    int do_this_file=FALSE, error, error_in_archive=PK_COOL;
    unsigned j;
#ifdef USE_EF_UT_TIME
    iztimes z_utime;
#endif
    min_info info;


/*---------------------------------------------------------------------------
    Unlike extract_or_test_files() but like list_files(), this function works
    on information in the central directory alone.  Thus we have a single,
    large loop through the entire directory, searching for the latest time
    stamp.
  ---------------------------------------------------------------------------*/

    *last_modtime = 0L;         /* assuming no zipfile data older than 1970 */
    *nmember = 0;
    G.pInfo = &info;

    for (j = 0; j++ < (unsigned)G.ecrec.total_entries_central_dir;) {

        if (readbuf(__G__ G.sig, 4) == 0)
            return PK_EOF;
        if (strncmp(G.sig, central_hdr_sig, 4)) {  /* just to make sure */
            Info(slide, 0x401, ((char *)slide, LoadFarString(CentSigMsg), j));
            Info(slide, 0x401, ((char *)slide, LoadFarString(ReportMsg)));
            return PK_BADERR;
        }
        /* process_cdir_file_hdr() sets pInfo->lcflag: */
        if ((error = process_cdir_file_hdr(__G)) != PK_COOL)
            return error;       /* only PK_EOF defined */
        if ((error = do_string(__G__ G.crec.filename_length, DS_FN)) != PK_OK)
        {        /*  ^-- (uses pInfo->lcflag) */
            error_in_archive = error;
            if (error > PK_WARN)   /* fatal:  can't continue */
                return error;
        }
        if (G.extra_field != (uch *)NULL) {
            free(G.extra_field);
            G.extra_field = (uch *)NULL;
        }
        if ((error = do_string(__G__ G.crec.extra_field_length, EXTRA_FIELD))
            != 0)
        {
            error_in_archive = error;
            if (error > PK_WARN)      /* fatal */
                return error;
        }
        if (!G.process_all_files) {   /* check if specified on command line */
            unsigned i;

            do_this_file = FALSE;
            for (i = 0; i < G.filespecs; i++)
                if (match(G.filename, G.pfnames[i], uO.C_flag)) {
                    do_this_file = TRUE;
                    break;       /* found match, so stop looping */
                }
            if (do_this_file) {  /* check if this is an excluded file */
                for (i = 0; i < G.xfilespecs; i++)
                    if (match(G.filename, G.pxnames[i], uO.C_flag)) {
                        do_this_file = FALSE;  /* ^-- ignore case in match */
                        break;
                    }
            }
        }

        /* If current file was specified on command line, or if no names were
         * specified, check the time for this file.  Either way, get rid of the
         * file comment and go back for the next file.
         * Directory entries are always ignored, to stay compatible with both
         * Zip and PKZIP.
         */
        if ((G.process_all_files || do_this_file) && !fn_is_dir(__G)) {
#ifdef USE_EF_UT_TIME
            if (G.extra_field &&
#ifdef IZ_CHECK_TZ
                G.tz_is_valid &&
#endif
                (ef_scan_for_izux(G.extra_field, G.crec.extra_field_length, 1,
                                  G.crec.last_mod_dos_datetime, &z_utime, NULL)
                 & EB_UT_FL_MTIME))
            {
                if (*last_modtime < z_utime.mtime)
                    *last_modtime = z_utime.mtime;
            } else
#endif /* USE_EF_UT_TIME */
            {
                time_t modtime = dos_to_unix_time(G.crec.last_mod_dos_datetime);

                if (*last_modtime < modtime)
                    *last_modtime = modtime;
            }
            ++*nmember;
        }
        SKIP_(G.crec.file_comment_length)

    } /* end for-loop (j: files in central directory) */

/*---------------------------------------------------------------------------
    Double check that we're back at the end-of-central-directory record.
  ---------------------------------------------------------------------------*/

    if (readbuf(__G__ G.sig, 4) == 0)
        return PK_EOF;
    if (strncmp(G.sig, end_central_sig, 4)) {   /* just to make sure again */
        Info(slide, 0x401, ((char *)slide, LoadFarString(EndSigMsg)));
        error_in_archive = PK_WARN;
    }
    if (*nmember == 0 && error_in_archive <= PK_WARN)
        error_in_archive = PK_FIND;

    return error_in_archive;

} /* end function get_time_stamp() */

#endif /* TIMESTAMP */





/********************/
/* Function ratio() */    /* also used by ZipInfo routines */
/********************/

int ratio(uc, c)
    ulg uc, c;
{
    ulg denom;

    if (uc == 0)
        return 0;
    if (uc > 2000000L) {    /* risk signed overflow if multiply numerator */
        denom = uc / 1000L;
        return ((uc >= c) ?
            (int) ((uc-c + (denom>>1)) / denom) :
          -((int) ((c-uc + (denom>>1)) / denom)));
    } else {             /* ^^^^^^^^ rounding */
        denom = uc;
        return ((uc >= c) ?
            (int) ((1000L*(uc-c) + (denom>>1)) / denom) :
          -((int) ((1000L*(c-uc) + (denom>>1)) / denom)));
    }                            /* ^^^^^^^^ rounding */
}





/************************/
/*  Function fnprint()  */    /* also used by ZipInfo routines */
/************************/

void fnprint(__G)    /* print filename (after filtering) and newline */
    __GDEF
{
    char *name = fnfilter(G.filename, slide);

    (*G.message)((zvoid *)&G, (uch *)name, (ulg)strlen(name), 0);
    (*G.message)((zvoid *)&G, (uch *)"\n", 1L, 0);

} /* end function fnprint() */
