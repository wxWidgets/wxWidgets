/////////////////////////////////////////////////////////////////////////////
// Name:        wx/stockitem.h
// Purpose:     wxStockItemID enum
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004-08-15
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2004
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STOCKITEM_H_
#define _WX_STOCKITEM_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "stockitem.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"

// ----------------------------------------------------------------------------
// Stock item IDs for use with wxButton, wxMenu and wxToolBar:
// ----------------------------------------------------------------------------

enum wxStockItemID
{
    wxSTOCK_NONE = 0,
    wxSTOCK_ADD,
    wxSTOCK_APPLY,
    wxSTOCK_BOLD,
    wxSTOCK_CANCEL,
    wxSTOCK_CLEAR,
    wxSTOCK_CLOSE,
    wxSTOCK_COPY,
    wxSTOCK_CUT,
    wxSTOCK_DELETE,
    wxSTOCK_FIND,
    wxSTOCK_FIND_AND_REPLACE,
    wxSTOCK_GO_BACK,
    wxSTOCK_GO_DOWN,
    wxSTOCK_GO_FORWARD,
    wxSTOCK_GO_UP,
    wxSTOCK_HELP,
    wxSTOCK_HOME,
    wxSTOCK_INDENT,
    wxSTOCK_INDEX,
    wxSTOCK_ITALIC,
    wxSTOCK_JUSTIFY_CENTER,
    wxSTOCK_JUSTIFY_FILL,
    wxSTOCK_JUSTIFY_LEFT,
    wxSTOCK_JUSTIFY_RIGHT,
    wxSTOCK_NEW,
    wxSTOCK_NO,
    wxSTOCK_OK,
    wxSTOCK_OPEN,
    wxSTOCK_PASTE,
    wxSTOCK_PREFERENCES,
    wxSTOCK_PRINT,
    wxSTOCK_PRINT_PREVIEW,
    wxSTOCK_PROPERTIES,
    wxSTOCK_QUIT,
    wxSTOCK_REDO,
    wxSTOCK_REFRESH,
    wxSTOCK_REMOVE,
    wxSTOCK_REVERT_TO_SAVED,
    wxSTOCK_SAVE,
    wxSTOCK_SAVE_AS,
    wxSTOCK_STOP,
    wxSTOCK_UNDELETE,
    wxSTOCK_UNDERLINE,
    wxSTOCK_UNDO,
    wxSTOCK_UNINDENT,
    wxSTOCK_YES,
    wxSTOCK_ZOOM_100,
    wxSTOCK_ZOOM_FIT,
    wxSTOCK_ZOOM_IN,
    wxSTOCK_ZOOM_OUT
    // NB: when you add new item here, add it to stockitem.cpp and
    //     documentation as well
};

// ----------------------------------------------------------------------------
// Helper functions:
// ----------------------------------------------------------------------------

// Returns label that should be used for given stock UI element (e.g. "&OK"
// for wxSTOCK_OK):
wxString wxGetStockItemLabel(wxStockItemID item);

#ifdef __WXGTK20__
// Translates stock ID to GTK+'s stock item string indentifier:
const char *wxStockItemToGTK(wxStockItemID item);
#endif

#endif // _WX_STOCKITEM_H_
