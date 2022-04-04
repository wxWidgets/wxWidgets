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
#include "wx/gtk/private/image.h"

//-----------------------------------------------------------------------------
// wxStaticBitmap
//-----------------------------------------------------------------------------

wxStaticBitmap::wxStaticBitmap(void)
{
}

wxStaticBitmap::wxStaticBitmap( wxWindow *parent, wxWindowID id, const wxBitmapBundle &bitmap,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
    Create( parent, id, bitmap, pos, size, style, name );
}

bool wxStaticBitmap::Create( wxWindow *parent, wxWindowID id, const wxBitmapBundle &bitmap,
                             const wxPoint &pos, const wxSize &size,
                             long style, const wxString &name )
{
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxStaticBitmap creation failed") );
        return false;
    }

    m_widget = wxGtkImage::New(this);
    g_object_ref(m_widget);

    if (bitmap.IsOk())
        SetBitmap(bitmap);

    PostCreation(size);
    m_parent->DoAddChild( this );

    return true;
}

void wxStaticBitmap::SetBitmap( const wxBitmapBundle &bitmap )
{
    const wxSize sizeOld(DoGetBestSize());

    m_bitmapBundle = bitmap;

    const wxSize sizeNew(DoGetBestSize());

    WX_GTK_IMAGE(m_widget)->Set(bitmap);

    if (sizeNew != sizeOld)
    {
        InvalidateBestSize();
        SetSize(GetBestSize());
    }
}

// static
wxVisualAttributes
wxStaticBitmap::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_image_new());
}

#endif // wxUSE_STATBMP

