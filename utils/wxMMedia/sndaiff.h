// /////////////////////////////////////////////////////////////////////////////
// Name:       sndaiff.h
// Purpose:    wxMMedia Aiff Codec
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    February 1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
/* Real -*- C++ -*- */
#ifndef __SND_aiff_H__
#define __SND_aiff_H__
#ifdef __GNUG__
#pragma interface
#endif

#include "mmriff.h"
#include "sndfile.h"

///
class wxSndAiffCodec : public wxSndFileCodec {
  ///
  DECLARE_DYNAMIC_CLASS(wxSndAiffCodec)
public:
  ///
  wxSndAiffCodec();
  ///
  wxSndAiffCodec(wxInputStream& s, bool preload = FALSE, bool seekable = TRUE);
  ///
  wxSndAiffCodec(wxOutputStream& s, bool seekable = TRUE);
  ///
  wxSndAiffCodec(const wxString& fname);
  ///
  virtual ~wxSndAiffCodec();

  virtual bool OnNeedData(char *buf, wxUint32 size);
  virtual bool OnWriteData(char *buf, wxUint32 size);

  virtual wxUint32 PrepareToPlay();
  virtual bool PrepareToRecord(wxUint32 file_size);

  virtual void SetFile(wxInputStream& s, bool preload = FALSE,
                       bool seekable = FALSE);
  virtual void SetFile(wxOutputStream& s,
                       bool seekable = FALSE);
protected:
  void ParseCOMM();
  void ParseSSND();

  void WriteCOMM();
  void WriteSSND(wxUint32 file_size);

protected:
  wxUint32 m_spos, m_slen;
  wxUint32 m_chunksize;
};

#endif
