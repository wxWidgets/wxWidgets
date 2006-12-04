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
#include "wx/buffer.h"

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


bool wxTextFile::OnRead(const wxMBConv& conv)
{
    // file should be opened and we must be in it's beginning
    wxASSERT( m_file.IsOpened() && m_file.Tell() == 0 );

    // read the entire file in memory: this is not the most efficient thing to
    // do but there is no good way to avoid it in Unicode build because if we
    // read the file block by block we can't convert each block to Unicode
    // separately (the last multibyte char in the block might be only partially
    // read and so the conversion would fail) and, as the file contents is kept
    // in memory by wxTextFile anyhow, it shouldn't be a big problem to read
    // the file entirely
    const size_t bufSize = (size_t)(m_file.Length() + 4 /* for trailing NULs */ );
    size_t bufPos = 0;
    wxCharBuffer buf(bufSize - 1 /* it adds 1 internally */);

    char block[1024];
    for ( bool eof = false; !eof; )
    {
        // try to read up to the size of the entire block
        ssize_t nRead = m_file.Read(block, WXSIZEOF(block));

        if ( nRead == wxInvalidOffset )
        {
            // read error (error message already given in wxFile::Read)
            return false;
        }

        if ( nRead == 0 )
            break;

        // this shouldn't happen but don't overwrite the buffer if it does
        wxCHECK_MSG( bufPos + nRead <= bufSize, false,
                     _T("read more than file length?") );

        // append to the buffer
        memcpy(buf.data() + bufPos, block, nRead);
        bufPos += nRead;
    }

    const wxString str(buf, conv, bufPos);

    // this doesn't risk to happen in ANSI build
#if wxUSE_UNICODE
    if ( bufSize > 4 && str.empty() )
    {
        wxLogError(_("Failed to convert file \"%s\" to Unicode."), GetName());
        return false;
    }
#endif // wxUSE_UNICODE

    free(buf.release()); // we don't need this memory any more


    // now break the buffer in lines

    // last processed character, we need to know if it was a CR or not
    wxChar chLast = '\0';

    // the beginning of the current line, changes inside the loop
    wxString::const_iterator lineStart = str.begin();
    const wxString::const_iterator end = str.end();
    for ( wxString::const_iterator p = lineStart; p != end; p++ )
    {
        const wxChar ch = *p;
        switch ( ch )
        {
            case '\n':
                // could be a DOS or Unix EOL
                if ( chLast == '\r' )
                {
                    if ( p - 1 >= lineStart )
                    {
                        AddLine(wxString(lineStart, p - 1), wxTextFileType_Dos);
                    }
                    else
                    {
                        // there were two line endings, so add an empty line:
                        AddLine(wxEmptyString, wxTextFileType_Dos);
                    }
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
                //else: we don't know what this is yet -- could be a Mac EOL or
                //      start of DOS EOL so wait for next char
                break;

            default:
                if ( chLast == '\r' )
                {
                    // Mac line termination
                    if ( p - 1 >= lineStart )
                    {
                        AddLine(wxString(lineStart, p - 1), wxTextFileType_Mac);
                    }
                    else
                    {
                        // there were two line endings, so add an empty line:
                        AddLine(wxEmptyString, wxTextFileType_Mac);
                    }
                    lineStart = p;
                }
        }

        chLast = ch;
    }

    // anything in the last line?
    if ( lineStart != end )
    {
        // add unterminated last line
        AddLine(wxString(lineStart, end), wxTextFileType_None);
    }

    return true;
}


bool wxTextFile::OnWrite(wxTextFileType typeNew, const wxMBConv& conv)
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
