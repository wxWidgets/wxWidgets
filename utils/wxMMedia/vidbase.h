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

#include "mmtype.h"
#include "mmfile.h"
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
class wxVideoOutput : public wxWindow {
  ///
  DECLARE_DYNAMIC_CLASS(wxVideoOutput)
protected:
  bool dyn_size;
public:
  ///
  wxVideoOutput();
  ///
  wxVideoOutput(wxWindow *parent, const wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, const long style = 0,
                const wxString& name = "video_output");
  ///
  virtual ~wxVideoOutput();
  
  ///
  bool DynamicSize() { return dyn_size; }
  ///
  void DynamicSize(bool dyn) { dyn_size = dyn; }
};

///
class wxVideoBaseDriver : public wxObject, public wxMMediaFile {
  ///
  DECLARE_ABSTRACT_CLASS(wxVideoBaseDriver)
protected:
  wxVideoOutput *video_output;
public:
  friend class wxVideoOutput;
 
  ///
  wxVideoBaseDriver();
  ///
  wxVideoBaseDriver(wxInputStream& str, bool seekable = FALSE);
  ///
  wxVideoBaseDriver(const wxString& fname);
  ///
  virtual ~wxVideoBaseDriver();

  ///
  virtual bool Pause() = 0;
  ///
  virtual bool Resume() = 0;

  ///
  virtual bool SetVolume(wxUint8 vol) = 0;
  ///
  virtual bool Resize(wxUint16 w, wxUint16 h) = 0;

  ///
  virtual bool IsCapable(wxVideoType WXUNUSED(v_type)) { return FALSE; }

  ///
  virtual void OnFinished() {}

  ///
  virtual bool AttachOutput(wxVideoOutput& output);
  ///
  virtual void DetachOutput();
};

extern wxFrame *wxVideoCreateFrame(wxVideoBaseDriver *vid_drv);


#endif
