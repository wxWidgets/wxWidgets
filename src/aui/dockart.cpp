///////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/dockart.cpp
// Purpose:     wxaui: wx advanced user interface - docking window manager
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2005-05-17
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
#include "wx/aui/auibook.h"
#include "wx/aui/tabart.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/image.h"
#endif

#ifdef __WXMAC__
#include "wx/osx/private.h"
#include "wx/graphics.h"
#include "wx/dcgraph.h"
#endif

#ifdef __WXGTK__
#include "wx/renderer.h"
#ifdef __WXGTK20__
    #include "wx/gtk/private/wrapgtk.h"
#else
    #include <gtk/gtk.h>
    #define gtk_widget_is_drawable GTK_WIDGET_DRAWABLE
#endif
#ifdef __WXGTK3__
    #include "wx/graphics.h"
    #include "wx/gtk/private.h"
#endif
#endif

#include <math.h>

wxColor wxAuiLightContrastColour(const wxColour& c)
{
    int amount = 120;

    // if the color is especially dark, then
    // make the contrast even lighter
    if (c.Red() < 128 && c.Green() < 128 && c.Blue() < 128)
        amount = 160;

    return c.ChangeLightness(amount);
}

inline float wxAuiGetSRGB(float r) {
    return r <= 0.03928 ? r/12.92 : pow((r+0.055)/1.055, 2.4);
}

float wxAuiGetRelativeLuminance(const wxColour& c)
{
    // based on https://www.w3.org/TR/2008/REC-WCAG20-20081211/#relativeluminancedef
    return 0.2126 * wxAuiGetSRGB(c.Red()/255.0)
        + 0.7152 * wxAuiGetSRGB(c.Green()/255.0)
        + 0.0722 * wxAuiGetSRGB(c.Blue()/255.0);
}

float wxAuiGetColourContrast(const wxColour& c1, const wxColour& c2)
{
    // based on https://www.w3.org/TR/UNDERSTANDING-WCAG20/visual-audio-contrast7.html
    float L1 = wxAuiGetRelativeLuminance(c1);
    float L2 = wxAuiGetRelativeLuminance(c2);
    return L1 > L2 ? (L1 + 0.05) / (L2 + 0.05) : (L2 + 0.05) / (L1 + 0.05);
}

// wxAuiBitmapFromBits() is a utility function that creates a
// masked bitmap from raw bits (XBM format)
wxBitmap wxAuiBitmapFromBits(const unsigned char bits[], int w, int h,
                             const wxColour& color)
{
    wxImage img = wxBitmap((const char*)bits, w, h).ConvertToImage();
    if (color.Alpha() == wxALPHA_OPAQUE)
    {
        img.Replace(0,0,0,123,123,123);
        img.Replace(255,255,255,color.Red(),color.Green(),color.Blue());
        img.SetMaskColour(123,123,123);
    }
    else
    {
        img.InitAlpha();
        const int newr = color.Red();
        const int newg = color.Green();
        const int newb = color.Blue();
        const int newa = color.Alpha();
        for (int x = 0; x < w; x++)
        {
            for (int y = 0; y < h; y++)
            {
                int r = img.GetRed(x, y);
                int g = img.GetGreen(x, y);
                int b = img.GetBlue(x, y);
                if (r == 0 && g == 0 && b == 0)
                {
                    img.SetAlpha(x, y, wxALPHA_TRANSPARENT);
                }
                else
                {
                    img.SetRGB(x, y, newr, newg, newb);
                    img.SetAlpha(x, y, newa);
                }
            }
        }
    }
    return wxBitmap(img);
}

// A utility function to scales a bitmap in place for use at the given scale
// factor.
void wxAuiScaleBitmap(wxBitmap& bmp, double scale)
{
#if wxUSE_IMAGE && !defined(__WXGTK3__) && !defined(__WXMAC__)
    // scale to a close round number to improve quality
    scale = floor(scale + 0.25);
    if (scale > 1.0 && !(bmp.GetScaleFactor() > 1.0))
    {
        wxImage img = bmp.ConvertToImage();
        img.Rescale(bmp.GetWidth()*scale, bmp.GetHeight()*scale,
            wxIMAGE_QUALITY_BOX_AVERAGE);
        bmp = wxBitmap(img);
    }
#else
    wxUnusedVar(bmp);
    wxUnusedVar(scale);
#endif // wxUSE_IMAGE
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


        r = start_color.Red() + (high <= 0 ? 0 : (((i*rd*100)/high)/100));
        g = start_color.Green() + (high <= 0 ? 0 : (((i*gd*100)/high)/100));
        b = start_color.Blue() + (high <= 0 ? 0 : (((i*bd*100)/high)/100));

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

wxString wxAuiChopText(wxDC& dc, const wxString& text, int max_size)
{
    wxCoord x,y;

    // first check if the text fits with no problems
    dc.GetTextExtent(text, &x, &y);
    if (x <= max_size)
        return text;

    size_t i, len = text.Length();
    size_t last_good_length = 0;
    for (i = 0; i < len; ++i)
    {
        wxString s = text.Left(i);
        s += wxT("...");

        dc.GetTextExtent(s, &x, &y);
        if (x > max_size)
            break;

        last_good_length = i;
    }

    wxString ret = text.Left(last_good_length);
    ret += wxT("...");
    return ret;
}

// -- wxAuiDefaultDockArt class implementation --

// wxAuiDefaultDockArt is an art provider class which does all of the drawing for
// wxAuiManager.  This allows the library caller to customize the dock art
// (probably by deriving from this class), or to completely replace all drawing
// with custom dock art (probably by writing a new stand-alone class derived
// from the wxAuiDockArt base class). The active dock art class can be set via
// wxAuiManager::SetDockArt()
wxAuiDefaultDockArt::wxAuiDefaultDockArt()
{
    UpdateColoursFromSystem();

#ifdef __WXMAC__
    m_captionFont = *wxSMALL_FONT;
#else
    m_captionFont = wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#endif

    // default metric values
#if defined( __WXMAC__ ) && wxOSX_USE_COCOA_OR_CARBON
    SInt32 height;
    GetThemeMetric( kThemeMetricSmallPaneSplitterHeight , &height );
    m_sashSize     = height;
#elif defined(__WXGTK__)
    m_sashSize     = wxRendererNative::Get().GetSplitterParams(NULL).widthSash;
#else
    m_sashSize     = wxWindow::FromDIP( 4, NULL);
#endif
    m_captionSize  = wxWindow::FromDIP(17, NULL);
    m_borderSize   = 1;
    m_buttonSize   = wxWindow::FromDIP(14, NULL);
    m_gripperSize  = wxWindow::FromDIP( 9, NULL);
    m_gradientType = wxAUI_GRADIENT_VERTICAL;

    InitBitmaps();
}

void
wxAuiDefaultDockArt::InitBitmaps ()
{
    // some built in bitmaps
    // TODO: Provide x1.5 and x2.0 versions or migrate to SVG.
#if defined( __WXMAC__ )
     static const unsigned char close_bits[]={
         0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xFE, 0x03, 0xF8, 0x01, 0xF0, 0x19, 0xF3,
         0xB8, 0xE3, 0xF0, 0xE1, 0xE0, 0xE0, 0xF0, 0xE1, 0xB8, 0xE3, 0x19, 0xF3,
         0x01, 0xF0, 0x03, 0xF8, 0x0F, 0xFE, 0xFF, 0xFF };
#elif defined(__WXGTK__)
     static const unsigned char close_bits[]={
         0xff, 0xff, 0xff, 0xff, 0x07, 0xf0, 0xfb, 0xef, 0xdb, 0xed, 0x8b, 0xe8,
         0x1b, 0xec, 0x3b, 0xee, 0x1b, 0xec, 0x8b, 0xe8, 0xdb, 0xed, 0xfb, 0xef,
         0x07, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
#else
    static const unsigned char close_bits[]={
         // reduced height, symmetric
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xf3, 0x9f, 0xf9,
         0x3f, 0xfc, 0x7f, 0xfe, 0x3f, 0xfc, 0x9f, 0xf9, 0xcf, 0xf3, 0xff, 0xff,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
      /*
         // same height as maximize/restore
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xe7, 0xcf, 0xf3, 0x9f, 0xf9,
         0x3f, 0xfc, 0x7f, 0xfe, 0x3f, 0xfc, 0x9f, 0xf9, 0xcf, 0xf3, 0xe7, 0xe7,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
      */
#endif

    static const unsigned char maximize_bits[] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xf0, 0xf7, 0xf7, 0x07, 0xf0,
        0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0x07, 0xf0,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    static const unsigned char restore_bits[]={
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xf0, 0x1f, 0xf0, 0xdf, 0xf7,
        0x07, 0xf4, 0x07, 0xf4, 0xf7, 0xf5, 0xf7, 0xf1, 0xf7, 0xfd, 0xf7, 0xfd,
        0x07, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    static const unsigned char pin_bits[]={
        0xff,0xff,0xff,0xff,0xff,0xff,0x1f,0xfc,0xdf,0xfc,0xdf,0xfc,
        0xdf,0xfc,0xdf,0xfc,0xdf,0xfc,0x0f,0xf8,0x7f,0xff,0x7f,0xff,
        0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

#ifdef __WXMAC__
    const wxColour inactive = wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION);
    const wxColour active = wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT);
#else
    const wxColor inactive = m_inactiveCaptionTextColour;
    const wxColor active = m_activeCaptionTextColour;
#endif

    m_inactiveCloseBitmap = wxAuiBitmapFromBits(close_bits, 16, 16, inactive);
    m_activeCloseBitmap = wxAuiBitmapFromBits(close_bits, 16, 16, active);

    m_inactiveMaximizeBitmap = wxAuiBitmapFromBits(maximize_bits, 16, 16, inactive);
    m_activeMaximizeBitmap = wxAuiBitmapFromBits(maximize_bits, 16, 16, active);

    m_inactiveRestoreBitmap = wxAuiBitmapFromBits(restore_bits, 16, 16, inactive);
    m_activeRestoreBitmap = wxAuiBitmapFromBits(restore_bits, 16, 16, active);

    m_inactivePinBitmap = wxAuiBitmapFromBits(pin_bits, 16, 16, inactive);
    m_activePinBitmap = wxAuiBitmapFromBits(pin_bits, 16, 16, active);
}

void wxAuiDefaultDockArt::UpdateColoursFromSystem()
{
    wxColor baseColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

    // the baseColour is too pale to use as our base colour,
    // so darken it a bit --
    if ((255-baseColour.Red()) +
        (255-baseColour.Green()) +
        (255-baseColour.Blue()) < 60)
    {
        baseColour = baseColour.ChangeLightness(92);
    }

    m_baseColour = baseColour;
    wxColor darker1Colour = baseColour.ChangeLightness(85);
    wxColor darker2Colour = baseColour.ChangeLightness(75);
    wxColor darker3Colour = baseColour.ChangeLightness(60);
    //wxColor darker4Colour = baseColour.ChangeLightness(50);
    wxColor darker5Colour = baseColour.ChangeLightness(40);

    m_activeCaptionColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    m_activeCaptionGradientColour = wxAuiLightContrastColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    m_activeCaptionTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    m_inactiveCaptionColour = darker1Colour;
    m_inactiveCaptionGradientColour = baseColour.ChangeLightness(97);
    m_inactiveCaptionTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTIONTEXT);

    m_sashBrush = wxBrush(baseColour);
    m_backgroundBrush = wxBrush(baseColour);
    m_gripperBrush = wxBrush(baseColour);

    m_borderPen = wxPen(darker2Colour);
    int pen_width = wxWindow::FromDIP(1, NULL);
    m_gripperPen1 = wxPen(darker5Colour, pen_width);
    m_gripperPen2 = wxPen(darker3Colour, pen_width);
    m_gripperPen3 = wxPen(*wxStockGDI::GetColour(wxStockGDI::COLOUR_WHITE), pen_width);
    InitBitmaps();
}

int wxAuiDefaultDockArt::GetMetric(int id)
{
    switch (id)
    {
        case wxAUI_DOCKART_SASH_SIZE:          return m_sashSize;
        case wxAUI_DOCKART_CAPTION_SIZE:       return m_captionSize;
        case wxAUI_DOCKART_GRIPPER_SIZE:       return m_gripperSize;
        case wxAUI_DOCKART_PANE_BORDER_SIZE:   return m_borderSize;
        case wxAUI_DOCKART_PANE_BUTTON_SIZE:   return m_buttonSize;
        case wxAUI_DOCKART_GRADIENT_TYPE:      return m_gradientType;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }

    return 0;
}

void wxAuiDefaultDockArt::SetMetric(int id, int new_val)
{
    switch (id)
    {
        case wxAUI_DOCKART_SASH_SIZE:          m_sashSize = new_val; break;
        case wxAUI_DOCKART_CAPTION_SIZE:       m_captionSize = new_val; break;
        case wxAUI_DOCKART_GRIPPER_SIZE:       m_gripperSize = new_val; break;
        case wxAUI_DOCKART_PANE_BORDER_SIZE:   m_borderSize = new_val; break;
        case wxAUI_DOCKART_PANE_BUTTON_SIZE:   m_buttonSize = new_val; break;
        case wxAUI_DOCKART_GRADIENT_TYPE:      m_gradientType = new_val; break;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }
}

wxColour wxAuiDefaultDockArt::GetColour(int id)
{
    switch (id)
    {
        case wxAUI_DOCKART_BACKGROUND_COLOUR:                return m_backgroundBrush.GetColour();
        case wxAUI_DOCKART_SASH_COLOUR:                      return m_sashBrush.GetColour();
        case wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR:          return m_inactiveCaptionColour;
        case wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR: return m_inactiveCaptionGradientColour;
        case wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR:     return m_inactiveCaptionTextColour;
        case wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR:            return m_activeCaptionColour;
        case wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR:   return m_activeCaptionGradientColour;
        case wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR:       return m_activeCaptionTextColour;
        case wxAUI_DOCKART_BORDER_COLOUR:                    return m_borderPen.GetColour();
        case wxAUI_DOCKART_GRIPPER_COLOUR:                   return m_gripperBrush.GetColour();
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }

    return wxColour();
}

void wxAuiDefaultDockArt::SetColour(int id, const wxColor& colour)
{
    switch (id)
    {
        case wxAUI_DOCKART_BACKGROUND_COLOUR:                m_backgroundBrush.SetColour(colour); break;
        case wxAUI_DOCKART_SASH_COLOUR:                      m_sashBrush.SetColour(colour); break;
        case wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR:          m_inactiveCaptionColour = colour; break;
        case wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR: m_inactiveCaptionGradientColour = colour; break;
        case wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR:     m_inactiveCaptionTextColour = colour; break;
        case wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR:            m_activeCaptionColour = colour; break;
        case wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR:   m_activeCaptionGradientColour = colour; break;
        case wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR:       m_activeCaptionTextColour = colour; break;
        case wxAUI_DOCKART_BORDER_COLOUR:                    m_borderPen.SetColour(colour); break;
        case wxAUI_DOCKART_GRIPPER_COLOUR:
            m_gripperBrush.SetColour(colour);
            m_gripperPen1.SetColour(colour.ChangeLightness(40));
            m_gripperPen2.SetColour(colour.ChangeLightness(60));
            break;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }

    InitBitmaps();
}

void wxAuiDefaultDockArt::SetFont(int id, const wxFont& font)
{
    if (id == wxAUI_DOCKART_CAPTION_FONT)
        m_captionFont = font;
}

wxFont wxAuiDefaultDockArt::GetFont(int id)
{
    if (id == wxAUI_DOCKART_CAPTION_FONT)
        return m_captionFont;
    return wxNullFont;
}

void wxAuiDefaultDockArt::DrawSash(wxDC& dc, wxWindow *window, int orientation, const wxRect& rect)
{
#if defined( __WXMAC__ ) && wxOSX_USE_COCOA_OR_CARBON
    wxUnusedVar(window);
    wxUnusedVar(orientation);

    if ( wxPlatformInfo::Get().CheckOSVersion(10, 14) && wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).Red() < 128 )
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_sashBrush);
        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    }
    else
    {
        HIRect splitterRect = CGRectMake( rect.x , rect.y , rect.width , rect.height );
        CGContextRef cgContext ;
        wxGCDCImpl *impl = (wxGCDCImpl*) dc.GetImpl();
        cgContext = (CGContextRef) impl->GetGraphicsContext()->GetNativeContext() ;

        HIThemeSplitterDrawInfo drawInfo ;
        drawInfo.version = 0 ;
        drawInfo.state = kThemeStateActive ;
        drawInfo.adornment = kHIThemeSplitterAdornmentNone ;
        HIThemeDrawPaneSplitter( &splitterRect , &drawInfo , cgContext , kHIThemeOrientationNormal ) ;
    }

#elif defined(__WXGTK__)
    // clear out the rectangle first
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_sashBrush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

#if 0
    GdkRectangle gdk_rect;
    if (orientation == wxVERTICAL )
    {
        gdk_rect.x = rect.x;
        gdk_rect.y = rect.y;
        gdk_rect.width = m_sashSize;
        gdk_rect.height = rect.height;
    }
    else
    {
        gdk_rect.x = rect.x;
        gdk_rect.y = rect.y;
        gdk_rect.width = rect.width;
        gdk_rect.height = m_sashSize;
    }
#endif

    if (!window) return;
    if (!window->m_wxwindow) return;
    if (!gtk_widget_is_drawable(window->m_wxwindow)) return;

#ifdef __WXGTK3__
    cairo_t* cr = static_cast<cairo_t*>(dc.GetGraphicsContext()->GetNativeContext());
    // invert orientation for widget (horizontal GtkPaned has a vertical splitter)
    wxOrientation orient = orientation == wxVERTICAL ? wxHORIZONTAL : wxVERTICAL;
    GtkWidget* widget = wxGTKPrivate::GetSplitterWidget(orient);
    GtkStyleContext* sc = gtk_widget_get_style_context(widget);
    gtk_style_context_save(sc);

    gtk_style_context_add_class(sc, GTK_STYLE_CLASS_PANE_SEPARATOR);
    gtk_render_handle(sc, cr, rect.x, rect.y, rect.width, rect.height);

    gtk_style_context_restore(sc);
#else
    gtk_paint_handle
    (
        gtk_widget_get_style(window->m_wxwindow),
        window->GTKGetDrawingWindow(),
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
#endif // !__WXGTK3__

#else
    wxUnusedVar(window);
    wxUnusedVar(orientation);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_sashBrush);
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
    dc.SetBrush(m_backgroundBrush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}

void wxAuiDefaultDockArt::DrawBorder(wxDC& dc, wxWindow* window, const wxRect& _rect,
                                  wxAuiPaneInfo& pane)
{
    dc.SetPen(m_borderPen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    wxRect rect = _rect;
    int i, border_width = GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE);

    if (pane.IsToolbar())
    {
        for (i = 0; i < border_width; ++i)
        {
            dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
            dc.DrawLine(rect.x, rect.y, rect.x+rect.width, rect.y);
            dc.DrawLine(rect.x, rect.y, rect.x, rect.y+rect.height);
            dc.SetPen(m_borderPen);
            dc.DrawLine(rect.x, rect.y+rect.height-1,
                        rect.x+rect.width, rect.y+rect.height-1);
            dc.DrawLine(rect.x+rect.width-1, rect.y,
                        rect.x+rect.width-1, rect.y+rect.height);
            rect.Deflate(1);
        }
    }
    else
    {
        // notebooks draw the border themselves, so they can use native rendering (e.g. tabartgtk)
        wxAuiTabArt* art = 0;
        wxAuiNotebook* nb = wxDynamicCast(window, wxAuiNotebook);
        if (nb)
            art = nb->GetArtProvider();

        if (art)
            art->DrawBorder(dc, window, rect);
        else
        {
            for (i = 0; i < border_width; ++i)
            {
                dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
                rect.Deflate(1);
            }
        }
    }
}


void wxAuiDefaultDockArt::DrawCaptionBackground(wxDC& dc, const wxRect& rect, bool active)
{
    if (m_gradientType == wxAUI_GRADIENT_NONE)
    {
        if (active)
            dc.SetBrush(wxBrush(m_activeCaptionColour));
        else
            dc.SetBrush(wxBrush(m_inactiveCaptionColour));

        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    }
    else
    {
        if (active)
        {
            // on mac the gradients are expected to become darker from the top
#ifdef __WXMAC__
            DrawGradientRectangle(dc, rect,
                                 m_activeCaptionColour,
                                 m_activeCaptionGradientColour,
                                 m_gradientType);
#else
            // on other platforms, active gradients become lighter at the top
            DrawGradientRectangle(dc, rect,
                                 m_activeCaptionGradientColour,
                                 m_activeCaptionColour,
                                 m_gradientType);
#endif
        }
        else
        {
#ifdef __WXMAC__
            // on mac the gradients are expected to become darker from the top
            DrawGradientRectangle(dc, rect,
                                 m_inactiveCaptionGradientColour,
                                 m_inactiveCaptionColour,
                                 m_gradientType);
#else
            // on other platforms, inactive gradients become lighter at the bottom
            DrawGradientRectangle(dc, rect,
                                 m_inactiveCaptionColour,
                                 m_inactiveCaptionGradientColour,
                                 m_gradientType);
#endif
        }
    }
}


void wxAuiDefaultDockArt::DrawCaption(wxDC& dc,
                                   wxWindow* window,
                                   const wxString& text,
                                   const wxRect& rect,
                                   wxAuiPaneInfo& pane)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetFont(m_captionFont);

    DrawCaptionBackground(dc, rect,
                          (pane.state & wxAuiPaneInfo::optionActive)?true:false);

    int caption_offset = 0;
    if ( pane.icon.IsOk() )
    {
        DrawIcon(dc, window, rect, pane);

        caption_offset += pane.icon.GetScaledWidth() + window->FromDIP(3);
    }

    if (pane.state & wxAuiPaneInfo::optionActive)
        dc.SetTextForeground(m_activeCaptionTextColour);
    else
        dc.SetTextForeground(m_inactiveCaptionTextColour);


    wxCoord w,h;
    dc.GetTextExtent(wxT("ABCDEFHXfgkj"), &w, &h);

    wxRect clip_rect = rect;
    clip_rect.width -= window->FromDIP(3); // text offset
    clip_rect.width -= window->FromDIP(2); // button padding
    if (pane.HasCloseButton())
        clip_rect.width -= m_buttonSize;
    if (pane.HasPinButton())
        clip_rect.width -= m_buttonSize;
    if (pane.HasMaximizeButton())
        clip_rect.width -= m_buttonSize;

    wxString draw_text = wxAuiChopText(dc, text, clip_rect.width);

    dc.SetClippingRegion(clip_rect);
    dc.DrawText(draw_text, rect.x+window->FromDIP(3) + caption_offset, rect.y+(rect.height/2)-(h/2)-1);
    dc.DestroyClippingRegion();
}

#if WXWIN_COMPATIBILITY_3_0
void wxAuiDefaultDockArt::DrawIcon(wxDC& dc, const wxRect& rect, wxAuiPaneInfo& pane)
{
    DrawIcon(dc, NULL, rect, pane);
}
#endif

void
wxAuiDefaultDockArt::DrawIcon(wxDC& dc, wxWindow *window, const wxRect& rect, wxAuiPaneInfo& pane)
{
    // Draw the icon centered vertically
    int xOffset = window ? window->FromDIP(2) : 2;
    dc.DrawBitmap(pane.icon,
                  rect.x+xOffset, rect.y+(rect.height-pane.icon.GetScaledHeight())/2,
                  true);
}

void wxAuiDefaultDockArt::DrawGripper(wxDC& dc,
                                   wxWindow* window,
                                   const wxRect& rect,
                                   wxAuiPaneInfo& pane)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_gripperBrush);

    dc.DrawRectangle(rect.x, rect.y, rect.width,rect.height);

    if (!pane.HasGripperTop())
    {
        int y = window->FromDIP(5);
        while (1)
        {
            dc.SetPen(m_gripperPen1);
            dc.DrawPoint(rect.x+window->FromDIP(3), rect.y+y);
            dc.SetPen(m_gripperPen2);
            dc.DrawPoint(rect.x+window->FromDIP(3), rect.y+y+window->FromDIP(1));
            dc.DrawPoint(rect.x+window->FromDIP(4), rect.y+y                   );
            dc.SetPen(m_gripperPen3);
            dc.DrawPoint(rect.x+window->FromDIP(5), rect.y+y+window->FromDIP(1));
            dc.DrawPoint(rect.x+window->FromDIP(5), rect.y+y+window->FromDIP(2));
            dc.DrawPoint(rect.x+window->FromDIP(4), rect.y+y+window->FromDIP(2));

            y += window->FromDIP(4);
            if (y > rect.GetHeight()-window->FromDIP(5))
                break;
        }
    }
    else
    {
        int x = window->FromDIP(5);
        while (1)
        {
            dc.SetPen(m_gripperPen1);
            dc.DrawPoint(rect.x+x, rect.y+window->FromDIP(3));
            dc.SetPen(m_gripperPen2);
            dc.DrawPoint(rect.x+x+window->FromDIP(1), rect.y+window->FromDIP(3));
            dc.DrawPoint(rect.x+x                   , rect.y+window->FromDIP(4));
            dc.SetPen(m_gripperPen3);
            dc.DrawPoint(rect.x+x+window->FromDIP(1), rect.y+window->FromDIP(5));
            dc.DrawPoint(rect.x+x+window->FromDIP(2), rect.y+window->FromDIP(5));
            dc.DrawPoint(rect.x+x+window->FromDIP(2), rect.y+window->FromDIP(4));

            x += window->FromDIP(4);
            if (x > rect.GetWidth()-window->FromDIP(5))
                break;
        }
    }
}

void wxAuiDefaultDockArt::DrawPaneButton(wxDC& dc,
                                      wxWindow* window,
                                      int button,
                                      int button_state,
                                      const wxRect& _rect,
                                      wxAuiPaneInfo& pane)
{
    wxBitmap bmp;
    switch (button)
    {
        default:
        case wxAUI_BUTTON_CLOSE:
            if (pane.state & wxAuiPaneInfo::optionActive)
                bmp = m_activeCloseBitmap;
            else
                bmp = m_inactiveCloseBitmap;
            break;
        case wxAUI_BUTTON_PIN:
            if (pane.state & wxAuiPaneInfo::optionActive)
                bmp = m_activePinBitmap;
            else
                bmp = m_inactivePinBitmap;
            break;
        case wxAUI_BUTTON_MAXIMIZE_RESTORE:
            if (pane.IsMaximized())
            {
                if (pane.state & wxAuiPaneInfo::optionActive)
                    bmp = m_activeRestoreBitmap;
                else
                    bmp = m_inactiveRestoreBitmap;
            }
            else
            {
                if (pane.state & wxAuiPaneInfo::optionActive)
                    bmp = m_activeMaximizeBitmap;
                else
                    bmp = m_inactiveMaximizeBitmap;
            }
            break;
    }

    wxAuiScaleBitmap(bmp, window->GetDPIScaleFactor());

    wxRect rect = _rect;

    rect.y = rect.y + (rect.height/2) - (bmp.GetScaledHeight()/2);

    if (button_state == wxAUI_BUTTON_STATE_PRESSED)
    {
        rect.x += window->FromDIP(1);
        rect.y += window->FromDIP(1);
    }

    if (button_state == wxAUI_BUTTON_STATE_HOVER ||
        button_state == wxAUI_BUTTON_STATE_PRESSED)
    {
        if (pane.state & wxAuiPaneInfo::optionActive)
        {
            dc.SetBrush(wxBrush(m_activeCaptionColour.ChangeLightness(120)));
            dc.SetPen(wxPen(m_activeCaptionColour.ChangeLightness(70)));
        }
        else
        {
            dc.SetBrush(wxBrush(m_inactiveCaptionColour.ChangeLightness(120)));
            dc.SetPen(wxPen(m_inactiveCaptionColour.ChangeLightness(70)));
        }

        // draw the background behind the button
        dc.DrawRectangle(rect.x, rect.y,
            bmp.GetScaledWidth() - window->FromDIP(1),
            bmp.GetScaledHeight() - window->FromDIP(1));
    }

    // draw the button itself
    dc.DrawBitmap(bmp, rect.x, rect.y, true);
}


#endif // wxUSE_AUI
