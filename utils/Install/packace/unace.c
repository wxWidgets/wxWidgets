/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      Main file of public UNACE.                                          */
/*                                                                          */
/* ------------------------------------------------------------------------ */


//--------------- include general files ------------------------------------//
#include <ctype.h>      // tolower()
#include <fcntl.h>      // open()
#include <stdio.h>      // printf() sprintf() remove()
#include <stdlib.h>     // malloc()
#include <string.h>     // str*()
#include <sys/types.h>
#include <sys/stat.h>   // S_I*  AMIGA: fstat()

#define DIRSEP	'\\'

#if (!defined(__EMX__) && !defined(__OS2__) && !defined(WINNT) && !defined(WIN32)) ||  defined(__CYGWIN__)
#include <sys/errno.h>
#endif


//--------------- include unace specific header files ----------------------//
#include "os.h"

#include "globals.h"
#include "portable.h"
#include "uac_comm.h"
#include "uac_crc.h"
#include "uac_crt.h"
#include "uac_dcpr.h"
#include "uac_sys.h"

#ifdef CRYPT
 #include "unace_ps.h"
#endif /* CRYPT */
int files=0;

//--------------- BEGIN OF UNACE ROUTINES ----------------------------------//

int pipeit(char *format, ...) {
	/* Do nothing ... perhaps pipe this somewhere in the future */
	return 0;
}

void init_unace(void)           // initializes unace
{
	buf_rd =malloc(size_rdb * sizeof(ULONG));  // Allocate buffers: increase
	buf    =malloc(size_buf);                  // sizes when possible to speed
	buf_wr =malloc(size_wrb);                  // up the program
	readbuf=malloc(size_headrdb);

	if (buf_rd ==NULL ||
		buf    ==NULL ||
		buf_wr ==NULL ||
		readbuf==NULL )
		f_err = ERR_MEM;

	make_crctable();             // initialize CRC table
	dcpr_init();                 // initialize decompression

	set_handler();               // ctrl+break etc.
}

void done_unace(void)
{
	if (buf_rd   ) free(buf_rd   );
	if (buf      ) free(buf      );
	if (buf_wr   ) free(buf_wr   );
	if (readbuf  ) free(readbuf  );
	if (dcpr_text) free(dcpr_text);
}

INT  read_header(INT print_err)         // reads any header from archive
{
	USHORT rd,
	head_size,
	crc_ok;
	LONG crc;
	UCHAR *tp=readbuf;

	lseek(archan, skipsize, SEEK_CUR);   // skip ADDSIZE block

	if (read(archan, &head, 4)<4)
		return (0);                       // read CRC and header size

#ifdef HI_LO_BYTE_ORDER
	WORDswap(&head.HEAD_CRC);
	WORDswap(&head.HEAD_SIZE);
#endif
	// read size_headrdb bytes into
	head_size = head.HEAD_SIZE;          // header structure
	rd = (head_size > size_headrdb) ? size_headrdb : head_size;
	if (read(archan, readbuf, rd) < rd)
		return (0);
	head_size -= rd;
	crc = getcrc(CRC_MASK, readbuf, rd);

	while (head_size)                    // skip rest of header
	{
		rd = (head_size > size_buf) ? size_buf : head_size;
		if (read(archan, buf, rd) < rd)
			return (0);
		head_size -= rd;
		crc = getcrc(crc, (UCHAR *)buf, rd);
	}

	head.HEAD_TYPE =*tp++;               // generic buffer to head conversion
	head.HEAD_FLAGS=BUFP2WORD(tp);

	if (head.HEAD_FLAGS & ACE_ADDSIZE)
		skipsize = head.ADDSIZE = BUF2LONG(tp);   // get ADDSIZE
	else
		skipsize = 0;

	// check header CRC
	if (!(crc_ok = head.HEAD_CRC == (crc & 0xffff)) && print_err)
		pipeit("\nError: archive is broken\n");
	else
		switch (head.HEAD_TYPE)              // specific buffer to head conversion
		{
		case MAIN_BLK:
			memcpy(mhead.ACESIGN, tp, acesign_len); tp+=acesign_len;
			mhead.VER_MOD=*tp++;
			mhead.VER_CR =*tp++;
			mhead.HOST_CR=*tp++;
			mhead.VOL_NUM=*tp++;
			mhead.TIME_CR=BUFP2LONG(tp);
			mhead.RES1   =BUFP2WORD(tp);
			mhead.RES2   =BUFP2WORD(tp);
			mhead.RES    =BUFP2LONG(tp);
			mhead.AV_SIZE=*tp++;
			memcpy(mhead.AV, tp, rd-(USHORT)(tp-readbuf));
			break;
		case FILE_BLK:
			fhead.PSIZE     =BUFP2LONG(tp);
			fhead.SIZE      =BUFP2LONG(tp);
			fhead.FTIME     =BUFP2LONG(tp);
			fhead.ATTR      =BUFP2LONG(tp);
			fhead.CRC32     =BUFP2LONG(tp);
			fhead.TECH.TYPE =*tp++;
			fhead.TECH.QUAL =*tp++;
			fhead.TECH.PARM =BUFP2WORD(tp);
			fhead.RESERVED  =BUFP2WORD(tp);
			fhead.FNAME_SIZE=BUFP2WORD(tp);
			memcpy(fhead.FNAME, tp, rd-(USHORT)(tp-readbuf));
			break;
			//    default: (REC_BLK and future things):
			//              do nothing 'cause isn't needed for extraction
		}

	return (crc_ok);
}
// maximum SFX module size
#define max_sfx_size 65536      // (needed by read_arc_head)

INT read_arc_head(void)         // searches for the archive header and reads it
{
	INT  i,
	flags,
	buf_pos = 0;
	LONG arc_head_pos,
		old_fpos,
	fpos = 0;
	struct stat st;

	fstat(archan, &st);

	memset(buf, 0, size_buf);

#if !defined(__EMX__) && !defined(__OS2__)
	while (ftell(farchan)<st.st_size && fpos < max_sfx_size)
#else
		while (tell(archan)<st.st_size && fpos < max_sfx_size)
#endif
		{
			old_fpos = fpos;
			fpos += read(archan, &buf[buf_pos], size_buf - buf_pos);

			for (i = 0; i < size_buf; i++)    // look for the acesign
			{
				if (!memcmp(acesign, &buf[i], acesign_len))
				{
					// seek to the probable begin
					// of the archive
					arc_head_pos = old_fpos + i - buf_pos -  bytes_before_acesign;
					lseek(archan, arc_head_pos, SEEK_SET);
					if (read_header(0))         // try to read archive header
					{
						flags = mhead.HEAD_FLAGS;
						adat.sol     = (flags & ACE_SOLID) > 0;
						adat.vol     = (flags & ACE_MULT_VOL) > 0;
						adat.vol_num = mhead.VOL_NUM;
						adat.time_cr = mhead.TIME_CR;
						return (1);
					}
				}
			}
			// was no archive header,
			// continue search
			lseek(archan, fpos, SEEK_SET);
			memcpy(buf, &buf[size_buf - 512], 512);
			buf_pos = 512;                    // keep 512 old bytes
		}
	return (0);
}

INT  open_archive(INT print_err)        // opens archive (or volume)
{
	CHAR av_str[80];


#if defined(__OS2_) || defined(__EMX__) || defined(WIN32)
	archan = open(aname, O_RDONLY | O_BINARY);   // open file
#else
	archan = open(aname, O_RDONLY);   // open file
#endif
#if !defined(__EMX__) && !defined(__OS2__)
	farchan = fdopen(archan, "rb");
#endif
	if (archan == -1)
	{
		pipeit("\nError opening file %s", aname);
		return (0);
	}
	if (!read_arc_head())                        // read archive header
	{
		if (print_err)
			pipeit("\nInvalid archive file: %s\n", aname);
#if !defined(__EMX__) && !defined(__OS2__)
		fclose(farchan);
#endif
		close(archan);
		return (0);
	}

	pipeit("\nProcessing archive: %s\n\n", aname);
	if (head.HEAD_FLAGS & ACE_AV)
	{
	   pipeit("Authenticity Verification:");   // print the AV
	   sprintf(av_str, "\ncreated on %d.%d.%d by ",
			   ts_day(adat.time_cr), ts_month(adat.time_cr), ts_year(adat.time_cr));
	   pipeit(av_str);
	   strncpy(av_str, (char *)mhead.AV, mhead.AV_SIZE);
	   av_str[mhead.AV_SIZE] = 0;
	   pipeit("%s\n\n", av_str);
	}
	comment_out("Main comment:");        // print main comment
	return (1);
}

void get_next_volname(void)             // get file name of next volume
{
	CHAR *cp;
	INT  num;

	if ((cp = (CHAR *) strrchr(aname, '.')) == NULL || !*(cp + 1))
		num = -1;
	else
	{
		cp++;
		num = (*(cp + 1) - '0') * 10 + *(cp + 2) - '0';
		if (!in(num, 0, 99))
			num = -1;
		if (in(*cp, '0', '9'))
			num += (*cp - '0') * 100;
	}
	num++;

	if (num < 100)
		*cp = 'C';
	else
		*cp = num / 100 + '0';
	*(cp + 1) = (num / 10) % 10 + '0';
	*(cp + 2) = num % 10 + '0';
}

INT  proc_vol(void)                     // opens volume
{
	INT  i;
	CHAR s[80];

	if (!fileexists(aname) || !f_allvol_pr)
	{
		do
		{
			sprintf(s, "Ready to process %s?", aname);
#if !defined(__MINGW32__)
			beep();
#else
			beep(500,500);
#endif
			i = wrask(s);                  // ask whether ready or not
			f_allvol_pr = (i == 1);        // "Always" --> process all volumes
			if (i >= 2)
			{
				f_err = ERR_FOUND;
				return (0);
			}
		}
		while (!fileexists(aname));
	}

	if (!open_archive(1))                // open volume
	{
		pipeit("\nError while opening archive. File not found or archive broken.\n");
		f_err = ERR_OPEN;
		return (0);
	}

	return (1);
}

INT  proc_next_vol(void)        // opens next volume to process
{
#if !defined(__EMX__) && !defined(__OS2__)
	fclose(farchan);
#endif
	close(archan);               // close handle
	get_next_volname();          // get file name of next volume

	if (!proc_vol())             // try to open volume, read archive header
		return 0;
	if (!read_header(1))         // read 2nd header
	{
		f_err=ERR_READ;
		return 0;
	}
	return 1;
}

INT  read_adds_blk(CHAR * buffer, INT len)      // reads part of ADD_SIZE block
{
	INT  rd = 0,
	l = len;
	LONG i;

#ifdef CRYPT
	char *cbuffer=buffer;

	if (head.HEAD_TYPE == FILE_BLK && (head.HEAD_FLAGS & ACE_PASSW))
		len = crypt_len(len);
#endif /* CRYPT */
	while (!f_err && len && skipsize)
	{
		i = (skipsize > len) ? len : skipsize;
		skipsize -= i;

		/* How do I check error condition when comping -mno-cygwin? */
#if !defined(__MINGW32__)
		errno = 0;
#endif
		rd += read(archan, buffer, i);
#if !defined(__MINGW32__)
		if (errno)
		{
			pipeit("\nRead error\n");
			f_err = ERR_READ;
		}
#endif

		buffer += i;
		len -= i;

		if (!skipsize)            // if block is continued on next volume
			if (head.HEAD_FLAGS & ACE_SP_AFTER && !proc_next_vol())
				break;
	}
#ifdef CRYPT
	if (head.HEAD_TYPE == FILE_BLK && (head.HEAD_FLAGS & ACE_PASSW))
		decrypt(cbuffer, rd);
#endif /* CRYPT */

	return (rd > l ? l : rd);
}

void crc_print(void)            // checks CRC, prints message
{
	INT  crc_not_ok = rd_crc != fhead.CRC32;  /* check CRC of file */

	if (!f_err)                  // print message
	{
		pipeit(crc_not_ok ? "          CRC-check error" : "          CRC OK");
		flush;
	}
}

void analyze_file(void)         // analyzes one file (for solid archives)
{
	pipeit("\n Analyzing");
	flush;
	while (!cancel() && (dcpr_adds_blk(buf_wr, size_wrb))) // decompress only
		;
	crc_print();
}

void extract_file(void)         // extracts one file
{
	INT  rd;

	pipeit("\n Extracting");
	flush;                       // decompress block
	while (!cancel() && (rd = dcpr_adds_blk(buf_wr, size_wrb)))
	{
		if (write(wrhan, buf_wr, rd) != rd)       // write block
		{
			pipeit("\nWrite error\n");
			f_err = ERR_WRITE;
		}
	}
	crc_print();
}

/* extracts or tests all files of the archive
 */
void extract_files(int nopath, int test)
{
	CHAR file[PATH_MAX];

	while (!cancel() && read_header(1))
	{
		if (head.HEAD_TYPE == FILE_BLK)
		{
			comment_out("File comment:");   // show file comment
			ace_fname(file, &head, nopath); // get file name
			pipeit("\n%s", file);
			flush;
			dcpr_init_file();               // initialize decompression of file
			if (!f_err)
			{
				if (test ||
					(wrhan = create_dest_file(file, (INT) fhead.ATTR))<0)
				{
					if (test || adat.sol)
						analyze_file();        // analyze file
				}
				else
				{
					extract_file();           // extract it
#ifdef DOS                               // set file time
					_dos_setftime(wrhan, (USHORT) (fhead.FTIME >> 16), (USHORT) fhead.FTIME);
#endif
					close(wrhan);
#ifdef DOS                               // set file attributes
					_dos_setfileattr(file, (UINT) fhead.ATTR);
#endif
#ifdef AMIGA
					{                         // set file date and time
						struct DateTime dt;
						char Date[9], Time[9];
						ULONG tstamp=fhead.FTIME;

						sprintf(Date, "%02d-%02d-%02d", ts_year(tstamp)-1900, ts_month(tstamp), ts_day(tstamp));
						sprintf(Time, "%02d:%02d:%02d", ts_hour(tstamp), ts_min(tstamp), ts_sec(tstamp));

						dt.dat_Format = FORMAT_INT;
						dt.dat_Flags  = 0;
						dt.dat_StrDate= Date;
						dt.dat_StrTime= Time;

						if (StrToDate(&dt))
							SetFileDate(file, &dt.dat_Stamp);
					}
#endif
					if (f_err)
						remove(file);
				}
			}
		}
	}
}

unsigned percentage(ULONG p, ULONG d)
{
	return (unsigned)( d ? (d/2+p*100)/d : 100 );
}

void list_files(int verbose)
{
	ULONG    size =0,
	psize=0,
	tpsize;
	CHAR     file[PATH_MAX];

	pipeit("Date    |Time |Packed     |Size     |Ratio|File\n");

	while (!cancel() && read_header(1))
	{
		if (head.HEAD_TYPE == FILE_BLK)
		{
			ULONG ti=fhead.FTIME;
			ace_fname(file, &head, verbose ? 0 : 1); // get file name

			size  += fhead.SIZE;
			psize +=
				tpsize = fhead.PSIZE;
			files++;

			while (head.HEAD_FLAGS & ACE_SP_AFTER)
			{
				skipsize=0;
				if (!proc_next_vol())
					break;
				psize += fhead.PSIZE;
				tpsize+= fhead.PSIZE;
			}
			if (!f_err)
				pipeit("%02u.%02u.%02u|%02u:%02u|%c%c%9lu|%9lu|%4u%%|%c%s\n",
					   ts_day (ti), ts_month(ti), ts_year(ti)%100,
					   ts_hour(ti), ts_min  (ti),
					   fhead.HEAD_FLAGS & ACE_SP_BEF   ? '<' : ' ',
					   fhead.HEAD_FLAGS & ACE_SP_AFTER ? '>' : ' ',
					   tpsize, fhead.SIZE, percentage(tpsize, fhead.SIZE),
					   fhead.HEAD_FLAGS & ACE_PASSW    ? '*'    : ' ',
					   file
					  );
		}
	}
	if (!f_err)
	{
		pipeit("\n                 %9lu|%9lu|%4u%%| %u file%s",
			   psize,
			   size,
			   percentage(psize, size),
			   files,
			   (char*)(files == 1 ? "" : "s")
			  );
	}
}

void showhelp(void)
{
	pipeit("\n"
		   "Usage: UNACE <command> <archive[.ace]>\n"
		   "\n"
		   "Where <command> is one of:\n"
		   "\n"
		   "  e   Extract files\n"
		   "  l   List archive\n"
		   "  t   Test archive integrity\n"
		   "  v   List archive (verbose)\n"
		   "  x   Extract files with full path"
		  );
}

int include_unpack(char *bleah)              // processes the archive
{
	CHAR *s;

	strcpy(aname, bleah);

	init_unace();                              // initialize unace

	if (!(s = (CHAR *) strrchr(aname, DIRSEP)))
		s = aname;
	if (!strrchr(s, '.'))
		strcat(aname, ".ACE");

	if (open_archive(1))                       // open archive to process
	{
		if (adat.vol_num)
			pipeit("\nFirst volume of archive required!\n");
		else
			list_files   (0   );

#if !defined(__EMX__) && !defined(__OS2__)
		fclose(farchan);
#endif
		close(archan);
		if (f_err)
		{
			pipeit("\nError occurred\n");
			if (f_criterr)
				pipeit("Critical error on drive %c\n", f_criterr);
		}
	}
	else
		f_err = ERR_CLINE;

	done_unace();
	return (f_err);
}

