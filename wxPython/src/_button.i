/////////////////////////////////////////////////////////////////////////////
// Name:        _button.i
// Purpose:     SWIG interface defs for wxButton, wxBitmapButton
//
// Author:      Robin Dunn
//
// Created:     10-June-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup;

MAKE_CONST_WXSTRING(ButtonNameStr);

enum {
    wxBU_LEFT,
    wxBU_TOP,
    wxBU_RIGHT,
    wxBU_BOTTOM,

    wxBU_ALIGN_MASK,

    wxBU_EXACTFIT,
    wxBU_AUTODRAW,
};

//---------------------------------------------------------------------------

DocStr(wxButton,
"A button is a control that contains a text string, and is one of the most
common elements of a GUI.  It may be placed on a dialog box or panel, or
indeed almost any other window.", "

Window Styles
-------------
    ==============   ==========================================
    wx.BU_LEFT       Left-justifies the label. Windows and GTK+ only.
    wx.BU_TOP        Aligns the label to the top of the button.
                     Windows and GTK+ only.
    wx.BU_RIGHT      Right-justifies the bitmap label. Windows and GTK+ only.
    wx.BU_BOTTOM     Aligns the label to the bottom of the button.
                     Windows and GTK+ only.
    wx.BU_EXACTFIT   Creates the button as small as possible
                     instead of making it of the standard size
                     (which is the default behaviour.)
    ==============   ==========================================

Events
------
    ============     ==========================================
    EVT_BUTTON       Sent when the button is clicked.
    ============     ==========================================

:see: `wx.BitmapButton`
");


MustHaveApp(wxButton);

class wxButton : public wxControl
{
public:
    %pythonAppend wxButton         "self._setOORInfo(self)"
    %pythonAppend wxButton()       ""
    %typemap(out) wxButton*;    // turn off this typemap


    DocCtorStr(
        wxButton(wxWindow* parent, wxWindowID id=-1,
                 const wxString& label=wxPyEmptyString,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxPyButtonNameStr),
        "Create and show a button.  The preferred way to create standard
buttons is to use a standard ID and an empty label.  In this case
wxWigets will automatically use a stock label that coresponds to the
ID given.  In additon, the button will be decorated with stock icons
under GTK+ 2.", "

The stock IDs and coresponding labels are

    =====================   ======================
    wx.ID_ADD               'Add'
    wx.ID_APPLY             '\&Apply'
    wx.ID_BOLD              '\&Bold'
    wx.ID_CANCEL            '\&Cancel'
    wx.ID_CLEAR             '\&Clear'
    wx.ID_CLOSE             '\&Close'
    wx.ID_COPY              '\&Copy'
    wx.ID_CUT               'Cu\&t'
    wx.ID_DELETE            '\&Delete'
    wx.ID_FIND              '\&Find'
    wx.ID_REPLACE           'Find and rep\&lace'
    wx.ID_BACKWARD          '\&Back'
    wx.ID_DOWN              '\&Down'
    wx.ID_FORWARD           '\&Forward'
    wx.ID_UP                '\&Up'
    wx.ID_HELP              '\&Help'
    wx.ID_HOME              '\&Home'
    wx.ID_INDENT            'Indent'
    wx.ID_INDEX             '\&Index'
    wx.ID_ITALIC            '\&Italic'
    wx.ID_JUSTIFY_CENTER    'Centered'
    wx.ID_JUSTIFY_FILL      'Justified'
    wx.ID_JUSTIFY_LEFT      'Align Left'
    wx.ID_JUSTIFY_RIGHT     'Align Right'
    wx.ID_NEW               '\&New'
    wx.ID_NO                '\&No'
    wx.ID_OK                '\&OK'
    wx.ID_OPEN              '\&Open'
    wx.ID_PASTE             '\&Paste'
    wx.ID_PREFERENCES       '\&Preferences'
    wx.ID_PRINT             '\&Print'
    wx.ID_PREVIEW           'Print previe\&w'
    wx.ID_PROPERTIES        '\&Properties'
    wx.ID_EXIT              '\&Quit'
    wx.ID_REDO              '\&Redo'
    wx.ID_REFRESH           'Refresh'
    wx.ID_REMOVE            'Remove'
    wx.ID_REVERT_TO_SAVED   'Revert to Saved'
    wx.ID_SAVE              '\&Save'
    wx.ID_SAVEAS            'Save \&As...'
    wx.ID_STOP              '\&Stop'
    wx.ID_UNDELETE          'Undelete'
    wx.ID_UNDERLINE         '\&Underline'
    wx.ID_UNDO              '\&Undo'
    wx.ID_UNINDENT          '\&Unindent'
    wx.ID_YES               '\&Yes'
    wx.ID_ZOOM_100          '\&Actual Size'
    wx.ID_ZOOM_FIT          'Zoom to \&Fit'
    wx.ID_ZOOM_IN           'Zoom \&In'
    wx.ID_ZOOM_OUT          'Zoom \&Out'
    =====================   ======================
");

    DocCtorStrName(
        wxButton(),
        "Precreate a Button for 2-phase creation.", "",
        PreButton);

    // Turn it back on again
    %typemap(out) wxButton* { $result = wxPyMake_wxObject($1, $owner); }


    DocDeclStr(
        bool , Create(wxWindow* parent, wxWindowID id=-1,
                      const wxString& label=wxPyEmptyString,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = 0,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxPyButtonNameStr),
        "Acutally create the GUI Button for 2-phase creation.", "");
    


    DocDeclStr(
        void , SetDefault(),
        "This sets the button to be the default item for the panel or dialog box.", "");
    

    DocDeclStr(
        static wxSize , GetDefaultSize(),
        "Returns the default button size for this platform.", "");   

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};



//---------------------------------------------------------------------------


DocStr(wxBitmapButton,
"A Button that contains a bitmap.  A bitmap button can be supplied with a
single bitmap, and wxWidgets will draw all button states using this bitmap. If
the application needs more control, additional bitmaps for the selected state,
unpressed focused state, and greyed-out state may be supplied.", "       

Window Styles
-------------
    ==============  =============================================
    wx.BU_AUTODRAW  If this is specified, the button will be drawn
                    automatically using the label bitmap only,
                    providing a 3D-look border. If this style is
                    not specified, the button will be drawn
                    without borders and using all provided
                    bitmaps. WIN32 only.
    wx.BU_LEFT      Left-justifies the label. WIN32 only.
    wx.BU_TOP       Aligns the label to the top of the button. WIN32
                    only.
    wx.BU_RIGHT     Right-justifies the bitmap label. WIN32 only.
    wx.BU_BOTTOM    Aligns the label to the bottom of the
                    button. WIN32 only.
    wx.BU_EXACTFIT  Creates the button as small as possible
                    instead of making it of the standard size
                    (which is the default behaviour.)
    ==============  =============================================

Events
------
     ===========   ==================================
     EVT_BUTTON    Sent when the button is clicked.
     ===========   ==================================

:see: `wx.Button`, `wx.Bitmap`
");

MustHaveApp(wxBitmapButton);

class wxBitmapButton : public wxButton
{
public:
    %pythonAppend wxBitmapButton         "self._setOORInfo(self)"
    %pythonAppend wxBitmapButton()       ""
    %typemap(out) wxBitmapButton*;    // turn off this typemap

    DocCtorStr(
        wxBitmapButton(wxWindow* parent, wxWindowID id=-1,
                       const wxBitmap& bitmap = wxNullBitmap,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxBU_AUTODRAW,
                       const wxValidator& validator = wxDefaultValidator,
                       const wxString& name = wxPyButtonNameStr),
        "Create and show a button with a bitmap for the label.", "");

    DocCtorStrName(
        wxBitmapButton(),
        "Precreate a BitmapButton for 2-phase creation.", "",
        PreBitmapButton);

    // Turn it back on again
    %typemap(out) wxBitmapButton* { $result = wxPyMake_wxObject($1, $owner); }


    DocDeclStr(
        bool , Create(wxWindow* parent, wxWindowID id=-1,
                      const wxBitmap& bitmap = wxNullBitmap,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxBU_AUTODRAW,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxPyButtonNameStr),
        "Acutally create the GUI BitmapButton for 2-phase creation.", "");
    

    DocDeclStr(
        wxBitmap , GetBitmapLabel(),
        "Returns the label bitmap (the one passed to the constructor).", "");
    
    DocDeclStr(
        wxBitmap , GetBitmapDisabled(),
        "Returns the bitmap for the disabled state.", "");
    
    DocDeclStr(
        wxBitmap , GetBitmapFocus(),
        "Returns the bitmap for the focused state.", "");
    

    DocDeclStr(
        wxBitmap , GetBitmapSelected(),
        "Returns the bitmap for the selected state.", "");
    
    DocDeclStr(
        wxBitmap , GetBitmapHover(),
        "Returns the bitmap used when the mouse is over the button, may be invalid.", "");
   

    DocDeclStr(
        void , SetBitmapDisabled(const wxBitmap& bitmap),
        "Sets the bitmap for the disabled button appearance.", "");
    

    DocDeclStr(
        void , SetBitmapFocus(const wxBitmap& bitmap),
        "Sets the bitmap for the button appearance when it has the keyboard focus.", "");
    

    DocDeclStr(
        void , SetBitmapSelected(const wxBitmap& bitmap),
        "Sets the bitmap for the selected (depressed) button appearance.", "");
    

    DocDeclStr(
        void , SetBitmapLabel(const wxBitmap& bitmap),
        "Sets the bitmap label for the button.  This is the bitmap used for the
unselected state, and for all other states if no other bitmaps are provided.", "");

    
    DocDeclStr(
        void , SetBitmapHover(const wxBitmap& hover),
        "Sets the bitmap to be shown when the mouse is over the button.  This function
is new since wxWidgets version 2.7.0 and the hover bitmap is currently only
supported in wxMSW.", "");
    
    void SetMargins(int x, int y);
    int GetMarginX() const;
    int GetMarginY() const;
    
    %property(BitmapDisabled, GetBitmapDisabled, SetBitmapDisabled, doc="See `GetBitmapDisabled` and `SetBitmapDisabled`");
    %property(BitmapFocus, GetBitmapFocus, SetBitmapFocus, doc="See `GetBitmapFocus` and `SetBitmapFocus`");
    %property(BitmapHover, GetBitmapHover, SetBitmapHover, doc="See `GetBitmapHover` and `SetBitmapHover`");
    %property(BitmapLabel, GetBitmapLabel, SetBitmapLabel, doc="See `GetBitmapLabel` and `SetBitmapLabel`");
    %property(BitmapSelected, GetBitmapSelected, SetBitmapSelected, doc="See `GetBitmapSelected` and `SetBitmapSelected`");
    %property(MarginX, GetMarginX, doc="See `GetMarginX`");
    %property(MarginY, GetMarginY, doc="See `GetMarginY`");
};


//---------------------------------------------------------------------------
