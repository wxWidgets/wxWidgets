// --------------------------------------------------------------------------
// Name: sndmsad(pcm).h
// Purpose: MS ADPCM codec
// Date: 25/02/2000
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 2000
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifndef _WX_SNDMSAD_H
#define _WX_SNDMSAD_H

#ifdef __GNUG__
#pragma interface "sndmsad.h"
#endif

#include "wx/defs.h"
#include "wx/dynarray.h"
#include "wx/mmedia/sndcodec.h"
#include "wx/mmedia/sndbase.h"

WX_DEFINE_EXPORTED_ARRAY(wxInt16, wxMSAdpcmCoeffs);

//
// MSADPCM format
//
class WXDLLEXPORT wxSoundFormatMSAdpcm: public wxSoundFormatBase {
public:
    wxSoundFormatMSAdpcm();
    ~wxSoundFormatMSAdpcm();
    
    void SetSampleRate(wxUint32 srate);
    wxUint32 GetSampleRate() const;
    
    void SetCoefs(wxInt16 **coefs, wxUint16 ncoefs, wxUint16 coefs_len);
    void GetCoefs(wxInt16 **&coefs, wxUint16& ncoefs,
                  wxUint16& coefs_len) const;

    void SetBlockSize(wxUint16 block_size);
    wxUint16 GetBlockSize() const;
    
    void SetChannels(wxUint16 channels);
    wxUint16 GetChannels() const;
    
    wxSoundFormatType GetType() const { return wxSOUND_MSADPCM; }
    wxSoundFormatBase *Clone() const;
    
    wxUint32 GetTimeFromBytes(wxUint32 bytes) const;
    wxUint32 GetBytesFromTime(wxUint32 time) const;
    
    bool operator !=(const wxSoundFormatBase& frmt2) const;

protected:
    wxUint32 m_srate, m_nchannels;
    wxInt16 **m_coefs;
    wxUint16 m_ncoefs, m_coefs_len;
    wxUint16 m_block_size;
};

//
// MS ADPCM converter class
//
class WXDLLEXPORT wxSoundRouterStream;
class WXDLLEXPORT wxSoundStreamMSAdpcm: public wxSoundStreamCodec {
public:
    wxSoundStreamMSAdpcm(wxSoundStream& sndio);
    ~wxSoundStreamMSAdpcm();
    
    wxSoundStream& Read(void *buffer, wxUint32 len);
    wxSoundStream& Write(const void *buffer, wxUint32 len);
    
    bool SetSoundFormat(const wxSoundFormatBase& format);

    wxUint32 GetBestSize() const;

protected:
    wxSoundRouterStream *m_router;

    typedef struct {
        wxInt32 predictor;
        wxInt16 samp1;
        wxInt16 samp2;
        wxInt16 coeff[2];
        wxInt32 iDelta;
    } AdpcmState;

    AdpcmState m_state[1];
    
    bool     m_got_header;
    bool     m_stereo;
    wxInt16  **m_coefs;
    wxUint16 m_block_size;
    wxUint16 m_ncoefs;
    wxUint16 m_next_block;
    
protected:
    wxUint32 DecodeMonoADPCM(const void *in_buffer, void *out_buffer,
                             wxUint32 in_len);
    wxUint32 DecodeStereoADPCM(const void *in_buffer, void *out_buffer,
                               wxUint32 in_len);
    void Nibble(wxInt8 nyb,
                AdpcmState *state,
                wxInt16 **out_buffer);
};

#endif
