/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:     wxBitmap
// Author:      Julian Smart, originally in bitmap.cpp
// Modified by:
// Created:     25/03/2003
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bmpmotif.h"
#endif

#ifdef __VMS
#define XtParent XTPARENT
#endif

#include "wx/defs.h"
#include "wx/motif/bmpmotif.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

#if wxHAVE_LIB_XPM
    #include <X11/xpm.h>
#endif
#include <math.h>

Pixmap XCreateInsensitivePixmap( Display *display, Pixmap pixmap );

wxBitmapCache::~wxBitmapCache()
{
    if( m_display )
    {
        Screen* screen = DefaultScreenOfDisplay( (Display*)m_display );

        if( m_labelPixmap )
            XmDestroyPixmap( screen, (Pixmap)m_labelPixmap );

        if( m_armPixmap )
            XmDestroyPixmap( screen, (Pixmap)m_armPixmap );

        if( m_insensPixmap )
            XmDestroyPixmap( screen, (Pixmap)m_insensPixmap );

    }

    if( m_image )
    {
        XmUninstallImage( (XImage*)m_image );
        XtFree( (char*)(XImage*)m_image );
    }
}

void wxBitmapCache::SetBitmap( const wxBitmap& bitmap )
{
    if( m_bitmap != bitmap )
    {
        InvalidateCache();
        m_bitmap = bitmap;

        if( m_image )
        {
            XmUninstallImage( (XImage*)m_image );
            XtFree( (char*)(XImage*)m_image );
            m_image = (WXImage*)NULL;
        }
    }
}

void wxBitmapCache::InvalidateCache()
{
    m_recalcPixmaps.label  = true;
    m_recalcPixmaps.arm    = true;
    m_recalcPixmaps.insens = true;
}

void wxBitmapCache::SetColoursChanged()
{
    InvalidateCache();
}

void wxBitmapCache::CreateImageIfNeeded( WXWidget w )
{
    if( m_image )
        return;

    m_display = w ?
                (WXDisplay*)XtDisplay( (Widget)w ) :
                (WXDisplay*)wxGetDisplay();

    XImage *ximage = XGetImage( (Display*)m_display,
                                (Drawable)m_bitmap.GetPixmap(),
                                0, 0,
                                m_bitmap.GetWidth(), m_bitmap.GetHeight(),
                                AllPlanes, ZPixmap );

    m_image = (WXImage*)ximage;

    if( m_image )
    {
        char tmp[128];
        sprintf( tmp, "Im%x", (unsigned int)ximage );
        XmInstallImage( ximage, tmp );
    }
}

WXPixmap wxBitmapCache::GetLabelPixmap( WXWidget w )
{
    if( m_labelPixmap && !m_recalcPixmaps.label )
        return m_labelPixmap;

    CreateImageIfNeeded( w );

    Screen* screen = DefaultScreenOfDisplay( (Display*)m_display );

    if( m_labelPixmap )
        XmDestroyPixmap( screen, (Pixmap)m_labelPixmap );

    if( !m_image )
        return (WXPixmap)NULL;

    char tmp[128];
    sprintf( tmp, "Im%x", (unsigned int)m_image );

    Pixel fg, bg;
    Widget widget = (Widget)w;

    while( XmIsGadget( widget ) )
        widget = XtParent( widget );
    XtVaGetValues( widget,
                   XmNbackground, &bg,
                   XmNforeground, &fg,
                   NULL );

    m_labelPixmap = (WXPixmap)XmGetPixmap( screen, tmp, fg, bg );

    m_recalcPixmaps.label = !m_labelPixmap;
    return m_labelPixmap;
}

WXPixmap wxBitmapCache::GetArmPixmap( WXWidget w )
{
    if( m_armPixmap && !m_recalcPixmaps.arm )
        return m_armPixmap;

    CreateImageIfNeeded( w );

    Screen* screen = DefaultScreenOfDisplay( (Display*)m_display );

    if( m_armPixmap )
        XmDestroyPixmap( screen, (Pixmap)m_armPixmap );

    if( !m_image )
        return (WXPixmap)NULL;

    char tmp[128];
    sprintf( tmp, "Im%x", (unsigned int)m_image );

    Pixel fg, bg;
    Widget widget = (Widget) w;

    XtVaGetValues( widget, XmNarmColor, &bg, NULL );
    while( XmIsGadget( widget ) )
        widget = XtParent( widget );
    XtVaGetValues( widget, XmNforeground, &fg, NULL );

    m_armPixmap = (WXPixmap)XmGetPixmap( screen, tmp, fg, bg );

    m_recalcPixmaps.arm = !m_armPixmap;
    return m_armPixmap;
}

WXPixmap wxBitmapCache::GetInsensPixmap( WXWidget w )
{
    if( m_insensPixmap && !m_recalcPixmaps.insens )
        return m_insensPixmap;

    CreateImageIfNeeded( w );

    Screen* screen = DefaultScreenOfDisplay( (Display*)m_display );

    if( m_insensPixmap )
        XmDestroyPixmap( screen, (Pixmap)m_insensPixmap );

    if( !m_image )
        return (WXPixmap)NULL;

    m_insensPixmap =
        (WXPixmap)XCreateInsensitivePixmap( (Display*)m_display,
                                            (Pixmap)m_bitmap.GetPixmap() );

    m_recalcPixmaps.insens = !m_insensPixmap;
    return m_insensPixmap;
}
