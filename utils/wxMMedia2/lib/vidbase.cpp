////////////////////////////////////////////////////////////////////////////////
// Name:       vidbdrv.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "vidbase.h"
#endif

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/stream.h>
#include <wx/wfstream.h>
#endif

#include "vidbase.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxVideoBaseDriver, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxVideoOutput, wxWindow)
#endif

wxVideoOutput::wxVideoOutput()
  : wxWindow()
{
  m_dyn_size = TRUE;
}

wxVideoOutput::wxVideoOutput(wxWindow *parent, const wxWindowID id, const wxPoint& position,
			     const wxSize& size, const long style,
			     const wxString& name)
  : wxWindow(parent, id, position, size, style, name)
{
  m_dyn_size = TRUE;
}

///
wxVideoOutput::~wxVideoOutput()
{
}

wxVideoBaseDriver::wxVideoBaseDriver()
{
  m_video_output = NULL;
}

wxVideoBaseDriver::wxVideoBaseDriver(wxInputStream& str)
{
  m_video_output = NULL;
}

wxVideoBaseDriver::~wxVideoBaseDriver()
{
}

bool wxVideoBaseDriver::AttachOutput(wxVideoOutput& output)
{
  m_video_output = &output;
  return TRUE;
}

void wxVideoBaseDriver::DetachOutput()
{
  m_video_output = NULL;
}

// Use an external frame for video output

wxFrame *wxVideoCreateFrame(wxVideoBaseDriver *vid_drv)
{
  wxFrame *frame = new wxFrame(NULL, -1, "Video Output", wxDefaultPosition, wxSize(100, 100));
  wxVideoOutput *vid_out = new wxVideoOutput(frame, -1, wxPoint(0, 0), wxSize(300, 300));

  vid_out->DynamicSize(TRUE);
  frame->Layout();
  frame->Show(TRUE);
  wxYield();

  vid_drv->AttachOutput(*vid_out);

  return frame;
}
