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
    wx.BU_LEFT       Left-justifies the label. WIN32 only.
    wx.BU_TOP        Aligns the label to the top of the button.
                     WIN32 only.
    wx.BU_RIGHT      Right-justifies the bitmap label. WIN32 only.
    wx.BU_BOTTOM     Aligns the label to the bottom of the button.
                     WIN32 only.
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


    DocCtorStr(
        wxButton(wxWindow* parent, wxWindowID id=-1,
                 const wxString& label=wxPyEmptyString,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxPyButtonNameStr),
        "Create and show a button.", "");

    DocCtorStrName(
        wxButton(),
        "Precreate a Button for 2-phase creation.", "",
        PreButton);

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
    

    void SetMargins(int x, int y);
    int GetMarginX() const;
    int GetMarginY() const;
};


//---------------------------------------------------------------------------
