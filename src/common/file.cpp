/////////////////////////////////////////////////////////////////////////////
// Name:        file.cpp
// Purpose:     wxFile - encapsulates low-level "file descriptor"
//              wxTempFile
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "file.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_FILE

// standard
#if defined(__WXMSW__) && !defined(__GNUWIN32__) && !defined(__WXMICROWIN__) && !defined(__WXWINCE__)
  #include  <io.h>

#ifndef __SALFORDC__
    #define   WIN32_LEAN_AND_MEAN
    #define   NOSERVICE
    #define   NOIME
    #define   NOATOM
    #define   NOGDI
    #define   NOGDICAPMASKS
    #define   NOMETAFILE
    #define   NOMINMAX
    #define   NOMSG
    #define   NOOPENFILE
    #define   NORASTEROPS
    #define   NOSCROLL
    #define   NOSOUND
    #define   NOSYSMETRICS
    #define   NOTEXTMETRIC
    #define   NOWH
    #define   NOCOMM
    #define   NOKANJI
    #define   NOCRYPT
    #define   NOMCX
#endif

#elif defined(__WXMSW__) && defined(__WXWINCE__)
    // TODO: what to include?
#elif (defined(__UNIX__) || defined(__GNUWIN32__))
    #include  <unistd.h>
    #include  <sys/stat.h>
    #ifdef __GNUWIN32__
        #include <windows.h>
    #endif
#elif defined(__DOS__)
    #if defined(__WATCOMC__)
       #include <io.h>
    #elif defined(__DJGPP__)
       #include <io.h>
       #include <unistd.h>
       #include <stdio.h>
    #else
        #error  "Please specify the header with file functions declarations."
    #endif
#elif (defined(__WXPM__))
    #include <io.h>
#elif (defined(__WXSTUBS__))
    // Have to ifdef this for different environments
    #include <io.h>
#elif (defined(__WXMAC__))
#if __MSL__ < 0x6000
    int access( const char *path, int mode ) { return 0 ; }
#else
    int _access( const char *path, int mode ) { return 0 ; }
#endif
    char* mktemp( char * path ) { return path ;}
    #include <stat.h>
    #include  <unistd.h>
#else
    #error  "Please specify the header with file functions declarations."
#endif  //Win/UNIX

#include  <stdio.h>       // SEEK_xxx constants

#ifndef __WXWINCE__
#include  <fcntl.h>       // O_RDONLY &c
#endif

#ifdef __WXWINCE__
// Nothing
#elif !defined(__MWERKS__)
    #include  <sys/types.h>   // needed for stat
    #include  <sys/stat.h>    // stat
#elif defined(__MWERKS__) && ( defined(__WXMSW__) || defined(__MACH__) )
    #include  <sys/types.h>   // needed for stat
    #include  <sys/stat.h>    // stat
#endif

// Windows compilers don't have these constants
#ifndef W_OK
    enum
    {
        F_OK = 0,   // test for existence
        X_OK = 1,   //          execute permission
        W_OK = 2,   //          write
        R_OK = 4    //          read
    };
#endif // W_OK

// there is no distinction between text and binary files under Unix, so define
// O_BINARY as 0 if the system headers don't do it already
#if defined(__UNIX__) && !defined(O_BINARY)
    #define   O_BINARY    (0)
#endif  //__UNIX__

#ifdef __SALFORDC__
    #include <unix.h>
#endif

#ifndef MAX_PATH
    #define MAX_PATH 512
#endif

// some broken compilers don't have 3rd argument in open() and creat()
#ifdef __SALFORDC__
    #define ACCESS(access)
    #define stat    _stat
#else // normal compiler
    #define ACCESS(access)  , (access)
#endif // Salford C

// wxWindows
#ifndef WX_PRECOMP
    #include  "wx/string.h"
    #include  "wx/intl.h"
    #include  "wx/log.h"
#endif // !WX_PRECOMP

#include  "wx/filename.h"
#include  "wx/file.h"
#include  "wx/filefn.h"

#ifdef __WXMSW__
    #include "wx/msw/mslu.h"
#endif

#ifdef __WXWINCE__
    #include "wx/msw/private.h"
#endif

// ============================================================================
// implementation of wxFile
// ============================================================================

// ----------------------------------------------------------------------------
// static functions
// ----------------------------------------------------------------------------

bool wxFile::Exists(const wxChar *name)
{
    return wxFileExists(name);
}

bool wxFile::Access(const wxChar *name, OpenMode mode)
{
    int how;

    switch ( mode )
    {
        default:
            wxFAIL_MSG(wxT("bad wxFile::Access mode parameter."));
            // fall through

        case read:
            how = R_OK;
            break;

        case write:
            how = W_OK;
            break;

        case read_write:
            how = R_OK | W_OK;
            break;
    }

#ifdef __WXWINCE__
    // FIXME: use CreateFile with 0 access to query the file
    return TRUE;
#else
    return wxAccess(name, how) == 0;
#endif
}

// ----------------------------------------------------------------------------
// opening/closing
// ----------------------------------------------------------------------------

// ctors
wxFile::wxFile(const wxChar *szFileName, OpenMode mode)
{
    m_fd = fd_invalid;
    m_error = FALSE;

    Open(szFileName, mode);
}

// create the file, fail if it already exists and bOverwrite
bool wxFile::Create(const wxChar *szFileName, bool bOverwrite, int accessMode)
{
    // if bOverwrite we create a new file or truncate the existing one,
    // otherwise we only create the new file and fail if it already exists
#if defined(__WXMAC__) && !defined(__UNIX__) && !wxUSE_UNICODE
    // Dominic Mazzoni [dmazzoni+@cs.cmu.edu] reports that open is still broken on the mac, so we replace
    // int fd = open(wxUnix2MacFilename( szFileName ), O_CREAT | (bOverwrite ? O_TRUNC : O_EXCL), access);
    int fd = creat( szFileName , accessMode);
#else
#ifdef __WXWINCE__
    HANDLE fileHandle = ::CreateFile(szFileName, GENERIC_WRITE, 0, NULL,
        bOverwrite ? CREATE_ALWAYS : CREATE_NEW, FILE_ATTRIBUTE_NORMAL,
        0);
    int fd = 0;
    if (fileHandle == INVALID_HANDLE_VALUE)
        fd = (int) fileHandle;
    else
        fd = -1;
#else
    int fd = wxOpen( szFileName,
                     O_BINARY | O_WRONLY | O_CREAT |
                     (bOverwrite ? O_TRUNC : O_EXCL)
                     ACCESS(accessMode) );
#endif
#endif
    if ( fd == -1 )
    {
        wxLogSysError(_("can't create file '%s'"), szFileName);
        return FALSE;
    }
    else
    {
        Attach(fd);
        return TRUE;
    }
}

// open the file
bool wxFile::Open(const wxChar *szFileName, OpenMode mode, int accessMode)
{
#ifdef __WXWINCE__
    DWORD access = 0;
    DWORD shareMode = 0;
    DWORD disposition = 0;

    int flags = O_BINARY;

    switch ( mode )
    {
        case read:
            access = GENERIC_READ;
            shareMode = FILE_SHARE_READ|FILE_SHARE_WRITE;
            disposition = OPEN_EXISTING;
            break;

        case write_append:
            if ( wxFile::Exists(szFileName) )
            {
                access = GENERIC_READ|GENERIC_WRITE;
                shareMode = FILE_SHARE_READ;
                disposition = 0;
                break;
            }
            //else: fall through as write_append is the same as write if the
            //      file doesn't exist

        case write:
            access = GENERIC_WRITE;
            shareMode = 0;
            disposition = TRUNCATE_EXISTING;
            break;

        case write_excl:
            access = GENERIC_WRITE;
            shareMode = 0;
            disposition = TRUNCATE_EXISTING;
            break;

        case read_write:
            access = GENERIC_READ|GENERIC_WRITE;
            shareMode = 0;
            disposition = 0;
            break;
    }

    int fd = 0;
    HANDLE fileHandle = ::CreateFile(szFileName, access, shareMode, NULL,
        disposition, FILE_ATTRIBUTE_NORMAL, 0);
    if (fileHandle == INVALID_HANDLE_VALUE)
        fd = -1;
    else
        fd = (int) fileHandle;
#else
    int flags = O_BINARY;

    switch ( mode )
    {
        case read:
            flags |= O_RDONLY;
            break;

        case write_append:
            if ( wxFile::Exists(szFileName) )
            {
                flags |= O_WRONLY | O_APPEND;
                break;
            }
            //else: fall through as write_append is the same as write if the
            //      file doesn't exist

        case write:
            flags |= O_WRONLY | O_CREAT | O_TRUNC;
            break;

        case write_excl:
            flags |= O_WRONLY | O_CREAT | O_EXCL;
            break;

        case read_write:
            flags |= O_RDWR;
            break;
    }

    int fd = wxOpen( szFileName, flags ACCESS(accessMode));
#endif
    if ( fd == -1 )
    {
        wxLogSysError(_("can't open file '%s'"), szFileName);
        return FALSE;
    }
    else {
        Attach(fd);
        return TRUE;
    }
}

// close
bool wxFile::Close()
{
    if ( IsOpened() ) {
#ifdef __WXWINCE__
        if (!CloseHandle((HANDLE) m_fd))
#else
        if ( close(m_fd) == -1 )
#endif
        {
            wxLogSysError(_("can't close file descriptor %d"), m_fd);
            m_fd = fd_invalid;
            return FALSE;
        }
        else
            m_fd = fd_invalid;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// read/write
// ----------------------------------------------------------------------------

// read
off_t wxFile::Read(void *pBuf, off_t nCount)
{
    wxCHECK( (pBuf != NULL) && IsOpened(), 0 );

#ifdef __WXWINCE__
    DWORD bytesRead = 0;
    int iRc = 0;
    if (ReadFile((HANDLE) m_fd, pBuf, (DWORD) nCount, & bytesRead, NULL))
        iRc = bytesRead;
    else
        iRc = -1;
#elif defined(__MWERKS__)
    int iRc = ::read(m_fd, (char*) pBuf, nCount);
#else
    int iRc = ::read(m_fd, pBuf, nCount);
#endif
    if ( iRc == -1 ) {
        wxLogSysError(_("can't read from file descriptor %d"), m_fd);
        return wxInvalidOffset;
    }
    else
        return (size_t)iRc;
}

// write
size_t wxFile::Write(const void *pBuf, size_t nCount)
{
    wxCHECK( (pBuf != NULL) && IsOpened(), 0 );

#ifdef __WXWINCE__
    DWORD bytesRead = 0;
    int iRc = 0;
    if (WriteFile((HANDLE) m_fd, pBuf, (DWORD) nCount, & bytesRead, NULL))
        iRc = bytesRead;
    else
        iRc = -1;
#elif defined(__MWERKS__)
#if __MSL__ >= 0x6000
    int iRc = ::write(m_fd, (void*) pBuf, nCount);
#else
    int iRc = ::write(m_fd, (const char*) pBuf, nCount);
#endif
#else
    int iRc = ::write(m_fd, pBuf, nCount);
#endif
    if ( iRc == -1 ) {
        wxLogSysError(_("can't write to file descriptor %d"), m_fd);
        m_error = TRUE;
        return 0;
    }
    else
        return iRc;
}

// flush
bool wxFile::Flush()
{
    if ( IsOpened() ) {
#ifdef __WXWINCE__
        // Do nothing
#elif defined(__VISUALC__) || wxHAVE_FSYNC
        if ( wxFsync(m_fd) == -1 )
        {
            wxLogSysError(_("can't flush file descriptor %d"), m_fd);
            return FALSE;
        }
#else // no fsync
        // just do nothing
#endif // fsync
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// seek
// ----------------------------------------------------------------------------

// seek
off_t wxFile::Seek(off_t ofs, wxSeekMode mode)
{
    wxASSERT( IsOpened() );

#ifdef __WXWINCE__
    int origin;
    switch ( mode ) {
        default:
            wxFAIL_MSG(_("unknown seek origin"));

        case wxFromStart:
            origin = FILE_BEGIN;
            break;

        case wxFromCurrent:
            origin = FILE_CURRENT;
            break;

        case wxFromEnd:
            origin = FILE_END;
            break;
    }

    DWORD res = SetFilePointer((HANDLE) m_fd, ofs, 0, origin) ;
    if (res == 0xFFFFFFFF && GetLastError() != NO_ERROR)
    {
        wxLogSysError(_("can't seek on file descriptor %d"), m_fd);
        return wxInvalidOffset;
    }
    else
        return (off_t)res;
#else
    int origin;
    switch ( mode ) {
        default:
            wxFAIL_MSG(_("unknown seek origin"));

        case wxFromStart:
            origin = SEEK_SET;
            break;

        case wxFromCurrent:
            origin = SEEK_CUR;
            break;

        case wxFromEnd:
            origin = SEEK_END;
            break;
    }

    int iRc = lseek(m_fd, ofs, origin);
    if ( iRc == -1 ) {
        wxLogSysError(_("can't seek on file descriptor %d"), m_fd);
        return wxInvalidOffset;
    }
    else
        return (off_t)iRc;
#endif
}

// get current off_t
off_t wxFile::Tell() const
{
    wxASSERT( IsOpened() );

#ifdef __WXWINCE__
    DWORD res = SetFilePointer((HANDLE) m_fd, 0, 0, FILE_CURRENT) ;
    if (res == 0xFFFFFFFF && GetLastError() != NO_ERROR)
    {
        wxLogSysError(_("can't get seek position on file descriptor %d"), m_fd);
        return wxInvalidOffset;
    }
    else
        return (off_t)res;
#else
    int iRc = wxTell(m_fd);
    if ( iRc == -1 ) {
        wxLogSysError(_("can't get seek position on file descriptor %d"), m_fd);
        return wxInvalidOffset;
    }
    else
        return (off_t)iRc;
#endif
}

// get current file length
off_t wxFile::Length() const
{
    wxASSERT( IsOpened() );

#ifdef __WXWINCE__
    DWORD off0 = SetFilePointer((HANDLE) m_fd, 0, 0, FILE_CURRENT);
    DWORD off1 = SetFilePointer((HANDLE) m_fd, 0, 0, FILE_END);
    off_t len = off1;

    // Restore position
    SetFilePointer((HANDLE) m_fd, off0, 0, FILE_BEGIN);
    return len;
#else
#ifdef __VISUALC__
    int iRc = _filelength(m_fd);
#else // !VC++
    int iRc = wxTell(m_fd);
    if ( iRc != -1 ) {
        // @ have to use const_cast :-(
        int iLen = ((wxFile *)this)->SeekEnd();
        if ( iLen != -1 ) {
            // restore old position
            if ( ((wxFile *)this)->Seek(iRc) == -1 ) {
                // error
                iLen = -1;
            }
        }

        iRc = iLen;
    }
#endif  // VC++

    if ( iRc == -1 ) {
        wxLogSysError(_("can't find length of file on file descriptor %d"), m_fd);
        return wxInvalidOffset;
    }
    else
        return (off_t)iRc;
#endif
}

// is end of file reached?
bool wxFile::Eof() const
{
    wxASSERT( IsOpened() );

#ifdef __WXWINCE__
    DWORD off0 = SetFilePointer((HANDLE) m_fd, 0, 0, FILE_CURRENT);
    DWORD off1 = SetFilePointer((HANDLE) m_fd, 0, 0, FILE_END);
    if (off0 == off1)
        return TRUE;
    else
    {
        SetFilePointer((HANDLE) m_fd, off0, 0, FILE_BEGIN);
        return FALSE;
    }
#else
    int iRc;

#if defined(__DOS__) || defined(__UNIX__) || defined(__GNUWIN32__) || defined( __MWERKS__ ) || defined(__SALFORDC__)
    // @@ this doesn't work, of course, on unseekable file descriptors
    off_t ofsCur = Tell(),
    ofsMax = Length();
    if ( ofsCur == wxInvalidOffset || ofsMax == wxInvalidOffset )
        iRc = -1;
    else
        iRc = ofsCur == ofsMax;
#else  // Windows and "native" compiler
    iRc = eof(m_fd);
#endif // Windows/Unix

    switch ( iRc ) {
        case 1:
            break;

        case 0:
            return FALSE;

        case -1:
            wxLogSysError(_("can't determine if the end of file is reached on descriptor %d"), m_fd);
                break;

        default:
            wxFAIL_MSG(_("invalid eof() return value."));
    }

    return TRUE;
#endif
}

// ============================================================================
// implementation of wxTempFile
// ============================================================================

// ----------------------------------------------------------------------------
// construction
// ----------------------------------------------------------------------------

wxTempFile::wxTempFile(const wxString& strName)
{
    Open(strName);
}

bool wxTempFile::Open(const wxString& strName)
{
    // we must have an absolute filename because otherwise CreateTempFileName()
    // would create the temp file in $TMP (i.e. the system standard location
    // for the temp files) which might be on another volume/drive/mount and
    // wxRename()ing it later to m_strName from Commit() would then fail
    //
    // with the absolute filename, the temp file is created in the same
    // directory as this one which ensures that wxRename() may work later
    wxFileName fn(strName);
    if ( !fn.IsAbsolute() )
    {
        fn.Normalize(wxPATH_NORM_ABSOLUTE);
    }

    m_strName = fn.GetFullPath();

    m_strTemp = wxFileName::CreateTempFileName(m_strName, &m_file);

    if ( m_strTemp.empty() )
    {
        // CreateTempFileName() failed
        return FALSE;
    }

#ifdef __UNIX__
    // the temp file should have the same permissions as the original one
    mode_t mode;
    
    wxStructStat st;
    if ( stat( (const char*) m_strName.fn_str(), &st) == 0 )
    {
        mode = st.st_mode;
    }
    else
    {
        // file probably didn't exist, just give it the default mode _using_
        // user's umask (new files creation should respect umask)
        mode_t mask = umask(0777);
        mode = 0666 & ~mask;
        umask(mask);
    }

    if ( chmod( (const char*) m_strTemp.fn_str(), mode) == -1 )
    {
        wxLogSysError(_("Failed to set temporary file permissions"));
    }
#endif // Unix

    return TRUE;
}

// ----------------------------------------------------------------------------
// destruction
// ----------------------------------------------------------------------------

wxTempFile::~wxTempFile()
{
    if ( IsOpened() )
        Discard();
}

bool wxTempFile::Commit()
{
    m_file.Close();

    if ( wxFile::Exists(m_strName) && wxRemove(m_strName) != 0 ) {
        wxLogSysError(_("can't remove file '%s'"), m_strName.c_str());
        return FALSE;
    }

    if ( wxRenameFile(m_strTemp, m_strName) != 0 ) {
        wxLogSysError(_("can't commit changes to file '%s'"), m_strName.c_str());
        return FALSE;
    }

    return TRUE;
}

void wxTempFile::Discard()
{
    m_file.Close();
    if ( wxRemove(m_strTemp) != 0 )
        wxLogSysError(_("can't remove temporary file '%s'"), m_strTemp.c_str());
}

#endif // wxUSE_FILE

