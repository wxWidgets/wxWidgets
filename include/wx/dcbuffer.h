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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "dcbuffer.h"
#endif

#include "wx/dcmemory.h"
#include "wx/dcclient.h"

// flags for wxBufferedDC ctor/Init()
enum
{
    // this is more efficient and hence default
    wxBUFFER_DC_OVERWRITE_BG = 0,

    // preserve the old background: more time consuming
    wxBUFFER_DC_PRESERVE_BG = 1,


    // flags used by default
    wxBUFFER_DC_DEFAULT = wxBUFFER_DC_OVERWRITE_BG
};

// ----------------------------------------------------------------------------
// Double buffering helper.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxBufferedDC : public wxMemoryDC
{
public:
    // Default ctor, must subsequently call Init for two stage construction.
    wxBufferedDC() : m_dc( 0 )
    {
    }

    // Construct a wxBufferedDC using a user supplied buffer.
    wxBufferedDC(wxDC *dc, const wxBitmap &buffer);

    // Construct a wxBufferedDC with an internal buffer of 'area'
    // (where area is usually something like the size of the window
    // being buffered)
    wxBufferedDC(wxDC *dc, const wxSize &area, int flags = wxBUFFER_DC_DEFAULT);

    // default copy ctor ok.

    // The usually desired  action in the dtor is to blit the buffer.
    virtual ~wxBufferedDC() { if ( m_dc ) UnMask(); }

    // These reimplement the actions of the ctors for two stage creation, but
    // are not used by the ctors themselves to save a few cpu cycles.
    void Init(wxDC *dc, const wxBitmap &bitmap);
    void Init(wxDC *dc, const wxSize &area, int flags = wxBUFFER_DC_DEFAULT);

    // Blits the buffer to the dc, and detaches the dc from the buffer (so it
    // can be effectively used once only).
    //
    // Usually called in the dtor or by the dtor of derived classes if the
    // BufferedDC must blit before the derived class (which may own the dc it's
    // blitting to) is destroyed.
    void UnMask();

private:
    // check that the bitmap is valid and use it
    void UseBuffer()
    {
        wxASSERT_MSG( m_buffer.Ok(), _T("invalid bitmap in wxBufferedDC") );

        SelectObject(m_buffer);
    }

    // preserve the background if necessary
    void SaveBg(const wxSize& area, int flags)
    {
        if ( flags & wxBUFFER_DC_PRESERVE_BG )
        {
            Blit(0, 0, area.GetWidth(), area.GetHeight(), m_dc, 0, 0);
        }
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
class WXDLLIMPEXP_ADV wxBufferedPaintDC : public wxBufferedDC
{
public:
    // this ctor creates a bitmap of the size of the window for buffering
    wxBufferedPaintDC(wxWindow *window, int flags = wxBUFFER_DC_DEFAULT)
        : m_paintdc(window)
    {
        Init(&m_paintdc, window->GetClientSize(), flags);

        Prepare(window);
    }

    // the bitmap must be valid here
    wxBufferedPaintDC(wxWindow *window, const wxBitmap& buffer)
        : m_paintdc(window)
    {
        Init(&m_paintdc, buffer);

        Prepare(window);
    }

    // default copy ctor ok.

    virtual ~wxBufferedPaintDC()
    {
        // We must UnMask here, else by the time the base class
        // does it, the PaintDC will have already been destroyed.
        UnMask();
    }

private:
    // prepare the underlying DC
    void Prepare(wxWindow *window)
    {
        window->PrepareDC(m_paintdc);
    }

    wxPaintDC m_paintdc;

    DECLARE_NO_COPY_CLASS(wxBufferedPaintDC)
};

#endif  // _WX_DCBUFFER_H_

