/////////////////////////////////////////////////////////////////////////////
// Name:        file.cpp
// Purpose:     wxFile - encapsulates low-level "file descriptor"
//              wxTempFile - safely replace the old file
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef   __FILEH__
#define   __FILEH__

#ifdef __GNUG__
#pragma interface "file.h"
#endif

#ifndef WX_PRECOMP
  #include  "wx/string.h"
  #include  "wx/filefn.h"
#endif

#if wxUSE_FILE

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// we redefine these constants here because S_IREAD &c are _not_ standard
// however, we do assume that the values correspond to the Unix umask bits
#define wxS_IRUSR 00400
#define wxS_IWUSR 00200
#define wxS_IXUSR 00100

#define wxS_IRGRP 00040
#define wxS_IWGRP 00020
#define wxS_IXGRP 00010

#define wxS_IROTH 00004
#define wxS_IWOTH 00002
#define wxS_IXOTH 00001

// default mode for the new files: corresponds to umask 022
#define wxS_DEFAULT   (wxS_IRUSR | wxS_IWUSR | wxS_IRGRP | wxS_IWGRP |\
                       wxS_IROTH | wxS_IWOTH)

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

  // static functions
  // ----------------
    // check whether a regular file by this name exists
  static bool Exists(const wxChar *name);
    // check whetther we can access the given file in given mode
    // (only read and write make sense here)
  static bool Access(const wxChar *name, OpenMode mode);

  // ctors
  // -----
    // def ctor
  wxFile() { m_fd = fd_invalid; }
    // open specified file (may fail, use IsOpened())
  wxFile(const wxChar *szFileName, OpenMode mode = read);
    // attach to (already opened) file
  wxFile(int fd) { m_fd = fd; }

  // open/close
    // create a new file (with the default value of bOverwrite, it will fail if
    // the file already exists, otherwise it will overwrite it and succeed)
  bool Create(const wxChar *szFileName, bool bOverwrite = FALSE,
              int access = wxS_DEFAULT);
  bool Open(const wxChar *szFileName, OpenMode mode = read,
            int access = wxS_DEFAULT);
  bool Close();  // Close is a NOP if not opened

  // assign an existing file descriptor and get it back from wxFile object
  void Attach(int fd) { Close(); m_fd = fd; }
  void Detach()       { m_fd = fd_invalid;  }
  int  fd() const { return m_fd; }

  // read/write (unbuffered)
    // returns number of bytes read or ofsInvalid on error
  off_t Read(void *pBuf, off_t nCount);
    // returns true on success
  size_t Write(const void *pBuf, size_t nCount);
    // returns true on success
  bool Write(const wxString& s) { return Write(s.c_str(), s.Len()*sizeof(wxChar)) != 0; }
    // flush data not yet written
  bool Flush();

  // file pointer operations (return ofsInvalid on failure)
    // move ptr ofs bytes related to start/current off_t/end of file
  off_t Seek(off_t ofs, wxSeekMode mode = wxFromStart);
    // move ptr to ofs bytes before the end
  off_t SeekEnd(off_t ofs = 0) { return Seek(ofs, wxFromEnd); }
    // get current off_t
  off_t Tell() const;
    // get current file length
  off_t Length() const;

  // simple accessors
    // is file opened?
  bool IsOpened() const { return m_fd != fd_invalid; }
    // is end of file reached?
  bool Eof() const;
    // is an error occured?
  bool Error() const { return m_error; }

  // dtor closes the file if opened
  ~wxFile();

private:
  // copy ctor and assignment operator are private because
  // it doesn't make sense to copy files this way:
  // attempt to do it will provoke a compile-time error.
  wxFile(const wxFile&);
  wxFile& operator=(const wxFile&);

  int m_fd; // file descriptor or INVALID_FD if not opened
  bool m_error; // error memory
};

// ----------------------------------------------------------------------------
// class wxTempFile: if you want to replace another file, create an instance
// of wxTempFile passing the name of the file to be replaced to the ctor. Then
// you can write to wxTempFile and call Commit() function to replace the old
// file (and close this one) or call Discard() to cancel the modification. If
// you call neither of them, dtor will call Discard().
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxTempFile
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
  bool Write(const void *p, size_t n) { return m_file.Write(p, n) != 0; }
  bool Write(const wxString& str)   { return m_file.Write(str); }

  // different ways to close the file
    // validate changes and delete the old file of name m_strName
  bool Commit();
    // discard changes
  void Discard();

  // dtor calls Discard() if file is still opened
 ~wxTempFile();

private:
  // no copy ctor/assignment operator
  wxTempFile(const wxTempFile&);
  wxTempFile& operator=(const wxTempFile&);

  wxString  m_strName,  // name of the file to replace in Commit()
            m_strTemp;  // temporary file name
  wxFile    m_file;     // the temporary file
};

#endif

#endif
        // _WX_FILEH__
