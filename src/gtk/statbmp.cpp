/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/statbmp.cpp
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:           wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STATBMP

#include "wx/statbmp.h"

#include "wx/gtk/private/wrapgtk.h"

#ifdef __WXGTK3__
extern "C" {
static gboolean image_draw(GtkWidget* widget, cairo_t* cr, wxStaticBitmap* win)
{
    wxBitmap bitmap(win->GetBitmap());
    if (!bitmap.IsOk() || bitmap.GetScaleFactor() <= 1)
        return false;

    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    gtk_render_background(gtk_widget_get_style_context(widget),
        cr, 0, 0, alloc.width, alloc.height);
    const wxSize size(bitmap.GetScaledSize());
    int x = (alloc.width  - size.x) / 2;
    int y = (alloc.height - size.y) / 2;
    if (!win->IsEnabled())
        bitmap = bitmap.CreateDisabled();
    bitmap.Draw(cr, x, y);
    return true;
}
}
#endif

//-----------------------------------------------------------------------------
// wxStaticBitmap
//-----------------------------------------------------------------------------

wxStaticBitmap::wxStaticBitmap(void)
{
}

wxStaticBitmap::wxStaticBitmap( wxWindow *parent, wxWindowID id, const wxBitmap &bitmap,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
    Create( parent, id, bitmap, pos, size, style, name );
}

bool wxStaticBitmap::Create( wxWindow *parent, wxWindowID id, const wxBitmap &bitmap,
                             const wxPoint &pos, const wxSize &size,
                             long style, const wxString &name )
{
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxStaticBitmap creation failed") );
        return false;
    }

    m_widget = gtk_image_new();
    g_object_ref(m_widget);

    if (bitmap.IsOk())
        SetBitmap(bitmap);

    PostCreation(size);
    m_parent->DoAddChild( this );
#ifdef __WXGTK3__
    g_signal_connect(m_widget, "draw", G_CALLBACK(image_draw), this);
#endif

    return true;
}

void wxStaticBitmap::SetBitmap( const wxBitmap &bitmap )
{
    m_bitmap = bitmap;

    // always use pixbuf, because pixmap mask does not
    // work with disabled images in some themes
    if (m_bitmap.IsOk())
    {
#ifdef __WXGTK3__
        if (bitmap.GetScaleFactor() > 1)
        {
            // Use a placeholder pixbuf with the correct size.
            // The original will be used by our "draw" signal handler.
            const wxSize size(bitmap.GetScaledSize());
            GdkPixbuf* pixbuf =
                gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, size.x, size.y);
            gtk_image_set_from_pixbuf(GTK_IMAGE(m_widget), pixbuf);
            g_object_unref(pixbuf);
        }
        else
#endif
        {
            gtk_image_set_from_pixbuf(GTK_IMAGE(m_widget), m_bitmap.GetPixbuf());
        }
    }
    else
        gtk_image_set_from_pixbuf(GTK_IMAGE(m_widget), NULL);

    InvalidateBestSize();
    SetSize(GetBestSize());
}

// static
wxVisualAttributes
wxStaticBitmap::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_image_new());
}

#endif // wxUSE_STATBMP

