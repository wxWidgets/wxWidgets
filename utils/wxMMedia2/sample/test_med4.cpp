// --------------------------------------------------------------------------
// Name: test_med.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#include <wx/wxprec.h>
#include <wx/app.h>
#include <wx/wfstream.h>
#include <wx/frame.h>
#include "../lib/sndoss.h"
#include "../lib/sndwav.h"
#include "../lib/sndaiff.h"
#include "../lib/sndwin.h"

class MyApp: public wxApp {
  wxSoundStream *oss_dev;
  wxOutputStream *f_output;
  wxSoundFileStream *wav_file;

  bool OnInit() {
    wxFrame *frame = new wxFrame(NULL, -1, "My Frame");
    wxSoundFormatPcm pcm;

    oss_dev = new wxSoundStreamWin();
    f_output = new wxFileOutputStream(argv[1]);
    wav_file = new wxSoundWave(*f_output, *oss_dev);

    if (oss_dev->GetError() != wxSOUND_NOERR) {
      wxPrintf("No device\n");
      return FALSE;
    }

    pcm.SetSampleRate(22050);
    pcm.SetBPS(8);
    pcm.SetChannels(1);

    wav_file->SetSoundFormat(pcm);

    // Record 10 sec of sound
    wav_file->Record(10);
    frame->Show(TRUE);
    return TRUE;
  }
  int OnExit() {
    delete wav_file;
    delete f_output;
    delete oss_dev;
    return 0;
  }
};

IMPLEMENT_APP(MyApp)
