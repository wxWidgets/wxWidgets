/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/artgtk.cpp
// Purpose:     stock wxArtProvider instance with native GTK+ stock icons
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004-08-22
// Copyright:   (c) Vaclav Slavik, 2004
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/artprov.h"

#include "wx/gtk/private.h"

#if !GTK_CHECK_VERSION(2,8,0)
    #define GTK_STOCK_FULLSCREEN "gtk-fullscreen"
#endif

// ----------------------------------------------------------------------------
// wxGTK2ArtProvider
// ----------------------------------------------------------------------------

class wxGTK2ArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                  const wxSize& size) override;
    virtual wxIconBundle CreateIconBundle(const wxArtID& id,
                                          const wxArtClient& client) override;
};

/*static*/ void wxArtProvider::InitNativeProvider()
{
    PushBack(new wxGTK2ArtProvider);
}

// ----------------------------------------------------------------------------
// CreateBitmap routine
// ----------------------------------------------------------------------------

namespace
{

#ifdef __WXGTK3__
    #define ART(wxId, unused, themeId) wxId, themeId,
#else
    #define ART(wxId, stockId, unused) wxId, stockId,
#endif

const wxString wxId2Gtk[] = {
    ART(wxART_ERROR,            GTK_STOCK_DIALOG_ERROR, "dialog-error")
    ART(wxART_INFORMATION,      GTK_STOCK_DIALOG_INFO, "dialog-information")
    ART(wxART_WARNING,          GTK_STOCK_DIALOG_WARNING, "dialog-warning")
    ART(wxART_QUESTION,         GTK_STOCK_DIALOG_QUESTION, "dialog-question")

    //ART(wxART_HELP_SIDE_PANEL,)
    ART(wxART_HELP_SETTINGS,    GTK_STOCK_SELECT_FONT, "preferences-desktop-font")
    //ART(wxART_HELP_BOOK,      )
    ART(wxART_HELP_FOLDER,      GTK_STOCK_DIRECTORY, "folder")
    ART(wxART_HELP_PAGE,        GTK_STOCK_FILE, "text-x-generic")
    ART(wxART_MISSING_IMAGE,    GTK_STOCK_MISSING_IMAGE, "image-missing")
    ART(wxART_ADD_BOOKMARK,     GTK_STOCK_ADD, "list-add")
    ART(wxART_DEL_BOOKMARK,     GTK_STOCK_REMOVE, "list-remove")
    ART(wxART_GO_BACK,          GTK_STOCK_GO_BACK, "go-previous")
    ART(wxART_GO_FORWARD,       GTK_STOCK_GO_FORWARD, "go-next")
    ART(wxART_GO_UP,            GTK_STOCK_GO_UP, "go-up")
    ART(wxART_GO_DOWN,          GTK_STOCK_GO_DOWN, "go-down")
    ART(wxART_GO_TO_PARENT,     GTK_STOCK_GO_UP, "go-up")
    ART(wxART_GO_HOME,          GTK_STOCK_HOME, "go-home")
    ART(wxART_GOTO_FIRST,       GTK_STOCK_GOTO_FIRST, "go-first")
    ART(wxART_GOTO_LAST,        GTK_STOCK_GOTO_LAST, "go-last")
    ART(wxART_FILE_OPEN,        GTK_STOCK_OPEN, "document-open")
    ART(wxART_PRINT,            GTK_STOCK_PRINT, "document-print")
    ART(wxART_HELP,             GTK_STOCK_HELP, "help-contents")
    ART(wxART_TIP,              GTK_STOCK_DIALOG_INFO, "dialog-information")
    //ART(wxART_REPORT_VIEW,    )
    //ART(wxART_LIST_VIEW,      )
    ART(wxART_NEW_DIR,          "folder-new", "folder-new")
    ART(wxART_FOLDER,           GTK_STOCK_DIRECTORY, "folder")
    ART(wxART_FOLDER_OPEN,      "folder-open", "folder-open")
    //ART(wxART_GO_DIR_UP,      )
    ART(wxART_EXECUTABLE_FILE,  GTK_STOCK_EXECUTE, "system-run")
    ART(wxART_NORMAL_FILE,      GTK_STOCK_FILE, "text-x-generic")
    ART(wxART_TICK_MARK,        GTK_STOCK_APPLY, "gtk-apply")
    ART(wxART_CROSS_MARK,       GTK_STOCK_CANCEL, "gtk-cancel")

    ART(wxART_FLOPPY,           GTK_STOCK_FLOPPY, "media-floppy")
    ART(wxART_CDROM,            GTK_STOCK_CDROM, "media-optical")
    ART(wxART_HARDDISK,         GTK_STOCK_HARDDISK, "drive-harddisk")
    ART(wxART_REMOVABLE,        "drive-removable-media", "drive-removable-media")

    ART(wxART_FILE_SAVE,        GTK_STOCK_SAVE, "document-save")
    ART(wxART_FILE_SAVE_AS,     GTK_STOCK_SAVE_AS, "document-save-as")

    ART(wxART_COPY,             GTK_STOCK_COPY, "edit-copy")
    ART(wxART_CUT,              GTK_STOCK_CUT, "edit-cut")
    ART(wxART_PASTE,            GTK_STOCK_PASTE, "edit-paste")
    ART(wxART_DELETE,           GTK_STOCK_DELETE, "edit-delete")
    ART(wxART_NEW,              GTK_STOCK_NEW, "document-new")

    ART(wxART_UNDO,             GTK_STOCK_UNDO, "edit-undo")
    ART(wxART_REDO,             GTK_STOCK_REDO, "edit-redo")

    ART(wxART_PLUS,             GTK_STOCK_ADD, "list-add")
    ART(wxART_MINUS,            GTK_STOCK_REMOVE, "list-remove")

    ART(wxART_CLOSE,            GTK_STOCK_CLOSE, "window-close")
    ART(wxART_QUIT,             GTK_STOCK_QUIT, "application-exit")

    ART(wxART_FIND,             GTK_STOCK_FIND, "edit-find")
    ART(wxART_FIND_AND_REPLACE, GTK_STOCK_FIND_AND_REPLACE, "edit-find-replace")
    ART(wxART_FULL_SCREEN,      GTK_STOCK_FULLSCREEN, "view-fullscreen")
    ART(wxART_REFRESH,          GTK_STOCK_REFRESH, "gtk-refresh")
    ART(wxART_STOP,             GTK_STOCK_STOP, "gtk-stop")

    ART(wxART_EDIT,             "accessories-text-editor", "accessories-text-editor")
};

#undef ART

wxString wxArtIDToStock(const wxArtID& id)
{
    // allow passing GTK+ stock IDs to wxArtProvider -- if a recognized wx
    // ID wasn't found, pass it to GTK+ in the hope it is a GTK+ or theme
    // icon name:
    wxString ret(id);

    for (unsigned i = 0; i < WXSIZEOF(wxId2Gtk); i += 2)
    {
        if (id == wxId2Gtk[i])
        {
            ret = wxId2Gtk[i + 1];
            break;
        }
    }
    return ret;
}

GtkIconSize ArtClientToIconSize(const wxArtClient& client)
{
    if (client == wxART_TOOLBAR)
        return GTK_ICON_SIZE_LARGE_TOOLBAR;
    else if (client == wxART_MENU || client == wxART_FRAME_ICON)
        return GTK_ICON_SIZE_MENU;
    else if (client == wxART_CMN_DIALOG || client == wxART_MESSAGE_BOX)
        return GTK_ICON_SIZE_DIALOG;
    else if (client == wxART_BUTTON)
        return GTK_ICON_SIZE_BUTTON;
    else
        return GTK_ICON_SIZE_INVALID; // this is arbitrary
}

GtkIconSize FindClosestIconSize(const wxSize& size)
{
    #define NUM_SIZES 6
    static struct
    {
        GtkIconSize icon;
        gint x, y;
    } s_sizes[NUM_SIZES];
    static bool s_sizesInitialized = false;

    if (!s_sizesInitialized)
    {
        s_sizes[0].icon = GTK_ICON_SIZE_MENU;
        s_sizes[1].icon = GTK_ICON_SIZE_SMALL_TOOLBAR;
        s_sizes[2].icon = GTK_ICON_SIZE_LARGE_TOOLBAR;
        s_sizes[3].icon = GTK_ICON_SIZE_BUTTON;
        s_sizes[4].icon = GTK_ICON_SIZE_DND;
        s_sizes[5].icon = GTK_ICON_SIZE_DIALOG;
        for (size_t i = 0; i < NUM_SIZES; i++)
        {
            gtk_icon_size_lookup(s_sizes[i].icon,
                                 &s_sizes[i].x, &s_sizes[i].y);
        }
        s_sizesInitialized = true;
    }

    GtkIconSize best = GTK_ICON_SIZE_DIALOG; // presumably largest
    unsigned distance = INT_MAX;
    for (size_t i = 0; i < NUM_SIZES; i++)
    {
        // only use larger bitmaps, scaling down looks better than scaling up:
        if (size.x > s_sizes[i].x || size.y > s_sizes[i].y)
            continue;

        unsigned dist = (size.x - s_sizes[i].x) * (size.x - s_sizes[i].x) +
                        (size.y - s_sizes[i].y) * (size.y - s_sizes[i].y);
        if (dist == 0)
            return s_sizes[i].icon;
        else if (dist < distance)
        {
            distance = dist;
            best = s_sizes[i].icon;
        }
    }
    return best;
}

#ifndef __WXGTK4__
GdkPixbuf *CreateStockIcon(const char *stockid, GtkIconSize size)
{
    // FIXME: This code is not 100% correct, because stock pixmap are
    //        context-dependent and may be affected by theme engine, the
    //        correct value can only be obtained for given GtkWidget object.
    //
    //        Fool-proof implementation of stock bitmaps would extend wxBitmap
    //        with "stock-id" representation (in addition to pixmap and pixbuf
    //        ones) and would convert it to pixbuf when rendered.

    GtkWidget* widget = wxGTKPrivate::GetButtonWidget();
#ifdef __WXGTK3__
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    GtkStyleContext* sc = gtk_widget_get_style_context(widget);
    GtkIconSet* iconset = gtk_style_context_lookup_icon_set(sc, stockid);
    GdkPixbuf* pixbuf = nullptr;
    if (iconset)
        pixbuf = gtk_icon_set_render_icon_pixbuf(iconset, sc, size);
    return pixbuf;
    wxGCC_WARNING_RESTORE()
#else
    GtkStyle* style = gtk_widget_get_style(widget);
    GtkIconSet* iconset = gtk_style_lookup_icon_set(style, stockid);

    if (!iconset)
        return nullptr;

    return gtk_icon_set_render_icon(iconset, style,
                                    gtk_widget_get_default_direction(),
                                    GTK_STATE_NORMAL, size, nullptr, nullptr);
#endif
}
#endif // !__WXGTK4__

GdkPixbuf *CreateThemeIcon(const char *iconname, int size)
{
    return gtk_icon_theme_load_icon
           (
               gtk_icon_theme_get_default(),
               iconname,
               size,
               (GtkIconLookupFlags)0,
               nullptr
           );
}


// creates either stock or theme icon
GdkPixbuf *CreateGtkIcon(const char *icon_name,
                         GtkIconSize stock_size, const wxSize& pixel_size)
{
#ifndef __WXGTK4__
    // try stock GTK+ icon first
    GdkPixbuf *pixbuf = CreateStockIcon(icon_name, stock_size);
    if ( pixbuf )
        return pixbuf;
#endif

    // if that fails, try theme icon
    wxSize size(pixel_size);
    if ( pixel_size == wxDefaultSize )
        gtk_icon_size_lookup(stock_size, &size.x, &size.y);
    return CreateThemeIcon(icon_name, size.x);
}

template<typename SizeType, typename LoaderFunc>
wxIconBundle DoCreateIconBundle(const char *stockid,
                                const SizeType *sizes_from,
                                const SizeType *sizes_to,
                                LoaderFunc get_icon)

{
    wxIconBundle bundle;

    for ( const SizeType *i = sizes_from; i != sizes_to; ++i )
    {
        GdkPixbuf *pixbuf = get_icon(stockid, *i);
        if ( !pixbuf )
            continue;

        wxIcon icon;
        icon.CopyFromBitmap(wxBitmap(pixbuf));
        bundle.AddIcon(icon);
    }

    return bundle;
}

} // anonymous namespace

wxBitmap wxGTK2ArtProvider::CreateBitmap(const wxArtID& id,
                                         const wxArtClient& client,
                                         const wxSize& size)
{
    const wxString stockid = wxArtIDToStock(id);

    GtkIconSize stocksize = (size == wxDefaultSize) ?
                                ArtClientToIconSize(client) :
                                FindClosestIconSize(size);
    // we must have some size, this is arbitrary
    if (stocksize == GTK_ICON_SIZE_INVALID)
        stocksize = GTK_ICON_SIZE_BUTTON;

    return wxBitmap(CreateGtkIcon(stockid.utf8_str(), stocksize, size));
}

wxIconBundle
wxGTK2ArtProvider::CreateIconBundle(const wxArtID& id,
                                    const wxArtClient& WXUNUSED(client))
{
    wxIconBundle bundle;
    const wxString stockid = wxArtIDToStock(id);

#ifndef __WXGTK4__
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    // try to load the bundle as stock icon first
    GtkWidget* widget = wxGTKPrivate::GetButtonWidget();
#ifdef __WXGTK3__
    GtkStyleContext* sc = gtk_widget_get_style_context(widget);
    GtkIconSet* iconset = gtk_style_context_lookup_icon_set(sc, stockid.utf8_str());
#else
    GtkStyle* style = gtk_widget_get_style(widget);
    GtkIconSet* iconset = gtk_style_lookup_icon_set(style, stockid.utf8_str());
#endif
    if ( iconset )
    {
        GtkIconSize *sizes;
        gint n_sizes;
        gtk_icon_set_get_sizes(iconset, &sizes, &n_sizes);
        bundle = DoCreateIconBundle
                              (
                                  stockid.utf8_str(),
                                  sizes, sizes + n_sizes,
                                  &CreateStockIcon
                              );
        g_free(sizes);
        return bundle;
    }
    wxGCC_WARNING_RESTORE()
#endif // !__WXGTK4__

    // otherwise try icon themes
    gint *sizes = gtk_icon_theme_get_icon_sizes
                  (
                      gtk_icon_theme_get_default(),
                      stockid.utf8_str()
                  );
    if ( !sizes )
        return bundle;

    gint *last = sizes;
    while ( *last )
        last++;

    bundle = DoCreateIconBundle
                          (
                              stockid.utf8_str(),
                              sizes, last,
                              &CreateThemeIcon
                          );
    g_free(sizes);

    return bundle;
}

// ----------------------------------------------------------------------------
// wxArtProvider::GetNativeSizeHint()
// ----------------------------------------------------------------------------

/*static*/
wxSize wxArtProvider::GetNativeDIPSizeHint(const wxArtClient& client)
{
    // Gtk has specific sizes for each client, see artgtk.cpp
    GtkIconSize gtk_size = ArtClientToIconSize(client);
    // no size hints for this client
    if (gtk_size == GTK_ICON_SIZE_INVALID)
        return wxDefaultSize;
    gint width, height;
    gtk_icon_size_lookup( gtk_size, &width, &height);
    return wxSize(width, height);
}
