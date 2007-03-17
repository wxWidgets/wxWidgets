///////////////////////////////////////////////////////////////////////////////
// Name:        common/sstream.cpp
// Purpose:     string-based streams implementation
// Author:      Vadim Zeitlin
// Modified by: Ryan Norton (UTF8 UNICODE)
// Created:     2004-09-19
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwindows.org>
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STREAMS

#include "wx/sstream.h"

// ============================================================================
// wxStringInputStream implementation
// ============================================================================

// ----------------------------------------------------------------------------
// construction/destruction
// ----------------------------------------------------------------------------

// TODO:  Do we want to include the null char in the stream?  If so then
// just add +1 to m_len in the ctor 
wxStringInputStream::wxStringInputStream(const wxString& s) 
#if wxUSE_UNICODE
    : m_str(s), m_buf(wxMBConvUTF8().cWX2MB(s).release()), m_len(strlen(m_buf))
#else
    : m_str(s), m_buf((char*)(const char*)s.c_str()), m_len(s.length())
#endif
{
#if wxUSE_UNICODE
    wxASSERT_MSG(m_buf != NULL, _T("Could not convert string to UTF8!"));
#endif
    m_pos = 0;
}

wxStringInputStream::~wxStringInputStream()
{
#if wxUSE_UNICODE
    // Note: wx[W]CharBuffer uses malloc()/free()
    free(m_buf);
#endif
}

// ----------------------------------------------------------------------------
// getlength
// ----------------------------------------------------------------------------

wxFileOffset wxStringInputStream::GetLength() const 
{ 
    return m_len; 
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
            ofs += m_len;
            break;

        case wxFromCurrent:
            ofs += m_pos;
            break;

        default:
            wxFAIL_MSG( _T("invalid seek mode") );
            return wxInvalidOffset;
    }

    if ( ofs < 0 || ofs > wx_static_cast(wxFileOffset, m_len) )
        return wxInvalidOffset;

    // FIXME: this can't be right
    m_pos = wx_truncate_cast(size_t, ofs);

    return ofs;
}

wxFileOffset wxStringInputStream::OnSysTell() const
{
    return wx_static_cast(wxFileOffset, m_pos);
}

// ----------------------------------------------------------------------------
// actual IO
// ----------------------------------------------------------------------------

size_t wxStringInputStream::OnSysRead(void *buffer, size_t size)
{
    const size_t sizeMax = m_len - m_pos;

    if ( size >= sizeMax )
    {
        if ( sizeMax == 0 )
        {
            m_lasterror = wxSTREAM_EOF;
            return 0;
        }

        size = sizeMax;
    }

    memcpy(buffer, m_buf + m_pos, size);
    m_pos += size;

    return size;
}

// ============================================================================
// wxStringOutputStream implementation
// ============================================================================

// ----------------------------------------------------------------------------
// seek/tell
// ----------------------------------------------------------------------------

wxFileOffset wxStringOutputStream::OnSysTell() const
{
    return wx_static_cast(wxFileOffset, m_pos);
}

// ----------------------------------------------------------------------------
// actual IO
// ----------------------------------------------------------------------------

size_t wxStringOutputStream::OnSysWrite(const void *buffer, size_t size)
{
    const char *p = wx_static_cast(const char *, buffer);

    // append the input buffer (may not be null terminated - thus 
    // the literal length
    m_str->Append(wxString(p, m_conv, size));

    // update position
    m_pos += size;

    // return number of bytes actually written
    return size;
}

#endif // wxUSE_STREAMS

