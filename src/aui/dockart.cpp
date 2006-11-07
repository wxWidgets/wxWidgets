///////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/dockart.cpp
// Purpose:     wxaui: wx advanced user interface - docking window manager
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2005-05-17
// RCS-ID:      $Id$
// Copyright:   (C) Copyright 2005-2006, Kirix Corporation, All Rights Reserved
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_AUI

#include "wx/aui/framemanager.h"
#include "wx/aui/dockart.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/image.h"
#endif

#ifdef __WXMAC__
#include "wx/mac/private.h"
#include "wx/graphics.h"
#endif

#ifdef __WXGTK__
#include <gtk/gtk.h>
#include "wx/gtk/win_gtk.h"
#include "wx/renderer.h"
#endif


// -- wxAuiDefaultDockArt class implementation --

// wxAuiDefaultDockArt is an art provider class which does all of the drawing for
// wxAuiManager.  This allows the library caller to customize the dock art
// (probably by deriving from this class), or to completely replace all drawing
// with custom dock art (probably by writing a new stand-alone class derived
// from the wxAuiDockArt base class). The active dock art class can be set via
// wxAuiManager::SetDockArt()


// StepColour() it a utility function that simply darkens
// or lightens a color, based on the specified percentage
static wxColor StepColour(const wxColor& c, int percent)
{
    int r = c.Red(), g = c.Green(), b = c.Blue();
    return wxColour((unsigned char)wxMin((r*percent)/100,255),
                    (unsigned char)wxMin((g*percent)/100,255),
                    (unsigned char)wxMin((b*percent)/100,255));
}

static wxColor LightContrastColour(const wxColour& c)
{
    int amount = 120;

    // if the color is especially dark, then
    // make the contrast even lighter
    if (c.Red() < 128 && c.Green() < 128 && c.Blue() < 128)
        amount = 160;

    return StepColour(c, amount);
}

// BitmapFromBits() is a utility function that creates a
// masked bitmap from raw bits (XBM format)
static wxBitmap BitmapFromBits(const unsigned char bits[], int w, int h,
                               const wxColour& color)
{
    wxImage img = wxBitmap((const char*)bits, w, h).ConvertToImage();
    img.Replace(0,0,0,123,123,123);
    img.Replace(255,255,255,color.Red(),color.Green(),color.Blue());
    img.SetMaskColour(123,123,123);
    return wxBitmap(img);
}


static void DrawGradientRectangle(wxDC& dc,
                                  const wxRect& rect,
                                  const wxColour& start_color,
                                  const wxColour& end_color,
                                  int direction)
{
    int rd, gd, bd, high = 0;
    rd = end_color.Red() - start_color.Red();
    gd = end_color.Green() - start_color.Green();
    bd = end_color.Blue() - start_color.Blue();

    if (direction == wxAUI_GRADIENT_VERTICAL)
        high = rect.GetHeight()-1;
         else
        high = rect.GetWidth()-1;

    for (int i = 0; i <= high; ++i)
    {
        int r,g,b;
        
        
        r = start_color.Red() + ((i*rd*100)/high)/100;
        g = start_color.Green() + ((i*gd*100)/high)/100;
        b = start_color.Blue() + ((i*bd*100)/high)/100;

        wxPen p(wxColor((unsigned char)r,
                        (unsigned char)g,
                        (unsigned char)b));
        dc.SetPen(p);

        if (direction == wxAUI_GRADIENT_VERTICAL)
            dc.DrawLine(rect.x, rect.y+i, rect.x+rect.width, rect.y+i);
             else
            dc.DrawLine(rect.x+i, rect.y, rect.x+i, rect.y+rect.height);
    }

}

wxAuiDefaultDockArt::wxAuiDefaultDockArt()
{
#ifdef __WXMAC__
    wxBrush toolbarbrush;
    toolbarbrush.MacSetTheme( kThemeBrushToolbarBackground );
    wxColor base_colour = toolbarbrush.GetColour();
#else
    wxColor base_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
#endif

    m_base_colour = base_colour;
    wxColor darker1_colour = StepColour(base_colour, 85);
    wxColor darker2_colour = StepColour(base_colour, 70);
    wxColor darker3_colour = StepColour(base_colour, 60);
    wxColor darker4_colour = StepColour(base_colour, 50);
    wxColor darker5_colour = StepColour(base_colour, 40);

    m_active_caption_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    m_active_caption_gradient_colour = LightContrastColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    m_active_caption_text_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    m_inactive_caption_colour = darker1_colour;
    m_inactive_caption_gradient_colour = StepColour(base_colour, 97);
    m_inactive_caption_text_colour = *wxBLACK;

#ifdef __WXMAC__
    m_sash_brush = toolbarbrush;
    m_background_brush = toolbarbrush;
    m_gripper_brush = toolbarbrush;
#else
    m_sash_brush = wxBrush(base_colour);
    m_background_brush = wxBrush(base_colour);
    m_gripper_brush = wxBrush(base_colour);
#endif
    m_border_pen = wxPen(darker2_colour);
    m_gripper_pen1 = wxPen(darker5_colour);
    m_gripper_pen2 = wxPen(darker3_colour);
    m_gripper_pen3 = *wxWHITE_PEN;

#ifdef __WXMAC__
    m_caption_font = *wxSMALL_FONT;
#else
    m_caption_font = wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE);
#endif

    // some built in bitmaps
#if defined( __WXMAC__ )
     static unsigned char close_bits[]={
         0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xFE, 0x03, 0xF8, 0x01, 0xF0, 0x19, 0xF3,
         0xB8, 0xE3, 0xF0, 0xE1, 0xE0, 0xE0, 0xF0, 0xE1, 0xB8, 0xE3, 0x19, 0xF3,
         0x01, 0xF0, 0x03, 0xF8, 0x0F, 0xFE, 0xFF, 0xFF };
#else
    static unsigned char close_bits[]={
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xf3, 0xcf, 0xf9,
         0x9f, 0xfc, 0x3f, 0xfe, 0x3f, 0xfe, 0x9f, 0xfc, 0xcf, 0xf9, 0xe7, 0xf3,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

    static unsigned char maximize_bits[] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xf0, 0xf7, 0xf7, 0x07, 0xf0,
        0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x07, 0xf0,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    static unsigned char restore_bits[]={
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xf0, 0x1f, 0xf0, 0xdf, 0xf7,
        0x07, 0xf4, 0x07, 0xf4, 0xf7, 0xf5, 0xf7, 0xf1, 0xf7, 0xfd, 0xf7, 0xfd,
        0x07, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
        
    static unsigned char pin_bits[]={
        0xff,0xff,0xff,0xff,0xff,0xff,0x1f,0xfc,0xdf,0xfc,0xdf,0xfc,
        0xdf,0xfc,0xdf,0xfc,0xdf,0xfc,0x0f,0xf8,0x7f,0xff,0x7f,0xff,
        0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

#ifdef __WXMAC__
    m_inactive_close_bitmap = BitmapFromBits(close_bits, 16, 16, *wxWHITE);
    m_active_close_bitmap = BitmapFromBits(close_bits, 16, 16, *wxWHITE );
#else
    m_inactive_close_bitmap = BitmapFromBits(close_bits, 16, 16, m_inactive_caption_text_colour);
    m_active_close_bitmap = BitmapFromBits(close_bits, 16, 16, m_active_caption_text_colour);
#endif

#ifdef __WXMAC__
    m_inactive_maximize_bitmap = BitmapFromBits(maximize_bits, 16, 16, *wxWHITE);
    m_active_maximize_bitmap = BitmapFromBits(maximize_bits, 16, 16, *wxWHITE );
#else
    m_inactive_maximize_bitmap = BitmapFromBits(maximize_bits, 16, 16, m_inactive_caption_text_colour);
    m_active_maximize_bitmap = BitmapFromBits(maximize_bits, 16, 16, m_active_caption_text_colour);
#endif

#ifdef __WXMAC__
    m_inactive_restore_bitmap = BitmapFromBits(restore_bits, 16, 16, *wxWHITE);
    m_active_restore_bitmap = BitmapFromBits(restore_bits, 16, 16, *wxWHITE );
#else
    m_inactive_restore_bitmap = BitmapFromBits(restore_bits, 16, 16, m_inactive_caption_text_colour);
    m_active_restore_bitmap = BitmapFromBits(restore_bits, 16, 16, m_active_caption_text_colour);
#endif

    m_inactive_pin_bitmap = BitmapFromBits(pin_bits, 16, 16, m_inactive_caption_text_colour);
    m_active_pin_bitmap = BitmapFromBits(pin_bits, 16, 16, m_active_caption_text_colour);

    // default metric values
#if defined(__WXMAC__)
    SInt32 height;
    GetThemeMetric( kThemeMetricSmallPaneSplitterHeight , &height );
    m_sash_size = height;
#elif defined(__WXGTK__)
    m_sash_size = wxRendererNative::Get().GetSplitterParams(NULL).widthSash;
#else
    m_sash_size = 4;
#endif
    m_caption_size = 17;
    m_border_size = 1;
    m_button_size = 14;
    m_gripper_size = 9;
    m_gradient_type = wxAUI_GRADIENT_VERTICAL;
}

int wxAuiDefaultDockArt::GetMetric(int id)
{
    switch (id)
    {
        case wxAUI_ART_SASH_SIZE:          return m_sash_size;
        case wxAUI_ART_CAPTION_SIZE:       return m_caption_size;
        case wxAUI_ART_GRIPPER_SIZE:       return m_gripper_size;
        case wxAUI_ART_PANE_BORDER_SIZE:   return m_border_size;
        case wxAUI_ART_PANE_BUTTON_SIZE:   return m_button_size;
        case wxAUI_ART_GRADIENT_TYPE:      return m_gradient_type;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }

    return 0;
}

void wxAuiDefaultDockArt::SetMetric(int id, int new_val)
{
    switch (id)
    {
        case wxAUI_ART_SASH_SIZE:          m_sash_size = new_val; break;
        case wxAUI_ART_CAPTION_SIZE:       m_caption_size = new_val; break;
        case wxAUI_ART_GRIPPER_SIZE:       m_gripper_size = new_val; break;
        case wxAUI_ART_PANE_BORDER_SIZE:   m_border_size = new_val; break;
        case wxAUI_ART_PANE_BUTTON_SIZE:   m_button_size = new_val; break;
        case wxAUI_ART_GRADIENT_TYPE:      m_gradient_type = new_val; break;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }
}

wxColour wxAuiDefaultDockArt::GetColour(int id)
{
    switch (id)
    {
        case wxAUI_ART_BACKGROUND_COLOUR:                return m_background_brush.GetColour();
        case wxAUI_ART_SASH_COLOUR:                      return m_sash_brush.GetColour();
        case wxAUI_ART_INACTIVE_CAPTION_COLOUR:          return m_inactive_caption_colour;
        case wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR: return m_inactive_caption_gradient_colour;
        case wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR:     return m_inactive_caption_text_colour;
        case wxAUI_ART_ACTIVE_CAPTION_COLOUR:            return m_active_caption_colour;
        case wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR:   return m_active_caption_gradient_colour;
        case wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR:       return m_active_caption_text_colour;
        case wxAUI_ART_BORDER_COLOUR:                    return m_border_pen.GetColour();
        case wxAUI_ART_GRIPPER_COLOUR:                   return m_gripper_brush.GetColour();
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }

    return wxColour();
}

void wxAuiDefaultDockArt::SetColour(int id, const wxColor& colour)
{
    switch (id)
    {
        case wxAUI_ART_BACKGROUND_COLOUR:                m_background_brush.SetColour(colour); break;
        case wxAUI_ART_SASH_COLOUR:                      m_sash_brush.SetColour(colour); break;
        case wxAUI_ART_INACTIVE_CAPTION_COLOUR:          m_inactive_caption_colour = colour; break;
        case wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR: m_inactive_caption_gradient_colour = colour; break;
        case wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR:     m_inactive_caption_text_colour = colour; break;
        case wxAUI_ART_ACTIVE_CAPTION_COLOUR:            m_active_caption_colour = colour; break;
        case wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR:   m_active_caption_gradient_colour = colour; break;
        case wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR:       m_active_caption_text_colour = colour; break;
        case wxAUI_ART_BORDER_COLOUR:                    m_border_pen.SetColour(colour); break;
        case wxAUI_ART_GRIPPER_COLOUR:
            m_gripper_brush.SetColour(colour);
            m_gripper_pen1.SetColour(StepColour(colour, 40));
            m_gripper_pen2.SetColour(StepColour(colour, 60));
            break;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }
}

void wxAuiDefaultDockArt::SetFont(int id, const wxFont& font)
{
    if (id == wxAUI_ART_CAPTION_FONT)
        m_caption_font = font;
}

wxFont wxAuiDefaultDockArt::GetFont(int id)
{
    if (id == wxAUI_ART_CAPTION_FONT)
        return m_caption_font;
    return wxNullFont;
}

void wxAuiDefaultDockArt::DrawSash(wxDC& dc, wxWindow *window, int orientation, const wxRect& rect)
{
#if defined(__WXMAC__)
    HIRect splitterRect = CGRectMake( rect.x , rect.y , rect.width , rect.height );
    CGContextRef cgContext ;
#if wxMAC_USE_CORE_GRAPHICS
    cgContext = (CGContextRef) dc.GetGraphicsContext()->GetNativeContext() ;
#else
    Rect bounds ;
    GetPortBounds( (CGrafPtr) dc.m_macPort , &bounds ) ;
    QDBeginCGContext( (CGrafPtr) dc.m_macPort , &cgContext ) ;
    CGContextTranslateCTM( cgContext , 0 , bounds.bottom - bounds.top ) ;
    CGContextScaleCTM( cgContext , 1 , -1 ) ;
#endif

    HIThemeSplitterDrawInfo drawInfo ;
    drawInfo.version = 0 ;
    drawInfo.state = kThemeStateActive ;
    drawInfo.adornment = kHIThemeSplitterAdornmentNone ;
    HIThemeDrawPaneSplitter( &splitterRect , &drawInfo , cgContext , kHIThemeOrientationNormal ) ;

#if wxMAC_USE_CORE_GRAPHICS
#else
    QDEndCGContext( (CGrafPtr) dc.m_macPort , &cgContext ) ;
#endif

#elif defined(__WXGTK__)

    GdkRectangle gdk_rect;
    if (orientation == wxVERTICAL )
    {
        gdk_rect.x = rect.x;
        gdk_rect.y = rect.y;
        gdk_rect.width = m_sash_size;
        gdk_rect.height = rect.height;
    }
    else
    {
        gdk_rect.x = rect.x;
        gdk_rect.y = rect.y;
        gdk_rect.width = rect.width;
        gdk_rect.height = m_sash_size;
    }

    if (!window) return;
    if (!window->m_wxwindow) return;
    if (!GTK_PIZZA(window->m_wxwindow)->bin_window) return;

    gtk_paint_handle
    (
        window->m_wxwindow->style,
        GTK_PIZZA(window->m_wxwindow)->bin_window,
        // flags & wxCONTROL_CURRENT ? GTK_STATE_PRELIGHT : GTK_STATE_NORMAL,
        GTK_STATE_NORMAL,
        GTK_SHADOW_NONE,
        NULL /* no clipping */,
        window->m_wxwindow,
        "paned",
        rect.x,
        rect.y,
        rect.width,
        rect.height,
        (orientation == wxVERTICAL) ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL
    );

#else
    wxUnusedVar(window);
    wxUnusedVar(orientation);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_sash_brush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
#endif
}


void wxAuiDefaultDockArt::DrawBackground(wxDC& dc, wxWindow *WXUNUSED(window), int, const wxRect& rect)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
#ifdef __WXMAC__
    // we have to clear first, otherwise we are drawing a light striped pattern
    // over an already darker striped background
    dc.SetBrush(*wxWHITE_BRUSH) ;
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
#endif
    dc.SetBrush(m_background_brush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}

void wxAuiDefaultDockArt::DrawBorder(wxDC& dc, wxWindow *WXUNUSED(window), const wxRect& _rect,
                                  wxAuiPaneInfo& pane)
{
    dc.SetPen(m_border_pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    wxRect rect = _rect;
    int i, border_width = GetMetric(wxAUI_ART_PANE_BORDER_SIZE);

    if (pane.IsToolbar())
    {
        for (i = 0; i < border_width; ++i)
        {
            dc.SetPen(*wxWHITE_PEN);
            dc.DrawLine(rect.x, rect.y, rect.x+rect.width, rect.y);
            dc.DrawLine(rect.x, rect.y, rect.x, rect.y+rect.height);
            dc.SetPen(m_border_pen);
            dc.DrawLine(rect.x, rect.y+rect.height-1,
                        rect.x+rect.width, rect.y+rect.height-1);
            dc.DrawLine(rect.x+rect.width-1, rect.y,
                        rect.x+rect.width-1, rect.y+rect.height);
            rect.Deflate(1);
        }
    }
    else
    {
        for (i = 0; i < border_width; ++i)
        {
            dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
            rect.Deflate(1);
        }
    }
}


void wxAuiDefaultDockArt::DrawCaptionBackground(wxDC& dc, const wxRect& rect, bool active)
{
    if (m_gradient_type == wxAUI_GRADIENT_NONE)
    {
        if (active)
            dc.SetBrush(wxBrush(m_active_caption_colour));
             else
            dc.SetBrush(wxBrush(m_inactive_caption_colour));

        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    }
    else
    {
        if (active)
        {
            // on mac the gradients are expected to become darker from the top
#ifdef __WXMAC__
            DrawGradientRectangle(dc, rect,
                                 m_active_caption_colour,
                                 m_active_caption_gradient_colour,
                                 m_gradient_type);
#else
            // on other platforms, active gradients become lighter at the top
            DrawGradientRectangle(dc, rect,
                                 m_active_caption_gradient_colour,
                                 m_active_caption_colour,
                                 m_gradient_type);
#endif
        }
         else
        {
#ifdef __WXMAC__
            // on mac the gradients are expected to become darker from the top
            DrawGradientRectangle(dc, rect,
                                 m_inactive_caption_gradient_colour,
                                 m_inactive_caption_colour,
                                 m_gradient_type);
#else
            // on other platforms, inactive gradients become lighter at the bottom
            DrawGradientRectangle(dc, rect,
                                 m_inactive_caption_colour,
                                 m_inactive_caption_gradient_colour,
                                 m_gradient_type);
#endif
        }
    }
}


void wxAuiDefaultDockArt::DrawCaption(wxDC& dc, wxWindow *WXUNUSED(window),
                                   const wxString& text,
                                   const wxRect& rect,
                                   wxAuiPaneInfo& pane)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetFont(m_caption_font);

    DrawCaptionBackground(dc, rect,
                          (pane.state & wxAuiPaneInfo::optionActive)?true:false);

    if (pane.state & wxAuiPaneInfo::optionActive)
        dc.SetTextForeground(m_active_caption_text_colour);
    else
        dc.SetTextForeground(m_inactive_caption_text_colour);


    wxCoord w,h;
    dc.GetTextExtent(wxT("ABCDEFHXfgkj"), &w, &h);

    dc.SetClippingRegion(rect);
    dc.DrawText(text, rect.x+3, rect.y+(rect.height/2)-(h/2)-1);
    dc.DestroyClippingRegion();
}

void wxAuiDefaultDockArt::DrawGripper(wxDC& dc, wxWindow *WXUNUSED(window),
                                   const wxRect& rect,
                                   wxAuiPaneInfo& pane)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_gripper_brush);

    dc.DrawRectangle(rect.x, rect.y, rect.width,rect.height);

    if (!pane.HasGripperTop())
    {
        int y = 5;
        while (1)
        {
            dc.SetPen(m_gripper_pen1);
            dc.DrawPoint(rect.x+3, rect.y+y);
            dc.SetPen(m_gripper_pen2);
            dc.DrawPoint(rect.x+3, rect.y+y+1);
            dc.DrawPoint(rect.x+4, rect.y+y);
            dc.SetPen(m_gripper_pen3);
            dc.DrawPoint(rect.x+5, rect.y+y+1);
            dc.DrawPoint(rect.x+5, rect.y+y+2);
            dc.DrawPoint(rect.x+4, rect.y+y+2);

            y += 4;
            if (y > rect.GetHeight()-5)
                break;
        }
    }
    else
    {
        int x = 5;
        while (1)
        {
            dc.SetPen(m_gripper_pen1);
            dc.DrawPoint(rect.x+x, rect.y+3);
            dc.SetPen(m_gripper_pen2);
            dc.DrawPoint(rect.x+x+1, rect.y+3);
            dc.DrawPoint(rect.x+x, rect.y+4);
            dc.SetPen(m_gripper_pen3);
            dc.DrawPoint(rect.x+x+1, rect.y+5);
            dc.DrawPoint(rect.x+x+2, rect.y+5);
            dc.DrawPoint(rect.x+x+2, rect.y+4);

            x += 4;
            if (x > rect.GetWidth()-5)
                break;
        }
    }
}

void wxAuiDefaultDockArt::DrawPaneButton(wxDC& dc, wxWindow *WXUNUSED(window),
                                      int button,
                                      int button_state,
                                      const wxRect& _rect,
                                      wxAuiPaneInfo& pane)
{
    wxRect rect = _rect;

    if (button_state == wxAUI_BUTTON_STATE_PRESSED)
    {
        rect.x++;
        rect.y++;
    }

    if (button_state == wxAUI_BUTTON_STATE_HOVER ||
        button_state == wxAUI_BUTTON_STATE_PRESSED)
    {
        if (pane.state & wxAuiPaneInfo::optionActive)
        {
            dc.SetBrush(wxBrush(StepColour(m_active_caption_colour, 120)));
            dc.SetPen(wxPen(StepColour(m_active_caption_colour, 70)));
        }
         else
        {
            dc.SetBrush(wxBrush(StepColour(m_inactive_caption_colour, 120)));
            dc.SetPen(wxPen(StepColour(m_inactive_caption_colour, 70)));
        }

        // draw the background behind the button
        dc.DrawRectangle(rect.x, rect.y, 15, 15);
    }

    wxBitmap bmp;
    switch (button)
    {
        default:
        case wxAUI_BUTTON_MAXIMIZE_RESTORE:
            if (pane.IsMaximized()) {
                if (pane.state & wxAuiPaneInfo::optionActive)
                    bmp = m_active_restore_bitmap;
                    else
                    bmp = m_inactive_restore_bitmap;
            } else {
                if (pane.state & wxAuiPaneInfo::optionActive)
                    bmp = m_active_maximize_bitmap;
                    else
                    bmp = m_inactive_maximize_bitmap;
            }
            break;
        case wxAUI_BUTTON_CLOSE:
            if (pane.state & wxAuiPaneInfo::optionActive)
                bmp = m_active_close_bitmap;
                 else
                bmp = m_inactive_close_bitmap;
            break;
        case wxAUI_BUTTON_PIN:
            if (pane.state & wxAuiPaneInfo::optionActive)
                bmp = m_active_pin_bitmap;
                 else
                bmp = m_inactive_pin_bitmap;
            break;
    }

    // draw the button itself
    dc.DrawBitmap(bmp, rect.x, rect.y, true);
}


#endif // wxUSE_AUI
