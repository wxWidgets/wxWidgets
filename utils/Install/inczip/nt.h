/* nt.h:  central header for EF_NTSD "SD" extra field */

#ifndef _NT_H
#define _NT_H

#define NTSD_BUFFERSIZE (1024)  /* threshold to cause malloc() */
#define OVERRIDE_BACKUP     1   /* we have SeBackupPrivilege on remote */
#define OVERRIDE_RESTORE    2   /* we have SeRestorePrivilege on remote */
#define OVERRIDE_SACL       4   /* we have SeSystemSecurityPrivilege on remote */

typedef struct {
    BOOL bValid;                /* are our contents valid? */
    BOOL bProcessDefer;         /* process deferred entry yet? */
    BOOL bUsePrivileges;        /* use privilege overrides? */
    DWORD dwFileSystemFlags;    /* describes target file system */
    BOOL bRemote;               /* is volume remote? */
    DWORD dwRemotePrivileges;   /* relevant only on remote volumes */
    DWORD dwFileAttributes;
    char RootPath[MAX_PATH+1];  /* path to network / filesystem */
} VOLUMECAPS, *PVOLUMECAPS, *LPVOLUMECAPS;

BOOL SecuritySet(char *resource, PVOLUMECAPS VolumeCaps, uch *securitydata);
BOOL GetVolumeCaps(char *rootpath, char *name, PVOLUMECAPS VolumeCaps);
BOOL ValidateSecurity(uch *securitydata);
BOOL ProcessDefer(PDWORD dwDirectoryCount, PDWORD dwBytesProcessed,
                  PDWORD dwDirectoryFail, PDWORD dwBytesFail);

#endif /* _NT_H */
