/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      Creates/Replaces files or directories.                              */
/*                                                                          */
/* ------------------------------------------------------------------------ */

#include "os.h"

#ifdef __EMX__
#include <sys/types.h>
#include <sys/dirent.h>
#include <stdlib.h>
#endif
#include <fcntl.h>
#include <stdio.h>     // printf() remove()
#include <string.h>    // strncpy()
#include <sys/types.h>
#include <sys/stat.h>  // struct stat
#include <time.h>
#include "install.h"

#if defined(DOS) || defined(WINNT) || defined(WIN16)
 #include <io.h>       // access()
#endif
#if defined(__CYGWIN__)
#include <Windows32/Base.h>
#endif 

#include "attribs.h"
#include "globals.h"
#include "uac_crt.h"
#include "uac_sys.h"

#ifdef ENABLE_LOGGING
extern FILE *logfile;
#endif
extern char installdir[400];

/* Undocumented functions */
#if defined(__OS2__) || defined(__EMX__)
APIRET APIENTRY DosReplaceModule(PSZ pszOldModule,PSZ pszNewModule,PSZ pszBackupModule);
#endif


/* gets file name from header
 */
CHAR *ace_fname(CHAR * s, thead * head, INT nopath)
{
	INT  i;
	char *cp;

	strncpy(s, (CHAR*)(*(tfhead *) head).FNAME, i = (*(tfhead *) head).FNAME_SIZE);
	s[i] = 0;

	if (nopath)
	{
		cp=strrchr(s, '\\');
		if (cp)
			memmove(s, cp+1, strlen(cp));
	}
#ifdef __UNIX__
	else
    {                                // by current OS seperator
		cp=s;
		while ((cp=strchr(cp, '\\'))!=NULL)
			*cp++='/';
	}
#endif

	return s;
}

void check_ext_dir(CHAR * f)        // checks/creates path of file
{
	char d[1024];
	char buffer[1024];
	int z, flag = 0, len = strlen(f);

	strcpy(buffer, f);
	for(z=len;z>-1;z--)
	{
		if(buffer[z] == '\\')
		{
			buffer[z+1] = 0;
			flag = 1;
			z = -1;
		}
	}
	if(!flag)
		return;
	for(z=0;z<strlen(buffer);z++)
	{
		if(buffer[z] == '\\')
		{
			if(!(z == 2 && buffer[1] == ':'))
			{
				strcpy(d, buffer);
				d[z] = 0;
				if (!fileexists(d))
				{
#if (defined(__OS2__) && !defined(__EMX__)) || (defined(WINNT) && !defined(__CYGWIN__))
					if (mkdir(d))
#else
						if (mkdir(d, 0))
#endif

						{
#if 0
							f_err = ERR_WRITE;
							error("Error while creating directory \"%s\".", d);
						}
						else
#else
				}
#endif					
				{
#ifdef ENABLE_LOGGING
					if(logfile)
					{
						if(strlen(d) > 1 && d[1] == ':')
							fprintf(logfile, "<NewDir>,%s\r\n", d);
						else
						{
							if(installdir[strlen(installdir)-1] == '\\')
								fprintf(logfile, "<NewDir>,%s%s\r\n", installdir, d);
							else
								fprintf(logfile, "<NewDir>,%s\\%s\r\n", installdir, d);
							}
					}
#endif
				}
			}
		}
	}

	}
}

INT  ovr_delete(CHAR * n)           // deletes directory or file
{
	if (remove(n) && rmdir(n))
	{
#if defined(__OS2__) || defined(__EMX__)
		DosReplaceModule(n, NULL, NULL);
#endif
		if (remove(n) && rmdir(n))
		{
			error("Could not delete file or directory: \"%s\" Access denied.", n);
			return (1);
		}
	}
	return (0);
}

INT  create_dest_file(CHAR * file, INT a)  // creates file or directory
{
	INT  han,
	i  = 0,
	ex = fileexists(file);
	struct stat st;
	extern int no_update;

	check_ext_dir(file);
	if (f_err)
		return (-1);
	if (a & _A_SUBDIR)
	{                                // create dir or file?
		if (ex)
		{
			stat(file, &st);
		}

#if (defined(__OS2__) && !defined(__EMX__)) || (!defined(__CYGWIN__) && defined(WINNT))
		if ((!ex && mkdir(file)) || (ex && (st.st_mode & S_IFDIR)))
#else
			if ((!ex && mkdir(file, 0)) || (ex && (st.st_mode & S_IFDIR)))
#endif
			{
				error("Could not create directory %s.");
				return (-1);
			}
			else
			{               /* I wonder why it never gets here... :/ BS */
#ifdef ENABLE_LOGGIN
				if(logfile)
					fprintf(logfile, "<NewDir>,%s\\%s\r\n", installdir, file);
#endif
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
#if defined(__OS2_) || defined(__EMX__)
			static int sddall = 0;
			FILESTATUS3 fileinfo;

			f_ovrall = 1;

			DosQueryPathInfo(file, FIL_STANDARD, &fileinfo, sizeof(FILESTATUS3));
			if(!sddall)
			{
				FDATE fdate;
				FTIME ftime;
				struct tm 	tc, tc2;
				time_t 		tt, tt2;

				*((USHORT*)&fdate) = (USHORT)(fhead.FTIME >> 16);
				*((USHORT*)&ftime) = (USHORT)fhead.FTIME;

				tc.tm_year = fileinfo.fdateLastWrite.year + 80;
				tc.tm_mon = fileinfo.fdateLastWrite.month - 1;
				tc.tm_mday = fileinfo.fdateLastWrite.day;
				tc.tm_hour = fileinfo.ftimeLastWrite.hours;
				tc.tm_min = fileinfo.ftimeLastWrite.minutes;
				tc.tm_sec = fileinfo.ftimeLastWrite.twosecs * 2;

				tc2.tm_year = fdate.year + 80;
				tc2.tm_mon = fdate.month - 1;
				tc2.tm_mday = fdate.day;
				tc2.tm_hour = ftime.hours;
				tc2.tm_min = ftime.minutes;
				tc2.tm_sec = ftime.twosecs * 2;

				if((tt = mktime(&tc)) == -1 || (tt2 = mktime(&tc2)) == -1 || tt > tt2)
				{

					if(file[1] == ':')
						i = confirm("File \"%s\" has a newer modification time. Overwrite?", file);  // prompt for overwrite
					else
					{
						if(installdir[strlen(installdir)-1] == '\\')
							i = confirm("File \"%s%s\" has a newer modification time. Overwrite?", installdir, file);  // prompt for overwrite
						else
							i = confirm("File \"%s\\%s\" has a newer modification time. Overwrite?", installdir, file);  // prompt for overwrite
					}
					if(i == 1)
					{
						sddall = 1;
						i = 0;
					}
					if (i == 3)
						f_err = ERR_USER;
					if(i)
						return -1;
				}
			}
			fileinfo.attrFile = FILE_NORMAL;
			DosSetPathInfo(file, FIL_STANDARD, (PVOID)&fileinfo, sizeof(FILESTATUS3), 0);
#endif
			if (!f_ovrall)
			{
				if(installdir[strlen(installdir)-1] == '\\')
					i = confirm("Overwrite file \"%s%s\"?", installdir, file);  // prompt for overwrite
				else
					i = confirm("Overwrite file \"%s\\%s\"?", installdir, file);  // prompt for overwrite
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
				error("Could not create destination file \"%s\".", file);
		else
		{
#ifdef ENABLE_LOGGING
			if(logfile)
			{
				if(!no_update)
				{
					if(strlen(file) > 1 && file[1] == ':')
						fprintf(logfile, "<FileInst>,%s\r\n", file);
					else
					{
						if(installdir[strlen(installdir)-1] == '\\')
							fprintf(logfile, "<FileInst>,%s%s\r\n", installdir, file);
						else
							fprintf(logfile, "<FileInst>,%s\\%s\r\n", installdir, file);
					}
				}
			}
#endif
		}

		return (han);
	}
}

