////////////////////////////////////////////////////////////////////////////////
// Name:       cdlinux.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "cdunix.h"
#endif

#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef __linux__
#include <linux/cdrom.h>
#else
// For Solaris
#include <sys/cdio.h>
#endif

#ifdef WX_PRECOMP
#include "wx/wxprec.h"
#else
#include "wx/wx.h"
#endif
#include "mmtype.h"
#include "cdunix.h"

wxCDAudioLinux::wxCDAudioLinux()
  : wxCDAudio(), m_fd(-1)
{
  OpenDevice("/dev/cdrom");
}

wxCDAudioLinux::wxCDAudioLinux(const char *dev_name)
  : wxCDAudio(), m_fd(-1)
{
  OpenDevice(dev_name);
}

wxCDAudioLinux::~wxCDAudioLinux()
{
  if (m_fd != -1) {
    close(m_fd);
    wxDELETE(m_trksize);
    wxDELETE(m_trkpos);
  }
}

void wxCDAudioLinux::OpenDevice(const char *dev_name)
{
  struct cdrom_tocentry entry, old_entry;
  struct cdrom_tochdr diskinf;
  struct cdrom_msf0 *msf = &entry.cdte_addr.msf,
  		    *old_msf = &old_entry.cdte_addr.msf;
  wxCDtime *the_track;
  wxCDtime tot_tm;
  wxUint8 nb_tracks, i;
  int hour, minute, second;

  if (m_fd != -1)
    return;

  m_fd = open(dev_name, O_RDONLY);
  if (m_fd == -1) {
    m_toc = NULL;
    return;
  }
  m_status = STOPPED;  

  ioctl(m_fd, CDROMREADTOCHDR, &diskinf);

  nb_tracks = diskinf.cdth_trk1-diskinf.cdth_trk0+1;
  m_trksize = new wxCDtime[nb_tracks+1];
  m_trkpos  = new wxCDtime[nb_tracks+1];

  old_msf->minute = 0;
  old_msf->second = 0;
  for (i=diskinf.cdth_trk0;i<=diskinf.cdth_trk1;i++) {
    entry.cdte_track = i;
    entry.cdte_format = CDROM_MSF;
    ioctl(m_fd, CDROMREADTOCENTRY, &entry);

    minute = msf->minute - old_msf->minute;
    second = msf->second - old_msf->second;
    if (second < 0) {
      minute--;
      second += 60;
    }

    hour = minute / 60;
    minute %= 60;

    the_track = &m_trksize[i-diskinf.cdth_trk0];
    the_track->track = i-diskinf.cdth_trk0;
    the_track->hour = hour;
    the_track->min = minute;
    the_track->sec = second;
    
    the_track = &m_trkpos[i-diskinf.cdth_trk0];
    the_track->track = i-diskinf.cdth_trk0;
    the_track->hour = old_msf->minute / 60;
    the_track->min = old_msf->minute % 60;
    the_track->sec = old_msf->second;
    old_entry = entry;
  }

  entry.cdte_track = CDROM_LEADOUT;
  entry.cdte_format = CDROM_MSF;
  ioctl(m_fd, CDROMREADTOCENTRY, &entry);

  tot_tm.track = nb_tracks;
  tot_tm.hour = msf->minute / 60;
  tot_tm.min = msf->minute % 60;
  tot_tm.sec = msf->second % 60;

  m_trksize[nb_tracks].track = nb_tracks;
  minute = msf->minute - old_msf->minute;
  second = msf->second - old_msf->second;
  if (second < 0) {
    minute--;
    second += 60;
  }
  hour = minute / 60;
  minute %= 60;

  m_trksize[nb_tracks].hour = hour;
  m_trksize[nb_tracks].min  = minute;
  m_trksize[nb_tracks].sec  = second;
  m_trkpos[nb_tracks].track = nb_tracks;
  m_trkpos[nb_tracks].hour  = old_msf->minute / 60;
  m_trkpos[nb_tracks].min   = old_msf->minute % 60;
  m_trkpos[nb_tracks].sec   = old_msf->second;

  m_toc = new CDtoc(tot_tm, m_trksize, m_trkpos);
}

bool wxCDAudioLinux::Play(const wxCDtime& beg_time, const wxCDtime& end_time)
{
  struct cdrom_msf track_msf;

  track_msf.cdmsf_min0 = beg_time.hour * 60 + beg_time.min;
  track_msf.cdmsf_sec0 = beg_time.sec;
  track_msf.cdmsf_frame0 = 0;
  track_msf.cdmsf_min1 = end_time.hour * 60 + end_time.min;
  track_msf.cdmsf_sec1 = end_time.sec;
  track_msf.cdmsf_frame1 = 0;
  return (ioctl(m_fd, CDROMPLAYMSF, &track_msf) != -1);
}

bool wxCDAudioLinux::Pause()
{
  return (ioctl(m_fd, CDROMPAUSE, 0) != -1);
}

bool wxCDAudioLinux::Resume()
{
  return (ioctl(m_fd, CDROMRESUME, 0) != -1);
}

wxCDAudio::CDstatus wxCDAudioLinux::GetStatus()
{
  struct cdrom_subchnl subchnl;
  ioctl(m_fd, CDROMSUBCHNL, &subchnl);
  switch (subchnl.cdsc_audiostatus) {
  case CDROM_AUDIO_PLAY: return PLAYING;
  case CDROM_AUDIO_PAUSED: return PAUSED;
  case CDROM_AUDIO_COMPLETED: return STOPPED;
  }

  return STOPPED;
}

wxCDtime wxCDAudioLinux::GetTime()
{
  wxCDtime cdtime;
  struct cdrom_subchnl subchnl;

  ioctl(m_fd, CDROMSUBCHNL, &subchnl);
  cdtime.track = subchnl.cdsc_trk;
  cdtime.min = subchnl.cdsc_reladdr.msf.minute;
  cdtime.hour = cdtime.min / 60;
  cdtime.min %= 60;
  cdtime.sec = subchnl.cdsc_reladdr.msf.second;

  return cdtime;
}

wxCDAudio::CDtoc& wxCDAudioLinux::GetToc()
{
  return *m_toc;
}
