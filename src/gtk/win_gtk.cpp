///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/win_gtk.cpp
// Purpose:     native GTK+ widget for wxWindow
// Author:      Paul Cornett
// Copyright:   (c) 2007 Paul Cornett
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif

#include "wx/private/access.h"

#include "wx/gtk/private.h"
#include "wx/gtk/private/win_gtk.h"

// We use GTK accessibility classes, which are only public since GTK 3.8, to
// implement wxPrivate::SetAccessibleElements().
#if GTK_CHECK_VERSION(3,8,0) && !defined(__WXGTK4__)
    #define wxHAS_GTK_ACCESSIBLE

    #include <gtk/gtk-a11y.h>
#endif

/*
wxPizza is a custom GTK+ widget derived from GtkFixed.  A custom widget
is needed to adapt GTK+ to wxWidgets needs in 3 areas: scrolling, window
borders, and RTL.

For scrolling, the "set_scroll_adjustments" signal is implemented
to make wxPizza appear scrollable to GTK+, allowing it to be put in a
GtkScrolledWindow.  Child widget positions are adjusted for the scrolling
position in size_allocate.

For borders, space is reserved in realize and size_allocate.  The border is
drawn on wxPizza's parent GdkWindow.

For RTL, child widget positions are mirrored in size_allocate.
*/

struct wxPizzaChild
{
    GtkWidget* widget;
    int x, y, width, height;
};

static GtkWidgetClass* parent_class;

#ifdef __WXGTK3__
enum {
    PROP_0,
    PROP_HADJUSTMENT,
    PROP_VADJUSTMENT,
    PROP_HSCROLL_POLICY,
    PROP_VSCROLL_POLICY
};
#endif

#ifdef wxHAS_GTK_ACCESSIBLE

// ----------------------------------------------------------------------------
// Accessibility support
// ----------------------------------------------------------------------------

/*
    wxPizza uses its own accessible type, which is just GtkContainerAccessible
    unless wxPrivate::SetAccessibleElements() is called, in which case it also
    reports the elements passed to it as its children, after the real ones.

    Each of these elements is represented by a wxPizzaAccessibleElement object.
*/

namespace
{

// Register a new static type with the name based on the given one but unique:
// this is needed in case several copies of wxWidgets are loaded into the same
// process, see also wxPizza::type().
GType RegisterUniqueType(GType parent, const char* baseName, const GTypeInfo& info)
{
    const char* name = baseName;
    char buf[64];
    for (unsigned i = 0; g_type_from_name(name); i++)
    {
        g_snprintf(buf, sizeof(buf), "%s%u", baseName, i);
        name = buf;
    }

    return g_type_register_static(parent, name, &info, GTypeFlags(0));
}

} // anonymous namespace

extern "C" {

struct wxPizzaAccessibleElement
{
    AtkObject parent;

    // The container, which owns us, may be null if it has been destroyed or
    // if this element has been removed from it.
    AtkObject* container;

    // Rectangle in the client coordinates of the container window.
    GdkRectangle rect;
};

struct wxPizzaAccessibleElementClass
{
    AtkObjectClass parent;
};

struct wxPizzaAccessible
{
    GtkContainerAccessible parent;

    // Array of wxPizzaAccessibleElement objects, all of which we own.
    GPtrArray* elements;
};

struct wxPizzaAccessibleClass
{
    GtkContainerAccessibleClass parent;
};

static AtkObjectClass* element_parent_class;
static AtkObjectClass* accessible_parent_class;

static GType wxPizzaAccessibleElement_get_type();
static GType wxPizzaAccessible_get_type();

#define WX_PIZZA_ACCESSIBLE_ELEMENT(obj) \
    G_TYPE_CHECK_INSTANCE_CAST(obj, wxPizzaAccessibleElement_get_type(), wxPizzaAccessibleElement)
#define WX_PIZZA_ACCESSIBLE(obj) \
    G_TYPE_CHECK_INSTANCE_CAST(obj, wxPizzaAccessible_get_type(), wxPizzaAccessible)
#define WX_IS_PIZZA_ACCESSIBLE(obj) \
    G_TYPE_CHECK_INSTANCE_TYPE(obj, wxPizzaAccessible_get_type())

static AtkObject* element_get_parent(AtkObject* obj)
{
    return WX_PIZZA_ACCESSIBLE_ELEMENT(obj)->container;
}

static int element_get_index_in_parent(AtkObject* obj)
{
    AtkObject* const container = WX_PIZZA_ACCESSIBLE_ELEMENT(obj)->container;
    if ( !container )
        return -1;

    const GPtrArray* const elements = WX_PIZZA_ACCESSIBLE(container)->elements;
    for ( guint n = 0; n < elements->len; n++ )
    {
        if ( g_ptr_array_index(elements, n) == obj )
            return accessible_parent_class->get_n_children(container) + int(n);
    }

    return -1;
}

static AtkStateSet* element_ref_state_set(AtkObject* obj)
{
    AtkStateSet* const states = element_parent_class->ref_state_set(obj);

    AtkObject* const container = WX_PIZZA_ACCESSIBLE_ELEMENT(obj)->container;
    GtkWidget* const widget = container
        ? gtk_accessible_get_widget(GTK_ACCESSIBLE(container))
        : nullptr;
    if ( !widget )
    {
        atk_state_set_add_state(states, ATK_STATE_DEFUNCT);
        return states;
    }

    atk_state_set_add_state(states, ATK_STATE_ENABLED);
    atk_state_set_add_state(states, ATK_STATE_SENSITIVE);

    if ( gtk_widget_get_visible(widget) )
    {
        atk_state_set_add_state(states, ATK_STATE_VISIBLE);

        if ( gtk_widget_get_mapped(widget) )
            atk_state_set_add_state(states, ATK_STATE_SHOWING);
    }

    return states;
}

static void element_get_extents(AtkComponent* component,
                                int* x, int* y, int* width, int* height,
                                AtkCoordType coord_type)
{
    const wxPizzaAccessibleElement* const
        element = WX_PIZZA_ACCESSIBLE_ELEMENT(component);

    *width = element->rect.width;
    *height = element->rect.height;

    GtkWidget* const widget = element->container
        ? gtk_accessible_get_widget(GTK_ACCESSIBLE(element->container))
        : nullptr;
    if ( !widget || !gtk_widget_is_drawable(widget) )
    {
        *x =
        *y = G_MININT;
        return;
    }

    *x = element->rect.x;
    *y = element->rect.y;

    // The window of wxPizza corresponds to the client area of wxWindow, so
    // its origin is the origin of the client coordinates.
    int xOrigin = 0,
        yOrigin = 0;
    switch ( coord_type )
    {
        case ATK_XY_SCREEN:
        case ATK_XY_WINDOW:
            {
                GdkWindow* const window = gtk_widget_get_window(widget);
                gdk_window_get_origin(window, &xOrigin, &yOrigin);

                if ( coord_type == ATK_XY_WINDOW )
                {
                    int xTLW, yTLW;
                    gdk_window_get_origin(gdk_window_get_toplevel(window),
                                          &xTLW, &yTLW);
                    xOrigin -= xTLW;
                    yOrigin -= yTLW;
                }
            }
            break;

        default:
            // This must be ATK_XY_PARENT, only available since ATK 2.30, and
            // our coordinates are already relative to the parent.
            break;
    }

    *x += xOrigin;
    *y += yOrigin;
}

static void element_component_init(void* g_iface, void*)
{
    AtkComponentIface* const iface = static_cast<AtkComponentIface*>(g_iface);
    iface->get_extents = element_get_extents;
}

static void element_class_init(void* g_class, void*)
{
    AtkObjectClass* const klass = ATK_OBJECT_CLASS(g_class);
    klass->get_parent = element_get_parent;
    klass->get_index_in_parent = element_get_index_in_parent;
    klass->ref_state_set = element_ref_state_set;

    element_parent_class = ATK_OBJECT_CLASS(g_type_class_peek_parent(g_class));
}

static gint accessible_get_n_children(AtkObject* obj)
{
    return accessible_parent_class->get_n_children(obj) +
            int(WX_PIZZA_ACCESSIBLE(obj)->elements->len);
}

static AtkObject* accessible_ref_child(AtkObject* obj, gint i)
{
    const gint numReal = accessible_parent_class->get_n_children(obj);
    if ( i < numReal )
        return accessible_parent_class->ref_child(obj, i);

    const GPtrArray* const elements = WX_PIZZA_ACCESSIBLE(obj)->elements;
    const guint n = guint(i - numReal);
    if ( n >= elements->len )
        return nullptr;

    return ATK_OBJECT(g_object_ref(g_ptr_array_index(elements, n)));
}

static void accessible_finalize(GObject* obj)
{
    GPtrArray* const elements = WX_PIZZA_ACCESSIBLE(obj)->elements;
    for ( guint n = 0; n < elements->len; n++ )
    {
        wxPizzaAccessibleElement* const element =
            WX_PIZZA_ACCESSIBLE_ELEMENT(g_ptr_array_index(elements, n));

        // The accessibility clients may still hold references to the element,
        // so it can outlive us: ensure it doesn't use a dangling pointer.
        element->container = nullptr;
        g_object_unref(element);
    }

    g_ptr_array_free(elements, TRUE);

    G_OBJECT_CLASS(accessible_parent_class)->finalize(obj);
}

static void accessible_init(GTypeInstance* instance, void*)
{
    WX_PIZZA_ACCESSIBLE(instance)->elements = g_ptr_array_new();
}

static void accessible_class_init(void* g_class, void*)
{
    G_OBJECT_CLASS(g_class)->finalize = accessible_finalize;

    AtkObjectClass* const klass = ATK_OBJECT_CLASS(g_class);
    klass->get_n_children = accessible_get_n_children;
    klass->ref_child = accessible_ref_child;

    accessible_parent_class = ATK_OBJECT_CLASS(g_type_class_peek_parent(g_class));
}

static GType wxPizzaAccessibleElement_get_type()
{
    static GType type;
    if (type == 0)
    {
        const GTypeInfo info = {
            sizeof(wxPizzaAccessibleElementClass),
            nullptr, nullptr,
            element_class_init,
            nullptr, nullptr,
            sizeof(wxPizzaAccessibleElement), 0,
            nullptr, nullptr
        };
        type = RegisterUniqueType(ATK_TYPE_OBJECT, "wxPizzaAccessibleElement", info);

        const GInterfaceInfo component_info = {
            element_component_init, nullptr, nullptr
        };
        g_type_add_interface_static(type, ATK_TYPE_COMPONENT, &component_info);
    }
    return type;
}

static GType wxPizzaAccessible_get_type()
{
    static GType type;
    if (type == 0)
    {
        const GTypeInfo info = {
            sizeof(wxPizzaAccessibleClass),
            nullptr, nullptr,
            accessible_class_init,
            nullptr, nullptr,
            sizeof(wxPizzaAccessible), 0,
            accessible_init,
            nullptr
        };
        type = RegisterUniqueType(GTK_TYPE_CONTAINER_ACCESSIBLE, "wxPizzaAccessible", info);
    }
    return type;
}

} // extern "C"

void
wxPrivate::SetAccessibleElements(wxWindow* win, const AccessibleElements& elements)
{
    GtkWidget* const widget = win->m_wxwindow;
    if ( !widget )
        return;

    AtkObject* const obj = gtk_widget_get_accessible(widget);
    if ( !WX_IS_PIZZA_ACCESSIBLE(obj) )
        return;

    GPtrArray* const current = WX_PIZZA_ACCESSIBLE(obj)->elements;
    const guint numReal = guint(accessible_parent_class->get_n_children(obj));
    const guint numNew = guint(elements.size());

    // Update the existing elements in place rather than replacing them, as
    // this preserves the position of the screen reader in them.
    for ( guint n = 0; n < current->len && n < numNew; n++ )
    {
        wxPizzaAccessibleElement* const element =
            WX_PIZZA_ACCESSIBLE_ELEMENT(g_ptr_array_index(current, n));

        const wxRect& rect = elements[n].rect;
        element->rect.x = rect.x;
        element->rect.y = rect.y;
        element->rect.width = rect.width;
        element->rect.height = rect.height;

        // Don't generate a notification if the text didn't change.
        const wxString& label = elements[n].label;
        const char* const name = atk_object_get_name(ATK_OBJECT(element));
        if ( !name || wxString::FromUTF8(name) != label )
            atk_object_set_name(ATK_OBJECT(element), label.utf8_str());
    }

    // Remove the extra elements, if any, starting from the end.
    while ( current->len > numNew )
    {
        const guint n = current->len - 1;

        wxPizzaAccessibleElement* const element =
            WX_PIZZA_ACCESSIBLE_ELEMENT(g_ptr_array_index(current, n));
        g_ptr_array_remove_index(current, n);

        element->container = nullptr;
        g_object_notify(G_OBJECT(element), "accessible-parent");
        g_signal_emit_by_name(obj, "children-changed::remove",
                              numReal + n, element, nullptr);

        g_object_unref(element);
    }

    // And add the new ones.
    for ( guint n = current->len; n < numNew; n++ )
    {
        wxPizzaAccessibleElement* const element = WX_PIZZA_ACCESSIBLE_ELEMENT(
            g_object_new(wxPizzaAccessibleElement_get_type(), nullptr));

        element->container = obj;

        const wxRect& rect = elements[n].rect;
        element->rect.x = rect.x;
        element->rect.y = rect.y;
        element->rect.width = rect.width;
        element->rect.height = rect.height;

        atk_object_set_role(ATK_OBJECT(element), ATK_ROLE_LABEL);
        atk_object_set_name(ATK_OBJECT(element), elements[n].label.utf8_str());

        g_ptr_array_add(current, element);

        g_object_notify(G_OBJECT(element), "accessible-parent");
        g_signal_emit_by_name(obj, "children-changed::add",
                              numReal + n, element, nullptr);
    }
}

#elif defined(__WXGTK3__) && !defined(__WXGTK4__)

// Provide a dummy version if we can't implement it.
void
wxPrivate::SetAccessibleElements(wxWindow* WXUNUSED(win),
                                 const AccessibleElements& WXUNUSED(elements))
{
}

#endif // wxHAS_GTK_ACCESSIBLE/wxGTK3 without it

extern "C" {

struct wxPizzaClass
{
    GtkFixedClass parent;
#ifndef __WXGTK3__
    void (*set_scroll_adjustments)(GtkWidget*, GtkAdjustment*, GtkAdjustment*);
#endif
};

static void pizza_size_allocate(GtkWidget* widget, GtkAllocation* alloc)
{
    wxPizza* pizza = WX_PIZZA(widget);
    GtkBorder border;
    pizza->get_border(border);
    int w = alloc->width - border.left - border.right;
    if (w < 0) w = 0;

    if (gtk_widget_get_realized(widget))
    {
        int h = alloc->height - border.top - border.bottom;
        if (h < 0) h = 0;
        const int x = alloc->x + border.left;
        const int y = alloc->y + border.top;

        GdkWindow* window = gtk_widget_get_window(widget);
        int old_x, old_y;
        gdk_window_get_position(window, &old_x, &old_y);

        if (x != old_x || y != old_y ||
            w != gdk_window_get_width(window) || h != gdk_window_get_height(window))
        {
            gdk_window_move_resize(window, x, y, w, h);

            if (border.left + border.right + border.top + border.bottom)
            {
                // old and new border areas need to be invalidated,
                // otherwise they will not be erased/redrawn properly
                GtkAllocation old_alloc;
                gtk_widget_get_allocation(widget, &old_alloc);
                GdkWindow* parent = gtk_widget_get_parent_window(widget);
                gdk_window_invalidate_rect(parent, &old_alloc, false);
                gdk_window_invalidate_rect(parent, alloc, false);
            }
        }
    }

    gtk_widget_set_allocation(widget, alloc);

    // adjust child positions
    for (const GList* p = pizza->m_children; p; p = p->next)
    {
        const wxPizzaChild* child = static_cast<wxPizzaChild*>(p->data);
        if (gtk_widget_get_visible(child->widget))
        {
            pizza->size_allocate_child(
                child->widget, child->x, child->y, child->width, child->height, w);
        }
    }
}

static void pizza_realize(GtkWidget* widget)
{
    parent_class->realize(widget);

    wxPizza* pizza = WX_PIZZA(widget);
    if (pizza->m_windowStyle & wxPizza::BORDER_STYLES)
    {
        GtkBorder border;
        pizza->get_border(border);
        GtkAllocation a;
        gtk_widget_get_allocation(widget, &a);
        int x = a.x + border.left;
        int y = a.y + border.top;
        int w = a.width - border.left - border.right;
        int h = a.height - border.top - border.bottom;
        if (w < 0) w = 0;
        if (h < 0) h = 0;
        gdk_window_move_resize(gtk_widget_get_window(widget), x, y, w, h);
    }
}

static void pizza_show(GtkWidget* widget)
{
    GtkWidget* parent = gtk_widget_get_parent(widget);
    if (parent && (WX_PIZZA(widget)->m_windowStyle & wxPizza::BORDER_STYLES))
    {
        // invalidate whole allocation so borders will be drawn properly
        GtkAllocation a;
        gtk_widget_get_allocation(widget, &a);
        gtk_widget_queue_draw_area(parent, a.x, a.y, a.width, a.height);
    }

    parent_class->show(widget);
}

static void pizza_hide(GtkWidget* widget)
{
    GtkWidget* parent = gtk_widget_get_parent(widget);
    if (parent && (WX_PIZZA(widget)->m_windowStyle & wxPizza::BORDER_STYLES))
    {
        // invalidate whole allocation so borders will be erased properly
        GtkAllocation a;
        gtk_widget_get_allocation(widget, &a);
        gtk_widget_queue_draw_area(parent, a.x, a.y, a.width, a.height);
    }

    parent_class->hide(widget);
}

static void pizza_add(GtkContainer* container, GtkWidget* widget)
{
    WX_PIZZA(container)->put(widget, 0, 0, 1, 1);
}

static void pizza_remove(GtkContainer* container, GtkWidget* widget)
{
    GTK_CONTAINER_CLASS(parent_class)->remove(container, widget);

    wxPizza* pizza = WX_PIZZA(container);
    for (GList* p = pizza->m_children; p; p = p->next)
    {
        wxPizzaChild* child = static_cast<wxPizzaChild*>(p->data);
        if (child->widget == widget)
        {
            pizza->m_children = g_list_delete_link(pizza->m_children, p);
            delete child;
            break;
        }
    }
}

#ifdef __WXGTK3__
// Get preferred size of children, to avoid GTK+ warnings complaining
// that they were size-allocated without asking their preferred size
static void children_get_preferred_size(const GList* p)
{
    for (; p; p = p->next)
    {
        const wxPizzaChild* child = static_cast<wxPizzaChild*>(p->data);
        if (gtk_widget_get_visible(child->widget))
        {
            GtkRequisition req;
            gtk_widget_get_preferred_size(child->widget, &req, nullptr);
        }
    }
}

static void pizza_get_preferred_width(GtkWidget* widget, int* minimum, int* natural)
{
    children_get_preferred_size(WX_PIZZA(widget)->m_children);
    *minimum = 0;
    gtk_widget_get_size_request(widget, natural, nullptr);
    if (*natural < 0)
        *natural = 0;
}

static void pizza_get_preferred_height(GtkWidget* widget, int* minimum, int* natural)
{
    children_get_preferred_size(WX_PIZZA(widget)->m_children);
    *minimum = 0;
    gtk_widget_get_size_request(widget, nullptr, natural);
    if (*natural < 0)
        *natural = 0;
}

static void pizza_adjust_size_request(GtkWidget* widget, GtkOrientation orientation, int* minimum, int* natural)
{
    parent_class->adjust_size_request(widget, orientation, minimum, natural);
    // Override adjustments to minimum size. GtkWidgetClass.adjust_size_request()
    // will use the size request, if set, as the minimum.
    // But don't override if in a GtkToolbar, it uses the minimum as actual size.
    GtkWidget* parent = gtk_widget_get_parent(widget);
    if (!GTK_IS_TOOL_ITEM(parent))
        *minimum = 0;
}

// GtkScrollable interface
static void pizza_get_property(GObject*, guint property_id, GValue* value, GParamSpec*)
{
    if (property_id == PROP_HSCROLL_POLICY || property_id == PROP_VSCROLL_POLICY)
    {
        // Use natural size, rather than minimum, as virtual size
        g_value_set_enum(value, GTK_SCROLL_NATURAL);
    }
}

static void pizza_set_property(GObject*, guint, const GValue*, GParamSpec*)
{
}
#else
// not used, but needs to exist so gtk_widget_set_scroll_adjustments will work
static void pizza_set_scroll_adjustments(GtkWidget*, GtkAdjustment*, GtkAdjustment*)
{
}

// Marshaller needed for set_scroll_adjustments signal,
// generated with GLib-2.4.6 glib-genmarshal
#define g_marshal_value_peek_object(v)   g_value_get_object (v)
static void
g_cclosure_user_marshal_VOID__OBJECT_OBJECT (GClosure     *closure,
                                             GValue       * /*return_value*/,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      /*invocation_hint*/,
                                             gpointer      marshal_data)
{
  typedef void (*GMarshalFunc_VOID__OBJECT_OBJECT) (gpointer     data1,
                                                    gpointer     arg_1,
                                                    gpointer     arg_2,
                                                    gpointer     data2);
  GMarshalFunc_VOID__OBJECT_OBJECT callback;
  GCClosure *cc = (GCClosure*) closure;
  gpointer data1, data2;

  g_return_if_fail (n_param_values == 3);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_VOID__OBJECT_OBJECT) (marshal_data ? marshal_data : cc->callback);

  callback (data1,
            g_marshal_value_peek_object (param_values + 1),
            g_marshal_value_peek_object (param_values + 2),
            data2);
}
#endif

static void class_init(void* g_class, void*)
{
    GtkWidgetClass* widget_class = (GtkWidgetClass*)g_class;
    widget_class->size_allocate = pizza_size_allocate;
    widget_class->realize = pizza_realize;
    widget_class->show = pizza_show;
    widget_class->hide = pizza_hide;
    GtkContainerClass* container_class = (GtkContainerClass*)g_class;
    container_class->add = pizza_add;
    container_class->remove = pizza_remove;

#ifdef __WXGTK3__
    widget_class->get_preferred_width = pizza_get_preferred_width;
    widget_class->get_preferred_height = pizza_get_preferred_height;
    widget_class->adjust_size_request = pizza_adjust_size_request;
    GObjectClass *gobject_class = G_OBJECT_CLASS(g_class);
    gobject_class->set_property = pizza_set_property;
    gobject_class->get_property = pizza_get_property;
    g_object_class_override_property(gobject_class, PROP_HADJUSTMENT, "hadjustment");
    g_object_class_override_property(gobject_class, PROP_VADJUSTMENT, "vadjustment");
    g_object_class_override_property(gobject_class, PROP_HSCROLL_POLICY, "hscroll-policy");
    g_object_class_override_property(gobject_class, PROP_VSCROLL_POLICY, "vscroll-policy");
#else
    wxPizzaClass* klass = static_cast<wxPizzaClass*>(g_class);
    // needed to make widget appear scrollable to GTK+
    klass->set_scroll_adjustments = pizza_set_scroll_adjustments;
    widget_class->set_scroll_adjustments_signal =
        g_signal_new(
            "set_scroll_adjustments",
            G_TYPE_FROM_CLASS(g_class),
            G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET(wxPizzaClass, set_scroll_adjustments),
            nullptr, nullptr,
            g_cclosure_user_marshal_VOID__OBJECT_OBJECT,
            G_TYPE_NONE, 2, GTK_TYPE_ADJUSTMENT, GTK_TYPE_ADJUSTMENT);
#endif
#ifdef wxHAS_GTK_ACCESSIBLE
    gtk_widget_class_set_accessible_type(widget_class, wxPizzaAccessible_get_type());
#endif

    parent_class = GTK_WIDGET_CLASS(g_type_class_peek_parent(g_class));
}

} // extern "C"

GType wxPizza::type()
{
    static GType type;
    if (type == 0)
    {
        const char* name = "wxPizza";
        char buf[30];
        for (unsigned i = 0; g_type_from_name(name); i++)
        {
            g_snprintf(buf, sizeof(buf), "wxPizza%u", i);
            name = buf;
        }
        const GTypeInfo info = {
            sizeof(wxPizzaClass),
            nullptr, nullptr,
            class_init,
            nullptr, nullptr,
            sizeof(wxPizza), 0,
            nullptr, nullptr
        };
        type = g_type_register_static(
            GTK_TYPE_FIXED, name, &info, GTypeFlags(0));
#ifdef __WXGTK3__
        const GInterfaceInfo interface_info = { nullptr, nullptr, nullptr };
        g_type_add_interface_static(type, GTK_TYPE_SCROLLABLE, &interface_info);
#endif
    }
    return type;
}

GtkWidget* wxPizza::New(long windowStyle)
{
    GtkWidget* widget = GTK_WIDGET(g_object_new(type(), nullptr));
    wxPizza* pizza = WX_PIZZA(widget);
    pizza->m_children = nullptr;
    pizza->m_scroll_x = 0;
    pizza->m_scroll_y = 0;
    pizza->m_windowStyle = windowStyle;
#ifdef __WXGTK3__
    gtk_widget_set_has_window(widget, true);
#else
    gtk_fixed_set_has_window(GTK_FIXED(widget), true);
#endif
    gtk_widget_add_events(widget,
        GDK_EXPOSURE_MASK |
        GDK_SCROLL_MASK |
#if GTK_CHECK_VERSION(3,4,0)
        GDK_SMOOTH_SCROLL_MASK |
#endif
        GDK_POINTER_MOTION_MASK |
        GDK_POINTER_MOTION_HINT_MASK |
        GDK_BUTTON_MOTION_MASK |
        GDK_BUTTON1_MOTION_MASK |
        GDK_BUTTON2_MOTION_MASK |
        GDK_BUTTON3_MOTION_MASK |
        GDK_BUTTON_PRESS_MASK |
        GDK_BUTTON_RELEASE_MASK |
        GDK_KEY_PRESS_MASK |
        GDK_KEY_RELEASE_MASK |
        GDK_ENTER_NOTIFY_MASK |
        GDK_LEAVE_NOTIFY_MASK |
        GDK_FOCUS_CHANGE_MASK);
    return widget;
}

void wxPizza::move(GtkWidget* widget, int x, int y, int width, int height)
{
    for (const GList* p = m_children; p; p = p->next)
    {
        wxPizzaChild* child = static_cast<wxPizzaChild*>(p->data);
        if (child->widget == widget)
        {
            child->x = x;
            child->y = y;
            child->width = width;
            child->height = height;
            // normally a queue-resize would be needed here, but we know
            // wxWindowGTK::DoMoveWindow() will take care of it
            break;
        }
    }
}

void wxPizza::size_allocate_child(
    GtkWidget* child, int x, int y, int width, int height, int parent_width)
{
    if (width <= 0 || height <= 0)
        return;

    GtkAllocation child_alloc;
    // note that child positions do not take border into account, they need to
    // be relative to widget->window, which has already been adjusted
    child_alloc.x = x - m_scroll_x;
    child_alloc.y = y - m_scroll_y;
    child_alloc.width  = width;
    child_alloc.height = height;
    if (gtk_widget_get_direction(GTK_WIDGET(this)) == GTK_TEXT_DIR_RTL)
    {
        if (parent_width < 0)
        {
            GtkBorder border;
            get_border(border);
            GtkAllocation alloc;
            gtk_widget_get_allocation(GTK_WIDGET(this), &alloc);
            parent_width = alloc.width - border.left - border.right;
        }
        child_alloc.x = parent_width - child_alloc.x - child_alloc.width;
    }
    gtk_widget_size_allocate(child, &child_alloc);
}

void wxPizza::put(GtkWidget* widget, int x, int y, int width, int height)
{
    // Re-parenting a TLW under a child window is possible at wx level but
    // using a TLW as child at GTK+ level results in problems, so don't do it.
    if (!gtk_widget_is_toplevel(GTK_WIDGET(widget)))
    {
        gtk_fixed_put(GTK_FIXED(this), widget, 0, 0);
        gtk_widget_set_size_request(widget, -1, -1);
    }

    wxPizzaChild* child = new wxPizzaChild;
    child->widget = widget;
    child->x = x;
    child->y = y;
    child->width = width;
    child->height = height;
    m_children = g_list_append(m_children, child);
}

struct AdjustData {
    GdkWindow* window;
    int dx, dy;
};

// Adjust allocations for all widgets using the GdkWindow which was just scrolled
extern "C" {
static void scroll_adjust(GtkWidget* widget, void* data)
{
    if (!gtk_widget_get_visible(widget))
        return;

    const AdjustData* p = static_cast<AdjustData*>(data);
    GtkAllocation a;
    gtk_widget_get_allocation(widget, &a);
    a.x += p->dx;
    a.y += p->dy;
    gtk_widget_set_allocation(widget, &a);

    if (gtk_widget_get_window(widget) == p->window)
    {
        // GtkFrame requires a queue_resize, otherwise parts of
        // the frame newly exposed by the scroll are not drawn.
        // To be safe, do it for all widgets.
        gtk_widget_queue_resize_no_redraw(widget);
        if (GTK_IS_CONTAINER(widget))
            gtk_container_forall(GTK_CONTAINER(widget), scroll_adjust, data);
    }
}
}

void wxPizza::scroll(int dx, int dy)
{
    GtkWidget* widget = GTK_WIDGET(this);
#ifndef __WXGTK3__
    if (gtk_widget_get_direction(widget) == GTK_TEXT_DIR_RTL)
        dx = -dx;
#endif
    m_scroll_x -= dx;
    m_scroll_y -= dy;
    GdkWindow* window = gtk_widget_get_window(widget);
    if (window)
    {
        gdk_window_scroll(window, dx, dy);
        // Adjust child allocations. Doing a queue_resize on the children is not
        // enough, sometimes they redraw in the wrong place during fast scrolling.
        AdjustData data = { window, dx, dy };
        gtk_container_forall(GTK_CONTAINER(widget), scroll_adjust, &data);
    }
}

void wxPizza::get_border(GtkBorder& border)
{
#ifndef __WXUNIVERSAL__
    if (m_windowStyle & wxBORDER_SIMPLE)
        border.left = border.right = border.top = border.bottom = 1;
    else if (m_windowStyle & (wxBORDER_RAISED | wxBORDER_SUNKEN | wxBORDER_THEME))
    {
#ifdef __WXGTK3__
        GtkStyleContext* sc;
        if (m_windowStyle & (wxHSCROLL | wxVSCROLL))
            sc = gtk_widget_get_style_context(wxGTKPrivate::GetTreeWidget());
        else
            sc = gtk_widget_get_style_context(wxGTKPrivate::GetEntryWidget());

        gtk_style_context_set_state(sc, GTK_STATE_FLAG_NORMAL);
        gtk_style_context_get_border(sc, GTK_STATE_FLAG_NORMAL, &border);
#else // !__WXGTK3__
        GtkStyle* style;
        if (m_windowStyle & (wxHSCROLL | wxVSCROLL))
            style = gtk_widget_get_style(wxGTKPrivate::GetTreeWidget());
        else
            style = gtk_widget_get_style(wxGTKPrivate::GetEntryWidget());

        border.left = border.right = style->xthickness;
        border.top = border.bottom = style->ythickness;
#endif // !__WXGTK3__
    }
    else
#endif // !__WXUNIVERSAL__
    {
        border.left = border.right = border.top = border.bottom = 0;
    }
}
