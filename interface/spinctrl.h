/////////////////////////////////////////////////////////////////////////////
// Name:        spinctrl.h
// Purpose:     interface of wxSpinCtrl
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxSpinCtrl
    @wxheader{spinctrl.h}

    wxSpinCtrl combines wxTextCtrl and
    wxSpinButton in one control.

    @beginStyleTable
    @style{wxSP_ARROW_KEYS}
           The user can use arrow keys to change the value.
    @style{wxSP_WRAP}
           The value wraps at the minimum and maximum.
    @endStyleTable

    @library{wxcore}
    @category{ctrl}
    <!-- @appearance{spinctrl.png} -->

    @see wxSpinButton, wxControl
*/
class wxSpinCtrl : public wxControl
{
public:
    /**
       Default constructor.
    */
    wxSpinCtrl();
    
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
        @param name
            Window name.

        @see Create()
    */
    wxSpinCtrl(wxWindow* parent, wxWindowID id = -1,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100,
               int initial = 0);

    /**
        Creation function called by the spin control constructor.
        See wxSpinCtrl() for details.
    */
    bool Create(wxWindow* parent, wxWindowID id = -1,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS,
                int min = 0, int max = 100,
                int initial = 0);

    /**
        Gets maximal allowable value.
    */
    int GetMax() const;

    /**
        Gets minimal allowable value.
    */
    int GetMin() const;

    /**
        Gets the value of the spin control.
    */
    int GetValue() const;

    /**
        Sets range of allowable values.
    */
    void SetRange(int minVal, int maxVal);

    /**
        Select the text in the text part of the control between  positions
        @a from (inclusive) and @a to (exclusive). This is similar to
        wxTextCtrl::SetSelection.
        @note this is currently only implemented for Windows and generic versions
        of the control.
    */
    void SetSelection(long from, long to);

    /**
        Sets the value of the spin control. Use the variant using int instead.
    */
    void SetValue(const wxString& text);

    /**
        Sets the value of the spin control.
    */
    void SetValue(int value);
};

