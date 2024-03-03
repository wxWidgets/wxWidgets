///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/renderer.cpp
// Purpose:     implementation of wxRendererNative for wxGTK
// Author:      Vadim Zeitlin
// Created:     20.07.2003
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
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


#include "wx/renderer.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
    #include "wx/module.h"
#endif

#include "wx/dcgraph.h"
#ifndef __WXGTK3__
    #include "wx/gtk/dc.h"
    #include "wx/gtk/private/wrapgtk.h"
    #if wxUSE_GRAPHICS_CONTEXT && defined(GDK_WINDOWING_X11)
        #include <gdk/gdkx.h>
        #include <cairo-xlib.h>
    #endif
#endif

#include "wx/gtk/private.h"
#include "wx/gtk/private/stylecontext.h"
#include "wx/gtk/private/value.h"

#if defined(__WXGTK3__) && !GTK_CHECK_VERSION(3,14,0)
    #define GTK_STATE_FLAG_CHECKED (1 << 11)
#endif

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
                                  wxHeaderButtonParams* params = nullptr) override;

    virtual int GetHeaderButtonHeight(wxWindow *win) override;

    virtual int GetHeaderButtonMargin(wxWindow *win) override;


    // draw the expanded/collapsed icon for a tree control item
    virtual void DrawTreeItemButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0) override;

    virtual void DrawSplitterBorder(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0) override;
    virtual void DrawSplitterSash(wxWindow *win,
                                  wxDC& dc,
                                  const wxSize& size,
                                  wxCoord position,
                                  wxOrientation orient,
                                  int flags = 0) override;

    virtual void DrawComboBoxDropButton(wxWindow *win,
                                        wxDC& dc,
                                        const wxRect& rect,
                                        int flags = 0) override;

    virtual void DrawDropArrow(wxWindow *win,
                               wxDC& dc,
                               const wxRect& rect,
                               int flags = 0) override;

    virtual void DrawCheckBox(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags = 0) override;

    virtual void DrawPushButton(wxWindow *win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags = 0) override;

    virtual void DrawItemSelectionRect(wxWindow *win,
                                       wxDC& dc,
                                       const wxRect& rect,
                                       int flags = 0) override;

    virtual void DrawChoice(wxWindow* win,
                            wxDC& dc,
                            const wxRect& rect,
                            int flags=0) override;

    virtual void DrawComboBox(wxWindow* win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags=0) override;

    virtual void DrawTextCtrl(wxWindow* win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags=0) override;

    virtual void DrawRadioBitmap(wxWindow* win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags=0) override;

    virtual void DrawFocusRect(wxWindow* win, wxDC& dc, const wxRect& rect, int flags = 0) override;

    virtual wxSize GetCheckBoxSize(wxWindow *win, int flags = 0) override;

    virtual wxSplitterRenderParams GetSplitterParams(const wxWindow *win) override;
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

#ifdef __WXGTK3__
#define NULL_RECT
typedef cairo_t wxGTKDrawable;

static cairo_t* wxGetGTKDrawable(const wxDC& dc)
{
    wxGraphicsContext* gc = dc.GetGraphicsContext();
    wxCHECK_MSG(gc, nullptr, "cannot use wxRendererNative on wxDC of this type");
    return static_cast<cairo_t*>(gc->GetNativeContext());
}

static const GtkStateFlags stateTypeToFlags[] = {
    GTK_STATE_FLAG_NORMAL, GTK_STATE_FLAG_ACTIVE, GTK_STATE_FLAG_PRELIGHT,
    GTK_STATE_FLAG_SELECTED, GTK_STATE_FLAG_INSENSITIVE, GTK_STATE_FLAG_INCONSISTENT,
    GTK_STATE_FLAG_FOCUSED
};

#else
#define NULL_RECT nullptr,
typedef GdkWindow wxGTKDrawable;

static GdkWindow* wxGetGTKDrawable(wxDC& dc)
{
    GdkWindow* gdk_window = nullptr;

#if wxUSE_GRAPHICS_CONTEXT && defined(GDK_WINDOWING_X11)
    cairo_t* cr = nullptr;
    wxGraphicsContext* gc = dc.GetGraphicsContext();
    if (gc)
        cr = static_cast<cairo_t*>(gc->GetNativeContext());
    if (cr)
    {
        cairo_surface_t* surf = cairo_get_target(cr);
        if (cairo_surface_get_type(surf) == CAIRO_SURFACE_TYPE_XLIB)
        {
            gdk_window = static_cast<GdkWindow*>(
                gdk_xid_table_lookup(cairo_xlib_surface_get_drawable(surf)));
        }
    }
    if (gdk_window == nullptr)
#endif
    {
        wxDCImpl *impl = dc.GetImpl();
        wxGTKDCImpl *gtk_impl = wxDynamicCast( impl, wxGTKDCImpl );
        if (gtk_impl)
            gdk_window = gtk_impl->GetGDKWindow();
        else
            wxFAIL_MSG("cannot use wxRendererNative on wxDC of this type");
    }

    return gdk_window;
}
#endif

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
    if (flags & wxCONTROL_SPECIAL)
        button = wxGTKPrivate::GetHeaderButtonWidgetFirst();
    if (flags & wxCONTROL_DIRTY)
        button = wxGTKPrivate::GetHeaderButtonWidgetLast();

    GtkStateType state = GTK_STATE_NORMAL;
    if (flags & wxCONTROL_DISABLED)
        state = GTK_STATE_INSENSITIVE;
    else
    {
        if (flags & wxCONTROL_CURRENT)
            state = GTK_STATE_PRELIGHT;
    }

#ifdef __WXGTK3__
    cairo_t* cr = wxGetGTKDrawable(dc);
    if (cr == nullptr)
        return 0;

    // AddTreeviewHeaderButton() is only available in 3.20 or later.
#if GTK_CHECK_VERSION(3,20,0)
    if (gtk_check_version(3,20,0) == nullptr)
    {
        int pos = 1;
        if (flags & wxCONTROL_SPECIAL)
            pos = 0;
        if (flags & wxCONTROL_DIRTY)
            pos = 2;

        wxGtkStyleContext sc(dc.GetContentScaleFactor());
        sc.AddTreeviewHeaderButton(pos);

        gtk_style_context_set_state(sc, stateTypeToFlags[state]);
        gtk_render_background(sc, cr, rect.x, rect.y, rect.width, rect.height);
        gtk_render_frame(sc, cr, rect.x, rect.y, rect.width, rect.height);
    }
    else
#endif // GTK >= 3.20
    {
        GtkStyleContext* sc = gtk_widget_get_style_context(button);
        gtk_style_context_save(sc);
        gtk_style_context_set_state(sc, stateTypeToFlags[state]);
        gtk_render_background(sc, cr, rect.x, rect.y, rect.width, rect.height);
        gtk_render_frame(sc, cr, rect.x, rect.y, rect.width, rect.height);
        gtk_style_context_restore(sc);
    }
#else
    int x_diff = 0;
    if (win->GetLayoutDirection() == wxLayout_RightToLeft)
        x_diff = rect.width;

    GdkWindow* gdk_window = wxGetGTKDrawable(dc);
    gtk_paint_box
    (
        gtk_widget_get_style(button),
        gdk_window,
        state,
        GTK_SHADOW_OUT,
        nullptr,
        button,
        "button",
        dc.LogicalToDeviceX(rect.x) - x_diff, rect.y, rect.width, rect.height
    );
#endif

    return DrawHeaderButtonContents(win, dc, rect, flags, sortArrow, params);
}

int wxRendererGTK::GetHeaderButtonHeight(wxWindow *WXUNUSED(win))
{
    GtkWidget *button = wxGTKPrivate::GetHeaderButtonWidget();

    GtkRequisition req;
#ifdef __WXGTK3__
    gtk_widget_get_preferred_height(button, nullptr, &req.height);
#else
    GTK_WIDGET_GET_CLASS(button)->size_request(button, &req);
#endif

    return req.height;
}

int wxRendererGTK::GetHeaderButtonMargin(wxWindow *WXUNUSED(win))
{
    return 0; // TODO: How to determine the real margin?
}


// draw a ">" or "v" button
void
wxRendererGTK::DrawTreeItemButton(wxWindow* WXUNUSED_IN_GTK3(win),
                                  wxDC& dc, const wxRect& rect, int flags)
{
    wxGTKDrawable* drawable = wxGetGTKDrawable(dc);
    if (drawable == nullptr)
        return;

    GtkWidget *tree = wxGTKPrivate::GetTreeWidget();

#ifdef __WXGTK3__
    int state = GTK_STATE_FLAG_NORMAL;
    if (flags & wxCONTROL_EXPANDED)
    {
        state = GTK_STATE_FLAG_ACTIVE;
        if (gtk_check_version(3,14,0) == nullptr)
            state = GTK_STATE_FLAG_CHECKED;
    }
    if (flags & wxCONTROL_CURRENT)
        state |= GTK_STATE_FLAG_PRELIGHT;

    int expander_size;
    gtk_widget_style_get(tree, "expander-size", &expander_size, nullptr);
    // +1 to match GtkTreeView behavior
    expander_size++;
    const int x = rect.x + (rect.width - expander_size) / 2;
    const int y = rect.y + (rect.width - expander_size) / 2;

    GtkStyleContext* sc = gtk_widget_get_style_context(tree);
    gtk_style_context_save(sc);
    gtk_style_context_set_state(sc, GtkStateFlags(state));
    gtk_style_context_add_class(sc, GTK_STYLE_CLASS_EXPANDER);
    gtk_render_expander(sc, drawable, x, y, expander_size, expander_size);
    gtk_style_context_restore(sc);
#else
    int x_diff = 0;
    if (win->GetLayoutDirection() == wxLayout_RightToLeft)
        x_diff = rect.width;

    GtkStateType state;
    if ( flags & wxCONTROL_CURRENT )
        state = GTK_STATE_PRELIGHT;
    else
        state = GTK_STATE_NORMAL;

    // x and y parameters specify the center of the expander
    gtk_paint_expander
    (
        gtk_widget_get_style(tree),
        drawable,
        state,
        nullptr,
        tree,
        "treeview",
        dc.LogicalToDeviceX(rect.x) + rect.width / 2 - x_diff,
        dc.LogicalToDeviceY(rect.y) + rect.height / 2,
        flags & wxCONTROL_EXPANDED ? GTK_EXPANDER_EXPANDED
                                   : GTK_EXPANDER_COLLAPSED
    );
#endif
}


// ----------------------------------------------------------------------------
// splitter sash drawing
// ----------------------------------------------------------------------------

static int GetGtkSplitterFullSize(GtkWidget* widget)
{
    gint handle_size;
    gtk_widget_style_get(widget, "handle_size", &handle_size, nullptr);
    // Narrow handles don't work well with wxSplitterWindow
    if (handle_size < 5)
        handle_size = 5;

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
wxRendererGTK::DrawSplitterSash(wxWindow* win,
                                wxDC& dc,
                                const wxSize& size,
                                wxCoord position,
                                wxOrientation orient,
                                int flags)
{
    if (gtk_widget_get_window(win->m_wxwindow) == nullptr)
    {
        // window not realized yet
        return;
    }

    wxGTKDrawable* drawable = wxGetGTKDrawable(dc);
    if (drawable == nullptr)
        return;

    // are we drawing vertical or horizontal splitter?
    const bool isVert = orient == wxVERTICAL;

    GtkWidget* widget = wxGTKPrivate::GetSplitterWidget(orient);
    const int full_size = GetGtkSplitterFullSize(widget);

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

#ifdef __WXGTK3__
    wxGtkStyleContext sc(dc.GetContentScaleFactor());
    sc.AddWindow();
    gtk_render_background(sc, drawable, rect.x, rect.y, rect.width, rect.height);

    sc.Add(GTK_TYPE_PANED, "paned", "pane-separator", nullptr);
    if (gtk_check_version(3,20,0) == nullptr)
        sc.Add("separator");

    gtk_style_context_set_state(sc,
        flags & wxCONTROL_CURRENT ? GTK_STATE_FLAG_PRELIGHT : GTK_STATE_FLAG_NORMAL);
    gtk_render_handle(sc, drawable, rect.x, rect.y, rect.width, rect.height);
#else
    int x_diff = 0;
    if (win->GetLayoutDirection() == wxLayout_RightToLeft)
        x_diff = rect.width;

    GdkWindow* gdk_window = wxGetGTKDrawable(dc);
    if (gdk_window == nullptr)
        return;
    gtk_paint_handle
    (
        gtk_widget_get_style(win->m_wxwindow),
        gdk_window,
        flags & wxCONTROL_CURRENT ? GTK_STATE_PRELIGHT : GTK_STATE_NORMAL,
        GTK_SHADOW_NONE,
        nullptr /* no clipping */,
        win->m_wxwindow,
        "paned",
        dc.LogicalToDeviceX(rect.x) - x_diff,
        dc.LogicalToDeviceY(rect.y),
        rect.width,
        rect.height,
        isVert ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL
    );
#endif
}

void
wxRendererGTK::DrawDropArrow(wxWindow*,
                             wxDC& dc,
                             const wxRect& rect,
                             int flags)
{
    // If we give WX_PIZZA(win->m_wxwindow)->bin_window as
    // a window for gtk_paint_xxx function, then it won't
    // work for wxMemoryDC. So that is why we assume wxDC
    // is wxWindowDC (wxClientDC, wxMemoryDC and wxPaintDC
    // are derived from it) and use its m_window.

    // draw arrow so that there is even space horizontally
    // on both sides
    const int size = rect.width / 2;
    const int x = rect.x + (size + 1) / 2;
    const int y = rect.y + (rect.height - size + 1) / 2;

    GtkStateType state;

    if ( flags & wxCONTROL_PRESSED )
        state = GTK_STATE_ACTIVE;
    else if ( flags & wxCONTROL_DISABLED )
        state = GTK_STATE_INSENSITIVE;
    else if ( flags & wxCONTROL_CURRENT )
        state = GTK_STATE_PRELIGHT;
    else
        state = GTK_STATE_NORMAL;

#ifdef __WXGTK3__
    cairo_t* cr = wxGetGTKDrawable(dc);
    if (cr)
    {
        wxGtkStyleContext sc(dc.GetContentScaleFactor());
        sc.AddButton();
        gtk_style_context_set_state(sc, stateTypeToFlags[state]);
        gtk_render_arrow(sc, cr, G_PI, x, y, size);
    }
#else
    GdkWindow* gdk_window = wxGetGTKDrawable(dc);
    if (gdk_window == nullptr)
        return;

    GtkWidget* button = wxGTKPrivate::GetButtonWidget();

    // draw arrow on button
    gtk_paint_arrow
    (
        gtk_widget_get_style(button),
        gdk_window,
        state,
        flags & wxCONTROL_PRESSED ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
        nullptr,
        button,
        "arrow",
        GTK_ARROW_DOWN,
        FALSE,
        x, y,
        size, size
    );
#endif
}

void
wxRendererGTK::DrawComboBoxDropButton(wxWindow *win,
                                      wxDC& dc,
                                      const wxRect& rect,
                                      int flags)
{
    DrawPushButton(win,dc,rect,flags);
    DrawDropArrow(win,dc,rect,flags);
}

// Helper used by GetCheckBoxSize() and DrawCheckBox().
namespace
{

struct CheckBoxInfo
{
#ifdef __WXGTK3__
    CheckBoxInfo(wxGtkStyleContext& sc, int flags)
    {
        wxUnusedVar(flags);

        sc.AddCheckButton();
        if (gtk_check_version(3,20,0) == nullptr)
        {
            sc.Add("check");
            gtk_style_context_get(sc, GTK_STATE_FLAG_NORMAL,
                                  "min-width", &indicator_width,
                                  "min-height", &indicator_height,
                                  nullptr);

            GtkBorder border, padding;
            gtk_style_context_get_border(sc, GTK_STATE_FLAG_NORMAL, &border);
            gtk_style_context_get_padding(sc, GTK_STATE_FLAG_NORMAL, &padding);

            margin_left = border.left + padding.left;
            margin_top = border.top + padding.top;
            margin_right = border.right + padding.right;
            margin_bottom = border.bottom + padding.bottom;
        }
        else
        {
            wxGtkValue value( G_TYPE_INT);

            gtk_style_context_get_style_property(sc, "indicator-size", value);
            indicator_width =
            indicator_height = g_value_get_int(value);

            gtk_style_context_get_style_property(sc, "indicator-spacing", value);
            margin_left =
            margin_top =
            margin_right =
            margin_bottom = g_value_get_int(value);
        }
    }
#else // !__WXGTK3__
    CheckBoxInfo(GtkWidget* button, int flags)
    {
        gint indicator_size, indicator_margin;
        gtk_widget_style_get(button,
                             "indicator_size", &indicator_size,
                             "indicator_spacing", &indicator_margin,
                             nullptr);

        // If wxCONTROL_CELL is set then we want to get the size of wxCheckBox
        // control to draw the check mark centered and at the same position as
        // wxCheckBox does, so offset the check mark itself by the focus margin
        // in the same way as gtk_real_check_button_draw_indicator() does it, see
        // https://github.com/GNOME/gtk/blob/GTK_2_16_0/gtk/gtkcheckbutton.c#L374
        if ( flags & wxCONTROL_CELL )
        {
            gint focus_width, focus_pad;
            gtk_widget_style_get(button,
                                 "focus-line-width", &focus_width,
                                 "focus-padding", &focus_pad,
                                 nullptr);

            indicator_margin += focus_width + focus_pad;
        }

        // In GTK 2 width and height are the same and so are left/right and
        // top/bottom.
        indicator_width =
        indicator_height = indicator_size;

        margin_left =
        margin_top =
        margin_right =
        margin_bottom = indicator_margin;
    }
#endif // __WXGTK3__/!__WXGTK3__

    // Make sure we fit into the provided rectangle, eliminating margins and
    // even reducing the size if necessary.
    void FitInto(const wxRect& rect)
    {
        if ( indicator_width > rect.width )
        {
            indicator_width = rect.width;
            margin_left =
            margin_right = 0;
        }
        else if ( indicator_width + margin_left + margin_right > rect.width )
        {
            margin_left =
            margin_right = (rect.width - indicator_width) / 2;
        }

        if ( indicator_height > rect.height )
        {
            indicator_height = rect.height;
            margin_top =
            margin_bottom = 0;
        }
        else if ( indicator_height + margin_top + margin_bottom > rect.height )
        {
            margin_top =
            margin_bottom = (rect.height - indicator_height) / 2;
        }
    }

    gint indicator_width,
         indicator_height;
    gint margin_left,
         margin_top,
         margin_right,
         margin_bottom;
};

} // anonymous namespace

wxSize
wxRendererGTK::GetCheckBoxSize(wxWindow* win, int flags)
{
    wxSize size;
    // Even though we don't use the window in this implementation, still check
    // that it's valid to avoid surprises when running the same code under the
    // other platforms.
    wxCHECK_MSG(win, size, "Must have a valid window");

#ifdef __WXGTK3__
    wxGtkStyleContext sc(win->GetContentScaleFactor());

    const CheckBoxInfo info(sc, flags);
#else // !__WXGTK3__
    GtkWidget* button = wxGTKPrivate::GetCheckButtonWidget();

    const CheckBoxInfo info(button, flags);
#endif // __WXGTK3__/!__WXGTK3__

    size.x = info.indicator_width + info.margin_left + info.margin_right;
    size.y = info.indicator_height + info.margin_top + info.margin_bottom;

    return size;
}

void
wxRendererGTK::DrawCheckBox(wxWindow*,
                            wxDC& dc,
                            const wxRect& rect,
                            int flags )
{
#ifdef __WXGTK3__
    cairo_t* cr = wxGetGTKDrawable(dc);
    if (cr == nullptr)
        return;

    int state = GTK_STATE_FLAG_NORMAL;
    if (flags & wxCONTROL_CHECKED)
    {
        state = GTK_STATE_FLAG_ACTIVE;
        if (gtk_check_version(3,14,0) == nullptr)
            state = GTK_STATE_FLAG_CHECKED;
    }
    if (flags & wxCONTROL_DISABLED)
        state |= GTK_STATE_FLAG_INSENSITIVE;
    if (flags & wxCONTROL_UNDETERMINED)
        state |= GTK_STATE_FLAG_INCONSISTENT;
    if (flags & wxCONTROL_CURRENT)
        state |= GTK_STATE_FLAG_PRELIGHT;

    wxGtkStyleContext sc(dc.GetContentScaleFactor());

    CheckBoxInfo info(sc, flags);
    info.FitInto(rect);

    const int w = info.indicator_width + info.margin_left + info.margin_right;
    const int h = info.indicator_height + info.margin_top + info.margin_bottom;

    int x = rect.x + (rect.width  - w) / 2;
    int y = rect.y + (rect.height - h) / 2;

    const bool isRTL = dc.GetLayoutDirection() == wxLayout_RightToLeft;
    if (isRTL)
    {
        // checkbox is not mirrored
        cairo_save(cr);
        cairo_scale(cr, -1, 1);
        x = -x - w;
    }

    if (gtk_check_version(3,20,0) == nullptr)
    {
        gtk_style_context_set_state(sc, GtkStateFlags(state));
        gtk_render_background(sc, cr, x, y, w, h);
        gtk_render_frame(sc, cr, x, y, w, h);

        // check is rendered in content area
        gtk_render_check(sc, cr,
                         x + info.margin_left, y + info.margin_top,
                         info.indicator_width, info.indicator_height);
    }
    else
    {
        // need save/restore for GTK+ 3.6 & 3.8
        gtk_style_context_save(sc);
        gtk_style_context_set_state(sc, GtkStateFlags(state));
        gtk_render_background(sc, cr, x, y, w, h);
        gtk_render_frame(sc, cr, x, y, w, h);
        gtk_style_context_add_class(sc, "check");
        gtk_render_check(sc, cr, x, y, w, h);
        gtk_style_context_restore(sc);
    }
    if (isRTL)
        cairo_restore(cr);

#else // !__WXGTK3__
    GtkWidget* button = wxGTKPrivate::GetCheckButtonWidget();

    CheckBoxInfo info(button, flags);
    info.FitInto(rect);

    GtkStateType state;

    if ( flags & wxCONTROL_PRESSED )
        state = GTK_STATE_ACTIVE;
    else if ( flags & wxCONTROL_DISABLED )
        state = GTK_STATE_INSENSITIVE;
    else if ( flags & wxCONTROL_CURRENT )
        state = GTK_STATE_PRELIGHT;
    else
        state = GTK_STATE_NORMAL;

    GtkShadowType shadow_type;

    if ( flags & wxCONTROL_UNDETERMINED )
        shadow_type = GTK_SHADOW_ETCHED_IN;
    else if ( flags & wxCONTROL_CHECKED )
        shadow_type = GTK_SHADOW_IN;
    else
        shadow_type = GTK_SHADOW_OUT;

    GdkWindow* gdk_window = wxGetGTKDrawable(dc);
    if (gdk_window == nullptr)
        return;

    gtk_paint_check
    (
        gtk_widget_get_style(button),
        gdk_window,
        state,
        shadow_type,
        nullptr,
        button,
        "cellcheck",
        dc.LogicalToDeviceX(rect.x) + info.margin_left,
        dc.LogicalToDeviceY(rect.y) + (rect.height - info.indicator_height) / 2,
        info.indicator_width, info.indicator_height
    );
#endif // __WXGTK3__/!__WXGTK3__
}

void
wxRendererGTK::DrawPushButton(wxWindow*,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags)
{
    GtkWidget *button = wxGTKPrivate::GetButtonWidget();

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

#ifdef __WXGTK3__
    cairo_t* cr = wxGetGTKDrawable(dc);
    if (cr)
    {
        GtkStyleContext* sc = gtk_widget_get_style_context(button);
        gtk_style_context_save(sc);
        gtk_style_context_set_state(sc, stateTypeToFlags[state]);
        gtk_render_background(sc, cr, rect.x, rect.y, rect.width, rect.height);
        gtk_render_frame(sc, cr, rect.x, rect.y, rect.width, rect.height);
        gtk_style_context_restore(sc);
    }
#else
    GdkWindow* gdk_window = wxGetGTKDrawable(dc);
    if (gdk_window == nullptr)
        return;

    gtk_paint_box
    (
        gtk_widget_get_style(button),
        gdk_window,
        state,
        flags & wxCONTROL_PRESSED ? GTK_SHADOW_IN : GTK_SHADOW_OUT,
        nullptr,
        button,
        "button",
        dc.LogicalToDeviceX(rect.x),
        dc.LogicalToDeviceY(rect.y),
        rect.width,
        rect.height
    );
#endif
}

void
wxRendererGTK::DrawItemSelectionRect(wxWindow* win,
                                     wxDC& dc,
                                     const wxRect& rect,
                                     int flags )
{
    wxGTKDrawable* drawable = wxGetGTKDrawable(dc);
    if (drawable == nullptr)
        return;

    if (flags & wxCONTROL_SELECTED)
    {
        GtkWidget* treeWidget = wxGTKPrivate::GetTreeWidget();

#ifdef __WXGTK3__
        GtkStyleContext* sc = gtk_widget_get_style_context(treeWidget);
        gtk_style_context_save(sc);
        int state = GTK_STATE_FLAG_SELECTED;
        if (flags & wxCONTROL_FOCUSED)
            state |= GTK_STATE_FLAG_FOCUSED;
        gtk_style_context_set_state(sc, GtkStateFlags(state));
        gtk_style_context_add_class(sc, GTK_STYLE_CLASS_CELL);
        gtk_render_background(sc, drawable, rect.x, rect.y, rect.width, rect.height);
        gtk_style_context_restore(sc);
#else
        int x_diff = 0;
        if (win->GetLayoutDirection() == wxLayout_RightToLeft)
            x_diff = rect.width;

        // the wxCONTROL_FOCUSED state is deduced
        // directly from the m_wxwindow by GTK+
        gtk_paint_flat_box(gtk_widget_get_style(treeWidget),
                        drawable,
                        GTK_STATE_SELECTED,
                        GTK_SHADOW_NONE,
                        NULL_RECT
                        win->m_wxwindow,
                        "cell_even",
                        dc.LogicalToDeviceX(rect.x) - x_diff,
                        dc.LogicalToDeviceY(rect.y),
                        rect.width,
                        rect.height );
#endif
    }

    if ((flags & wxCONTROL_CURRENT) && (flags & wxCONTROL_FOCUSED))
        DrawFocusRect(win, dc, rect, flags);
}

void wxRendererGTK::DrawFocusRect(wxWindow* win, wxDC& dc, const wxRect& rect, int flags)
{
    wxGTKDrawable* drawable = wxGetGTKDrawable(dc);
    if (drawable == nullptr)
        return;

    GtkStateType state;
    if (flags & wxCONTROL_SELECTED)
        state = GTK_STATE_SELECTED;
    else
        state = GTK_STATE_NORMAL;

#ifdef __WXGTK3__
    GtkStyleContext* sc = gtk_widget_get_style_context(win->m_widget);
    gtk_style_context_save(sc);
    gtk_style_context_set_state(sc, stateTypeToFlags[state]);
    gtk_render_focus(sc, drawable, rect.x, rect.y, rect.width, rect.height);
    gtk_style_context_restore(sc);
#else
    gtk_paint_focus( gtk_widget_get_style(win->m_widget),
                     drawable,
                     state,
                     NULL_RECT
                     win->m_wxwindow,
                     nullptr,
                     dc.LogicalToDeviceX(rect.x),
                     dc.LogicalToDeviceY(rect.y),
                     rect.width,
                     rect.height );
#endif
}

// Uses the theme to draw the border and fill for something like a wxTextCtrl
void wxRendererGTK::DrawTextCtrl(wxWindow*, wxDC& dc, const wxRect& rect, int flags)
{
    wxGTKDrawable* drawable = wxGetGTKDrawable(dc);
    if (drawable == nullptr)
        return;

#ifdef __WXGTK3__
    int state = GTK_STATE_FLAG_NORMAL;
    if (flags & wxCONTROL_FOCUSED)
        state = GTK_STATE_FLAG_FOCUSED;
    if (flags & wxCONTROL_DISABLED)
        state = GTK_STATE_FLAG_INSENSITIVE;

    wxGtkStyleContext sc(dc.GetContentScaleFactor());
    sc.Add(GTK_TYPE_ENTRY, "entry", "entry", nullptr);

    gtk_style_context_set_state(sc, GtkStateFlags(state));
    gtk_render_background(sc, drawable, rect.x, rect.y, rect.width, rect.height);
    gtk_render_frame(sc, drawable, rect.x, rect.y, rect.width, rect.height);
#else
    GtkWidget* entry = wxGTKPrivate::GetEntryWidget();

    GtkStateType state = GTK_STATE_NORMAL;
    if ( flags & wxCONTROL_DISABLED )
        state = GTK_STATE_INSENSITIVE;

    gtk_widget_set_can_focus(entry, (flags & wxCONTROL_CURRENT) != 0);

    gtk_paint_shadow
    (
        gtk_widget_get_style(entry),
        drawable,
        state,
        GTK_SHADOW_OUT,
        NULL_RECT
        entry,
        "entry",
        dc.LogicalToDeviceX(rect.x),
        dc.LogicalToDeviceY(rect.y),
        rect.width,
        rect.height
  );
#endif
}

// Draw the equivalent of a wxComboBox
void wxRendererGTK::DrawComboBox(wxWindow* win, wxDC& dc, const wxRect& rect, int flags)
{
    wxGTKDrawable* drawable = wxGetGTKDrawable(dc);
    if (drawable == nullptr)
        return;

    GtkWidget* combo = wxGTKPrivate::GetComboBoxWidget();

    GtkStateType state = GTK_STATE_NORMAL;
    if ( flags & wxCONTROL_DISABLED )
       state = GTK_STATE_INSENSITIVE;

    gtk_widget_set_can_focus(combo, (flags & wxCONTROL_CURRENT) != 0);

#ifdef __WXGTK3__
    GtkStyleContext* sc = gtk_widget_get_style_context(combo);
    gtk_style_context_save(sc);
    gtk_style_context_set_state(sc, stateTypeToFlags[state]);
    gtk_render_background(sc, drawable, rect.x, rect.y, rect.width, rect.height);
    gtk_render_frame(sc, drawable, rect.x, rect.y, rect.width, rect.height);
    gtk_style_context_restore(sc);
    wxRect r = rect;
    r.x += r.width - r.height;
    r.width = r.height;
    DrawComboBoxDropButton(win, dc, r, flags);
#else
    wxUnusedVar(win);
    gtk_paint_shadow
    (
        gtk_widget_get_style(combo),
        drawable,
        state,
        GTK_SHADOW_OUT,
        NULL_RECT
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
        gtk_widget_get_style(combo),
        drawable,
        state,
        GTK_SHADOW_OUT,
        NULL_RECT
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
        gtk_widget_get_style(combo),
        drawable,
        state,
        GTK_SHADOW_ETCHED_OUT,
        NULL_RECT
        combo,
        "vseparator",
        dc.LogicalToDeviceX(r.x),
        dc.LogicalToDeviceY(r.y+1),
        r.width,
        r.height-2
    );
#endif
}

void wxRendererGTK::DrawChoice(wxWindow* win, wxDC& dc,
                           const wxRect& rect, int flags)
{
    DrawComboBox( win, dc, rect, flags );
}


// Draw a themed radio button
void wxRendererGTK::DrawRadioBitmap(wxWindow*, wxDC& dc, const wxRect& rect, int flags)
{
    wxGTKDrawable* drawable = wxGetGTKDrawable(dc);
    if (drawable == nullptr)
        return;

#ifdef __WXGTK3__
    int state = GTK_STATE_FLAG_NORMAL;
    if (flags & wxCONTROL_CHECKED)
    {
        state = GTK_STATE_FLAG_ACTIVE;
        if (gtk_check_version(3,14,0) == nullptr)
            state = GTK_STATE_FLAG_CHECKED;
    }
    if (flags & wxCONTROL_DISABLED)
        state |= GTK_STATE_FLAG_INSENSITIVE;
    if (flags & wxCONTROL_UNDETERMINED)
        state |= GTK_STATE_FLAG_INCONSISTENT;
    if (flags & wxCONTROL_CURRENT)
        state |= GTK_STATE_FLAG_PRELIGHT;

    int min_width, min_height;
    wxGtkStyleContext sc(dc.GetContentScaleFactor());
    sc.Add(GTK_TYPE_RADIO_BUTTON, "radiobutton", nullptr);
    if (gtk_check_version(3,20,0) == nullptr)
    {
        sc.Add("radio");
        gtk_style_context_get(sc, GTK_STATE_FLAG_NORMAL,
            "min-width", &min_width, "min-height", &min_height, nullptr);
    }
    else
    {
        wxGtkValue value( G_TYPE_INT);
        gtk_style_context_get_style_property(sc, "indicator-size", value);
        min_width = g_value_get_int(value);
        min_height = min_width;
    }

    // need save/restore for GTK+ 3.6 & 3.8
    gtk_style_context_save(sc);
    gtk_style_context_set_state(sc, GtkStateFlags(state));
    const int x = rect.x + (rect.width - min_width) / 2;
    const int y = rect.y + (rect.height - min_height) / 2;
    gtk_render_background(sc, drawable, x, y, min_width, min_height);
    gtk_render_frame(sc, drawable, x, y, min_width, min_height);
    gtk_style_context_add_class(sc, "radio");
    gtk_render_option(sc, drawable, x, y, min_width, min_height);
    gtk_style_context_restore(sc);
#else
    GtkWidget* button = wxGTKPrivate::GetRadioButtonWidget();

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
        gtk_widget_get_style(button),
        drawable,
        state,
        shadow_type,
        NULL_RECT
        button,
        "radiobutton",
        dc.LogicalToDeviceX(rect.x),
        dc.LogicalToDeviceY(rect.y),
        rect.width, rect.height
    );
#endif
}
