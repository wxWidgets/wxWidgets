// /////////////////////////////////////////////////////////////////////////////
// Name:       sndwav.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    February 1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
/* Real -*- C++ -*- */
#ifndef __SND_wav_H__
#define __SND_wav_H__
#ifdef __GNUG__
#pragma interface
#endif

#include "mmriff.h"
#include "sndfile.h"

///
class wxSndWavCodec : public wxSndFileCodec {
  ///
  DECLARE_DYNAMIC_CLASS(wxSndWavCodec)
public:
  ///
  wxSndWavCodec();
  ///
  wxSndWavCodec(wxInputStream& s, bool preload = FALSE, bool seekable = TRUE);
  ///
  wxSndWavCodec(wxOutputStream& s, bool seekable = TRUE);
  ///
  wxSndWavCodec(const wxString& fname);
  ///
  virtual ~wxSndWavCodec();

  virtual bool OnNeedData(char *buf, wxUint32 size);
  virtual bool OnWriteData(char *buf, wxUint32 size);

  virtual wxUint32 PrepareToPlay();
  virtual bool PrepareToRecord(wxUint32 file_size);

  virtual void SetFile(wxInputStream& s, bool preload = FALSE,
                       bool seekable = FALSE);
  virtual void SetFile(wxOutputStream& s,
                       bool seekable = FALSE);

protected:
  wxRiffCodec riff_codec;
  struct {
    wxUint16 format;
    wxUint16 channels;
    wxUint32 sample_fq;
    wxUint32 byte_p_sec;
    wxUint16 byte_p_spl;
    wxUint16 bits_p_spl;
  } wav_hdr;
};

#endif
