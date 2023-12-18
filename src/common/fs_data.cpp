/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/fs_data.cpp
// Purpose:     DATA scheme file system
// Author:      Vyacheslav Lisovski
// Copyright:   (c) 2023 Vyacheslav Lisovski
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_FILESYSTEM

#include "wx/fs_data.h"

#include "wx/base64.h"
#include "wx/filesys.h"
#include "wx/mstream.h"
#include "wx/sstream.h"
#include "wx/url.h"

// This stream holds the buffer and deletes when destroyed
class wxBufferedMemoryInputStream : public wxMemoryInputStream
{
    wxMemoryBuffer m_buffer;
public:
    wxBufferedMemoryInputStream(const wxMemoryBuffer& buffer) :
        wxMemoryInputStream(buffer.GetData(), buffer.GetDataLen())
    {
        m_buffer = buffer;
    }
};

// URL syntax: data:[<mediatype>][;base64],<data>
static int GetMetadata(const wxString& location, wxString& mediatype, bool& isBase64)
{
    int dataPos = location.Find(',');
    if (dataPos > 0)
    {
        int hdrPos = location.Find(':');
        if ((hdrPos > 0))
        {
            wxString metadata(location, hdrPos + 1, dataPos - hdrPos - 1);

            int encPos = metadata.Find(';', true);

            if (encPos > 0)
            {
                auto encoding = metadata.Right(metadata.Len() - encPos - 1);
                if (encoding.IsSameAs("base64", false))
                {
                    isBase64 = true;
                }
                else
                {
                    encPos = metadata.Len();
                }
            }
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
    return GetProtocol(location).IsSameAs("data", false);
#endif
    return false;
}

wxFSFile* wxDataSchemeFSHandler::OpenFile(wxFileSystem& WXUNUSED(fs),
                                          const wxString& location)
{
#if !wxUSE_URL
    return nullptr;
#else
    wxString mediatype;
    bool isBase64 = false;
    int dataPos = GetMetadata(location, mediatype, isBase64);

    if (dataPos < 0)
        return nullptr;

    if (mediatype.IsEmpty())
        mediatype = "text/plain";

    wxInputStream* stream = nullptr;
    if (isBase64)
    {
#if wxUSE_BASE64
        stream = new wxBufferedMemoryInputStream(
            wxBase64Decode(location.Right(location.Len() - dataPos)));
#endif // wxUSE_BASE64
    }
    else
    {
        stream = new wxStringInputStream(
            wxURL::Unescape(location.Right(location.Len() - dataPos)));
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
