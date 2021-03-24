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

#include "wx/gtk/private/wrapgtk.h"
#include "wx/gtk/private/gtk3-compat.h"
#include "wx/gtk/private/win_gtk.h"
#include "wx/gtk/private/stylecontext.h"
#include "wx/gtk/private/value.h"

bool wxGetFrameExtents(GdkWindow* window, int* left, int* right, int* top, int* bottom);

// ----------------------------------------------------------------------------
// wxSystemSettings implementation
// ----------------------------------------------------------------------------

static wxFont gs_fontSystem;
static int gs_scrollWidth;
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

static GtkWidget* ScrollBarWidget()
{
    static GtkWidget* s_widget;
    if (s_widget == NULL)
    {
        s_widget = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, NULL);
        g_object_add_weak_pointer(G_OBJECT(s_widget), (void**)&s_widget);
        gtk_container_add(ContainerWidget(), s_widget);
#ifndef __WXGTK3__
        gtk_widget_ensure_style(s_widget);
#endif
    }
    return s_widget;
}

#ifndef __WXGTK3__

extern "C" {
static void style_set(GtkWidget*, GtkStyle*, void*)
{
    gs_fontSystem = wxNullFont;
    gs_scrollWidth = 0;
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
        if (!wx_is_at_least_gtk2(11))
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
    gs_scrollWidth = 0;
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

//-----------------------------------------------------------------------------

class wxGtkWidgetPath
{
public:
    wxGtkWidgetPath() : m_path(gtk_widget_path_new()) { }
    ~wxGtkWidgetPath() { gtk_widget_path_free(m_path); }
    operator GtkWidgetPath*() { return m_path; }
private:
    GtkWidgetPath* const m_path;
};

//-----------------------------------------------------------------------------
// wxGtkStyleContext
//-----------------------------------------------------------------------------

wxGtkStyleContext::wxGtkStyleContext(double scale)
    : m_path(gtk_widget_path_new())
    , m_scale(int(scale))
{
    m_context = NULL;
}

wxGtkStyleContext& wxGtkStyleContext::Add(GType type, const char* objectName, ...)
{
    if (m_context == NULL && type != GTK_TYPE_WINDOW)
        AddWindow();

    gtk_widget_path_append_type(m_path, type);
#if GTK_CHECK_VERSION(3,20,0)
    if (gtk_check_version(3,20,0) == NULL)
        gtk_widget_path_iter_set_object_name(m_path, -1, objectName);
#endif
    va_list args;
    va_start(args, objectName);
    const char* className;
    while ((className = va_arg(args, char*)))
        gtk_widget_path_iter_add_class(m_path, -1, className);
    va_end(args);

    GtkStyleContext* sc = gtk_style_context_new();
#if GTK_CHECK_VERSION(3,10,0)
    if (gtk_check_version(3,10,0) == NULL)
        gtk_style_context_set_scale(sc, m_scale);
#endif
    gtk_style_context_set_path(sc, m_path);
    if (m_context)
    {
#if GTK_CHECK_VERSION(3,4,0)
        if (gtk_check_version(3,4,0) == NULL)
            gtk_style_context_set_parent(sc, m_context);
#endif
        g_object_unref(m_context);
    }
    m_context = sc;
    return *this;
}

wxGtkStyleContext& wxGtkStyleContext::Add(const char* objectName)
{
    return Add(G_TYPE_NONE, objectName, NULL);
}

wxGtkStyleContext::~wxGtkStyleContext()
{
    gtk_widget_path_free(m_path);
    if (m_context == NULL)
        return;
    if (gtk_check_version(3,16,0) == NULL || gtk_check_version(3,4,0))
    {
        g_object_unref(m_context);
        return;
    }
#if GTK_CHECK_VERSION(3,4,0)
    // GTK+ < 3.16 does not properly handle freeing child context before parent
    GtkStyleContext* sc = m_context;
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

wxGtkStyleContext& wxGtkStyleContext::AddButton()
{
    return Add(GTK_TYPE_BUTTON, "button", "button", NULL);
}

wxGtkStyleContext& wxGtkStyleContext::AddCheckButton()
{
    return Add(GTK_TYPE_CHECK_BUTTON, "checkbutton", NULL);
}

#if GTK_CHECK_VERSION(3,10,0)
wxGtkStyleContext& wxGtkStyleContext::AddHeaderbar()
{
    return Add(GTK_TYPE_HEADER_BAR, "headerbar", "titlebar", "header-bar", NULL);
}
#endif

wxGtkStyleContext& wxGtkStyleContext::AddLabel()
{
    return Add(GTK_TYPE_LABEL, "label", NULL);
}

wxGtkStyleContext& wxGtkStyleContext::AddMenu()
{
    return AddWindow("popup").Add(GTK_TYPE_MENU, "menu", "menu", NULL);
}

wxGtkStyleContext& wxGtkStyleContext::AddMenuItem()
{
    return AddMenu().Add(GTK_TYPE_MENU_ITEM, "menuitem", "menuitem", NULL);
}

wxGtkStyleContext& wxGtkStyleContext::AddTextview(const char* child1, const char* child2)
{
    Add(GTK_TYPE_TEXT_VIEW, "textview", "view", NULL);
    if (child1 && gtk_check_version(3,20,0) == NULL)
    {
        Add(child1);
        if (child2)
            Add(child2);
    }
    return *this;
}

wxGtkStyleContext& wxGtkStyleContext::AddTreeview()
{
    return Add(GTK_TYPE_TREE_VIEW, "treeview", "view", NULL);
}

#if GTK_CHECK_VERSION(3,20,0)
wxGtkStyleContext& wxGtkStyleContext::AddTreeviewHeaderButton(int pos)
{
    AddTreeview().Add("header");
    GtkStyleContext* sc = gtk_style_context_new();

    wxGtkWidgetPath siblings;
    gtk_widget_path_append_type(siblings, GTK_TYPE_BUTTON);
    gtk_widget_path_iter_set_object_name(siblings, -1, "button");
    gtk_widget_path_append_type(siblings, GTK_TYPE_BUTTON);
    gtk_widget_path_iter_set_object_name(siblings, -1, "button");
    gtk_widget_path_append_type(siblings, GTK_TYPE_BUTTON);
    gtk_widget_path_iter_set_object_name(siblings, -1, "button");

    gtk_widget_path_append_with_siblings(m_path, siblings, pos);

    gtk_style_context_set_path(sc, m_path);
    gtk_style_context_set_parent(sc, m_context);
    g_object_unref(m_context);
    m_context = sc;
    return *this;
}
#endif // GTK_CHECK_VERSION(3,20,0)

wxGtkStyleContext& wxGtkStyleContext::AddTooltip()
{
    wxASSERT(m_context == NULL);
    GtkWidgetPath* path = m_path;
    gtk_widget_path_append_type(path, GTK_TYPE_WINDOW);
#if GTK_CHECK_VERSION(3,20,0)
    if (gtk_check_version(3,20,0) == NULL)
        gtk_widget_path_iter_set_object_name(path, -1, "tooltip");
#endif
    gtk_widget_path_iter_add_class(path, -1, "background");
    gtk_widget_path_iter_add_class(path, -1, "tooltip");
    gtk_widget_path_iter_set_name(path, -1, "gtk-tooltip");
    m_context = gtk_style_context_new();
    gtk_style_context_set_path(m_context, m_path);
    return *this;
}

wxGtkStyleContext& wxGtkStyleContext::AddWindow(const char* className2)
{
    return Add(GTK_TYPE_WINDOW, "window", "background", className2, NULL);
}

void wxGtkStyleContext::Bg(wxColour& color, int state) const
{
    GdkRGBA* rgba;
    cairo_pattern_t* pattern = NULL;
    gtk_style_context_set_state(m_context, GtkStateFlags(state));
    gtk_style_context_get(m_context, GtkStateFlags(state),
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
                    if (a == 0)
                        break;
                    wxFALLTHROUGH;
                case CAIRO_FORMAT_RGB24:
                    r = guchar(pixel >> 16);
                    g = guchar(pixel >> 8);
                    b = guchar(pixel);
                    if (a != 0xff)
                    {
                        // un-premultiply
                        r = guchar((r * 0xff) / a);
                        g = guchar((g * 0xff) / a);
                        b = guchar((b * 0xff) / a);
                    }
                    color.Set(r, g, b, a);
                    break;
                default:
                    break;
                }
            }
        }
        cairo_pattern_destroy(pattern);
    }

    if (color.Alpha() == 0)
    {
        // Try TLW as last resort, but not if we're already doing it
        if (gtk_widget_path_length(m_path) > 1)
            wxGtkStyleContext().AddWindow().Bg(color, state);
    }
}

void wxGtkStyleContext::Fg(wxColour& color, int state) const
{
    GdkRGBA rgba;
    gtk_style_context_set_state(m_context, GtkStateFlags(state));
#ifdef __WXGTK4__
    gtk_style_context_get_color(m_context, &rgba);
#else
    gtk_style_context_get_color(m_context, GtkStateFlags(state), &rgba);
#endif
    color = wxColour(rgba);
}

void wxGtkStyleContext::Border(wxColour& color) const
{
    GdkRGBA* rgba;
    gtk_style_context_get(m_context, GTK_STATE_FLAG_NORMAL, "border-color", &rgba, NULL);
    color = wxColour(*rgba);
    gdk_rgba_free(rgba);
}

//-----------------------------------------------------------------------------

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

    wxGtkStyleContext sc;

    switch (index)
    {
    case wxSYS_COLOUR_ACTIVECAPTION:
    case wxSYS_COLOUR_INACTIVECAPTION:
    case wxSYS_COLOUR_GRADIENTACTIVECAPTION:
    case wxSYS_COLOUR_GRADIENTINACTIVECAPTION:
#if GTK_CHECK_VERSION(3,10,0)
        if (gtk_check_version(3,10,0) == NULL)
        {
            int state = GTK_STATE_FLAG_NORMAL;
            if (index == wxSYS_COLOUR_INACTIVECAPTION ||
                index == wxSYS_COLOUR_GRADIENTINACTIVECAPTION)
            {
                state = GTK_STATE_FLAG_BACKDROP;
            }
            sc.AddHeaderbar().Bg(color, state);
            break;
        }
        wxFALLTHROUGH;
#endif
    case wxSYS_COLOUR_3DLIGHT:
    case wxSYS_COLOUR_ACTIVEBORDER:
    case wxSYS_COLOUR_BTNFACE:
    case wxSYS_COLOUR_DESKTOP:
    case wxSYS_COLOUR_INACTIVEBORDER:
    case wxSYS_COLOUR_SCROLLBAR:
    case wxSYS_COLOUR_WINDOWFRAME:
        sc.AddButton().Bg(color);
        break;
    case wxSYS_COLOUR_HIGHLIGHT:
        sc.AddTextview("text", "selection");
        sc.Bg(color, GTK_STATE_FLAG_SELECTED | GTK_STATE_FLAG_FOCUSED);
        break;
    case wxSYS_COLOUR_HIGHLIGHTTEXT:
        sc.AddTextview("text", "selection");
        sc.Fg(color, GTK_STATE_FLAG_SELECTED | GTK_STATE_FLAG_FOCUSED);
        break;
    case wxSYS_COLOUR_WINDOWTEXT:
        sc.AddTextview("text").Fg(color);
        break;
    case wxSYS_COLOUR_BTNHIGHLIGHT:
        sc.AddButton().Bg(color, GTK_STATE_FLAG_PRELIGHT);
        break;
    case wxSYS_COLOUR_BTNSHADOW:
        sc.AddButton().Border(color);
        break;
    case wxSYS_COLOUR_CAPTIONTEXT:
#if GTK_CHECK_VERSION(3,10,0)
        if (gtk_check_version(3,10,0) == NULL)
        {
            sc.AddHeaderbar().AddLabel().Fg(color);
            break;
        }
        wxFALLTHROUGH;
#endif
    case wxSYS_COLOUR_BTNTEXT:
        sc.AddButton().AddLabel().Fg(color);
        break;
    case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
#if GTK_CHECK_VERSION(3,10,0)
        if (gtk_check_version(3,10,0) == NULL)
        {
            sc.AddHeaderbar().AddLabel().Fg(color, GTK_STATE_FLAG_BACKDROP);
            break;
        }
        wxFALLTHROUGH;
#endif
    case wxSYS_COLOUR_GRAYTEXT:
        sc.AddLabel().Fg(color, GTK_STATE_FLAG_INSENSITIVE);
        break;
    case wxSYS_COLOUR_HOTLIGHT:
        sc.Add(GTK_TYPE_LINK_BUTTON, "button", "link", NULL);
        if (wx_is_at_least_gtk3(12))
            sc.Fg(color, GTK_STATE_FLAG_LINK);
#ifndef __WXGTK4__
        else
        {
            wxGCC_WARNING_SUPPRESS(deprecated-declarations)
            wxGtkValue value( GDK_TYPE_COLOR);
            gtk_style_context_get_style_property(sc, "link-color", value);
            GdkColor* link_color = static_cast<GdkColor*>(g_value_get_boxed(value));
            GdkColor gdkColor = { 0, 0, 0, 0xeeee };
            if (link_color)
                gdkColor = *link_color;
            color = wxColour(gdkColor);
            wxGCC_WARNING_RESTORE()
        }
#endif
        break;
    case wxSYS_COLOUR_INFOBK:
        sc.AddTooltip().Bg(color);
        break;
    case wxSYS_COLOUR_INFOTEXT:
        sc.AddTooltip().AddLabel().Fg(color);
        break;
    case wxSYS_COLOUR_LISTBOX:
        sc.AddTreeview().Bg(color);
        break;
    case wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT:
        sc.AddTreeview().Fg(color, GTK_STATE_FLAG_SELECTED | GTK_STATE_FLAG_FOCUSED);
        break;
    case wxSYS_COLOUR_LISTBOXTEXT:
        sc.AddTreeview().Fg(color);
        break;
    case wxSYS_COLOUR_MENU:
        sc.AddMenu().Bg(color);
        break;
    case wxSYS_COLOUR_MENUBAR:
        sc.Add(GTK_TYPE_MENU_BAR, "menubar", "menubar", NULL).Bg(color);
        break;
    case wxSYS_COLOUR_MENUHILIGHT:
        sc.AddMenuItem().Bg(color, GTK_STATE_FLAG_PRELIGHT);
        break;
    case wxSYS_COLOUR_MENUTEXT:
        sc.AddMenuItem().AddLabel().Fg(color);
        break;
    case wxSYS_COLOUR_APPWORKSPACE:
    case wxSYS_COLOUR_WINDOW:
        sc.AddTextview().Bg(color);
        break;
    case wxSYS_COLOUR_3DDKSHADOW:
        color.Set(0, 0, 0);
        break;
    default:
        wxFAIL_MSG("invalid system colour index");
        color.Set(0, 0, 0, 0);
        break;
    }

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
        case wxSYS_COLOUR_GRADIENTINACTIVECAPTION:
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
        case wxSYS_COLOUR_GRADIENTACTIVECAPTION:
        case wxSYS_COLOUR_MENUHILIGHT:
            color = wxColor(MenuItemStyle()->bg[GTK_STATE_SELECTED]);
            break;

        case wxSYS_COLOUR_HOTLIGHT:
            {
                GdkColor c = { 0, 0, 0, 0xeeee };
                if (gtk_check_version(2,10,0) == NULL)
                {
                    GdkColor* linkColor = NULL;
                    gtk_widget_style_get(ButtonWidget(), "link-color", &linkColor, NULL);
                    if (linkColor)
                    {
                        c = *linkColor;
                        gdk_color_free(linkColor);
                    }
                }
                color = wxColour(c);
            }
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
                ContainerWidget();
                int scale = 1;
#if GTK_CHECK_VERSION(3,10,0)
                if (wx_is_at_least_gtk3(10))
                    scale = gtk_widget_get_scale_factor(gs_tlw_parent);
#endif
                wxGtkStyleContext sc(scale);
                sc.AddButton().AddLabel();
                gtk_style_context_get(sc, GTK_STATE_FLAG_NORMAL,
                    GTK_STYLE_PROPERTY_FONT, &info.description, NULL);
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

static int GetBorderWidth(wxSystemMetric index, const wxWindow* win)
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

#ifdef __WXGTK4__
static GdkRectangle GetMonitorGeom(GdkWindow* window)
{
    GdkMonitor* monitor;
    if (window)
        monitor = gdk_display_get_monitor_at_window(gdk_window_get_display(window), window);
    else
        monitor = gdk_display_get_primary_monitor(gdk_display_get_default());
    GdkRectangle rect;
    gdk_monitor_get_geometry(monitor, &rect);
    return rect;
}
#endif

static int GetScrollbarWidth()
{
    int width;
#ifdef __WXGTK3__
    if (wx_is_at_least_gtk3(20))
    {
        GtkBorder border;
#if GTK_CHECK_VERSION(3,10,0)
        wxGtkStyleContext sc(gtk_widget_get_scale_factor(ScrollBarWidget()));
#else
        wxGtkStyleContext sc;
#endif
        sc.Add(GTK_TYPE_SCROLLBAR, "scrollbar", "scrollbar", "vertical", "right", NULL);

        gtk_style_context_get_border(sc, GTK_STATE_FLAG_NORMAL, &border);

        sc.Add("contents").Add("trough").Add("slider");

        gtk_style_context_get(sc, GTK_STATE_FLAG_NORMAL, "min-width", &width, NULL);
        width += border.left + border.right;

        gtk_style_context_get_border(sc, GTK_STATE_FLAG_NORMAL, &border);
        width += border.left + border.right;
        gtk_style_context_get_padding(sc, GTK_STATE_FLAG_NORMAL, &border);
        width += border.left + border.right;
        gtk_style_context_get_margin(sc, GTK_STATE_FLAG_NORMAL, &border);
        width += border.left + border.right;
    }
    else
#endif
    {
        int slider_width, trough_border;
        gtk_widget_style_get(ScrollBarWidget(),
            "slider-width", &slider_width, "trough-border", &trough_border, NULL);
        width = slider_width + (2 * trough_border);
    }
    return width;
}

int wxSystemSettingsNative::GetMetric( wxSystemMetric index, const wxWindow* win )
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

        case wxSYS_CARET_ON_MSEC:
        case wxSYS_CARET_OFF_MSEC:
            {
                gboolean should_blink = true;
                gint blink_time = -1;
                g_object_get(GetSettingsForWindowScreen(window),
                                "gtk-cursor-blink", &should_blink,
                                "gtk-cursor-blink-time", &blink_time,
                                NULL);
                if (!should_blink)
                    return 0;

                if (blink_time > 0)
                    return blink_time / 2;

                return -1;
            }

        case wxSYS_CARET_TIMEOUT_MSEC:
            {
                gboolean should_blink = true;
                gint timeout = 0;
                g_object_get(GetSettingsForWindowScreen(window),
                                "gtk-cursor-blink", &should_blink,
                                "gtk-cursor-blink-timeout", &timeout,
                                NULL);
                if (!should_blink)
                    return 0;

                // GTK+ returns this value in seconds, not milliseconds,
                // Special value of 2147483647 means that the cursor never
                // blinks and we handle any value that would overflow int after
                // multiplication in the same manner as it looks quite
                // unnecessary to support cursor blinking once a month.
                if (timeout > 0 && timeout < 2147483647 / 1000)
                    return timeout * 1000;

                return -1;  // no timeout, blink forever
            }

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
#ifdef __WXGTK4__
            return GetMonitorGeom(window).width;
#else
            wxGCC_WARNING_SUPPRESS(deprecated-declarations)
            if (window)
                return gdk_screen_get_width(gdk_window_get_screen(window));
            else
                return gdk_screen_width();
            wxGCC_WARNING_RESTORE()
#endif

        case wxSYS_SCREEN_Y:
#ifdef __WXGTK4__
            return GetMonitorGeom(window).height;
#else
            wxGCC_WARNING_SUPPRESS(deprecated-declarations)
            if (window)
                return gdk_screen_get_height(gdk_window_get_screen(window));
            else
                return gdk_screen_height();
            wxGCC_WARNING_RESTORE()
#endif

        case wxSYS_HSCROLL_Y:
        case wxSYS_VSCROLL_X:
            if (gs_scrollWidth == 0)
                gs_scrollWidth = GetScrollbarWidth();
            return gs_scrollWidth;

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
    virtual bool OnInit() wxOVERRIDE { return true; }
    virtual void OnExit() wxOVERRIDE;
    wxDECLARE_DYNAMIC_CLASS(wxSystemSettingsModule);
};
wxIMPLEMENT_DYNAMIC_CLASS(wxSystemSettingsModule, wxModule);

void wxSystemSettingsModule::OnExit()
{
#ifdef __WXGTK3__
    GtkSettings* settings = gtk_settings_get_default();
    if (settings)
    {
        g_signal_handlers_disconnect_by_func(settings,
            (void*)notify_gtk_theme_name, NULL);
        g_signal_handlers_disconnect_by_func(settings,
            (void*)notify_gtk_font_name, NULL);
    }
#endif
    if (gs_tlw_parent)
    {
        gtk_widget_destroy(gs_tlw_parent);
        gs_tlw_parent = NULL;
    }
}
