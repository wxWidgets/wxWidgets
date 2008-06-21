/////////////////////////////////////////////////////////////////////////////
// Name:        wxwallctrlprojectapp.h
// Purpose:     A test application for wxWallCtrl
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     02/06/2008 00:11:34
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _WXWALLCTRLPROJECTAPP_H_
#define _WXWALLCTRLPROJECTAPP_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
#include "wxwallctrltest.h"
#include "wx/Wallctrl/WallCtrl.h"
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
////@end control identifiers

/*!
 * WxWallCtrlProjectApp class declaration
 */

class WxWallCtrlProjectApp: public wxApp
{    
    DECLARE_CLASS( WxWallCtrlProjectApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    WxWallCtrlProjectApp();

    void Init();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin WxWallCtrlProjectApp event handler declarations

////@end WxWallCtrlProjectApp event handler declarations

////@begin WxWallCtrlProjectApp member function declarations

////@end WxWallCtrlProjectApp member function declarations

////@begin WxWallCtrlProjectApp member variables
////@end WxWallCtrlProjectApp member variables
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(WxWallCtrlProjectApp)
////@end declare app

#endif
    // _WXWALLCTRLPROJECTAPP_H_
