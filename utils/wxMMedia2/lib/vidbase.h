// /////////////////////////////////////////////////////////////////////////////
// Name:       vidbase.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// CVS:	       $Id$
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
/* Real -*- C++ -*- */
#ifndef __VID_bdrv_H__
#define __VID_bdrv_H__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/string.h"
#include "wx/window.h"
#include "wx/frame.h"

///
typedef enum {
  wxVIDEO_MSAVI,
  wxVIDEO_MPEG,
  wxVIDEO_QT,
  wxVIDEO_GIF,
  wxVIDEO_JMOV,
  wxVIDEO_FLI,
  wxVIDEO_IFF,
  wxVIDEO_SGI,
  wxVIDEO_MPEG2
} ///
 wxVideoType;

///
class wxVideoBaseDriver;

///
class wxVideoBaseDriver : public wxObject {
  ///
  DECLARE_ABSTRACT_CLASS(wxVideoBaseDriver)
protected:
  wxWindow *m_video_output;
public:
  //
  wxVideoBaseDriver();
  //
  wxVideoBaseDriver(wxInputStream& str);
  //
  virtual ~wxVideoBaseDriver();

  //
  virtual bool Play() = 0;
  //
  virtual bool Stop() = 0;
  //
  virtual bool Pause() = 0;
  //
  virtual bool Resume() = 0;

  //
  virtual bool SetVolume(wxUint8 vol) = 0;
  //
  virtual bool Resize(wxUint16 w, wxUint16 h) = 0;
  //
  virtual bool GetSize(wxSize& size) const = 0;

  //
  virtual bool IsCapable(wxVideoType WXUNUSED(v_type)) { return FALSE; }

  //
  virtual void OnFinished() {}

  //
  virtual bool AttachOutput(wxWindow& output);
  //
  virtual void DetachOutput();

  virtual bool IsPaused() = 0;
  virtual bool IsStopped() = 0;
};

extern wxFrame *wxVideoCreateFrame(wxVideoBaseDriver *vid_drv);


#endif
