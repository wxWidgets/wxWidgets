////////////////////////////////////////////////////////////////////////////////
// Name:       vidbdrv.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/stream.h"
#include "wx/wfstream.h"
#include "wx/intl.h"
#endif

#include "wx/mmedia/vidbase.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

IMPLEMENT_ABSTRACT_CLASS(wxVideoBaseDriver, wxObject)

///
wxVideoBaseDriver::wxVideoBaseDriver()
{
  m_video_output = NULL;
}

wxVideoBaseDriver::wxVideoBaseDriver(wxInputStream& WXUNUSED(str))
{
  m_video_output = NULL;
}

wxVideoBaseDriver::wxVideoBaseDriver(const wxString& WXUNUSED(filename))
{
  m_video_output = NULL;
}

wxVideoBaseDriver::~wxVideoBaseDriver()
{
}

bool wxVideoBaseDriver::AttachOutput(wxWindow& output)
{
  m_video_output = &output;
  return true;
}

void wxVideoBaseDriver::DetachOutput()
{
  m_video_output = NULL;
}

// Use an external frame for video output

wxFrame *wxVideoCreateFrame(wxVideoBaseDriver *vid_drv)
{
  wxFrame *frame = new wxFrame(NULL, wxID_ANY, _("Video Output"), wxDefaultPosition, wxSize(100, 100));
  wxWindow *vid_out = new wxWindow(frame, wxID_ANY, wxPoint(0, 0), wxSize(300, 300));

  frame->Layout();
  frame->Show(true);

  vid_drv->AttachOutput(*vid_out);
  vid_drv->Play();

  return frame;
}
