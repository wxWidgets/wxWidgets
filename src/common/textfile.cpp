///////////////////////////////////////////////////////////////////////////////
// Name:        textfile.cpp
// Purpose:     implementation of wxTextFile class
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     03.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers
// ============================================================================

#ifdef __GNUG__
#pragma implementation "textfile.h"
#endif

#include  "wx/wxprec.h"

#ifdef    __BORLANDC__
  #pragma hdrstop
#endif  //__BORLANDC__

#include  <wx/string.h>
#include  <wx/intl.h>
#include  <wx/file.h>
#include  <wx/log.h>
#include  <wx/textfile.h>

// ============================================================================
// wxTextFile class implementation
// ============================================================================

// ----------------------------------------------------------------------------
// static variables
// ----------------------------------------------------------------------------
  
// default type is the native one
const wxTextFile::Type wxTextFile::typeDefault = wxTextFile::
#if   defined(__WXMSW__)
  Type_Dos;
#elif defined(__UNIX__)
  Type_Unix;
#elif defined(__MAC__)
  Type_Mac;
  // if you feel brave, remove the next line
  #error  "wxTextFile: code for Mac files is untested."
#else
  Type_None;
  #error  "wxTextFile: unsupported platform."
#endif


// ----------------------------------------------------------------------------
// ctors & dtor
// ----------------------------------------------------------------------------

wxTextFile::wxTextFile(const wxString& strFile) : m_strFile(strFile)
{
}

wxTextFile::~wxTextFile()
{
  // m_file dtor called automatically
}

// ----------------------------------------------------------------------------
// file operations
// ----------------------------------------------------------------------------

bool wxTextFile::Open(const wxString& strFile)
{
  m_strFile = strFile;
  return Open();
}

bool wxTextFile::Open()
{
  // file name must be either given in ctor or in Open(const wxString&)
  wxASSERT( !m_strFile.IsEmpty() );

  // open file in read-only mode
  if ( !m_file.Open(m_strFile) )
    return FALSE;

  // read file into memory
  bool bRet = Read();

  m_file.Close();

  return bRet;
}

// analyse some lines of the file trying to guess it's type.
// if it fails, it assumes the native type for our platform.
wxTextFile::Type wxTextFile::GuessType() const
{
  // file should be opened and we must be in it's beginning
  wxASSERT( m_file.IsOpened() && m_file.Tell() == 0 );

  // scan the file lines
  uint nUnix = 0,     // number of '\n's alone
       nDos  = 0,     // number of '\r\n'
       nMac  = 0;     // number of '\r's

  // we take MAX_LINES_SCAN in the beginning, middle and the end of file
  #define MAX_LINES_SCAN    (10)
  uint nCount = m_aLines.Count() / 3,
       nScan =  nCount > 3*MAX_LINES_SCAN ? MAX_LINES_SCAN : nCount / 3;

  #define   AnalyseLine(n)              \
    switch ( m_aTypes[n] ) {            \
      case Type_Unix: nUnix++; break;   \
      case Type_Dos:  nDos++;  break;   \
      case Type_Mac:  nMac++;  break;   \
    }

  uint n;
  for ( n = 0; n < nScan; n++ )     // the beginning
    AnalyseLine(n);
  for ( n = (nCount - nScan)/2; n < (nCount + nScan)/2; n++ )
    AnalyseLine(n);
  for ( n = nCount - nScan; n < nCount; n++ )
    AnalyseLine(n);

  #undef   AnalyseLine

  // interpret the results (@@ far from being even 50% fool proof)
  if ( nDos + nUnix + nMac == 0 ) {
    // no newlines at all
    wxLogWarning("'%s' is probably a binary file.", m_strFile.c_str());
  }
  else {
    #define   GREATER_OF(t1, t2) n##t1 == n##t2 ? typeDefault               \
                                                : n##t1 > n##t2 ? Type_##t1 \
                                                                : Type_##t2

    if ( nDos > nUnix )
      return GREATER_OF(Dos, Mac);
    else if ( nDos < nUnix )
      return GREATER_OF(Unix, Mac);
    else {
      // nDos == nUnix
      return nMac > nDos ? Type_Mac : typeDefault;
    }

    #undef    GREATER_OF
  }

  return typeDefault;
}

bool wxTextFile::Read()
{
  // file should be opened and we must be in it's beginning
  wxASSERT( m_file.IsOpened() && m_file.Tell() == 0 );

  wxString str;
  char ch, chLast = '\0';
  while ( !m_file.Eof() ) {
    // @@ should really use a buffer for efficiency
    if ( m_file.Read(&ch, sizeof(ch)) == ofsInvalid ) {
      // read error
      m_file.Close();
      return FALSE;
    }

    #ifdef  __MAC__
      #pragma message("wxTextFile::Read() hasn't been tested with Mac files.")
    #endif

    switch ( ch ) {
      case '\n':
        // Dos/Unix line termination
        m_aLines.Add(str);
        m_aTypes.Add(chLast == '\r' ? Type_Dos : Type_Unix);
        str.Empty();
        chLast = '\n';
        break;

      case '\r':
        if ( chLast == '\r' ) {
          // Mac empty line
          m_aLines.Add("");
          m_aTypes.Add(Type_Mac);
        }
        else
          chLast = '\r';
        break;

      default:
        if ( chLast == '\r' ) {
          // Mac line termination
          m_aLines.Add(str);
          m_aTypes.Add(Type_Mac);
          str = ch;
        }
        else {
          // add to the current line
          str += ch;
        }
    }
  }

  // anything in the last line?
  if ( !str.IsEmpty() ) {
    m_aTypes.Add(Type_None);  // no line terminator
    m_aLines.Add(str);
  }

  return TRUE;
}

bool wxTextFile::Write(Type typeNew)
{
  wxTempFile fileTmp(m_strFile);

  if ( !fileTmp.IsOpened() ) {
    wxLogError("can't write file '%s' to disk.", m_strFile.c_str());
    return FALSE;
  }

  uint nCount = m_aLines.Count();
  for ( uint n = 0; n < nCount; n++ ) {
    fileTmp.Write(m_aLines[n] + 
                  GetEOL(typeNew == Type_None ? m_aTypes[n] : typeNew));
  }

  // replace the old file with this one
  return fileTmp.Commit();
}