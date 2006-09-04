///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/stockitem.cpp
// Purpose:     Stock buttons, menu and toolbar items labels
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004-08-15
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2004
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/stockitem.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/utils.h" // for wxStripMenuCodes()
#endif

bool wxIsStockID(wxWindowID id)
{
    switch (id)
    {
        case wxID_ABOUT:
        case wxID_ADD:
        case wxID_APPLY:
        case wxID_BOLD:
        case wxID_CANCEL:
        case wxID_CLEAR:
        case wxID_CLOSE:
        case wxID_COPY:
        case wxID_CUT:
        case wxID_DELETE:
        case wxID_EDIT:
        case wxID_FIND:
        case wxID_FILE:
        case wxID_REPLACE:
        case wxID_BACKWARD:
        case wxID_DOWN:
        case wxID_FORWARD:
        case wxID_UP:
        case wxID_HELP:
        case wxID_HOME:
        case wxID_INDENT:
        case wxID_INDEX:
        case wxID_ITALIC:
        case wxID_JUSTIFY_CENTER:
        case wxID_JUSTIFY_FILL:
        case wxID_JUSTIFY_LEFT:
        case wxID_JUSTIFY_RIGHT:
        case wxID_NEW:
        case wxID_NO:
        case wxID_OK:
        case wxID_OPEN:
        case wxID_PASTE:
        case wxID_PREFERENCES:
        case wxID_PRINT:
        case wxID_PREVIEW:
        case wxID_PROPERTIES:
        case wxID_EXIT:
        case wxID_REDO:
        case wxID_REFRESH:
        case wxID_REMOVE:
        case wxID_REVERT_TO_SAVED:
        case wxID_SAVE:
        case wxID_SAVEAS:
        case wxID_SELECTALL:
        case wxID_STOP:
        case wxID_UNDELETE:
        case wxID_UNDERLINE:
        case wxID_UNDO:
        case wxID_UNINDENT:
        case wxID_YES:
        case wxID_ZOOM_100:
        case wxID_ZOOM_FIT:
        case wxID_ZOOM_IN:
        case wxID_ZOOM_OUT:
            return true;

        default:
            return false;
    }
}

wxString wxGetStockLabel(wxWindowID id, long flags)
{
    wxString stockLabel;

    #define STOCKITEM(stockid, label) \
        case stockid:                 \
            stockLabel = label;       \
            break;

    switch (id)
    {
        STOCKITEM(wxID_ABOUT,               _("&About"))
        STOCKITEM(wxID_ADD,                 _("Add"))
        STOCKITEM(wxID_APPLY,               _("&Apply"))
        STOCKITEM(wxID_BOLD,                _("&Bold"))
        STOCKITEM(wxID_CANCEL,              _("&Cancel"))
        STOCKITEM(wxID_CLEAR,               _("&Clear"))
        STOCKITEM(wxID_CLOSE,               _("&Close"))
        STOCKITEM(wxID_COPY,                _("&Copy"))
        STOCKITEM(wxID_CUT,                 _("Cu&t"))
        STOCKITEM(wxID_DELETE,              _("&Delete"))
        STOCKITEM(wxID_EDIT,                _("&Edit"))
        STOCKITEM(wxID_FIND,                _("&Find"))
        STOCKITEM(wxID_FILE,                _("&File"))
        STOCKITEM(wxID_REPLACE,             _("Rep&lace"))
        STOCKITEM(wxID_BACKWARD,            _("&Back"))
        STOCKITEM(wxID_DOWN,                _("&Down"))
        STOCKITEM(wxID_FORWARD,             _("&Forward"))
        STOCKITEM(wxID_UP,                  _("&Up"))
        STOCKITEM(wxID_HELP,                _("&Help"))
        STOCKITEM(wxID_HOME,                _("&Home"))
        STOCKITEM(wxID_INDENT,              _("Indent"))
        STOCKITEM(wxID_INDEX,               _("&Index"))
        STOCKITEM(wxID_ITALIC,              _("&Italic"))
        STOCKITEM(wxID_JUSTIFY_CENTER,      _("Centered"))
        STOCKITEM(wxID_JUSTIFY_FILL,        _("Justified"))
        STOCKITEM(wxID_JUSTIFY_LEFT,        _("Align Left"))
        STOCKITEM(wxID_JUSTIFY_RIGHT,       _("Align Right"))
        STOCKITEM(wxID_NEW,                 _("&New"))
        STOCKITEM(wxID_NO,                  _("&No"))
        STOCKITEM(wxID_OK,                  _("&OK"))
        STOCKITEM(wxID_OPEN,                _("&Open"))
        STOCKITEM(wxID_PASTE,               _("&Paste"))
        STOCKITEM(wxID_PREFERENCES,         _("&Preferences"))
        STOCKITEM(wxID_PRINT,               _("&Print"))
        STOCKITEM(wxID_PREVIEW,             _("Print previe&w"))
        STOCKITEM(wxID_PROPERTIES,          _("&Properties"))
        STOCKITEM(wxID_EXIT,                _("&Quit"))
        STOCKITEM(wxID_REDO,                _("&Redo"))
        STOCKITEM(wxID_REFRESH,             _("Refresh"))
        STOCKITEM(wxID_REMOVE,              _("Remove"))
        STOCKITEM(wxID_REVERT_TO_SAVED,     _("Revert to Saved"))
        STOCKITEM(wxID_SAVE,                _("&Save"))
        STOCKITEM(wxID_SAVEAS,              _("Save &As..."))
        STOCKITEM(wxID_SELECTALL,           _("Select all"))
        STOCKITEM(wxID_STOP,                _("&Stop"))
        STOCKITEM(wxID_UNDELETE,            _("Undelete"))
        STOCKITEM(wxID_UNDERLINE,           _("&Underline"))
        STOCKITEM(wxID_UNDO,                _("&Undo"))
        STOCKITEM(wxID_UNINDENT,            _("&Unindent"))
        STOCKITEM(wxID_YES,                 _("&Yes"))
        STOCKITEM(wxID_ZOOM_100,            _("&Actual Size"))
        STOCKITEM(wxID_ZOOM_FIT,            _("Zoom to &Fit"))
        STOCKITEM(wxID_ZOOM_IN,             _("Zoom &In"))
        STOCKITEM(wxID_ZOOM_OUT,            _("Zoom &Out"))

        default:
            wxFAIL_MSG( _T("invalid stock item ID") );
            break;
    };

    #undef STOCKITEM

    if(flags & wxSTOCK_WITH_MNEMONIC)
    {
        stockLabel = wxStripMenuCodes( stockLabel );
    }

#if wxUSE_ACCEL

    if (!stockLabel.empty() && (flags & wxSTOCK_WITH_ACCELERATOR))
    {
        stockLabel += _T("\t");

        wxAcceleratorEntry accel = wxGetStockAccelerator(id);
        if (accel.IsOk())
            stockLabel += accel.ToString();
    }

#endif // wxUSE_ACCEL

    return stockLabel;
}

#if wxUSE_ACCEL

wxAcceleratorEntry wxGetStockAccelerator(wxWindowID id)
{
    wxAcceleratorEntry ret;

    #define STOCKITEM(stockid, flags, keycode)      \
        case stockid:                               \
            ret.Set(flags, keycode, stockid);       \
            break;

    switch (id)
    {
        STOCKITEM(wxID_COPY,                wxACCEL_CTRL,'C')
        STOCKITEM(wxID_CUT,                 wxACCEL_CTRL,'X')
        STOCKITEM(wxID_FIND,                wxACCEL_CTRL,'F')
        STOCKITEM(wxID_REPLACE,             wxACCEL_CTRL,'R')
        STOCKITEM(wxID_HELP,                wxACCEL_CTRL,'H')
        STOCKITEM(wxID_NEW,                 wxACCEL_CTRL,'N')
        STOCKITEM(wxID_OPEN,                wxACCEL_CTRL,'O')
        STOCKITEM(wxID_PASTE,               wxACCEL_CTRL,'V')
        STOCKITEM(wxID_SAVE,                wxACCEL_CTRL,'S')

        default:
            // set the wxAcceleratorEntry to return into an invalid state:
            // there's no stock accelerator for that.
            ret.Set(0, 0, id);
            break;
    };

    #undef STOCKITEM

    // always use wxAcceleratorEntry::IsOk on returned value !
    return ret;
}

#endif // wxUSE_ACCEL

bool wxIsStockLabel(wxWindowID id, const wxString& label)
{
    if (label.empty())
        return true;

    wxString stock = wxGetStockLabel(id);

    if (label == stock)
        return true;

    stock.Replace(_T("&"), wxEmptyString);
    if (label == stock)
        return true;

    return false;
}


#ifdef __WXGTK20__

#include <gtk/gtk.h>

const char *wxGetStockGtkID(wxWindowID id)
{
    #define STOCKITEM(wx,gtk)      \
        case wx:                   \
            return gtk;

    #define STOCKITEM_MISSING(wx)  \
        case wx:                 \
            return NULL;

    #if GTK_CHECK_VERSION(2,4,0)
        #define STOCKITEM_24(wx,gtk) STOCKITEM(wx,gtk)
    #else
        #define STOCKITEM_24(wx,gtk) STOCKITEM_MISSING(wx)
    #endif

    #if GTK_CHECK_VERSION(2,6,0)
        #define STOCKITEM_26(wx,gtk) STOCKITEM(wx,gtk)
    #else
        #define STOCKITEM_26(wx,gtk) STOCKITEM_MISSING(wx)
    #endif

    #if GTK_CHECK_VERSION(2,10,0)
        #define STOCKITEM_210(wx,gtk) STOCKITEM(wx,gtk)
    #else
        #define STOCKITEM_210(wx,gtk) STOCKITEM_MISSING(wx)
    #endif


    switch (id)
    {
        STOCKITEM_26(wxID_ABOUT,         GTK_STOCK_ABOUT)
        STOCKITEM(wxID_ADD,              GTK_STOCK_ADD)
        STOCKITEM(wxID_APPLY,            GTK_STOCK_APPLY)
        STOCKITEM(wxID_BOLD,             GTK_STOCK_BOLD)
        STOCKITEM(wxID_CANCEL,           GTK_STOCK_CANCEL)
        STOCKITEM(wxID_CLEAR,            GTK_STOCK_CLEAR)
        STOCKITEM(wxID_CLOSE,            GTK_STOCK_CLOSE)
        STOCKITEM(wxID_COPY,             GTK_STOCK_COPY)
        STOCKITEM(wxID_CUT,              GTK_STOCK_CUT)
        STOCKITEM(wxID_DELETE,           GTK_STOCK_DELETE)
        STOCKITEM_26(wxID_EDIT,          GTK_STOCK_EDIT)
        STOCKITEM(wxID_FIND,             GTK_STOCK_FIND)
        STOCKITEM_26(wxID_FILE,          GTK_STOCK_FILE)
        STOCKITEM(wxID_REPLACE,          GTK_STOCK_FIND_AND_REPLACE)
        STOCKITEM(wxID_BACKWARD,         GTK_STOCK_GO_BACK)
        STOCKITEM(wxID_DOWN,             GTK_STOCK_GO_DOWN)
        STOCKITEM(wxID_FORWARD,          GTK_STOCK_GO_FORWARD)
        STOCKITEM(wxID_UP,               GTK_STOCK_GO_UP)
        STOCKITEM(wxID_HELP,             GTK_STOCK_HELP)
        STOCKITEM(wxID_HOME,             GTK_STOCK_HOME)
        STOCKITEM_24(wxID_INDENT,        GTK_STOCK_INDENT)
        STOCKITEM(wxID_INDEX,            GTK_STOCK_INDEX)
        STOCKITEM(wxID_ITALIC,           GTK_STOCK_ITALIC)
        STOCKITEM(wxID_JUSTIFY_CENTER,   GTK_STOCK_JUSTIFY_CENTER)
        STOCKITEM(wxID_JUSTIFY_FILL,     GTK_STOCK_JUSTIFY_FILL)
        STOCKITEM(wxID_JUSTIFY_LEFT,     GTK_STOCK_JUSTIFY_LEFT)
        STOCKITEM(wxID_JUSTIFY_RIGHT,    GTK_STOCK_JUSTIFY_RIGHT)
        STOCKITEM(wxID_NEW,              GTK_STOCK_NEW)
        STOCKITEM(wxID_NO,               GTK_STOCK_NO)
        STOCKITEM(wxID_OK,               GTK_STOCK_OK)
        STOCKITEM(wxID_OPEN,             GTK_STOCK_OPEN)
        STOCKITEM(wxID_PASTE,            GTK_STOCK_PASTE)
        STOCKITEM(wxID_PREFERENCES,      GTK_STOCK_PREFERENCES)
        STOCKITEM(wxID_PRINT,            GTK_STOCK_PRINT)
        STOCKITEM(wxID_PREVIEW,          GTK_STOCK_PRINT_PREVIEW)
        STOCKITEM(wxID_PROPERTIES,       GTK_STOCK_PROPERTIES)
        STOCKITEM(wxID_EXIT,             GTK_STOCK_QUIT)
        STOCKITEM(wxID_REDO,             GTK_STOCK_REDO)
        STOCKITEM(wxID_REFRESH,          GTK_STOCK_REFRESH)
        STOCKITEM(wxID_REMOVE,           GTK_STOCK_REMOVE)
        STOCKITEM(wxID_REVERT_TO_SAVED,  GTK_STOCK_REVERT_TO_SAVED)
        STOCKITEM(wxID_SAVE,             GTK_STOCK_SAVE)
        STOCKITEM(wxID_SAVEAS,           GTK_STOCK_SAVE_AS)
        STOCKITEM_210(wxID_SELECTALL,    GTK_STOCK_SELECT_ALL)
        STOCKITEM(wxID_STOP,             GTK_STOCK_STOP)
        STOCKITEM(wxID_UNDELETE,         GTK_STOCK_UNDELETE)
        STOCKITEM(wxID_UNDERLINE,        GTK_STOCK_UNDERLINE)
        STOCKITEM(wxID_UNDO,             GTK_STOCK_UNDO)
        STOCKITEM_24(wxID_UNINDENT,      GTK_STOCK_UNINDENT)
        STOCKITEM(wxID_YES,              GTK_STOCK_YES)
        STOCKITEM(wxID_ZOOM_100,         GTK_STOCK_ZOOM_100)
        STOCKITEM(wxID_ZOOM_FIT,         GTK_STOCK_ZOOM_FIT)
        STOCKITEM(wxID_ZOOM_IN,          GTK_STOCK_ZOOM_IN)
        STOCKITEM(wxID_ZOOM_OUT,         GTK_STOCK_ZOOM_OUT)

        default:
            wxFAIL_MSG( _T("invalid stock item ID") );
            break;
    };

    #undef STOCKITEM

    return NULL;
}

bool wxGetStockGtkAccelerator(const char *id, GdkModifierType *mod, guint *key)
{
    GtkStockItem stock_item;
    if (gtk_stock_lookup (id, &stock_item))
    {
        if (key) *key = stock_item.keyval;
        if (mod) *mod = stock_item.modifier;

        // some GTK stock items have zero values for the keyval;
        // it means that they do not have an accelerator...
        if (stock_item.keyval)
            return true;
    }

    return false;
}

#endif // __WXGTK20__
