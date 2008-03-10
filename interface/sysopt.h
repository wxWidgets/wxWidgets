/////////////////////////////////////////////////////////////////////////////
// Name:        sysopt.h
// Purpose:     interface of wxSystemOptions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxSystemOptions
    @wxheader{sysopt.h}

    wxSystemOptions stores option/value pairs that wxWidgets itself or
    applications can use to alter behaviour at run-time. It can be
    used to optimize behaviour that doesn't deserve a distinct API,
    but is still important to be able to configure.

    These options are currently recognised by wxWidgets.

    @library{wxbase}
    @category{misc}

    @see wxSystemOptions::SetOption, wxSystemOptions::GetOptionInt,
    wxSystemOptions::HasOption
*/
class wxSystemOptions : public wxObject
{
public:
    /**
        Default constructor. You don't need to create an instance of wxSystemOptions
        since all of its functions are static.
    */
    wxSystemOptions();

    /**
        Gets an option. The function is case-insensitive to @e name.
        Returns empty string if the option hasn't been set.
        
        @see SetOption(), GetOptionInt(),
             HasOption()
    */
    wxString GetOption(const wxString& name) const;

    /**
        Gets an option as an integer. The function is case-insensitive to @e name.
        If the option hasn't been set, this function returns 0.
        
        @see SetOption(), GetOption(),
             HasOption()
    */
    int GetOptionInt(const wxString& name) const;

    /**
        Returns @true if the given option is present. The function is
        case-insensitive to @e name.
        
        @see SetOption(), GetOption(),
             GetOptionInt()
    */
    bool HasOption(const wxString& name) const;

    /**
        Returns @true if the option with the given @a name had been set to 0
        value. This is mostly useful for boolean options for which you can't use
        @c GetOptionInt(name) == 0 as this would also be @true if the option
        hadn't been set at all.
    */
    bool IsFalse(const wxString& name) const;

    //@{
    /**
        Sets an option. The function is case-insensitive to @e name.
    */
    void SetOption(const wxString& name, const wxString& value);
    void SetOption(const wxString& name, int value);
    //@}
};

