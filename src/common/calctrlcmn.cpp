///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/calctrlcmn.cpp
// Purpose:     wxCalendarCtrl common code
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29.12.99
// RCS-ID:      $Id: calctrl.cpp 43778 2006-12-03 21:30:23Z VZ $
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
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

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/settings.h"
    #include "wx/brush.h"
    #include "wx/combobox.h"
    #include "wx/listbox.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
#endif //WX_PRECOMP

#if wxUSE_CALENDARCTRL

#include "wx/calctrl.h"

// ----------------------------------------------------------------------------
// XTI
// ----------------------------------------------------------------------------

wxDEFINE_FLAGS( wxCalendarCtrlStyle )
wxBEGIN_FLAGS( wxCalendarCtrlStyle )
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

    wxFLAGS_MEMBER(wxCAL_SUNDAY_FIRST)
    wxFLAGS_MEMBER(wxCAL_MONDAY_FIRST)
    wxFLAGS_MEMBER(wxCAL_SHOW_HOLIDAYS)
    wxFLAGS_MEMBER(wxCAL_NO_YEAR_CHANGE)
    wxFLAGS_MEMBER(wxCAL_NO_MONTH_CHANGE)
    wxFLAGS_MEMBER(wxCAL_SEQUENTIAL_MONTH_SELECTION)
    wxFLAGS_MEMBER(wxCAL_SHOW_SURROUNDING_WEEKS)

wxEND_FLAGS( wxCalendarCtrlStyle )

wxIMPLEMENT_DYNAMIC_CLASS_XTI(wxCalendarCtrl, wxControl, "wx/calctrl.h")

wxBEGIN_PROPERTIES_TABLE(wxCalendarCtrl)
    wxEVENT_RANGE_PROPERTY( Updated, wxEVT_CALENDAR_SEL_CHANGED, \
                            wxEVT_CALENDAR_WEEKDAY_CLICKED, wxCalendarEvent )
    
    wxHIDE_PROPERTY( Children )

    wxPROPERTY( Date,wxDateTime, SetDate, GetDate, EMPTY_MACROVALUE, \
                0 /*flags*/, wxT("Helpstring"), wxT("group"))
    wxPROPERTY_FLAGS( WindowStyle, wxCalendarCtrlStyle, long, \
                      SetWindowStyleFlag, GetWindowStyleFlag, \
                      EMPTY_MACROVALUE, 0 /*flags*/, wxT("Helpstring"), \
                      wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxCalendarCtrl)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_6( wxCalendarCtrl, wxWindow*, Parent, wxWindowID, Id, \
                 wxDateTime, Date, wxPoint, Position, wxSize, Size, long, WindowStyle )


#endif // wxUSE_CALENDARCTRL
