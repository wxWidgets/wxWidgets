////////////////////////////////////////////////////////////////////////////////
// Name:       vidxanm.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "vidxanm.h"
#endif
#define WXMMEDIA_INTERNAL
#ifdef __XT__
#define Uses_XLib
#define Uses_XtIntrinsic
#endif
#include "vidbase.h"
#include "vidxanm.h"
#ifdef WX_PRECOMP
#include "wx_prec.h"
#else
#include "wx/wx.h"
#endif
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#ifdef __WXGTK__
#include <gtk/gtkwidget.h>
#include <gdk/gdk.h>
#include <gdk/gdkprivate.h>
#endif

wxVideoXANIM::wxVideoXANIM()
 : wxVideoBaseDriver()
{
  internal = new wxXANIMinternal;
  xanim_started = false;
  paused = false;
}

wxVideoXANIM::wxVideoXANIM(wxInputStream& str)
  : wxVideoBaseDriver(str, false)
{
  internal = new wxXANIMinternal;
  xanim_started = false;
  paused = false;
}

wxVideoXANIM::wxVideoXANIM(const wxString& fname)
  : wxVideoBaseDriver(fname)
{
  internal = new wxXANIMinternal;
  xanim_started = false;
}

wxVideoXANIM::~wxVideoXANIM()
{
  if (xanim_started)
    StopPlay();
  delete internal;
}

bool wxVideoXANIM::StartPlay()
{
  if (!paused && xanim_started)
    return true;
  if (!video_output)
    wxVideoCreateFrame(this);

  if (SendCommand(" ")) {
    paused = false;
    return true;
  }
  return false;
}

bool wxVideoXANIM::Pause()
{
  if (!paused && SendCommand(" ")) {
    paused = TRUE;
    return TRUE;
  }
  return FALSE;
}

bool wxVideoXANIM::Resume()
{
  if (paused && SendCommand(" ")) {
    paused = FALSE;
    return TRUE;
  }
  return FALSE;
}

void wxVideoXANIM::StopPlay()
{
  if (!xanim_started)
    return;

  SendCommand("q");

  xanim_started = FALSE;
  paused = FALSE;
}

bool wxVideoXANIM::SetVolume(wxUint8 vol)
{
  if (vol > 100)
    vol = 100;

  wxString str_vol("v%d", vol);
  return SendCommand(str_vol.GetData());
}

bool wxVideoXANIM::Resize(wxUint16 WXUNUSED(w), wxUint16 WXUNUSED(h))
{
  // Not implemented
  //  Actually, I think that we just need to resize the output window ...
  return FALSE;
}

bool wxVideoXANIM::IsCapable(wxVideoType v_type)
{
  if (v_type == wxVIDEO_MSAVI || v_type == wxVIDEO_MPEG ||
      v_type == wxVIDEO_QT || v_type == wxVIDEO_GIF || v_type == wxVIDEO_JMOV ||
      v_type == wxVIDEO_FLI || v_type == wxVIDEO_IFF || v_type == wxVIDEO_SGI)
    return true;
  else
    return false;
}

bool wxVideoXANIM::AttachOutput(wxVideoOutput& out)
{
  if (!wxVideoBaseDriver::AttachOutput(out))
    return false;

  return RestartXANIM();
}

void wxVideoXANIM::DetachOutput()
{
  SendCommand("q");
  xanim_started = false;
  paused = false;

  wxVideoBaseDriver::DetachOutput();
}

bool wxVideoXANIM::SendCommand(const char *command, char **ret,
                                 wxUint32 *size)
{
  if (!xanim_started)
    if (!RestartXANIM())
      return false;

  // Send a command to XAnim through X11 Property
  XChangeProperty(internal->xanim_dpy, internal->xanim_window,
                  internal->xanim_atom,
		  XA_STRING, 8, PropModeReplace, (unsigned char *)command,
		  strlen(command));
  XFlush(internal->xanim_dpy);
  if (ret) {
    int prop_format;
    Atom prop_type;
    unsigned long extra;

    XGetWindowProperty(internal->xanim_dpy, internal->xanim_window,
                       internal->xanim_ret, 0, 16, True, AnyPropertyType,
                       &prop_type, &prop_format, (unsigned long *)size,
                       &extra, (unsigned char **)ret);
  }
  return true;
}

bool wxVideoXANIM::RestartXANIM()
{
  wxString xanim_command;
  int ret;
  Atom prop_type;
  int prop_format;
  unsigned long nitems;
  unsigned long extra;
  char prop[4];
  bool xanim_chg_size;

  if (!video_output || xanim_started || !GetCurrentFile())
    return false;
  
  // Check if we can change the size of the window dynamicly
  xanim_chg_size =  video_output->DynamicSize();
  // Get current display
#ifdef __XT__
  internal->xanim_dpy = wxAPP_DISPLAY;
#endif
#ifdef __WXGTK__
  internal->xanim_dpy = gdk_display;
#endif
  // Get the window id
#ifdef __XT__
  internal->xanim_window = XtWindow(video_output->FWidget());
#else
  internal->xanim_window =
            ((GdkWindowPrivate *)video_output->m_widget->window)->xwindow;
#endif
  // Get the XANIM atom
  internal->xanim_atom = XInternAtom(internal->xanim_dpy,
                                     "XANIM_PROPERTY", False);

  // Build the command
  xanim_command.sprintf(__XANIM_COMMAND__ " +W%d +Wp +f +B -Zr +q +Zpe +Ae "
			"+Av70 %s %s", internal->xanim_window,
			(xanim_chg_size == true) ? "+Sr" : "",
			(const char *)GetCurrentFile());
  // Execute it
  if (!wxExecute(xanim_command, false))
    return false;

  // Wait for XAnim to be ready
  nitems = 0;
  while (nitems == 0) {
    ret = XGetWindowProperty(internal->xanim_dpy, internal->xanim_window,
			     internal->xanim_atom,
			     0, 4, False, AnyPropertyType, &prop_type,
			     &prop_format, &nitems, &extra,
			     (unsigned char **)&prop);
//    wxYield();
  }

  xanim_started = true;

  return true;
}
