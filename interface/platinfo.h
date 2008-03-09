/////////////////////////////////////////////////////////////////////////////
// Name:        platinfo.h
// Purpose:     documentation for wxPlatformInfo class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxPlatformInfo
    @wxheader{platinfo.h}

    This class holds informations about the operating system and the toolkit that
    the application
    is running under and some basic architecture info of the machine where it's
    running.

    @library{wxbase}
    @category{FIXME}

    @seealso
    wxGetOSVersion, wxIsPlatformLittleEndian, wxIsPlatform64Bit, wxAppTraits
*/
class wxPlatformInfo : public wxObject
{
public:
    //@{
    /**
        Initializes the object using given values.
    */
    wxPlatformInfo();
    wxPlatformInfo(wxPortId pid = wxPORT_UNKNOWN,
                   int tkMajor = -1,
                   int tkMinor = -1,
                   wxOperatingSystemId id = wxOS_UNKNOWN,
                   int osMajor = -1,
                   int osMinor = -1,
                   wxArchitecture arch = wxARCH_INVALID,
                   wxEndianness endian = wxENDIAN_INVALID);
    //@}

    /**
        Returns @true if the OS version is at least @c major.minor.
        
        @see GetOSMajorVersion(), GetOSMinorVersion(),
             CheckToolkitVersion()
    */
    bool CheckOSVersion(int major, int minor) const;

    /**
        Returns @true if the toolkit version is at least @c major.minor.
        
        @see GetToolkitMajorVersion(),
             GetToolkitMinorVersion(), CheckOSVersion()
    */
    bool CheckToolkitVersion(int major, int minor) const;

    /**
        Returns the global wxPlatformInfo object, initialized with the values for the
        currently running platform.
    */
    static const wxPlatformInfo Get();

    /**
        Converts the given string to a wxArchitecture enum value or to
        wxARCH_INVALID if the given string is not a valid architecture string
        (i.e. does not contain nor @c 32 nor @c 64 strings).
    */
    static wxArchitecture GetArch(const wxString& arch);

    //@{
    /**
        Returns the name for the architecture of this wxPlatformInfo instance.
    */
    static wxString GetArchName(wxArchitecture arch) const;
    wxString GetArchName() const;
    //@}

    /**
        Returns the architecture ID of this wxPlatformInfo instance.
    */
    wxArchitecture GetArchitecture() const;

    //@{
    /**
        Returns the endianness ID of this wxPlatformInfo instance.
    */
    static wxEndianness GetEndianness(const wxString& end) const;
    wxEndianness GetEndianness() const;
    //@}

    //@{
    /**
        Returns the name for the endianness of this wxPlatformInfo instance.
    */
    static wxString GetEndiannessName(wxEndianness end) const;
    wxString GetEndiannessName() const;
    //@}

    /**
        Returns the run-time major version of the OS associated with this
        wxPlatformInfo instance.
        See wxGetOsVersion for more info.
        
        @see CheckOSVersion()
    */
    int GetOSMajorVersion() const;

    /**
        Returns the run-time minor version of the OS associated with this
        wxPlatformInfo instance.
        See wxGetOsVersion for more info.
        
        @see CheckOSVersion()
    */
    int GetOSMinorVersion() const;

    //@{
    /**
        Returns the operating system family name of the OS associated with this
        wxPlatformInfo instance.
    */
    static wxString GetOperatingSystemFamilyName(wxOperatingSystemId os) const;
    wxString GetOperatingSystemFamilyName() const;
    //@}

    //@{
    /**
        Returns the operating system ID of this wxPlatformInfo instance.
    */
    static wxOperatingSystemId GetOperatingSystemId(const wxString& name) const;
    wxOperatingSystemId GetOperatingSystemId() const;
    //@}

    //@{
    /**
        Returns the operating system name of the OS associated with this wxPlatformInfo
        instance.
    */
    static wxString GetOperatingSystemIdName(wxOperatingSystemId os) const;
    wxString GetOperatingSystemIdName() const;
    //@}

    //@{
    /**
        Returns the wxWidgets port ID associated with this wxPlatformInfo instance.
    */
    static wxPortId GetPortId(const wxString& portname) const;
    wxPortId GetPortId() const;
    //@}

    //@{
    /**
        Returns the name of the wxWidgets port ID associated with this wxPlatformInfo
        instance.
    */
    static wxString GetPortIdName(wxPortId port, bool usingUniversal) const;
    wxString GetPortIdName() const;
    //@}

    //@{
    /**
        Returns the short name of the wxWidgets port ID associated with this
        wxPlatformInfo instance.
    */
    static wxString GetPortIdShortName(wxPortId port,
                                       bool usingUniversal) const;
    wxString GetPortIdShortName() const;
    //@}

    /**
        Returns the run-time major version of the toolkit associated with this
        wxPlatformInfo instance.
        Note that if GetPortId() returns wxPORT_BASE, then this value is zero (unless
        externally modified with wxPlatformInfo::SetToolkitVersion); that is, no native toolkit is in use.
        See wxAppTraits::GetToolkitVersion for more info.
        
        @see CheckToolkitVersion()
    */
    int GetToolkitMajorVersion() const;

    /**
        Returns the run-time minor version of the toolkit associated with this
        wxPlatformInfo instance.
        Note that if GetPortId() returns wxPORT_BASE, then this value is zero (unless
        externally modified with wxPlatformInfo::SetToolkitVersion); that is, no native toolkit is in use.
        See wxAppTraits::GetToolkitVersion for more info.
        
        @see CheckToolkitVersion()
    */
    int GetToolkitMinorVersion() const;

    /**
        Returns @true if this instance is fully initialized with valid values.
    */
    bool IsOk() const;

    /**
        Returns @true if this wxPlatformInfo describes wxUniversal build.
    */
    bool IsUsingUniversalWidgets() const;

    /**
        Sets the architecture enum value associated with this wxPlatformInfo instance.
    */
    void SetArchitecture(wxArchitecture n);

    /**
        Sets the endianness enum value associated with this wxPlatformInfo instance.
    */
    void SetEndianness(wxEndianness n);

    /**
        Sets the version of the operating system associated with this wxPlatformInfo
        instance.
    */
    void SetOSVersion(int major, int minor);

    /**
        Sets the operating system associated with this wxPlatformInfo instance.
    */
    void SetOperatingSystemId(wxOperatingSystemId n);

    /**
        Sets the wxWidgets port ID associated with this wxPlatformInfo instance.
    */
    void SetPortId(wxPortId n);

    /**
        Sets the version of the toolkit associated with this wxPlatformInfo instance.
    */
    void SetToolkitVersion(int major, int minor);

    /**
        Inequality operator. Tests all class' internal variables.
    */
    bool operator!=(const wxPlatformInfo& t) const;

    /**
        Equality operator. Tests all class' internal variables.
    */
    bool operator==(const wxPlatformInfo& t) const;
};
