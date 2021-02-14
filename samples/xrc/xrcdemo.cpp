//-----------------------------------------------------------------------------
// Name:        xrcdemo.cpp
// Purpose:     XML resources sample: Main application file
// Author:      Robert O'Connor (rob@medicalmnemonics.com), Vaclav Slavik
// Copyright:   (c) Robert O'Connor and Vaclav Slavik
// Licence:     wxWindows licence
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Standard wxWidgets headers
//-----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// For all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//-----------------------------------------------------------------------------
// Header of this .cpp file
//-----------------------------------------------------------------------------

#include "xrcdemo.h"

//-----------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then app one
//-----------------------------------------------------------------------------

#include "wx/image.h"               // wxImage

#include "wx/xrc/xmlres.h"          // XRC XML resources

#if wxUSE_RIBBON
    #include "wx/xrc/xh_ribbon.h"
#endif // wxUSE_RIBBON

#if wxUSE_AUI
    #include "wx/xrc/xh_aui.h"
    #include "wx/xrc/xh_auitoolb.h"
#endif // wxUSE_AUI

#include "wx/cshelp.h"              // wxSimpleHelpProvider for helptext

#include "myframe.h"

//-----------------------------------------------------------------------------
// wxWidgets macro: Declare the application.
//-----------------------------------------------------------------------------

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. the_app and
// not wxApp).
wxIMPLEMENT_APP(MyApp);

//-----------------------------------------------------------------------------
// Public methods
//-----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // If there is any of a certain format of image in the xrcs, then first
    // load a handler for that image type. This example uses XPMs & a gif, but
    // if you want PNGs, then add a PNG handler, etc. See wxImage::AddHandler()
    // documentation for the types of image handlers available.
#if wxUSE_XPM
    wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_GIF
    wxImage::AddHandler(new wxGIFHandler);
#endif

    // Initialize all the XRC handlers. Always required (unless you feel like
    // going through and initializing a handler of each control type you will
    // be using (ie initialize the spinctrl handler, initialize the textctrl
    // handler). However, if you are only using a few control types, it will
    // save some space to only initialize the ones you will be using. See
    // wxXRC docs for details.
    wxXmlResource::Get()->InitAllHandlers();

    // Allow using environment variables in the file paths in the resources,
    // while keeping the default wxXRC_USE_LOCALE flag.
    wxXmlResource::Get()->SetFlags(wxXRC_USE_LOCALE | wxXRC_USE_ENVVARS);

#if wxUSE_RIBBON
    wxXmlResource::Get()->AddHandler(new wxRibbonXmlHandler);
#endif

#if wxUSE_AUI
    wxXmlResource::Get()->AddHandler(new wxAuiXmlHandler);
    wxXmlResource::Get()->AddHandler(new wxAuiToolBarXmlHandler);
#endif

    // Load all of the XRC files that will be used. You can put everything
    // into one giant XRC file if you wanted, but then they become more
    // diffcult to manage, and harder to reuse in later projects.
    if ( !wxXmlResource::Get()->LoadAllFiles("rc") )
        return false;

#if wxUSE_HELP
    // Use the simple help provider to show the context-sensitive help
    wxHelpProvider::Set( new wxSimpleHelpProvider );
#endif // wxUSE_HELP

    // Make an instance of your derived frame. Passing NULL (the default value
    // of MyFrame's constructor is NULL) as the frame doesn't have a parent
    // since it is the main application window.
    MyFrame *frame = new MyFrame();

    // Show the frame as it's created initially hidden.
    frame->Show(true);

    // Return true to tell program to continue (false would terminate).
    return true;
}

