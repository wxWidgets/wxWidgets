///////////////////////////////////////////////////////////////////////////////
// Name:        univ/themes/win32.cpp
// Purpose:     wxUniversal theme implementing Win32-like LNF
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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/dc.h"
    #include "wx/window.h"

    #include "wx/button.h"
    #include "wx/scrolbar.h"
#endif // WX_PRECOMP

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/colschem.h"
#include "wx/univ/theme.h"

// ----------------------------------------------------------------------------
// wxWin32Renderer: draw the GUI elements in Win32 style
// ----------------------------------------------------------------------------

class wxWin32Renderer : public wxRenderer
{
public:
    // constants
    enum wxArrowDirection
    {
        Arrow_Left,
        Arrow_Right,
        Arrow_Up,
        Arrow_Down,
        Arrow_Max
    };

    enum wxArrowStyle
    {
        Arrow_Normal,
        Arrow_Disabled,
        Arrow_StateMax
    };

    // ctor
    wxWin32Renderer(const wxColourScheme *scheme);

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
                           int indexAccel = -1);
    virtual void DrawBorder(wxDC& dc,
                            wxBorder border,
                            const wxRect& rect,
                            int flags = 0,
                            wxRect *rectIn = (wxRect *)NULL);
    virtual void DrawFrame(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = 0,
                           int alignment = wxALIGN_LEFT,
                           int indexAccel = -1);
    virtual void DrawButtonBorder(wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0,
                                  wxRect *rectIn = (wxRect *)NULL);
    virtual void DrawArrow(wxDC& dc,
                           wxDirection dir,
                           const wxRect& rect,
                           int flags = 0);
    virtual void DrawScrollbar(wxDC& dc,
                               wxOrientation orient,
                               int thumbPosStart,
                               int thumbPosEnd,
                               const wxRect& rect,
                               int flags = 0);

    virtual void AdjustSize(wxSize *size, const wxWindow *window);

protected:
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

    // draw the normal 3D border
    void DrawRaisedBorder(wxDC& dc, wxRect *rect);

    // draw the border used for scrollbar arrows
    void DrawArrowBorder(wxDC& dc, wxRect *rect);

    // public DrawArrow()s helper
    void DrawArrow(wxDC& dc, const wxRect& rect,
                   wxArrowDirection arrowDir, wxArrowStyle arrowStyle);

    // DrawArrowButton is used by DrawScrollbar and DrawComboButton
    void DrawArrowButton(wxDC& dc, const wxRect& rect,
                         wxArrowDirection arrowDir,
                         wxArrowStyle arrowStyle);

private:
    // the sizing parameters (TODO make them changeable)
    wxSize m_sizeScrollbarArrow;

    // GDI objects we use for drawing
    wxColour m_colDarkGrey,
             m_colBg,
             m_colHighlight;

    wxPen m_penBlack,
          m_penDarkGrey,
          m_penLightGrey,
          m_penHighlight;

    // first row is for the normal state, second - for the disabled
    wxBitmap m_bmpArrows[Arrow_StateMax][Arrow_Max];
};

// ----------------------------------------------------------------------------
// wxWin32InputHandler and derived classes: process the keyboard and mouse
// messages according to Windows standards
// ----------------------------------------------------------------------------

class wxWin32InputHandler : public wxInputHandler
{
public:
    virtual wxControlActions Map(const wxKeyEvent& event, bool pressed);
    virtual wxControlActions Map(const wxMouseEvent& event);
};

class wxWin32ButtonInputHandler : public wxWin32InputHandler
{
public:
    wxWin32ButtonInputHandler();

    virtual wxControlActions Map(const wxKeyEvent& event, bool pressed);
    virtual wxControlActions Map(const wxMouseEvent& event);

private:
    wxWindow *m_winCapture;
};

// ----------------------------------------------------------------------------
// wxWin32ColourScheme: uses (default) Win32 colours
// ----------------------------------------------------------------------------

class wxWin32ColourScheme : public wxColourScheme
{
public:
    virtual wxColour Get(StdColour col, int flags = 0) const;
};

// ----------------------------------------------------------------------------
// wxWin32Theme
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY(wxInputHandler *, wxArrayHandlers);

class wxWin32Theme : public wxTheme
{
public:
    wxWin32Theme();
    virtual ~wxWin32Theme();

    virtual wxRenderer *GetRenderer() { return m_renderer; }
    virtual wxInputHandler *GetInputHandler(const wxString& control);
    virtual wxColourScheme *GetColourScheme(const wxString& control);

private:
    wxWin32Renderer *m_renderer;

    // the names of the already created handlers and the handlers themselves
    // (these arrays are synchronized)
    wxSortedArrayString m_handlerNames;
    wxArrayHandlers m_handlers;

    wxWin32ColourScheme *m_scheme;

    WX_DECLARE_THEME();
};

// ============================================================================
// implementation
// ============================================================================

WX_IMPLEMENT_THEME(wxWin32Theme, win32, wxTRANSLATE("Win32 theme"));

// ----------------------------------------------------------------------------
// wxWin32Theme
// ----------------------------------------------------------------------------

wxWin32Theme::wxWin32Theme()
{
    m_scheme = new wxWin32ColourScheme;
    m_renderer = new wxWin32Renderer(m_scheme);
}

wxWin32Theme::~wxWin32Theme()
{
    WX_CLEAR_ARRAY(m_handlers);

    delete m_renderer;
    delete m_scheme;
}

wxInputHandler *wxWin32Theme::GetInputHandler(const wxString& control)
{
    wxInputHandler *handler;
    int n = m_handlerNames.Index(control);
    if ( n == wxNOT_FOUND )
    {
        // create a new handler
        n = m_handlerNames.Add(control);

        if ( control == wxCONTROL_BUTTON )
            handler = new wxWin32ButtonInputHandler;
        else if ( control == wxCONTROL_SCROLLBAR )
            handler = new wxWin32InputHandler; // TODO
        else
        {
            wxASSERT_MSG( control == wxCONTROL_DEFAULT,
                          _T("no input handler defined for this control") );

            handler = new wxWin32InputHandler;
        }

        m_handlers.Insert(handler, n);
    }
    else // we already have it
    {
        handler = m_handlers[n];
    }

    return handler;
}

wxColourScheme *wxWin32Theme::GetColourScheme(const wxString& control)
{
    // Win32 has only one colour scheme for all controls
    return m_scheme;
}

// ============================================================================
// wxWin32ColourScheme
// ============================================================================

wxColour wxWin32ColourScheme::Get(wxWin32ColourScheme::StdColour col,
                                  int flags) const
{
    switch ( col )
    {
        case CONTROL:           return wxColour(0xc0c0c0);
        case CONTROL_TEXT:      return *wxBLACK;

        case HIGHLIGHT:         return wxColour(0x800000);
        case HIGHLIGHT_TEXT:    return wxColour(0xffffff);

        case SHADOW_DARK:       return *wxBLACK;
        case SHADOW_HIGHLIGHT:  return wxColour(0xe0e0e0);
        case SHADOW_IN:         return wxColour(0xc0c0c0);
        case SHADOW_OUT:        return wxColour(0x7f7f7f);

        case MAX:
        default:
            wxFAIL_MSG(_T("invalid standard colour"));
            return *wxBLACK;
    }
}

// ============================================================================
// wxWin32Renderer
// ============================================================================

// ----------------------------------------------------------------------------
// construction
// ----------------------------------------------------------------------------

wxWin32Renderer::wxWin32Renderer(const wxColourScheme *scheme)
{
    // init data
    m_sizeScrollbarArrow = wxSize(16, 16);

    // init colours and pens
    m_penBlack = wxPen(scheme->Get(wxColourScheme::SHADOW_DARK), 0, wxSOLID);

    m_colDarkGrey = scheme->Get(wxColourScheme::SHADOW_OUT);
    m_penDarkGrey = wxPen(m_colDarkGrey, 0, wxSOLID);

    m_colBg = scheme->Get(wxColourScheme::SHADOW_IN);
    m_penLightGrey = wxPen(m_colBg, 0, wxSOLID);

    m_colHighlight = scheme->Get(wxColourScheme::SHADOW_HIGHLIGHT);
    m_penHighlight = wxPen(m_colHighlight, 0, wxSOLID);

    // init the arrow bitmaps
    static const size_t ARROW_WIDTH = 7;
    static const size_t ARROW_LENGTH = 4;

    wxMask *mask;
    wxMemoryDC dcNormal, dcDisabled;
    for ( size_t n = 0; n < Arrow_Max; n++ )
    {
        bool isVertical = n > Arrow_Right;
        int w, h;
        if ( isVertical )
        {
            w = ARROW_WIDTH;
            h = ARROW_LENGTH;
        }
        else
        {
            h = ARROW_WIDTH;
            w = ARROW_LENGTH;
        }

        // disabled arrow is larger because of the shadow
        m_bmpArrows[Arrow_Normal][n].Create(w, h);
        m_bmpArrows[Arrow_Disabled][n].Create(w + 1, h + 1);

        dcNormal.SelectObject(m_bmpArrows[Arrow_Normal][n]);
        dcDisabled.SelectObject(m_bmpArrows[Arrow_Disabled][n]);

        dcNormal.SetBackground(*wxWHITE_BRUSH);
        dcDisabled.SetBackground(*wxWHITE_BRUSH);
        dcNormal.Clear();
        dcDisabled.Clear();

        dcNormal.SetPen(m_penBlack);
        dcDisabled.SetPen(m_penDarkGrey);

        // calculate the position of the point of the arrow
        wxCoord x1, y1;
        if ( isVertical )
        {
            x1 = (ARROW_WIDTH - 1)/2;
            y1 = n == Arrow_Up ? 0 : ARROW_LENGTH - 1;
        }
        else // horizontal
        {
            x1 = n == Arrow_Left ? 0 : ARROW_LENGTH - 1;
            y1 = (ARROW_WIDTH - 1)/2;
        }

        wxCoord x2 = x1,
                y2 = y1;

        if ( isVertical )
            x2++;
        else
            y2++;

        for ( size_t i = 0; i < ARROW_LENGTH; i++ )
        {
            dcNormal.DrawLine(x1, y1, x2, y2);
            dcDisabled.DrawLine(x1, y1, x2, y2);

            if ( isVertical )
            {
                x1--;
                x2++;

                if ( n == Arrow_Up )
                {
                    y1++;
                    y2++;
                }
                else // down arrow
                {
                    y1--;
                    y2--;
                }
            }
            else // left or right arrow
            {
                y1--;
                y2++;

                if ( n == Arrow_Left )
                {
                    x1++;
                    x2++;
                }
                else
                {
                    x1--;
                    x2--;
                }
            }
        }

        // draw the shadow for the disabled one
        dcDisabled.SetPen(m_penHighlight);
        switch ( n )
        {
            case Arrow_Left:
                y1 += 2;
                dcDisabled.DrawLine(x1, y1, x2, y2);
                break;

            case Arrow_Right:
                x1 = ARROW_LENGTH - 1;
                y1 = (ARROW_WIDTH - 1)/2 + 1;
                x2 = 0;
                y2 = ARROW_WIDTH;
                dcDisabled.DrawLine(x1, y1, x2, y2);
                dcDisabled.DrawLine(++x1, y1, x2, ++y2);
                break;

            case Arrow_Up:
                x1 += 2;
                dcDisabled.DrawLine(x1, y1, x2, y2);
                break;

            case Arrow_Down:
                x1 = ARROW_WIDTH - 1;
                y1 = 1;
                x2 = (ARROW_WIDTH - 1)/2;
                y2 = ARROW_LENGTH;
                dcDisabled.DrawLine(x1, y1, x2, y2);
                dcDisabled.DrawLine(++x1, y1, x2, ++y2);
                break;

        }

        dcNormal.SelectObject(wxNullBitmap);
        dcDisabled.SelectObject(wxNullBitmap);

        mask = new wxMask(m_bmpArrows[Arrow_Normal][n], *wxWHITE);
        m_bmpArrows[Arrow_Normal][n].SetMask(mask);
        mask = new wxMask(m_bmpArrows[Arrow_Disabled][n], *wxWHITE);
        m_bmpArrows[Arrow_Disabled][n].SetMask(mask);
    }
}

// ----------------------------------------------------------------------------
// border stuff
// ----------------------------------------------------------------------------

/*
   The raised border in Win32 looks like this:

   IIIIIIIIIIIIIIIIIIIIIIB
   I                    GB
   I                    GB  I = white       (HILIGHT)
   I                    GB  H = light grey  (LIGHT)
   I                    GB  G = dark grey   (SHADOI)
   I                    GB  B = black       (DKSHADOI)
   I                    GB  I = hIghlight (COLOR_3DHILIGHT)
   I                    GB
   IGGGGGGGGGGGGGGGGGGGGGB
   BBBBBBBBBBBBBBBBBBBBBBB

   The sunken border looks like this:

   GGGGGGGGGGGGGGGGGGGGGGI
   GBBBBBBBBBBBBBBBBBBBBHI
   GB                   HI
   GB                   HI
   GB                   HI
   GB                   HI
   GB                   HI
   GB                   HI
   GHHHHHHHHHHHHHHHHHHHHHI
   IIIIIIIIIIIIIIIIIIIIIII

   The static border (used for the controls which don't get focus) is like
   this:

   GGGGGGGGGGGGGGGGGGGGGGW
   G                     W
   G                     W
   G                     W
   G                     W
   G                     W
   G                     W
   G                     W
   WWWWWWWWWWWWWWWWWWWWWWW

   The most complicated is the double border:

   HHHHHHHHHHHHHHHHHHHHHHB
   HWWWWWWWWWWWWWWWWWWWWGB
   HWHHHHHHHHHHHHHHHHHHHGB
   HWH                 HGB
   HWH                 HGB
   HWH                 HGB
   HWH                 HGB
   HWHHHHHHHHHHHHHHHHHHHGB
   HGGGGGGGGGGGGGGGGGGGGGB
   BBBBBBBBBBBBBBBBBBBBBBB

   And the simple border is, well, simple:

   BBBBBBBBBBBBBBBBBBBBBBB
   B                     B
   B                     B
   B                     B
   B                     B
   B                     B
   B                     B
   B                     B
   B                     B
   BBBBBBBBBBBBBBBBBBBBBBB
*/

void wxWin32Renderer::DrawRect(wxDC& dc, wxRect *rect, const wxPen& pen)
{
    // draw
    dc.SetPen(pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(*rect);

    // adjust the rect
    rect->Inflate(-1);
}

void wxWin32Renderer::DrawHalfRect(wxDC& dc, wxRect *rect, const wxPen& pen)
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

void wxWin32Renderer::DrawShadedRect(wxDC& dc, wxRect *rect,
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

void wxWin32Renderer::DrawRaisedBorder(wxDC& dc, wxRect *rect)
{
    DrawShadedRect(dc, rect, m_penHighlight, m_penBlack);
    DrawShadedRect(dc, rect, m_penLightGrey, m_penDarkGrey);
}

void wxWin32Renderer::DrawArrowBorder(wxDC& dc, wxRect *rect)
{
    DrawShadedRect(dc, rect, m_penLightGrey, m_penBlack);
    DrawShadedRect(dc, rect, m_penHighlight, m_penDarkGrey);
}

void wxWin32Renderer::DrawBorder(wxDC& dc,
                                 wxBorder border,
                                 const wxRect& rectTotal,
                                 int WXUNUSED(flags),
                                 wxRect *rectIn)
{
    wxRect rect = rectTotal;

    switch ( border )
    {
        case wxBORDER_SUNKEN:
            DrawShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
            DrawShadedRect(dc, &rect, m_penBlack, m_penLightGrey);
            break;

        case wxBORDER_STATIC:
            DrawShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
            break;

        case wxBORDER_RAISED:
            DrawRaisedBorder(dc, &rect);
            break;

        case wxBORDER_DOUBLE:
            DrawArrowBorder(dc, &rect);
            DrawRect(dc, &rect, m_penLightGrey);
            break;

        case wxBORDER_SIMPLE:
            DrawRect(dc, &rect, m_penBlack);
            break;

        default:
            wxFAIL_MSG(_T("unknwon border type"));
            // fall through

        case wxBORDER_DEFAULT:
        case wxBORDER_NONE:
            break;
    }

    if ( rectIn )
        *rectIn = rect;
}

// ----------------------------------------------------------------------------
// button border
// ----------------------------------------------------------------------------

void wxWin32Renderer::DrawButtonBorder(wxDC& dc,
                                       const wxRect& rectTotal,
                                       int flags,
                                       wxRect *rectIn)
{
    wxRect rect = rectTotal;

    if ( flags & wxCONTROL_PRESSED )
    {
        // button pressed: draw a double border around it
        DrawRect(dc, &rect, m_penBlack);
        DrawRect(dc, &rect, m_penDarkGrey);
    }
    else
    {
        // button not pressed

        if ( flags & (wxCONTROL_FOCUSED | wxCONTROL_ISDEFAULT) )
        {
            // button either default or focused (or both): add an extra border around it
            DrawRect(dc, &rect, m_penBlack);
        }

        // now draw a normal button
        DrawShadedRect(dc, &rect, m_penHighlight, m_penBlack);
        DrawHalfRect(dc, &rect, m_penDarkGrey);
    }

    if ( rectIn )
    {
        *rectIn = rect;
    }
}

// ----------------------------------------------------------------------------
// frame
// ----------------------------------------------------------------------------

void wxWin32Renderer::DrawFrame(wxDC& dc,
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
    }

    // draw the frame
    DrawShadedRect(dc, &rectFrame, m_penDarkGrey, m_penHighlight);
    DrawShadedRect(dc, &rectFrame, m_penHighlight, m_penDarkGrey);

    // and overwrite it with label (if any)
    if ( !label.empty() )
    {
        // TODO: the +5 and space insertion should be customizable

        wxRect rectText;
        rectText.x = rectFrame.x + 5;
        rectText.y = rect.y;
        rectText.width = rectFrame.width - 7; // +2 border width
        rectText.height = height;

        wxString label2;
        label2 << _T(' ') << label << _T(' ');
        if ( indexAccel != -1 )
        {
            // adjust it as we prepended a space
            indexAccel++;
        }

        dc.SetBackgroundMode(wxSOLID);
        DrawLabel(dc, label2, rectText, flags, alignment, indexAccel);
        dc.SetBackgroundMode(wxTRANSPARENT);
    }
}

// ----------------------------------------------------------------------------
// label
// ----------------------------------------------------------------------------

void wxWin32Renderer::DrawLabel(wxDC& dc,
                                const wxString& label,
                                const wxRect& rect,
                                int flags,
                                int alignment,
                                int indexAccel)
{
    // shift the label if a button is pressed
    wxRect rectLabel = rect;
    if ( flags & wxCONTROL_PRESSED )
    {
        rectLabel.x++;
        rectLabel.y++;
    }

    if ( flags & wxCONTROL_DISABLED )
    {
        // make the text grey and draw a shade for it
        dc.SetTextForeground(m_colHighlight);
        wxRect rectShadow = rectLabel;
        rectShadow.x++;
        rectShadow.y++;
        dc.DrawLabel(label, rectShadow, alignment, indexAccel);
        dc.SetTextForeground(m_colDarkGrey);
    }

    wxRect rectText = rectLabel;
    if ( flags & wxCONTROL_FOCUSED )
    {
        rectText.Inflate(-2);
    }

    dc.DrawLabel(label, rectText, alignment, indexAccel);

    if ( flags & wxCONTROL_FOCUSED )
    {
        // VZ: this doesn't work under Windows, the dotted pen has dots of 3
        //     pixels each while we really need dots here... PS_ALTERNATE might
        //     work, but it is for NT 5 only
#if 0
        DrawRect(dc, &rectText, wxPen(*wxBLACK, 0, wxDOT));
#else
        // draw the pixels manually
        dc.SetPen(wxPen(*wxBLACK, 0, wxSOLID));

        // Windows quirk: appears to draw them like this, from right to left
        // (and I don't have Hebrew windows to see what happens there)
        for ( wxCoord x = rectText.GetRight(); x >= rectText.GetLeft(); x -= 2 )
        {
            dc.DrawPoint(x, rectText.GetTop());
            dc.DrawPoint(x, rectText.GetBottom());
        }

        wxCoord shift = rectText.width % 2 ? 0 : 1;
        for ( wxCoord y = rectText.GetTop() + 2; y <= rectText.GetBottom(); y+= 2 )
        {
            dc.DrawPoint(rectText.GetLeft(), y - shift);
            dc.DrawPoint(rectText.GetRight(), y);
        }

        if ( shift )
        {
            dc.DrawPoint(rectText.GetLeft(), rectText.GetBottom() - 1);
        }
#endif // 0/1
    }
}

// ----------------------------------------------------------------------------
// background
// ----------------------------------------------------------------------------

void wxWin32Renderer::DrawBackground(wxDC& dc,
                                     const wxColour& col,
                                     const wxRect& rect,
                                     int flags)
{
    // just fill it with the current colour
    wxBrush brush(col, wxSOLID);
    dc.SetBrush(brush);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect);
}

// ----------------------------------------------------------------------------
// scrollbar
// ----------------------------------------------------------------------------

void wxWin32Renderer::DrawArrow(wxDC& dc,
                                wxDirection dir,
                                const wxRect& rect,
                                int flags)
{
    // get the bitmap for this arrow
    wxArrowDirection arrowDir;
    switch ( dir )
    {
        case wxLEFT:    arrowDir = Arrow_Left; break;
        case wxRIGHT:   arrowDir = Arrow_Right; break;
        case wxUP:      arrowDir = Arrow_Up; break;
        case wxDOWN:    arrowDir = Arrow_Down; break;

        default:
            wxFAIL_MSG(_T("unknown arrow direction"));
            return;
    }

    wxArrowStyle arrowStyle = flags & wxCONTROL_DISABLED ? Arrow_Disabled
                                                         : Arrow_Normal;
    DrawArrow(dc, rect, arrowDir, arrowStyle);
}

void wxWin32Renderer::DrawArrow(wxDC& dc,
                                const wxRect& rect,
                                wxArrowDirection arrowDir,
                                wxArrowStyle arrowStyle)
{
    const wxBitmap& bmp = m_bmpArrows[arrowStyle][arrowDir];

    // under Windows the arrows always have the same size so just centre it in
    // the provided rectangle
    wxCoord x = rect.x + (rect.width - bmp.GetWidth()) / 2,
            y = rect.y + (rect.height - bmp.GetHeight()) / 2;

    // Windows does it like this...
    if ( arrowDir == Arrow_Left )
        x--;

    // draw it
    dc.DrawBitmap(bmp, x, y, TRUE /* use mask */);
}

void wxWin32Renderer::DrawArrowButton(wxDC& dc,
                                      const wxRect& rectAll,
                                      wxArrowDirection arrowDir,
                                      wxArrowStyle arrowStyle)
{
    wxRect rect = rectAll;
    DrawArrowBorder(dc, &rect);
    DrawArrow(dc, rect, arrowDir, arrowStyle);
}

void wxWin32Renderer::DrawScrollbar(wxDC& dc,
                                    wxOrientation orient,
                                    int thumbPosStart,
                                    int thumbPosEnd,
                                    const wxRect& rect,
                                    int flags)
{
    wxArrowStyle arrowStyle = flags & wxCONTROL_DISABLED ? Arrow_Disabled
                                                         : Arrow_Normal;

    // first, draw the arrows at the ends
    wxRect rectArrow[2];
    wxArrowDirection arrowDir[2];

    rectArrow[0] =
    rectArrow[1] = rect;
    if ( orient == wxVERTICAL )
    {
        rectArrow[0].height =
        rectArrow[1].height = m_sizeScrollbarArrow.y;
        rectArrow[1].y = rect.GetBottom() - rectArrow[1].height;

        arrowDir[0] = Arrow_Up;
        arrowDir[1] = Arrow_Down;
    }
    else // horizontal
    {
        rectArrow[0].width =
        rectArrow[1].width = m_sizeScrollbarArrow.x;
        rectArrow[1].x = rect.GetRight() - rectArrow[1].width;

        arrowDir[0] = Arrow_Left;
        arrowDir[1] = Arrow_Right;
    }

    for ( size_t nArrow = 0; nArrow < 2; nArrow++ )
    {
        DrawArrowButton(dc, rectArrow[nArrow], arrowDir[nArrow], arrowStyle);
    }

    // next draw the scrollbar area
    wxRect rectBar = rect;
    if ( orient == wxVERTICAL )
        rectBar.Inflate(0, -m_sizeScrollbarArrow.y);
    else
        rectBar.Inflate(-m_sizeScrollbarArrow.x, 0);

    DrawBackground(dc, m_colHighlight, rectBar);

    // and, finally, the thumb, if any
    if ( thumbPosStart < thumbPosEnd )
    {
        wxRect rectThumb = rectBar;
        if ( orient == wxVERTICAL )
        {
            rectThumb.y += (rectBar.height*thumbPosStart)/100;
            rectThumb.height = (rectBar.height*(thumbPosEnd - thumbPosStart))/100;
        }
        else // horizontal
        {
            rectThumb.x += (rectBar.width*thumbPosStart)/100;
            rectThumb.width = (rectBar.width*(thumbPosEnd - thumbPosStart))/100;
        }

        DrawArrowBorder(dc, &rectThumb);
        DrawBackground(dc, m_colBg, rectThumb);
    }
}

// ----------------------------------------------------------------------------
// size adjustments
// ----------------------------------------------------------------------------

void wxWin32Renderer::AdjustSize(wxSize *size, const wxWindow *window)
{
    if ( wxDynamicCast(window, wxScrollBar) )
    {
        // we only set the width of vert scrollbars and height of the
        // horizontal ones
        if ( window->GetWindowStyle() & wxSB_HORIZONTAL )
            size->y = m_sizeScrollbarArrow.y;
        else
            size->x = m_sizeScrollbarArrow.x;
    }
    else if ( wxDynamicCast(window, wxButton) )
    {
        // TODO
        size->x += 3*window->GetCharWidth();
        size->y = (11*(window->GetCharHeight() + 8))/10;
    }
    else
    {
        // take into account the border width
        wxBorder border = (wxBorder)(window->GetWindowStyle() & wxBORDER_MASK);
        switch ( border )
        {
            case wxBORDER_SUNKEN:
            case wxBORDER_RAISED:
                size->x += 4;
                size->y += 4;
                break;

            case wxBORDER_SIMPLE:
            case wxBORDER_STATIC:
                size->x += 2;
                size->y += 2;
                break;

            case wxBORDER_DOUBLE:
                size->x += 6;
                size->y += 6;
                break;

            default:
                wxFAIL_MSG(_T("unknwon border type"));
                // fall through

            case wxBORDER_DEFAULT:
            case wxBORDER_NONE:
                break;
        }
    }
}

// ============================================================================
// wxInputHandler
// ============================================================================

// ----------------------------------------------------------------------------
// wxWin32InputHandler
// ----------------------------------------------------------------------------

wxControlActions wxWin32InputHandler::Map(const wxKeyEvent& event, bool pressed)
{
    return wxACTION_NONE;
}

wxControlActions wxWin32InputHandler::Map(const wxMouseEvent& event)
{
    return wxACTION_NONE;
}

// ----------------------------------------------------------------------------
// wxWin32ButtonInputHandler
// ----------------------------------------------------------------------------

wxWin32ButtonInputHandler::wxWin32ButtonInputHandler()
{
    m_winCapture = NULL;
}

wxControlActions wxWin32ButtonInputHandler::Map(const wxKeyEvent& event,
                                                bool pressed)
{
    int keycode = event.GetKeyCode();
    if ( keycode == WXK_SPACE || keycode == WXK_RETURN )
    {
        return wxACTION_BUTTON_TOGGLE;
    }

    return wxWin32InputHandler::Map(event, pressed);
}

wxControlActions wxWin32ButtonInputHandler::Map(const wxMouseEvent& event)
{
    if ( event.IsButton() )
    {
        if ( event.ButtonDown() )
        {
            m_winCapture = wxWindow::FindFocus();
            m_winCapture->CaptureMouse();
        }
        else // up
        {
            m_winCapture->ReleaseMouse();
        }

        return wxACTION_BUTTON_TOGGLE;
    }

    return wxWin32InputHandler::Map(event);
}
