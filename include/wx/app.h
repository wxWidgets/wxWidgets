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

#ifndef __APPH_BASE__
#define __APPH_BASE__

#ifndef __GTK__
class WXDLLEXPORT wxApp;
typedef wxApp* (*wxAppInitializerFunction) (void);
#endif

#include "wx/object.h"

#ifdef __GTK__
typedef wxObject* (*wxAppInitializerFunction) (void);  // returning wxApp* won't work with gcc
#endif

#if defined(__WINDOWS__)
#include "wx/msw/app.h"
#elif defined(__MOTIF__)
#include "wx/xt/app.h"
#elif defined(__GTK__)
#include "wx/gtk/app.h"
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

#if defined(AIX) || defined(AIX4) /* || defined(____HPUX__) */
#define IMPLEMENT_WXWIN_MAIN int main(int argc, char *argv[]) { return wxEntry(argc, argv); }
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
    // __APPH_BASE__
