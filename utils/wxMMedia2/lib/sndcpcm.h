// --------------------------------------------------------------------------
// Name: sndcpcm.h
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDCPCM_H
#define _WX_SNDCPCM_H

#ifdef __GNUG__
#pragma interface "sndcpcm.h"
#endif

#include <stddef.h>
#include "sndcodec.h"

//
// PCM converter class
//

class wxSoundStreamPcm: public wxSoundStreamCodec {
 public:
  typedef void (*ConverterType)(const char *buf_in, char *buf_out, wxUint32 len);

  wxSoundStreamPcm(wxSoundStream& sndio);
  ~wxSoundStreamPcm();

  wxSoundStream& Read(void *buffer, wxUint32 len);
  wxSoundStream& Write(const void *buffer, wxUint32 len);

  bool SetSoundFormat(const wxSoundFormatBase& format);

 protected:
  ConverterType m_function_out, m_function_in;

  bool m_16_to_8;
};

#endif
