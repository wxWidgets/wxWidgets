///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/image_gtk.cpp
// Author:      Paul Cornett
// Copyright:   (c) 2020 Paul Cornett
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/bmpbndl.h"
#include "wx/log.h"
#include "wx/window.h"

#include "wx/gtk/private/wrapgtk.h"
#include "wx/gtk/private/gtk3-compat.h"
#include "wx/gtk/private/image.h"

namespace
{

#ifdef __WXGTK3__

// Default provider for HiDPI common case
struct BitmapProviderDefault: wxGtkImage::BitmapProvider
{
    BitmapProviderDefault(wxWindow* win) : m_win(win) { }

    virtual wxBitmap Get(int scale) const wxOVERRIDE;
    virtual void Set(const wxBitmapBundle& bitmap) wxOVERRIDE;

    wxWindow* const m_win;

    // All the bitmaps we use.
    wxBitmapBundle m_bitmapBundle;
};

wxBitmap BitmapProviderDefault::Get(int scale) const
{
    wxBitmap bitmap(GetAtScale(m_bitmapBundle, scale));
    if (m_win && !m_win->IsEnabled())
        bitmap = bitmap.CreateDisabled();

    return bitmap;
}

void BitmapProviderDefault::Set(const wxBitmapBundle& bitmapBundle)
{
    m_bitmapBundle = bitmapBundle;
}

#else // !__WXGTK3__

// Trivial version for GTK < 3 which doesn't provide any high DPI support.
struct BitmapProviderDefault: wxGtkImage::BitmapProvider
{
    BitmapProviderDefault(wxWindow*) { }
    virtual wxBitmap Get(int /*scale*/) const wxOVERRIDE { return wxBitmap(); }
};

#endif // __WXGTK3__/!__WXGTK3__

} // namespace

extern "C" {
static void wxGtkImageClassInit(void* g_class, void* class_data);
}

GType wxGtkImage::Type()
{
    static GType type;
    if (type == 0)
    {
        const GTypeInfo info = {
            sizeof(GtkImageClass),
            NULL, NULL,
            wxGtkImageClassInit, NULL, NULL,
            sizeof(wxGtkImage), 0, NULL,
            NULL
        };
        type = g_type_register_static(
            GTK_TYPE_IMAGE, "wxGtkImage", &info, GTypeFlags(0));
    }
    return type;
}

GtkWidget* wxGtkImage::New(BitmapProvider* provider)
{
    wxGtkImage* image = WX_GTK_IMAGE(g_object_new(Type(), NULL));
    image->m_provider = provider;
    return GTK_WIDGET(image);
}

GtkWidget* wxGtkImage::New(wxWindow* win)
{
    return New(new BitmapProviderDefault(win));
}

void wxGtkImage::Set(const wxBitmapBundle& bitmapBundle)
{
    m_provider->Set(bitmapBundle);

    // Always set the default bitmap to use the correct size, even if we draw a
    // different bitmap below.
    wxBitmap bitmap = bitmapBundle.GetBitmap(wxDefaultSize);

    GdkPixbuf* pixbuf = NULL;
    if (bitmap.IsOk())
    {
        pixbuf = bitmap.GetPixbuf();
    }
    gtk_image_set_from_pixbuf(GTK_IMAGE(this), pixbuf);
}

static GtkWidgetClass* wxGtkImageParentClass;

extern "C"
{
#ifdef __WXGTK3__
static gboolean wxGtkImageDraw(GtkWidget* widget, cairo_t* cr)
#else
static gboolean wxGtkImageDraw(GtkWidget* widget, GdkEventExpose* event)
#endif
{
    wxGtkImage* image = WX_GTK_IMAGE(widget);

    int scale = 1;
#if GTK_CHECK_VERSION(3,10,0)
    if (wx_is_at_least_gtk3(10))
        scale = gtk_widget_get_scale_factor(widget);
#endif
    const wxBitmap bitmap(image->m_provider->Get(scale));

    if (!bitmap.IsOk())
    {
#ifdef __WXGTK3__
        // Missing bitmap, just do the default
        return wxGtkImageParentClass->draw(widget, cr);
#else
        // We rely on GTK to draw default disabled images
        return wxGtkImageParentClass->expose_event(widget, event);
#endif
    }

    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    int x = (alloc.width  - int(bitmap.GetLogicalWidth() )) / 2;
    int y = (alloc.height - int(bitmap.GetLogicalHeight())) / 2;
#ifdef __WXGTK3__
    gtk_render_background(gtk_widget_get_style_context(widget),
        cr, 0, 0, alloc.width, alloc.height);
    if (bitmap.IsOk())
        bitmap.Draw(cr, x, y);
#else
    x += alloc.x;
    y += alloc.y;
    gdk_draw_pixbuf(
        gtk_widget_get_window(widget), gtk_widget_get_style(widget)->black_gc, bitmap.GetPixbuf(),
        0, 0, x, y,
        -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);
#endif
    return false;
}

static void wxGtkImageFinalize(GObject* object)
{
    wxGtkImage* image = WX_GTK_IMAGE(object);
    delete image->m_provider;
    image->m_provider = NULL;
    G_OBJECT_CLASS(wxGtkImageParentClass)->finalize(object);
}

static void wxGtkImageClassInit(void* g_class, void* /*class_data*/)
{
#ifdef __WXGTK3__
    GTK_WIDGET_CLASS(g_class)->draw = wxGtkImageDraw;
#else
    GTK_WIDGET_CLASS(g_class)->expose_event = wxGtkImageDraw;
#endif
    G_OBJECT_CLASS(g_class)->finalize = wxGtkImageFinalize;
    wxGtkImageParentClass = GTK_WIDGET_CLASS(g_type_class_peek_parent(g_class));
}
} // extern "C"
