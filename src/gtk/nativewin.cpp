///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/nativewin.cpp
// Purpose:     wxNativeWindow implementation
// Author:      Vadim Zeitlin
// Created:     2008-03-05
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/nativewin.h"

#include <gtk/gtk.h>

// ============================================================================
// implementation
// ============================================================================

bool wxNativeContainerWindow::Create(wxNativeContainerWindowHandle *win)
{
    if ( !wxTopLevelWindow::Create(NULL, wxID_ANY, "") )
        return false;

    // we need to realize the window first before reparenting it
    gtk_widget_realize(m_widget);
    gdk_window_reparent(m_widget->window, win, 0, 0);

    // we should be initially visible as we suppose that the native window we
    // wrap is (we could use gdk_window_is_visible() to test for this but this
    // doesn't make much sense unless we also react to visibility changes, so
    // just suppose it's always shown for now)
    Show();
}

bool wxNativeContainerWindow::Create(wxNativeContainerWindowId anid)
{
    bool rc;
    GdkWindow * const win = gdk_window_foreign_new(anid);
    if ( win )
    {
        rc = Create(win);
        g_object_unref(win);
    }
    else // invalid native window id
    {
        rc = false;
    }

    return rc;
}

wxNativeContainerWindow::~wxNativeContainerWindow()
{
    // there doesn't seem to be anything to do here, GTK+ seems to handle
    // everything correctly due to its use of reference counting
}
