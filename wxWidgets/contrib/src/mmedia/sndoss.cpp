// --------------------------------------------------------------------------
// Name: sndoss.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999, 2000
// CVSID: $Id$
// wxWindows licence
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// System dependent headers
// --------------------------------------------------------------------------

#include <sys/soundcard.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef __WXGTK__
#include <gdk/gdk.h>
#endif

// --------------------------------------------------------------------------
// wxWidgets headers
// --------------------------------------------------------------------------
#include "wx/defs.h"
#include "wx/string.h"
#include "wx/mmedia/sndbase.h"
#include "wx/mmedia/sndoss.h"
#include "wx/mmedia/sndpcm.h"

wxSoundStreamOSS::wxSoundStreamOSS(const wxString& dev_name)
{
    wxSoundFormatPcm pcm_default;

    // Open the OSS device
    m_fd = open(dev_name.mb_str(), O_WRONLY);
    if (m_fd == -1) {
        // OSS not found
        m_oss_ok   = false;
        m_snderror = wxSOUND_INVDEV;
        return;
    }

    // Remember the device name
    m_devname = dev_name;

    // Initialize the default format
    wxSoundStreamOSS::SetSoundFormat(pcm_default);

    // Get the default best size for OSS
    ioctl(m_fd, SNDCTL_DSP_GETBLKSIZE, &m_bufsize);

    m_snderror = wxSOUND_NOERROR;

    // Close OSS
    close(m_fd);

    m_oss_ok   = true;
    m_oss_stop = true;
    m_q_filled = true;
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

wxSoundStream& wxSoundStreamOSS::Read(void *buffer, wxUint32 len)
{
    int ret;

    if (m_oss_stop) {
        m_snderror = wxSOUND_NOTSTARTED;
        m_lastcount = 0;
        return *this;
    }

    ret = read(m_fd, buffer, len);
    m_lastcount = (wxUint32)ret;
    m_q_filled  = true;

    if (ret < 0)
        m_snderror = wxSOUND_IOERROR;
    else
        m_snderror = wxSOUND_NOERROR;

    return *this;
}

wxSoundStream& wxSoundStreamOSS::Write(const void *buffer, wxUint32 len)
{
    int ret;

    if (m_oss_stop) {
        m_snderror = wxSOUND_NOTSTARTED;
        m_lastcount= 0;
        return *this;
    }

    ret = write(m_fd, buffer, len);
    m_q_filled = true;

    if (ret < 0) {
        m_lastcount = 0;
        m_snderror  = wxSOUND_IOERROR;
    } else {
        m_snderror = wxSOUND_NOERROR;
        m_lastcount = (wxUint32)ret;
    }

    return *this;
}

bool wxSoundStreamOSS::SetSoundFormat(const wxSoundFormatBase& format)
{
    int tmp;
    wxSoundFormatPcm *pcm_format;

    if (format.GetType() != wxSOUND_PCM) {
        m_snderror = wxSOUND_INVFRMT;
        return false;
    }

    if (!m_oss_ok) {
        m_snderror = wxSOUND_INVDEV;
        return false;
    }

    if (m_sndformat)
        delete m_sndformat;

    m_sndformat = format.Clone();
    if (!m_sndformat) {
        m_snderror = wxSOUND_MEMERROR;
        return false;
    }
    pcm_format = (wxSoundFormatPcm *)m_sndformat;

    // We temporary open the OSS device
    if (m_oss_stop) {
        m_fd = open(m_devname.mb_str(), O_WRONLY);
        if (m_fd == -1) {
            m_snderror = wxSOUND_INVDEV;
            return false;
        }
    }

    // Set the sample rate field.
    tmp = pcm_format->GetSampleRate();
    ioctl(m_fd, SNDCTL_DSP_SPEED, &tmp);

    pcm_format->SetSampleRate(tmp);

    // Detect the best format
    DetectBest(pcm_format);
    // Try to apply it
    SetupFormat(pcm_format);

    tmp = pcm_format->GetChannels();
    ioctl(m_fd, SNDCTL_DSP_CHANNELS, &tmp);
    pcm_format->SetChannels(tmp);

    // Close the OSS device
    if (m_oss_stop)
        close(m_fd);

    m_snderror = wxSOUND_NOERROR;
    if (*pcm_format != format) {
        m_snderror = wxSOUND_NOEXACT;
        return false;
    }

    return true;
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
            pcm_format->Signed(false);
            break;
        case AFMT_S8:
            pcm_format->SetBPS(8);
            pcm_format->Signed(true);
            break;
        case AFMT_U16_LE:
            pcm_format->SetBPS(16);
            pcm_format->Signed(false);
            pcm_format->SetOrder(wxLITTLE_ENDIAN);
            break;
        case AFMT_U16_BE:
            pcm_format->SetBPS(16);
            pcm_format->Signed(false);
            pcm_format->SetOrder(wxBIG_ENDIAN);
            break;
        case AFMT_S16_LE:
            pcm_format->SetBPS(16);
            pcm_format->Signed(true);
            pcm_format->SetOrder(wxLITTLE_ENDIAN);
            break;
        case AFMT_S16_BE:
            pcm_format->SetBPS(16);
            pcm_format->Signed(true);
            pcm_format->SetOrder(wxBIG_ENDIAN);
            break;
    }
    return true;
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
    m_q_filled = false;
    OnSoundEvent(evt);
}

bool wxSoundStreamOSS::StartProduction(int evt)
{
    wxSoundFormatBase *old_frmt;

    if (!m_oss_stop)
        StopProduction();

    old_frmt = m_sndformat->Clone();
    if (!old_frmt) {
        m_snderror = wxSOUND_MEMERROR;
        return false;
    }

    if (evt == wxSOUND_OUTPUT)
        m_fd = open(m_devname.mb_str(), O_WRONLY);
    else if (evt == wxSOUND_INPUT)
        m_fd = open(m_devname.mb_str(), O_RDONLY);

    if (m_fd == -1) {
        m_snderror = wxSOUND_INVDEV;
        return false;
    }

    SetSoundFormat(*old_frmt);
    delete old_frmt;

    int trig;

    if (evt == wxSOUND_OUTPUT) {
#ifdef __WXGTK__
        m_tag = gdk_input_add(m_fd, GDK_INPUT_WRITE, _wxSound_OSS_CBack, (gpointer)this);
#endif
        trig = PCM_ENABLE_OUTPUT;
    } else {
#ifdef __WXGTK__
        m_tag = gdk_input_add(m_fd, GDK_INPUT_READ, _wxSound_OSS_CBack, (gpointer)this);
#endif
        trig = PCM_ENABLE_INPUT;
    }

  ioctl(m_fd, SNDCTL_DSP_SETTRIGGER, &trig);

  m_oss_stop = false;
  m_q_filled = false;

  return true;
}

bool wxSoundStreamOSS::StopProduction()
{
  if (m_oss_stop)
    return false;

#ifdef __WXGTK__
  gdk_input_remove(m_tag);
#endif

  close(m_fd);
  m_oss_stop = true;
  m_q_filled = true;
  return true;
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
  if (pcm->GetOrder() == wxBIG_ENDIAN &&
      ((fmt_mask & (AFMT_S16_BE | AFMT_U16_BE)) != 0))
    best_pcm.SetOrder(wxBIG_ENDIAN);

  // It supports little endianness
  if (pcm->GetOrder() == wxLITTLE_ENDIAN &&
      ((fmt_mask & (AFMT_S16_LE | AFMT_U16_LE)) != 0))
    best_pcm.SetOrder(wxLITTLE_ENDIAN);

  // It supports signed samples
  if (pcm->Signed() &&
      ((fmt_mask & (AFMT_S16_LE | AFMT_S16_BE | AFMT_S8)) != 0))
    best_pcm.Signed(true);

  // It supports unsigned samples
  if (!pcm->Signed() &&
      ((fmt_mask & (AFMT_U16_LE | AFMT_U16_BE | AFMT_U8)) != 0))
    best_pcm.Signed(false);

  // Finally recopy the new format
  *pcm = best_pcm;
}
