/////////////////////////////////////////////////////////////////////////////
// Name:        taskbarbutton.h
// Purpose:     interface of wxTaskBarButton
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    State of the taskbar button.
*/
enum WXDLLIMPEXP_CORE wxTaskBarButtonState
{
    wxTASKBAR_BUTTON_NO_PROGRESS   = 0,
    wxTASKBAR_BUTTON_INDETERMINATE = 1,
    wxTASKBAR_BUTTON_NORMAL        = 2,
    wxTASKBAR_BUTTON_ERROR         = 4,
    wxTASKBAR_BUTTON_PAUSED        = 8
};

/**
    @class wxThumbBarButton

    A thumbnail toolbar button is a control that displayed in the thumbnail
    image of a window in a taskbar button flyout.

    @library{wxcore}
    @category{misc}

    @onlyfor{wxmsw}

    @see wxTaskBarButton
*/
class WXDLLIMPEXP_CORE wxThumbBarButton {
public:
    /**
        Default constructor to allow 2-phase creation.
    */
    wxThumbBarButton();

    /**
        Constructs the thumbnail toolbar button.

        @param id
            The identifier for the control.
        @param icon
            The icon used as the button image.
        @param tooltip
            The text of the button's tooltip, displayed when the mouse pointer
            hovers over the button.
        @param enable
            If @true (default), the button is active and available to the user.
            If @false, the button is disabled. It is present, but has a visual
            state that indicates that it will not respond to user action.
        @param dismissOnClick
            If @true, when the button is clicked, the taskbar button's flyout
            closes immediately. @false by default.
        @param hasBackground
            If @false, the button border is not drawn. @true by default.
        @param shown
            If @false, the button is not shown to the user. @true by default.
        @param interactive
            If @false, the button is enabled but not interactive; no pressed
            button state is drawn. This flag is intended for instances where
            the button is used in a notification. @true by default.
    */
    wxThumbBarButton(int id,
                     const wxIcon& icon,
                     const wxString& tooltip = wxString(),
                     bool enable = true,
                     bool dismissOnClick = false,
                     bool hasBackground = true,
                     bool shown = true,
                     bool interactive = true);

    bool Create(int id,
                const wxIcon& icon,
                const wxString& tooltip = wxString(),
                bool enable = true,
                bool dismissOnClick = false,
                bool hasBackground = true,
                bool shown = true,
                bool interactive = true);

    virtual ~wxThumbBarButton();

    /**
        Returns the identifier associated with this control.
    */
    int GetID() const;

    /**
        Returns the icon associated with this control.
    */
    const wxIcon& GetIcon() const;

    /**
        Returns the tooltip.
    */
    const wxString& GetTooltip() const;

    /**
        Returns @true if the button is enabled, @false if it has been disabled.
    */
    bool IsEnable() const;

    /**
        Enables or disables the thumbnail toolbar button.
    */
    void Enable(bool enable = true);

    /**
        Equivalent to calling wxThumbBarButton::Enable(false).
    */
    void Disable();

    /**
        Returns @true if the button will dismiss on click.
    */
    bool IsDismissOnClick() const;

    /**
        Whether the window thumbnail is dismissed after a button click.
    */
    void EnableDismissOnClick(bool enable = true);

    /**
        Equivalent to calling wxThumbBarButton::DisableDimissOnClick(false).
    */
    void DisableDimissOnClick();

    /**
        Returns @true if the button has button border.
    */
    bool HasBackground() const;

    /**
        Set the property that whether the button has background.
    */
    void SetHasBackground(bool has = true);

    /**
        Returns @true if the button is shown, @false if it has been hidden.
    */
    bool IsShown() const;

    /**
        Show or hide the thumbnail toolbar button.
    */
    void Show(bool shown = true);

    /**
        Hide the thumbnail toolbar button. Equivalent to calling wxThumbBarButton::Show(false).
    */
    void Hide();

    /**
        Returns @true if the button is interactive.
    */
    bool IsInteractive() const;

    /**
        Set the property which holds whether the button is interactive.

        A non-interactive thumbnail toolbar button does not react to user
        interaction, but is still visually enabled.
    */
    void SetInteractive(bool interactive = true);
};

/**
    @class wxTaskBarButton

    A taskbar button that associated with the window under Windows 7 or later.

    It is used to access the functionality including thumbnail representations,
    thumbnail toolbars, notification and status overlays, and progress
    indicators.

    @note This class is only created and initialized in the internal implementation
    of wxFrame by design. You can only get the pointer of the instance which
    associated with the frame by calling wxFrame::MSWGetTaskBarButton().

    @library{wxcore}
    @category{misc}

    @nativeimpl{wxmsw}
    @onlyfor{wxmsw}

    @see wxFrame::MSWGetTaskBarButton()
*/
class WXDLLIMPEXP_CORE wxTaskBarButton
{
public:
    /**
        Starts showing a determinate progress indicator.

        Call SetProgressValue() after this call to update the progress
        indicator.

        If @a range is 0, the progress indicator is dismissed.
    */
    virtual void SetProgressRange(int range);

    /**
        Update the determinate progress indicator.

        @param value Must be in the range from 0 to the argument to the last
        SetProgressRange() call. When it is equal to the range, the progress
        bar is dismissed.
    */
    virtual void SetProgressValue(int value);

    /**
        Updates indeterminate progress indicator.

        The first call to this method starts showing the indeterminate progress
        indicator if it hadn't been shown yet.

        Call SetProgressRange(0) to stop showing the progress indicator.
    */
    virtual void PulseProgress();

    /**
        Show in the taskbar.
    */
    virtual void Show(bool show = true);

    /**
        Hide in the taskbar.
    */
    virtual void Hide();

    /**
        Specifies or updates the text of the tooltip that is displayed
        when the mouse pointer rests on an individual preview thumbnail
        in a taskbar button flyout.
    */
    virtual void SetThumbnailTooltip(const wxString& tooltip);

    /**
        Set the state of the progress indicator displayed on a taskbar button.

        @see wxTaskBarButtonState
    */
    virtual void SetProgressState(wxTaskBarButtonState state);

    /**
        Set an overlay icon to indicate application status or a notification top
        the user.

        @param icon
            This should be a small icon, measuring 16x16 pixels at 96 dpi. If an
            overlay icon is already applied to the taskbar button, that existing
            overlay is replaced. Setting with wxNullIcon to remove.
        @param description
            The property holds the description of the overlay for accessibility
            purposes.
    */
    virtual void SetOverlayIcon(const wxIcon& icon,
                                const wxString& description = wxString());

    /**
        Selects a portion of a window's client area to display as that window's
        thumbnail in the taskbar.

        @param  rect
            The portion inside of the window. Setting with an empty wxRect will
            restore the default diaplay of the thumbnail.
    */
    virtual void SetThumbnailClip(const wxRect& rect);

    /**
        Selects the child window area to display as that window's thumbnail in
        the taskbar.
    */
    virtual void SetThumbnailContents(const wxWindow *child);

    /**
        Inserts the given button before the position pos to the taskbar
        thumbnail toolbar.

        @note The number of buttons and separators is limited to 7.

        @see AppendThumbBarButton(), AppendSeparatorInThumbBar()
    */
    virtual bool InsertThumbBarButton(size_t pos, wxThumbBarButton *button);

    /**
        Appends a button to the taskbar thumbnail toolbar.

        @note The number of buttons and separators is limited to 7.

        @see InsertThumbBarButton(), AppendSeparatorInThumbBar()
    */
    virtual bool AppendThumbBarButton(wxThumbBarButton *button);

    /**
        Appends a separator to the taskbar thumbnail toolbar.

        @note The number of buttons and separators is limited to 7.

        @see AppendThumbBarButton(), InsertThumbBarButton()
    */
    virtual bool AppendSeparatorInThumbBar();

    /**
        Removes the thumbnail toolbar button from the taskbar button but doesn't
        delete the associated c++ object.

        @param button
            The thumbnail toolbar button to remove.

        @return A pointer to the button which was detached from the taskbar
            button.
    */
    virtual wxThumbBarButton* RemoveThumbBarButton(wxThumbBarButton *button);

    /**
        Removes the thumbnail toolbar button from the taskbar button but doesn't
        delete the associated c++ object.

        @param id
            The identifier of the thumbnail toolbar button to remove.

        @return A pointer to the button which was detached from the taskbar
            button.
    */
    virtual wxThumbBarButton* RemoveThumbBarButton(int id);
};
