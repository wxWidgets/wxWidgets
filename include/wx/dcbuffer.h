/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dcbuffer.h
// Purpose:     wxBufferedDC class
// Author:      Ron Lee <ron@debian.org>
// Modified by: Vadim Zeitlin (refactored, added bg preservation)
// Created:     16/03/02
// RCS-ID:      $Id$
// Copyright:   (c) Ron Lee
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCBUFFER_H_
#define _WX_DCBUFFER_H_

#include "wx/dcmemory.h"
#include "wx/dcclient.h"


// ----------------------------------------------------------------------------
// Double buffering helper.
// ----------------------------------------------------------------------------

class wxBufferedDC : public wxMemoryDC
{
public:
    // Default ctor, must subsequently call Init for two stage construction.
    wxBufferedDC() : m_dc( 0 )
    {
    }

    // Construct a wxBufferedDC using a user supplied buffer.
    wxBufferedDC(wxDC *dc, const wxBitmap &buffer)
        : m_dc( dc ),
          m_buffer( buffer )
    {
        UseBuffer();
    }

    // Construct a wxBufferedDC with an internal buffer of 'area'
    // (where area is usually something like the size of the window
    // being buffered)
    wxBufferedDC(wxDC *dc, const wxSize &area)
        : m_dc( dc ),
          m_buffer( area.GetWidth(), area.GetHeight() )
    {
        UseBuffer();
    }

    // default copy ctor ok.

    // The usually desired  action in the dtor is to blit the buffer.
    virtual ~wxBufferedDC()
    {
        if ( m_dc ) UnMask();
    }

    // These reimplement the actions of the ctors for two stage creation, but
    // are not used by the ctors themselves to save a few cpu cycles.
    void Init(wxDC *dc, const wxBitmap &buffer)
    {
        wxASSERT_MSG( m_dc == 0 && m_buffer == wxNullBitmap,
                      _T("wxBufferedDC already initialised") );
        m_dc = dc;
        m_buffer = buffer;
        UseBuffer();
    }

    void Init(wxDC *dc, const wxSize &area)
    {
        Init(dc, wxBitmap(area.GetWidth(), area.GetHeight()));
    }

    // Blits the buffer to the dc, and detaches the dc from the buffer (so it
    // can be effectively used once only).
    //
    // Usually called in the dtor or by the dtor of derived classes if the
    // BufferedDC must blit before the derived class (which may own the dc it's
    // blitting to) is destroyed.
    void UnMask()
    {
        wxASSERT_MSG( m_dc != 0,
                      _T("No underlying DC associated with wxBufferedDC (anymore)") );

        m_dc->Blit( 0, 0,
                    m_buffer.GetWidth(), m_buffer.GetHeight(), this,
                    0, 0 );
        m_dc = NULL;
    }

private:
    // check that the bitmap is valid and use it
    void UseBuffer()
    {
        wxASSERT_MSG( m_buffer.Ok(), _T("invalid bitmap in wxBufferedDC") );

        SelectObject(m_buffer);
    }

    // the underlying DC to which we copy everything drawn on this one in
    // UnMask()
    //
    // NB: Without the existence of a wxNullDC, this must be a pointer, else it
    //     could probably be a reference.
    wxDC *m_dc;

    // the buffer (selected in this DC)
    wxBitmap m_buffer;

    DECLARE_NO_COPY_CLASS(wxBufferedDC)
};


// ----------------------------------------------------------------------------
// Double buffered PaintDC.
// ----------------------------------------------------------------------------

// Creates a double buffered wxPaintDC, optionally allowing the
// user to specify their own buffer to use.
class wxBufferedPaintDC : public wxBufferedDC
{
public:
    // If no bitmap is supplied by the user, a temporary one wil; be created.
    wxBufferedPaintDC(wxWindow *window, const wxBitmap& buffer = wxNullBitmap)
        : m_paintdc(window)
    {
        window->PrepareDC( m_paintdc );

        if( buffer != wxNullBitmap )
            Init(&m_paintdc, buffer);
        else
            Init(&m_paintdc, window->GetClientSize());
    }

    // default copy ctor ok.

    virtual ~wxBufferedPaintDC()
    {
        // We must UnMask here, else by the time the base class
        // does it, the PaintDC will have already been destroyed.
        UnMask();
    }

private:
    wxPaintDC m_paintdc;

    DECLARE_NO_COPY_CLASS(wxBufferedPaintDC)
};

#endif  // _WX_DCBUFFER_H_

