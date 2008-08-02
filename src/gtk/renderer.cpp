///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/renderer.cpp
// Purpose:     implementation of wxRendererNative for wxGTK
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.07.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows licence
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

#include "wx/renderer.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
    #include "wx/module.h"
#endif

#include "wx/gtk/dc.h"
#include "wx/gtk/private.h"

#include <gtk/gtk.h>

// ----------------------------------------------------------------------------
// wxRendererGTK: our wxRendererNative implementation
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRendererGTK : public wxDelegateRendererNative
{
public:
    // draw the header control button (used by wxListCtrl)
    virtual int  DrawHeaderButton(wxWindow *win,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0,
                                  wxHeaderSortIconType sortArrow = wxHDR_SORT_ICON_NONE,
                                  wxHeaderButtonParams* params = NULL);

    // draw the expanded/collapsed icon for a tree control item
    virtual void DrawTreeItemButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0);

    virtual void DrawSplitterBorder(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0);
    virtual void DrawSplitterSash(wxWindow *win,
                                  wxDC& dc,
                                  const wxSize& size,
                                  wxCoord position,
                                  wxOrientation orient,
                                  int flags = 0);

    virtual void DrawComboBoxDropButton(wxWindow *win,
                                        wxDC& dc,
                                        const wxRect& rect,
                                        int flags = 0);

    virtual void DrawDropArrow(wxWindow *win,
                               wxDC& dc,
                               const wxRect& rect,
                               int flags = 0);

    virtual void DrawCheckBox(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags = 0);

    virtual void DrawPushButton(wxWindow *win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags = 0);

    virtual void DrawItemSelectionRect(wxWindow *win,
                                       wxDC& dc,
                                       const wxRect& rect,
                                       int flags = 0);

    virtual void DrawFocusRect(wxWindow* win, wxDC& dc, const wxRect& rect, int flags = 0);

    virtual wxSize GetCheckBoxSize(wxWindow *win);

    virtual wxSplitterRenderParams GetSplitterParams(const wxWindow *win);
};

// ============================================================================
// implementation
// ============================================================================

/* static */
wxRendererNative& wxRendererNative::GetDefault()
{
    static wxRendererGTK s_rendererGTK;

    return s_rendererGTK;
}

// ----------------------------------------------------------------------------
// list/tree controls drawing
// ----------------------------------------------------------------------------

int
wxRendererGTK::DrawHeaderButton(wxWindow *win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags,
                                wxHeaderSortIconType sortArrow,
                                wxHeaderButtonParams* params)
{

    GtkWidget *button = wxGTKPrivate::GetHeaderButtonWidget();

    GdkWindow* gdk_window = NULL;
#if wxUSE_NEW_DC
    wxDCImpl *impl = dc.GetImpl();
    wxGTKDCImpl *gtk_impl = wxDynamicCast( impl, wxGTKDCImpl );
    if (gtk_impl)
        gdk_window = gtk_impl->GetGDKWindow();
#else
    gdk_window = dc.GetGDKWindow();
#endif
    wxASSERT_MSG( gdk_window,
                  wxT("cannot use wxRendererNative on wxDC of this type") );

    int x_diff = 0;
    if (win->GetLayoutDirection() == wxLayout_RightToLeft)
        x_diff = rect.width;

    GtkStateType state = GTK_STATE_NORMAL;
    if (flags & wxCONTROL_DISABLED)
        state = GTK_STATE_INSENSITIVE;
    else
    {
        if (flags & wxCONTROL_CURRENT)
            state = GTK_STATE_PRELIGHT;
    }

    gtk_paint_box
    (
        button->style,
        gdk_window,
        state,
        GTK_SHADOW_OUT,
        NULL,
        button,
        "button",
        dc.LogicalToDeviceX(rect.x) - x_diff, rect.y, rect.width, rect.height
    );

    return DrawHeaderButtonContents(win, dc, rect, flags, sortArrow, params);
}

// draw a ">" or "v" button
void
wxRendererGTK::DrawTreeItemButton(wxWindow* win,
                                  wxDC& dc, const wxRect& rect, int flags)
{
    GtkWidget *tree = wxGTKPrivate::GetTreeWidget();

    GdkWindow* gdk_window = NULL;
#if wxUSE_NEW_DC
    wxDCImpl *impl = dc.GetImpl();
    wxGTKDCImpl *gtk_impl = wxDynamicCast( impl, wxGTKDCImpl );
    if (gtk_impl)
        gdk_window = gtk_impl->GetGDKWindow();
#else
    gdk_window = dc.GetGDKWindow();
#endif
    wxASSERT_MSG( gdk_window,
                  wxT("cannot use wxRendererNative on wxDC of this type") );

    GtkStateType state;
    if ( flags & wxCONTROL_CURRENT )
        state = GTK_STATE_PRELIGHT;
    else
        state = GTK_STATE_NORMAL;

    int x_diff = 0;
    if (win->GetLayoutDirection() == wxLayout_RightToLeft)
        x_diff = rect.width;

    // VZ: I don't know how to get the size of the expander so as to centre it
    //     in the given rectangle, +2/3 below is just what looks good here...
    gtk_paint_expander
    (
        tree->style,
        gdk_window,
        state,
        NULL,
        tree,
        "treeview",
        dc.LogicalToDeviceX(rect.x) + 6 - x_diff,
        dc.LogicalToDeviceY(rect.y) + 3,
        flags & wxCONTROL_EXPANDED ? GTK_EXPANDER_EXPANDED
                                   : GTK_EXPANDER_COLLAPSED
    );
}


// ----------------------------------------------------------------------------
// splitter sash drawing
// ----------------------------------------------------------------------------

static int GetGtkSplitterFullSize(GtkWidget* widget)
{
    gint handle_size;
    gtk_widget_style_get(widget, "handle_size", &handle_size, NULL);

    return handle_size;
}

wxSplitterRenderParams
wxRendererGTK::GetSplitterParams(const wxWindow *WXUNUSED(win))
{
    // we don't draw any border, hence 0 for the second field
    return wxSplitterRenderParams
           (
               GetGtkSplitterFullSize(wxGTKPrivate::GetSplitterWidget()),
               0,
               true     // hot sensitive
           );
}

void
wxRendererGTK::DrawSplitterBorder(wxWindow * WXUNUSED(win),
                                  wxDC& WXUNUSED(dc),
                                  const wxRect& WXUNUSED(rect),
                                  int WXUNUSED(flags))
{
    // nothing to do
}

void
wxRendererGTK::DrawSplitterSash(wxWindow *win,
                                wxDC& dc,
                                const wxSize& size,
                                wxCoord position,
                                wxOrientation orient,
                                int flags)
{
    if ( !win->m_wxwindow->window )
    {
        // window not realized yet
        return;
    }

    GdkWindow* gdk_window = NULL;
#if wxUSE_NEW_DC
    wxDCImpl *impl = dc.GetImpl();
    wxGTKDCImpl *gtk_impl = wxDynamicCast( impl, wxGTKDCImpl );
    if (gtk_impl)
        gdk_window = gtk_impl->GetGDKWindow();
#else
    gdk_window = dc.GetGDKWindow();
#endif
    wxASSERT_MSG( gdk_window,
                  wxT("cannot use wxRendererNative on wxDC of this type") );

    wxCoord full_size = GetGtkSplitterFullSize(wxGTKPrivate::GetSplitterWidget());

    // are we drawing vertical or horizontal splitter?
    const bool isVert = orient == wxVERTICAL;

    GdkRectangle rect;

    if ( isVert )
    {
        rect.x = position;
        rect.y = 0;
        rect.width = full_size;
        rect.height = size.y;
    }
    else // horz
    {
        rect.x = 0;
        rect.y = position;
        rect.height = full_size;
        rect.width = size.x;
    }

    int x_diff = 0;
    if (win->GetLayoutDirection() == wxLayout_RightToLeft)
        x_diff = rect.width;

    gtk_paint_handle
    (
        win->m_wxwindow->style,
        gdk_window,
        flags & wxCONTROL_CURRENT ? GTK_STATE_PRELIGHT : GTK_STATE_NORMAL,
        GTK_SHADOW_NONE,
        NULL /* no clipping */,
        win->m_wxwindow,
        "paned",
        dc.LogicalToDeviceX(rect.x) - x_diff,
        dc.LogicalToDeviceY(rect.y),
        rect.width,
        rect.height,
        isVert ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL
    );
}

void
wxRendererGTK::DrawDropArrow(wxWindow *WXUNUSED(win),
                             wxDC& dc,
                             const wxRect& rect,
                             int flags)
{
    GtkWidget *button = wxGTKPrivate::GetButtonWidget();

    // If we give GTK_PIZZA(win->m_wxwindow)->bin_window as
    // a window for gtk_paint_xxx function, then it won't
    // work for wxMemoryDC. So that is why we assume wxDC
    // is wxWindowDC (wxClientDC, wxMemoryDC and wxPaintDC
    // are derived from it) and use its m_window.
    GdkWindow* gdk_window = NULL;
#if wxUSE_NEW_DC
    wxDCImpl *impl = dc.GetImpl();
    wxGTKDCImpl *gtk_impl = wxDynamicCast( impl, wxGTKDCImpl );
    if (gtk_impl)
        gdk_window = gtk_impl->GetGDKWindow();
#else
    gdk_window = dc.GetGDKWindow();
#endif
    wxASSERT_MSG( gdk_window,
                  wxT("cannot use wxRendererNative on wxDC of this type") );

    // draw arrow so that there is even space horizontally
    // on both sides
    int arrowX = rect.width/4 + 1;
    int arrowWidth = rect.width - (arrowX*2);

    // scale arrow's height accoording to the width
    int arrowHeight = rect.width/3;
    int arrowY = (rect.height-arrowHeight)/2 +
                 ((rect.height-arrowHeight) & 1);

    GtkStateType state;

    if ( flags & wxCONTROL_PRESSED )
        state = GTK_STATE_ACTIVE;
    else if ( flags & wxCONTROL_DISABLED )
        state = GTK_STATE_INSENSITIVE;
    else if ( flags & wxCONTROL_CURRENT )
        state = GTK_STATE_PRELIGHT;
    else
        state = GTK_STATE_NORMAL;

    // draw arrow on button
    gtk_paint_arrow
    (
        button->style,
        gdk_window,
        state,
        flags & wxCONTROL_PRESSED ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
        NULL,
        button,
        "arrow",
        GTK_ARROW_DOWN,
        FALSE,
        rect.x + arrowX,
        rect.y + arrowY,
        arrowWidth,
        arrowHeight
    );
}

void
wxRendererGTK::DrawComboBoxDropButton(wxWindow *win,
                                      wxDC& dc,
                                      const wxRect& rect,
                                      int flags)
{
    DrawPushButton(win,dc,rect,flags);
    DrawDropArrow(win,dc,rect);
}

wxSize
wxRendererGTK::GetCheckBoxSize(wxWindow *WXUNUSED(win))
{
    gint indicator_size, indicator_spacing;
    gtk_widget_style_get(wxGTKPrivate::GetCheckButtonWidget(),
                         "indicator_size", &indicator_size,
                         "indicator_spacing", &indicator_spacing,
                         NULL);

    int size = indicator_size + indicator_spacing * 2;
    return wxSize(size, size);
}

void
wxRendererGTK::DrawCheckBox(wxWindow *WXUNUSED(win),
                            wxDC& dc,
                            const wxRect& rect,
                            int flags )
{
    GtkWidget *button = wxGTKPrivate::GetCheckButtonWidget();

    GdkWindow* gdk_window = NULL;
#if wxUSE_NEW_DC
    wxDCImpl *impl = dc.GetImpl();
    wxGTKDCImpl *gtk_impl = wxDynamicCast( impl, wxGTKDCImpl );
    if (gtk_impl)
        gdk_window = gtk_impl->GetGDKWindow();
#else
    gdk_window = dc.GetGDKWindow();
#endif
    wxASSERT_MSG( gdk_window,
                  wxT("cannot use wxRendererNative on wxDC of this type") );

    gint indicator_size, indicator_spacing;
    gtk_widget_style_get(button,
                         "indicator_size", &indicator_size,
                         "indicator_spacing", &indicator_spacing,
                         NULL);

    GtkStateType state;

    if ( flags & wxCONTROL_PRESSED )
        state = GTK_STATE_ACTIVE;
    else if ( flags & wxCONTROL_DISABLED )
        state = GTK_STATE_INSENSITIVE;
    else if ( flags & wxCONTROL_CURRENT )
        state = GTK_STATE_PRELIGHT;
    else
        state = GTK_STATE_NORMAL;

    gtk_paint_check
    (
        button->style,
        gdk_window,
        state,
        flags & wxCONTROL_CHECKED ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
        NULL,
        button,
        "cellcheck",
        dc.LogicalToDeviceX(rect.x) + indicator_spacing,
        dc.LogicalToDeviceY(rect.y) + indicator_spacing,
        indicator_size, indicator_size
    );
}

void
wxRendererGTK::DrawPushButton(wxWindow *WXUNUSED(win),
                              wxDC& dc,
                              const wxRect& rect,
                              int flags)
{
    GtkWidget *button = wxGTKPrivate::GetButtonWidget();

    GdkWindow* gdk_window = NULL;
#if wxUSE_NEW_DC
    wxDCImpl *impl = dc.GetImpl();
    wxGTKDCImpl *gtk_impl = wxDynamicCast( impl, wxGTKDCImpl );
    if (gtk_impl)
        gdk_window = gtk_impl->GetGDKWindow();
#else
    gdk_window = dc.GetGDKWindow();
#endif
    wxASSERT_MSG( gdk_window,
                  wxT("cannot use wxRendererNative on wxDC of this type") );

    // draw button
    GtkStateType state;

    if ( flags & wxCONTROL_PRESSED )
        state = GTK_STATE_ACTIVE;
    else if ( flags & wxCONTROL_DISABLED )
        state = GTK_STATE_INSENSITIVE;
    else if ( flags & wxCONTROL_CURRENT )
        state = GTK_STATE_PRELIGHT;
    else
        state = GTK_STATE_NORMAL;

    gtk_paint_box
    (
        button->style,
        gdk_window,
        state,
        flags & wxCONTROL_PRESSED ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
        NULL,
        button,
        "button",
        rect.x, rect.y, rect.width, rect.height
    );
}

void
wxRendererGTK::DrawItemSelectionRect(wxWindow *win,
                                     wxDC& dc,
                                     const wxRect& rect,
                                     int flags )
{
    GdkWindow* gdk_window = NULL;
#if wxUSE_NEW_DC
    wxDCImpl *impl = dc.GetImpl();
    wxGTKDCImpl *gtk_impl = wxDynamicCast( impl, wxGTKDCImpl );
    if (gtk_impl)
        gdk_window = gtk_impl->GetGDKWindow();
#else
    gdk_window = dc.GetGDKWindow();
#endif
    wxASSERT_MSG( gdk_window,
                  wxT("cannot use wxRendererNative on wxDC of this type") );

    int x_diff = 0;
    if (win->GetLayoutDirection() == wxLayout_RightToLeft)
        x_diff = rect.width;

    GtkStateType state;
    if (flags & wxCONTROL_SELECTED)
    {
        // the wxCONTROL_FOCUSED state is deduced
        // directly from the m_wxwindow by GTK+
        state = GTK_STATE_SELECTED;

        gtk_paint_flat_box( win->m_widget->style,
                        gdk_window,
                        state,
                        GTK_SHADOW_NONE,
                        NULL,
                        win->m_wxwindow,
                        "cell_even",
                        dc.LogicalToDeviceX(rect.x) - x_diff,
                        dc.LogicalToDeviceY(rect.y),
                        rect.width,
                        rect.height );
    }
    else // !wxCONTROL_SELECTED
    {
        state = GTK_STATE_NORMAL;
    }

    if ((flags & wxCONTROL_CURRENT) && (flags & wxCONTROL_FOCUSED))
    {
        gtk_paint_focus( win->m_widget->style,
                         gdk_window,
                         state,
                         NULL,
                         win->m_wxwindow,
                         // Detail "treeview" causes warning with GTK+ 2.12 Clearlooks theme:
                         // "... no property named `row-ending-details'"
                         // Using "treeview-middle" would fix the warning, but the right
                         // edge of the focus rect is not getting erased properly either.
                         // Better to not specify this detail unless the drawing is fixed.
                         NULL,
                         dc.LogicalToDeviceX(rect.x),
                         dc.LogicalToDeviceY(rect.y),
                         rect.width,
                         rect.height );
    }
}

void wxRendererGTK::DrawFocusRect(wxWindow* win, wxDC& dc, const wxRect& rect, int flags)
{
    GdkWindow* gdk_window = NULL;
#if wxUSE_NEW_DC
    wxDCImpl *impl = dc.GetImpl();
    wxGTKDCImpl *gtk_impl = wxDynamicCast( impl, wxGTKDCImpl );
    if (gtk_impl)
        gdk_window = gtk_impl->GetGDKWindow();
#else
    gdk_window = dc.GetGDKWindow();
#endif
    wxASSERT_MSG( gdk_window,
                  wxT("cannot use wxRendererNative on wxDC of this type") );

    GtkStateType state;
    if (flags & wxCONTROL_SELECTED)
        state = GTK_STATE_SELECTED;
    else
        state = GTK_STATE_NORMAL;

    gtk_paint_focus( win->m_widget->style,
                     gdk_window,
                     state,
                     NULL,
                     win->m_wxwindow,
                     NULL,
                     dc.LogicalToDeviceX(rect.x),
                     dc.LogicalToDeviceY(rect.y),
                     rect.width,
                     rect.height );
}
