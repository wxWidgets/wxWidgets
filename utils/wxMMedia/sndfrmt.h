#ifndef __SNDFRMT_H__
#define __SNDFRMT_H__

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/object.h>
#include <wx/stream.h>

class wxSndBuffer;

// Standard Microsoft types (why change ?)
#define WXSOUND_PCM   0x0001
#define WXSOUND_ADPCM 0x0002
#define WXSOUND_ALAW  0x0006
#define WXSOUND_ULAW  0x0007

class wxSoundCodec;
class wxSoundDataFormat {
 public:
  wxSoundDataFormat();
  wxSoundDataFormat(const wxSoundDataFormat& format);
  ~wxSoundDataFormat();

  void SetSampleRate(int srate) { m_srate = srate; }
  void SetChannels(int channels);
  void SetStereo(bool on);
  void SetCodecNo(int no);
  void SetCodecCreate(bool create) { m_codcreate = create; }

  int GetSampleRate() const { return m_srate; }
  int GetChannels()  const { return m_channels; }
  bool GetStereo() const { return (m_channels == 2); }
  int GetCodecNo() const { return m_codno; }

  wxSoundCodec *GetCodec();

  wxSoundDataFormat& operator =(const wxSoundDataFormat& format);
  bool operator ==(const wxSoundDataFormat& format) const;
  bool operator !=(const wxSoundDataFormat& format) const
        { return !(operator ==(format)); }

  /// PCM format
  void SetByteOrder(int order);
  void SetSign(int sign);
  int GetByteOrder() const { return m_byteorder; }
  int GetSign() const { return m_sign; }

  void SetBps(int bps);
  int GetBps() const { return m_bps; }

 protected:
  void CodecChange();

 protected:
  int m_srate, m_bps, m_channels, m_codno;
  int m_byteorder, m_sign;
  bool m_codchange, m_codcreate;
  wxSoundCodec *m_codec;
};

class wxSoundCodec : public wxObject, public wxStreamBase {
  DECLARE_ABSTRACT_CLASS(wxSoundCodec)
 public:
  typedef enum {
    WAITING = 0,
    ENCODING,
    DECODING
  } ModeType;
 public:
  wxSoundCodec();
  virtual ~wxSoundCodec();

  void SetIOBuffer(wxSndBuffer *sndbuf) { m_io_sndbuf = sndbuf; }
  size_t Available();

  void InitIO(const wxSoundDataFormat& format);

  inline void SetInStream(wxStreamBuffer *s)
          { m_in_sound = s; }
  inline void SetOutStream(wxStreamBuffer *s)
          { m_out_sound = s; }
  inline wxStreamBuffer *GetInStream() const { return m_in_sound; }
  inline wxStreamBuffer *GetOutStream() const { return m_out_sound; }

  inline bool StreamOk() const
       { return (m_in_sound->Stream()->LastError() == wxStream_NOERROR) &&
                (m_out_sound->Stream()->LastError() == wxStream_NOERROR); }

  virtual size_t GetByteRate() const = 0; 
  virtual wxSoundDataFormat GetPreferredFormat(int codec = 0) const = 0; 

  virtual void InitMode(ModeType mode);
  virtual void ExitMode();
  virtual void Decode() = 0;
  virtual void Encode() = 0;

  static wxSoundCodec *Get(int no);

 protected:
  void CopyToOutput();

  unsigned short Convert8_16(unsigned char s) { return (s & 0xff) << 8; }
  unsigned char Convert16_8(unsigned short s) { return (s & 0xff00) >> 8; }

  bool ChainCodecBefore(wxSoundDataFormat& cod_to);
  bool ChainCodecAfter(wxSoundDataFormat& cod_to);

  // -------------
  // wxStream part
  // -------------
  size_t OnSysWrite(const void *buffer, size_t bsize);
  size_t OnSysRead(void *buffer, size_t bsize);

 protected:
  wxSndBuffer *m_io_sndbuf;
  wxSoundDataFormat m_io_format;
  wxStreamBuffer *m_in_sound, *m_out_sound;
  wxSoundCodec *m_chain_codec;
  bool m_init, m_chain_before;
  ModeType m_mode;
};

#endif
