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

%{
#include <wx/tglbtn.h>

    wxChar* wxToggleButtonNameStr = _T("wxToggleButton");
    DECLARE_DEF_STRING(ToggleButtonNameStr);

%}

//---------------------------------------------------------------------------
%newgroup


%{
#ifdef __WXMAC__
// implement dummy classes and such for wxMac

#define wxEVT_COMMAND_TOGGLEBUTTON_CLICKED 0
class wxToggleButton : public wxControl
{
public:
    wxToggleButton(wxWindow *, wxWindowID, const wxString&,
                   const wxPoint&, const wxSize&, long,
                   const wxValidator&, const wxString&)
        { PyErr_SetNone(PyExc_NotImplementedError); }
    
    wxToggleButton()
        { PyErr_SetNone(PyExc_NotImplementedError); }
};
#endif
%}



enum { wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, };

%pythoncode {
    EVT_TOGGLEBUTTON = wx.PyEventBinder( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, 1)
}


class wxToggleButton : public wxControl
{
public:
    %addtofunc wxToggleButton         "self._setOORInfo(self)"
    %addtofunc wxToggleButton()       ""

    wxToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyToggleButtonNameStr);
    %name(PreToggleButton)wxToggleButton();

    bool Create(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyToggleButtonNameStr);

#ifndef __WXMAC__
    void SetValue(bool value);
    bool GetValue() const ;
    void SetLabel(const wxString& label);
#endif
};

//---------------------------------------------------------------------------
