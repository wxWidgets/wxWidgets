/////////////////////////////////////////////////////////////////////////////
// Name:        layouttest.h
// Purpose:
// Author:      Jorgen Bodde
// Modified by:
// Created:     25/06/2004
// RCS-ID:      $Id$
// Copyright:   (c) Jorgen Bodde
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _LAYOUTTEST_H_
#define _LAYOUTTEST_H_

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
#define ID_DIALOG 10001
#define SYMBOL_LAYOUTTEST_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_LAYOUTTEST_TITLE _("Layout Test")
#define SYMBOL_LAYOUTTEST_IDNAME ID_DIALOG
#define SYMBOL_LAYOUTTEST_SIZE wxSize(400, 300)
#define SYMBOL_LAYOUTTEST_POSITION wxDefaultPosition
#define ID_PANEL1 10006
#define ID_BUTTON 10007
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
 * LayoutTest class declaration
 */

class LayoutTest: public wxPanel
{
    DECLARE_CLASS( LayoutTest )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    LayoutTest( );
    LayoutTest( wxWindow* parent, wxWindowID id = SYMBOL_LAYOUTTEST_IDNAME, const wxString& caption = SYMBOL_LAYOUTTEST_TITLE, const wxPoint& pos = SYMBOL_LAYOUTTEST_POSITION, const wxSize& size = SYMBOL_LAYOUTTEST_SIZE, long style = SYMBOL_LAYOUTTEST_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_LAYOUTTEST_IDNAME, const wxString& caption = SYMBOL_LAYOUTTEST_TITLE, const wxPoint& pos = SYMBOL_LAYOUTTEST_POSITION, const wxSize& size = SYMBOL_LAYOUTTEST_SIZE, long style = SYMBOL_LAYOUTTEST_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin LayoutTest event handler declarations

////@end LayoutTest event handler declarations

////@begin LayoutTest member function declarations


    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end LayoutTest member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin LayoutTest member variables
////@end LayoutTest member variables
};

#endif
    // _LAYOUTTEST_H_
