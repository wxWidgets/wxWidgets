// --------------------------------------------------------------------------
// Name: sndg72x.h
// Purpose:
// Date: 08/26/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDG72X_H
#define _WX_SNDG72X_H

#ifdef __GNUG__
#pragma interface "sndg72x.h"
#endif

#include <stddef.h>
#include "sndcodec.h"
#include "sndbase.h"

typedef enum {
  wxSOUND_G721,
  wxSOUND_G723_24,
  wxSOUND_G723_40
} wxSoundG72XType;

// This fixes a bug in Mingw95
typedef struct g72x_state g72state;

//
// G72X format
//
class WXDLLEXPORT wxSoundFormatG72X: public wxSoundFormatBase {
 public:
  wxSoundFormatG72X();
  ~wxSoundFormatG72X();

  void SetG72XType(wxSoundG72XType type);
  wxSoundG72XType GetG72XType() const { return m_g72x_type; }

  void SetSampleRate(wxUint32 srate);
  wxUint32 GetSampleRate() const;

  wxSoundFormatType GetType() const { return wxSOUND_G72X; }
  wxSoundFormatBase *Clone() const;

  wxUint32 GetTimeFromBytes(wxUint32 bytes) const;
  wxUint32 GetBytesFromTime(wxUint32 time) const;

  bool operator !=(const wxSoundFormatBase& frmt2) const;

 protected:
  wxUint32 m_srate;
  wxSoundG72XType m_g72x_type;
};

//
// ULAW converter class
//

class WXDLLEXPORT wxSoundRouterStream;
class WXDLLEXPORT wxSoundStreamG72X: public wxSoundStreamCodec {
 public:
  wxSoundStreamG72X(wxSoundStream& sndio);
  ~wxSoundStreamG72X();

  wxSoundStream& Read(void *buffer, wxUint32 len);
  wxSoundStream& Write(const void *buffer, wxUint32 len);

  bool SetSoundFormat(const wxSoundFormatBase& format);

 protected:
  wxSoundRouterStream *m_router;
  wxUint8 m_n_bits, m_current_mask, m_current_b_pos, m_current_byte;
  wxUint8 *m_io_buffer;
  g72state *m_state;

  int (*m_coder)(int code, int in_code, struct g72x_state *state);
  int (*m_decoder)(int code, int out_code, struct g72x_state *state);

 protected:
  void PutBits(wxUint8 bits);
  wxUint8 GetBits();
};

#endif
