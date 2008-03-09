/////////////////////////////////////////////////////////////////////////////
// Name:        combo.h
// Purpose:     documentation for wxComboPopup class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxComboPopup
    @wxheader{combo.h}

    In order to use a custom popup with wxComboCtrl,
    an interface class must be derived from wxComboPopup. For more information
    how to use it, see @ref overview_wxcomboctrl "Setting Custom Popup for
    wxComboCtrl".

    @library{wxcore}
    @category{FIXME}

    @seealso
    wxComboCtrl
*/
class wxComboPopup
{
public:
    /**
        Default constructor. It is recommended that internal variables
        are prepared in Init() instead
        (because @ref mcombo() m_combo is not valid in constructor).
    */
    wxComboPopup();

    /**
        The derived class must implement this to create the popup control.
        
        @returns @true if the call succeeded, @false otherwise.
    */
    bool Create(wxWindow* parent);

    /**
        Utility function that hides the popup.
    */
    void Dismiss();

    /**
        The derived class may implement this to return adjusted size
        for the popup control, according to the variables given.
        
        @param minWidth
            Preferred minimum width.
        @param prefHeight
            Preferred height. May be -1 to indicate
            no preference.
        @param maxWidth
            Max height for window, as limited by
            screen size.
        
        @remarks Called each time popup is about to be shown.
    */
    wxSize GetAdjustedSize(int minWidth, int prefHeight,
                           int maxHeight);

    /**
        The derived class must implement this to return pointer
        to the associated control created in Create().
    */
    wxWindow* GetControl();

    /**
        The derived class must implement this to return
        string representation of the value.
    */
    wxString GetStringValue();

    /**
        The derived class must implement this to initialize
        its internal variables. This method is called immediately
        after construction finishes. @ref mcombo() m_combo
        member variable has been initialized before the call.
    */
    void Init();

    /**
        Utility method that returns @true if Create has been called.
        Useful in conjunction with LazyCreate().
    */
    bool IsCreated();

    /**
        The derived class may implement this to return
        @true if it wants to delay call to Create()
        until the popup is shown for the first time. It is more
        efficient, but on the other hand it is often more convenient
        to have the control created immediately.
        
        @remarks Base implementation returns @false.
    */
    bool LazyCreate();

    /**
        The derived class may implement this to do something
        when the parent wxComboCtrl gets double-clicked.
    */
    void OnComboDoubleClick();

    /**
        The derived class may implement this to receive
        key events from the parent wxComboCtrl.
        Events not handled should be skipped, as usual.
    */
    void OnComboKeyEvent(wxKeyEvent& event);

    /**
        The derived class may implement this to do
        special processing when popup is hidden.
    */
    void OnDismiss();

    /**
        The derived class may implement this to do
        special processing when popup is shown.
    */
    void OnPopup();

    /**
        The derived class may implement this to paint
        the parent wxComboCtrl.
        Default implementation draws value as string.
    */
    void PaintComboControl(wxDC& dc, const wxRect& rect);

    /**
        The derived class must implement this to receive
        string value changes from wxComboCtrl.
    */
    void SetStringValue(const wxString& value);

    /**
        wxComboCtrl m_combo
        Parent wxComboCtrl. This is parameter has
        been prepared before Init() is called.
    */
};


/**
    @class wxComboCtrl
    @wxheader{combo.h}

    A combo control is a generic combobox that allows totally
    custom popup. In addition it has other customization features.
    For instance, position and size of the dropdown button
    can be changed.

    @beginStyleTable
    @style{wxCB_READONLY}:
           Text will not be editable.
    @style{wxCB_SORT}:
           Sorts the entries in the list alphabetically.
    @style{wxTE_PROCESS_ENTER}:
           The control will generate the event wxEVT_COMMAND_TEXT_ENTER
           (otherwise pressing Enter key is either processed internally by the
           control or used for navigation between dialog controls). Windows
           only.
    @style{wxCC_SPECIAL_DCLICK}:
           Double-clicking triggers a call to popup's OnComboDoubleClick.
           Actual behaviour is defined by a derived class. For instance,
           wxOwnerDrawnComboBox will cycle an item. This style only applies if
           wxCB_READONLY is used as well.
    @style{wxCC_STD_BUTTON}:
           Drop button will behave more like a standard push button.
    @endStyleTable

    @beginEventTable
    @event{EVT_TEXT(id, func)}:
           Process a wxEVT_COMMAND_TEXT_UPDATED event, when the text changes.
    @event{EVT_TEXT_ENTER(id, func)}:
           Process a wxEVT_COMMAND_TEXT_ENTER event, when RETURN is pressed in
           the combo control.
    @endEventTable

    @library{wxbase}
    @category{ctrl}
    @appearance{comboctrl.png}

    @seealso
    wxComboBox, wxChoice, wxOwnerDrawnComboBox, wxComboPopup, wxCommandEvent
*/
class wxComboCtrl : public wxControl
{
public:
    //@{
    /**
        Constructor, creating and showing a combo control.
        
        @param parent
            Parent window. Must not be @NULL.
        @param id
            Window identifier. The value wxID_ANY indicates a default value.
        @param value
            Initial selection string. An empty string indicates no selection.
        @param pos
            Window position.
        @param size
            Window size. If wxDefaultSize is specified then the window is
        sized
            appropriately.
        @param style
            Window style. See wxComboCtrl.
        @param validator
            Window validator.
        @param name
            Window name.
        
        @see Create(), wxValidator
    */
    wxComboCtrl();
    wxComboCtrl(wxWindow* parent, wxWindowID id,
                const wxString& value = "",
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = "comboCtrl");
    //@}

    /**
        Destructor, destroying the combo control.
    */
    ~wxComboCtrl();

    /**
        This member function is not normally called in application code.
        Instead, it can be implemented in a derived class to create a
        custom popup animation.
        
        @returns @true if animation finishes before the function returns. @false
                 otherwise. In the latter case you need to manually call
                 DoShowPopup after the animation ends.
    */
    virtual bool AnimateShow(const wxRect& rect, int flags);

    /**
        Copies the selected text to the clipboard.
    */
    void Copy();

    /**
        Creates the combo control for two-step construction. Derived classes
        should call or replace this function. See wxComboCtrl()
        for further details.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& value = "",
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = "comboCtrl");

    /**
        Copies the selected text to the clipboard and removes the selection.
    */
    void Cut();

    /**
        This member function is not normally called in application code.
        Instead, it can be implemented in a derived class to return
        default wxComboPopup, incase @c popup is @NULL.
        @b Note: If you have implemented OnButtonClick to do
        something else than show the popup, then DoSetPopupControl
        must always return @NULL.
    */
    void DoSetPopupControl(wxComboPopup* popup);

    /**
        This member function is not normally called in application code.
        Instead, it must be called in a derived class to make sure popup
        is properly shown after a popup animation has finished (but only
        if AnimateShow() did not finish
        the animation within it's function scope).
        
        @param rect
            Position to show the popup window at, in screen coordinates.
        @param flags
            Combination of any of the following:
    */
    virtual void DoShowPopup(const wxRect& rect, int flags);

    /**
        Enables or disables popup animation, if any, depending on the value of
        the argument.
    */
    void EnablePopupAnimation(bool enable = true);

    /**
        Returns disabled button bitmap that has been set with
        SetButtonBitmaps().
        
        @returns A reference to the disabled state bitmap.
    */
    const wxBitmap GetBitmapDisabled();

    /**
        Returns button mouse hover bitmap that has been set with
        SetButtonBitmaps().
        
        @returns A reference to the mouse hover state bitmap.
    */
    const wxBitmap GetBitmapHover();

    /**
        Returns default button bitmap that has been set with
        SetButtonBitmaps().
        
        @returns A reference to the normal state bitmap.
    */
    const wxBitmap GetBitmapNormal();

    /**
        Returns depressed button bitmap that has been set with
        SetButtonBitmaps().
        
        @returns A reference to the depressed state bitmap.
    */
    const wxBitmap GetBitmapPressed();

    /**
        Returns current size of the dropdown button.
    */
    wxSize GetButtonSize();

    /**
        Returns custom painted area in control.
        
        @see SetCustomPaintWidth().
    */
    int GetCustomPaintWidth();

    /**
        Returns features supported by wxComboCtrl. If needed feature is missing,
        you need to instead use wxGenericComboCtrl, which however may lack
        native look and feel (but otherwise sports identical API).
        
        @returns Value returned is a combination of following flags:
    */
    static int GetFeatures();

    /**
        Returns the insertion point for the combo control's text field.
        @b Note: Under wxMSW, this function always returns 0 if the combo control
        doesn't have the focus.
    */
    long GetInsertionPoint();

    /**
        Returns the last position in the combo control text field.
    */
    long GetLastPosition();

    /**
        Returns current popup interface that has been set with SetPopupControl.
    */
    wxComboPopup* GetPopupControl();

    /**
        Returns popup window containing the popup control.
    */
    wxWindow* GetPopupWindow();

    /**
        Get the text control which is part of the combo control.
    */
    wxTextCtrl* GetTextCtrl();

    /**
        Returns actual indentation in pixels.
    */
    wxCoord GetTextIndent();

    /**
        Returns area covered by the text field (includes everything except
        borders and the dropdown button).
    */
    const wxRect GetTextRect();

    /**
        Returns text representation of the current value. For writable
        combo control it always returns the value in the text field.
    */
    wxString GetValue();

    /**
        Dismisses the popup window.
    */
    void HidePopup();

    /**
        Returns @true if the popup is currently shown
    */
    bool IsPopupShown();

    /**
        Returns @true if the popup window is in the given state.
        Possible values are:
        
        @c Hidden()
        
        Popup window is hidden.
        
        @c Animating()
        
        Popup window is being shown, but the
        popup animation has not yet finished.
        
        @c Visible()
        
        Popup window is fully visible.
    */
    bool IsPopupWindowState(int state);

    /**
        Implement in a derived class to define what happens on
        dropdown button click.
        Default action is to show the popup.
        @b Note: If you implement this to do something else than
        show the popup, you must then also implement
        DoSetPopupControl() to always
        return @NULL.
    */
    void OnButtonClick();

    /**
        Pastes text from the clipboard to the text field.
    */
    void Paste();

    /**
        Removes the text between the two positions in the combo control text field.
        
        @param from
            The first position.
        @param to
            The last position.
    */
    void Remove(long from, long to);

    /**
        Replaces the text between two positions with the given text, in the combo
        control text field.
        
        @param from
            The first position.
        @param to
            The second position.
        @param text
            The text to insert.
    */
    void Replace(long from, long to, const wxString& value);

    /**
        Sets custom dropdown button graphics.
        
        @param bmpNormal
            Default button image.
        @param pushButtonBg
            If @true, blank push button background is painted
            below the image.
        @param bmpPressed
            Depressed button image.
        @param bmpHover
            Button image when mouse hovers above it. This
            should be ignored on platforms and themes that do not generally draw
            different kind of button on mouse hover.
        @param bmpDisabled
            Disabled button image.
    */
    void SetButtonBitmaps(const wxBitmap& bmpNormal,
                          bool pushButtonBg = false,
                          const wxBitmap& bmpPressed = wxNullBitmap,
                          const wxBitmap& bmpHover = wxNullBitmap,
                          const wxBitmap& bmpDisabled = wxNullBitmap);

    /**
        Sets size and position of dropdown button.
        
        @param width
            Button width. Value = 0 specifies default.
        @param height
            Button height. Value = 0 specifies default.
        @param side
            Indicates which side the button will be placed.
            Value can be wxLEFT or wxRIGHT.
        @param spacingX
            Horizontal spacing around the button. Default is 0.
    */
    void SetButtonPosition(int width = -1, int height = -1,
                           int side = wxRIGHT,
                           int spacingX = 0);

    /**
        Set width, in pixels, of custom painted area in control without @c wxCB_READONLY
        style. In read-only wxOwnerDrawnComboBox, this is used
        to indicate area that is not covered by the focus rectangle.
    */
    void SetCustomPaintWidth(int width);

    /**
        Sets the insertion point in the text field.
        
        @param pos
            The new insertion point.
    */
    void SetInsertionPoint(long pos);

    /**
        Sets the insertion point at the end of the combo control text field.
    */
    void SetInsertionPointEnd();

    /**
        Set side of the control to which the popup will align itself. Valid values are
        @c wxLEFT, @c wxRIGHT and 0. The default value 0 means that the most appropriate
        side is used (which, currently, is always @c wxLEFT).
    */
    void SetPopupAnchor(int anchorSide);

    /**
        Set popup interface class derived from wxComboPopup.
        This method should be called as soon as possible after the control
        has been created, unless OnButtonClick()
        has been overridden.
    */
    void SetPopupControl(wxComboPopup* popup);

    /**
        Extends popup size horizontally, relative to the edges of the combo control.
        
        @param extLeft
            How many pixel to extend beyond the left edge of the
            control. Default is 0.
        @param extRight
            How many pixel to extend beyond the right edge of the
            control. Default is 0.
        
        @remarks Popup minimum width may override arguments.
    */
    void SetPopupExtents(int extLeft, int extRight);

    /**
        Sets preferred maximum height of the popup.
        
        @remarks Value -1 indicates the default.
    */
    void SetPopupMaxHeight(int height);

    /**
        Sets minimum width of the popup. If wider than combo control, it will extend to
        the left.
        
        @remarks Value -1 indicates the default.
    */
    void SetPopupMinWidth(int width);

    /**
        Selects the text between the two positions, in the combo control text field.
        
        @param from
            The first position.
        @param to
            The second position.
    */
    void SetSelection(long from, long to);

    /**
        Sets the text for the text field without affecting the
        popup. Thus, unlike SetValue(), it works
        equally well with combo control using @c wxCB_READONLY style.
    */
    void SetText(const wxString& value);

    /**
        This will set the space in pixels between left edge of the control and the
        text, regardless whether control is read-only or not. Value -1 can be
        given to indicate platform default.
    */
    void SetTextIndent(int indent);

    /**
        Sets the text for the combo control text field.
        @b NB: For a combo control with @c wxCB_READONLY style the
        string must be accepted by the popup (for instance, exist in the dropdown
        list), otherwise the call to SetValue() is ignored
    */
    void SetValue(const wxString& value);

    /**
        Same as SetValue, but also sends wxCommandEvent of type
        wxEVT_COMMAND_TEXT_UPDATED
        if @c withEvent is @true.
    */
    void SetValueWithEvent(const wxString& value,
                           bool withEvent = true);

    /**
        Show the popup.
    */
    void ShowPopup();

    /**
        Undoes the last edit in the text field. Windows only.
    */
    void Undo();

    /**
        Enable or disable usage of an alternative popup window, which guarantees
        ability to focus the popup control, and allows common native controls to
        function normally. This alternative popup window is usually a wxDialog,
        and as such, when it is shown, its parent top-level window will appear
        as if the focus has been lost from it.
    */
    void UseAltPopupWindow(bool enable = true);
};
