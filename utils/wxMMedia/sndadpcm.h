#ifndef __MEDIA_SNDADPCM_H__
#define __MEDIA_SNDADPCM_H__

#ifdef __GNUG__
#pragma interface
#endif

#include "sndfrmt.h"

class wxSoundAdpcmCodec : public wxSoundCodec {
  DECLARE_DYNAMIC_CLASS(wxSoundAdpcmCodec)
 public:
  wxSoundAdpcmCodec();
  ~wxSoundAdpcmCodec();

  size_t GetByteRate() const;
  wxSoundDataFormat GetPreferredFormat(int codec = 0) const;
  int GetBits(int bits);

  void Decode();
  void Encode();

  void InitWith(const wxSoundDataFormat& format);
 protected:
  struct g72x_state *m_codec_state;
  int m_bits_waiting, m_current_byte;
  int m_srate;
};

#endif
