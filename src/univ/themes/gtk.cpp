///////////////////////////////////////////////////////////////////////////////
// Name:        univ/themes/gtk.cpp
// Purpose:     wxUniversal theme implementing GTK-like LNF
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/dcmemory.h"
    #include "wx/window.h"

    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/listbox.h"
    #include "wx/checklst.h"
    #include "wx/scrolbar.h"
    #include "wx/textctrl.h"
#endif // WX_PRECOMP

#include "wx/spinbutt.h"

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/colschem.h"
#include "wx/univ/theme.h"

// ----------------------------------------------------------------------------
// constants (to be removed, for testing only)
// ----------------------------------------------------------------------------

static const size_t BORDER_THICKNESS = 1;

// ----------------------------------------------------------------------------
// wxGTKRenderer: draw the GUI elements in GTK style
// ----------------------------------------------------------------------------

class wxGTKRenderer : public wxRenderer
{
public:
    wxGTKRenderer(const wxColourScheme *scheme);

    // implement the base class pure virtuals
    virtual void DrawBackground(wxDC& dc,
                                const wxColour& col,
                                const wxRect& rect,
                                int flags = 0);
    virtual void DrawLabel(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = 0,
                           int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                           int indexAccel = -1,
                           wxRect *rectBounds = NULL);
    virtual void DrawButtonLabel(wxDC& dc,
                                 const wxString& label,
                                 const wxBitmap& image,
                                 const wxRect& rect,
                                 int flags = 0,
                                 int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                                 int indexAccel = -1,
                                 wxRect *rectBounds = NULL);
    virtual void DrawBorder(wxDC& dc,
                            wxBorder border,
                            const wxRect& rect,
                            int flags = 0,
                            wxRect *rectIn = (wxRect *)NULL);
    virtual void DrawHorizontalLine(wxDC& dc,
                                    wxCoord y, wxCoord x1, wxCoord x2);
    virtual void DrawVerticalLine(wxDC& dc,
                                  wxCoord x, wxCoord y1, wxCoord y2);
    virtual void DrawFrame(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = 0,
                           int alignment = wxALIGN_LEFT,
                           int indexAccel = -1);
    virtual void DrawTextBorder(wxDC& dc,
                                wxBorder border,
                                const wxRect& rect,
                                int flags = 0,
                                wxRect *rectIn = (wxRect *)NULL);
    virtual void DrawButtonBorder(wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0,
                                  wxRect *rectIn = (wxRect *)NULL);
    virtual void DrawArrow(wxDC& dc,
                           wxDirection dir,
                           const wxRect& rect,
                           int flags = 0);
    virtual void DrawScrollbarArrow(wxDC& dc,
                                    wxDirection dir,
                                    const wxRect& rect,
                                    int flags = 0);
    virtual void DrawScrollbarThumb(wxDC& dc,
                                    wxOrientation orient,
                                    const wxRect& rect,
                                    int flags = 0);
    virtual void DrawScrollbarShaft(wxDC& dc,
                                    wxOrientation orient,
                                    const wxRect& rect,
                                    int flags = 0);
    virtual void DrawScrollCorner(wxDC& dc,
                                  const wxRect& rect);
    virtual void DrawItem(wxDC& dc,
                          const wxString& label,
                          const wxRect& rect,
                          int flags = 0);
    virtual void DrawCheckItem(wxDC& dc,
                               const wxString& label,
                               const wxBitmap& bitmap,
                               const wxRect& rect,
                               int flags = 0);
    virtual void DrawCheckButton(wxDC& dc,
                                 const wxString& label,
                                 const wxBitmap& bitmap,
                                 const wxRect& rect,
                                 int flags = 0,
                                 wxAlignment align = wxALIGN_LEFT,
                                 int indexAccel = -1);

    virtual void DrawRadioButton(wxDC& dc,
                                 const wxString& label,
                                 const wxBitmap& bitmap,
                                 const wxRect& rect,
                                 int flags = 0,
                                 wxAlignment align = wxALIGN_LEFT,
                                 int indexAccel = -1);

    virtual void DrawTextLine(wxDC& dc,
                              const wxString& text,
                              const wxRect& rect,
                              int selStart = -1,
                              int selEnd = -1,
                              int flags = 0);
    virtual void DrawLineWrapMark(wxDC& dc, const wxRect& rect);

    virtual void GetComboBitmaps(wxBitmap *bmpNormal,
                                 wxBitmap *bmpPressed,
                                 wxBitmap *bmpDisabled);

    virtual void AdjustSize(wxSize *size, const wxWindow *window);
    virtual wxRect GetBorderDimensions(wxBorder border) const;
    virtual bool AreScrollbarsInsideBorder() const;

    // geometry and hit testing
    virtual wxSize GetScrollbarArrowSize() const
        { return m_sizeScrollbarArrow; }
    virtual wxRect GetScrollbarRect(const wxScrollBar *scrollbar,
                                    wxScrollBar::Element elem,
                                    int thumbPos = -1) const;
    virtual wxCoord GetScrollbarSize(const wxScrollBar *scrollbar);
    virtual wxHitTest HitTestScrollbar(const wxScrollBar *scrollbar,
                                       const wxPoint& pt) const;
    virtual wxCoord ScrollbarToPixel(const wxScrollBar *scrollbar,
                                     int thumbPos = -1);
    virtual int PixelToScrollbar(const wxScrollBar *scrollbar, wxCoord coord);
    virtual wxCoord GetListboxItemHeight(wxCoord fontHeight)
        { return fontHeight + 2; }
    virtual wxSize GetCheckBitmapSize() const
        { return wxSize(10, 10); }
    virtual wxSize GetRadioBitmapSize() const
        { return wxSize(11, 11); }
    virtual wxCoord GetCheckItemMargin() const
        { return 2; }

    virtual wxRect GetTextTotalArea(const wxTextCtrl *text,
                                    const wxRect& rect);
    virtual wxRect GetTextClientArea(const wxTextCtrl *text,
                                     const wxRect& rect,
                                     wxCoord *extraSpaceBeyond);

    // helpers for "wxBitmap wxColourScheme::Get()"
    void DrawCheckBitmap(wxDC& dc, const wxRect& rect);
    void DrawUncheckBitmap(wxDC& dc, const wxRect& rect, bool isPressed);

protected:
    // DrawBackground() helpers

    // get the colour to use for background
    wxColour GetBackgroundColour(int flags) const
    {
        if ( flags & wxCONTROL_PRESSED )
            return wxSCHEME_COLOUR(m_scheme, CONTROL_PRESSED);
        else if ( flags & wxCONTROL_CURRENT )
            return wxSCHEME_COLOUR(m_scheme, CONTROL_CURRENT);
        else
            return wxSCHEME_COLOUR(m_scheme, CONTROL);
    }

    // draw the background with any colour, not only the default one(s)
    void DoDrawBackground(wxDC& dc,
                          const wxColour& col,
                          const wxRect& rect);

    // DrawBorder() helpers: all of them shift and clip the DC after drawing
    // the border

    // just draw a rectangle with the given pen
    void DrawRect(wxDC& dc, wxRect *rect, const wxPen& pen);

    // draw the lower left part of rectangle
    void DrawHalfRect(wxDC& dc, wxRect *rect, const wxPen& pen);

    // draw the rectange using the first brush for the left and top sides and
    // the second one for the bottom and right ones
    void DrawShadedRect(wxDC& dc, wxRect *rect,
                        const wxPen& pen1, const wxPen& pen2);

    // as DrawShadedRect() but the pixels in the bottom left and upper right
    // border are drawn with the pen1, not pen2
    void DrawAntiShadedRect(wxDC& dc, wxRect *rect,
                            const wxPen& pen1, const wxPen& pen2);

    // used for drawing opened rectangles - draws only one side of it at once
    // (and doesn't adjust the rect)
    void DrawAntiShadedRectSide(wxDC& dc,
                                const wxRect& rect,
                                const wxPen& pen1,
                                const wxPen& pen2,
                                wxDirection dir);

    // draw an opened rect for the arrow in given direction
    void DrawArrowBorder(wxDC& dc,
                         wxRect *rect,
                         wxDirection dir);

    // draw two sides of the rectangle
    void DrawThumbBorder(wxDC& dc,
                         wxRect *rect,
                         wxOrientation orient);

    // draw the normal 3D border
    void DrawRaisedBorder(wxDC& dc, wxRect *rect);

    // returns the size of the arrow for the scrollbar (depends on
    // orientation)
    wxSize GetScrollbarArrowSize(const wxScrollBar *scrollbar) const
    {
        wxSize size;
        if ( scrollbar->IsVertical() )
        {
            size = m_sizeScrollbarArrow;
        }
        else
        {
            size.x = m_sizeScrollbarArrow.y;
            size.y = m_sizeScrollbarArrow.x;
        }

        return size;
    }

    // get the line wrap indicator bitmap
    wxBitmap GetLineWrapBitmap();

    // DrawCheckBitmap and DrawRadioBitmap helpers

    // draw the check bitmaps once and cache them for later use
    wxBitmap GetCheckBitmap(int flags);

    // draw a /\ or \/ line from (x1, y1) to (x2, y1) passing by the point
    // ((x1 + x2)/2, y2)
    void DrawUpZag(wxDC& dc,
                   wxCoord x1, wxCoord x2,
                   wxCoord y1, wxCoord y2);
    void DrawDownZag(wxDC& dc,
                     wxCoord x1, wxCoord x2,
                     wxCoord y1, wxCoord y2);

    // draw the radio button bitmap for the given state
    void DrawRadioBitmap(wxDC& dc, const wxRect& rect, int flags);

    // draw check/radio - the bitmap must be a valid one by now
    void DoDrawCheckOrRadioBitmap(wxDC& dc,
                                  const wxString& label,
                                  const wxBitmap& bitmap,
                                  const wxRect& rectTotal,
                                  int flags,
                                  wxAlignment align,
                                  int indexAccel);

private:
    const wxColourScheme *m_scheme;

    // data
    wxSize m_sizeScrollbarArrow;

    // GDI objects
    wxPen m_penBlack,
          m_penDarkGrey,
          m_penGrey,
          m_penLightGrey,
          m_penHighlight;

    // the checkbox bitmaps: first row is for the normal, second for the
    // pressed state and the columns are for checked and unchecked status
    // respectively
    wxBitmap m_bitmapsCheckbox[2][2];

    // the line wrap bitmap (drawn at the end of wrapped lines)
    wxBitmap m_bmpLineWrap;
};

// ----------------------------------------------------------------------------
// wxGTKInputHandler and derived classes: process the keyboard and mouse
// messages according to GTK standards
// ----------------------------------------------------------------------------

class wxGTKInputHandler : public wxInputHandler
{
public:
    wxGTKInputHandler(wxGTKRenderer *renderer);

    virtual bool HandleKey(wxControl *control,
                           const wxKeyEvent& event,
                           bool pressed);
    virtual bool HandleMouse(wxControl *control,
                             const wxMouseEvent& event);
    virtual bool HandleMouseMove(wxControl *control, const wxMouseEvent& event);

protected:
    wxGTKRenderer *m_renderer;
};

class wxGTKScrollBarInputHandler : public wxStdScrollBarInputHandler
{
public:
    wxGTKScrollBarInputHandler(wxRenderer *renderer, wxInputHandler *handler)
        : wxStdScrollBarInputHandler(renderer, handler) { }

protected:
    virtual void Highlight(wxScrollBar *scrollbar, bool doIt)
    {
        // only arrows and the thumb can be highlighted
        if ( !IsArrow() && m_htLast != wxHT_SCROLLBAR_THUMB )
            return;

        wxStdScrollBarInputHandler::Highlight(scrollbar, doIt);
    }

    virtual void Press(wxScrollBar *scrollbar, bool doIt)
    {
        // only arrows can be pressed
        if ( !IsArrow() )
            return;

        wxStdScrollBarInputHandler::Press(scrollbar, doIt);
    }

    virtual bool IsAllowedButton(int WXUNUSED(button)) { return TRUE; }

    bool IsArrow() const
    {
        return m_htLast == wxHT_SCROLLBAR_ARROW_LINE_1 ||
                m_htLast == wxHT_SCROLLBAR_ARROW_LINE_2;
    }
};

class wxGTKCheckboxInputHandler : public wxStdCheckboxInputHandler
{
public:
    wxGTKCheckboxInputHandler(wxInputHandler *handler)
        : wxStdCheckboxInputHandler(handler) { }

    virtual bool HandleKey(wxControl *control,
                           const wxKeyEvent& event,
                           bool pressed);
};

class wxGTKTextCtrlInputHandler : public wxStdTextCtrlInputHandler
{
public:
    wxGTKTextCtrlInputHandler(wxInputHandler *handler)
        : wxStdTextCtrlInputHandler(handler) { }

    virtual bool HandleKey(wxControl *control,
                           const wxKeyEvent& event,
                           bool pressed);
};

// ----------------------------------------------------------------------------
// wxGTKColourScheme: uses the standard GTK colours
// ----------------------------------------------------------------------------

class wxGTKColourScheme : public wxColourScheme
{
public:
    virtual wxColour Get(StdColour col) const;
    virtual wxColour GetBackground(wxWindow *win) const;
};

// ----------------------------------------------------------------------------
// wxGTKTheme
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY(wxInputHandler *, wxArrayHandlers);

class wxGTKTheme : public wxTheme
{
public:
    wxGTKTheme();
    virtual ~wxGTKTheme();

    virtual wxRenderer *GetRenderer() { return m_renderer; }
    virtual wxInputHandler *GetInputHandler(const wxString& control);
    virtual wxColourScheme *GetColourScheme() { return m_scheme; }

private:
    // get the default input handler
    wxInputHandler *GetDefaultInputHandler();

    wxGTKRenderer *m_renderer;

    // the names of the already created handlers and the handlers themselves
    // (these arrays are synchronized)
    wxSortedArrayString m_handlerNames;
    wxArrayHandlers m_handlers;

    wxGTKInputHandler *m_handlerDefault;

    wxGTKColourScheme *m_scheme;

    WX_DECLARE_THEME(gtk)
};

// ============================================================================
// implementation
// ============================================================================

WX_IMPLEMENT_THEME(wxGTKTheme, gtk, wxTRANSLATE("GTK+ theme"));

// ----------------------------------------------------------------------------
// wxGTKTheme
// ----------------------------------------------------------------------------

wxGTKTheme::wxGTKTheme()
{
    m_scheme = new wxGTKColourScheme;
    m_renderer = new wxGTKRenderer(m_scheme);
    m_handlerDefault = NULL;
}

wxGTKTheme::~wxGTKTheme()
{
    size_t count = m_handlers.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( m_handlers[n] != m_handlerDefault )
            delete m_handlers[n];
    }

    delete m_handlerDefault;
    delete m_renderer;
    delete m_scheme;
}

wxInputHandler *wxGTKTheme::GetDefaultInputHandler()
{
    if ( !m_handlerDefault )
    {
        m_handlerDefault = new wxGTKInputHandler(m_renderer);
    }

    return m_handlerDefault;
}

wxInputHandler *wxGTKTheme::GetInputHandler(const wxString& control)
{
    wxInputHandler *handler;
    int n = m_handlerNames.Index(control);
    if ( n == wxNOT_FOUND )
    {
        // create a new handler
        if ( control == wxINP_HANDLER_SCROLLBAR )
            handler = new wxGTKScrollBarInputHandler(m_renderer,
                                                     GetDefaultInputHandler());
#if wxUSE_BUTTON
        else if ( control == wxINP_HANDLER_BUTTON )
            handler = new wxStdButtonInputHandler(GetDefaultInputHandler());
#endif // wxUSE_CHECKBOX
#if wxUSE_CHECKBOX
        else if ( control == wxINP_HANDLER_CHECKBOX )
            handler = new wxGTKCheckboxInputHandler(GetDefaultInputHandler());
#endif // wxUSE_CHECKBOX
#if wxUSE_COMBOBOX
        else if ( control == wxINP_HANDLER_COMBOBOX )
            handler = new wxStdComboBoxInputHandler(GetDefaultInputHandler());
#endif // wxUSE_COMBOBOX
#if wxUSE_LISTBOX
        else if ( control == wxINP_HANDLER_LISTBOX )
            handler = new wxStdListboxInputHandler(GetDefaultInputHandler());
#endif // wxUSE_LISTBOX
#if wxUSE_CHECKLISTBOX
        else if ( control == wxINP_HANDLER_CHECKLISTBOX )
            handler = new wxStdCheckListboxInputHandler(GetDefaultInputHandler());
#endif // wxUSE_CHECKLISTBOX
#if wxUSE_TEXTCTRL
        else if ( control == wxINP_HANDLER_TEXTCTRL )
            handler = new wxGTKTextCtrlInputHandler(GetDefaultInputHandler());
#endif // wxUSE_TEXTCTRL
#if wxUSE_SPINBTN
        else if ( control == wxINP_HANDLER_SPINBTN )
            handler = new wxStdSpinButtonInputHandler(GetDefaultInputHandler());
#endif // wxUSE_SPINBTN
        else
            handler = GetDefaultInputHandler();

        n = m_handlerNames.Add(control);
        m_handlers.Insert(handler, n);
    }
    else // we already have it
    {
        handler = m_handlers[n];
    }

    return handler;
}

// ============================================================================
// wxGTKColourScheme
// ============================================================================

wxColour wxGTKColourScheme::GetBackground(wxWindow *win) const
{
    wxColour col;
    if ( win->UseBgCol() )
    {
        // use the user specified colour
        col = win->GetBackgroundColour();
    }

    if ( win->IsContainerWindow() )
    {
        // doesn't depend on the state
        if ( !col.Ok() )
        {
            col = Get(WINDOW);
        }
    }
    else
    {
        int flags = win->GetStateFlags();

        // the colour set by the user should be used for the normal state
        // and for the states for which we don't have any specific colours
        if ( !col.Ok() || (flags != 0) )
        {
            if ( wxDynamicCast(win, wxScrollBar) )
                col = Get(SCROLLBAR);
            else if ( (flags & wxCONTROL_CURRENT) && win->CanBeHighlighted() )
                col = Get(CONTROL_CURRENT);
            else if ( flags & wxCONTROL_PRESSED )
                col = Get(CONTROL_PRESSED);
            else
                col = Get(CONTROL);
        }
    }

    return col;
}

wxColour wxGTKColourScheme::Get(wxGTKColourScheme::StdColour col) const
{
    switch ( col )
    {
        case WINDOW:            return *wxWHITE;

        case SHADOW_DARK:       return *wxBLACK;
        case SHADOW_HIGHLIGHT:  return *wxWHITE;
        case SHADOW_IN:         return wxColour(0xd6d6d6);
        case SHADOW_OUT:        return wxColour(0x969696);

        case CONTROL:           return wxColour(0xd6d6d6);
        case CONTROL_PRESSED:   return wxColour(0xc3c3c3);
        case CONTROL_CURRENT:   return *wxBLUE; //wxColour(0xeaeaea);

        case CONTROL_TEXT:      return *wxBLACK;
        case CONTROL_TEXT_DISABLED:
                                return wxColour(0x757575);
        case CONTROL_TEXT_DISABLED_SHADOW:
                                return *wxWHITE;

        case SCROLLBAR:
        case SCROLLBAR_PRESSED: return wxColour(0xc3c3c3);

        case HIGHLIGHT:         return wxColour(0x9c0000);
        case HIGHLIGHT_TEXT:    return wxColour(0xffffff);

        case MAX:
        default:
            wxFAIL_MSG(_T("invalid standard colour"));
            return *wxBLACK;
    }
}

// ============================================================================
// wxGTKRenderer
// ============================================================================

// ----------------------------------------------------------------------------
// construction
// ----------------------------------------------------------------------------

wxGTKRenderer::wxGTKRenderer(const wxColourScheme *scheme)
{
    // init data
    m_scheme = scheme;
    m_sizeScrollbarArrow = wxSize(15, 14);

    // init pens
    m_penBlack = wxPen(wxSCHEME_COLOUR(scheme, SHADOW_DARK), 0, wxSOLID);
    m_penDarkGrey = wxPen(wxSCHEME_COLOUR(scheme, SHADOW_OUT), 0, wxSOLID);
    m_penGrey = wxPen(wxSCHEME_COLOUR(scheme, SCROLLBAR), 0, wxSOLID);
    m_penLightGrey = wxPen(wxSCHEME_COLOUR(scheme, SHADOW_IN), 0, wxSOLID);
    m_penHighlight = wxPen(wxSCHEME_COLOUR(scheme, SHADOW_HIGHLIGHT), 0, wxSOLID);
}

// ----------------------------------------------------------------------------
// border stuff
// ----------------------------------------------------------------------------

void wxGTKRenderer::DrawRect(wxDC& dc, wxRect *rect, const wxPen& pen)
{
    // draw
    dc.SetPen(pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(*rect);

    // adjust the rect
    rect->Inflate(-1);
}

void wxGTKRenderer::DrawHalfRect(wxDC& dc, wxRect *rect, const wxPen& pen)
{
    // draw the bottom and right sides
    dc.SetPen(pen);
    dc.DrawLine(rect->GetLeft(), rect->GetBottom(),
                rect->GetRight() + 1, rect->GetBottom());
    dc.DrawLine(rect->GetRight(), rect->GetTop(),
                rect->GetRight(), rect->GetBottom());

    // adjust the rect
    rect->width--;
    rect->height--;
}

void wxGTKRenderer::DrawShadedRect(wxDC& dc, wxRect *rect,
                                   const wxPen& pen1, const wxPen& pen2)
{
    // draw the rectangle
    dc.SetPen(pen1);
    dc.DrawLine(rect->GetLeft(), rect->GetTop(),
                rect->GetLeft(), rect->GetBottom());
    dc.DrawLine(rect->GetLeft() + 1, rect->GetTop(),
                rect->GetRight(), rect->GetTop());
    dc.SetPen(pen2);
    dc.DrawLine(rect->GetRight(), rect->GetTop(),
                rect->GetRight(), rect->GetBottom());
    dc.DrawLine(rect->GetLeft(), rect->GetBottom(),
                rect->GetRight() + 1, rect->GetBottom());

    // adjust the rect
    rect->Inflate(-1);
}

void wxGTKRenderer::DrawAntiShadedRectSide(wxDC& dc,
                                           const wxRect& rect,
                                           const wxPen& pen1,
                                           const wxPen& pen2,
                                           wxDirection dir)
{
    dc.SetPen(dir == wxLEFT || dir == wxUP ? pen1 : pen2);

    switch ( dir )
    {
        case wxLEFT:
            dc.DrawLine(rect.GetLeft(), rect.GetTop(),
                        rect.GetLeft(), rect.GetBottom() + 1);
            break;

        case wxUP:
            dc.DrawLine(rect.GetLeft(), rect.GetTop(),
                        rect.GetRight() + 1, rect.GetTop());
            break;

        case wxRIGHT:
            dc.DrawLine(rect.GetRight(), rect.GetTop(),
                        rect.GetRight(), rect.GetBottom() + 1);
            break;

        case wxDOWN:
            dc.DrawLine(rect.GetLeft(), rect.GetBottom(),
                        rect.GetRight() + 1, rect.GetBottom());
            break;

        default:
            wxFAIL_MSG(_T("unknown rectangle side"));
    }
}

void wxGTKRenderer::DrawAntiShadedRect(wxDC& dc, wxRect *rect,
                                       const wxPen& pen1, const wxPen& pen2)
{
    // draw the rectangle
    dc.SetPen(pen1);
    dc.DrawLine(rect->GetLeft(), rect->GetTop(),
                rect->GetLeft(), rect->GetBottom() + 1);
    dc.DrawLine(rect->GetLeft() + 1, rect->GetTop(),
                rect->GetRight() + 1, rect->GetTop());
    dc.SetPen(pen2);
    dc.DrawLine(rect->GetRight(), rect->GetTop() + 1,
                rect->GetRight(), rect->GetBottom());
    dc.DrawLine(rect->GetLeft() + 1, rect->GetBottom(),
                rect->GetRight() + 1, rect->GetBottom());

    // adjust the rect
    rect->Inflate(-1);
}

void wxGTKRenderer::DrawRaisedBorder(wxDC& dc, wxRect *rect)
{
    DrawShadedRect(dc, rect, m_penHighlight, m_penBlack);
    DrawShadedRect(dc, rect, m_penLightGrey, m_penDarkGrey);
}

void wxGTKRenderer::DrawBorder(wxDC& dc,
                               wxBorder border,
                               const wxRect& rectTotal,
                               int flags,
                               wxRect *rectIn)
{
    size_t width;

    wxRect rect = rectTotal;

    switch ( border )
    {
        case wxBORDER_SUNKEN:
            for ( width = 0; width < BORDER_THICKNESS; width++ )
            {
                DrawAntiShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
                DrawShadedRect(dc, &rect, m_penBlack, m_penLightGrey);
            }
            break;

        case wxBORDER_STATIC:
            DrawShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
            break;

        case wxBORDER_RAISED:
            for ( width = 0; width < BORDER_THICKNESS; width++ )
            {
                DrawRaisedBorder(dc, &rect);
            }
            break;

        case wxBORDER_DOUBLE:
            DrawShadedRect(dc, &rect, m_penLightGrey, m_penBlack);
            DrawShadedRect(dc, &rect, m_penHighlight, m_penDarkGrey);
            DrawRect(dc, &rect, m_penLightGrey);
            break;

        case wxBORDER_SIMPLE:
            DrawRect(dc, &rect, m_penBlack);
            break;

        default:
            wxFAIL_MSG(_T("unknown border type"));
            // fall through

        case wxBORDER_DEFAULT:
        case wxBORDER_NONE:
            break;
    }

    if ( rectIn )
        *rectIn = rect;
}

wxRect wxGTKRenderer::GetBorderDimensions(wxBorder border) const
{
    wxCoord width;
    switch ( border )
    {
        case wxBORDER_RAISED:
        case wxBORDER_SUNKEN:
            width = 2*BORDER_THICKNESS;
            break;

        case wxBORDER_SIMPLE:
        case wxBORDER_STATIC:
            width = 1;
            break;

        case wxBORDER_DOUBLE:
            width = 3;
            break;

        default:
            wxFAIL_MSG(_T("unknown border type"));
            // fall through

        case wxBORDER_DEFAULT:
        case wxBORDER_NONE:
            width = 0;
            break;
    }

    wxRect rect;
    rect.x =
    rect.y =
    rect.width =
    rect.height = width;

    return rect;
}

bool wxGTKRenderer::AreScrollbarsInsideBorder() const
{
    // no, the scrollbars are outside the border in GTK+
    return FALSE;
}

// ----------------------------------------------------------------------------
// special borders
// ----------------------------------------------------------------------------

void wxGTKRenderer::DrawTextBorder(wxDC& dc,
                                   wxBorder border,
                                   const wxRect& rectOrig,
                                   int flags,
                                   wxRect *rectIn)
{
    wxRect rect = rectOrig;

    if ( flags & wxCONTROL_FOCUSED )
    {
        DrawRect(dc, &rect, m_penBlack);
        DrawAntiShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
    }
    else // !focused
    {
        DrawAntiShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
        DrawAntiShadedRect(dc, &rect, m_penBlack, m_penHighlight);
    }

    if ( rectIn )
        *rectIn = rect;
}

void wxGTKRenderer::DrawButtonBorder(wxDC& dc,
                                     const wxRect& rectTotal,
                                     int flags,
                                     wxRect *rectIn)
{
    wxRect rect = rectTotal;

    if ( flags & wxCONTROL_PRESSED )
    {
        // button pressed: draw a black border around it and an inward shade
        DrawRect(dc, &rect, m_penBlack);
        DrawAntiShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
        DrawAntiShadedRect(dc, &rect, m_penBlack, m_penDarkGrey);
    }
    else
    {
        // button not pressed

        if ( flags & wxCONTROL_ISDEFAULT )
        {
            // TODO
        }

        if ( flags & wxCONTROL_FOCUSED )
        {
            // button is currently default: add an extra border around it
            DrawRect(dc, &rect, m_penBlack);
        }

        // now draw a normal button
        DrawShadedRect(dc, &rect, m_penHighlight, m_penBlack);
        DrawAntiShadedRect(dc, &rect,
                           wxPen(GetBackgroundColour(flags), 0, wxSOLID),
                           m_penDarkGrey);
    }

    if ( rectIn )
    {
        *rectIn = rect;
    }
}

// ----------------------------------------------------------------------------
// lines and frames
// ----------------------------------------------------------------------------

void wxGTKRenderer::DrawHorizontalLine(wxDC& dc,
                                       wxCoord y, wxCoord x1, wxCoord x2)
{
    dc.SetPen(m_penDarkGrey);
    dc.DrawLine(x1, y, x2 + 1, y);
    dc.SetPen(m_penHighlight);
    y++;
    dc.DrawLine(x1, y, x2 + 1, y);
}

void wxGTKRenderer::DrawVerticalLine(wxDC& dc,
                                     wxCoord x, wxCoord y1, wxCoord y2)
{
    dc.SetPen(m_penDarkGrey);
    dc.DrawLine(x, y1, x, y2 + 1);
    dc.SetPen(m_penHighlight);
    x++;
    dc.DrawLine(x, y1, x, y2 + 1);
}

void wxGTKRenderer::DrawFrame(wxDC& dc,
                              const wxString& label,
                              const wxRect& rect,
                              int flags,
                              int alignment,
                              int indexAccel)
{
    wxCoord height = 0; // of the label
    wxRect rectFrame = rect;
    if ( !label.empty() )
    {
        // the text should touch the top border of the rect, so the frame
        // itself should be lower
        dc.GetTextExtent(label, NULL, &height);
        rectFrame.y += height / 2;
        rectFrame.height -= height / 2;

        // TODO: the +4 should be customizable

        wxRect rectText;
        rectText.x = rectFrame.x + 4;
        rectText.y = rect.y;
        rectText.width = rectFrame.width - 8;
        rectText.height = height;

        wxRect rectLabel;
        DrawLabel(dc, label, rectText, flags, alignment, indexAccel, &rectLabel);
        rectLabel.x -= 1;
        rectLabel.width += 2;

        StandardDrawFrame(dc, rectFrame, rectLabel);

        // GTK+ does it like this
        dc.SetPen(m_penHighlight);
        dc.DrawPoint(rectText.x, rectFrame.y);
        dc.DrawPoint(rectText.x + rectLabel.width - 3, rectFrame.y);
    }
    else
    {
        // just draw the complete frame
        DrawShadedRect(dc, &rectFrame, m_penDarkGrey, m_penHighlight);
        DrawShadedRect(dc, &rectFrame, m_penHighlight, m_penDarkGrey);
    }
}

// ----------------------------------------------------------------------------
// label
// ----------------------------------------------------------------------------

void wxGTKRenderer::DrawLabel(wxDC& dc,
                              const wxString& label,
                              const wxRect& rect,
                              int flags,
                              int alignment,
                              int indexAccel,
                              wxRect *rectBounds)
{
    DrawButtonLabel(dc, label, wxNullBitmap, rect, flags,
                    alignment, indexAccel, rectBounds);
}

void wxGTKRenderer::DrawButtonLabel(wxDC& dc,
                                    const wxString& label,
                                    const wxBitmap& image,
                                    const wxRect& rect,
                                    int flags,
                                    int alignment,
                                    int indexAccel,
                                    wxRect *rectBounds)
{
    if ( flags & wxCONTROL_DISABLED )
    {
        // make the text grey and draw a shade for it
        dc.SetTextForeground(*wxWHITE); // FIXME hardcoded colour
        wxRect rectShadow = rect;
        rectShadow.x++;
        rectShadow.y++;
        dc.DrawLabel(label, rectShadow, alignment, indexAccel);
        dc.SetTextForeground(wxSCHEME_COLOUR(m_scheme, CONTROL_TEXT_DISABLED));
    }

    dc.DrawLabel(label, image, rect, alignment, indexAccel, rectBounds);
}

void wxGTKRenderer::DrawItem(wxDC& dc,
                             const wxString& label,
                             const wxRect& rect,
                             int flags)
{
    wxLogTrace(_T("listbox"), _T("drawing item '%s' at (%d, %d)-(%d, %d)"),
               label.c_str(),
               rect.x, rect.y,
               rect.x + rect.width, rect.y + rect.height);

    wxColour colFg;
    if ( flags & wxCONTROL_SELECTED )
    {
        dc.SetBrush(wxBrush(wxSCHEME_COLOUR(m_scheme, HIGHLIGHT), wxSOLID));
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(rect);

        colFg = dc.GetTextForeground();
        dc.SetTextForeground(wxSCHEME_COLOUR(m_scheme, HIGHLIGHT_TEXT));
    }

    if ( flags & wxCONTROL_FOCUSED )
    {
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        wxRect rectFocus = rect;
        DrawRect(dc, &rectFocus, m_penBlack);
    }

    wxRect rectText = rect;
    rectText.x += 2;
    rectText.y++;
    dc.DrawLabel(label, wxNullBitmap, rectText);

    if ( flags & wxCONTROL_SELECTED )
    {
        dc.SetBackgroundMode(wxTRANSPARENT);
    }

    // restore the text colour
    if ( colFg.Ok() )
    {
        dc.SetTextForeground(colFg);
    }
}

void wxGTKRenderer::DrawCheckItem(wxDC& dc,
                                  const wxString& label,
                                  const wxBitmap& bitmap,
                                  const wxRect& rect,
                                  int flags)
{
    wxRect rectBitmap = rect;
    rectBitmap.x -= 1;
    rectBitmap.width = GetCheckBitmapSize().x;
    // never draw the focus rect around the check indicators here
    DrawCheckButton(dc, _T(""), bitmap, rectBitmap, flags & ~wxCONTROL_FOCUSED);

    wxRect rectLabel = rect;
    wxCoord shift = rectBitmap.width + 2*GetCheckItemMargin(); 
    rectLabel.x += shift;
    rectLabel.width -= shift;
    DrawItem(dc, label, rectLabel, flags);
}

// ----------------------------------------------------------------------------
// check/radion buttons
// ----------------------------------------------------------------------------

void wxGTKRenderer::DrawUncheckBitmap(wxDC& dc,
                                      const wxRect& rectTotal,
                                      bool isPressed)
{
    wxRect rect = rectTotal;
    DrawShadedRect(dc, &rect, m_penHighlight, m_penBlack);
    DrawAntiShadedRect(dc, &rect, m_penLightGrey, m_penDarkGrey);

    wxColour col = wxSCHEME_COLOUR(m_scheme, SHADOW_IN);
    dc.SetPen(wxPen(col, 0, wxSOLID));
    dc.DrawPoint(rect.GetRight() - 1, rect.GetBottom() - 1);

    if ( isPressed )
        col = wxSCHEME_COLOUR(m_scheme, CONTROL_PRESSED);
    //else: it is SHADOW_IN, leave as is

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(col, wxSOLID));
    dc.DrawRectangle(rect);
}

void wxGTKRenderer::DrawCheckBitmap(wxDC& dc, const wxRect& rectTotal)
{
    wxRect rect = rectTotal;
    DrawAntiShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
    DrawShadedRect(dc, &rect, m_penBlack, m_penLightGrey);

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(wxSCHEME_COLOUR(m_scheme, CONTROL_PRESSED), wxSOLID));
    dc.DrawRectangle(rect);
}

void wxGTKRenderer::DrawRadioBitmap(wxDC& dc,
                                    const wxRect& rect,
                                    int flags)
{
    wxCoord x = rect.x,
            y = rect.y,
            xRight = rect.GetRight(),
            yBottom = rect.GetBottom();

    wxCoord yMid = (y + yBottom) / 2;

    // this looks ugly when the background colour of the control is not the
    // same ours - radiobox is not transparent as it should be
#if 0
    // first fill the middle: as FloodFill() is not implemented on all
    // platforms, this is the only thing to do
    wxColour colBg = flags & wxCONTROL_CURRENT
                        ? wxSCHEME_COLOUR(m_scheme, CONTROL_CURRENT)
                        : wxSCHEME_COLOUR(m_scheme, SHADOW_IN);
    dc.SetBrush(wxBrush(colBg, wxSOLID));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect);
#endif // 0

    // then draw the upper half
    dc.SetPen(flags & wxCONTROL_CHECKED ? m_penDarkGrey : m_penHighlight);
    DrawUpZag(dc, x, xRight, yMid, y);
    DrawUpZag(dc, x + 1, xRight - 1, yMid, y + 1);

    bool drawIt = TRUE;
    if ( flags & wxCONTROL_CHECKED )
        dc.SetPen(m_penBlack);
    else if ( flags & wxCONTROL_PRESSED )
        dc.SetPen(wxPen(wxSCHEME_COLOUR(m_scheme, CONTROL_PRESSED), 0, wxSOLID));
    else // unchecked and unpressed
        drawIt = FALSE;

    if ( drawIt )
        DrawUpZag(dc, x + 2, xRight - 2, yMid, y + 2);

    // and then the lower one
    dc.SetPen(flags & wxCONTROL_CHECKED ? m_penHighlight : m_penBlack);
    DrawDownZag(dc, x, xRight, yMid, yBottom);
    if ( !(flags & wxCONTROL_CHECKED) )
        dc.SetPen(m_penDarkGrey);
    DrawDownZag(dc, x + 1, xRight - 1, yMid, yBottom - 1);

    if ( !(flags & wxCONTROL_CHECKED) )
        drawIt = TRUE; // with the same pen
    else if ( flags & wxCONTROL_PRESSED )
    {
        dc.SetPen(wxPen(wxSCHEME_COLOUR(m_scheme, CONTROL_PRESSED), 0, wxSOLID));
        drawIt = TRUE;
    }
    else // checked and unpressed
        drawIt = FALSE;

    if ( drawIt )
        DrawDownZag(dc, x + 2, xRight - 2, yMid, yBottom - 2);
}

void wxGTKRenderer::DrawUpZag(wxDC& dc,
                              wxCoord x1,
                              wxCoord x2,
                              wxCoord y1,
                              wxCoord y2)
{
    wxCoord xMid = (x1 + x2) / 2;
    dc.DrawLine(x1, y1, xMid, y2);
    dc.DrawLine(xMid, y2, x2 + 1, y1 + 1);
}

void wxGTKRenderer::DrawDownZag(wxDC& dc,
                                wxCoord x1,
                                wxCoord x2,
                                wxCoord y1,
                                wxCoord y2)
{
    wxCoord xMid = (x1 + x2) / 2;
    dc.DrawLine(x1 + 1, y1 + 1, xMid, y2);
    dc.DrawLine(xMid, y2, x2, y1);
}

wxBitmap wxGTKRenderer::GetCheckBitmap(int flags)
{
    if ( !m_bitmapsCheckbox[0][0].Ok() )
    {
        // init the bitmaps once only
        wxRect rect;
        wxSize size = GetCheckBitmapSize();
        rect.width = size.x;
        rect.height = size.y;
        for ( int i = 0; i < 2; i++ )
        {
            for ( int j = 0; j < 2; j++ )
                m_bitmapsCheckbox[i][j].Create(rect.width, rect.height);
        }

        wxMemoryDC dc;

        // normal checked
        dc.SelectObject(m_bitmapsCheckbox[0][0]);
        DrawCheckBitmap(dc, rect);

        // normal unchecked
        dc.SelectObject(m_bitmapsCheckbox[0][1]);
        DrawUncheckBitmap(dc, rect, FALSE);

        // pressed checked
        m_bitmapsCheckbox[1][0] = m_bitmapsCheckbox[0][0];

        // pressed unchecked
        dc.SelectObject(m_bitmapsCheckbox[1][1]);
        DrawUncheckBitmap(dc, rect, TRUE);
    }

    int row = flags & wxCONTROL_PRESSED ? 1 : 0;
    int col = flags & wxCONTROL_CHECKED ? 0 : 1;

    return m_bitmapsCheckbox[row][col];
}

wxBitmap wxGTKRenderer::GetLineWrapBitmap()
{
    if ( !m_bmpLineWrap.Ok() )
    {
        // the line wrap bitmap as used by GTK+
        #define line_wrap_width 6
        #define line_wrap_height 9
        static const char line_wrap_bits[] =
        {
          0x1e, 0x3e, 0x30, 0x30, 0x39, 0x1f, 0x0f, 0x0f, 0x1f,
        };

        wxBitmap bmpLineWrap(line_wrap_bits, line_wrap_width, line_wrap_height);
        if ( !bmpLineWrap.Ok() )
        {
            wxFAIL_MSG( _T("Failed to create line wrap XBM") );
        }
        else
        {
            m_bmpLineWrap = bmpLineWrap;
        }
    }

    return m_bmpLineWrap;
}

void wxGTKRenderer::DrawCheckButton(wxDC& dc,
                                    const wxString& label,
                                    const wxBitmap& bitmapOrig,
                                    const wxRect& rectTotal,
                                    int flags,
                                    wxAlignment align,
                                    int indexAccel)
{
    wxBitmap bitmap;
    if ( bitmapOrig.Ok() )
    {
        bitmap = bitmapOrig;
    }
    else
    {
        bitmap = GetCheckBitmap(flags);
    }

    DoDrawCheckOrRadioBitmap(dc, label, bitmap, rectTotal,
                             flags, align, indexAccel);
}

void wxGTKRenderer::DoDrawCheckOrRadioBitmap(wxDC& dc,
                                             const wxString& label,
                                             const wxBitmap& bitmap,
                                             const wxRect& rectTotal,
                                             int flags,
                                             wxAlignment align,
                                             int indexAccel)
{
    wxRect rect = rectTotal;

    if ( flags & wxCONTROL_FOCUSED )
    {
        // draw the focus border around everything
        DrawRect(dc, &rect, m_penBlack);
    }
    else
    {
        // the border does not offset the string under GTK
        rect.Inflate(-1);
    }

    // calculate the position of the bitmap and of the label
    wxCoord xBmp,
            yBmp = rect.y + (rect.height - bitmap.GetHeight()) / 2;

    wxRect rectLabel;
    dc.GetMultiLineTextExtent(label, NULL, &rectLabel.height);
    rectLabel.y = rect.y + (rect.height - rectLabel.height) / 2;

    if ( align == wxALIGN_RIGHT )
    {
        xBmp = rect.GetRight() - bitmap.GetWidth();
        rectLabel.x = rect.x + 2;
        rectLabel.SetRight(xBmp);
    }
    else // normal (checkbox to the left of the text) case
    {
        xBmp = rect.x + 2;
        rectLabel.x = xBmp + bitmap.GetWidth() + 4;
        rectLabel.SetRight(rect.GetRight());
    }

    dc.DrawBitmap(bitmap, xBmp, yBmp, TRUE /* use mask */);

    DrawLabel(dc, label, rectLabel, flags,
              wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL, indexAccel);
}

void wxGTKRenderer::DrawRadioButton(wxDC& dc,
                                    const wxString& label,
                                    const wxBitmap& bitmapOrig,
                                    const wxRect& rectTotal,
                                    int flags,
                                    wxAlignment align,
                                    int indexAccel)
{
    wxBitmap bitmap;
    if ( bitmapOrig.Ok() )
    {
        bitmap = bitmapOrig;
    }
    else
    {
        wxRect rect;
        wxSize size = GetRadioBitmapSize();
        rect.width = size.x;
        rect.height = size.y;
        bitmap.Create(rect.width, rect.height);
        wxMemoryDC dc;
        dc.SelectObject(bitmap);
        dc.SetBackground(*wxLIGHT_GREY_BRUSH);
        dc.Clear();
        DrawRadioBitmap(dc, rect, flags);
        bitmap.SetMask(new wxMask(bitmap, *wxLIGHT_GREY));
    }

    DoDrawCheckOrRadioBitmap(dc, label, bitmap, rectTotal,
                             flags, align, indexAccel);
}

// ----------------------------------------------------------------------------
// text control
// ----------------------------------------------------------------------------

wxRect wxGTKRenderer::GetTextTotalArea(const wxTextCtrl *text,
                                       const wxRect& rect)
{
    wxRect rectTotal = rect;
    rectTotal.Inflate(2);
    return rectTotal;
}

wxRect wxGTKRenderer::GetTextClientArea(const wxTextCtrl *text,
                                        const wxRect& rect,
                                        wxCoord *extraSpaceBeyond)
{
    wxRect rectText = rect;
    rectText.Inflate(-2);

    if ( text->WrapLines() )
    {
        // leave enough for the line wrap bitmap indicator
        wxCoord widthMark = GetLineWrapBitmap().GetWidth() + 2;

        rectText.width -= widthMark;

        if ( extraSpaceBeyond )
            *extraSpaceBeyond = widthMark;
    }

    return rectText;
}

void wxGTKRenderer::DrawTextLine(wxDC& dc,
                                 const wxString& text,
                                 const wxRect& rect,
                                 int selStart,
                                 int selEnd,
                                 int flags)
{
    // TODO: GTK+ draws selection even for unfocused controls, just with
    //       different colours
    StandardDrawTextLine(dc, text, rect, selStart, selEnd, flags);
}

void wxGTKRenderer::DrawLineWrapMark(wxDC& dc, const wxRect& rect)
{
    wxBitmap bmpLineWrap = GetLineWrapBitmap();

    // for a mono bitmap he colours it appears in depends on the current text
    // colours, so set them correctly
    wxColour colFgOld;
    if ( bmpLineWrap.GetDepth() == 1 )
    {
        colFgOld = dc.GetTextForeground();

        // FIXME: I wonder what should we do if the background is black too?
        dc.SetTextForeground(*wxBLACK);
    }

    dc.DrawBitmap(bmpLineWrap,
                  rect.x, rect.y + (rect.height - bmpLineWrap.GetHeight())/2);

    if ( colFgOld.Ok() )
    {
        // restore old colour
        dc.SetTextForeground(colFgOld);
    }
}

// ----------------------------------------------------------------------------
// combobox
// ----------------------------------------------------------------------------

void wxGTKRenderer::GetComboBitmaps(wxBitmap *bmpNormal,
                                    wxBitmap *bmpPressed,
                                    wxBitmap *bmpDisabled)
{
    // TODO
}

// ----------------------------------------------------------------------------
// background
// ----------------------------------------------------------------------------

void wxGTKRenderer::DoDrawBackground(wxDC& dc,
                                     const wxColour& col,
                                     const wxRect& rect)
{
    wxBrush brush(col, wxSOLID);
    dc.SetBrush(brush);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect);
}

void wxGTKRenderer::DrawBackground(wxDC& dc,
                                   const wxColour& col,
                                   const wxRect& rect,
                                   int flags)
{
    wxColour colBg = col.Ok() ? col : GetBackgroundColour(flags);
    DoDrawBackground(dc, colBg, rect);
}

// ----------------------------------------------------------------------------
// scrollbar
// ----------------------------------------------------------------------------

void wxGTKRenderer::DrawArrowBorder(wxDC& dc,
                                    wxRect *rect,
                                    wxDirection dir)
{
    static const wxDirection sides[] =
    {
        wxUP, wxLEFT, wxRIGHT, wxDOWN
    };

    wxRect rect1, rect2, rectInner;
    rect1 =
    rect2 =
    rectInner = *rect;

    rect2.Inflate(-1);
    rectInner.Inflate(-2);

    DoDrawBackground(dc, wxSCHEME_COLOUR(m_scheme, SCROLLBAR), *rect);

    // find the side not to draw and also adjust the rectangles to compensate
    // for it
    wxDirection sideToOmit;
    switch ( dir )
    {
        case wxUP:
            sideToOmit = wxDOWN;
            rect2.height += 1;
            rectInner.height += 1;
            break;

        case wxDOWN:
            sideToOmit = wxUP;
            rect2.y -= 1;
            rect2.height += 1;
            rectInner.y -= 2;
            rectInner.height += 1;
            break;

        case wxLEFT:
            sideToOmit = wxRIGHT;
            rect2.width += 1;
            rectInner.width += 1;
            break;

        case wxRIGHT:
            sideToOmit = wxLEFT;
            rect2.x -= 1;
            rect2.width += 1;
            rectInner.x -= 2;
            rectInner.width += 1;
            break;

        default:
            wxFAIL_MSG(_T("unknown arrow direction"));
            return;
    }

    // the outer rect first
    size_t n;
    for ( n = 0; n < WXSIZEOF(sides); n++ )
    {
        wxDirection side = sides[n];
        if ( side == sideToOmit )
            continue;

        DrawAntiShadedRectSide(dc, rect1, m_penDarkGrey, m_penHighlight, side);
    }

    // and then the inner one
    for ( n = 0; n < WXSIZEOF(sides); n++ )
    {
        wxDirection side = sides[n];
        if ( side == sideToOmit )
            continue;

        DrawAntiShadedRectSide(dc, rect2, m_penBlack, m_penGrey, side);
    }

    *rect = rectInner;
}

void wxGTKRenderer::DrawScrollbarArrow(wxDC& dc,
                                       wxDirection dir,
                                       const wxRect& rectArrow,
                                       int flags)
{
    // first of all, draw the border around it - but we don't want the border
    // on the side opposite to the arrow point
    wxRect rect = rectArrow;
    DrawArrowBorder(dc, &rect, dir);

    // then the arrow itself
    DrawArrow(dc, dir, rect, flags);
}

// gtk_default_draw_arrow() takes ~350 lines and we can't do much better here
// these people are just crazy :-(
void wxGTKRenderer::DrawArrow(wxDC& dc,
                              wxDirection dir,
                              const wxRect& rect,
                              int flags)
{
    enum
    {
        Point_First,
        Point_Second,
        Point_Third,
        Point_Max
    };

    wxPoint ptArrow[Point_Max];

    wxColour colInside = GetBackgroundColour(flags);
    wxPen penShadow[4];
    if ( flags & wxCONTROL_DISABLED )
    {
        penShadow[0] = m_penDarkGrey;
        penShadow[1] = m_penDarkGrey;
        penShadow[2] = wxNullPen;
        penShadow[3] = wxNullPen;
    }
    else if ( flags & wxCONTROL_PRESSED )
    {
        penShadow[0] = m_penDarkGrey;
        penShadow[1] = m_penHighlight;
        penShadow[2] = wxNullPen;
        penShadow[3] = m_penBlack;
    }
    else // normal arrow
    {
        penShadow[0] = m_penHighlight;
        penShadow[1] = m_penBlack;
        penShadow[2] = m_penDarkGrey;
        penShadow[3] = wxNullPen;
    }

    wxCoord middle;
    if ( dir == wxUP || dir == wxDOWN )
    {
        // horz middle
        middle = (rect.GetRight() + rect.GetLeft() + 1) / 2;
    }
    else // horz arrow
    {
        middle = (rect.GetTop() + rect.GetBottom() + 1) / 2;
    }

    // draw the arrow interior
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(colInside, wxSOLID));

    switch ( dir )
    {
        case wxUP:
            ptArrow[Point_First].x = rect.GetLeft();
            ptArrow[Point_First].y = rect.GetBottom();
            ptArrow[Point_Second].x = middle;
            ptArrow[Point_Second].y = rect.GetTop();
            ptArrow[Point_Third].x = rect.GetRight();
            ptArrow[Point_Third].y = rect.GetBottom();
            break;

        case wxDOWN:
            ptArrow[Point_First] = rect.GetPosition();
            ptArrow[Point_Second].x = middle;
            ptArrow[Point_Second].y = rect.GetBottom();
            ptArrow[Point_Third].x = rect.GetRight();
            ptArrow[Point_Third].y = rect.GetTop();
            break;

        case wxLEFT:
            ptArrow[Point_First].x = rect.GetRight();
            ptArrow[Point_First].y = rect.GetTop();
            ptArrow[Point_Second].x = rect.GetLeft();
            ptArrow[Point_Second].y = middle;
            ptArrow[Point_Third].x = rect.GetRight();
            ptArrow[Point_Third].y = rect.GetBottom();
            break;

        case wxRIGHT:
            ptArrow[Point_First] = rect.GetPosition();
            ptArrow[Point_Second].x = rect.GetRight();
            ptArrow[Point_Second].y = middle;
            ptArrow[Point_Third].x = rect.GetLeft();
            ptArrow[Point_Third].y = rect.GetBottom();
            break;

        default:
            wxFAIL_MSG(_T("unknown arrow direction"));
    }

    dc.DrawPolygon(WXSIZEOF(ptArrow), ptArrow);

    // draw the arrow border
    dc.SetPen(penShadow[0]);
    switch ( dir )
    {
        case wxUP:
            dc.DrawLine(ptArrow[Point_Second], ptArrow[Point_First]);
            dc.DrawPoint(ptArrow[Point_First]);
            if ( penShadow[3].Ok() )
            {
                dc.SetPen(penShadow[3]);
                dc.DrawLine(ptArrow[Point_First].x + 1, ptArrow[Point_First].y,
                            ptArrow[Point_Second].x, ptArrow[Point_Second].y);
            }
            dc.SetPen(penShadow[1]);
            dc.DrawLine(ptArrow[Point_Second].x + 1, ptArrow[Point_Second].y + 1,
                        ptArrow[Point_Third].x, ptArrow[Point_Third].y);
            dc.DrawPoint(ptArrow[Point_Third]);
            dc.DrawLine(ptArrow[Point_Third].x - 2, ptArrow[Point_Third].y,
                        ptArrow[Point_First].x + 1, ptArrow[Point_First].y);
            if ( penShadow[2].Ok() )
            {
                dc.SetPen(penShadow[2]);
                dc.DrawLine(ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y,
                            ptArrow[Point_Second].x, ptArrow[Point_Second].y + 1);
                dc.DrawLine(ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y - 1,
                            ptArrow[Point_First].x + 2, ptArrow[Point_First].y - 1);
            }
            break;

        case wxDOWN:
            dc.DrawLine(ptArrow[Point_First], ptArrow[Point_Second]);
            dc.DrawLine(ptArrow[Point_First].x + 2, ptArrow[Point_First].y,
                        ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y);
            if ( penShadow[2].Ok() )
            {
                dc.SetPen(penShadow[2]);
                dc.DrawLine(ptArrow[Point_Second].x, ptArrow[Point_Second].y - 1,
                            ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y - 1);
            }
            dc.SetPen(penShadow[1]);
            dc.DrawLine(ptArrow[Point_Second], ptArrow[Point_Third]);
            dc.DrawPoint(ptArrow[Point_Third]);
            break;

        case wxLEFT:
            dc.DrawLine(ptArrow[Point_Second], ptArrow[Point_First]);
            dc.DrawPoint(ptArrow[Point_First]);
            if ( penShadow[2].Ok() )
            {
                dc.SetPen(penShadow[2]);
                dc.DrawLine(ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y,
                            ptArrow[Point_First].x - 1, ptArrow[Point_First].y + 2);
                dc.DrawLine(ptArrow[Point_Third].x, ptArrow[Point_Third].y,
                            ptArrow[Point_Second].x + 2, ptArrow[Point_Second].y + 1);
            }
            dc.SetPen(penShadow[1]);
            dc.DrawLine(ptArrow[Point_Third].x, ptArrow[Point_Third].y,
                        ptArrow[Point_First].x, ptArrow[Point_First].y + 1);
            dc.DrawLine(ptArrow[Point_Second].x + 1, ptArrow[Point_Second].y + 1,
                        ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y);
            break;

        case wxRIGHT:
            dc.DrawLine(ptArrow[Point_First], ptArrow[Point_Third]);
            dc.DrawLine(ptArrow[Point_First].x + 2, ptArrow[Point_First].y + 1,
                        ptArrow[Point_Second].x, ptArrow[Point_Second].y);
            dc.SetPen(penShadow[1]);
            dc.DrawLine(ptArrow[Point_Second], ptArrow[Point_Third]);
            dc.DrawPoint(ptArrow[Point_Third]);
            break;

        default:
            wxFAIL_MSG(_T("unknown arrow direction"));
            return;
    }
}

void wxGTKRenderer::DrawThumbBorder(wxDC& dc,
                                    wxRect *rect,
                                    wxOrientation orient)
{
    if ( orient == wxVERTICAL )
    {
        DrawAntiShadedRectSide(dc, *rect, m_penDarkGrey, m_penHighlight,
                               wxLEFT);
        DrawAntiShadedRectSide(dc, *rect, m_penDarkGrey, m_penHighlight,
                               wxRIGHT);
        rect->Inflate(-1, 0);

        DrawAntiShadedRectSide(dc, *rect, m_penBlack, m_penGrey,
                               wxLEFT);
        DrawAntiShadedRectSide(dc, *rect, m_penBlack, m_penGrey,
                               wxRIGHT);
        rect->Inflate(-1, 0);
    }
    else
    {
        DrawAntiShadedRectSide(dc, *rect, m_penDarkGrey, m_penHighlight,
                               wxUP);
        DrawAntiShadedRectSide(dc, *rect, m_penDarkGrey, m_penHighlight,
                               wxDOWN);
        rect->Inflate(0, -1);

        DrawAntiShadedRectSide(dc, *rect, m_penBlack, m_penGrey,
                               wxUP);
        DrawAntiShadedRectSide(dc, *rect, m_penBlack, m_penGrey,
                               wxDOWN);
        rect->Inflate(0, -1);
    }
}

void wxGTKRenderer::DrawScrollbarThumb(wxDC& dc,
                                       wxOrientation orient,
                                       const wxRect& rect,
                                       int flags)
{
    // the thumb is never pressed never has focus border under GTK and the
    // scrollbar background never changes at all
    int flagsThumb = flags & ~(wxCONTROL_PRESSED | wxCONTROL_FOCUSED);

    // we don't want the border in the direction of the scrollbar movement
    wxRect rectThumb = rect;
    DrawThumbBorder(dc, &rectThumb, orient);

    DrawButtonBorder(dc, rectThumb, flagsThumb, &rectThumb);
    DrawBackground(dc, wxNullColour, rectThumb, flagsThumb);
}

void wxGTKRenderer::DrawScrollbarShaft(wxDC& dc,
                                       wxOrientation orient,
                                       const wxRect& rect,
                                       int flags)
{
    wxRect rectBar = rect;
    DrawThumbBorder(dc, &rectBar, orient);
    DoDrawBackground(dc, wxSCHEME_COLOUR(m_scheme, SCROLLBAR), rectBar);
}

void wxGTKRenderer::DrawScrollCorner(wxDC& dc, const wxRect& rect)
{
    DoDrawBackground(dc, wxSCHEME_COLOUR(m_scheme, CONTROL), rect);
}

wxRect wxGTKRenderer::GetScrollbarRect(const wxScrollBar *scrollbar,
                                       wxScrollBar::Element elem,
                                       int thumbPos) const
{
    // as GTK scrollbars can't be disabled, it makes no sense to remove the
    // thumb for a scrollbar with range 0 - instead, make it fill the entire
    // scrollbar shaft
    if ( (elem == wxScrollBar::Element_Thumb) && !scrollbar->GetRange() )
    {
        elem = wxScrollBar::Element_Bar_2;
    }

    return StandardGetScrollbarRect(scrollbar, elem,
                                    thumbPos,
                                    GetScrollbarArrowSize(scrollbar));
}

wxCoord wxGTKRenderer::GetScrollbarSize(const wxScrollBar *scrollbar)
{
    return StandardScrollBarSize(scrollbar, GetScrollbarArrowSize(scrollbar));
}

wxHitTest wxGTKRenderer::HitTestScrollbar(const wxScrollBar *scrollbar,
                                          const wxPoint& pt) const
{
    return StandardHitTestScrollbar(scrollbar, pt,
                                    GetScrollbarArrowSize(scrollbar));
}

wxCoord wxGTKRenderer::ScrollbarToPixel(const wxScrollBar *scrollbar,
                                        int thumbPos)
{
    return StandardScrollbarToPixel(scrollbar, thumbPos,
                                    GetScrollbarArrowSize(scrollbar));
}

int wxGTKRenderer::PixelToScrollbar(const wxScrollBar *scrollbar,
                                    wxCoord coord)
{
    return StandardPixelToScrollbar(scrollbar, coord,
                                    GetScrollbarArrowSize(scrollbar));
}

// ----------------------------------------------------------------------------
// size adjustments
// ----------------------------------------------------------------------------

void wxGTKRenderer::AdjustSize(wxSize *size, const wxWindow *window)
{
    if ( wxDynamicCast(window, wxButton) )
    {
        // TODO: this is ad hoc...
        size->x += 3*window->GetCharWidth();
        if ( size->x < 80 )
            size->x = 80;
        wxCoord minBtnHeight = 22;
        if ( size->y < minBtnHeight )
            size->y = minBtnHeight;

        // button border width
        size->y += 4;
    }
    else if ( wxDynamicCast(window, wxScrollBar) )
    {
        // we only set the width of vert scrollbars and height of the
        // horizontal ones
        if ( window->GetWindowStyle() & wxSB_HORIZONTAL )
            size->y = m_sizeScrollbarArrow.x;
        else
            size->x = m_sizeScrollbarArrow.x;
    }
    else
    {
        // take into account the border width
        wxRect rectBorder = GetBorderDimensions(window->GetBorder());
        size->x += rectBorder.x + rectBorder.width;
        size->y += rectBorder.y + rectBorder.height;
    }
}

// ============================================================================
// wxInputHandler
// ============================================================================

// ----------------------------------------------------------------------------
// wxGTKInputHandler
// ----------------------------------------------------------------------------

wxGTKInputHandler::wxGTKInputHandler(wxGTKRenderer *renderer)
{
    m_renderer = renderer;
}

bool wxGTKInputHandler::HandleKey(wxControl *control,
                                  const wxKeyEvent& event,
                                  bool pressed)
{
    return FALSE;
}

bool wxGTKInputHandler::HandleMouse(wxControl *control,
                                    const wxMouseEvent& event)
{
    // clicking on the control gives it focus
    if ( event.ButtonDown() )
    {
        control->SetFocus();

        return TRUE;
    }

    return FALSE;
}

bool wxGTKInputHandler::HandleMouseMove(wxControl *control,
                                        const wxMouseEvent& event)
{
    if ( event.Entering() )
    {
        control->SetCurrent(TRUE);
    }
    else if ( event.Leaving() )
    {
        control->SetCurrent(FALSE);
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxGTKCheckboxInputHandler
// ----------------------------------------------------------------------------

bool wxGTKCheckboxInputHandler::HandleKey(wxControl *control,
                                          const wxKeyEvent& event,
                                          bool pressed)
{
    if ( pressed )
    {
        int keycode = event.GetKeyCode();
        if ( keycode == WXK_SPACE || keycode == WXK_RETURN )
        {
            control->PerformAction(wxACTION_CHECKBOX_TOGGLE);

            return TRUE;
        }
    }

    return FALSE;
}

// ----------------------------------------------------------------------------
// wxGTKTextCtrlInputHandler
// ----------------------------------------------------------------------------

bool wxGTKTextCtrlInputHandler::HandleKey(wxControl *control,
                                          const wxKeyEvent& event,
                                          bool pressed)
{
    // handle only GTK-specific text bindings here, the others are handled in
    // the base class
    if ( pressed )
    {
        wxControlAction action;
        int keycode = event.GetKeyCode();
        if ( event.ControlDown() )
        {
            switch ( keycode )
            {
                case 'A':
                    action = wxACTION_TEXT_HOME;
                    break;

                case 'B':
                    action = wxACTION_TEXT_LEFT;
                    break;

                case 'D':
                    action << wxACTION_TEXT_PREFIX_DEL << wxACTION_TEXT_RIGHT;
                    break;

                case 'E':
                    action = wxACTION_TEXT_END;
                    break;

                case 'F':
                    action = wxACTION_TEXT_RIGHT;
                    break;

                case 'H':
                    action << wxACTION_TEXT_PREFIX_DEL << wxACTION_TEXT_LEFT;
                    break;

                case 'K':
                    action << wxACTION_TEXT_PREFIX_DEL << wxACTION_TEXT_END;
                    break;

                case 'N':
                    action = wxACTION_TEXT_DOWN;
                    break;

                case 'P':
                    action = wxACTION_TEXT_UP;
                    break;

                case 'U':
                    //delete the entire line
                    control->PerformAction(wxACTION_TEXT_HOME);
                    action << wxACTION_TEXT_PREFIX_DEL << wxACTION_TEXT_END;
                    break;

                case 'W':
                    action << wxACTION_TEXT_PREFIX_DEL << wxACTION_TEXT_WORD_LEFT;
                    break;
            }
        }
        else if ( event.AltDown() )
        {
            switch ( keycode )
            {
                case 'B':
                    action = wxACTION_TEXT_WORD_LEFT;
                    break;

                case 'D':
                    action << wxACTION_TEXT_PREFIX_DEL << wxACTION_TEXT_WORD_RIGHT;
                    break;

                case 'F':
                    action = wxACTION_TEXT_WORD_RIGHT;
                    break;
            }
        }

        if ( action != wxACTION_NONE )
        {
            control->PerformAction(action);

            return TRUE;
        }
    }

    return wxStdTextCtrlInputHandler::HandleKey(control, event, pressed);
}
