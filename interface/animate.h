/////////////////////////////////////////////////////////////////////////////
// Name:        animate.h
// Purpose:     documentation for wxAnimationCtrl class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxAnimationCtrl
    @wxheader{animate.h}

    This is a static control which displays an animation.
    wxAnimationCtrl API is simple as possible and won't give you full control on the
    animation; if you need it then use wxMediaCtrl.

    This control is useful to display a (small) animation while doing a long task
    (e.g. a "throbber").

    It is only available if @c wxUSE_ANIMATIONCTRL is set to 1 (the default).

    @beginStyleTable
    @style{wxAC_DEFAULT_STYLE}:
           The default style: wxBORDER_NONE.
    @style{wxAC_NO_AUTORESIZE}:
           By default, the control will adjust its size to exactly fit to the
           size of the animation when SetAnimation is called. If this style
           flag is given, the control will not change its size
    @endStyleTable

    @library{wxadv}
    @category{ctrl}
    @appearance{animationctrl.png}

    @seealso
    wxAnimation
*/
class wxAnimationCtrl : public wxControl
{
public:
    /**
        Initializes the object and calls Create() with
        all the parameters.
    */
    wxAnimationCtrl(wxWindow* parent, wxWindowID id,
                    const wxAnimation& anim,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxAC_DEFAULT_STYLE,
                    const wxString& name = "animationctrl");

    /**
        After control creation you must explicitely call Play()
        to start to play the animation. Until that function won't be called, the first
        frame
        of the animation is displayed.
        
        @param parent
            Parent window, must be non-@NULL.
        @param id
            The identifier for the control.
        @param anim
            The initial animation shown in the control.
        @param pos
            Initial position.
        @param size
            Initial size.
        @param style
            The window style, see wxAC_* flags.
        @param name
            Control name.
        
        @returns @true if the control was successfully created or @false if
                 creation failed.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxAnimation& anim,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxAC_DEFAULT_STYLE,
                const wxString& name = "animationctrl");

    /**
        Returns the animation associated with this control.
    */
    wxAnimation GetAnimation();

    /**
        Returns the inactive bitmap shown in this control when the;
        see SetInactiveBitmap() for more info.
    */
    wxBitmap GetInactiveBitmap();

    /**
        Returns @true if the animation is being played.
    */
    bool IsPlaying();

    /**
        Loads the animation from the given file and calls SetAnimation().
        See wxAnimation::LoadFile for more info.
    */
    bool LoadFile(const wxString& file,
                  wxAnimationType animType = wxANIMATION_TYPE_ANY);

    /**
        Starts playing the animation.
        The animation is always played in loop mode (unless the last frame of the
        animation
        has an infinite delay time) and always start from the first frame
        (even if you @ref stop() stopped it while some other frame was
        displayed).
    */
    bool Play();

    /**
        Sets the animation to play in this control.
        If the previous animation is being played, it's @ref stop() Stopped.
        Until Play() isn't called, a static image, the first
        frame of the given animation or the background colour will be shown
        (see SetInactiveBitmap() for more info).
    */
    void SetAnimation(const wxAnimation& anim);

    /**
        Sets the bitmap to show on the control when it's not playing an animation.
        If you set as inactive bitmap @c wxNullBitmap (which is the default), then the
        first frame of the animation is instead shown when the control is inactive; in
        this case,
        if there's no valid animation associated with the control (see
        wxAnimationCtrl::SetAnimation),
        then the background colour of the window is shown.
        If the control is not playing the animation, the given bitmap will be
        immediately
        shown, otherwise it will be shown as soon as Stop()
        is called.
        Note that the inactive bitmap, if smaller than the control's size, will be
        centered in
        the control; if bigger, it will be stretched to fit it.
    */
    void SetInactiveBitmap(const wxBitmap& bmp);

    /**
        Stops playing the animation.
        The control will show the first frame of the animation, a custom static image or
        the window's background colour as specified by the
        last SetInactiveBitmap() call.
    */
    void Stop();
};


/**
    @class wxAnimation
    @wxheader{animate.h}

    This class encapsulates the concept of a platform-dependent animation.
    An animation is a sequence of frames of the same size.
    Sound is not supported by wxAnimation.

    @library{wxadv}
    @category{FIXME}

    @stdobjects
    Objects:
    wxNullAnimation

    @seealso
    wxAnimationCtrl
*/
class wxAnimation : public wxGDIObject
{
public:
    //@{
    /**
        Loads an animation from a file.
        
        @param name
            The name of the file to load.
        @param type
            See LoadFile for more info.
    */
    wxAnimation();
    wxAnimation(const wxAnimation& anim);
    wxAnimation(const wxString& name,
                wxAnimationType type = wxANIMATION_TYPE_ANY);
    //@}

    /**
        Destructor.
        See @ref overview_refcountdestruct "reference-counted object destruction" for
        more info.
    */
    ~wxAnimation();

    /**
        Returns the delay for the i-th frame in milliseconds.
        If @c -1 is returned the frame is to be displayed forever.
    */
    int GetDelay(unsigned int i);

    /**
        Returns the i-th frame as a wxImage.
    */
    wxImage GetFrame(unsigned int i);

    /**
        Returns the number of frames for this animation.
    */
    unsigned int GetFrameCount();

    /**
        Returns the size of the animation.
    */
    wxSize GetSize();

    /**
        Returns @true if animation data is present.
    */
    bool IsOk();

    /**
        Loads an animation from the given stream.
        
        @param stream
            The stream to use to load the animation.
        @param type
            One of the following values:
        
        
        
        
        
        
        
            wxANIMATION_TYPE_GIF
        
        
        
        
            Load an animated GIF file.
        
        
        
        
        
            wxANIMATION_TYPE_ANI
        
        
        
        
            Load an ANI file.
        
        
        
        
        
            wxANIMATION_TYPE_ANY
        
        
        
        
            Try to autodetect the filetype.
        
        @returns @true if the operation succeeded, @false otherwise.
    */
    bool Load(wxInputStream& stream,
              wxAnimationType type = wxANIMATION_TYPE_ANY);

    /**
        Loads an animation from a file.
        
        @param name
            A filename.
        @param type
            One of the following values:
        
        
        
        
        
        
        
            wxANIMATION_TYPE_GIF
        
        
        
        
            Load an animated GIF file.
        
        
        
        
        
            wxANIMATION_TYPE_ANI
        
        
        
        
            Load an ANI file.
        
        
        
        
        
            wxANIMATION_TYPE_ANY
        
        
        
        
            Try to autodetect the filetype.
        
        @returns @true if the operation succeeded, @false otherwise.
    */
    bool LoadFile(const wxString& name,
                  wxAnimationType type = wxANIMATION_TYPE_ANY);

    /**
        Assignment operator, using @ref overview_trefcount "reference counting".
    */
    wxAnimation operator =(const wxAnimation& brush);
};
