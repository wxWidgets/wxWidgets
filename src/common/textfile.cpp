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
    wxASSERT( m_file.IsOpened() &&
                (m_file.GetKind() != wxFILE_KIND_DISK || m_file.Tell() == 0) );

    static const size_t BUF_SIZE = 1024;
#if wxUSE_UNICODE
    static const size_t NUL_SIZE = 4;
#else
    static const size_t NUL_SIZE = 1;
#endif

    char buf[BUF_SIZE + NUL_SIZE];
    wxChar chLast = '\0';
    wxString str;

    for ( ;; )
    {
        // leave space for trailing NUL
        ssize_t nRead = m_file.Read(buf, BUF_SIZE);

        if ( nRead == wxInvalidOffset )
        {
            // read error (error message already given in wxFile::Read)
            return false;
        }

        if ( nRead == 0 )
            break;

#if wxUSE_UNICODE
        // we have to properly NUL-terminate the string for any encoding it may
        // use -- 4 NULs should be enough for everyone (this is why we add 4
        // extra bytes to the buffer)
        buf[nRead] =
        buf[nRead + 1] =
        buf[nRead + 2] =
        buf[nRead + 3] = '\0';

        // append to the remains of the last block, don't overwrite
        wxString strbuf(buf, conv);
        if ( strbuf.empty() )
        {
            // conversion failed
            return false;
        }

        str += strbuf;
#else // ANSI
        wxUnusedVar(conv);
        buf[nRead] = '\0';
        str += buf;
#endif // wxUSE_UNICODE/!wxUSE_UNICODE


        // the beginning of the current line, changes inside the loop
        const wxChar *lineStart = str.begin();
        const wxChar * const end = str.end();
        for ( const wxChar *p = lineStart; p != end; p++ )
        {
            const wxChar ch = *p;
            switch ( ch )
            {
                case '\n':
                    // could be a DOS or Unix EOL
                    if ( chLast == '\r' )
                    {
                        AddLine(wxString(lineStart, p - 1), wxTextFileType_Dos);
                    }
                    else // bare '\n', Unix style
                    {
                        AddLine(wxString(lineStart, p), wxTextFileType_Unix);
                    }

                    lineStart = p + 1;
                    break;

                case '\r':
                    if ( chLast == '\r' )
                    {
                        // Mac empty line
                        AddLine(wxEmptyString, wxTextFileType_Mac);
                        lineStart = p + 1;
                    }
                    //else: we don't what this is yet -- could be a Mac EOL or
                    //      start of DOS EOL so wait for next char
                    break;

                default:
                    if ( chLast == '\r' )
                    {
                        // Mac line termination
                        AddLine(wxString(lineStart, p - 1), wxTextFileType_Mac);
                        lineStart = p;
                    }
            }

            chLast = ch;
        }

        // remove the part we already processed
        str.erase(0, lineStart - str.begin());
    }

    // anything in the last line?
    if ( !str.empty() )
    {
        AddLine(str, wxTextFileType_None); // no line terminator
    }

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

