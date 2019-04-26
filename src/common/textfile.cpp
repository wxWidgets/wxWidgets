///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/textfile.cpp
// Purpose:     implementation of wxTextFile class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     03.04.98
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


bool wxTextFile::OnOpen(const wxString &strBufferName, wxTextBufferOpenMode openMode)
{
    wxFile::OpenMode fileOpenMode = wxFile::read_write;

    switch ( openMode )
    {
        case ReadAccess:
            fileOpenMode = wxFile::read;
            break;

        case WriteAccess:
            fileOpenMode = wxFile::write;
            break;
    }

    if ( fileOpenMode == wxFile::read_write )
    {
        // This must mean it hasn't been initialized in the switch above.
        wxFAIL_MSG( wxT("unknown open mode in wxTextFile::Open") );
        return false;
    }

    return m_file.Open(strBufferName, fileOpenMode);
}


bool wxTextFile::OnClose()
{
    return m_file.Close();
}


bool wxTextFile::OnRead(const wxMBConv& conv)
{
    // file should be opened
    wxASSERT_MSG( m_file.IsOpened(), wxT("can't read closed file") );

    wxString str;
    if ( !m_file.ReadAll(&str, conv) )
    {
        wxLogError(_("Failed to read text file \"%s\"."), GetName());
        return false;
    }

    // now break the buffer in lines

    // the beginning of the current line, changes inside the loop
    wxString::const_iterator lineStart = str.begin();
    const wxString::const_iterator end = str.end();
    for ( wxString::const_iterator p = lineStart; p != end; ++p )
    {
        const wxChar ch = *p;
        if ( ch == '\r' || ch == '\n' )
        {
            // Determine the kind of line ending this is.
            wxTextFileType lineType = wxTextFileType_None;
            if ( ch == '\r' )
            {
                wxString::const_iterator next = p + 1;
                if ( next != end && *next == '\n' )
                    lineType = wxTextFileType_Dos;
                else
                    lineType = wxTextFileType_Mac;
            }
            else // ch == '\n'
            {
                lineType = wxTextFileType_Unix;
            }

            AddLine(wxString(lineStart, p), lineType);

            // DOS EOL is the only one consisting of two chars, not one.
            if ( lineType == wxTextFileType_Dos )
                ++p;

            lineStart = p + 1;
        }
    }

    // anything in the last line?
    if ( lineStart != end )
    {
        // Add the last line; notice that it is certainly not terminated with a
        // newline, otherwise it would be handled above.
        wxString lastLine(lineStart, end);
        AddLine(lastLine, wxTextFileType_None);
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
        wxLogError(_("can't write buffer '%s' to disk."), m_strBufferName);
        return false;
    }

    // Writing to wxTempFile in reasonably-sized chunks is much faster than
    // doing it line by line.
    const size_t chunk_size = 16384;
    wxString chunk;
    chunk.reserve(chunk_size);

    size_t nCount = GetLineCount();
    for ( size_t n = 0; n < nCount; n++ )
    {
        chunk += GetLine(n) +
                  GetEOL(typeNew == wxTextFileType_None ? GetLineType(n)
                                                        : typeNew);
        if ( chunk.size() >= chunk_size )
        {
            fileTmp.Write(chunk, conv);
            chunk.clear();
        }
    }

    if ( !chunk.empty() )
        fileTmp.Write(chunk, conv);

    // replace the old file with this one
    return fileTmp.Commit();
}

#endif // wxUSE_TEXTFILE
