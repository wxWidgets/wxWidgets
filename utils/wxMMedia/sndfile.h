// /////////////////////////////////////////////////////////////////////////////
// Name:       sndfile.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
/* Real -*- C++ -*- */
#ifndef __SND_file_H__
#define __SND_file_H__

#ifdef __GNUG__
#pragma interface
#endif

#include "mmfile.h"
#include "sndsnd.h"

///
class wxSndFileCodec : public wxMMediaFile, public wxSndBuffer {
  ///
  DECLARE_ABSTRACT_CLASS(wxSndFileCodec)
public:

  typedef enum {
    wxSFILE_STOPPED,
    wxSFILE_PLAYING,
    wxSFILE_RECORDING
  } FileState;

protected:
  wxUint32 m_fsize, m_fpos, m_fdone;
  FileState m_fstate;
  wxMMtime m_sndtime;
public:

  ///
  wxSndFileCodec();
  ///
  wxSndFileCodec(wxInputStream& s, bool preload, bool seekable);
  ///
  wxSndFileCodec(wxOutputStream& s, bool seekable);
  ///
  wxSndFileCodec(const wxString& fname);
  ///
  virtual ~wxSndFileCodec();

  ///
  void Play(wxSound& snd);
  ///
  void Stop(wxSound& snd);
  ///
  void Record(wxSound& snd,
              const wxSoundDataFormat& format, wxUint32 seconds);

  ///
  void OnNeedOutputData(char *data, wxUint32& size);
  ///
  void OnBufferInFinished(char *iobuf, wxUint32& size);

  ///
  virtual bool OnNeedData(char *buf, wxUint32 size) = 0;
  ///
  virtual bool OnWriteData(char *buf, wxUint32 size) = 0;
  ///
  virtual wxUint32 PrepareToPlay() = 0;
  ///
  virtual bool PrepareToRecord(wxUint32 file_size) = 0;
  ///
  virtual bool TranslateBuffer(wxSndBuffer& buf);
  ///
  virtual bool RestartBuffer(wxSndMode mode);
  ///
  virtual wxUint32 GetSize() const;
  ///
  virtual wxUint32 Available() const;
  ///
  virtual wxMMtime GetLength();

  ///
  wxMMtime GetPosition();

  ///
  virtual bool StartPlay();
  ///
  virtual void StopPlay();
};

#endif
