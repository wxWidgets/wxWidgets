/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dcbuffer.h
// Purpose:     wxBufferedDC class
// Author:      Ron Lee <ron@debian.org>
// Modified by:
// Created:     16/03/02
// RCS-ID:      $Id$
// Copyright:   (c) Ron Lee
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCBUFFER_H_
#define _WX_DCBUFFER_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "dcbuffer.h"
#endif

#include "wx/dcmemory.h"
#include "wx/dcclient.h"


// ==============================================================
//   Double buffering helper.
// --------------------------------------------------------------

class WXDLLEXPORT wxBufferedDC : public wxMemoryDC
{
private:

        // Without the existence of a wxNullDC, this must be
        // a pointer, else it could probably be a reference.

    wxDC        *m_dc;
    wxBitmap    m_buffer;

public:

        // Default ctor, must subsequently call Init for
        // two stage construction.

    wxBufferedDC()
        : m_dc( 0 )
    {}

        // Construct a wxBufferedDC using a user supplied buffer.

    wxBufferedDC( wxDC *dc, const wxBitmap &buffer );

        // Construct a wxBufferedDC with an internal buffer of 'area'
        // (where area is usually something like the size of the window
        // being buffered)

    wxBufferedDC( wxDC *dc, const wxSize &area );

    // default copy ctor ok.

        // The usually desired  action in the dtor is to blit the buffer.

    ~wxBufferedDC();

        // These reimplement the actions of the ctors for
        // two stage creation, but are not used by the ctors
        // themselves to save a few cpu cycles.

    void Init( wxDC *dc, const wxBitmap &bitmap );
    void Init( wxDC *dc, const wxSize &area );

        // Blits the buffer to the dc, and detaches the dc from
        // the buffer.  Usually called in the dtor or by the dtor
        // of derived classes if the BufferedDC must blit before
        // the derived class (which may own the dc it's blitting
        // to) is destroyed.

    void UnMask();

    DECLARE_NO_COPY_CLASS(wxBufferedDC)
};


// ==============================================================
//   Double buffered PaintDC.
// --------------------------------------------------------------

// Creates a double buffered wxPaintDC, optionally allowing the
// user to specify their own buffer to use.

class WXDLLEXPORT wxBufferedPaintDC : public wxBufferedDC
{
private:

    wxPaintDC    m_paintdc;

public:

    wxBufferedPaintDC( wxWindow *window, const wxBitmap &buffer = wxNullBitmap );

    // default copy ctor ok.

    ~wxBufferedPaintDC();

    DECLARE_NO_COPY_CLASS(wxBufferedPaintDC)
};


#endif  // _WX_DCBUFFER_H_

