// --------------------------------------------------------------------------
// Name: sndulaw.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// wxWindows licence
// --------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/defs.h"
  #include "wx/memory.h"
  #include "wx/log.h"
#endif

#include "wx/mmedia/sndbase.h"
#include "wx/mmedia/sndfile.h"
#include "wx/mmedia/sndpcm.h"
#include "wx/mmedia/sndmsad.h"

// --------------------------------------------------------------------------
// wxSoundFormatMSAdpcm
// --------------------------------------------------------------------------

wxSoundFormatMSAdpcm::wxSoundFormatMSAdpcm()
        : m_srate(22050)
{
    m_ncoefs    = 0;
    m_coefs_len = 0;
    m_coefs     = NULL;
}

wxSoundFormatMSAdpcm::~wxSoundFormatMSAdpcm()
{
    if (m_ncoefs) {
        wxUint16 i;

        for (i=0;i<m_ncoefs;i++)
            delete[] m_coefs[i];
        delete[] m_coefs;
    }
    
}

void wxSoundFormatMSAdpcm::SetSampleRate(wxUint32 srate)
{
    m_srate = srate;
}

wxUint32 wxSoundFormatMSAdpcm::GetSampleRate() const
{
    return m_srate;
}

void wxSoundFormatMSAdpcm::SetChannels(wxUint16 nchannels)
{
    m_nchannels = nchannels;
}

wxUint16 wxSoundFormatMSAdpcm::GetChannels() const
{
    return m_nchannels; 
}

void wxSoundFormatMSAdpcm::SetCoefs(wxInt16 **WXUNUSED(coefs), wxUint16 ncoefs,
                                    wxUint16 coefs_len)
{
    wxUint16 i;

    if (m_ncoefs) {
        for (i=0;i<m_ncoefs;i++)
           delete[] (m_coefs[i]);
        delete[] m_coefs;
    }
    // TODO: Add some memory checking here
    m_coefs = new wxInt16 *[ncoefs];

    for (i=0;i<ncoefs;i++)
       m_coefs[i] = new wxInt16[coefs_len];

    m_ncoefs = ncoefs;
    m_coefs_len = coefs_len;
}

void wxSoundFormatMSAdpcm::GetCoefs(wxInt16 **& coefs, wxUint16& ncoefs,
                                    wxUint16& coefs_len) const
{
    coefs     = m_coefs;
    ncoefs    = m_ncoefs;
    coefs_len = m_coefs_len;
}

void wxSoundFormatMSAdpcm::SetBlockSize(wxUint16 block_size)
{
    m_block_size = block_size;
}

wxUint16 wxSoundFormatMSAdpcm::GetBlockSize() const
{
    return m_block_size;
}

wxSoundFormatBase *wxSoundFormatMSAdpcm::Clone() const
{
    wxSoundFormatMSAdpcm *adpcm = new wxSoundFormatMSAdpcm();
    
    adpcm->m_srate       = m_srate;
    adpcm->SetCoefs(m_coefs, m_ncoefs, m_coefs_len);
    adpcm->m_nchannels   = m_nchannels;
    adpcm->m_block_size  = m_block_size;
    return adpcm;
}

wxUint32 wxSoundFormatMSAdpcm::GetTimeFromBytes(wxUint32 bytes) const
{
    return 2 * bytes / (m_nchannels * m_srate);
}

wxUint32 wxSoundFormatMSAdpcm::GetBytesFromTime(wxUint32 time) const
{
    return time * m_nchannels * m_srate / 2;
}

bool wxSoundFormatMSAdpcm::operator !=(const wxSoundFormatBase& frmt2) const
{
    const wxSoundFormatMSAdpcm *adpcm = (const wxSoundFormatMSAdpcm *)&frmt2;
    
    if (frmt2.GetType() != wxSOUND_MSADPCM)
        return true;
    
    return (adpcm->m_srate != m_srate) && (adpcm->m_nchannels != m_nchannels);
}

// --------------------------------------------------------------------------
// wxSoundStreamMSAdpcm
// --------------------------------------------------------------------------
wxSoundStreamMSAdpcm::wxSoundStreamMSAdpcm(wxSoundStream& sndio)
        : wxSoundStreamCodec(sndio)
{
    // PCM converter
    m_router     = new wxSoundRouterStream(sndio);
    m_got_header = false;
    m_stereo = false;
}

wxSoundStreamMSAdpcm::~wxSoundStreamMSAdpcm()
{
    delete m_router;
}

wxSoundStream& wxSoundStreamMSAdpcm::Read(void *WXUNUSED(buffer), wxUint32 WXUNUSED(len))
{
    m_snderror = wxSOUND_NOCODEC;
    m_lastcount = 0;
    return *this;
}

static wxInt16 gl_ADPCMcoeff_delta[] = {
    230, 230, 230, 230, 307, 409, 512, 614, 768, 614, 512, 409, 307,
    230, 230, 230
};

wxUint32 wxSoundStreamMSAdpcm::DecodeMonoADPCM(const void *in_buffer,
                                               void *out_buffer,
                                               wxUint32 in_len)
{
    wxUint8  *ADPCMdata;
    wxInt16  *PCMdata;
    AdpcmState *state;
    wxUint32 out_len;
    
    ADPCMdata = (wxUint8 *)in_buffer;
    PCMdata   = (wxInt16 *)out_buffer;
    state     = &m_state[0];
    
#define GET_DATA_16(i) i = *ADPCMdata++, i |= ((wxUint32)(*ADPCMdata++) << 8)
#define GET_DATA_8(i) i = (*ADPCMdata++)

    out_len = 0;
    while (in_len != 0) {
        if (m_next_block == 0) {
            GET_DATA_8(state->predictor);
            GET_DATA_16(state->iDelta);
            
            GET_DATA_16(state->samp1);
            GET_DATA_16(state->samp2);
            
            state->coeff[0] = state->coeff[1] = m_coefs[0][ state->predictor ];
            
            *PCMdata++ = state->samp2;
            *PCMdata++ = state->samp1;
            in_len     -= 7;
            out_len    += 4;
            m_next_block = m_block_size;
            continue;
        }
    
        while (in_len != 0 && m_next_block != 0) {
            wxUint8 nib[2];
            
            GET_DATA_8(nib[0]);
            nib[1] = (nib[0] >> 4) & 0x0f;
            nib[0] &= 0x0f;
        
            Nibble(nib[0], state, &PCMdata);
            Nibble(nib[1], state, &PCMdata);
        
            in_len       -= 4;
            out_len      += 4;
            m_next_block -= 4;
        }
    }

    return out_len;

#undef GET_DATA_16
#undef GET_DATA_8
}

wxUint32 wxSoundStreamMSAdpcm::DecodeStereoADPCM(const void *in_buffer,
                                                 void *out_buffer,
                                                 wxUint32 in_len)
{            
    wxUint8  *ADPCMdata;
    wxInt16  *PCMdata;
    AdpcmState *state0, *state1;
    wxUint32 out_len;
    
    ADPCMdata = (wxUint8 *)in_buffer;
    PCMdata   = (wxInt16 *)out_buffer;
    
    state0 = &m_state[0];
    state1 = &m_state[1];
    
#define GET_DATA_16(i) i = *ADPCMdata++, i |= ((wxUint32)(*ADPCMdata++) << 8)
#define GET_DATA_8(i) i = (*ADPCMdata++)

    out_len = 0;
    while (in_len != 0) {
        if (!m_next_block) {
            GET_DATA_8(state0->predictor);
            GET_DATA_8(state1->predictor);
            
            GET_DATA_16(state0->iDelta);
            GET_DATA_16(state1->iDelta);
        
            GET_DATA_16(state0->samp1);
            GET_DATA_16(state1->samp1);
            GET_DATA_16(state0->samp2);
            GET_DATA_16(state1->samp2);
        
            *PCMdata++ = state0->samp2;
            *PCMdata++ = state1->samp2;
            *PCMdata++ = state0->samp1;
            *PCMdata++ = state1->samp1;

            in_len     -= 14;
            out_len    += 8;
            m_next_block = m_block_size;
            continue;
        }
    
        while (in_len != 0 && m_next_block > 0) {
            wxUint8 nib[2];
            
            GET_DATA_8(nib[0]);
            nib[1] = (nib[0] >> 4) & 0x0f;
            nib[0] &= 0x0f;
            
            Nibble(nib[0], state0, &PCMdata);
            Nibble(nib[1], state1, &PCMdata);
            
            in_len       -= 4;
            out_len      += 4;
            m_next_block -= 4;
        }
    }

    return out_len;

#undef GET_DATA_16
#undef GET_DATA_8
}

void wxSoundStreamMSAdpcm::Nibble(wxInt8 nyb,
                                  AdpcmState *state,
                                  wxInt16 **out_buffer)
{
    wxUint32 new_delta;
    wxInt32  new_sample;

    // First: compute the next delta value
    new_delta  = (state->iDelta * gl_ADPCMcoeff_delta[nyb]) >> 8;
    // If null, minor it by 16
    if (!new_delta)
        new_delta = 16;

    // Barycentre
    new_sample = (state->samp1 * state->coeff[0] +
                  state->samp2 * state->coeff[1]) / 256;

    // Regenerate the sign
    if (nyb & 0x08)
        nyb -= 0x10;
    
    new_sample += state->iDelta * nyb;

    // Samples must be in [-32767, 32768]
    if (new_sample < -32768)
        new_sample = -32768;
    else if (new_sample > 32767)
        new_sample = 32767;
    
    state->iDelta = new_delta;
    state->samp2  = state->samp1;
    state->samp1  = new_sample;

    *(*out_buffer)++ = new_sample;
}

wxSoundStream& wxSoundStreamMSAdpcm::Write(const void *buffer, wxUint32 len)
{
    wxUint8 *out_buf;
    wxUint32 new_len;
    
    // TODO: prealloc the output buffer
    out_buf = new wxUint8[len*2];

    if (!m_stereo)
        new_len = DecodeMonoADPCM(buffer, out_buf, len);
    else
        new_len = DecodeStereoADPCM(buffer, out_buf, len);
    
    m_router->Write(out_buf, new_len);

    m_lastcount = len;
    m_snderror  = wxSOUND_NOERROR;
    
    delete[] out_buf;
    
    return *this;
}

wxUint32 wxSoundStreamMSAdpcm::GetBestSize() const
{
    return m_sndio->GetBestSize() / 2;
}

bool wxSoundStreamMSAdpcm::SetSoundFormat(const wxSoundFormatBase& format)
{
    if (format.GetType() != wxSOUND_MSADPCM) {
        m_snderror = wxSOUND_INVFRMT;
        return false;
    }
    
    wxSoundFormatPcm pcm;
    wxSoundFormatMSAdpcm *adpcm;
    wxUint16 ncoefs, coefs_len;
    
    wxSoundStreamCodec::SetSoundFormat(format);
    
    adpcm = (wxSoundFormatMSAdpcm *)m_sndformat;

    adpcm->GetCoefs(m_coefs, ncoefs, coefs_len);

    if (!ncoefs) {
        wxLogError(wxT("Number of ADPCM coefficients must be non null"));
        return false;
    }
    
    pcm.SetSampleRate(adpcm->GetSampleRate());
    pcm.SetBPS(16);
    pcm.SetChannels(adpcm->GetChannels());
    pcm.Signed(true);
    pcm.SetOrder(wxBYTE_ORDER);

    m_stereo = (adpcm->GetChannels() == 2);
    m_block_size = adpcm->GetBlockSize();
    m_next_block = 0;
    
    m_router->SetSoundFormat(pcm);
    
    return true;
}

