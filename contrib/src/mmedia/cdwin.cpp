////////////////////////////////////////////////////////////////////////////////
// Name:       cdwin.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/defs.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifdef __WINDOWS__

// ---------------------------------------------------------------------------
// System headers
// ---------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>

// ---------------------------------------------------------------------------
// MMedia headers
// ---------------------------------------------------------------------------

#define WXMMEDIA_INTERNAL
#include "wx/mmedia/cdbase.h"
#include "wx/mmedia/cdwin.h"

// ---------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCDAudioWin, wxCDAudio)
    
wxCDAudioWin::wxCDAudioWin(void)
  : wxCDAudio(), m_trksize(NULL), m_trkpos(NULL), m_toc(NULL), m_ok(true)
{
  MCI_OPEN_PARMS open_struct;
  MCI_SET_PARMS set_struct;

  m_internal = new CDAW_Internal;
  open_struct.lpstrDeviceType = _T("cdaudio");
  DWORD ret = mciSendCommand((MCIDEVICEID)NULL, MCI_OPEN, MCI_OPEN_TYPE,
                       (DWORD)&open_struct);
  if (ret) {
    m_ok = false;
    return;
  }
  m_internal->dev_id = open_struct.wDeviceID;

  set_struct.dwTimeFormat = MCI_FORMAT_MSF;
  mciSendCommand(m_internal->dev_id, MCI_SET, MCI_SET_TIME_FORMAT,
                       (DWORD)(LPVOID)&set_struct);

  PrepareToc();

  set_struct.dwTimeFormat = MCI_FORMAT_TMSF;
  mciSendCommand(m_internal->dev_id, MCI_SET, MCI_SET_TIME_FORMAT,
                       (DWORD)(LPVOID)&set_struct);
}

wxCDAudioWin::~wxCDAudioWin(void)
{
  if (m_ok) {
    mciSendCommand(m_internal->dev_id, MCI_CLOSE, 0,(DWORD_PTR)NULL);
    delete m_toc;
    delete[] m_trksize;
    delete[] m_trkpos;
  }
  delete m_internal;
}

void wxCDAudioWin::PrepareToc(void)
{
  MCI_STATUS_PARMS status_struct;
  wxUint16 i, nb_m_trksize;
  wxCDtime total_time, *trk;
  DWORD tmem;

  if (!m_ok)
    return;

  status_struct.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
  mciSendCommand(m_internal->dev_id, MCI_STATUS, MCI_STATUS_ITEM,
                 (DWORD)&status_struct);
  nb_m_trksize = status_struct.dwReturn;

  m_trksize = new wxCDtime[nb_m_trksize+1];
  m_trkpos = new wxCDtime[nb_m_trksize+1];

  status_struct.dwItem = MCI_STATUS_LENGTH;
  mciSendCommand(m_internal->dev_id, MCI_STATUS, MCI_STATUS_ITEM,
                 (DWORD)&status_struct);
  total_time.track = nb_m_trksize;
  tmem = status_struct.dwReturn;
  total_time.min = MCI_MSF_MINUTE(tmem);
  total_time.sec = MCI_MSF_SECOND(tmem);
  total_time.hour = total_time.min / 60;
  total_time.min %= 60;

  for (i=1;i<=nb_m_trksize;i++) {
    status_struct.dwItem = MCI_STATUS_POSITION;
    status_struct.dwTrack = i;
    mciSendCommand(m_internal->dev_id, MCI_STATUS,
                   MCI_STATUS_ITEM | MCI_TRACK,
                   (DWORD)(LPVOID)&status_struct);
    tmem = status_struct.dwReturn;

    trk        = &m_trkpos[i];
    trk->track = i;
    trk->min   = MCI_MSF_MINUTE(tmem);
    trk->sec   = MCI_MSF_SECOND(tmem);
    trk->hour  = trk->min / 60;
    trk->min  %= 60;

    status_struct.dwItem = MCI_STATUS_LENGTH;
    status_struct.dwTrack = i;
    mciSendCommand(m_internal->dev_id, MCI_STATUS,
                   MCI_STATUS_ITEM | MCI_TRACK,
                   (DWORD)(LPVOID)&status_struct);
    tmem = status_struct.dwReturn;

    trk        = &m_trksize[i];
    trk->track = i;
    trk->min   = MCI_MSF_MINUTE(tmem);
    trk->sec   = MCI_MSF_SECOND(tmem);
    trk->hour  = trk->min / 60;
    trk->min  %= 60;
  }

  m_toc = new CDtoc(total_time, m_trksize, m_trkpos);
}

bool wxCDAudioWin::Play(const wxCDtime& beg_time, const wxCDtime& end_time)
{
  DWORD tmsf;
  MCI_PLAY_PARMS play_struct;

  if (!m_ok)
    return false;

  tmsf = MCI_MAKE_TMSF(beg_time.track, beg_time.min,
                       beg_time.sec, 0);
  play_struct.dwFrom = tmsf;
  tmsf = MCI_MAKE_TMSF(end_time.track, end_time.min,
                       end_time.sec, 0);
  play_struct.dwTo = tmsf;

  mciSendCommand(m_internal->dev_id, MCI_PLAY, 0, (DWORD)&play_struct);
  return true;
}

bool wxCDAudioWin::Pause(void)
{
  if (!m_ok)
    return false;

  return (mciSendCommand(m_internal->dev_id, MCI_PAUSE, 0, 0) == 0);
}

bool wxCDAudioWin::Resume(void)
{
  if (!m_ok)
    return false;

  return (mciSendCommand(m_internal->dev_id, MCI_RESUME, 0, 0) == 0);
}

wxCDAudio::CDstatus wxCDAudioWin::GetStatus(void)
{
  MCI_STATUS_PARMS status_struct;

  if (!m_ok)
    return STOPPED;

  status_struct.dwItem = MCI_STATUS_MODE;
  mciSendCommand(m_internal->dev_id, MCI_STATUS, MCI_STATUS_ITEM,
                 (DWORD)&status_struct);
  switch (status_struct.dwReturn) {
  case MCI_MODE_PAUSE:
    return PAUSED;
  case MCI_MODE_PLAY:
    return PLAYING;
  }
  return STOPPED;
}

wxCDtime wxCDAudioWin::GetTime(void)
{
  MCI_STATUS_PARMS status_struct;
  wxCDtime cd_time = {0,0,0,0};

  if (!m_ok)
    return cd_time;

  status_struct.dwItem = MCI_STATUS_TIME_FORMAT;
  mciSendCommand(m_internal->dev_id, MCI_STATUS, MCI_STATUS_ITEM,
                 (DWORD)&status_struct);
  cd_time.track = MCI_TMSF_TRACK(status_struct.dwReturn);
  cd_time.min = MCI_TMSF_MINUTE(status_struct.dwReturn);
  cd_time.sec = MCI_TMSF_SECOND(status_struct.dwReturn);
  cd_time.hour = cd_time.min / 60;
  cd_time.min %= 60;
  return cd_time;
}

const wxCDAudio::CDtoc& wxCDAudioWin::GetToc(void)
{
  return *m_toc;
}

#endif
   // __WINDOWS__
