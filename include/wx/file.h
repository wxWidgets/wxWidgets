/////////////////////////////////////////////////////////////////////////////
// Name:        file.cpp
// Purpose:     wxFile - encapsulates low-level "file descriptor"
//              wxTempFile - safely replace the old file
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef   __FILEH__
#define   __FILEH__

#ifdef __GNUG__
#pragma interface "file.h"
#endif

// ----------------------------------------------------------------------------
// simple types
// ----------------------------------------------------------------------------

#include  <wx/filefn.h>

// define off_t
#include  <sys/types.h>

#ifdef    _MSC_VER
  #define   off_t       _off_t
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// error return value for Seek() functions
const off_t ofsInvalid = (off_t)-1;

// ----------------------------------------------------------------------------
// class wxFile: raw file IO
//
// NB: for space efficiency this class has no virtual functions, including
//     dtor which is _not_ virtual, so it shouldn't be used as a base class.
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxFile
{
public:
  // more file constants
  // -------------------
    
    // opening mode
  enum OpenMode { read, write, read_write, write_append };
    // standard values for file descriptor
  enum { fd_invalid = -1, fd_stdin, fd_stdout, fd_stderr };
    // seek type
  enum SeekMode { FromStart, FromEnd, FromCurrent };

  // static functions
  // ----------------
  static bool Exists(const char *sz);  // also checks it's a regular file

  // ctors
  // -----
    // def ctor
  wxFile() { m_fd = fd_invalid; }
    // open specified file (may fail, use IsOpened())
  wxFile(const char *szFileName, OpenMode mode = read);
    // attach to (already opened) file
  wxFile(int fd) { m_fd = fd; }

  // open/close
  bool Create(const char *szFileName, bool bOverwrite = FALSE);
  bool Open(const char *szFileName, OpenMode mode = read);
  inline bool Close();  // Close is a NOP if not opened

  // assign an existing file descriptor and get it back from wxFile object
  void Attach(int fd) { Close(); m_fd = fd; }
  void Detach()       { m_fd = fd_invalid;  }
  int  fd() const { return m_fd; }

  // read/write (unbuffered)
    // returns number of bytes read or ofsInvalid on error
  off_t Read(void *pBuf, off_t nCount);
    // returns true on success
  bool Write(const void *pBuf, uint nCount);
    // returns true on success
  bool Write(const wxString& str) { return Write(str.c_str(), str.Len()); }
    // flush data not yet written
  bool Flush();

  // file pointer operations (return ofsInvalid on failure)
    // move ptr ofs bytes related to start/current off_t/end of file
  off_t Seek(off_t ofs, SeekMode mode = FromStart);
    // move ptr to ofs bytes before the end
  off_t SeekEnd(off_t ofs = 0) { return Seek(ofs, FromEnd); }
    // get current off_t
  off_t Tell() const;
    // get current file length
  off_t Length() const;

  // simple accessors
    // is file opened?
  bool IsOpened() const { return m_fd != fd_invalid; }
    // is end of file reached?
  bool Eof() const;
    
  // dtor closes the file if opened
 ~wxFile();

private:
  // copy ctor and assignment operator are private because
  // it doesn't make sense to copy files this way:
  // attempt to do it will provoke a compile-time error.
  wxFile(const wxFile&);
  wxFile& operator=(const wxFile&);

  int m_fd; // file descriptor or INVALID_FD if not opened
};

// ----------------------------------------------------------------------------
// class wxTempFile: if you want to replace another file, create an instance
// of wxTempFile passing the name of the file to be replaced to the ctor. Then
// you can write to wxTempFile and call Commit() function to replace the old
// file (and close this one) or call Discard() to cancel the modification. If
// you call neither of them, dtor will call Discard().
// ----------------------------------------------------------------------------
class wxTempFile
{
public:
  // ctors
    // default
  wxTempFile() { }
    // associates the temp file with the file to be replaced and opens it
  wxTempFile(const wxString& strName);

  // open the temp file (strName is the name of file to be replaced)
  bool Open(const wxString& strName);

  // is the file opened?
  bool IsOpened() const { return m_file.IsOpened(); }

  // I/O (both functions return true on success, false on failure)
  bool Write(const void *p, uint n) { return m_file.Write(p, n); }
  bool Write(const wxString& str)   { return m_file.Write(str);  }

  // different ways to close the file
    // validate changes and delete the old file of name m_strName
  bool Commit();
    // discard changes
  void Discard();

  // dtor calls Discard() if file is still opened
 ~wxTempFile();

private:
  wxString  m_strName,  // name of the file to replace in Commit()
            m_strTemp;  // temporary file name
  wxFile    m_file;     // the temporary file
};

#endif
	// __FILEH__
