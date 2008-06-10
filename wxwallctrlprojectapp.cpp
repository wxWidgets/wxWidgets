/////////////////////////////////////////////////////////////////////////////
// Name:        wxwallctrlprojectapp.cpp
// Purpose:     
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     02/06/2008 00:11:34
// RCS-ID:      
// Copyright:   Copyright 2008 by Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include <wx/apptrait.h>
#if wxUSE_STACKWALKER && defined( __WXDEBUG__ ) // silly workaround for the link error with debug configuration:
// \src\common\appbase.cpp
wxString wxAppTraitsBase::GetAssertStackTrace()
{
	return wxT("");
}
#endif


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "wxwallctrlprojectapp.h"

////@begin XPM images
////@end XPM images


/*!
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( WxWallCtrlProjectApp )
////@end implement app


/*!
 * WxWallCtrlProjectApp type definition
 */

IMPLEMENT_CLASS( WxWallCtrlProjectApp, wxApp )


/*!
 * WxWallCtrlProjectApp event table definition
 */

BEGIN_EVENT_TABLE( WxWallCtrlProjectApp, wxApp )

////@begin WxWallCtrlProjectApp event table entries
////@end WxWallCtrlProjectApp event table entries

END_EVENT_TABLE()


/*!
 * Constructor for WxWallCtrlProjectApp
 */

WxWallCtrlProjectApp::WxWallCtrlProjectApp()
{
    Init();
}


/*!
 * Member initialisation
 */

void WxWallCtrlProjectApp::Init()
{
////@begin WxWallCtrlProjectApp member initialisation
////@end WxWallCtrlProjectApp member initialisation
}

/*!
 * Initialisation for WxWallCtrlProjectApp
 */

bool WxWallCtrlProjectApp::OnInit()
{    
////@begin WxWallCtrlProjectApp initialisation
	// Remove the comment markers above and below this block
	// to make permanent changes to the code.

#if wxUSE_XPM
	wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
	wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
	wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
	wxImage::AddHandler(new wxGIFHandler);
#endif
	wxWallCtrlTest* mainWindow = new wxWallCtrlTest(NULL);
	/* int returnValue = */ mainWindow->ShowModal();

	mainWindow->Destroy();
	// A modal dialog application should return false to terminate the app.
	return false;
////@end WxWallCtrlProjectApp initialisation

    return true;
}


/*!
 * Cleanup for WxWallCtrlProjectApp
 */

int WxWallCtrlProjectApp::OnExit()
{    
////@begin WxWallCtrlProjectApp cleanup
	return wxApp::OnExit();
////@end WxWallCtrlProjectApp cleanup
}

