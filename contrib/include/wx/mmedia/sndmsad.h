// --------------------------------------------------------------------------
// Name: sndmsad(pcm).h
// Purpose: MS ADPCM codec
// Date: 25/02/2000
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 2000
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDULAW_H
#define _WX_SNDULAW_H

#ifdef __GNUG__
#pragma interface "sndmsad.h"
#endif

#include "wx/defs.h"
#include "wx/dynarray.h"
#include "wx/mmedia/sndcodec.h"
#include "wx/mmedia/sndbase.h"

WX_DEFINE_EXPORTED_ARRAY(wxUint16, wxMSAdpcmCoeffs);

//
// MSADPCM format
//
class WXDLLEXPORT wxSoundFormatMSAdpcm: public wxSoundFormatBase {
 public:
  wxSoundFormatMSAdpcm();
  ~wxSoundFormatMSAdpcm();

  void SetSampleRate(wxUint32 srate);
  wxUint32 GetSampleRate() const;

  void SetSamplesBlock(wxUint16 sampblock);
  wxUint16 GetSamplesBlock() const;
  
  void SetCoefs(wxMSAdpcmCoefs& coefs);
  wxMSAdpcmCoefs& GetCoefs() const;
  
  wxSoundFormatType GetType() const { return wxSOUND_ULAW; }
  wxSoundFormatBase *Clone() const;

  wxUint32 GetTimeFromBytes(wxUint32 bytes) const;
  wxUint32 GetBytesFromTime(wxUint32 time) const;

  bool operator !=(const wxSoundFormatBase& frmt2) const;

 protected:
  wxUint32 m_srate;
  wxMSAdpcmCoefs *m_coefs;
};

//
// MS ADPCM converter class
//
class WXDLLEXPORT wxSoundRouterStream;
class WXDLLEXPORT wxSoundStreamAdpcm: public wxSoundStreamCodec {
public:
    wxSoundStreamAdpcm(wxSoundStream& sndio);
    ~wxSoundStreamAdpcm();
    
    wxSoundStream& Read(void *buffer, wxUint32 len);
    wxSoundStream& Write(const void *buffer, wxUint32 len);
    
    bool SetSoundFormat(const wxSoundFormatBase& format);

    wxUint32 GetBestSize() const;

protected:
    wxSoundRouterStream *m_router;
};

#endif
