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
#include "../lib/cdunix.h"

class MyApp: public wxApp {
 bool OnInit() {
   wxCDAudioLinux m_cd;

   m_cd.Play(m_cd.GetToc().GetTrackTime(0),m_cd.GetToc().GetTrackTime(1));
   return TRUE;
 }
};

IMPLEMENT_APP(MyApp)
