/////////////////////////////////////////////////////////////////////////////
// Name:        artstd.cpp
// Purpose:     stock wxArtProvider instance with native GTK+ stock icons
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004-08-22
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2004
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#if defined(__WXGTK20__) && !defined(__WXUNIVERSAL__)

#include "wx/artprov.h"

#include <gtk/gtk.h>

// compatibility with older GTK+ versions:
#ifndef GTK_STOCK_FILE
    #define GTK_STOCK_FILE "gtk-file"
#endif
#ifndef GTK_STOCK_DIRECTORY
    #define GTK_STOCK_DIRECTORY "gtk-directory"
#endif


// ----------------------------------------------------------------------------
// wxGTK2ArtProvider
// ----------------------------------------------------------------------------

class wxGTK2ArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                  const wxSize& size);
};

/*static*/ void wxArtProvider::InitNativeProvider()
{
    wxArtProvider::PushProvider(new wxGTK2ArtProvider);
}

// ----------------------------------------------------------------------------
// CreateBitmap routine
// ----------------------------------------------------------------------------

static const char *wxArtIDToStock(const wxArtID& id)
{
    #define ART(wxid, gtkid) \
           if (id == wxid) return gtkid;
    
    ART(wxART_ERROR,                               GTK_STOCK_DIALOG_ERROR)
    ART(wxART_INFORMATION,                         GTK_STOCK_DIALOG_INFO)
    ART(wxART_WARNING,                             GTK_STOCK_DIALOG_WARNING)
    ART(wxART_QUESTION,                            GTK_STOCK_DIALOG_QUESTION)
    
    //ART(wxART_HELP_SIDE_PANEL,                     )
    ART(wxART_HELP_SETTINGS,                       GTK_STOCK_SELECT_FONT)
    //ART(wxART_HELP_BOOK,                           )
    ART(wxART_HELP_FOLDER,                         GTK_STOCK_DIRECTORY)
    ART(wxART_HELP_PAGE,                           GTK_STOCK_FILE)
    ART(wxART_MISSING_IMAGE,                       GTK_STOCK_MISSING_IMAGE)
    ART(wxART_ADD_BOOKMARK,                        GTK_STOCK_ADD)
    ART(wxART_DEL_BOOKMARK,                        GTK_STOCK_REMOVE)
    ART(wxART_GO_BACK,                             GTK_STOCK_GO_BACK)
    ART(wxART_GO_FORWARD,                          GTK_STOCK_GO_FORWARD)
    ART(wxART_GO_UP,                               GTK_STOCK_GO_UP)
    ART(wxART_GO_DOWN,                             GTK_STOCK_GO_DOWN)
    ART(wxART_GO_TO_PARENT,                        GTK_STOCK_GO_UP)
    ART(wxART_GO_HOME,                             GTK_STOCK_HOME)
    ART(wxART_FILE_OPEN,                           GTK_STOCK_OPEN)
    ART(wxART_PRINT,                               GTK_STOCK_PRINT)
    ART(wxART_HELP,                                GTK_STOCK_HELP)
    ART(wxART_TIP,                                 GTK_STOCK_DIALOG_INFO)
    //ART(wxART_REPORT_VIEW,                         )
    //ART(wxART_LIST_VIEW,                           )
    //ART(wxART_NEW_DIR,                             )
    ART(wxART_FOLDER,                              GTK_STOCK_DIRECTORY)
    //ART(wxART_GO_DIR_UP,                           )
    ART(wxART_EXECUTABLE_FILE,                     GTK_STOCK_EXECUTE)
    ART(wxART_NORMAL_FILE,                         GTK_STOCK_FILE)
    ART(wxART_TICK_MARK,                           GTK_STOCK_APPLY)
    ART(wxART_CROSS_MARK,                          GTK_STOCK_CANCEL)
    
    return NULL;
    
    #undef ART
}

static GtkIconSize wxArtClientToIconSize(const wxArtClient& client)
{
    if (client == wxART_TOOLBAR)
        return GTK_ICON_SIZE_LARGE_TOOLBAR;
    else if (client == wxART_MENU)
        return GTK_ICON_SIZE_MENU;
    else if (client == wxART_CMN_DIALOG || client == wxART_MESSAGE_BOX)
        return GTK_ICON_SIZE_DIALOG;
    else if (client == wxART_BUTTON)
        return GTK_ICON_SIZE_BUTTON;
    else
        return GTK_ICON_SIZE_BUTTON; // this is arbitrary
}

static GtkIconSize FindClosestIconSize(const wxSize& size)
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

static GdkPixbuf *CreateStockIcon(const char *stockid, GtkIconSize size)
{
    // FIXME: This code is not 100% correct, because stock pixmap are
    //        context-dependent and may be affected by theme engine, the
    //        correct value can only be obtained for given GtkWidget object.
    //        
    //        Fool-proof implementation of stock bitmaps would extend wxBitmap
    //        with "stock-id" representation (in addition to pixmap and pixbuf
    //        ones) and would convert it to pixbuf when rendered.
    
    GtkStyle *style = gtk_widget_get_default_style();
    GtkIconSet *iconset = gtk_style_lookup_icon_set(style, stockid);

    if (!iconset)
        return NULL;

    return gtk_icon_set_render_icon(iconset, style,
                                    gtk_widget_get_default_direction(),
                                    GTK_STATE_NORMAL, size, NULL, NULL);
}

#if GTK_CHECK_VERSION(2,4,0)
static GdkPixbuf *CreateThemeIcon(const char *iconname,
                                  GtkIconSize iconsize, const wxSize& sz)
{
    wxSize size(sz);
    if (size == wxDefaultSize)
    {
        gtk_icon_size_lookup(iconsize, &size.x, &size.y);
    }
    
    return gtk_icon_theme_load_icon(
                    gtk_icon_theme_get_default(),
                    iconname,
                    size.x,
                    (GtkIconLookupFlags)0, NULL);
}
#endif // GTK+ >= 2.4.0

wxBitmap wxGTK2ArtProvider::CreateBitmap(const wxArtID& id,
                                         const wxArtClient& client,
                                         const wxSize& size)
{
    wxCharBuffer stockid = wxArtIDToStock(id);
    GtkIconSize stocksize = (size == wxDefaultSize) ?
                                wxArtClientToIconSize(client) : 
                                FindClosestIconSize(size);

    // allow passing GTK+ stock IDs to wxArtProvider:
    if (!stockid)
        stockid = id.ToAscii();

    GdkPixbuf *pixbuf = CreateStockIcon(stockid, stocksize);

#if GTK_CHECK_VERSION(2,4,0)
    if (!pixbuf)
        pixbuf = CreateThemeIcon(stockid, stocksize, size);
#endif

    if (pixbuf && size != wxDefaultSize &&
        (size.x != gdk_pixbuf_get_width(pixbuf) ||
         size.y != gdk_pixbuf_get_height(pixbuf)))
    {
        GdkPixbuf *p2 = gdk_pixbuf_scale_simple(pixbuf, size.x, size.y,
                                                GDK_INTERP_BILINEAR);
        if (p2)
        {
            gdk_pixbuf_unref(pixbuf);
            pixbuf = p2;
        }
    }
    
    if (!pixbuf)
        return wxNullBitmap;

    wxBitmap bmp;
    bmp.SetWidth(gdk_pixbuf_get_width(pixbuf));
    bmp.SetHeight(gdk_pixbuf_get_height(pixbuf));
    bmp.SetPixbuf(pixbuf);

    return bmp;
}

#endif // defined(__WXGTK20__) && !defined(__WXUNIVERSAL__)
