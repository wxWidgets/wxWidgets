///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/textfile.cpp
// Purpose:     implementation of wxTextFile class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     03.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers
// ============================================================================

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/file.h"
    #include "wx/log.h"
#endif

#include "wx/textfile.h"
#include "wx/filename.h"

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
    wxFile::OpenMode FileOpenMode;

    switch ( OpenMode )
    {
        default:
            wxFAIL_MSG( _T("unknown open mode in wxTextFile::Open") );
            // fall through

        case ReadAccess :
            FileOpenMode = wxFile::read;
            break;

        case WriteAccess :
            FileOpenMode = wxFile::write;
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

    char *strBuf, *strPtr, *strEnd;
    char ch, chLast = '\0';
    char buf[1024];
    int n, nRead;

    strPtr = strBuf = new char[1024];
    strEnd = strBuf + 1024;

    do
    {
        nRead = m_file.Read(buf, WXSIZEOF(buf));
        if ( nRead == wxInvalidOffset )
        {
            // read error (error message already given in wxFile::Read)
            delete[] strBuf;
            return false;
        }

        for (n = 0; n < nRead; n++)
        {
            ch = buf[n];
            switch ( ch )
            {
                case '\n':
                    // Dos/Unix line termination
                    *strPtr = '\0';
                    AddLine(wxString(strBuf, conv),
                            chLast == '\r' ? wxTextFileType_Dos
                                           : wxTextFileType_Unix);
                    strPtr = strBuf;
                    chLast = '\n';
                    break;

                case '\r':
                    if ( chLast == '\r' )
                    {
                        // Mac empty line
                        AddLine(wxEmptyString, wxTextFileType_Mac);
                    }
                    else
                        chLast = '\r';
                    break;

                default:
                    if ( chLast == '\r' )
                    {
                        // Mac line termination
                        *strPtr = '\0';
                        AddLine(wxString(strBuf, conv), wxTextFileType_Mac);
                        chLast = ch;
                        strPtr = strBuf;
                        *(strPtr++) = ch;
                    }
                    else
                    {
                        // add to the current line
                        *(strPtr++) = ch;
                        if ( strPtr == strEnd )
                        {
                            // we must allocate more memory
                            size_t size = strEnd - strBuf;
                            char *newBuf = new char[size + 1024];
                            memcpy(newBuf, strBuf, size);
                            delete[] strBuf;
                            strBuf = newBuf;
                            strEnd = strBuf + size + 1024;
                            strPtr = strBuf + size;
                        }
                    }
            }
        }
    } while ( nRead == WXSIZEOF(buf) );

    // anything in the last line?
    if ( strPtr != strBuf )
    {
        *strPtr = '\0';
        AddLine(wxString(strBuf, conv),
                wxTextFileType_None); // no line terminator
    }

    delete[] strBuf;
    return true;
}


bool wxTextFile::OnWrite(wxTextFileType typeNew, wxMBConv& conv)
{
    wxFileName fn = m_strBufferName;

    // We do NOT want wxPATH_NORM_CASE here, or the case will not
    // be preserved.
    if ( !fn.IsAbsolute() )
        fn.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE |
                     wxPATH_NORM_ABSOLUTE | wxPATH_NORM_LONG);

    wxTempFile fileTmp(fn.GetFullPath());

    if ( !fileTmp.IsOpened() ) {
        wxLogError(_("can't write buffer '%s' to disk."), m_strBufferName.c_str());
        return false;
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

