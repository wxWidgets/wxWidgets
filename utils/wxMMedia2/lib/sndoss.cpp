// --------------------------------------------------------------------------
// Name: sndoss.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "sndoss.cpp"
#endif

#include <sys/soundcard.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <wx/defs.h>
#include <wx/string.h>
#include "sndbase.h"
#include "sndoss.h"
#include "sndpcm.h"
#ifdef __WXGTK__
#include <gdk/gdk.h>
#endif

wxSoundStreamOSS::wxSoundStreamOSS(const wxString& dev_name)
{
  wxSoundFormatPcm pcm_default;

  m_fd = open(dev_name.mb_str(), O_RDWR);

  if (m_fd == -1) {
    m_snderror = wxSOUND_INVDEV;
    return;
  }

  m_devname = dev_name;
 
  wxSoundStreamOSS::SetSoundFormat(pcm_default);

  ioctl(m_fd, SNDCTL_DSP_GETBLKSIZE, &m_bufsize);

  m_snderror = wxSOUND_NOERR;

  close(m_fd);

  m_oss_stop = TRUE;
  m_q_filled = TRUE;
}

wxSoundStreamOSS::~wxSoundStreamOSS()
{
  if (m_fd > 0)
    close(m_fd);
}

wxUint32 wxSoundStreamOSS::GetBestSize() const
{
  return m_bufsize;
}

wxSoundStream& wxSoundStreamOSS::Read(void *buffer, size_t len)
{
  int ret;

  m_lastcount = (size_t)ret = read(m_fd, buffer, len);
  m_q_filled  = TRUE;

  if (ret < 0)
    m_snderror = wxSOUND_IOERR;
  else
    m_snderror = wxSOUND_NOERR;

  return *this;
}

wxSoundStream& wxSoundStreamOSS::Write(const void *buffer, size_t len)
{
  int ret;

  m_lastcount = (size_t)ret = write(m_fd, buffer, len);
  m_q_filled  = TRUE;

  if (ret < 0)
    m_snderror = wxSOUND_IOERR;
  else
    m_snderror = wxSOUND_NOERR;

  return *this;
}

bool wxSoundStreamOSS::SetSoundFormat(const wxSoundFormatBase& format)
{
  int tmp;
  wxSoundFormatPcm *pcm_format;

  if (format.GetType() != wxSOUND_PCM) {
    m_snderror = wxSOUND_INVFRMT;
    return FALSE;
  }

  if (m_fd == -1) {
    m_snderror = wxSOUND_INVDEV;
    return FALSE;
  }

  if (m_sndformat)
    delete m_sndformat;

  m_sndformat = format.Clone();
  if (!m_sndformat) {
    m_snderror = wxSOUND_MEMERR;
    return FALSE;
  }
  pcm_format = (wxSoundFormatPcm *)m_sndformat;

  // Set the sample rate field.
  tmp = pcm_format->GetSampleRate();
  ioctl(m_fd, SNDCTL_DSP_SPEED, &tmp);

  pcm_format->SetSampleRate(tmp);

  // Detect the best format
  DetectBest(pcm_format);
  SetupFormat(pcm_format);

  tmp = pcm_format->GetChannels();
  ioctl(m_fd, SNDCTL_DSP_CHANNELS, &tmp);
  pcm_format->SetChannels(tmp);

  m_snderror = wxSOUND_NOERR;
  if (*pcm_format != format) {
    m_snderror = wxSOUND_NOTEXACT;
    return FALSE;
  }
  return TRUE;
}

bool wxSoundStreamOSS::SetupFormat(wxSoundFormatPcm *pcm_format)
{
  int tmp;

  switch(pcm_format->GetBPS()) {
  case 8:
    if (pcm_format->Signed())
      tmp = AFMT_S8;
    else
      tmp = AFMT_U8;
    break;
  case 16:
    switch (pcm_format->GetOrder()) {
    case wxBIG_ENDIAN:
      if (pcm_format->Signed())
        tmp = AFMT_S16_BE;
      else
        tmp = AFMT_U16_BE;
      break;
    case wxLITTLE_ENDIAN:
      if (pcm_format->Signed())
        tmp = AFMT_S16_LE;
      else
        tmp = AFMT_U16_LE;
      break;
    }
    break;
  }

  ioctl(m_fd, SNDCTL_DSP_SETFMT, &tmp);

  // Demangling.
  switch (tmp) {
  case AFMT_U8:
    pcm_format->SetBPS(8);
    pcm_format->Signed(FALSE);
    break;
  case AFMT_S8:
    pcm_format->SetBPS(8);
    pcm_format->Signed(TRUE);
    break;
  case AFMT_U16_LE:
    pcm_format->SetBPS(16);
    pcm_format->Signed(FALSE);
    pcm_format->SetOrder(wxLITTLE_ENDIAN);
    break;
  case AFMT_U16_BE:
    pcm_format->SetBPS(16);
    pcm_format->Signed(FALSE);
    pcm_format->SetOrder(wxBIG_ENDIAN);
    break;
  case AFMT_S16_LE:
    pcm_format->SetBPS(16);
    pcm_format->Signed(TRUE);
    pcm_format->SetOrder(wxLITTLE_ENDIAN);
    break;
  case AFMT_S16_BE:
    pcm_format->SetBPS(16);
    pcm_format->Signed(TRUE);
    pcm_format->SetOrder(wxBIG_ENDIAN);
    break;
  }
  return TRUE;
}

#ifdef __WXGTK__
static void _wxSound_OSS_CBack(gpointer data, int source,
                               GdkInputCondition condition)
{
  wxSoundStreamOSS *oss = (wxSoundStreamOSS *)data;

  switch (condition) {
  case GDK_INPUT_READ:
    oss->WakeUpEvt(wxSOUND_INPUT);
    break;
  case GDK_INPUT_WRITE:
    oss->WakeUpEvt(wxSOUND_OUTPUT);
    break;
  default:
    break;
  }
}
#endif

void wxSoundStreamOSS::WakeUpEvt(int evt)
{
  m_q_filled = FALSE;
  OnSoundEvent(evt);
}

bool wxSoundStreamOSS::StartProduction(int evt)
{
  wxSoundFormatBase *old_frmt;

  if (!m_oss_stop)
    StopProduction();

  old_frmt = m_sndformat->Clone();

  if (evt == wxSOUND_OUTPUT)
    m_fd = open(m_devname.mb_str(), O_WRONLY);
  else if (evt == wxSOUND_INPUT)
    m_fd = open(m_devname.mb_str(), O_RDONLY);

  if (m_fd == -1) {
    m_snderror = wxSOUND_INVDEV;
    return FALSE;
  }

  SetSoundFormat(*old_frmt);
  delete old_frmt;

#ifdef __WXGTK__
  int trig;

  if (evt == wxSOUND_OUTPUT) {
    m_tag = gdk_input_add(m_fd, GDK_INPUT_WRITE, _wxSound_OSS_CBack, (gpointer)this);
    trig = PCM_ENABLE_OUTPUT;
  } else {
    m_tag = gdk_input_add(m_fd, GDK_INPUT_READ, _wxSound_OSS_CBack, (gpointer)this);
    trig = PCM_ENABLE_INPUT;
  }
#else
  while (!m_oss_stop)
    OnSoundEvent(evt);
#endif

  ioctl(m_fd, SNDCTL_DSP_SETTRIGGER, &trig);

  m_oss_stop = FALSE;
  m_q_filled = FALSE;

  return TRUE;
}

bool wxSoundStreamOSS::StopProduction()
{
  if (m_oss_stop)
    return FALSE;

#ifdef __WXGTK__
  gdk_input_remove(m_tag);
#endif

  close(m_fd);
  m_oss_stop = TRUE;
  m_q_filled = TRUE;
  return TRUE;
}

bool wxSoundStreamOSS::QueueFilled() const
{
  return m_q_filled;
}

//
// Detect the closest format (The best).
//
void wxSoundStreamOSS::DetectBest(wxSoundFormatPcm *pcm)
{
#define MASK_16BITS (AFMT_S16_LE | AFMT_S16_BE | AFMT_U16_LE | AFMT_U16_BE)

  int fmt_mask;
  wxSoundFormatPcm best_pcm;

  // We change neither the number of channels nor the sample rate

  best_pcm.SetSampleRate(pcm->GetSampleRate());
  best_pcm.SetChannels(pcm->GetChannels());

  // Get the supported format by the sound card
  ioctl(m_fd, SNDCTL_DSP_GETFMTS, &fmt_mask);

  // It supports 16 bits
  if (pcm->GetBPS() == 16 && ((fmt_mask & MASK_16BITS) != 0))
    best_pcm.SetBPS(16);

  // It supports big endianness
  if (pcm->GetOrder() == wxBIG_ENDIAN && ((fmt_mask & (AFMT_S16_BE | AFMT_U16_BE)) != 0))
    best_pcm.SetOrder(wxBIG_ENDIAN);

  // It supports little endianness
  if (pcm->GetOrder() == wxLITTLE_ENDIAN && ((fmt_mask & (AFMT_S16_LE | AFMT_U16_LE)) != 0))
    best_pcm.SetOrder(wxLITTLE_ENDIAN);

  // It supports signed samples
  if (pcm->Signed() && ((fmt_mask & (AFMT_S16_LE | AFMT_S16_BE | AFMT_S8)) != 0))
    best_pcm.Signed(TRUE);

  // It supports unsigned samples
  if (!pcm->Signed() && ((fmt_mask & (AFMT_U16_LE | AFMT_U16_BE | AFMT_U8)) != 0))
    best_pcm.Signed(FALSE);

  // Finally recopy the new format
  *pcm = best_pcm;
}
