// -------------------------------------------------------------------------
// Name:       vidxanm.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, 1999 Guilhem Lavaux
// License:    wxWindows license
// -------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "vidxanm.h"
#endif

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
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
#include <wx/datstrm.h>
#include <wx/tokenzr.h>

#define WXMMEDIA_INTERNAL
#include "wx/mmedia/vidbase.h"
#include "wx/mmedia/vidxanm.h"

IMPLEMENT_DYNAMIC_CLASS(wxVideoXANIM, wxVideoBaseDriver)

// -------------------------------------------------------------------------
// End process detector

class wxVideoXANIMProcess: public wxProcess {
public:
    wxVideoXANIMProcess(wxVideoXANIM *xanim);

    void OnTerminate(int pid, int status);

protected:
    wxVideoXANIM *m_vid_xanim;
};

class wxVideoXANIMOutput: public wxProcess {
public:
    wxVideoXANIMOutput();

    void OnTerminate(int pid, int status);
    
    bool IsTerminated() const;
protected:
    bool m_terminated;
};

// -------------------------------------------------------------------------
// XAnim video driver (process handling implementation)

wxVideoXANIMProcess::wxVideoXANIMProcess(wxVideoXANIM *xanim)
{
    m_vid_xanim = xanim;
}

void wxVideoXANIMProcess::OnTerminate(int WXUNUSED(pid), int WXUNUSED(status))
{
    m_vid_xanim->m_xanim_started = FALSE;
    m_vid_xanim->OnFinished();
}

wxVideoXANIMOutput::wxVideoXANIMOutput()
        : wxProcess(NULL, -1)
{
    m_terminated = FALSE;
    Redirect();
}

bool wxVideoXANIMOutput::IsTerminated() const
{
    return m_terminated;
}

void wxVideoXANIMOutput::OnTerminate(int pid, int status)
{
    m_terminated = TRUE;
}

// -------------------------------------------------------------------------
// XAnim video driver (implementation)

wxVideoXANIM::wxVideoXANIM()
 : wxVideoBaseDriver()
{
    m_internal       = new wxXANIMinternal;
    m_xanim_detector = new wxVideoXANIMProcess(this);
    m_xanim_started  = FALSE;
    m_paused         = FALSE;
    m_filename       = "";
    m_remove_file    = FALSE;
}

wxVideoXANIM::wxVideoXANIM(wxInputStream& str)
  : wxVideoBaseDriver(str)
{
    m_internal       = new wxXANIMinternal;
    m_xanim_detector = new wxVideoXANIMProcess(this);
    m_xanim_started  = FALSE;
    m_paused         = FALSE;
    m_size[0]        = 0;
    m_size[1]        = 0;
    
    m_filename       = wxGetTempFileName("vidxa");
    m_remove_file    = TRUE;
    wxFileOutputStream fout(m_filename);
    
    fout << str;

    CollectInfo();
}

wxVideoXANIM::wxVideoXANIM(const wxString& filename)
{
    m_internal       = new wxXANIMinternal;
    m_xanim_detector = new wxVideoXANIMProcess(this);
    m_xanim_started  = FALSE;
    m_paused         = FALSE;

    m_filename       = filename;
    m_remove_file    = FALSE;
    m_size[0]        = 0;
    m_size[1]        = 0;
    
    CollectInfo();
}

wxVideoXANIM::~wxVideoXANIM()
{
    if (m_xanim_started)
        Stop();
    delete m_internal;
    delete m_xanim_detector;
    
    if (m_remove_file)
        wxRemoveFile(m_filename);
}

// -------------------------------------------------------------------------
// Movie controller

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

    // We are waiting for the termination of the subprocess.
    while (m_xanim_started) { 
      wxYield();
    }

    m_paused = FALSE;
    
    return TRUE;
}

// -------------------------------------------------------------------------
// Movie size controller

bool wxVideoXANIM::SetSize(wxSize size)
{
    if (!m_video_output)
      return FALSE;

    m_video_output->SetSize(size.GetWidth(), size.GetHeight());
    return FALSE;
}

bool wxVideoXANIM::GetSize(wxSize& size) const
{
    if (m_size[0] == 0)
        return FALSE;
    size.Set(m_size[0], m_size[1]);
    return TRUE;
}

// -------------------------------------------------------------------------
// Capabilities of XAnim

bool wxVideoXANIM::IsCapable(wxVideoType v_type) const
{
    if (v_type == wxVIDEO_MSAVI || v_type == wxVIDEO_MPEG ||
	v_type == wxVIDEO_QT || v_type == wxVIDEO_GIF || v_type == wxVIDEO_JMOV ||
	v_type == wxVIDEO_FLI || v_type == wxVIDEO_IFF || v_type == wxVIDEO_SGI)
        return TRUE;
    else
        return FALSE;
}

// -------------------------------------------------------------------------
// Movie state

wxString wxVideoXANIM::GetMovieCodec() const
{
    if (m_size[0] == 0)
        return wxT("");
    return m_movieCodec;
}

wxString wxVideoXANIM::GetAudioCodec() const
{
    if (m_size[0] == 0)
        return wxT("");
    return m_audioCodec;
}

wxUint32 wxVideoXANIM::GetSampleRate() const
{
    if (m_size[0] == 0)
        return 0;
    return m_sampleRate;
}

wxUint8 wxVideoXANIM::GetChannels() const
{
    if (m_size[0] == 0)
        return 0;
    return m_channels;
}

wxUint8 wxVideoXANIM::GetBPS() const
{
    if (m_size[0] == 0)
        return 0;
    return m_bps;
}

double wxVideoXANIM::GetFrameRate() const
{
    if (m_size[0] == 0)
        return 0.0;
    return m_frameRate;
}

wxUint32 wxVideoXANIM::GetNbFrames() const
{
    if (m_size[0] == 0)
        return 0;
    return m_frames;
}


bool wxVideoXANIM::IsPaused() const
{
    return m_paused;
}

bool wxVideoXANIM::IsStopped() const
{
    return !m_xanim_started;
}

// -------------------------------------------------------------------------
// Output management

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

// -------------------------------------------------------------------------
// Lowlevel XAnim controller

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

bool wxVideoXANIM::CollectInfo()
{
    wxVideoXANIMOutput *xanimProcess;
    wxString xanim_command;
    wxStringTokenizer tokenizer;
    
    xanimProcess = new wxVideoXANIMOutput;
    xanim_command = wxT("xanim +v +Zv -Ae ");
    xanim_command += m_filename;
    if (!wxExecute(xanim_command, FALSE, xanimProcess))
        return FALSE;

    wxInputStream *infoStream = xanimProcess->GetInputStream();
    wxString totalOutput;
        
    while (infoStream->LastError() == wxSTREAM_NOERROR) {
        char line[100];

        infoStream->Read(line, sizeof(line)-1);
        if (infoStream->LastRead() == 0)
            break;
        
        line[infoStream->LastRead()] = 0;
       
        totalOutput += line;        
    }

    // This is good for everything ... :-)
    int position = totalOutput.Find(wxT("Video Codec:"));
    
    totalOutput.Remove(0, position+13);

    position = totalOutput.Find(wxT("depth="));
    m_movieCodec = totalOutput(0, position);

    totalOutput.Remove(0, position);
    tokenizer.SetString(totalOutput, "\n\r");

    // the rest of the line
    wxString token = tokenizer.GetNextToken();
    unsigned long my_long;
    
#define GETINT(i) \
totalOutput.ToULong(&my_long); \
i = my_long;
    
    // 'Audio Codec:'
    totalOutput = tokenizer.GetString();
    totalOutput.Remove(0, totalOutput.Find(wxT(":"))+2);

    position = totalOutput.Find(wxT("Rate"));
    m_audioCodec = totalOutput(0, position-1);

    // 'Rate='
    totalOutput.Remove(0, totalOutput.Find(wxT("="))+1);
    GETINT(m_sampleRate);
    // 'Chans='
    totalOutput.Remove(0, totalOutput.Find(wxT("="))+1);
    GETINT(m_channels);
    // 'Bps='
    totalOutput.Remove(0, totalOutput.Find(wxT("="))+1);
    GETINT(m_bps);
    // 'Frame Stats:'
    tokenizer.Reinit(totalOutput);
    tokenizer.GetNextToken();
    totalOutput = tokenizer.GetString();
    totalOutput.Remove(0, totalOutput.Find(wxT(":"))+2);
    // 'Size='
    totalOutput.Remove(0, totalOutput.Find(wxT("="))+1);
    GETINT(m_size[0]);
    // 'x'
    totalOutput.Remove(0,1);
    GETINT(m_size[1]);
    // 'Frames='
    totalOutput.Remove(0, totalOutput.Find(wxT("="))+1);
    GETINT(m_frames);
    // 'avfps='
    totalOutput.Remove(0, totalOutput.Find(wxT("="))+1);
    totalOutput.ToDouble(&m_frameRate);

    // We wait for the conclusion
    while (!xanimProcess->IsTerminated())
        wxYield();

    delete xanimProcess;

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
    GtkPizza *pizza = GTK_PIZZA( m_video_output->m_wxwindow );
    GdkWindow *window = pizza->bin_window;
    
    m_internal->xanim_window =
        ((GdkWindowPrivate *)window)->xwindow;
#endif
    // Get the XANIM atom
    m_internal->xanim_atom = XInternAtom(m_internal->xanim_dpy,
					 "XANIM_PROPERTY", False);
    
    // Build the command
    xanim_command.Printf(wxT("xanim -Zr +Ze +Sr +f +W%d +f +q "
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

    wxSize vibrato_size;
    
    vibrato_size = m_video_output->GetSize();
    
    vibrato_size.SetWidth(vibrato_size.GetWidth()+1);
    m_video_output->SetSize(vibrato_size);
    vibrato_size.SetWidth(vibrato_size.GetWidth()-1);
    m_video_output->SetSize(vibrato_size);
    // Very useful ! Actually it sends a SETSIZE event to XAnim
    
    m_paused = FALSE;
    
    return TRUE;
}
