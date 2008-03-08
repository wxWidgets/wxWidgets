/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.h
// Purpose:     documentation for wxBitmapButton class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxBitmapButton
    @wxheader{bmpbuttn.h}
    
    A bitmap button is a control that contains a bitmap.
    It may be placed on a @ref overview_wxdialog "dialog box" or panel, or indeed
    almost any other window.
    
    @beginStyleTable
    @style{wxBU_AUTODRAW}:
           If this is specified, the button will be drawn automatically using
           the label bitmap only, providing a 3D-look border. If this style is
           not specified, the button will be drawn without borders and using
           all provided bitmaps. WIN32 only.
    @style{wxBU_LEFT}:
           Left-justifies the bitmap label. WIN32 only.
    @style{wxBU_TOP}:
           Aligns the bitmap label to the top of the button. WIN32 only.
    @style{wxBU_RIGHT}:
           Right-justifies the bitmap label. WIN32 only.
    @style{wxBU_BOTTOM}:
           Aligns the bitmap label to the bottom of the button. WIN32 only.
    @endStyleTable
    
    @beginEventTable
    @event{EVT_BUTTON(id\, func)}:
           Process a wxEVT_COMMAND_BUTTON_CLICKED event, when the button is
           clicked.
    @endEventTable
    
    @library{wxcore}
    @category{ctrl}
    @appearance{bitmapbutton.png}
    
    @seealso
    wxButton
*/
class wxBitmapButton : public wxButton
{
public:
    //@{
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
        
        @remarks The bitmap parameter is normally the only bitmap you need to
                   provide, and wxWidgets will draw the button correctly
                   in its different states. If you want more control,
                   call any of the functions
                   SetBitmapSelected(),
                   SetBitmapFocus(),
                   SetBitmapDisabled().
        
        @sa Create(), wxValidator
    */
    wxBitmapButton();
        wxBitmapButton(wxWindow* parent, wxWindowID id,
                       const wxBitmap& bitmap,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxBU_AUTODRAW,
                       const wxValidator& validator = wxDefaultValidator,
                       const wxString& name = "button");
    //@}

    /**
        Destructor, destroying the button.
    */
    ~wxBitmapButton();

    /**
        Button creation function for two-step creation. For more details, see
        wxBitmapButton().
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxBitmap& bitmap,
                const wxPoint& pos,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator,
                const wxString& name = "button");

    //@{
    /**
        Returns the bitmap for the disabled state, may be invalid.
        
        @returns A reference to the disabled state bitmap.
        
        @sa SetBitmapDisabled()
    */
    const wxBitmap GetBitmapDisabled();
        wxBitmap GetBitmapDisabled();
    //@}

    //@{
    /**
        Returns the bitmap for the focused state, may be invalid.
        
        @returns A reference to the focused state bitmap.
        
        @sa SetBitmapFocus()
    */
    const wxBitmap GetBitmapFocus();
        wxBitmap GetBitmapFocus();
    //@}

    //@{
    /**
        Returns the bitmap used when the mouse is over the button, may be invalid.
        
        @sa SetBitmapHover()
    */
    const wxBitmap GetBitmapHover();
        wxBitmap GetBitmapHover();
    //@}

    //@{
    /**
        Returns the label bitmap (the one passed to the constructor), always valid.
        
        @returns A reference to the button's label bitmap.
        
        @sa SetBitmapLabel()
    */
    const wxBitmap GetBitmapLabel();
        wxBitmap GetBitmapLabel();
    //@}

    /**
        Returns the bitmap for the selected state.
        
        @returns A reference to the selected state bitmap.
        
        @sa SetBitmapSelected()
    */
    wxBitmap GetBitmapSelected();

    /**
        Sets the bitmap for the disabled button appearance.
        
        @param bitmap 
        The bitmap to set.
        
        @sa GetBitmapDisabled(), SetBitmapLabel(),
              SetBitmapSelected(), SetBitmapFocus()
    */
    void SetBitmapDisabled(const wxBitmap& bitmap);

    /**
        Sets the bitmap for the button appearance when it has the keyboard focus.
        
        @param bitmap 
        The bitmap to set.
        
        @sa GetBitmapFocus(), SetBitmapLabel(),
              SetBitmapSelected(), SetBitmapDisabled()
    */
    void SetBitmapFocus(const wxBitmap& bitmap);

    /**
        Sets the bitmap to be shown when the mouse is over the button.
        
        This function is new since wxWidgets version 2.7.0 and the hover bitmap is
        currently only supported in wxMSW. 
        
        @sa GetBitmapHover()
    */
    void SetBitmapHover(const wxBitmap& bitmap);

    /**
        Sets the bitmap label for the button.
        
        @param bitmap 
        The bitmap label to set.
        
        @remarks This is the bitmap used for the unselected state, and for all
                   other states if no other bitmaps are provided.
        
        @sa GetBitmapLabel()
    */
    void SetBitmapLabel(const wxBitmap& bitmap);

    /**
        Sets the bitmap for the selected (depressed) button appearance.
        
        @param bitmap 
        The bitmap to set.
    */
    void SetBitmapSelected(const wxBitmap& bitmap);
};
