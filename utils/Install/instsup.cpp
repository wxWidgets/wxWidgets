/*
 *  instsup.c (c) 1999,2000 Brian Smith
 */

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#if defined(__OS2__) || defined(__EMX__) || defined(WIN32) || defined(WINNT)
#include <process.h>
#endif
#include <sys/types.h>
#ifdef WIN32
#include <shlobj.h>
#endif
#include "install.h"
#include "instsup.h"

extern char *INSTALLER_TITLE;
extern char *INSTALLER_PROGRAM;
extern char *INSTALLER_FOLDER;
extern char *INSTALLER_SHADOW;
extern char *INSTALLER_OBJECT;
extern char tempPath[], installdir[], csfile[], bufile[], bootdrive[], instlog[], installdir2[];
extern int installstate, success;

extern FILE *self;

#ifdef __cplusplus
extern "C" {
#endif

char *replaceem(char *orig);

int sendmessage(int destination, int messid)
{
#if 0
	/* Update percentage bar */
	if(messid == 1)
	{
		wxMutexGuiEnter();

		updatepercent();

		wxMutexGuiLeave();
	}
	if(messid == 2)
	{
		extern wxCondition *InstCond;

		InstCond->Broadcast();
	}

#endif
	return 0;
}

void DoGUI(void)
{
	updatepercent();
	wxYield();
}
/* This should return the current color depth */
unsigned long color_depth(void)
{
#if __OS2__
	HDC hdc = WinOpenWindowDC(HWND_DESKTOP);
	LONG colors;

	DevQueryCaps(hdc, CAPS_COLORS, 1, &colors);
	DevCloseDC(hdc);
	return colors;
#endif
	return 0;
}

/*
 * Call the reboot vector.
 */
void sysreboot(void)
{
#if __OS2__
#define SYSFUNC               0xD5
#define REBOOT                0xAB
#define REBOOTDEV             "\\DEV\\DOS$"

	APIRET rc;
	HFILE  hREBOOT;
	ULONG  ulAction;

	rc = DosOpen(REBOOTDEV,
				 &hREBOOT,
				 &ulAction,
				 0L,
				 FILE_NORMAL,
				 FILE_OPEN,
				 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,
				 0L);
	if (rc == 0)
	{
		DosDevIOCtl(hREBOOT,
					SYSFUNC,
					REBOOT,
					NULL,
					0L,
					NULL,
					NULL,
					0L,
					NULL);
		DosClose(hREBOOT);
	}
#endif
}

/*
 * Display an informational dialog box to the user with the given text.
 */
int mesg(char *format, ...) {
	va_list args;
	char outbuf[4096];

	va_start(args, format);
	vsprintf(outbuf, format, args);
	va_end(args);

	wxMessageBox(outbuf, INSTALLER_TITLE,
				 wxOK | wxICON_EXCLAMATION, NULL);

	return strlen(outbuf);
}

int checktext(char *text, char *buffer, int buflen)
{
	int z, len = strlen(text);

	for(z=0;z<(buflen-len);z++)
	{
		if(memcmp(text, &buffer[z], len) == 0)
			return z;
	}
	return -1;

}

/*
 * Returns the offset withing the executable to the specified text.
 */
long findtext(char *text)
{
	char buffer[512];
	int offset;
	unsigned long curpos = 0;

	fseek(self, 0, SEEK_SET);
	fread(buffer, 1, 512, self);
	if((offset = checktext(text, buffer, 512)) > -1)
		return offset;
	while(!feof(self))
	{
		memcpy(buffer, &buffer[256], 256);
		fread(&buffer[256], 1, 256, self);
		curpos += 256;
		if((offset = checktext(text, buffer, 512)) > -1)
			return offset+curpos;

	}
	return -1;
}

/* We encode archive search text so we don't get confused
 * by the string table - I was using LXLite to take care
 * of this problem on OS/2 but in portable code this may
 * not be an option. */
char *decode(char *input)
{
	char	*result;
	int	i = 0;

	result = (char *)malloc(strlen(input) / 2 + 1);

	while (input[0] && input[1])
	{
		result[i] = ((input[0] - 0x41) << 4) | (input[1] - 0x41);
		input += 2;
		i++;
	}
	result[i] = '\0';

	return result;
}

/*
 * Removes any carriage returns or line feeds from the buffer.
 */
void stripcrlf(char *buffer)
{
	int z, len = strlen(buffer);

	for(z=0;z<len;z++)
	{
		if(buffer[z] == '\r' || buffer[z] == '\n')
		{
			buffer[z] = 0;
			return;
		}
	}
}

/*
 * Returns the space free on a given drive... where 0 is A: in MB
 */
unsigned long drivefree(int drive)
{
#if __OS2__
	ULONG   aulFSInfoBuf[40] = {0};
	APIRET  rc               = NO_ERROR;
	double	bytesFree;

	DosError(FERR_DISABLEHARDERR);
	rc = DosQueryFSInfo(drive,
						FSIL_ALLOC,
						(PVOID)aulFSInfoBuf,
						sizeof(aulFSInfoBuf));

	DosError(FERR_ENABLEHARDERR);
	if (rc != NO_ERROR)
		return 0;

	bytesFree = (double)aulFSInfoBuf[3] * (double)aulFSInfoBuf[1] * (USHORT)aulFSInfoBuf[4];
	return (unsigned long)(bytesFree / (1024.0 * 1024.0));
#endif
	return 0;
}


/*
 * Display a fatal error message and set the abort flag in case we are in a secondary thread.
 */
void error(char *format, ...) {
	va_list args;
	char errstring[1024];

	va_start(args, format);
	vsprintf(errstring, format, args);
	va_end(args);

	if(installstate != ABORTED)
	{
		success=1;
		installstate=ABORTED;
	}
	wxMessageBox(errstring, INSTALLER_TITLE,
			wxOK | wxICON_EXCLAMATION, NULL);
}

void setdrivedir(char *drivedir)
{
	wxSetWorkingDirectory(drivedir);
}

/*
 * Make the TEMP directory the current directory, or the root directory of the boot drive.
 */
void settempdir(void)
{
#if defined(__EMX__) || defined(__OS2__) || defined(WIN32) || defined(WINNT)
    /* Windows or OS/2 */
	char *envdir = getenv("TMP");
	int len;

	if (!envdir)
		envdir = getenv("TEMP");
	if (!envdir)
		envdir = replaceem("%BOOTDRIVE%:\\");
	strcpy(tempPath,envdir);
	len = strlen(tempPath);
	if (len > 3 && tempPath[len-1] == '\\')
		tempPath[len-1] = 0;
	strupr(tempPath);
	setdrivedir(tempPath);
#else
    /* Unix */
	setdrivedir("/tmp");
#endif
}

void getbootdrive(void)
{
	/* On windows I don't think you can boot from anything
	   except C: drive.  So I am not going to do anything here. */
}

void PM_backslash(char *s)
{
	unsigned int pos = strlen(s);
	if (s[pos-1] != '\\') {
		s[pos] = '\\';
		s[pos+1] = '\0';
	}
}

/*
 * Makes a folder on the desktop.
 */
void MakeFolder(char Title[], char Icon[], char dest[], char id[], char setup[])
{
#ifdef __OS2__
	char szArg[200];

	memset(szArg,0,sizeof(szArg));

	if ((Icon != NULL) && (strlen(Icon) != 0))
	{
		strcat(szArg,"ICONFILE=");
		strcat(szArg,Icon);
	}

	if ((id != NULL) && (strlen(id) != 0))
	{
		strcat(szArg,";OBJECTID=");
		strcat(szArg,id);
	}

	if ((setup != NULL) && (strlen(setup) != 0))
	{
		strcat(szArg,";");
		strcat(szArg,setup);
	}

	WinCreateObject("WPFolder",Title,szArg,dest,CO_REPLACEIFEXISTS);
#elif defined(WIN32)
	char startpath[MAX_PATH];
	LPITEMIDLIST  pidl;

	if(!SHGetSpecialFolderLocation(NULL, CSIDL_PROGRAMS, &pidl))
	{
		SHGetPathFromIDList(pidl, startpath);

		if(startpath[strlen(startpath)-1] != '\\')
			strcat(startpath, "\\");
		strcat(startpath, Title);
		CreateDirectory(startpath, NULL);
	}
#else
    /* Unix? */
#endif
}

#ifdef WIN32
HRESULT CreateLink(LPCSTR lpszPathObj, 
    LPSTR lpszPathLink, LPSTR lpszDesc) 
{ 
	HRESULT hres;
	IShellLink* psl;

	// Get a pointer to the IShellLink interface.
	hres = CoCreateInstance(CLSID_ShellLink, NULL,
							CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);
	if (SUCCEEDED(hres)) {
		IPersistFile* ppf;

		// Set the path to the shortcut target, and add the
		// description.
		psl->SetPath(lpszPathObj);

        psl->SetDescription(lpszDesc);
 
       // Query IShellLink for the IPersistFile interface for saving the 
       // shortcut in persistent storage. 
        hres = psl->QueryInterface(IID_IPersistFile,
            (void **)&ppf);
 
        if (SUCCEEDED(hres)) { 
            WCHAR wsz[MAX_PATH];
 
            // Ensure that the string is ANSI. 
            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, 
                wsz, MAX_PATH); 

 
            // Save the link by calling IPersistFile::Save. 
            hres = ppf->Save(wsz, TRUE);
            ppf->Release();
        } 
        psl->Release();
    } 
    return hres; 
}
#endif

/*
 * Makes a Program object on the desktop.
 */
void MakeProgram(char Title[], char Program[], char Icon[], char dest[], char id[], char setup[])
{
#ifdef __OS2__
	char szArg[200];

	memset(szArg,0,sizeof(szArg));

	strcat(szArg,"EXENAME=");
	strcat(szArg,Program);

	if ((Icon != NULL) && (strlen(Icon) != 0))
	{
		strcat(szArg,";ICONFILE=");
		strcat(szArg,Icon);
	}

	if ((id != NULL) && (strlen(id) != 0))
	{
		strcat(szArg,";OBJECTID=");
		strcat(szArg,id);
	}

	if ((setup != NULL) && (strlen(setup) != 0))
	{
		strcat(szArg,";");
		strcat(szArg,setup);
	}

	WinCreateObject("WPProgram",Title,szArg,dest,CO_REPLACEIFEXISTS);
#elif defined(WIN32)
	char startpath[MAX_PATH];
	LPITEMIDLIST  pidl;

	if(!SHGetSpecialFolderLocation(NULL, CSIDL_PROGRAMS, &pidl))
	{
		SHGetPathFromIDList(pidl, startpath);

		if(startpath[strlen(startpath)-1] != '\\')
			strcat(startpath, "\\");
		strcat(startpath, dest);
		strcat(startpath, "\\");
		strcat(startpath, Title);
		strcat(startpath, ".lnk");

		CoInitialize(NULL);
		CreateLink(Program, startpath, Title);
		CoUninitialize();
	}


#else
	/* Unix? */
#endif
}
/*
 * Makes a user defined object on the desktop.
 */
void MakeObject(char Title[], char oclass[], char dest[], char id[], char setup[])
{
#ifdef __OS2__
	char szArg[200];

	memset(szArg,0,sizeof(szArg));

	if ((oclass == NULL) || (strlen(oclass) == 0))
		return;

	if ((id != NULL) && (strlen(id) != 0))
	{
		strcat(szArg,"OBJECTID=");
		strcat(szArg,id);
	}

	if ((setup != NULL) && (strlen(setup) != 0))
	{
		if ((id != NULL) && (strlen(id) != 0))
			strcat(szArg,";");
		strcat(szArg,setup);
	}

	WinCreateObject(oclass,Title,szArg,dest,CO_REPLACEIFEXISTS);
#elif defined(WIN32)
    /* Not sure if there is an equivilent on Windows */
#else
	/* Unix? */
#endif
}
/*
 * Makes a shadow on the desktop.
 */
void MakeShadow(char Title[], char reference[], char dest[], char id[])
{
#ifdef __OS2__
	char szArg[400];

	memset(szArg,0,sizeof(szArg));

	strcpy(szArg,"SHADOWID=");
	strcat(szArg,reference);
	if ((id != NULL) && (strlen(id) != 0))
	{
		strcat(szArg,";OBJECTID=");
		strcat(szArg,id);
	}
    strcat(szArg,";");
	WinCreateObject("WPShadow",Title,szArg,dest,CO_REPLACEIFEXISTS);
#elif defined(WIN32)
    /* Nothing like this on Windows9x anyway */
#else
	/* Unix? */
#endif
}

/* This creates program objects on the desktop, it was originally designed
 * for the OS/2 Workplace Shell so it may be somewhat different in use on
 * other platforms.
 */
void create_wps_objects(void)
{
	char *arg1, *arg2, *arg3, *arg4, *arg5, *arg6;
	char temp[5000];
	char zerotext[2] = "";
	int z, argn, len;

    /* No distinction for the moment... this may change.. */
	strcpy(installdir2, installdir);

	/* Create Folder Objects */
	if(strlen(INSTALLER_FOLDER)>0)
	{
		strcpy(temp, replaceem(INSTALLER_FOLDER));
		argn=0;
		arg1=&temp[0];
		arg2=arg3=arg4=arg5=&zerotext[0];
        len = strlen(temp);
		for(z=0;z<len;z++)
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
					arg4=&temp[z+1];
					break;
				case 4:
					arg5=&temp[z+1];
					break;
				case 5:
					argn=0;
					MakeFolder(arg1, arg2, arg3, arg4, arg5);
#ifdef ENABLE_LOGGING
					fprintf(logfile, "<WPSFolderAdd>,%s,%s,%s,%s,%s\r\n", arg1, arg2,arg3,arg4,arg5);
#endif
					arg1=&temp[z+1];
					arg2=arg3=arg4=arg5=&zerotext[0];
					break;
				}
			}
		}
		MakeFolder(arg1, arg2, arg3, arg4, arg5);
#ifdef ENABLE_LOGGING
		fprintf(logfile, "<WPSFolderAdd>,%s,%s,%s,%s,%s\r\n", arg1, arg2,arg3,arg4,arg5);
#endif
	}

	/* Create Program Objects */
	if(strlen(INSTALLER_PROGRAM)>0)
	{
		strcpy(temp, replaceem(INSTALLER_PROGRAM));
		argn=0;
		arg1=&temp[0];
		arg2=arg3=arg4=arg5=arg6=&zerotext[0];
        len = strlen(temp);
		for(z=0;z<len;z++)
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
					arg4=&temp[z+1];
					break;
				case 4:
					arg5=&temp[z+1];
					break;
				case 5:
					arg6=&temp[z+1];
					break;
				case 6:
					argn=0;
					MakeProgram(arg1, arg2, arg3, arg4, arg5, arg6);
#ifdef ENABLE_LOGGING
					fprintf(logfile, "<WPSProgramAdd>,%s,%s,%s,%s,%s,%s\r\n", arg1,arg2,arg3,arg4,arg5,arg6);
#endif
					arg1=&temp[z+1];
					arg2=arg3=arg4=arg5=arg6=&zerotext[0];
					break;
				}
			}
		}
		MakeProgram(arg1, arg2, arg3, arg4, arg5, arg6);
#ifdef ENABLE_LOGGING
		fprintf(logfile, "<WPSProgramAdd>,%s,%s,%s,%s,%s,%s\r\n", arg1, arg2,arg3,arg4,arg5,arg6);
#endif
	}

	/* Create Shadow Objects */
	if(strlen(INSTALLER_SHADOW)>0)
	{
		strcpy(temp, replaceem(INSTALLER_SHADOW));
		argn=0;
		arg1=&temp[0];
		arg2=arg3=arg4=&zerotext[0];
        len = strlen(temp);
		for(z=0;z<len;z++)
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
					arg4=&temp[z+1];
					break;
				case 4:
					argn=0;
					MakeShadow(arg1, arg2, arg3, arg4);
#ifdef ENABLE_LOGGING
					fprintf(logfile, "<WPSShadowAdd>,%s,%s,%s,%s\r\n", arg1,arg2,arg3,arg4);
#endif
					arg1=&temp[z+1];
					arg2=arg3=arg4=&zerotext[0];
					break;
				}
			}
		}
		MakeShadow(arg1, arg2, arg3, arg4);
#ifdef ENABLE_LOGGING
		fprintf(logfile, "<WPSShadowAdd>,%s,%s,%s,%s\r\n", arg1,arg2,arg3,arg4);
#endif
	}

	/* Create Generic Objects */
	if(strlen(INSTALLER_OBJECT)>0)
	{
		strcpy(temp, replaceem(INSTALLER_OBJECT));
		argn=0;
		arg1=&temp[0];
		arg2=arg3=arg4=arg5=&zerotext[0];
        len = strlen(temp);
		for(z=0;z<len;z++)
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
					arg4=&temp[z+1];
					break;
				case 4:
					arg5=&temp[z+1];
					break;
				case 5:
					argn=0;
					MakeObject(arg1, arg2, arg3, arg4, arg5);
#ifdef ENABLE_LOGGING
					fprintf(logfile, "<WPSObjectAdd>,%s,%s,%s,%s,%s\r\n", arg1,arg2,arg3,arg4,arg5);
#endif
					arg1=&temp[z+1];
					arg2=arg3=arg4=arg5=&zerotext[0];
					break;
				}
			}
		}
		MakeObject(arg1, arg2, arg3, arg4, arg5);
#ifdef ENABLE_LOGGING
		fprintf(logfile, "<WPSObjectAdd>,%s,%s,%s,%s,%s\r\n", arg1, arg2,arg3,arg4,arg5);
#endif
	}
}


#ifdef __cplusplus
}
#endif