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


%{
    DECLARE_DEF_STRING(ButtonNameStr);
%}

enum {
    wxBU_LEFT,
    wxBU_TOP,
    wxBU_RIGHT,
    wxBU_BOTTOM,

    wxBU_EXACTFIT,
};

//---------------------------------------------------------------------------

// A button is a control that contains a text string, and is one of the most
// common elements of a GUI.  It may be placed on a dialog box or panel, or
// indeed almost any other window.
//
// Styles
//    wxBU_LEFT:  Left-justifies the label. WIN32 only.
//    wxBU_TOP:  Aligns the label to the top of the button. WIN32 only.
//    wxBU_RIGHT:  Right-justifies the bitmap label. WIN32 only.
//    wxBU_BOTTOM:  Aligns the label to the bottom of the button. WIN32 only.
//    wxBU_EXACTFIT: Creates the button as small as possible instead of making
//                   it of the standard size (which is the default behaviour.)
//
// Events
//     EVT_BUTTON(win,id,func):
//         Sent when the button is clicked.
//
class wxButton : public wxControl
{
public:
    %addtofunc wxButton         "self._setOORInfo(self)"
    %addtofunc wxButton()       ""


    // Constructor, creating and showing a button.
    //
    // parent:  Parent window.  Must not be None.
    // id:      Button identifier.  A value of -1 indicates a default value.
    // label:   The text to be displayed on the button.
    // pos:     The button position on it's parent.
    // size:    Button size.  If the default size (-1, -1) is specified then the
    //          button is sized appropriately for the text.
    // style:   Window style.  See wxButton.
    // validator: Window validator.
    // name:    Window name.
    wxButton(wxWindow* parent, wxWindowID id, const wxString& label,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxPyButtonNameStr);

    // Default constructor
    %name(PreButton)wxButton();

    // Button creation function for two-step creation.
    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxPyButtonNameStr);


    // This sets the button to be the default item for the panel or dialog box.
    //
    // Under Windows, only dialog box buttons respond to this function. As
    // normal under Windows and Motif, pressing return causes the default
    // button to be depressed when the return key is pressed. See also
    // wxWindow.SetFocus which sets the keyboard focus for windows and text
    // panel items, and wxPanel.SetDefaultItem.
    void SetDefault();


#ifdef __WXMSW__
    // show the image in the button in addition to the label
    void SetImageLabel(const wxBitmap& bitmap);

    // set the margins around the image
    void SetImageMargins(wxCoord x, wxCoord y);
#endif

    // returns the default button size for this platform
    static wxSize GetDefaultSize();
};



//---------------------------------------------------------------------------

class wxBitmapButton : public wxButton
{
public:
    %addtofunc wxBitmapButton         "self._setOORInfo(self)"
    %addtofunc wxBitmapButton()       ""

    wxBitmapButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyButtonNameStr);
    %name(PreBitmapButton)wxBitmapButton();

    bool Create(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyButtonNameStr);

    wxBitmap GetBitmapLabel();
    wxBitmap GetBitmapDisabled();
    wxBitmap GetBitmapFocus();
    wxBitmap GetBitmapSelected();
    void SetBitmapDisabled(const wxBitmap& bitmap);
    void SetBitmapFocus(const wxBitmap& bitmap);
    void SetBitmapSelected(const wxBitmap& bitmap);
    void SetBitmapLabel(const wxBitmap& bitmap);

    void SetMargins(int x, int y);
    int GetMarginX() const;
    int GetMarginY() const;
};


//---------------------------------------------------------------------------
