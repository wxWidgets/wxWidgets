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

void wxStaticBitmap::CreatePixmapWidget()
{
    wxCHECK_RET( m_bitmap.Ok(), wxT("should only be called if we have a bitmap") );

    GdkBitmap *mask = (GdkBitmap *) NULL;
    if ( m_bitmap.GetMask() )
        mask = m_bitmap.GetMask()->GetBitmap();
    m_widget = gtk_pixmap_new( m_bitmap.GetPixmap(), mask );

    // insert GTK representation
    (*m_parent->m_insertCallback)(m_parent, this);

    gtk_widget_show( m_widget );

    m_focusWidget = m_widget;

    PostCreation(wxDefaultSize);
}

bool wxStaticBitmap::Create( wxWindow *parent, wxWindowID id, const wxBitmap &bitmap,
                             const wxPoint &pos, const wxSize &size,
                             long style, const wxString &name )
{
    m_needParent = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxXX creation failed") );
    return FALSE;
    }

    m_bitmap = bitmap;

    if (m_bitmap.Ok())
    {
        GdkBitmap *mask = (GdkBitmap *) NULL;
        if ( m_bitmap.GetMask() )
            mask = m_bitmap.GetMask()->GetBitmap();
        m_widget = gtk_pixmap_new( m_bitmap.GetPixmap(), mask );
    }
    else
    {
        m_widget = gtk_label_new( "Bitmap" );
        m_focusWidget = m_widget;
    }

    PostCreation(size);
    m_parent->DoAddChild( this );

    return TRUE;
}

void wxStaticBitmap::SetBitmap( const wxBitmap &bitmap )
{
    bool hasWidget = m_bitmap.Ok();
    m_bitmap = bitmap;

    if (m_bitmap.Ok())
    {
        if (!hasWidget)
        {
            gtk_widget_destroy( m_widget );

            /* recreate m_widget because we've created a label
               and not a bitmap above */
            CreatePixmapWidget();
        }
        else
        {
            GdkBitmap *mask = (GdkBitmap *) NULL;
            if (m_bitmap.GetMask()) mask = m_bitmap.GetMask()->GetBitmap();
            gtk_pixmap_set( GTK_PIXMAP(m_widget), m_bitmap.GetPixmap(), mask );
        }

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

