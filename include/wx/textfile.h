///////////////////////////////////////////////////////////////////////////////
// Name:        textfile.h
// Purpose:     class wxTextFile to work with text files of _small_ size
//              (file is fully loaded in memory) and which understands CR/LF
//              differences between platforms.
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     03.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef   _TEXTFILE_H
#define   _TEXTFILE_H

#ifdef __GNUG__
#pragma interface "textfile.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/file.h"
#include "wx/dynarray.h"

// ----------------------------------------------------------------------------
// wxTextFile
// ----------------------------------------------------------------------------
class wxTextFile
{
public:
  // constants
  enum Type
  {
    Type_None,  // incomplete (the last line of the file only)
    Type_Unix,  // line is terminated with 'LF' = 0xA = 10 = '\n'
    Type_Dos,   //                         'CR' 'LF'
    Type_Mac    //                         'CR' = 0xD = 13 = '\r'
  };

  // default type for current platform (determined at compile time)
  static const Type typeDefault;

  // ctors
    // def ctor, use Open(string)
  wxTextFile() { }
    // 
  wxTextFile(const wxString& strFile);

  // file operations
    // file exists?
  bool Exists() const;
    // Open() also loads file in memory on success
  bool Open();
    // same as Open() but with (another) file name
  bool Open(const wxString& strFile);
    // closes the file and frees memory, losing all changes
  bool Close();
    // is file currently opened?
  bool IsOpened() const { return m_file.IsOpened(); }

  // accessors
    // get the number of lines in the file
  size_t GetLineCount() const { return m_aLines.Count(); }
    // the returned line may be modified (but don't add CR/LF at the end!)
  wxString& GetLine(size_t n)    const { return m_aLines[n]; }
  wxString& operator[](size_t n) const { return m_aLines[n]; }

    // the current line has meaning only when you're using
    // GetFirstLine()/GetNextLine() functions, it doesn't get updated when
    // you're using "direct access" i.e. GetLine()
  size_t GetCurrentLine() const { return m_nCurLine; }
  void GoToLine(size_t n) { m_nCurLine = n; }
  bool Eof() const { return m_nCurLine == m_aLines.Count(); }

    // these methods allow more "iterator-like" traversal of the list of
    // lines, i.e. you may write something like:
    //  for ( str = GetFirstLine(); !Eof(); str = GetNextLine() ) { ... }

    // @@@ const is commented out because not all compilers understand
    //     'mutable' keyword yet (m_nCurLine should be mutable)
  wxString& GetFirstLine() /* const */ { return m_aLines[m_nCurLine = 0]; }
  wxString& GetNextLine()  /* const */ { return m_aLines[++m_nCurLine];   }
  wxString& GetPrevLine()  /* const */
    { wxASSERT(m_nCurLine > 0); return m_aLines[--m_nCurLine];   }
  wxString& GetLastLine() /* const */
    { return m_aLines[m_nCurLine = m_aLines.Count() - 1]; }

    // get the type of the line (see also GetEOL)
  Type GetLineType(size_t n) const { return m_aTypes[n]; }
    // guess the type of file (m_file is supposed to be opened)
  Type GuessType() const;
    // get the name of the file
  const char *GetName() const { return m_strFile.c_str(); }

  // add/remove lines
    // add a line to the end
  void AddLine(const wxString& str, Type type = typeDefault) 
    { m_aLines.Add(str); m_aTypes.Add(type); }
    // insert a line before the line number n
  void InsertLine(const wxString& str, size_t n, Type type = typeDefault) 
    { m_aLines.Insert(str, n); m_aTypes.Insert(type, n); }
    // delete one line
  void RemoveLine(size_t n) { m_aLines.Remove(n); m_aTypes.Remove(n); }

  // change the file on disk (default argument means "don't change type")
  // possibly in another format
  bool Write(Type typeNew = Type_None);

  // get the file termination string
  inline static const char *GetEOL(Type type = typeDefault)
  {
    switch ( type ) {
      case Type_None: return "";
      case Type_Unix: return "\n";
      case Type_Dos:  return "\r\n";
      case Type_Mac:  return "\r";

      default:
        wxFAIL_MSG("bad file type in wxTextFile::GetEOL.");
        return (const char *) NULL;
    }
  }

  // dtor
  ~wxTextFile();

private:
  // copy ctor/assignment operator not implemented
  wxTextFile(const wxTextFile&);
  wxTextFile& operator=(const wxTextFile&);

  // read the file in memory (m_file is supposed to be just opened)
  bool Read();

  WX_DEFINE_ARRAY(Type, ArrayFileType);

  wxFile        m_file;     // current file

  ArrayFileType m_aTypes;   // type of each line
  wxArrayString m_aLines;   // lines of file

  size_t        m_nCurLine; // number of current line in the file

  wxString      m_strFile;  // name of the file
};

#endif  //_TEXTFILE_H
