/////////////////////////////////////////////////////////////////////////////
// Name:        unix/mediactrl.cpp
// Purpose:     Built-in Media Backends for Unix
// Author:      Ryan Norton <wxprojects@comcast.net>
// Modified by:
// Created:     02/04/05
// RCS-ID:      $Id$
// Copyright:   (c) 2004-2005 Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//  DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
// Pre-compiled header stuff
//---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "mediactrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "wx/mediactrl.h"

//---------------------------------------------------------------------------
// Compilation guard
//---------------------------------------------------------------------------
#if wxUSE_MEDIACTRL

//===========================================================================
//  BACKEND DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
//
//  wxGStreamerMediaBackend
//
// This won't compile/work without a little work yet...
// Uses nanoseconds...
//---------------------------------------------------------------------------
#if wxUSE_GSTREAMER

//---------------------------------------------------------------------------
//  GStreamer Includes
//---------------------------------------------------------------------------
#include <gst/gst.h>
#include <gst/xoverlay/xoverlay.h>

#include <string.h> //strstr

#include "wx/log.h"

#ifdef __WXGTK__
    //for <gdk/gdkx.h>/related for GDK_WINDOW_XWINDOW
#   include "wx/gtk/win_gtk.h"
#endif

class WXDLLIMPEXP_MEDIA wxGStreamerMediaBackend : public wxMediaBackend
{
public:

    wxGStreamerMediaBackend();
    ~wxGStreamerMediaBackend();

    virtual bool CreateControl(wxControl* ctrl, wxWindow* parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name);

    virtual bool Play();
    virtual bool Pause();
    virtual bool Stop();

    virtual bool Load(const wxString& fileName);
    virtual bool Load(const wxURI& location);

    virtual wxMediaState GetState();

    virtual bool SetPosition(wxLongLong where);
    virtual wxLongLong GetPosition();
    virtual wxLongLong GetDuration();

    virtual void Move(int x, int y, int w, int h);
    wxSize GetVideoSize() const;

    virtual double GetPlaybackRate();
    virtual bool SetPlaybackRate(double dRate);

    void Cleanup();

    static void OnFinish(GstElement *play,  gpointer data);
    static void OnError (GstElement *play,  GstElement *src,
                         GError     *err,   gchar      *debug,
                         gpointer    data);

    GstElement* m_player;       //GStreamer media element
    GstElement* m_audiosink;
    GstElement* m_videosink;

    DECLARE_DYNAMIC_CLASS(wxGStreamerMediaBackend);
};


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxGStreamerMediaBackend
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

IMPLEMENT_DYNAMIC_CLASS(wxGStreamerMediaBackend, wxMediaBackend);

wxGStreamerMediaBackend::wxGStreamerMediaBackend()
{
}

wxGStreamerMediaBackend::~wxGStreamerMediaBackend()
{
    gst_element_set_state (m_player, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (m_player));
    gst_object_unref (GST_OBJECT (m_videosink));
    gst_object_unref (GST_OBJECT (m_audiosink));
}

bool wxGStreamerMediaBackend::CreateControl(wxControl* ctrl, wxWindow* parent,
                                wxWindowID id,
                                const wxPoint& pos,
                                const wxSize& size,
                                long style,
                                const wxValidator& validator,
                                const wxString& name)
{
    //init gstreamer
    gst_init(NULL, NULL);

    //
    // Create window
    // By default wxWindow(s) is created with a border -
    // so we need to get rid of those    return Load(
    //
    // Since we don't have a child window like most other
    // backends, we don't need wxCLIP_CHILDREN
    //
    if ( !
    ctrl->wxControl::Create(parent, id, pos, size,
                            style,  //remove borders???
                            validator, name)
        )
        return false;

    m_player    = gst_element_factory_make ("playbin", "play");
    m_audiosink = gst_element_factory_make ("alsasink", "audiosink");
    m_videosink = gst_element_factory_make ("xvimagesink", "videosink");

    g_object_set (G_OBJECT (m_player),
                    "video-sink", m_videosink,
                    "audio-sink", m_audiosink,
                    NULL);

    g_signal_connect (m_player, "eos", G_CALLBACK (OnError), this);
    g_signal_connect (m_player, "error", G_CALLBACK (OnFinish), this);

    if ( ! GST_IS_X_OVERLAY(m_videosink) )
        return false;

    gst_x_overlay_set_xwindow_id( GST_X_OVERLAY(m_videosink),
#ifdef __WXGTK__
                        GDK_WINDOW_XWINDOW(ctrl->GetHandle())
#else
                        ctrl->GetHandle()
#endif
                                  );
    return true;
}

void wxGStreamerMediaBackend::OnFinish(GstElement *play, gpointer    data)
{
    wxGStreamerMediaBackend* m_parent = (wxGStreamerMediaBackend*) data;

    wxMediaEvent theEvent(wxEVT_MEDIA_STOP,
                        m_parent->m_ctrl->GetId());
    m_parent->m_ctrl->ProcessEvent(theEvent);

    if(theEvent.IsAllowed())
    {
        bool bOk = m_parent->Stop();
        wxASSERT(bOk);

        //send the event to our child
        wxMediaEvent theEvent(wxEVT_MEDIA_FINISHED,
                            m_parent->m_ctrl->GetId());
        m_parent->m_ctrl->ProcessEvent(theEvent);
    }
}

void wxGStreamerMediaBackend::OnError(GstElement *play,
    GstElement *src,
    GError     *err,
    gchar      *debug,
    gpointer    data)
{
    wxLogSysError(wxString::Format(wxT("Error in GStreamer Playback!\nError Message:%s"), wxString(err->message).c_str()));
}


bool wxGStreamerMediaBackend::Load(const wxString& fileName)
{
    return Load(
                wxURI(
                    wxString( wxT("file://") ) + fileName
                     )
               );
}

bool wxGStreamerMediaBackend::Load(const wxURI& location)
{
    Cleanup();
    wxString locstring = location.BuildURI();


    if ( GST_STATE(m_player) > GST_STATE_READY )
        gst_element_set_state(m_player, GST_STATE_READY);

    g_object_set (G_OBJECT (m_player), "uri", locstring.c_str(), NULL);

    gst_x_overlay_expose(GST_X_OVERLAY(m_videosink));

    return GST_STATE(m_player) == GST_STATE_READY;
}

bool wxGStreamerMediaBackend::Play()
{
    if (gst_element_set_state (m_player, GST_STATE_PLAYING)
            != GST_STATE_SUCCESS)
        return false;
    return true;
}

bool wxGStreamerMediaBackend::Pause()
{
    if (gst_element_set_state (m_player, GST_STATE_PAUSED)
            != GST_STATE_SUCCESS)
        return false;
    return true;
}

bool wxGStreamerMediaBackend::Stop()
{
    if (gst_element_set_state (m_player,
                    GST_STATE_READY)    != GST_STATE_SUCCESS)
        return false;
    return true;
}

wxMediaState wxGStreamerMediaBackend::GetState()
{
    switch(GST_STATE(m_player))
    {
        case GST_STATE_PLAYING:
            return wxMEDIASTATE_PLAYING;
        case GST_STATE_PAUSED:
            return wxMEDIASTATE_PAUSED;
        default://case GST_STATE_READY:
            return wxMEDIASTATE_STOPPED;
    }
}

bool wxGStreamerMediaBackend::SetPosition(wxLongLong where)
{
    return gst_element_seek (play, (GstSeekType) (GST_SEEK_METHOD_SET |
            GST_FORMAT_TIME | GST_SEEK_FLAG_FLUSH),
            where * GST_MSECOND );
}

wxLongLong wxGStreamerMediaBackend::GetPosition()
{
    gint64 pos;
    GstFormat fmtTime = GST_FORMAT_TIME;

    if (!gst_element_query (play, GST_QUERY_POSITION, &fmtTime, &pos))
        return 0;
    return pos / GST_MSECOND ;
}

wxLongLong wxGStreamerMediaBackend::GetDuration()
{
    gint64 length;
    GstFormat fmtTime = GST_FORMAT_TIME;

    if(!gst_element_query(m_player, GST_QUERY_TOTAL, &fmtTime, &length))
        return 0;
    return length / GST_MSECOND ;
}

void wxGStreamerMediaBackend::Move(int x, int y, int w, int h)
{
}

wxSize wxGStreamerMediaBackend::GetVideoSize() const
{
    //TODO: check state
    //TODO: maybe cache size
    wxSize retSize = wxSize(0,0);

    const GList *list = NULL;
    g_object_get (G_OBJECT (m_player), "stream-info", &list, NULL);

    for ( ; list != NULL; list = list->next)
    {
        GObject *info = (GObject *) list->data;
        gint type;
        GParamSpec *pspec;
        GEnumValue *val;
        GstPad *pad = NULL;

        g_object_get (info, "type", &type, NULL);
        pspec = g_object_class_find_property (
                        G_OBJECT_GET_CLASS (info), "type");
        val = g_enum_get_value (G_PARAM_SPEC_ENUM (pspec)->enum_class, type);

        if (strstr (val->value_name, "VIDEO"))
        {
            g_object_get (info, "object", &pad, NULL);
            pad = (GstPad *) GST_PAD_REALIZE (pad);
            wxAssert(pad);

            GstCaps* caps = GST_PAD_CAPS (pad);
            wxAssert(caps);

            const GstStructure *s;
            s = gst_caps_get_structure (caps, 0);
            wxAssert(s);

            gst_structure_get_int (s, "width", &retSize.x);
            gst_structure_get_int (s, "height", &retSize.y);

            const GValue *par;
            par = gst_structure_get_value (s, "pixel-aspect-ratio"));

            if (par)
            {
                int num = gst_value_get_fraction_numerator (par),
                    den = gst_value_get_fraction_denominator (par);

                //TODO: maybe better fraction normalization...
                if (num > den)
                    retSize.x = (int) ((float) num * retSize.x / den);
                else
                    retSize.y = (int) ((float) den * retSize.y / num);
            }
        }
    }
}

double wxGStreamerMediaBackend::GetPlaybackRate()
{
    //guess...
    GstClock* theClock = gst_element_get_clock(m_player);
    wxAssert(theClock);
    return gst_clock_get_speed(theClock);
}

bool wxGStreamerMediaBackend::SetPlaybackRate(double dRate)
{
    //guess...
    GstClock* theClock = gst_element_get_clock(m_player);
    wxAssert(theClock);
    return gst_clock_change_speed(theClock, GetPlaybackRate(), dRate)==dRate;
}

#endif //wxUSE_GSTREAMER

//in source file that contains stuff you don't directly use
#include <wx/html/forcelnk.h>
FORCE_LINK_ME(basewxmediabackends);

#endif //wxUSE_MEDIACTRL





