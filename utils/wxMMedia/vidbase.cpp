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
#include <wx/fstream.h>
#include "vidbase.h"
#ifdef WX_PRECOMP
#include "wx_prec.h"
#else
#include "wx/wx.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

wxVideoOutput::wxVideoOutput()
  : wxWindow()
{
  dyn_size = TRUE;
}

wxVideoOutput::wxVideoOutput(wxWindow *parent, const wxWindowID id, const wxPoint& position,
			     const wxSize& size, const long style,
			     const wxString& name)
  : wxWindow(parent, id, position, size, style, name)
{
  dyn_size = TRUE;
}

///
wxVideoOutput::~wxVideoOutput()
{
}

wxVideoBaseDriver::wxVideoBaseDriver()
  : wxMMediaFile()
{
}

wxVideoBaseDriver::wxVideoBaseDriver(wxInputStream& str, bool seekable)
  : wxMMediaFile(str, FALSE, seekable)
{
}

wxVideoBaseDriver::wxVideoBaseDriver(const wxString& fname)
  : wxMMediaFile(fname)
{
}

wxVideoBaseDriver::~wxVideoBaseDriver()
{
}

bool wxVideoBaseDriver::AttachOutput(wxVideoOutput& output)
{
  video_output = &output;
  return TRUE;
}

void wxVideoBaseDriver::DetachOutput()
{
  video_output = NULL;
}

// Use an external frame for video output

wxFrame *wxVideoCreateFrame(wxVideoBaseDriver *vid_drv)
{
  wxFrame *frame = new wxFrame(NULL, -1, "Video Output", wxDefaultPosition, wxSize(100, 100));
  wxVideoOutput *vid_out = new wxVideoOutput(frame, -1);

  vid_out->DynamicSize(TRUE);
  vid_drv->AttachOutput(*vid_out);
  frame->Layout();
  frame->Show(TRUE);

  return frame;
}
