//-----------------------------------------------------------------------------
// Name:        xrcdemo.cpp
// Purpose:     XML resources sample: Main application file
// Author:      Robert O'Connor (rob@medicalmnemonics.com), Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Robert O'Connor and Vaclav Slavik
// Licence:     wxWindows licence
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// GCC implementation
//-----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "xrcdemo.h"
#endif

//-----------------------------------------------------------------------------
// Standard wxWindows headers
//-----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// For all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//-----------------------------------------------------------------------------
// Header of this .cpp file
//-----------------------------------------------------------------------------

#include "xrcdemo.h"

//-----------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//-----------------------------------------------------------------------------

#include "wx/image.h"               // wxImage

//-----------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"          // XRC XML resouces

//-----------------------------------------------------------------------------

#include "myframe.h"

//-----------------------------------------------------------------------------
// wxWindows macro: Declare the application.
//-----------------------------------------------------------------------------

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. the_app and
// not wxApp).
IMPLEMENT_APP(MyApp)

//-----------------------------------------------------------------------------
// Public methods
//-----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // If there is any of a certain format of image in the xrcs, then first
    // load a handler for that image type. This example uses XPMs, but if
    // you want PNGs, then add a PNG handler, etc. See wxImage::AddHandler()
    // documentation for the types of image handlers available.
    wxImage::AddHandler(new wxXPMHandler);
    
    // Initialize all the XRC handlers. Always required (unless you feel like
    // going through and initializing a handler of each control type you will
    // be using (ie initialize the spinctrl handler, initialize the textctrl
    // handler). However, if you are only using a few control types, it will
    // save some space to only initialize the ones you will be using. See
    // wxXRC docs for details.
    wxXmlResource::Get()->InitAllHandlers();    
      
    // Load all of the XRC files that will be used. You can put everything
    // into one giant XRC file if you wanted, but then they become more 
    // diffcult to manage, and harder to reuse in later projects.   
    // The menubar
    wxXmlResource::Get()->Load("rc/menu.xrc");
    // The toolbar
    wxXmlResource::Get()->Load("rc/toolbar.xrc");
    // Non-derived dialog example
    wxXmlResource::Get()->Load("rc/basicdlg.xrc");
    // Derived dialog example
    wxXmlResource::Get()->Load("rc/derivdlg.xrc");
    // Controls property example
    wxXmlResource::Get()->Load("rc/controls.xrc");
    // Frame example
    wxXmlResource::Get()->Load("rc/frame.xrc");
    // Uncentered example
    wxXmlResource::Get()->Load("rc/uncenter.xrc");    
    // Custom class example
    wxXmlResource::Get()->Load("rc/custclas.xrc");
    // wxArtProvider example
    wxXmlResource::Get()->Load("rc/artprov.xrc");
    // Platform property example
    wxXmlResource::Get()->Load("rc/platform.xrc");
    // Variable expansion example
    wxXmlResource::Get()->Load("rc/variable.xrc");

    // Make an instance of your derived frame. Passing NULL (the default value 
    // of MyFrame's constructor is NULL) as the frame doesn't have a frame
    // since it is the first window. 
    MyFrame *frame = new MyFrame();
    
    // Show the frame.
    frame->Show(TRUE);
    
    // Return TRUE to tell program to continue (FALSE would terminate).
    return TRUE;
}

