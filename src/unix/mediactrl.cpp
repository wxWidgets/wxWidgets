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
#include "wx/msgdlg.h"

#ifdef __WXGTK__
    //for <gdk/gdkx.h>/related for GDK_WINDOW_XWINDOW
#    include "wx/gtk/win_gtk.h"
#    include <gtk/gtksignal.h>
#endif

//FIXME:
//FIXME:  This is really not the best way to play-stop -
//FIXME:  it should just have one playbin and stick with it the whole
//FIXME:  instance of wxGStreamerMediaBackend - but stopping appears
//FIXME:  to invalidate the playbin object...
//FIXME:

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
    static void OnVideoCapsReady(GstPad* pad,  GParamSpec* pspec, gpointer data);
    
    static bool TransCapsToVideoSize(wxGStreamerMediaBackend* be, GstPad* caps);
    void PostRecalcSize();
    
#ifdef __WXGTK__
    static gint OnGTKRealize(GtkWidget* theWidget, wxGStreamerMediaBackend* be);
#endif

    GstElement* m_player;       //GStreamer media element
    GstElement* m_audiosink;
    GstElement* m_videosink;
    
    wxSize      m_videoSize;
    wxControl*  m_ctrl;
    
    //FIXME:
    //FIXME: In lue of the last big FIXME, when you pause and seek gstreamer
    //FIXME: doesn't update the position sometimes, so we need to keep track of whether    
    //FIXME: we have paused or not and keep track of the time after the pause
    //FIXME: and whenever the user seeks while paused
    //FIXME:
    wxLongLong m_nPausedPos;

    DECLARE_DYNAMIC_CLASS(wxGStreamerMediaBackend);
};


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxGStreamerMediaBackend
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

IMPLEMENT_DYNAMIC_CLASS(wxGStreamerMediaBackend, wxMediaBackend);

wxGStreamerMediaBackend::wxGStreamerMediaBackend() : m_player(NULL), m_videoSize(0,0)
{
}

wxGStreamerMediaBackend::~wxGStreamerMediaBackend()
{
    Cleanup();
}

#ifdef __WXGTK__

#ifdef __WXDEBUG__

#if wxUSE_THREADS
#   define DEBUG_MAIN_THREAD if (wxThread::IsMain() && g_mainThreadLocked) printf("gui reentrance");
#else
#   define DEBUG_MAIN_THREAD
#endif
#else
#define DEBUG_MAIN_THREAD
#endif // Debug

extern void wxapp_install_idle_handler();
extern bool g_isIdle;
extern bool g_mainThreadLocked;

gint wxGStreamerMediaBackend::OnGTKRealize(GtkWidget* theWidget, 
                                           wxGStreamerMediaBackend* be)
{
    DEBUG_MAIN_THREAD
    
    if (g_isIdle)
        wxapp_install_idle_handler();

    wxYield();    //X Server gets an error if I don't do this or a messagebox beforehand?!?!??
    
    GdkWindow *window = GTK_PIZZA(theWidget)->bin_window;
    wxASSERT(window);
    
    gst_x_overlay_set_xwindow_id( GST_X_OVERLAY(be->m_videosink),
                        GDK_WINDOW_XWINDOW( window )
                                  );
    
    return 0;
}


#endif

void wxGStreamerMediaBackend::Cleanup()
{
    if(m_player && GST_IS_OBJECT(m_player))
    {
        //    wxASSERT(GST_IS_OBJECT(m_audiosink));
        //    wxASSERT(GST_IS_OBJECT(m_videosink));
    
        gst_element_set_state (m_player, GST_STATE_NULL);
        gst_object_unref (GST_OBJECT (m_player));
        //gst_object_unref (GST_OBJECT (m_videosink));
        //gst_object_unref (GST_OBJECT (m_audiosink));
    }
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
    
    m_ctrl = ctrl;

    return m_ctrl->wxControl::Create(parent, id, pos, size,
                            style,  //remove borders???
                            validator, name);
}

bool wxGStreamerMediaBackend::TransCapsToVideoSize(wxGStreamerMediaBackend* be, GstPad* pad)
{
    const GstCaps* caps = GST_PAD_CAPS (pad);
    if(caps)
    {

        const GstStructure *s;
        s = gst_caps_get_structure (caps, 0);
        wxASSERT(s);

        gst_structure_get_int (s, "width", &be->m_videoSize.x);
        gst_structure_get_int (s, "height", &be->m_videoSize.y);

        const GValue *par;
        par = gst_structure_get_value (s, "pixel-aspect-ratio");

        if (par)
        {
            int num = gst_value_get_fraction_numerator (par),
                den = gst_value_get_fraction_denominator (par);

            //TODO: maybe better fraction normalization...
            if (num > den)
                be->m_videoSize.x = (int) ((float) num * be->m_videoSize.x / den);
            else
                be->m_videoSize.y = (int) ((float) den * be->m_videoSize.y / num);
        }

        be->PostRecalcSize();        
        return true;
    }//end if caps
    
    return false;
}

//forces parent to recalc its layout if it has sizers to update
//to the new video size
void wxGStreamerMediaBackend::PostRecalcSize()
{
        m_ctrl->InvalidateBestSize();
        m_ctrl->GetParent()->Layout();
        m_ctrl->GetParent()->Refresh();
        m_ctrl->GetParent()->Update();
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
    wxMessageBox(wxString::Format(wxT("Error in wxMediaCtrl!\nError Message:%s"), wxString(err->message, wxConvLocal).c_str()));
}


bool wxGStreamerMediaBackend::Load(const wxString& fileName)
{
    return Load( 
                    wxURI( 
                            wxString( wxT("file://") ) + fileName 
                         ) 
               );
}

void wxGStreamerMediaBackend::OnVideoCapsReady(GstPad* pad, GParamSpec* pspec, gpointer data)
{
    wxGStreamerMediaBackend::TransCapsToVideoSize((wxGStreamerMediaBackend*) data, pad);    
}

bool wxGStreamerMediaBackend::Load(const wxURI& location)
{
    Cleanup();
    
    m_player    = gst_element_factory_make ("playbin", "play");
    m_audiosink = gst_element_factory_make ("alsasink", "audiosink");
    m_videosink = gst_element_factory_make ("xvimagesink", "videosink");

    //no playbin -- outta here :)
    if (!m_player)
        return false;
        
    //have alsa?
    if (GST_IS_OBJECT(m_audiosink) == false)
    {
        //nope, try OSS
        m_audiosink = gst_element_factory_make ("osssink", "audiosink");
         wxASSERT_MSG(GST_IS_OBJECT(m_audiosink), wxT("WARNING: Alsa and OSS drivers for gstreamer not found - audio will be unavailable for wxMediaCtrl"));
    }
    
        
    wxASSERT_MSG(GST_IS_OBJECT(m_videosink), wxT("WARNING: No X video driver for gstreamer not found - video will be unavailable for wxMediaCtrl"));

    g_object_set (G_OBJECT (m_player),
                    "video-sink", m_videosink,
                    "audio-sink", m_audiosink,
                    NULL);

    g_signal_connect (m_player, "eos", G_CALLBACK (OnError), this);
    g_signal_connect (m_player, "error", G_CALLBACK (OnFinish), this);

    wxASSERT( GST_IS_X_OVERLAY(m_videosink) );
    if ( ! GST_IS_X_OVERLAY(m_videosink) )
        return false;
    
    wxString locstring = location.BuildUnescapedURI();
    wxASSERT(gst_uri_protocol_is_valid("file"));
    wxASSERT(gst_uri_is_valid(locstring.mb_str()));

    g_object_set (G_OBJECT (m_player), "uri", (const char*)locstring.mb_str(), NULL);
        
#ifdef __WXGTK__
    if(!GTK_WIDGET_REALIZED(m_ctrl->m_wxwindow))
    {
        //Not realized yet - set to connect at realization time
        gtk_signal_connect( GTK_OBJECT(m_ctrl->m_wxwindow), 
                            "realize",
                            GTK_SIGNAL_FUNC(wxGStreamerMediaBackend::OnGTKRealize),
                            (gpointer) this );
    }        
    else
    {
        wxYield(); //see realize callback...
        GdkWindow *window = GTK_PIZZA(m_ctrl->m_wxwindow)->bin_window;
        wxASSERT(window);
#endif

 
    gst_x_overlay_set_xwindow_id( GST_X_OVERLAY(m_videosink),
#ifdef __WXGTK__
                        GDK_WINDOW_XWINDOW( window )
#else
                        ctrl->GetHandle()
#endif
                                  );

#ifdef __WXGTK__                                  
    } //end else block
#endif 

    wxASSERT(gst_element_set_state (m_player,
                    GST_STATE_PAUSED)    == GST_STATE_SUCCESS);            
    
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
            //Newer gstreamer 0.8+ is SUPPOSED to have "object"...
            //but a lot of old plugins still use "pad" :)
            pspec = g_object_class_find_property (
                        G_OBJECT_GET_CLASS (info), "object");
                        
            if (!pspec)
                g_object_get (info, "pad", &pad, NULL);
            else
                g_object_get (info, "object", &pad, NULL);
            
            pad = (GstPad *) GST_PAD_REALIZE (pad);
            wxASSERT(pad);

            if(!wxGStreamerMediaBackend::TransCapsToVideoSize(this, pad));
            {
                //wait for those caps to get ready
                g_signal_connect(
                pad, 
                "notify::caps", 
                G_CALLBACK(wxGStreamerMediaBackend::OnVideoCapsReady),
                this);
            }
        }//end if video
        else
        {
            m_videoSize = wxSize(0,0);
            PostRecalcSize();
        }
    }//end searching through info list    

    m_nPausedPos = 0;
    return true;
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
    m_nPausedPos = GetPosition();
    if (gst_element_set_state (m_player, GST_STATE_PAUSED)
            != GST_STATE_SUCCESS)
        return false;
    return true;
}

bool wxGStreamerMediaBackend::Stop()
{
    //FIXME:  GStreamer won't update the position for getposition for some reason
    //until it is played again...
    if (gst_element_set_state (m_player,
                    GST_STATE_PAUSED)    != GST_STATE_SUCCESS)
        return false;
    return wxGStreamerMediaBackend::SetPosition(0);
}

wxMediaState wxGStreamerMediaBackend::GetState()
{
    switch(GST_STATE(m_player))
    {
        case GST_STATE_PLAYING:
            return wxMEDIASTATE_PLAYING;
        case GST_STATE_PAUSED:
            if (m_nPausedPos == 0)
                return wxMEDIASTATE_STOPPED;
            else
                return wxMEDIASTATE_PAUSED;
        default://case GST_STATE_READY:
            return wxMEDIASTATE_STOPPED;
    }
}

bool wxGStreamerMediaBackend::SetPosition(wxLongLong where)
{
    if( gst_element_seek (m_player, (GstSeekType) (GST_SEEK_METHOD_SET |
            GST_FORMAT_TIME | GST_SEEK_FLAG_FLUSH),
            where.GetValue() * GST_MSECOND ) )
    {
        if (GetState() != wxMEDIASTATE_PLAYING)
            m_nPausedPos = where;
        
        return true;
    }        
            
    return false;
}

wxLongLong wxGStreamerMediaBackend::GetPosition()
{
    if(GetState() != wxMEDIASTATE_PLAYING)
        return m_nPausedPos;
    else
    {
        gint64 pos;
        GstFormat fmtTime = GST_FORMAT_TIME;
    
        if (!gst_element_query (m_player, GST_QUERY_POSITION, &fmtTime, &pos))
            return 0;
        return pos / GST_MSECOND ;
    }
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
    return m_videoSize;
}

//
//PlaybackRate not currently supported via playbin directly -
// Ronald S. Bultje noted on gstreamer-devel:
//
// Like "play at twice normal speed"? Or "play at 25 fps and 44,1 kHz"? As
// for the first, yes, we have elements for that, btu they"re not part of
// playbin. You can create a bin (with a ghost pad) containing the actual
// video/audiosink and the speed-changing element for this, and set that
// element as video-sink or audio-sink property in playbin. The
// audio-element is called "speed", the video-element is called "videodrop"
// (although that appears to be deprecated in favour of "videorate", which
// again cannot do this, so this may not work at all in the end). For
// forcing frame/samplerates, see audioscale and videorate. Audioscale is
// part of playbin.
//

double wxGStreamerMediaBackend::GetPlaybackRate()
{
    //not currently supported via playbin
    return 1.0;
}

bool wxGStreamerMediaBackend::SetPlaybackRate(double dRate)
{
    //not currently supported via playbin
    return false;
}

#endif //wxUSE_GSTREAMER

//in source file that contains stuff you don't directly use
#include <wx/html/forcelnk.h>
FORCE_LINK_ME(basewxmediabackends);

#endif //wxUSE_MEDIACTRL





