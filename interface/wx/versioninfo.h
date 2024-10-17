/////////////////////////////////////////////////////////////////////////////
// Name:        versioninfo.h
// Purpose:     interface of wxVersionInfo
// Author:      Troels K
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    Defines the context in which the version information is requested.

    Elements of this enum can be used to select between getting the compile- or
    run-time wxVersionInfo object for the components supporting this.

    @since 3.3.0
 */
enum class wxVersionContext
{
    /// Ask for the version used during run-time.
    RunTime,

    /// Ask for the version that the application was built with.
    BuildTime
};


/**
    @class wxVersionInfo

    wxVersionInfo contains version information.

    This class is used by wxWidgets to provide version information about the
    libraries it uses and itself, but you can also apply it in user space, to
    provide version information about your own libraries, or other libraries
    that you use.

    @library{wxbase}

    @category{data}

    @since 2.9.2
*/
class wxVersionInfo
{
public:
    /**
        Constructor.

        The version information objects need to be initialized with this
        constructor and are immutable once they are created.

        @param name The name of the library or other entity that this object
            pertains to.
        @param major The major version component.
        @param minor The minor version component.
        @param micro The micro version component, 0 by default.
        @param revision The revision version component, also known as "build
            number". This component is also 0 by default and is only available
            since wxWidgets 3.2.0.
        @param description Free form description of this version, none by
            default.
        @param copyright Copyright string, none by default.
    */
    wxVersionInfo(const wxString& name = wxString(),
                  int major = 0,
                  int minor = 0,
                  int micro = 0,
                  int revision = 0,
                  const wxString& description = wxString(),
                  const wxString& copyright = wxString());

    /**
        Return true if the version is at least equal to the given one.

        @param major Major version to compare with.
        @param minor Optional minor version to compare with.
        @param micro Optional micro version to compare with.
        @return @true if this version is equal to or greater than the given one.

        @since 3.3.0
     */
    bool AtLeast(int major, int minor = 0, int micro = 0) const;

    /**
        Return true if there is actually at least some version information.

        For the default-constructed object, this function returns @false,
        allowing to distinguish it from any object filled with the version
        information.

        @since 3.3.0
     */
    bool IsOk() const;

    /**
        Get the name of the object (library).

        @return Name string.
    */
    const wxString& GetName() const;

    /**
        Get the major version number.

        @return Major version number.
    */
    int GetMajor() const;

    /**
        Get the minor version number.

        @return Minor version number.
    */
    int GetMinor() const;

    /**
        Get the micro version, or release number.

        This is the third component of the version.

        @return Micro version, or release number.
    */
    int GetMicro() const;

    /**
        Get the revision version, or build number.

        This is the fourth component of the version.

        @return Revision version, or build number.

        @since 3.2.0
    */
    int GetRevision() const;

    /**
        Get the string representation of this version object.

        This function returns the description if it is non-empty or
        GetVersionString() if there is no description.

        @see GetDescription(), GetVersionString()
    */
    wxString ToString() const;

    /**
        Get the string representation of only numeric version components.

        The micro and revision components of the version are ignored/not used
        if they are both zero. If the revision component is non-zero all four
        parts will be used even if the micro component is zero.

        @return The version string in the form "major.minor[.micro[.revision]]".

        @since 3.3.0
     */
    wxString GetNumericVersionString() const;

    /**
        Get the string representation.

        The micro and revision components of the version are ignored/not used
        if they are both zero. If the revision component is non-zero all four
        parts will be used even if the micro component is zero.

        Note that this function includes the name of the object this version is
        defined for, if this is undesired, use GetNumericVersionString() instead.

        @return The version string in the form "name major.minor[.micro[.revision]]".
    */
    wxString GetVersionString() const;

    /**
        Return @true if a description string has been specified.

        @see GetDescription()
    */
    bool HasDescription() const;

    /**
        Get the description string.

        The description may be empty.

        @return The description string, free-form.
    */
    const wxString& GetDescription();

    /**
        Returns @true if a copyright string has been specified.

        @see GetCopyright()
    */
    bool HasCopyright() const;

    /**
        Get the copyright string.

        The copyright string may be empty.

        @return The copyright string.
    */
    const wxString& GetCopyright() const;
};
