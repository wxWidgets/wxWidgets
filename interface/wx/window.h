/////////////////////////////////////////////////////////////////////////////
// Name:        window.h
// Purpose:     interface of wxWindow
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/**
    Background styles. See wxWindow::SetBackgroundStyle().
*/
enum wxBackgroundStyle
{
    /// Use the default background, as determined by
    /// the system or the current theme.
    wxBG_STYLE_SYSTEM,

    /// Use a solid colour for the background, this style is set automatically if you call
    /// SetBackgroundColour() so you only need to set it explicitly if you had
    /// changed the background style to something else before.
    wxBG_STYLE_COLOUR,

    /// Don't draw the background at all, it's supposed that it is drawn by
    /// the user-defined erase background event handler.
    /// This style should be used to avoid flicker when the background is entirely
    /// custom-drawn.
    wxBG_STYLE_CUSTOM,

    /// The background is (partially) transparent,this style is automatically set if you call
    /// SetTransparent() which is used to set the transparency level.
    wxBG_STYLE_TRANSPARENT
};


/**
    Valid values for wxWindow::ShowWithEffect() and wxWindow::HideWithEffect().
*/
enum wxShowEffect
{
    /// Roll window to the left
    wxSHOW_EFFECT_ROLL_TO_LEFT,

    /// Roll window to the right
    wxSHOW_EFFECT_ROLL_TO_RIGHT,

    /// Roll window to the top
    wxSHOW_EFFECT_ROLL_TO_TOP,

    /// Roll window to the bottom
    wxSHOW_EFFECT_ROLL_TO_BOTTOM,

    /// Slide window to the left
    wxSHOW_EFFECT_SLIDE_TO_LEFT,

    /// Slide window to the right
    wxSHOW_EFFECT_SLIDE_TO_RIGHT,

    /// Slide window to the top
    wxSHOW_EFFECT_SLIDE_TO_TOP,

    /// Slide window to the bottom
    wxSHOW_EFFECT_SLIDE_TO_BOTTOM,

    /// Fade in or out effect
    wxSHOW_EFFECT_BLEND,

    /// Expanding or collapsing effect
    wxSHOW_EFFECT_EXPAND
};

/**
    Different window variants, on platforms like eg mac uses different
    rendering sizes.
*/
enum wxWindowVariant
{
    wxWINDOW_VARIANT_NORMAL,  //!< Normal size
    wxWINDOW_VARIANT_SMALL,   //!< Smaller size (about 25 % smaller than normal)
    wxWINDOW_VARIANT_MINI,    //!< Mini size (about 33 % smaller than normal)
    wxWINDOW_VARIANT_LARGE,   //!< Large size (about 25 % larger than normal)
    wxWINDOW_VARIANT_MAX
};


/**
    Flags which can be used in wxWindow::UpdateWindowUI().
*/
enum wxUpdateUI
{
    wxUPDATE_UI_NONE,
    wxUPDATE_UI_RECURSE,
    wxUPDATE_UI_FROMIDLE  /**<  Invoked from On(Internal)Idle */
};


/**
    @class wxWindow

    wxWindow is the base class for all windows and represents any visible object
    om screen. All controls, top level windows and so on are windows. Sizers and
    device contexts are not, however, as they don't appear on screen themselves.

    Please note that all children of the window will be deleted automatically by
    the destructor before the window itself is deleted which means that you don't
    have to worry about deleting them manually. Please see the @ref
    overview_windowdeletion "window deletion overview" for more information.

    Also note that in this, and many others, wxWidgets classes some
    @c GetXXX() methods may be overloaded (as, for example,
    wxWindow::GetSize or wxWindow::GetClientSize). In this case, the overloads
    are non-virtual because having multiple virtual functions with the same name
    results in a virtual function name hiding at the derived class level (in
    English, this means that the derived class has to override all overloaded
    variants if it overrides any of them). To allow overriding them in the derived
    class, wxWidgets uses a unique protected virtual @c DoGetXXX() method
    and all @c GetXXX() ones are forwarded to it, so overriding the former
    changes the behaviour of the latter.

    @beginStyleTable
    @style{wxBORDER_DEFAULT}
           The window class will decide the kind of border to show, if any.
    @style{wxBORDER_SIMPLE}
           Displays a thin border around the window. wxSIMPLE_BORDER is the
           old name for this style.
    @style{wxBORDER_SUNKEN}
           Displays a sunken border. wxSUNKEN_BORDER is the old name for this
           style.
    @style{wxBORDER_RAISED}
           Displays a raised border. wxRAISED_BORDER is the old name for this
           style.
    @style{wxBORDER_STATIC}
           Displays a border suitable for a static control.  wxSTATIC_BORDER
           is the old name for this style. Windows only.
    @style{wxBORDER_THEME}
           Displays a native border suitable for a control, on the current
           platform. On Windows XP or Vista, this will be a themed border; on
           most other platforms a sunken border will be used. For more
           information for themed borders on Windows, please see Themed
           borders on Windows.
    @style{wxBORDER_NONE}
           Displays no border, overriding the default border style for the
           window. wxNO_BORDER is the old name for this style.
    @style{wxBORDER_DOUBLE}
           This style is obsolete and should not be used.
    @style{wxTRANSPARENT_WINDOW}
           The window is transparent, that is, it will not receive paint
           events. Windows only.
    @style{wxTAB_TRAVERSAL}
           Use this to enable tab traversal for non-dialog windows.
    @style{wxWANTS_CHARS}
           Use this to indicate that the window wants to get all char/key
           events for all keys - even for keys like TAB or ENTER which are
           usually used for dialog navigation and which wouldn't be generated
           without this style.  If you need to use this style in order to get
           the arrows or etc., but would still like to have normal keyboard
           navigation take place, you should call Navigate in response to the
           key events for Tab and Shift-Tab.
    @style{wxNO_FULL_REPAINT_ON_RESIZE}
           On Windows, this style used to disable repainting the window
           completely when its size is changed. Since this behaviour is now
           the default, the style is now obsolete and no longer has an effect.
    @style{wxVSCROLL}
           Use this style to enable a vertical scrollbar. Notice that this
           style cannot be used with native controls which don't support
           scrollbars nor with top-level windows in most ports.
    @style{wxHSCROLL}
           Use this style to enable a horizontal scrollbar. The same
           limitations as for wxVSCROLL apply to this style.
    @style{wxALWAYS_SHOW_SB}
           If a window has scrollbars, disable them instead of hiding them
           when they are not needed (i.e. when the size of the window is big
           enough to not require the scrollbars to navigate it). This style is
           currently implemented for wxMSW, wxGTK and wxUniversal and does
           nothing on the other platforms.
    @style{wxCLIP_CHILDREN}
           Use this style to eliminate flicker caused by the background being
           repainted, then children being painted over them. Windows only.
    @style{wxFULL_REPAINT_ON_RESIZE}
           Use this style to force a complete redraw of the window whenever it
           is resized instead of redrawing just the part of the window
           affected by resizing. Note that this was the behaviour by default
           before 2.5.1 release and that if you experience redraw problems
           with code which previously used to work you may want to try this.
           Currently this style applies on GTK+ 2 and Windows only, and full
           repainting is always done on other platforms.
    @endStyleTable

    @beginExtraStyleTable
    @style{wxWS_EX_VALIDATE_RECURSIVELY}
           By default, Validate/TransferDataTo/FromWindow() only work on
           direct children of the window (compatible behaviour). Set this flag
           to make them recursively descend into all subwindows.
    @style{wxWS_EX_BLOCK_EVENTS}
           wxCommandEvents and the objects of the derived classes are
           forwarded to the parent window and so on recursively by default.
           Using this flag for the given window allows to block this
           propagation at this window, i.e. prevent the events from being
           propagated further upwards. Dialogs have this flag on by default
           for the reasons explained in the @ref overview_eventhandling "Event Handling Overview".
    @style{wxWS_EX_TRANSIENT}
           Don't use this window as an implicit parent for the other windows:
           this must be used with transient windows as otherwise there is the
           risk of creating a dialog/frame with this window as a parent which
           would lead to a crash if the parent is destroyed before the child.
    @style{wxWS_EX_CONTEXTHELP}
           Under Windows, puts a query button on the caption. When pressed,
           Windows will go into a context-sensitive help mode and wxWidgets
           will send a wxEVT_HELP event if the user clicked on an application window.
           This style cannot be used together with wxMAXIMIZE_BOX or wxMINIMIZE_BOX, so
           these two styles are automatically turned of if this one is used.
    @style{wxWS_EX_PROCESS_IDLE}
           This window should always process idle events, even if the mode set
           by wxIdleEvent::SetMode is wxIDLE_PROCESS_SPECIFIED.
    @style{wxWS_EX_PROCESS_UI_UPDATES}
           This window should always process UI update events, even if the
           mode set by wxUpdateUIEvent::SetMode is wxUPDATE_UI_PROCESS_SPECIFIED.
    @endExtraStyleTable

    @library{wxcore}
    @category{miscwnd}

    @see @ref overview_eventhandling "Event handling overview",
         @ref overview_windowsizing "Window sizing overview"
*/
class wxWindow : public wxEvtHandler
{
public:
    /**
       Default constructor
    */
    wxWindow();

    /**
        Constructs a window, which can be a child of a frame, dialog or any other
        non-control window.

        @param parent
            Pointer to a parent window.
        @param id
            Window identifier. If wxID_ANY, will automatically create an identifier.
        @param pos
            Window position. wxDefaultPosition indicates that wxWidgets
            should generate a default position for the window.
            If using the wxWindow class directly, supply an actual position.
        @param size
            Window size. wxDefaultSize indicates that wxWidgets should generate
            a default size for the window. If no suitable size can  be found, the
            window will be sized to 20x20 pixels so that the window is visible but
            obviously not correctly sized.
        @param style
            Window style. For generic window styles, please see wxWindow.
        @param name
            Window name.
    */
    wxWindow(wxWindow* parent, wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = wxPanelNameStr);

    /**
        Destructor.

        Deletes all sub-windows, then deletes itself. Instead of using
        the @b delete operator explicitly, you should normally  use Destroy()
        so that wxWidgets can delete a window only when it is safe to do so, in idle time.

        @see @ref overview_windowdeletion "Window Deletion Overview",
             Destroy(), wxCloseEvent
    */
    virtual ~wxWindow();

    /**
        This method may be overridden in the derived classes to return @false to
        indicate that this control doesn't accept input at all (i.e. behaves like
        e.g. wxStaticText) and so doesn't need focus.

        @see AcceptsFocusFromKeyboard()
    */
    virtual bool AcceptsFocus() const;

    /**
        This method may be overridden in the derived classes to return @false to
        indicate that while this control can, in principle, have focus if the user
        clicks it with the mouse, it shouldn't be included in the TAB traversal chain
        when using the keyboard.
    */
    virtual bool AcceptsFocusFromKeyboard() const;

     /**
        Overridden to indicate wehter this window or one of its children accepts
        focus. Usually it's the same as AcceptsFocus() but is overridden for
        container windows.
     */
    virtual bool AcceptsFocusRecursively() const;

    /**
        Adds a child window. This is called automatically by window creation
        functions so should not be required by the application programmer.
        Notice that this function is mostly internal to wxWidgets and shouldn't be
        called by the user code.

        @param child
            Child window to add.
    */
    virtual void AddChild(wxWindow* child);

    /**
        Call this function to force one or both scrollbars to be always shown, even if
        the window is big enough to show its entire contents without scrolling.

        @since 2.9.0

        @param hflag
            Whether the horizontal scroll bar should always be visible.
        @param vflag
            Whether the vertical scroll bar should always be visible.

        @remarks This function is currently only implemented under Mac/Carbon.
    */
    void AlwaysShowScrollbars(bool hflag, bool vflag);

    /**
        Sets the cached best size value.
    */
    void CacheBestSize(const wxSize& size) const;

    /**
        Returns @true if the system supports transparent windows and calling
        SetTransparent() may succeed. If this function returns @false, transparent
        windows are definitely not supported by the current system.
    */
    virtual bool CanSetTransparent();

    /**
        Directs all mouse input to this window.
        Call ReleaseMouse() to release the capture.

        Note that wxWidgets maintains the stack of windows having captured the mouse
        and when the mouse is released the capture returns to the window which had had
        captured it previously and it is only really released if there were no previous
        window. In particular, this means that you must release the mouse as many times
        as you capture it, unless the window receives the wxMouseCaptureLostEvent event.

        Any application which captures the mouse in the beginning of some operation
        must handle wxMouseCaptureLostEvent and cancel this operation when it receives
        the event. The event handler must not recapture mouse.

        @see ReleaseMouse(), wxMouseCaptureLostEvent
    */
    void CaptureMouse();

    /**
        A synonym for Centre().
    */
    void Center(int direction);

    /**
        A synonym for CentreOnParent().
    */
    void CenterOnParent(int direction);

    /**
        Centres the window.

        @param direction
            Specifies the direction for the centering. May be wxHORIZONTAL, wxVERTICAL
            or wxBOTH. It may also include wxCENTRE_ON_SCREEN flag
            if you want to center the window on the entire screen and not on its
            parent window.

        @remarks If the window is a top level one (i.e. doesn't have a parent),
                 it will be centered relative to the screen anyhow.

        @see Center()
    */
    void Centre(int direction = wxBOTH);

    /**
        Centres the window on its parent. This is a more readable synonym for Centre().

        @param direction
            Specifies the direction for the centering. May be wxHORIZONTAL, wxVERTICAL
            or wxBOTH.

        @remarks This methods provides for a way to center top level windows over
                 their parents instead of the entire screen.  If there
                 is no parent or if the window is not a top level
                 window, then behaviour is the same as Centre().

        @see wxTopLevelWindow::CentreOnScreen
    */
    void CentreOnParent(int direction = wxBOTH);

    /**
        Clears the window by filling it with the current background colour. Does not
        cause an erase background event to be generated.
    */
    virtual void ClearBackground();

    /**
        Converts to screen coordinates from coordinates relative to this window.

        @param x
            A pointer to a integer value for the x coordinate. Pass the client
            coordinate in, and a screen coordinate will be passed out.
        @param y
            A pointer to a integer value for the y coordinate. Pass the client
            coordinate in, and a screen coordinate will be passed out.

        @beginWxPythonOnly
        In place of a single overloaded method name, wxPython implements the following methods:
            - ClientToScreen(point): Accepts and returns a wxPoint
            - ClientToScreenXY(x, y): Returns a 2-tuple, (x, y)
        @endWxPythonOnly
    */
    void ClientToScreen(int* x, int* y) const;

    /**
        Converts to screen coordinates from coordinates relative to this window.

        @param pt
            The client position for the second form of the function.
    */
    wxPoint ClientToScreen(const wxPoint& pt) const;

    /**
        Converts client area size @a size to corresponding window size.

        In other words, the returned value is what would GetSize() return if this
        window had client area of given size.  Components with wxDefaultCoord
        value are left unchanged.  Note that the conversion is not always
        exact, it assumes that non-client area doesn't change and so doesn't
        take into account things like menu bar (un)wrapping or (dis)appearance
        of the scrollbars.

        @since 2.8.8

        @see WindowToClientSize()
    */
    virtual wxSize ClientToWindowSize(const wxSize& size) const;

    /**
        Converts window size @a size to corresponding client area size
        In other words, the returned value is what would GetClientSize() return if
        this window had given window size. Components with wxDefaultCoord value
        are left unchanged.

        Note that the conversion is not always exact, it assumes that
        non-client area doesn't change and so doesn't take into account things
        like menu bar (un)wrapping or (dis)appearance of the scrollbars.

        @since 2.8.8

        @see ClientToWindowSize()
    */
    virtual wxSize WindowToClientSize(const wxSize& size) const;

    /**
        This function simply generates a wxCloseEvent whose handler usually tries
        to close the window. It doesn't close the window itself, however.

        @param force
            @false if the window's close handler should be able to veto the destruction
            of this window, @true if it cannot.

        @remarks Close calls the close handler for the window, providing an
                 opportunity for the window to choose whether to destroy
                 the window. Usually it is only used with the top level
                 windows (wxFrame and wxDialog classes) as the others
                 are not supposed to have any special OnClose() logic.
                The close handler should check whether the window is being deleted
                forcibly, using wxCloseEvent::CanVeto, in which case it should
                destroy the window using wxWindow::Destroy.
                Note that calling Close does not guarantee that the window will
                be destroyed; but it provides a way to simulate a manual close
                of a window, which may or may not be implemented by destroying
                the window. The default implementation of wxDialog::OnCloseWindow
                does not necessarily delete the dialog, since it will simply
                simulate an wxID_CANCEL event which is handled by the appropriate
                button event handler and may do anything at all.
                To guarantee that the window will be destroyed, call
                wxWindow::Destroy instead

        @see @ref overview_windowdeletion "Window Deletion Overview",
             Destroy(), wxCloseEvent
    */
    bool Close(bool force = false);

    //@{
    /**
        Converts a point or size from dialog units to pixels.

        For the x dimension, the dialog units are multiplied by the average character
        width and then divided by 4.
        For the y dimension, the dialog units are multiplied by the average character
        height and then divided by 8.

        @remarks Dialog units are used for maintaining a dialog's proportions
                 even if the font changes.
                You can also use these functions programmatically.
                A convenience macro is defined:
                @code
                #define wxDLG_UNIT(parent, pt) parent->ConvertDialogToPixels(pt)
                @endcode

        @see ConvertPixelsToDialog()
    */
    wxPoint ConvertDialogToPixels(const wxPoint& pt);
    wxSize ConvertDialogToPixels(const wxSize& sz);
    //@}

    //@{
    /**
        Converts a point or size from pixels to dialog units.

        For the x dimension, the pixels are multiplied by 4 and then divided by the
        average character width.
        For the y dimension, the pixels are multiplied by 8 and then divided by the
        average character height.

        @remarks Dialog units are used for maintaining a dialog's proportions
                 even if the font changes.

        @see ConvertDialogToPixels()
    */
    wxPoint ConvertPixelsToDialog(const wxPoint& pt);
    wxSize ConvertPixelsToDialog(const wxSize& sz);
    //@}

    /**
        Destroys the window safely. Use this function instead of the delete operator,
        since different window classes can be destroyed differently. Frames and dialogs
        are not destroyed immediately when this function is called -- they are added
        to a list of windows to be deleted on idle time, when all the window's events
        have been processed. This prevents problems with events being sent to
        non-existent windows.

        @return @true if the window has either been successfully deleted, or it
                 has been added to the list of windows pending real deletion.
    */
    virtual bool Destroy();

    /**
        Destroys all children of a window. Called automatically by the destructor.
    */
    virtual void DestroyChildren();

    /**
        Returns true if this window is in process of being destroyed.

        The top level windows are not deleted immediately but are rather
        scheduled for later destruction to give them time to process any
        pending messages, see Destroy() description.

        This function returns @true if this window, or one of its parent
        windows, is scheduled for destruction and can be useful to avoid
        manipulating it as it's usually useless to do something with a window
        which is on the point of disappearing anyhow.
     */
    bool IsBeingDeleted() const;

    /**
        Disables the window. Same as @ref Enable() Enable(@false).

        @return Returns @true if the window has been disabled, @false if it had
                been already disabled before the call to this function.
    */
    bool Disable();

    /**
        Gets the size which best suits the window: for a control, it would be
        the minimal size which doesn't truncate the control, for a panel - the
        same size as it would have after a call to Fit().

        The default implementation of this function is designed for use in container
        windows, such as wxPanel, and works something like this:
        -# If the window has a sizer then it is used to calculate the best size.
        -# Otherwise if the window has layout constraints then those are used to
           calculate the best size.
        -# Otherwise if the window has children then the best size is set to be large
           enough to show all the children.
        -# Otherwise if there are no children then the window's minimal size will be
           used as its best size.
        -# Otherwise if there is no minimal size set, then the current size is used
           for the best size.

        @see @ref overview_windowsizing
    */
    virtual wxSize DoGetBestSize() const;

    /**
        Does the window-specific updating after processing the update event.
        This function is called by UpdateWindowUI() in order to check return
        values in the wxUpdateUIEvent and act appropriately.
        For example, to allow frame and dialog title updating, wxWidgets
        implements this function as follows:

        @code
        // do the window-specific processing after processing the update event
        void wxTopLevelWindowBase::DoUpdateWindowUI(wxUpdateUIEvent& event)
        {
            if ( event.GetSetEnabled() )
                Enable(event.GetEnabled());

            if ( event.GetSetText() )
            {
                if ( event.GetText() != GetTitle() )
                    SetTitle(event.GetText());
            }
        }
        @endcode
    */
    virtual void DoUpdateWindowUI(wxUpdateUIEvent& event);

    /**
        Enables or disables eligibility for drop file events (OnDropFiles).

        @param accept
            If @true, the window is eligible for drop file events.
            If @false, the window will not accept drop file events.

        @remarks Windows only.
    */
    virtual void DragAcceptFiles(bool accept);

    /**
        Enable or disable the window for user input. Note that when a parent window is
        disabled, all of its children are disabled as well and they are reenabled again
        when the parent is.

        @param enable
            If @true, enables the window for input. If @false, disables the window.

        @return Returns @true if the window has been enabled or disabled, @false
                if nothing was done, i.e. if the window had already
                been in the specified state.

        @see IsEnabled(), Disable(), wxRadioBox::Enable
    */
    virtual bool Enable(bool enable = true);

    /**
        Finds the window or control which currently has the keyboard focus.

        @remarks Note that this is a static function, so it can be called without
                 needing a wxWindow pointer.

        @see SetFocus(), HasFocus()
    */
    static wxWindow* FindFocus();

    /**
        Find a child of this window, by @a id.
        May return @a this if it matches itself.
    */
    wxWindow* FindWindow(long id) const;

    /**
        Find a child of this window, by name.
        May return @a this if it matches itself.
    */
    wxWindow* FindWindow(const wxString& name) const;

    /**
        Find the first window with the given @e id.

        If @a parent is @NULL, the search will start from all top-level frames
        and dialog boxes; if non-@NULL, the search will be limited to the given
        window hierarchy.
        The search is recursive in both cases.

        @see FindWindow()
    */
    static wxWindow* FindWindowById(long id, wxWindow* parent = NULL);

    /**
        Find a window by its label.

        Depending on the type of window, the label may be a window title
        or panel item label. If @a parent is @NULL, the search will start from all
        top-level frames and dialog boxes; if non-@NULL, the search will be
        limited to the given window hierarchy.
        The search is recursive in both cases.

        @see FindWindow()
    */
    static wxWindow* FindWindowByLabel(const wxString& label,
                                       wxWindow* parent = NULL);

    /**
        Find a window by its name (as given in a window constructor or @b Create
        function call).

        If @a parent is @NULL, the search will start from all top-level frames
        and dialog boxes; if non-@NULL, the search will be limited to the given
        window hierarchy.

        The search is recursive in both cases. If no window with such name is found,
        FindWindowByLabel() is called.

        @see FindWindow()
    */
    static wxWindow* FindWindowByName(const wxString& name,
                                      wxWindow* parent = NULL);

    /**
        Sizes the window so that it fits around its subwindows.

        This function won't do anything if there are no subwindows and will only really
        work correctly if sizers are used for the subwindows layout.

        Also, if the window has exactly one subwindow it is better (faster and the result
        is more precise as Fit() adds some margin to account for fuzziness of its calculations)
        to call:

        @code
        window->SetClientSize(child->GetSize());
        @endcode

        instead of calling Fit().

        @see @ref overview_windowsizing
    */
    virtual void Fit();

    /**
        Similar to Fit(), but sizes the interior (virtual) size of a window.

        Mainly useful with scrolled windows to reset scrollbars after sizing
        changes that do not trigger a size event, and/or scrolled windows without
        an interior sizer.  This function similarly won't do anything if there are
        no subwindows.
    */
    virtual void FitInside();

    /**
        Freezes the window or, in other words, prevents any updates from taking
        place on screen, the window is not redrawn at all.

        Thaw() must be called to reenable window redrawing. Calls to these two
        functions may be nested but to ensure that the window is properly
        repainted again, you must thaw it exactly as many times as you froze it.

        If the window has any children, they are recursively frozen too.

        This method is useful for visual appearance optimization (for example,
        it is a good idea to use it before doing many large text insertions in
        a row into a wxTextCtrl under wxGTK) but is not implemented on all
        platforms nor for all controls so it is mostly just a hint to wxWidgets
        and not a mandatory directive.

        @see wxWindowUpdateLocker, Thaw(), IsFrozen()
    */
    void Freeze();

    /**
        Gets the accelerator table for this window. See wxAcceleratorTable.
    */
    wxAcceleratorTable* GetAcceleratorTable();

    /**
        Returns the accessible object for this window, if any.
        See also wxAccessible.
    */
    wxAccessible* GetAccessible();

    /**
        @deprecated
        This method is deprecated, use GetEffectiveMinSize() instead.
    */
    wxSize GetAdjustedBestSize() const;

    /**
        Returns the background colour of the window.

        @see SetBackgroundColour(), SetForegroundColour(), GetForegroundColour()
    */
    wxColour GetBackgroundColour() const;

    /**
        Returns the background style of the window.
        The background style can be one of the wxBackgroundStyle.

        @see SetBackgroundColour(), GetForegroundColour(),
             SetBackgroundStyle(), SetTransparent()
    */
    virtual wxBackgroundStyle GetBackgroundStyle() const;

    /**
        This functions returns the best acceptable minimal size for the window.

        For example, for a static control, it will be the minimal size such that the
        control label is not truncated. For windows containing subwindows (typically
        wxPanel), the size returned by this function will be the same as the size
        the window would have had after calling Fit().
    */
    wxSize GetBestSize() const;

    /**
        Returns the currently captured window.

        @see HasCapture(), CaptureMouse(), ReleaseMouse(),
             wxMouseCaptureLostEvent, wxMouseCaptureChangedEvent
    */
    static wxWindow* GetCapture();

    /**
        Returns the caret() associated with the window.
    */
    wxCaret* GetCaret() const;

    /**
        Returns the character height for this window.
    */
    virtual int GetCharHeight() const;

    /**
        Returns the average character width for this window.
    */
    virtual int GetCharWidth() const;

    //@{
    /**
        Returns a reference to the list of the window's children. @c wxWindowList
        is a type-safe wxList-like class whose elements are of type @c wxWindow*.
    */
    wxWindowList& GetChildren();
    const wxWindowList& GetChildren() const;
    //@}

    /**
        Returns the default font and colours which are used by the control.

        This is useful if you want to use the same font or colour in your own control
        as in a standard control -- which is a much better idea than hard coding specific
        colours or fonts which might look completely out of place on the users
        system, especially if it uses themes.

        The @a variant parameter is only relevant under Mac currently and is
        ignore under other platforms. Under Mac, it will change the size of the
        returned font. See SetWindowVariant() for more about this.

        This static method is "overridden" in many derived classes and so calling,
        for example, wxButton::GetClassDefaultAttributes() will typically
        return the values appropriate for a button which will be normally different
        from those returned by, say, wxListCtrl::GetClassDefaultAttributes().

        The @c wxVisualAttributes structure has at least the fields
        @c font, @c colFg and @c colBg. All of them may be invalid
        if it was not possible to determine the default control appearance or,
        especially for the background colour, if the field doesn't make sense as is
        the case for @c colBg for the controls with themed background.

        @see InheritAttributes()
    */
    static wxVisualAttributes GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    //@{
    /**
        Returns the size of the window 'client area' in pixels.
        The client area is the area which may be drawn on by the programmer,
        excluding title bar, border, scrollbars, etc.
        Note that if this window is a top-level one and it is currently minimized, the
        return size is empty (both width and height are 0).

        @param width
            Receives the client width in pixels.
        @param height
            Receives the client height in pixels.

        @see GetSize(), GetVirtualSize()
    */
    void GetClientSize(int* width, int* height) const;
    wxSize GetClientSize() const;
    //@}

    /**
        Returns a pointer to the window's layout constraints, or @NULL if there are none.
    */
    wxLayoutConstraints* GetConstraints() const;

    /**
        Return the sizer that this window is a member of, if any, otherwise @NULL.
    */
    wxSizer* GetContainingSizer() const;

    /**
        Return the cursor associated with this window.

        @see SetCursor()
    */
    const wxCursor& GetCursor() const;

    /**
        Currently this is the same as calling
        wxWindow::GetClassDefaultAttributes(wxWindow::GetWindowVariant()).

        One advantage of using this function compared to the static version is that
        the call is automatically dispatched to the correct class (as usual with
        virtual functions) and you don't have to specify the class name explicitly.

        The other one is that in the future this function could return different
        results, for example it might return a different font for an "Ok" button
        than for a generic button if the users GUI is configured to show such buttons
        in bold font. Of course, the down side is that it is impossible to call this
        function without actually having an object to apply it to whereas the static
        version can be used without having to create an object first.
    */
    virtual wxVisualAttributes GetDefaultAttributes() const;

    /**
        Returns the associated drop target, which may be @NULL.

        @see SetDropTarget(), @ref overview_dnd
    */
    virtual wxDropTarget* GetDropTarget() const;

    /**
        Merges the window's best size into the min size and returns the result.
        This is the value used by sizers to determine the appropriate
        ammount of space to allocate for the widget.

        @see GetBestSize(), SetInitialSize(), @ref overview_windowsizing
    */
    wxSize GetEffectiveMinSize() const;

    /**
        Returns the event handler for this window.
        By default, the window is its own event handler.

        @see SetEventHandler(), PushEventHandler(),
             PopEventHandler(), wxEvtHandler::ProcessEvent, wxEvtHandler
    */
    wxEvtHandler* GetEventHandler() const;

    /**
        Returns the extra style bits for the window.
    */
    long GetExtraStyle() const;

    /**
        Returns the font for this window.

        @see SetFont()
    */
    wxFont GetFont() const;

    /**
        Returns the foreground colour of the window.

        @remarks The interpretation of foreground colour is open to
                 interpretation according to the window class; it may be
                 the text colour or other colour, or it may not be used at all.

        @see SetForegroundColour(), SetBackgroundColour(),
             GetBackgroundColour()
    */
    wxColour GetForegroundColour() const;

    /**
        Returns the grandparent of a window, or @NULL if there isn't one.
    */
    wxWindow* GetGrandParent() const;

    /**
        Returns the platform-specific handle of the physical window.
        Cast it to an appropriate handle, such as @b HWND for Windows,
        @b Widget for Motif, @b GtkWidget for GTK or @b WinHandle for PalmOS.
    */
    void* GetHandle() const;

    /**
        Gets the help text to be used as context-sensitive help for this window.
        Note that the text is actually stored by the current wxHelpProvider
        implementation, and not in the window object itself.

        @see SetHelpText(), GetHelpTextAtPoint(), wxHelpProvider
    */
    wxString GetHelpText() const;

    /**
        Gets the help text to be used as context-sensitive help for this window.
        This method should be overridden if the help message depends on the position
        inside the window, otherwise GetHelpText() can be used.

        @param point
            Coordinates of the mouse at the moment of help event emission.
        @param origin
            Help event origin, see also wxHelpEvent::GetOrigin.
    */
    virtual wxString GetHelpTextAtPoint(const wxPoint point,
                                        wxHelpEvent::Origin origin) const;

    /**
        Returns the identifier of the window.

        @remarks Each window has an integer identifier. If the application
                 has not provided one (or the default wxID_ANY) an unique
                 identifier with a negative value will be generated.

        @see SetId(), @ref overview_windowids
    */
    int GetId() const;

    /**
        Generic way of getting a label from any window, for
        identification purposes.

        @remarks The interpretation of this function differs from class to class.
                 For frames and dialogs, the value returned is the
                 title. For buttons or static text controls, it is the
                 button text. This function can be useful for
                 meta-programs (such as testing tools or special-needs
                 access programs) which need to identify windows by name.
    */
    virtual wxString GetLabel() const;

    /**
        Returns the maximum size of window's client area.

        This is an indication to the sizer layout mechanism that this is the maximum
        possible size as well as the upper bound on window's size settable using
        SetClientSize().

        @see GetMaxSize()
    */
    virtual wxSize GetMaxClientSize() const;

    /**
        Returns the maximum size of the window.

        This is an indication to the sizer layout mechanism that this is the maximum
        possible size as well as the upper bound on window's size settable using SetSize().

        @see GetMaxClientSize()
    */
    virtual wxSize GetMaxSize() const;

    /**
        Returns the minimum size of window's client area, an indication to the sizer
        layout mechanism that this is the minimum required size of its client area.

        It normally just returns the value set by SetMinClientSize(), but it can be
        overridden to do the calculation on demand.

        @see GetMinSize()
    */
    virtual wxSize GetMinClientSize() const;

    /**
        Returns the minimum size of the window, an indication to the sizer layout
        mechanism that this is the minimum required size.

        This method normally just returns the value set by SetMinSize(), but it
        can be overridden to do the calculation on demand.

        @see GetMinClientSize()
    */
    virtual wxSize GetMinSize() const;

    /**
        Returns the window's name.

        @remarks This name is not guaranteed to be unique; it is up to the
                 programmer to supply an appropriate name in the window
                 constructor or via SetName().

        @see SetName()
    */
    virtual wxString GetName() const;

    /**
        Returns the next window after this one among the parent children or @NULL
        if this window is the last child.

        @since 2.8.8

        @see GetPrevSibling()
    */
    wxWindow* GetNextSibling() const;

    /**
        Returns the parent of the window, or @NULL if there is no parent.
    */
    wxWindow* GetParent() const;

    /**
        This function shows a popup menu at the given position in this window and
        returns the selected id. It can be more convenient than the general purpose
        PopupMenu() function for simple menus proposing a choice in a list of
        strings to the user.

        @param menu
            The menu to show
        @param pos
            The position at which to show the menu in client coordinates

        @return The selected menu item id or wxID_NONE if none selected or an
                 error occurred.
    */
    int GetPopupMenuSelectionFromUser(wxMenu& menu, const wxPoint& pos);

    /**
        See the GetPopupMenuSelectionFromUser(wxMenu&, const wxPoint&) overload.
        This overload differs only because it takes two integers for the
        menu position instead of a wxPoint.
    */
    int GetPopupMenuSelectionFromUser(wxMenu& menu, int x, int y);

    /**
        This gets the position of the window in pixels, relative to the parent window
        for the child windows or relative to the display origin for the top level windows.

        @param x
            Receives the x position of the window if non-@NULL.
        @param y
            Receives the y position of the window if non-@NULL.

        @see GetScreenPosition()
    */
    void GetPosition(int* x, int* y) const;

    /**
        This gets the position of the window in pixels, relative to the parent window
        for the child windows or relative to the display origin for the top level windows.

        @see GetScreenPosition()
    */
    wxPoint GetPosition() const;

    /**
        Returns the previous window before this one among the parent children or @c
        @NULL if this window is the first child.

        @since 2.8.8

        @see GetNextSibling()
    */
    wxWindow* GetPrevSibling() const;

    /**
        Returns the position and size of the window as a wxRect object.

        @see GetScreenRect()
    */
    wxRect GetRect() const;

    /**
        Returns the window position in screen coordinates, whether the window is a
        child window or a top level one.

        @param x
            Receives the x position of the window on the screen if non-@NULL.
        @param y
            Receives the y position of the window on the screen if non-@NULL.

        @see GetPosition()
    */
    void GetScreenPosition(int* x, int* y) const;

    /**
        Returns the window position in screen coordinates, whether the window is a
        child window or a top level one.

        @see GetPosition()
    */
    wxPoint GetScreenPosition() const;

    /**
        Returns the position and size of the window on the screen as a wxRect object.

        @see GetRect()
    */
    wxRect GetScreenRect() const;

    /**
        Returns the built-in scrollbar position.

        @see See SetScrollbar()
    */
    virtual int GetScrollPos(int orientation) const;

    /**
        Returns the built-in scrollbar range.

        @see SetScrollbar()
    */
    virtual int GetScrollRange(int orientation) const;

    /**
        Returns the built-in scrollbar thumb size.

        @see SetScrollbar()
    */
    virtual int GetScrollThumb(int orientation) const;

    /**
        Returns the size of the entire window in pixels, including title bar, border,
        scrollbars, etc.

        Note that if this window is a top-level one and it is currently minimized, the
        returned size is the restored window size, not the size of the window icon.

        @param width
            Receives the window width.
        @param height
            Receives the window height.

        @see GetClientSize(), GetVirtualSize()
    */
    void GetSize(int* width, int* height) const;

    /**
        See the GetSize(int*,int*) overload for more info.
    */
    wxSize GetSize() const;

    /**
        Return the sizer associated with the window by a previous call to
        SetSizer() or @NULL.
    */
    wxSizer* GetSizer() const;

    /**
        Gets the dimensions of the string as it would be drawn on the
        window with the currently selected font.

        The text extent is returned in @a w and @a h pointers.

        @param string
            String whose extent is to be measured.
        @param w
            Return value for width.
        @param h
            Return value for height.
        @param descent
            Return value for descent (optional).
        @param externalLeading
            Return value for external leading (optional).
        @param font
            Font to use instead of the current window font (optional).
        @param use16
            If @true, string contains 16-bit characters. The default is @false.
    */
    virtual void GetTextExtent(const wxString& string, int* w, int* h,
                               int* descent = NULL,
                               int* externalLeading = NULL,
                               const wxFont* font = NULL,
                               bool use16 = false) const;

    /**
        Gets the dimensions of the string as it would be drawn on the
        window with the currently selected font.
    */
    wxSize  GetTextExtent(const wxString& string) const;

    /**
        Get the associated tooltip or @NULL if none.
    */
    wxToolTip* GetToolTip() const;

    /**
        Returns the region specifying which parts of the window have been damaged.
        Should only be called within an wxPaintEvent handler.

        @see wxRegion, wxRegionIterator
    */
    virtual wxRegion GetUpdateRegion() const;

    /**
        Returns a pointer to the current validator for the window, or @NULL if
        there is none.
    */
    virtual wxValidator* GetValidator();

    //@{
    /**
        This gets the virtual size of the window in pixels.
        By default it returns the client size of the window, but after a call to
        SetVirtualSize() it will return that size.

        @param width
            Receives the window virtual width.
        @param height
            Receives the window virtual height.
    */
    void GetVirtualSize(int* width, int* height) const;
    wxSize GetVirtualSize() const;
    //@}

    /**
        Returns the size of the left/right and top/bottom borders of this window in x
        and y components of the result respectively.
    */
    virtual wxSize GetWindowBorderSize() const;

    /**
        Gets the window style that was passed to the constructor or @b Create
        method. @b GetWindowStyle() is another name for the same function.
    */
    virtual long GetWindowStyleFlag() const;

    /**
        Returns the value previously passed to SetWindowVariant().
    */
    wxWindowVariant GetWindowVariant() const;

    /**
        This function will generate the appropriate call to
        Navigate() if the key event is one normally used for
        keyboard navigation and return @true in this case.

        @return Returns @true if the key pressed was for navigation and was
                handled, @false otherwise.

        @see Navigate()
    */
    bool HandleAsNavigationKey(const wxKeyEvent& event);

    /**
        Shorthand for:
        @code
        GetEventHandler()->SafelyProcessEvent(event);
        @endcode
    */
    bool HandleWindowEvent(wxEvent& event) const;

    /**
        Returns @true if this window has the current mouse capture.

        @see CaptureMouse(), ReleaseMouse(), wxMouseCaptureLostEvent,
             wxMouseCaptureChangedEvent
    */
    virtual bool HasCapture() const;

    /**
        Returns @true if the window has the given @a exFlag bit set in its
        extra styles.

        @see SetExtraStyle()
    */
    bool HasExtraStyle(int exFlag) const;

    /**
        Returns @true if the window has the given @a flag bit set.
    */
    bool HasFlag(int flag) const;

    /**
        Returns @true if the window (or in case of composite controls, its main
        child window) has focus.

        @see FindFocus()
    */
    virtual bool HasFocus() const;

    /**
        This method should be overridden to return @true if this window has
        multiple pages. All standard class with multiple pages such as
        wxNotebook, wxListbook and wxTreebook already override it to return @true
        and user-defined classes with similar behaviour should do it as well to
        allow the library to handle such windows appropriately.
    */
    virtual bool HasMultiplePages() const;

    /**
        Returns @true if this window has a scroll bar for this orientation.

        @param orient
            Orientation to check, either wxHORIZONTAL or wxVERTICAL.
    */
    bool HasScrollbar(int orient) const;

    /**
        Returns @true if this window background is transparent (as, for example,
        for wxStaticText) and should show the parent window background.

        This method is mostly used internally by the library itself and you normally
        shouldn't have to call it. You may, however, have to override it in your
        wxWindow-derived class to ensure that background is painted correctly.
    */
    virtual bool HasTransparentBackground();

    /**
        Equivalent to calling wxWindow::Show(@false).
    */
    bool Hide();

    /**
        This function hides a window, like Hide(), but using a special visual
        effect if possible.

        The parameters of this function are the same as for ShowWithEffect(),
        please see their description there.

        @since 2.9.0
    */
    virtual bool HideWithEffect(wxShowEffect effect,
                                unsigned timeout = 0);

    /**
        This function is (or should be, in case of custom controls) called during
        window creation to intelligently set up the window visual attributes, that is
        the font and the foreground and background colours.

        By "intelligently" the following is meant: by default, all windows use their
        own @ref GetClassDefaultAttributes() default attributes.
        However if some of the parents attributes are explicitly (that is, using
        SetFont() and not wxWindow::SetOwnFont) changed and if the corresponding
        attribute hadn't been explicitly set for this window itself, then this
        window takes the same value as used by the parent.
        In addition, if the window overrides ShouldInheritColours() to return @false,
        the colours will not be changed no matter what and only the font might.

        This rather complicated logic is necessary in order to accommodate the
        different usage scenarios. The most common one is when all default attributes
        are used and in this case, nothing should be inherited as in modern GUIs
        different controls use different fonts (and colours) than their siblings so
        they can't inherit the same value from the parent. However it was also deemed
        desirable to allow to simply change the attributes of all children at once by
        just changing the font or colour of their common parent, hence in this case we
        do inherit the parents attributes.
    */
    virtual void InheritAttributes();

    /**
        Sends an @c wxEVT_INIT_DIALOG event, whose handler usually transfers data
        to the dialog via validators.
    */
    virtual void InitDialog();

    /**
        Resets the cached best size value so it will be recalculated the next time it
        is needed.
    */
    void InvalidateBestSize();

    /**
        Returns @true if the window contents is double-buffered by the system, i.e. if
        any drawing done on the window is really done on a temporary backing surface
        and transferred to the screen all at once later.

        @see wxBufferedDC
    */
    virtual bool IsDoubleBuffered() const;

    /**
        Returns @true if the window is enabled, i.e. if it accepts user input,
        @false otherwise.

        Notice that this method can return @false even if this window itself hadn't
        been explicitly disabled when one of its parent windows is disabled.
        To get the intrinsic status of this window, use IsThisEnabled()

        @see Enable()
    */
    bool IsEnabled() const;

    //@{
    /**
        Returns @true if the given point or rectangle area has been exposed since the
        last repaint. Call this in an paint event handler to optimize redrawing by
        only redrawing those areas, which have been exposed.
    */
    bool IsExposed(int x, int y) const;
    const bool IsExposed(wxPoint amp;pt) const;
    const bool IsExposed(int x, int y, int w, int h) const;
    const bool IsExposed(wxRect amp;rect) const;
    //@}

    /**
        Returns @true if the window is currently frozen by a call to Freeze().

        @see Freeze(), Thaw()
    */
    bool IsFrozen() const;

    /**
        Returns @true if the window is retained, @false otherwise.

        @remarks Retained windows are only available on X platforms.
    */
    virtual bool IsRetained() const;

    /**
        Return whether a scrollbar is always shown.

        @param orient
            Orientation to check, either wxHORIZONTAL or wxVERTICAL.

        @see AlwaysShowScrollbars()
    */
    virtual bool IsScrollbarAlwaysShown(int orient) const;

    /**
        Returns @true if the window is shown, @false if it has been hidden.

        @see IsShownOnScreen()
    */
    virtual bool IsShown() const;

    /**
        Returns @true if the window is physically visible on the screen, i.e. it
        is shown and all its parents up to the toplevel window are shown as well.

        @see IsShown()
    */
    virtual bool IsShownOnScreen() const;

    /**
        Returns @true if this window is intrinsically enabled, @false otherwise,
        i.e. if @ref Enable() Enable(@false) had been called. This method is
        mostly used for wxWidgets itself, user code should normally use
        IsEnabled() instead.
    */
    bool IsThisEnabled() const;

    /**
        Returns @true if the given window is a top-level one. Currently all frames and
        dialogs are considered to be top-level windows (even if they have a parent
        window).
    */
    virtual bool IsTopLevel() const;

    /**
        Invokes the constraint-based layout algorithm or the sizer-based algorithm
        for this window.

        See SetAutoLayout(): when auto layout is on, this function gets called automatically
        when the window is resized.

        @see @ref overview_windowsizing
    */
    void Layout();

    /**
        Lowers the window to the bottom of the window hierarchy (Z-order).

        @see Raise()
    */
    virtual void Lower();

    /**
        Disables all other windows in the application so that
        the user can only interact with this window.

        @param flag
            If @true, this call disables all other windows in the application so that
            the user can only interact with this window. If @false, the effect is
            reversed.
    */
    virtual void MakeModal(bool flag);

    /**
        Moves the window to the given position.

        @param x
            Required x position.
        @param y
            Required y position.

        @remarks Implementations of SetSize can also implicitly implement the
                 Move() function, which is defined in the base wxWindow class as the call:
                 @code
                 SetSize(x, y, wxDefaultCoord, wxDefaultCoord, wxSIZE_USE_EXISTING);
                 @endcode

        @see SetSize()
    */
    void Move(int x, int y);

    /**
        Moves the window to the given position.

        @param pt
            wxPoint object representing the position.

        @remarks Implementations of SetSize() can also implicitly implement the
                 Move() function, which is defined in the base wxWindow class as the call:
                 @code
                 SetSize(x, y, wxDefaultCoord, wxDefaultCoord, wxSIZE_USE_EXISTING);
                 @endcode

        @see SetSize()
    */
    void Move(const wxPoint& pt);

    /**
        Moves this window in the tab navigation order after the specified @e win.
        This means that when the user presses @c TAB key on that other window,
        the focus switches to this window.

        Default tab order is the same as creation order, this function and
        MoveBeforeInTabOrder() allow to change
        it after creating all the windows.

        @param win
            A sibling of this window which should precede it in tab order,
            must not be @NULL
    */
    void MoveAfterInTabOrder(wxWindow* win);

    /**
        Same as MoveAfterInTabOrder() except that it inserts this window just
        before @a win instead of putting it right after it.
    */
    void MoveBeforeInTabOrder(wxWindow* win);

    /**
        Performs a keyboard navigation action starting from this window.
        This method is equivalent to calling NavigateIn() method on the
        parent window.

        @param flags
            A combination of wxNavigationKeyEvent::IsForward and
            wxNavigationKeyEvent::WinChange.

        @return Returns @true if the focus was moved to another window or @false
                if nothing changed.

        @remarks You may wish to call this from a text control custom keypress
                 handler to do the default navigation behaviour for the
                 tab key, since the standard default behaviour for a
                 multiline text control with the wxTE_PROCESS_TAB style
                 is to insert a tab and not navigate to the next
                 control. See also wxNavigationKeyEvent and
                 HandleAsNavigationKey.
    */
    bool Navigate(int flags = wxNavigationKeyEvent::IsForward);

    /**
        Performs a keyboard navigation action inside this window.
        See Navigate() for more information.
    */
    bool NavigateIn(int flags = wxNavigationKeyEvent::IsForward);

    /**
        Create a new ID or range of IDs that are not currently in use.
        The IDs will be reserved until assigned to a wxWindow ID
        or unreserved with UnreserveControlId().

        See @ref overview_windowids for more information.

        @param count
            The number of sequential IDs to reserve.

        @return Returns the ID or the first ID of the range, or wxID_NONE if the
                specified number of identifiers couldn't be allocated.

        @see UnreserveControlId(), wxIdManager,
             @ref overview_windowids
    */
    static wxWindowID NewControlId(int count = 1);

    /**
        This virtual function is normally only used internally, but
        sometimes an application may need it to implement functionality
        that should not be disabled by an application defining an OnIdle
        handler in a derived class.

        This function may be used to do delayed painting, for example,
        and most implementations call UpdateWindowUI()
        in order to send update events to the window in idle time.
    */
    virtual void OnInternalIdle();

    /**
        Same as #ScrollLines (-1).
    */
    bool LineUp();

    /**
        Same as #ScrollLines (1).
    */
    bool LineDown();

    /**
        Same as #ScrollPages (-1).
    */
    bool PageUp();

    /**
        Same as #ScrollPages (1).
    */
    bool PageDown();


    /**
        Removes and returns the top-most event handler on the event handler stack.

        @param deleteHandler
            If this is @true, the handler will be deleted after it is removed.
            The default value is @false.

        @see SetEventHandler(), GetEventHandler(),
             PushEventHandler(), wxEvtHandler::ProcessEvent, wxEvtHandler
    */
    wxEvtHandler* PopEventHandler(bool deleteHandler = false);

    //@{
    /**
        Pops up the given menu at the specified coordinates, relative to this
        window, and returns control when the user has dismissed the menu.

        If a menu item is selected, the corresponding menu event is generated and will be
        processed as usually. If the coordinates are not specified, current mouse
        cursor position is used.

        @param menu
            Menu to pop up.
        @param pos
            The position where the menu will appear.
        @param x
            Required x position for the menu to appear.
        @param y
            Required y position for the menu to appear.

        @remarks Just before the menu is popped up, wxMenu::UpdateUI is called to
                 ensure that the menu items are in the correct state.
                 The menu does not get deleted by the window.
                 It is recommended to not explicitly specify coordinates when
                 calling PopupMenu in response to mouse click, because some of
                 the ports (namely, wxGTK) can do a better job of positioning
                 the menu in that case.

        @see wxMenu
    */
    bool PopupMenu(wxMenu* menu,
                   const wxPoint& pos = wxDefaultPosition);
    bool PopupMenu(wxMenu* menu, int x, int y);
    //@}

    /**
        Posts a size event to the window.

        This is the same as SendSizeEvent() with @c wxSEND_EVENT_POST argument.
     */
    void PostSizeEvent();

    /**
        Posts a size event to the parent of this window.

        This is the same as SendSizeEventToParent() with @c wxSEND_EVENT_POST
        argument.
     */
    void PostSizeEventToParent();

    /**
        Pushes this event handler onto the event stack for the window.

        @param handler
            Specifies the handler to be pushed.

        @remarks An event handler is an object that is capable of processing the
                 events sent to a window. By default, the window is its
                 own event handler, but an application may wish to
                 substitute another, for example to allow central
                 implementation of event-handling for a variety of
                 different window classes.
                 wxWindow::PushEventHandler allows an application to set up a
                 chain of event handlers, where an event not handled by one event
                 handler is handed to the next one in the chain.
                 Use wxWindow::PopEventHandler to remove the event handler.

        @see SetEventHandler(), GetEventHandler(),
             PopEventHandler(), wxEvtHandler::ProcessEvent, wxEvtHandler
    */
    void PushEventHandler(wxEvtHandler* handler);

    /**
        Raises the window to the top of the window hierarchy (Z-order).
        In current version of wxWidgets this works both for managed and child windows.

        @see Lower()
    */
    virtual void Raise();

    /**
        Causes this window, and all of its children recursively (except under wxGTK1
        where this is not implemented), to be repainted. Note that repainting doesn't
        happen immediately but only during the next event loop iteration, if you need
        to update the window immediately you should use Update() instead.

        @param eraseBackground
            If @true, the background will be erased.
        @param rect
            If non-@NULL, only the given rectangle will be treated as damaged.

        @see RefreshRect()
    */
    virtual void Refresh(bool eraseBackground = true,
                         const wxRect* rect = NULL);

    /**
        Redraws the contents of the given rectangle: only the area inside it will be
        repainted.

        This is the same as Refresh() but has a nicer syntax as it can be called
        with a temporary wxRect object as argument like this @c RefreshRect(wxRect(x, y, w, h)).
    */
    void RefreshRect(const wxRect& rect, bool eraseBackground = true);

    /**
        Registers a system wide hotkey. Every time the user presses the hotkey
        registered here, this window will receive a hotkey event.

        It will receive the event even if the application is in the background
        and does not have the input focus because the user is working with some
        other application.

        @param hotkeyId
            Numeric identifier of the hotkey. For applications this must be between 0
            and 0xBFFF. If this function is called from a shared DLL, it must be a
            system wide unique identifier between 0xC000 and 0xFFFF.
            This is a MSW specific detail.
        @param modifiers
            A bitwise combination of wxMOD_SHIFT, wxMOD_CONTROL, wxMOD_ALT
            or wxMOD_WIN specifying the modifier keys that have to be pressed along
            with the key.
        @param virtualKeyCode
            The virtual key code of the hotkey.

        @return @true if the hotkey was registered successfully. @false if some
                 other application already registered a hotkey with this
                 modifier/virtualKeyCode combination.

        @remarks Use EVT_HOTKEY(hotkeyId, fnc) in the event table to capture the
                 event. This function is currently only implemented
                 under Windows. It is used in the Windows CE port for
                 detecting hardware button presses.

        @see UnregisterHotKey()
    */
    bool RegisterHotKey(int hotkeyId, int modifiers,
                        int virtualKeyCode);

    /**
        Releases mouse input captured with CaptureMouse().

        @see CaptureMouse(), HasCapture(), ReleaseMouse(),
             wxMouseCaptureLostEvent, wxMouseCaptureChangedEvent
    */
    void ReleaseMouse();

    /**
        Removes a child window.

        This is called automatically by window deletion functions so should not
        be required by the application programmer.
        Notice that this function is mostly internal to wxWidgets and shouldn't be
        called by the user code.

        @param child
            Child window to remove.
    */
    virtual void RemoveChild(wxWindow* child);

    /**
        Find the given @a handler in the windows event handler chain and remove
        (but not delete) it from it.

        @param handler
            The event handler to remove, must be non-@NULL and
            must be present in this windows event handlers chain

        @return Returns @true if it was found and @false otherwise (this also
                results in an assert failure so this function should
                only be called when the handler is supposed to be there).

        @see PushEventHandler(), PopEventHandler()
    */
    bool RemoveEventHandler(wxEvtHandler* handler);

    /**
        Reparents the window, i.e the window will be removed from its
        current parent window (e.g. a non-standard toolbar in a wxFrame)
        and then re-inserted into another.

        @param newParent
            New parent.
    */
    virtual bool Reparent(wxWindow* newParent);

    //@{
    /**
        Converts from screen to client window coordinates.

        @param x
            Stores the screen x coordinate and receives the client x coordinate.
        @param y
            Stores the screen x coordinate and receives the client x coordinate.
        @param pt
            The screen position for the second form of the function.
    */
    virtual void ScreenToClient(int* x, int* y) const;
    virtual wxPoint ScreenToClient(const wxPoint& pt) const;
    //@}

    /**
        Scrolls the window by the given number of lines down (if @a lines is
        positive) or up.

        @return Returns @true if the window was scrolled, @false if it was already
                on top/bottom and nothing was done.

        @remarks This function is currently only implemented under MSW and
                 wxTextCtrl under wxGTK (it also works for wxScrolled classes
                 under all platforms).

        @see ScrollPages()
    */
    virtual bool ScrollLines(int lines);

    /**
        Scrolls the window by the given number of pages down (if @a pages is
        positive) or up.

        @return Returns @true if the window was scrolled, @false if it was already
                on top/bottom and nothing was done.

        @remarks This function is currently only implemented under MSW and wxGTK.

        @see ScrollLines()
    */
    virtual bool ScrollPages(int pages);

    /**
        Physically scrolls the pixels in the window and move child windows accordingly.

        @param dx
            Amount to scroll horizontally.
        @param dy
            Amount to scroll vertically.
        @param rect
            Rectangle to scroll, if it is @NULL, the whole window is
            scrolled (this is always the case under wxGTK which doesn't support this
            parameter)

        @remarks Note that you can often use wxScrolled instead of using this
                 function directly.
    */
    virtual void ScrollWindow(int dx, int dy,
                              const wxRect* rect = NULL);

    /**
        This function sends a dummy @ref overview_wxsizeevent "size event" to
        the window allowing it to re-layout its children positions.

        It is sometimes useful to call this function after adding or deleting a
        children after the frame creation or if a child size changes. Note that
        if the frame is using either sizers or constraints for the children
        layout, it is enough to call wxWindow::Layout() directly and this
        function should not be used in this case.

        If @a flags includes @c wxSEND_EVENT_POST value, this function posts
        the event, i.e. schedules it for later processing, instead of
        dispatching it directly. You can also use PostSizeEvent() as a more
        readable equivalent of calling this function with this flag.

        @param flags
            May include @c wxSEND_EVENT_POST. Default value is 0.
    */
    virtual void SendSizeEvent(int flags = 0);

    /**
        Safe wrapper for GetParent()->SendSizeEvent().

        This function simply checks that the window has a valid parent which is
        not in process of being deleted and calls SendSizeEvent() on it. It is
        used internally by windows such as toolbars changes to whose state
        should result in parent re-layout (e.g. when a toolbar is added to the
        top of the window, all the other windows must be shifted down).

        @see PostSizeEventToParent()

        @param flags
            See description of this parameter in SendSizeEvent() documentation.
     */
    void SendSizeEventToParent(int flags = 0);

    /**
        Sets the accelerator table for this window. See wxAcceleratorTable.
    */
    virtual void SetAcceleratorTable(const wxAcceleratorTable& accel);

    /**
        Sets the accessible for this window. Any existing accessible for this window
        will be deleted first, if not identical to @e accessible.
        See also wxAccessible.
    */
    void SetAccessible(wxAccessible* accessible);

    /**
        Determines whether the Layout() function will be called automatically
        when the window is resized. Please note that this only happens for the
        windows usually used to contain children, namely wxPanel and wxTopLevelWindow
        (and the classes deriving from them).

        This method is called implicitly by SetSizer() but if you use SetConstraints()
        you should call it manually or otherwise the window layout won't be correctly
        updated when its size changes.

        @param autoLayout
            Set this to @true if you wish the Layout function to be
            called automatically when the window is resized.

        @see SetConstraints()
    */
    void SetAutoLayout(bool autoLayout);

    /**
        Sets the background colour of the window.
        Please see InheritAttributes() for explanation of the difference between
        this method and SetOwnBackgroundColour().

        @param colour
            The colour to be used as the background colour, pass
            wxNullColour to reset to the default colour.

        @remarks The background colour is usually painted by the default
                 wxEraseEvent event handler function under Windows and
                 automatically under GTK.
                 Note that setting the background colour does not cause an
                 immediate refresh, so you may wish to call wxWindow::ClearBackground
                 or wxWindow::Refresh after calling this function.
                 Using this function will disable attempts to use themes for
                 this window, if the system supports them. Use with care since
                 usually the themes represent the appearance chosen by the user
                 to be used for all applications on the system.

        @see GetBackgroundColour(), SetForegroundColour(),
             GetForegroundColour(), ClearBackground(),
             Refresh(), wxEraseEvent
    */
    virtual bool SetBackgroundColour(const wxColour& colour);

    /**
        Sets the background style of the window. see GetBackgroundStyle() for
        the description of the possible style values.

        @see SetBackgroundColour(), GetForegroundColour(),
             SetTransparent()
    */
    virtual void SetBackgroundStyle(wxBackgroundStyle style);

    /**
        This method is only implemented by ports which have support for
        native TAB traversal (such as GTK+ 2.0).

        It is called by wxWidgets' container control code to give the native
        system a hint when doing TAB traversal. A call to this does not disable
        or change the effect of programmatically calling SetFocus().

        @see wxFocusEvent, wxPanel::SetFocus, wxPanel::SetFocusIgnoringChildren
    */
    virtual void SetCanFocus(bool canFocus);

    /**
        Sets the caret() associated with the window.
    */
    void SetCaret(wxCaret* caret);

    //@{
    /**
        This sets the size of the window client area in pixels.

        Using this function to size a window tends to be more device-independent
        than SetSize(), since the application need not worry about what dimensions
        the border or title bar have when trying to fit the window around panel
        items, for example.

        @param width
            The required client area width.
        @param height
            The required client area height.
        @param size
            The required client size.
    */
    virtual void SetClientSize(int width, int height);
    virtual void SetClientSize(const wxSize& size);
    //@}

    /**
        Sets the window to have the given layout constraints. The window
        will then own the object, and will take care of its deletion.
        If an existing layout constraints object is already owned by the
        window, it will be deleted.

        @param constraints
            The constraints to set. Pass @NULL to disassociate and delete the window's
            constraints.

        @remarks You must call SetAutoLayout() to tell a window to use
                 the constraints automatically in OnSize; otherwise, you
                 must override OnSize and call Layout() explicitly. When
                 setting both a wxLayoutConstraints and a wxSizer, only
                 the sizer will have effect.
    */
    void SetConstraints(wxLayoutConstraints* constraints);

    /**
        This normally does not need to be called by user code.
        It is called when a window is added to a sizer, and is used so the window
        can remove itself from the sizer when it is destroyed.
    */
    void SetContainingSizer(wxSizer* sizer);

    /**
        Sets the window's cursor. Notice that the window cursor also sets it for the
        children of the window implicitly.

        The @a cursor may be @c wxNullCursor in which case the window cursor will
        be reset back to default.

        @param cursor
            Specifies the cursor that the window should normally display.

        @see ::wxSetCursor, wxCursor
    */
    virtual void SetCursor(const wxCursor& cursor);

    /**
        Associates a drop target with this window.
        If the window already has a drop target, it is deleted.

        @see GetDropTarget(), @ref overview_dnd
    */
    virtual void SetDropTarget(wxDropTarget* target);

    /**
        Sets the event handler for this window.

        @param handler
            Specifies the handler to be set.

        @remarks An event handler is an object that is capable of processing the
                 events sent to a window. By default, the window is its
                 own event handler, but an application may wish to
                 substitute another, for example to allow central
                 implementation of event-handling for a variety of
                 different window classes.
                 It is usually better to use wxWindow::PushEventHandler since
                 this sets up a chain of event handlers, where an event not
                handled by one event handler is handed to the next one in the chain.

        @see GetEventHandler(), PushEventHandler(),
             PopEventHandler(), wxEvtHandler::ProcessEvent, wxEvtHandler
    */
    void SetEventHandler(wxEvtHandler* handler);

    /**
        Sets the extra style bits for the window.
        The currently defined extra style bits are reported in the class
        description.
    */
    virtual void SetExtraStyle(long exStyle);

    /**
        This sets the window to receive keyboard input.

        @see HasFocus(), wxFocusEvent, wxPanel::SetFocus,
             wxPanel::SetFocusIgnoringChildren
    */
    virtual void SetFocus();

    /**
        This function is called by wxWidgets keyboard navigation code when the user
        gives the focus to this window from keyboard (e.g. using @c TAB key).

        By default this method simply calls SetFocus() but
        can be overridden to do something in addition to this in the derived classes.
    */
    virtual void SetFocusFromKbd();

    /**
        Sets the font for this window. This function should not be called for the
        parent window if you don't want its font to be inherited by its children,
        use SetOwnFont() instead in this case and see InheritAttributes() for more
        explanations.

        Please notice that the given font is not automatically used for
        wxPaintDC objects associated with this window, you need to
        call wxDC::SetFont too. However this font is used by
        any standard controls for drawing their text as well as by
        GetTextExtent().

        @param font
            Font to associate with this window, pass
            wxNullFont to reset to the default font.

        @return @true if the want was really changed, @false if it was already set
                to this  font and so nothing was done.

        @see GetFont(), InheritAttributes()
    */
    virtual bool SetFont(const wxFont& font);

    /**
        Sets the foreground colour of the window.
        Please see InheritAttributes() for explanation of the difference between
        this method and SetOwnForegroundColour().

        @param colour
            The colour to be used as the foreground colour, pass
            wxNullColour to reset to the default colour.

        @remarks The interpretation of foreground colour is open to
                 interpretation according to the window class; it may be
                 the text colour or other colour, or it may not be used at all.

        @see GetForegroundColour(), SetBackgroundColour(),
             GetBackgroundColour(), ShouldInheritColours()
    */
    virtual void SetForegroundColour(const wxColour& colour);

    /**
        Sets the help text to be used as context-sensitive help for this window.
        Note that the text is actually stored by the current wxHelpProvider
        implementation, and not in the window object itself.

        @see GetHelpText(), wxHelpProvider::AddHelp()
    */
    void SetHelpText(const wxString& helpText);

    /**
        Sets the identifier of the window.

        @remarks Each window has an integer identifier. If the application has
                 not provided one, an identifier will be generated.
                 Normally, the identifier should be provided on creation
                 and should not be modified subsequently.

        @see GetId(), @ref overview_windowids
    */
    void SetId(int id);

    /**
        Sets the initial window size if none is given (i.e. at least one of the
        components of the size passed to ctor/Create() is wxDefaultCoord).
    */
    virtual void SetInitialBestSize(const wxSize& size);

    /**
        A @e smart SetSize that will fill in default size components with the
        window's @e best size values.

        Also sets the window's minsize to the value passed in for use with sizers.
        This means that if a full or partial size is passed to this function then
        the sizers will use that size instead of the results of GetBestSize() to
        determine the minimum needs of the window for layout.

        Most controls will use this to set their initial size, and their min
        size to the passed in value (if any.)

        @see SetSize(), GetBestSize(), GetEffectiveMinSize(),
             @ref overview_windowsizing
    */
    void SetInitialSize(const wxSize& size = wxDefaultSize);

    /**
        Sets the window's label.

        @param label
            The window label.

        @see GetLabel()
    */
    virtual void SetLabel(const wxString& label);

    /**
        Sets the maximum client size of the window, to indicate to the sizer
        layout mechanism that this is the maximum possible size of its client area.

        @see SetMaxSize()
    */
    virtual void SetMaxClientSize(const wxSize& size);

    /**
        Sets the maximum size of the window, to indicate to the sizer layout mechanism
        that this is the maximum possible size.

        @see SetMaxClientSize()
    */
    virtual void SetMaxSize(const wxSize& size);

    /**
        Sets the minimum client size of the window, to indicate to the sizer
        layout mechanism that this is the minimum required size of window's client
        area.

        You may need to call this if you change the window size after
        construction and before adding to its parent sizer.

        Note, that just as with SetMinSize(), calling this method doesn't
        prevent the program from explicitly making the window smaller than the
        specified size.

        @see SetMinSize()
    */
    virtual void SetMinClientSize(const wxSize& size);

    /**
        Sets the minimum size of the window, to indicate to the sizer layout
        mechanism that this is the minimum required size.

        You may need to call this if you change the window size after
        construction and before adding to its parent sizer.

        Notice that calling this method doesn't prevent the program from making
        the window explicitly smaller than the specified size by calling
        SetSize(), it just ensures that it won't become smaller than this size
        during the automatic layout.

        @see SetMinClientSize()
    */
    virtual void SetMinSize(const wxSize& size);

    /**
        Sets the window's name.

        @param name
            A name to set for the window.

        @see GetName()
    */
    virtual void SetName(const wxString& name);

    /**
        Sets the background colour of the window but prevents it from being inherited
        by the children of this window.

        @see SetBackgroundColour(), InheritAttributes()
    */
    void SetOwnBackgroundColour(const wxColour& colour);

    /**
        Sets the font of the window but prevents it from being inherited by the
        children of this window.

        @see SetFont(), InheritAttributes()
    */
    void SetOwnFont(const wxFont& font);

    /**
        Sets the foreground colour of the window but prevents it from being inherited
        by the children of this window.

        @see SetForegroundColour(), InheritAttributes()
    */
    void SetOwnForegroundColour(const wxColour& colour);

    /**
        @deprecated use wxDC::SetPalette instead.
    */
    virtual void SetPalette(wxPalette* palette);

    /**
        Sets the position of one of the built-in scrollbars.

        @param orientation
            Determines the scrollbar whose position is to be set.
            May be wxHORIZONTAL or wxVERTICAL.
        @param pos
            Position in scroll units.
        @param refresh
            @true to redraw the scrollbar, @false otherwise.

        @remarks This function does not directly affect the contents of the
                 window: it is up to the application to take note of
                 scrollbar attributes and redraw contents accordingly.

        @see SetScrollbar(), GetScrollPos(), GetScrollThumb(), wxScrollBar,
             wxScrolled
    */
    virtual void SetScrollPos(int orientation, int pos,
                              bool refresh = true);

    /**
        Sets the scrollbar properties of a built-in scrollbar.

        @param orientation
            Determines the scrollbar whose page size is to be set.
            May be wxHORIZONTAL or wxVERTICAL.
        @param position
            The position of the scrollbar in scroll units.
        @param thumbSize
            The size of the thumb, or visible portion of the scrollbar, in scroll units.
        @param range
            The maximum position of the scrollbar.
        @param refresh
            @true to redraw the scrollbar, @false otherwise.

        @remarks
            Let's say you wish to display 50 lines of text, using the same font.
            The window is sized so that you can only see 16 lines at a time.
            You would use:
            @code
            SetScrollbar(wxVERTICAL, 0, 16, 50);
            @endcode
            Note that with the window at this size, the thumb position can never
            go above 50 minus 16, or 34. You can determine how many lines are
            currently visible by dividing the current view size by the character
            height in pixels.
            When defining your own scrollbar behaviour, you will always need
            to recalculate the scrollbar settings when the window size changes.
            You could therefore put your scrollbar calculations and SetScrollbar
            call into a function named AdjustScrollbars, which can be called
            initially and also from your wxSizeEvent handler function.

        @see @ref overview_scrolling, wxScrollBar, wxScrolled, wxScrollWinEvent
    */
    virtual void SetScrollbar(int orientation, int position,
                              int thumbSize, int range,
                              bool refresh = true);

    /**
        Sets the size of the window in pixels.

        @param x
            Required x position in pixels, or wxDefaultCoord to indicate that the
            existing value should be used.
        @param y
            Required y position in pixels, or wxDefaultCoord to indicate that the
            existing value should be used.
        @param width
            Required width in pixels, or wxDefaultCoord to indicate that the existing
            value should be used.
        @param height
            Required height position in pixels, or wxDefaultCoord to indicate that the
            existing value should be used.
        @param sizeFlags
            Indicates the interpretation of other parameters.
            It is a bit list of the following:
            - @c wxSIZE_AUTO_WIDTH: a wxDefaultCoord width value is taken to indicate
                                    a wxWidgets-supplied default width.
            - @c wxSIZE_AUTO_HEIGHT: a wxDefaultCoord height value is taken to indicate
                                     a wxWidgets-supplied default height.
            - @c wxSIZE_AUTO: wxDefaultCoord size values are taken to indicate
                              a wxWidgets-supplied default size.
            - @c wxSIZE_USE_EXISTING: existing dimensions should be used
                                      if wxDefaultCoord values are supplied.
            - @c wxSIZE_ALLOW_MINUS_ONE: allow negative dimensions (i.e. value of
                                         wxDefaultCoord) to be interpreted as real
                                         dimensions, not default values.
            - @c wxSIZE_FORCE: normally, if the position and the size of the window are
                               already the same as the parameters of this function,
                               nothing is done. but with this flag a window resize may
                               be forced even in this case (supported in wx 2.6.2 and
                               later and only implemented for MSW and ignored elsewhere
                               currently).

        @remarks This overload sets the position and optionally size, of the window.
                 Parameters may be wxDefaultCoord to indicate either that a default
                 should be supplied by wxWidgets, or that the current value of the
                 dimension should be used.

        @see Move()
    */
    virtual void SetSize(int x, int y, int width, int height,
                         int sizeFlags = wxSIZE_AUTO);

    //@{
    /**
        Sets the size of the window in pixels.
        The size is specified using a wxRect, wxSize or by a couple of @c int objects.

        @remarks This form must be used with non-default width and height values.

        @see Move()
    */
    virtual void SetSize(const wxRect& rect);
    virtual void SetSize(const wxSize& size);
    virtual void SetSize(int width, int height);
    //@}

    /**
        Sets the window to have the given layout sizer.
        The window will then own the object, and will take care of its deletion.
        If an existing layout constraints object is already owned by the
        window, it will be deleted if the deleteOld parameter is @true.

        Note that this function will also call SetAutoLayout() implicitly with @true
        parameter if the @a sizer is non-@NULL and @false otherwise.

        @param sizer
            The sizer to set. Pass @NULL to disassociate and conditionally delete
            the window's sizer. See below.
        @param deleteOld
            If @true (the default), this will delete any pre-existing sizer.
            Pass @false if you wish to handle deleting the old sizer yourself.

        @remarks SetSizer enables and disables Layout automatically.
    */
    void SetSizer(wxSizer* sizer, bool deleteOld = true);

    /**
        This method calls SetSizer() and then wxSizer::SetSizeHints which sets the initial
        window size to the size needed to accommodate all sizer elements and sets the
        size hints which, if this window is a top level one, prevent the user from
        resizing it to be less than this minimial size.
    */
    void SetSizerAndFit(wxSizer* sizer, bool deleteOld = true);

    /**
        This function tells a window if it should use the system's "theme" code
        to draw the windows' background instead if its own background drawing
        code. This does not always have any effect since the underlying platform
        obviously needs to support the notion of themes in user defined windows.
        One such platform is GTK+ where windows can have (very colourful) backgrounds
        defined by a user's selected theme.

        Dialogs, notebook pages and the status bar have this flag set to @true
        by default so that the default look and feel is simulated best.
    */
    virtual void SetThemeEnabled(bool enable);

    //@{
    /**
        Attach a tooltip to the window.

        wxToolTip pointer can be @NULL in the overload taking the pointer,
        meaning to unset any existing tooltips, however UnsetToolTip() provides
        a more readable alternative to this operation.

        Notice that these methods are always available, even if wxWidgets was
        compiled with @c wxUSE_TOOLTIPS set to 0, but don't do anything in this
        case.

        @see GetToolTip(), wxToolTip
    */
    void SetToolTip(const wxString& tip);
    void SetToolTip(wxToolTip* tip);
    //@}

    /**
        Set the transparency of the window. If the system supports transparent windows,
        returns @true, otherwise returns @false and the window remains fully opaque.
        See also CanSetTransparent().

        The parameter @a alpha is in the range 0..255 where 0 corresponds to a
        fully transparent window and 255 to the fully opaque one. The constants
        @c wxIMAGE_ALPHA_TRANSPARENT and @c wxIMAGE_ALPHA_OPAQUE can be used.
    */
    virtual bool SetTransparent(wxByte alpha);

    /**
        Deletes the current validator (if any) and sets the window validator, having
        called wxValidator::Clone to create a new validator of this type.
    */
    virtual void SetValidator(const wxValidator& validator);

    //@{
    /**
        Sets the virtual size of the window in pixels.
    */
    void SetVirtualSize(int width, int height);
    void SetVirtualSize(const wxSize& size);
    //@}

    /**
        Identical to SetWindowStyleFlag().
    */
    void SetWindowStyle(long style);

    /**
        Sets the style of the window. Please note that some styles cannot be changed
        after the window creation and that Refresh() might need to be be called
        after changing the others for the change to take place immediately.

        See @ref overview_windowstyles "Window styles" for more information about flags.

        @see GetWindowStyleFlag()
    */
    virtual void SetWindowStyleFlag(long style);

    /**
        This function can be called under all platforms but only does anything under
        Mac OS X 10.3+ currently. Under this system, each of the standard control can
        exist in several sizes which correspond to the elements of wxWindowVariant enum.

        By default the controls use the normal size, of course, but this function can
        be used to change this.
    */
    void SetWindowVariant(wxWindowVariant variant);

    /**
        Return @true from here to allow the colours of this window to be changed by
        InheritAttributes(), returning @false forbids inheriting them from the parent window.

        The base class version returns @false, but this method is overridden in
        wxControl where it returns @true.
    */
    virtual bool ShouldInheritColours() const;

    /**
        Shows or hides the window. You may need to call Raise()
        for a top level window if you want to bring it to top, although this is not
        needed if Show() is called immediately after the frame creation.

        @param show
            If @true displays the window. Otherwise, hides it.

        @return @true if the window has been shown or hidden or @false if nothing
                 was done because it already was in the requested state.

        @see IsShown(), Hide(), wxRadioBox::Show, wxShowEvent.
    */
    virtual bool Show(bool show = true);

    /**
        This function shows a window, like Show(), but using a special visual
        effect if possible.

        @param effect
            The effect to use.

        @param timeout
            The @a timeout parameter specifies the time of the animation, in
            milliseconds. If the default value of 0 is used, the default
            animation time for the current platform is used.

        @note Currently this function is only implemented in wxMSW and does the
              same thing as Show() in the other ports.

        @since 2.9.0

        @see HideWithEffect()
    */
    virtual bool ShowWithEffect(wxShowEffect effect,
                                unsigned timeout = 0);

    /**
        Reenables window updating after a previous call to Freeze().

        To really thaw the control, it must be called exactly the same number
        of times as Freeze().

        If the window has any children, they are recursively thawn too.

        @see wxWindowUpdateLocker, Freeze(), IsFrozen()
    */
    void Thaw();

    /**
        Turns the given @a flag on if it's currently turned off and vice versa.
        This function cannot be used if the value of the flag is 0 (which is often
        the case for default flags).

        Also, please notice that not all styles can be changed after the control
        creation.

        @return Returns @true if the style was turned on by this function, @false
                 if it was switched off.

        @see SetWindowStyleFlag(), HasFlag()
    */
    bool ToggleWindowStyle(int flag);

    /**
        Transfers values from child controls to data areas specified by their
        validators. Returns @false if a transfer failed.

        If the window has @c wxWS_EX_VALIDATE_RECURSIVELY extra style flag set,
        the method will also call TransferDataFromWindow() of all child windows.

        @see TransferDataToWindow(), wxValidator, Validate()
    */
    virtual bool TransferDataFromWindow();

    /**
        Transfers values to child controls from data areas specified by their
        validators.

        If the window has @c wxWS_EX_VALIDATE_RECURSIVELY extra style flag set,
        the method will also call TransferDataToWindow() of all child windows.

        @return Returns @false if a transfer failed.

        @see TransferDataFromWindow(), wxValidator, Validate()
    */
    virtual bool TransferDataToWindow();

    /**
        Unregisters a system wide hotkey.

        @param hotkeyId
            Numeric identifier of the hotkey. Must be the same id that was passed to
            RegisterHotKey().

        @return @true if the hotkey was unregistered successfully, @false if the
                id was invalid.

        @remarks This function is currently only implemented under MSW.

        @see RegisterHotKey()
    */
    bool UnregisterHotKey(int hotkeyId);

    /**
        Unreserve an ID or range of IDs that was reserved by NewControlId().
        See @ref overview_windowids for more information.

        @param id
            The starting ID of the range of IDs to unreserve.
        @param count
            The number of sequential IDs to unreserve.

        @see NewControlId(), wxIdManager, @ref overview_windowids
    */
    static void UnreserveControlId(wxWindowID id, int count = 1);

    /**
        Unset any existing tooltip.

        @since 2.9.0

        @see SetToolTip()
     */
    void UnsetToolTip();

    /**
        Calling this method immediately repaints the invalidated area of the window and
        all of its children recursively while this would usually only happen when the
        flow of control returns to the event loop.

        Notice that this function doesn't invalidate any area of the window so
        nothing happens if nothing has been invalidated (i.e. marked as requiring
        a redraw). Use Refresh() first if you want to immediately redraw the
        window unconditionally.
    */
    virtual void Update();

    /**
        This function sends one or more wxUpdateUIEvent to the window.
        The particular implementation depends on the window; for example a
        wxToolBar will send an update UI event for each toolbar button,
        and a wxFrame will send an update UI event for each menubar menu item.

        You can call this function from your application to ensure that your
        UI is up-to-date at this point (as far as your wxUpdateUIEvent handlers
        are concerned). This may be necessary if you have called
        wxUpdateUIEvent::SetMode() or wxUpdateUIEvent::SetUpdateInterval() to limit
        the overhead that wxWidgets incurs by sending update UI events in idle time.
        @a flags should be a bitlist of one or more of the wxUpdateUI enumeration.

        If you are calling this function from an OnInternalIdle or OnIdle
        function, make sure you pass the wxUPDATE_UI_FROMIDLE flag, since
        this tells the window to only update the UI elements that need
        to be updated in idle time. Some windows update their elements
        only when necessary, for example when a menu is about to be shown.
        The following is an example of how to call UpdateWindowUI from
        an idle function.

        @code
        void MyWindow::OnInternalIdle()
        {
            if (wxUpdateUIEvent::CanUpdate(this))
                UpdateWindowUI(wxUPDATE_UI_FROMIDLE);
        }
        @endcode

        @see wxUpdateUIEvent, DoUpdateWindowUI(), OnInternalIdle()
    */
    virtual void UpdateWindowUI(long flags = wxUPDATE_UI_NONE);

    /**
        Validates the current values of the child controls using their validators.
        If the window has @c wxWS_EX_VALIDATE_RECURSIVELY extra style flag set,
        the method will also call Validate() of all child windows.

        @return Returns @false if any of the validations failed.

        @see TransferDataFromWindow(), TransferDataToWindow(),
             wxValidator
    */
    virtual bool Validate();

    /**
        Moves the pointer to the given position on the window.

        @note This function is not supported under Mac because Apple Human
              Interface Guidelines forbid moving the mouse cursor programmatically.

        @param x
            The new x position for the cursor.
        @param y
            The new y position for the cursor.
    */
    virtual void WarpPointer(int x, int y);
};



// ============================================================================
// Global functions/macros
// ============================================================================

/** @ingroup group_funcmacro_misc */
//@{

/**
    Find the deepest window at the mouse pointer position, returning the window
    and current pointer position in screen coordinates.

    @header{wx/window.h}
*/
wxWindow* wxFindWindowAtPointer(wxPoint& pt);

/**
    Gets the currently active window (implemented for MSW and GTK only
    currently, always returns @NULL in the other ports).

    @header{wx/window.h}
*/
wxWindow* wxGetActiveWindow();

/**
    Returns the first top level parent of the given window, or in other words,
    the frame or dialog containing it, or @NULL.

    @header{wx/window.h}
*/
wxWindow* wxGetTopLevelParent(wxWindow* window);

//@}

