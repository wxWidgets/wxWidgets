///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/sstream.cpp
// Purpose:     string-based streams implementation
// Author:      Vadim Zeitlin
// Modified by: Ryan Norton (UTF8 UNICODE)
// Created:     2004-09-19
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_STREAMS

#include "wx/sstream.h"

// ============================================================================
// wxStringInputStream implementation
// ============================================================================

// ----------------------------------------------------------------------------
// construction/destruction
// ----------------------------------------------------------------------------

wxStringInputStream::wxStringInputStream(const wxString& s)
    : m_str(s), m_buf(s.utf8_str())
{
    wxASSERT_MSG(m_buf.data() != nullptr, wxT("Could not convert string to UTF8!"));
    m_pos = 0;
}

// ----------------------------------------------------------------------------
// getlength
// ----------------------------------------------------------------------------

wxFileOffset wxStringInputStream::GetLength() const
{
    return GetBufferSize();
}

// ----------------------------------------------------------------------------
// seek/tell
// ----------------------------------------------------------------------------

wxFileOffset wxStringInputStream::OnSysSeek(wxFileOffset ofs, wxSeekMode mode)
{
    switch ( mode )
    {
        case wxFromStart:
            // nothing to do, ofs already ok
            break;

        case wxFromEnd:
            ofs += GetBufferSize();
            break;

        case wxFromCurrent:
            ofs += m_pos;
            break;

        default:
            wxFAIL_MSG( wxT("invalid seek mode") );
            return wxInvalidOffset;
    }

    if ( ofs < 0 || ofs > static_cast<wxFileOffset>(GetBufferSize()) )
        return wxInvalidOffset;

    // FIXME: this can't be right
    m_pos = wx_truncate_cast(size_t, ofs);

    return ofs;
}

wxFileOffset wxStringInputStream::OnSysTell() const
{
    return static_cast<wxFileOffset>(m_pos);
}

// ----------------------------------------------------------------------------
// actual IO
// ----------------------------------------------------------------------------

size_t wxStringInputStream::OnSysRead(void *buffer, size_t size)
{
    const size_t sizeMax = GetBufferSize() - m_pos;

    if ( size >= sizeMax )
    {
        if ( sizeMax == 0 )
        {
            m_lasterror = wxSTREAM_EOF;
            return 0;
        }

        size = sizeMax;
    }

    memcpy(buffer, m_buf.data() + m_pos, size);
    m_pos += size;

    return size;
}

// ============================================================================
// wxStringOutputStream implementation
// ============================================================================

wxStringOutputStream::wxStringOutputStream(wxString *pString, wxMBConv& conv)
    : m_conv(conv)
    , m_unconv(0)
{
    m_str = pString ? pString : &m_strInternal;

    // We can avoid doing the conversion in the common case of using UTF-8
    // conversion in UTF-8 build, as it is exactly the same as the string
    // length anyhow in this case.
#if wxUSE_UNICODE_UTF8
    if ( conv.IsUTF8() )
        m_pos = m_str->utf8_length();
    else
#endif // wxUSE_UNICODE_UTF8
        m_pos = m_conv.FromWChar(nullptr, 0, m_str->wc_str(), m_str->length());
}

// ----------------------------------------------------------------------------
// seek/tell
// ----------------------------------------------------------------------------

wxFileOffset wxStringOutputStream::OnSysTell() const
{
    return static_cast<wxFileOffset>(m_pos);
}

// ----------------------------------------------------------------------------
// actual IO
// ----------------------------------------------------------------------------

size_t wxStringOutputStream::OnSysWrite(const void *buffer, size_t size)
{
    const char *p = static_cast<const char *>(buffer);

    // the part of the string we have here may be incomplete, i.e. it can stop
    // in the middle of an UTF-8 character and so converting it would fail; if
    // this is the case, accumulate the part which we failed to convert until
    // we get the rest (and also take into account the part which we might have
    // left unconverted before)
    const char *src;
    size_t srcLen;
    if ( m_unconv.GetDataLen() )
    {
        // append the new data to the data remaining since the last time
        m_unconv.AppendData(p, size);
        src = m_unconv;
        srcLen = m_unconv.GetDataLen();
    }
    else // no unconverted data left, avoid extra copy
    {
        src = p;
        srcLen = size;
    }

    size_t wlen;
    wxWCharBuffer wbuf(m_conv.cMB2WC(src, srcLen, &wlen));
    if ( wbuf )
    {
        // conversion succeeded, clear the unconverted buffer
        m_unconv = wxMemoryBuffer(0);

        m_str->append(wbuf, wlen);
    }
    else // conversion failed
    {
        // remember unconverted data if there had been none before (otherwise
        // we've already got it in the buffer)
        if ( src == p )
            m_unconv.AppendData(src, srcLen);

        // pretend that we wrote the data anyhow, otherwise the caller would
        // believe there was an error and this might not be the case, but do
        // not update m_pos as m_str hasn't changed
        return size;
    }

    // update position
    m_pos += size;

    // return number of bytes actually written
    return size;
}

#endif // wxUSE_STREAMS

