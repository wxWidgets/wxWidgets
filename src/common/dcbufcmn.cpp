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

// ============================================================================
// wxSharedDCBufferManager
//   Helper class to free shared buffer when the app exists.
// ============================================================================

class wxSharedDCBufferManager
{
    friend class wxBufferedDC;
public:

    wxSharedDCBufferManager()
    {
        m_buffer = NULL;
    }

    ~wxSharedDCBufferManager()
    {
        delete m_buffer;
    }

    wxBitmap* GetBuffer(wxWindow* win, const wxSize& area)
    {
        int width = area.x;
        int height = area.y;

        if ( width <= 0 )
            win->GetClientSize(&width, &height);

        if ( !m_buffer ||
             width > m_buffer->GetWidth() ||
             height > m_buffer->GetHeight() )
        {
            delete m_buffer;

            // Create slightly larger bitmap so we don't need to
            // be reallocating constantly when the user enlarges
            // the frame for the first time.
            m_buffer = new wxBitmap(width+20, height+20);
        }

        return m_buffer;
    }

private:
    wxBitmap*   m_buffer;
};

static wxSharedDCBufferManager gs_sharedDCBufferManager;

// ============================================================================
// wxBufferedDC
// ============================================================================

// Blits the buffer to the dc, and detaches the dc from the buffer (so it
// can be effectively used once only).
//
// Usually called in the dtor or by the dtor of derived classes if the
// BufferedDC must blit before the derived class (which may own the dc it's
// blitting to) is destroyed.
void wxBufferedDC::UnMask()
{
    if ( m_buffer )
    {
        wxASSERT_MSG( m_mainDc != NULL,
                      _T("No underlying DC associated with wxBufferedDC (anymore)") );

        wxDC* bufferDc = DetachDC();

        wxASSERT( bufferDc->IsKindOf(CLASSINFO(wxMemoryDC)) );

        wxCoord x=0, y=0;

        if (m_style & wxBUFFER_CLIENT_AREA)
            bufferDc->GetDeviceOrigin(& x, & y);

        m_mainDc->Blit( 0, 0,
                        m_buffer->GetWidth(), m_buffer->GetHeight(), bufferDc,
                        -x, -y );
        m_mainDc = NULL;
        m_buffer = NULL;
        delete bufferDc;
    }
}

void wxBufferedDC::PrepareBuffer(wxWindow* win, const wxSize& area)
{
    m_buffer = gs_sharedDCBufferManager.GetBuffer(win, area);
}

void wxBufferedDC::UseBuffer()
{
    wxASSERT(m_buffer);

    wxMemoryDC* memoryDc = new wxMemoryDC(m_mainDc);
    memoryDc->SelectObject(*m_buffer);

    AttachDC(memoryDc);
}


