///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/renderer.h
// Purpose:     wxRenderer class declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

/*
   wxRenderer class is used to draw all wxWindows controls. This is an ABC and
   the look of the application is determined by the concrete derivation of
   wxRenderer used in the program.
 */

#ifdef __GNUG__
    #pragma interface "renderer.h"
#endif

#ifndef _WX_UNIV_RENDERER_H_
#define _WX_UNIV_RENDERER_H_

class WXDLLEXPORT wxControl;
class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxWindow;

#include "wx/string.h"
#include "wx/gdicmn.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control state flags used in wxRenderer
enum
{
    wxRENDER_ENABLED    = 0x00000001,
    wxRENDER_FOCUSED    = 0x00000002,
    wxRENDER_PRESSED    = 0x00000004,
    wxRENDER_DEFAULT    = 0x00000008,      // button...
    wxRENDER_HIGHLIGHT  = 0x00000010,

    wxRENDER_FLAGS_MASK = 0x0000001f
};

// ----------------------------------------------------------------------------
// wxRenderer: abstract renderers interface
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRenderer
{
public:
    // draw the controls background
    virtual void DrawBackground(wxDC& dc,
                                const wxColour& col,
                                const wxRect& rect,
                                int flags) = 0;

    // draw the label inside the given rectangle with the specified alignment
    // and optionally emphasize the character with the given index
    virtual void DrawLabel(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = wxRENDER_ENABLED,
                           int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                           int indexAccel = -1) = 0;

    // draw the border and optionally return the rectangle containing the
    // region inside the border
    virtual void DrawBorder(wxDC& dc,
                            wxBorder border,
                            const wxRect& rect,
                            int flags = wxRENDER_ENABLED,
                            wxRect *rectIn = (wxRect *)NULL) = 0;

    // draw push button border and return the rectangle left for the label
    virtual void DrawButtonBorder(wxDC& dc,
                                  const wxRect& rect,
                                  int flags = wxRENDER_ENABLED,
                                  wxRect *rectIn = (wxRect *)NULL) = 0;

    // draw a frame with the label (horizontal alignment can be specified)
    virtual void DrawFrame(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = wxRENDER_ENABLED,
                           int alignment = wxALIGN_LEFT,
                           int indexAccel = -1) = 0;

    // TODO: having this is ugly but I don't see how to solve GetBestSize()
    //       problem without something like this

    // adjust the size of the control of the given class: for most controls,
    // this just takes into account the border, but for some (buttons, for
    // example) it is more complicated - the result being, in any case, that
    // the control looks "nice" if it uses the adjusted rectangle
    virtual void AdjustSize(wxSize *size, const wxWindow *window) = 0;

    // virtual dtor for any base class
    virtual ~wxRenderer();
};

// ----------------------------------------------------------------------------
// wxDelegateRenderer: it is impossible to inherit from any of standard
// renderers as their declarations are in private code, but you can use this
// class to override only some of the Draw() functions - all the other ones
// will be left to the original renderer
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDelegateRenderer : public wxRenderer
{
public:
    wxDelegateRenderer(wxRenderer *renderer) : m_renderer(renderer) { }

    virtual void DrawBackground(wxDC& dc,
                                const wxColour& col,
                                const wxRect& rect,
                                int flags)
        { m_renderer->DrawBackground(dc, col, rect, flags); }
    virtual void DrawLabel(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = wxRENDER_ENABLED,
                           int align = wxALIGN_LEFT | wxALIGN_TOP,
                           int indexAccel = -1)
        { m_renderer->DrawLabel(dc, label, rect, flags, align, indexAccel); }
    virtual void DrawBorder(wxDC& dc,
                            wxBorder border,
                            const wxRect& rect,
                            int flags = wxRENDER_ENABLED,
                            wxRect *rectIn = (wxRect *)NULL)
        { m_renderer->DrawBorder(dc, border, rect, flags, rectIn); }
    virtual void DrawFrame(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = wxRENDER_ENABLED,
                           int align = wxALIGN_LEFT,
                           int indexAccel = -1)
        { m_renderer->DrawFrame(dc, label, rect, flags, align, indexAccel); }
    virtual void DrawButtonBorder(wxDC& dc,
                                  const wxRect& rect,
                                  int flags = wxRENDER_ENABLED,
                                  wxRect *rectIn = (wxRect *)NULL)
        { m_renderer->DrawButtonBorder(dc, rect, flags, rectIn); }

    virtual void AdjustSize(wxSize *size, const wxWindow *window)
        { m_renderer->AdjustSize(size, window); }

protected:
    wxRenderer *m_renderer;
};

// ----------------------------------------------------------------------------
// wxControlRenderer: wraps the wxRenderer functions in a form easy to use from
// OnPaint()
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxControlRenderer
{
public:
    // create a renderer for this dc with this "fundamental" renderer
    wxControlRenderer(wxControl *control, wxDC& dc, wxRenderer *renderer);

    // operations
    void DrawLabel();
    void DrawBorder();
    void DrawButtonBorder();
    void DrawFrame();

    // accessors
    wxRenderer *GetRenderer() const { return m_renderer; }

    wxDC& GetDC() { return m_dc; }

    const wxRect& GetRect() const { return m_rect; }
    wxRect& GetRect() { return m_rect; }

protected:
    // the current window state
    int GetStateFlags() const;

private:
    wxControl *m_ctrl;
    wxRenderer *m_renderer;
    wxDC& m_dc;
    wxRect m_rect;
};

#endif // _WX_UNIV_RENDERER_H_

