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
  friend class wxSoundDataFormat;
  wxSoundDataFormat m_orig_format;
  char m_char_stack;
  bool m_char_bool;
};

#endif
