///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/nonownedwnd.cpp
// Purpose:     wxGTK implementation of wxNonOwnedWindow.
// Author:      Vadim Zeitlin
// Created:     2011-10-12
// RCS-ID:      $Id: wxhead.cpp,v 1.11 2010-04-22 12:44:51 zeitlin Exp $
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
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
    #include "wx/string.h"
#endif // WX_PRECOMP

#include "wx/gtk/private.h"

#include <gdk/gdk.h>

namespace
{

// helper
bool do_shape_combine_region(GdkWindow* window, const wxRegion& region)
{
    if (window)
    {
        if (region.IsEmpty())
        {
            gdk_window_shape_combine_mask(window, NULL, 0, 0);
        }
        else
        {
            gdk_window_shape_combine_region(window, region.GetRegion(), 0, 0);
            return true;
        }
    }
    return false;
}

} // anonymous namespace


// ============================================================================
// wxNonOwnedWindow implementation
// ============================================================================

void wxNonOwnedWindow::GTKHandleRealized()
{
    wxNonOwnedWindowBase::GTKHandleRealized();

    if (HasFlag(wxFRAME_SHAPED))
        SetShape(m_shape);
}

bool wxNonOwnedWindow::SetShape(const wxRegion& region)
{
    wxCHECK_MSG( HasFlag(wxFRAME_SHAPED), false,
                 wxT("Shaped windows must be created with the wxFRAME_SHAPED style."));

    if ( gtk_widget_get_realized(m_widget) )
    {
        if ( m_wxwindow )
            do_shape_combine_region(gtk_widget_get_window(m_wxwindow), region);

        return do_shape_combine_region(gtk_widget_get_window(m_widget), region);
    }
    else // not realized yet
    {
        // store the shape to set, it will be really set once we're realized
        m_shape = region;

        // we don't know if we're going to succeed or fail, be optimistic by
        // default
        return true;
    }
}
