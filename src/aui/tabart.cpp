///////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/tabart.cpp
// Purpose:     wxaui: wx advanced user interface - notebook-art
// Author:      Benjamin I. Williams
// Modified by: Malcolm MacLeod (mmacleod@webmail.co.za)
// Modified by: Jens Lody
// Created:     2012-03-21
// Copyright:   (C) Copyright 2006, Kirix Corporation, All Rights Reserved
//                            2012, Jens Lody for the code related to left
//                                  and right positioning
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_AUI

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
    #include "wx/bitmap.h"
    #include "wx/menu.h"
#endif

#include "wx/renderer.h"
#include "wx/aui/auibook.h"
#include "wx/aui/tabart.h"
#include "wx/aui/framemanager.h"
#include "wx/aui/dockart.h"

#ifdef __WXMAC__
#include "wx/osx/private.h"
#endif


// -- GUI helper classes and functions --

class wxAuiCommandCapture : public wxEvtHandler
{
public:

    wxAuiCommandCapture() { m_lastId = 0; }
    int GetCommandId() const { return m_lastId; }

    bool ProcessEvent(wxEvent& evt)
    {
        if (evt.GetEventType() == wxEVT_MENU)
        {
            m_lastId = evt.GetId();
            return true;
        }

        if (GetNextHandler())
            return GetNextHandler()->ProcessEvent(evt);

        return false;
    }

private:
    int m_lastId;
};


// these functions live in dockart.cpp -- they'll eventually
// be moved to a new utility cpp file
wxBitmap wxAuiBitmapFromBits(const unsigned char bits[], int w, int h, const wxColour& color);
wxString wxAuiChopText(wxDC& dc, const wxString& text, int maxSize);


static void DrawButtons(wxDC& dc, const wxRect& _rect, const wxBitmap& bmp, const wxColour& bkcolour, int buttonState)
{
    wxRect rect = _rect;

    if (buttonState == wxAUI_BUTTON_STATE_PRESSED)
    {
        rect.x++;
        rect.y++;
    }

    if (buttonState == wxAUI_BUTTON_STATE_HOVER || buttonState == wxAUI_BUTTON_STATE_PRESSED)
    {
        dc.SetBrush(wxBrush(bkcolour.ChangeLightness(120)));
        dc.SetPen(wxPen(bkcolour.ChangeLightness(75)));

        // draw the background behind the button
        dc.DrawRectangle(rect.x, rect.y, 15, 15);
    }

    // draw the button itself
    dc.DrawBitmap(bmp, rect.x, rect.y, true);
}

static void IndentPressedBitmap(wxRect* rect, int buttonState)
{
    if (buttonState == wxAUI_BUTTON_STATE_PRESSED)
    {
        rect->x++;
        rect->y++;
    }
}

// -- bitmaps --

#if defined( __WXMAC__ )
 static const unsigned char close_bits[]={
     0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xFE, 0x03, 0xF8, 0x01, 0xF0, 0x19, 0xF3,
     0xB8, 0xE3, 0xF0, 0xE1, 0xE0, 0xE0, 0xF0, 0xE1, 0xB8, 0xE3, 0x19, 0xF3,
     0x01, 0xF0, 0x03, 0xF8, 0x0F, 0xFE, 0xFF, 0xFF };
#elif defined( __WXGTK__)
 static const unsigned char close_bits[]={
     0xff, 0xff, 0xff, 0xff, 0x07, 0xf0, 0xfb, 0xef, 0xdb, 0xed, 0x8b, 0xe8,
     0x1b, 0xec, 0x3b, 0xee, 0x1b, 0xec, 0x8b, 0xe8, 0xdb, 0xed, 0xfb, 0xef,
     0x07, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
#else
 static const unsigned char close_bits[]={
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xf3, 0xcf, 0xf9,
     0x9f, 0xfc, 0x3f, 0xfe, 0x3f, 0xfe, 0x9f, 0xfc, 0xcf, 0xf9, 0xe7, 0xf3,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

static const unsigned char left_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfe, 0x3f, 0xfe,
   0x1f, 0xfe, 0x0f, 0xfe, 0x1f, 0xfe, 0x3f, 0xfe, 0x7f, 0xfe, 0xff, 0xfe,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static const unsigned char up_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfc,
   0x3f, 0xf8, 0x1f, 0xf0, 0x0f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

static const unsigned char down_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0x07, 0xf0, 0x0f, 0xf8, 0x1f, 0xfc, 0x3f, 0xfe, 0x7f, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

static const unsigned char right_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xff, 0x9f, 0xff, 0x1f, 0xff,
   0x1f, 0xfe, 0x1f, 0xfc, 0x1f, 0xfe, 0x1f, 0xff, 0x9f, 0xff, 0xdf, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static const unsigned char list_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0x1f, 0xfc, 0x3f, 0xfe, 0x7f, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};






// -- wxAuiGenericTabArt class implementation --

wxAuiGenericTabArt::wxAuiGenericTabArt()
{
    m_requestedSize.x = -1;
    m_requestedSize.y = -1;
    m_normalFont = *wxNORMAL_FONT;
    m_selectedFont = *wxNORMAL_FONT;
    m_selectedFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_measuringFont = m_selectedFont;

    m_requiredBitmapSize = wxDefaultSize;

    m_fixedTabSize = 20;
    m_tabCtrlHeight = 100;

#if defined( __WXMAC__ ) && wxOSX_USE_COCOA_OR_CARBON
    wxColor baseColour = wxColour( wxMacCreateCGColorFromHITheme(kThemeBrushToolbarBackground));
#else
    wxColor baseColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
#endif

    // the baseColour is too pale to use as our base colour,
    // so darken it a bit --
    if ((255-baseColour.Red()) +
        (255-baseColour.Green()) +
        (255-baseColour.Blue()) < 60)
    {
        baseColour = baseColour.ChangeLightness(92);
    }

    m_activeColour = baseColour;
    m_baseColour = baseColour;
    wxColor borderColour = baseColour.ChangeLightness(75);

    m_borderPen = wxPen(borderColour);
    m_baseColourPen = wxPen(m_baseColour);
    m_baseColourBrush = wxBrush(m_baseColour);

    m_activeCloseBmp = wxAuiBitmapFromBits(close_bits, 16, 16, *wxBLACK);
    m_disabledCloseBmp = wxAuiBitmapFromBits(close_bits, 16, 16, wxColour(128,128,128));

    m_activeLeftBmp = wxAuiBitmapFromBits(left_bits, 16, 16, *wxBLACK);
    m_disabledLeftBmp = wxAuiBitmapFromBits(left_bits, 16, 16, wxColour(128,128,128));

    m_activeUpBmp = wxAuiBitmapFromBits(up_bits, 16, 16, *wxBLACK);
    m_disabledUpBmp = wxAuiBitmapFromBits(up_bits, 16, 16, wxColour(128,128,128));

    m_activeDownBmp = wxAuiBitmapFromBits(down_bits, 16, 16, *wxBLACK);
    m_disabledDownBmp = wxAuiBitmapFromBits(down_bits, 16, 16, wxColour(128,128,128));

    m_activeRightBmp = wxAuiBitmapFromBits(right_bits, 16, 16, *wxBLACK);
    m_disabledRightBmp = wxAuiBitmapFromBits(right_bits, 16, 16, wxColour(128,128,128));

    m_activeWindowListBmp = wxAuiBitmapFromBits(list_bits, 16, 16, *wxBLACK);
    m_disabledWindowListBmp = wxAuiBitmapFromBits(list_bits, 16, 16, wxColour(128,128,128));

    m_flags = 0;
}

wxAuiGenericTabArt::~wxAuiGenericTabArt()
{
}

wxAuiTabArt* wxAuiGenericTabArt::Clone()
{
    return new wxAuiGenericTabArt(*this);
}

void wxAuiGenericTabArt::SetFlags(unsigned int flags)
{
    m_flags = flags;
}

void wxAuiGenericTabArt::SetSizingInfo(const wxSize& tabCtrlSize, size_t tabCount)
{
    if (IsHorizontal())
    {
        m_fixedTabSize = 100;

        int tot_width = (int)tabCtrlSize.x - GetIndentSize() - 4;

        if (m_flags & wxAUI_NB_CLOSE_BUTTON)
            tot_width -= m_activeCloseBmp.GetWidth();
        if (m_flags & wxAUI_NB_WINDOWLIST_BUTTON)
            tot_width -= m_activeWindowListBmp.GetWidth();

        if (tabCount > 0)
        {
                m_fixedTabSize = tot_width/(int)tabCount;
        }

        if (m_fixedTabSize < 100)
            m_fixedTabSize = 100;

        if (m_fixedTabSize > tot_width/2)
            m_fixedTabSize = tot_width/2;

        if (m_fixedTabSize > 220)
            m_fixedTabSize = 220;

        m_tabCtrlHeight = tabCtrlSize.y;
    }
    else
    {
        m_fixedTabSize = 20;

        int tot_height = (int)tabCtrlSize.y - GetIndentSize() - 4;

        int btn_height = 0;

        if (m_flags & wxAUI_NB_CLOSE_BUTTON)
            btn_height = m_activeCloseBmp.GetHeight();
        if (m_flags & wxAUI_NB_WINDOWLIST_BUTTON)
            btn_height = wxMax(btn_height,m_activeWindowListBmp.GetHeight());

        tot_height -= btn_height;

        if (tabCount > 0)
        {
            m_fixedTabSize = tot_height/(int)tabCount;
        }


        if (m_fixedTabSize < 20)
            m_fixedTabSize = 20;

        if (m_fixedTabSize > tot_height/2)
            m_fixedTabSize = tot_height/2;

        if (m_fixedTabSize > 60)
            m_fixedTabSize = 60;

        m_tabCtrlWidth = tabCtrlSize.x;
    }
}

void wxAuiGenericTabArt::SetTabCtrlHeight(int size)
{
    m_requestedSize.y = size;
}

void wxAuiGenericTabArt::SetTabCtrlWidth(int size)
{
    m_requestedSize.x = size;
}

void wxAuiGenericTabArt::SetUniformBitmapSize(const wxSize& size)
{
    m_requiredBitmapSize = size;
}

wxSize wxAuiGenericTabArt::GetRequestedSize() const
{
    return wxSize(m_tabCtrlWidth, m_tabCtrlHeight);
}

void wxAuiGenericTabArt::DrawBorder(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    int i, border_width = GetBorderWidth(wnd);

    wxRect theRect(rect);
    for (i = 0; i < border_width; ++i)
    {
        dc.DrawRectangle(theRect.x, theRect.y, theRect.width, theRect.height);
        theRect.Deflate(1);
    }
}

void wxAuiGenericTabArt::DrawBackground(wxDC& dc, wxWindow* WXUNUSED(wnd), const wxRect& rect)
{
    // draw background

    wxColor topColor      = m_baseColour.ChangeLightness(90);
    wxColor bottomColor   = m_baseColour.ChangeLightness(170);
    wxRect bgRect, baseRect;

    wxDirection d;

    // draw base lines

    int y = rect.GetHeight();
    int w = rect.GetWidth();

    if (HasFlag(wxAUI_NB_BOTTOM))
    {
         bgRect = wxRect(rect.x, rect.y + 3, rect.width, rect.height - 3);
         baseRect = wxRect(-1, 0, w + 2, 4);
         d = wxNORTH;
    }
    else if (HasFlag(wxAUI_NB_LEFT))
    {
        bgRect = wxRect(rect.x, rect.y, rect.width - 3, rect.height);
        baseRect = wxRect(w - 5, -1, w, y + 2);
        d = wxEAST;
    }
    else if (HasFlag(wxAUI_NB_RIGHT))
    {
         bgRect = wxRect(rect.x + 3, rect.y, rect.width - 3, rect.height);
         baseRect = wxRect(0, -1, 5, y + 2);
         d = wxWEST;
    }
    else //for wxAUI_NB_TOP
    {
        bgRect = wxRect(rect.x, rect.y, rect.width, rect.height - 3);
        baseRect = wxRect (-1, y - 4, w + 2, 4);
        d = wxSOUTH;
    }

    dc.SetPen(m_borderPen);
    dc.SetBrush(m_baseColourBrush);
    dc.GradientFillLinear(bgRect, topColor, bottomColor,d);
    dc.DrawRectangle(baseRect);
}


// DrawTab() draws an individual tab.
//
// dc       - output dc
// inRect  - rectangle the tab should be confined to
// caption  - tab's caption
// active   - whether or not the tab is active
// outTabRect - actual output rectangle for whole tab
// outButtonRect - actual output rectangle for button
// xExtent - the advance x; where the next tab should start

void wxAuiGenericTabArt::DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiPaneInfo& page, const wxRect& inRect, int closeButtonState, bool haveFocus, wxRect* outTabRect, wxRect* outButtonRect, int* xExtent)
{
    wxCoord normalTextX, normalTextY;
    wxCoord selectedTextX, selectedTextY;
    wxCoord texty;
	
	bool isCompact = page.IsCompactTab() && !page.HasFlag(wxAuiPaneInfo::optionActiveNotebook) && page.GetIcon().IsOk();

    // if the caption is empty, measure some temporary text
	wxString actualCaption = isCompact ? wxString() : page.GetCaption();
    wxString caption = actualCaption;
    if (caption.empty())
        caption = wxT("Xj");

    dc.SetFont(m_selectedFont);
    dc.GetTextExtent(caption, &selectedTextX, &selectedTextY);

    dc.SetFont(m_normalFont);
    dc.GetTextExtent(caption, &normalTextX, &normalTextY);

    // figure out the size of the tab
    wxSize tabSize = GetTabSize(dc,
                                 wnd,
                                 actualCaption,
                                 page.GetIcon(),
                                 page.HasFlag(wxAuiPaneInfo::optionActiveNotebook),
                                 closeButtonState,
                                 xExtent);

    wxCoord tabHeight; 
    wxCoord tabWidth; 

    if (IsHorizontal()) {
        tabHeight = inRect.GetHeight()-3;
        tabWidth = tabSize.GetWidth();
    } else {
        tabHeight = tabSize.GetHeight();
        tabWidth  = inRect.GetWidth()-3;
    }

    wxCoord tabX = inRect.x;
    wxCoord tabY = inRect.y + inRect.height - tabHeight;

    if (HasFlag(wxAUI_NB_LEFT))
    {
        tabHeight += 1;
        tabWidth = inRect.width - 4;
        tabY = inRect.y - 1;
    }
    if (HasFlag(wxAUI_NB_RIGHT))
    {
        tabHeight += 1;
        tabWidth = inRect.width - 4;
        tabY = inRect.y - 1;
        tabX += 3;
    }
    if (HasFlag(wxAUI_NB_BOTTOM))
        tabY += 2;


    // select pen, brush and font for the tab to be drawn

    if (page.HasFlag(wxAuiPaneInfo::optionActiveNotebook))
    {
        dc.SetFont(m_selectedFont);
        texty = selectedTextY;
    }
    else
    {
        dc.SetFont(m_normalFont);
        texty = normalTextY;
    }


    // create points that will make the tab outline

    int clipHeight = tabHeight;
    int clipWidth = tabWidth;
    if (tabX + clipWidth > inRect.x + inRect.width)
        clipWidth = (inRect.x + inRect.width) - tabX;
    if (tabY + tabHeight > inRect.y + inRect.height)
        clipHeight = (inRect.y + inRect.height) - tabY;
    ++clipWidth;
    ++clipHeight;

    // since the above code above doesn't play well with WXDFB or WXCOCOA,
    // we'll just use a rectangle for the clipping region for now --
    dc.SetClippingRegion(tabX, tabY, clipWidth+1, clipHeight+1);

    wxDirection direction;

    wxPoint borderPoints[6];
    if (HasFlag(wxAUI_NB_BOTTOM))
    {
        borderPoints[0] = wxPoint(tabX,             tabY);
        borderPoints[1] = wxPoint(tabX,             tabY+tabHeight-6);
        borderPoints[2] = wxPoint(tabX+2,           tabY+tabHeight-4);
        borderPoints[3] = wxPoint(tabX+tabWidth-2, tabY+tabHeight-4);
        borderPoints[4] = wxPoint(tabX+tabWidth,   tabY+tabHeight-6);
        borderPoints[5] = wxPoint(tabX+tabWidth,   tabY);
        direction = wxNORTH;
    }
    else  if (HasFlag(wxAUI_NB_LEFT))
    {
        borderPoints[0] = wxPoint(tabX+tabWidth, tabY);
        borderPoints[1] = wxPoint(tabX+2,         tabY);
        borderPoints[2] = wxPoint(tabX,           tabY+2);
        borderPoints[3] = wxPoint(tabX,           tabY+tabHeight-2);
        borderPoints[4] = wxPoint(tabX+2,         tabY+tabHeight);
        borderPoints[5] = wxPoint(tabX+tabWidth, tabY+tabHeight);
        direction = wxEAST;
    }
    else  if (HasFlag(wxAUI_NB_RIGHT))
    {
        borderPoints[0] = wxPoint(tabX,             tabY);
        borderPoints[1] = wxPoint(tabX+tabWidth-2, tabY);
        borderPoints[2] = wxPoint(tabX+tabWidth,   tabY+2);
        borderPoints[3] = wxPoint(tabX+tabWidth,   tabY+tabHeight-2);
        borderPoints[4] = wxPoint(tabX+tabWidth-2, tabY+tabHeight);
        borderPoints[5] = wxPoint(tabX,             tabY+tabHeight);
        direction = wxWEST;
    }
    else // must be wxAUI_NB_TOP) {}
    {
        borderPoints[0] = wxPoint(tabX,             tabY+tabHeight-4);
        borderPoints[1] = wxPoint(tabX,             tabY+2);
        borderPoints[2] = wxPoint(tabX+2,           tabY);
        borderPoints[3] = wxPoint(tabX+tabWidth-2, tabY);
        borderPoints[4] = wxPoint(tabX+tabWidth,   tabY+2);
        borderPoints[5] = wxPoint(tabX+tabWidth,   tabY+tabHeight-4);
        direction = wxSOUTH;
    }

    int drawnTabYOff = borderPoints[1].y;
    int drawnTabHeight = borderPoints[0].y - borderPoints[1].y;

    if (!IsHorizontal())
    {
        drawnTabHeight = borderPoints[3].y - borderPoints[2].y;
        drawnTabYOff = borderPoints[2].y;
    }


    if (page.HasFlag(wxAuiPaneInfo::optionActiveNotebook))
    {
        // draw active tab

        // draw base background color
        wxRect r(tabX, tabY, tabWidth, tabHeight);
        dc.SetPen(wxPen(m_activeColour));
        dc.SetBrush(wxBrush(m_activeColour));
        if (HasFlag(wxAUI_NB_RIGHT))
            dc.DrawRectangle(r.x, r.y+1, r.width, r.height-1);
        else if (HasFlag(wxAUI_NB_LEFT))
            dc.DrawRectangle(r.x+1, r.y+1, r.width, r.height-1);
        else // must be wxAUI_NB_TOP or wxAUI_NB_BOTTOM
            dc.DrawRectangle(r.x+1, r.y+1, r.width-1, r.height-4);

        // this white helps fill out the gradient at the top of the tab
        dc.SetPen(*wxWHITE_PEN);
        dc.SetBrush(*wxWHITE_BRUSH);
        if (HasFlag(wxAUI_NB_RIGHT))
            dc.DrawRectangle(r.x+1, r.y+1, r.width-2, r.height-1);
        else if (HasFlag(wxAUI_NB_LEFT))
            dc.DrawRectangle(r.x+2, r.y+1, r.width-2, r.height-1);
        else // must be wxAUI_NB_TOP or wxAUI_NB_BOTTOM
            dc.DrawRectangle(r.x+2, r.y+1, r.width-3, r.height-4);

        // these two points help the rounded corners appear more antialiased
        dc.SetPen(wxPen(m_activeColour));
        if (HasFlag(wxAUI_NB_RIGHT))
        {
            dc.DrawPoint(r.x+r.width-2, r.y+1);
            dc.DrawPoint(r.x+r.width-2, r.y+r.height-1);
        }
        else if (HasFlag(wxAUI_NB_LEFT))
        {
            dc.DrawPoint(r.x+2, r.y+1);
            dc.DrawPoint(r.x+2, r.y+r.height-1);
        }
        else if (HasFlag(wxAUI_NB_BOTTOM))
        {
            dc.DrawPoint(r.x+2, r.y+r.height-5);
            dc.DrawPoint(r.x+r.width-2, r.y+r.height-5);
        }
        else // must be wxAUI_NB_TOP
        {
            dc.DrawPoint(r.x+2, r.y+1);
            dc.DrawPoint(r.x+r.width-2, r.y+1);
        }

        if (HasFlag(wxAUI_NB_RIGHT))
        {
            // set rectangle down a bit for gradient drawing
            r.SetWidth(r.GetWidth()/2);
            r.y += 1;
            r.height -= 1;
        }
        else if (HasFlag(wxAUI_NB_LEFT))
        {
            // set rectangle down a bit for gradient drawing
            r.SetWidth(r.GetWidth()/2);
            r.y += 1;
            r.height -= 1;
            r.x += r.width;
        }
        else if (HasFlag(wxAUI_NB_BOTTOM))
        {
            // set rectangle down a bit for gradient drawing
            r.SetHeight(r.GetHeight()/2);
            r.x += 2;
            r.width -= 3;
        }
        else // must be wxAUI_NB_TOP
        {
            // set rectangle down a bit for gradient drawing
            r.SetHeight(r.GetHeight()/2);
            r.x += 2;
            r.width -= 3;
            r.y += (r.height - 2);
        }

        // draw gradient background
        wxColor topColor = *wxWHITE;
        wxColor bottomColor = m_activeColour;
        dc.GradientFillLinear(r, topColor, bottomColor, direction);
    }
    else
    {
        // draw inactive tab

        wxRect rTop(tabX, tabY+1, tabWidth, tabHeight-3);
        wxRect rBase;

        // start the gradent up a bit and leave the inside border inset
        // by a pixel for a 3D look.  Only the top half of the inactive
        // tab will have a slight gradient
        if (IsHorizontal())
        {
            rTop.x += 3;
            rTop.y++;
            rTop.width -= 4;
            rTop.height /= 2;
            rTop.height--;

            rBase = rTop;

            if (HasFlag(wxAUI_NB_BOTTOM))
            {
                rTop.y += rTop.height;
                rTop.y--;
            }
            else // must be wxAUI_NB_TOP
            {
                rBase.y += rBase.height;
                rBase.y--;
            }
        }
        else
        {
            rTop.x += 2;
            rTop.y++;
            rTop.width -= 2;
            rTop.height += 2;
            rTop.width /= 2;

            rBase = rTop;

            if (HasFlag(wxAUI_NB_RIGHT))
            {
                 rTop.x += rTop.width;
                 rTop.x--;
            }
            else // must be wxAUI_NB_LEFT
            {
                rBase.x += rBase.width;
                rBase.x--;
            }
        }

        // -- draw top gradient fill for glossy look
        wxColor topColor = m_baseColour;
        wxColor bottomColor = topColor.ChangeLightness(160);
        dc.GradientFillLinear(rTop, bottomColor, topColor, direction);

        // -- draw bottom fill for glossy look
        topColor = m_baseColour;
        bottomColor = m_baseColour;
        dc.GradientFillLinear(rBase, topColor, bottomColor, direction);


    }

    // draw tab outline
    dc.SetPen(m_borderPen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawPolygon(WXSIZEOF(borderPoints), borderPoints);

    // there are two horizontal grey lines at the bottom of the tab control,
    // this gets rid of the top one of those lines in the tab control
    if (page.HasFlag(wxAuiPaneInfo::optionActiveNotebook))
    {
        dc.SetPen(m_baseColourPen);

        if (IsHorizontal())
        {
            dc.DrawLine(borderPoints[0].x+1,
                        borderPoints[0].y,
                        borderPoints[5].x,
                        borderPoints[5].y);
        }
        else
        {
            dc.DrawLine(borderPoints[0].x,
                        borderPoints[0].y,
                        borderPoints[5].x,
                        borderPoints[5].y+1);
        }
    }
    else
    {
        dc.SetPen(m_baseColourPen);

        if (!IsHorizontal())
        {
            dc.DrawLine(borderPoints[0].x,
                        borderPoints[0].y,
                        borderPoints[5].x,
                        borderPoints[5].y+1);
        }

    }


    int textOffset = tabX + 8;
    int closeButtonWidth = 0;
    if (closeButtonState != wxAUI_BUTTON_STATE_HIDDEN)
    {
        closeButtonWidth = m_activeCloseBmp.GetWidth();
    }

    int bitmapOffset = 0;
    if (page.GetIcon().IsOk())
    {
        bitmapOffset = tabX + 8;

        // draw bitmap
        dc.DrawBitmap(page.GetIcon(),
                      bitmapOffset,
                      drawnTabYOff + (drawnTabHeight/2) - (page.GetIcon().GetHeight()/2),
                      true);

        textOffset = bitmapOffset + page.GetIcon().GetWidth();
        textOffset += 3; // bitmap padding

    }
    else
    {
        textOffset = tabX + 8;
    }


    wxString drawText = wxAuiChopText(dc, actualCaption, tabWidth - (textOffset-tabX) - closeButtonWidth);

    // draw tab text
    dc.DrawText(drawText, textOffset, drawnTabYOff + (drawnTabHeight)/2 - (texty/2) - 1);

    // draw focus rectangle
    if (page.HasFlag(wxAuiPaneInfo::optionActiveNotebook) && haveFocus)
    {
        wxRect focusRectText(textOffset, (drawnTabYOff + (drawnTabHeight)/2 - (texty/2) - 1),
            selectedTextX, selectedTextY);

        wxRect focusRect;
        wxRect focusRectBitmap;

        if (page.GetIcon().IsOk())
            focusRectBitmap = wxRect(bitmapOffset, drawnTabYOff + (drawnTabHeight/2) - (page.GetIcon().GetHeight()/2),
                                            page.GetIcon().GetWidth(), page.GetIcon().GetHeight());

        if (page.GetIcon().IsOk() && drawText.IsEmpty())
            focusRect = focusRectBitmap;
        else if (!page.GetIcon().IsOk() && !drawText.IsEmpty())
            focusRect = focusRectText;
        else if (page.GetIcon().IsOk() && !drawText.IsEmpty())
            focusRect = focusRectText.Union(focusRectBitmap);

        focusRect.Inflate(2, 2);

        wxRendererNative::Get().DrawFocusRect(wnd, dc, focusRect, 0);
    }

    // draw close button if necessary
    if (closeButtonState != wxAUI_BUTTON_STATE_HIDDEN)
    {
        wxBitmap bmp = m_disabledCloseBmp;

        if (closeButtonState == wxAUI_BUTTON_STATE_HOVER ||
            closeButtonState == wxAUI_BUTTON_STATE_PRESSED)
        {
            bmp = m_activeCloseBmp;
        }

        int offsetY = tabY-1;
        if (m_flags & wxAUI_NB_BOTTOM)
            offsetY = 1;

        wxRect rect(tabX + tabWidth - closeButtonWidth - 1,
                    offsetY + (tabHeight/2) - (bmp.GetHeight()/2),
                    closeButtonWidth,
                    tabHeight);

        IndentPressedBitmap(&rect, closeButtonState);
        dc.DrawBitmap(bmp, rect.x, rect.y, true);

        *outButtonRect = rect;
    }

    *outTabRect = wxRect(tabX, tabY, clipWidth+1, clipHeight+1);

    dc.DestroyClippingRegion();
}

int wxAuiGenericTabArt::GetIndentSize()
{
    return 5;
}

int wxAuiGenericTabArt::GetBorderWidth(wxWindow* wnd)
{
    wxAuiManager* mgr = wxAuiManager::GetManager(wnd);
    if (mgr)
    {
       wxAuiDockArt*  art = mgr->GetArtProvider();
        if (art)
            return art->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE);
    }
    return 1;
}

int wxAuiGenericTabArt::GetAdditionalBorderSpace(wxWindow* wnd)
{
    return GetBorderWidth(wnd);
}

wxSize wxAuiGenericTabArt::GetTabSize(wxDC& dc, wxWindow* WXUNUSED(wnd), const wxString& caption, const wxBitmap& bitmap, bool WXUNUSED(active), int closeButtonState, int* extent)
{
    wxCoord measuredTextX, measuredTextY, tmp;

    dc.SetFont(m_measuringFont);
    dc.GetTextExtent(caption, &measuredTextX, &measuredTextY);

    dc.GetTextExtent(wxT("ABCDEFXj"), &tmp, &measuredTextY);

    // add padding around the text
    wxCoord tabWidth = measuredTextX;
    wxCoord tabHeight = measuredTextY;

    // if the close button is showing or the tab-position is left or right,
    //  add space for it
    if ((closeButtonState != wxAUI_BUTTON_STATE_HIDDEN) || (!IsHorizontal() && !(m_flags & wxAUI_NB_CLOSE_BUTTON)))
     {
         tabWidth += m_activeCloseBmp.GetWidth();
         tabWidth += 3;
         tabHeight = wxMax(tabHeight, m_activeCloseBmp.GetHeight());
     }

    // if there's a bitmap, add space for it
    if (bitmap.IsOk())
    {
        tabWidth += bitmap.GetWidth();
        tabWidth += 3; // right side bitmap padding
        tabHeight = wxMax(tabHeight, bitmap.GetHeight());
    }

    // add padding
    tabWidth += 16;
    tabHeight += 10;

    if (IsHorizontal())
    {
        if (m_flags & wxAUI_NB_TAB_FIXED_WIDTH)
        {
            tabWidth = m_fixedTabSize;
        }
        *extent = tabWidth;
    }
    else
    {
        if (m_flags & wxAUI_NB_TAB_FIXED_HEIGHT)
        {
            tabHeight = m_fixedTabSize;
        }
        *extent = tabHeight;
    }

    return wxSize(tabWidth, tabHeight);
}


void wxAuiGenericTabArt::DrawButton(wxDC& dc, wxWindow* WXUNUSED(wnd), const wxRect& inRect, int bitmapID, int buttonState, int orientation, wxRect* outRect)
{
    wxBitmap bmp;
    wxRect rect = inRect;
    int xOffset = 0;

    switch (bitmapID)
    {
        case wxAUI_BUTTON_CLOSE:
            if (buttonState & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabledCloseBmp;
            else
                bmp = m_activeCloseBmp;
            rect.x = inRect.x + inRect.width - bmp.GetWidth();
            if (HasFlag(wxAUI_NB_LEFT))
                rect.x -= 5;
            break;

        case wxAUI_BUTTON_LEFT:
            if (buttonState & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabledLeftBmp;
            else
                bmp = m_activeLeftBmp;
            break;
        case wxAUI_BUTTON_UP:
            if (buttonState & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabledUpBmp;
            else
                bmp = m_activeUpBmp;
            rect.x = ((rect.x + rect.width)/2) - (bmp.GetWidth()/2);
            break;
        case wxAUI_BUTTON_DOWN:
            if (buttonState & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabledDownBmp;
            else
                bmp = m_activeDownBmp;
            break;
        case wxAUI_BUTTON_RIGHT:
            if (buttonState & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabledRightBmp;
            else
                bmp = m_activeRightBmp;
            if (HasFlag(wxAUI_NB_WINDOWLIST_BUTTON))
                xOffset += m_activeWindowListBmp.GetWidth();
            if (HasFlag(wxAUI_NB_CLOSE_BUTTON))
                xOffset += m_activeCloseBmp.GetWidth();
            break;
        case wxAUI_BUTTON_WINDOWLIST:
            if (buttonState & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabledWindowListBmp;
            else
                bmp = m_activeWindowListBmp;
            rect.x = inRect.x + inRect.width - bmp.GetWidth();
            if (HasFlag(wxAUI_NB_CLOSE_BUTTON))
                xOffset += m_activeCloseBmp.GetWidth();
            if (HasFlag(wxAUI_NB_LEFT))
                rect.x -= 5;
            break;
    }


    if (!bmp.IsOk())
        return;

    if (orientation == wxLEFT)
    {
        rect.x = inRect.x;
        rect.y = ((inRect.y + inRect.height)/2) - (bmp.GetHeight()/2);
        rect.width = bmp.GetWidth();
        rect.height = bmp.GetHeight();
    }
    else if (orientation == wxUP)
    {
        rect.y = inRect.y;
        rect.width = bmp.GetWidth();
        rect.height = bmp.GetHeight();
    }
    else if (orientation == wxDOWN)
    {
        rect.x = ((inRect.x + inRect.width)/2) - (bmp.GetWidth()/2);
        rect.y = inRect.y  + inRect.height - bmp.GetHeight() - bmp.GetHeight() / 2;
        rect.width = bmp.GetWidth();
        rect.height = bmp.GetHeight();
    }
    else if (orientation == wxRIGHT)
    {
        rect = wxRect(inRect.x + inRect.width - bmp.GetWidth(),
                      ((inRect.y + inRect.height)/2) - (bmp.GetHeight()/2),
                      bmp.GetWidth(), bmp.GetHeight());
    }

    rect.x -= xOffset;

    IndentPressedBitmap(&rect, buttonState);
    dc.DrawBitmap(bmp, rect.x, rect.y, true);

    *outRect = rect;
}

int wxAuiGenericTabArt::ShowDropDown(wxWindow* wnd, const wxAuiPaneInfoPtrArray& pages, int WXUNUSED(activeIndex))
{
    wxMenu menuPopup;

    size_t i, count = pages.GetCount();
    for (i = 0; i < count; ++i)
    {
        const wxAuiPaneInfo& page = *pages.Item(i);
        wxString caption = page.GetCaption();

        // if there is no caption, make it a space.  This will prevent
        // an assert in the menu code.
        if (caption.IsEmpty())
            caption = wxT(" ");

        wxMenuItem* item = new wxMenuItem(NULL, 1000+i, caption);
        if (page.GetIcon().IsOk())
            item->SetBitmap(page.GetIcon());
        menuPopup.Append(item);
    }

    // find out where to put the popup menu of window items
    wxPoint pt = ::wxGetMousePosition();
    pt = wnd->ScreenToClient(pt);

    wxAuiCommandCapture* cc = new wxAuiCommandCapture;
    wnd->PushEventHandler(cc);
    wnd->PopupMenu(&menuPopup, pt);
    int command = cc->GetCommandId();
    wnd->PopEventHandler(true);

    if (command >= 1000)
        return command-1000;

    return -1;
}

wxSize wxAuiGenericTabArt::GetBestTabSize(wxWindow* wnd, const wxAuiPaneInfoPtrArray& pages, const wxSize& requiredBmpSize)
{
    wxClientDC dc(wnd);
    dc.SetFont(m_measuringFont);

    SetSizingInfo(wnd->GetSize(), pages.GetCount());

    // sometimes a standard bitmap size needs to be enforced, especially
    // if some tabs have bitmaps and others don't.  This is important because
    // it prevents the tab control from resizing when tabs are added.
    wxBitmap measureBmp;
    if (requiredBmpSize.IsFullySpecified())
    {
        measureBmp.Create(requiredBmpSize.x,
                           requiredBmpSize.y);
    }


    wxSize maxSize;
    size_t i, pageCount = pages.GetCount();
    for (i = 0; i < pageCount; ++i)
    {
        wxAuiPaneInfo& page = *pages.Item(i);

		bool isCompact = page.IsCompactTab() && !page.HasFlag(wxAuiPaneInfo::optionActiveNotebook) && page.GetIcon().IsOk();
		
        wxBitmap bmp;
        if (measureBmp.IsOk())
            bmp = measureBmp;
        else
            bmp = page.GetIcon();

        int ext = 0;
        wxSize s = GetTabSize(dc,
                              wnd,
                              isCompact ? wxString() : page.GetCaption(),
                              bmp,
                              true,
                              wxAUI_BUTTON_STATE_HIDDEN,
                              &ext);

        maxSize.x = wxMax(maxSize.x, s.x);
        maxSize.y = wxMax(maxSize.y, s.y);
    }

    maxSize.IncBy(2, 2);

    return maxSize;
}

void wxAuiGenericTabArt::SetNormalFont(const wxFont& font)
{
    m_normalFont = font;
}

void wxAuiGenericTabArt::SetSelectedFont(const wxFont& font)
{
    m_selectedFont = font;
}

void wxAuiGenericTabArt::SetMeasuringFont(const wxFont& font)
{
    m_measuringFont = font;
}

void wxAuiGenericTabArt::SetColour(const wxColour& colour)
{
    m_baseColour = colour;
    m_borderPen = wxPen(m_baseColour.ChangeLightness(75));
    m_baseColourPen = wxPen(m_baseColour);
    m_baseColourBrush = wxBrush(m_baseColour);
}

void wxAuiGenericTabArt::SetActiveColour(const wxColour& colour)
{
    m_activeColour = colour;
}

wxSize wxAuiGenericTabArt::GetRequiredBitmapSize() const
{
    return m_requiredBitmapSize;
}

// -- wxAuiSimpleTabArt class implementation --

wxAuiSimpleTabArt::wxAuiSimpleTabArt()
{
    m_normalFont = *wxNORMAL_FONT;
    m_selectedFont = *wxNORMAL_FONT;
    m_selectedFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_measuringFont = m_selectedFont;
    m_requestedSize.x = -1;
    m_requestedSize.y = -1;
    m_flags = 0;
    m_fixedTabSize = 20;

    wxColour baseColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

    wxColour backgroundColour = baseColour;
    wxColour normaltabColour = baseColour;
    wxColour selectedtabColour = *wxWHITE;

    m_bkBrush = wxBrush(backgroundColour);
    m_normalBkBrush = wxBrush(normaltabColour);
    m_normalBkPen = wxPen(normaltabColour);
    m_selectedBkBrush = wxBrush(selectedtabColour);
    m_selectedBkPen = wxPen(selectedtabColour);

    m_activeCloseBmp = wxAuiBitmapFromBits(close_bits, 16, 16, *wxBLACK);
    m_disabledCloseBmp = wxAuiBitmapFromBits(close_bits, 16, 16, wxColour(128,128,128));

    m_activeLeftBmp = wxAuiBitmapFromBits(left_bits, 16, 16, *wxBLACK);
    m_disabledLeftBmp = wxAuiBitmapFromBits(left_bits, 16, 16, wxColour(128,128,128));

    m_activeUpBmp = wxAuiBitmapFromBits(up_bits, 16, 16, *wxBLACK);
    m_disabledUpBmp = wxAuiBitmapFromBits(up_bits, 16, 16, wxColour(128,128,128));

    m_activeDownBmp = wxAuiBitmapFromBits(down_bits, 16, 16, *wxBLACK);
    m_disabledDownBmp = wxAuiBitmapFromBits(down_bits, 16, 16, wxColour(128,128,128));

    m_activeRightBmp = wxAuiBitmapFromBits(right_bits, 16, 16, *wxBLACK);
    m_disabledRightBmp = wxAuiBitmapFromBits(right_bits, 16, 16, wxColour(128,128,128));

    m_activeWindowListBmp = wxAuiBitmapFromBits(list_bits, 16, 16, *wxBLACK);
    m_disabledWindowListBmp = wxAuiBitmapFromBits(list_bits, 16, 16, wxColour(128,128,128));

}

wxAuiSimpleTabArt::~wxAuiSimpleTabArt()
{
}

wxAuiTabArt* wxAuiSimpleTabArt::Clone()
{
    return new wxAuiSimpleTabArt(*this);
}

void wxAuiSimpleTabArt::SetFlags(unsigned int flags)
{
    m_flags = flags;
}

void wxAuiSimpleTabArt::SetSizingInfo(const wxSize& tabCtrlSize, size_t tabCount)
{
    if (IsHorizontal())
    {
        m_fixedTabSize = 100;

        int tot_width = (int)tabCtrlSize.x - GetIndentSize() - 4;

        if (m_flags & wxAUI_NB_CLOSE_BUTTON)
            tot_width -= m_activeCloseBmp.GetWidth();
        if (m_flags & wxAUI_NB_WINDOWLIST_BUTTON)
            tot_width -= m_activeWindowListBmp.GetWidth();

        if (tabCount > 0)
        {
                m_fixedTabSize = tot_width/(int)tabCount;
        }


        if (m_fixedTabSize < 100)
            m_fixedTabSize = 100;

        if (m_fixedTabSize > tot_width/2)
            m_fixedTabSize = tot_width/2;

        if (m_fixedTabSize > 220)
            m_fixedTabSize = 220;
    }
    else
    {
        m_fixedTabSize = 20;

        int tot_height = (int)tabCtrlSize.y - GetIndentSize() - 4;

        int btn_height = 0;

        if (m_flags & wxAUI_NB_CLOSE_BUTTON)
            btn_height = m_activeCloseBmp.GetHeight();
        if (m_flags & wxAUI_NB_WINDOWLIST_BUTTON)
            btn_height = wxMax(btn_height,m_activeWindowListBmp.GetHeight());

        tot_height -= btn_height;

        if (tabCount > 0)
        {
            m_fixedTabSize = tot_height/(int)tabCount;
        }


        if (m_fixedTabSize < 20)
            m_fixedTabSize = 20;

        if (m_fixedTabSize > tot_height/2)
            m_fixedTabSize = tot_height/2;

        if (m_fixedTabSize > 60)
            m_fixedTabSize = 60;
    }
}

void wxAuiSimpleTabArt::SetColour(const wxColour& colour)
{
    m_bkBrush = wxBrush(colour);
    m_normalBkBrush = wxBrush(colour);
    m_normalBkPen = wxPen(colour);
}

void wxAuiSimpleTabArt::SetActiveColour(const wxColour& colour)
{
    m_selectedBkBrush = wxBrush(colour);
    m_selectedBkPen = wxPen(colour);
}

void wxAuiSimpleTabArt::DrawBorder(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    int i, border_width = GetBorderWidth(wnd);

    wxRect theRect(rect);
    for (i = 0; i < border_width; ++i)
    {
        dc.DrawRectangle(theRect.x, theRect.y, theRect.width, theRect.height);
        theRect.Deflate(1);
    }
}

void wxAuiSimpleTabArt::DrawBackground(wxDC& dc, wxWindow* WXUNUSED(wnd), const wxRect& rect)
{
    // draw background
    dc.SetBrush(m_bkBrush);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(-1, -1, rect.GetWidth()+2, rect.GetHeight()+2);

    // draw base line
    dc.SetPen(*wxGREY_PEN);
    if (HasFlag(wxAUI_NB_LEFT))
    {
        dc.DrawLine(rect.GetWidth(), 0, rect.GetWidth()-1, rect.GetHeight());
    }
    else if (HasFlag(wxAUI_NB_RIGHT))
    {
        dc.DrawLine(1, 0, 1, rect.GetHeight());
    }
    else if (HasFlag(wxAUI_NB_BOTTOM))
    {
        dc.DrawLine(0, 1, rect.GetWidth(), 1);
    }
    else // must be wxAUI_NB_TOP
    {
        dc.DrawLine(0, rect.GetHeight()-1, rect.GetWidth(), rect.GetHeight()-1);
    }

}


// DrawTab() draws an individual tab.
//
// dc       - output dc
// inRect  - rectangle the tab should be confined to
// caption  - tab's caption
// active   - whether or not the tab is active
// outRect - actual output rectangle
// extent - the advance x; where the next tab should start

void wxAuiSimpleTabArt::DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiPaneInfo& page, const wxRect& inRect, int closeButtonState, bool haveFocus, wxRect* outTabRect, wxRect* outButtonRect, int* extent)
{
    wxCoord normalTextX, normalTextY;
    wxCoord selectedTextX, selectedTextY;
    wxCoord textx, texty;

    // if the caption is empty, measure some temporary text
	bool isCompact = page.IsCompactTab() && !page.HasFlag(wxAuiPaneInfo::optionActiveNotebook) && page.GetIcon().IsOk();
	wxString actualCaption = isCompact ? wxString() : page.GetCaption();
    wxString caption = actualCaption;
    if (caption.empty())
        caption = wxT("Xj");

    dc.SetFont(m_selectedFont);
    dc.GetTextExtent(caption, &selectedTextX, &selectedTextY);

    dc.SetFont(m_normalFont);
    dc.GetTextExtent(caption, &normalTextX, &normalTextY);

    // figure out the size of the tab
    wxSize tabSize = GetTabSize(dc,
                                 wnd,
                                 actualCaption,
                                 page.GetIcon(),
                                 page.HasFlag(wxAuiPaneInfo::optionActiveNotebook),
                                 closeButtonState,
                                 extent);

    wxCoord tabHeight = tabSize.y;
    wxCoord tabWidth = tabSize.x;
    wxCoord tabX = inRect.x;
    wxCoord tabY = inRect.y + inRect.height - tabHeight;
    if (!IsHorizontal())
    {
        tabWidth = inRect.width;
        tabY = inRect.y - 1;
        tabHeight += 2;
    }


    // select pen, brush and font for the tab to be drawn

    if (page.HasFlag(wxAuiPaneInfo::optionActiveNotebook))
    {
        dc.SetPen(m_selectedBkPen);
        dc.SetBrush(m_selectedBkBrush);
        dc.SetFont(m_selectedFont);
        textx = selectedTextX;
        texty = selectedTextY;
    }
    else
    {
        dc.SetPen(m_normalBkPen);
        dc.SetBrush(m_normalBkBrush);
        dc.SetFont(m_normalFont);
        textx = normalTextX;
        texty = normalTextY;
    }

    // -- draw line --
    wxPoint points[7];
    if (HasFlag(wxAUI_NB_LEFT))
    {
        points[0].x = tabX;
        points[0].y = tabY + tabHeight;
        points[1].x = tabX;
        points[1].y = tabY + tabHeight - 2;
        points[2].x = tabX + tabHeight - 4;
        points[2].y = tabY + 2;
        points[3].x = tabX + tabHeight + 2;
        points[3].y = tabY;
        points[4].x = tabX + tabWidth-1;
        points[4].y = tabY;
        points[5].x = tabX + tabWidth-1;
        points[5].y = tabY + tabHeight;
        points[6] = points[0];
    }
    else if (HasFlag(wxAUI_NB_RIGHT))
    {
        points[0].x = tabX;
        points[0].y = tabY + tabHeight;
        points[1].x = tabX;
        points[1].y = tabY;
        points[2].x = tabX + tabWidth - tabHeight - 3;
        points[2].y = tabY;
        points[3].x = tabX + tabWidth - tabHeight + 3;
        points[3].y = tabY + 2;
        points[4].x = tabX + tabWidth;
        points[4].y = tabY + tabHeight - 2;
        points[5].x = tabX + tabWidth;
        points[5].y = tabY + tabHeight;
        points[6] = points[0];
    }
    else if (HasFlag(wxAUI_NB_BOTTOM))
    {
        tabY -= 3;
        points[0].x = tabX;
        points[0].y = tabY;
        points[1].x = tabX + tabHeight - 3;
        points[1].y = tabY + tabHeight - 4;
        points[2].x = tabX + tabHeight + 3;
        points[2].y = tabY + tabHeight - 2;
        points[3].x = tabX + tabWidth - 2;
        points[3].y = tabY + tabHeight - 2;
        points[4].x = tabX + tabWidth;
        points[4].y = tabY + tabHeight - 4;
        points[5].x = tabX + tabWidth;
        points[5].y = tabY;
        points[6] = points[0];
     }
     else // must be wxAUI_NB_TOP
     {
        points[0].x = tabX;
        points[0].y = tabY + tabHeight - 1;
        points[1].x = tabX + tabHeight - 3;
        points[1].y = tabY + 2;
        points[2].x = tabX + tabHeight + 3;
        points[2].y = tabY;
        points[3].x = tabX + tabWidth - 2;
        points[3].y = tabY;
        points[4].x = tabX + tabWidth;
        points[4].y = tabY + 2;
        points[5].x = tabX + tabWidth;
        points[5].y = tabY + tabHeight - 1;
        points[6] = points[0];
    }

    int clipWidth = tabWidth;
    int clipHeight = tabHeight;

    if (tabX + clipWidth > inRect.x + inRect.width)
        clipWidth = (inRect.x + inRect.width) - tabX;
    if (tabY + tabHeight > inRect.y + inRect.height)
        clipHeight = (inRect.y + inRect.height) - tabY;
    ++clipWidth;
    ++clipHeight;

    dc.SetClippingRegion(tabX, tabY, clipWidth, clipHeight);

    dc.DrawPolygon(WXSIZEOF(points) - 1, points);

    dc.SetPen(*wxGREY_PEN);

    dc.DrawLines(WXSIZEOF(points), points);


    int textOffset;

    int closeButtonWidth = 0;
    if (closeButtonState != wxAUI_BUTTON_STATE_HIDDEN)
    {
        closeButtonWidth = m_activeCloseBmp.GetWidth();
        textOffset = tabX + (tabHeight/2) + ((tabWidth-closeButtonWidth)/2) - (textx/2);
    }
    else
    {
        textOffset = tabX + (tabHeight/3) + (tabWidth/2) - (textx/2);
    }

    // set minimum text offset
    if (textOffset < tabX + tabHeight)
        textOffset = tabX + tabHeight;

    if (HasFlag(wxAUI_NB_RIGHT))
        textOffset -= tabHeight;
    // chop text if necessary
    wxString drawText = wxAuiChopText(dc, actualCaption, tabWidth - (HasFlag(wxAUI_NB_RIGHT)&&HasFlag(wxAUI_MGR_NB_TAB_FIXED_WIDTH)?tabHeight:0) - (textOffset-tabX) - closeButtonWidth);

    // draw tab text
    dc.DrawText(drawText, textOffset, tabY + (tabHeight - texty) / 2 + 1);

    // draw focus rectangle
    if (page.HasFlag(wxAuiPaneInfo::optionActiveNotebook) && haveFocus)
    {
        wxRect focusRect(textOffset, (tabY + (tabHeight - texty) / 2 + 1),
            selectedTextX, selectedTextY);

        focusRect.Inflate(2, 2);

        wxRendererNative::Get().DrawFocusRect(wnd, dc, focusRect, 0);
    }

    // draw close button if necessary
    if (closeButtonState != wxAUI_BUTTON_STATE_HIDDEN)
    {
        wxBitmap bmp;
        if (page.HasFlag(wxAuiPaneInfo::optionActiveNotebook))
            bmp = m_activeCloseBmp;
        else
            bmp = m_disabledCloseBmp;

        wxRect rect(tabX + tabWidth - closeButtonWidth - 1,
                    tabY + (tabHeight/2) - (bmp.GetHeight()/2) + 1,
                    closeButtonWidth,
                    tabHeight - 1);

        if (HasFlag(wxAUI_NB_RIGHT))
            rect.x -= tabHeight;

        DrawButtons(dc, rect, bmp, *wxWHITE, closeButtonState);

        *outButtonRect = rect;
    }


    *outTabRect = wxRect(tabX, tabY, tabWidth, tabHeight);

    dc.DestroyClippingRegion();
}

int wxAuiSimpleTabArt::GetIndentSize()
{
    return 0;
}

int wxAuiSimpleTabArt::GetBorderWidth(wxWindow* wnd)
{
    wxAuiManager* mgr = wxAuiManager::GetManager(wnd);
    if (mgr)
    {
       wxAuiDockArt*  art = mgr->GetArtProvider();
        if (art)
            return art->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE);
    }
    return 1;
}

int wxAuiSimpleTabArt::GetAdditionalBorderSpace(wxWindow* wnd)
{
    return GetBorderWidth(wnd);
}

wxSize wxAuiSimpleTabArt::GetTabSize(wxDC& dc, wxWindow* WXUNUSED(wnd), const wxString& caption, const wxBitmap& WXUNUSED(bitmap), bool WXUNUSED(active), int closeButtonState, int* extent)
{
    wxCoord measuredTextX, measuredTextY, tmp;

    dc.SetFont(m_measuringFont);
    dc.GetTextExtent(caption, &measuredTextX, &measuredTextY);

    dc.GetTextExtent(wxT("ABCDEFXj"), &tmp, &measuredTextY);

    wxCoord tabHeight = measuredTextY + 4;
    wxCoord tabWidth = measuredTextX + tabHeight + 5;

    if (closeButtonState != wxAUI_BUTTON_STATE_HIDDEN)
        tabWidth += m_activeCloseBmp.GetWidth();

    if (IsHorizontal())
    {
        if (HasFlag(wxAUI_MGR_NB_TAB_FIXED_WIDTH))
        {
            tabWidth = m_fixedTabSize + tabHeight/2 ;
        }
        *extent = tabWidth - (tabHeight/2) - 1;
    }
    else
    {
        if (HasFlag(wxAUI_NB_TAB_FIXED_HEIGHT))
             tabHeight = m_fixedTabSize - 2;
        tabWidth += 16;
        *extent = tabHeight + 2;
    }

    return wxSize(tabWidth, tabHeight);
}


void wxAuiSimpleTabArt::DrawButton(wxDC& dc, wxWindow* WXUNUSED(wnd), const wxRect& inRect, int bitmapID, int buttonState, int orientation, wxRect* outRect)
{
    wxBitmap bmp;
    wxRect rect = inRect;
    int xOffset = 0;

    switch (bitmapID)
    {
        case wxAUI_BUTTON_CLOSE:
            if (buttonState & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabledCloseBmp;
            else
                bmp = m_activeCloseBmp;
            rect.x = inRect.x + inRect.width - bmp.GetWidth();
            break;
        case wxAUI_BUTTON_UP:
            if (buttonState & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabledUpBmp;
            else
                bmp = m_activeUpBmp;
            rect.x = ((rect.x + rect.width)/2) - (bmp.GetWidth()/2);
            break;
        case wxAUI_BUTTON_DOWN:
            if (buttonState & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabledDownBmp;
            else
                bmp = m_activeDownBmp;
            break;
        case wxAUI_BUTTON_LEFT:
            if (buttonState & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabledLeftBmp;
            else
                bmp = m_activeLeftBmp;
            break;
        case wxAUI_BUTTON_RIGHT:
            if (buttonState & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabledRightBmp;
            else
                bmp = m_activeRightBmp;
            if (HasFlag(wxAUI_NB_WINDOWLIST_BUTTON))
                xOffset += m_activeWindowListBmp.GetWidth();
            if (HasFlag(wxAUI_NB_CLOSE_BUTTON))
                xOffset += m_activeCloseBmp.GetWidth();
            break;
        case wxAUI_BUTTON_WINDOWLIST:
            if (buttonState & wxAUI_BUTTON_STATE_DISABLED)
                bmp = m_disabledWindowListBmp;
            else
                bmp = m_activeWindowListBmp;
            rect.x = inRect.x + inRect.width - bmp.GetWidth();
            if (HasFlag(wxAUI_NB_CLOSE_BUTTON))
                xOffset += m_activeCloseBmp.GetWidth();
            break;
    }

    if (!bmp.IsOk())
        return;

    if (orientation == wxLEFT)
    {
        rect.x = inRect.x;
        rect.y = ((inRect.y + inRect.height)/2) - (bmp.GetHeight()/2);
        rect.width = bmp.GetWidth();
        rect.height = bmp.GetHeight();
    }
    else if (orientation == wxUP)
    {
        rect.y = inRect.y;
        rect.width = bmp.GetWidth();
        rect.height = bmp.GetHeight();
    }
    else if (orientation == wxDOWN)
    {
        rect.x = ((inRect.x + inRect.width)/2) - (bmp.GetWidth()/2);
        rect.y = inRect.y  + inRect.height - bmp.GetHeight() - bmp.GetHeight() / 2;
        rect.width = bmp.GetWidth();
        rect.height = bmp.GetHeight();
    }
    else if (orientation == wxRIGHT)
    {
        rect = wxRect(inRect.x + inRect.width - bmp.GetWidth(),
                      ((inRect.y + inRect.height)/2) - (bmp.GetHeight()/2),
                      bmp.GetWidth(), bmp.GetHeight());
    }

    rect.x -= xOffset;

    DrawButtons(dc, rect, bmp, *wxWHITE, buttonState);

    *outRect = rect;
}

int wxAuiSimpleTabArt::ShowDropDown(wxWindow* wnd, const wxAuiPaneInfoPtrArray& pages, int activeIndex)
{
    wxMenu menuPopup;

    size_t i, count = pages.GetCount();
    for (i = 0; i < count; ++i)
    {
        const wxAuiPaneInfo& page = *pages.Item(i);
        menuPopup.AppendCheckItem(1000+i, page.GetCaption());
    }

    if (activeIndex != -1)
    {
        menuPopup.Check(1000+activeIndex, true);
    }

    // find out where to put the popup menu of window items
    wxPoint pt = ::wxGetMousePosition();
    pt = wnd->ScreenToClient(pt);

    wxAuiCommandCapture* cc = new wxAuiCommandCapture;
    wnd->PushEventHandler(cc);
    wnd->PopupMenu(&menuPopup, pt);
    int command = cc->GetCommandId();
    wnd->PopEventHandler(true);

    if (command >= 1000)
        return command-1000;

    return -1;
}

wxSize wxAuiSimpleTabArt::GetBestTabSize(wxWindow* wnd, const wxAuiPaneInfoPtrArray& pages, const wxSize& WXUNUSED(requiredBmpSize))
{
    wxClientDC dc(wnd);
    dc.SetFont(m_measuringFont);

    SetSizingInfo(wnd->GetSize(), pages.GetCount());

    wxSize maxSize;
    size_t i, pageCount = pages.GetCount();
    for (i = 0; i < pageCount; ++i)
    {
        wxAuiPaneInfo* page = pages.Item(i);
		bool isCompact = page->IsCompactTab() && !page->HasFlag(wxAuiPaneInfo::optionActiveNotebook) && page->GetIcon().IsOk();

        int ext = 0;
        wxSize s = GetTabSize(dc,
                              wnd,
                              isCompact ? wxString() : page->GetCaption(),
                              wxNullBitmap,
                              true,
                              wxAUI_BUTTON_STATE_HIDDEN,
                              &ext);

        maxSize.x = wxMax(maxSize.x, s.x);
        maxSize.y = wxMax(maxSize.y, s.y);
    }

    maxSize.IncBy(3, 3);
    return maxSize;
}

void wxAuiSimpleTabArt::SetNormalFont(const wxFont& font)
{
    m_normalFont = font;
}

void wxAuiSimpleTabArt::SetSelectedFont(const wxFont& font)
{
    m_selectedFont = font;
}

void wxAuiSimpleTabArt::SetMeasuringFont(const wxFont& font)
{
    m_measuringFont = font;
}

void wxAuiSimpleTabArt::SetTabCtrlHeight(int size)
{
    m_requestedSize.y = size;
}

void wxAuiSimpleTabArt::SetTabCtrlWidth(int size)
{
    m_requestedSize.x = size;
}

void wxAuiSimpleTabArt::SetUniformBitmapSize(const wxSize& size)
{
    m_requiredBitmapSize = size;
}

wxSize wxAuiSimpleTabArt::GetRequestedSize() const
{
    return wxSize(m_tabCtrlWidth, m_tabCtrlHeight);
}

wxSize wxAuiSimpleTabArt::GetRequiredBitmapSize() const
{
    return m_requiredBitmapSize;
}

#endif // wxUSE_AUI
