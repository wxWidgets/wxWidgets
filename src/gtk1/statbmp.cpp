/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:           wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "statbmp.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STATBMP

#include "wx/statbmp.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// wxStaticBitmap
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap,wxControl)

wxStaticBitmap::wxStaticBitmap(void)
{
}

wxStaticBitmap::wxStaticBitmap( wxWindow *parent, wxWindowID id, const wxBitmap &bitmap,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
    Create( parent, id, bitmap, pos, size, style, name );
}

#ifndef __WXGTK20__
// empty bitmap, so that we can create GtkPixmap widget:
static char * bogus_xpm[] = {
"2 2 1 1",
" 	c None",
"  ",
"  "};
#endif

bool wxStaticBitmap::Create( wxWindow *parent, wxWindowID id, const wxBitmap &bitmap,
                             const wxPoint &pos, const wxSize &size,
                             long style, const wxString &name )
{
    m_needParent = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxStaticBitmap creation failed") );
        return false;
    }

    m_bitmap = bitmap;

#ifdef __WXGTK20__
    m_widget = gtk_image_new();
#else
    wxBitmap bmp(bitmap.Ok() ? bitmap : wxBitmap(bogus_xpm));
    m_widget = gtk_pixmap_new(bmp.GetPixmap(), NULL);
#endif

    if (bitmap.Ok())
        SetBitmap(bitmap);

    PostCreation(size);
    m_parent->DoAddChild( this );

    return true;
}

void wxStaticBitmap::SetBitmap( const wxBitmap &bitmap )
{
    m_bitmap = bitmap;

    if (m_bitmap.Ok())
    {
        GdkBitmap *mask = (GdkBitmap *) NULL;
        if (m_bitmap.GetMask())
            mask = m_bitmap.GetMask()->GetBitmap();
    
#ifdef __WXGTK20__
        if (m_bitmap.HasPixbuf())
        {
            gtk_image_set_from_pixbuf(GTK_IMAGE(m_widget),
                                      m_bitmap.GetPixbuf());
        }
        else
            gtk_image_set_from_pixmap(GTK_IMAGE(m_widget),
                                      m_bitmap.GetPixmap(), mask);
#else
        gtk_pixmap_set(GTK_PIXMAP(m_widget), m_bitmap.GetPixmap(), mask);
#endif

        InvalidateBestSize();
        SetSize(GetBestSize());
    }
}

// static
wxVisualAttributes
wxStaticBitmap::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    // TODO: overload to allow using gtk_pixmap_new?
    return GetDefaultAttributesFromGTKWidget(gtk_label_new);
}

#endif // wxUSE_STATBMP

