///////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/themes/mono.cpp
// Purpose:     wxUniversal theme for monochrome displays
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2006-08-27
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
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
#endif // WX_PRECOMP

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/colschem.h"
#include "wx/univ/theme.h"

class wxMonoColourScheme;

#define wxMONO_BG_COL   (*wxWHITE)
#define wxMONO_FG_COL   (*wxBLACK)

// ----------------------------------------------------------------------------
// wxMonoRenderer: draw the GUI elements in simplest possible way
// ----------------------------------------------------------------------------

// Warning: many of the methods here are not implemented, the code won't work
// if any but a few wxUSE_XXXs are on
class wxMonoRenderer : public wxStdRenderer
{
public:
    wxMonoRenderer(const wxColourScheme *scheme);

    virtual void DrawBackground(wxDC& dc,
                                const wxColour& col,
                                const wxRect& rect,
                                int flags,
                                wxWindow *window = NULL);

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
                            wxRect *rectIn = NULL);

    virtual void DrawTextBorder(wxDC& dc,
                                wxBorder border,
                                const wxRect& rect,
                                int flags = 0,
                                wxRect *rectIn = NULL);

    virtual void DrawButtonBorder(wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0,
                                  wxRect *rectIn = NULL);

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

#if wxUSE_TOOLBAR
    virtual void DrawToolBarButton(wxDC& dc,
                                   const wxString& label,
                                   const wxBitmap& bitmap,
                                   const wxRect& rect,
                                   int flags = 0,
                                   long style = 0,
                                   int tbarStyle = 0);
#endif // wxUSE_TOOLBAR

#if wxUSE_TEXTCTRL
    virtual void DrawTextLine(wxDC& dc,
                              const wxString& text,
                              const wxRect& rect,
                              int selStart = -1,
                              int selEnd = -1,
                              int flags = 0);

    virtual void DrawLineWrapMark(wxDC& dc, const wxRect& rect);
#endif // wxUSE_TEXTCTRL

#if wxUSE_NOTEBOOK
    virtual void DrawTab(wxDC& dc,
                         const wxRect& rect,
                         wxDirection dir,
                         const wxString& label,
                         const wxBitmap& bitmap = wxNullBitmap,
                         int flags = 0,
                         int indexAccel = -1);
#endif // wxUSE_NOTEBOOK

#if wxUSE_SLIDER

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

    virtual void DrawSliderTicks(wxDC& dc,
                                 const wxRect& rect,
                                 int lenThumb,
                                 wxOrientation orient,
                                 int start,
                                 int end,
                                 int step = 1,
                                 int flags = 0,
                                 long style = 0);
#endif // wxUSE_SLIDER

#if wxUSE_MENUS
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
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    virtual void DrawStatusField(wxDC& dc,
                                 const wxRect& rect,
                                 const wxString& label,
                                 int flags = 0, int style = 0);
#endif // wxUSE_STATUSBAR

    virtual void DrawFrameTitleBar(wxDC& dc,
                                   const wxRect& rect,
                                   const wxString& title,
                                   const wxIcon& icon,
                                   int flags,
                                   int specialButton = 0,
                                   int specialButtonFlags = 0);

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


    virtual void GetComboBitmaps(wxBitmap *bmpNormal,
                                 wxBitmap *bmpFocus,
                                 wxBitmap *bmpPressed,
                                 wxBitmap *bmpDisabled);


    virtual wxRect GetBorderDimensions(wxBorder border) const;

    virtual bool AreScrollbarsInsideBorder() const;

    virtual void AdjustSize(wxSize *size, const wxWindow *window);

#if wxUSE_SCROLLBAR

    virtual wxSize GetScrollbarArrowSize() const;

    virtual wxRect GetScrollbarRect(const wxScrollBar *scrollbar,
                                    wxScrollBar::Element elem,
                                    int thumbPos = -1) const;

    virtual wxCoord GetScrollbarSize(const wxScrollBar *scrollbar);

    virtual wxHitTest HitTestScrollbar(const wxScrollBar *scrollbar,
                                       const wxPoint& pt) const;

    virtual wxCoord ScrollbarToPixel(const wxScrollBar *scrollbar,
                                     int thumbPos = -1);
    virtual int PixelToScrollbar(const wxScrollBar *scrollbar,
                                 wxCoord coord);

#endif // wxUSE_SCROLLBAR

    virtual wxCoord GetListboxItemHeight(wxCoord fontHeight);

    virtual wxSize GetCheckBitmapSize() const;
    virtual wxSize GetRadioBitmapSize() const;
    virtual wxCoord GetCheckItemMargin() const;

    virtual wxSize GetToolBarButtonSize(wxCoord *separator) const;

    virtual wxSize GetToolBarMargin() const;

#if wxUSE_TEXTCTRL
    virtual wxRect GetTextTotalArea(const wxTextCtrl *text,
                                    const wxRect& rectText) const;

    virtual wxRect GetTextClientArea(const wxTextCtrl *text,
                                     const wxRect& rectTotal,
                                     wxCoord *extraSpaceBeyond) const;
#endif // wxUSE_TEXTCTRL

#if wxUSE_NOTEBOOK
    virtual wxSize GetTabIndent() const;

    virtual wxSize GetTabPadding() const;
#endif // wxUSE_NOTEBOOK

#if wxUSE_SLIDER
    virtual wxCoord GetSliderDim() const;

    virtual wxCoord GetSliderTickLen() const;

    virtual wxRect GetSliderShaftRect(const wxRect& rect,
                                      int lenThumb,
                                      wxOrientation orient,
                                      long style = 0) const;

    virtual wxSize GetSliderThumbSize(const wxRect& rect,
                                      int lenThumb,
                                      wxOrientation orient) const;
#endif // wxUSE_SLIDER

    virtual wxSize GetProgressBarStep() const;

#if wxUSE_MENUS
    virtual wxSize GetMenuBarItemSize(const wxSize& sizeText) const;

    virtual wxMenuGeometryInfo *GetMenuGeometry(wxWindow *win,
                                                const wxMenu& menu) const;
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    virtual wxSize GetStatusBarBorders(wxCoord *borderBetweenFields) const;
#endif // wxUSE_STATUSBAR

    virtual wxRect GetFrameClientArea(const wxRect& rect, int flags) const;

    virtual wxSize GetFrameTotalSize(const wxSize& clientSize, int flags) const;

    virtual wxSize GetFrameMinSize(int flags) const;

    virtual wxSize GetFrameIconSize() const;

    virtual int HitTestFrame(const wxRect& rect,
                             const wxPoint& pt,
                             int flags = 0) const;

protected:
    // draw the background with any colour, not only the default one(s)
    void DoDrawBackground(wxDC& dc,
                          const wxColour& col,
                          const wxRect& rect,
                          wxWindow *window = NULL);

    // DrawBorder() helpers: all of them shift and clip the DC after drawing
    // the border

    // just draw a rectangle with the given pen
    void DrawRect(wxDC& dc, wxRect *rect, const wxPen& pen);

    // draw an opened rect for the arrow in given direction
    void DrawArrowBorder(wxDC& dc,
                         wxRect *rect,
                         wxDirection dir);

    // draw two sides of the rectangle
    void DrawThumbBorder(wxDC& dc,
                         wxRect *rect,
                         wxOrientation orient);

#if wxUSE_SCROLLBAR
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
#endif // wxUSE_SCROLLBAR

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
                        const wxString& accel = wxEmptyString,
                        const wxBitmap& bitmap = wxNullBitmap,
                        const wxMonoMenuGeometryInfo *geometryInfo = NULL);

#if wxUSE_COMBOBOX
    // initialize the combo bitmaps
    void InitComboBitmaps();
#endif // wxUSE_COMBOBOX

private:
    // data
    wxSize m_sizeScrollbarArrow;

    // the checkbox bitmaps: first row is for the normal, second for the
    // pressed state and the columns are for checked, unchecked and
    // undeterminated respectively
    wxBitmap m_bitmapsCheckbox[2][3];

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
// wxMonoInputHandler
// ----------------------------------------------------------------------------

class wxMonoInputHandler : public wxInputHandler
{
public:
    wxMonoInputHandler(wxMonoRenderer *renderer);

    virtual bool HandleKey(wxInputConsumer *control,
                           const wxKeyEvent& event,
                           bool pressed);
    virtual bool HandleMouse(wxInputConsumer *control,
                             const wxMouseEvent& event);
    virtual bool HandleMouseMove(wxInputConsumer *control, const wxMouseEvent& event);

protected:
    wxMonoRenderer *m_renderer;
};

// ----------------------------------------------------------------------------
// wxMonoColourScheme: uses just white and black
// ----------------------------------------------------------------------------

class wxMonoColourScheme : public wxColourScheme
{
public:
    // we use only 2 colours, white and black, but we avoid referring to them
    // like this, instead use the functions below
    wxColour GetFg() const { return wxMONO_FG_COL; }
    wxColour GetBg() const { return wxMONO_BG_COL; }

    // implement base class pure virtuals
    virtual wxColour Get(StdColour col) const;
    virtual wxColour GetBackground(wxWindow *win) const;
};

// ----------------------------------------------------------------------------
// wxMonoArtProvider
// ----------------------------------------------------------------------------

class wxMonoArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id,
                                  const wxArtClient& client,
                                  const wxSize& size);
};

// ----------------------------------------------------------------------------
// wxMonoTheme
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY_PTR(wxInputHandler *, wxArrayHandlers);

class wxMonoTheme : public wxTheme
{
public:
    wxMonoTheme();
    virtual ~wxMonoTheme();

    virtual wxRenderer *GetRenderer();
    virtual wxArtProvider *GetArtProvider();
    virtual wxInputHandler *GetInputHandler(const wxString& control);
    virtual wxColourScheme *GetColourScheme();

private:
    wxMonoRenderer *m_renderer;
    wxMonoArtProvider *m_artProvider;
    wxMonoColourScheme *m_scheme;

    WX_DECLARE_THEME(gtk)
};

// ============================================================================
// implementation
// ============================================================================

WX_IMPLEMENT_THEME(wxMonoTheme, mono, wxTRANSLATE("Simple monochrome theme"));

// ----------------------------------------------------------------------------
// wxMonoTheme
// ----------------------------------------------------------------------------

wxMonoTheme::wxMonoTheme()
{
    m_scheme = NULL;
    m_renderer = NULL;
    m_artProvider = NULL;
}

wxMonoTheme::~wxMonoTheme()
{
    delete m_renderer;
    delete m_scheme;
    wxArtProvider::RemoveProvider(m_artProvider);
}

wxRenderer *wxMonoTheme::GetRenderer()
{
    if ( !m_renderer )
    {
        m_renderer = new wxMonoRenderer(GetColourScheme());
    }

    return m_renderer;
}

wxArtProvider *wxMonoTheme::GetArtProvider()
{
    if ( !m_artProvider )
    {
        m_artProvider = new wxMonoArtProvider;
    }

    return m_artProvider;
}

wxColourScheme *wxMonoTheme::GetColourScheme()
{
    if ( !m_scheme )
    {
        m_scheme = new wxMonoColourScheme;
    }

    return m_scheme;
}

wxInputHandler *wxMonoTheme::GetInputHandler(const wxString& WXUNUSED(control),
                                             wxInputConsumer *consumer)
{
    // no special input handlers so far
    return consumer->DoGetStdInputHandler(NULL);
}

// ============================================================================
// wxMonoColourScheme
// ============================================================================

wxColour wxMonoColourScheme::GetBackground(wxWindow *win) const
{
    wxColour col;
    if ( win->UseBgCol() )
    {
        // use the user specified colour
        col = win->GetBackgroundColour();
    }

    // doesn't depend on the state
    if ( !col.Ok() )
    {
        col = GetBg();
    }
}

wxColour wxMonoColourScheme::Get(wxMonoColourScheme::StdColour col) const
{
    switch ( col )
    {
        case WINDOW:
        case CONTROL:
        case CONTROL_PRESSED:
        case CONTROL_CURRENT:
        case SCROLLBAR:
        case SCROLLBAR_PRESSED:
        case GAUGE:
        case HIGHLIGHT:
        case TITLEBAR:
        case TITLEBAR_ACTIVE:
        case DESKTOP:
            return GetBg();

        case MAX:
        default:
            wxFAIL_MSG(_T("invalid standard colour"));
            // fall through

        case SHADOW_DARK:
        case SHADOW_HIGHLIGHT:
        case SHADOW_IN:
        case SHADOW_OUT:
        case CONTROL_TEXT:
        case CONTROL_TEXT_DISABLED:
        case CONTROL_TEXT_DISABLED_SHADOW:
        case HIGHLIGHT_TEXT:
        case TITLEBAR_TEXT:
        case TITLEBAR_ACTIVE_TEXT:
            return GetFg();

    }
}

// ============================================================================
// wxMonoRenderer
// ============================================================================

// ----------------------------------------------------------------------------
// construction
// ----------------------------------------------------------------------------

wxMonoRenderer::wxMonoRenderer(const wxColourScheme *scheme)
              : wxStdRenderer(scheme)
{
    m_penFg = wxPen(wxSCHEME_COLOUR(scheme, SHADOW_OUT));
}

// ----------------------------------------------------------------------------
// border stuff
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// special borders
// ----------------------------------------------------------------------------

void wxMonoRenderer::DrawTextBorder(wxDC& dc,
                                    wxBorder border,
                                    const wxRect& rectOrig,
                                    int flags,
                                    wxRect *rectIn)
{
    DrawBorder(dc, border, rectOrig, flags, rectIn);
}

void wxMonoRenderer::DrawButtonBorder(wxDC& dc,
                                     const wxRect& rectTotal,
                                     int flags,
                                     wxRect *rectIn)
{
    DrawBorder(dc, wxBORDER_SIMPLE, rectOrig, flags, rectIn);
}

// ----------------------------------------------------------------------------
// lines and frames
// ----------------------------------------------------------------------------

void
wxMonoRenderer::DrawHorizontalLine(wxDC& dc, wxCoord y, wxCoord x1, wxCoord x2)
{
    dc.SetPen(m_penFg);
    dc.DrawLine(x1, y, x2 + 1, y);
}

void
wxMonoRenderer::DrawVerticalLine(wxDC& dc, wxCoord x, wxCoord y1, wxCoord y2)
{
    dc.SetPen(m_penFg);
    dc.DrawLine(x, y1, x, y2 + 1);
}

void wxMonoRenderer::DrawFrame(wxDC& dc,
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
    }
    else // no label
    {
        // just draw the complete frame
        DrawShadedRect(dc, &rectFrame, m_penDarkGrey, m_penHighlight);
        DrawShadedRect(dc, &rectFrame, m_penHighlight, m_penDarkGrey);
    }
}

// ----------------------------------------------------------------------------
// label
// ----------------------------------------------------------------------------

void wxMonoRenderer::DrawLabel(wxDC& dc,
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

void wxMonoRenderer::DrawButtonLabel(wxDC& dc,
                                    const wxString& label,
                                    const wxBitmap& image,
                                    const wxRect& rect,
                                    int flags,
                                    int alignment,
                                    int indexAccel,
                                    wxRect *rectBounds)
{
    dc.SetTextForeground(wxSCHEME_COLOUR(m_scheme, CONTROL_TEXT));
    dc.DrawLabel(label, image, rect, alignment, indexAccel, rectBounds);

    if ( flags & wxCONTROL_DISABLED )
    {
        // this is ugly but I don't know how to show disabled button visually
        // in monochrome theme otherwise
        dc.SetPen(m_penFg);
        dc.DrawLine(rect.GetTopLeft(), rect.GetBottomRight());
        dc.DrawLine(rect.GetTopRight(), rect.GetBottomLeft());
    }
}

void wxMonoRenderer::DrawItem(wxDC& dc,
                              const wxString& label,
                              const wxRect& rect,
                              int flags)
{
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

void wxMonoRenderer::DrawCheckItem(wxDC& dc,
                                  const wxString& label,
                                  const wxBitmap& bitmap,
                                  const wxRect& rect,
                                  int flags)
{
    wxRect rectBitmap = rect;
    rectBitmap.x -= 1;
    rectBitmap.width = GetCheckBitmapSize().x;

    // never draw the focus rect around the check indicators here
    DrawCheckButton(dc, wxEmptyString, bitmap, rectBitmap, flags & ~wxCONTROL_FOCUSED);

    wxRect rectLabel = rect;
    wxCoord shift = rectBitmap.width + 2*GetCheckItemMargin();
    rectLabel.x += shift;
    rectLabel.width -= shift;
    DrawItem(dc, label, rectLabel, flags);
}

// ----------------------------------------------------------------------------
// check/radion buttons
// ----------------------------------------------------------------------------

void wxMonoRenderer::DrawUndeterminedBitmap(wxDC& dc,
                                           const wxRect& rectTotal,
                                           bool isPressed)
{
    // FIXME: For sure it is not Mono look but it is better than nothing.
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

void wxMonoRenderer::DrawUncheckBitmap(wxDC& dc,
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

void wxMonoRenderer::DrawCheckBitmap(wxDC& dc, const wxRect& rectTotal)
{
    wxRect rect = rectTotal;
    DrawAntiShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
    DrawShadedRect(dc, &rect, m_penBlack, m_penLightGrey);

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(wxSCHEME_COLOUR(m_scheme, CONTROL_PRESSED), wxSOLID));
    dc.DrawRectangle(rect);
}

void wxMonoRenderer::DrawRadioBitmap(wxDC& dc,
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

void wxMonoRenderer::DrawUpZag(wxDC& dc,
                              wxCoord x1,
                              wxCoord x2,
                              wxCoord y1,
                              wxCoord y2)
{
    wxCoord xMid = (x1 + x2) / 2;
    dc.DrawLine(x1, y1, xMid, y2);
    dc.DrawLine(xMid, y2, x2 + 1, y1 + 1);
}

void wxMonoRenderer::DrawDownZag(wxDC& dc,
                                wxCoord x1,
                                wxCoord x2,
                                wxCoord y1,
                                wxCoord y2)
{
    wxCoord xMid = (x1 + x2) / 2;
    dc.DrawLine(x1 + 1, y1 + 1, xMid, y2);
    dc.DrawLine(xMid, y2, x2, y1);
}

wxBitmap wxMonoRenderer::GetCheckBitmap(int flags)
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

void wxMonoRenderer::DrawCheckButton(wxDC& dc,
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

void wxMonoRenderer::DoDrawCheckOrRadioBitmap(wxDC& dc,
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
        // the border does not offset the string under Mono
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

void wxMonoRenderer::DrawRadioButton(wxDC& dc,
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

#if wxUSE_TOOLBAR
void wxMonoRenderer::DrawToolBarButton(wxDC& dc,
                                      const wxString& label,
                                      const wxBitmap& bitmap,
                                      const wxRect& rectOrig,
                                      int flags,
                                      long WXUNUSED(style),
                                      int tbarStyle)
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

        if(tbarStyle & wxTB_TEXT)
        {
            if(tbarStyle & wxTB_HORIZONTAL)
            {
                dc.DrawLabel(label, bitmap, rect, wxALIGN_CENTRE);
            }
            else
            {
                dc.DrawLabel(label, bitmap, rect, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
            }
        }
        else
        {
            int xpoint = (rect.GetLeft() + rect.GetRight() + 1 - bitmap.GetWidth()) / 2;
            int ypoint = (rect.GetTop() + rect.GetBottom() + 1 - bitmap.GetHeight()) / 2;
            dc.DrawBitmap(bitmap, xpoint, ypoint);
        }
    }
}
#endif // wxUSE_TOOLBAR

// ----------------------------------------------------------------------------
// text control
// ----------------------------------------------------------------------------

#if wxUSE_TEXTCTRL

wxRect wxMonoRenderer::GetTextTotalArea(const wxTextCtrl * WXUNUSED(text),
                                       const wxRect& rect) const
{
    wxRect rectTotal = rect;
    rectTotal.Inflate(2*BORDER_THICKNESS);
    return rectTotal;
}

wxRect wxMonoRenderer::GetTextClientArea(const wxTextCtrl *text,
                                         const wxRect& rect,
                                         wxCoord *extraSpaceBeyond) const
{
    wxRect rectText = rect;
    rectText.Deflate(2*BORDER_THICKNESS);

    return rectText;
}

void wxMonoRenderer::DrawTextLine(wxDC& dc,
                                  const wxString& text,
                                  const wxRect& rect,
                                  int selStart,
                                  int selEnd,
                                  int flags)
{
    StandardDrawTextLine(dc, text, rect, selStart, selEnd, flags);
}

void
wxMonoRenderer::DrawLineWrapMark(wxDC& WXUNUSED(dc),
                                 const wxRect& WXUNUSED(rect))
{
    // no line wrapping indicators
}

#endif // wxUSE_TEXTCTRL

// ----------------------------------------------------------------------------
// combobox
// ----------------------------------------------------------------------------

#if wxUSE_COMBOBOX

void wxMonoRenderer::InitComboBitmaps()
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

void wxMonoRenderer::GetComboBitmaps(wxBitmap *bmpNormal,
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

#endif // wxUSE_COMBOBOX

// ----------------------------------------------------------------------------
// background
// ----------------------------------------------------------------------------

void wxMonoRenderer::DoDrawBackground(wxDC& dc,
                                      const wxColour& col,
                                      const wxRect& rect,
                                      wxWindow * WXUNUSED(window))
{
    wxBrush brush(col, wxSOLID);
    dc.SetBrush(brush);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect);
}

void wxMonoRenderer::DrawBackground(wxDC& dc,
                                    const wxColour& col,
                                    const wxRect& rect,
                                    int WXUNUSED(flags),
                                    wxWindow *window)
{
}

// ----------------------------------------------------------------------------
// scrollbar
// ----------------------------------------------------------------------------

void wxMonoRenderer::DrawArrowBorder(wxDC& dc,
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

void wxMonoRenderer::DrawScrollbarArrow(wxDC& dc,
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
void wxMonoRenderer::DrawArrow(wxDC& dc,
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

void wxMonoRenderer::DrawThumbBorder(wxDC& dc,
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

void wxMonoRenderer::DrawScrollbarThumb(wxDC& dc,
                                       wxOrientation orient,
                                       const wxRect& rect,
                                       int flags)
{
    // the thumb is never pressed never has focus border under Mono and the
    // scrollbar background never changes at all
    int flagsThumb = flags & ~(wxCONTROL_PRESSED | wxCONTROL_FOCUSED);

    // we don't want the border in the direction of the scrollbar movement
    wxRect rectThumb = rect;
    DrawThumbBorder(dc, &rectThumb, orient);

    DrawButtonBorder(dc, rectThumb, flagsThumb, &rectThumb);
    DrawBackground(dc, wxNullColour, rectThumb, flagsThumb);
}

void wxMonoRenderer::DrawScrollbarShaft(wxDC& dc,
                                       wxOrientation orient,
                                       const wxRect& rect,
                                       int WXUNUSED(flags))
{
    wxRect rectBar = rect;
    DrawThumbBorder(dc, &rectBar, orient);
    DoDrawBackground(dc, wxSCHEME_COLOUR(m_scheme, SCROLLBAR), rectBar);
}

void wxMonoRenderer::DrawScrollCorner(wxDC& dc, const wxRect& rect)
{
    DoDrawBackground(dc, wxSCHEME_COLOUR(m_scheme, CONTROL), rect);
}

#if wxUSE_SCROLLBAR
wxRect wxMonoRenderer::GetScrollbarRect(const wxScrollBar *scrollbar,
                                       wxScrollBar::Element elem,
                                       int thumbPos) const
{
    // as Mono scrollbars can't be disabled, it makes no sense to remove the
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

wxCoord wxMonoRenderer::GetScrollbarSize(const wxScrollBar *scrollbar)
{
    return StandardScrollBarSize(scrollbar, GetScrollbarArrowSize(scrollbar));
}

wxHitTest wxMonoRenderer::HitTestScrollbar(const wxScrollBar *scrollbar,
                                          const wxPoint& pt) const
{
    return StandardHitTestScrollbar(scrollbar, pt,
                                    GetScrollbarArrowSize(scrollbar));
}

wxCoord wxMonoRenderer::ScrollbarToPixel(const wxScrollBar *scrollbar,
                                        int thumbPos)
{
    return StandardScrollbarToPixel(scrollbar, thumbPos,
                                    GetScrollbarArrowSize(scrollbar));
}

int wxMonoRenderer::PixelToScrollbar(const wxScrollBar *scrollbar,
                                    wxCoord coord)
{
    return StandardPixelToScrollbar(scrollbar, coord,
                                    GetScrollbarArrowSize(scrollbar));
}
#endif // wxUSE_SCROLLBAR

// ----------------------------------------------------------------------------
// size adjustments
// ----------------------------------------------------------------------------

void wxMonoRenderer::AdjustSize(wxSize *size, const wxWindow *window)
{
#if wxUSE_BMPBUTTON
    if ( wxDynamicCast(window, wxBitmapButton) )
    {
        size->x += 4;
        size->y += 4;
    } else
#endif // wxUSE_BMPBUTTON
#if wxUSE_BUTTON || wxUSE_TOGGLEBTN
    if ( 0
#  if wxUSE_BUTTON
         || wxDynamicCast(window, wxButton)
#  endif // wxUSE_BUTTON
#  if wxUSE_TOGGLEBTN
         || wxDynamicCast(window, wxToggleButton)
#  endif // wxUSE_TOGGLEBTN
        )
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
#endif // wxUSE_BUTTON || wxUSE_TOGGLEBTN
#if wxUSE_SCROLLBAR
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
#endif // wxUSE_SCROLLBAR
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

void wxMonoRenderer::DrawFrameTitleBar(wxDC& WXUNUSED(dc),
                                      const wxRect& WXUNUSED(rect),
                                      const wxString& WXUNUSED(title),
                                      const wxIcon& WXUNUSED(icon),
                                      int WXUNUSED(flags),
                                      int WXUNUSED(specialButton),
                                      int WXUNUSED(specialButtonFlag))
{
}

void wxMonoRenderer::DrawFrameBorder(wxDC& WXUNUSED(dc),
                                    const wxRect& WXUNUSED(rect),
                                    int WXUNUSED(flags))
{
}

void wxMonoRenderer::DrawFrameBackground(wxDC& WXUNUSED(dc),
                                        const wxRect& WXUNUSED(rect),
                                        int WXUNUSED(flags))
{
}

void wxMonoRenderer::DrawFrameTitle(wxDC& WXUNUSED(dc),
                                   const wxRect& WXUNUSED(rect),
                                   const wxString& WXUNUSED(title),
                                   int WXUNUSED(flags))
{
}

void wxMonoRenderer::DrawFrameIcon(wxDC& WXUNUSED(dc),
                                  const wxRect& WXUNUSED(rect),
                                  const wxIcon& WXUNUSED(icon),
                                  int WXUNUSED(flags))
{
}

void wxMonoRenderer::DrawFrameButton(wxDC& WXUNUSED(dc),
                                    wxCoord WXUNUSED(x),
                                    wxCoord WXUNUSED(y),
                                    int WXUNUSED(button),
                                    int WXUNUSED(flags))
{
}

wxRect
wxMonoRenderer::GetFrameClientArea(const wxRect& rect,
                                  int WXUNUSED(flags)) const
{
    return rect;
}

wxSize
wxMonoRenderer::GetFrameTotalSize(const wxSize& clientSize,
                                 int WXUNUSED(flags)) const
{
    return clientSize;
}

wxSize wxMonoRenderer::GetFrameMinSize(int WXUNUSED(flags)) const
{
    return wxSize(0,0);
}

wxSize wxMonoRenderer::GetFrameIconSize() const
{
    return wxSize(wxDefaultCoord, wxDefaultCoord);
}

int
wxMonoRenderer::HitTestFrame(const wxRect& WXUNUSED(rect),
                            const wxPoint& WXUNUSED(pt),
                            int WXUNUSED(flags)) const
{
    return wxHT_TOPLEVEL_CLIENT_AREA;
}


// ----------------------------------------------------------------------------
// standard icons
// ----------------------------------------------------------------------------

/* Copyright (c) Julian Smart */
static const char *error_xpm[] = {
/* columns rows colors chars-per-pixel */
"48 48 4 1",
"  c None",
"X c #242424",
"o c #DCDF00",
". c #C00000",
/* pixels */
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                    .....                       ",
"                .............                   ",
"              .................                 ",
"             ...................                ",
"           .......................              ",
"          .........................             ",
"         ...........................            ",
"         ...........................X           ",
"        .............................X          ",
"       ...............................          ",
"       ...............................X         ",
"      .................................X        ",
"      .................................X        ",
"      .................................XX       ",
"      ...ooooooooooooooooooooooooooo...XX       ",
"     ....ooooooooooooooooooooooooooo....X       ",
"     ....ooooooooooooooooooooooooooo....X       ",
"     ....ooooooooooooooooooooooooooo....XX      ",
"     ....ooooooooooooooooooooooooooo....XX      ",
"     ....ooooooooooooooooooooooooooo....XX      ",
"      ...ooooooooooooooooooooooooooo...XXX      ",
"      ...ooooooooooooooooooooooooooo...XXX      ",
"      .................................XX       ",
"      .................................XX       ",
"       ...............................XXX       ",
"       ...............................XXX       ",
"        .............................XXX        ",
"         ...........................XXXX        ",
"         ...........................XXX         ",
"          .........................XXX          ",
"           .......................XXXX          ",
"            X...................XXXXX           ",
"             X.................XXXXX            ",
"               X.............XXXXX              ",
"                XXXX.....XXXXXXXX               ",
"                  XXXXXXXXXXXXX                 ",
"                      XXXXX                     ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                "
};

/* Copyright (c) Julian Smart */
static const char *info_xpm[] = {
/* columns rows colors chars-per-pixel */
"48 48 9 1",
"$ c Black",
"O c #FFFFFF",
"@ c #808080",
"+ c #000080",
"o c #E8EB01",
"  c None",
"X c #FFFF40",
"# c #C0C0C0",
". c #ABAD01",
/* pixels */
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                     .....                      ",
"                   ..XXXXX..                    ",
"                 ..XXXXXXXXo..                  ",
"                .XXXOXXXXXXXoo.                 ",
"                .XOOXXX+XXXXXo.                 ",
"               .XOOOXX+++XXXXoo.                ",
"               .XOOXXX+++XXXXXo.                ",
"              .XOOOXXX+++XXXXXXo.               ",
"              .XOOXXXX+++XXXXXXo.               ",
"              .XXXXXXX+++XXXXXXX.               ",
"              .XXXXXXX+++XXXXXXo.               ",
"              .XXXXXXX+++XXXXXoo.               ",
"               .XXXXXX+++XXXXXo.                ",
"               .XXXXXXX+XXXXXXo.                ",
"                .XXXXXXXXXXXXo.                 ",
"                .XXXXX+++XXXoo.                 ",
"                 .XXXX+++XXoo.                  ",
"                  .XXXXXXXXo.                   ",
"                  ..XXXXXXo..                   ",
"                   .XXXXXo..                    ",
"                   @#######@                    ",
"                   @@@@@@@@@                    ",
"                   @#######@                    ",
"                   @@@@@@@@@                    ",
"                   @#######@                    ",
"                    @@@@@@@                     ",
"                      ###                       ",
"                      $$$                       ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                "
};

/* Copyright (c) Julian Smart */
static const char *warning_xpm[] = {
/* columns rows colors chars-per-pixel */
"48 48 9 1",
"@ c Black",
"o c #A6A800",
"+ c #8A8C00",
"$ c #B8BA00",
"  c None",
"O c #6E7000",
"X c #DCDF00",
". c #C00000",
"# c #373800",
/* pixels */
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                      .                         ",
"                     ...                        ",
"                     ...                        ",
"                    .....                       ",
"                   ...X..                       ",
"                   ..XXX..                      ",
"                  ...XXX...                     ",
"                  ..XXXXX..                     ",
"                 ..XXXXXX...                    ",
"                ...XXoO+XX..                    ",
"                ..XXXO@#XXX..                   ",
"               ..XXXXO@#XXX...                  ",
"              ...XXXXO@#XXXX..                  ",
"              ..XXXXXO@#XXXX...                 ",
"             ...XXXXXo@OXXXXX..                 ",
"            ...XXXXXXo@OXXXXXX..                ",
"            ..XXXXXXX$@OXXXXXX...               ",
"           ...XXXXXXXX@XXXXXXXX..               ",
"          ...XXXXXXXXXXXXXXXXXX...              ",
"          ..XXXXXXXXXXOXXXXXXXXX..              ",
"         ...XXXXXXXXXO@#XXXXXXXXX..             ",
"         ..XXXXXXXXXXX#XXXXXXXXXX...            ",
"        ...XXXXXXXXXXXXXXXXXXXXXXX..            ",
"       ...XXXXXXXXXXXXXXXXXXXXXXXX...           ",
"       ..............................           ",
"       ..............................           ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                "
};

/* Copyright (c) Julian Smart */
static const char *question_xpm[] = {
/* columns rows colors chars-per-pixel */
"48 48 21 1",
". c Black",
"> c #696969",
"O c #1F1F00",
"+ c #181818",
"o c #F6F900",
"; c #3F3F00",
"$ c #111111",
"  c None",
"& c #202020",
"X c #AAAA00",
"@ c #949400",
": c #303030",
"1 c #383838",
"% c #2A2A00",
", c #404040",
"= c #B4B400",
"- c #484848",
"# c #151500",
"< c #9F9F00",
"2 c #6A6A00",
"* c #353500",
/* pixels */
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                   .........                    ",
"                 ...XXXXXXX..                   ",
"               ..XXXXoooooXXXO+                 ",
"             ..XXooooooooooooX@..               ",
"            ..XoooooooooooooooXX#.              ",
"           $%XoooooooooooooooooXX#.             ",
"          &.XoooooooXXXXXXooooooXX..            ",
"          .XooooooXX.$...$XXoooooX*.            ",
"         $.XoooooX%.$     .*oooooo=..           ",
"         .XooooooX..      -.XoooooX..           ",
"         .XoooooX..+       .XoooooX;.           ",
"         ...XXXX..:        .XoooooX;.           ",
"          ........        >.XoooooX;.           ",
"                          +.XoooooX..           ",
"                         ,.Xoooooo<..           ",
"                        1#XooooooXO..           ",
"                       &#XooooooX2..            ",
"                      $%XooooooXX..             ",
"                     $%XooooooXX..              ",
"                    $%XooooooXX..               ",
"                   &.XooooooXX..                ",
"                   .XooooooXX..                 ",
"                  &.XoooooXX..                  ",
"                  ..XooooXX..                   ",
"                  ..XooooX...                   ",
"                  ..XXooXX..&                   ",
"                  ...XXXXX..                    ",
"                   ........                     ",
"                                                ",
"                                                ",
"                   .......                      ",
"                  ..XXXXX..                     ",
"                 ..XXoooXX..                    ",
"                 ..XoooooX..                    ",
"                 ..XoooooX..                    ",
"                 ..XXoooXX..                    ",
"                  ..XXXXX..                     ",
"                   .......                      ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                "
};

wxBitmap wxMonoArtProvider::CreateBitmap(const wxArtID& id,
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

