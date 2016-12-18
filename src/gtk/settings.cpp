/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/settings.cpp
// Purpose:
// Author:      Robert Roebling
// Modified by: Mart Raudsepp (GetMetric)
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/settings.h"

#ifndef WX_PRECOMP
    #include "wx/toplevel.h"
    #include "wx/module.h"
#endif

#include "wx/fontutil.h"
#include "wx/fontenum.h"

#include <gtk/gtk.h>
#include "wx/gtk/private/win_gtk.h"
#include "wx/gtk/private/gtk2-compat.h"

bool wxGetFrameExtents(GdkWindow* window, int* left, int* right, int* top, int* bottom);

// ----------------------------------------------------------------------------
// wxSystemSettings implementation
// ----------------------------------------------------------------------------

static wxFont gs_fontSystem;

#ifndef __WXGTK3__
static GtkWidget* gs_tlw_parent;

static GtkContainer* ContainerWidget()
{
    static GtkContainer* s_widget;
    if (s_widget == NULL)
    {
        s_widget = GTK_CONTAINER(gtk_fixed_new());
        g_object_add_weak_pointer(G_OBJECT(s_widget), (void**)&s_widget);
        gs_tlw_parent = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_container_add(GTK_CONTAINER(gs_tlw_parent), GTK_WIDGET(s_widget));
    }
    return s_widget;
}

extern "C" {
static void style_set(GtkWidget*, GtkStyle*, void*)
{
    gs_fontSystem = wxNullFont;
}
}

static GtkWidget* ButtonWidget()
{
    static GtkWidget* s_widget;
    if (s_widget == NULL)
    {
        s_widget = gtk_button_new();
        g_object_add_weak_pointer(G_OBJECT(s_widget), (void**)&s_widget);
        gtk_container_add(ContainerWidget(), s_widget);
        gtk_widget_ensure_style(s_widget);
        g_signal_connect(s_widget, "style_set", G_CALLBACK(style_set), NULL);
    }
    return s_widget;
}

static GtkWidget* ListWidget()
{
    static GtkWidget* s_widget;
    if (s_widget == NULL)
    {
        s_widget = gtk_tree_view_new_with_model(
            GTK_TREE_MODEL(gtk_list_store_new(1, G_TYPE_INT)));
        g_object_add_weak_pointer(G_OBJECT(s_widget), (void**)&s_widget);
        gtk_container_add(ContainerWidget(), s_widget);
        gtk_widget_ensure_style(s_widget);
    }
    return s_widget;
}

static GtkWidget* TextCtrlWidget()
{
    static GtkWidget* s_widget;
    if (s_widget == NULL)
    {
        s_widget = gtk_text_view_new();
        g_object_add_weak_pointer(G_OBJECT(s_widget), (void**)&s_widget);
        gtk_container_add(ContainerWidget(), s_widget);
        gtk_widget_ensure_style(s_widget);
    }
    return s_widget;
}

static GtkWidget* MenuItemWidget()
{
    static GtkWidget* s_widget;
    if (s_widget == NULL)
    {
        s_widget = gtk_menu_item_new();
        g_object_add_weak_pointer(G_OBJECT(s_widget), (void**)&s_widget);
        gtk_container_add(ContainerWidget(), s_widget);
        gtk_widget_ensure_style(s_widget);
    }
    return s_widget;
}

static GtkWidget* MenuBarWidget()
{
    static GtkWidget* s_widget;
    if (s_widget == NULL)
    {
        s_widget = gtk_menu_bar_new();
        g_object_add_weak_pointer(G_OBJECT(s_widget), (void**)&s_widget);
        gtk_container_add(ContainerWidget(), s_widget);
        gtk_widget_ensure_style(s_widget);
    }
    return s_widget;
}

static GtkWidget* ToolTipWidget()
{
    static GtkWidget* s_widget;
    if (s_widget == NULL)
    {
        s_widget = gtk_window_new(GTK_WINDOW_POPUP);
        g_object_add_weak_pointer(G_OBJECT(s_widget), (void**)&s_widget);
        g_signal_connect_swapped(ContainerWidget(), "destroy",
            G_CALLBACK(gtk_widget_destroy), s_widget);
        const char* name = "gtk-tooltip";
        if (gtk_check_version(2, 11, 0))
            name = "gtk-tooltips";
        gtk_widget_set_name(s_widget, name);
        gtk_widget_ensure_style(s_widget);
    }
    return s_widget;
}
#endif // !__WXGTK3__

#ifdef __WXGTK3__

#if !GTK_CHECK_VERSION(3,12,0)
    #define GTK_STATE_FLAG_LINK (1 << 9)
#endif

static wxColour gs_systemColorCache[wxSYS_COLOUR_MAX + 1];

extern "C" {
static void notify_gtk_theme_name(GObject*, GParamSpec*, void*)
{
    gs_fontSystem.UnRef();
    for (int i = wxSYS_COLOUR_MAX; i--;)
        gs_systemColorCache[i].UnRef();
}

static void notify_gtk_font_name(GObject*, GParamSpec*, void*)
{
    gs_fontSystem.UnRef();
}
}

// Some notes on using GtkStyleContext. Style information from a context
// attached to a non-visible GtkWidget is not accurate. The context has an
// internal visibility state, controlled by the widget, which it presumably
// uses to avoid doing unnecessary work. Creating a new style context from the
// GtkWidgetPath in a context attached to a widget also does not work. The path
// does not accurately reproduce the context state with older versions of GTK+,
// and there is no context hierarchy (parent contexts). The hierarchy of parent
// contexts is necessary, even though it would seem that the widget path has
// the same hierarchy in it. So the best way to get style information seems
// to be creating the widget paths and context hierarchy directly.

static GtkStyleContext* StyleContext(
    GtkStyleContext* parent,
    GtkWidgetPath* path,
    GType type,
    const char* objectName,
    const char* className1 = NULL,
    const char* className2 = NULL)
{
    gtk_widget_path_append_type(path, type);
#if GTK_CHECK_VERSION(3,20,0)
    if (gtk_check_version(3,20,0) == NULL)
        gtk_widget_path_iter_set_object_name(path, -1, objectName);
#endif
    if (className1)
        gtk_widget_path_iter_add_class(path, -1, className1);
    if (className2)
        gtk_widget_path_iter_add_class(path, -1, className2);
    GtkStyleContext* sc = gtk_style_context_new();
    gtk_style_context_set_path(sc, path);
    if (parent)
    {
#if GTK_CHECK_VERSION(3,4,0)
        if (gtk_check_version(3,4,0) == NULL)
            gtk_style_context_set_parent(sc, parent);
#endif
        g_object_unref(parent);
    }
    return sc;
}

static GtkStyleContext* StyleContext(
    GtkWidgetPath* path,
    GType type,
    const char* objectName,
    const char* className1 = NULL,
    const char* className2 = NULL)
{
    GtkStyleContext* sc;
    sc = StyleContext(NULL, path, GTK_TYPE_WINDOW, "window", "background");
    sc = StyleContext(sc, path, type, objectName, className1, className2);
    return sc;
}

static void StyleContextFree(GtkStyleContext* sc)
{
    if (gtk_check_version(3,16,0) == NULL || gtk_check_version(3,4,0))
    {
        g_object_unref(sc);
        return;
    }
#if GTK_CHECK_VERSION(3,4,0)
    // GTK+ < 3.16 does not properly handle freeing child context before parent
    do {
        GtkStyleContext* parent = gtk_style_context_get_parent(sc);
        if (parent)
        {
            g_object_ref(parent);
            gtk_style_context_set_parent(sc, NULL);
        }
        g_object_unref(sc);
        sc = parent;
    } while (sc);
#endif
}

static GtkStyleContext* ButtonContext(GtkWidgetPath* path)
{
    GtkStyleContext* sc;
    sc = StyleContext(path, GTK_TYPE_BUTTON, "button", "button");
    return sc;
}

static GtkStyleContext* ButtonLabelContext(GtkWidgetPath* path)
{
    GtkStyleContext* sc;
    sc = ButtonContext(path);
    sc = StyleContext(sc, path, GTK_TYPE_LABEL, "label");
    return sc;
}

static GtkStyleContext* HeaderbarContext(GtkWidgetPath* path)
{
    GtkStyleContext* sc;
    sc = StyleContext(path, GTK_TYPE_HEADER_BAR, "headerbar", "titlebar", "header-bar");
    return sc;
}

static GtkStyleContext* HeaderbarLabelContext(GtkWidgetPath* path)
{
    GtkStyleContext* sc;
    sc = HeaderbarContext(path);
    sc = StyleContext(sc, path, GTK_TYPE_LABEL, "label");
    return sc;
}

static GtkStyleContext* MenuContext(GtkWidgetPath* path)
{
    GtkStyleContext* sc;
    sc = StyleContext(NULL, path, GTK_TYPE_WINDOW, "window", "background", "popup");
    sc = StyleContext(sc, path, GTK_TYPE_MENU, "menu", "menu");
    return sc;
}

static GtkStyleContext* MenuItemContext(GtkWidgetPath* path)
{
    GtkStyleContext* sc;
    sc = MenuContext(path);
    sc = StyleContext(sc, path, GTK_TYPE_MENU_ITEM, "menuitem", "menuitem");
    return sc;
}

static GtkStyleContext* TextviewContext(GtkWidgetPath* path, const char* child1 = NULL, const char* child2 = NULL)
{
    GtkStyleContext* sc;
    sc = StyleContext(path, GTK_TYPE_TEXT_VIEW, "textview", "view");
    if (child1 && gtk_check_version(3,20,0) == NULL)
    {
        sc = StyleContext(sc, path, G_TYPE_NONE, child1);
        if (child2)
            sc = StyleContext(sc, path, G_TYPE_NONE, child2);
    }
    return sc;
}

static GtkStyleContext* TreeviewContext(GtkWidgetPath* path)
{
    GtkStyleContext* sc;
    sc = StyleContext(path, GTK_TYPE_TREE_VIEW, "treeview", "view");
    return sc;
}

static GtkStyleContext* TooltipContext(GtkWidgetPath* path)
{
    gtk_widget_path_append_type(path, GTK_TYPE_WINDOW);
#if GTK_CHECK_VERSION(3,20,0)
    if (gtk_check_version(3,20,0) == NULL)
        gtk_widget_path_iter_set_object_name(path, -1, "tooltip");
#endif
    gtk_widget_path_iter_add_class(path, -1, "background");
    gtk_widget_path_iter_add_class(path, -1, "tooltip");
    gtk_widget_path_iter_set_name(path, -1, "gtk-tooltip");
    GtkStyleContext* sc = gtk_style_context_new();
    gtk_style_context_set_path(sc, path);
    return sc;
}

static void bg(GtkStyleContext* sc, wxColour& color, int state = GTK_STATE_FLAG_NORMAL)
{
    GdkRGBA* rgba;
    cairo_pattern_t* pattern = NULL;
    gtk_style_context_set_state(sc, GtkStateFlags(state));
    gtk_style_context_get(sc, GtkStateFlags(state),
        "background-color", &rgba, "background-image", &pattern, NULL);
    color = wxColour(*rgba);
    gdk_rgba_free(rgba);

    // "background-image" takes precedence over "background-color".
    // If there is an image, try to get a color out of it.
    if (pattern)
    {
        if (cairo_pattern_get_type(pattern) == CAIRO_PATTERN_TYPE_SURFACE)
        {
            cairo_surface_t* surf;
            cairo_pattern_get_surface(pattern, &surf);
            if (cairo_surface_get_type(surf) == CAIRO_SURFACE_TYPE_IMAGE)
            {
                const guchar* data = cairo_image_surface_get_data(surf);
                const int stride = cairo_image_surface_get_stride(surf);
                // choose a pixel in the middle vertically,
                // images often have a vertical gradient
                const int i = stride * (cairo_image_surface_get_height(surf) / 2);
                const unsigned* p = reinterpret_cast<const unsigned*>(data + i);
                const unsigned pixel = *p;
                guchar r, g, b, a = 0xff;
                switch (cairo_image_surface_get_format(surf))
                {
                case CAIRO_FORMAT_ARGB32:
                    a = guchar(pixel >> 24);
                    // fallthrough
                case CAIRO_FORMAT_RGB24:
                    r = guchar(pixel >> 16);
                    g = guchar(pixel >> 8);
                    b = guchar(pixel);
                    break;
                default:
                    a = 0;
                    break;
                }
                if (a != 0)
                {
                    if (a != 0xff)
                    {
                        // un-premultiply
                        r = guchar((r * 0xff) / a);
                        g = guchar((g * 0xff) / a);
                        b = guchar((b * 0xff) / a);
                    }
                    color.Set(r, g, b, a);
                }
            }
        }
        cairo_pattern_destroy(pattern);
    }

    if (color.Alpha() == 0)
    {
        // Try TLW as last resort, but not if we're already doing it
        const GtkWidgetPath* path0 = gtk_style_context_get_path(sc);
        if (gtk_widget_path_length(path0) > 1 ||
            gtk_widget_path_iter_get_object_type(path0, 0) != GTK_TYPE_WINDOW)
        {
            GtkWidgetPath* path = gtk_widget_path_new();
            GtkStyleContext* sc2;
            sc2 = StyleContext(NULL, path, GTK_TYPE_WINDOW, "window", "background");
            gtk_widget_path_unref(path);
            bg(sc2, color, state);
        }
    }

    StyleContextFree(sc);
}

static void fg(GtkStyleContext* sc, wxColour& color, int state = GTK_STATE_FLAG_NORMAL)
{
    GdkRGBA rgba;
    gtk_style_context_set_state(sc, GtkStateFlags(state));
    gtk_style_context_get_color(sc, GtkStateFlags(state), &rgba);
    color = wxColour(rgba);
    StyleContextFree(sc);
}

static void border(GtkStyleContext* sc, wxColour& color)
{
    GdkRGBA* rgba;
    gtk_style_context_get(sc, GTK_STATE_FLAG_NORMAL, "border-color", &rgba, NULL);
    color = wxColour(*rgba);
    gdk_rgba_free(rgba);
    StyleContextFree(sc);
}

wxColour wxSystemSettingsNative::GetColour(wxSystemColour index)
{
    if (unsigned(index) > wxSYS_COLOUR_MAX)
        index = wxSYS_COLOUR_MAX;

    wxColour& color = gs_systemColorCache[index];
    if (color.IsOk())
        return color;

    static bool once;
    if (!once)
    {
        once = true;
        g_signal_connect(gtk_settings_get_default(), "notify::gtk-theme-name",
            G_CALLBACK(notify_gtk_theme_name), NULL);
    }

    GtkWidgetPath* path = gtk_widget_path_new();
    GtkStyleContext* sc;

    switch (index)
    {
    case wxSYS_COLOUR_ACTIVECAPTION:
    case wxSYS_COLOUR_INACTIVECAPTION:
#if GTK_CHECK_VERSION(3,10,0)
        if (gtk_check_version(3,10,0) == NULL)
        {
            sc = HeaderbarContext(path);
            int state = GTK_STATE_FLAG_NORMAL;
            if (index == wxSYS_COLOUR_INACTIVECAPTION)
                state = GTK_STATE_FLAG_BACKDROP;
            bg(sc, color, state);
            break;
        }
#endif
        // fall through
    case wxSYS_COLOUR_3DLIGHT:
    case wxSYS_COLOUR_ACTIVEBORDER:
    case wxSYS_COLOUR_BTNFACE:
    case wxSYS_COLOUR_DESKTOP:
    case wxSYS_COLOUR_INACTIVEBORDER:
    case wxSYS_COLOUR_SCROLLBAR:
    case wxSYS_COLOUR_WINDOWFRAME:
        sc = ButtonContext(path);
        bg(sc, color);
        break;
    case wxSYS_COLOUR_HIGHLIGHT:
        sc = TextviewContext(path, "text", "selection");
        bg(sc, color, GTK_STATE_FLAG_SELECTED | GTK_STATE_FLAG_FOCUSED);
        break;
    case wxSYS_COLOUR_HIGHLIGHTTEXT:
        sc = TextviewContext(path, "text", "selection");
        fg(sc, color, GTK_STATE_FLAG_SELECTED | GTK_STATE_FLAG_FOCUSED);
        break;
    case wxSYS_COLOUR_WINDOWTEXT:
        sc = TextviewContext(path, "text");
        fg(sc, color);
        break;
    case wxSYS_COLOUR_BTNHIGHLIGHT:
        sc = ButtonContext(path);
        bg(sc, color, GTK_STATE_FLAG_PRELIGHT);
        break;
    case wxSYS_COLOUR_BTNSHADOW:
        sc = ButtonContext(path);
        border(sc, color);
        break;
    case wxSYS_COLOUR_CAPTIONTEXT:
#if GTK_CHECK_VERSION(3,10,0)
        if (gtk_check_version(3,10,0) == NULL)
        {
            sc = HeaderbarLabelContext(path);
            fg(sc, color);
            break;
        }
#endif
        // fall through
    case wxSYS_COLOUR_BTNTEXT:
        sc = ButtonLabelContext(path);
        fg(sc, color);
        break;
    case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
#if GTK_CHECK_VERSION(3,10,0)
        if (gtk_check_version(3,10,0) == NULL)
        {
            sc = HeaderbarLabelContext(path);
            fg(sc, color, GTK_STATE_FLAG_BACKDROP);
            break;
        }
#endif
        // fall through
    case wxSYS_COLOUR_GRAYTEXT:
        sc = StyleContext(path, GTK_TYPE_LABEL, "label");
        fg(sc, color, GTK_STATE_FLAG_INSENSITIVE);
        break;
    case wxSYS_COLOUR_HOTLIGHT:
        sc = StyleContext(path, GTK_TYPE_LINK_BUTTON, "button", "link");
        if (gtk_check_version(3,12,0) == NULL)
            fg(sc, color, GTK_STATE_FLAG_LINK);
        else
        {
            wxGCC_WARNING_SUPPRESS(deprecated-declarations)
            GValue value = G_VALUE_INIT;
            g_value_init(&value, GDK_TYPE_COLOR);
            gtk_style_context_get_style_property(sc, "link-color", &value);
            GdkColor* link_color = static_cast<GdkColor*>(g_value_get_boxed(&value));
            GdkColor gdkColor = { 0, 0, 0, 0xeeee };
            if (link_color)
                gdkColor = *link_color;
            color = wxColour(gdkColor);
            g_value_unset(&value);
            StyleContextFree(sc);
            wxGCC_WARNING_RESTORE()
        }
        break;
    case wxSYS_COLOUR_INFOBK:
        sc = TooltipContext(path);
        bg(sc, color);
        break;
    case wxSYS_COLOUR_INFOTEXT:
        sc = TooltipContext(path);
        sc = StyleContext(sc, path, GTK_TYPE_LABEL, "label");
        fg(sc, color);
        break;
    case wxSYS_COLOUR_LISTBOX:
        sc = TreeviewContext(path);
        bg(sc, color);
        break;
    case wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT:
        sc = TreeviewContext(path);
        fg(sc, color, GTK_STATE_FLAG_SELECTED | GTK_STATE_FLAG_FOCUSED);
        break;
    case wxSYS_COLOUR_LISTBOXTEXT:
        sc = TreeviewContext(path);
        fg(sc, color);
        break;
    case wxSYS_COLOUR_MENU:
        sc = MenuContext(path);
        bg(sc, color);
        break;
    case wxSYS_COLOUR_MENUBAR:
        sc = StyleContext(path, GTK_TYPE_MENU_BAR, "menubar", "menubar");
        bg(sc, color);
        break;
    case wxSYS_COLOUR_MENUHILIGHT:
        sc = MenuItemContext(path);
        bg(sc, color, GTK_STATE_FLAG_PRELIGHT);
        break;
    case wxSYS_COLOUR_MENUTEXT:
        sc = MenuItemContext(path);
        sc = StyleContext(sc, path, GTK_TYPE_LABEL, "label");
        fg(sc, color);
        break;
    case wxSYS_COLOUR_APPWORKSPACE:
    case wxSYS_COLOUR_WINDOW:
        sc = TextviewContext(path);
        bg(sc, color);
        break;
    case wxSYS_COLOUR_3DDKSHADOW:
    case wxSYS_COLOUR_GRADIENTACTIVECAPTION:
    case wxSYS_COLOUR_GRADIENTINACTIVECAPTION:
        color.Set(0, 0, 0);
        break;
    default:
        wxFAIL_MSG("invalid system colour index");
        color.Set(0, 0, 0, 0);
        break;
    }

    gtk_widget_path_unref(path);

    return color;
}
#else // !__WXGTK3__
static const GtkStyle* ButtonStyle()
{
    return gtk_widget_get_style(ButtonWidget());
}

static const GtkStyle* ListStyle()
{
    return gtk_widget_get_style(ListWidget());
}

static const GtkStyle* TextCtrlStyle()
{
    return gtk_widget_get_style(TextCtrlWidget());
}

static const GtkStyle* MenuItemStyle()
{
    return gtk_widget_get_style(MenuItemWidget());
}

static const GtkStyle* MenuBarStyle()
{
    return gtk_widget_get_style(MenuBarWidget());
}

static const GtkStyle* ToolTipStyle()
{
    return gtk_widget_get_style(ToolTipWidget());
}

wxColour wxSystemSettingsNative::GetColour( wxSystemColour index )
{
    wxColor color;
    switch (index)
    {
        case wxSYS_COLOUR_SCROLLBAR:
        case wxSYS_COLOUR_BACKGROUND:
        //case wxSYS_COLOUR_DESKTOP:
        case wxSYS_COLOUR_INACTIVECAPTION:
        case wxSYS_COLOUR_MENU:
        case wxSYS_COLOUR_WINDOWFRAME:
        case wxSYS_COLOUR_ACTIVEBORDER:
        case wxSYS_COLOUR_INACTIVEBORDER:
        case wxSYS_COLOUR_BTNFACE:
        //case wxSYS_COLOUR_3DFACE:
        case wxSYS_COLOUR_3DLIGHT:
            color = wxColor(ButtonStyle()->bg[GTK_STATE_NORMAL]);
            break;

        case wxSYS_COLOUR_WINDOW:
            color = wxColor(TextCtrlStyle()->base[GTK_STATE_NORMAL]);
            break;

        case wxSYS_COLOUR_MENUBAR:
            color = wxColor(MenuBarStyle()->bg[GTK_STATE_NORMAL]);
            break;

        case wxSYS_COLOUR_3DDKSHADOW:
            color = *wxBLACK;
            break;

        case wxSYS_COLOUR_GRAYTEXT:
        case wxSYS_COLOUR_BTNSHADOW:
        //case wxSYS_COLOUR_3DSHADOW:
            {
                wxColour faceColour(GetColour(wxSYS_COLOUR_3DFACE));
                color =
                   wxColour((unsigned char) (faceColour.Red() * 2 / 3),
                            (unsigned char) (faceColour.Green() * 2 / 3),
                            (unsigned char) (faceColour.Blue() * 2 / 3));
            }
            break;

        case wxSYS_COLOUR_BTNHIGHLIGHT:
        //case wxSYS_COLOUR_BTNHILIGHT:
        //case wxSYS_COLOUR_3DHIGHLIGHT:
        //case wxSYS_COLOUR_3DHILIGHT:
            color = *wxWHITE;
            break;

        case wxSYS_COLOUR_HIGHLIGHT:
            color = wxColor(ButtonStyle()->bg[GTK_STATE_SELECTED]);
            break;

        case wxSYS_COLOUR_LISTBOX:
            color = wxColor(ListStyle()->base[GTK_STATE_NORMAL]);
            break;

        case wxSYS_COLOUR_LISTBOXTEXT:
            color = wxColor(ListStyle()->text[GTK_STATE_NORMAL]);
            break;

        case wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT:
            // This is for the text in a list control (or tree) when the
            // item is selected, but not focused
            color = wxColor(ListStyle()->text[GTK_STATE_ACTIVE]);
            break;

        case wxSYS_COLOUR_MENUTEXT:
        case wxSYS_COLOUR_WINDOWTEXT:
        case wxSYS_COLOUR_CAPTIONTEXT:
        case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
        case wxSYS_COLOUR_BTNTEXT:
            color = wxColor(ButtonStyle()->fg[GTK_STATE_NORMAL]);
            break;

        case wxSYS_COLOUR_INFOBK:
            color = wxColor(ToolTipStyle()->bg[GTK_STATE_NORMAL]);
            break;

        case wxSYS_COLOUR_INFOTEXT:
            color = wxColor(ToolTipStyle()->fg[GTK_STATE_NORMAL]);
            break;

        case wxSYS_COLOUR_HIGHLIGHTTEXT:
            color = wxColor(ButtonStyle()->fg[GTK_STATE_SELECTED]);
            break;

        case wxSYS_COLOUR_APPWORKSPACE:
            color = *wxWHITE;    // ?
            break;

        case wxSYS_COLOUR_ACTIVECAPTION:
        case wxSYS_COLOUR_MENUHILIGHT:
            color = wxColor(MenuItemStyle()->bg[GTK_STATE_SELECTED]);
            break;

        case wxSYS_COLOUR_HOTLIGHT:
        case wxSYS_COLOUR_GRADIENTACTIVECAPTION:
        case wxSYS_COLOUR_GRADIENTINACTIVECAPTION:
            // TODO
            color = *wxBLACK;
            break;

        case wxSYS_COLOUR_MAX:
        default:
            wxFAIL_MSG( wxT("unknown system colour index") );
            color = *wxWHITE;
            break;
    }

    wxASSERT(color.IsOk());
    return color;
}
#endif // !__WXGTK3__

wxFont wxSystemSettingsNative::GetFont( wxSystemFont index )
{
    wxFont font;
    switch (index)
    {
        case wxSYS_OEM_FIXED_FONT:
        case wxSYS_ANSI_FIXED_FONT:
        case wxSYS_SYSTEM_FIXED_FONT:
            font = *wxNORMAL_FONT;
            break;

        case wxSYS_ANSI_VAR_FONT:
        case wxSYS_SYSTEM_FONT:
        case wxSYS_DEVICE_DEFAULT_FONT:
        case wxSYS_DEFAULT_GUI_FONT:
            if (!gs_fontSystem.IsOk())
            {
                wxNativeFontInfo info;
#ifdef __WXGTK3__
                static bool once;
                if (!once)
                {
                    once = true;
                    g_signal_connect(gtk_settings_get_default(), "notify::gtk-font-name",
                        G_CALLBACK(notify_gtk_font_name), NULL);
                }
                GtkWidgetPath* path = gtk_widget_path_new();
                GtkStyleContext* sc;
                sc = ButtonLabelContext(path);
                gtk_style_context_get(sc, GTK_STATE_FLAG_NORMAL,
                    GTK_STYLE_PROPERTY_FONT, &info.description, NULL);
                gtk_widget_path_unref(path);
                StyleContextFree(sc);
#else
                info.description = ButtonStyle()->font_desc;
#endif
                gs_fontSystem = wxFont(info);

#if wxUSE_FONTENUM
                // (try to) heal the default font (on some common systems e.g. Ubuntu
                // it's "Sans Serif" but the real font is called "Sans"):
                if (!wxFontEnumerator::IsValidFacename(gs_fontSystem.GetFaceName()) &&
                    gs_fontSystem.GetFaceName() == "Sans Serif")
                {
                    gs_fontSystem.SetFaceName("Sans");
                }
#endif // wxUSE_FONTENUM

#ifndef __WXGTK3__
                info.description = NULL;
#endif
            }
            font = gs_fontSystem;
            break;

        default:
            break;
    }

    wxASSERT( font.IsOk() );

    return font;
}

// helper: return the GtkSettings either for the screen the current window is
// on or for the default screen if window is NULL
static GtkSettings *GetSettingsForWindowScreen(GdkWindow *window)
{
    return window ? gtk_settings_get_for_screen(gdk_window_get_screen(window))
                  : gtk_settings_get_default();
}

static int GetBorderWidth(wxSystemMetric index, wxWindow* win)
{
    if (win->m_wxwindow)
    {
        wxPizza* pizza = WX_PIZZA(win->m_wxwindow);
        GtkBorder border;
        pizza->get_border(border);
        switch (index)
        {
            case wxSYS_BORDER_X:
            case wxSYS_EDGE_X:
            case wxSYS_FRAMESIZE_X:
                return border.left;
            default:
                return border.top;
        }
    }
    return -1;
}

int wxSystemSettingsNative::GetMetric( wxSystemMetric index, wxWindow* win )
{
    GdkWindow *window = NULL;
    if (win)
        window = gtk_widget_get_window(win->GetHandle());

    switch (index)
    {
        case wxSYS_BORDER_X:
        case wxSYS_BORDER_Y:
        case wxSYS_EDGE_X:
        case wxSYS_EDGE_Y:
        case wxSYS_FRAMESIZE_X:
        case wxSYS_FRAMESIZE_Y:
            if (win)
            {
                wxTopLevelWindow *tlw = wxDynamicCast(win, wxTopLevelWindow);
                if (!tlw)
                    return GetBorderWidth(index, win);
                else if (window)
                {
                    // Get the frame extents from the windowmanager.
                    // In most cases the top extent is the titlebar, so we use the bottom extent
                    // for the heights.
                    int right, bottom;
                    if (wxGetFrameExtents(window, NULL, &right, NULL, &bottom))
                    {
                        switch (index)
                        {
                            case wxSYS_BORDER_X:
                            case wxSYS_EDGE_X:
                            case wxSYS_FRAMESIZE_X:
                                return right; // width of right extent
                            default:
                                return bottom; // height of bottom extent
                        }
                    }
                }
            }

            return -1; // no window specified

        case wxSYS_CURSOR_X:
        case wxSYS_CURSOR_Y:
                return gdk_display_get_default_cursor_size(
                            window ? gdk_window_get_display(window)
                                   : gdk_display_get_default());

        case wxSYS_DCLICK_X:
        case wxSYS_DCLICK_Y:
            gint dclick_distance;
            g_object_get(GetSettingsForWindowScreen(window),
                            "gtk-double-click-distance", &dclick_distance, NULL);

            return dclick_distance * 2;

        case wxSYS_DCLICK_MSEC:
            gint dclick;
            g_object_get(GetSettingsForWindowScreen(window),
                            "gtk-double-click-time", &dclick, NULL);
            return dclick;

        case wxSYS_DRAG_X:
        case wxSYS_DRAG_Y:
            gint drag_threshold;
            g_object_get(GetSettingsForWindowScreen(window),
                            "gtk-dnd-drag-threshold", &drag_threshold, NULL);

            // The correct thing here would be to double the value
            // since that is what the API wants. But the values
            // are much bigger under GNOME than under Windows and
            // just seem to much in many cases to be useful.
            // drag_threshold *= 2;

            return drag_threshold;

        case wxSYS_ICON_X:
        case wxSYS_ICON_Y:
            return 32;

        case wxSYS_SCREEN_X:
            if (window)
                return gdk_screen_get_width(gdk_window_get_screen(window));
            else
                return gdk_screen_width();

        case wxSYS_SCREEN_Y:
            if (window)
                return gdk_screen_get_height(gdk_window_get_screen(window));
            else
                return gdk_screen_height();

        case wxSYS_HSCROLL_Y:
        case wxSYS_VSCROLL_X:
            return 15;

        case wxSYS_CAPTION_Y:
            if (!window)
                // No realized window specified, and no implementation for that case yet.
                return -1;

            wxASSERT_MSG( wxDynamicCast(win, wxTopLevelWindow),
                          wxT("Asking for caption height of a non toplevel window") );

            // Get the height of the top windowmanager border.
            // This is the titlebar in most cases. The titlebar might be elsewhere, and
            // we could check which is the thickest wm border to decide on which side the
            // titlebar is, but this might lead to interesting behaviours in used code.
            // Reconsider when we have a way to report to the user on which side it is.
            {
                int top;
                if (wxGetFrameExtents(window, NULL, NULL, &top, NULL))
                {
                    return top; // top frame extent
                }
            }

            // Try a default approach without a window pointer, if possible
            // ...

            return -1;

        case wxSYS_PENWINDOWS_PRESENT:
            // No MS Windows for Pen computing extension available in X11 based gtk+.
            return 0;

        default:
            return -1;   // metric is unknown
    }
}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature index)
{
    switch (index)
    {
        case wxSYS_CAN_ICONIZE_FRAME:
            return false;

        case wxSYS_CAN_DRAW_FRAME_DECORATIONS:
            return true;

        default:
            return false;
    }
}

class wxSystemSettingsModule: public wxModule
{
public:
    virtual bool OnInit() { return true; }
    virtual void OnExit();
    wxDECLARE_DYNAMIC_CLASS(wxSystemSettingsModule);
};
wxIMPLEMENT_DYNAMIC_CLASS(wxSystemSettingsModule, wxModule);

void wxSystemSettingsModule::OnExit()
{
#ifdef __WXGTK3__
    GtkSettings* settings = gtk_settings_get_default();
    g_signal_handlers_disconnect_by_func(settings,
        (void*)notify_gtk_theme_name, NULL);
    g_signal_handlers_disconnect_by_func(settings,
        (void*)notify_gtk_font_name, NULL);
#else
    if (gs_tlw_parent)
    {
        gtk_widget_destroy(gs_tlw_parent);
        gs_tlw_parent = NULL;
    }
#endif
}
