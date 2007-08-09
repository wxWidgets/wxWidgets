/////////////////////////////////////////////////////////////////////////////
// Name:        _checkbox.i
// Purpose:     SWIG interface defs for wxCheckBox
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

MAKE_CONST_WXSTRING(CheckBoxNameStr);


enum {
    // Determine whether to use a 3-state or 2-state
    // checkbox. 3-state enables to differentiate
    // between 'unchecked', 'checked' and 'undetermined'.
    wxCHK_2STATE,
    wxCHK_3STATE,


    // If this style is set the user can set the checkbox to the
    // undetermined state. If not set the undetermined set can only
    // be set programmatically.
    // This style can only be used with 3 state checkboxes.    
    wxCHK_ALLOW_3RD_STATE_FOR_USER,
};

enum wxCheckBoxState
{
    wxCHK_UNCHECKED,
    wxCHK_CHECKED,
    wxCHK_UNDETERMINED /* 3-state checkbox only */
};


//---------------------------------------------------------------------------

DocStr(wxCheckBox,
"A checkbox is a labelled box which by default is either on (the
checkmark is visible) or off (no checkmark). Optionally (When the
wx.CHK_3STATE style flag is set) it can have a third state, called the
mixed or undetermined state. Often this is used as a \"Does Not
Apply\" state.", "

Window Styles
-------------
    =================================  ===============================
    wx.CHK_2STATE                      Create a 2-state checkbox. 
                                       This is the default.
    wx.CHK_3STATE                      Create a 3-state checkbox.
    wx.CHK_ALLOW_3RD_STATE_FOR_USER    By default a user can't set a
                                       3-state checkbox to the
                                       third state. It can only be
                                       done from code. Using this
                                       flags allows the user to set
                                       the checkbox to the third
                                       state by clicking.
                                       wx.ALIGN_RIGHT Makes the
                                       text appear on the left of
                                       the checkbox.
    =================================  ===============================

Events
------
    ===============================  ===============================
    EVT_CHECKBOX                     Sent when checkbox is clicked.
    ===============================  ===============================
");


        
MustHaveApp(wxCheckBox);

class wxCheckBox : public wxControl
{
public:
    %pythonAppend wxCheckBox         "self._setOORInfo(self)"
    %pythonAppend wxCheckBox()       ""

    DocCtorStr(
        wxCheckBox(wxWindow* parent, wxWindowID id=-1,
                   const wxString& label = wxPyEmptyString,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyCheckBoxNameStr),
        "Creates and shows a CheckBox control", "");

    DocCtorStrName(
        wxCheckBox(),
        "Precreate a CheckBox for 2-phase creation.", "",
        PreCheckBox);

    
    DocDeclStr(
        bool, Create(wxWindow* parent, wxWindowID id=-1,
                     const wxString& label = wxPyEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = 0,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxPyCheckBoxNameStr),
        "Actually create the GUI CheckBox for 2-phase creation.", "");

    
    DocDeclStr(
        bool, GetValue(),
        "Gets the state of a 2-state CheckBox.  Returns True if it is checked,
False otherwise.", "");
    
    DocDeclStr(
        bool, IsChecked(),
        "Similar to GetValue, but raises an exception if it is not a 2-state
CheckBox.", "");
    
    DocDeclStr(
        void, SetValue(const bool state),
        "Set the state of a 2-state CheckBox.  Pass True for checked, False for
unchecked.", "");
    
    DocDeclStr(
        wxCheckBoxState, Get3StateValue() const,
        "Returns wx.CHK_UNCHECKED when the CheckBox is unchecked,
wx.CHK_CHECKED when it is checked and wx.CHK_UNDETERMINED when it's in
the undetermined state.  Raises an exceptiion when the function is
used with a 2-state CheckBox.", "");
    
    DocDeclStr(
        void, Set3StateValue(wxCheckBoxState state),
        "Sets the CheckBox to the given state.  The state parameter can be one
of the following: wx.CHK_UNCHECKED (Check is off), wx.CHK_CHECKED (the
Check is on) or wx.CHK_UNDETERMINED (Check is mixed). Raises an
exception when the CheckBox is a 2-state checkbox and setting the
state to wx.CHK_UNDETERMINED.", "");
    
    DocDeclStr(
        bool, Is3State() const,
        "Returns whether or not the CheckBox is a 3-state CheckBox.", "");
    
    DocDeclStr(
        bool, Is3rdStateAllowedForUser() const,
        "Returns whether or not the user can set the CheckBox to the third
state.", "");
    
    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    %property(ThreeStateValue, Get3StateValue, Set3StateValue, doc="See `Get3StateValue` and `Set3StateValue`");
    %property(Value, GetValue, SetValue, doc="See `GetValue` and `SetValue`");
    
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
