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
#include "../lib/sndoss.h"
#include "../lib/sndwav.h"
#include "../lib/sndaiff.h"

class MyApp: public wxApp {
 bool OnInit() {
   wxSoundStreamOSS *oss_dev = new wxSoundStreamOSS();
   wxFileInputStream *f_input = new wxFileInputStream(argv[1]);
   wxSoundFileStream *wav_file = new wxSoundAiff(*f_input, *oss_dev);
   wxFrame *frame = new wxFrame(NULL, -1, "My Frame");
   wxSoundFormatPcm pcm;

   if (oss_dev->GetError() != wxSOUND_NOERR) {
     wxPrintf("No device\n");
     return FALSE;
   }

   wav_file->Play();
   frame->Show(TRUE);
   return TRUE;
 }
};

IMPLEMENT_APP(MyApp)
