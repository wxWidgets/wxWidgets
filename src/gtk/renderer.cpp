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
#endif

#include <gtk/gtk.h>
#include "wx/gtk/win_gtk.h"

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

    virtual wxSplitterRenderParams GetSplitterParams(const wxWindow *win);

// private:
    // FIXME: shouldn't we destroy these windows somewhere?

    // used by DrawPushButton and DrawDropArrow
    static GtkWidget *GetButtonWidget();

    // used by DrawTreeItemButton()
    static GtkWidget *GetTreeWidget();

    // used by DrawCheckBox()
    static GtkWidget *GetCheckButtonWidget();

    // used by DrawRadioButton()
    static GtkWidget *GetRadioButtonWidget();

    // Used by DrawHeaderButton
    static GtkWidget *GetHeaderButtonWidget();
    
    // Used by DrawTextCtrl
    static GtkWidget *GetTextEntryWidget();
    
    // Used by DrawComboBox
    static GtkWidget *GetComboBoxWidget();
};

GdkWindow* wxGetGdkWindowForDC(wxWindow* win, wxDC& dc)
{
    // The way to get a GdkWindow* from a wxWindow is to use 
    // GTK_PIZZA(win->m_wxwindow)->bin_window, but this approach
    // won't work when drawing to a wxMemoryDC as it has its own
    // GdkWindow* for its bitmap. wxWindowDC's GetGDKWindow() was
    // designed to create a solution for all DCs, but we can't
    // implement it with wxGCDC since it doesn't retain its wxWindow. 
    // So, to work around this, we use GetGDKWindow whenever possible
    // and use bin_window for wxGCDC. 
    GdkWindow* gdk_window = NULL;
#if wxUSE_GRAPHICS_CONTEXT
    if ( dc.IsKindOf( CLASSINFO(wxGCDC) ) )
        gdk_window = GTK_PIZZA(win->m_wxwindow)->bin_window;    
    else
#endif
       gdk_window = dc.GetGDKWindow();
    wxASSERT_MSG( gdk_window,
                  wxT("cannot use wxRendererNative on wxDC of this type") );
    
    return gdk_window;
}

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
// helper functions
// ----------------------------------------------------------------------------

GtkWidget *
wxRendererGTK::GetButtonWidget()
{
    static GtkWidget *s_button = NULL;
    static GtkWidget *s_window = NULL;

    if ( !s_button )
    {
        s_window = gtk_window_new( GTK_WINDOW_POPUP );
        gtk_widget_realize( s_window );
        s_button = gtk_button_new();
        gtk_container_add( GTK_CONTAINER(s_window), s_button );
        gtk_widget_realize( s_button );
    }

    return s_button;
}

GtkWidget *
wxRendererGTK::GetTextEntryWidget()
{
    static GtkWidget *s_button = NULL;
    static GtkWidget *s_window = NULL;

    if ( !s_button )
    {
        s_window = gtk_window_new( GTK_WINDOW_POPUP );
        gtk_widget_realize( s_window );
        s_button = gtk_entry_new();
        gtk_container_add( GTK_CONTAINER(s_window), s_button );
        gtk_widget_realize( s_button );
    }

    return s_button;
}

GtkWidget *
wxRendererGTK::GetComboBoxWidget()
{
    static GtkWidget *s_button = NULL;
    static GtkWidget *s_window = NULL;

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        if ( !s_button )
        {
            s_window = gtk_window_new( GTK_WINDOW_POPUP );
            gtk_widget_realize( s_window );
            s_button = gtk_combo_box_new();
            gtk_container_add( GTK_CONTAINER(s_window), s_button );
            gtk_widget_realize( s_button );
        }
    }
#endif
    return s_button;
}

GtkWidget *
wxRendererGTK::GetCheckButtonWidget()
{
    static GtkWidget *s_button = NULL;
    static GtkWidget *s_window = NULL;

    if ( !s_button )
    {
        s_window = gtk_window_new( GTK_WINDOW_POPUP );
        gtk_widget_realize( s_window );
        s_button = gtk_check_button_new();
        gtk_container_add( GTK_CONTAINER(s_window), s_button );
        gtk_widget_realize( s_button );
    }

    return s_button;
}

GtkWidget *
wxRendererGTK::GetRadioButtonWidget()
{
    static GtkWidget *s_button = NULL;
    static GtkWidget *s_window = NULL;

    if ( !s_button )
    {
        s_window = gtk_window_new( GTK_WINDOW_POPUP );
        gtk_widget_realize( s_window );
        s_button = gtk_radio_button_new(NULL);
        gtk_container_add( GTK_CONTAINER(s_window), s_button );
        gtk_widget_realize( s_button );
    }

    return s_button;
}

GtkWidget *
wxRendererGTK::GetTreeWidget()
{
    static GtkWidget *s_tree = NULL;
    static GtkWidget *s_window = NULL;

    if ( !s_tree )
    {
        s_tree = gtk_tree_view_new();
        s_window = gtk_window_new( GTK_WINDOW_POPUP );
        gtk_widget_realize( s_window );
        gtk_container_add( GTK_CONTAINER(s_window), s_tree );
        gtk_widget_realize( s_tree );
    }

    return s_tree;
}


// This one just gets the button used by the column header.  Although it's
// still a gtk_button the themes will typically differentiate and draw them
// differently if the button is in a treeview.
GtkWidget *
wxRendererGTK::GetHeaderButtonWidget()
{
    static GtkWidget *s_button = NULL;
    
    if ( !s_button )
    {
        // Get the dummy tree widget, give it a column, and then use the
        // widget in the column header for the rendering code.
        GtkWidget* treewidget = GetTreeWidget();
        GtkTreeViewColumn*  column = gtk_tree_view_column_new();
        gtk_tree_view_append_column(GTK_TREE_VIEW(treewidget), column);
        s_button = column->button;
    }

    return s_button;
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

    GtkWidget *button = GetHeaderButtonWidget();
    
    GdkWindow* gdk_window = wxGetGdkWindowForDC(win, dc);

    int x_diff = 0;
    if (win->GetLayoutDirection() == wxLayout_RightToLeft)
        x_diff = rect.width;
        
    gtk_paint_box
    (
        button->style,
        gdk_window,
        flags & wxCONTROL_DISABLED ? GTK_STATE_INSENSITIVE : GTK_STATE_NORMAL,
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
    GtkWidget *tree = GetTreeWidget();

    GdkWindow* gdk_window = wxGetGdkWindowForDC(win, dc);

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

static int GetGtkSplitterFullSize()
{
    static GtkWidget *s_paned = NULL;
    if (s_paned == NULL)
        s_paned = gtk_vpaned_new();

    gint handle_size;
    gtk_widget_style_get (s_paned, "handle_size", &handle_size, NULL);

    return handle_size;
}

wxSplitterRenderParams
wxRendererGTK::GetSplitterParams(const wxWindow *WXUNUSED(win))
{
    // we don't draw any border, hence 0 for the second field
    return wxSplitterRenderParams
           (
               GetGtkSplitterFullSize(),
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

    GdkWindow* gdk_window = wxGetGdkWindowForDC(win, dc);

    wxCoord full_size = GetGtkSplitterFullSize();

    // are we drawing vertical or horizontal splitter?
    const bool isVert = orient == wxVERTICAL;

    GdkRectangle rect;

    if ( isVert )
    {
        int h = win->GetClientSize().GetHeight();

        rect.x = position;
        rect.y = 0;
        rect.width = full_size;
        rect.height = h;
    }
    else // horz
    {
        int w = win->GetClientSize().GetWidth();

        rect.x = 0;
        rect.y = position;
        rect.height = full_size;
        rect.width = w;
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
wxRendererGTK::DrawDropArrow(wxWindow *win,
                             wxDC& dc,
                             const wxRect& rect,
                             int flags)
{
    GtkWidget *button = GetButtonWidget();

    GdkWindow* gdk_window = wxGetGdkWindowForDC(win, dc);

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

void
wxRendererGTK::DrawCheckBox(wxWindow *win,
                            wxDC& dc,
                            const wxRect& rect,
                            int flags )
{
    GtkWidget *button = GetCheckButtonWidget();

    GdkWindow* gdk_window = wxGetGdkWindowForDC(win, dc);

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
        dc.LogicalToDeviceX(rect.x)+2,
        dc.LogicalToDeviceY(rect.y)+3,
        13, 13
    );
}

void
wxRendererGTK::DrawPushButton(wxWindow* win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags)
{
    GtkWidget *button = GetButtonWidget();

    GdkWindow* gdk_window = wxGetGdkWindowForDC(win, dc);

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
        dc.LogicalToDeviceX(rect.x), 
        dc.LogicalToDeviceY(rect.y), 
        rect.width, 
        rect.height
    );
}

void
wxRendererGTK::DrawItemSelectionRect(wxWindow *win,
                                     wxDC& dc,
                                     const wxRect& rect,
                                     int flags )
{
    GdkWindow* gdk_window = wxGetGdkWindowForDC(win, dc);

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

    if (flags & wxCONTROL_CURRENT)
    {
        gtk_paint_focus( win->m_widget->style, 
                         gdk_window,
                         GTK_STATE_SELECTED,
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


// ----------------------------------------------------------------------------
// Other renderer functions to be merged in to wxRenderer class in 2.9, but
// they are standalone functions here to protect the ABI.
// ----------------------------------------------------------------------------

// Uses the theme to draw the border and fill for something like a wxTextCtrl
void wxRenderer_DrawTextCtrl(wxWindow* win, wxDC& dc, const wxRect& rect, int flags)
{
    GtkWidget *entry = wxRendererGTK::GetTextEntryWidget();

    GdkWindow* gdk_window = wxGetGdkWindowForDC(win, dc);

    GtkStateType state = GTK_STATE_NORMAL;
    if ( flags & wxCONTROL_DISABLED )
       state = GTK_STATE_INSENSITIVE;
       
   if (flags & wxCONTROL_CURRENT )
      GTK_WIDGET_SET_FLAGS( entry, GTK_HAS_FOCUS );
   else
      GTK_WIDGET_UNSET_FLAGS( entry, GTK_HAS_FOCUS );

    gtk_paint_shadow
    (
      entry->style, 
      gdk_window,
      state,
      GTK_SHADOW_OUT,
      NULL,
      entry,
      "entry",
      dc.LogicalToDeviceX(rect.x),
      dc.LogicalToDeviceY(rect.y),
      rect.width,
      rect.height 
   );
}

// Draw the equivallent of a wxComboBox
void wxRenderer_DrawComboBox(wxWindow* win, wxDC& dc, const wxRect& rect, int flags)
{
    if (gtk_check_version(2,4,0))
       return;

    GtkWidget *combo = wxRendererGTK::GetComboBoxWidget();

    GdkWindow* gdk_window = wxGetGdkWindowForDC(win, dc);

    GtkStateType state = GTK_STATE_NORMAL;
    if ( flags & wxCONTROL_DISABLED )
       state = GTK_STATE_INSENSITIVE;
       
   if (flags & wxCONTROL_CURRENT )
      GTK_WIDGET_SET_FLAGS( combo, GTK_HAS_FOCUS );
   else
      GTK_WIDGET_UNSET_FLAGS( combo, GTK_HAS_FOCUS );

   gtk_paint_shadow
   (
     combo->style, 
     gdk_window,
     state,
     GTK_SHADOW_OUT,
     NULL,
     combo,
     "combobox",
      dc.LogicalToDeviceX(rect.x),
      dc.LogicalToDeviceY(rect.y),
      rect.width,
      rect.height 
   );

   wxRect r = rect;
   int extent = rect.height / 2;
   r.x += rect.width - extent - extent/2;
   r.y += extent/2;
   r.width = extent;
   r.height = extent;

   gtk_paint_arrow
   (
     combo->style, 
     gdk_window,
     state,
     GTK_SHADOW_OUT,
     NULL,
     combo,
     "arrow",
     GTK_ARROW_DOWN,
     TRUE,
      dc.LogicalToDeviceX(r.x),
      dc.LogicalToDeviceY(r.y),
      r.width,
      r.height
   );

   r = rect;
   r.x += rect.width - 2*extent;
   r.width = 2;

   gtk_paint_box
   (
      combo->style, 
      gdk_window,
      state,
      GTK_SHADOW_ETCHED_OUT,
      NULL,
      combo,
      "vseparator",
      dc.LogicalToDeviceX(r.x),
      dc.LogicalToDeviceY(r.y+1),
      r.width,
      r.height-2
   );
}


void wxRenderer_DrawChoice(wxWindow* win, wxDC& dc,
                           const wxRect& rect, int flags)
{
   wxRenderer_DrawComboBox( win, dc, rect, flags );
}

    
// Draw a themed radio button
void wxRenderer_DrawRadioButton(wxWindow* win, wxDC& dc, const wxRect& rect, int flags)
{
    GtkWidget *button = wxRendererGTK::GetRadioButtonWidget();

    GdkWindow* gdk_window = wxGetGdkWindowForDC(win, dc);
    
    GtkShadowType shadow_type = GTK_SHADOW_OUT;
    if ( flags & wxCONTROL_CHECKED )
        shadow_type = GTK_SHADOW_IN;
    else if ( flags & wxCONTROL_UNDETERMINED )
        shadow_type = GTK_SHADOW_ETCHED_IN;
        
    GtkStateType state = GTK_STATE_NORMAL;
    if ( flags & wxCONTROL_DISABLED )
       state = GTK_STATE_INSENSITIVE;
    if ( flags & wxCONTROL_PRESSED )
    state = GTK_STATE_ACTIVE;
/*
    Don't know when to set this
       state_type = GTK_STATE_PRELIGHT;
*/

    gtk_paint_option
    (
        button->style,
        gdk_window,
        state,
        shadow_type,
        NULL,
        button,
        "radiobutton",
        dc.LogicalToDeviceX(rect.x), 
        dc.LogicalToDeviceY(rect.y), 
        rect.width, rect.height
    );
}
