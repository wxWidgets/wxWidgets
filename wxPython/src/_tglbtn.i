/////////////////////////////////////////////////////////////////////////////
// Name:        _tglbtn.i
// Purpose:     SWIG interface defs for wxToggleButton
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

MAKE_CONST_WXSTRING2(ToggleButtonNameStr, _T("wxToggleButton"));

//---------------------------------------------------------------------------
%newgroup


%{
#if !wxUSE_TOGGLEBTN
// implement dummy items for platforms that don't have this class

#define wxEVT_COMMAND_TOGGLEBUTTON_CLICKED 0
    
class wxToggleButton : public wxControl
{
public:
    wxToggleButton(wxWindow *, wxWindowID, const wxString&,
                   const wxPoint&, const wxSize&, long,
                   const wxValidator&, const wxString&)
        { wxPyRaiseNotImplemented(); }
    
    wxToggleButton()
        { wxPyRaiseNotImplemented(); }
};
#endif
%}



%constant wxEventType wxEVT_COMMAND_TOGGLEBUTTON_CLICKED;

%pythoncode {
    EVT_TOGGLEBUTTON = wx.PyEventBinder( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, 1)
}


MustHaveApp(wxToggleButton);

class wxToggleButton : public wxControl
{
public:
    %pythonAppend wxToggleButton         "self._setOORInfo(self)"
    %pythonAppend wxToggleButton()       ""

    wxToggleButton(wxWindow *parent,
                   wxWindowID id=-1,
                   const wxString& label = wxPyEmptyString,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyToggleButtonNameStr);
    %RenameCtor(PreToggleButton, wxToggleButton());

    bool Create(wxWindow *parent,
                   wxWindowID id=-1,
                   const wxString& label = wxPyEmptyString,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyToggleButtonNameStr);

    void SetValue(bool value);
    bool GetValue() const ;
    void SetLabel(const wxString& label);

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    %property(Value, GetValue, SetValue, doc="See `GetValue` and `SetValue`");
};

//---------------------------------------------------------------------------
