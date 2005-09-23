/////////////////////////////////////////////////////////////////////////////
// Name:        test.h
// Purpose:
// Author:      Jorgen Bodde
// Modified by:
// Created:     27/06/2004
// RCS-ID:      $Id$
// Copyright:   (c) Jorgen Bodde
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _TEST_H_
#define _TEST_H_

/*!
 * Includes
 */

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10001
#define SYMBOL_TESTTEST_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_TESTTEST_TITLE _("Test Me")
#define SYMBOL_TESTTEST_IDNAME ID_DIALOG
#define SYMBOL_TESTTEST_SIZE wxSize(400, 200)
#define SYMBOL_TESTTEST_POSITION wxDefaultPosition
#define ID_PANEL7 10015
#define ID_PANEL6 10014
#define ID_PANEL3 10008
#define ID_PANEL 10002
#define ID_CHOICE 10003
#define ID_RADIOBUTTON 10004
#define ID_RADIOBUTTON1 10005
#define ID_PANEL2 10007
#define ID_PANEL4 10009
#define ID_PANEL5 10010
#define ID_CHOICE1 10011
#define ID_RADIOBUTTON2 10012
#define ID_RADIOBUTTON3 10013
#define ID_PANEL1 10006
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
 * TestTest class declaration
 */

class TestTest: public wxDialog
{
    DECLARE_CLASS( TestTest )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    TestTest( );
    TestTest( wxWindow* parent, wxWindowID id = SYMBOL_TESTTEST_IDNAME, const wxString& caption = SYMBOL_TESTTEST_TITLE, const wxPoint& pos = SYMBOL_TESTTEST_POSITION, const wxSize& size = SYMBOL_TESTTEST_SIZE, long style = SYMBOL_TESTTEST_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_TESTTEST_IDNAME, const wxString& caption = SYMBOL_TESTTEST_TITLE, const wxPoint& pos = SYMBOL_TESTTEST_POSITION, const wxSize& size = SYMBOL_TESTTEST_SIZE, long style = SYMBOL_TESTTEST_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin TestTest event handler declarations

////@end TestTest event handler declarations

////@begin TestTest member function declarations


    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end TestTest member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin TestTest member variables
    wxBoxSizer* blaat;
////@end TestTest member variables
};

#endif
    // _TEST_H_
