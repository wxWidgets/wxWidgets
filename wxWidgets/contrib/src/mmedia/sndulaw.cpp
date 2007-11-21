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
#endif

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mmedia/sndbase.h"
#include "wx/mmedia/sndfile.h"
#include "wx/mmedia/sndpcm.h"
#include "wx/mmedia/sndulaw.h"
#include "wx/mmedia/internal/g72x.h"

// --------------------------------------------------------------------------
// wxSoundFormatUlaw
// --------------------------------------------------------------------------

wxSoundFormatUlaw::wxSoundFormatUlaw()
        : m_srate(22050), m_channels(1)
{
}

wxSoundFormatUlaw::~wxSoundFormatUlaw()
{
}

void wxSoundFormatUlaw::SetSampleRate(wxUint32 srate)
{
    m_srate = srate;
}

wxUint32 wxSoundFormatUlaw::GetSampleRate() const
{
    return m_srate;
}

wxUint8 wxSoundFormatUlaw::GetChannels() const
{
    return m_channels;
}

void wxSoundFormatUlaw::SetChannels(wxUint8 nchannels)
{
    m_channels = nchannels;
}

wxSoundFormatBase *wxSoundFormatUlaw::Clone() const
{
    wxSoundFormatUlaw *ulaw = new wxSoundFormatUlaw();
    
    ulaw->m_srate = m_srate;
    ulaw->m_channels = m_channels;
    return ulaw;
}

wxUint32 wxSoundFormatUlaw::GetTimeFromBytes(wxUint32 bytes) const
{
    return (bytes / m_srate);
}

wxUint32 wxSoundFormatUlaw::GetBytesFromTime(wxUint32 time) const
{
    return time * m_srate;
}

bool wxSoundFormatUlaw::operator !=(const wxSoundFormatBase& frmt2) const
{
    wxSoundFormatUlaw *ulaw = (wxSoundFormatUlaw *)&frmt2;
    
    if (frmt2.GetType() != wxSOUND_ULAW)
        return true;
    
    return (ulaw->m_srate != m_srate);
}

// --------------------------------------------------------------------------
// wxSoundStreamUlaw
// --------------------------------------------------------------------------
wxSoundStreamUlaw::wxSoundStreamUlaw(wxSoundStream& sndio)
 : wxSoundStreamCodec(sndio)
{
  // PCM converter
  m_router = new wxSoundRouterStream(sndio);
}

wxSoundStreamUlaw::~wxSoundStreamUlaw()
{
  delete m_router;
}

wxSoundStream& wxSoundStreamUlaw::Read(void *buffer, wxUint32 len)
{
    wxUint16 *old_linear;
    register wxUint16 *linear_buffer;
    register const wxUint8 *ulaw_buffer;
    register wxUint32 countdown;

    old_linear = linear_buffer = new wxUint16[len*2];
    ulaw_buffer = (const wxUint8 *)buffer;
    
    m_router->Read(linear_buffer, len * 2);
    
    m_lastcount = countdown = m_router->GetLastAccess() / 2;
    m_snderror  = m_router->GetError();
    if (m_snderror != wxSOUND_NOERROR)
        return *this;
    
    while (countdown > 0) {
        *linear_buffer++ = ulaw2linear(*ulaw_buffer++);
        countdown--;
    }
    
    delete[] old_linear;
    
    return *m_router;    
}

wxSoundStream& wxSoundStreamUlaw::Write(const void *buffer, wxUint32 len)
{
    wxUint16 *old_linear;
    register wxUint16 *linear_buffer;
    register const wxUint8 *ulaw_buffer;
    register wxUint32 countdown = len;
    
    old_linear = linear_buffer = new wxUint16[len*2];
    ulaw_buffer = (const wxUint8 *)buffer;
    
    while (countdown > 0) {
        *linear_buffer++ = ulaw2linear(*ulaw_buffer++);
        countdown--;
    }
    
    m_router->Write(old_linear, len * 2);
    
    delete[] old_linear;
    
    return *m_router;
}

wxUint32 wxSoundStreamUlaw::GetBestSize() const
{
    return m_sndio->GetBestSize() / 2;
}

bool wxSoundStreamUlaw::SetSoundFormat(const wxSoundFormatBase& format)
{
    if (format.GetType() != wxSOUND_ULAW) {
        m_snderror = wxSOUND_INVFRMT;
        return false;
    }
    
    // As the codec only support 16 bits, Mono we must use a wxSoundRouter
    // to filter the data and to translate them to a format supported
    // by the sound card.
    
    wxSoundFormatPcm pcm;
    wxSoundFormatUlaw *ulaw;
    
    wxSoundStreamCodec::SetSoundFormat(format);
    
    ulaw = (wxSoundFormatUlaw *)m_sndformat;
    
    pcm.SetSampleRate(ulaw->GetSampleRate());
    pcm.SetBPS(16);
    pcm.SetChannels(ulaw->GetChannels());
    pcm.Signed(true);
    pcm.SetOrder(wxBYTE_ORDER);
    
    m_router->SetSoundFormat(pcm);
    
    return true;
}
