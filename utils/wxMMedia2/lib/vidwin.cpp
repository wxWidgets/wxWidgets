////////////////////////////////////////////////////////////////////////////////
// Name:       vidwin.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    February 1998
// Updated:
// Copyright:  (C) 1998, 1999, 2000 Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "vidwin.h"
#endif

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/stream.h"
#include "wx/wfstream.h"

#define WXMMEDIA_INTERNAL
#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include "vidwin.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

IMPLEMENT_DYNAMIC_CLASS(wxVideoWindows, wxVideoBaseDriver)

wxVideoWindows::wxVideoWindows()
{
}

wxVideoWindows::wxVideoWindows(wxInputStream& str)
  : wxVideoBaseDriver(str)
{
    m_internal    = new wxVIDWinternal;
    m_remove_file = TRUE;
    m_filename    = wxGetTempFileName("wxvid");
    m_paused      = FALSE;
    m_stopped     = TRUE;
    
    wxFileOutputStream temp_file(m_filename);
    temp_file << str;

    OpenFile();
}

wxVideoWindows::wxVideoWindows(const wxString& filename)
  : wxVideoBaseDriver(filename)
{
    m_internal    = new wxVIDWinternal;
    m_remove_file = FALSE;
    m_filename    = filename;
    m_paused      = FALSE;
    m_stopped     = TRUE;
    OpenFile();
}

wxVideoWindows::~wxVideoWindows(void)
{
    mciSendCommand(m_internal->m_dev_id, MCI_CLOSE, 0, 0);

    if (m_internal)
        delete m_internal;
}

void wxVideoWindows::OpenFile()
{
    MCI_DGV_OPEN_PARMS open_struct;
    DWORD ret;

    open_struct.lpstrDeviceType = "avivideo";
    open_struct.lpstrElementName = (LPSTR)(m_filename.mb_str());
    open_struct.hWndParent = 0;
    
    ret = mciSendCommand(0, MCI_OPEN,
			 MCI_OPEN_ELEMENT|MCI_DGV_OPEN_PARENT|MCI_OPEN_TYPE|MCI_DGV_OPEN_32BIT,
			 (DWORD)(LPVOID)&open_struct);
    m_internal->m_dev_id = open_struct.wDeviceID;
}

bool wxVideoWindows::Pause()
{
    if (m_paused || m_stopped)
        return TRUE;
    m_paused = TRUE;
    return (mciSendCommand(m_internal->m_dev_id, MCI_PAUSE, 0, 0) == 0);
}

bool wxVideoWindows::Resume()
{
    if (!m_paused || m_stopped)
        return TRUE;
    m_paused = FALSE;
    return (mciSendCommand(m_internal->m_dev_id, MCI_PAUSE, 0, 0) == 0);
}

bool wxVideoWindows::IsPaused()
{
    return m_paused;
}

bool wxVideoWindows::IsStopped()
{
    return m_stopped;
}

bool wxVideoWindows::GetSize(wxSize& size) const
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

bool wxVideoWindows::AttachOutput(wxWindow& output)
{
    MCI_DGV_WINDOW_PARMS win_struct;
    
    if (!wxVideoBaseDriver::AttachOutput(output))
        return FALSE;
    
    win_struct.hWnd = (HWND)output.GetHWND();
    mciSendCommand(m_internal->m_dev_id, MCI_WINDOW,
		   MCI_DGV_WINDOW_HWND, (DWORD)(LPVOID)&win_struct);
    return TRUE;
}

void wxVideoWindows::DetachOutput()
{
    MCI_DGV_WINDOW_PARMS win_struct;

    wxVideoBaseDriver::DetachOutput();
    
    win_struct.hWnd = 0;
    mciSendCommand(m_internal->m_dev_id, MCI_WINDOW,
		   MCI_DGV_WINDOW_HWND, (DWORD)(LPVOID)&win_struct);
}

bool wxVideoWindows::Play()
{
    if (!m_stopped)
        return FALSE;
    m_stopped = FALSE;
    return (mciSendCommand(m_internal->m_dev_id, MCI_PLAY, 0, NULL) == 0);
}

bool wxVideoWindows::Stop()
{
    if (m_stopped)
        return FALSE;
    m_stopped = TRUE;
    return (mciSendCommand(m_internal->m_dev_id, MCI_STOP, 0, NULL) == 0);
}
