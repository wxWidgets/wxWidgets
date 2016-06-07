/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/mimetype.cpp
// Purpose:     classes and functions to manage MIME types
// Author:      Hans Mackowiak
// Created:     2016-06-05
// Copyright:   (c) 2016 Hans Mackowiak <hanmac@gmx.de>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_MIMETYPE

#include "wx/gtk/mimetype.h"

#include <gio/gio.h>
#include <gtk/gtk.h>

#include "wx/gtk/private/string.h"
#include "wx/gtk/private/object.h"

wxString wxGTKMimeTypesManagerImpl::GetIconFromMimeType(const wxString& mime)
{
    wxGtkString type(g_content_type_from_mime_type(mime.utf8_str()));

    wxGtkObject<GIcon> gicon(g_content_type_get_icon(type));
    if ( !gicon )
        return wxString();

    GtkIconTheme *theme(gtk_icon_theme_get_default());
    if ( !theme )
        return wxString();

    wxGtkObject<GtkIconInfo> giconinfo(gtk_icon_theme_lookup_by_gicon
                                       (
                                            theme,
                                            gicon,
                                            256,
                                            GTK_ICON_LOOKUP_NO_SVG
                                       ));
    if ( !giconinfo )
        return wxString();

    return wxString::FromUTF8(gtk_icon_info_get_filename(giconinfo));
}

wxMimeTypesManagerImpl *wxGTKMimeTypesManagerFactory::CreateMimeTypesManagerImpl()
{
    return new wxGTKMimeTypesManagerImpl();
}

#endif // wxUSE_MIMETYPE
