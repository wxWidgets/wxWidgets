/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcmemory.h"
#endif

#include "wx/dcmemory.h"
#include "wx/settings.h"
#include "wx/utils.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC, wxWindowDC)

wxMemoryDC::wxMemoryDC(void)
{
    m_ok = TRUE;
    m_display = wxGetDisplay();
    
    Display* display = (Display*) m_display;
    
    XGCValues gcvalues;
    gcvalues.foreground = BlackPixel (display, DefaultScreen (display));
    gcvalues.background = WhitePixel (display, DefaultScreen (display));
    gcvalues.graphics_exposures = False;
    gcvalues.subwindow_mode = IncludeInferiors;
    gcvalues.line_width = 1;
    m_gc = (WXGC) XCreateGC (display, RootWindow (display, DefaultScreen (display)),
        GCForeground | GCBackground | GCGraphicsExposures | GCLineWidth | GCSubwindowMode,
        &gcvalues);
    
    m_backgroundPixel = (int) gcvalues.background;
    
    // Get the current Font so we can set it back later
    XGCValues valReturn;
    XGetGCValues((Display*) m_display, (GC) m_gc, GCFont, &valReturn);
    m_oldFont = (WXFont) valReturn.font;
    SetBrush (* wxWHITE_BRUSH);
    SetPen (* wxBLACK_PEN);
    SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));    
};

wxMemoryDC::wxMemoryDC( wxDC* dc )
{
    m_ok = TRUE;
    if (dc && dc->IsKindOf(CLASSINFO(wxWindowDC)))
        m_display = ((wxWindowDC*)dc)->GetDisplay();
    else
        m_display = wxGetDisplay();
    
    Display* display = (Display*) m_display;
    
    XGCValues gcvalues;
    gcvalues.foreground = BlackPixel (display, DefaultScreen (display));
    gcvalues.background = WhitePixel (display, DefaultScreen (display));
    gcvalues.graphics_exposures = False;
    gcvalues.subwindow_mode = IncludeInferiors;
    gcvalues.line_width = 1;
    m_gc = (WXGC) XCreateGC (display, RootWindow (display, DefaultScreen (display)),
        GCForeground | GCBackground | GCGraphicsExposures | GCLineWidth | GCSubwindowMode,
        &gcvalues);
    
    m_backgroundPixel = (int) gcvalues.background;
    
    // Get the current Font so we can set it back later
    XGCValues valReturn;
    XGetGCValues((Display*) m_display, (GC) m_gc, GCFont, &valReturn);
    m_oldFont = (WXFont) valReturn.font;
    SetBrush (* wxWHITE_BRUSH);
    SetPen (* wxBLACK_PEN);
};

wxMemoryDC::~wxMemoryDC(void)
{
};

void wxMemoryDC::SelectObject( const wxBitmap& bitmap )
{
    m_bitmap = bitmap;
    
    if (m_gc)
        XFreeGC((Display*) m_display, (GC) m_gc);
    m_gc = (WXGC) NULL;
    
    if (m_bitmap.Ok() && (bitmap.GetDisplay() == m_display))
    {
        m_pixmap = m_bitmap.GetPixmap();
        Display* display = (Display*) m_display;
        
        XGCValues gcvalues;
        gcvalues.foreground = BlackPixel (display, DefaultScreen (display));
        gcvalues.background = WhitePixel (display, DefaultScreen (display));
        gcvalues.graphics_exposures = False;
        gcvalues.subwindow_mode = IncludeInferiors;
        gcvalues.line_width = 1;
        m_gc = (WXGC) XCreateGC (display, RootWindow (display, DefaultScreen (display)),
            GCForeground | GCBackground | GCGraphicsExposures | GCLineWidth | GCSubwindowMode,
            &gcvalues);
        
        m_backgroundPixel = (int) gcvalues.background;
        
        // Get the current Font so we can set it back later
        XGCValues valReturn;
        XGetGCValues((Display*) m_display, (GC) m_gc, GCFont, &valReturn);
        m_oldFont = (WXFont) valReturn.font;
        
        bool oldOpt = GetOptimization();
        SetOptimization(FALSE);
        
        SetBrush (* wxWHITE_BRUSH);
        SetPen (* wxBLACK_PEN);
        
        SetOptimization(oldOpt);
        
        m_ok = TRUE;
    }
    else
    {
        m_ok = FALSE;
        m_pixmap = (WXPixmap) 0;
    };
};

void wxMemoryDC::DoGetSize( int *width, int *height ) const
{
    if (m_bitmap.Ok())
    {
        if (width) (*width) = m_bitmap.GetWidth();
        if (height) (*height) = m_bitmap.GetHeight();
    }
    else
    {
        if (width) (*width) = 0;
        if (height) (*height) = 0;
    };
};


