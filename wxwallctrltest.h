/////////////////////////////////////////////////////////////////////////////
// Name:        wxwallctrltest.h
// Purpose:     
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     02/06/2008 12:13:29  
// Copyright:   Copyright 2008 by Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _WXWALLCTRLTEST_H_
#define _WXWALLCTRLTEST_H_


/*!
 * Includes
 */

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_WXWALLCTRLTEST 10000
#define ID_BUTTON2 10002
#define ID_SCROLLEDWINDOW1 10005
#define ID_BUTTON3 10003
#define ID_BUTTON1 10001
#define ID_PANEL1 10004
#define SYMBOL_WXWALLCTRLTEST_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_WXWALLCTRLTEST_TITLE _("Test for wxWallCtrl")
#define SYMBOL_WXWALLCTRLTEST_IDNAME ID_WXWALLCTRLTEST
#define SYMBOL_WXWALLCTRLTEST_SIZE wxSize(400, 300)
#define SYMBOL_WXWALLCTRLTEST_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * wxWallCtrlTest class declaration
 */

class wxWallCtrlTest: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( wxWallCtrlTest )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    wxWallCtrlTest();
    wxWallCtrlTest( wxWindow* parent, wxWindowID id = SYMBOL_WXWALLCTRLTEST_IDNAME, const wxString& caption = SYMBOL_WXWALLCTRLTEST_TITLE, const wxPoint& pos = SYMBOL_WXWALLCTRLTEST_POSITION, const wxSize& size = SYMBOL_WXWALLCTRLTEST_SIZE, long style = SYMBOL_WXWALLCTRLTEST_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXWALLCTRLTEST_IDNAME, const wxString& caption = SYMBOL_WXWALLCTRLTEST_TITLE, const wxPoint& pos = SYMBOL_WXWALLCTRLTEST_POSITION, const wxSize& size = SYMBOL_WXWALLCTRLTEST_SIZE, long style = SYMBOL_WXWALLCTRLTEST_STYLE );

    /// Destructor
    ~wxWallCtrlTest();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxWallCtrlTest event handler declarations

////@end wxWallCtrlTest event handler declarations

////@begin wxWallCtrlTest member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxWallCtrlTest member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxWallCtrlTest member variables
////@end wxWallCtrlTest member variables
};

#endif
    // _WXWALLCTRLTEST_H_
