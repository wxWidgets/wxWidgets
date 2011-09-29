/////////////////////////////////////////////////////////////////////////////
// Name:        wx/timectrl.h
// Purpose:     interface of wxTimePickerCtrl
// Author:      Vadim Zeitlin
// Created:     2011-09-22
// RCS-ID:      $Id$
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxTimePickerCtrl

    This control allows the user to enter time.

    It is similar to wxDatePickerCtrl but is used for time, and not date,
    selection. While GetValue() and SetValue() still work with values of type
    wxDateTime (because wxWidgets doesn't provide a time-only class), their
    date part is ignored by this control.

    It is only available if @c wxUSE_TIMEPICKCTRL is set to 1.

    This control currently doesn't have any specific flags.

    @beginEventEmissionTable{wxDateEvent}
    @event{EVT_TIME_CHANGED(id, func)}
           This event fires when the user changes the current selection in the
           control.
    @endEventTable

    @library{wxadv}
    @category{pickers}
    @appearance{timepickerctrl.png}

    @see wxDatePickerCtrl, wxDateEvent

    @since 2.9.3
*/
class wxTimePickerCtrl : public wxControl
{
public:
    /**
        Initializes the object and calls Create() with all the parameters.
    */
    wxTimePickerCtrl(wxWindow* parent, wxWindowID id,
                     const wxDateTime& dt = wxDefaultDateTime,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxTP_DEFAULT,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = "timectrl");

    /**
        Create the control window.

        This method should only be used for objects created using default
        constructor.

        @param parent
            Parent window, must not be non-@NULL.
        @param id
            The identifier for the control.
        @param dt
            The initial value of the control, if an invalid date (such as the
            default value) is used, the control is set to current time.
        @param pos
            Initial position.
        @param size
            Initial size. If left at default value, the control chooses its own
            best size by using the height approximately equal to a text control
            and width large enough to show the time fully.
        @param style
            The window style, should be left at 0 as there are no special
            styles for this control in this version.
        @param validator
            Validator which can be used for additional checks.
        @param name
            Control name.

        @return @true if the control was successfully created or @false if
                 creation failed.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxDateTime& dt = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDP_DEFAULT | wxDP_SHOWCENTURY,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = "timectrl");

    /**
        Returns the currently entered time.

        The date part of the returned wxDateTime object is always set to today
        and should be ignored, only the time part is relevant.
    */
    virtual wxDateTime GetValue() const = 0;

    /**
        Changes the current value of the control.

        The date part of @a dt is ignored, only the time part is displayed in
        the control. The @a dt object must however be valid.

        Calling this method does not result in a time change event.
    */
    virtual void SetValue(const wxDateTime& dt) = 0;
};
