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
    wxBU_AUTODRAW,
};

//---------------------------------------------------------------------------

DocStr(wxButton,
       "A button is a control that contains a text string, and is one of the most\n"
       "common elements of a GUI.  It may be placed on a dialog box or panel, or\n"
       "indeed almost any other window.");

RefDoc(wxButton,  "
 Styles
    wxBU_LEFT:     Left-justifies the label. WIN32 only.
    wxBU_TOP:      Aligns the label to the top of the button. WIN32 only.
    wxBU_RIGHT:    Right-justifies the bitmap label. WIN32 only.
    wxBU_BOTTOM:   Aligns the label to the bottom of the button. WIN32 only.
    wxBU_EXACTFIT: Creates the button as small as possible instead of making
                   it of the standard size (which is the default behaviour.)

 Events
     EVT_BUTTON(win,id,func):
         Sent when the button is clicked.
");

class wxButton : public wxControl
{
public:
    %addtofunc wxButton         "self._setOORInfo(self)"
    %addtofunc wxButton()       ""


    DocStr(wxButton, "Create and show a button.")
    wxButton(wxWindow* parent, wxWindowID id, const wxString& label,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxPyButtonNameStr);

    DocStr(wxButton(), "Precreate a Button for 2-phase creation.");
    %name(PreButton)wxButton();

    DocStr(Create, "Acutally create the GUI Button for 2-phase creation.");
    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxPyButtonNameStr);


    DocStr(SetDefault, "This sets the button to be the default item for the panel or dialog box.");
    void SetDefault();


#ifdef __WXMSW__
    // show the image in the button in addition to the label
    void SetImageLabel(const wxBitmap& bitmap);

    // set the margins around the image
    void SetImageMargins(wxCoord x, wxCoord y);
#endif

    DocStr(GetDefaultButtonSize, "Returns the default button size for this platform.");
    static wxSize GetDefaultSize();
};



//---------------------------------------------------------------------------


DocStr(wxBitmapButton, "A Buttont that contains a bitmap.");

class wxBitmapButton : public wxButton
{
public:
    %addtofunc wxBitmapButton         "self._setOORInfo(self)"
    %addtofunc wxBitmapButton()       ""

    DocStr(wxBitmapButton, "Create and show a button.")
    wxBitmapButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyButtonNameStr);

    DocStr(wxBitmapButton(), "Precreate a BitmapButton for 2-phase creation.");
    %name(PreBitmapButton)wxBitmapButton();

    DocStr(Create, "Acutally create the GUI BitmapButton for 2-phase creation.");
    bool Create(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxBU_AUTODRAW,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyButtonNameStr);

    DocStr(GetBitmapLabel, "Returns the label bitmap (the one passed to the constructor).");
    wxBitmap GetBitmapLabel();

    DocStr(GetBitmapDisabled, "Returns the bitmap for the disabled state.");
    wxBitmap GetBitmapDisabled();

    DocStr(GetBitmapFocus, "Returns the bitmap for the focused state.");
    wxBitmap GetBitmapFocus();

    DocStr(GetBitmapSelected, "Returns the bitmap for the selected state.");
    wxBitmap GetBitmapSelected();

    DocStr(SetBitmapDisabled, "Sets the bitmap for the disabled button appearance.");
    void SetBitmapDisabled(const wxBitmap& bitmap);

    DocStr(SetBitmapFocus, "Sets the bitmap for the button appearance when it has the keyboard focus.");
    void SetBitmapFocus(const wxBitmap& bitmap);

    DocStr(SetBitmapSelected, "Sets the bitmap for the selected (depressed) button appearance.");
    void SetBitmapSelected(const wxBitmap& bitmap);

    DocStr(SetBitmapLabel,
           "Sets the bitmap label for the button.  This is the bitmap used for the\n"
           "unselected state, and for all other states if no other bitmaps are provided.");
    void SetBitmapLabel(const wxBitmap& bitmap);

    void SetMargins(int x, int y);
    int GetMarginX() const;
    int GetMarginY() const;
};


//---------------------------------------------------------------------------
