// --------------------------------------------------------------------------
// Name: sndesd.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "sndesd.cpp"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wx/defs.h>
#include <wx/string.h>
#include <esd.h>
#include "sndbase.h"
#include "sndesd.h"
#include "sndpcm.h"
#ifdef __WXGTK__
#include <gdk/gdk.h>
#endif

#define MY_ESD_NAME "wxWindows/wxSoundStreamESD"

wxSoundStreamESD::wxSoundStreamESD(const wxString& hostname)
{
  wxSoundFormatPcm pcm_default;

  m_fd = esd_play_stream(ESD_PLAY | ESD_STREAM | ESD_MONO | ESD_BITS8, 22050, 
//                            hostname.mb_str(), MY_ESD_NAME);
                            NULL, MY_ESD_NAME);

  if (m_fd == -1) {
    m_snderror = wxSOUND_INVDEV;
    return;
  }

  esd_close(m_fd);
 
  m_hostname = hostname;

  SetSoundFormat(pcm_default);

  m_snderror = wxSOUND_NOERR;
  m_esd_stop = TRUE;
  m_q_filled = TRUE;
}

wxSoundStreamESD::~wxSoundStreamESD()
{
  if (m_fd > 0)
    esd_close(m_fd);
}

wxSoundStream& wxSoundStreamESD::Read(void *buffer, wxUint32 len)
{
  int ret;

  m_lastcount = (wxUint32)ret = read(m_fd, buffer, len);

  if (ret < 0)
    m_snderror = wxSOUND_IOERR;
  else
    m_snderror = wxSOUND_NOERR;

  return *this;
}

wxSoundStream& wxSoundStreamESD::Write(const void *buffer, wxUint32 len)
{
  int ret;

  m_lastcount = (wxUint32)ret = write(m_fd, buffer, len);

  if (ret < 0)
    m_snderror = wxSOUND_IOERR;
  else
    m_snderror = wxSOUND_NOERR;

  m_q_filled = TRUE;

  return *this;
}

bool wxSoundStreamESD::SetSoundFormat(const wxSoundFormatBase& format)
{
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

  // Detect the best format
  DetectBest(pcm_format);

  m_snderror = wxSOUND_NOERR;
  if (*pcm_format != format) {
    m_snderror = wxSOUND_NOTEXACT;
    return FALSE;
  }
  return TRUE;
}

#ifdef __WXGTK__
static void _wxSound_OSS_CBack(gpointer data, int source,
                               GdkInputCondition condition)
{
  wxSoundStreamESD *esd = (wxSoundStreamESD *)data;

  switch (condition) {
  case GDK_INPUT_READ:
    esd->WakeUpEvt(wxSOUND_INPUT);
    break;
  case GDK_INPUT_WRITE:
    esd->WakeUpEvt(wxSOUND_OUTPUT);
    break;
  default:
    break;
  }
}
#endif

void wxSoundStreamESD::WakeUpEvt(int evt)
{
  m_q_filled = FALSE;
  OnSoundEvent(evt);
}

bool wxSoundStreamESD::StartProduction(int evt)
{
  wxSoundFormatPcm *pcm;
  int flag = 0;

  if (!m_esd_stop)
    StopProduction();

  pcm = (wxSoundFormatPcm *)m_sndformat;

  flag |= (pcm->GetBPS() == 16) ? ESD_BITS16 : ESD_BITS8;
  flag |= (pcm->GetChannels() == 2) ? ESD_STEREO : ESD_MONO;

  if (evt == wxSOUND_OUTPUT) {
    flag |= ESD_PLAY | ESD_STREAM;
    m_fd = esd_play_stream(flag, pcm->GetSampleRate(), NULL,
			   MY_ESD_NAME);
  } else {
    flag |= ESD_RECORD | ESD_STREAM;
    m_fd = esd_record_stream(flag, pcm->GetSampleRate(), NULL,
                             MY_ESD_NAME);
  }

#ifdef __WXGTK__
  if (evt == wxSOUND_OUTPUT)
    m_tag = gdk_input_add(m_fd, GDK_INPUT_WRITE, _wxSound_OSS_CBack, (gpointer)this);
  else
    m_tag = gdk_input_add(m_fd, GDK_INPUT_READ, _wxSound_OSS_CBack, (gpointer)this);
#endif

  m_esd_stop = FALSE;
  m_q_filled = FALSE;

  return TRUE;
}

bool wxSoundStreamESD::StopProduction()
{
  if (m_esd_stop)
    return FALSE;

  gdk_input_remove(m_tag);
  esd_close(m_fd);
  m_esd_stop = TRUE;
  m_q_filled = TRUE;
  return TRUE;
}

//
// Detect the closest format (The best).
//
void wxSoundStreamESD::DetectBest(wxSoundFormatPcm *pcm)
{
  wxSoundFormatPcm best_pcm;

  // We change neither the number of channels nor the sample rate

  best_pcm.SetSampleRate(pcm->GetSampleRate());
  best_pcm.SetChannels(pcm->GetChannels());

  // It supports 16 bits
  if (pcm->GetBPS() == 16)
    best_pcm.SetBPS(16);

  best_pcm.SetOrder(wxLITTLE_ENDIAN);
  best_pcm.Signed(TRUE);

  // Finally recopy the new format
  *pcm = best_pcm;
}
