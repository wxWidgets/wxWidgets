#ifndef __SNDPCM_H__
#define __SNDPCM_H__

#ifdef __GNUG__
#pragma interface
#endif

#include "sndfrmt.h"

class wxSoundPcmCodec : public wxSoundCodec {
  DECLARE_DYNAMIC_CLASS(wxSoundPcmCodec)
 public:
  wxSoundPcmCodec();
  virtual ~wxSoundPcmCodec();

  void SetSampleRate(int srate) { m_orig_format.SetSampleRate(srate); }
  void SetBits(int bits) { m_orig_format.SetBps(bits); }
  void SetByteOrder(int order) { m_orig_format.SetByteOrder(order); }
  void SetSign(int sample_sign) { m_orig_format.SetSign(sample_sign); }

  size_t GetByteRate() const;
  wxSoundDataFormat GetPreferredFormat(int codec = 0) const;

  void Decode();
  void Encode();

 protected:
  void InputSign8();
  void InputSwapAndSign16();
  void OutputSign8();
  void OutputSwapAndSign16();

 protected:
  wxSoundDataFormat m_orig_format;
  char m_char_stack;
  bool m_char_bool;
};

#endif
