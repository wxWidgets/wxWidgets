// --------------------------------------------------------------------------
// Name: sndwav.h
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDWAV_H
#define _WX_SNDWAV_H

#ifdef __GNUG__
#pragma interface "sndwav.h"
#endif

#include <wx/stream.h>
#include "sndbase.h"
#include "sndcodec.h"
#include "sndfile.h"

//
// WAVE codec
//

class wxSoundWave: public wxSoundFileStream {
 public:
  wxSoundWave(wxInputStream& stream, wxSoundStream& io_sound);
  wxSoundWave(wxOutputStream& stream, wxSoundStream& io_sound);
  ~wxSoundWave();

  bool CanRead();

 protected:
  bool PrepareToPlay(); 
  bool PrepareToRecord(unsigned long time);
  bool FinishRecording();

  size_t GetData(void *buffer, size_t len);
  size_t PutData(const void *buffer, size_t len);
};

#endif
