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

%{
    DECLARE_DEF_STRING(CheckBoxNameStr);
%}


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

class wxCheckBox : public wxControl
{
public:
    %addtofunc wxCheckBox         "self._setOORInfo(self)"
    %addtofunc wxCheckBox()       ""
    
    wxCheckBox(wxWindow* parent, wxWindowID id, const wxString& label,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxPyCheckBoxNameStr);
    %name(PreCheckBox)wxCheckBox();

    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxPyCheckBoxNameStr);

    bool GetValue();
    bool IsChecked();
    void SetValue(const bool state);
    wxCheckBoxState Get3StateValue() const;
    void Set3StateValue(wxCheckBoxState state);
    bool Is3State() const;
    bool Is3rdStateAllowedForUser() const;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
