/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/fs_data.cpp
// Purpose:     DATA scheme file system
// Author:      Vyacheslav Lisovski
// Copyright:   (c) 2023 Vyacheslav Lisovski
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_FILESYSTEM

#include "wx/url.h"
#include "wx/filesys.h"
#include <wx/sstream.h>
#include <wx/base64.h>
#include <wx/mstream.h>
#include "wx/fs_data.h"

// This stream holds the buffer and deletes when destroyed
class wxBufferedMemoryInputStream : public wxMemoryInputStream
{
    wxMemoryBuffer m_buffer;
public:
    wxBufferedMemoryInputStream(wxMemoryBuffer& buffer) :
        wxMemoryInputStream(buffer.GetData(), buffer.GetDataLen())
    {
        m_buffer = buffer;
    };
};

// URL syntax: data:[<mediatype>][;base64],<data>
static int GetMetadata(const wxString& location, wxString& mediatype, wxString& encoding)
{
    int dataPos = location.Find(",");
    if (dataPos > 0)
    {
        int hdrPos = location.Find(":");
        if ((hdrPos > 0))
        {
            wxString metadata(location, hdrPos + 1, dataPos - hdrPos - 1);

            int encPos = metadata.Find(";");

            if (encPos > 0)
                encoding = metadata.Right(metadata.Len() - encPos - 1);
            else
                encPos = metadata.Len();

            mediatype = metadata.Left(encPos);
        }
        ++dataPos;
    }
    return dataPos;
}

// ----------------------------------------------------------------------------
// wxDataSchemeFSHandler
// ----------------------------------------------------------------------------

bool wxDataSchemeFSHandler::CanOpen(const wxString& location)
{
#if wxUSE_URL
    return GetProtocol(location) == "data";
#endif
    return false;
}

wxFSFile* wxDataSchemeFSHandler::OpenFile(wxFileSystem& WXUNUSED(fs),
                                          const wxString& location)
{
#if !wxUSE_URL
    return nullptr;
#else
    wxString mediatype, encoding;
    int dataPos = GetMetadata(location, mediatype, encoding);

    if (dataPos < 0)
        return nullptr;
    
    if (mediatype.IsEmpty())
        mediatype = "text/plain";

    wxInputStream* stream = nullptr;
    if (encoding.IsEmpty())
    {
        stream = new wxStringInputStream(
            wxURL::Unescape(location.Right(location.Len() - dataPos)));
    }
    else if (encoding == "base64")
    {
        stream = new wxBufferedMemoryInputStream(
            wxBase64Decode(location.Right(location.Len() - dataPos)));
    }

    if (stream)
    {
        return new wxFSFile(stream,
                            "",
                            mediatype,
                            ""
#if wxUSE_DATETIME
                            , wxDateTime::Now()
#endif // wxUSE_DATETIME
                    );
    }

    return nullptr;
#endif
}

#endif // wxUSE_FILESYSTEM
