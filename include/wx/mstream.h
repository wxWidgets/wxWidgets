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

class WXDLLEXPORT wxMemoryInputStream : public wxInputStream
{
public:
    wxMemoryInputStream(const void *data, size_t length);
    virtual ~wxMemoryInputStream();
    virtual size_t GetSize() const { return m_length; }
    virtual bool Eof() const;

    char Peek();

    wxStreamBuffer *GetInputStreamBuffer() const { return m_i_streambuf; }

    // deprecated, compatibility only
    wxStreamBuffer *InputStreamBuffer() const { return m_i_streambuf; }

protected:
    wxStreamBuffer *m_i_streambuf;

    size_t OnSysRead(void *buffer, size_t nbytes);
    off_t OnSysSeek(off_t pos, wxSeekMode mode);
    off_t OnSysTell() const;

private:
    size_t m_length;
};

class WXDLLEXPORT wxMemoryOutputStream : public wxOutputStream
{
public:
    // if data is !NULL it must be allocated with malloc()
    wxMemoryOutputStream(void *data = NULL, size_t length = 0);
    virtual ~wxMemoryOutputStream();
    virtual size_t GetSize() const { return m_o_streambuf->GetLastAccess(); }

    size_t CopyTo(void *buffer, size_t len) const;

    wxStreamBuffer *GetOutputStreamBuffer() const { return m_o_streambuf; }

    // deprecated, compatibility only
    wxStreamBuffer *OutputStreamBuffer() const { return m_o_streambuf; }

protected:
    wxStreamBuffer *m_o_streambuf;

protected:
    size_t OnSysWrite(const void *buffer, size_t nbytes);
    off_t OnSysSeek(off_t pos, wxSeekMode mode);
    off_t OnSysTell() const;
};

#endif
  // wxUSE_STREAMS

#endif
  // _WX_WXMMSTREAM_H__

