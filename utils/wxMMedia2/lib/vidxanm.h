// /////////////////////////////////////////////////////////////////////////////
// Name:       vidxanm.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
/* Real -*- C++ -*- */
#ifndef __VID_xanim_H__
#define __VID_xanim_H__

#ifdef __GNUG__
    #pragma interface "vidxanm.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/process.h"

#if defined(WXMMEDIA_INTERNAL) && (defined(__X__) || defined(__WXGTK__))
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

#include "vidbase.h"

#ifdef WXMMEDIA_INTERNAL
typedef struct wxXANIMinternal {
  Display *xanim_dpy;
  Window xanim_window;
  Atom xanim_atom, xanim_ret;
} wxXANIMinternal;

#ifndef __XANIM_COMMAND__
#define __XANIM_COMMAND__ "/usr/X11R6/bin/xanim"
#endif
#endif

class wxVideoXANIM : public wxVideoBaseDriver {
  DECLARE_DYNAMIC_CLASS(wxVideoXANIM)
protected:
  bool m_xanim_started, m_paused;
  struct wxXANIMinternal *m_internal;
  wxString m_filename;
  wxProcess *m_xanim_detector;
public:
  wxVideoXANIM();
  wxVideoXANIM(wxInputStream& str);
  ~wxVideoXANIM();

  bool Play();
  bool Pause();
  bool Resume();
  bool Stop();

  bool SetVolume(wxUint8 vol);
  bool Resize(wxUint16 w, wxUint16 h);
  bool GetSize(wxSize& size) const;

  bool IsCapable(wxVideoType v_type);

  bool AttachOutput(wxWindow& output);
  void DetachOutput();

  bool IsPaused();
  bool IsStopped();

  friend class wxVideoXANIMProcess;

protected:
  ///
  bool RestartXANIM();
  ///
  bool SendCommand(const char *command, char **ret = NULL,
                     wxUint32 *size = NULL);
};

#endif
