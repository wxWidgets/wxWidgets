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

#if !wxUSE_FILE
    #undef wxUSE_TEXTFILE
    #define wxUSE_TEXTFILE 0
#endif // wxUSE_FILE

#ifndef WX_PRECOMP
    #include  "wx/string.h"
    #include  "wx/intl.h"
    #include  "wx/file.h"
    #include  "wx/log.h"
#endif

#include  "wx/textfile.h"

// ============================================================================
// wxTextFile class implementation
// ============================================================================

// ----------------------------------------------------------------------------
// static methods (always compiled in)
// ----------------------------------------------------------------------------

// default type is the native one
const wxTextFileType wxTextFile::typeDefault =
#if   defined(__WINDOWS__)
  wxTextFileType_Dos;
#elif defined(__UNIX__)
  wxTextFileType_Unix;
#elif defined(__WXMAC__)
  wxTextFileType_Mac;
#elif defined(__WXPM__)
  wxTextFileType_Os2;
#else
  wxTextFileType_None;
  #error  "wxTextFile: unsupported platform."
#endif

const wxChar *wxTextFile::GetEOL(wxTextFileType type)
{
    switch ( type ) {
      default:
        wxFAIL_MSG(wxT("bad file type in wxTextFile::GetEOL."));
        // fall through nevertheless - we must return something...

      case wxTextFileType_None: return wxT("");
      case wxTextFileType_Unix: return wxT("\n");
      case wxTextFileType_Dos:  return wxT("\r\n");
      case wxTextFileType_Mac:  return wxT("\r");
    }
}

wxString wxTextFile::Translate(const wxString& text, wxTextFileType type)
{
    // don't do anything if there is nothing to do
    if ( type == wxTextFileType_None )
        return text;

    // GRG: don't do anything either if it is empty
    if ( text.IsEmpty() )
        return text;

    wxString eol = GetEOL(type), result;

    // optimization: we know that the length of the new string will be about
    // the same as the length of the old one, so prealloc memory to aviod
    // unnecessary relocations
    result.Alloc(text.Len());

    wxChar chLast = 0;
    for ( const wxChar *pc = text.c_str(); *pc; pc++ )
    {
        wxChar ch = *pc;
        switch ( ch ) {
            case _T('\n'):
                // Dos/Unix line termination
                result += eol;
                chLast = 0;
                break;

            case _T('\r'):
                if ( chLast == _T('\r') ) {
                    // Mac empty line
                    result += eol;
                }
                else {
                    // just remember it: we don't know whether it is just "\r"
                    // or "\r\n" yet
                    chLast = _T('\r');
                }
                break;

            default:
                if ( chLast == _T('\r') ) {
                    // Mac line termination
                    result += eol;
                }

                // add to the current line
                result += ch;
        }
    }

    if ( chLast ) {
        // trailing '\r'
        result += eol;
    }

    return result;
}

#if wxUSE_TEXTFILE

// ----------------------------------------------------------------------------
// ctors & dtor
// ----------------------------------------------------------------------------

wxTextFile::wxTextFile(const wxString& strFile) : m_strFile(strFile)
{
  m_nCurLine = 0;
  m_isOpened = FALSE;
}

wxTextFile::~wxTextFile()
{
  // m_file dtor called automatically
}

// ----------------------------------------------------------------------------
// file operations
// ----------------------------------------------------------------------------

bool wxTextFile::Exists() const
{
    return wxFile::Exists(m_strFile);
}

bool wxTextFile::Create(const wxString& strFile)
{
  m_strFile = strFile;

  return Create();
}

bool wxTextFile::Create()
{
  // file name must be either given in ctor or in Create(const wxString&)
  wxASSERT( !m_strFile.IsEmpty() );

  // if the file already exists do nothing
  if ( Exists() ) return FALSE;
  
  if ( m_file.Open(m_strFile, wxFile::write) )
  {
    m_file.Close();
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

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
  m_isOpened = Read();

  m_file.Close();

  return m_isOpened;
}

// analyse some lines of the file trying to guess it's type.
// if it fails, it assumes the native type for our platform.
wxTextFileType wxTextFile::GuessType() const
{
  // file should be opened and we must be in it's beginning
  wxASSERT( m_file.IsOpened() && m_file.Tell() == 0 );

  // scan the file lines
  size_t nUnix = 0,     // number of '\n's alone
       nDos  = 0,     // number of '\r\n'
       nMac  = 0;     // number of '\r's

  // we take MAX_LINES_SCAN in the beginning, middle and the end of file
  #define MAX_LINES_SCAN    (10)
  size_t nCount = m_aLines.Count() / 3,
       nScan =  nCount > 3*MAX_LINES_SCAN ? MAX_LINES_SCAN : nCount / 3;

  #define   AnalyseLine(n)              \
    switch ( m_aTypes[n] ) {            \
      case wxTextFileType_Unix: nUnix++; break;   \
      case wxTextFileType_Dos:  nDos++;  break;   \
      case wxTextFileType_Mac:  nMac++;  break;   \
      default: wxFAIL_MSG(_("unknown line terminator")); \
    }

  size_t n;
  for ( n = 0; n < nScan; n++ )     // the beginning
    AnalyseLine(n);
  for ( n = (nCount - nScan)/2; n < (nCount + nScan)/2; n++ )
    AnalyseLine(n);
  for ( n = nCount - nScan; n < nCount; n++ )
    AnalyseLine(n);

  #undef   AnalyseLine

  // interpret the results (FIXME far from being even 50% fool proof)
  if ( nDos + nUnix + nMac == 0 ) {
    // no newlines at all
    wxLogWarning(_("'%s' is probably a binary file."), m_strFile.c_str());
  }
  else {
    #define   GREATER_OF(t1, t2) n##t1 == n##t2 ? typeDefault               \
                                                : n##t1 > n##t2             \
                                                    ? wxTextFileType_##t1   \
                                                    : wxTextFileType_##t2

    // Watcom C++ doesn't seem to be able to handle the macro
#if !defined(__WATCOMC__)
    if ( nDos > nUnix )
      return GREATER_OF(Dos, Mac);
    else if ( nDos < nUnix )
      return GREATER_OF(Unix, Mac);
    else {
      // nDos == nUnix
      return nMac > nDos ? wxTextFileType_Mac : typeDefault;
    }
#endif // __WATCOMC__

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
  char buf[1024];
  int n, nRead;
  while ( !m_file.Eof() ) {
    nRead = m_file.Read(buf, WXSIZEOF(buf));
    if ( nRead == wxInvalidOffset ) {
      // read error (error message already given in wxFile::Read)
      return FALSE;
    }

    for ( n = 0; n < nRead; n++ ) {
      ch = buf[n];
      switch ( ch ) {
        case '\n':
          // Dos/Unix line termination
          m_aLines.Add(str);
          m_aTypes.Add(chLast == '\r' ? wxTextFileType_Dos
                                      : wxTextFileType_Unix);
          str.Empty();
          chLast = '\n';
          break;

        case '\r':
          if ( chLast == '\r' ) {
            // Mac empty line
            m_aLines.Add(wxEmptyString);
            m_aTypes.Add(wxTextFileType_Mac);
          }
          else
            chLast = '\r';
          break;

        default:
          if ( chLast == '\r' ) {
            // Mac line termination
            m_aLines.Add(str);
            m_aTypes.Add(wxTextFileType_Mac);
            chLast = ch;
            str = ch;
          }
          else {
            // add to the current line
            str += ch;
          }
      }
    }
  }

  // anything in the last line?
  if ( !str.IsEmpty() ) {
    m_aTypes.Add(wxTextFileType_None);  // no line terminator
    m_aLines.Add(str);
  }

  return TRUE;
}

bool wxTextFile::Close()
{
    m_aTypes.Clear();
    m_aLines.Clear();
    m_nCurLine = 0;
    m_isOpened = FALSE;

    return TRUE;
}

bool wxTextFile::Write(wxTextFileType typeNew)
{
  wxTempFile fileTmp(m_strFile);

  if ( !fileTmp.IsOpened() ) {
    wxLogError(_("can't write file '%s' to disk."), m_strFile.c_str());
    return FALSE;
  }

  size_t nCount = m_aLines.Count();
  for ( size_t n = 0; n < nCount; n++ ) {
    fileTmp.Write(m_aLines[n] +
                  GetEOL(typeNew == wxTextFileType_None ? m_aTypes[n]
                                                        : typeNew));
  }

  // replace the old file with this one
  return fileTmp.Commit();
}

#endif // wxUSE_TEXTFILE

