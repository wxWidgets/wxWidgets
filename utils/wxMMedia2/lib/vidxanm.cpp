////////////////////////////////////////////////////////////////////////////////
// Name:       vidxanm.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, 1999 Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "vidxanm.h"
#endif
#ifdef WX_PRECOMP
#include <wx_prec.h>
#else
#include <wx/wx.h>
#endif

// Pizza !
#include <wx/gtk/win_gtk.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#ifdef __WXGTK__
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>
#include <gdk/gdk.h>
#include <gdk/gdkprivate.h>
#endif

#include <wx/filefn.h>
#include <wx/wfstream.h>

#define WXMMEDIA_INTERNAL
#include "vidbase.h"
#include "vidxanm.h"

IMPLEMENT_DYNAMIC_CLASS(wxVideoXANIM, wxVideoBaseDriver)

class wxVideoXANIMProcess: public wxProcess {
 public:
  wxVideoXANIMProcess(wxVideoXANIM *xanim);

  void OnTerminate(int pid, int status);

 protected:
  wxVideoXANIM *m_vid_xanim;
};


wxVideoXANIMProcess::wxVideoXANIMProcess(wxVideoXANIM *xanim)
{
  m_vid_xanim = xanim;
}

void wxVideoXANIMProcess::OnTerminate(int WXUNUSED(pid), int WXUNUSED(status))
{
  m_vid_xanim->m_xanim_started = FALSE;
}

wxVideoXANIM::wxVideoXANIM()
 : wxVideoBaseDriver()
{
  m_internal = new wxXANIMinternal;
  m_xanim_detector = new wxVideoXANIMProcess(this);
  m_xanim_started = FALSE;
  m_paused = FALSE;
  m_filename = "";
}

wxVideoXANIM::wxVideoXANIM(wxInputStream& str)
  : wxVideoBaseDriver(str)
{
  m_internal = new wxXANIMinternal;
  m_xanim_detector = new wxVideoXANIMProcess(this);
  m_xanim_started = FALSE;
  m_paused = FALSE;

  m_filename = wxGetTempFileName("vidxa");
  wxFileOutputStream fout(m_filename);

  fout << str;
}

wxVideoXANIM::~wxVideoXANIM()
{
  if (m_xanim_started)
    Stop();
  delete m_internal;
  delete m_xanim_detector;

  wxRemoveFile(m_filename);
}

bool wxVideoXANIM::Play()
{
  if (!m_paused && m_xanim_started)
    return TRUE; 
  if (!m_video_output) {
    wxVideoCreateFrame(this);
    return TRUE;
  }

  // The movie starts with xanim
  if (RestartXANIM()) {
    m_paused = FALSE;
    return TRUE;
  }
  return FALSE;
}

bool wxVideoXANIM::Pause()
{
  if (!m_paused && SendCommand(" ")) {
    m_paused = TRUE;
    return TRUE;
  }
  return FALSE;
}

bool wxVideoXANIM::Resume()
{
  if (m_paused && SendCommand(" ")) {
    m_paused = FALSE;
    return TRUE;
  }
  return FALSE;
}

bool wxVideoXANIM::Stop()
{
  if (!m_xanim_started)
    return FALSE;

  SendCommand("q");

  m_xanim_started = FALSE;
  m_paused = FALSE;

  return TRUE;
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

bool wxVideoXANIM::GetSize(wxSize& size) const
{
  // Not implemented
  return FALSE;
}

bool wxVideoXANIM::IsCapable(wxVideoType v_type)
{
  if (v_type == wxVIDEO_MSAVI || v_type == wxVIDEO_MPEG ||
      v_type == wxVIDEO_QT || v_type == wxVIDEO_GIF || v_type == wxVIDEO_JMOV ||
      v_type == wxVIDEO_FLI || v_type == wxVIDEO_IFF || v_type == wxVIDEO_SGI)
    return TRUE;
  else
    return FALSE;
}

bool wxVideoXANIM::IsPaused()
{
  return m_paused;
}

bool wxVideoXANIM::IsStopped()
{
  return !m_xanim_started;
}

bool wxVideoXANIM::AttachOutput(wxWindow& out)
{
  if (!wxVideoBaseDriver::AttachOutput(out))
    return FALSE;

  return TRUE;
}

void wxVideoXANIM::DetachOutput()
{
  SendCommand("q");
  m_xanim_started = FALSE;
  m_paused = FALSE;

  wxVideoBaseDriver::DetachOutput();
}

bool wxVideoXANIM::SendCommand(const char *command, char **ret,
                                 wxUint32 *size)
{
  if (!m_xanim_started)
    if (!RestartXANIM())
      return FALSE;

  // Send a command to XAnim through X11 Property
  XChangeProperty(m_internal->xanim_dpy, m_internal->xanim_window,
                  m_internal->xanim_atom,
		  XA_STRING, 8, PropModeReplace, (unsigned char *)command,
		  strlen(command));
  XFlush(m_internal->xanim_dpy);
  if (ret) {
    int prop_format;
    Atom prop_type;
    unsigned long extra;

    XGetWindowProperty(m_internal->xanim_dpy, m_internal->xanim_window,
                       m_internal->xanim_ret, 0, 16, True, AnyPropertyType,
                       &prop_type, &prop_format, (unsigned long *)size,
                       &extra, (unsigned char **)ret);
  }
  return TRUE;
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

  if (!m_video_output || m_xanim_started)
    return FALSE;
  
  // Check if we can change the size of the window dynamicly
  xanim_chg_size = TRUE;
  // Get current display
#ifdef __WXGTK__
  m_internal->xanim_dpy = gdk_display;
  // We absolutely need the window to be realized.
  GtkPizza *pizza = GTK_PIZZA( m_video_output->m_wxwindow );
  GdkWindow *window = pizza->bin_window;

  m_internal->xanim_window =
            ((GdkWindowPrivate *)window)->xwindow;
#endif
  // Get the XANIM atom
  m_internal->xanim_atom = XInternAtom(m_internal->xanim_dpy,
                                     "XANIM_PROPERTY", False);

  // Build the command
  xanim_command.Printf(_T("xanim -Zr +Ze +Sr +f +W%d +f +q "
			"+Av70 %s %s"), m_internal->xanim_window,
			(xanim_chg_size) ? _T("") : _T(""),
			WXSTRINGCAST m_filename);

  // Execute it
  if (!wxExecute(xanim_command, FALSE, m_xanim_detector))
    return FALSE;

  // Wait for XAnim to be ready
  nitems = 0;
  m_xanim_started = TRUE;
  while (nitems == 0 && m_xanim_started) {
    ret = XGetWindowProperty(m_internal->xanim_dpy, m_internal->xanim_window,
			     m_internal->xanim_atom,
			     0, 4, False, AnyPropertyType, &prop_type,
			     &prop_format, &nitems, &extra,
			     (unsigned char **)&prop);
    wxYield();
  }

  m_paused = FALSE;

  return TRUE;
}
