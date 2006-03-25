/*
 *  install.c (c) 1998,1999 Brian Smith
 *  parts by Daniele Vistalli
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#if defined(__OS2__) || defined(__EMX__) || defined(WINNT) || defined(WIN32)
#include <process.h>
#endif
#include <sys/types.h>
#include "install.h"
#include "instsup.h"

#if !defined(__EMX__) && !defined(__OS2__) && !defined(WIN32) && !defined(WINNT)
#define stricmp strcasecmp
#endif

/* My Global variables ;) unusually many due to multiple dialogs */
char tempPath[MAX_PATH];
int installstate = NONE;
int installstage = 0;
int current_file=0, success=0;
unsigned long int acepos=0, aceoffset=0;
int pixels=0;
char confirmstring[1024];
/* I know I am being excessive but... better safe than sorry ;) */
char *configsys[8196];
int configfilecount=-1;
int files = 0, files_deleted=0, packagesize=0, packagesselected[20];
/* Global flags often set by the user */
int driveselected, packagechosen, express = 1, driverstatus = -1, no_update = 0;
int licensechecked = 0, custom = 0, checkforupdate = 1, downloadsite = 0;
int usescitech = 1, usecurrent = 0, checking = 0, checkerror = 0, newerver = 0;
char sddfilename[256] = "",  sddversion[256] = "",  sdddate[256] = "";
char sddurl[4][256] = { "", "", "", "" };

/* So these are accessible to REXX */
int drivelist[26];
FILE *self;

/* These get loaded in loadheader */
char *INSTALLER_APPLICATION;
char *INSTALLER_VERSION;
char *INSTALLER_TITLE;
char *INSTALLER_PATH;
char *INSTALLER_FOLDER;
char *INSTALLER_PROGRAM;
char *INSTALLER_SHADOW;
char *INSTALLER_OBJECT;
char *INSTALLER_SETS;
char *INSTALLER_SYSVAR;
char *INSTALLER_SYSLINE;
char *INSTALLER_PACKAGES[20];
char *INSTALLER_CONFIRM_WPS;
char *INSTALLER_CONFIRM_CONFIGSYS;
char *INSTALLER_CONFIRM_OVERWRITE;
int INSTALLER_PACKAGE_COUNT;

/* Will have to seperate this stuff to platform specifics... */

/* Config.Sys -- Note the drive letter gets replaced with the boot drive letter
                 It is just a place holder. (For the next 3 entries)             */
char csfile[] = "C:\\CONFIG.SYS";
/* Backup Config.Sys filename */
char bufile[] = "C:\\CONFIG.SDD";
/* Installation Log Database -- Used for uninstallation and aborting */
#if defined(__OS2__) || defined(__EMX__) || defined(WINNT) || defined(WIN32)
char instlog[] = "C:\\DBINST.LOG";
#else
char instlog[] = "~/dbinst.log";
#endif
char installdir[400];

char bootdrive[2] = "C";
char winpath[400] = "C:\\OS2\\MDOS\\WINOS2";
char winjpath[400] = "C:\\OS2\\MDOS\\WINJOS2";
char wintpath[400] = "C:\\OS2\\MDOS\\WINTOS2";
char winhpath[400] = "C:\\OS2\\MDOS\\WINHOS2";
char browsedir[400] = "C:\\";
char installdir2[400] = "";
char empty_string[] = "";
char currentcf[400] = "";

#ifdef ENABLE_LOGGING
FILE *logfile;
#endif

/* Function prototypes */
int installer_unpack(char *filename, int operation);
void resetglobals(void);

typedef struct _replacements {
	char *replacestring, *replacevar;
} Replacements;

/* The variables in this array must be static buffers */
Replacements InstRep[] = {
	{ "%INSTALLPATH%",   installdir },
	{ "%BOOTDRIVE%",     bootdrive },
	{ "%ANYSTRING%",     empty_string },
	{ "%WINPATH%",       winpath },
	{ "%WINJPATH%",      winjpath },
	{ "%WINTPATH%",      wintpath },
	{ "%WINHPATH%",      winhpath },
	{ "%USERPATH%",      installdir2 },
	{ "%VERSION%",       "1.0" }, /* This may be depricated */
	{ "%WEB_VER%",       sddversion },
	{ "%WEB_DATE%",      sdddate },
	{ "%WEB_LOCATION1%", sddurl[0] },
	{ "%WEB_LOCATION2%", sddurl[1] },
	{ "%WEB_LOCATION3%", sddurl[2] },
	{ "%WEB_LOCATION4%", sddurl[3] },
	{ "%PRI%",           "SVGA" }
};
int replacemax = 16;

/* In str1, str2 gets replaced by str3 */
char *replacestr(char *str1, char *str2, char *str3)
{
	char bigbuffer[4096];
	int z, x=0, len1 = strlen(str1), len2 = strlen(str2), len3 = strlen(str3);

	for(z=0;z<len1;z++)
	{
		if(len2 > 0 && strncmp(&str1[z], str2, len2)==0)
		{
			int i;
			for(i=0;i<len3;i++)
			{
				bigbuffer[x] = str3[i];
				x++;
			}
			z=z+(len2-1);
		} else {
			bigbuffer[x] = str1[z];
			x++;
		}
	}
	bigbuffer[x] = 0;
	return (char *)strdup(bigbuffer);
}

/* This function parses a string and replaces all the text in the
 * Replacement array with the current dynamic text */
char *replaceem(char *orig)
{
	char *tmp1 = NULL, *tmp2 = (char *)strdup(orig);
	int z;

	for(z=0;z<replacemax;z++)
	{
		tmp1 = replacestr(tmp2, InstRep[z].replacestring, InstRep[z].replacevar);
		free(tmp2);
		tmp2 = tmp1;
		tmp1 = NULL;
	}
	return tmp2;
}

/*
 * Find the offset withing the executable of the ace data for use.
 */
int aceseek_entry(int num)
{
	long headerstart;
	char headerbuf[20], packageconfig[100];
	int z, start = 0, entry = 0, packageoffset=0;
	/* Decode DBSOFT-ACE - small memleak but
	 * install should not be running for very long. :) */
	sprintf(headerbuf, "%s%d", decode("EEECFDEPEGFECNEBEDEF"), num);
	if((headerstart = findtext(headerbuf)) < 0)
	{
		mesg("Could not find ACE header in executable.");
		exit(2);
	}

	fseek(self, headerstart+strlen(headerbuf), SEEK_SET);
	fread(packageconfig, 1, 100, self);

	for(z=0;z<100;z++)
	{
		if(packageconfig[z] == '-' || packageconfig[z] == '*')
		{
			char cur = packageconfig[z];
			packageconfig[z] = 0;
			switch(entry)
			{
			case 0:
				current_file = 0;
				files = atoi(&packageconfig[start]);
				break;
			case 1:
				packagesize = atoi(&packageconfig[start]);
				break;
			}
			start = z+1;
			if(cur == '*')
			{
				packageoffset = z + 1;
				z = 100;
			}
			entry++;
		}
	}
	aceoffset=headerstart+strlen(headerbuf)+packageoffset;
	fseek(self, aceoffset, SEEK_SET);
	return TRUE;
}

/*
 * Reads the embedded .cfg data from the executable and place it in the global
 * INSTALL_* variables for use.
 */
int loadheader(void)
{
	char *buffer;
	long headerstart;
	int z, start=0, entry=0;

	buffer = malloc(8096*4);

	/* Decode DBSOFT-HEADER */
	if((headerstart = findtext(decode("EEECFDEPEGFECNEIEFEBEEEFFC"))) < 0)
	{
		mesg("Could not find Selfinstaller header in executable.");
		exit(2);
	}
	fseek(self, headerstart+13, SEEK_SET);

	fread(buffer, 1, 8096*4, self);
	for(z=0;z<8096*4;z++)
	{
		if(buffer[z] == '-' || buffer[z] == '*')
		{
			char cur = buffer[z];
			buffer[z] = 0;
			switch(entry)
			{
			case 0:
				INSTALLER_APPLICATION = (char *)strdup(&buffer[start]);
				break;
			case 1:
				INSTALLER_VERSION = (char *)strdup(&buffer[start]);
				break;
			case 2:
				INSTALLER_TITLE = (char *)strdup(&buffer[start]);
				break;
			case 3:
				INSTALLER_PATH = (char *)strdup(&buffer[start]);
				break;
			case 4:
				INSTALLER_FOLDER = (char *)strdup(&buffer[start]);
				break;
			case 5:
				INSTALLER_PROGRAM = (char *)strdup(&buffer[start]);
				break;
			case 6:
				INSTALLER_SHADOW = (char *)strdup(&buffer[start]);
				break;
			case 7:
				INSTALLER_OBJECT = (char *)strdup(&buffer[start]);
				break;
			case 8:
				INSTALLER_SETS = (char *)strdup(&buffer[start]);
				break;
			case 9:
				INSTALLER_SYSVAR = (char *)strdup(&buffer[start]);
				break;
			case 10:
				INSTALLER_SYSLINE = (char *)strdup(&buffer[start]);
				break;
			case 11:
				INSTALLER_CONFIRM_WPS = (char *)strdup(&buffer[start]);
				break;
			case 12:
				INSTALLER_CONFIRM_CONFIGSYS = (char *)strdup(&buffer[start]);
				break;
			case 13:
				INSTALLER_CONFIRM_OVERWRITE = (char *)strdup(&buffer[start]);
				break;
			case 14:
				INSTALLER_PACKAGE_COUNT = atoi(&buffer[start]);
				break;
			default:
				INSTALLER_PACKAGES[entry-15] = malloc((z-start)+1);
				strcpy(INSTALLER_PACKAGES[entry-15], &buffer[start]);
				break;
			}
			start = z+1;
			if(cur == '*')
			{
				free(buffer);
				return TRUE;
			}

			entry++;
		}
	}
	free(buffer);
	return FALSE;
}

/*
 * Functions to work on an ace file embedded within the archive in an
 * abstract manner.
 */
int aceread(void *buf, size_t count)
{
	unsigned long curpos = ftell(self);
	size_t readit;

	if(count >  (packagesize-(curpos-aceoffset)))
		readit = (packagesize-(curpos-aceoffset));
	else
		readit = count;

	return fread(buf, 1, readit, self);
}

off_t acelseek(off_t offset, int whence)
{
	switch(whence)
	{
	case SEEK_SET:
		fseek(self, aceoffset+offset, SEEK_SET);
		break;
	case SEEK_CUR:
		fseek(self, offset, SEEK_CUR);
		break;
	}
    acepos = ftell(self);
	return acepos-aceoffset;
}

int aceopen(const char *path, int flags)
{
	fseek(self, aceoffset, SEEK_SET);
	return 1;
}

int aceclose(int fd)
{
	fseek(self, aceoffset, SEEK_SET);
	return  0;
}

int acesize(void)
{
	return packagesize;
}

int acetell(int fd)
{
	return ftell(self)-aceoffset;
}

/*
 * Read the generated log file and remove any files installed.
 */
void delete_files(void)
{
	char tmpbuf[8196], *fileptr;
	FILE *tmplf;
	int linenum=0, found=-1, z;

	files_deleted=1;

	if((tmplf=fopen(instlog, "rb"))==NULL)
		return;

	while(!feof(tmplf))
	{
		fgets(tmpbuf, 8196, tmplf);
		linenum++;
		if(tmpbuf[0]=='[' && (char *)strstr(tmpbuf, INSTALLER_APPLICATION) != NULL && !feof(tmplf))
		{
			fgets(tmpbuf, 8196, tmplf);
			linenum++;
			if((char *)strstr(tmpbuf, "<Version>") != NULL && (char *)strstr(tmpbuf, INSTALLER_VERSION) != NULL)
				found=linenum;
		}
	}
	if(found != -1)
	{
		rewind(tmplf);
		for (z=0;z<found;z++)
			fgets(tmpbuf, 8196, tmplf);
		while(!feof(tmplf))
		{
			fgets(tmpbuf, 8196, tmplf);
			if((char *)strstr(tmpbuf, "<FileInst>") != NULL)
			{
				fileptr = (char *)strchr(tmpbuf, ',')+1;
				/* Remove trailing CRLFs */
				if(fileptr[strlen(fileptr)-1] == '\r' || fileptr[strlen(fileptr)-1] == '\n')
					fileptr[strlen(fileptr)-1]=0;
				if(fileptr[strlen(fileptr)-1] == '\r' || fileptr[strlen(fileptr)-1] == '\n')
					fileptr[strlen(fileptr)-1]=0;
				remove(fileptr);
				current_file--;
#if 0				
				sendmessage(0, 1);
#endif				
			}
			if((char *)strstr(tmpbuf, "<End>") != NULL)
			{
				fclose(tmplf);
				return;
			}
		}
	}
	fclose(tmplf);
	return;
}

/*
 * Reads a config file into memory for editing with updatesys, updateset, etc.
 */
int readconfigfile(char *filename)
{
	char tmpbuf[8196];
	FILE *tmpcs;

	/* Reset this value when restarting */
	configfilecount = -1;

	if((tmpcs=fopen(filename, "rb"))==NULL)
	{
		strcpy(currentcf, empty_string);
		return 1;
	}
	else
		strcpy(currentcf, filename);

	while(!feof(tmpcs))
	{
		configfilecount++;
		fgets(tmpbuf, 8196, tmpcs);
		configsys[configfilecount] = malloc(strlen(tmpbuf)+1);
		strcpy(configsys[configfilecount], tmpbuf);
        stripcrlf(configsys[configfilecount]);
	}

	fclose(tmpcs);
	return 0;
}

/*
 * Write the updated config file to disk and backup the original.
 */
int writeconfigfile(char *filename, char *backup)
{
	FILE *tmpcs;
	int i;

	if(backup)
	{
		remove(backup);
		rename(filename, backup);
	}
	else
		remove(filename);

	if((tmpcs=fopen(filename, "wb"))==NULL)
		return 1;

	for(i=0;i<configfilecount;i++)
	{
		if(configsys[i])
		{
			fwrite(configsys[i], 1, strlen(configsys[i]), tmpcs);
			/* Add the CRLF that got stripped in the reading stage. */
			fwrite("\r\n", 1, 2, tmpcs);
			free(configsys[i]);
		}
	}

	fclose(tmpcs);
	return 0;
}
/*
 * Adds or replaces a SET variable based on the flags (CONFIG.SYS)
 */
void updateset(char *setname, char *newvalue, int flag)
{
	char *cmpbuf1, *cmpbuf2, *tmpptr, *tmpptr2, *nv;
	int i, z, t;

	nv=replaceem(newvalue);

	cmpbuf1=malloc(strlen(setname)+2);
	strcpy(cmpbuf1, setname);
	strcat(cmpbuf1, "=");
	for(i=0;i<configfilecount;i++)
	{
		if(strlen(cmpbuf1) <= strlen(configsys[i]))
		{
			tmpptr=(char *)strdup(configsys[i]);
#if defined(__EMX__) || defined(__OS2__) || defined(WIN32) || defined(WINNT)
			strupr(tmpptr);
#endif
			if((tmpptr2=(char*)strstr(tmpptr, "SET "))!=NULL)
			{
				tmpptr2 += 4;
				cmpbuf2=malloc(strlen(tmpptr2)+1);
				/* Remove any spaces from the string */
				z=0;
				for (t=0;t<strlen(tmpptr2) && z < strlen(cmpbuf1);t++)
				{
					if(tmpptr2[t] != ' ')
					{
						cmpbuf2[z]=tmpptr2[t];
						z++;
					}
				}
				cmpbuf2[z]=0;
				if(stricmp(cmpbuf1, cmpbuf2) == 0)
				{
					/* Ok we found the entry, and if UPDATE_ALWAYS change it to the
					 new entry, otherwise exit */
					if(flag == UPDATE_ALWAYS)
					{
#ifdef ENABLE_LOGGING
						fprintf(logfile, "<CFRemLine>,%s,%s\r\n", currentcf, configsys[i]);
#endif
						free(configsys[i]);
						configsys[i] = malloc(strlen(setname)+strlen(nv)+6);
						strcpy(configsys[i], "SET ");
						strcat(configsys[i], setname);
						strcat(configsys[i], "=");
						strcat(configsys[i], nv);
#ifdef ENABLE_LOGGING
						fprintf(logfile, "<CFAddLine>,%s,%s\r\n", currentcf, configsys[i]);
#endif
						free(cmpbuf1);free(cmpbuf2);free(tmpptr);
					}
					return;
				}
				free(cmpbuf2);
			}
			free(tmpptr);
		}
	}
	/* Couldn't find the line so we'll add it */
	configsys[configfilecount]=malloc(strlen(cmpbuf1)+strlen(nv)+6);
	strcpy(configsys[configfilecount], "SET ");
	strcat(configsys[configfilecount], setname);
	strcat(configsys[configfilecount], "=");
	strcat(configsys[configfilecount], nv);
#ifdef ENABLE_LOGGING
	fprintf(logfile, "<CFAddLine>,%s,%s\r\n", currentcf, configsys[configfilecount]);
#endif
	configfilecount++;
	free(cmpbuf1);
}     

/*
 * Adds an entry to a system variable (CONFIG.SYS)
 */
void updatesys(char *sysname, char *newvalue)
{
	char *cmpbuf1, *cmpbuf2, *tmpptr, *tmpptr2, *capbuf1, *capbuf2, *nv, *brian;
	int i, z, t;

	nv=replaceem(newvalue);

	cmpbuf1=malloc(strlen(sysname)+2);
	strcpy(cmpbuf1, sysname);
	strcat(cmpbuf1, "=");
	for(i=0;i<configfilecount;i++)
	{
		if(strlen(cmpbuf1) <= strlen(configsys[i]))
		{
			cmpbuf2=malloc(strlen(configsys[i])+1);
			/* Remove any spaces from the string */
			z=0;
			for (t=0;t<strlen(configsys[i]) && z < strlen(cmpbuf1);t++)
			{
				if(configsys[i][t] != ' ')
				{
					cmpbuf2[z]=configsys[i][t];
					z++;
				}
			}
			cmpbuf2[z]=0;
			if(stricmp(cmpbuf1, cmpbuf2) == 0)
			{
				/* Do a case insensitive comparison but preserve the case */
				tmpptr = &configsys[i][t];
				capbuf1=malloc(strlen(tmpptr)+1);
				capbuf2=malloc(strlen(nv)+1);
				strcpy(capbuf1, tmpptr);
				strcpy(capbuf2, nv);
#if defined(__EMX__) || defined(__OS2__) || defined(WINNT) || defined(WIN32)				
				strupr(capbuf1);
				strupr(capbuf2);
#endif				
				/* Ok, we found the line, and it doesn't have an entry so we'll add it */
				if((tmpptr2=(char *)strstr(capbuf1, capbuf2)) == NULL)
				{
#ifdef ENABLE_LOGGING
					fprintf(logfile, "<CFRemLine>,%s,%s\r\n", currentcf, configsys[i]);
#endif
					brian = configsys[i];
					configsys[i] = malloc(strlen(configsys[i])+strlen(nv)+4);
					strcpy(configsys[i], brian);
					free(brian);
					/* Remove any trailing CRLFs */
					if(configsys[i][strlen(configsys[i])-1]!=';')
						strcat(configsys[i], ";");
					strcat(configsys[i], nv);
					strcat(configsys[i], ";");
#ifdef ENABLE_LOGGING
					fprintf(logfile, "<CFAddLine>,%s,%s\r\n", currentcf, configsys[i]);
#endif
				}
				free(cmpbuf1);free(cmpbuf2);free(capbuf1);free(capbuf2);
				return;
			}
			free(cmpbuf2);
		}
	}
	/* Couldn't find the line so we'll add it */
	configsys[configfilecount]=malloc(strlen(cmpbuf1)+strlen(nv)+3);
	strcpy(configsys[configfilecount], cmpbuf1);
	strcat(configsys[configfilecount], nv);
	strcat(configsys[configfilecount], ";");
#ifdef ENABLE_LOGGING
	fprintf(logfile, "<CFAddLine>,%s,%s", currentcf, configsys[configfilecount]);
#endif
	configfilecount++;
	free(cmpbuf1);
	if(nv)
		free(nv);
}

/*
 * Removes a line from a config file.
 */
void removeline(char *text)
{
	int z;
	for(z=0;z<configfilecount;z++)
	{
		if(stricmp(configsys[z], text) == 0)
		{
			int t;

#ifdef ENABLE_LOGGING
			fprintf(logfile, "<CFRemLine>,%s,%s\r\n", currentcf, configsys[z]);
#endif
			free(configsys[z]);
			for(t=z;t<(configfilecount-1);t++)
				configsys[t] = configsys[t+1];
			configfilecount--;
		}
	}
}


/*
 * The Window peocedure for the confirmation dialog.
 */
#if 0 
MRESULT	EXPENTRY ConfirmDlgProc(HWND hWnd, ULONG msg, MPARAM mp1,	MPARAM mp2)
{
	SWP winpos;

	switch (msg)
	{
	case WM_INITDLG:
		WinSetWindowText(hWnd, INSTALLER_TITLE);
		WinEnableWindow(WinWindowFromID(mainhwnd, I_Cancel), FALSE);
		WinSetDlgItemText(hWnd,	I_Confirm, confirmstring);
		WinQueryWindowPos(mainhwnd, &winpos);
		WinSetWindowPos(hWnd, HWND_TOP, winpos.x+30, winpos.y+30, 0, 0, SWP_MOVE | SWP_ZORDER);
		break;
	case WM_COMMAND:
		WinEnableWindow(WinWindowFromID(mainhwnd, I_Cancel), TRUE);
		switch ( SHORT1FROMMP(mp1) )
		{
		case I_Ja:
			WinDismissDlg(hWnd, 0);
			break;
		case I_Alle:
			WinDismissDlg(hWnd, 1);
			break;
		case I_Nein:
			WinDismissDlg(hWnd, 2);
			break;
		case I_Halt:
			success=2;
			installstate=ABORTED;
			WinDismissDlg(hWnd, 3);
			break;
		}
		break;
	default :
		return(WinDefDlgProc(hWnd, msg, mp1, mp2));
	}
	return(0L);
}
#endif
/*
 * Display a confirmation dialog with the options: YES NO ALL CANCEL
 * Returns: 0 for YES, 1 for ALL, 2 for NO and 3 for CANCEL
 */
int confirm(char *format, ...) {
	va_list args;

	/* if no confirmation, return 1, meaning overwrite all */
	if (stricmp(INSTALLER_CONFIRM_OVERWRITE, "no") == 0)
		return 1;

	va_start(args, format);
	vsprintf(confirmstring, format, args);
	va_end(args);

	/* Do something here in wxwindows */
	return 1;
}

/*
 * A function to grab a file from an embedded archive and extract it to the TEMP directory.
 */
void grabfile(char *filename)
{
	no_update = 1;
	settempdir();
	remove(filename);
	aceseek_entry(0);
	resetglobals();
	installer_unpack(filename, 2);
	no_update = 0;
}

/*
 * This thread runs along side the main thread allowing the user to cancel the process.
 */
void install_thread(void *param)
{
	char tmpinstallpath[1024];
	int  k, j, installcount=0, installed=0;

	if(INSTALLER_PACKAGE_COUNT == 2)
		packagesselected[1] = TRUE;
			
	installstate = INSTALLING;

#ifdef ENABLE_LOGGING
	if((logfile=fopen(instlog, "ab"))==NULL)
	{
		error("Log file \"%s\" open failed! Installation aborted!", instlog);
		exit(1);
	}

	fprintf(logfile, "[%s]\r\n<Version>,%s\r\n<Start>\r\n", INSTALLER_APPLICATION, INSTALLER_VERSION);
#endif

	/* Create nested subdirectories if necessary. */
	strcpy(tmpinstallpath, installdir);
	for(k=3;k<strlen(installdir);k++)
	{
		if(tmpinstallpath[k] == '\\')
		{
			tmpinstallpath[k] = 0;
#if defined(__EMX__) || defined(__CYGWIN__) || defined(UNIX)
			if (!mkdir(tmpinstallpath, 0))
#else
			if (!mkdir(tmpinstallpath))
#endif
#ifdef ENABLE_LOGGING
				fprintf(logfile, "<NewDir>,%s\r\n", tmpinstallpath);
#else
				;
#endif
			tmpinstallpath[k] = '\\';
		}
	}

#if defined(__EMX__) || defined(__CYGWIN__) || defined(UNIX)
			if (!mkdir(installdir, 0))
#else
			if (!mkdir(installdir))
#endif
#ifdef ENABLE_LOGGING
		fprintf(logfile, "<NewDir>,%s\r\n", installdir);
#else
		;
#endif

	/*if(strlen(installdir) > 0 && installdir[0] > 'a'-1 && installdir[0] < 'z'+1)
		installdir[0]=installdir[0] - ('a'-'A');
	if(strlen(installdir)>2 && installdir[1]==':' && installdir[2]=='\\')
		DosSetDefaultDisk((int)(installdir[0]-'A'+1));*/

	setdrivedir(installdir);

	/* Unpack files to destination directory */
	for(j=1;j<INSTALLER_PACKAGE_COUNT;j++)
	{
		if(packagesselected[j] == TRUE)
			installcount++;
	}
	if(installcount == 0)
	{
		mesg("No packages selected for installation!");
	}
	else
	{
		for(j=1;j<INSTALLER_PACKAGE_COUNT;j++)
		{
			if(packagesselected[j] == TRUE)
			{
#if 0				
				char statustext[512];
#endif				
				aceseek_entry(j);
				resetglobals();
#if 0
				sprintf(statustext,"Copying Files for %s %d/%d, Press \"Exit Installation\" to Abort.", INSTALLER_PACKAGES[j], installed+1, installcount);
				WinSetDlgItemText(mainhwnd,	I_Info3, statustext);
#endif
				/* filename parameter when null is all files */
				installer_unpack(NULL, 2);
				installed++;
			}
		}
	}

	if(success==1)
	{
		error("Error unpacking files, Installer may be corrupt!");
	}

	if(success==2)
	{
		error("User aborted installation!");
	}

	if(installstate != ABORTED)
		installstate = COMPLETED;

#if 0
	sendmessage(0, 2);
#endif	
}

/*
 * Use the information from the .cfg file which is embedded in the EXE to update the
 * CONFIG.SYS settings.
 */
void configsys_update(void)
{
	char *arg1, *arg2, *arg3;
	char temp[5000];
	int z, argn=0;

	if(readconfigfile(csfile))
		return;

	/* Update Miscellaneous lines */
	if(strlen(INSTALLER_SYSLINE)>0)
	{
		char *tmpptr = &temp[0];
        int len;

		strcpy(temp, INSTALLER_SYSLINE);
		temp[4999] = 0;
		len = strlen(temp);

		for(z=0;z<len;z++)
		{
			if(temp[z]==',')
			{
				char *tmpptr2;

				temp[z] = 0;
				tmpptr2 = replaceem(tmpptr);
				tmpptr = &temp[z+1];
				removeline(tmpptr2);
				configsys[configfilecount] = tmpptr2;
#ifdef ENABLE_LOGGING
				fprintf(logfile, "<CFAddLine>,%s,%s\r\n", currentcf, tmpptr2);
#endif
				configfilecount++;
			}
		}
		if(tmpptr && *tmpptr)
		{
			char *tmpptr2;

			tmpptr2 = replaceem(tmpptr);
			removeline(tmpptr2);
			configsys[configfilecount] = tmpptr2;
#ifdef ENABLE_LOGGING
			fprintf(logfile, "<CFAddLine>,%s,%s\r\n", currentcf, tmpptr2);
#endif
			configfilecount++;
		}

	}
	/* Update SET variables */
	if(strlen(INSTALLER_SETS)>0)
	{
		strcpy(temp, INSTALLER_SETS);
		argn=0;
		arg1=&temp[0];
		arg2=arg3=NULL;
		for(z=0;z<strlen(INSTALLER_SETS);z++)
		{
			if(temp[z]==',')
			{
				argn++;
				temp[z]=0;
				switch(argn)
				{
				case 1:
					arg2=&temp[z+1];
					break;
				case 2:
					arg3=&temp[z+1];
					break;
				case 3:
					argn=0;
					updateset(arg1, arg2, (int)(arg3[0]-'0'));
					arg1=&temp[z+1];
					arg2=arg3=NULL;
					break;
				}
			}
		}
		if(arg3)
			updateset(arg1, arg2, (int)arg3-'0');
	}
	/* Update system variables */
	if(strlen(INSTALLER_SYSVAR)>0)
	{
		strcpy(temp, INSTALLER_SYSVAR);
		argn=0;
		arg1=&temp[0];
		arg2=NULL;
		for(z=0;z<strlen(INSTALLER_SYSVAR);z++)
		{
			if(temp[z]==',')
			{
				argn++;
				temp[z]=0;
				switch(argn)
				{
				case 1:
					arg2=&temp[z+1];
					break;
				case 2:
					argn=0;
					updatesys(arg1, arg2);
					arg1=&temp[z+1];
					arg2=NULL;
					break;
				}
			}
		}
		if(arg2)
			updatesys(arg1, arg2);
	}

	writeconfigfile(csfile, bufile);
}

/*
 * Reads a line from a file and returns it in raw.
 */
void getline(FILE *f, char *raw)
{
	memset(raw, 0, 256);
	fgets(raw, 255, f);
	stripcrlf(raw);
}

/*
 * Removes a character from a buffer by advancing the buffer to the left.
 */
void removechar(char *buffer, int thisone, int len)
{
	int x;

	for(x=thisone;x<len;x++)
		buffer[x] = buffer[x+1];
}

/*
 * Breaks up a line in raw into it's components.
 */
void parseline(char *raw, char comment, char delimiter, char quotes, char *entry, char *entrydata, char *entrydata2)
{
    char in[256];
	int z, len, in_quotes = 0, entrynum=0, last = 0;

	strcpy(entry, empty_string);
	strcpy(entrydata, empty_string);
	strcpy(entrydata2, empty_string);
	strcpy(in, raw);

	if(in[strlen(in)-1] == '\n')
		in[strlen(in)-1] = 0;

	if(in[0] != comment)
	{
        len=strlen(in);
		for(z=0;z<len;z++)
		{
			if(!in_quotes && in[z] == delimiter)
			{
				in[z] = 0;
				/* Strip any other delimiters */
				z++;
				while(in[z] == delimiter && z < len)
					z++;
				if(!entrynum)
					strcpy(entry, in);
				else
				{
					strcpy(entrydata, &in[last]);
					strcpy(entrydata2, &in[z]);
				}
				last = z;
				if(entrynum)
					return;
				entrynum++;
			}
			if(in[z] == quotes)
			{
				removechar(in, z, len);
				z--;
				len--;
				if(in_quotes)
					in_quotes = 0;
				else
					in_quotes = 1;
			}
		}
		if(!entrynum)
			strcpy(entry, in);
		else
			strcpy(entrydata, &in[last]);
	}
}

/*
 * Reads a line from the file and splits it up into it's components.
 */
int getparseline(FILE *f, char comment, char delimiter, char quotes, char *raw, char *entry, char *entrydata, char *entrydata2)
{
	getline(f, raw);
	parseline(raw, comment, delimiter, quotes, entry, entrydata, entrydata2);
	return strlen(raw);
}


void install_init(char *installername)
{
	if((self = fopen(installername, "rb")) == NULL)
	{
		mesg("Could not open SFX archive for reading!");
		exit(1);
	}
	if(loadheader() == FALSE)
	{
		mesg("Could not load all required variables!");
		exit(3);
	}
 
	strcpy(installdir, INSTALLER_PATH);
}


