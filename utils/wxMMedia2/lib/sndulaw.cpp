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

#include "sndbase.h"
#include "sndfile.h"
#include "sndpcm.h"
#include "sndulaw.h"
#include "g72x.h"

// --------------------------------------------------------------------------
// wxSoundFormatUlaw
// --------------------------------------------------------------------------

wxSoundFormatUlaw::wxSoundFormatUlaw()
  : m_srate(22050)
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

wxSoundFormatBase *wxSoundFormatUlaw::Clone() const
{
  wxSoundFormatUlaw *ulaw = new wxSoundFormatUlaw();

  ulaw->m_srate = m_srate;
  return ulaw;
}

wxUint32 wxSoundFormatUlaw::GetTimeFromByte(wxUint32 bytes) const
{
  return (bytes / m_srate);
}

wxUint32 wxSoundFormatUlaw::GetByteFromTime(wxUint32 time) const
{
  return time * m_srate;
}

bool wxSoundFormatUlaw::operator !=(const wxSoundFormatBase& frmt2) const
{
  wxSoundFormatUlaw *ulaw = (wxSoundFormatUlaw *)&frmt2;

  if (frmt2.GetType() != wxSOUND_ULAW)
    return TRUE;

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

wxSoundStream& wxSoundStreamUlaw::Read(void *buffer, size_t len)
{
  return *this;
}

wxSoundStream& wxSoundStreamUlaw::Write(const void *buffer, size_t len)
{
  wxUint16 *old_linear;
  register wxUint16 *linear_buffer;
  register const wxUint8 *ulaw_buffer;
  register size_t countdown = len;

  old_linear = linear_buffer = new wxUint16[len*2];
  ulaw_buffer = (const wxUint8 *)buffer;

  while (countdown != 0) {
    *linear_buffer++ = ulaw2linear(*ulaw_buffer++);
    countdown--;
  }

  m_router->Write(old_linear, len * 2);

  delete[] old_linear;

  return *m_router;
}

bool wxSoundStreamUlaw::SetSoundFormat(const wxSoundFormatBase& format)
{
  if (format.GetType() != wxSOUND_ULAW) {
    m_snderror = wxSOUND_INVFRMT;
    return FALSE;
  }

  wxSoundFormatPcm pcm;
  wxSoundFormatUlaw *ulaw;

  wxSoundStreamCodec::SetSoundFormat(format);

  ulaw = (wxSoundFormatUlaw *)m_sndformat;

  pcm.SetSampleRate(ulaw->GetSampleRate());
  pcm.SetBPS(16);
  pcm.SetChannels(1);
  pcm.Signed(TRUE);
  pcm.SetOrder(wxBYTE_ORDER);

  m_router->SetSoundFormat(pcm);

  return TRUE;
}
