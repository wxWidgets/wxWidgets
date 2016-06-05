/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/mimetype.cpp
// Purpose:     classes and functions to manage MIME types
// Author:      Hans Mackowiak
// Modified by:
// Created:     05.06.2016
// Copyright:   (c) 2016 Hans Mackowiak <hanmac@gmx.de>
// Licence:     wxWindows licence (part of wxExtra library)
/////////////////////////////////////////////////////////////////////////////


#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_MIMETYPE && wxUSE_FILE

#include "wx/gtk/mimetype.h"

#include <gio/gio.h>
#include <gtk/gtk.h>

#include "wx/gtk/private/string.h"

wxString wxGTKMimeTypesManagerImpl::GetIconFromMimeType(const wxString& mime)
{
    wxString icon;
    wxGtkString type(g_content_type_from_mime_type(mime.mb_str()));

    GIcon *gicon(g_content_type_get_icon(type));
    if (gicon) {
        GtkIconTheme *theme(gtk_icon_theme_get_default());
        if (theme) {
            GtkIconInfo *ticon(gtk_icon_theme_lookup_by_gicon(theme, gicon, 256, GTK_ICON_LOOKUP_NO_SVG));

            if (ticon) {
                icon = wxString(gtk_icon_info_get_filename(ticon));
                g_object_unref(ticon);
            }
        }
        g_object_unref(gicon);
    }
    return icon;
}

wxMimeTypesManagerImpl *wxGTKMimeTypesManagerFactory::CreateMimeTypesManagerImpl()
{
    return new wxGTKMimeTypesManagerImpl();
}

#endif
  // wxUSE_MIMETYPE && wxUSE_FILE
