// --------------------------------------------------------------------------
// Name: sndulaw.h
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDULAW_H
#define _WX_SNDULAW_H

#ifdef __GNUG__
#pragma interface "sndulaw.h"
#endif

#include <stddef.h>
#include "sndcodec.h"
#include "sndbase.h"

//
// ULAW format
//
class WXDLLEXPORT wxSoundFormatUlaw: public wxSoundFormatBase {
 public:
  wxSoundFormatUlaw();
  ~wxSoundFormatUlaw();

  void SetSampleRate(wxUint32 srate);
  wxUint32 GetSampleRate() const;

  wxSoundFormatType GetType() const { return wxSOUND_ULAW; }
  wxSoundFormatBase *Clone() const;

  wxUint32 GetTimeFromByte(wxUint32 bytes) const;
  wxUint32 GetByteFromTime(wxUint32 time) const;

  bool operator !=(const wxSoundFormatBase& frmt2) const;

 protected:
  wxUint32 m_srate;
};

//
// ULAW converter class
//

class WXDLLEXPORT wxSoundRouterStream;
class WXDLLEXPORT wxSoundStreamUlaw: public wxSoundStreamCodec {
 public:
  wxSoundStreamUlaw(wxSoundStream& sndio);
  ~wxSoundStreamUlaw();

  wxSoundStream& Read(void *buffer, size_t len);
  wxSoundStream& Write(const void *buffer, size_t len);

  bool SetSoundFormat(const wxSoundFormatBase& format);

 protected:
  wxSoundRouterStream *m_router;
};

#endif
