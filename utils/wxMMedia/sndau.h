// /////////////////////////////////////////////////////////////////////////////
// Name:       sndau.h
// Purpose:    wxMMedia Sun Audio File Codec
// Author:     Guilhem Lavaux
// Created:    1998
// Updated:
// Copyright:  (C) 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
/* Real -*- C++ -*- */
#ifndef __SND_au_H__
#define __SND_au_H__
#ifdef __GNUG__
#pragma interface
#endif

#include "mmriff.h"
#include "sndfile.h"

///
class wxSndAuCodec : public wxSndFileCodec {
  ///
  DECLARE_DYNAMIC_CLASS(wxSndAuCodec)
public:
  ///
  wxSndAuCodec();
  ///
  wxSndAuCodec(wxInputStream& s, bool preload = FALSE, bool seekable = TRUE);
  ///
  wxSndAuCodec(wxOutputStream& s, bool seekable = TRUE);
  ///
  wxSndAuCodec(const wxString& fname);
  ///
  virtual ~wxSndAuCodec();

  bool OnNeedData(char *buf, wxUint32 size);
  bool OnWriteData(char *buf, wxUint32 size);

  wxUint32 PrepareToPlay();
  bool PrepareToRecord(wxUint32 file_size);

protected:
  wxUint32 m_spos, m_slen;
  wxUint32 m_chunksize;
};

#endif
