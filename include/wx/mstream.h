/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mstream.h
// Purpose:     Memory stream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WXMMSTREAM_H__
#define _WX_WXMMSTREAM_H__

#include "wx/stream.h"

#if wxUSE_STREAMS

class WXDLLIMPEXP_BASE wxMemoryInputStream : public wxInputStream
{
public:
    wxMemoryInputStream(const void *data, size_t length);
    virtual ~wxMemoryInputStream();
    virtual wxFileOffset GetLength() const { return m_length; }
    virtual bool Eof() const;
    virtual bool IsSeekable() const { return true; }

    char Peek();

    wxStreamBuffer *GetInputStreamBuffer() const { return m_i_streambuf; }

    // deprecated, compatibility only
    wxStreamBuffer *InputStreamBuffer() const { return m_i_streambuf; }

protected:
    wxStreamBuffer *m_i_streambuf;

    size_t OnSysRead(void *buffer, size_t nbytes);
    wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode);
    wxFileOffset OnSysTell() const;

private:
    size_t m_length;

    DECLARE_NO_COPY_CLASS(wxMemoryInputStream)
};

class WXDLLIMPEXP_BASE wxMemoryOutputStream : public wxOutputStream
{
public:
    // if data is !NULL it must be allocated with malloc()
    wxMemoryOutputStream(void *data = NULL, size_t length = 0);
    virtual ~wxMemoryOutputStream();
    virtual wxFileOffset GetLength() const { return m_o_streambuf->GetLastAccess(); }
    virtual bool IsSeekable() const { return true; }

    size_t CopyTo(void *buffer, size_t len) const;

    wxStreamBuffer *GetOutputStreamBuffer() const { return m_o_streambuf; }

    // deprecated, compatibility only
    wxStreamBuffer *OutputStreamBuffer() const { return m_o_streambuf; }

protected:
    wxStreamBuffer *m_o_streambuf;

protected:
    size_t OnSysWrite(const void *buffer, size_t nbytes);
    wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode);
    wxFileOffset OnSysTell() const;

    DECLARE_NO_COPY_CLASS(wxMemoryOutputStream)
};

#endif
  // wxUSE_STREAMS

#endif
  // _WX_WXMMSTREAM_H__

