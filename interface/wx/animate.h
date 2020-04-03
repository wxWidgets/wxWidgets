/////////////////////////////////////////////////////////////////////////////
// Name:        animate.h
// Purpose:     interface of wxAnimation* classes
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    Supported animation types.
*/
enum wxAnimationType
{
    wxANIMATION_TYPE_INVALID,

    /** represents an animated GIF file. */
    wxANIMATION_TYPE_GIF,

    /** represents an ANI file. */
    wxANIMATION_TYPE_ANI,

    /** autodetect the filetype. */
    wxANIMATION_TYPE_ANY
};


#define wxAC_NO_AUTORESIZE       (0x0010)
#define wxAC_DEFAULT_STYLE       (wxBORDER_NONE)

/**
   Animation implementation types

   @since 3.1.4
*/
enum wxAnimationImplType
{
    /** With this flag wxAnimation will use a native implemetation if available. */
    wxANIMATION_IMPL_TYPE_NATIVE,
    /** Using this flag will cause wxAnimation to use a generic implementation. */
    wxANIMATION_IMPL_TYPE_GENERIC
};



/**
    @class wxGenericAnimationCtrl

    This is a static control which displays an animation.
    wxAnimationCtrl API is as simple as possible and won't give you full control
    on the animation; if you need it then use wxMediaCtrl.

    This control is useful to display a (small) animation while doing a long task
    (e.g. a "throbber").

    It is only available if @c wxUSE_ANIMATIONCTRL is set to 1 (the default).

    @beginStyleTable
    @style{wxAC_DEFAULT_STYLE}
           The default style: wxBORDER_NONE.
    @style{wxAC_NO_AUTORESIZE}
           By default, the control will adjust its size to exactly fit to the
           size of the animation when SetAnimation is called. If this style
           flag is given, the control will not change its size
    @endStyleTable

    @library{wxcore}
    @category{ctrl}

    @nativeimpl{wxgtk,wxmsw}

    @appearance{animationctrl}

    @see wxAnimation, @sample{animate}
*/
class wxGenericAnimationCtrl : public wxControl
{
public:
    /**
        Initializes the object and calls Create() with
        all the parameters.
    */
    wxGenericAnimationCtrl(wxWindow* parent, wxWindowID id,
                    const wxAnimation& anim = wxNullAnimation,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxAC_DEFAULT_STYLE,
                    const wxString& name = wxAnimationCtrlNameStr);

    /**
        Creates the control with the given @a anim animation.

        After control creation you must explicitly call Play() to start to play
        the animation. Until that function won't be called, the first frame
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

        @return @true if the control was successfully created or @false if
                creation failed.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxAnimation& anim = wxNullAnimation,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxAC_DEFAULT_STYLE,
                const wxString& name = wxAnimationCtrlNameStr);

    /**
        Returns the animation associated with this control.
    */
    virtual wxAnimation GetAnimation() const;

    /**
        Returns the inactive bitmap shown in this control when the;
        see SetInactiveBitmap() for more info.
    */
    wxBitmap GetInactiveBitmap() const;

    /**
        Returns @true if the animation is being played.
    */
    virtual bool IsPlaying() const;

    /**
        Loads the animation from the given file and calls SetAnimation().
        See wxAnimation::LoadFile for more info.
    */
    virtual bool LoadFile(const wxString& file,
                          wxAnimationType animType = wxANIMATION_TYPE_ANY);

    /**
        Loads the animation from the given stream and calls SetAnimation().
        See wxAnimation::Load() for more info.
    */
    virtual bool Load(wxInputStream& file,
                      wxAnimationType animType = wxANIMATION_TYPE_ANY);

    /**
        Starts playing the animation.

        The animation is always played in loop mode (unless the last frame of the
        animation has an infinite delay time) and always start from the first frame
        even if you @ref Stop "stopped" it while some other frame was displayed.
    */
    virtual bool Play();

    /**
        Sets the animation to play in this control.

        If the previous animation is being played, it's @ref Stop() stopped.
        Until Play() isn't called, a static image, the first frame of the given
        animation or the background colour will be shown
        (see SetInactiveBitmap() for more info).
    */
    virtual void SetAnimation(const wxAnimation& anim);

    /**
        Sets the bitmap to show on the control when it's not playing an animation.

        If you set as inactive bitmap ::wxNullBitmap (which is the default), then the
        first frame of the animation is instead shown when the control is inactive;
        in this case, if there's no valid animation associated with the control
        (see SetAnimation()), then the background colour of the window is shown.

        If the control is not playing the animation, the given bitmap will be
        immediately shown, otherwise it will be shown as soon as Stop() is called.

        Note that the inactive bitmap, if smaller than the control's size, will be
        centered in the control; if bigger, it will be stretched to fit it.
    */
    virtual void SetInactiveBitmap(const wxBitmap& bmp);

    /**
        Stops playing the animation.
        The control will show the first frame of the animation, a custom static image or
        the window's background colour as specified by the last SetInactiveBitmap() call.
    */
    virtual void Stop();


    /**
       Specify whether the animation's background colour is to be shown (the default),
       or whether the window background should show through

       @note This method is only effective when using the generic version of the control.
    */
    void SetUseWindowBackgroundColour(bool useWinBackground = true);

    /**
       Returns @c true if the window's background colour is being used.

       @note This method is only effective when using the generic version of the control.
    */
    bool IsUsingWindowBackgroundColour() const;

    /**
       This overload of Play() lets you specify if the animation must loop or not

       @note This method is only effective when using the generic version of the control.
    */
    bool Play(bool looped);

    /**
       Draw the current frame of the animation into given DC.
       This is fast as current frame is always cached.

       @note This method is only effective when using the generic version of the control.
    */
    void DrawCurrentFrame(wxDC& dc);


    /**
       Returns a wxBitmap with the current frame drawn in it.

       @note This method is only effective when using the generic version of the control.
    */
    wxBitmap& GetBackingStore();
};


/**
   @class wxAnimationCtrl

   If the platform supports a native animation control (currently just wxGTK)
   then this class implements the control via the native widget.
   Otherwise it is virtually the same as @c wxGenericAnimationCtrl.

   Note that on wxGTK wxAnimationCtrl by default is capable of loading the
   formats supported by the internally-used @c gdk-pixbuf library (typically
   this means only @c wxANIMATION_TYPE_GIF). See @c wxGenericAnimationCtrl if
   you need to support additional file types.

   @library{wxcore}
   @category{gdi}
*/

class wxAnimationCtrl : public wxGenericAnimationCtrl
{
public:
    wxAnimationCtrl();
    wxAnimationCtrl(wxWindow *parent,
                    wxWindowID id,
                    const wxAnimation& anim = wxNullAnimation,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxAC_DEFAULT_STYLE,
                    const wxString& name = wxAnimationCtrlNameStr);

    bool Create(wxWindow *parent, wxWindowID id,
                const wxAnimation& anim = wxNullAnimation,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxAC_DEFAULT_STYLE,
                const wxString& name = wxAnimationCtrlNameStr);

};


/**
   @class wxAnimationImpl

   Abstract base class for native and generic animation classes. An instance
   of one of these classes is used by @c wxAnimation to handle the details of
   the interface between the animation file and the animation control.

   @See wxAnimationGenericImpl
*/
class  wxAnimationImpl : public wxObject, public wxRefCounter
{
public:
    wxAnimationImpl();

    virtual wxAnimationImplType GetImplType() = 0;

    virtual bool IsOk() const = 0;

    virtual int GetDelay(unsigned int frame) const = 0;

    virtual unsigned int GetFrameCount() const = 0;
    virtual wxImage GetFrame(unsigned int frame) const = 0;
    virtual wxSize GetSize() const = 0;

    virtual bool LoadFile(const wxString& name,
                          wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;
    virtual bool Load(wxInputStream& stream,
                      wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;

};


/**
    @class wxAnimationGenericImpl

    This class provides a generic implementation for the @c wxAnimation
    class. It can be used all platforms even if the platform provides a native
    implementation.

    It is unlikely that an application developer would explicitly create
    instances of this class, instead the @c wxAnimation class will create its
    own instance of an implementation when it is created.

    @c wxAnimationGenericImpl supports animated GIF and ANI files out of the
    box. Additional file types can be supported by implementing a class
    derived from @c wxAnimationDecoder and adding an instance of it to the
    handlers managed by this class.

    @library{wxcore}
    @category{gdi}

    @see wxAnimationCtrl, wxAnimation @sample{animate}
*/
class wxAnimationGenericImpl : public wxAnimationImpl
{
public:
    /**
       Default ctor.
    */
    wxAnimationGenericImpl();

    virtual wxAnimationImplType GetImplType();

    /**
        Returns the delay for the i-th frame in milliseconds.
        If @c -1 is returned the frame is to be displayed forever.
    */
    virtual int GetDelay(unsigned int i) const;

    /**
        Returns the i-th frame as a wxImage.

        This method is not implemented in the native wxGTK implementation of
        this class and always returns an invalid image there.
    */
    virtual wxImage GetFrame(unsigned int i) const;

    /**
        Returns the number of frames for this animation.

        This method is not implemented in the native wxGTK implementation of
        this class and always returns 0 there.
    */
    virtual unsigned int GetFrameCount() const;

    /**
        Returns the size of the animation.
    */
    virtual wxSize GetSize() const;

    /**
        Returns @true if animation data is present.
    */
    virtual bool IsOk() const;

    /**
        Loads an animation from the given stream.

        @param stream
            The stream to use to load the animation.
            Under wxGTK may be any kind of stream; under other platforms
            this must be a seekable stream.
        @param type
            One of the ::wxAnimationType enumeration values.

        @return @true if the operation succeeded, @false otherwise.
    */
    virtual bool Load(wxInputStream& stream,
                      wxAnimationType type = wxANIMATION_TYPE_ANY);

    /**
        Loads an animation from a file.

        @param name
            A filename.
        @param type
            One of the ::wxAnimationType values; wxANIMATION_TYPE_ANY
            means that the function should try to autodetect the filetype.

        @return @true if the operation succeeded, @false otherwise.
    */
    virtual bool LoadFile(const wxString& name,
                          wxAnimationType type = wxANIMATION_TYPE_ANY);

    /**
       Retuns the position of the given frame.

       Some kinds animation formats may provide partial frames that should be
       overlayed on the previous frame at a postion other than (0,0).

       @note This method is only available when using the generic version of
       @c wxAnimation and @c wxAnimationCtrl.
     */
    virtual wxPoint GetFramePosition(unsigned int frame) const;

    /**
       Returns the size of the given animation frame.

       @note This method is only available when using the generic version of
       @c wxAnimation and @c wxAnimationCtrl.
     */
    virtual wxSize GetFrameSize(unsigned int frame) const;

    /**
       Returns the type of disposal that should be done for the given
       animation frame.

       @note This method is only available when using the generic version of
       @c wxAnimation and @c wxAnimationCtrl.
     */
    virtual wxAnimationDisposal GetDisposalMethod(unsigned int frame) const;

    /**
       Returns the colour that should be treated as transparent. Returns @c
       wxNullColour if the current decoder does not indicate a transparent
       colour is to be used.

       @note This method is only available when using the generic version of
       @c wxAnimation and @c wxAnimationCtrl.
     */
    virtual wxColour GetTransparentColour(unsigned int frame) const;

    /**
       Returns the colour that should be on the animation's background, if any.
       Returns @c wxNullColour otherwise.

       @note This method is only available when using the generic version of
       @c wxAnimation and @c wxAnimationCtrl.
     */
    virtual wxColour GetBackgroundColour() const;
};


/**
   @class wxAnimation

   The @c wxAnimation class handles the interface between the animation
   control and the details of the animation image or data.

    @stdobjects
    ::wxNullAnimation

    @see wxAnimationCtrl, @sample{animate}
 */
class WXDLLIMPEXP_CORE wxAnimation : public wxObject
{
public:
    wxAnimation(wxAnimationImplType implType = wxANIMATION_IMPL_TYPE_NATIVE);
    wxAnimation(const wxString &name, wxAnimationType type = wxANIMATION_TYPE_ANY,
                wxAnimationImplType implType = wxANIMATION_IMPL_TYPE_NATIVE);
    wxAnimation(const wxAnimation& other);

    wxAnimationImpl* GetImpl() const;

    bool IsOk() const;

    int GetDelay(unsigned int frame) const;
    unsigned int GetFrameCount() const;
    wxImage GetFrame(unsigned int frame);
    wxSize GetSize() const;

    bool LoadFile(const wxString& name, wxAnimationType type = wxANIMATION_TYPE_ANY);
    bool Load(wxInputStream& stream, wxAnimationType type = wxANIMATION_TYPE_ANY);

    
    static inline wxAnimationDecoderList& GetHandlers();
    static void AddHandler(wxAnimationDecoder *handler);
    static void InsertHandler(wxAnimationDecoder *handler);
    static const wxAnimationDecoder *FindHandler( wxAnimationType animType );

    static void CleanUpHandlers();
    static void InitStandardHandlers();    
};



// ============================================================================
// Global functions/macros
// ============================================================================

/**
    An empty animation object.
*/
wxAnimation wxNullAnimation;
