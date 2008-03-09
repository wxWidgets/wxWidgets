/////////////////////////////////////////////////////////////////////////////
// Name:        mediactrl.h
// Purpose:     documentation for wxMediaEvent class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMediaEvent
    @wxheader{mediactrl.h}

    Event wxMediaCtrl uses.

    @library{wxmedia}
    @category{FIXME}
*/
class wxMediaEvent : public wxNotifyEvent
{
public:

};


/**
    @class wxMediaCtrl
    @wxheader{mediactrl.h}

    wxMediaCtrl is a class for displaying types of
    media, such as videos, audio files, natively through native codecs.

    wxMediaCtrl uses native backends to render media, for example on Windows
    there is a ActiveMovie/DirectShow backend, and on Macintosh there is a
    QuickTime backend.

    @library{wxmedia}
    @category{media}

    @seealso
    wxMediaEvent
*/
class wxMediaCtrl : public wxControl
{
public:
    //@{
    /**
        ,
                 wxPoint&
        
        @param pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& szBackend = wxT(""),
                const wxValidatorvalidator = wxDefaultValidator,
                const wxString& name = wxPanelNameStr
                           )
        
        Constructor that calls Create.  You may prefer to call Create directly to check
        to see if wxMediaCtrl is available on the system.
        
        parent
            parent of this control.  Must not be @NULL.
        @param id
            id to use for events
        @param fileName
            If not empty, the path of a file to open.
        @param pos
            Position to put control at.
        @param size
            Size to put the control at and to stretch movie to.
        @param style
            Optional styles.
        @param szBackend
            Name of backend you want to use, leave blank to make
            wxMediaCtrl figure it out.
        @param validator
            validator to use.
        @param name
            Window name.
    */
    wxMediaCtrl() const;
    wxMediaCtrl(wxWindow* parent, wxWindowID id) const;
    //@}

    /**
        Generally, you should almost certainly leave this part up to
        wxMediaCtrl - but if you need a certain backend for a particular
        reason, such as QuickTime for playing .mov files, all you need
        to do to choose a specific backend is to pass the
        name of the backend class to
        Create().
        The following are valid backend identifiers -
        
        @b wxMEDIABACKEND_DIRECTSHOW
        
        
        Use ActiveMovie/DirectShow.  Uses the native ActiveMovie
        (I.E. DirectShow) control. Default backend on Windows and
        supported by nearly all Windows versions, even some
        Windows CE versions. May display a windows media player
        logo while inactive.
        
        @b wxMEDIABACKEND_QUICKTIME
        
        Use QuickTime.  Mac Only.
        WARNING: May not working correctly embedded in a wxNotebook.
        
        @b wxMEDIABACKEND_GSTREAMER
        
        Use GStreamer.  Unix Only. Requires GStreamer 0.8 along
        with at the very least the xvimagesink, xoverlay, and
        gst-play modules of gstreamer to function. You need the correct
        modules to play the relavant files, for example the mad module
        to play mp3s, etc.
        
        @b wxMEDIABACKEND_WMP10
        
        Uses Windows Media Player 10 (Windows only) - works on mobile
        machines with Windows Media Player 10 and desktop machines with
        either Windows Media Player 9 or 10
        
        Note that other backends such as wxMEDIABACKEND_MCI can now be
        found at wxCode.
    */


    /**
        ,
                 wxPoint&
        
        @param pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& szBackend = wxT(""),
                const wxValidatorvalidator = wxDefaultValidator,
                const wxString& name = wxPanelNameStr
                           )
        
        Creates this control.  Returns @false if it can't load the movie located at
        fileName or it cannot load one of its native backends.
        
        If you specify a file to open via fileName and you don't specify a backend to
        use, wxMediaCtrl tries each of its backends until one that can render the path referred to by fileName can be found.
        
        parent
            parent of this control.  Must not be @NULL.
        @param id
            id to use for events
        @param fileName
            If not empty, the path of a file to open.
        @param pos
            Position to put control at.
        @param size
            Size to put the control at and to stretch movie to.
        @param style
            Optional styles.
        @param szBackend
            Name of backend you want to use, leave blank to make
            wxMediaCtrl figure it out.
        @param validator
            validator to use.
        @param name
            Window name.
    */
    bool Create(wxWindow* parent, wxWindowID id) const;

    /**
        Creating a backend for wxMediaCtrl is a rather simple process. Simply derive
        from wxMediaBackendCommonBase and implement the methods you want. The methods
        in wxMediaBackend correspond to those in wxMediaCtrl except for CreateControl
        which does the actual creation of the control, in cases where a custom control
        is not needed you may simply call wxControl::Create.
        You need to make sure to use the DECLARE_CLASS and IMPLEMENT_CLASS macros.
        The only real tricky part is that you need to make sure the file in compiled
        in, which if there are just backends in there will not happen and you may need
        to use a force link hack (see http://www.wxwidgets.org/wiki/index.php/RTTI).
        This is a rather simple example of how to create a backend in the
        wxActiveXContainer documentation.
    */


    /**
        Obtains the best size relative to the original/natural size of the
        video, if there is any. See @ref overview_videosizewxmediactrl "Video size"
        for more information.
    */
    wxSize GetBestSize();

    /**
        Obtains the playback rate, or speed of the media. @c 1.0 represents normal
        speed, while @c 2.0 represents twice the normal speed of the media, for
        example. Not supported on the GStreamer (Unix) backend.
        Returns 0 on failure.
    */
    double GetPlaybackrate();

    /**
        Obtains the state the playback of the media is in -
        
        @b wxMEDIASTATE_STOPPED
        
        The movie has stopped.
        
        @b wxMEDIASTATE_PAUSED
        
        The movie is paused.
        
        @b wxMEDIASTATE_PLAYING
        
        The movie is currently playing.
    */
    wxMediaCtrlState GetState();

    /**
        Gets the volume of the media from a 0.0 to 1.0 range. Note that due to rounding
        and other errors this may not be the exact value sent to SetVolume.
    */
    double GetVolume();

    /**
        Obtains the length - the total amount of time the movie has in milliseconds.
    */
    wxFileOffset Length();

    /**
        Loads the location that @c uri refers to with the proxy @c proxy. Not
        implemented on most backends so it should be called with caution. Returns @false if loading fails.
    */
    bool Load(const wxURI& uri, const wxURI& proxy);

    /**
        Same as @ref loaduri() Load. Kept for wxPython compatability.
    */
    bool LoadURI(const wxURI& uri);

    /**
        Same as @ref loaduriwithproxy() Load. Kept for wxPython compatability.
    */
    bool LoadURIWithProxy(const wxURI& uri, const wxURI& proxy);

    /**
        When wxMediaCtrl plays a file, it plays until the stop position
        is reached (currently the end of the file/stream).  Right before
        it hits the end of the stream, it fires off a EVT_MEDIA_STOP
        event to its parent window, at which point the event handler
        can choose to veto the event, preventing the stream from actually
        stopping.
        Example:
        
        When wxMediaCtrl stops, either by the EVT_MEDIA_STOP not being
        vetoed, or by manually calling
        Stop(), where it actually
        stops is not at the beginning, rather, but at the beginning of
        the stream.  That is, when it stops and play is called, playback
        is gauranteed to start at the beginning of the media.  This is
        because some streams are not seekable, and when stop is called
        on them they return to the beginning, thus wxMediaCtrl tries
        to keep consistant for all types of media.
        Note that when changing the state of the media through Play()
        and other methods, the media may not actually be in the
        wxMEDIASTATE_PLAYING, for example. If you are relying on the
        media being in certain state catch the event relevant to the state.
        See wxMediaEvent for the kinds of events
        that you can catch.
    */


    /**
        Pauses playback of the movie.
    */
    bool Pause();

    /**
        Resumes playback of the movie.
    */
    bool Play();

    /**
        Normally, when you use wxMediaCtrl it is just a window for the video to
        play in.  However, some toolkits have their own media player interface.
        For example, QuickTime generally has a bar below the video with a slider.
        A special feature available to wxMediaCtrl, you can use the toolkit's interface
        instead of
        making your own by using the ShowPlayerControls()
        function.  There are several options for the flags parameter, with
        the two general flags being wxMEDIACTRLPLAYERCONTROLS_NONE which turns off
        the native interface, and wxMEDIACTRLPLAYERCONTROLS_DEFAULT which lets
        wxMediaCtrl decide what native controls on the interface. Be sure to review
        the caveats outlined in @ref overview_videosizewxmediactrl "Video size" before
        doing so.
    */


    /**
        Depending upon the backend, wxMediaCtrl can render
        and display pretty much any kind of media that the native system can -
        such as an image, mpeg video, or mp3 (without license restrictions -
        since it relies on native system calls that may not technically
        have mp3 decoding available, for example, it falls outside the
        realm of licensing restrictions).
        For general operation, all you need to do is call
        Load() to load the file
        you want to render, catch the EVT_MEDIA_LOADED event,
        and then call Play()
        to show the video/audio of the media in that event.
        More complex operations are generally more heavily dependant on the
        capabilities of the backend.  For example, QuickTime cannot set
        the playback rate of certain streaming media - while DirectShow is
        slightly more flexible in that regard.
    */


    /**
        Seeks to a position within the movie.
    */
    wxFileOffset Seek(wxFileOffset where, wxSeekMode mode);

    /**
        Sets the playback rate, or speed of the media, to that referred by @c dRate.
        @c 1.0 represents normal speed, while @c 2.0 represents twice the normal
        speed of the media, for example. Not supported on the GStreamer (Unix) backend.
        Returns @true if successful.
    */
    bool SetPlaybackRate(double dRate);

    /**
        Sets the volume of the media from a 0.0 to 1.0 range to that referred
        by @c dVolume.  @c 1.0 represents full volume, while @c 0.5
        represents half (50 percent) volume, for example.  Note that this may not be
        exact due to conversion and rounding errors, although setting the volume to
        full or none is always exact. Returns @true if successful.
    */
    bool SetVolume(double dVolume);

    /**
        A special feature to wxMediaCtrl. Applications using native toolkits such as
        QuickTime usually have a scrollbar, play button, and more provided to
        them by the toolkit. By default wxMediaCtrl does not do this. However, on
        the directshow and quicktime backends you can show or hide the native controls
        provided by the underlying toolkit at will using ShowPlayerControls. Simply
        calling the function with default parameters tells wxMediaCtrl to use the
        default controls provided by the toolkit. The function takes a
        @c wxMediaCtrlPlayerControls enumeration as follows:
        
        @b wxMEDIACTRLPLAYERCONTROLS_NONE
        
        No controls. return wxMediaCtrl to it's default state.
        
        @b wxMEDIACTRLPLAYERCONTROLS_STEP
        
        Step controls like fastfoward, step one frame etc.
        
        @b wxMEDIACTRLPLAYERCONTROLS_VOLUME
        
        Volume controls like the speaker icon, volume slider, etc.
        
        @b wxMEDIACTRLPLAYERCONTROLS_DEFAULT
        
        Default controls for the toolkit. Currently a typedef for
        wxMEDIACTRLPLAYERCONTROLS_STEP and wxMEDIACTRLPLAYERCONTROLS_VOLUME.
        
        For more see @ref overview_playercontrolswxmediactrl "Player controls".
        Currently
        only implemented on the QuickTime and DirectShow backends. The function
        returns @true on success.
    */
    bool ShowPlayerControls(wxMediaCtrlPlayerControls flags = wxMEDIACTRLPLAYERCONTROLS_DEFAULT);

    /**
        Stops the media.
        See Operation for an overview of how
        stopping works.
    */
    bool Stop();

    /**
        Obtains the current position in time within the movie in milliseconds.
    */
    wxFileOffset Tell();

    /**
        By default, wxMediaCtrl will scale the size of the video to the
        requested amount passed to either it's constructor or Create().
        After calling Load or performing an equivilant operation, you
        can subsequently obtain the "real" size of the video (if there
        is any) by calling GetBestSize(). Note that the actual result
        on the display will be slightly different when ShowPlayerControls
        is activated and the actual video size will be less then
        specified due to the extra controls provided by the native toolkit.
        In addition, the backend may modify GetBestSize() to include the
        size of the extra controls - so if you want the real size of the
        video just disable ShowPlayerControls().
        The idea with setting GetBestSize to the size of the video is
        that GetBestSize is a wxWindow-derived function that is called
        when sizers on a window recalculate. What this means is that
        if you use sizers by default the video will show in it's
        original size without any extra assistance needed from the user.
    */
};
