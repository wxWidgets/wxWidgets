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
#include "install.h"
#include "instsup.h"

extern char *INSTALLER_TITLE;
extern char tempPath[], installdir[], csfile[], bufile[], bootdrive[], instlog[];
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
#if 0	
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
#if 0	
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
 * Opens a Window which contains the text specfied by given filename.
 */
void viewfile(char *filename)
{
#if 0	
	HWND hwndFrame, /*hwndMLE,*/ hwndClient;
	ULONG flStyle = FCF_MINMAX | FCF_SYSMENU | FCF_TITLEBAR |
		FCF_SIZEBORDER | FCF_SHELLPOSITION | FCF_TASKLIST;
	PPRESPARAMS ppp;
	char deffont[] = "9.Warpsans";

	WinRegisterClass(localhab, "VIEW", ViewWndProc, CS_SIZEREDRAW, 32);

	hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
								   WS_VISIBLE | WS_MAXIMIZED,
								   &flStyle,
								   "VIEW",
								   filename,
								   0L,
								   NULLHANDLE,
								   0L,
								   &hwndClient);

	ppp = malloc((sizeof(ULONG) * 3) + strlen(deffont) + 1);
	ppp->cb = (sizeof(ULONG) * 2) + strlen(deffont) + 1;
	ppp->aparam[0].id = PP_FONTNAMESIZE;
	ppp->aparam[0].cb = strlen(deffont)+1;
	memcpy(&ppp->aparam[0].ab, deffont, strlen(deffont)+1);

	hwndMLE = WinCreateWindow(hwndFrame,
							  WC_MLE,
							  "",
							  WS_VISIBLE | MLS_VSCROLL | MLS_WORDWRAP | MLS_READONLY | MLS_BORDER,
							  0,0,100,100,
							  hwndFrame,
							  HWND_TOP,
							  999L,
							  NULL,
							  ppp);

	WinSetWindowPos(hwndFrame, HWND_TOP, 0,0,0,0,SWP_MAXIMIZE | SWP_ZORDER);
	WinSetWindowPos(hwndMLE, HWND_TOP, 0,0,0,0,SWP_SHOW);

	if(!hwndMLE)
	{
		mesg("Error %lu while creating MLE window.", WinGetLastError(localhab));
	}
	else
	{
		FILE *f;
		char buffer[1024];
		ULONG bytes, point = -1;

		if((f = fopen(filename, "rb")) != NULL)
		{
			WinSendMsg(hwndMLE, MLM_SETIMPORTEXPORT, MPFROMP(buffer), MPFROMLONG(1024L));
			while(!feof(f))
			{
				bytes = fread(buffer, 1, 1024, f);
				WinSendMsg(hwndMLE, MLM_IMPORT, MPFROMP(&point), MPFROMLONG(bytes));
			}
			fclose(f);
		}

}
#endif
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
#if 0	
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
	setdrivedir("/tmp");
#endif
}

void getbootdrive(void)
{
	/* On windows I don't think you can boot from anything
	   except C: drive.  So I am not going to do anything here. */
}
/*
 * Start an application using CMD.EXE.
 */
int cmdrun(char *progname)
{
#if 0	
	STARTDATA SData       = {0};
	PSZ       PgmTitle    = "",
		      PgmName     = "CMD.EXE";
	APIRET    rc          = NO_ERROR;
	PID       pid         = 0;
	ULONG     ulSessID    = 0;
	CHAR      achObjBuf[256] = {0};
	HQUEUE hqQueue;
	REQUESTDATA rdRequest;
	ULONG ulSzData;
	BYTE bPriority;
	PVOID pvData;

	SData.Length  = sizeof(STARTDATA);
	SData.Related = SSF_RELATED_CHILD;
	SData.FgBg    = SSF_FGBG_BACK;
	SData.TraceOpt = SSF_TRACEOPT_NONE;
	SData.PgmTitle = PgmTitle;
	SData.PgmName = PgmName;
	SData.PgmInputs = progname;

	SData.TermQ = "\\QUEUES\\CHILD.QUE";
	SData.Environment = 0;
	SData.InheritOpt = SSF_INHERTOPT_SHELL;
	SData.SessionType = SSF_TYPE_WINDOWABLEVIO;
    SData.IconFile = 0;
	SData.PgmHandle = 0;

	SData.PgmControl = SSF_CONTROL_INVISIBLE;
	SData.Reserved = 0;
	SData.ObjectBuffer = achObjBuf;
	SData.ObjectBuffLen = (ULONG) sizeof(achObjBuf);

	if(!(rc = DosCreateQueue(&hqQueue, QUE_FIFO | QUE_CONVERT_ADDRESS,"\\QUEUES\\CHILD.QUE")))
	{
		if(!(rc = DosStartSession(&SData, &ulSessID, &pid)))
			if(!(rc=DosReadQueue(hqQueue, &rdRequest, &ulSzData, &pvData, 0, 0, &bPriority, 0)))
				DosFreeMem(pvData);
		DosCloseQueue(hqQueue);
	}

	if (rc != NO_ERROR) {
		mesg("Error %d while attempting to run %s!", rc, progname);
		return 1;
	}
	return NO_ERROR;
#endif	
	return 0;
}

void PM_backslash(char *s)
{
	unsigned int pos = strlen(s);
	if (s[pos-1] != '\\') {
		s[pos] = '\\';
		s[pos+1] = '\0';
	}
}
#ifdef __cplusplus
}
#endif