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

class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxListBox;
class WXDLLEXPORT wxScrollBar;
class WXDLLEXPORT wxWindow;

#include "wx/string.h"
#include "wx/gdicmn.h"
#include "wx/scrolbar.h"            // for wxScrollBar::Element

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
                                const wxColour& col,
                                const wxRect& rect,
                                int flags) = 0;

    // draw the label inside the given rectangle with the specified alignment
    // and optionally emphasize the character with the given index
    virtual void DrawLabel(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = 0,
                           int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                           int indexAccel = -1,
                           wxRect *rectBounds = NULL) = 0;

    // same but also draw a bitmap if it is valid
    virtual void DrawButtonLabel(wxDC& dc,
                                 const wxString& label,
                                 const wxBitmap& image,
                                 const wxRect& rect,
                                 int flags = 0,
                                 int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                                 int indexAccel = -1,
                                 wxRect *rectBounds = NULL) = 0;

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

    // draw the scrollbar thumb
    virtual void DrawScrollbarThumb(wxDC& dc,
                                    wxOrientation orient,
                                    const wxRect& rect,
                                    int flags = 0) = 0;

    // draw a (part of) scrollbar shaft
    virtual void DrawScrollbarShaft(wxDC& dc,
                                    wxOrientation orient,
                                    const wxRect& rect,
                                    int flags = 0) = 0;

    // draw the rectangle in the corner between two scrollbars
    virtual void DrawScrollCorner(wxDC& dc,
                                  const wxRect& rect) = 0;

    // draw an item of a wxControlWithItems
    virtual void DrawItem(wxDC& dc,
                          const wxString& label,
                          const wxRect& rect,
                          int flags = 0) = 0;

    // draw a checkbutton
    virtual void DrawCheckButton(wxDC& dc,
                                 const wxString& label,
                                 const wxBitmap& bitmap,
                                 const wxRect& rect,
                                 int flags = 0,
                                 wxAlignment align = wxALIGN_LEFT,
                                 int indexAccel = -1) = 0;

    // geometry functions
    // ------------------

    // get the dimensions of the border: rect.x/y contain the width/height of
    // the left/top side, width/heigh - of the right/bottom one
    virtual wxRect GetBorderDimensions(wxBorder border) const = 0;

    // the scrollbars may be drawn either inside the window border or outside
    // it - this function is used to decide how to draw them
    virtual bool AreScrollbarsInsideBorder() const = 0;

    // adjust the size of the control of the given class: for most controls,
    // this just takes into account the border, but for some (buttons, for
    // example) it is more complicated - the result being, in any case, that
    // the control looks "nice" if it uses the adjusted rectangle
    virtual void AdjustSize(wxSize *size, const wxWindow *window) = 0;

    // gets the bounding box for a scrollbar element for the given (by default
    // - current) thumb position
    virtual wxRect GetScrollbarRect(const wxScrollBar *scrollbar,
                                    wxScrollBar::Element elem,
                                    int thumbPos = -1) const = 0;

    // returns the size of the scrollbar shaft excluding the arrows
    virtual wxCoord GetScrollbarSize(const wxScrollBar *scrollbar) = 0;

    // returns one of wxHT_SCROLLBAR_XXX constants
    virtual wxHitTest HitTestScrollbar(const wxScrollBar *scrollbar,
                                       const wxPoint& pt) const = 0;

    // translate the scrollbar position (in logical units) into physical
    // coordinate (in pixels) and the other way round
    virtual wxCoord ScrollbarToPixel(const wxScrollBar *scrollbar,
                                     int thumbPos = -1) = 0;
    virtual int PixelToScrollbar(const wxScrollBar *scrollbar,
                                 wxCoord coord) = 0;

    // get the height of a listbox item from the base font height
    virtual wxCoord GetListboxItemHeight(wxCoord fontHeight) = 0;

    // virtual dtor for any base class
    virtual ~wxRenderer();

protected:
    // draw a frame around rectFrame rectangle but not touching the rectLabel
    // one: this is used by DrawFrame()
    void StandardDrawFrame(wxDC& dc,
                           const wxRect& rectFrame,
                           const wxRect& rectLabel);

    // standard scrollbar hit testing: this assumes that it only has 2 arrows
    // and a thumb, so the themes which have more complicated scrollbars (e.g.
    // BeOS) can't use this method
    static wxRect StandardGetScrollbarRect(const wxScrollBar *scrollbar,
                                           wxScrollBar::Element elem,
                                           int thumbPos,
                                           const wxSize& sizeArrow);
    static wxHitTest StandardHitTestScrollbar(const wxScrollBar *scrollbar,
                                              const wxPoint& pt,
                                              const wxSize& sizeArrow);
    static wxCoord StandardScrollbarToPixel(const wxScrollBar *scrollbar,
                                            int thumbPos,
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
                                const wxColour& col,
                                const wxRect& rect,
                                int flags)
        { m_renderer->DrawBackground(dc, col, rect, flags); }
    virtual void DrawLabel(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = 0,
                           int align = wxALIGN_LEFT | wxALIGN_TOP,
                           int indexAccel = -1,
                           wxRect *rectBounds = NULL)
        { m_renderer->DrawLabel(dc, label, rect,
                                flags, align, indexAccel, rectBounds); }
    virtual void DrawButtonLabel(wxDC& dc,
                                 const wxString& label,
                                 const wxBitmap& image,
                                 const wxRect& rect,
                                 int flags = 0,
                                 int align = wxALIGN_LEFT | wxALIGN_TOP,
                                 int indexAccel = -1,
                                 wxRect *rectBounds = NULL)
        { m_renderer->DrawButtonLabel(dc, label, image, rect,
                                      flags, align, indexAccel, rectBounds); }
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
    virtual void DrawScrollbarThumb(wxDC& dc,
                                    wxOrientation orient,
                                    const wxRect& rect,
                                    int flags = 0)
        { m_renderer->DrawScrollbarThumb(dc, orient, rect, flags); }
    virtual void DrawScrollbarShaft(wxDC& dc,
                                    wxOrientation orient,
                                    const wxRect& rect,
                                    int flags = 0)
        { m_renderer->DrawScrollbarShaft(dc, orient, rect, flags); }
    virtual void DrawScrollCorner(wxDC& dc,
                                  const wxRect& rect)
        { m_renderer->DrawScrollCorner(dc, rect); }
    virtual void DrawItem(wxDC& dc,
                          const wxString& label,
                          const wxRect& rect,
                          int flags = 0)
        { m_renderer->DrawItem(dc, label, rect, flags); }
    virtual void DrawCheckButton(wxDC& dc,
                                 const wxString& label,
                                 const wxBitmap& bitmap,
                                 const wxRect& rect,
                                 int flags = 0,
                                 wxAlignment align = wxALIGN_LEFT,
                                 int indexAccel = -1)
        { m_renderer->DrawCheckButton(dc, label, bitmap, rect,
                                      flags, align, indexAccel); }

    virtual void AdjustSize(wxSize *size, const wxWindow *window)
        { m_renderer->AdjustSize(size, window); }
    virtual wxRect GetBorderDimensions(wxBorder border) const
        { return m_renderer->GetBorderDimensions(border); }
    virtual bool AreScrollbarsInsideBorder() const
        { return m_renderer->AreScrollbarsInsideBorder(); }

    virtual wxRect GetScrollbarRect(const wxScrollBar *scrollbar,
                                    wxScrollBar::Element elem,
                                    int thumbPos = -1) const
        { return m_renderer->GetScrollbarRect(scrollbar, elem, thumbPos); }
    virtual wxCoord GetScrollbarSize(const wxScrollBar *scrollbar)
        { return m_renderer->GetScrollbarSize(scrollbar); }
    virtual wxHitTest HitTestScrollbar(const wxScrollBar *scrollbar,
                                       const wxPoint& pt) const
        { return m_renderer->HitTestScrollbar(scrollbar, pt); }
    virtual wxCoord ScrollbarToPixel(const wxScrollBar *scrollbar,
                                     int thumbPos = -1)
        { return m_renderer->ScrollbarToPixel(scrollbar, thumbPos); }
    virtual int PixelToScrollbar(const wxScrollBar *scrollbar,
                                 wxCoord coord)
        { return m_renderer->PixelToScrollbar(scrollbar, coord); }
    virtual wxCoord GetListboxItemHeight(wxCoord fontHeight)
        { return m_renderer->GetListboxItemHeight(fontHeight); }

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
    wxControlRenderer(wxWindow *control, wxDC& dc, wxRenderer *renderer);

    // operations
    void DrawLabel(const wxBitmap& bitmap = wxNullBitmap,
                   wxCoord marginX = 0, wxCoord marginY = 0);
    void DrawItems(const wxListBox *listbox,
                   size_t itemFirst, size_t itemLast);
    void DrawBorder();
    void DrawButtonBorder();
    // the line must be either horizontal or vertical
    void DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);
    void DrawFrame();
    void DrawBitmap(const wxBitmap& bitmap);
    void DrawBitmap(const wxBitmap& bitmap,
                    const wxRect& rect,
                    int alignment = wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL,
                    wxStretch stretch = wxSTRETCH_NOT);
    void DrawBackgroundBitmap();
    void DrawScrollbar(const wxScrollBar *scrollbar, int thumbPosOld);

    // accessors
    wxWindow *GetWindow() const { return m_window; }
    wxRenderer *GetRenderer() const { return m_renderer; }

    wxDC& GetDC() { return m_dc; }

    const wxRect& GetRect() const { return m_rect; }
    wxRect& GetRect() { return m_rect; }

private:
    wxWindow *m_window;
    wxRenderer *m_renderer;
    wxDC& m_dc;
    wxRect m_rect;
};

#endif // _WX_UNIV_RENDERER_H_

