///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/bannerwindow.cpp
// Purpose:     wxBannerWindow class implementation
// Author:      Vadim Zeitlin
// Created:     2011-08-16
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_BANNERWINDOW

#include "wx/bannerwindow.h"

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/colour.h"
#endif

#include "wx/dcbuffer.h"

namespace
{

// Some constants for banner layout, currently they're hard coded but we could
// easily make them configurable if needed later.
const int MARGIN_X = 5;
const int MARGIN_Y = 5;

} // anonymous namespace

const char wxBannerWindowNameStr[] = "bannerwindow";

wxBEGIN_EVENT_TABLE(wxBannerWindow, wxWindow)
    EVT_SIZE(wxBannerWindow::OnSize)
    EVT_DPI_CHANGED(wxBannerWindow::OnDPIChanged)
    EVT_PAINT(wxBannerWindow::OnPaint)
wxEND_EVENT_TABLE()

void wxBannerWindow::Init()
{
    m_direction = wxLEFT;

    m_colStart = *wxWHITE;
    m_colEnd = *wxBLUE;
}

bool
wxBannerWindow::Create(wxWindow* parent,
                       wxWindowID winid,
                       wxDirection dir,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    wxCHECK_MSG(dir == wxLEFT || dir == wxRIGHT ||
                    dir == wxTOP || dir == wxBOTTOM,
                false, wxS("Invalid banner direction"));

    if ( !wxWindow::Create(parent, winid, pos, size, style, name) )
        return false;

    m_direction = dir;

    SetBackgroundStyle(wxBG_STYLE_PAINT);

    return true;
}

void wxBannerWindow::SetBitmap(const wxBitmapBundle& bmp)
{
    m_bitmapBundle = bmp;

    m_colBitmapBg = wxColour();

    InvalidateBestSize();

    Refresh();
}

void wxBannerWindow::SetText(const wxString& title, const wxString& message)
{
    m_title = title;
    m_message = message;

    // wxBannerWindow is custom-painted, so publish its visible text through
    // the ordinary wxWindow name/label path used by accessibility backends.
    SetLabel(title.empty() ? message :
             message.empty() ? title : title + "\n" + message);

    InvalidateBestSize();

    Refresh();
}

void wxBannerWindow::SetGradient(const wxColour& start, const wxColour& end)
{
    m_colStart = start;
    m_colEnd = end;

    Refresh();
}

wxFont wxBannerWindow::GetTitleFont() const
{
    wxFont font = GetFont();
    font.MakeBold().MakeLarger();
    return font;
}

wxSize wxBannerWindow::DoGetBestClientSize() const
{
    if ( m_bitmapBundle.IsOk() )
    {
        return m_bitmapBundle.GetPreferredLogicalSizeFor(this);
    }
    else
    {
        wxInfoDC dc(const_cast<wxBannerWindow *>(this));
        const wxSize sizeText = dc.GetMultiLineTextExtent(m_message);

        dc.SetFont(GetTitleFont());

        const wxSize sizeTitle = dc.GetTextExtent(m_title);

        wxSize sizeWin(wxMax(sizeTitle.x, sizeText.x), sizeTitle.y + sizeText.y);

        // If we draw the text vertically width and height are swapped.
        if ( m_direction == wxLEFT || m_direction == wxRIGHT )
            wxSwap(sizeWin.x, sizeWin.y);

        sizeWin += 2*FromDIP(wxSize(MARGIN_X, MARGIN_Y));

        return sizeWin;
    }
}

void wxBannerWindow::OnSize(wxSizeEvent& event)
{
    Refresh();

    event.Skip();
}

void wxBannerWindow::OnDPIChanged(wxDPIChangedEvent& event)
{
    m_colBitmapBg = wxColour();
    InvalidateBestSize();
    Refresh();
    event.Skip();
}

void wxBannerWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    if ( m_bitmapBundle.IsOk() && m_title.empty() && m_message.empty() )
    {
        // No need for buffering in this case.
        wxPaintDC dc(this);

        DrawBitmapBackground(dc);
    }
    else // We need to compose our contents ourselves.
    {
        wxAutoBufferedPaintDC dc(this);

        // Deal with the background first.
        if ( m_bitmapBundle.IsOk() )
        {
            DrawBitmapBackground(dc);
        }
        else // Draw gradient background.
        {
            wxDirection gradientDir;
            if ( m_direction == wxLEFT )
            {
                gradientDir = wxTOP;
            }
            else if ( m_direction == wxRIGHT )
            {
                gradientDir = wxBOTTOM;
            }
            else // For both wxTOP and wxBOTTOM.
            {
                gradientDir = wxRIGHT;
            }

            dc.GradientFillLinear(GetClientRect(), m_colStart, m_colEnd,
                                  gradientDir);
        }

        // Now draw the text on top of it.
        dc.SetFont(GetTitleFont());

        const wxSize margin = FromDIP(wxSize(MARGIN_X, MARGIN_Y));
        wxPoint pos(margin.x, margin.y);
        DrawBannerTextLine(dc, m_title, pos);
        pos.y += dc.GetTextExtent(m_title).y;

        dc.SetFont(GetFont());

        wxArrayString lines = wxSplit(m_message, '\n', '\0');
        const unsigned numLines = lines.size();
        for ( unsigned n = 0; n < numLines; n++ )
        {
            const wxString& line = lines[n];

            DrawBannerTextLine(dc, line, pos);
            pos.y += dc.GetTextExtent(line).y;
        }
    }
}

wxColour wxBannerWindow::GetBitmapBg()
{
    if ( m_colBitmapBg.IsOk() )
        return m_colBitmapBg;

    // Determine the colour to use to extend the bitmap. It's the colour of the
    // bitmap pixels at the edge closest to the area where it can be extended.
    const wxBitmap bitmap = m_bitmapBundle.GetBitmapFor(this);
    if ( !bitmap.IsOk() )
        return GetBackgroundColour();

    wxImage image(bitmap.ConvertToImage());
    if ( !image.IsOk() || image.GetWidth() <= 0 || image.GetHeight() <= 0 )
        return GetBackgroundColour();

    // The point we get the colour from. The choice is arbitrary and in general
    // the bitmap should have the same colour on the entire edge of this point
    // for extending it to look good.
    wxPoint p;

    wxSize size = image.GetSize();
    size.x--;
    size.y--;

    switch ( m_direction )
    {
        case wxTOP:
        case wxBOTTOM:
            // The bitmap will be extended to the right.
            p.x = size.x;
            p.y = 0;
            break;

        case wxLEFT:
            // The bitmap will be extended from the top.
            p.x = 0;
            p.y = 0;
            break;

        case wxRIGHT:
            // The bitmap will be extended to the bottom.
            p.x = 0;
            p.y = size.y;
            break;

        // This case is there only to prevent g++ warnings about not handling
        // some enum elements in the switch, it can't really happen.
        case wxALL:
            wxFAIL_MSG( wxS("Unreachable") );
    }

    m_colBitmapBg.Set(image.GetRed(p.x, p.y),
                      image.GetGreen(p.x, p.y),
                      image.GetBlue(p.x, p.y));

    return m_colBitmapBg;
}

void wxBannerWindow::DrawBitmapBackground(wxDC& dc)
{
    const wxSize size = GetClientSize();
    const wxBitmap currentBitmap = m_bitmapBundle.GetBitmapFor(this);
    if ( !currentBitmap.IsOk() )
    {
        dc.SetBackground(wxBrush(GetBackgroundColour()));
        dc.Clear();
        return;
    }

    // Fill the entire client area first. The historical code filled just the
    // strip in the banner's extension direction and could therefore leave an
    // unpainted perpendicular strip whenever both client dimensions exceeded
    // the bitmap dimensions.
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(GetBitmapBg());
    dc.DrawRectangle(0, 0, size.x, size.y);

    switch ( m_direction )
    {
        case wxTOP:
        case wxBOTTOM:
            // Draw the bitmap at the origin, its rightmost could be truncated,
            // as it's meant to be.
            dc.DrawBitmap(currentBitmap, 0, 0);
            break;

        case wxLEFT:
            // The top most part of the bitmap may be truncated but its bottom
            // must be always visible so intentionally draw it possibly partly
            // outside of the window.
            dc.DrawBitmap(currentBitmap, 0,
                          size.y - currentBitmap.GetLogicalHeight());
            break;

        case wxRIGHT:
            // Draw the bitmap at the origin, possibly truncating its
            // bottommost part.
            dc.DrawBitmap(currentBitmap, 0, 0);
            break;

        // This case is there only to prevent g++ warnings about not handling
        // some enum elements in the switch, it can't really happen.
        case wxALL:
            wxFAIL_MSG( wxS("Unreachable") );
    }
}

void
wxBannerWindow::DrawBannerTextLine(wxDC& dc,
                                   const wxString& str,
                                   const wxPoint& pos)
{
    switch ( m_direction )
    {
        case wxTOP:
        case wxBOTTOM:
            // The simple case: we just draw the text normally.
            dc.DrawText(str, pos);
            break;

        case wxLEFT:
            // We draw the text vertically and start from the lower left
            // corner and not the upper left one as usual.
            dc.DrawRotatedText(str, pos.y, GetClientSize().y - pos.x, 90);
            break;

        case wxRIGHT:
            // We also draw the text vertically but now we start from the upper
            // right corner and draw it from top to bottom.
            dc.DrawRotatedText(str, GetClientSize().x - pos.y, pos.x, -90);
            break;

        case wxALL:
            wxFAIL_MSG( wxS("Unreachable") );
    }
}

#endif // wxUSE_BANNERWINDOW
