// /////////////////////////////////////////////////////////////////////////////
// Name:       mmriff.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1998
// Updated:
// Copyright:  (C) 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
/* Real -*- C++ -*- */
#ifndef __MM_riff_H__
#define __MM_riff_H__
#ifdef __GNUG__
#pragma interface
#endif

#include "mmtype.h"
#include "mmfile.h"

#define RIFF_READ 0
#define RIFF_WRITE 1
#define INVALID_CHUNK_LEN ((wxUint32)-1)

///
class wxRiffCodec {
public:
  ///
  wxRiffCodec();
  ///
  wxRiffCodec(wxOutputStream& s);
  ///
  wxRiffCodec(wxInputStream& s);
  ///
  virtual ~wxRiffCodec();

  ///
  void SetFile(wxInputStream& s) { cout << "RIFF:SetFile(i)" << endl; riff_i_stream = &s; riff_o_stream = NULL; }
  ///
  void SetFile(wxOutputStream& s) { cout << "RIFF::SetFile(o)" << endl; riff_i_stream = NULL; riff_o_stream = &s; }

  ///
  bool Read32(wxUint32& i32);
  ///
  bool Read16(wxUint16& i16);
  ///
  bool ReadData(void *data, wxUint32 size);
  ///
  bool Skip(wxUint32 skip);

  ///
  bool Write32(wxUint32 i32);
  ///
  bool Write16(wxUint16 i16);
  ///
  bool WriteData(void *data, wxUint32 size);

  ///
  inline wxUint32 GetChunkLength() const { return chunk_length; }
  ///
  inline const wxString& GetChunkName() const { return m_chunk; }

  ///
  bool RiffReset(wxUint8 state);

  ///
  bool FindChunk(const wxString& name = "", bool from_here = FALSE);
  ///
  bool CreateChunk(const wxString& name, wxUint32 chunk_size);

  ///
  inline wxMMFileError GetError() const { return riff_error; }

protected:
  wxInputStream *riff_i_stream;
  wxOutputStream *riff_o_stream;
  wxUint32 chunk_length;
  wxMMFileError riff_error;
  wxString m_chunk;
};

#endif
