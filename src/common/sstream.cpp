///////////////////////////////////////////////////////////////////////////////
// Name:        common/sstream.cpp
// Purpose:     string-based streams implementation
// Author:      Vadim Zeitlin
// Modified by:
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
// seek/tell
// ----------------------------------------------------------------------------

wxFileOffset wxStringInputStream::OnSysSeek(wxFileOffset ofs, wxSeekMode mode)
{
    const size_t ofsMax = m_str.length()*sizeof(wxChar);

    switch ( mode )
    {
        case wxFromStart:
            // nothing to do, ofs already ok
            break;

        case wxFromEnd:
            ofs += ofsMax;
            break;

        case wxFromCurrent:
            ofs += m_pos;
            break;

        default:
            wxFAIL_MSG( _T("invalid seek mode") );
            return wxInvalidOffset;
    }

    if ( ofs < 0 || wx_static_cast(size_t, ofs) >= ofsMax )
        return wxInvalidOffset;

    m_pos = wx_static_cast(size_t, ofs);

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
    const size_t sizeMax = m_str.length()*sizeof(wxChar) - m_pos;

    if ( size >= sizeMax )
    {
        if ( sizeMax == 0 )
        {
            m_lasterror = wxSTREAM_EOF;
            return 0;
        }

        size = sizeMax;
    }

    memcpy(buffer, m_str.data() + m_pos, size);
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
    // in Unicode mode we might not be able to write the last byte
    size_t len = size / sizeof(wxChar);

    const wxChar *p = wx_static_cast(const wxChar *, buffer);

    m_str->Append(wxString(p, p + len));

    // return number of bytes actually written
    len *= sizeof(wxChar);
    m_pos += len;

    return len;
}

#endif // wxUSE_STREAMS

