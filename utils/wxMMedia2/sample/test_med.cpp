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
#include "../lib/sndulaw.h"

wxSoundStreamOSS *oss_dev;
wxInputStream *f_input;
wxSoundStreamUlaw *ulaw_codec;

class MySoundStream: public wxSoundStream {
 public:
  wxSoundStream& Read(void *buffer, size_t len) { return *this; }
  wxSoundStream& Write(const void *buffer, size_t len) { return *this; }

  bool StartProduction(int evt) { return FALSE; }
  bool StopProduction() { return FALSE; }

  void SetDuplexMode(bool on) {}

 void OnSoundEvent(int evt) {
   char buffer[2048];

   f_input->Read(buffer, sizeof(buffer));
   ulaw_codec->Write(buffer, sizeof(buffer));
 }
};

class MyApp: public wxApp {
 bool OnInit() {
   wxSoundFormatUlaw ulaw;
   MySoundStream strm;

   oss_dev = new wxSoundStreamOSS();
   f_input = new wxFileInputStream(argv[1]);

   if (oss_dev->GetError() != wxSOUND_NOERR) {
     wxPrintf("No device\n");
     return FALSE;
   }

   ulaw.SetSampleRate(8000);
   ulaw_codec = new wxSoundStreamUlaw(*oss_dev);
   ulaw_codec->SetSoundFormat(ulaw);

   oss_dev->SetEventHandler(&strm);
   oss_dev->StartProduction(wxSOUND_OUTPUT);
   
   while (1) {
//     wxYield();
     strm.OnSoundEvent(0);
   }
   return TRUE;
 }
};

IMPLEMENT_APP(MyApp)
