#ifndef __MEDIA_SNDMULAW_H__
#define __MEDIA_SNDMULAW_H__

#ifdef __GNUG__
#pragma interface
#endif

#include "sndfrmt.h"

class wxSoundMulawCodec : public wxSoundCodec {
  DECLARE_DYNAMIC_CLASS(wxSoundMulawCodec)
 public:
  wxSoundMulawCodec();
  virtual ~wxSoundMulawCodec();

  void SetSampleRate(int srate) { m_srate = srate; }

  size_t GetByteRate() const;
  wxSoundDataFormat GetPreferredFormat(int codec = 0) const;

  void Decode();
  void Encode();

 protected:
  int m_srate;
};

#endif
