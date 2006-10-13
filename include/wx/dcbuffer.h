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
#include "wx/window.h"

// Split platforms into two groups - those which have well-working
// double-buffering by default, and those which do not.
#if defined(__WXMAC__) || defined(__WXGTK20__)
    #define wxALWAYS_NATIVE_DOUBLE_BUFFER       1
#else
    #define wxALWAYS_NATIVE_DOUBLE_BUFFER       0
#endif


// ----------------------------------------------------------------------------
// Double buffering helper.
// ----------------------------------------------------------------------------

// Assumes the buffer bitmap covers the entire scrolled window,
// and prepares the window DC accordingly
#define wxBUFFER_VIRTUAL_AREA       0x01

// Assumes the buffer bitmap only covers the client area;
// does not prepare the window DC
#define wxBUFFER_CLIENT_AREA        0x02

class wxBufferedDC : public wxMemoryDC
{
public:
    // Default ctor, must subsequently call Init for two stage construction.
    wxBufferedDC() : m_dc( 0 ), m_style(0)
    {
    }

    // Construct a wxBufferedDC using a user supplied buffer.
    wxBufferedDC(wxDC *dc,
                 const wxBitmap &buffer = wxNullBitmap,
                 int style = wxBUFFER_CLIENT_AREA)
        : m_dc( dc ),
          m_buffer( buffer ),
          m_style(style)
    {
        UseBuffer();
    }

    // Construct a wxBufferedDC with an internal buffer of 'area'
    // (where area is usually something like the size of the window
    // being buffered)
    wxBufferedDC(wxDC *dc, const wxSize &area, int style = wxBUFFER_CLIENT_AREA)
        : m_dc( dc ),
          m_buffer( area.GetWidth(), area.GetHeight() ),
          m_style(style)

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
    void Init(wxDC *dc,
              const wxBitmap &buffer=wxNullBitmap,
              int style = wxBUFFER_CLIENT_AREA)
    {
        wxASSERT_MSG( m_dc == 0 && m_buffer == wxNullBitmap,
                      _T("wxBufferedDC already initialised") );
        m_dc = dc;
        m_buffer = buffer;
        m_style = style;
        UseBuffer();
    }

    void Init(wxDC *dc, const wxSize &area, int style = wxBUFFER_CLIENT_AREA)
    {
        Init(dc, wxBitmap(area.GetWidth(), area.GetHeight()), style);
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

        wxCoord x=0, y=0;

        if (m_style & wxBUFFER_CLIENT_AREA)
            GetDeviceOrigin(&x, &y);

        m_dc->Blit( 0, 0,
                    m_buffer.GetWidth(), m_buffer.GetHeight(), this,
                    -x, -y );
        m_dc = NULL;
    }

    // Set and get the style
    void SetStyle(int style) { m_style = style; }
    int GetStyle() const { return m_style; }

private:
    // check that the bitmap is valid and use it
    void UseBuffer()
    {
        if (!m_buffer.Ok())
        {
            wxCoord w, h;
            m_dc->GetSize(&w, &h);
            m_buffer = wxBitmap(w, h);
        }

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

    // the buffering style
    int m_style;

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
    // If no bitmap is supplied by the user, a temporary one will be created.
    wxBufferedPaintDC(wxWindow *window, const wxBitmap& buffer, int style = wxBUFFER_CLIENT_AREA)
        : m_paintdc(window)
    {
        // If we're buffering the virtual window, scale the paint DC as well
        if (style & wxBUFFER_VIRTUAL_AREA)
            window->PrepareDC( m_paintdc );

        if( buffer != wxNullBitmap )
            Init(&m_paintdc, buffer, style);
        else
            Init(&m_paintdc, window->GetClientSize(), style);
    }

    // If no bitmap is supplied by the user, a temporary one will be created.
    wxBufferedPaintDC(wxWindow *window, int style = wxBUFFER_CLIENT_AREA)
        : m_paintdc(window)
    {
        // If we're using the virtual window, scale the paint DC as well
        if (style & wxBUFFER_VIRTUAL_AREA)
            window->PrepareDC( m_paintdc );

        Init(&m_paintdc, window->GetClientSize(), style);
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



//
// wxAutoBufferedPaintDC is a wxPaintDC in toolkits which have double-
// buffering by default. Otherwise it is a wxBufferedPaintDC. Thus,
// you can only expect it work with a simple constructor that
// accepts single wxWindow* argument.
//
#if wxALWAYS_NATIVE_DOUBLE_BUFFER
    #define wxAutoBufferedPaintDCBase           wxPaintDC
#else
    #define wxAutoBufferedPaintDCBase           wxBufferedPaintDC
#endif


#ifdef __WXDEBUG__

class wxAutoBufferedPaintDC : public wxAutoBufferedPaintDCBase
{
public:

    wxAutoBufferedPaintDC(wxWindow* win)
        : wxAutoBufferedPaintDCBase(win)
    {
        TestWinStyle(win);
    }

    virtual ~wxAutoBufferedPaintDC() { }

private:

    void TestWinStyle(wxWindow* win)
    {
        // Help the user to get the double-buffering working properly.
        wxASSERT_MSG( win->GetBackgroundStyle() == wxBG_STYLE_CUSTOM,
                      wxT("In constructor, you need to call GetBackgroundStyle(wxBG_STYLE_CUSTOM), ")
                      wxT("and also, if needed, paint the background manually in the paint event handler."));
    }

    DECLARE_NO_COPY_CLASS(wxAutoBufferedPaintDC)
};

#else // !__WXDEBUG__

// In release builds, just use typedef
typedef wxAutoBufferedPaintDCBase wxAutoBufferedPaintDC;

#endif


// Check if the window is natively double buffered and will return a wxPaintDC
// if it is, a wxBufferedPaintDC otherwise.  It is the caller's responsibility
// to delete the wxDC pointer when finished with it.
inline wxDC* wxAutoBufferedPaintDCFactory(wxWindow* window)
{
    if ( window->IsDoubleBuffered() )
        return new wxPaintDC(window);
    else
        return new wxBufferedPaintDC(window);
}
            

#endif  // _WX_DCBUFFER_H_
