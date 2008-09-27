/////////////////////////////////////////////////////////////////////////////
// Name:        spinctrg.h
// Purpose:     interface of wxSpinCtrlDouble
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxSpinCtrlDouble

    wxSpinCtrlDouble combines wxTextCtrl and wxSpinButton in one control and
    displays a real number. (wxSpinCtrl displays an integer.)

    @beginStyleTable
    @style{wxSP_ARROW_KEYS}
           The user can use arrow keys to change the value.
    @style{wxSP_WRAP}
           The value wraps at the minimum and maximum.
    @endStyleTable

    @library{wxcore}
    @category{ctrl}
    <!-- @appearance{spinctrd.png} -->

    @see wxSpinButton, wxSpinCtrl, wxControl
*/
class wxSpinCtrlDouble : public wxControl
{
public:
    /**
       Default constructor.
    */
    wxSpinCtrlDouble();
    
    /**
        Constructor, creating and showing a spin control.

        @param parent
            Parent window. Must not be @NULL.
        @param value
            Default value (as text).
        @param id
            Window identifier. The value wxID_ANY indicates a default value.
        @param pos
            Window position. If wxDefaultPosition is specified then a default
        position is chosen.
        @param size
            Window size. If wxDefaultSize is specified then a default size
        is chosen.
        @param style
            Window style. See wxSpinButton.
        @param min
            Minimal value.
        @param max
            Maximal value.
        @param initial
            Initial value.
        @param inc
            Increment value.
        @param name
            Window name.

        @see Create()
    */
    wxSpinCtrlDouble(wxWindow* parent, wxWindowID id = -1,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               double min = 0, double max = 100,
               double initial = 0, double inc = 1,
               const wxString& name = _T("wxSpinCtrlDouble"));

    /**
        Creation function called by the spin control constructor.
        See wxSpinCtrlDouble() for details.
    */
    bool Create(wxWindow* parent, wxWindowID id = -1,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS,
                double min = 0, double max = 100,
                double initial = 0, double inc = 1,
                const wxString& name = _T("wxSpinCtrlDouble"));

    /**
        Gets the number of digits in the display.
    */
    unsigned GetDigits() const;

    /**
        Gets the increment value.
    */
    double GetIncrement() const;

    /**
        Gets maximal allowable value.
    */
    double GetMax() const;

    /**
        Gets minimal allowable value.
    */
    double GetMin() const;

    /**
        Gets the value of the spin control.
    */
    double GetValue() const;

    /**
        Sets the number of digits in the display.
    */
    void SetDigits(unsigned digits);

    /**
        Sets the increment value.
    */
    void SetIncrement(double inc);

    /**
        Sets range of allowable values.
    */
    void SetRange(double minVal, double maxVal);

    /**
        Sets the value of the spin control. Use the variant using double instead.
    */
    virtual void SetValue(const wxString& text);

    /**
        Sets the value of the spin control.
    */
    void SetValue(double value);
};

