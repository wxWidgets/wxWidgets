/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dcbufcmn.cpp
// Purpose:     Buffered DC implementation
// Author:      Ron Lee, Jaakko Salli
// Modified by:
// Created:     Sep-20-2006
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

#ifndef WX_PRECOMP
#endif

#include "wx/dcbuffer.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxSharedDCBufferManager: helper class maintaining backing store bitmap
// ----------------------------------------------------------------------------

class wxSharedDCBufferManager
{
public:
    wxSharedDCBufferManager() { }
    ~wxSharedDCBufferManager() { }

    wxBitmap GetBuffer(int w, int h)
    {
        if ( !m_buffer.IsOk() ||
             w > m_buffer.GetWidth() ||
             h > m_buffer.GetHeight() )
        {
            // Create slightly larger bitmap so we don't need to
            // be reallocating constantly when the user enlarges
            // the frame for the first time.
            m_buffer = wxBitmap(w, h);
        }

        return m_buffer;
    }

private:
    wxBitmap m_buffer;
};

static wxSharedDCBufferManager gs_sharedDCBufferManager;


// ============================================================================
// wxBufferedDC
// ============================================================================

void wxBufferedDC::UseBuffer(wxCoord w, wxCoord h)
{
    if ( !m_buffer.IsOk() )
    {
        if ( w == -1 || h == -1 )
            m_dc->GetSize(&w, &h);

        m_buffer = gs_sharedDCBufferManager.GetBuffer(w, h);
    }

    SelectObject(m_buffer);
}

