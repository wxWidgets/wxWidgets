/////////////////////////////////////////////////////////////////////////////
// Name:        _spin.i
// Purpose:     SWIG interface defs for wxSpinButton and wxSpinCtrl
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
    DECLARE_DEF_STRING(SPIN_BUTTON_NAME);
    wxChar* wxSpinCtrlNameStr = _T("wxSpinCtrl");
    DECLARE_DEF_STRING(SpinCtrlNameStr);
%}

//---------------------------------------------------------------------------
%newgroup


enum {
    wxSP_HORIZONTAL,
    wxSP_VERTICAL,
    wxSP_ARROW_KEYS,
    wxSP_WRAP
};


//  The wxSpinButton is like a small scrollbar than is often placed next
//  to a text control.
//
//  Styles:
//  wxSP_HORIZONTAL:   horizontal spin button
//  wxSP_VERTICAL:     vertical spin button (the default)
//  wxSP_ARROW_KEYS:   arrow keys increment/decrement value
//  wxSP_WRAP:         value wraps at either end
class wxSpinButton : public wxControl
{
public:
    %addtofunc wxSpinButton         "self._setOORInfo(self)"
    %addtofunc wxSpinButton()       ""

    wxSpinButton(wxWindow* parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_HORIZONTAL,
                 const wxString& name = wxPySPIN_BUTTON_NAME);
    %name(PreSpinButton)wxSpinButton();

    bool Create(wxWindow* parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_HORIZONTAL,
                 const wxString& name = wxPySPIN_BUTTON_NAME);

    virtual int GetValue() const;
    virtual int GetMin() const;
    virtual int GetMax() const;

    virtual void SetValue(int val);
    virtual void SetMin(int minVal);
    virtual void SetMax(int maxVal);
    virtual void SetRange(int minVal, int maxVal);

    // is this spin button vertically oriented?
    bool IsVertical() const;
};


//---------------------------------------------------------------------------


// a spin ctrl is a text control with a spin button which is usually used to
// prompt the user for a numeric input

class wxSpinCtrl : public wxControl
{
public:
    %addtofunc wxSpinCtrl         "self._setOORInfo(self)"
    %addtofunc wxSpinCtrl()       ""

    wxSpinCtrl(wxWindow *parent,
               wxWindowID id = -1,
               const wxString& value = wxPyEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const wxString& name = wxPySpinCtrlNameStr);
    %name(PreSpinCtrl)wxSpinCtrl();

    bool Create(wxWindow *parent,
               wxWindowID id = -1,
               const wxString& value = wxPyEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const wxString& name = wxPySpinCtrlNameStr);

    virtual int GetValue() const;
    virtual void SetValue( int value );
    %name(SetValueString) void SetValue(const wxString& text);

    virtual void SetRange( int minVal, int maxVal );
    virtual int GetMin() const;
    virtual int GetMax() const;
#ifdef __WXGTK__
    %extend {
        void SetSelection(long from, long to) {
        }
    }
#else
    void SetSelection(long from, long to);
#endif
};

enum {
    wxEVT_COMMAND_SPINCTRL_UPDATED
};


%pythoncode {
EVT_SPINCTRL = wx.PyEventBinder( wxEVT_COMMAND_SPINCTRL_UPDATED, 1)
}


//---------------------------------------------------------------------------
