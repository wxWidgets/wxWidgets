////////////////////////////////////////////////////////////////////////////////
// Name:       mmriff.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "mmriff.h"
#endif

#ifdef WX_PRECOMP
#include "wx_prec.h"
#else
#include "wx/wx.h"
#endif
#include "mmfile.h"
#include "mmriff.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

wxRiffCodec::wxRiffCodec() :
  riff_i_stream(NULL), riff_o_stream(NULL), chunk_length(INVALID_CHUNK_LEN)
{
}

wxRiffCodec::wxRiffCodec(wxInputStream& s) :
  riff_i_stream(&s), riff_o_stream(NULL), chunk_length(INVALID_CHUNK_LEN)
{
}

wxRiffCodec::wxRiffCodec(wxOutputStream& s) :
  riff_i_stream(NULL), riff_o_stream(&s), chunk_length(INVALID_CHUNK_LEN)
{
}

wxRiffCodec::~wxRiffCodec()
{
}

bool wxRiffCodec::RiffReset(wxUint8 mode)
{
  switch (mode) {
  case RIFF_READ:
    if (!riff_i_stream)
      return FALSE;
    riff_i_stream->SeekI(0);
    chunk_length = INVALID_CHUNK_LEN;
    return TRUE;
  case RIFF_WRITE:
    if (!riff_o_stream)
      return FALSE;
    riff_o_stream->SeekO(0);
    chunk_length = INVALID_CHUNK_LEN; 
    return TRUE;
  }
  return FALSE;
}

bool wxRiffCodec::ReadData(void *data, wxUint32 size)
{
  if (!riff_i_stream)
    return FALSE;

  if (chunk_length != INVALID_CHUNK_LEN && (wxUint32)chunk_length < size) {
    riff_error = wxMMFILE_EOF;
    return FALSE;
  }
  if (chunk_length != INVALID_CHUNK_LEN)
    chunk_length -= size;

  bool ret = (riff_i_stream->Read((char *)data, size).LastRead() == size);

  return ret;
}

bool wxRiffCodec::WriteData(void *data, wxUint32 size)
{
  if (!riff_o_stream)
    return FALSE;

  if (chunk_length < size) {
    riff_error = wxMMFILE_EOF;
    return FALSE;
  }
  chunk_length -= size;

  riff_o_stream->Write(data, size);

  return TRUE;
}

bool wxRiffCodec::Read32(wxUint32& i32)
{
  wxUint8 i8[4];

  if (!ReadData(i8, 4))
    return FALSE;

  i32 = i8[0];
  i32 |= ((wxUint32)i8[1]) << 8;
  i32 |= ((wxUint32)i8[2]) << 16;
  i32 |= ((wxUint32)i8[3]) << 24;
  
  return TRUE;
}

bool wxRiffCodec::Write32(wxUint32 i32)
{
  wxUint8 i8[4];

  i8[0] = i32 & 0xff;
  i8[1] = (i32 >> 8) & 0xff;
  i8[2] = (i32 >> 16) & 0xff;
  i8[3] = (i32 >> 24) & 0xff;

  if (!WriteData(i8, 4))
    return FALSE;

  return TRUE;
}

bool wxRiffCodec::Read16(wxUint16& i16)
{
  wxUint8 i8[2];

  if (!ReadData(i8, 2))
    return FALSE;

  i16 = i8[0];
  i16 |= ((wxUint16)i8[1]) << 8;
  
  return TRUE;
}

bool wxRiffCodec::Write16(wxUint16 i16)
{
  wxUint8 i8[2];

  i8[0] = i16 & 0xff;
  i8[1] = (i16 >> 8) & 0xff;

  if (!WriteData(i8, 2))
    return FALSE;

  return TRUE;
}

bool wxRiffCodec::Skip(wxUint32 skip)
{
  if (!riff_i_stream || (chunk_length != INVALID_CHUNK_LEN && (wxInt32)skip > chunk_length))
    return FALSE;

  if (chunk_length != INVALID_CHUNK_LEN)
    chunk_length -= skip;
  riff_i_stream->SeekI(skip, wxFromCurrent);

  return TRUE;
}

bool wxRiffCodec::CreateChunk(const wxString& name, wxUint32 size)
{
  if (!riff_o_stream || name.Length() != 4)
    return FALSE;

  if (riff_o_stream->Write(name.GetData(), 4).LastError()) {
    riff_error = wxMMFILE_EOF;
    return FALSE;
  }

  chunk_length = size+4;

  return Write32(size);
}

bool wxRiffCodec::FindChunk(const wxString& name, bool from_here)
{
  char buf[5];
  wxString str2;

  if (!riff_i_stream)
    return FALSE;

  if (chunk_length != INVALID_CHUNK_LEN && !from_here)
    Skip(chunk_length);
  while (1) {
    if (riff_i_stream->Read(buf, 4).LastError()) {
      riff_error = wxMMFILE_EOF;
      return FALSE;
    }

    chunk_length = INVALID_CHUNK_LEN;
    if (!Read32(chunk_length)) {
      riff_error = wxMMFILE_EOF;
      return FALSE;
    }

    buf[4] = 0;
    str2 = buf;
    if ((!name.IsNull()) && str2 != name) {
      Skip(chunk_length);
      continue;
    }

    m_chunk = str2;

    return TRUE;
  }

  return TRUE;
}
