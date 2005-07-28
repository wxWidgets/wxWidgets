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
//TODO:
//TODO:  This is really not the best way to play-stop -
//TODO:  it should just have one playbin and stick with it the whole
//TODO:  instance of wxGStreamerMediaBackend - but stopping appears
//TODO:  to invalidate the playbin object...
//TODO:
//
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
#    include "wx/gtk/win_gtk.h"
#    include <gtk/gtksignal.h>
#    if wxUSE_DYNLIB_CLASS
#        include "wx/dynlib.h"
#    endif
//#    include <gst/gconf/gconf.h> //gstreamer gnome interface - needs deps
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
    static void OnVideoCapsReady(GstPad* pad,  GParamSpec* pspec, gpointer data);
    
    static bool TransCapsToVideoSize(wxGStreamerMediaBackend* be, GstPad* caps);
    void PostRecalcSize();
    
#ifdef __WXGTK__
    static gint OnGTKRealize(GtkWidget* theWidget, wxGStreamerMediaBackend* be);
#endif

    GstElement* m_player;       //GStreamer media element
    
    wxSize      m_videoSize;
    wxControl*  m_ctrl;
    
    wxLongLong m_nPausedPos;

    DECLARE_DYNAMIC_CLASS(wxGStreamerMediaBackend);
};


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxGStreamerMediaBackend
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

IMPLEMENT_DYNAMIC_CLASS(wxGStreamerMediaBackend, wxMediaBackend);

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend Constructor
//
// Sets m_player to NULL signifying we havn't loaded anything yet
//---------------------------------------------------------------------------
wxGStreamerMediaBackend::wxGStreamerMediaBackend() : m_player(NULL), m_videoSize(0,0)
{
}

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend Destructor
//
// Stops/cleans up memory  
//---------------------------------------------------------------------------
wxGStreamerMediaBackend::~wxGStreamerMediaBackend()
{
    Cleanup();
}

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::OnGTKRealize
//
// If the window wasn't realized when Load was called, this is the
// callback for when it is.
//
// 1) Installs GTK idle handler if it doesn't exist
// 2) Yeilds to avoid an X11 bug (?)
// 3) Tells GStreamer to play the video in our control
//---------------------------------------------------------------------------
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

    wxYield();    //FIXME: X Server gets an error if I don't do this or a messagebox beforehand?!?!??
    
    GdkWindow *window = GTK_PIZZA(theWidget)->bin_window;
    wxASSERT(window);
    
    GstElement* videosink;
    g_object_get (G_OBJECT (be->m_player), "video-sink", &videosink, NULL);
    
    GstElement* overlay = gst_bin_get_by_interface (GST_BIN (videosink),
                                    GST_TYPE_X_OVERLAY);
    gst_x_overlay_set_xwindow_id( GST_X_OVERLAY(overlay),
                                GDK_WINDOW_XWINDOW( window )
                                );
    
    return 0;
}


#endif

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::Cleanup
//
// Frees the gstreamer interfaces if there were any created
//---------------------------------------------------------------------------
void wxGStreamerMediaBackend::Cleanup()
{
    if(m_player && GST_IS_OBJECT(m_player))
    {
        gst_element_set_state (m_player, GST_STATE_NULL);
        gst_object_unref (GST_OBJECT (m_player));
    }
}

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::CreateControl
//
// Initializes GStreamer and creates the wx side of our media control
//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::TransCapsToVideoSize
//
// Gets the size of our video (in wxSize) from a GstPad
//---------------------------------------------------------------------------
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

        wxLogDebug(wxT("Native video size: [%i,%i]"), be->m_videoSize.x, be->m_videoSize.y);

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

        wxLogDebug(wxT("Adjusted video size: [%i,%i]"), be->m_videoSize.x, be->m_videoSize.y);

        be->PostRecalcSize();        
        return true;
    }//end if caps
    
    return false;
}

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::PostRecalcSize
//
// Forces parent to recalc its layout if it has sizers to update
// to the new video size
//---------------------------------------------------------------------------
void wxGStreamerMediaBackend::PostRecalcSize()
{
        m_ctrl->InvalidateBestSize();
        m_ctrl->GetParent()->Layout();
        m_ctrl->GetParent()->Refresh();
        m_ctrl->GetParent()->Update();
        m_ctrl->SetSize(m_ctrl->GetSize());
}

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::OnFinish
//
// Called by gstreamer when the media is done playing
//
// 1) Send a wxEVT_MEDIA_STOP to the control
// 2) If veteod, break out
// 3) really stop the media
// 4) Send a wxEVT_MEDIA_FINISHED to the control
//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::OnError
//
// Called by gstreamer when an error is encountered playing the media
//
// TODO: Make this better - maybe some more intelligent wxLog stuff
//---------------------------------------------------------------------------
void wxGStreamerMediaBackend::OnError(GstElement *play,
    GstElement *src,
    GError     *err,
    gchar      *debug,
    gpointer    data)
{
    wxLogSysError(
        wxString::Format(
            wxT("Error in wxMediaCtrl!\nError Message:%s\nDebug:%s\n"), 
            (const wxChar*)wxConvUTF8.cMB2WX(err->message),
            (const wxChar*)wxConvUTF8.cMB2WX(debug)
                        )
                 );
}


//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::Load (File version)
//
// Just calls the URI version
//---------------------------------------------------------------------------
bool wxGStreamerMediaBackend::Load(const wxString& fileName)
{
    return Load( 
                    wxURI( 
                            wxString( wxT("file://") ) + fileName 
                         ) 
               );
}

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::OnVideoCapsReady
//
// Called by gstreamer when the video caps for the media is ready
//---------------------------------------------------------------------------
void wxGStreamerMediaBackend::OnVideoCapsReady(GstPad* pad, GParamSpec* pspec, gpointer data)
{
    wxGStreamerMediaBackend::TransCapsToVideoSize((wxGStreamerMediaBackend*) data, pad);    
}

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::Load (URI version)
//
// 1) Stops/Cleanups the previous instance if there is any
// 2) Creates the gstreamer playbin
// 3) If there is no playbin bail out
// 4) Set up the error and end-of-stream callbacks for our player
// 5) Make our video sink and make sure it supports the x overlay interface
// 6) Make sure the passed URI is valid and tell playbin to load it
// 7) Use the xoverlay extension to tell gstreamer to play in our window
// 8) Get the video size - pause required to set the stream in action
//---------------------------------------------------------------------------
bool wxGStreamerMediaBackend::Load(const wxURI& location)
{
    //1
    Cleanup();
    
    //2
    m_player    = gst_element_factory_make ("playbin", "play");

    //3
    if (!m_player)
        return false;
        
    //4
    g_signal_connect (m_player, "eos", G_CALLBACK (OnFinish), this);
    g_signal_connect (m_player, "error", G_CALLBACK (OnError), this);

    //5
    GstElement* overlay = NULL;
    GstElement* videosink;

#if defined(__WXGTK__) && wxUSE_DYNLIB_CLASS

    //use gnome-specific gstreamer extensions
    //if synthisis (?) file not found, it 
    //spits out a warning and uses ximagesink
    wxDynamicLibrary gstgconf;
    if(gstgconf.Load(gstgconf.CanonicalizeName(wxT("gstgconf-0.8"))))
    {
        typedef GstElement* (*LPgst_gconf_get_default_video_sink) (void);
        LPgst_gconf_get_default_video_sink pGst_gconf_get_default_video_sink = 
        (LPgst_gconf_get_default_video_sink)
            gstgconf.GetSymbol(wxT("gst_gconf_get_default_video_sink"));

        if (pGst_gconf_get_default_video_sink)        
        {
            videosink = (*pGst_gconf_get_default_video_sink) ();
            wxASSERT( GST_IS_BIN(videosink) );
            overlay = gst_bin_get_by_interface (GST_BIN (videosink),
                                            GST_TYPE_X_OVERLAY);
        }
        
        gstgconf.Detach();
    }
        
        if ( ! GST_IS_X_OVERLAY(overlay) )
        {
#endif
            wxLogDebug(wxT("Could not load Gnome preferences, reverting to xvimagesink for video for gstreamer"));
            videosink = gst_element_factory_make ("xvimagesink", "videosink");
            if ( !GST_IS_OBJECT(videosink) )
                videosink = gst_element_factory_make ("ximagesink", "videosink");
            
            overlay = videosink;
        
            wxASSERT( GST_IS_X_OVERLAY(overlay) );
            if ( ! GST_IS_X_OVERLAY(overlay) )
                return false;
#if defined(__WXGTK__) && wxUSE_DYNLIB_CLASS
        }
#endif

    g_object_set (G_OBJECT (m_player),
                    "video-sink", videosink,
//                    "audio-sink", m_audiosink,
                    NULL);    

    //6
    wxString locstring = location.BuildUnescapedURI();
    wxASSERT(gst_uri_protocol_is_valid("file"));
    wxASSERT(gst_uri_is_valid(locstring.mb_str()));

    g_object_set (G_OBJECT (m_player), "uri", (const char*)locstring.mb_str(), NULL);
        
    //7    
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

 
    gst_x_overlay_set_xwindow_id( GST_X_OVERLAY(overlay),
#ifdef __WXGTK__
                        GDK_WINDOW_XWINDOW( window )
#else
                        ctrl->GetHandle()
#endif
                                  );

#ifdef __WXGTK__                                  
    } //end else block
#endif 
    
    //8
	int nResult = gst_element_set_state (m_player, GST_STATE_PAUSED);
	if(nResult != GST_STATE_SUCCESS)
	{
	    wxLogDebug(wxT("Could not set initial state to paused!"));
	    return false;
	}

    const GList *list = NULL;
    g_object_get (G_OBJECT (m_player), "stream-info", &list, NULL);

    bool bVideoFound = false;

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

            bVideoFound = true;
            break;
        }//end if video
        else
        {
            m_videoSize = wxSize(0,0);
            PostRecalcSize();
        }
    }//end searching through info list    

    if(!bVideoFound)
    {
        wxLogDebug(wxT("No video found for gstreamer stream"));
    }
    m_nPausedPos = 0;

    //send loaded event
    wxMediaEvent theEvent(wxEVT_MEDIA_LOADED,
                            m_ctrl->GetId());
    m_ctrl->AddPendingEvent(theEvent);
    
    return true;
}

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::Play
//
// Sets the stream to a playing state
//---------------------------------------------------------------------------
bool wxGStreamerMediaBackend::Play()
{
    if (gst_element_set_state (m_player, GST_STATE_PLAYING)
            != GST_STATE_SUCCESS)
        return false;
    return true;
}

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::Pause
//
// Marks where we paused and pauses the stream
//---------------------------------------------------------------------------
bool wxGStreamerMediaBackend::Pause()
{
    m_nPausedPos = GetPosition();
    if (gst_element_set_state (m_player, GST_STATE_PAUSED)
            != GST_STATE_SUCCESS)
        return false;
    return true;
}

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::Stop
//
// Pauses the stream and sets the position to 0
//---------------------------------------------------------------------------
bool wxGStreamerMediaBackend::Stop()
{
    if (gst_element_set_state (m_player,
                    GST_STATE_PAUSED)    != GST_STATE_SUCCESS)
        return false;
    return wxGStreamerMediaBackend::SetPosition(0);
}

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::GetState
//
// Gets the state of the stream
//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::GetPosition
//
// If paused, returns our marked position - otherwise it queries the 
// GStreamer playbin for the position and returns that
//
//TODO:
//TODO: In lue of the last big TODO, when you pause and seek gstreamer
//TODO: doesn't update the position sometimes, so we need to keep track of whether    
//TODO: we have paused or not and keep track of the time after the pause
//TODO: and whenever the user seeks while paused
//TODO:
//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::SetPosition
//
// Sets the position of the stream
// Note that GST_MSECOND is 1000000 (GStreamer uses nanoseconds - so
// there is 1000000 nanoseconds in a millisecond)
//
// If paused marks where we seeked to 
//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::GetDuration
//
// Obtains the total time of our stream
//---------------------------------------------------------------------------
wxLongLong wxGStreamerMediaBackend::GetDuration()
{
    gint64 length;
    GstFormat fmtTime = GST_FORMAT_TIME;

    if(!gst_element_query(m_player, GST_QUERY_TOTAL, &fmtTime, &length))
        return 0;
    return length / GST_MSECOND ;
}

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::Move
//
// Called when the window is moved - GStreamer takes care of this
// for us so nothing is needed
//---------------------------------------------------------------------------
void wxGStreamerMediaBackend::Move(int x, int y, int w, int h)
{
}

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::GetVideoSize
//
// Returns our cached video size from Load/OnVideoCapsReady
//---------------------------------------------------------------------------
wxSize wxGStreamerMediaBackend::GetVideoSize() const
{    
    return m_videoSize;
}

//---------------------------------------------------------------------------
// wxGStreamerMediaBackend::GetPlaybackRate
// wxGStreamerMediaBackend::SetPlaybackRate
//
// Obtains/Sets the playback rate of the stream
//
//TODO: PlaybackRate not currently supported via playbin directly -
//TODO: Ronald S. Bultje noted on gstreamer-devel:
//TODO:
//TODO: Like "play at twice normal speed"? Or "play at 25 fps and 44,1 kHz"? As
//TODO: for the first, yes, we have elements for that, btu they"re not part of
//TODO: playbin. You can create a bin (with a ghost pad) containing the actual
//TODO: video/audiosink and the speed-changing element for this, and set that
//TODO: element as video-sink or audio-sink property in playbin. The
//TODO: audio-element is called "speed", the video-element is called "videodrop"
//TODO: (although that appears to be deprecated in favour of "videorate", which
//TODO: again cannot do this, so this may not work at all in the end). For
//TODO: forcing frame/samplerates, see audioscale and videorate. Audioscale is
//TODO: part of playbin.
//---------------------------------------------------------------------------
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





