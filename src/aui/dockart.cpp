///////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/dockart.cpp
// Purpose:     wxaui: wx advanced user interface - docking window manager
// Author:      Benjamin I. Williams
// Modified by: Malcolm MacLeod (mmacleod@webmail.co.za)
// Modified by: Jens Lody
// Created:     2005-05-17
// RCS-ID:      $Id: dockart.cpp 61738 2009-08-23 07:52:17Z MJM $
// Copyright:   (C) Copyright 2005-2006, Kirix Corporation, All Rights Reserved
//                            2012, Jens Lody for the code related to left
//                                  and right positioning
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
#include "wx/aui/tabmdi.h"
#include "wx/aui/tabart.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/image.h"
#endif

#ifdef __WXMAC__
#include "wx/osx/private.h"
#include "wx/graphics.h"
#include "wx/dcgraph.h"
#endif

#ifdef __WXGTK__
#include <gtk/gtk.h>
#ifdef __WXGTK20__
   #include "wx/gtk/private/gtk2-compat.h"
#else
   #define gtk_widget_is_drawable GTK_WIDGET_DRAWABLE
#endif
#ifdef __WXGTK3__
    #include "wx/graphics.h"
    #include "wx/gtk/private.h"
#endif
#endif

#include "wx/renderer.h"
#include "wx/arrimpl.cpp"

// -- wxAuiDefaultDockArt class implementation --

// wxAuiDefaultDockArt is an art provider class which does all of the drawing for
// wxAuiManager.  This allows the library caller to customize the dock art
// (probably by deriving from this class), or to completely replace all drawing
// with custom dock art (probably by writing a new stand-alone class derived
// from the wxAuiDockArt base class). The active dock art class can be set via
// wxAuiManager::SetDockArt()
wxColor wxAuiLightContrastColour(const wxColour& c)
{
    int amount = 120;

    // if the color is especially dark, then
    // make the contrast even lighter
    if (c.Red() < 128 && c.Green() < 128 && c.Blue() < 128)
        amount = 160;

    return c.ChangeLightness(amount);
}

// wxAuiBitmapFromBits() is a utility function that creates a
// masked bitmap from raw bits (XBM format)
wxBitmap wxAuiBitmapFromBits(const unsigned char bits[], int w, int h,
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
                                  const wxColour& startColor,
                                  const wxColour& endColor,
                                  int direction)
{
    int rd, gd, bd, high = 0;
    rd = endColor.Red() - startColor.Red();
    gd = endColor.Green() - startColor.Green();
    bd = endColor.Blue() - startColor.Blue();

    if (direction == wxAUI_GRADIENT_VERTICAL)
        high = rect.GetHeight()-1;
    else
        high = rect.GetWidth()-1;

    for (int i = 0; i <= high; ++i)
    {
        int r,g,b;


        r = startColor.Red() + (high <= 0 ? 0 : (((i*rd*100)/high)/100));
        g = startColor.Green() + (high <= 0 ? 0 : (((i*gd*100)/high)/100));
        b = startColor.Blue() + (high <= 0 ? 0 : (((i*bd*100)/high)/100));

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

wxString wxAuiChopText(wxDC& dc, const wxString& text, int maxSize)
{
    wxCoord x,y;

    // first check if the text fits with no problems
    dc.GetTextExtent(text, &x, &y);
    if (x <= maxSize)
        return text;

    size_t i, len = text.Length();
    size_t lastGoodLength = 0;
    for (i = 0; i < len; ++i)
    {
        wxString s = text.Left(i);
        s += wxT("...");

        dc.GetTextExtent(s, &x, &y);
        if (x > maxSize)
            break;

        lastGoodLength = i;
    }

    wxString ret = text.Left(lastGoodLength);
    ret += wxT("...");
    return ret;
}


bool TabHasCloseButton(unsigned int flags,wxAuiPaneInfo& page);

wxAuiDefaultDockArt::wxAuiDefaultDockArt()
{
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
    m_inactiveCaptionTextColour = *wxBLACK;

    m_sashBrush = wxBrush(baseColour);
    m_backgroundBrush = wxBrush(baseColour);
    m_gripperBrush = wxBrush(baseColour);

    m_borderPen = wxPen(darker2Colour);
    m_gripperPen1 = wxPen(darker5Colour);
    m_gripperPen2 = wxPen(darker3Colour);
    m_gripperPen3 = *wxWHITE_PEN;

#ifdef __WXMAC__
    m_captionFont = *wxSMALL_FONT;
#else
    m_captionFont = wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#endif

    // default metric values
#if defined( __WXMAC__ ) && wxOSX_USE_COCOA_OR_CARBON
    SInt32 height;
    GetThemeMetric( kThemeMetricSmallPaneSplitterHeight , &height );
    m_sashSize = height;
#elif defined(__WXGTK__)
    m_sashSize = wxRendererNative::Get().GetSplitterParams(NULL).widthSash;
#else
    m_sashSize = 4;
#endif
    m_captionSize = 17;
    m_borderSize = 1;
    m_buttonSize = 14;
    m_gripperSize = 9;
    m_gradientType = wxAUI_GRADIENT_VERTICAL;

    InitBitmaps();
}

void
wxAuiDefaultDockArt::InitBitmaps ()
{
    // some built in bitmaps
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
    m_inactiveCloseBitmap = wxAuiBitmapFromBits(close_bits, 16, 16, *wxWHITE);
    m_activeCloseBitmap = wxAuiBitmapFromBits(close_bits, 16, 16, *wxWHITE );
#else
    m_inactiveCloseBitmap = wxAuiBitmapFromBits(close_bits, 16, 16, m_inactiveCaptionTextColour);
    m_activeCloseBitmap = wxAuiBitmapFromBits(close_bits, 16, 16, m_activeCaptionTextColour);
#endif

#ifdef __WXMAC__
    m_inactiveMaximizeBitmap = wxAuiBitmapFromBits(maximize_bits, 16, 16, *wxWHITE);
    m_activeMaximizeBitmap = wxAuiBitmapFromBits(maximize_bits, 16, 16, *wxWHITE );
#else
    m_inactiveMaximizeBitmap = wxAuiBitmapFromBits(maximize_bits, 16, 16, m_inactiveCaptionTextColour);
    m_activeMaximizeBitmap = wxAuiBitmapFromBits(maximize_bits, 16, 16, m_activeCaptionTextColour);
#endif

#ifdef __WXMAC__
    m_inactiveRestoreBitmap = wxAuiBitmapFromBits(restore_bits, 16, 16, *wxWHITE);
    m_activeRestoreBitmap = wxAuiBitmapFromBits(restore_bits, 16, 16, *wxWHITE );
#else
    m_inactiveRestoreBitmap = wxAuiBitmapFromBits(restore_bits, 16, 16, m_inactiveCaptionTextColour);
    m_activeRestoreBitmap = wxAuiBitmapFromBits(restore_bits, 16, 16, m_activeCaptionTextColour);
#endif

    m_inactivePinBitmap = wxAuiBitmapFromBits(pin_bits, 16, 16, m_inactiveCaptionTextColour);
    m_activePinBitmap = wxAuiBitmapFromBits(pin_bits, 16, 16, m_activeCaptionTextColour);
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

void wxAuiDefaultDockArt::DrawSash(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect)
{
#if defined( __WXMAC__ ) && wxOSX_USE_COCOA_OR_CARBON
    wxUnusedVar(window);
    wxUnusedVar(orientation);

    HIRect splitterRect = CGRectMake( rect.x , rect.y , rect.width , rect.height );
    CGContextRef cgContext ;
    wxGCDCImpl* impl = (wxGCDCImpl*) dc.GetImpl();
    cgContext = (CGContextRef) impl->GetGraphicsContext()->GetNativeContext() ;

    HIThemeSplitterDrawInfo drawInfo ;
    drawInfo.version = 0 ;
    drawInfo.state = kThemeStateActive ;
    drawInfo.adornment = kHIThemeSplitterAdornmentNone ;
    HIThemeDrawPaneSplitter( &splitterRect , &drawInfo , cgContext , kHIThemeOrientationNormal ) ;

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


void wxAuiDefaultDockArt::DrawBackground(wxDC& dc, wxWindow* WXUNUSED(window), int, const wxRect& rect)
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
    int i, borderWidth = GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE);

    if (pane.IsToolbar())
    {
        for (i = 0; i < borderWidth; ++i)
        {
            dc.SetPen(*wxWHITE_PEN);
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
        wxAuiTabArt* art = NULL;

        // find if the pane is in a notebook
        wxAuiManager*      mgr = wxAuiManager::GetManager(pane.GetWindow());
        wxAuiTabContainer* ctrl;
        int                idx;

        if (mgr && mgr->FindTab(pane.GetWindow(), &ctrl, &idx))          
            art = mgr->GetTabArtProvider();

        if (art)
        {
            art->DrawBorder(dc, window, rect);
        }
        else
        {
            for (i = 0; i < borderWidth; ++i)
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


void wxAuiDefaultDockArt::DrawCaption(wxDC& dc, wxWindow* WXUNUSED(window),
                                   const wxString& text,
                                   const wxRect& rect,
                                   wxAuiPaneInfo& pane)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetFont(m_captionFont);
    
    wxAuiManager* mgr = wxAuiManager::GetManager(pane.GetWindow());
    bool showActive = pane.HasFlag(wxAuiPaneInfo::optionActive) && mgr && mgr->HasFlag(wxAUI_MGR_ALLOW_ACTIVE_PANE);

    DrawCaptionBackground(dc, rect, showActive);

    int captionOffset = 0;
    if ( pane.GetIcon().IsOk() )
    {
        DrawIcon(dc, rect, pane);

        captionOffset += pane.GetIcon().GetWidth() + 3;
    }


    dc.SetTextForeground(showActive ?  m_activeCaptionTextColour : m_inactiveCaptionTextColour);

    wxCoord w,h;
    dc.GetTextExtent(wxT("ABCDEFHXfgkj"), &w, &h);

    wxRect clipRect = rect;
    clipRect.width -= 3; // text offset
    clipRect.width -= 2; // button padding
    if (pane.HasCloseButton())
        clipRect.width -= m_buttonSize;
    if (pane.HasPinButton())
        clipRect.width -= m_buttonSize;
    if (pane.HasMaximizeButton())
        clipRect.width -= m_buttonSize;

    wxString drawText = wxAuiChopText(dc, text, clipRect.width);

    dc.SetClippingRegion(clipRect);
    dc.DrawText(drawText, rect.x+3 + captionOffset, rect.y+(rect.height/2)-(h/2)-1);
    dc.DestroyClippingRegion();
}

void wxAuiDefaultDockArt::DrawIcon(wxDC& dc, const wxRect& rect, wxAuiPaneInfo& pane)
{
   // Draw the icon centered vertically
   dc.DrawBitmap(pane.GetIcon(),
                 rect.x+2, rect.y+(rect.height-pane.GetIcon().GetHeight())/2,
                 true);
}

void wxAuiDefaultDockArt::DrawGripper(wxDC& dc, wxWindow* WXUNUSED(window),
                                   const wxRect& rect,
                                   wxAuiPaneInfo& pane)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_gripperBrush);

    dc.DrawRectangle(rect.x, rect.y, rect.width,rect.height);

    if (!pane.HasGripperTop())
    {
        int y = 5;
        while (1)
        {
            dc.SetPen(m_gripperPen1);
            dc.DrawPoint(rect.x+3, rect.y+y);
            dc.SetPen(m_gripperPen2);
            dc.DrawPoint(rect.x+3, rect.y+y+1);
            dc.DrawPoint(rect.x+4, rect.y+y);
            dc.SetPen(m_gripperPen3);
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
            dc.SetPen(m_gripperPen1);
            dc.DrawPoint(rect.x+x, rect.y+3);
            dc.SetPen(m_gripperPen2);
            dc.DrawPoint(rect.x+x+1, rect.y+3);
            dc.DrawPoint(rect.x+x, rect.y+4);
            dc.SetPen(m_gripperPen3);
            dc.DrawPoint(rect.x+x+1, rect.y+5);
            dc.DrawPoint(rect.x+x+2, rect.y+5);
            dc.DrawPoint(rect.x+x+2, rect.y+4);

            x += 4;
            if (x > rect.GetWidth()-5)
                break;
        }
    }
}

void wxAuiDefaultDockArt::DrawPaneButton(wxDC& dc, wxWindow* WXUNUSED(window),
                                      int button,
                                      int buttonState,
                                      const wxRect& _rect,
                                      wxAuiPaneInfo& pane)
{
    wxBitmap bmp;
    if (!(&pane))
        return;

    wxAuiManager* mgr = wxAuiManager::GetManager(pane.GetWindow());
    bool showActive = pane.HasFlag(wxAuiPaneInfo::optionActive) && mgr && mgr->HasFlag(wxAUI_MGR_ALLOW_ACTIVE_PANE);

    switch (button)
    {
        default:
        case wxAUI_BUTTON_CLOSE:
            bmp = showActive ? m_activeCloseBitmap : m_inactiveCloseBitmap;
            break;
        case wxAUI_BUTTON_PIN:
            bmp = showActive ? m_activePinBitmap : m_inactivePinBitmap;
            break;
        case wxAUI_BUTTON_MAXIMIZE_RESTORE:
            if (pane.IsMaximized())
                bmp = showActive ? m_activeRestoreBitmap : m_inactiveRestoreBitmap;
            else
                bmp = showActive ? m_activeMaximizeBitmap : m_inactiveMaximizeBitmap;
            break;
    }


    wxRect rect = _rect;

    int oldY = rect.y;
    rect.y = rect.y + (rect.height/2) - (bmp.GetHeight()/2);
    rect.height = oldY + rect.height - rect.y - 1;


    if (buttonState == wxAUI_BUTTON_STATE_PRESSED)
    {
        rect.x++;
        rect.y++;
    }

    if (buttonState == wxAUI_BUTTON_STATE_HOVER || buttonState == wxAUI_BUTTON_STATE_PRESSED)
    {
        wxColour color = showActive ? m_activeCaptionColour : m_inactiveCaptionColour;

        dc.SetBrush(wxBrush(color.ChangeLightness(120)));
        dc.SetPen(wxPen(color.ChangeLightness(70)));

        // draw the background behind the button
        dc.DrawRectangle(rect.x, rect.y, 15, 15);
    }


    // draw the button itself
    dc.DrawBitmap(bmp, rect.x, rect.y, true);
}




WX_DEFINE_OBJARRAY(wxAuiTabContainerButtonArray)

// -- wxAuiTabContainer class implementation --


// wxAuiTabContainer is a class which contains information about each
// tab.  It also can render an entire tab control to a specified DC.
// It's not a window class itself, because this code will be used by
// the wxFrameMananger, where it is disadvantageous to have separate
// windows for each tab control in the case of "docked tabs"

// A derived class, wxAuiTabCtrl, is an actual wxWindow-derived window
// which can be used as a tab control in the normal sense.
wxAuiTabContainer::wxAuiTabContainer(wxAuiTabArt* artProvider,wxAuiManager* mgr)
: m_focus(false)
, m_mgr(mgr)
, m_tab_art(artProvider)
{
    m_tabOffset = 0;
    m_flags = 0;

    AddButton(wxAUI_BUTTON_LEFT, wxLEFT);
    AddButton(wxAUI_BUTTON_RIGHT, wxRIGHT);
    AddButton(wxAUI_BUTTON_WINDOWLIST, wxRIGHT);
    AddButton(wxAUI_BUTTON_CLOSE, wxRIGHT);
}

wxAuiTabContainer::~wxAuiTabContainer()
{
    unsigned int i;
    for(i=0;i<m_pages.GetCount();i++)
    {
        m_pages[i]->GetWindow()->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler(wxAuiTabContainer::OnChildKeyDown) );
    }
}

void wxAuiTabContainer::SetArtProvider(wxAuiTabArt* art)
{
    m_tab_art = art;

    if (m_tab_art)
    {
        m_tab_art->SetFlags(m_flags);
    }
}

wxAuiTabArt* wxAuiTabContainer::GetArtProvider() const
{
    return m_tab_art;
}

void wxAuiTabContainer::SetFlags(unsigned int flags)
{
    m_flags = flags;

    // check for new close button settings
    RemoveButton(wxAUI_BUTTON_LEFT);
    RemoveButton(wxAUI_BUTTON_RIGHT);
    RemoveButton(wxAUI_BUTTON_UP);
    RemoveButton(wxAUI_BUTTON_DOWN);
    RemoveButton(wxAUI_BUTTON_WINDOWLIST);
    RemoveButton(wxAUI_BUTTON_CLOSE);


    if (IsHorizontal())
    {
        if (flags & wxAUI_MGR_NB_SCROLL_BUTTONS)
        {
            AddButton(wxAUI_BUTTON_LEFT, wxLEFT);
            AddButton(wxAUI_BUTTON_RIGHT, wxRIGHT);
        }

        if (flags & wxAUI_MGR_NB_WINDOWLIST_BUTTON)
        {
            AddButton(wxAUI_BUTTON_WINDOWLIST, wxRIGHT);
        }

        if (flags & wxAUI_MGR_NB_CLOSE_BUTTON)
        {
            AddButton(wxAUI_BUTTON_CLOSE, wxRIGHT);
        }
    }
    else
    {
        if (flags & wxAUI_NB_SCROLL_BUTTONS)
        {
            AddButton(wxAUI_BUTTON_UP, wxUP);
            AddButton(wxAUI_BUTTON_DOWN, wxDOWN);
        }

        if (flags & wxAUI_NB_WINDOWLIST_BUTTON)
        {
            AddButton(wxAUI_BUTTON_WINDOWLIST, wxUP);
        }

        if (flags & wxAUI_NB_CLOSE_BUTTON)
        {
            AddButton(wxAUI_BUTTON_CLOSE, wxUP);
        }
    }
    if (m_tab_art)
    {
        m_tab_art->SetFlags(m_flags);
    }
}

unsigned int wxAuiTabContainer::GetFlags() const
{
    return m_flags;
}

bool wxAuiTabContainer::HasFlag(int flag) const
{
    return (m_flags & flag) != 0;
}


bool wxAuiTabContainer::IsHorizontal() const
{
    return HasFlag(wxAUI_NB_TOP | wxAUI_NB_BOTTOM);
}



void wxAuiTabContainer::SetNormalFont(const wxFont& font)
{
    m_tab_art->SetNormalFont(font);
}

void wxAuiTabContainer::SetSelectedFont(const wxFont& font)
{
    m_tab_art->SetSelectedFont(font);
}

void wxAuiTabContainer::SetMeasuringFont(const wxFont& font)
{
    m_tab_art->SetMeasuringFont(font);
}

void wxAuiTabContainer::SetRect(const wxRect& rect)
{
    m_targetRect = rect;
    m_rect.x = 0;
    m_rect.y = 0;
    m_rect.width = rect.width;
    m_rect.height = rect.height;

    if (m_tab_art)
    {
        m_tab_art->SetSizingInfo(rect.GetSize(), m_pages.GetCount());
    }
}

bool wxAuiTabContainer::AddPage(wxAuiPaneInfo& info)
{
    return InsertPage(info.GetWindow(), info, info.GetPage());
}

bool wxAuiTabContainer::InsertPage(wxWindow* page, wxAuiPaneInfo& info, size_t idx)
{
    info.GetWindow()->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler(wxAuiTabContainer::OnChildKeyDown)  ,NULL,this);

    info.Window(page);

    if (idx >= m_pages.GetCount()) {
        info.Page(m_pages.size());
        m_pages.Add(&info);        
    } else {
        m_pages.Insert(&info, idx);
        for(size_t i=idx; i < m_pages.GetCount(); i++)
            m_pages[i]->Page(i);
    }

    // let the art provider know how many pages we have
    if (m_tab_art)
    {
        m_tab_art->SetSizingInfo(m_rect.GetSize(), m_pages.GetCount());
    }

    return true;
}

bool wxAuiTabContainer::MovePage(wxWindow* page, size_t newIndex)
{
    int idx = GetIdxFromWindow(page);
    if (idx == -1)
        return false;

    // get page entry, make a copy of it
    wxAuiPaneInfo p = GetPage(idx);

    // remove old page entry
    RemovePage(page);

    // insert page where it should be
    InsertPage(page, p, newIndex);

    return true;
}

bool wxAuiTabContainer::RemovePage(wxWindow* wnd)
{
    size_t i, pageCount = m_pages.GetCount();
    for (i = 0; i < pageCount; ++i)
    {
        wxAuiPaneInfo& page = *m_pages.Item(i);
        if (page.GetWindow() == wnd)
        {
            m_pages.RemoveAt(i);

            // let the art provider know how many pages we have
            if (m_tab_art)
            {
                m_tab_art->SetSizingInfo(m_rect.GetSize(), m_pages.GetCount());
            }

            wnd->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler(wxAuiTabContainer::OnChildKeyDown) );
            return true;
        }
    }

    return false;
}

bool wxAuiTabContainer::SetActivePage(wxWindow* wnd)
{
    bool found = false;

    size_t i, pageCount = m_pages.GetCount();
    for (i = 0; i < pageCount; ++i)
    {
        wxAuiPaneInfo& page = *m_pages.Item(i);
        if (page.GetWindow() == wnd)
        {
            if (page.HasFlag(wxAuiPaneInfo::optionActiveNotebook) && page.GetWindow()->IsShown())
            {
                SetFocus(true);
            }
            else
            {
                page.GetWindow()->SetFocus();
                page.GetWindow()->Show(true);
                page.SetFlag(wxAuiPaneInfo::optionActive,true);
                page.SetFlag(wxAuiPaneInfo::optionActiveNotebook,true);
                found = true;
                MakeTabVisible(i);
            }
        }
        else
        {
            page.GetWindow()->Show(false);
            page.SetFlag(wxAuiPaneInfo::optionActive,false);
            page.SetFlag(wxAuiPaneInfo::optionActiveNotebook,false);
        }
    }

    return found;
}

void wxAuiTabContainer::SetNoneActive()
{
    size_t i, pageCount = m_pages.GetCount();
    for (i = 0; i < pageCount; ++i)
    {
        wxAuiPaneInfo& page = *m_pages.Item(i);
        page.SetFlag(wxAuiPaneInfo::optionActiveNotebook,false);
    }
}

bool wxAuiTabContainer::SetActivePage(size_t page)
{
    if (page >= m_pages.GetCount())
        return false;

    return SetActivePage(m_pages.Item(page)->GetWindow());
}

int wxAuiTabContainer::GetActivePage() const
{
    size_t i, pageCount = m_pages.GetCount();
    for (i = 0; i < pageCount; ++i)
    {
        wxAuiPaneInfo& page = *m_pages.Item(i);
        if (page.HasFlag(wxAuiPaneInfo::optionActiveNotebook))
            return i;
    }

    return -1;
}

wxWindow* wxAuiTabContainer::GetWindowFromIdx(size_t idx) const
{
    if (idx >= m_pages.GetCount())
        return NULL;

    return m_pages[idx]->GetWindow();
}

int wxAuiTabContainer::GetIdxFromWindow(wxWindow* wnd) const
{
    const size_t pageCount = m_pages.GetCount();
    for ( size_t i = 0; i < pageCount; ++i )
    {
        wxAuiPaneInfo& page = *m_pages.Item(i);
        if (page.GetWindow() == wnd)
            return i;
    }
    return wxNOT_FOUND;
}

wxAuiPaneInfo& wxAuiTabContainer::GetPage(size_t idx)
{
    wxASSERT_MSG(idx < m_pages.GetCount(), wxT("Invalid Page index"));

    return *m_pages[idx];
}

const wxAuiPaneInfo& wxAuiTabContainer::GetPage(size_t idx) const
{
    wxASSERT_MSG(idx < m_pages.GetCount(), wxT("Invalid Page index"));

    return *m_pages[idx];
}

wxAuiPaneInfoPtrArray& wxAuiTabContainer::GetPages()
{
    return m_pages;
}

size_t wxAuiTabContainer::GetPageCount() const
{
    return m_pages.GetCount();
}

void wxAuiTabContainer::AddButton(int id,
                                  int location,
                                  const wxBitmap& normalBitmap,
                                  const wxBitmap& disabledBitmap)
{
    wxAuiTabContainerButton button;
    button.id = id;
    button.bitmap = normalBitmap;
    button.disBitmap = disabledBitmap;
    button.location = location;
    button.curState = wxAUI_BUTTON_STATE_NORMAL;

    m_buttons.Add(button);
}

void wxAuiTabContainer::RemoveButton(int id)
{
    size_t i, buttonCount = m_buttons.GetCount();

    for (i = 0; i < buttonCount; ++i)
    {
        if (m_buttons.Item(i).id == id)
        {
            m_buttons.RemoveAt(i);
            return;
        }
    }
}


size_t wxAuiTabContainer::GetTabOffset() const
{
    return m_tabOffset;
}

void wxAuiTabContainer::SetTabOffset(size_t offset)
{
    m_tabOffset = offset;
}



void wxAuiTabContainer::DrawTabs(wxDC* dc, wxWindow* wnd,const wxRect& rect)
{
    SetRect(rect);
    Render(dc,wnd);
}

void wxAuiTabContainer::CalculateRequiredHeight(wxDC& dc,wxWindow* wnd,int& totalSize,int& visibleSize) const
{
    totalSize=0;
    visibleSize=0;

    size_t i;
    size_t pageCount = m_pages.GetCount();
    for (i = 0; i < pageCount; ++i)
    {
        wxAuiPaneInfo& page = *m_pages.Item(i);

        // determine if a close button is on this tab
        bool closeButton = TabHasCloseButton(m_flags,page);
		bool isCompact = page.IsCompactTab() && !page.HasFlag(wxAuiPaneInfo::optionActiveNotebook) && page.GetIcon().IsOk();

        int extent = 0;
        wxSize size = m_tab_art->GetTabSize(dc,
                            wnd,
                            isCompact ? wxString() : page.GetCaption(),
                            page.GetIcon(),
                            page.HasFlag(wxAuiPaneInfo::optionActiveNotebook),
                            closeButton ?
                              wxAUI_BUTTON_STATE_NORMAL :
                              wxAUI_BUTTON_STATE_HIDDEN,
                            &extent);

        if (IsHorizontal())
        {
            totalSize = std::max(totalSize,size.y);
        }
        else
        {
            if (i+1 < pageCount)
                totalSize += extent;
            else
                totalSize += size.y;
        }
        if (i >= m_tabOffset)
        {
            if (IsHorizontal())
            {
                visibleSize = std::max(visibleSize,size.y);
            }
            else
            {
                if (i+1 < pageCount)
                    visibleSize += extent;
                else
                    visibleSize += size.y;
            }
        }
    }
}


void wxAuiTabContainer::CalculateRequiredWidth(wxDC& dc,wxWindow* wnd,int& totalSize,int& visibleSize) const
{
    totalSize=0;
    visibleSize=0;

    size_t i;
    size_t pageCount = m_pages.GetCount();
    for (i = 0; i < pageCount; ++i)
    {
        wxAuiPaneInfo& page = *m_pages.Item(i);

        // determine if a close button is on this tab
        bool closeButton = TabHasCloseButton(m_flags,page);
		bool isCompact = page.IsCompactTab() && !page.HasFlag(wxAuiPaneInfo::optionActiveNotebook) && page.GetIcon().IsOk();

        int extent = 0;
        wxSize size = m_tab_art->GetTabSize(dc,
                            wnd,
                            isCompact ? wxString() : page.GetCaption(),
                            page.GetIcon(),
                            page.HasFlag(wxAuiPaneInfo::optionActiveNotebook),
                            closeButton ?
                              wxAUI_BUTTON_STATE_NORMAL :
                              wxAUI_BUTTON_STATE_HIDDEN,
                            &extent);

        if (IsHorizontal())
        {
            if (i+1 < pageCount)
                totalSize += extent;
            else
                totalSize += size.x;
        }
        else
        {
             totalSize = std::max(totalSize,size.x);
        }

        if (i >= m_tabOffset)
        {
            if (IsHorizontal())
            {
                if (i+1 < pageCount)
                    visibleSize += extent;
                else
                    visibleSize += size.x;
            }
            else
            {
                visibleSize = std::max(visibleSize,size.x);
            }
        }
    }
}

// Render() renders the tab catalog to the specified DC
// It is a virtual function and can be overridden to
// provide custom drawing capabilities
void wxAuiTabContainer::Render(wxDC* rawDC, wxWindow* wnd)
{
    if (!rawDC || !rawDC->IsOk() || m_rect.IsEmpty())
        return;

    wxMemoryDC dc;

    // use the same layout direction as the window DC uses to ensure that the
    // text is rendered correctly
    dc.SetLayoutDirection(rawDC->GetLayoutDirection());

    wxBitmap bmp;
    size_t i;
    size_t pageCount = m_pages.GetCount();
    size_t buttonCount = m_buttons.GetCount();

    // create off-screen bitmap
    bmp.Create(m_rect.GetWidth(), m_rect.GetHeight());
    dc.SelectObject(bmp);

    if (!dc.IsOk())
        return;

    // draw background
    m_tab_art->DrawBackground(dc, wnd, m_rect);

    // first we create all buttons on the right/bottom, except the forward arrows
    // so we can determine, whether we need the backward- and forward-arrows

    // draw buttons
    int backwButtonsSize = 0;
    int forwButtonsSize = 0;


    int locationBtnRightTop;
    int locationBtnLeftBottom;
    int idBtnLeftBottom;
    int idBtnArrowRightUp;
    int testSize;

    if (IsHorizontal())
    {
        locationBtnRightTop = wxRIGHT;
        locationBtnLeftBottom = wxLEFT;
        idBtnLeftBottom = wxAUI_BUTTON_LEFT;
        idBtnArrowRightUp = wxAUI_BUTTON_RIGHT;
        testSize = m_rect.GetWidth();
    }
    else
    {
        locationBtnRightTop = wxUP;
        locationBtnLeftBottom = wxDOWN;
        idBtnLeftBottom = wxAUI_BUTTON_DOWN;
        idBtnArrowRightUp = wxAUI_BUTTON_UP;
        testSize = m_rect.GetHeight();
    }


    // draw the forward buttons on the top
    for (i = buttonCount; i > 0 ; --i)
    {
        wxAuiTabContainerButton& button = m_buttons.Item(i - 1);
        if (button.location != locationBtnRightTop)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;
        if (button.id == idBtnArrowRightUp)
            continue;

        wxRect button_rect = m_rect;
        button_rect.SetX(1);
        button_rect.SetY(1);

        m_tab_art->DrawButton(dc,
                          wnd,
                          button_rect,
                          button.id,
                          button.curState,
                          locationBtnRightTop,
                          &button.rect);

        if (IsHorizontal())
        {
            forwButtonsSize += button.rect.GetWidth();
        }
        else
        {
            backwButtonsSize = wxMax(backwButtonsSize, button.rect.GetHeight());
        }

    }

    // find out if size of tabs is larger than can be
    // afforded on screen
    int totalSize = 0;
    int visibleSize = 0;
    int btn_size;
    if(IsHorizontal())
    {
        CalculateRequiredWidth(dc,wnd,totalSize,visibleSize);
        btn_size = forwButtonsSize;
    }
    else
    {
        CalculateRequiredHeight(dc,wnd,totalSize,visibleSize);
        btn_size = backwButtonsSize;
    }
    // ensure, we show as max tabs as possible
    while (m_tabOffset > 0 && IsTabVisible(pageCount-1, m_tabOffset-1, &dc, wnd) )
            --m_tabOffset;

    // show up/down buttons
    for (i = 0; i < buttonCount; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.Item(i);
        if (button.id == idBtnArrowRightUp || button.id == idBtnLeftBottom)
        {
            if (totalSize > (testSize - btn_size - m_tab_art->GetIndentSize()) || m_tabOffset != 0)
                button.curState &= ~wxAUI_BUTTON_STATE_HIDDEN;
            else
                button.curState |= wxAUI_BUTTON_STATE_HIDDEN;
        }
    }

    // determine whether left/up button should be enabled
    for (i = 0; i < buttonCount; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.Item(i);
        if (button.id == wxAUI_BUTTON_LEFT || button.id == wxAUI_BUTTON_UP)
        {
            if (m_tabOffset == 0)
                button.curState |= wxAUI_BUTTON_STATE_DISABLED;
            else
                button.curState &= ~wxAUI_BUTTON_STATE_DISABLED;
        }
    }

    // now draw forward arrows, keep the rect where it should be drawn, so we are
    // able to redraw it, if it should be enabled. We can only determine this, if it is actually drawn
    // because we do not know it's exact size
    wxRect forw_btn_rect;

    for (i = buttonCount; i > 0; --i)
    {
        wxAuiTabContainerButton& button = m_buttons.Item(i - 1);

        if (button.location != locationBtnRightTop)
            continue;
        if (button.id != idBtnArrowRightUp)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;

        wxRect button_rect = m_rect;
        button_rect.SetY(1);

        if (IsHorizontal())
        {
            forw_btn_rect = button_rect;
        }

        m_tab_art->DrawButton(dc,
                          wnd,
                          button_rect,
                          button.id,
                          button.curState,
                          locationBtnRightTop,
                          &button.rect);

        if (IsHorizontal())
        {
            forwButtonsSize += button.rect.GetWidth();
        }
        else
        {
            backwButtonsSize = wxMax(backwButtonsSize, button.rect.GetHeight());
        }
    }



    // draw the buttons on the bottom side

    for (i = buttonCount; i > 0 ; --i)
    {
        wxAuiTabContainerButton& button = m_buttons.Item(i - 1);

        if (button.location != locationBtnLeftBottom)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;

        wxRect button_rect = m_rect;
        button_rect.SetX(1);
        button_rect.SetY(1);

        if (!IsHorizontal() && button.id == wxAUI_BUTTON_DOWN)
        {
            forw_btn_rect = button_rect;
        }

        m_tab_art->DrawButton(dc,
                          wnd,
                          button_rect,
                          button.id,
                          button.curState,
                          locationBtnLeftBottom,
                          &button.rect);

        if (IsHorizontal())
        {
            backwButtonsSize += button.rect.GetWidth();
        }
        else
        {
            forwButtonsSize = wxMax(forwButtonsSize, button.rect.GetHeight());
        }
    }

    // this is not a typo, we use the size we determined when drawing the top (aka backw) buttons
    int offset = backwButtonsSize;

    if (offset == 0)
        offset += m_tab_art->GetIndentSize();


    // prepare the tab-close-button array
    // make sure tab button entries which aren't used are marked as hidden
    for (i = pageCount; i < m_tabCloseButtons.GetCount(); ++i)
        m_tabCloseButtons.Item(i).curState = wxAUI_BUTTON_STATE_HIDDEN;

    // make sure there are enough tab button entries to accommodate all tabs
    while (m_tabCloseButtons.GetCount() < pageCount)
    {
        wxAuiTabContainerButton tempbtn;
        tempbtn.id = wxAUI_BUTTON_CLOSE;
        tempbtn.location = wxCENTER;
        tempbtn.curState = wxAUI_BUTTON_STATE_HIDDEN;
        m_tabCloseButtons.Add(tempbtn);
    }


    // buttons before the tab offset must be set to hidden
    for (i = 0; i < m_tabOffset; ++i)
    {
        m_tabCloseButtons.Item(i).curState = wxAUI_BUTTON_STATE_HIDDEN;
    }

    // determine whether right/down button should be enabled
    for (i = 0; i < buttonCount; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.Item(i);
        if (button.id == wxAUI_BUTTON_RIGHT || button.id == wxAUI_BUTTON_DOWN)
        {
            if (visibleSize < testSize - (backwButtonsSize + forwButtonsSize))
                button.curState |= wxAUI_BUTTON_STATE_DISABLED;
            else
                button.curState &= ~wxAUI_BUTTON_STATE_DISABLED;

            m_tab_art->DrawButton(dc,
                              wnd,
                              forw_btn_rect,
                              button.id,
                              button.curState,
                              button.location,
                              &button.rect);
        }
    }

    // draw the tabs

    size_t active = 999;
    int active_offset = 0;
    wxRect active_rect;

    int extent = 0;
    wxRect rect = m_rect;
    if (IsHorizontal())
    {
        rect.y = 0;
        rect.height = m_rect.height;
    }
    else
    {
        rect.x = 0;
        rect.width = m_rect.width;
    }

    for (i = m_tabOffset; i < pageCount; ++i)
    {
        wxAuiPaneInfo& page = *m_pages.Item(i);
        wxAuiTabContainerButton& tab_button = m_tabCloseButtons.Item(i);


        if (TabHasCloseButton(m_flags,page))
        {
            if (tab_button.curState == wxAUI_BUTTON_STATE_HIDDEN)
            {
                tab_button.id = wxAUI_BUTTON_CLOSE;
                tab_button.curState = wxAUI_BUTTON_STATE_NORMAL;
                tab_button.location = wxCENTER;
            }
        }
        else
        {
            tab_button.curState = wxAUI_BUTTON_STATE_HIDDEN;
        }

        if (IsHorizontal())
        {
            rect.x = offset;
            rect.width = m_rect.width - forwButtonsSize - offset - 2;
            if (rect.width <= 0)
                break;
        }
        else
        {
             rect.y = offset;
             rect.height = m_rect.height - forwButtonsSize - offset - 2;
             if (rect.height <= 0)
                 break;
        }

        wxRect page_rect = page.GetRect();
        m_tab_art->DrawTab(dc,
                       wnd,
                       page,
                       rect,
                       tab_button.curState,
                       HasFocus(),
                       &page_rect,
                       &tab_button.rect,
                       &extent);
        page.Rect(page_rect);

        if (page.HasFlag(wxAuiPaneInfo::optionActiveNotebook))
        {
            active = i;
            active_offset = offset;
            active_rect = rect;
        }

        offset += extent;
    }


    // make sure to deactivate buttons which are off the screen to the right
    for (++i; i < m_tabCloseButtons.GetCount(); ++i)
    {
        m_tabCloseButtons.Item(i).curState = wxAUI_BUTTON_STATE_HIDDEN;
    }


    // draw the active tab again so it stands in the foreground
    if (active >= m_tabOffset && active < m_pages.GetCount())
    {
        wxAuiPaneInfo& page = *m_pages.Item(active);

        wxAuiTabContainerButton& tab_button = m_tabCloseButtons.Item(active);

        rect.x = active_offset;
        
        wxRect page_rect = page.GetRect();
        m_tab_art->DrawTab(dc,
                       wnd,
                       page,
                       active_rect,
                       tab_button.curState,
                       HasFocus(),
                       &page_rect,
                       &tab_button.rect,
                       &extent);
        page.Rect(page_rect);
    }


    rawDC->Blit(m_targetRect.x, m_targetRect.y,
                 m_targetRect.GetWidth(), m_targetRect.GetHeight(),
                 &dc, 0, 0);
}

// Is the tab visible?
bool wxAuiTabContainer::IsTabVisible(int tabPage, int tabOffset, wxDC* dc, wxWindow* wnd)
{
    if (!dc || !dc->IsOk())
        return false;

    size_t i;
    size_t pageCount = m_pages.GetCount();
    size_t buttonCount = m_buttons.GetCount();

    // Hasn't been rendered yet; assume it's visible
    if (m_tabCloseButtons.GetCount() < pageCount)
        return true;

    // First check if both buttons are disabled - if so, there's no need to
    // check further for visibility.
    int arrowButtonVisibleCount = 0;
    for (i = 0; i < buttonCount; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.Item(i);
        if (button.id == wxAUI_BUTTON_LEFT ||
            button.id == wxAUI_BUTTON_RIGHT ||
            button.id == wxAUI_BUTTON_UP ||
            button.id == wxAUI_BUTTON_DOWN)
        {
            if ((button.curState & wxAUI_BUTTON_STATE_HIDDEN) == 0)
                arrowButtonVisibleCount ++;
        }
    }

    // Tab must be visible
    if (arrowButtonVisibleCount == 0)
        return true;

    // If tab is less than the given offset, it must be invisible by definition
    if (tabPage < tabOffset)
        return false;

    // draw buttons
    int backwButtonsSize = 0;
    int forwButtonsSize = 0;

    int offset = 0;

    // calculate size of the buttons on the right/bottom side
    if (IsHorizontal())
    {
        offset = m_rect.x + m_rect.width;
        for (i = 0; i < buttonCount; ++i)
        {
            wxAuiTabContainerButton& button = m_buttons.Item(buttonCount - i - 1);
            if (button.location != wxRIGHT)
                continue;
            if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
                continue;

            offset -= button.rect.GetWidth();
            forwButtonsSize += button.rect.GetWidth();
        }
    }
    else // must be wxAUI_NB_TOP or wxAUI_NB_BOTTOM
    {
        for (i = 0; i < buttonCount; ++i)
        {
            wxAuiTabContainerButton& button = m_buttons.Item(buttonCount - i - 1);

            if (button.location != wxDOWN)
                continue;
            if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
                continue;

            offset = wxMax(offset, button.rect.GetHeight());
            forwButtonsSize = offset;
        }
    }

    offset = 0;

    int location_btn_left_top;
    int testSize;

    if (IsHorizontal())
    {
        location_btn_left_top = wxLEFT;
        testSize = m_rect.GetWidth();
    }
    else
    {
        location_btn_left_top = wxUP;
        testSize = m_rect.GetHeight();
    }


    for (i = 0; i < buttonCount; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.Item(buttonCount - i - 1);

        if (button.location != location_btn_left_top)
            continue;
        if (button.curState & wxAUI_BUTTON_STATE_HIDDEN)
            continue;

        if (IsHorizontal())
        {
            offset += button.rect.GetWidth();
            backwButtonsSize += button.rect.GetWidth();
        }
        else
        {
            offset = wxMax(offset, button.rect.GetHeight());
            backwButtonsSize = offset;
        }
    }

    offset = backwButtonsSize;

    // calculate size of the buttons on the left/top side
    if (offset == 0)
        offset += m_tab_art->GetIndentSize();

    // See if the given page is visible at the given tab offset (effectively scroll position)
    for (i = tabOffset; i < pageCount; ++i)
    {
        wxAuiPaneInfo& page = *m_pages.Item(i);
        wxAuiTabContainerButton& tab_button = m_tabCloseButtons.Item(i);
		bool isCompact = page.IsCompactTab() && !page.HasFlag(wxAuiPaneInfo::optionActiveNotebook) && page.GetIcon().IsOk();

        if ((testSize - forwButtonsSize - offset - 2) <= 0)
            return false; // haven't found the tab, and we've run out of space, so return false

        int extent = 0;
        wxSize size = m_tab_art->GetTabSize(*dc,
                            wnd,
                            isCompact ? wxString() : page.GetCaption(),
                            page.GetIcon(),
                            page.HasFlag(wxAuiPaneInfo::optionActiveNotebook),
                            tab_button.curState,
                            &extent);

        offset += extent;

        if (i == (size_t) tabPage)
        {
            // If not all of the tab is visible, and supposing there's space to display it all,
            // we could do better so we return false.
            if (((testSize - forwButtonsSize - offset - 2) <= 0) && ((testSize - forwButtonsSize - backwButtonsSize) > extent))
                return false;
            else
                return true;
        }
    }

    // Shouldn't really get here, but if it does, assume the tab is visible to prevent
    // further looping in calling code.
    return true;
}

// Make the tab visible if it wasn't already
void wxAuiTabContainer::MakeTabVisible(int tabPage)
{
    wxWindow* win = m_pages.Item(tabPage)->GetWindow();
    wxClientDC dc(win);
    if (!IsTabVisible(tabPage, GetTabOffset(), & dc, win))
    {
        int i;
        for (i = 0; i < (int) m_pages.GetCount(); i++)
        {
            if (IsTabVisible(tabPage, i, & dc, win))
            {
                SetTabOffset(i);
                win->Refresh();
                return;
            }
        }
    }
}

// TabHitTest() tests if a tab was hit, passing the window pointer
// back if that condition was fulfilled.  The function returns
// true if a tab was hit, otherwise false
bool wxAuiTabContainer::TabHitTest(int x, int y, wxAuiPaneInfo** hit) const
{
    //Remap the x and y to our internal rect
    x += m_rect.x-m_targetRect.x;
    y += m_rect.y-m_targetRect.y;

    wxRect rect = m_rect;
    if (m_flags & (wxAUI_NB_LEFT | wxAUI_NB_RIGHT))
    {
        int bottomSize = 0;
        for (size_t i = m_buttons.GetCount(); i > 0 ; --i)
        {
            wxAuiTabContainerButton& button = m_buttons.Item(i - 1);
            if (button.location == wxDOWN)
            {
                // if we have hit one of the bottom buttons step out, or we can not click it
                if (button.rect.Contains(x,y))
                {
                    bottomSize = 0;
                    break;
                }
                bottomSize = wxMax(bottomSize, button.rect.GetHeight());
            }
        }
        if (bottomSize)
           rect.height -= (bottomSize + 2);
    }

    if (!rect.Contains(x,y))
        return false;

    wxAuiTabContainerButton* btn = NULL;
    if (ButtonHitTest(x, y, &btn) && !(btn->curState & wxAUI_BUTTON_STATE_DISABLED))
    {
        if (m_buttons.Index(*btn) != wxNOT_FOUND)
            return false;
    }

    size_t i, pageCount = m_pages.GetCount();
    for (i = m_tabOffset; i < pageCount; ++i)
    {
        wxAuiPaneInfo& page = *m_pages.Item(i);
        if (page.GetRect().Contains(x,y))
        {
            if (hit)
                *hit = &page;
            return true;
        }
    }

    return false;
}

// ButtonHitTest() tests if a button was hit. The function returns
// true if a button was hit, otherwise false
bool wxAuiTabContainer::ButtonHitTest(int x, int y,
                                      wxAuiTabContainerButton** hit) const
{
    //Remap the x and y to our internal rect
    x += m_rect.x-m_targetRect.x;
    y += m_rect.y-m_targetRect.y;

    wxRect rect = m_rect;
    if (m_flags & (wxAUI_NB_LEFT | wxAUI_NB_RIGHT))
    {
        int bottomSize = 0;
        for (size_t i = m_buttons.GetCount(); i > 0 ; --i)
        {
            wxAuiTabContainerButton& button = m_buttons.Item(i - 1);
            if (button.location == wxDOWN)
            {
                // if we have hit one of the bottom buttons step out, or we can not click it
                if (button.rect.Contains(x,y))
                {
                    bottomSize = 0;
                    break;
                }
                bottomSize = wxMax(bottomSize, button.rect.GetHeight());
            }
        }
        if (bottomSize)
            rect.height -= (bottomSize + 2);
    }

    if (!rect.Contains(x,y))
        return false;

    size_t i, buttonCount;


    buttonCount = m_buttons.GetCount();
    for (i = 0; i < buttonCount; ++i)
    {
        wxAuiTabContainerButton& button = m_buttons.Item(i);
        if (button.rect.Contains(x,y) &&
            !(button.curState & (wxAUI_BUTTON_STATE_HIDDEN |
                                   wxAUI_BUTTON_STATE_DISABLED)))
        {
            if (hit)
                *hit = &button;
            return true;
        }
    }

    buttonCount = m_tabCloseButtons.GetCount();
    for (i = 0; i < buttonCount; ++i)
    {
        wxAuiTabContainerButton& button = m_tabCloseButtons.Item(i);
        if (button.rect.Contains(x,y) &&
            !(button.curState & (wxAUI_BUTTON_STATE_HIDDEN |
                                   wxAUI_BUTTON_STATE_DISABLED)))
        {
            if (hit)
                *hit = &button;
            return true;
        }
    }

    return false;
}

void wxAuiTabContainer::OnChildKeyDown(wxKeyEvent& evt)
{
    evt.Skip();
    if (HasFocus())
    {
        if (evt.GetKeyCode() == WXK_LEFT)
        {
            SetActivePage(GetActivePage()-1);
            SetFocus(true);
            evt.Skip(false);

            m_mgr->Update();
        }
        else if (evt.GetKeyCode() == WXK_RIGHT)
        {
            SetActivePage(GetActivePage()+1);
            SetFocus(true);
            evt.Skip(false);

            m_mgr->Update();
        }
    }
    return;
}
#endif // wxUSE_AUI
