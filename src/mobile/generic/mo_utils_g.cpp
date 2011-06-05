/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_utils_g.cpp
// Purpose:     iPhone 'emulator' utilities.
// Author:      Julian Smart
// Modified by:
// Created:     12/05/2009 08:59:11
// RCS-ID:
// Copyright:   Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"

#include "wx/mobile/generic/utils.h"

bool wxMoRenderer::DrawButtonBackground(wxWindow* WXUNUSED(window), wxDC& dc,
                                        const wxColour& backgroundColour,
                                        const wxColour& colour,
                                        const wxColour& borderColour1, 
                                        const wxRect& rect1,
                                        int state, int buttonStyle, int inc)
{
    wxRect rect(rect1);

    wxColour borderColour(borderColour1);
    if (!borderColour.IsOk())
        borderColour = wxColour(160, 160, 160);

    if (backgroundColour.IsOk() && (buttonStyle & wxMO_BUTTON_NO_BACKGROUND) == 0)
    {
        dc.SetBackground(wxBrush(backgroundColour));
        dc.SetClippingRegion(rect);
        dc.Clear();
        dc.DestroyClippingRegion();
    }

    if (buttonStyle & wxMO_BUTTON_NO_BORDER)
    {
        rect.x --;
        rect.y --;
        rect.width += 2;
        rect.height += 2;
        dc.SetPen(wxNullPen);
    }
    else
        dc.SetPen(wxPen(borderColour));

    if (buttonStyle & wxMO_BUTTON_TWO_TONE)
    {
        int colourInc = inc;
        int colourDec = -inc;

        if (state == wxCTRL_STATE_HIGHLIGHTED)
        {
            colourInc -= 20;
            colourDec -= 20;
        }
        
        wxColour lightColour = colour;
        wxColour darkerColour = colour;

        if (state & wxCTRL_STATE_DISABLED)
        {
            lightColour = wxColour(180, 180, 180);
            darkerColour = lightColour;
        }

        IncColourValues(lightColour, colourInc);
        IncColourValues(darkerColour, colourDec);

        dc.SetBrush(wxBrush(lightColour));

        if (buttonStyle & wxMO_BUTTON_BACK)
        {
            // Clip to a back-shaped button
            int arrowWidth = wxMO_BACK_BUTTON_ARROW_SIZE;
            wxPoint points[5];
            points[0] = wxPoint(rect.x, rect.y + rect.height/2);
            points[1] = wxPoint(rect.x+arrowWidth, rect.y);
            points[2] = wxPoint(rect.x+rect.width, rect.y);
            points[3] = wxPoint(rect.x+rect.width, rect.y+rect.height/2);
            points[4] = wxPoint(rect.x, rect.y+rect.height/2);
            wxRegion region1(5, points);

            dc.SetClippingRegion(region1);
            if (buttonStyle & wxMO_BUTTON_NO_BORDER)
                dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
            else
                dc.DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, 4);
            dc.DestroyClippingRegion();

            dc.SetBrush(wxBrush(darkerColour));

            points[0] = wxPoint(rect.x, rect.y + rect.height/2);
            points[1] = wxPoint(rect.x+arrowWidth, rect.y+rect.height);
            points[2] = wxPoint(rect.x+rect.width, rect.y+rect.height);
            points[3] = wxPoint(rect.x+rect.width, rect.y+rect.height/2);
            points[4] = wxPoint(rect.x+arrowWidth, rect.y+rect.height/2);
            wxRegion region2(5, points);

            dc.SetClippingRegion(region2);
            if (buttonStyle & wxMO_BUTTON_NO_BORDER)
                dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
            else
                dc.DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, 4);
            dc.DestroyClippingRegion();

            dc.DrawLine(rect.x, rect.y + rect.height/2, rect.x+arrowWidth, rect.y);
            dc.DrawLine(rect.x, rect.y + rect.height/2, rect.x+arrowWidth, rect.y+rect.height);
        }
        else
        {
            if (buttonStyle & wxMO_BUTTON_NO_BORDER)
                dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
            else
                dc.DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, 4);

            if (buttonStyle & wxMO_BUTTON_STRAIGHT_LEFT_EDGE)
            {
                // Straighten up the top-left corner
                dc.SetPen(*wxTRANSPARENT_PEN);
                dc.DrawRectangle(rect.x, rect.y, 4, 4);

                dc.SetPen(borderColour);
                dc.DrawLine(rect.x, rect.y, rect.x, rect.y+4);
                dc.DrawLine(rect.x, rect.y, rect.x+4, rect.y);
            }

            if (buttonStyle & wxMO_BUTTON_STRAIGHT_RIGHT_EDGE)
            {
                // Straighten up the top-right corner
                dc.SetPen(*wxTRANSPARENT_PEN);
                dc.DrawRectangle(rect.x+rect.width-4, rect.y, 4, 4);

                dc.SetPen(borderColour);
                dc.DrawLine(rect.x+rect.width-4-1, rect.y, rect.x+rect.width-1, rect.y);
                dc.DrawLine(rect.x+rect.width-1, rect.y, rect.x+rect.width-1, rect.y+4);
            }

            dc.SetBrush(wxBrush(darkerColour));
            
            wxRect clipRect(rect.x, rect.y + rect.height/2+1, rect.width, rect.height/2+1);
            dc.SetClippingRegion(clipRect);
            if (buttonStyle & wxMO_BUTTON_NO_BORDER)
                dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
            else
                dc.DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, 4);

            if (buttonStyle & wxMO_BUTTON_STRAIGHT_LEFT_EDGE)
            {
                // Straighten up the bottom-left corner
                dc.SetPen(*wxTRANSPARENT_PEN);
                dc.DrawRectangle(rect.x, rect.y+rect.height-4, 4, 4);

                dc.SetPen(borderColour);
                dc.DrawLine(rect.x, rect.y+rect.height-4-1, rect.x, rect.y+rect.height-1);
                dc.DrawLine(rect.x, rect.y+rect.height-1, rect.x+4, rect.y+rect.height-1);
            }

            if (buttonStyle & wxMO_BUTTON_STRAIGHT_RIGHT_EDGE)
            {
                // Straighten up the bottom-right corner
                dc.SetPen(*wxTRANSPARENT_PEN);
                dc.DrawRectangle(rect.x+rect.width-4, rect.y+rect.height-4, 4, 4);

                dc.SetPen(borderColour);
                dc.DrawLine(rect.x+rect.width-4-1, rect.y+rect.height-1, rect.x+rect.width-1, rect.y+rect.height-1);
                dc.DrawLine(rect.x+rect.width-1, rect.y+rect.height-1, rect.x+rect.width-1, rect.y+rect.height-4-1);
            }

            dc.DestroyClippingRegion();

        }
    }
    else
    {
        int colourInc = 0;

        if (state == wxCTRL_STATE_HIGHLIGHTED)
            colourInc -= 8;

        wxColour actualColour(colour);
        if (state & wxCTRL_STATE_DISABLED)
            actualColour = wxColour(180, 180, 180);

        IncColourValues(actualColour, colourInc);
        dc.SetBrush(wxBrush(actualColour));        
        if (buttonStyle & wxMO_BUTTON_NO_BORDER)
            dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
        else
            dc.DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, 4);
    }

    return true;
}

unsigned char wxMoRenderer::BlendColour(unsigned char fg, unsigned char bg, double alpha)
{
    double result = bg + (alpha * (fg - bg));
    if (result < 0.0)
        result = 0.0;
    if (result > 255)
        result = 255;
    return (unsigned char)result;
}

bool wxMoRenderer::IncColourValues(wxColour& colour, int inc)
{
    int red = int(colour.Red()) + inc;
    int green = int(colour.Green()) + inc;
    int blue = int(colour.Blue()) + inc;

    red = wxMax(0, red);
    red = wxMin(255, red);

    green = wxMax(0, green);
    green = wxMin(255, green);
    
    blue = wxMax(0, blue);
    blue = wxMin(255, blue);

    colour.Set(red, green, blue);

    return true;
}

wxBitmap wxMoRenderer::MakeDisabledBitmap(wxBitmap& bmp)
{
    wxImage image = bmp.ConvertToImage();

    int mr, mg, mb;
    mr = image.GetMaskRed();
    mg = image.GetMaskGreen();
    mb = image.GetMaskBlue();

    unsigned char* data = image.GetData();
    int width = image.GetWidth();
    int height = image.GetHeight();
    bool has_mask = image.HasMask();

    for (int y = height-1; y >= 0; --y)
    {
        for (int x = width-1; x >= 0; --x)
        {
            data = image.GetData() + (y*(width*3))+(x*3);
            unsigned char* r = data;
            unsigned char* g = data+1;
            unsigned char* b = data+2;

            if (has_mask && *r == mr && *g == mg && *b == mb)
                continue;

            *r = BlendColour(*r, 255, 0.4);
            *g = BlendColour(*g, 255, 0.4);
            *b = BlendColour(*b, 255, 0.4);
        }
    }

    return wxBitmap(image);
}

bool wxMoButtonPressDetector::ProcessMouseEvent(wxWindow* win, const wxRect& rect, wxMouseEvent& event, int id)
{
    wxPoint pt = event.GetPosition();

    bool shouldRefresh = false;

    if (event.LeftUp())
    {
        if (wxWindow::GetCapture() == win)
        {
            win->ReleaseMouse();
        }
        if (m_buttonState == wxCTRL_STATE_HIGHLIGHTED)
        {
            m_buttonState = wxCTRL_STATE_NORMAL;
            m_buttonId = 0;
            win->Refresh();
            return true;
        }
        m_buttonState = wxCTRL_STATE_NORMAL;
        m_buttonId = 0;
    }
    else if (event.LeftDown() && rect.Contains(pt))
    {
        if (wxWindow::GetCapture() != win)
        {
            win->CaptureMouse();
        }

        if (m_buttonState != wxCTRL_STATE_HIGHLIGHTED)
        {
            m_buttonState = wxCTRL_STATE_HIGHLIGHTED;
            m_buttonId = id;
            shouldRefresh = true;
        }
    }
    else if (event.LeftIsDown() && !rect.Contains(pt))
    {
        if (m_buttonState == wxCTRL_STATE_HIGHLIGHTED)
        {
            m_buttonState = wxCTRL_STATE_NORMAL;
            m_buttonId = 0;
            shouldRefresh = true;
        }
    }
    else if (event.Leaving())
    {
        m_buttonState = wxCTRL_STATE_NORMAL;
        m_buttonId = 0;
        if (wxWindow::GetCapture() == win)
        {
            win->ReleaseMouse();
        }
    }

    if (shouldRefresh)
        win->Refresh();

    return false;
}

