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

#include "wx/gtk/private/wrapgtk.h"

#include "wx/gtk/private/string.h"
#include "wx/gtk/private/object.h"

#if defined(__UNIX__)
wxString wxGTKMimeTypesManagerImpl::GetIconFromMimeType(const wxString& mime)
{
    wxString icon;
#if GTK_CHECK_VERSION(2,14,0)
    if (!wx_is_at_least_gtk2(14))
        return icon;

    wxGtkString type(g_content_type_from_mime_type(mime.utf8_str()));

    wxGtkObject<GIcon> gicon(g_content_type_get_icon(type));
    if ( !gicon )
        return icon;

    GtkIconTheme *theme(gtk_icon_theme_get_default());
    if ( !theme )
        return icon;

    for (int flags = GTK_ICON_LOOKUP_NO_SVG; ; flags = 0)
    {
        GtkIconInfo* const giconinfo = gtk_icon_theme_lookup_by_gicon
                                       (
                                            theme,
                                            gicon,
                                            32,
                                            GtkIconLookupFlags(flags)
                                       );

        if ( giconinfo )
        {
            const char* filename = gtk_icon_info_get_filename(giconinfo);

            // If icon theme has only SVG files, icon lookup with GTK_ICON_LOOKUP_NO_SVG
            // may use a builtin icon, in which case the GtkIconInfo filename is a
            // resource path of the form "/org/gtk/libgtk/icons/...", not a file path.
            if (filename && strncmp(filename, "/org/", 5) != 0)
                icon = wxString::FromUTF8(filename);

            wxGCC_WARNING_SUPPRESS(deprecated-declarations)
            // g_object_unref() cannot be used for GtkIconInfo prior to GTK 3.8
            gtk_icon_info_free(giconinfo);
            wxGCC_WARNING_RESTORE()
        }
        if (!icon.empty() || flags == 0)
            break;
        // else try again without GTK_ICON_LOOKUP_NO_SVG
    }
#endif // GTK_CHECK_VERSION(2,14,0)
    return icon;
}
#endif // defined(__UNIX__)

wxMimeTypesManagerImpl *wxGTKMimeTypesManagerFactory::CreateMimeTypesManagerImpl()
{
    return new wxGTKMimeTypesManagerImpl();
}

#endif // wxUSE_MIMETYPE
