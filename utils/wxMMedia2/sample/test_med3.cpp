// --------------------------------------------------------------------------
// Name: test_med.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#include <wx/app.h>
#include <wx/wfstream.h>
#include <wx/frame.h>
#include "../lib/vidxanm.h"

class MyApp: public wxApp {
 bool OnInit() {
   wxFileInputStream *file = new wxFileInputStream(argv[1]);
   wxVideoXANIM *vidxanm = new wxVideoXANIM(*file);

   vidxanm->Play();
   return TRUE;
 }
};

IMPLEMENT_APP(MyApp)
