/////////////////////////////////////////////////////////////////////////////
// Name:        file.cpp
// Purpose:     wxFile - encapsulates low-level "file descriptor"
//              wxTempFile
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "file.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wx/defs.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// standard
#if 	defined(__WINDOWS__) && !defined(__GNUWIN32__)
  #include  <io.h>
#elif (defined(__UNIX__) || defined(__GNUWIN32__))
  #include  <unistd.h>
#else
	#error	"Please specify the header with file functions declarations."
#endif  //Win/UNIX

#include  <stdio.h>       // SEEK_xxx constants
#include  <fcntl.h>       // O_RDONLY &c
#include  <sys/types.h>   // needed for stat
#include  <sys/stat.h>    // stat

// Microsoft compiler loves underscores, feed them to it
#ifdef    _MSC_VER
  // functions
  #define   open        _open
  #define   close       _close
  #define   read        _read
  #define   write       _write
  #define   lseek       _lseek
  #define   fsync       _commit
  #define   access      _access
  #define   eof         _eof

  // types
  #define   stat        _stat

  // constants
  #define   O_RDONLY    _O_RDONLY
  #define   O_WRONLY    _O_WRONLY
  #define   O_RDWR      _O_RDWR
  #define   O_EXCL      _O_EXCL
  #define   O_CREAT     _O_CREAT
  #define   O_BINARY    _O_BINARY

  #define   S_IFDIR     _S_IFDIR
  #define   S_IFREG     _S_IFREG

  #define   S_IREAD     _S_IREAD
  #define   S_IWRITE    _S_IWRITE
#else
  #define   tell(fd)    lseek(fd, 0, SEEK_CUR)
#endif  //_MSC_VER

// there is no distinction between text and binary files under Unix
#ifdef		__UNIX__
	#define 	O_BINARY		(0)
#endif	//__UNIX__

// wxWindows
#include  <wx/string.h>
#include  <wx/intl.h>
#include  <wx/file.h>
#include  <wx/log.h>


// ============================================================================
// implementation of wxFile
// ============================================================================

// ----------------------------------------------------------------------------
// static functions
// ----------------------------------------------------------------------------
bool wxFile::Exists(const char *sz)
{ 
  struct stat st; 
  return !access(sz, 0) && !stat(sz, &st) && (st.st_mode & S_IFREG);
}

// ----------------------------------------------------------------------------
// opening/closing
// ----------------------------------------------------------------------------

// ctors
wxFile::wxFile(const char *szFileName, OpenMode mode)
{
  m_fd = fd_invalid;

  Open(szFileName, mode);
}

// dtor
wxFile::~wxFile()
{
  Close();
}

// create the file, fail if it already exists and bOverwrite
bool wxFile::Create(const char *szFileName, bool bOverwrite)
{
  // if bOverwrite we create a new file or truncate the existing one,
  // otherwise we only create the new file and fail if it already exists
  int fd = open(szFileName, O_CREAT | (bOverwrite ? O_TRUNC : O_EXCL));

  if ( fd == -1 ) {
    wxLogSysError("can't create file '%s'", szFileName);
    return FALSE;
  }
  else {
    Attach(fd);
    return TRUE;
  }
}

// open the file
bool wxFile::Open(const char *szFileName, OpenMode mode)
{
  int flags = O_BINARY;

  switch ( mode ) {
    case read:      
      flags |= O_RDONLY; 
      break;

    case write:     
      flags |= O_WRONLY | O_CREAT | O_TRUNC; 
      break;

    case write_append:
      flags |= O_WRONLY | O_APPEND;
      break;

    case read_write: 
      flags |= O_RDWR;
      break;
  }

  int fd = open(szFileName, flags, S_IREAD | S_IWRITE);

  if ( fd == -1 ) {
    wxLogSysError("can't open file '%s'", szFileName);
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
    if ( close(m_fd) == -1 ) {
      wxLogSysError("can't close file descriptor %d", m_fd);
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

  int iRc = ::read(m_fd, pBuf, nCount);
  if ( iRc == -1 ) {
    wxLogSysError("can't read from file descriptor %d", m_fd);
    return ofsInvalid;
  }
  else
    return (uint)iRc;
}

// write
bool wxFile::Write(const void *pBuf, uint nCount)
{
  wxCHECK( (pBuf != NULL) && IsOpened(), 0 );

  int iRc = ::write(m_fd, pBuf, nCount);
  if ( iRc == -1 ) {
    wxLogSysError("can't write to file descriptor %d", m_fd);
    return FALSE;
  }
  else
    return TRUE;
}

// flush
bool wxFile::Flush()
{
  if ( IsOpened() ) {
		// @@@ fsync() is not ANSI (BSDish)
//    if ( fsync(m_fd) == -1 ) { // TODO
      if (TRUE) {
      wxLogSysError("can't flush file descriptor %d", m_fd);
      return FALSE;
    }
  }

  return TRUE;
}

// ----------------------------------------------------------------------------
// seek
// ----------------------------------------------------------------------------

// seek
off_t wxFile::Seek(off_t ofs, SeekMode mode)
{
  wxASSERT( IsOpened() );

  int flag = -1;
  switch ( mode ) {
    case FromStart:
      flag = SEEK_SET;
      break;

    case FromCurrent:
      flag = SEEK_CUR;
      break;

    case FromEnd:
      flag = SEEK_END;
      break;

    default:
      wxFAIL_MSG("unknown seek origin");
  }

  int iRc = lseek(m_fd, ofs, flag);
  if ( iRc == -1 ) {
    wxLogSysError("can't seek on file descriptor %d", m_fd);
    return ofsInvalid;
  }
  else
    return (off_t)iRc;
}

// get current off_t
off_t wxFile::Tell() const
{
  wxASSERT( IsOpened() );

  int iRc = tell(m_fd);
  if ( iRc == -1 ) {
    wxLogSysError("can't get seek position on file descriptor %d", m_fd);
    return ofsInvalid;
  }
  else
    return (off_t)iRc;
}

// get current file length
off_t wxFile::Length() const
{
  wxASSERT( IsOpened() );

  #ifdef  _MSC_VER
    int iRc = _filelength(m_fd);
  #else
    int iRc = tell(m_fd);
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

  #endif  //_MSC_VER

  if ( iRc == -1 ) {
    wxLogSysError("can't find length of file on file descriptor %d", m_fd);
    return ofsInvalid;
  }
  else
    return (off_t)iRc;
}

// is end of file reached?
bool wxFile::Eof() const
{
  wxASSERT( IsOpened() );

  int iRc;

  #if defined(__UNIX__) || defined(__GNUWIN32__)
    // @@ this doesn't work, of course, on unseekable file descriptors
    off_t ofsCur = Tell(),
          ofsMax = Length();
    if ( ofsCur == ofsInvalid || ofsMax == ofsInvalid )
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
      wxLogSysError("can't determine if the end of file is reached on "
                    "descriptor %d", m_fd);
      break;

    default:
      wxFAIL_MSG("invalid eof() return value.");
  }

  return TRUE;
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
  m_strName = strName;
  
  // we want to create the file in the same directory as strName because
  // otherwise rename() in Commit() might not work (if the files are on
  // different partitions for example). Unfortunately, the only standard
  // (POSIX) temp file creation function tmpnam() can't do it.
  #ifdef  __UNIX__
    static const char *szMktempSuffix = "XXXXXX";
    m_strTemp << strName << szMktempSuffix;
    mktemp((char *)m_strTemp.c_str()); // @@@ even if the length doesn't change
    //m_strTemp.UngetWriteBuf();
  #else // Windows
    m_strTemp = tmpnam(NULL);
  #endif  // Windows/Unix
    
  return m_file.Open(m_strTemp, wxFile::write);
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

  if ( wxFile::Exists(m_strName) && remove(m_strName) != 0 ) {
    wxLogSysError("can't remove file '%s'", m_strName.c_str());
    return FALSE;
  }

  if ( rename(m_strTemp, m_strName) != 0 ) {
    wxLogSysError("can't commit changes to file '%s'", m_strName.c_str());
    return FALSE;
  }

  return TRUE;
}

void wxTempFile::Discard()
{
  m_file.Close();
  if ( remove(m_strTemp) != 0 )
    wxLogSysError("can't remove temporary file '%s'", m_strTemp.c_str());
}
