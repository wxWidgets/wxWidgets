// --------------------------------------------------------------------------
// Name: sndfile.h
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDFILE_H
#define _WX_SNDFILE_H

#include <wx/defs.h>
#include <wx/stream.h>
#include <stdlib.h>
#include "sndbase.h"
#include "sndcodec.h"

#define wxSOUND_INFINITE_TIME ((unsigned long)-1)

//
// Codec router class
//

class WXDLLEXPORT wxSoundRouterStream: public wxSoundStreamCodec {
 public:
  wxSoundRouterStream(wxSoundStream& sndio);
  ~wxSoundRouterStream();

  wxSoundStream& Read(void *buffer, wxUint32 len);
  wxSoundStream& Write(const void *buffer, wxUint32 len);

  bool SetSoundFormat(const wxSoundFormatBase& format);

  bool StartProduction(int evt);
  bool StopProduction();

  wxUint32 GetBestSize() const;

 protected:
  wxSoundStream *m_router;
};

typedef enum {
 wxSOUND_FILE_STOPPED,
 wxSOUND_FILE_PAUSED,
 wxSOUND_FILE_PLAYING,
 wxSOUND_FILE_RECORDING
} wxSoundFileState;

//
// Base class for file coders/decoders
//

class wxSoundFileStream: public wxSoundStream {
 public:
  wxSoundFileStream(wxInputStream& stream, wxSoundStream& io_sound);
  wxSoundFileStream(wxOutputStream& stream, wxSoundStream& io_sound);
  ~wxSoundFileStream();

  bool Play();
  bool Record(unsigned long time);
  bool Stop();
  bool Pause();
  bool Resume();

  bool IsStopped() const { return m_state == wxSOUND_FILE_STOPPED; }

  bool StartProduction(int evt);
  bool StopProduction();

  unsigned long GetLength() const;

  wxSoundStream& Read(void *buffer, wxUint32 len); 
  wxSoundStream& Write(const void *buffer, wxUint32 len);

  void SetDuplexMode(bool duplex);

  bool SetSoundFormat(const wxSoundFormatBase& format);

  virtual bool CanRead() { return FALSE; }

 protected:
  wxSoundRouterStream m_codec; 
  wxSoundStream *m_sndio;
  wxInputStream *m_input;
  wxOutputStream *m_output;

  wxSoundFileState m_state, m_oldstate;
  wxUint32 m_len;

 protected:
  virtual bool PrepareToPlay() = 0; 
  virtual bool PrepareToRecord(unsigned long time) = 0;
  virtual bool FinishRecording() = 0;

  virtual wxUint32 GetData(void *buffer, wxUint32 len) = 0;
  virtual wxUint32 PutData(const void *buffer, wxUint32 len) = 0;

  void OnSoundEvent(int evt);
};

#endif
