///////////////////////////////////////////////////////////////////////////////
// Name:        univ/themes/gtk.cpp
// Purpose:     wxUniversal theme implementing GTK-like LNF
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
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

    #include "wx/menu.h"

    #include "wx/bmpbuttn.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/listbox.h"
    #include "wx/checklst.h"
    #include "wx/combobox.h"
    #include "wx/scrolbar.h"
    #include "wx/slider.h"
    #include "wx/textctrl.h"
    #include "wx/toolbar.h"
    #include "wx/statusbr.h"

    #include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/notebook.h"
#include "wx/spinbutt.h"
#include "wx/toplevel.h"
#include "wx/artprov.h"
#include "wx/image.h"

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/colschem.h"
#include "wx/univ/theme.h"

class WXDLLEXPORT wxGTKMenuGeometryInfo;

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
                                int flags = 0,
                                wxWindow *window = NULL );
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

    virtual void DrawToolBarButton(wxDC& dc,
                                   const wxString& label,
                                   const wxBitmap& bitmap,
                                   const wxRect& rect,
                                   int flags = 0,
                                   long style = 0);

    virtual void DrawTextLine(wxDC& dc,
                              const wxString& text,
                              const wxRect& rect,
                              int selStart = -1,
                              int selEnd = -1,
                              int flags = 0);
    virtual void DrawLineWrapMark(wxDC& dc, const wxRect& rect);
    virtual void DrawTab(wxDC& dc,
                         const wxRect& rect,
                         wxDirection dir,
                         const wxString& label,
                         const wxBitmap& bitmap = wxNullBitmap,
                         int flags = 0,
                         int indexAccel = -1);

    virtual void DrawSliderShaft(wxDC& dc,
                                 const wxRect& rect,
                                 int lenThumb,
                                 wxOrientation orient,
                                 int flags = 0,
                                 long style = 0,
                                 wxRect *rectShaft = NULL);
    virtual void DrawSliderThumb(wxDC& dc,
                                 const wxRect& rect,
                                 wxOrientation orient,
                                 int flags = 0,
                                 long style = 0);
    virtual void DrawSliderTicks(wxDC& WXUNUSED(dc),
                                 const wxRect& WXUNUSED(rect),
                                 int WXUNUSED(lenThumb),
                                 wxOrientation WXUNUSED(orient),
                                 int WXUNUSED(start),
                                 int WXUNUSED(end),
                                 int WXUNUSED(step) = 1,
                                 int WXUNUSED(flags) = 0,
                                 long WXUNUSED(style) = 0)
    {
        // we don't have the ticks in GTK version
    }

    virtual void DrawMenuBarItem(wxDC& dc,
                                 const wxRect& rect,
                                 const wxString& label,
                                 int flags = 0,
                                 int indexAccel = -1);
    virtual void DrawMenuItem(wxDC& dc,
                              wxCoord y,
                              const wxMenuGeometryInfo& geometryInfo,
                              const wxString& label,
                              const wxString& accel,
                              const wxBitmap& bitmap = wxNullBitmap,
                              int flags = 0,
                              int indexAccel = -1);
    virtual void DrawMenuSeparator(wxDC& dc,
                                   wxCoord y,
                                   const wxMenuGeometryInfo& geomInfo);

    virtual void DrawStatusField(wxDC& dc,
                                 const wxRect& rect,
                                 const wxString& label,
                                 int flags = 0, int style = 0);

    virtual void DrawFrameTitleBar(wxDC& dc,
                                   const wxRect& rect,
                                   const wxString& title,
                                   const wxIcon& icon,
                                   int flags,
                                   int specialButton = 0,
                                   int specialButtonFlag = 0);
    virtual void DrawFrameBorder(wxDC& dc,
                                 const wxRect& rect,
                                 int flags);
    virtual void DrawFrameBackground(wxDC& dc,
                                     const wxRect& rect,
                                     int flags);
    virtual void DrawFrameTitle(wxDC& dc,
                                const wxRect& rect,
                                const wxString& title,
                                int flags);
    virtual void DrawFrameIcon(wxDC& dc,
                               const wxRect& rect,
                               const wxIcon& icon,
                               int flags);
    virtual void DrawFrameButton(wxDC& dc,
                                 wxCoord x, wxCoord y,
                                 int button,
                                 int flags = 0);

    // titlebars
    virtual wxRect GetFrameClientArea(const wxRect& rect, int flags) const;
    virtual wxSize GetFrameTotalSize(const wxSize& clientSize, int flags) const;
    virtual wxSize GetFrameMinSize(int flags) const;
    virtual wxSize GetFrameIconSize() const;
    virtual int HitTestFrame(const wxRect& rect, const wxPoint& pt, int flags) const;

    virtual void GetComboBitmaps(wxBitmap *bmpNormal,
                                 wxBitmap *bmpFocus,
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

    virtual wxSize GetToolBarButtonSize(wxCoord *separator) const
        { if ( separator ) *separator = 5; return wxSize(16, 15); }
    virtual wxSize GetToolBarMargin() const
        { return wxSize(6, 6); }

    virtual wxRect GetTextTotalArea(const wxTextCtrl *text,
                                    const wxRect& rect) const;
    virtual wxRect GetTextClientArea(const wxTextCtrl *text,
                                     const wxRect& rect,
                                     wxCoord *extraSpaceBeyond) const;

    virtual wxSize GetTabIndent() const { return wxSize(2, 2); }
    virtual wxSize GetTabPadding() const { return wxSize(6, 6); }

    virtual wxCoord GetSliderDim() const { return 15; }
    virtual wxCoord GetSliderTickLen() const { return 0; }
    virtual wxRect GetSliderShaftRect(const wxRect& rect,
                                      int lenThumb,
                                      wxOrientation orient,
                                      long style = 0) const;
    virtual wxSize GetSliderThumbSize(const wxRect& rect,
                                      int lenThumb,
                                      wxOrientation orient) const;
    virtual wxSize GetProgressBarStep() const { return wxSize(16, 32); }

    virtual wxSize GetMenuBarItemSize(const wxSize& sizeText) const;
    virtual wxMenuGeometryInfo *GetMenuGeometry(wxWindow *win,
                                                const wxMenu& menu) const;

    virtual wxSize GetStatusBarBorders(wxCoord *borderBetweenFields) const;

    // helpers for "wxBitmap wxColourScheme::Get()"
    void DrawCheckBitmap(wxDC& dc, const wxRect& rect);
    void DrawUncheckBitmap(wxDC& dc, const wxRect& rect, bool isPressed);
    void DrawUndeterminedBitmap(wxDC& dc, const wxRect& rect, bool isPressed);

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
                          const wxRect& rect,
                          wxWindow *window = NULL);

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

    // just as DrawRaisedBorder() except that the bottom left and up right
    // pixels of the interior rect are drawn in another colour (i.e. the inner
    // rect is drawn with DrawAntiShadedRect() and not DrawShadedRect())
    void DrawAntiRaisedBorder(wxDC& dc, wxRect *rect);

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
    wxBitmap GetLineWrapBitmap() const;

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

    // common part of DrawMenuItem() and DrawMenuBarItem()
    void DoDrawMenuItem(wxDC& dc,
                        const wxRect& rect,
                        const wxString& label,
                        int flags,
                        int indexAccel,
                        const wxString& accel = _T(""),
                        const wxBitmap& bitmap = wxNullBitmap,
                        const wxGTKMenuGeometryInfo *geometryInfo = NULL);

    // initialize the combo bitmaps
    void InitComboBitmaps();

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
    // pressed state and the columns are for checked, unchecked and
    // undeterminated respectively
    wxBitmap m_bitmapsCheckbox[2][3];

    // the line wrap bitmap (drawn at the end of wrapped lines)
    wxBitmap m_bmpLineWrap;

    // the combobox bitmaps
    enum
    {
        ComboState_Normal,
        ComboState_Focus,
        ComboState_Pressed,
        ComboState_Disabled,
        ComboState_Max
    };

    wxBitmap m_bitmapsCombo[ComboState_Max];
};

// ----------------------------------------------------------------------------
// wxGTKInputHandler and derived classes: process the keyboard and mouse
// messages according to GTK standards
// ----------------------------------------------------------------------------

class wxGTKInputHandler : public wxInputHandler
{
public:
    wxGTKInputHandler(wxGTKRenderer *renderer);

    virtual bool HandleKey(wxInputConsumer *control,
                           const wxKeyEvent& event,
                           bool pressed);
    virtual bool HandleMouse(wxInputConsumer *control,
                             const wxMouseEvent& event);
    virtual bool HandleMouseMove(wxInputConsumer *control, const wxMouseEvent& event);

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

    virtual bool IsAllowedButton(int WXUNUSED(button)) { return true; }

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

    virtual bool HandleKey(wxInputConsumer *control,
                           const wxKeyEvent& event,
                           bool pressed);
};

class wxGTKTextCtrlInputHandler : public wxStdTextCtrlInputHandler
{
public:
    wxGTKTextCtrlInputHandler(wxInputHandler *handler)
        : wxStdTextCtrlInputHandler(handler) { }

    virtual bool HandleKey(wxInputConsumer *control,
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
// wxGTKArtProvider
// ----------------------------------------------------------------------------

class wxGTKArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id,
                                  const wxArtClient& client,
                                  const wxSize& size);
};

// ----------------------------------------------------------------------------
// wxGTKTheme
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY_PTR(wxInputHandler *, wxArrayHandlers);

class wxGTKTheme : public wxTheme
{
public:
    wxGTKTheme();
    virtual ~wxGTKTheme();

    virtual wxRenderer *GetRenderer();
    virtual wxArtProvider *GetArtProvider();
    virtual wxInputHandler *GetInputHandler(const wxString& control);
    virtual wxColourScheme *GetColourScheme();

private:
    // get the default input handler
    wxInputHandler *GetDefaultInputHandler();

    wxGTKRenderer *m_renderer;

    wxGTKArtProvider *m_artProvider;

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
    m_scheme = NULL;
    m_renderer = NULL;
    m_handlerDefault = NULL;
    m_artProvider = NULL;
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
    wxArtProvider::RemoveProvider(m_artProvider);
}

wxRenderer *wxGTKTheme::GetRenderer()
{
    if ( !m_renderer )
    {
        m_renderer = new wxGTKRenderer(GetColourScheme());
    }

    return m_renderer;
}

wxArtProvider *wxGTKTheme::GetArtProvider()
{
    if ( !m_artProvider )
    {
        m_artProvider = new wxGTKArtProvider;
    }

    return m_artProvider;
}

wxColourScheme *wxGTKTheme::GetColourScheme()
{
    if ( !m_scheme )
    {
        m_scheme = new wxGTKColourScheme;
    }
    return m_scheme;
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
#if wxUSE_SLIDER
        else if ( control == wxINP_HANDLER_SLIDER )
            handler = new wxStdSliderButtonInputHandler(GetDefaultInputHandler());
#endif // wxUSE_SLIDER
#if wxUSE_SPINBTN
        else if ( control == wxINP_HANDLER_SPINBTN )
            handler = new wxStdSpinButtonInputHandler(GetDefaultInputHandler());
#endif // wxUSE_SPINBTN
#if wxUSE_NOTEBOOK
        else if ( control == wxINP_HANDLER_NOTEBOOK )
            handler = new wxStdNotebookInputHandler(GetDefaultInputHandler());
#endif // wxUSE_NOTEBOOK
#if wxUSE_TOOLBAR
        else if ( control == wxINP_HANDLER_TOOLBAR )
            handler = new wxStdToolbarInputHandler(GetDefaultInputHandler());
#endif // wxUSE_TOOLBAR
        else if ( control == wxINP_HANDLER_TOPLEVEL )
            handler = new wxStdFrameInputHandler(GetDefaultInputHandler());
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

    if ( !win->ShouldInheritColours() )
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
        case CONTROL_CURRENT:   return wxColour(0xeaeaea);

        case CONTROL_TEXT:      return *wxBLACK;
        case CONTROL_TEXT_DISABLED:
                                return wxColour(0x757575);
        case CONTROL_TEXT_DISABLED_SHADOW:
                                return *wxWHITE;

        case SCROLLBAR:
        case SCROLLBAR_PRESSED: return wxColour(0xc3c3c3);

        case HIGHLIGHT:         return wxColour(0x9c0000);
        case HIGHLIGHT_TEXT:    return wxColour(0xffffff);

        case GAUGE:             return Get(CONTROL_CURRENT);

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

void wxGTKRenderer::DrawAntiRaisedBorder(wxDC& dc, wxRect *rect)
{
    DrawShadedRect(dc, rect, m_penHighlight, m_penBlack);
    DrawAntiShadedRect(dc, rect, m_penLightGrey, m_penDarkGrey);
}

void wxGTKRenderer::DrawBorder(wxDC& dc,
                               wxBorder border,
                               const wxRect& rectTotal,
                               int WXUNUSED(flags),
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
            for ( width = 0; width < BORDER_THICKNESS; width++ )
            {
                DrawShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
            }
            break;

        case wxBORDER_RAISED:
            for ( width = 0; width < BORDER_THICKNESS; width++ )
            {
                DrawRaisedBorder(dc, &rect);
            }
            break;

        case wxBORDER_DOUBLE:
            for ( width = 0; width < BORDER_THICKNESS; width++ )
            {
                DrawShadedRect(dc, &rect, m_penLightGrey, m_penBlack);
                DrawShadedRect(dc, &rect, m_penHighlight, m_penDarkGrey);
                DrawRect(dc, &rect, m_penLightGrey);
            }
            break;

        case wxBORDER_SIMPLE:
            for ( width = 0; width < BORDER_THICKNESS; width++ )
            {
                DrawRect(dc, &rect, m_penBlack);
            }
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
            width = BORDER_THICKNESS;
            break;

        case wxBORDER_DOUBLE:
            width = 3*BORDER_THICKNESS;
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
    return false;
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

    if ( border != wxBORDER_NONE )
    {
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

        for ( size_t width = 0; width < BORDER_THICKNESS; width++ )
        {
            DrawAntiShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
            DrawAntiShadedRect(dc, &rect, m_penBlack, m_penDarkGrey);
        }
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
        for ( size_t width = 0; width < BORDER_THICKNESS; width++ )
        {
            DrawShadedRect(dc, &rect, m_penHighlight, m_penBlack);
            DrawAntiShadedRect(dc, &rect,
                               wxPen(GetBackgroundColour(flags), 0, wxSOLID),
                               m_penDarkGrey);
        }
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
    else
    {
        dc.SetTextForeground(wxSCHEME_COLOUR(m_scheme, CONTROL_TEXT));
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

void wxGTKRenderer::DrawUndeterminedBitmap(wxDC& dc,
                                           const wxRect& rectTotal,
                                           bool isPressed)
{
    // FIXME: For sure it is not GTK look but it is better than nothing.
    // Show me correct look and I will immediatelly make it better (ABX)
    wxRect rect = rectTotal;

    wxColour col1, col2;

    if ( isPressed )
    {
        col1 = wxSCHEME_COLOUR(m_scheme, SHADOW_DARK);
        col2 = wxSCHEME_COLOUR(m_scheme, CONTROL_PRESSED);
    }
    else
    {
        col1 = wxSCHEME_COLOUR(m_scheme, SHADOW_DARK);
        col2 = wxSCHEME_COLOUR(m_scheme, SHADOW_IN);
    }

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(col1, wxSOLID));
    dc.DrawRectangle(rect);
    rect.Deflate(1);
    dc.SetBrush(wxBrush(col2, wxSOLID));
    dc.DrawRectangle(rect);
}

void wxGTKRenderer::DrawUncheckBitmap(wxDC& dc,
                                      const wxRect& rectTotal,
                                      bool isPressed)
{
    wxRect rect = rectTotal;
    DrawAntiRaisedBorder(dc, &rect);

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

    bool drawIt = true;
    if ( flags & wxCONTROL_CHECKED )
        dc.SetPen(m_penBlack);
    else if ( flags & wxCONTROL_PRESSED )
        dc.SetPen(wxPen(wxSCHEME_COLOUR(m_scheme, CONTROL_PRESSED), 0, wxSOLID));
    else // unchecked and unpressed
        drawIt = false;

    if ( drawIt )
        DrawUpZag(dc, x + 2, xRight - 2, yMid, y + 2);

    // and then the lower one
    dc.SetPen(flags & wxCONTROL_CHECKED ? m_penHighlight : m_penBlack);
    DrawDownZag(dc, x, xRight, yMid, yBottom);
    if ( !(flags & wxCONTROL_CHECKED) )
        dc.SetPen(m_penDarkGrey);
    DrawDownZag(dc, x + 1, xRight - 1, yMid, yBottom - 1);

    if ( !(flags & wxCONTROL_CHECKED) )
        drawIt = true; // with the same pen
    else if ( flags & wxCONTROL_PRESSED )
    {
        dc.SetPen(wxPen(wxSCHEME_COLOUR(m_scheme, CONTROL_PRESSED), 0, wxSOLID));
        drawIt = true;
    }
    else // checked and unpressed
        drawIt = false;

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
            for ( int j = 0; j < 3; j++ )
                m_bitmapsCheckbox[i][j].Create(rect.width, rect.height);
        }

        wxMemoryDC dc;

        // normal checked
        dc.SelectObject(m_bitmapsCheckbox[0][0]);
        DrawCheckBitmap(dc, rect);

        // normal unchecked
        dc.SelectObject(m_bitmapsCheckbox[0][1]);
        DrawUncheckBitmap(dc, rect, false);

        // normal undeterminated
        dc.SelectObject(m_bitmapsCheckbox[0][2]);
        DrawUndeterminedBitmap(dc, rect, false);

        // pressed checked
        m_bitmapsCheckbox[1][0] = m_bitmapsCheckbox[0][0];

        // pressed unchecked
        dc.SelectObject(m_bitmapsCheckbox[1][1]);
        DrawUncheckBitmap(dc, rect, true);

        // pressed undeterminated
        dc.SelectObject(m_bitmapsCheckbox[1][2]);
        DrawUndeterminedBitmap(dc, rect, true);
    }

    int row = flags & wxCONTROL_PRESSED
                  ? 1
                  : 0;
    int col = flags & wxCONTROL_CHECKED
                  ? 0
                  : ( flags & wxCONTROL_UNDETERMINED
                          ? 2
                          : 1 );

    return m_bitmapsCheckbox[row][col];
}

wxBitmap wxGTKRenderer::GetLineWrapBitmap() const
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
            wxConstCast(this, wxGTKRenderer)->m_bmpLineWrap = bmpLineWrap;
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

    dc.DrawBitmap(bitmap, xBmp, yBmp, true /* use mask */);

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

        // must unselect the bitmap before setting a mask for it because of the
        // MSW limitations
        dc.SelectObject(wxNullBitmap);
        bitmap.SetMask(new wxMask(bitmap, *wxLIGHT_GREY));
    }

    DoDrawCheckOrRadioBitmap(dc, label, bitmap, rectTotal,
                             flags, align, indexAccel);
}

void wxGTKRenderer::DrawToolBarButton(wxDC& dc,
                                      const wxString& label,
                                      const wxBitmap& bitmap,
                                      const wxRect& rectOrig,
                                      int flags,
                                      long WXUNUSED(style))
{
    // we don't draw the separators at all
    if ( !label.empty() || bitmap.Ok() )
    {
        wxRect rect = rectOrig;
        rect.Deflate(BORDER_THICKNESS);

        if ( flags & wxCONTROL_PRESSED )
        {
            DrawBorder(dc, wxBORDER_SUNKEN, rect, flags, &rect);

            DrawBackground(dc, wxSCHEME_COLOUR(m_scheme, CONTROL_PRESSED), rect);
        }
        else if ( flags & wxCONTROL_CURRENT )
        {
            DrawBorder(dc, wxBORDER_RAISED, rect, flags, &rect);

            DrawBackground(dc, wxSCHEME_COLOUR(m_scheme, CONTROL_CURRENT), rect);
        }

        dc.DrawLabel(label, bitmap, rect, wxALIGN_CENTRE);
    }
}

// ----------------------------------------------------------------------------
// text control
// ----------------------------------------------------------------------------

wxRect wxGTKRenderer::GetTextTotalArea(const wxTextCtrl * WXUNUSED(text),
                                       const wxRect& rect) const
{
    wxRect rectTotal = rect;
    rectTotal.Inflate(2*BORDER_THICKNESS);
    return rectTotal;
}

wxRect wxGTKRenderer::GetTextClientArea(const wxTextCtrl *text,
                                        const wxRect& rect,
                                        wxCoord *extraSpaceBeyond) const
{
    wxRect rectText = rect;
    rectText.Deflate(2*BORDER_THICKNESS);

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
// notebook
// ----------------------------------------------------------------------------

void wxGTKRenderer::DrawTab(wxDC& dc,
                            const wxRect& rectOrig,
                            wxDirection dir,
                            const wxString& label,
                            const wxBitmap& bitmap,
                            int flags,
                            int indexAccel)
{
    #define SELECT_FOR_VERTICAL(X,Y) ( isVertical ? Y : X )
    #define REVERSE_FOR_VERTICAL(X,Y) \
        SELECT_FOR_VERTICAL(X,Y)      \
        ,                             \
        SELECT_FOR_VERTICAL(Y,X)

    wxRect rect = rectOrig;

    bool isVertical = ( dir == wxLEFT ) || ( dir == wxRIGHT );

    // the current tab is drawn indented (to the top for default case) and
    // bigger than the other ones
    const wxSize indent = GetTabIndent();
    if ( flags & wxCONTROL_SELECTED )
    {
        rect.Inflate( SELECT_FOR_VERTICAL( indent.x , 0),
                      SELECT_FOR_VERTICAL( 0, indent.y ));
        switch ( dir )
        {
            default:
                wxFAIL_MSG(_T("invaild notebook tab orientation"));
                // fall through

            case wxTOP:
                rect.y -= indent.y;
                // fall through
            case wxBOTTOM:
                rect.height += indent.y;
                break;

            case wxLEFT:
                rect.x -= indent.x;
                // fall through
            case wxRIGHT:
                rect.width += indent.x;
                break;
        }
    }

    // selected tab has different colour
    wxColour col = flags & wxCONTROL_SELECTED
                        ? wxSCHEME_COLOUR(m_scheme, SHADOW_IN)
                        : wxSCHEME_COLOUR(m_scheme, SCROLLBAR);
    DoDrawBackground(dc, col, rect);

    if ( flags & wxCONTROL_FOCUSED )
    {
        // draw the focus rect
        wxRect rectBorder = rect;
        rectBorder.Deflate(4, 3);
        if ( dir == wxBOTTOM )
            rectBorder.Offset(0, -1);
        if ( dir == wxRIGHT )
            rectBorder.Offset(-1, 0);

        DrawRect(dc, &rectBorder, m_penBlack);
    }

    // draw the text, image and the focus around them (if necessary)
    wxRect rectLabel( REVERSE_FOR_VERTICAL(rect.x,rect.y),
                      REVERSE_FOR_VERTICAL(rect.width,rect.height)
                    );
    rectLabel.Deflate(1, 1);
    if ( isVertical )
    {
        // draw it horizontally into memory and rotate for screen
        wxMemoryDC dcMem;
        wxBitmap bitmapRotated,
                 bitmapMem( rectLabel.x + rectLabel.width,
                            rectLabel.y + rectLabel.height );
        dcMem.SelectObject(bitmapMem);
        dcMem.SetBackground(dc.GetBackground());
        dcMem.SetFont(dc.GetFont());
        dcMem.SetTextForeground(dc.GetTextForeground());
        dcMem.Clear();
        bitmapRotated = wxBitmap( wxImage( bitmap.ConvertToImage() ).Rotate90(dir==wxLEFT) );
        dcMem.DrawLabel(label, bitmapRotated, rectLabel, wxALIGN_CENTRE, indexAccel);
        dcMem.SelectObject(wxNullBitmap);
        bitmapMem = bitmapMem.GetSubBitmap(rectLabel);
        bitmapMem = wxBitmap(wxImage(bitmapMem.ConvertToImage()).Rotate90(dir==wxRIGHT));
        dc.DrawBitmap(bitmapMem, rectLabel.y, rectLabel.x, false);
    }
    else
    {
        dc.DrawLabel(label, bitmap, rectLabel, wxALIGN_CENTRE, indexAccel);
    }

    // now draw the tab itself
    wxCoord x = SELECT_FOR_VERTICAL(rect.x,rect.y),
            y = SELECT_FOR_VERTICAL(rect.y,rect.x),
            x2 = SELECT_FOR_VERTICAL(rect.GetRight(),rect.GetBottom()),
            y2 = SELECT_FOR_VERTICAL(rect.GetBottom(),rect.GetRight());
    switch ( dir )
    {
        default:
            // default is top
        case wxLEFT:
            // left orientation looks like top but IsVertical makes x and y reversed
        case wxTOP:
            // top is not vertical so use coordinates in written order
            dc.SetPen(m_penHighlight);
            dc.DrawLine(REVERSE_FOR_VERTICAL(x, y2),
                        REVERSE_FOR_VERTICAL(x, y));
            dc.DrawLine(REVERSE_FOR_VERTICAL(x + 1, y),
                        REVERSE_FOR_VERTICAL(x2, y));

            dc.SetPen(m_penBlack);
            dc.DrawLine(REVERSE_FOR_VERTICAL(x2, y2),
                        REVERSE_FOR_VERTICAL(x2, y));

            dc.SetPen(m_penDarkGrey);
            dc.DrawLine(REVERSE_FOR_VERTICAL(x2 - 1, y2),
                        REVERSE_FOR_VERTICAL(x2 - 1, y + 1));

            if ( flags & wxCONTROL_SELECTED )
            {
                dc.SetPen(m_penLightGrey);

                // overwrite the part of the border below this tab
                dc.DrawLine(REVERSE_FOR_VERTICAL(x + 1, y2 + 1),
                            REVERSE_FOR_VERTICAL(x2 - 1, y2 + 1));

                // and the shadow of the tab to the left of us
                dc.DrawLine(REVERSE_FOR_VERTICAL(x + 1, y + 2),
                            REVERSE_FOR_VERTICAL(x + 1, y2 + 1));
            }
            break;

        case wxRIGHT:
            // right orientation looks like bottom but IsVertical makes x and y reversed
        case wxBOTTOM:
            // bottom is not vertical so use coordinates in written order
            dc.SetPen(m_penHighlight);

            // we need to continue one pixel further to overwrite the corner of
            // the border for the selected tab
            dc.DrawLine(REVERSE_FOR_VERTICAL(x, y - (flags & wxCONTROL_SELECTED ? 1 : 0)),
                        REVERSE_FOR_VERTICAL(x, y2));

            // it doesn't work like this (TODO: implement it properly)
#if 0
            // erase the corner of the tab to the right
            dc.SetPen(m_penLightGrey);
            dc.DrawPoint(REVERSE_FOR_VERTICAL(x2 - 1, y - 2));
            dc.DrawPoint(REVERSE_FOR_VERTICAL(x2 - 2, y - 2));
            dc.DrawPoint(REVERSE_FOR_VERTICAL(x2 - 2, y - 1));
#endif // 0

            dc.SetPen(m_penBlack);
            dc.DrawLine(REVERSE_FOR_VERTICAL(x + 1, y2),
                        REVERSE_FOR_VERTICAL(x2, y2));
            dc.DrawLine(REVERSE_FOR_VERTICAL(x2, y),
                        REVERSE_FOR_VERTICAL(x2, y2));

            dc.SetPen(m_penDarkGrey);
            dc.DrawLine(REVERSE_FOR_VERTICAL(x + 2, y2 - 1),
                        REVERSE_FOR_VERTICAL(x2 - 1, y2 - 1));
            dc.DrawLine(REVERSE_FOR_VERTICAL(x2 - 1, y),
                        REVERSE_FOR_VERTICAL(x2 - 1, y2));

            if ( flags & wxCONTROL_SELECTED )
            {
                dc.SetPen(m_penLightGrey);

                // overwrite the part of the (double!) border above this tab
                dc.DrawLine(REVERSE_FOR_VERTICAL(x + 1, y - 1),
                            REVERSE_FOR_VERTICAL(x2 - 1, y - 1));
                dc.DrawLine(REVERSE_FOR_VERTICAL(x + 1, y - 2),
                            REVERSE_FOR_VERTICAL(x2 - 1, y - 2));

                // and the shadow of the tab to the left of us
                dc.DrawLine(REVERSE_FOR_VERTICAL(x + 1, y2 - 1),
                            REVERSE_FOR_VERTICAL(x + 1, y - 1));
            }
            break;
    }
}

// ----------------------------------------------------------------------------
// slider
// ----------------------------------------------------------------------------

wxSize wxGTKRenderer::GetSliderThumbSize(const wxRect& rect,
                                         int lenThumb,
                                         wxOrientation orient) const
{
    static const wxCoord SLIDER_THUMB_LENGTH = 30;

    wxSize size;

    wxRect rectShaft = GetSliderShaftRect(rect, lenThumb, orient);
    if ( orient == wxHORIZONTAL )
    {
        size.x = wxMin(SLIDER_THUMB_LENGTH, rectShaft.width);
        size.y = rectShaft.height;
    }
    else // vertical
    {
        size.y = wxMin(SLIDER_THUMB_LENGTH, rectShaft.height);
        size.x = rectShaft.width;
    }

    return size;
}

wxRect wxGTKRenderer::GetSliderShaftRect(const wxRect& rect,
                                         int WXUNUSED(lenThumb),
                                         wxOrientation WXUNUSED(orient),
                                         long WXUNUSED(style)) const
{
    return rect.Deflate(2*BORDER_THICKNESS, 2*BORDER_THICKNESS);
}

void wxGTKRenderer::DrawSliderShaft(wxDC& dc,
                                    const wxRect& rectOrig,
                                    int WXUNUSED(lenThumb),
                                    wxOrientation WXUNUSED(orient),
                                    int flags,
                                    long WXUNUSED(style),
                                    wxRect *rectShaft)
{
    wxRect rect = rectOrig;

    // draw the border first
    if ( flags & wxCONTROL_FOCUSED )
    {
        DrawRect(dc, &rect, m_penBlack);
        DrawAntiShadedRect(dc, &rect, m_penBlack, m_penLightGrey);
    }
    else // not focused, normal
    {
        DrawAntiShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
        DrawAntiShadedRect(dc, &rect, m_penBlack, m_penLightGrey);
    }

    // and the background
    DoDrawBackground(dc, wxSCHEME_COLOUR(m_scheme, SCROLLBAR), rect);

    if ( rectShaft )
        *rectShaft = rect;
}

void wxGTKRenderer::DrawSliderThumb(wxDC& dc,
                                    const wxRect& rectOrig,
                                    wxOrientation orient,
                                    int WXUNUSED(flags),
                                    long WXUNUSED(style))
{
    // draw the thumb border
    wxRect rect = rectOrig;
    DrawAntiRaisedBorder(dc, &rect);

    // draw the handle in the middle
    if ( orient == wxVERTICAL )
    {
        rect.height = 2*BORDER_THICKNESS;
        rect.y = rectOrig.y + (rectOrig.height - rect.height) / 2;
    }
    else // horz
    {
        rect.width = 2*BORDER_THICKNESS;
        rect.x = rectOrig.x + (rectOrig.width - rect.width) / 2;
    }

    DrawShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
}

// ----------------------------------------------------------------------------
// menu and menubar
// ----------------------------------------------------------------------------

// wxGTKMenuGeometryInfo: the wxMenuGeometryInfo used by wxGTKRenderer
class WXDLLEXPORT wxGTKMenuGeometryInfo : public wxMenuGeometryInfo
{
public:
    virtual wxSize GetSize() const { return m_size; }

    wxCoord GetLabelOffset() const { return m_ofsLabel; }
    wxCoord GetAccelOffset() const { return m_ofsAccel; }

    wxCoord GetItemHeight() const { return m_heightItem; }

private:
    // the total size of the menu
    wxSize m_size;

    // the offset of the start of the menu item label
    wxCoord m_ofsLabel;

    // the offset of the start of the accel label
    wxCoord m_ofsAccel;

    // the height of a normal (not separator) item
    wxCoord m_heightItem;

    friend wxMenuGeometryInfo *
        wxGTKRenderer::GetMenuGeometry(wxWindow *, const wxMenu&) const;
};

// FIXME: all constants are hardcoded but shouldn't be
static const wxCoord MENU_LEFT_MARGIN = 9;
static const wxCoord MENU_RIGHT_MARGIN = 6;

static const wxCoord MENU_HORZ_MARGIN = 6;
static const wxCoord MENU_VERT_MARGIN = 3;

// the margin around bitmap/check marks (on each side)
static const wxCoord MENU_BMP_MARGIN = 2;

// the margin between the labels and accel strings
static const wxCoord MENU_ACCEL_MARGIN = 8;

// the separator height in pixels: in fact, strangely enough, the real height
// is 2 but Windows adds one extra pixel in the bottom margin, so take it into
// account here
static const wxCoord MENU_SEPARATOR_HEIGHT = 3;

// the size of the standard checkmark bitmap
static const wxCoord MENU_CHECK_SIZE = 9;

void wxGTKRenderer::DrawMenuBarItem(wxDC& dc,
                                    const wxRect& rect,
                                    const wxString& label,
                                    int flags,
                                    int indexAccel)
{
    DoDrawMenuItem(dc, rect, label, flags, indexAccel);
}

void wxGTKRenderer::DrawMenuItem(wxDC& dc,
                                 wxCoord y,
                                 const wxMenuGeometryInfo& gi,
                                 const wxString& label,
                                 const wxString& accel,
                                 const wxBitmap& bitmap,
                                 int flags,
                                 int indexAccel)
{
    const wxGTKMenuGeometryInfo& geomInfo = (const wxGTKMenuGeometryInfo&)gi;

    wxRect rect;
    rect.x = 0;
    rect.y = y;
    rect.width = geomInfo.GetSize().x;
    rect.height = geomInfo.GetItemHeight();

    DoDrawMenuItem(dc, rect, label, flags, indexAccel, accel, bitmap, &geomInfo);
}

void wxGTKRenderer::DoDrawMenuItem(wxDC& dc,
                                   const wxRect& rectOrig,
                                   const wxString& label,
                                   int flags,
                                   int indexAccel,
                                   const wxString& accel,
                                   const wxBitmap& bitmap,
                                   const wxGTKMenuGeometryInfo *geometryInfo)
{
    wxRect rect = rectOrig;

    // draw the selected item specially
    if ( flags & wxCONTROL_SELECTED )
    {
        wxRect rectIn;
        DrawBorder(dc, wxBORDER_RAISED, rect, flags, &rectIn);

        DrawBackground(dc, wxSCHEME_COLOUR(m_scheme, CONTROL_CURRENT), rectIn);
    }

    rect.Deflate(MENU_HORZ_MARGIN, MENU_VERT_MARGIN);

    // draw the bitmap: use the bitmap provided or the standard checkmark for
    // the checkable items
    if ( geometryInfo )
    {
        wxBitmap bmp = bitmap;
        if ( !bmp.Ok() && (flags & wxCONTROL_CHECKABLE) )
        {
            bmp = GetCheckBitmap(flags);
        }

        if ( bmp.Ok() )
        {
            rect.SetRight(geometryInfo->GetLabelOffset());
            wxControlRenderer::DrawBitmap(dc, bmp, rect);
        }
    }
    //else: menubar items don't have bitmaps

    // draw the label
    if ( geometryInfo )
    {
        rect.x = geometryInfo->GetLabelOffset();
        rect.SetRight(geometryInfo->GetAccelOffset());
    }

    DrawLabel(dc, label, rect, flags, wxALIGN_CENTRE_VERTICAL, indexAccel);

    // draw the accel string
    if ( !accel.empty() )
    {
        // menubar items shouldn't have them
        wxCHECK_RET( geometryInfo, _T("accel strings only valid for menus") );

        rect.x = geometryInfo->GetAccelOffset();
        rect.SetRight(geometryInfo->GetSize().x);

        // NB: no accel index here
        DrawLabel(dc, accel, rect, flags, wxALIGN_CENTRE_VERTICAL);
    }

    // draw the submenu indicator
    if ( flags & wxCONTROL_ISSUBMENU )
    {
        wxCHECK_RET( geometryInfo, _T("wxCONTROL_ISSUBMENU only valid for menus") );

        rect.x = geometryInfo->GetSize().x - MENU_RIGHT_MARGIN;
        rect.width = MENU_RIGHT_MARGIN;

        DrawArrow(dc, wxRIGHT, rect, flags);
    }
}

void wxGTKRenderer::DrawMenuSeparator(wxDC& dc,
                                      wxCoord y,
                                      const wxMenuGeometryInfo& geomInfo)
{
    DrawHorizontalLine(dc, y + MENU_VERT_MARGIN, 0, geomInfo.GetSize().x);
}

wxSize wxGTKRenderer::GetMenuBarItemSize(const wxSize& sizeText) const
{
    wxSize size = sizeText;

    // TODO: make this configurable
    size.x += 2*MENU_HORZ_MARGIN;
    size.y += 2*MENU_VERT_MARGIN;

    return size;
}

wxMenuGeometryInfo *wxGTKRenderer::GetMenuGeometry(wxWindow *win,
                                                   const wxMenu& menu) const
{
    // prepare the dc: for now we draw all the items with the system font
    wxClientDC dc(win);
    dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

    // the height of a normal item
    wxCoord heightText = dc.GetCharHeight();

    // the total height
    wxCoord height = 0;

    // the max length of label and accel strings: the menu width is the sum of
    // them, even if they're for different items (as the accels should be
    // aligned)
    //
    // the max length of the bitmap is never 0 as Windows always leaves enough
    // space for a check mark indicator
    wxCoord widthLabelMax = 0,
            widthAccelMax = 0,
            widthBmpMax = MENU_LEFT_MARGIN;

    for ( wxMenuItemList::compatibility_iterator node = menu.GetMenuItems().GetFirst();
          node;
          node = node->GetNext() )
    {
        // height of this item
        wxCoord h;

        wxMenuItem *item = node->GetData();
        if ( item->IsSeparator() )
        {
            h = MENU_SEPARATOR_HEIGHT;
        }
        else // not separator
        {
            h = heightText;

            wxCoord widthLabel;
            dc.GetTextExtent(item->GetLabel(), &widthLabel, NULL);
            if ( widthLabel > widthLabelMax )
            {
                widthLabelMax = widthLabel;
            }

            wxCoord widthAccel;
            dc.GetTextExtent(item->GetAccelString(), &widthAccel, NULL);
            if ( widthAccel > widthAccelMax )
            {
                widthAccelMax = widthAccel;
            }

            const wxBitmap& bmp = item->GetBitmap();
            if ( bmp.Ok() )
            {
                wxCoord widthBmp = bmp.GetWidth();
                if ( widthBmp > widthBmpMax )
                    widthBmpMax = widthBmp;
            }
            //else if ( item->IsCheckable() ): no need to check for this as
            // MENU_LEFT_MARGIN is big enough to show the check mark
        }

        h += 2*MENU_VERT_MARGIN;

        // remember the item position and height
        item->SetGeometry(height, h);

        height += h;
    }

    // bundle the metrics into a struct and return it
    wxGTKMenuGeometryInfo *gi = new wxGTKMenuGeometryInfo;

    gi->m_ofsLabel = widthBmpMax + 2*MENU_BMP_MARGIN;
    gi->m_ofsAccel = gi->m_ofsLabel + widthLabelMax;
    if ( widthAccelMax > 0 )
    {
        // if we actually have any accesl, add a margin
        gi->m_ofsAccel += MENU_ACCEL_MARGIN;
    }

    gi->m_heightItem = heightText + 2*MENU_VERT_MARGIN;

    gi->m_size.x = gi->m_ofsAccel + widthAccelMax + MENU_RIGHT_MARGIN;
    gi->m_size.y = height;

    return gi;
}

// ----------------------------------------------------------------------------
// status bar
// ----------------------------------------------------------------------------

wxSize
wxGTKRenderer::GetStatusBarBorders(wxCoord * WXUNUSED(borderBetweenFields)) const
{
    return wxSize(0, 0);
}

void wxGTKRenderer::DrawStatusField(wxDC& WXUNUSED(dc),
                                    const wxRect& WXUNUSED(rect),
                                    const wxString& WXUNUSED(label),
                                    int WXUNUSED(flags), int WXUNUSED(style))
{
}

// ----------------------------------------------------------------------------
// combobox
// ----------------------------------------------------------------------------

void wxGTKRenderer::InitComboBitmaps()
{
    wxSize sizeArrow = m_sizeScrollbarArrow;
    sizeArrow.x -= 2;
    sizeArrow.y -= 2;

    size_t n;

    for ( n = ComboState_Normal; n < ComboState_Max; n++ )
    {
        m_bitmapsCombo[n].Create(sizeArrow.x, sizeArrow.y);
    }

    static const int comboButtonFlags[ComboState_Max] =
    {
        0,
        wxCONTROL_CURRENT,
        wxCONTROL_PRESSED,
        wxCONTROL_DISABLED,
    };

    wxRect rect(sizeArrow);

    wxMemoryDC dc;
    for ( n = ComboState_Normal; n < ComboState_Max; n++ )
    {
        int flags = comboButtonFlags[n];

        dc.SelectObject(m_bitmapsCombo[n]);
        DoDrawBackground(dc, GetBackgroundColour(flags), rect);
        DrawArrow(dc, wxDOWN, rect, flags);
    }
}

void wxGTKRenderer::GetComboBitmaps(wxBitmap *bmpNormal,
                                    wxBitmap *bmpFocus,
                                    wxBitmap *bmpPressed,
                                    wxBitmap *bmpDisabled)
{
    if ( !m_bitmapsCombo[ComboState_Normal].Ok() )
    {
        InitComboBitmaps();
    }

    if ( bmpNormal )
        *bmpNormal = m_bitmapsCombo[ComboState_Normal];
    if ( bmpFocus )
        *bmpFocus = m_bitmapsCombo[ComboState_Focus];
    if ( bmpPressed )
        *bmpPressed = m_bitmapsCombo[ComboState_Pressed];
    if ( bmpDisabled )
        *bmpDisabled = m_bitmapsCombo[ComboState_Disabled];
}

// ----------------------------------------------------------------------------
// background
// ----------------------------------------------------------------------------

void wxGTKRenderer::DoDrawBackground(wxDC& dc,
                                     const wxColour& col,
                                     const wxRect& rect,
                                     wxWindow * WXUNUSED(window))
{
    wxBrush brush(col, wxSOLID);
    dc.SetBrush(brush);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect);
}

void wxGTKRenderer::DrawBackground(wxDC& dc,
                                   const wxColour& col,
                                   const wxRect& rect,
                                   int flags,
                                   wxWindow *window )
{
    wxColour colBg = col.Ok() ? col : GetBackgroundColour(flags);
    DoDrawBackground(dc, colBg, rect, window );
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
                                       int WXUNUSED(flags))
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
#if wxUSE_BMPBUTTON
    if ( wxDynamicCast(window, wxBitmapButton) )
    {
        size->x += 4;
        size->y += 4;
    } else
#endif // wxUSE_BMPBUTTON
#if wxUSE_BUTTON
    if ( wxDynamicCast(window, wxButton) )
    {
        if ( !(window->GetWindowStyle() & wxBU_EXACTFIT) )
        {
            // TODO: this is ad hoc...
            size->x += 3*window->GetCharWidth();
            wxCoord minBtnHeight = 18;
            if ( size->y < minBtnHeight )
                size->y = minBtnHeight;

            // button border width
            size->y += 4;
        }
    } else
#endif //wxUSE_BUTTON
    if ( wxDynamicCast(window, wxScrollBar) )
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

// ----------------------------------------------------------------------------
// top level windows
// ----------------------------------------------------------------------------

void wxGTKRenderer::DrawFrameTitleBar(wxDC& WXUNUSED(dc),
                                      const wxRect& WXUNUSED(rect),
                                      const wxString& WXUNUSED(title),
                                      const wxIcon& WXUNUSED(icon),
                                      int WXUNUSED(flags),
                                      int WXUNUSED(specialButton),
                                      int WXUNUSED(specialButtonFlag))
{
}

void wxGTKRenderer::DrawFrameBorder(wxDC& WXUNUSED(dc),
                                    const wxRect& WXUNUSED(rect),
                                    int WXUNUSED(flags))
{
}

void wxGTKRenderer::DrawFrameBackground(wxDC& WXUNUSED(dc),
                                        const wxRect& WXUNUSED(rect),
                                        int WXUNUSED(flags))
{
}

void wxGTKRenderer::DrawFrameTitle(wxDC& WXUNUSED(dc),
                                   const wxRect& WXUNUSED(rect),
                                   const wxString& WXUNUSED(title),
                                   int WXUNUSED(flags))
{
}

void wxGTKRenderer::DrawFrameIcon(wxDC& WXUNUSED(dc),
                                  const wxRect& WXUNUSED(rect),
                                  const wxIcon& WXUNUSED(icon),
                                  int WXUNUSED(flags))
{
}

void wxGTKRenderer::DrawFrameButton(wxDC& WXUNUSED(dc),
                                    wxCoord WXUNUSED(x),
                                    wxCoord WXUNUSED(y),
                                    int WXUNUSED(button),
                                    int WXUNUSED(flags))
{
}

wxRect
wxGTKRenderer::GetFrameClientArea(const wxRect& rect,
                                  int WXUNUSED(flags)) const
{
    return rect;
}

wxSize
wxGTKRenderer::GetFrameTotalSize(const wxSize& clientSize,
                                 int WXUNUSED(flags)) const
{
    return clientSize;
}

wxSize wxGTKRenderer::GetFrameMinSize(int WXUNUSED(flags)) const
{
    return wxSize(0,0);
}

wxSize wxGTKRenderer::GetFrameIconSize() const
{
    return wxSize(wxDefaultCoord, wxDefaultCoord);
}

int
wxGTKRenderer::HitTestFrame(const wxRect& WXUNUSED(rect),
                            const wxPoint& WXUNUSED(pt),
                            int WXUNUSED(flags)) const
{
    return wxHT_TOPLEVEL_CLIENT_AREA;
}


// ----------------------------------------------------------------------------
// standard icons
// ----------------------------------------------------------------------------

static const char *error_xpm[] = {
/* columns rows colors chars-per-pixel */
"48 48 537 2",
"   c Gray0",
".  c #000001010101",
"X  c #010101010101",
"o  c #010102020202",
"O  c #020202020202",
"+  c #020203030303",
"@  c #030302020202",
"#  c Gray1",
"$  c #020204040404",
"%  c #030304040404",
"&  c #070703030202",
"*  c #040404040404",
"=  c #040405050505",
"-  c Gray2",
";  c #050507070707",
":  c #060606060606",
">  c #060607070707",
",  c #070707070707",
"<  c #070709090909",
"1  c #0c0c04040303",
"2  c #0d0d04040404",
"3  c #0d0d05050404",
"4  c Gray3",
"5  c #080809090909",
"6  c #090909090909",
"7  c #0b0b0b0b0b0b",
"8  c #0a0a0d0d0d0d",
"9  c #0b0b0d0d0d0d",
"0  c #0c0c0c0c0c0c",
"q  c Gray5",
"w  c #0d0d0f0f1010",
"e  c #101006060505",
"r  c #141404040303",
"t  c #141407070606",
"y  c #171707070606",
"u  c #1d1d09090707",
"i  c #181809090808",
"p  c #1d1d09090808",
"a  c #1e1e0a0a0808",
"s  c #1e1e0b0b0909",
"d  c #101010101010",
"f  c #101011111212",
"g  c Gray7",
"h  c #131313131313",
"j  c Gray9",
"k  c #181818181818",
"l  c #191919191919",
"z  c Gray11",
"x  c #1d1d1d1d1d1d",
"c  c Gray12",
"v  c #24240b0b0a0a",
"b  c #27270d0d0b0b",
"n  c #2b2b0e0e0c0c",
"m  c #2d2d0e0e0b0b",
"M  c #30300e0e0b0b",
"N  c #33330d0d0909",
"B  c #3a3a0f0f0b0b",
"V  c #333310100e0e",
"C  c #373710100d0d",
"Z  c #373711110e0e",
"A  c #363612120f0f",
"S  c #3d3d13130f0f",
"D  c #363612121010",
"F  c Gray14",
"G  c #252525252525",
"H  c #2a2a2a2a2a2a",
"J  c Gray18",
"K  c #323232323232",
"L  c Gray20",
"P  c Gray22",
"I  c #3f3f3f3f3f3f",
"U  c #414113130e0e",
"Y  c #414113130f0f",
"T  c #404013131010",
"R  c #404014141111",
"E  c #404015151212",
"W  c #4d4d17171212",
"Q  c #4e4e18181313",
"!  c #4e4e18181414",
"~  c #4e4e19191515",
"^  c #4e4e1a1a1616",
"/  c #57571b1b1515",
"(  c #595917171010",
")  c #5b5b1a1a1313",
"_  c #58581b1b1616",
"`  c #58581c1c1717",
"'  c #5c5c1e1e1a1a",
"]  c #5c5c1f1f1b1b",
"[  c #6e6e19190f0f",
"{  c #67671c1c1616",
"}  c #6b6b1b1b1212",
"|  c #68681e1e1717",
" . c #6e6e1e1e1616",
".. c #79791e1e1515",
"X. c #666622221d1d",
"o. c #6b6b24241e1e",
"O. c #6c6c22221d1d",
"+. c #6d6d24241f1f",
"@. c #7d7d23231c1c",
"#. c #727226262020",
"$. c #757526262020",
"%. c #777728282222",
"&. c #7f7f28282121",
"*. c #484848484848",
"=. c Gray33",
"-. c #555555555555",
";. c #656565656565",
":. c Gray",
">. c #94941f1f1212",
",. c #96961f1f1111",
"<. c #98981f1f1111",
"1. c #818126261e1e",
"2. c #858523231919",
"3. c #858525251c1c",
"4. c #878728281e1e",
"5. c #898921211717",
"6. c #8a8a22221616",
"7. c #8b8b25251c1c",
"8. c #8c8c27271d1d",
"9. c #888828281f1f",
"0. c #8a8a29291f1f",
"q. c #959520201111",
"w. c #969620201111",
"e. c #949424241717",
"r. c #969624241717",
"t. c #909024241919",
"y. c #929225251919",
"u. c #929225251b1b",
"i. c #959526261b1b",
"p. c #969624241818",
"a. c #90902a2a1f1f",
"s. c #969629291f1f",
"d. c #9b9b20201313",
"f. c #999924241616",
"g. c #9c9c21211212",
"h. c #9f9f21211212",
"j. c #9d9d22221414",
"k. c #9d9d23231414",
"l. c #9c9c23231616",
"z. c #989827271b1b",
"x. c #999927271b1b",
"c. c #9a9a26261b1b",
"v. c #989827271c1c",
"b. c #9c9c25251818",
"n. c #9c9c27271b1b",
"m. c #9d9d27271b1b",
"M. c #999928281c1c",
"N. c #999929291e1e",
"B. c #9b9b28281c1c",
"V. c #9b9b28281d1d",
"C. c #9a9a29291e1e",
"Z. c #9a9a2a2a1e1e",
"A. c #9a9a2b2b1f1f",
"S. c #9b9b2a2a1f1f",
"D. c #9c9c28281c1c",
"F. c #9e9e29291f1f",
"G. c #9f9f29291e1e",
"H. c #9e9e2a2a1e1e",
"J. c #83832b2b2424",
"K. c #83832c2c2525",
"L. c #84842a2a2424",
"P. c #8b8b29292121",
"I. c #89892b2b2424",
"U. c #8b8b2c2c2626",
"Y. c #8f8f2a2a2222",
"T. c #8f8f2b2b2323",
"R. c #8d8d2e2e2828",
"E. c #8f8f2f2f2828",
"W. c #8f8f38383232",
"Q. c #919129292020",
"!. c #90902b2b2222",
"~. c #91912d2d2525",
"^. c #90902d2d2626",
"/. c #969629292020",
"(. c #95952c2c2323",
"). c #97972c2c2222",
"_. c #94942d2d2525",
"`. c #94942e2e2626",
"'. c #97972d2d2525",
"]. c #96962e2e2424",
"[. c #97972e2e2626",
"{. c #97972f2f2727",
"}. c #99992b2b2020",
"|. c #99992c2c2121",
" X c #98982d2d2323",
".X c #99992c2c2222",
"XX c #9b9b2c2c2121",
"oX c #9a9a2c2c2323",
"OX c #98982d2d2424",
"+X c #98982e2e2525",
"@X c #98982e2e2626",
"#X c #9d9d2b2b2121",
"$X c #9e9e2a2a2020",
"%X c #9c9c2c2c2121",
"&X c #9c9c2d2d2323",
"*X c #9d9d2e2e2323",
"=X c #9f9f2d2d2323",
"-X c #9e9e2e2e2020",
";X c #9f9f2e2e2323",
":X c #9c9c2d2d2424",
">X c #9d9d2f2f2525",
",X c #9c9c2f2f2626",
"<X c #9d9d2f2f2626",
"1X c #9f9f2e2e2424",
"2X c #9f9f2f2f2525",
"3X c #9f9f2f2f2626",
"4X c #939330302828",
"5X c #909036362f2f",
"6X c #949430302929",
"7X c #959530302828",
"8X c #949430302a2a",
"9X c #969630302828",
"0X c #969630302929",
"qX c #9d9d30302727",
"wX c #9e9e30302626",
"eX c #9e9e30302727",
"rX c #9e9e31312727",
"tX c #9f9f30302626",
"yX c #989831312929",
"uX c #9a9a30302929",
"iX c #9a9a31312a2a",
"pX c #9a9a32322a2a",
"aX c #9d9d31312929",
"sX c #9d9d32322929",
"dX c #9c9c32322a2a",
"fX c #9d9d32322a2a",
"gX c #9d9d33332a2a",
"hX c #9d9d33332b2b",
"jX c #9e9e31312828",
"kX c #9e9e31312929",
"lX c #9f9f31312828",
"zX c #9e9e32322929",
"xX c #9f9f32322a2a",
"cX c #9f9f33332a2a",
"vX c #9f9f33332b2b",
"bX c #9d9d3a3a3232",
"nX c #9f9f39393030",
"mX c #9f9f3e3e3636",
"MX c #a3a323231313",
"NX c #a0a022221414",
"BX c #a2a223231414",
"VX c #a0a024241616",
"CX c #a4a422221212",
"ZX c #a4a423231313",
"AX c #a5a522221212",
"SX c #a6a622221212",
"DX c #a6a622221313",
"FX c #a7a722221212",
"GX c #a4a424241515",
"HX c #a5a525251616",
"JX c #a7a724241414",
"KX c #a7a724241515",
"LX c #a6a625251717",
"PX c #a7a725251616",
"IX c #a7a725251717",
"UX c #a6a626261717",
"YX c #a0a025251818",
"TX c #a3a325251818",
"RX c #a2a226261818",
"EX c #a3a326261818",
"WX c #a2a227271a1a",
"QX c #a2a227271b1b",
"!X c #a3a327271a1a",
"~X c #a5a527271919",
"^X c #a5a527271a1a",
"/X c #a6a626261818",
"(X c #a6a627271818",
")X c #a6a627271919",
"_X c #a3a328281b1b",
"`X c #a1a128281c1c",
"'X c #a1a129291d1d",
"]X c #a1a129291e1e",
"[X c #a0a02a2a1f1f",
"{X c #a1a12a2a1f1f",
"}X c #a2a228281c1c",
"|X c #a2a229291c1c",
" o c #a3a32b2b1f1f",
".o c #a5a528281a1a",
"Xo c #a5a528281b1b",
"oo c #a5a529291b1b",
"Oo c #a4a429291c1c",
"+o c #a4a429291d1d",
"@o c #a5a529291c1c",
"#o c #a5a529291d1d",
"$o c #a4a42a2a1d1d",
"%o c #a4a42a2a1e1e",
"&o c #a4a42b2b1e1e",
"*o c #a4a42b2b1f1f",
"=o c #a9a921211010",
"-o c #a9a921211111",
";o c #a8a822221111",
":o c #a9a922221111",
">o c #a8a822221212",
",o c #a8a823231212",
"<o c #a8a823231313",
"1o c #abab23231313",
"2o c #a8a823231414",
"3o c #a9a924241313",
"4o c #a8a824241414",
"5o c #acac22221111",
"6o c #aeae22221212",
"7o c #aeae23231212",
"8o c #afaf24241313",
"9o c #a9a927271818",
"0o c #abab27271919",
"qo c #a8a829291b1b",
"wo c #abab28281a1a",
"eo c #a8a829291c1c",
"ro c #a8a82a2a1d1d",
"to c #abab29291c1c",
"yo c #adad29291b1b",
"uo c #adad2a2a1b1b",
"io c #aeae28281a1a",
"po c #adad2b2b1d1d",
"ao c #b1b123231111",
"so c #b3b323231010",
"do c #b3b323231111",
"fo c #b1b126261515",
"go c #b1b126261717",
"ho c #b2b225251414",
"jo c #b6b624241212",
"ko c #b5b525251414",
"lo c #b5b526261515",
"zo c #b4b427271717",
"xo c #b1b127271818",
"co c #b3b32a2a1a1a",
"vo c #b6b628281919",
"bo c #b7b728281919",
"no c #b5b52a2a1c1c",
"mo c #b4b42c2c1d1d",
"Mo c #b9b923231111",
"No c #bbbb25251313",
"Bo c #baba26261414",
"Vo c #bebe25251212",
"Co c #bdbd27271616",
"Zo c #baba2b2b1a1a",
"Ao c #bcbc2a2a1818",
"So c #bebe2b2b1b1b",
"Do c #bdbd2c2c1d1d",
"Fo c #a0a02a2a2020",
"Go c #a0a02b2b2020",
"Ho c #a2a22b2b2020",
"Jo c #a0a02c2c2020",
"Ko c #a0a02c2c2121",
"Lo c #a0a02d2d2222",
"Po c #a0a02d2d2323",
"Io c #a1a12d2d2222",
"Uo c #a0a02e2e2323",
"Yo c #a1a12f2f2222",
"To c #a2a22d2d2121",
"Ro c #a3a32c2c2020",
"Eo c #a3a32c2c2121",
"Wo c #a3a32d2d2121",
"Qo c #a2a22d2d2222",
"!o c #a2a22e2e2323",
"~o c #a0a02e2e2424",
"^o c #a0a02f2f2525",
"/o c #a1a12f2f2424",
"(o c #a1a12f2f2525",
")o c #a2a22e2e2424",
"_o c #a2a22f2f2424",
"`o c #a9a92f2f2020",
"'o c #aaaa2f2f2020",
"]o c #a0a031312727",
"[o c #a1a130302626",
"{o c #a1a130302727",
"}o c #a0a031312828",
"|o c #a0a032322929",
" O c #a0a032322a2a",
".O c #a1a137372d2d",
"XO c #a2a236362c2c",
"oO c #a6a636362b2b",
"OO c #a3a338382e2e",
"+O c #a7a739392f2f",
"@O c #a7a73a3a2f2f",
"#O c #abab32322424",
"$O c #abab32322525",
"%O c #aaaa33332626",
"&O c #aaaa33332727",
"*O c #abab33332626",
"=O c #aaaa34342727",
"-O c #acac30302121",
";O c #acac30302222",
":O c #acac31312323",
">O c #a9a935352929",
",O c #a9a936362a2a",
"<O c #a9a936362b2b",
"1O c #a9a937372b2b",
"2O c #aaaa34342828",
"3O c #aaaa35352929",
"4O c #a8a837372c2c",
"5O c #a9a937372c2c",
"6O c #a8a838382d2d",
"7O c #a8a838382e2e",
"8O c #a8a839392e2e",
"9O c #a9a93f3f2f2f",
"0O c #a2a23b3b3232",
"qO c #a2a23d3d3535",
"wO c #a2a23e3e3636",
"eO c #a6a63b3b3131",
"rO c #a5a53d3d3434",
"tO c #a5a53e3e3535",
"yO c #afaf3f3f3333",
"uO c #c3c325251212",
"iO c #c3c326261313",
"pO c #c3c327271414",
"aO c #c7c728281616",
"sO c #c6c62c2c1a1a",
"dO c #c7c72e2e1d1d",
"fO c #cdcd2e2e1c1c",
"gO c #cfcf2f2f1c1c",
"hO c #d0d028281313",
"jO c #d3d329291414",
"kO c #d5d529291313",
"lO c #d0d02e2e1b1b",
"zO c #d8d829291414",
"xO c #dddd2a2a1414",
"cO c #dddd2d2d1717",
"vO c #dfdf30301c1c",
"bO c #e2e22b2b1515",
"nO c #ebeb2d2d1414",
"mO c #eded2e2e1717",
"MO c #e4e431311b1b",
"NO c #e9e930301a1a",
"BO c #ebeb31311b1b",
"VO c #e8e833331d1d",
"CO c #e9e932321c1c",
"ZO c #ebeb33331e1e",
"AO c #eeee30301a1a",
"SO c #eeee32321b1b",
"DO c #eaea3a3a1b1b",
"FO c #f1f132321b1b",
"GO c #f3f331311919",
"HO c #f7f732321b1b",
"JO c #f6f636361b1b",
"KO c #f6f638381c1c",
"LO c #fcfc30301717",
"PO c #fefe33331717",
"IO c #ffff32321616",
"UO c #fdfd35351717",
"YO c #ffff34341717",
"TO c #fafa30301919",
"RO c #fafa31311818",
"EO c #fafa36361919",
"WO c #fdfd33331919",
"QO c #ffff33331818",
"!O c #fdfd35351818",
"~O c #fafa3c3c1a1a",
"^O c #fafa3d3d1a1a",
"/O c #fbfb3d3d1d1d",
"(O c #fdfd38381919",
")O c #fcfc39391a1a",
"_O c #ffff3a3a1a1a",
"`O c #ffff3f3f1b1b",
"'O c #ffff3c3c1d1d",
"]O c #ffff3e3e1d1d",
"[O c #c1c178782e2e",
"{O c #ffff40401e1e",
"}O c #d4d48d8d1d1d",
"|O c #cdcd86862222",
" + c #c1c181813838",
".+ c #c7c79d9d3737",
"X+ c #c5c59c9c3939",
"o+ c #c4c49c9c3a3a",
"O+ c #c8c89b9b3030",
"++ c #cbcb9d9d3131",
"@+ c #caca9e9e3434",
"#+ c #cccc9d9d3030",
"$+ c #cccc9f9f3232",
"%+ c #cbcba0a03737",
"&+ c #cfcfa3a33737",
"*+ c #cfcfa3a33838",
"=+ c #cfcfa5a53e3e",
"-+ c #d5d5a8a82e2e",
";+ c #dadaa8a82f2f",
":+ c #dadaacac2f2f",
">+ c #dbdbacac2e2e",
",+ c #dddda8a82a2a",
"<+ c #ddddacac2a2a",
"1+ c #dedeadad2929",
"2+ c #dfdfaeae2828",
"3+ c #dcdcadad2d2d",
"4+ c #d0d0a1a13131",
"5+ c #d1d1a2a23030",
"6+ c #d1d1a3a33333",
"7+ c #d2d2a3a33232",
"8+ c #d3d3a3a33232",
"9+ c #d3d3a4a43333",
"0+ c #d1d1a4a43636",
"q+ c #d1d1a4a43737",
"w+ c #d2d2a4a43535",
"e+ c #d2d2a4a43636",
"r+ c #d5d5a5a53333",
"t+ c #d5d5a6a63434",
"y+ c #d4d4a6a63737",
"u+ c #d6d6a7a73535",
"i+ c #d7d7a7a73434",
"p+ c #d2d2a5a53939",
"a+ c #d3d3a6a63838",
"s+ c #d3d3a6a63a3a",
"d+ c #d0d0a5a53d3d",
"f+ c #d1d1a5a53c3c",
"g+ c #d0d0a5a53f3f",
"h+ c #d8d8a7a73333",
"j+ c #d9d9a8a83232",
"k+ c #d9d9acac3232",
"l+ c #dfdfadad3636",
"z+ c #d9d9abab3e3e",
"x+ c #dadaaeae3939",
"c+ c #dbdbafaf3a3a",
"v+ c #dadaacac3f3f",
"b+ c #dbdbadad3e3e",
"n+ c #dfdfb1b13535",
"m+ c #dfdfb0b03636",
"M+ c #dcdcb0b03b3b",
"N+ c #ddddb0b03a3a",
"B+ c #dedeb1b13939",
"V+ c #e3e3afaf2222",
"C+ c #e0e0adad2525",
"Z+ c #e1e1aeae2424",
"A+ c #e4e4afaf2121",
"S+ c #e2e2b2b23232",
"D+ c #e0e0b2b23434",
"F+ c #cfcfa5a54040",
"G+ c #d3d3aaaa4747",
"H+ c #d5d5abab4343",
"J+ c #d6d6abab4242",
"K+ c #d4d4aaaa4444",
"L+ c #d2d2aaaa4848",
"P+ c #d8d8acac4040",
"I+ c #dfdfb4b44545",
"U+ c #fbfbfbfbfbfb",
"Y+ c Gray99",
"T+ c #fdfdfdfdfdfd",
"R+ c #fefefefefefe",
"E+ c Gray100",
"W+ c None",
/* pixels */
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+'O(OWOTOGOAOBONOCOZOVO  W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+]O_OQOmOjOpOBokohohofozoAosOfOgOSo  W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+)OIOxOiOdo7o1o>oAX,.q.BXPXPX9o0ocoDono    W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+`O!OnOuOao5o:o>o,oSXh.MXKXPXPXUXTX(X)Xqouoto5.  W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+^OPOzOMo5o-o:o>oFXSXw.NXKXPXIXUX/X)X)XWXXoooro#o..    W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+~OYOhOso=o-o;o>o<o<oZXd.ZXPXLXUX/X*O*O.oXooo}X$o$ov.(     W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+{OUOkOao=o:o>o>o<o2oJXKXGXj.VXHX/X)X~X=O2OooOoB.'X]XH.2.s   W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+)ObOjo=o:o>o,o<o2o-O;OPXPXHXl.f.RX^X.oXo3O>O,O%o&o[XFoN.{ w   W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+/OLOVo5o:o>o,o<o'o`oKX:O:OUX/X(XYXr.b.Xo@oOo$o<O1O oRoGo$X8.Z q   W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+EOjO6o;o>o<o<o4o>.-X:OIX#O$O)X)X.o!Xe.p.`X$o%o*o4O6OEoJoKo/.) $     W+W+W+W+W+W+",
"W+W+W+W+W+W+W+DORONo;o>oCX<.JXKXk.PXLX#O$O*O%O.oXooo_Xy.t.m.*o oRo7O8OQoLoXX@.i f   W+W+W+W+W+W+",
"W+W+W+W+W+W+W+KOcO8oFX<oDXg.KXPXPXUX/X/X*O&OyO2O@oOo$o|Xx.u.V.RoEoTo+O9OYoUo9.M 8   W+W+W+W+W+W+",
"W+W+W+W+W+W+  JOaO3o}OA+A+V+Z+Z+C+2+2+1+,+<+B+I+N+:+j+k+h+7+O+6+y+y+a+[OIo1X!.Y = 7   W+W+W+W+W+",
"W+W+W+W+W+W+  HOCo<oA+V+V+Z+Z+C+2+2+1+<+<+3+>+x+c+b+k+h+i+i+t+e+y+a+s+p+(o2X(.W % 9   W+W+W+W+W+",
"W+W+W+W+W+W+  FOloJXV+V+Z+S+C+2+1+1+<+3+3+>+;+-+5+z+v+i+u+u+y+a+a+s+f+f+[o3X]./ + < F W+W+W+W+W+",
"W+W+W+W+W+W+  SOfoKXV+Z+C+C+D+n+1+<+3+3+>+:+:+j+8+4+r+P+P+y+a+a+.+X+X+o+{owX_._ o ; z W+W+W+W+W+",
"W+W+W+W+W+W+  MOgoPXZ+C+2+2+1+l+m+3+>+>+:+j+k+h+i+9+$+w+J+H+H+s+f+f+d+g+]oeX~.! . + l W+W+W+W+W+",
"W+W+W+W+W+W+  vOxoPXC+2+2+1+<+<+B+N+>+:+j+k+#+i+i+u+0+@+&+s+K+G+d+d+=+F+}ojXI.S . + l W+W+W+W+W+",
"W+W+W+W+W+W+  lOvoUX|O1+1+<+<+3+>+N+M+j+k+h+++++u+y+y+q+%+*+f+L+L+=+F+ +|ouX&.v . + l E+W+W+W+W+",
"W+W+W+W+W+W+  boZo/XEX)X.oXooon.c.$o<O5O oRoEoA.}.!o|..X~o&X2X{o]orO|o|oaX7XO.2 o > z E+W+W+W+W+",
"W+W+W+W+W+W+  [ dOwo~X.oXoooOo$oz.i.G.oORoEoWoQoa.a._o X(o^o<XtX}o|otOcXfXU.~ X + 5 F E+W+W+W+W+",
"W+W+W+W+W+W+  4 ioyo.oQX@oOo$o%o&oM.F.C.XOKoQo!o)o4.0.OXOX{otX,XlX|ocXwOyX$.p O * q K W+W+W+W+W+",
"W+W+W+W+W+W+  q } moeo@oOo$o%o&o oRoZ.HoOO.O%X=X/ooX>X[o'.}o}oqXkXcXvXmXW.^ X + - j =.W+W+W+W+W+",
"W+W+W+W+W+W+W+  - 6.po+o'X%o&o oRoRoA.}.Qo@OPo*X~o[o[o]o[.[.|oxXzXvXhXE.X.e O + 6 J Y+W+W+W+W+W+",
"W+W+W+W+W+W+W+  X r n.$o]X{X oRoEoWoQo|.|.)oeO(o[o{o]o}o|o{.xXvXvXdX8X#.b X + o d I Y+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+  - B D.[X[XGoEoWoQo!o!o_o(oeOnXY.+X}o|o|o9X0XvXhX8X%.D X O # 6 H :.W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+    * N 7.S.GoKoKo!o)o/o(o[oOX0O5X@X|o OcXvXgXpXR.+.V X O + + j =.E+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+L   # u  .s.#XLoPo;X(o[o[o]o[.bXtOxXcXfXpX6XK.] a X O + - j *.T+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+    -   U 3.Q.).:X2X2XwXeXrXjXqOsXiX4XJ.o.E & O O + O 0 K T+E+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+      -   1 C | 1.P.T._.`.`.^.U.L.$.' A 3 X O O # - q J :.Y+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+      # - # O y m T Q _ ` ~ R n t @ O O O + O # g P :.E+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+    - # # # O O O O O O O O O O O + # + 7 x ;.T+Y+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+  G k : # - # # X X + . # # # O , g j -.:.R+E+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+    h 6 : 6 6 # # # 6 6 0 g F *.:.T+E+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+  L F c z z z c F K =.T+E+U+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+E+E+E+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+",
"W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+W+"
};

static const char *info_xpm[] = {
/* columns rows colors chars-per-pixel */
"48 48 478 2",
"   c Gray0",
".  c #010101010101",
"X  c #020202020202",
"o  c #050505050404",
"O  c Gray2",
"+  c #080808080707",
"@  c #090909090808",
"#  c #0b0b0b0b0909",
"$  c #0b0b0b0b0b0b",
"%  c #0c0c0c0c0b0b",
"&  c #0d0d0d0d0a0a",
"*  c #0c0c0c0c0c0c",
"=  c #0f0f0e0e0c0c",
"-  c #0f0f0f0f0e0e",
";  c Gray6",
":  c #101010100e0e",
">  c #121212120f0f",
",  c #101010101010",
"<  c #121212121111",
"1  c #131313131111",
"2  c #131313131313",
"3  c #141414141111",
"4  c #141414141212",
"5  c #161616161313",
"6  c #161616161414",
"7  c #1a1a1a1a1717",
"8  c #1d1d1d1d1616",
"9  c #191919191919",
"0  c #1e1e1e1e1e1e",
"q  c #20201d1d1313",
"w  c #202020201c1c",
"e  c #212121211d1d",
"r  c #212120201e1e",
"t  c #232323231f1f",
"y  c #242421211919",
"u  c Gray15",
"i  c #272727272727",
"p  c #2a2a2a2a2727",
"a  c #282828282828",
"s  c #2a2a2a2a2a2a",
"d  c #2c2c2c2c2a2a",
"f  c #2f2f2f2f2929",
"g  c #2d2d2d2d2d2d",
"h  c Gray18",
"j  c #313131312b2b",
"k  c #303030302f2f",
"l  c #333333332f2f",
"z  c #353535352e2e",
"x  c #383835352626",
"c  c #3b3b37372424",
"v  c #333333333131",
"b  c Gray20",
"n  c #343434343434",
"m  c #353535353535",
"M  c #393937373232",
"N  c #383838383737",
"B  c #3d3d3d3d3535",
"V  c Gray23",
"C  c #3e3e3e3e3e3e",
"Z  c #42423f3f3636",
"A  c #434340403232",
"S  c #424242423a3a",
"D  c #434343433d3d",
"F  c #444444443b3b",
"G  c #474744443939",
"H  c #4f4f49493838",
"J  c #4d4d49493c3c",
"K  c #434343434242",
"L  c #434343434343",
"P  c #474747474242",
"I  c #464646464444",
"U  c #464646464646",
"Y  c Gray28",
"T  c #4b4b4b4b4444",
"R  c #4d4d4c4c4747",
"E  c #4e4e4e4e4545",
"W  c #494949494949",
"Q  c #4b4b4b4b4b4b",
"!  c #4e4e4e4e4e4e",
"~  c #4f4f4f4f4e4e",
"^  c #525252524848",
"/  c #525252524949",
"(  c #555555554e4e",
")  c #565656564f4f",
"_  c #5a5a59594c4c",
"`  c #5f5f5c5c4f4f",
"'  c Gray32",
"]  c Gray33",
"[  c #585858585151",
"{  c #585858585858",
"}  c #595959595858",
"|  c #5c5c5c5c5b5b",
" . c #5f5f5e5e5959",
".. c #6d6d66664d4d",
"X. c #646461615959",
"o. c #71716d6d5b5b",
"O. c #797971715757",
"+. c #626262626262",
"@. c Gray39",
"#. c #646464646464",
"$. c #696969696464",
"%. c #6d6d6d6d6464",
"&. c #696969696868",
"*. c #6b6b6b6b6a6a",
"=. c #6d6d6d6d6d6d",
"-. c #6f6f6f6f6f6f",
";. c #73736f6f6161",
":. c #707070706b6b",
">. c #717171716f6f",
",. c #757575756c6c",
"<. c #797976766c6c",
"1. c #7f7f7a7a6464",
"2. c Gray45",
"3. c #757575757171",
"4. c #7b7b7b7b7a7a",
"5. c #7b7b7b7b7b7b",
"6. c #7f7f7f7f7b7b",
"7. c Gray49",
"8. c #818179795959",
"9. c #808079795f5f",
"0. c #8f8f82825555",
"q. c #959588885757",
"w. c #9b9b8c8c5a5a",
"e. c #929289896363",
"r. c #9d9d91916464",
"t. c #a9a999995959",
"y. c #abab9b9b5b5b",
"u. c #b3b3a0a05e5e",
"i. c #a9a9a0a07777",
"p. c #aeaea4a47575",
"a. c #b9b9a9a96464",
"s. c #babaa8a86464",
"d. c #b7b7aaaa7c7c",
"f. c #c1c1aeae6767",
"g. c #c4c4b2b26969",
"h. c #c4c4b3b36b6b",
"j. c #c6c6b3b36a6a",
"k. c #c9c9b7b76d6d",
"l. c #ccccb9b96e6e",
"z. c #c1c1b3b37272",
"x. c #c2c2b0b07171",
"c. c #c6c6b6b67777",
"v. c #cacab8b87171",
"b. c #cdcdbcbc7070",
"n. c #d1d1bcbc7070",
"m. c #d1d1bfbf7373",
"M. c #d1d1c0c07676",
"N. c #d9d9c5c57777",
"B. c #d9d9c6c67777",
"V. c #dadac7c77777",
"C. c #d9d9c6c67979",
"Z. c #dbdbc7c77c7c",
"A. c #dbdbc8c87878",
"S. c #dbdbc9c97c7c",
"D. c #dadac8c87e7e",
"F. c #dbdbc9c97e7e",
"G. c #e2e2cdcd7f7f",
"H. c #e4e4d0d07e7e",
"J. c #e4e4d0d07f7f",
"K. c #e4e4d1d17e7e",
"L. c #e6e6d2d27e7e",
"P. c #818181818181",
"I. c #838383838383",
"U. c #848484848484",
"Y. c Gray53",
"T. c #898987878282",
"R. c #8d8d8d8d8585",
"E. c #8d8d8d8d8787",
"W. c #929292928d8d",
"Q. c #969696968e8e",
"!. c #989895958585",
"~. c #9b9b95958484",
"^. c #929292929292",
"/. c #939393939393",
"(. c Gray58",
"). c Gray60",
"_. c #9d9d9d9d9999",
"`. c #9e9e9e9e9a9a",
"'. c #a2a29c9c8484",
"]. c None",
"[. c #b8b8b0b09595",
"{. c #a0a0a0a0a0a0",
"}. c Gray63",
"|. c #a5a5a5a5a5a5",
" X c #a7a7a7a7a7a7",
".X c #a9a9a9a9a4a4",
"XX c #aeaeaeaea9a9",
"oX c #babab6b6a9a9",
"OX c #b3b3b3b3b0b0",
"+X c #b3b3b3b3b1b1",
"@X c #b4b4b4b4b2b2",
"#X c #b4b4b4b4b4b4",
"$X c #b6b6b6b6b6b6",
"%X c #b9b9b9b9b1b1",
"&X c Gray73",
"*X c #bbbbbbbbbbbb",
"=X c #bcbcbcbcb9b9",
"-X c Gray75",
";X c #c1c1b9b99999",
":X c #c2c2bebeafaf",
">X c #d7d7c4c48080",
",X c #d8d8c7c78e8e",
"<X c #dfdfcbcb8080",
"1X c #dcdccbcb8484",
"2X c #d7d7c8c89696",
"3X c #c4c4c4c4b5b5",
"4X c #c8c8c8c8b9b9",
"5X c #cbcbcbcbbcbc",
"6X c #dfdfd2d2a9a9",
"7X c #e0e0cece8a8a",
"8X c #e3e3d1d18686",
"9X c #e7e7d1d18080",
"0X c #e5e5d1d18787",
"qX c #e7e7d4d48080",
"wX c #e5e5d4d48d8d",
"eX c #e6e6d6d68c8c",
"rX c #eeeed6d68181",
"tX c #efefd7d78282",
"yX c #e8e8d6d68b8b",
"uX c #e8e8d4d48d8d",
"iX c #ececd7d78c8c",
"pX c #efefd8d88181",
"aX c #efefd8d88282",
"sX c #efefd9d98686",
"dX c #eeeedbdb8b8b",
"fX c #eeeedada8d8d",
"gX c #efefdada8e8e",
"hX c #efefdcdc8b8b",
"jX c #efefdddd8f8f",
"kX c #eaeadada9191",
"lX c #e9e9dada9696",
"zX c #ededd9d99090",
"xX c #efefdddd9292",
"cX c #efefdddd9393",
"vX c #ebebdbdb9999",
"bX c #f1f1dcdc8686",
"nX c #f3f3dede8787",
"mX c #f4f4dede8787",
"MX c #f2f2dcdc8a8a",
"NX c #f3f3dfdf8d8d",
"BX c #f0f0dcdc9191",
"VX c #f1f1dcdc9292",
"CX c #f0f0dfdf9292",
"ZX c #f0f0dddd9c9c",
"AX c #f1f1dfdfa1a1",
"SX c #f1f1e0e08e8e",
"DX c #f6f6e3e38f8f",
"FX c #f7f7e5e58f8f",
"GX c #f2f2e0e09090",
"HX c #f2f2e0e09393",
"JX c #f3f3e0e09393",
"KX c #f3f3e2e29090",
"LX c #f1f1e1e19595",
"PX c #f2f2e0e09797",
"IX c #f3f3e2e29595",
"UX c #f3f3e2e29696",
"YX c #f4f4e1e19191",
"TX c #f5f5e1e19090",
"RX c #f5f5e3e39191",
"EX c #f5f5e2e29393",
"WX c #f5f5e3e39393",
"QX c #f4f4e3e39595",
"!X c #f6f6e5e59191",
"~X c #f6f6e4e49393",
"^X c #f6f6e5e59393",
"/X c #f7f7e4e49393",
"(X c #f7f7e6e69191",
")X c #f6f6e4e49696",
"_X c #f7f7e6e69595",
"`X c #f6f6e6e69797",
"'X c #f1f1e1e19b9b",
"]X c #f2f2e2e29a9a",
"[X c #f4f4e2e29898",
"{X c #f6f6e4e49898",
"}X c #f6f6e4e49999",
"|X c #f6f6e5e59b9b",
" o c #f4f4e4e49d9d",
".o c #f5f5e4e49c9c",
"Xo c #f6f6e5e59d9d",
"oo c #f7f7e6e69f9f",
"Oo c #f8f8e6e69191",
"+o c #f8f8e7e79191",
"@o c #f8f8e7e79393",
"#o c #f8f8e7e79595",
"$o c #f8f8e7e79696",
"%o c #f9f9e8e89b9b",
"&o c #f9f9e9e99b9b",
"*o c #f9f9e8e89e9e",
"=o c #edede7e7baba",
"-o c #f1f1e3e3a4a4",
";o c #f3f3e3e3a7a7",
":o c #f2f2e4e4a1a1",
">o c #f3f3e4e4a1a1",
",o c #f3f3e6e6a3a3",
"<o c #f4f4e5e5a0a0",
"1o c #f5f5e4e4a3a3",
"2o c #f5f5e7e7a2a2",
"3o c #f6f6e6e6a0a0",
"4o c #f7f7e7e7a0a0",
"5o c #f7f7e7e7a5a5",
"6o c #f6f6e7e7a7a7",
"7o c #f1f1e6e6adad",
"8o c #f3f3e6e6adad",
"9o c #f3f3e7e7adad",
"0o c #f7f7e8e8a6a6",
"qo c #f7f7e8e8a7a7",
"wo c #f7f7eaeaafaf",
"eo c #f9f9e9e9a7a7",
"ro c #fafaeaeaa6a6",
"to c #f8f8e8e8a9a9",
"yo c #f8f8eaeaaeae",
"uo c #f8f8eaeaafaf",
"io c #fafaececafaf",
"po c #f4f4e7e7b2b2",
"ao c #f5f5e7e7b2b2",
"so c #f4f4e7e7bbbb",
"do c #f7f7ebebb1b1",
"fo c #f4f4e9e9bbbb",
"go c #f6f6ebebbebe",
"ho c #f8f8ebebb1b1",
"jo c #f9f9ebebb1b1",
"ko c #fafaededb7b7",
"lo c #f8f8ececb8b8",
"zo c #f8f8ececb9b9",
"xo c #f8f8ededbaba",
"co c #fafaeeeebbbb",
"vo c #c2c2c2c2c1c1",
"bo c #c3c3c3c3c1c1",
"no c Gray76",
"mo c #c3c3c3c3c2c2",
"Mo c #c7c7c7c7c5c5",
"No c #cacacacac4c4",
"Bo c Gray79",
"Vo c #cacacacacaca",
"Co c #cbcbcbcbcbcb",
"Zo c #cfcfcfcfc8c8",
"Ao c Gray80",
"So c #cfcfcfcfcece",
"Do c #d8d8d8d8cdcd",
"Fo c #ddddd9d9caca",
"Go c #d4d4d4d4d1d1",
"Ho c #d7d7d7d7d3d3",
"Jo c #d5d5d5d5d5d5",
"Ko c Gray85",
"Lo c #dadadadad8d8",
"Po c #dadadadadada",
"Io c gainsboro",
"Uo c #dddddddddddd",
"Yo c #dededededddd",
"To c #dfdfdfdfdede",
"Ro c #e0e0e0e0d7d7",
"Eo c #e5e5e5e5dddd",
"Wo c #f3f3eaeac1c1",
"Qo c #f4f4e9e9c1c1",
"!o c #f5f5e9e9c2c2",
"~o c #f4f4e9e9c4c4",
"^o c #f7f7eeeec3c3",
"/o c #f4f4eaeac9c9",
"(o c #f4f4eaeacaca",
")o c #f9f9efefcbcb",
"_o c #f8f8efefcfcf",
"`o c #f7f7efefd0d0",
"'o c #f7f7eeeed9d9",
"]o c #f7f7efefd8d8",
"[o c #f9f9f0f0c7c7",
"{o c #fafaf3f3d0d0",
"}o c #fafaf2f2d2d2",
"|o c #fafaf2f2d3d3",
" O c #f8f8f1f1dbdb",
".O c #f9f9f1f1dbdb",
"XO c #f8f8f2f2dbdb",
"oO c #f9f9f2f2dbdb",
"OO c #fafaf3f3dada",
"+O c #fafaf3f3dbdb",
"@O c #f8f8f1f1dcdc",
"#O c #f8f8f2f2dede",
"$O c #fafaf4f4d9d9",
"%O c #fafaf5f5dede",
"&O c #e7e7e7e7e1e1",
"*O c #e4e4e4e4e4e4",
"=O c #e9e9e9e9e2e2",
"-O c #ebebebebe6e6",
";O c Gray91",
":O c #edededede8e8",
">O c #ecececececec",
",O c Gray93",
"<O c #efefefefeeee",
"1O c #efefefefefef",
"2O c #f1f1f1f1ecec",
"3O c #f2f2f2f2eeee",
"4O c #fafaf3f3e5e5",
"5O c #f9f9f4f4e0e0",
"6O c #f9f9f4f4e2e2",
"7O c #f9f9f4f4e3e3",
"8O c #f9f9f5f5e3e3",
"9O c #fafaf4f4e1e1",
"0O c #f9f9f4f4e5e5",
"qO c #f9f9f5f5e4e4",
"wO c #f9f9f5f5e5e5",
"eO c #f9f9f4f4e6e6",
"rO c #f9f9f4f4e7e7",
"tO c #f9f9f5f5e6e6",
"yO c #f9f9f6f6e5e5",
"uO c #f9f9f6f6e7e7",
"iO c #fafaf5f5e4e4",
"pO c #fafaf5f5e5e5",
"aO c #fafaf5f5e6e6",
"sO c #fafaf5f5e7e7",
"dO c #fbfbf5f5e6e6",
"fO c #fafaf6f6e6e6",
"gO c #fafaf6f6e7e7",
"hO c #fbfbf6f6e7e7",
"jO c #f9f9f6f6eaea",
"kO c #fafaf5f5e8e8",
"lO c #fafaf6f6e8e8",
"zO c #fafaf6f6e9e9",
"xO c #fafaf7f7eaea",
"cO c #fafaf7f7ebeb",
"vO c #fbfbf7f7eaea",
"bO c #fbfbf7f7ecec",
"nO c #fbfbf7f7eded",
"mO c #fafaf8f8ecec",
"MO c #fbfbf8f8eded",
"NO c #fbfbf8f8eeee",
"BO c #fbfbf9f9efef",
"VO c Gray94",
"CO c #f1f1f1f1f1f1",
"ZO c #f4f4f4f4f0f0",
"AO c #f4f4f4f4f1f1",
"SO c #f5f5f5f5f2f2",
"DO c #f6f6f6f6f3f3",
"FO c #f4f4f4f4f4f4",
"GO c Gray96",
"HO c #f6f6f6f6f4f4",
"JO c #f7f7f7f7f5f5",
"KO c #f6f6f6f6f6f6",
"LO c Gray97",
"PO c #f8f8f8f8f5f5",
"IO c #f8f8f8f8f6f6",
"UO c #f8f8f8f8f7f7",
"YO c #f9f9f9f9f7f7",
"TO c #fafaf9f9f4f4",
"RO c #fafafafaf6f6",
"EO c #fbfbfbfbf7f7",
"WO c #fefefcfcf0f0",
"QO c #fefefcfcf1f1",
"!O c #fefefcfcf3f3",
"~O c #fefefcfcf4f4",
"^O c #fefefcfcf5f5",
"/O c #fefefdfdf5f5",
"(O c #fefefdfdf6f6",
")O c #fefefdfdf7f7",
"_O c #f9f9f9f9f8f8",
"`O c #f9f9f9f9f9f9",
"'O c #fafafafaf8f8",
"]O c #fafafafaf9f9",
"[O c #fbfbfbfbf8f8",
"{O c #fbfbfbfbf9f9",
"}O c Gray98",
"|O c #fbfbfbfbfafa",
" + c #fbfbfbfbfbfb",
".+ c #fcfcfcfcf9f9",
"X+ c #fcfcfcfcfafa",
"o+ c #fcfcfcfcfbfb",
"O+ c #fefefdfdf8f8",
"++ c #fefefdfdf9f9",
"@+ c #fefefdfdfafa",
"#+ c #fefefdfdfbfb",
"$+ c #fefefefefafa",
"%+ c #fefefefefbfb",
"&+ c Gray99",
"*+ c #fdfdfdfdfcfc",
"=+ c #fdfdfdfdfdfd",
"-+ c #fefefefefcfc",
";+ c #fefefefefdfd",
":+ c #fefefefefefe",
">+ c None",
",+ c None",
/* pixels */
",+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+>+>+>+>+>+>+>+>+>+>+>+>+>+>+>+>+>+>+>+>+>+,+,+,+,+,+,+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+>+>+>+>+>+>+>+>+>+>+>+>+>+>+>+>+(O>+>+>+>+>+>+>+>+,+,+,+,+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+>+>+>+>+>+>+>+QOO+>+>+>+>+>+>+>+++!OO+>+>+>+>+>+>+>+>+,+,+,+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+,+,+>+>+>+>+>+>+>+++/O$+>+>+>+>+>+>+>+(O(O/O>+>+>+>+>+>+>+>+>+,+,+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+>+>+>+>+>+>+>+>+>+$+)OO+>+>+>+>+>+>+%+$+++O+>+>+>+>+>+>+>+>+>+>+>+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+>+>+>+>+>+>+>+>+>+O+++++-+>+>+>+>+;+-+;+-+@+>+>+>+>+>+>+)O>+>+>+>+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+>+>+>+>+>+>+>+>+>+>+@+;+;+-+>+>+>+>+;+:+:+;+%+>+>+>+>+%+/O~O>+>+>+>+>+,+",
",+,+,+,+,+,+,+,+,+,+,+>+>+>+>+>+>+>+>+>+>+>+-+;+:+:+:+>+>+:+:+:+:+:+:+>+;+-+++++)O$+>+>+>+>+>+>+",
",+,+,+,+,+,+,+,+,+,+>+>+>+/OQO++>+>+>+>+>+>+;+:+:+:+:+:+:+:+:+:+:+:+:+:+;+-+-+;+++%+>+>+>+>+>+>+",
",+,+,+,+,+,+,+,+,+,+>+>+>+)O~O/O)O@+;+>+>+>+:+:+:+:+:+:+:+:+:+:+:+:+:+:+:+:+:+;+@+>+>+>+>+>+>+>+",
",+,+,+,+,+,+,+,+,+>+>+>+>+>+$+)O#+;+;+;+:+:+:+:+:+:+COVoCoCo,O:+:+:+:+:+:+:+:+;+;+>+>+>+>+>+>+>+",
",+,+,+,+,+,+,+,+,+>+>+>+>+>+>+++-+:+:+:+:+:+:+,O).Q 2 h M Z y a +.no}O:+:+:+:+:+>+>+>+>+>+>+>+>+",
",+,+,+,+,+,+,+,+>+>+>+>+>+>+>+>+-+:+:+:+:+:+nog ;.7XjX[XYXJXiX,Xd.w.` $X:+:+:+:+>+>+>+>+>+>+>+>+",
",+,+,+,+,+,+,+,+>+>+>+>+>+>+>+>+;+;+:+:+:+}.0 :XVXTX5oyoeo#o$o.ovXG.>Xr I.`O:+:+>+>+>+>+>+>+>+>+",
",+,+,+,+,+,+,+,+>+>+>+>+>+>+>+>+>+:+:+:+no0 FosXgXuo[o%O{oro*o@o/XcX0X6X} /.:+:+:+:+;+;+%+@+/OQO",
",+,+,+,+,+,+,+>+>+>+(O!O(OO+@+-+;+:+:+}OY oX1omXpo8OfOMOBOmOOO&o+oDXxX<X2Xb Bo:+:+:+:+;+-+O+(O>+",
",+,+,+,+,+,+,+>+>+>+WO~O)O@+-+;+:+:+:+#X] ZXNXoo_opO6OhOsOzOfOko%oOoRXyXn.'.' :+:+:+:+;+%+%+>+>+",
",+,+,+,+,+,+,+>+>+>+>+++)O$+;+:+:+:+:+U.!.rXEXto9OgO6OqO6OtOsO^o4o(XFXSXA.c.p Po:+:+:+;+;+>+>+>+",
",+,+,+,+,+,+,+>+>+>+>+>+>+-+%+;+:+:+:+#.;XaX}XxoiOpOdOaO.O7OXOgo3oOoOoKXH.f.X.{.:+:+:+:+>+>+>+>+",
",+,+,+,+,+,+,+>+>+>+>+>+>+>+>+:+:+:+:+#.;XpX{XxoqOlOlOwO6O#O`o!o<o_X_XIXL.j.<.5.:+:+>+>+>+>+>+>+",
",+,+,+,+,+,+,+>+>+>+>+>+>+>+>+>+:+:+:+@.[.pXEXxosONOcOyOjO5O/odo2o`X|XHXqXl.O.(.:+>+>+>+>+>+>+>+",
",+,+,+,+,+,+,+>+>+>+>+>+>+>+>+;+:+:+:+P.T.BX)XwopObOuOkOnOcolo(o,o^X~XGXK.g...|.:+:+>+>+>+>+>+>+",
",+,+,+,+,+,+,+>+>+>+>+>+>+>+>+-+:+:+:+-XU zXMX0o$ONO OrO)oioao=o]X!XWXhXB.y.q Jo:+:+:+>+>+>+>+>+",
",+,+,+,+,+,+,+>+>+>+>+>+>+>+EOX+&+&+*+`O-.~.bXXo|ovO0O4Ozo6o9o-oQXRXkXJ.b.8.! `O:+:+:+;+>+>+>+>+",
",+,+,+,+,+,+,+>+>+>+>+>+>+++[OX+&+&+*+*+YoC fXqo}oxOeO'o7o o:o'XCXwXF.N.a.H  X:+:+:+:+;+-+>+>+>+",
",+,+,+,+,+,+,+>+>+>+>+++(O++[O|Oo+&+*+*+=+u PXjo+O@O]o~o>oUXLXlXeXD.V.h.q.m FO:+:+:+:+;+$+%+>+>+",
",+,+,+,+,+,+,+,+>+>+>+O+(OO+TO[O.+o+&+*+*OA nXhooOWoQo8o'XjXdX1XM.m.k.r.D }.:+:+:+:+;+-+++O+$+>+",
",+,+,+,+,+,+,+,+>+>+>+)O++%+ROo+{OX+o+&+voJ tXfosoAX;oPXjX8XC.v.z.p.1.$ 7.}O:+:+:+;+-+@+O+)O^O>+",
",+,+,+,+,+,+,+,+>+>+>+>+>+>+'O|Oo+*+=+=+%.< x  ._ i.uX9XZ.S.s.0.c = W *X:+:+>+>+>+>+>+>+>+>+>+>+",
",+,+,+,+,+,+,+,+,+>+>+>+>+>+_O]Oo+&+*+=+z _.@X<OLo$.8 e.x.u.t.G +.IoGO:+:+:+>+>+>+>+>+>+>+>+>+>+",
",+,+,+,+,+,+,+,+,+>+>+>+>+>+YO'O|Oo+&+`Oo 6 ( R.GoKONo[ R 9.o.s Ko*+*+:+:+:+>+>+>+>+>+>+>+>+>+>+",
",+,+,+,+,+,+,+,+,+,+>+>+>+>+IOYO]O|Oo+{ t Mo.XB 1 E.bo4.| & V &X*+*+*+:+:+;+>+>+>+>+>+>+>+>+>+>+",
",+,+,+,+,+,+,+,+,+,+>+>+>+>+HOIOYOVO|O9 ^ Ho=+LOZof P >.3.; >O&+&+&+&+:+:+-+>+>+>+>+>+>+>+>+>+>+",
",+,+,+,+,+,+,+,+,+,+,+>+>+>+AOHOIO>OVO  + f Q.To ++X:.~ S L =+=+=+o+o+:+:+%+>+>+>+>+>+>+>+>+>+>+",
",+,+,+,+,+,+,+,+,+,+,+,+>+>+3OAOHO;O9   =XXXT - W.^.-.&.4 Y.*+*+*+X+[O;+;+@+>+>+>+>+>+>+>+>+>+,+",
",+,+,+,+,+,+,+,+,+,+,+,+>+>+:O3OAOHO  ,.moVOVO%X/ I 6.=.O LO&+o+o+o+'O++@+O+%+>+>+>+>+>+>+>+>+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+>+Eo:O3OAO    F v OX1OSo&.K ) i  + +|O|O|O]O>+%+(O@+>+>+>+>+>+>+>+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+,+5XEo-O2O*   5 > f d `.-.*.  n ]O]O]O'O'O_O>+>+++O+>+>+>+>+>+>+,+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+,+].5XRo-OAo    : e w N &.2.  UOYOYOYOUOIOPO>+>+>+>+>+>+>+>+>+,+,+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+,+].].].Do&O3 @ 5 1 # j l k , JOJOJOHOHODOSO>+>+>+>+>+>+>+,+,+,+,+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+,+].].].].5XE . 3 7 % :   PoUoAOAOAOAOZO3O3O>+>+>+>+>+>+,+,+,+,+,+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+,+].].].].].].X X . X * -O:O2O2O2O2O2O:O-O-O>+>+>+>+,+,+,+,+,+,+,+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+,+].].].].].].].].3X4XDoRoRo=O=O=O=O=ORoRoDo>+>+,+,+,+,+,+,+,+,+,+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+,+].].].].].].].].].].].].].5X5X5X5X5X].].].,+,+,+,+,+,+,+,+,+,+,+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+,+].].].].].].].].].].].].].].].].].].].].].,+,+,+,+,+,+,+,+,+,+,+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+,+].].].].].].].].].].].].].].].].].].].].].,+,+,+,+,+,+,+,+,+,+,+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+",
",+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+,+"
};

/* XPM */
static const char *warning_xpm[] = {
/* columns rows colors chars-per-pixel */
"48 48 270 2",
"   c Gray0",
".  c #010100000000",
"X  c #010101010000",
"o  c #010101010101",
"O  c #020201010000",
"+  c #030301010000",
"@  c #020202020000",
"#  c #020202020202",
"$  c Gray1",
"%  c #040403030000",
"&  c #050504040000",
"*  c #070704040000",
"=  c #040404040404",
"-  c Gray2",
";  c #060606060606",
":  c #090907070000",
">  c #090907070101",
",  c #0e0e03030202",
"<  c #0d0d04040303",
"1  c #0a0a08080000",
"2  c #0b0b09090000",
"3  c #0e0e0b0b0000",
"4  c Gray3",
"5  c #090909090909",
"6  c Gray4",
"7  c #0b0b0b0b0b0b",
"8  c Gray6",
"9  c #171704040202",
"0  c #10100d0d0101",
"q  c #13130f0f0000",
"w  c #13130f0f0101",
"e  c #1c1c07070505",
"r  c #151510100101",
"t  c #191913130000",
"y  c #1d1d16160202",
"u  c #1e1e17170202",
"i  c #111111111111",
"p  c #161616161616",
"a  c #212107070505",
"s  c #222207070505",
"d  c #232307070404",
"f  c #232307070505",
"g  c #262608080606",
"h  c #2b2b0a0a0707",
"j  c #2c2c08080505",
"k  c #2e2e08080505",
"l  c #2e2e09090606",
"z  c #2e2e0a0a0808",
"x  c #24241c1c0303",
"c  c #25251d1d0202",
"v  c #25251d1d0303",
"b  c #27271e1e0202",
"n  c #3b3b0b0b0707",
"m  c #3b3b0c0c0909",
"M  c #3c3c0c0c0909",
"N  c #3d3d0c0c0909",
"B  c #3e3e0c0c0808",
"V  c #292920200303",
"C  c #2c2c23230303",
"Z  c #313126260404",
"A  c #313126260505",
"S  c #333327270404",
"D  c #38382c2c0505",
"F  c #3c3c2e2e0505",
"G  c Gray17",
"H  c #41410c0c0707",
"J  c #42420c0c0606",
"K  c #42420c0c0707",
"L  c #42420d0d0808",
"P  c #44440e0e0909",
"I  c #44440e0e0a0a",
"U  c #47470e0e0909",
"Y  c #46460e0e0a0a",
"T  c #49490d0d0707",
"R  c #4d4d0d0d0707",
"E  c #49490e0e0909",
"W  c #49490e0e0a0a",
"Q  c #4d4d10100c0c",
"!  c #52520e0e0707",
"~  c #575711110909",
"^  c #5a5a12120d0d",
"/  c #5d5d11110b0b",
"(  c #5e5e11110a0a",
")  c #5c5c12120d0d",
"_  c #5e5e12120c0c",
"`  c #404031310404",
"'  c #404031310505",
"]  c #414132320606",
"[  c #424233330505",
"{  c #454535350606",
"}  c #4b4b3a3a0707",
"|  c #4e4e3d3d0606",
" . c #51513f3f0707",
".. c #606012120b0b",
"X. c #636311110909",
"o. c #616113130e0e",
"O. c #646412120909",
"+. c #6a6a13130b0b",
"@. c #6e6e13130a0a",
"#. c #6e6e14140a0a",
"$. c #6f6f14140b0b",
"%. c #6d6d16160e0e",
"&. c #6e6e15150c0c",
"*. c #717115150d0d",
"=. c #727215150d0d",
"-. c #737315150c0c",
";. c #737316160e0e",
":. c #777715150c0c",
">. c #787815150b0b",
",. c #787815150c0c",
"<. c #737317171111",
"1. c #7a7a17171010",
"2. c #787818181212",
"3. c #7b7b19191212",
"4. c #525240400707",
"5. c #676750500909",
"6. c #696952520a0a",
"7. c #717157570a0a",
"8. c #74745a5a0c0c",
"9. c #7a7a61610909",
"0. c #7c7c61610c0c",
"q. c #858517170c0c",
"w. c #868618180d0d",
"e. c #8a8a18180c0c",
"r. c #8a8a19190f0f",
"t. c #808018181010",
"y. c #80801a1a1313",
"u. c #868619191010",
"i. c #86861b1b1313",
"p. c #87871b1b1212",
"a. c #85851b1b1414",
"s. c #88881a1a1111",
"d. c #89891a1a1111",
"f. c #8b8b1c1c1515",
"g. c #8d8d1b1b1212",
"h. c #8f8f1b1b1010",
"j. c #8c8c1c1c1414",
"k. c #90901a1a0f0f",
"l. c #91911a1a0f0f",
"z. c #92921a1a0e0e",
"x. c #9b9b1b1b0e0e",
"c. c #9a9a1c1c0f0f",
"v. c #93931b1b1010",
"b. c #90901e1e1212",
"n. c #97971e1e1515",
"m. c #99991d1d1313",
"M. c #98981d1d1414",
"N. c #98981f1f1717",
"B. c #99991f1f1616",
"V. c #9a9a1f1f1515",
"C. c #9b9b1e1e1414",
"Z. c #9b9b1f1f1717",
"A. c #9c9c1e1e1313",
"S. c #9d9d1e1e1212",
"D. c #9e9e1d1d1111",
"F. c #9f9f1d1d1010",
"G. c #9f9f1e1e1313",
"H. c #9d9d1f1f1515",
"J. c #9c9c1f1f1616",
"K. c #9e9e1e1e1414",
"L. c #a0a01d1d0f0f",
"P. c #a1a11c1c0e0e",
"I. c #a2a21d1d0f0f",
"U. c #a3a31c1c0e0e",
"Y. c #a3a31d1d0f0f",
"T. c #a4a41c1c0e0e",
"R. c #a6a61d1d0f0f",
"E. c #a7a71d1d0e0e",
"W. c #a9a91d1d0f0f",
"Q. c #a1a11d1d1010",
"!. c #a1a11d1d1111",
"~. c #a0a01e1e1212",
"^. c #a2a21d1d1010",
"/. c #b3b31f1f0f0f",
"(. c #b2b21f1f1010",
"). c #b9b920200f0f",
"_. c #b6b621211111",
"`. c #b7b720201010",
"'. c #baba20201010",
"]. c #bdbd21211111",
"[. c #bfbf22221212",
"{. c #abab42421616",
"}. c #b1b140401010",
"|. c #b9b95b5b1313",
" X c #bbbb5b5b1111",
".X c #bfbf6f6f1616",
"XX c #92924f4f4848",
"oX c #c6c622221010",
"OX c #c8c823231212",
"+X c #caca23231010",
"@X c #cdcd25251313",
"#X c #d1d124241212",
"$X c #d2d224241111",
"%X c #d2d226261414",
"&X c #d5d525251111",
"*X c #d4d425251313",
"=X c #d9d926261313",
"-X c #dbdb26261212",
";X c #d8d827271515",
":X c #dcdc26261313",
">X c #dede26261212",
",X c #e0e027271212",
"<X c #e3e327271313",
"1X c #e6e627271212",
"2X c #e4e42e2e1a1a",
"3X c #ebeb27271313",
"4X c #ebeb28281212",
"5X c #e8e828281414",
"6X c #eaea29291515",
"7X c #eaea2a2a1616",
"8X c #ecec29291313",
"9X c #eeee29291313",
"0X c #eded29291414",
"qX c #ecec2a2a1616",
"wX c #efef29291414",
"eX c #f1f129291212",
"rX c #f3f329291212",
"tX c #f4f429291313",
"yX c #f5f529291313",
"uX c #f5f52a2a1313",
"iX c #f6f629291313",
"pX c #f7f729291313",
"aX c #f4f429291414",
"sX c #f4f42a2a1414",
"dX c #f6f62b2b1414",
"fX c #c1c169691616",
"gX c #c2c26f6f1212",
"hX c #c1c16f6f1414",
"jX c #c5c56b6b1212",
"kX c #c5c57e7e1616",
"lX c #c7c77e7e1414",
"zX c #c8c87e7e1414",
"xX c #b9b993930e0e",
"cX c #bdbd93931212",
"vX c #c9c985851717",
"bX c #cbcb86861717",
"nX c #cdcd87871313",
"mX c #cccc86861414",
"MX c #cccc86861515",
"NX c #cbcb89891414",
"BX c #cfcf88881212",
"VX c #cfcf8c8c1212",
"CX c #c3c39b9b0f0f",
"ZX c #c4c498981313",
"AX c #c7c79a9a1414",
"SX c #cbcb9e9e1313",
"DX c #d1d189891212",
"FX c #d2d288881414",
"GX c #d0d08c8c1313",
"HX c #d7d7a8a81616",
"JX c #d7d7a8a81a1a",
"KX c #d8d8a5a51c1c",
"LX c #dadaaaaa1616",
"PX c #dadaaaaa1717",
"IX c #dbdbaaaa1616",
"UX c #dbdbabab1717",
"YX c #dcdcabab1515",
"TX c #dcdcaeae1111",
"RX c #dedeacac1313",
"EX c #dfdfadad1313",
"WX c #d8d8a9a91919",
"QX c #d8d8a9a91a1a",
"!X c #d9d9aaaa1919",
"~X c #dfdfb1b11212",
"^X c #e0e0adad1111",
"/X c #e1e1aeae1212",
"(X c #e0e0aeae1414",
")X c #e0e0b2b21313",
"_X c #808080808080",
"`X c None",
/* pixels */
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X9XsXoXb._X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X8XdX$XW.q.    `X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`XE.iX&XE.P.x.O.    `X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`XE.pX,X/.T.T.P.e.k i   `X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X4X8X).T.T.}.T.c.X.    `X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`XE.uX+XT.T. XgXY.L.z.n     `X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`XE.yX-XR.T.T.^X~XY.Y.L.:.      `X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X4X8X`.T.U./X~X~XFXY.F.l.R     `X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`XE.uX+XT.T.jX~X~X~X~XQ.^.F.-.      `X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`XE.tX>XR.T.T.^XTXxXTXRXBX^.F.k.T     `X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X1XwX'.T.T.DXCXX 3 ' RXRX^.!.D.-.      `X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`XE.rX#XT.T.T.~X9.* q @ RXRXnX!.D.k.E     `X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X4X5X(.T.T.)X~XC + . t RXRXYX!.!.S.=.      `X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`XE.uXOXT.T.FX~X~Xc   % V RXYXYXnX~.S.h.E     `X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`XE.eX:XR.T.Y.~X~X~Xb   y [ YXYXYXYX~.~.S.*.      `X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X@.0X].T.T.GX~X~XRX|   ` { YXYXYXIXmX~.S.d.U     `X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`XR.aX=XR.T.Y.~X~X~XRXu X v ] YXYXIXIXIX~.G.m.(       `X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X<X6X`.T.I.)X~X~XRXRX: X 2 v YXIXIXIXIXMXG.A.u.j     `X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`XR.wX*XT.Y.FX~X~XRXRXRX& 0 1 S IXIXIXIXPXPXG.G.M._   p   `X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X2X6X`.Y.Y.~X~XRXRXRXRX& w % 4.IXIXIXPXPXPXMXK.C.1.e     `X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`XR.wX*XY.Y.GX~XRXRXRXRXYXD  .r 7.IXIXPXPXPXPXWXK.K.g.W     `X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X2X6X`.Y.Y.~XRXRXRXRXYXYXcX5.Z AXIXIXPXPXPXWXWXbXK.V.%.$     `X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`XR.wX*XY.Y.GX~XRXRXRXYXYXYXYXIXIXIXIXPXPXPXPXWXWXWXH.V.p.P     `X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X2X6X_.Y.Y.~XRXRXRXYXYXYXYXSXv x ZXPXPXPXPXWXWXWXJXfXH.n.) =     `X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`XR.wX%XY.Y.VXRXRXRXRXYXYXYXIX} O O 6.PXPXPXWXWXWXWXJXvXH.B.3.h     `X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`XXXqX_.Y.Y.RXRXRXRXYXYXYXYXIXF % > 8.PXPXWXWXWXWXJXJXJXH.J.j.Y -   `X`X`X`X`X`X",
"`X`X`X`X`X`X`X`XR.3X;XY.Y.zX(XEXRXYXYXYXYXIXIXcXA 0.HXPXWXWXWXWXJXJXJXJX.XJ.B.o.  7   `X`X`X`X`X",
"`X`X`X`X`X`X`X`X#.7X[.Y.^.^.^.^.!.!.!.|.hXlXNXLXUXUXUX!X!X!X!XQXQXQXQXKXkX{.B.2.g   G `X`X`X`X`X",
"`X`X`X`X`X`X`X`XJ @XY.L.^.^.^.!.!.!.!.~.~.~.~.G.G.G.G.K.K.K.H.H.H.H.J.J.J.J.Z.f.I       `X`X`X`X",
"`X`X`X`X`X`X`X`X9 >.! +.$.,.w.r.v.D.S.~.~.~.G.G.G.G.K.K.K.H.H.H.H.J.J.J.J.Z.Z.N.^ o     `X`X`X`X",
"`X`X`X`X`X`X`X`X  - # # , d l H K ~ / ..&.;.;.;.t.s.s.s.s.p.p.p.i.i.i.i.a.a.y.<.Q # ;   `X`X`X`X",
"`X`X`X`X`X`X`X`X      # # # # # # # # # < s a a f L B B B B B B N N N M M M m z e #     `X`X`X`X",
"`X`X`X`X`X`X`X`X            $ $ # # # # # # # # # # # # # # # # # # # # # # # # #   o   `X`X`X`X",
"`X`X`X`X`X`X`X`X`X        8 6     o o = = $   $     # # # # # # # # # # # # # # $ $     `X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X                    5 5 4                                     `X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X                                          `X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X",
"`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X`X"
};

/* XPM */
static const char *question_xpm[] = {
/* columns rows colors chars-per-pixel */
"48 48 101 2",
"   c Gray0",
".  c Transparent",
"X  c #010101010101",
"o  c #000000000202",
"O  c #000001010202",
"+  c #010101010202",
"@  c #010102020303",
"#  c #020202020202",
"$  c #020202020303",
"%  c Gray1",
"&  c #030305050707",
"*  c #040404040404",
"=  c #040404040606",
"-  c #070707070707",
";  c #060607070808",
":  c #060608080a0a",
">  c #060608080b0b",
",  c Gray3",
"<  c #0d0d0d0d0c0c",
"1  c Gray6",
"2  c #191915150d0d",
"3  c Gray8",
"4  c Gray10",
"5  c #22221c1c1212",
"6  c #393939393939",
"7  c #2b2b3d3d6161",
"8  c #353545456464",
"9  c #65654f4f2424",
"0  c #6b6b55552727",
"q  c #6e6e55552626",
"w  c #707056562727",
"e  c #717159592929",
"r  c #73735f5f3b3b",
"t  c #7c7c62622d2d",
"y  c #7f7f69694141",
"u  c Gray39",
"i  c #727272727272",
"p  c #737375757979",
"a  c Gray50",
"s  c #808063632d2d",
"d  c #828266662f2f",
"f  c #87876a6a3131",
"g  c #8c8c6d6d3131",
"h  c #929273733535",
"j  c #939374743535",
"k  c #949475753636",
"l  c #979777773737",
"z  c #99997a7a3b3b",
"x  c #9d9d7d7d3a3a",
"c  c #a2a27f7f3b3b",
"v  c #92927c7c5252",
"b  c #a6a682823c3c",
"n  c #a8a884843d3d",
"m  c #aaaa86863e3e",
"M  c #a6a687874848",
"N  c #a3a38e8e5555",
"B  c #a4a48b8b5a5a",
"V  c #b3b38d8d4040",
"C  c #b8b892924343",
"Z  c #b9b993934444",
"A  c #bebe95954444",
"S  c #bebe9b9b5353",
"D  c #bebea3a36363",
"F  c #bfbfa1a16a6a",
"G  c #bebea2a27272",
"H  c #c0c097974545",
"J  c #c3c39f9f5555",
"K  c #c3c39f9f5757",
"L  c #c9c9a4a45b5b",
"P  c #d2d2a6a64c4c",
"I  c #d2d2a6a64d4d",
"U  c #d8d8abab4e4e",
"Y  c #d8d8acac5858",
"T  c #d8d8acac5b5b",
"R  c #d8d8b1b15f5f",
"E  c #c3c3a4a46666",
"W  c #c6c6a7a76a6a",
"Q  c #c9c9acac7373",
"!  c #d2d2b0b06c6c",
"~  c #d8d8b1b16363",
"^  c #d8d8b1b16565",
"/  c #dcdcb4b46363",
"(  c #d8d8b5b56e6e",
")  c #d8d8b6b66e6e",
"_  c #dadab8b87272",
"`  c #ddddbcbc7474",
"'  c #d8d8baba7b7b",
"]  c #f7f7c3c35a5a",
"[  c #f7f7c9c96d6d",
"{  c #f7f7cfcf7e7e",
"}  c #aaaaaaaaaaaa",
"|  c #d8d8bebe8686",
" . c #dcdcc4c49494",
".. c #f7f7d4d48c8c",
"X. c #f7f7d8d89999",
"o. c #f7f7dcdca5a5",
"O. c #f7f7dfdfafaf",
"+. c #f7f7e2e2b8b8",
"@. c #f7f7e5e5c0c0",
"#. c Gray100",
"$. c None",
/* pixels */
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.<                   $.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.      c C U x b t         $.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.    U X.X.+.X.' ' ' S b       $.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.    ] O.@.X...] ] ] { ' R b q     $.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.<   ] O.o.] n j j m ] ] { ) R b     * $.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.  D X.+.] k         h ] ] [ R R s     1 $.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.<   [ @.{ k     o 7 @   K ] { U R b     % $.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.  N ..O.U     o 8 a a   B ] [ R U b     # #.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.  I { ] b     8 a #.#.  B [ [ ) U d     # } #.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.  ] { ] b     a #.#.$.  B ] ) ) U 9     % } #.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.  / R A d     a #.$.    E [ ' U b       a } #.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.              a #.$.  g ! | R U e     + a } #.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.            : a #.    Z ..| U A       3 a #.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.  a a a a a a -   V [ | U U       $ a } #.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  a a a a #.    ] ..( Y z     . 3 a #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  0 J  ._ Q F 2     3 a } #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  f ! ( L W y     & a } #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  l ' ^ T G     . p } #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  H ' ~ T v     = a #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  U | R U r     a } #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  S ' U A     X a #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  M M b s     ; } #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  4 6       O a } #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.          O > a #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  a i u a a } #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.5         , #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  ` { ..A       $.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  { X.X.U     # $.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  ....R A     # } #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  ..) U A     # } #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  P R A w     # } #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.            # } #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.          + a } #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  , , , a a } #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.  a a a } #.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.",
"$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$.$."
};

wxBitmap wxGTKArtProvider::CreateBitmap(const wxArtID& id,
                                        const wxArtClient& WXUNUSED(client),
                                        const wxSize& WXUNUSED(size))
{
    if ( id == wxART_INFORMATION )
        return wxBitmap(info_xpm);
    if ( id == wxART_ERROR )
        return wxBitmap(error_xpm);
    if ( id == wxART_WARNING )
        return wxBitmap(warning_xpm);
    if ( id == wxART_QUESTION )
        return wxBitmap(question_xpm);
    return wxNullBitmap;
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

bool wxGTKInputHandler::HandleKey(wxInputConsumer * WXUNUSED(control),
                                  const wxKeyEvent& WXUNUSED(event),
                                  bool WXUNUSED(pressed))
{
    return false;
}

bool wxGTKInputHandler::HandleMouse(wxInputConsumer *control,
                                    const wxMouseEvent& event)
{
    // clicking on the control gives it focus
    if ( event.ButtonDown() && wxWindow::FindFocus() != control->GetInputWindow() )
    {
        control->GetInputWindow()->SetFocus();

        return true;
    }

    return false;
}

bool wxGTKInputHandler::HandleMouseMove(wxInputConsumer *control,
                                        const wxMouseEvent& event)
{
    if ( event.Entering() )
    {
        control->GetInputWindow()->SetCurrent(true);
    }
    else if ( event.Leaving() )
    {
        control->GetInputWindow()->SetCurrent(false);
    }
    else
    {
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------
// wxGTKCheckboxInputHandler
// ----------------------------------------------------------------------------

bool wxGTKCheckboxInputHandler::HandleKey(wxInputConsumer *control,
                                          const wxKeyEvent& event,
                                          bool pressed)
{
    if ( pressed )
    {
        int keycode = event.GetKeyCode();
        if ( keycode == WXK_SPACE || keycode == WXK_RETURN )
        {
            control->PerformAction(wxACTION_CHECKBOX_TOGGLE);

            return true;
        }
    }

    return false;
}

// ----------------------------------------------------------------------------
// wxGTKTextCtrlInputHandler
// ----------------------------------------------------------------------------

bool wxGTKTextCtrlInputHandler::HandleKey(wxInputConsumer *control,
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

            return true;
        }
    }

    return wxStdTextCtrlInputHandler::HandleKey(control, event, pressed);
}
