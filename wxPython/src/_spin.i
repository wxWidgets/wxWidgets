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

MAKE_CONST_WXSTRING(SPIN_BUTTON_NAME);
MAKE_CONST_WXSTRING2(SpinCtrlNameStr, _T("wxSpinCtrl"));

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
MustHaveApp(wxSpinButton);
class wxSpinButton : public wxControl
{
public:
    %pythonAppend wxSpinButton         "self._setOORInfo(self)"
    %pythonAppend wxSpinButton()       ""

    wxSpinButton(wxWindow* parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_HORIZONTAL,
                 const wxString& name = wxPySPIN_BUTTON_NAME);
    %RenameCtor(PreSpinButton, wxSpinButton());

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

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    %property(Max, GetMax, SetMax, doc="See `GetMax` and `SetMax`");
    %property(Min, GetMin, SetMin, doc="See `GetMin` and `SetMin`");
    %property(Value, GetValue, SetValue, doc="See `GetValue` and `SetValue`");
};


//---------------------------------------------------------------------------


// a spin ctrl is a text control with a spin button which is usually used to
// prompt the user for a numeric input

MustHaveApp(wxSpinCtrl);

class wxSpinCtrl : public wxControl
{
public:
    %pythonAppend wxSpinCtrl         "self._setOORInfo(self)"
    %pythonAppend wxSpinCtrl()       ""

    wxSpinCtrl(wxWindow *parent,
               wxWindowID id = -1,
               const wxString& value = wxPyEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const wxString& name = wxPySpinCtrlNameStr);
    %RenameCtor(PreSpinCtrl, wxSpinCtrl());

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
    %Rename(SetValueString,  void, SetValue(const wxString& text));

    virtual void SetRange( int minVal, int maxVal );
    virtual int GetMin() const;
    virtual int GetMax() const;
    void SetSelection(long from, long to);

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    %property(Max, GetMax, doc="See `GetMax`");
    %property(Min, GetMin, doc="See `GetMin`");
    %property(Value, GetValue, SetValue, doc="See `GetValue` and `SetValue`");
};


//---------------------------------------------------------------------------

class wxSpinEvent : public wxNotifyEvent
{
public:
    wxSpinEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);

    // get the current value of the control
    int GetPosition() const;
    void SetPosition(int pos);

    %property(Position, GetPosition, SetPosition, doc="See `GetPosition` and `SetPosition`");
};


%constant wxEventType wxEVT_COMMAND_SPINCTRL_UPDATED;


%pythoncode {
EVT_SPIN_UP   = wx.PyEventBinder( wx.wxEVT_SCROLL_LINEUP, 1)
EVT_SPIN_DOWN = wx.PyEventBinder( wx.wxEVT_SCROLL_LINEDOWN, 1)
EVT_SPIN      = wx.PyEventBinder( wx.wxEVT_SCROLL_THUMBTRACK, 1)
EVT_SPINCTRL  = wx.PyEventBinder( wxEVT_COMMAND_SPINCTRL_UPDATED, 1)
}


//---------------------------------------------------------------------------
