/* ///////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/win_gtk.h
// Purpose:     native GTK+ widget for wxWindow
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////// */

#ifndef _WX_GTK_PIZZA_H_
#define _WX_GTK_PIZZA_H_

#include <gtk/gtk.h>

#ifdef __WXGTK30__
#define WX_PIZZA(obj) G_TYPE_CHECK_INSTANCE_CAST(obj, wxpizza_get_type(), wxPizza)
#define WX_IS_PIZZA(obj) G_TYPE_CHECK_INSTANCE_TYPE(obj, wxpizza_get_type())
#else
#define WX_PIZZA(obj) G_TYPE_CHECK_INSTANCE_CAST(obj, wxPizza::type(), wxPizza)
#define WX_IS_PIZZA(obj) G_TYPE_CHECK_INSTANCE_TYPE(obj, wxPizza::type())
#endif

struct WXDLLIMPEXP_CORE wxPizza
{
    // borders styles which can be used with wxPizza
    enum { BORDER_STYLES =
        wxBORDER_SIMPLE | wxBORDER_RAISED | wxBORDER_SUNKEN | wxBORDER_THEME };

    static GtkWidget* New(long windowStyle = 0);
#if defined(__WXGTK20__) && !defined(__WXGTK30__)
    static GType type();
#endif
    void move(GtkWidget* widget, int x, int y);
    void put(GtkWidget* widget, int x, int y);
    void scroll(int dx, int dy);
    void get_border_widths(int& x, int& y);

    GtkFixed m_fixed;
    int m_scroll_x;
    int m_scroll_y;
    int m_border_style;
    bool m_is_scrollable;
};

extern "C" {
GType wxpizza_get_type(void);
}

#endif // _WX_GTK_PIZZA_H_
