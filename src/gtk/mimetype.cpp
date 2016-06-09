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
    wxString icon;
#if GTK_CHECK_VERSION(2,14,0)
#ifndef __WXGTK3__
    if (gtk_check_version(2,14,0))
        return icon;
#endif
    wxGtkString type(g_content_type_from_mime_type(mime.utf8_str()));

    wxGtkObject<GIcon> gicon(g_content_type_get_icon(type));
    if ( !gicon )
        return icon;

    GtkIconTheme *theme(gtk_icon_theme_get_default());
    if ( !theme )
        return icon;

    // Notice that we can't use wxGtkObject here because a special function
    // needs to be used for freeing this object prior to GTK+ 3.8.
    GtkIconInfo* const giconinfo = gtk_icon_theme_lookup_by_gicon
                                   (
                                        theme,
                                        gicon,
                                        256,
                                        GTK_ICON_LOOKUP_NO_SVG
                                   );

    if ( giconinfo )
    {
        icon = wxString::FromUTF8(gtk_icon_info_get_filename(giconinfo));

        wxGCC_WARNING_SUPPRESS(deprecated-declarations)
        gtk_icon_info_free(giconinfo);
        wxGCC_WARNING_RESTORE()
    }
#endif // GTK_CHECK_VERSION(2,14,0)
    return icon;
}

wxMimeTypesManagerImpl *wxGTKMimeTypesManagerFactory::CreateMimeTypesManagerImpl()
{
    return new wxGTKMimeTypesManagerImpl();
}

#endif // wxUSE_MIMETYPE
