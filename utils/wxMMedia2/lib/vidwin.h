// /////////////////////////////////////////////////////////////////////////////
// Name:       vidwin.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    February 1998
// Updated:
// Copyright:  (C) 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
/* Real -*- C++ -*- */
#ifndef __VID_windows_H__
#define __VID_windows_H__

#ifdef __GNUG__
#pragma interface
#endif

#include "mmtype.h"
#include "mmfile.h"
#ifdef WX_PRECOMP
#include "wx/wxprec.h"
#else
#include "wx/wx.h"
#endif
#include "vidbase.h"

#ifdef WXMMEDIA_INTERNAL
#include <windows.h>
#include <mmsystem.h>

typedef struct VIDW_Internal {
  MCIDEVICEID dev_id;
} VIDW_Internal;
#endif

class wxVideoWindows : public wxVideoBaseDriver {
  DECLARE_DYNAMIC_CLASS(wxVideoWindows)
protected:
  struct VIDW_Internal *internal;

  void OpenFile(const char *fname);
public:
  wxVideoWindows(void);
  wxVideoWindows(wxInputStream& str, bool seekable = FALSE);
  wxVideoWindows(const char *fname);
  virtual ~wxVideoWindows(void);

  virtual bool StartPlay(void);
  virtual void StopPlay(void);
  virtual bool Pause(void);
  virtual bool Resume(void);

  virtual bool SetVolume(wxUint8 vol);
  virtual bool Resize(wxUint16 w, wxUint16 h);

  virtual bool IsCapable(wxVideoType v_type);

  virtual bool AttachOutput(wxVideoOutput& output);
  virtual void DetachOutput(void);
};

#endif
