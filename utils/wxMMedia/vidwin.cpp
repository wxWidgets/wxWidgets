////////////////////////////////////////////////////////////////////////////////
// Name:       vidwin.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    February 1998
// Updated:
// Copyright:  (C) 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "vidwin.h"
#endif

#if 0
#include "wx/wxprec.h"
#else
#include "wx/wx.h"
#endif

#define WXMMEDIA_INTERNAL
#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include "mmtype.h"
#include "mmfile.h"
#include "vidwin.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

wxVideoWindows::wxVideoWindows(void)
{
}

wxVideoWindows::wxVideoWindows(wxInputStream& str, bool seekable)
  : wxVideoBaseDriver(str, seekable)
{
  OpenFile(GetCurrentFile());
}

wxVideoWindows::wxVideoWindows(const char *fname)
  : wxVideoBaseDriver(fname)
{
  OpenFile(fname);
}

wxVideoWindows::~wxVideoWindows(void)
{
  mciSendCommand(internal->dev_id, MCI_CLOSE, 0, 0);

  if (internal)
    delete internal;
}

void wxVideoWindows::OpenFile(const char *fname)
{
  MCI_DGV_OPEN_PARMS open_struct;
  DWORD ret;

  internal = new VIDW_Internal;

  open_struct.lpstrDeviceType = "avivideo";
  open_struct.lpstrElementName = (LPSTR)fname;
  open_struct.hWndParent = 0;

  ret = mciSendCommand(0, MCI_OPEN,
        MCI_OPEN_ELEMENT|MCI_DGV_OPEN_PARENT|MCI_OPEN_TYPE|MCI_DGV_OPEN_32BIT,
                       (DWORD)(LPVOID)&open_struct);
  internal->dev_id = open_struct.wDeviceID;
}

bool wxVideoWindows::Pause(void)
{
  return (mciSendCommand(internal->dev_id, MCI_PAUSE, 0, 0) == 0);
}

bool wxVideoWindows::Resume(void)
{
  return (mciSendCommand(internal->dev_id, MCI_PAUSE, 0, 0) == 0);
}

bool wxVideoWindows::SetVolume(wxUint8 vol)
{
  return TRUE;
}

bool wxVideoWindows::Resize(wxUint16 w, wxUint16 h)
{
  return TRUE;
}

bool wxVideoWindows::IsCapable(wxVideoType v_type)
{
  return (v_type == wxVIDEO_MSAVI);
}

bool wxVideoWindows::AttachOutput(wxVideoOutput& output)
{
  MCI_DGV_WINDOW_PARMS win_struct;

  if (!wxVideoBaseDriver::AttachOutput(output))
    return FALSE;

  win_struct.hWnd = (HWND)output.GetHWND();
  mciSendCommand(internal->dev_id, MCI_WINDOW,
                 MCI_DGV_WINDOW_HWND, (DWORD)(LPVOID)&win_struct);
  return TRUE;
}

void wxVideoWindows::DetachOutput(void)
{
  MCI_DGV_WINDOW_PARMS win_struct;

  wxVideoBaseDriver::DetachOutput();

  win_struct.hWnd = 0;
  mciSendCommand(internal->dev_id, MCI_WINDOW,
                 MCI_DGV_WINDOW_HWND, (DWORD)(LPVOID)&win_struct);
}

bool wxVideoWindows::StartPlay(void)
{
  return (mciSendCommand(internal->dev_id, MCI_PLAY, 0, NULL) == 0);
}

void wxVideoWindows::StopPlay(void)
{
  mciSendCommand(internal->dev_id, MCI_STOP, 0, NULL);
}
