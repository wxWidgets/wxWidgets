/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:     wxApp inclusion
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APP_H_BASE_
#define _WX_APP_H_BASE_

#ifdef __WXMSW__
class WXDLLEXPORT wxApp;
typedef wxApp* (*wxAppInitializerFunction) (void);
#endif

#include "wx/object.h"

#ifndef __WXMSW__
typedef wxObject* (*wxAppInitializerFunction) (void);  // returning wxApp* won't work with gcc
#endif

#if defined(__WXMSW__)
#include "wx/msw/app.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/app.h"
#elif defined(__WXQT__)
#include "wx/qt/app.h"
#elif defined(__WXGTK__)
#include "wx/gtk/app.h"
#elif defined(__WXMAC__)
#include "wx/mac/app.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/app.h"
#endif

// Having a global instance of this class allows
// wxApp to be aware of the app creator function.
// wxApp can then call this function to create a new
// app object. Convoluted, but necessary.

class WXDLLEXPORT wxAppInitializer
{
public:
	wxAppInitializer(wxAppInitializerFunction fn)
	{
		wxApp::SetInitializerFunction(fn);
	}
};

// Here's a macro you can use if your compiler
// really, really wants main() to be in your main program
// (e.g. hello.cpp).
// Now IMPLEMENT_APP should add this code if required.

#if defined(__AIX__) || defined(__HPUX__)
#define IMPLEMENT_WXWIN_MAIN \
extern int wxEntry( int argc, char *argv[] ); \
int main(int argc, char *argv[]) { return wxEntry(argc, argv); }
#else
#define IMPLEMENT_WXWIN_MAIN
#endif

#define IMPLEMENT_APP(appname)                          \
        wxApp *wxCreateApp(void) { return new appname; }      \
		wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp); \
        appname& wxGetApp(void) { return *(appname *)wxTheApp; } \
        IMPLEMENT_WXWIN_MAIN

#define DECLARE_APP(appname) \
	extern appname& wxGetApp(void) ;


#endif
    // _WX_APP_H_BASE_
