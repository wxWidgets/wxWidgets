// --------------------------------------------------------------------------
// Name: sndulaw.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "sndulaw.cpp"
#endif

#include <wx/wxprec.h>
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
    m_coefs = new wxMSAdpcmCoefs();
}

wxSoundFormatMSAdpcm::~wxSoundFormatMSAdpcm()
{
    delete m_coefs;
}

void wxSoundFormatMSAdpcm::SetSampleRate(wxUint32 srate)
{
    m_srate = srate;
}

wxUint32 wxSoundFormatMSAdpcm::GetSampleRate() const
{
    return m_srate;
}

wxSoundFormatBase *wxSoundFormatMSAdpcm::Clone() const
{
    wxSoundFormatMSAdpcm *adpcm = new wxSoundFormatMSAdpcm();
    
    adpcm->m_srate = m_srate;
    adpcm->m_coefs = new wxMSAdpcmCoefs();
    *(adpcm->m_coefs) = *m_coefs;
    return adpcm;
}

wxUint32 wxSoundFormatMSAdpcm::GetTimeFromBytes(wxUint32 bytes) const
{
    return 0;
}

wxUint32 wxSoundFormatMSAdpcm::GetBytesFromTime(wxUint32 time) const
{
    return 0;
}

bool wxSoundFormatMSAdpcm::operator !=(const wxSoundFormatBase& frmt2) const
{
    wxSoundFormatUlaw *adpcm = (wxSoundFormatMSAdpcm *)&frmt2;
    
    if (frmt2.GetType() != wxSOUND_MSADPCM)
        return TRUE;
    
    return (adpcm->m_srate != m_srate) && 0;
}

// --------------------------------------------------------------------------
// wxSoundStreamMSAdpcm
// --------------------------------------------------------------------------
wxSoundStreamMSAdpcm::wxSoundStreamMSAdpcm(wxSoundStream& sndio)
        : wxSoundStreamCodec(sndio)
{
    // PCM converter
    m_router     = new wxSoundRouterStream(sndio);
    m_got_header = FALSE;
}

wxSoundStreamMSAdpcm::~wxSoundStreamMSAdpcm()
{
    delete m_router;
}

wxSoundStream& wxSoundStreamMSAdpcm::Read(void *buffer, wxUint32 len)
{
    m_snderror = wxSOUND_NOCODEC;
    m_lastcount = 0;
    return *this;
}

static wxInt16 gl_ADPCMcoeff_delta[] = {
    230, 230, 230, 230, 307, 409, 512, 614, 768, 614, 512, 409, 307, 230, 230, 230
};

static wxInt16 gl_ADPCMcoeff_1[] = {
    256, 512, 0, 192, 240, 460, 392
};

static wxInt16 gl_ADPCMcoeff_2[] = {
    0, -256, 0, 64, 0, -208, -232
};

wxSoundStream& wxSoundStreamMSAdpcm::Write(const void *buffer, wxUint32 len)
{
    wxInt16 delta;
    wxUint8 ADPCMdata;
    wxUint16 *PCMdata;
    wxInt16 coeff1, coeff2;
    
#define GET_DATA_16 (*ADPCMdata++ | ((wxUint32)(*ADPCMdata++) << 8);
#define GET_DATA_8 (*ADPCMdata++)
                     
    if (!m_got_header) {
        i_predict = GET_DATA_8;
        delta     = GET_DATA_16;
        samp1     = GET_DATA_16;
        PCMdata   = GET_DATA_16;
        len -= 3*2 + 1;
        m_got_header = TRUE;

        coeff1 = gl_ADPCMcoeff_1[i_predict];
        coeff2 = gl_ADPCMcoeff_2[i_predict];
    }
    
    while (len > 0) {
        nyb1 = GET_DATA_8;
        nyb0 = (nyb1 & 0xf0) >> 4;
        nyb1 &= 0x0f;
        
        
    return *this;
}

wxUint32 wxSoundStreamMSAdpcm::GetBestSize() const
{
    return m_sndio->GetBestSize() / 2;
}

bool wxSoundStreamMSAdpcm::SetSoundFormat(const wxSoundFormatBase& format)
{
    if (format.GetType() != wxSOUND_ULAW) {
        m_snderror = wxSOUND_INVFRMT;
        return FALSE;
    }
    
    wxSoundFormatPcm pcm;
    wxSoundFormatUlaw *ulaw;
    
    wxSoundStreamCodec::SetSoundFormat(format);
    
    ulaw = (wxSoundFormatMSAdpcm *)m_sndformat;
    
    pcm.SetSampleRate(adpcm->GetSampleRate());
    pcm.SetBPS(16);
    pcm.SetChannels(adpcm->GetChannels());
    pcm.Signed(TRUE);
    pcm.SetOrder(wxBYTE_ORDER);
    
    m_router->SetSoundFormat(pcm);
    
    return TRUE;
}

