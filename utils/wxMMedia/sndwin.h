// /////////////////////////////////////////////////////////////////////////////
// Name:       sndwin.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
/* Real -*- C++ -*- */
#ifndef __SND_win_H__
#define __SND_win_H__

#include "sndsnd.h"
#include "sndfrag.h"

#ifdef WXMMEDIA_INTERNAL
#include <wx/msw/private.h>
#include <windows.h>
#include <mmsystem.h>

typedef struct wxWinSoundInternal {
  HWAVEOUT devout_id;
  HWAVEIN devin_id;
  HWND sndWin;
} wxWinSoundInternal;

typedef struct wxSndWinInfo {
  HGLOBAL h_data, h_hdr;

  char *data;
  WAVEHDR *hdr;
} wxSndWinInfo;

#endif

/** Sound buffer fragmenter: windows specific implementation
  * @author Guilhem Lavaux
  */
class wxSndWinFragment : public wxFragmentBuffer {
public:
  wxSndWinFragment(wxSound& io_drv);
  virtual ~wxSndWinFragment(void);

  virtual void AllocIOBuffer(void);
  virtual void FreeIOBuffer(void);

  virtual bool OnBufferFilled(wxFragBufPtr *ptr, wxSndMode mode);

  void WaitForAll();
};

///
class wxWinSound : public wxSound {
  ///
  DECLARE_DYNAMIC_CLASS(wxWinSound)
protected:
  struct wxWinSoundInternal *internal;

  ///
  bool wout_opened, win_opened;
  ///
  wxUint32 curr_o_srate, curr_i_srate;
  ///
  wxUint8 curr_o_bps, curr_i_bps;
  ///
  bool curr_o_stereo, curr_i_stereo;
  ///
  wxSndMode curr_mode;

  ///
  wxSndWinFragment fragments;

#ifdef WXMMEDIA_INTERNAL
  ///
  friend LRESULT APIENTRY _EXPORT wxSoundHandlerWndProc(HWND win,
                                             UINT message,
                                             WPARAM wParam, LPARAM lParam);

#endif

public:
  ///
  wxWinSound(void);
  ///
  virtual ~wxWinSound(void);

  ///
  void OnNeedBuffer(wxSndMode mode);
  ///
  void StopBuffer(wxSndBuffer& buf);
protected:
  ///
  virtual bool Wakeup(wxSndBuffer& buf);

  ///
  bool Reopen(wxSndBuffer& buf, bool force);

  ///
  friend class wxSndWinFragment;

  ///
  void PrepareHeader(wxFragmentBuffer::wxFragBufPtr& frag, wxSndMode mode);
  ///
  void UnprepareHeader(wxFragmentBuffer::wxFragBufPtr& frag, wxSndMode mode);
};

#endif
