///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/image_gtk.cpp
// Author:      Paul Cornett
// Copyright:   (c) 2020 Paul Cornett
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/bitmap.h"
#include "wx/window.h"

#include "wx/gtk/private/wrapgtk.h"
#include "wx/gtk/private/image.h"

namespace
{
// Default provider for HiDPI common case
struct BitmapProviderDefault: wxGtkImage::BitmapProvider
{
#ifdef __WXGTK3__
    BitmapProviderDefault(wxWindow* win) : m_win(win) { }
    virtual wxBitmap Get() const wxOVERRIDE;
    virtual void Set(const wxBitmap& bitmap) wxOVERRIDE;
    wxWindow* const m_win;
    wxBitmap m_bitmap;
    wxBitmap m_bitmapDisabled;
#else
    BitmapProviderDefault(wxWindow*) { }
    virtual wxBitmap Get() const wxOVERRIDE { return wxBitmap(); }
#endif
};

#ifdef __WXGTK3__
wxBitmap BitmapProviderDefault::Get() const
{
    return (m_win == NULL || m_win->IsEnabled()) ? m_bitmap : m_bitmapDisabled;
}

void BitmapProviderDefault::Set(const wxBitmap& bitmap)
{
    m_bitmap.UnRef();
    m_bitmapDisabled.UnRef();
    if (bitmap.IsOk() && bitmap.GetScaleFactor() > 1)
    {
        m_bitmap = bitmap;
        if (m_win)
            m_bitmapDisabled = bitmap.CreateDisabled();
    }
}
#endif // __WXGTK3__
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

void wxGtkImage::Set(const wxBitmap& bitmap)
{
    m_provider->Set(bitmap);

    GdkPixbuf* pixbuf = NULL;
    GdkPixbuf* pixbufNew = NULL;
    if (bitmap.IsOk())
    {
        if (bitmap.GetScaleFactor() <= 1)
            pixbuf = bitmap.GetPixbuf();
        else
        {
            // Placeholder pixbuf for correct size
            pixbufNew =
            pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8,
                int(bitmap.GetScaledWidth()), int(bitmap.GetScaledHeight()));
        }
    }
    gtk_image_set_from_pixbuf(GTK_IMAGE(this), pixbuf);
    if (pixbufNew)
        g_object_unref(pixbufNew);
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
    const wxBitmap bitmap(image->m_provider->Get());
    if (!bitmap.IsOk())
    {
#ifdef __WXGTK3__
        return wxGtkImageParentClass->draw(widget, cr);
#else
        return wxGtkImageParentClass->expose_event(widget, event);
#endif
    }

    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    int x = (alloc.width  - int(bitmap.GetScaledWidth() )) / 2;
    int y = (alloc.height - int(bitmap.GetScaledHeight())) / 2;
#ifdef __WXGTK3__
    gtk_render_background(gtk_widget_get_style_context(widget),
        cr, 0, 0, alloc.width, alloc.height);
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
