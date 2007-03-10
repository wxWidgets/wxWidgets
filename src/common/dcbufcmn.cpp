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

#include "wx/dcbuffer.h"

#ifndef WX_PRECOMP
    #include "wx/module.h"
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxSharedDCBufferManager: helper class maintaining backing store bitmap
// ----------------------------------------------------------------------------

class wxSharedDCBufferManager : public wxModule
{
public:
    wxSharedDCBufferManager() { }

    virtual bool OnInit() { return true; }
    virtual void OnExit() { wxDELETE(ms_buffer); }

    static wxBitmap* GetBuffer(int w, int h)
    {
        if ( ms_usingSharedBuffer )
            return new wxBitmap(w, h);

        if ( !ms_buffer ||
                w > ms_buffer->GetWidth() ||
                    h > ms_buffer->GetHeight() )
        {
            delete ms_buffer;
            ms_buffer = new wxBitmap(w, h);
        }

        ms_usingSharedBuffer = true;
        return ms_buffer;
    }

    static void ReleaseBuffer(wxBitmap* buffer)
    {
        if ( buffer == ms_buffer )
        {
            wxASSERT_MSG( ms_usingSharedBuffer, wxT("shared buffer already released") );
            ms_usingSharedBuffer = false;
        }
        else
        {
            delete buffer;
        }
    }

private:
    static wxBitmap *ms_buffer;
    static bool ms_usingSharedBuffer;

    DECLARE_DYNAMIC_CLASS(wxSharedDCBufferManager)
};

wxBitmap* wxSharedDCBufferManager::ms_buffer = NULL;
bool wxSharedDCBufferManager::ms_usingSharedBuffer = false;

IMPLEMENT_DYNAMIC_CLASS(wxSharedDCBufferManager, wxModule)

// ============================================================================
// wxBufferedDC
// ============================================================================

void wxBufferedDC::UseBuffer(wxCoord w, wxCoord h)
{
    if ( !m_buffer || !m_buffer->IsOk() )
    {
        if ( w == -1 || h == -1 )
            m_dc->GetSize(&w, &h);

        m_buffer = wxSharedDCBufferManager::GetBuffer(w, h);
        m_style |= wxBUFFER_USES_SHARED_BUFFER;
    }

    SelectObject(*m_buffer);
}

void wxBufferedDC::UnMask()
{
    wxCHECK_RET( m_dc, _T("no underlying wxDC?") );
    wxASSERT_MSG( m_buffer && m_buffer->IsOk(), _T("invalid backing store") );

    wxCoord x = 0,
            y = 0;

    if ( m_style & wxBUFFER_CLIENT_AREA )
        GetDeviceOrigin(&x, &y);

    m_dc->Blit(0, 0, m_buffer->GetWidth(), m_buffer->GetHeight(),
               this, -x, -y );
    m_dc = NULL;

    if ( m_style & wxBUFFER_USES_SHARED_BUFFER )
        wxSharedDCBufferManager::ReleaseBuffer(m_buffer);
}
