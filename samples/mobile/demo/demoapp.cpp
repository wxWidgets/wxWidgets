/////////////////////////////////////////////////////////////////////////////
// Name:        demoapp.cpp
// Purpose:     
// Author:      Julian Smart
// Modified by: 
// Created:     12/05/2009 09:16:41
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

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

#include "demoapp.h"
#include "demoframe.h"

////@begin XPM images
////@end XPM images


/*
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( MobileDemoApp )
////@end implement app


/*
 * MobileDemoApp type definition
 */

IMPLEMENT_CLASS( MobileDemoApp, wxMoApp )


/*
 * MobileDemoApp event table definition
 */

BEGIN_EVENT_TABLE( MobileDemoApp, wxMoApp )

////@begin MobileDemoApp event table entries
////@end MobileDemoApp event table entries

END_EVENT_TABLE()


/*
 * Constructor for MobileDemoApp
 */

MobileDemoApp::MobileDemoApp()
{
    Init();
}


/*
 * Member initialisation
 */

void MobileDemoApp::Init()
{
////@begin MobileDemoApp member initialisation
////@end MobileDemoApp member initialisation
}

/*
 * Initialisation for MobileDemoApp
 */

bool MobileDemoApp::OnInit()
{
    wxMoApp::OnInit();

    // PNG handler added in OnInit already
#if wxUSE_XPM
    wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBJPEG
    wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
    wxImage::AddHandler(new wxGIFHandler);
#endif
    DemoFrame* mainWindow = new DemoFrame( NULL );
    mainWindow->Show(true);

    return true;
}


/*
 * Cleanup for MobileDemoApp
 */

int MobileDemoApp::OnExit()
{    
    return wxMoApp::OnExit();
}

