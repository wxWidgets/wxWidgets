/////////////////////////////////////////////////////////////////////////////
// Name:        demoapp.h
// Purpose:     
// Author:      Julian Smart
// Modified by: 
// Created:     12/05/2009 09:16:41
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _DEMOAPP_H_
#define _DEMOAPP_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
#include "demoframe.h"
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
 * MobileDemoApp class declaration
 */

class MobileDemoApp: public wxApp
{    
    DECLARE_CLASS( MobileDemoApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    MobileDemoApp();

    void Init();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin MobileDemoApp event handler declarations

////@end MobileDemoApp event handler declarations

////@begin MobileDemoApp member function declarations

////@end MobileDemoApp member function declarations

////@begin MobileDemoApp member variables
////@end MobileDemoApp member variables
};

#endif
    // _DEMOAPP_H_
