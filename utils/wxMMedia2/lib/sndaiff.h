// --------------------------------------------------------------------------
// Name: sndaiff.h
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDAIFF_H
#define _WX_SNDAIFF_H

#ifdef __GNUG__
#pragma interface "sndaiff.h"
#endif

#include <wx/stream.h>
#include "sndbase.h"
#include "sndcodec.h"
#include "sndfile.h"

//
// AIFF codec
//

class wxSoundAiff: public wxSoundFileStream {
 public:
  wxSoundAiff(wxInputStream& stream, wxSoundStream& io_sound);
  wxSoundAiff(wxOutputStream& stream, wxSoundStream& io_sound);
  ~wxSoundAiff();

 protected:
  bool PrepareToPlay(); 
  bool PrepareToRecord(unsigned long time);
  bool FinishRecording();

  wxUint32 GetData(void *buffer, wxUint32 len);
  wxUint32 PutData(const void *buffer, wxUint32 len);
};

#endif
