// --------------------------------------------------------------------------
// Name: sndcodec.h
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDCODEC_H
#define _WX_SNDCODEC_H

#ifdef __GNUG__
#pragma interface "sndcodec.h"
#endif

#include "sndbase.h"

class wxSoundStreamCodec: public wxSoundStream {
 public:
  wxSoundStreamCodec(wxSoundStream& snd_io);
  ~wxSoundStreamCodec();

  bool StartProduction(int evt);
  bool StopProduction();

  void SetDuplexMode(bool duplex);
  wxUint32 GetBestSize() const;

 protected:
  wxSoundStream *m_sndio;
};

#endif
