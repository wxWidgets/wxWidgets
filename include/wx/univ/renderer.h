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

   It also contains a few static methods which may be used by the concrete
   renderers and provide the functionality which is often similar or identical
   in all renderers (using inheritance here would be more restrictive as the
   given concrete renderer may need an arbitrary subset of the base class
   methods)
 */

#ifdef __GNUG__
    #pragma interface "renderer.h"
#endif

#ifndef _WX_UNIV_RENDERER_H_
#define _WX_UNIV_RENDERER_H_

class WXDLLEXPORT wxControl;
class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxScrollBar;
class WXDLLEXPORT wxWindow;

#include "wx/string.h"
#include "wx/gdicmn.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// hit test results
enum wxHitTest
{
    wxHT_NOWHERE,

    // scrollbar
    wxHT_SCROLLBAR_FIRST = wxHT_NOWHERE,
    wxHT_SCROLLBAR_ARROW_LINE_1,    // left or upper arrow to scroll by line
    wxHT_SCROLLBAR_ARROW_LINE_2,    // right or down
    wxHT_SCROLLBAR_ARROW_PAGE_1,    // left or upper arrow to scroll by page
    wxHT_SCROLLBAR_ARROW_PAGE_2,    // right or down
    wxHT_SCROLLBAR_THUMB,           // on the thumb
    wxHT_SCROLLBAR_BAR_1,           // bar to the left/above the thumb
    wxHT_SCROLLBAR_BAR_2,           // bar to the right/below the thumb
    wxHT_SCROLLBAR_LAST,

    wxHT_MAX
};

// ----------------------------------------------------------------------------
// wxRenderer: abstract renderers interface
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRenderer
{
public:
    // drawing functions
    // -----------------

    // draw the controls background
    virtual void DrawBackground(wxDC& dc,
                                const wxRect& rect,
                                int flags) = 0;

    // draw the label inside the given rectangle with the specified alignment
    // and optionally emphasize the character with the given index
    virtual void DrawLabel(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = 0,
                           int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                           int indexAccel = -1) = 0;

    // draw the border and optionally return the rectangle containing the
    // region inside the border
    virtual void DrawBorder(wxDC& dc,
                            wxBorder border,
                            const wxRect& rect,
                            int flags = 0,
                            wxRect *rectIn = (wxRect *)NULL) = 0;

    // draw push button border and return the rectangle left for the label
    virtual void DrawButtonBorder(wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0,
                                  wxRect *rectIn = (wxRect *)NULL) = 0;

    // draw a horizontal line
    virtual void DrawHorizontalLine(wxDC& dc,
                                    wxCoord y, wxCoord x1, wxCoord x2) = 0;

    // draw a vertical line
    virtual void DrawVerticalLine(wxDC& dc,
                                  wxCoord x, wxCoord y1, wxCoord y2) = 0;

    // draw a frame with the label (horizontal alignment can be specified)
    virtual void DrawFrame(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = 0,
                           int alignment = wxALIGN_LEFT,
                           int indexAccel = -1) = 0;

    // draw an arrow in the given direction
    virtual void DrawArrow(wxDC& dc,
                           wxDirection dir,
                           const wxRect& rect,
                           int flags = 0) = 0;

    // draw a scrollbar: thumb positions are in percent of the full scrollbar
    // length and the flags array contains the flags corresponding to each of
    // wxScrollBar::Elements
    virtual void DrawScrollbar(wxDC& dc,
                               wxOrientation orient,
                               int thumbPosStart,
                               int thumbPosEnd,
                               const wxRect& rect,
                               const int *flags = NULL) = 0;

    // TODO: having this is ugly but I don't see how to solve GetBestSize()
    //       problem without something like this

    // adjust the size of the control of the given class: for most controls,
    // this just takes into account the border, but for some (buttons, for
    // example) it is more complicated - the result being, in any case, that
    // the control looks "nice" if it uses the adjusted rectangle
    virtual void AdjustSize(wxSize *size, const wxWindow *window) = 0;

    // hit testing functions
    // ---------------------

    // returns one of wxHT_SCROLLBAR_XXX constants
    virtual wxHitTest HitTestScrollbar(const wxScrollBar *scrollbar,
                                       const wxPoint& pt) const = 0;

    // translate the scrollbar position (in logical units) into physical
    // coordinate (in pixels) and the other way round
    virtual wxCoord ScrollbarToPixel(const wxScrollBar *scrollbar) = 0;
    virtual int PixelToScrollbar(const wxScrollBar *scrollbar,
                                 wxCoord coord) = 0;

    // virtual dtor for any base class
    virtual ~wxRenderer();

protected:
    // standard scrollbar hit testing: this assumes that it only has 2 arrows
    // and a thumb, so the themes which have more complicated scrollbars (e.g.
    // BeOS) can't use this method
    static wxHitTest StandardHitTestScrollbar(const wxScrollBar *scrollbar,
                                              const wxPoint& pt,
                                              const wxSize& sizeArrow);
    static wxCoord StandardScrollbarToPixel(const wxScrollBar *scrollbar,
                                            const wxSize& sizeArrow);
    static int StandardPixelToScrollbar(const wxScrollBar *scrollbar,
                                        wxCoord coord,
                                        const wxSize& sizeArrow);
    static wxCoord StandardScrollBarSize(const wxScrollBar *scrollbar,
                                         const wxSize& sizeArrow);
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
                                const wxRect& rect,
                                int flags)
        { m_renderer->DrawBackground(dc, rect, flags); }
    virtual void DrawLabel(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = 0,
                           int align = wxALIGN_LEFT | wxALIGN_TOP,
                           int indexAccel = -1)
        { m_renderer->DrawLabel(dc, label, rect, flags, align, indexAccel); }
    virtual void DrawBorder(wxDC& dc,
                            wxBorder border,
                            const wxRect& rect,
                            int flags = 0,
                            wxRect *rectIn = (wxRect *)NULL)
        { m_renderer->DrawBorder(dc, border, rect, flags, rectIn); }
    virtual void DrawFrame(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = 0,
                           int align = wxALIGN_LEFT,
                           int indexAccel = -1)
        { m_renderer->DrawFrame(dc, label, rect, flags, align, indexAccel); }
    virtual void DrawHorizontalLine(wxDC& dc,
                                    wxCoord y, wxCoord x1, wxCoord x2)
        { m_renderer->DrawHorizontalLine(dc, y, x1, x2); }
    virtual void DrawVerticalLine(wxDC& dc,
                                  wxCoord x, wxCoord y1, wxCoord y2)
        { m_renderer->DrawVerticalLine(dc, x, y1, y2); }
    virtual void DrawButtonBorder(wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0,
                                  wxRect *rectIn = (wxRect *)NULL)
        { m_renderer->DrawButtonBorder(dc, rect, flags, rectIn); }
    virtual void DrawArrow(wxDC& dc,
                           wxDirection dir,
                           const wxRect& rect,
                           int flags = 0)
        { m_renderer->DrawArrow(dc, dir, rect, flags); }
    virtual void DrawScrollbar(wxDC& dc,
                               wxOrientation orient,
                               int thumbPosStart,
                               int thumbPosEnd,
                               const wxRect& rect,
                               const int *flags = NULL)
        { m_renderer->DrawScrollbar(dc, orient, thumbPosStart,
                                    thumbPosEnd, rect, flags); }

    virtual void AdjustSize(wxSize *size, const wxWindow *window)
        { m_renderer->AdjustSize(size, window); }

    virtual wxHitTest HitTestScrollbar(const wxScrollBar *scrollbar,
                                       const wxPoint& pt) const
        { return m_renderer->HitTestScrollbar(scrollbar, pt); }
    virtual wxCoord ScrollbarToPixel(const wxScrollBar *scrollbar)
        { return m_renderer->ScrollbarToPixel(scrollbar); }
    virtual int PixelToScrollbar(const wxScrollBar *scrollbar,
                                 wxCoord coord)
        { return m_renderer->PixelToScrollbar(scrollbar, coord); }

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
    // the line must be either horizontal or vertical
    void DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);
    void DrawFrame();
    void DrawBitmap(const wxBitmap& bitmap);
    void DrawBackgroundBitmap();
    void DrawScrollbar(const wxScrollBar *scrollbar);

    // accessors
    wxRenderer *GetRenderer() const { return m_renderer; }

    wxDC& GetDC() { return m_dc; }

    const wxRect& GetRect() const { return m_rect; }
    wxRect& GetRect() { return m_rect; }

protected:
    void DoDrawBitmap(const wxBitmap& bmp, int alignment, wxStretch stretch);

private:
    wxControl *m_ctrl;
    wxRenderer *m_renderer;
    wxDC& m_dc;
    wxRect m_rect;
};

#endif // _WX_UNIV_RENDERER_H_

