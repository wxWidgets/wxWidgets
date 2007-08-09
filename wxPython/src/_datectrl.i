/////////////////////////////////////////////////////////////////////////////
// Name:        _datectrl.i
// Purpose:     SWIG interface defs for wxwxDatePickerCtrl
//
// Author:      Robin Dunn
//
// Created:     15-Feb-2005
// RCS-ID:      $Id$
// Copyright:   (c) 2005 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING(DatePickerCtrlNameStr);

//---------------------------------------------------------------------------
%newgroup

DocStr(wxDatePickerCtrl,
"This control allows the user to select a date. Unlike
`wx.calendar.CalendarCtrl`, which is a relatively big control,
`wx.DatePickerCtrl` is implemented as a small window showing the
currently selected date. The control can be edited using the keyboard,
and can also display a popup window for more user-friendly date
selection, depending on the styles used and the platform.", "

Styles
------
    =================  ======================================================
    wx.DP_SPIN         Creates a control without month calendar drop down but
                       with spin control-like arrows to change individual
                       date components. This style is not supported by the
                       generic version.

    wx.DP_DROPDOWN     Creates a control with a month calendar drop down
                       part from which the user can select a date.

    wx.DP_DEFAULT      Creates a control with default style which is the
                       best supported for the current platform
                       (currently wx.DP_SPIN under Windows and
                       wx.DP_DROPDOWN elsewhere).

    wx.DP_ALLOWNONE    With this style, the control allows the user to not
                       enter any valid date at all. Without it -- which
                       is by default -- the control always has some
                       valid date.

    wx.DP_SHOWCENTURY  Forces display of the century in the default
                       date format. Without this flas the century
                       could be displayed or not depending on the
                       default date representation in the system.
    =================  ======================================================

Events
------

    =================  ======================================================
    EVT_DATE_CHANGED   This event fires when the user changes the current
                       selection in the control.
    =================  ======================================================

:see: `wx.calendar.CalendarCtrl`, `wx.DateEvent`
");




enum
{
    wxDP_DEFAULT = 0,
    wxDP_SPIN = 1,
    wxDP_DROPDOWN = 2,
    wxDP_SHOWCENTURY = 4,
    wxDP_ALLOWNONE = 8
};


MustHaveApp(wxDatePickerCtrl);

class wxDatePickerCtrl : public wxControl
{
public:
    %pythonAppend wxDatePickerCtrl         "self._setOORInfo(self)";
    %pythonAppend wxDatePickerCtrl()       "";

    DocCtorStr(
        wxDatePickerCtrl(wxWindow *parent,
                         wxWindowID id=-1,
                         const wxDateTime& dt = wxDefaultDateTime,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = wxDP_DEFAULT | wxDP_SHOWCENTURY,
                         const wxValidator& validator = wxDefaultValidator,
                         const wxString& name = wxPyDatePickerCtrlNameStr),
        "Create a new DatePickerCtrl.", "");

    
    DocCtorStrName(
        wxDatePickerCtrl(),
        "Precreate a DatePickerCtrl for use in 2-phase creation.", "",
        PreDatePickerCtrl);
    

    DocDeclStr(
        bool , Create(wxWindow *parent,
                      wxWindowID id=-1,
                      const wxDateTime& dt = wxDefaultDateTime,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxDP_DEFAULT | wxDP_SHOWCENTURY,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxPyDatePickerCtrlNameStr),
        "Create the GUI parts of the DatePickerCtrl, for use in 2-phase
creation.", "");
    


    DocDeclStr(
        virtual void , SetValue(const wxDateTime& dt),
        "Changes the current value of the control. The date should be valid and
included in the currently selected range, if any.

Calling this method does not result in a date change event.", "");
    
    DocDeclStr(
        virtual wxDateTime , GetValue() const,
        "Returns the currently selected date. If there is no selection or the
selection is outside of the current range, an invalid `wx.DateTime`
object is returned.", "");
    

    DocDeclStr(
        virtual void , SetRange(const wxDateTime& dt1, const wxDateTime& dt2),
        "Sets the valid range for the date selection. If dt1 is valid, it
becomes the earliest date (inclusive) accepted by the control. If dt2
is valid, it becomes the latest possible date.

If the current value of the control is outside of the newly set range
bounds, the behaviour is undefined.", "");
    
    // virtual bool GetRange(wxDateTime *dt1, wxDateTime *dt2) const;

    %extend {
        DocStr(
            GetLowerLimit,
            "Get the lower limit of the valid range for the date selection, if any.
If there is no range or there is no lower limit, then the
`wx.DateTime` value returned will be invalid.", "");
        wxDateTime GetLowerLimit() {
            wxDateTime rv;
            self->GetRange(&rv, NULL);
            return rv;
        }

        
        DocStr(
            GetUpperLimit,
            "Get the upper limit of the valid range for the date selection, if any.
If there is no range or there is no upper limit, then the
`wx.DateTime` value returned will be invalid.", "");
        wxDateTime GetUpperLimit() {
            wxDateTime rv;
            self->GetRange(NULL, &rv);
            return rv;
        }
    }
    
    %property(LowerLimit, GetLowerLimit, doc="See `GetLowerLimit`");
    %property(UpperLimit, GetUpperLimit, doc="See `GetUpperLimit`");
    %property(Value, GetValue, SetValue, doc="See `GetValue` and `SetValue`");
};




//---------------------------------------------------------------------------


