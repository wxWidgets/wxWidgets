/////////////////////////////////////////////////////////////////////////////
// Name:        slider.h
// Purpose:     interface of wxSlider
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxSlider

    A slider is a control with a handle which can be pulled
    back and forth to change the value.

    On Windows, the track bar control is used.

    Slider events are handled in the same way as a scrollbar.

    @beginStyleTable
    @style{wxSL_HORIZONTAL}
           Displays the slider horizontally (this is the default).
    @style{wxSL_VERTICAL}
           Displays the slider vertically.
    @style{wxSL_AUTOTICKS}
           Displays tick marks.
    @style{wxSL_LABELS}
           Displays minimum, maximum and value labels.
    @style{wxSL_LEFT}
           Displays ticks on the left and forces the slider to be vertical.
    @style{wxSL_RIGHT}
           Displays ticks on the right and forces the slider to be vertical.
    @style{wxSL_TOP}
           Displays ticks on the top.
    @style{wxSL_BOTTOM}
           Displays ticks on the bottom (this is the default).
    @style{wxSL_SELRANGE}
           Allows the user to select a range on the slider. Windows only.
    @style{wxSL_INVERSE}
           Inverses the mininum and maximum endpoints on the slider. Not
           compatible with wxSL_SELRANGE.
    @endStyleTable

    @library{wxcore}
    @category{ctrl}
    <!-- @appearance{slider.png} -->

    @see @ref overview_eventhandlingoverview, wxScrollBar
*/
class wxSlider : public wxControl
{
public:
    /**
       Default constructor
    */
    wxSlider();
    
    /**
        Constructor, creating and showing a slider.

        @param parent
            Parent window. Must not be @NULL.
        @param id
            Window identifier. The value wxID_ANY indicates a default value.
        @param value
            Initial position for the slider.
        @param minValue
            Minimum slider position.
        @param maxValue
            Maximum slider position.
        @param size
            Window size. If wxDefaultSize is specified then a default size
        is chosen.
        @param style
            Window style. See wxSlider.
        @param validator
            Window validator.
        @param name
            Window name.

        @see Create(), wxValidator
    */
    wxSlider(wxWindow* parent, wxWindowID id, int value,
             int minValue, int maxValue,
             const wxPoint& point = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = "slider");

    /**
        Destructor, destroying the slider.
    */
    virtual ~wxSlider();

    /**
        Clears the selection, for a slider with the @b wxSL_SELRANGE style.

        @remarks Windows 95 only.
    */
    virtual void ClearSel();

    /**
        Clears the ticks.

        @remarks Windows 95 only.
    */
    virtual void ClearTicks();

    /**
        Used for two-step slider construction. See wxSlider()
        for further details.
    */
    bool Create(wxWindow* parent, wxWindowID id, int value,
                int minValue, int maxValue,
                const wxPoint& point = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSL_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = "slider");

    /**
        Returns the line size.

        @see SetLineSize()
    */
    virtual int GetLineSize() const;

    /**
        Gets the maximum slider value.

        @see GetMin(), SetRange()
    */
    virtual int GetMax() const;

    /**
        Gets the minimum slider value.

        @see GetMin(), SetRange()
    */
    virtual int GetMin() const;

    /**
        Returns the page size.

        @see SetPageSize()
    */
    virtual int GetPageSize() const;

    /**
        Returns the selection end point.

        @remarks Windows 95 only.

        @see GetSelStart(), SetSelection()
    */
    virtual int GetSelEnd() const;

    /**
        Returns the selection start point.

        @remarks Windows 95 only.

        @see GetSelEnd(), SetSelection()
    */
    virtual int GetSelStart() const;

    /**
        Returns the thumb length.

        @remarks Windows 95 only.

        @see SetThumbLength()
    */
    virtual int GetThumbLength() const;

    /**
        Returns the tick frequency.

        @remarks Windows 95 only.

        @see SetTickFreq()
    */
    virtual int GetTickFreq() const;

    /**
        Gets the current slider value.

        @see GetMin(), GetMax(), SetValue()
    */
    virtual int GetValue() const;

    /**
        Sets the line size for the slider.

        @param lineSize
            The number of steps the slider moves when the user moves it up or down a
        line.

        @see GetLineSize()
    */
    virtual void SetLineSize(int lineSize);

    /**
        Sets the page size for the slider.

        @param pageSize
            The number of steps the slider moves when the user pages up or down.

        @see GetPageSize()
    */
    virtual void SetPageSize(int pageSize);

    /**
        Sets the minimum and maximum slider values.

        @see GetMin(), GetMax()
    */
    virtual void SetRange(int minValue, int maxValue);

    /**
        Sets the selection.

        @param startPos
            The selection start position.
        @param endPos
            The selection end position.

        @remarks Windows 95 only.

        @see GetSelStart(), GetSelEnd()
    */
    virtual void SetSelection(int startPos, int endPos);

    /**
        Sets the slider thumb length.

        @param len
            The thumb length.

        @remarks Windows 95 only.

        @see GetThumbLength()
    */
    virtual void SetThumbLength(int len);

    /**
        Sets a tick position.

        @param tickPos
            The tick position.

        @remarks Windows 95 only.

        @see SetTickFreq()
    */
    virtual void SetTick(int tickPos);

    /**
        Sets the tick mark frequency and position.

        @param n
            Frequency. For example, if the frequency is set to two, a tick mark is
        displayed for
            every other increment in the slider's range.
        @param pos
            Position. Must be greater than zero. TODO: what is this for?

        @remarks Windows 95 only.

        @see GetTickFreq()
    */
    virtual void SetTickFreq(int n, int pos);

    /**
        Sets the slider position.

        @param value
            The slider position.
    */
    virtual void SetValue(int value);
};

