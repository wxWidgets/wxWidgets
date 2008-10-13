/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.h
// Purpose:     interface of wxBitmapButton
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxBitmapButton

    A bitmap button is a control that contains a bitmap.
    It may be placed on a wxDialog or a wxPanel, or indeed almost any other window.

    @remarks
    A bitmap button can be supplied with a single bitmap, and wxWidgets will draw
    all button states using this bitmap. If the application needs more control,
    additional bitmaps for the selected state, unpressed focused state, and greyed-out
    state may be supplied.

    @section wxbitmapbutton_states Button states
    This class supports bitmaps for several different states:

    @li @b normal: this is the bitmap shown in the default state, it must be always
        valid while all the other bitmaps are optional and don't have to be set.
    @li @b disabled: bitmap shown when the button is disabled.
    @li @b selected: bitmap shown when the button is pushed (e.g. while the user
        keeps the mouse button pressed on it)
    @li @b focus: bitmap shown when the button has keyboard focus but is not pressed.
    @li @b hover: bitmap shown when the mouse is over the button (but it is not pressed).
        Notice that if hover bitmap is not specified but the current platform UI uses
        hover images for the buttons (such as Windows XP or GTK+), then the focus bitmap
        is used for hover state as well. This makes it possible to set focus bitmap only
        to get reasonably good behaviour on all platforms.

    @beginStyleTable
    @style{wxBU_AUTODRAW}
           If this is specified, the button will be drawn automatically using
           the label bitmap only, providing a 3D-look border. If this style is
           not specified, the button will be drawn without borders and using
           all provided bitmaps. Has effect only under MS Windows.
    @style{wxBU_LEFT}
           Left-justifies the bitmap label. Has effect only under MS Windows.
    @style{wxBU_TOP}
           Aligns the bitmap label to the top of the button.
           Has effect only under MS Windows.
    @style{wxBU_RIGHT}
           Right-justifies the bitmap label. Has effect only under MS Windows.
    @style{wxBU_BOTTOM}
           Aligns the bitmap label to the bottom of the button.
           Has effect only under MS Windows.
    @endStyleTable

    Note that the wxBU_EXACTFIT style supported by wxButton is not used by this
    class as bitmap buttons don't have any minimal standard size by default.

    @beginEventTable{wxCommandEvent}
    @event{EVT_BUTTON(id, func)}
           Process a wxEVT_COMMAND_BUTTON_CLICKED event, when the button is clicked.
    @endEventTable

    @library{wxcore}
    @category{ctrl}
    @appearance{bitmapbutton.png}

    @see wxButton
*/
class wxBitmapButton : public wxButton
{
public:
    /**
        Default ctor.
    */
    wxBitmapButton();

    /**
        Constructor, creating and showing a button.

        @param parent
            Parent window. Must not be @NULL.
        @param id
            Button identifier. The value wxID_ANY indicates a default value.
        @param bitmap
            Bitmap to be displayed.
        @param pos
            Button position.
        @param size
            Button size. If wxDefaultSize is specified then the button is sized
            appropriately for the bitmap.
        @param style
            Window style. See wxBitmapButton.
        @param validator
            Window validator.
        @param name
            Window name.

        @remarks The bitmap parameter is normally the only bitmap you need to provide,
                 and wxWidgets will draw the button correctly in its different states.
                 If you want more control, call any of the functions SetBitmapSelected(),
                 SetBitmapFocus(), SetBitmapDisabled().

        @see Create(), wxValidator
    */
    wxBitmapButton(wxWindow* parent, wxWindowID id,
                   const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxButtonNameStr);

    /**
        Destructor, destroying the button.
    */
    virtual ~wxBitmapButton();

    /**
        Button creation function for two-step creation.
        For more details, see wxBitmapButton().
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxBitmap& bitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxBU_AUTODRAW,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    //@{
    /**
        Returns the bitmap for the disabled state, which may be invalid.

        @return A reference to the disabled state bitmap.

        @see SetBitmapDisabled()
    */
    const wxBitmap& GetBitmapDisabled() const;
    wxBitmap& GetBitmapDisabled();
    //@}

    //@{
    /**
        Returns the bitmap for the focused state, which may be invalid.

        @return A reference to the focused state bitmap.

        @see SetBitmapFocus()
    */
    const wxBitmap& GetBitmapFocus() const;
    wxBitmap&  GetBitmapFocus();
    //@}

    //@{
    /**
        Returns the bitmap used when the mouse is over the button, which may be invalid.

        @see SetBitmapHover()
    */
    const wxBitmap& GetBitmapHover();
    wxBitmap&  GetBitmapHover();
    //@}

    //@{
    /**
        Returns the label bitmap (the one passed to the constructor), always valid.

        @return A reference to the button's label bitmap.

        @see SetBitmapLabel()
    */
    const wxBitmap& GetBitmapLabel();
    wxBitmap&  GetBitmapLabel();
    //@}

    /**
        Returns the bitmap for the selected state.

        @return A reference to the selected state bitmap.

        @see SetBitmapSelected()
    */
    const wxBitmap& GetBitmapSelected() const;

    /**
        Sets the bitmap for the disabled button appearance.

        @param bitmap
            The bitmap to set.

        @see GetBitmapDisabled(), SetBitmapLabel(),
             SetBitmapSelected(), SetBitmapFocus()
    */
    virtual void SetBitmapDisabled(const wxBitmap& bitmap);

    /**
        Sets the bitmap for the button appearance when it has the keyboard focus.

        @param bitmap
            The bitmap to set.

        @see GetBitmapFocus(), SetBitmapLabel(),
             SetBitmapSelected(), SetBitmapDisabled()
    */
    virtual void SetBitmapFocus(const wxBitmap& bitmap);

    /**
        Sets the bitmap to be shown when the mouse is over the button.

        @since 2.7.0

        The hover bitmap is currently only supported in wxMSW.

        @see GetBitmapHover()
    */
    virtual void SetBitmapHover(const wxBitmap& bitmap);

    /**
        Sets the bitmap label for the button.

        @param bitmap
            The bitmap label to set.

        @remarks This is the bitmap used for the unselected state, and for all
                 other states if no other bitmaps are provided.

        @see GetBitmapLabel()
    */
    virtual void SetBitmapLabel(const wxBitmap& bitmap);

    /**
        Sets the bitmap for the selected (depressed) button appearance.

        @param bitmap
            The bitmap to set.
    */
    virtual void SetBitmapSelected(const wxBitmap& bitmap);
};

