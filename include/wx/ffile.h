/////////////////////////////////////////////////////////////////////////////
// Name:        ffile.h
// Purpose:     wxFFile - encapsulates "FILE *" stream
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.07.99
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_FFILE_H_
#define   _WX_FFILE_H_

#ifdef __GNUG__
    #pragma interface "ffile.h"
#endif

#if wxUSE_FILE

#ifndef WX_PRECOMP
  #include  "wx/string.h"
  #include  "wx/filefn.h"
#endif

#include <stdio.h>

// ----------------------------------------------------------------------------
// class wxFFile: standard C stream library IO
//
// NB: for space efficiency this class has no virtual functions, including
//     dtor which is _not_ virtual, so it shouldn't be used as a base class.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFFile
{
public:
  // ctors
  // -----
    // def ctor
  wxFFile() { m_fp = NULL; }
    // open specified file (may fail, use IsOpened())
  wxFFile(const wxChar *filename, const char *mode = "r");
    // attach to (already opened) file
  wxFFile(FILE *fp) { m_fp = fp; }

  // open/close
    // open a file (existing or not - the mode controls what happens)
  bool Open(const wxChar *filename, const char *mode = "r");
    // closes the opened file (this is a NOP if not opened)
  bool Close();

  // assign an existing file descriptor and get it back from wxFFile object
  void Attach(FILE *fp, const wxString& name = wxT(""))
    { Close(); m_fp = fp; m_name = name; }
  void Detach() { m_fp = NULL; }
  FILE *fp() const { return m_fp; }

  // read/write (unbuffered)
    // read all data from the file into a string (useful for text files)
  bool ReadAll(wxString *str);
    // returns number of bytes read - use Eof() and Error() to see if an error
    // occured or not
  size_t Read(void *pBuf, size_t nCount);
    // returns the number of bytes written
  size_t Write(const void *pBuf, size_t nCount);
    // returns true on success
  bool Write(const wxString& s, wxMBConv& conv = wxConvLibc)
  {
      wxWX2MBbuf buf = s.mb_str();
      size_t size = strlen(buf);
      return Write((const char *) buf, size) == size;
  }
    // flush data not yet written
  bool Flush();

  // file pointer operations (return ofsInvalid on failure)
    // move ptr ofs bytes related to start/current pos/end of file
  bool Seek(long ofs, wxSeekMode mode = wxFromStart);
    // move ptr to ofs bytes before the end
  bool SeekEnd(long ofs = 0) { return Seek(ofs, wxFromEnd); }
    // get current position in the file
  size_t Tell() const;
    // get current file length
  size_t Length() const;

  // simple accessors
    // is file opened?
  bool IsOpened() const { return m_fp != NULL; }
    // is end of file reached?
  bool Eof() const { return feof(m_fp) != 0; }
    // is an error occured?
  bool Error() const { return ferror(m_fp) != 0; }
    // get the file name
  const wxString& GetName() const { return m_name; }

  // dtor closes the file if opened
  ~wxFFile() { Close(); }

private:
  // copy ctor and assignment operator are private because it doesn't make
  // sense to copy files this way: attempt to do it will provoke a compile-time
  // error.
  wxFFile(const wxFFile&);
  wxFFile& operator=(const wxFFile&);

  FILE *m_fp;       // IO stream or NULL if not opened

  wxString m_name;  // the name of the file (for diagnostic messages)
};

#endif // wxUSE_FILE

#endif // _WX_FFILE_H_

