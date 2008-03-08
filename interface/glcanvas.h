/////////////////////////////////////////////////////////////////////////////
// Name:        glcanvas.h
// Purpose:     documentation for wxGLContext class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxGLContext
    @wxheader{glcanvas.h}

    An instance of a wxGLContext represents the state of an OpenGL state machine
    and the connection between OpenGL and the system.

    The OpenGL state includes everything that can be set with the OpenGL API:
    colors, rendering variables, display lists, texture objects, etc.
    Although it is possible to have multiple rendering contexts share display lists
    in order to save resources,
    this method is hardly used today any more, because display lists are only a
    tiny fraction of the overall state.

    Therefore, one rendering context is usually used with or bound to multiple
    output windows in turn,
    so that the application has access to the complete and identical state while
    rendering into each window.

    Binding (making current) a rendering context with another instance of a
    wxGLCanvas however works only
    if the other wxGLCanvas was created with the same attributes as the wxGLCanvas
    from which the wxGLContext
    was initialized. (This applies to sharing display lists among contexts
    analogously.)

    Note that some wxGLContext features are extremely platform-specific - its best
    to check your native platform's glcanvas header (on windows include/wx/msw/glcanvas.h) to see what features your native platform provides.

    @library{wxgl}
    @category{gl}

    @seealso
    wxGLCanvas
*/
class wxGLContext : public wxObject
{
public:
    /**
        Constructor.
        
        @param win
        The canvas that is used to initialize this context. This parameter is needed
        only temporarily,
        and the caller may do anything with it (e.g. destroy the window) after the
        constructor returned.
        
        It will be possible to bind (make current) this context to any other wxGLCanvas
        that has been created
        with equivalent attributes as win.
        
        @param other
        Context to share display lists with or @NULL (the default) for no sharing.
    */
    wxGLContext(wxGLCanvas* win, const wxGLContext* other=@NULL);

    /**
        Makes the OpenGL state that is represented by this rendering context current
        with the wxGLCanvas @e win.
        Note that @e win can be a different wxGLCanvas window than the one that was
        passed to the constructor of this rendering context.
        If  @e RC  is an object of type wxGLContext, the statements @e
        RC.SetCurrent(win); and @e win.SetCurrent(RC); are equivalent,
        see wxGLCanvas::SetCurrent.
    */
    void SetCurrent(const wxGLCanvas& win);
};


/**
    @class wxGLCanvas
    @wxheader{glcanvas.h}

    wxGLCanvas is a class for displaying OpenGL graphics. It is always used in
    conjunction with wxGLContext as the context can only be
    be made current (i.e. active for the OpenGL commands) when it is associated to
    a wxGLCanvas.

    More precisely, you first need to create a wxGLCanvas window and then create an
    instance of a wxGLContext that is initialized with this
    wxGLCanvas and then later use either wxGLCanvas::SetCurrent
    with the instance of the wxGLContext or
    wxGLContext::SetCurrent with the instance of
    the wxGLCanvas (which might be not the same as was used
    for the creation of the context) to bind the OpenGL state that is represented
    by the rendering context to the canvas, and then finally call
    wxGLCanvas::SwapBuffers to swap the buffers of
    the OpenGL canvas and thus show your current output.

    Notice that previous versions of wxWidgets used to implicitly create a
    wxGLContext inside wxGLCanvas itself. This is still supported in the current
    version but is deprecated now and will be removed in the future, please update
    your code to create the rendering contexts explicitly.

    To set up the attributes for the canvas (number of bits for the depth buffer,
    number of bits for the stencil buffer and so on) you should set up the correct
    values of
    the @e attribList parameter. The values that should be set up and their
    meanings will be described below.

    Notice that OpenGL is not enabled by default. To switch it on, you need to edit
    setup.h under Windows and set @c wxUSE_GLCANVAS to 1 (you may also need
    to have to add @c opengl32.lib and @c glu32.lib to the list of libraries
    your program is linked with). On Unix, pass @c --with-opengl to configure.

    @library{wxgl}
    @category{gl}

    @seealso
    wxGLContext
*/
class wxGLCanvas : public wxWindow
{
public:
    /**
        Creates a window with the given parameters. Notice that you need to create and
        use a wxGLContext to output to this window.
        
        If
        
        @param attribList is not specified, double buffered RGBA mode is used.
        
        parent
        Pointer to a parent window.
        
        @param id
        Window identifier. If -1, will automatically create an identifier.
        
        @param pos
        Window position. wxDefaultPosition is (-1, -1) which indicates that wxWidgets
        should generate a default position for the window.
        
        @param size
        Window size. wxDefaultSize is (-1, -1) which indicates that wxWidgets should
        generate a default size for the window. If no suitable size can be found, the
        window will be sized to 20x20 pixels so that the window is visible but obviously not correctly sized.
        
        @param style
        Window style.
        
        @param name
        Window name.
        
        @param attribList
        Array of integers. With this parameter you can set the device context
        attributes associated to this window.
        This array is zero-terminated: it should be set up with constants described in
        the table above.
        If a constant should be followed by a value, put it in the next array position.
        For example, the WX_GL_DEPTH_SIZE should be followed by the value that
        indicates the number of
        bits for the depth buffer, so:
        
        @param palette
        Palette for indexed colour (i.e. non WX_GL_RGBA) mode.
        Ignored under most platforms.
    */
    wxGLCanvas(wxWindow* parent, wxWindowID id = wxID_ANY,
               const int* attribList = @NULL,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style=0,
               const wxString& name="GLCanvas",
               const wxPalette& palette = wxNullPalette);

    /**
        Determines if a canvas having the specified attributes is available.
        
        Returns @true if attributes are supported.
        
        @param attribList
        See attribList for wxGLCanvas().
    */
    static bool IsDisplaySupported(const int * attribList = @NULL);

    /**
        Sets the current colour for this window (using @c glcolor3f()), using the
        wxWidgets colour database to find a named colour.
    */
    void SetColour(const wxString& colour);

    /**
        Makes the OpenGL state that is represented by the OpenGL rendering context
        @e context current, i.e. it will be used by all subsequent OpenGL calls.
        
        This is equivalent to wxGLContext::SetCurrent
        called with this window as parameter.
        
        Note that this function may only be called when the window is shown on screen,
        in particular it can't usually be called from the constructor as the window
        isn't yet shown at this moment.
        
        Returns @false if an error occurred.
    */
    bool SetCurrent(const wxGLContext context);

    /**
        Swaps the double-buffer of this window, making the back-buffer the front-buffer
        and vice versa,
        so that the output of the previous OpenGL commands is displayed on the window.
        
        Returns @false if an error occurred.
    */
    bool SwapBuffers();
};
