/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      Creates/Replaces files or directories.                              */
/*                                                                          */
/* ------------------------------------------------------------------------ */

#include "os.h"

#include <stdlib.h>
#include <fcntl.h>     // AMIGA: open()
#include <stdio.h>     // pipeit() remove()
#include <string.h>    // strncpy()
#include <sys/types.h>
#include <sys/stat.h>  // struct stat
#include <string.h>


#if defined(DOS) || defined(WINNT) || defined(WIN16)
 #include <io.h>       // access()
#endif
#if defined(__IBMC__)
 #include <direct.h>
#endif

#define DIRSEP 	'\\'

#include "attribs.h"
#include "globals.h"
#include "uac_crt.h"
#include "uac_sys.h"

/* gets file name from header
 */
CHAR *ace_fname(CHAR * s, thead * head, INT nopath)
{
	INT  i;
	char *cp;

	strncpy(s, (CHAR *)(*(tfhead *) head).FNAME, i = (*(tfhead *) head).FNAME_SIZE);
	s[i] = 0;

	if (nopath)
	{
		cp=strrchr(s, '\\');
		if (cp)
			memmove(s, cp+1, strlen(cp));
	}

	return s;
}

void check_ext_dir(CHAR * f)        // checks/creates path of file
{
	CHAR *cp,
	d[PATH_MAX];
	INT  i;

	d[0] = 0;

	for (;;)
	{
		if ((cp = (CHAR *) strchr(&f[strlen(d) + 1], DIRSEP))!=NULL)
		{
			i = cp - f;
			strncpy(d, f, i);
			d[i] = 0;
		}
		else
			return;

		if (!fileexists(d))
#if (defined(__OS2__) && !defined(__EMX__)) || (defined(WINNT) && !defined(__CYGWIN__))
			if (mkdir(d))
#else
				if (mkdir(d, 0))
#endif
				{
					f_err = ERR_WRITE;
					pipeit("\n    Error while creating directory.\n");
				}
	}
}

INT  ovr_delete(CHAR * n)           // deletes directory or file
{
	if (remove(n) && rmdir(n))
	{
		pipeit("\n    Could not delete file or directory. Access denied.\n");
		return (1);
	}
	return (0);
}

INT  create_dest_file(CHAR * file, INT a)  // creates file or directory
{
	INT  han,
	i  = 0,
	ex = fileexists(file);
	struct stat st;

	check_ext_dir(file);
	if (f_err)
		return (-1);
	if (a & _A_SUBDIR)
	{                                // create dir or file?
		if (ex)
			stat(file, &st);
#if (defined(__OS2__) && !defined(__EMX__)) || (!defined(__CYGWIN__) && defined(WINNT))
		if ((!ex && mkdir(file)) || (ex && (st.st_mode & S_IFDIR)))
#else
			if ((!ex && mkdir(file, 0)) || (ex && (st.st_mode & S_IFDIR)))
#endif
			{
				pipeit("\n    Could not create directory.\n");
				return (-1);
			}
#ifdef DOS
		_dos_setfileattr(file, a);    // set directory attributes
#endif
		return (-1);
	}
	else
	{
		if (ex)
		{                             // does the file already exist
			if (!f_ovrall)
			{
				i = wrask("Overwrite existing file?");  // prompt for overwrite
				f_ovrall = (i == 1);
				if (i == 3)
					f_err = ERR_USER;
			}
			if ((i && !f_ovrall) || ovr_delete(file))
				return (-1);            // delete?
		}
#if defined(__OS2_) || defined(__EMX__) || defined(WIN32)
		if ((han = open(file, O_WRONLY | O_TRUNC | O_CREAT | O_BINARY,
						S_IREAD | S_IWRITE | S_IEXEC | S_IDELETE |
						S_IRGRP | S_IWGRP  | S_IROTH | S_IWOTH )) < 0)
#else
			if ((han = open(file, O_WRONLY | O_TRUNC | O_CREAT,
							S_IREAD | S_IWRITE | S_IEXEC | S_IDELETE |
							S_IRGRP | S_IWGRP  | S_IROTH | S_IWOTH )) < 0)
#endif
				pipeit("\n    Could not create destination file.\n");
		return (han);
	}
}

