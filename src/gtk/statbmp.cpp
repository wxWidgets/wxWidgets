/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:           wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbmp.h"
#endif

#include "wx/statbmp.h"

#if wxUSE_STATBMP

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
    wxCHECK_RET( m_bitmap.Ok(), T("should only be called if we have a bitmap") );

    GdkBitmap *mask = (GdkBitmap *) NULL;
    if ( m_bitmap.GetMask() )
        mask = m_bitmap.GetMask()->GetBitmap();
    m_widget = gtk_pixmap_new( m_bitmap.GetPixmap(), mask );

    /* insert GTK representation */
    (*m_parent->m_insertCallback)(m_parent, this);

    gtk_widget_show( m_widget );

    PostCreation();
}

bool wxStaticBitmap::Create( wxWindow *parent, wxWindowID id, const wxBitmap &bitmap,
                             const wxPoint &pos, const wxSize &size,
                             long style, const wxString &name )
{
    m_needParent = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( T("wxXX creation failed") );
	return FALSE;
    }

    m_bitmap = bitmap;

    if (m_bitmap.Ok())
    {
       wxSize newSize = size;

        GdkBitmap *mask = (GdkBitmap *) NULL;
        if ( m_bitmap.GetMask() )
            mask = m_bitmap.GetMask()->GetBitmap();
        m_widget = gtk_pixmap_new( m_bitmap.GetPixmap(), mask );

        if (newSize.x == -1) newSize.x = m_bitmap.GetWidth();
        if (newSize.y == -1) newSize.y = m_bitmap.GetHeight();
        SetSize( newSize.x, newSize.y );
    }
    else
    {
        m_widget = gtk_label_new( "Bitmap" );

        PostCreation();
    }

    m_parent->DoAddChild( this );

    Show( TRUE );

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

        SetSize( m_bitmap.GetWidth(), m_bitmap.GetHeight() );
    }
}

#endif
