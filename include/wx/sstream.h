///////////////////////////////////////////////////////////////////////////////
// Name:        wx/sstream.h
// Purpose:     string-based streams
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2004-09-19
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SSTREAM_H_
#define _WX_SSTREAM_H_

#include "wx/stream.h"

#if wxUSE_STREAMS

// ----------------------------------------------------------------------------
// wxStringInputStream is a stream reading from the given (fixed size) string
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStringInputStream : public wxInputStream
{
public:
    // ctor associates the stream with the given string which makes a copy of
    // it
    wxStringInputStream(const wxString& s)
        : m_str(s)
    {
        m_pos = 0;
    }

    virtual size_t GetSize() const { return m_str.length(); }

protected:
    virtual wxFileOffset OnSysSeek(wxFileOffset ofs, wxSeekMode mode);
    virtual wxFileOffset OnSysTell() const;
    virtual size_t OnSysRead(void *buffer, size_t size);

private:
    // the string we're reading from
    wxString m_str;

    // position in the stream in bytes, *not* in chars
    size_t m_pos;


    DECLARE_NO_COPY_CLASS(wxStringInputStream)
};

// ----------------------------------------------------------------------------
// wxStringOutputStream writes data to the given string, expanding it as needed
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStringOutputStream : public wxOutputStream
{
public:
    // The stream will write data either to the provided string or to an
    // internal string which can be retrieved using GetString()
    wxStringOutputStream(wxString *pString = NULL)
    {
        m_str = pString ? pString : &m_strInternal;
        m_pos = m_str->length() / sizeof(wxChar);
    }

    // get the string containing current output
    const wxString& GetString() const { return *m_str; }

protected:
    virtual wxFileOffset OnSysTell() const;
    virtual size_t OnSysWrite(const void *buffer, size_t size);

private:
    // internal string, not used if caller provided his own string
    wxString m_strInternal;

    // pointer given by the caller or just pointer to m_strInternal
    wxString *m_str;

    // position in the stream in bytes, *not* in chars
    size_t m_pos;


    DECLARE_NO_COPY_CLASS(wxStringOutputStream)
};

#endif // wxUSE_STREAMS

#endif // _WX_SSTREAM_H_

