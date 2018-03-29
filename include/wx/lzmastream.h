///////////////////////////////////////////////////////////////////////////////
// Name:        wx/lzmastream.h
// Purpose:     Filters streams using LZMA(2) compression
// Author:      Vadim Zeitlin
// Created:     2018-03-29
// Copyright:   (c) 2018 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LZMASTREAM_H_
#define _WX_LZMASTREAM_H_

#include "wx/defs.h"

#if wxUSE_LIBLZMA && wxUSE_STREAMS

#include "wx/stream.h"
#include "wx/versioninfo.h"

namespace wxPrivate
{

// Private wrapper for lzma_stream struct.
struct wxLZMAStream;

// Common part of input and output LZMA streams: this is just an implementation
// detail and is not part of the public API.
class WXDLLIMPEXP_BASE wxLZMAData
{
protected:
    wxLZMAData();
    ~wxLZMAData();

    wxLZMAStream* m_stream;
    wxUint8* m_streamBuf;
    wxFileOffset m_pos;

    wxDECLARE_NO_COPY_CLASS(wxLZMAData);
};

} // namespace wxPrivate

// ----------------------------------------------------------------------------
// Filter for decompressing data compressed using LZMA
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxLZMAInputStream : public wxFilterInputStream,
                                           private wxPrivate::wxLZMAData
{
public:
    explicit wxLZMAInputStream(wxInputStream& stream)
        : wxFilterInputStream(stream)
    {
        Init();
    }

    explicit wxLZMAInputStream(wxInputStream* stream)
        : wxFilterInputStream(stream)
    {
        Init();
    }

    char Peek() wxOVERRIDE { return wxInputStream::Peek(); }
    wxFileOffset GetLength() const wxOVERRIDE { return wxInputStream::GetLength(); }

protected:
    size_t OnSysRead(void *buffer, size_t size) wxOVERRIDE;
    wxFileOffset OnSysTell() const wxOVERRIDE { return m_pos; }

private:
    void Init();
};

WXDLLIMPEXP_BASE wxVersionInfo wxGetLibLZMAVersionInfo();

#endif // wxUSE_LIBLZMA && wxUSE_STREAMS

#endif // _WX_LZMASTREAM_H_
