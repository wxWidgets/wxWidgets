// -----------------------------------------------------------------------------
// Name:       vidwin.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    February 1998
// Updated:
// Copyright:  (C) 1998, 1999, 2000 Guilhem Lavaux
// License:    wxWindows license
// -----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if defined(__WINDOWS__) && !defined(__MINGW32__) && !defined(__WATCOMC__) && !defined(__CYGWIN__)
// versions of Open Watcom and MinGW tested against this source does not
// deliver "digitalv.h" required in this feature

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/stream.h"
#include "wx/wfstream.h"

#define WXMMEDIA_INTERNAL
#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include "wx/mmedia/vidwin.h"

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
    m_remove_file = true;
    m_filename    = wxGetTempFileName(_T("wxvid"));
    m_paused      = false;
    m_stopped     = true;
    m_frameRate   = 1.0;
    
    wxFileOutputStream temp_file(m_filename);
    temp_file << str;

    OpenFile();
}

wxVideoWindows::wxVideoWindows(const wxString& filename)
  : wxVideoBaseDriver(filename)
{
    m_internal    = new wxVIDWinternal;
    m_remove_file = false;
    m_filename    = filename;
    m_paused      = false;
    m_stopped     = true;
    m_frameRate   = 1.0;
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
    MCI_DGV_OPEN_PARMS openStruct;
    MCI_DGV_SET_PARMS setStruct;
    MCI_STATUS_PARMS statusStruct;

    openStruct.lpstrDeviceType = _T("avivideo");
    openStruct.lpstrElementName = (wxChar *)m_filename.c_str();
    openStruct.hWndParent = 0;
    
    mciSendCommand(0, MCI_OPEN,
             MCI_OPEN_ELEMENT|MCI_DGV_OPEN_PARENT|MCI_OPEN_TYPE|MCI_DGV_OPEN_32BIT,
                         (DWORD)(LPVOID)&openStruct);
    m_internal->m_dev_id = openStruct.wDeviceID;


    setStruct.dwCallback = 0;
    setStruct.dwTimeFormat = MCI_FORMAT_FRAMES;

    mciSendCommand(m_internal->m_dev_id, MCI_SET, MCI_SET_TIME_FORMAT,
                         (DWORD)(LPVOID)&setStruct);


    statusStruct.dwCallback = 0;
    statusStruct.dwItem = MCI_DGV_STATUS_FRAME_RATE;
    mciSendCommand(m_internal->m_dev_id, MCI_STATUS,
                         MCI_STATUS_ITEM,
                         (DWORD)(LPVOID)&statusStruct);

    m_frameRate = ((double)statusStruct.dwReturn) / 1000;

    statusStruct.dwItem = MCI_DGV_STATUS_BITSPERSAMPLE;
    mciSendCommand(m_internal->m_dev_id, MCI_STATUS, MCI_STATUS_ITEM,
                         (DWORD)(LPVOID)&statusStruct);
    m_bps = statusStruct.dwReturn;

}

bool wxVideoWindows::Pause()
{
    if (m_paused || m_stopped)
        return true;
    m_paused = true;
    return (mciSendCommand(m_internal->m_dev_id, MCI_PAUSE, MCI_WAIT, 0) == 0);
}

bool wxVideoWindows::Resume()
{
    if (!m_paused || m_stopped)
        return true;
    m_paused = false;
    return (mciSendCommand(m_internal->m_dev_id, MCI_RESUME, 0, 0) == 0);
}

bool wxVideoWindows::IsPaused() const
{
    return m_paused;
}

bool wxVideoWindows::IsStopped() const
{
    return m_stopped;
}

bool wxVideoWindows::GetSize(wxSize& size) const
{
    size.SetWidth(200);
    size.SetHeight(200);
    return true;
}

bool wxVideoWindows::SetSize(wxSize WXUNUSED(size))
{
    return true;
}

bool wxVideoWindows::IsCapable(wxVideoType v_type) const
{
    return (v_type == wxVIDEO_MSAVI);
}

bool wxVideoWindows::AttachOutput(wxWindow& output)
{
    MCI_DGV_WINDOW_PARMS win_struct;
    
    if (!wxVideoBaseDriver::AttachOutput(output))
        return false;
    
    win_struct.hWnd = (HWND)output.GetHWND();
    mciSendCommand(m_internal->m_dev_id, MCI_WINDOW,
           MCI_DGV_WINDOW_HWND, (DWORD)(LPVOID)&win_struct);
    return true;
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
        return false;
    m_stopped = false;
    return (mciSendCommand(m_internal->m_dev_id, MCI_PLAY, 0, NULL) == 0);
}

bool wxVideoWindows::Stop()
{
    MCI_SEEK_PARMS seekStruct;

    if (m_stopped)
        return false;
    m_stopped = true;
    if (::mciSendCommand(m_internal->m_dev_id, MCI_STOP, MCI_WAIT, NULL) != 0)
      return false;

    seekStruct.dwCallback = 0;
    seekStruct.dwTo = 0;
    return (::mciSendCommand(m_internal->m_dev_id, MCI_SEEK, MCI_SEEK_TO_START|MCI_WAIT, (DWORD)(LPVOID)&seekStruct) == 0);
}

// TODO TODO
// I hate windows :-(. The doc says MCI_STATUS should return all info I want but when I call it
// it returns to me with an UNSUPPORTED_FUNCTION error. I will have to do all by myself. Grrrr !

wxString wxVideoWindows::GetMovieCodec() const
{
    return wxT("No info");
}

wxString wxVideoWindows::GetAudioCodec() const
{
    return wxT("No info");
}

wxUint32 wxVideoWindows::GetSampleRate() const
{
    return 22500;
}

wxUint8 wxVideoWindows::GetChannels() const
{
    return 1;
}

wxUint8 wxVideoWindows::GetBPS() const
{
    return m_bps;
}

double wxVideoWindows::GetFrameRate() const
{
    return m_frameRate;
}

wxUint32 wxVideoWindows::GetNbFrames() const
{
    return 0;
}

#endif
   // __WINDOWS__
