///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/textfile.cpp
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

#if !wxUSE_FILE || !wxUSE_TEXTBUFFER
    #undef wxUSE_TEXTFILE
    #define wxUSE_TEXTFILE 0
#endif // wxUSE_FILE

#if wxUSE_TEXTFILE

#ifndef WX_PRECOMP
    #include  "wx/string.h"
    #include  "wx/intl.h"
    #include  "wx/file.h"
    #include  "wx/log.h"
#endif

#include "wx/textfile.h"

// ============================================================================
// wxTextFile class implementation
// ============================================================================

wxTextFile::wxTextFile(const wxString& strFileName)
          : wxTextBuffer(strFileName)
{
}


// ----------------------------------------------------------------------------
// file operations
// ----------------------------------------------------------------------------

bool wxTextFile::OnExists() const
{
    return wxFile::Exists(m_strBufferName);
}


bool wxTextFile::OnOpen(const wxString &strBufferName, wxTextBufferOpenMode OpenMode)
{
    wxFile::OpenMode FileOpenMode = wxFile::read;
    int                             nAssertVal = 0;

    switch (OpenMode)
  {
        case ReadAccess :
            FileOpenMode = wxFile::read;
            break;
        case WriteAccess :
            FileOpenMode = wxFile::write;
            break;
        default :
            wxASSERT(nAssertVal); // Should not happen.
            break;
  }

    return m_file.Open(strBufferName.c_str(), FileOpenMode);
}


bool wxTextFile::OnClose()
{
    return m_file.Close();
}


bool wxTextFile::OnRead(wxMBConv& conv)
{
  // file should be opened and we must be in it's beginning
  wxASSERT( m_file.IsOpened() && m_file.Tell() == 0 );

#if wxUSE_UNICODE
  char conv_mbBuf[2];
  wchar_t conv_wcBuf[2];
  conv_mbBuf[1] = 0;
#else
  (void)conv;
#endif

  wxString str;
  char ch, chLast = '\0';
  char buf[1024];
  int n, nRead;
  do {
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
                AddLine(str, chLast == '\r' ? wxTextFileType_Dos
                                      : wxTextFileType_Unix);
          str.Empty();
          chLast = '\n';
          break;

        case '\r':
          if ( chLast == '\r' ) {
            // Mac empty line
                    AddLine(wxEmptyString, wxTextFileType_Mac);
          }
          else
            chLast = '\r';
          break;

        default:
          if ( chLast == '\r' ) {
            // Mac line termination
                    AddLine(str, wxTextFileType_Mac);
            chLast = ch;
#if wxUSE_UNICODE
            conv_mbBuf[0] = ch;
            if (conv.MB2WC(conv_wcBuf, conv_mbBuf, 2) == (size_t)-1)
                conv_wcBuf[0] = ch;
            str = conv_wcBuf[0];
#else
            str = ch;
#endif
          }
          else {
            // add to the current line
#if wxUSE_UNICODE
            conv_mbBuf[0] = ch;
            if (conv.MB2WC(conv_wcBuf, conv_mbBuf, 2) == (size_t)-1)
                conv_wcBuf[0] = ch;
            str += conv_wcBuf[0];
#else
            str += ch;
#endif
          }
      }
    }
  } while ( nRead == WXSIZEOF(buf) );

  // anything in the last line?
  if ( !str.IsEmpty() ) {
        AddLine(str, wxTextFileType_None);  // no line terminator
  }

  return TRUE;
}


bool wxTextFile::OnWrite(wxTextFileType typeNew, wxMBConv& conv)
{
    wxTempFile fileTmp(m_strBufferName);

    if ( !fileTmp.IsOpened() ) {
        wxLogError(_("can't write buffer '%s' to disk."), m_strBufferName.c_str());
        return FALSE;
    }

    size_t nCount = GetLineCount();
    for ( size_t n = 0; n < nCount; n++ ) {
        fileTmp.Write(GetLine(n) +
                      GetEOL(typeNew == wxTextFileType_None ? GetLineType(n)
                                                            : typeNew),
                      conv);
    }

    // replace the old file with this one
    return fileTmp.Commit();
}

#endif // wxUSE_TEXTFILE

