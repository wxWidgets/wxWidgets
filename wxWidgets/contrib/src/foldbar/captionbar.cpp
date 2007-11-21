/////////////////////////////////////////////////////////////////////////////
// Name:        captionbar.cpp
// Purpose:     wxCaptionBar class belonging to the wxFoldPanel (but can be used independent)
// Author:      Jorgen Bodde
// Modified by: ABX - 19/12/2004 : possibility of horizontal orientation
//                               : wxWidgets coding standards
// Created:     18/06/2004
// RCS-ID:      $Id$
// Copyright:   (c) Jorgen Bodde
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/foldbar/foldpanelbar.h"

/*
 * wxCaptionBar
 */

BEGIN_EVENT_TABLE(wxCaptionBar, wxWindow)
    EVT_PAINT(wxCaptionBar::OnPaint)
    EVT_CHAR(wxCaptionBar::OnChar)
    EVT_MOUSE_EVENTS(wxCaptionBar::OnMouseEvent)
    EVT_SIZE(wxCaptionBar::OnSize)
END_EVENT_TABLE()

wxCaptionBar::wxCaptionBar(wxWindow* parent, const wxString &caption, wxImageList *images, wxWindowID id,
                           const wxCaptionBarStyle &cbstyle, const wxPoint& pos, const wxSize& size, long style)
    : wxWindow(parent, id, pos, size, style)
    , m_caption(caption)
    , m_foldIcons(images)
    , m_rightIndent(wxFPB_BMP_RIGHTSPACE)
    , m_iconWidth(16)
    , m_iconHeight(16)
    , m_collapsed(false)
{
    // do initialisy thingy stuff

    ApplyCaptionStyle(cbstyle, true);

    // set initial size
    if(m_foldIcons)
    {
        wxASSERT(m_foldIcons->GetImageCount() > 1);
        m_foldIcons->GetSize(0, m_iconWidth, m_iconHeight);
    }
}

wxCaptionBar::~wxCaptionBar()
{

}

void wxCaptionBar::ApplyCaptionStyle(const wxCaptionBarStyle &cbstyle, bool applyDefault)
{
    wxASSERT(GetParent());

    wxCaptionBarStyle newstyle = cbstyle;

    // set defaults in newly created style copy if needed
    if(applyDefault)
    {
        // get first colour from style or make it default
        if(!newstyle.FirstColourUsed())
            newstyle.SetFirstColour(*wxWHITE);

        // get second colour from style or make it default
        if(!newstyle.SecondColourUsed())
        {
            // make the second colour slightly darker then the background
            wxColour col = GetParent()->GetBackgroundColour();
            col.Set((unsigned char)((col.Red() >> 1) + 20),
                    (unsigned char)((col.Green() >> 1) + 20),
                    (unsigned char)((col.Blue() >> 1) + 20));
            newstyle.SetSecondColour(col);
        }

        // get text colour
        if(!newstyle.CaptionColourUsed())
            newstyle.SetCaptionColour(*wxBLACK);

        // get font colour
        if(!newstyle.CaptionFontUsed())
            newstyle.SetCaptionFont(GetParent()->GetFont());

        // apply caption style
        if(!newstyle.CaptionStyleUsed())
            newstyle.SetCaptionStyle(wxCAPTIONBAR_GRADIENT_V);
    }

    // apply the style
    m_captionStyle = newstyle;
}

void wxCaptionBar::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    wxRect wndRect = GetRect();
    bool vertical = IsVertical();

    // TODO: Maybe first a memory DC should draw all, and then paint it on the
    // caption. This way a flickering arrow during resize is not visible

    // draw basics

    FillCaptionBackground(dc);

    dc.SetFont(m_captionStyle.GetCaptionFont());
    if(vertical)
        dc.DrawText(m_caption, 4, (wxFPB_EXTRA_Y / 2));
    else
        dc.DrawRotatedText(m_caption, (wxFPB_EXTRA_Y / 2) , wndRect.GetBottom() - 4 , 90 );

    // draw small icon, either collapsed or expanded
    // based on the state of the bar. If we have
    // any bmp's

    if(m_foldIcons)
    {
        wxCHECK2(m_foldIcons->GetImageCount() > 1, return);

        int index = 0;
        if(m_collapsed)
            index = 1;

        if(vertical)
            m_foldIcons->Draw(index,
                              dc,
                              wndRect.GetRight() - m_iconWidth - m_rightIndent,
                              (wndRect.GetHeight() - m_iconHeight) / 2,
                              wxIMAGELIST_DRAW_TRANSPARENT);
        else
            m_foldIcons->Draw(index,
                              dc,
                              (wndRect.GetWidth() - m_iconWidth) / 2,
                              m_rightIndent,
                              wxIMAGELIST_DRAW_TRANSPARENT);
    }
}

void wxCaptionBar::FillCaptionBackground(wxPaintDC &dc)
{
    // dispatch right style for caption drawing

    switch(m_captionStyle.GetCaptionStyle())
    {
    case wxCAPTIONBAR_GRADIENT_V:
        if (IsVertical())
            DrawVerticalGradient(dc, GetRect());
        else
            DrawHorizontalGradient(dc, GetRect());
        break;
    case wxCAPTIONBAR_GRADIENT_H:
        if (IsVertical())
            DrawHorizontalGradient(dc, GetRect());
        else
            DrawVerticalGradient(dc, GetRect());
        break;
    case wxCAPTIONBAR_SINGLE:
        DrawSingleColour(dc, GetRect());
        break;
    case wxCAPTIONBAR_RECTANGLE:
    case wxCAPTIONBAR_FILLED_RECTANGLE:
        DrawSingleRectangle(dc, GetRect());
        break;
    default:
        break;
    }
}

void wxCaptionBar::OnMouseEvent(wxMouseEvent& event)
{
    // if clicked on the arrow (single) or double on the caption
    // we change state and an event must be fired to let this
    // panel collapse or expand

    bool send_event = false;

    if (event.LeftDown() && m_foldIcons)
    {
        wxPoint pt(event.GetPosition());
        wxRect rect = GetRect();
        bool vertical = IsVertical();

        if((vertical && pt.x > (rect.GetWidth() - m_iconWidth - m_rightIndent))||
           (!vertical && pt.y < m_iconHeight + m_rightIndent))
            send_event = true;
    }
    else if(event.LeftDClick())
        send_event = true;

    // send the collapse, expand event to the parent

    if(send_event)
    {
        wxCaptionBarEvent event(wxEVT_CAPTIONBAR);
        event.SetCaptionBar(this);

        ::wxPostEvent(this, event);

    }
}

void wxCaptionBar::OnChar(wxKeyEvent &event)
{
    // TODO: Anything here?

    event.Skip();
}

wxSize wxCaptionBar::DoGetBestSize() const
{
    int x,y;

    if(IsVertical())
        GetTextExtent(m_caption, &x, &y);
    else
        GetTextExtent(m_caption, &y, &x);

    if(x < m_iconWidth)
        x = m_iconWidth;

    if(y < m_iconHeight)
        y = m_iconHeight;

    // TODO: The extra wxFPB_EXTRA_X constants should be adjustable as well

    return wxSize(x + wxFPB_EXTRA_X, y + wxFPB_EXTRA_Y);
}


void wxCaptionBar::DrawVerticalGradient(wxDC &dc, const wxRect &rect )
{
    // gradient fill from colour 1 to colour 2 with top to bottom

    if(rect.height < 1 || rect.width < 1)
        return;

    int size = rect.height;

    dc.SetPen(*wxTRANSPARENT_PEN);


    // calculate gradient coefficients
    wxColour col2 = m_captionStyle.GetSecondColour(),
             col1 = m_captionStyle.GetFirstColour();

    double rstep = double((col2.Red() -   col1.Red())) / double(size), rf = 0,
           gstep = double((col2.Green() - col1.Green())) / double(size), gf = 0,
           bstep = double((col2.Blue() -  col1.Blue())) / double(size), bf = 0;

    wxColour currCol;
    for(int y = rect.y; y < rect.y + size; y++)
    {
        currCol.Set(
            (unsigned char)(col1.Red() + rf),
            (unsigned char)(col1.Green() + gf),
            (unsigned char)(col1.Blue() + bf)
        );
        dc.SetBrush( wxBrush( currCol, wxSOLID ) );
        dc.DrawRectangle( rect.x, rect.y + (y - rect.y), rect.width, size );
        //currCol.Set(currCol.Red() + rstep, currCol.Green() + gstep, currCol.Blue() + bstep);
        rf += rstep; gf += gstep; bf += bstep;
    }
}

void wxCaptionBar::DrawHorizontalGradient(wxDC &dc, const wxRect &rect )
{
    // gradient fill from colour 1 to colour 2 with left to right

    if(rect.height < 1 || rect.width < 1)
        return;

    int size = rect.width;

    dc.SetPen(*wxTRANSPARENT_PEN);

    // calculate gradient coefficients
    wxColour col2 = m_captionStyle.GetSecondColour(),
             col1 = m_captionStyle.GetFirstColour();

    double rstep = double((col2.Red() -   col1.Red())) / double(size), rf = 0,
           gstep = double((col2.Green() - col1.Green())) / double(size), gf = 0,
           bstep = double((col2.Blue() -  col1.Blue())) / double(size), bf = 0;

    wxColour currCol;
    for(int x = rect.x; x < rect.x + size; x++)
    {
        currCol.Set(
            (unsigned char)(col1.Red() + rf),
            (unsigned char)(col1.Green() + gf),
            (unsigned char)(col1.Blue() + bf)
        );
        dc.SetBrush( wxBrush( currCol, wxSOLID ) );
        dc.DrawRectangle( rect.x + (x - rect.x), rect.y, 1, rect.height );
        rf += rstep; gf += gstep; bf += bstep;
    }
}

void wxCaptionBar::DrawSingleColour(wxDC &dc, const wxRect &rect )
{
    // single colour fill. This is the most easy one to find

    if(rect.height < 1 || rect.width < 1)
        return;

    dc.SetPen(*wxTRANSPARENT_PEN);

    // draw simple rectangle
    dc.SetBrush( wxBrush( m_captionStyle.GetFirstColour(), wxSOLID ) );
    dc.DrawRectangle( rect.x, rect.y, rect.width, rect.height );
}

void wxCaptionBar::DrawSingleRectangle(wxDC &dc, const wxRect &rect )
{
    wxASSERT(GetParent());

    // single colour fill. This is the most easy one to find

    if(rect.height < 2 || rect.width < 1)
        return;

    // single frame, set up internal fill colour

    wxBrush br;
    br.SetStyle(wxSOLID);

    if(m_captionStyle.GetCaptionStyle() == wxCAPTIONBAR_RECTANGLE)
        br.SetColour(GetParent()->GetBackgroundColour());
    else
        br.SetColour(m_captionStyle.GetFirstColour());

    // setup the pen frame

    wxPen pen(m_captionStyle.GetSecondColour());
    dc.SetPen(pen);

    dc.SetBrush( br );
    dc.DrawRectangle( rect.x, rect.y, rect.width, rect.height - 1);

    wxPen bgpen(GetParent()->GetBackgroundColour());
    dc.SetPen(bgpen);
    dc.DrawLine(rect.x, rect.y + rect.height - 1, rect.x + rect.width, rect.y + rect.height - 1);
}


void wxCaptionBar::OnSize(wxSizeEvent &event)
{
    wxSize size = event.GetSize();

    if(m_foldIcons)
    {
        // What I am doing here is simply invalidating the part of the window exposed. So when I
        // make a rect with as width the newly exposed part, and the x,y of the old window size origin,
        // I don't need a bitmap calulation in it, or do I ? The bitmap needs redrawing anyway. Leave it
        // like this until I figured it out

        // set rect to redraw as old bitmap area which is entitled to redraw

        wxRect rect(size.GetWidth() - m_iconWidth - m_rightIndent, 0, m_iconWidth + m_rightIndent,
                    m_iconWidth + m_rightIndent);

        // adjust rectangle when more is slided so we need to redraw all
        // the old stuff but not all (ugly flickering)

        int diffX = size.GetWidth() - m_oldSize.GetWidth();
        if(diffX > 1)
        {
            // adjust the rect with all the crap to redraw

            rect.SetWidth(rect.GetWidth() + diffX + 10);
            rect.SetX(rect.GetX() - diffX - 10);
        }

        RefreshRect(rect);
    }
    else
    {
        wxRect rect = GetRect();
        RefreshRect(rect);
    }

    m_oldSize = size;
}

void wxCaptionBar::RedrawIconBitmap()
{
    if(m_foldIcons)
    {
        // invalidate the bitmap area and force a redraw

        wxRect rect = GetRect();

        rect.SetX(rect.GetWidth() - m_iconWidth - m_rightIndent);
        rect.SetWidth(m_iconWidth + m_rightIndent);
        RefreshRect(rect);
    }
}

bool wxCaptionBar::IsVertical() const
{
    // parent of wxCaptionBar is wxFoldPanelItem
    // default is vertical
    wxFoldPanelItem *bar = wxDynamicCast(GetParent(), wxFoldPanelItem);
    wxCHECK_MSG( bar, true, _T("wrong parent") );
    return bar->IsVertical();
}

/*
 * wxCaptionBarEvent
 */

DEFINE_EVENT_TYPE(wxEVT_CAPTIONBAR)

wxCaptionBarEvent::wxCaptionBarEvent(const wxCaptionBarEvent &event)
    : wxCommandEvent(event)
{
    m_captionBar = event.m_captionBar;
}

//DEFINE_EVENT_TYPE(wxEVT_CAPTIONBAR)
//IMPLEMENT_DYNAMIC_CLASS(wxCaptionBarEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxCaptionBarEvent, wxCommandEvent)
