/////////////////////////////////////////////////////////////////////////////
// Name:        toplevel.h
// Purpose:     documentation for wxTopLevelWindow class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxTopLevelWindow
    @wxheader{toplevel.h}

    wxTopLevelWindow is a common base class for wxDialog and
    wxFrame. It is an abstract base class meaning that you never
    work with objects of this class directly, but all of its methods are also
    applicable for the two classes above.

    @library{wxcore}
    @category{managedwnd}

    @seealso
    wxTopLevelWindow::SetTransparent
*/
class wxTopLevelWindow : public wxWindow
{
public:
    /**
        Returns @true if the platform supports making the window translucent.
        
        @sa SetTransparent()
    */
    virtual bool CanSetTransparent();

    /**
        A synonym for CentreOnScreen().
    */
    void CenterOnScreen(int direction);

    /**
        Centres the window on screen.
        
        @param direction
        Specifies the direction for the centering. May be wxHORIZONTAL, wxVERTICAL
        or wxBOTH.
        
        @sa wxWindow::CentreOnParent
    */
    void CentreOnScreen(int direction = wxBOTH);

    /**
        Enables or disables the Close button (most often in the right
        upper corner of a dialog) and the Close entry of the system
        menu (most often in the left upper corner of the dialog).
        Currently only implemented for wxMSW and wxGTK. Returns
        @true if operation was successful. This may be wrong on
        X11 (including GTK+) where the window manager may not support
        this operation and there is no way to find out.
    */
    bool EnableCloseButton(bool enable = @true);

    /**
        Returns a pointer to the button which is the default for this window, or @c
        @NULL.
        The default button is the one activated by pressing the Enter key.
    */
    wxWindow * GetDefaultItem();

    /**
        Returns the standard icon of the window. The icon will be invalid if it hadn't
        been previously set by SetIcon().
        
        @sa GetIcons()
    */
    const wxIcon GetIcon();

    /**
        Returns all icons associated with the window, there will be none of them if
        neither SetIcon() nor
        SetIcons() had been called before.
        
        Use GetIcon() to get the main icon of the
        window.
        
        @sa wxIconBundle
    */
    const wxIconBundle GetIcons();

    /**
        Gets a string containing the window title.
        
        @sa SetTitle()
    */
    wxString GetTitle();

    /**
        Unique to the wxWinCE port. Responds to showing/hiding SIP (soft input panel)
        area and resize
        window accordingly. Override this if you want to avoid resizing or do additional
        operations.
    */
    virtual bool HandleSettingChange(WXWPARAM wParam,
                                     WXLPARAM lParam);

    /**
        Iconizes or restores the window.
        
        @param iconize
        If @true, iconizes the window; if @false, shows and restores it.
        
        @sa IsIconized(), Maximize().
    */
    void Iconize(bool iconize);

    /**
        Returns @true if this window is currently active, i.e. if the user is
        currently
        working with it.
    */
    bool IsActive();

    /**
        Returns @true if this window is expected to be always maximized, either due
        to platform policy
        or due to local policy regarding particular class.
    */
    virtual bool IsAlwaysMaximized();

    /**
        Returns @true if the window is in fullscreen mode.
        
        @sa ShowFullScreen()
    */
    bool IsFullScreen();

    /**
        Returns @true if the window is iconized.
    */
    bool IsIconized();

    /**
        Returns @true if the window is maximized.
    */
    bool IsMaximized();

    /**
        @b @c This method is specific to wxUniversal port
        
        Returns @true if this window is using native decorations, @false if we draw
        them ourselves.
        
        @sa UseNativeDecorations(),
              UseNativeDecorationsByDefault()
    */
    bool IsUsingNativeDecorations();

    /**
        Maximizes or restores the window.
        
        @param maximize
        If @true, maximizes the window, otherwise it restores it.
        
        @sa Iconize()
    */
    void Maximize(bool maximize);

    /**
        Use a system-dependent way to attract users attention to the window when it is
        in background.
        
        @e flags may have the value of either @c wxUSER_ATTENTION_INFO
        (default) or @c wxUSER_ATTENTION_ERROR which results in a more drastic
        action. When in doubt, use the default value.
        
        Note that this function should normally be only used when the application is
        not already in foreground.
        
        This function is currently implemented for Win32 where it flashes the
        window icon in the taskbar, and for wxGTK with task bars supporting it.
    */
    void RequestUserAttention(int flags = wxUSER_ATTENTION_INFO);

    /**
        Changes the default item for the panel, usually @e win is a button.
        
        @sa GetDefaultItem()
    */
    void SetDefaultItem(wxWindow win);

    /**
        Sets the icon for this window.
        
        @param icon
        The icon to associate with this window.
        
        @remarks The window takes a 'copy' of icon, but since it uses reference
                   counting, the copy is very quick. It is safe to
                   delete icon after calling this function.
    */
    void SetIcon(const wxIcon& icon);

    /**
        Sets several icons of different sizes for this window: this allows to use
        different icons for different situations (e.g. task switching bar, taskbar,
        window title bar) instead of scaling, with possibly bad looking results, the
        only icon set by SetIcon().
        
        @param icons
        The icons to associate with this window.
        
        @sa wxIconBundle.
    */
    void SetIcons(const wxIconBundle& icons);

    /**
        Sets action or menu activated by pressing left hardware button on the smart
        phones.
        Unavailable on full keyboard machines.
        
        @param id
        Identifier for this button.
        
        @param label
        Text to be displayed on the screen area dedicated to this hardware button.
        
        @param subMenu
        The menu to be opened after pressing this hardware button.
        
        @sa SetRightMenu().
    */
    void SetLeftMenu(int id = wxID_ANY,
                     const wxString& label = wxEmptyString,
                     wxMenu * subMenu = @NULL);

    /**
        A simpler interface for setting the size hints than
        SetSizeHints().
    */
    void SetMaxSize(const wxSize& size);

    /**
        A simpler interface for setting the size hints than
        SetSizeHints().
    */
    void SetMinSize(const wxSize& size);

    /**
        Sets action or menu activated by pressing right hardware button on the smart
        phones.
        Unavailable on full keyboard machines.
        
        @param id
        Identifier for this button.
        
        @param label
        Text to be displayed on the screen area dedicated to this hardware button.
        
        @param subMenu
        The menu to be opened after pressing this hardware button.
        
        @sa SetLeftMenu().
    */
    void SetRightMenu(int id = wxID_ANY,
                      const wxString& label = wxEmptyString,
                      wxMenu * subMenu = @NULL);

    /**
        If the platform supports it, sets the shape of the window to that
        depicted by @e region.  The system will not display or
        respond to any mouse event for the pixels that lie outside of the
        region.  To reset the window to the normal rectangular shape simply
        call @e SetShape again with an empty region.  Returns @true if the
        operation is successful.
    */
    bool SetShape(const wxRegion& region);

    //@{
    /**
        Allows specification of minimum and maximum window sizes, and window size
        increments.
        If a pair of values is not set (or set to -1), no constraints will be used.
        
        @param incW
        Specifies the increment for sizing the width (GTK/Motif/Xt only).
        
        @param incH
        Specifies the increment for sizing the height (GTK/Motif/Xt only).
        
        @param incSize
        Increment size (only taken into account under X11-based
        ports such as wxGTK/wxMotif/wxX11).
        
        @remarks Notice that this function not only prevents the user from
                   resizing the window outside the given bounds but it
                   also prevents the program itself from doing it using
                   SetSize.
    */
    virtual void SetSizeHints(int minW, int minH, int maxW=-1,
                              int maxH=-1,
                              int incW=-1,
                              int incH=-1);
    void SetSizeHints(const wxSize& minSize,
                      const wxSize& maxSize=wxDefaultSize,
                      const wxSize& incSize=wxDefaultSize);
    //@}

    /**
        Sets the window title.
        
        @param title
        The window title.
        
        @sa GetTitle()
    */
    virtual void SetTitle(const wxString& title);

    /**
        If the platform supports it will set the window to be translucent
        
        @param alpha
        Determines how opaque or transparent the window will
          be, if the platform supports the opreration.  A value of 0 sets the
          window to be fully transparent, and a value of 255 sets the window
          to be fully opaque.
    */
    virtual bool SetTransparent(int alpha);

    /**
        This virtual function is not meant to be called directly but can be overridden
        to return @false (it returns @true by default) to allow the application to
        close even if this, presumably not very important, window is still opened.
        By default, the application stays alive as long as there are any open top level
        windows.
    */
    virtual bool ShouldPreventAppExit();

    /**
        Depending on the value of @e show parameter the window is either shown full
        screen or restored to its normal state. @e style is a bit list containing
        some or all of the following values, which indicate what elements of the window
        to hide in full-screen mode:
        
         wxFULLSCREEN_NOMENUBAR
         wxFULLSCREEN_NOTOOLBAR
         wxFULLSCREEN_NOSTATUSBAR
         wxFULLSCREEN_NOBORDER
         wxFULLSCREEN_NOCAPTION
         wxFULLSCREEN_ALL (all of the above)
        
        This function has not been tested with MDI frames.
        
        Note that showing a window full screen also actually
        @ref wxWindow::show Show()s if it hadn't been shown yet.
        
        @sa IsFullScreen()
    */
    bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL);

    /**
        @b @c This method is specific to wxUniversal port
        
        Use native or custom-drawn decorations for this window only. Notice that to
        have any effect this method must be called before really creating the window,
        i.e. two step creation must be used:
        
        @sa UseNativeDecorationsByDefault(),
              IsUsingNativeDecorations()
    */
    void UseNativeDecorations(bool native = @true);

    /**
        @b @c This method is specific to wxUniversal port
        
        Top level windows in wxUniversal port can use either system-provided window
        decorations (i.e. title bar and various icons, buttons and menus in it) or draw
        the decorations themselves. By default the system decorations are used if they
        are available, but this method can be called with @e native set to @false to
        change this for all windows created after this point.
        
        Also note that if @c WXDECOR environment variable is set, then custom
        decorations are used by default and so it may make sense to call this method
        with default argument if the application can't use custom decorations at all
        for some reason.
    */
    void UseNativeDecorationsByDefault(bool native = @true);
};
