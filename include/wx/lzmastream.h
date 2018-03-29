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

struct wxLZMAStream;

// ----------------------------------------------------------------------------
// Filter for decompressing data compressed using LZMA
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxLZMAInputStream : public wxFilterInputStream
{
public:
    explicit wxLZMAInputStream(wxInputStream& stream);
    explicit wxLZMAInputStream(wxInputStream* stream);
    virtual ~wxLZMAInputStream();

    char Peek() wxOVERRIDE { return wxInputStream::Peek(); }
    wxFileOffset GetLength() const wxOVERRIDE { return wxInputStream::GetLength(); }

protected:
    size_t OnSysRead(void *buffer, size_t size) wxOVERRIDE;
    wxFileOffset OnSysTell() const wxOVERRIDE { return m_pos; }

private:
    void Init();

    wxLZMAStream* m_stream;
    wxUint8* m_inbuf;
    wxFileOffset m_pos;

    wxDECLARE_NO_COPY_CLASS(wxLZMAInputStream);
};

WXDLLIMPEXP_BASE wxVersionInfo wxGetLibLZMAVersionInfo();

#endif // wxUSE_LIBLZMA && wxUSE_STREAMS

#endif // _WX_LZMASTREAM_H_
