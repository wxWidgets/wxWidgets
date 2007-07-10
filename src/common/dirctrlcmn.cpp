/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dirctrlcmn.cpp
// Purpose:     wxGenericDirCtrl common code
// Author:      Harm van der Heijden, Robert Roebling, Julian Smart
// Modified by:
// Created:     12/12/98
// RCS-ID:      $Id: dirctrlg.cpp 45395 2007-04-11 00:23:19Z VZ $
// Copyright:   (c) Harm van der Heijden, Robert Roebling and Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DIRDLG || wxUSE_FILEDLG

#include "wx/generic/dirctrlg.h"

//-----------------------------------------------------------------------------
// XTI
//-----------------------------------------------------------------------------

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxGenericDirCtrlStyle )

wxBEGIN_FLAGS( wxGenericDirCtrlStyle )
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

    wxFLAGS_MEMBER(wxDIRCTRL_DIR_ONLY)
    wxFLAGS_MEMBER(wxDIRCTRL_3D_INTERNAL)
    wxFLAGS_MEMBER(wxDIRCTRL_SELECT_FIRST)
    wxFLAGS_MEMBER(wxDIRCTRL_SHOW_FILTERS)

wxEND_FLAGS( wxGenericDirCtrlStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxGenericDirCtrl, wxControl,"wx/dirctrl.h")

wxBEGIN_PROPERTIES_TABLE(wxGenericDirCtrl)
    wxHIDE_PROPERTY( Children )

    wxPROPERTY( DefaultPath, wxString, SetDefaultPath, GetDefaultPath, \
                EMPTY_MACROVALUE, 0 /*flags*/, wxT("Helpstring"), wxT("group"))
    wxPROPERTY( Filter, wxString, SetFilter, GetFilter, EMPTY_MACROVALUE, \
                0 /*flags*/, wxT("Helpstring"), wxT("group") )
    wxPROPERTY( DefaultFilter, int, SetFilterIndex, GetFilterIndex, \
                EMPTY_MACROVALUE, 0 /*flags*/, wxT("Helpstring"), wxT("group") )

    wxPROPERTY_FLAGS( WindowStyle, wxGenericDirCtrlStyle, long, SetWindowStyleFlag, \
                      GetWindowStyleFlag, EMPTY_MACROVALUE, 0, wxT("Helpstring"), \
                      wxT("group") )
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxGenericDirCtrl)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_8( wxGenericDirCtrl, wxWindow*, Parent, wxWindowID, Id, \
                 wxString, DefaultPath, wxPoint, Position, wxSize, Size, \
                 long, WindowStyle, wxString, Filter, int, DefaultFilter )
#else
IMPLEMENT_DYNAMIC_CLASS(wxGenericDirCtrl, wxControl)
#endif

#endif // wxUSE_DIRDLG || wxUSE_FILEDLG
