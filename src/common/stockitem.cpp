///////////////////////////////////////////////////////////////////////////////
// Name:        common/stockitem.cpp
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "stockitem.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/stockitem.h"
#include "wx/intl.h"

wxString wxGetStockItemLabel(wxStockItemID item)
{
    #define STOCKITEM(stockid, label) \
        case stockid:                 \
            return label;             \
            break;

    switch (item)
    {
        STOCKITEM(wxSTOCK_ADD,                 _("Add"))
        STOCKITEM(wxSTOCK_APPLY,               _("&Apply"))
        STOCKITEM(wxSTOCK_BOLD,                _("&Bold"))
        STOCKITEM(wxSTOCK_CANCEL,              _("&Cancel"))
        STOCKITEM(wxSTOCK_CLEAR,               _("&Clear"))
        STOCKITEM(wxSTOCK_CLOSE,               _("&Close"))
        STOCKITEM(wxSTOCK_COPY,                _("&Copy"))
        STOCKITEM(wxSTOCK_CUT,                 _("Cu&t"))
        STOCKITEM(wxSTOCK_DELETE,              _("&Delete"))
        STOCKITEM(wxSTOCK_FIND,                _("&Find"))
        STOCKITEM(wxSTOCK_FIND_AND_REPLACE,    _("Find and rep&lace"))
        STOCKITEM(wxSTOCK_GO_BACK,             _("&Back"))
        STOCKITEM(wxSTOCK_GO_DOWN,             _("&Down"))
        STOCKITEM(wxSTOCK_GO_FORWARD,          _("&Forward"))
        STOCKITEM(wxSTOCK_GO_UP,               _("&Up"))
        STOCKITEM(wxSTOCK_HELP,                _("&Help"))
        STOCKITEM(wxSTOCK_HOME,                _("&Home"))
        STOCKITEM(wxSTOCK_INDENT,              _("Indent"))
        STOCKITEM(wxSTOCK_INDEX,               _("&Index"))
        STOCKITEM(wxSTOCK_ITALIC,              _("&Italic"))
        STOCKITEM(wxSTOCK_JUSTIFY_CENTER,      _("Centered"))
        STOCKITEM(wxSTOCK_JUSTIFY_FILL,        _("Justified"))
        STOCKITEM(wxSTOCK_JUSTIFY_LEFT,        _("Align Left"))
        STOCKITEM(wxSTOCK_JUSTIFY_RIGHT,       _("Align Right"))
        STOCKITEM(wxSTOCK_NEW,                 _("&New"))
        STOCKITEM(wxSTOCK_NO,                  _("&No"))
        STOCKITEM(wxSTOCK_OK,                  _("&OK"))
        STOCKITEM(wxSTOCK_OPEN,                _("&Open"))
        STOCKITEM(wxSTOCK_PASTE,               _("&Paste"))
        STOCKITEM(wxSTOCK_PREFERENCES,         _("&Preferences"))
        STOCKITEM(wxSTOCK_PRINT,               _("&Print"))
        STOCKITEM(wxSTOCK_PRINT_PREVIEW,       _("Print previe&w"))
        STOCKITEM(wxSTOCK_PROPERTIES,          _("&Properties"))
        STOCKITEM(wxSTOCK_QUIT,                _("&Quit"))
        STOCKITEM(wxSTOCK_REDO,                _("&Redo"))
        STOCKITEM(wxSTOCK_REFRESH,             _("Refresh"))
        STOCKITEM(wxSTOCK_REMOVE,              _("Remove"))
        STOCKITEM(wxSTOCK_REVERT_TO_SAVED,     _("Revert to Saved"))
        STOCKITEM(wxSTOCK_SAVE,                _("&Save"))
        STOCKITEM(wxSTOCK_SAVE_AS,             _("Save &As..."))
        STOCKITEM(wxSTOCK_STOP,                _("&Stop"))
        STOCKITEM(wxSTOCK_UNDELETE,            _("Undelete"))
        STOCKITEM(wxSTOCK_UNDERLINE,           _("&Underline"))
        STOCKITEM(wxSTOCK_UNDO,                _("&Undo"))
        STOCKITEM(wxSTOCK_UNINDENT,            _("&Unindent"))
        STOCKITEM(wxSTOCK_YES,                 _("&Yes"))
        STOCKITEM(wxSTOCK_ZOOM_100,            _("&Actual Size"))
        STOCKITEM(wxSTOCK_ZOOM_FIT,            _("Zoom to &Fit"))
        STOCKITEM(wxSTOCK_ZOOM_IN,             _("Zoom &In"))
        STOCKITEM(wxSTOCK_ZOOM_OUT,            _("Zoom &Out"))

        case wxSTOCK_NONE:
        default:
            wxFAIL_MSG( _T("invalid stock item ID") );
            break;
    };

    #undef STOCKITEM
    
    return wxEmptyString;
}


#ifdef __WXGTK20__

#include <gtk/gtk.h>

const char *wxStockItemToGTK(wxStockItemID item)
{
    #define STOCKITEM(stockid)    \
        case wx##stockid:         \
            return GTK_##stockid; \
            break;

    switch (item)
    {
        STOCKITEM(STOCK_ADD)
        STOCKITEM(STOCK_APPLY)
        STOCKITEM(STOCK_BOLD)
        STOCKITEM(STOCK_CANCEL)
        STOCKITEM(STOCK_CLEAR)
        STOCKITEM(STOCK_CLOSE)
        STOCKITEM(STOCK_COPY)
        STOCKITEM(STOCK_CUT)
        STOCKITEM(STOCK_DELETE)
        STOCKITEM(STOCK_FIND)
        STOCKITEM(STOCK_FIND_AND_REPLACE)
        STOCKITEM(STOCK_GO_BACK)
        STOCKITEM(STOCK_GO_DOWN)
        STOCKITEM(STOCK_GO_FORWARD)
        STOCKITEM(STOCK_GO_UP)
        STOCKITEM(STOCK_HELP)
        STOCKITEM(STOCK_HOME)
        STOCKITEM(STOCK_INDENT)
        STOCKITEM(STOCK_INDEX)
        STOCKITEM(STOCK_ITALIC)
        STOCKITEM(STOCK_JUSTIFY_CENTER)
        STOCKITEM(STOCK_JUSTIFY_FILL)
        STOCKITEM(STOCK_JUSTIFY_LEFT)
        STOCKITEM(STOCK_JUSTIFY_RIGHT)
        STOCKITEM(STOCK_NEW)
        STOCKITEM(STOCK_NO)
        STOCKITEM(STOCK_OK)
        STOCKITEM(STOCK_OPEN)
        STOCKITEM(STOCK_PASTE)
        STOCKITEM(STOCK_PREFERENCES)
        STOCKITEM(STOCK_PRINT)
        STOCKITEM(STOCK_PRINT_PREVIEW)
        STOCKITEM(STOCK_PROPERTIES)
        STOCKITEM(STOCK_QUIT)
        STOCKITEM(STOCK_REDO)
        STOCKITEM(STOCK_REFRESH)
        STOCKITEM(STOCK_REMOVE)
        STOCKITEM(STOCK_REVERT_TO_SAVED)
        STOCKITEM(STOCK_SAVE)
        STOCKITEM(STOCK_SAVE_AS)
        STOCKITEM(STOCK_STOP)
        STOCKITEM(STOCK_UNDELETE)
        STOCKITEM(STOCK_UNDERLINE)
        STOCKITEM(STOCK_UNDO)
        STOCKITEM(STOCK_UNINDENT)
        STOCKITEM(STOCK_YES)
        STOCKITEM(STOCK_ZOOM_100)
        STOCKITEM(STOCK_ZOOM_FIT)
        STOCKITEM(STOCK_ZOOM_IN)
        STOCKITEM(STOCK_ZOOM_OUT)

        case wxSTOCK_NONE:
        default:
            wxFAIL_MSG( _T("invalid stock item ID") );
            break;
    };

    #undef STOCKITEM
    
    return NULL;
}

#endif // __WXGTK20__
