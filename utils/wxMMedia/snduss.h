// /////////////////////////////////////////////////////////////////////////////
// Name:       snduss.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
/* Real -*- C++ -*- */
#ifndef __SND_ussnd_H__
#define __SND_ussnd_H__
#ifdef __GNUG__
#pragma interface
#endif

#include <wx/thread.h>
#include "sndsnd.h"
#include "sndfrag.h"
#include "sndfrmt.h"

///
class wxUssSound : public wxSound, public wxThread {
  ///
  DECLARE_DYNAMIC_CLASS(wxUssSound)
public:
  wxUssSound();
  ///
  virtual ~wxUssSound();
  
  ///
  void OnNeedBuffer();
protected:
  ///
  virtual bool Wakeup(wxSndBuffer& buf);
  ///
  void USS_Sleep();
  ///
  virtual void StopBuffer(wxSndBuffer& buf);

  ///
  bool OnSetupDriver(wxSndBuffer& buf, wxSndMode mode);

  ///
  bool SetupSound(wxUint16 srate, wxUint8 bps, bool stereo);

  ///
  wxUint32 GetNbFragments();
  ///
  wxUint32 GetFragmentSize();
  ///
  void ThreadEntryPoint();

protected:
  ///
  wxUint16 m_srate;
  ///
  wxUint8 m_bps;
  ///
  wxUint32 m_max_bufsize;
  ///
  bool m_stereo;
  ///
  wxSndMode m_mode;
  ///
  wxSoundDataFormat m_ussformat;
  ///
  wxStreamBuffer *m_sndbuf;
  ///
  bool m_stop_thrd, m_sleeping;
  /// Sound file descriptor.
  int m_fd;
  /// Thread sleep mutexes and conditions.
  wxMutex m_sleep_mtx;
  wxCondition m_sleep_cond;

  ///
  bool InitBuffer(wxSndBuffer *buf);
  bool DoInput(wxSndBuffer *buf);
  bool DoOutput(wxSndBuffer *buf);

  ///
  virtual void *Entry();
};

#endif
