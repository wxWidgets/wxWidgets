/////////////////////////////////////////////////////////////////////////////
// Name:        captionbar.cpp
// Purpose:     wxCaptionBar class belonging to the wxFoldPanel (but can be used independent)
// Author:      Jorgen Bodde
// Modified by:
// Created:     18/06/2004
// RCS-ID:      $Id$
// Copyright:   (c) Jorgen Bodde
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "foldpanelbar.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/dcmemory.h"
#include "wx/dcclient.h"
#endif

#include <wx/app.h>

#include "wx/foldbar/captionbar.h"

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
    , _caption(caption)
    , _foldIcons(images)
    , _rightIndent(wxFPB_BMP_RIGHTSPACE)
    , _iconWidth(16)
    , _iconHeight(16)
    , _collapsed(false)
{
    // do initialisy thingy stuff

    ApplyCaptionStyle(cbstyle, true);

    // set initial size
    if(_foldIcons)
    {
        wxASSERT(_foldIcons->GetImageCount() > 1);
        _foldIcons->GetSize(0, _iconWidth, _iconHeight);
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
            col.Set((col.Red() >> 1) + 20, (col.Green() >> 1) + 20, (col.Blue() >> 1) + 20);
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
    _style = newstyle;
}

void wxCaptionBar::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    // TODO: Maybe first a memory DC should draw all, and then paint it on the
    // caption. This way a flickering arrow during resize is not visible

    // draw basics

    FillCaptionBackground(dc);

    dc.SetFont(_style.GetCaptionFont());
    dc.DrawText(_caption, 4, (wxFPB_EXTRA_Y / 2));

    // draw small icon, either collapsed or expanded
    // based on the state of the bar. If we have
    // any bmp's

    if(_foldIcons)
    {
        wxCHECK2(_foldIcons->GetImageCount() > 1, return);

        int index = 0;
        if(_collapsed)
            index = 1;

        wxRect wndRect = GetRect();
        _foldIcons->Draw(index, dc, wndRect.GetRight() - _iconWidth - _rightIndent, (wndRect.GetHeight() - _iconHeight) / 2,
                         wxIMAGELIST_DRAW_TRANSPARENT);
    }
}

void wxCaptionBar::FillCaptionBackground(wxPaintDC &dc)
{
    // dispatch right style for caption drawing

    switch(_style.GetCaptionStyle())
    {
    case wxCAPTIONBAR_GRADIENT_V:
        DrawVerticalGradient(dc, GetRect());
        break;
    case wxCAPTIONBAR_GRADIENT_H:
        DrawHorizontalGradient(dc, GetRect());
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

    if (event.LeftDown() && _foldIcons)
    {
        wxPoint pt(event.GetPosition());
        wxRect rect = GetRect();

        if(pt.x > (rect.GetWidth() - _iconWidth - _rightIndent))
            send_event = true;
    }
    else if(event.LeftDClick())
        send_event = true;

    // send the collapse, expand event to the parent

    if(send_event)
    {
        wxCaptionBarEvent event(wxEVT_CAPTIONBAR);
        event.SetBar(this);

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

    GetTextExtent(_caption, &x, &y);

    if(x < _iconWidth)
        x = _iconWidth;

    if(y < _iconHeight)
        y = _iconHeight;

    // TODO: The extra wxFPB_EXTRA_X constants should be adjustable as well

    return wxSize(x + wxFPB_EXTRA_X, y + wxFPB_EXTRA_Y);
}


void wxCaptionBar::DrawVerticalGradient(wxDC &dc, const wxRect &rect )
{
    // gradient fill from colour 1 to colour 2 with top to bottom

    if(rect.height < 1 || rect.width < 1)
        return;

    dc.SetPen(*wxTRANSPARENT_PEN);


    // calculate gradient coefficients
    wxColour col2 = _style.GetSecondColour(),
             col1 = _style.GetFirstColour();

    double rstep = double((col2.Red() -   col1.Red())) / double(rect.height), rf = 0,
           gstep = double((col2.Green() - col1.Green())) / double(rect.height), gf = 0,
           bstep = double((col2.Blue() -  col1.Blue())) / double(rect.height), bf = 0;

    wxColour currCol;
    for(int y = rect.y; y < rect.y + rect.height; y++)
    {
        currCol.Set(
            (unsigned char)(col1.Red() + rf),
            (unsigned char)(col1.Green() + gf), 
            (unsigned char)(col1.Blue() + bf)
        );
        dc.SetBrush( wxBrush( currCol, wxSOLID ) );
        dc.DrawRectangle( rect.x, rect.y + (y - rect.y), rect.width, rect.height );
        //currCol.Set(currCol.Red() + rstep, currCol.Green() + gstep, currCol.Blue() + bstep);
        rf += rstep; gf += gstep; bf += bstep;
    }
}

void wxCaptionBar::DrawHorizontalGradient(wxDC &dc, const wxRect &rect )
{
    // gradient fill from colour 1 to colour 2 with left to right

    if(rect.height < 1 || rect.width < 1)
        return;

    dc.SetPen(*wxTRANSPARENT_PEN);

    // calculate gradient coefficients
    wxColour col2 = _style.GetSecondColour(),
             col1 = _style.GetFirstColour();

    double rstep = double((col2.Red() -   col1.Red())) / double(rect.width), rf = 0,
           gstep = double((col2.Green() - col1.Green())) / double(rect.width), gf = 0,
           bstep = double((col2.Blue() -  col1.Blue())) / double(rect.width), bf = 0;

    wxColour currCol;
    for(int x = rect.x; x < rect.x + rect.width; x++)
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
    dc.SetBrush( wxBrush( _style.GetFirstColour(), wxSOLID ) );
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

    if(_style.GetCaptionStyle() == wxCAPTIONBAR_RECTANGLE)
        br.SetColour(GetParent()->GetBackgroundColour());
    else
        br.SetColour(_style.GetFirstColour());

    // setup the pen frame

    wxPen pen(_style.GetSecondColour());
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

    if(_foldIcons)
    {
        // What I am doing here is simply invalidating the part of the window exposed. So when I
        // make a rect with as width the newly exposed part, and the x,y of the old window size origin,
        // I don't need a bitmap calulation in it, or do I ? The bitmap needs redrawing anyway. Leave it
        // like this until I figured it out

        // set rect to redraw as old bitmap area which is entitled to redraw

        wxRect rect(size.GetWidth() - _iconWidth - _rightIndent, 0, _iconWidth + _rightIndent,
                    _iconWidth + _rightIndent);

        // adjust rectangle when more is slided so we need to redraw all
        // the old stuff but not all (ugly flickering)

        int diffX = size.GetWidth() - _oldSize.GetWidth();
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

    _oldSize = size;
}

void wxCaptionBar::RedrawIconBitmap()
{
    if(_foldIcons)
    {
        // invalidate the bitmap area and force a redraw

        wxRect rect = GetRect();

        rect.SetX(rect.GetWidth() - _iconWidth - _rightIndent);
        rect.SetWidth(_iconWidth + _rightIndent);
        RefreshRect(rect);
    }
}

/*
 * wxCaptionBarEvent
 */

DEFINE_EVENT_TYPE(wxEVT_CAPTIONBAR)

wxCaptionBarEvent::wxCaptionBarEvent(const wxCaptionBarEvent &event)
    : wxCommandEvent(event)
{
    _bar = event._bar;
}

//DEFINE_EVENT_TYPE(wxEVT_CAPTIONBAR)
//IMPLEMENT_DYNAMIC_CLASS(wxCaptionBarEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxCaptionBarEvent, wxCommandEvent)
