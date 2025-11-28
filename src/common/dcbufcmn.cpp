/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dcbufcmn.cpp
// Purpose:     Buffered DC implementation
// Author:      Ron Lee, Jaakko Salli
// Created:     Sep-20-2006
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


#include "wx/dcbuffer.h"

#ifndef WX_PRECOMP
    #include "wx/module.h"
#endif

// ============================================================================
// implementation
// ============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxBufferedDC, wxMemoryDC);
wxIMPLEMENT_ABSTRACT_CLASS(wxBufferedPaintDC, wxBufferedDC);

// ----------------------------------------------------------------------------
// wxSharedDCBufferManager: helper class maintaining backing store bitmap
// ----------------------------------------------------------------------------

class wxSharedDCBufferManager : public wxModule
{
public:
    wxSharedDCBufferManager() { }

    virtual bool OnInit() override { return true; }
    virtual void OnExit() override { wxDELETE(ms_buffer); }

    static wxBitmap* GetBuffer(wxDC* dc, wxSize size)
    {
        if ( ms_usingSharedBuffer )
            return DoCreateBuffer(dc, size);

        if ( !ms_buffer ||
                !ms_buffer->GetLogicalSize().IsAtLeast(size) ||
                (dc && dc->GetContentScaleFactor() != ms_buffer->GetScaleFactor()) )
        {
            delete ms_buffer;

            ms_buffer = DoCreateBuffer(dc, size);
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
    static wxBitmap* DoCreateBuffer(wxDC* dc, wxSize size)
    {
        const double scale = dc ? dc->GetContentScaleFactor() : 1.0;
        wxBitmap* const buffer = new wxBitmap;

        // we must always return a valid bitmap but creating a bitmap of
        // size 0 would fail, so create a 1*1 bitmap in this case
        size.IncTo(wxSize(1, 1));

        // Explicitly request 24bpp bitmap under MSW to avoid slow down when
        // drawing bitmaps with alpha channel on 32bpp bitmap: as explained in
        // AlphaBlt() implementation in src/msw/dc.cpp, we need to manually
        // reset alpha values in this case, which involves converting the
        // bitmap to DIB and back and may be very slow. As we don't really care
        // about the alpha values in the backing store bitmap (everything is
        // already blended when drawing to it), use 24bpp bitmap to avoid this.
        constexpr int depth =
#if defined(__WXMSW__)
            24;
#else
            wxBITMAP_SCREEN_DEPTH;
#endif

        buffer->CreateWithLogicalSize(size, scale, depth);

        return buffer;
    }

    static wxBitmap *ms_buffer;
    static bool ms_usingSharedBuffer;

    wxDECLARE_DYNAMIC_CLASS(wxSharedDCBufferManager);
};

wxBitmap* wxSharedDCBufferManager::ms_buffer = nullptr;
bool wxSharedDCBufferManager::ms_usingSharedBuffer = false;

wxIMPLEMENT_DYNAMIC_CLASS(wxSharedDCBufferManager, wxModule);

// ============================================================================
// wxBufferedDC
// ============================================================================

void wxBufferedDC::UseBuffer(wxSize size)
{
    wxCHECK_RET( size.x >= -1 && size.y >= -1, "Invalid buffer size" );

    if ( !m_buffer || !m_buffer->IsOk() )
    {
        if ( !size.IsFullySpecified() )
            size = m_dc->GetSize();

        m_buffer = wxSharedDCBufferManager::GetBuffer(m_dc, size);
        m_style |= wxBUFFER_USES_SHARED_BUFFER;
        m_area = size;
    }
    else
        m_area = m_buffer->GetSize();

    SelectObject(*m_buffer);

    // now that the DC is valid we can inherit the attributes (fonts, colours,
    // layout direction, ...) from the original DC
    if ( m_dc && m_dc->IsOk() )
        CopyAttributes(*m_dc);
}

void wxBufferedDC::UnMask()
{
    wxCHECK_RET( m_dc, wxT("no underlying wxDC?") );
    wxASSERT_MSG( m_buffer && m_buffer->IsOk(), wxT("invalid backing store") );

    wxCoord x = 0,
            y = 0;

    // Ensure the scale matches the device
    SetUserScale(1.0, 1.0);

    if ( m_style & wxBUFFER_CLIENT_AREA )
        GetDeviceOrigin(&x, &y);

    // It's possible that the buffer may be bigger than the area that needs to
    // be drawn (the client size of the window is smaller than the bitmap, or
    // a shared bitmap has been reused for a smaller area, etc.) so avoid
    // blitting too much if possible, but only use the real DC size if the
    // wxBUFFER_VIRTUAL_AREA style is not set.
    int width = m_area.GetWidth(),
        height = m_area.GetHeight();

    if (!(m_style & wxBUFFER_VIRTUAL_AREA))
    {
        int widthDC,
            heightDC;
        m_dc->GetSize(&widthDC, &heightDC);
        width = wxMin(width, widthDC);
        height = wxMin(height, heightDC);
    }

    const wxPoint origin = GetLogicalOrigin();
    m_dc->Blit(-origin.x, -origin.y, width, height, this, -x, -y);
    m_dc = nullptr;

    if ( m_style & wxBUFFER_USES_SHARED_BUFFER )
        wxSharedDCBufferManager::ReleaseBuffer(m_buffer);
}
