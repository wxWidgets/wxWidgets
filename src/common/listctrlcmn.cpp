////////////////////////////////////////////////////////////////////////////////
// Name:        src/common/listctrlcmn.cpp
// Purpose:     Common defines for wxListCtrl and wxListCtrl-based classes.
// Author:      Kevin Ollivier
// Created:     09/15/06
// RCS-ID:      $Id$
// Copyright:   (c) Kevin Ollivier
// Licence:     wxWindows licence
////////////////////////////////////////////////////////////////////////////////

// =============================================================================
// declarations
// =============================================================================

// -----------------------------------------------------------------------------
// headers
// -----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_LISTCTRL

#include "wx/listctrl.h"

const wxChar wxListCtrlNameStr[] = wxT("listCtrl");

// ListCtrl events
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_BEGIN_DRAG)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_BEGIN_RDRAG)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_END_LABEL_EDIT)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_DELETE_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_SELECTED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_DESELECTED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_KEY_DOWN)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_INSERT_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_RIGHT_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_BEGIN_DRAG)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_DRAGGING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_END_DRAG)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_ACTIVATED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_FOCUSED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_CACHE_HINT)

// -----------------------------------------------------------------------------
// XTI
// -----------------------------------------------------------------------------

wxDEFINE_FLAGS( wxListCtrlStyle )
wxBEGIN_FLAGS( wxListCtrlStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

    wxFLAGS_MEMBER(wxLC_LIST)
    wxFLAGS_MEMBER(wxLC_REPORT)
    wxFLAGS_MEMBER(wxLC_ICON)
    wxFLAGS_MEMBER(wxLC_SMALL_ICON)
    wxFLAGS_MEMBER(wxLC_ALIGN_TOP)
    wxFLAGS_MEMBER(wxLC_ALIGN_LEFT)
    wxFLAGS_MEMBER(wxLC_AUTOARRANGE)
    wxFLAGS_MEMBER(wxLC_USER_TEXT)
    wxFLAGS_MEMBER(wxLC_EDIT_LABELS)
    wxFLAGS_MEMBER(wxLC_NO_HEADER)
    wxFLAGS_MEMBER(wxLC_SINGLE_SEL)
    wxFLAGS_MEMBER(wxLC_SORT_ASCENDING)
    wxFLAGS_MEMBER(wxLC_SORT_DESCENDING)
    wxFLAGS_MEMBER(wxLC_VIRTUAL)
wxEND_FLAGS( wxListCtrlStyle )

wxIMPLEMENT_DYNAMIC_CLASS_XTI(wxListCtrl, wxControl, "wx/listctrl.h")

wxBEGIN_PROPERTIES_TABLE(wxListCtrl)
    wxEVENT_PROPERTY( TextUpdated, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEvent )

    wxPROPERTY_FLAGS( WindowStyle, wxListCtrlStyle, long, SetWindowStyleFlag, \
                      GetWindowStyleFlag, EMPTY_MACROVALUE, 0 /*flags*/, \
                      wxT("Helpstring"), wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxListCtrl)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_5( wxListCtrl, wxWindow*, Parent, wxWindowID, Id, \
                 wxPoint, Position, wxSize, Size, long, WindowStyle )

/*
 TODO : Expose more information of a list's layout etc. via appropriate objects 
        (à la NotebookPageInfo)
*/

#endif // wxUSE_LISTCTRL
