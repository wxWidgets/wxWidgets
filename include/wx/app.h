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

/*
class WXDLLEXPORT wxApp;
typedef wxApp* (*wxAppInitializerFunction) (void);
*/

#include "wx/object.h"

typedef wxObject* (*wxAppInitializerFunction) (void);  // returning wxApp* won't work with gcc

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

#define IMPLEMENT_APP(appname)                          \
        wxApp *wxCreateApp(void) { return new appname; }      \
		wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp); \
        appname& wxGetApp(void) { return *(appname *)wxTheApp; } \
\
        extern int wxEntry( int argc, char *argv[] ); \
        int main(int argc, char *argv[]) { return wxEntry(argc, argv); }


#define DECLARE_APP(appname) \
	extern appname& wxGetApp(void) ;


#endif
    // __APPH_BASE__
