/////////////////////////////////////////////////////////////////////////////
// Name:        platinfo.h
// Purpose:     interface of wxPlatformInfo
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/**
    The following are the operating systems which are recognized by wxWidgets and
    whose version can be detected at run-time.

    The values of the constants are chosen so that they can be combined as flags;
    this allows to check for operating system families like e.g. wxOS_MAC and wxOS_UNIX.
*/
enum wxOperatingSystemId
{
    wxOS_UNKNOWN = 0,                 //!< returned on error

    wxOS_MAC_OS         = 1 << 0,     //!< Apple Mac OS 8/9/X with Mac paths
    wxOS_MAC_OSX_DARWIN = 1 << 1,     //!< Apple Mac OS X with Unix paths
    wxOS_MAC = wxOS_MAC_OS|wxOS_MAC_OSX_DARWIN,

    wxOS_WINDOWS_9X     = 1 << 2,     //!< Windows 9x family (95/98/ME)
    wxOS_WINDOWS_NT     = 1 << 3,     //!< Windows NT family (NT/2000/XP)
    wxOS_WINDOWS_MICRO  = 1 << 4,     //!< MicroWindows
    wxOS_WINDOWS_CE     = 1 << 5,     //!< Windows CE (Window Mobile)
    wxOS_WINDOWS = wxOS_WINDOWS_9X       |
                    wxOS_WINDOWS_NT      |
                    wxOS_WINDOWS_MICRO   |
                    wxOS_WINDOWS_CE,

    wxOS_UNIX_LINUX     = 1 << 6,       //!< Linux
    wxOS_UNIX_FREEBSD   = 1 << 7,       //!< FreeBSD
    wxOS_UNIX_OPENBSD   = 1 << 8,       //!< OpenBSD
    wxOS_UNIX_NETBSD    = 1 << 9,       //!< NetBSD
    wxOS_UNIX_SOLARIS   = 1 << 10,      //!< SunOS
    wxOS_UNIX_AIX       = 1 << 11,      //!< AIX
    wxOS_UNIX_HPUX      = 1 << 12,      //!< HP/UX
    wxOS_UNIX = wxOS_UNIX_LINUX     |
                wxOS_UNIX_FREEBSD   |
                wxOS_UNIX_OPENBSD   |
                wxOS_UNIX_NETBSD    |
                wxOS_UNIX_SOLARIS   |
                wxOS_UNIX_AIX       |
                wxOS_UNIX_HPUX,

    wxOS_DOS            = 1 << 15,      //!< Microsoft DOS
    wxOS_OS2            = 1 << 16       //!< OS/2
};

/**
    The list of wxWidgets ports.

    Some of them can be used with more than a single (native) toolkit;
    e.g. wxWinCE port sources can be used with smartphones, pocket PCs
    and handheld devices SDKs.
*/
enum wxPortId
{
    wxPORT_UNKNOWN  = 0,            //!< returned on error

    wxPORT_BASE     = 1 << 0,       //!< wxBase, no native toolkit used

    wxPORT_MSW      = 1 << 1,       //!< wxMSW, native toolkit is Windows API
    wxPORT_MOTIF    = 1 << 2,       //!< wxMotif, using [Open]Motif or Lesstif
    wxPORT_GTK      = 1 << 3,       //!< wxGTK, using GTK+ 1.x, 2.x, GPE or Maemo
    wxPORT_MGL      = 1 << 4,       //!< wxMGL, using wxUniversal
    wxPORT_X11      = 1 << 5,       //!< wxX11, using wxUniversal
    wxPORT_OS2      = 1 << 6,       //!< wxOS2, using OS/2 Presentation Manager
    wxPORT_MAC      = 1 << 7,       //!< wxMac, using Carbon or Classic Mac API
    wxPORT_COCOA    = 1 << 8,       //!< wxCocoa, using Cocoa NextStep/Mac API
    wxPORT_WINCE    = 1 << 9,       //!< wxWinCE, toolkit is WinCE SDK API
    wxPORT_PALMOS   = 1 << 10,      //!< wxPalmOS, toolkit is PalmOS API
    wxPORT_DFB      = 1 << 11       //!< wxDFB, using wxUniversal
};


/**
    The architecture of the operating system
    (regardless of the build environment of wxWidgets library - see ::wxIsPlatform64bit()
    documentation for more info).
*/
enum wxArchitecture
{
    wxARCH_INVALID = -1,        //!< returned on error

    wxARCH_32,                  //!< 32 bit
    wxARCH_64,

    wxARCH_MAX
};


/**
    The endian-ness of the machine.
*/
enum wxEndianness
{
    wxENDIAN_INVALID = -1,      //!< returned on error

    wxENDIAN_BIG,               //!< 4321
    wxENDIAN_LITTLE,            //!< 1234
    wxENDIAN_PDP,               //!< 3412

    wxENDIAN_MAX
};


/**
    @class wxPlatformInfo

    This class holds informations about the operating system and the toolkit that
    the application is running under and some basic architecture info of the machine
    where it's running.

    @library{wxbase}
    @category{misc}

    @see ::wxGetOsVersion(), wxIsPlatformLittleEndian(), wxIsPlatform64Bit(),
         wxAppTraits
*/
class wxPlatformInfo : public wxObject
{
public:

    /**
        Initializes the instance with the values corresponding to the currently
        running platform.

        This is a fast operation because it only requires to copy the values
        internally cached for the currently running platform.

        @see Get()
    */
    wxPlatformInfo();

    /**
        Initializes the object using given values.
    */
    wxPlatformInfo(wxPortId pid = wxPORT_UNKNOWN,
                   int tkMajor = -1,
                   int tkMinor = -1,
                   wxOperatingSystemId id = wxOS_UNKNOWN,
                   int osMajor = -1,
                   int osMinor = -1,
                   wxArchitecture arch = wxARCH_INVALID,
                   wxEndianness endian = wxENDIAN_INVALID);


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
        Returns the global wxPlatformInfo object, initialized with the values
        for the currently running platform.
    */
    static const wxPlatformInfo& Get();

    /**
        Converts the given string to a wxArchitecture enum value or to
        @c wxARCH_INVALID if the given string is not a valid architecture string
        (i.e. does not contain nor @c 32 nor @c 64 strings).
    */
    static wxArchitecture GetArch(const wxString& arch);

    /**
        Returns the name for the given wxArchitecture enumeration value.
    */
    static wxString GetArchName(wxArchitecture arch);

    /**
        Returns the name for the architecture of this wxPlatformInfo instance.
    */
    wxString GetArchName() const;

    /**
        Returns the architecture ID of this wxPlatformInfo instance.
    */
    wxArchitecture GetArchitecture() const;

    /**
        Converts the given string to a wxEndianness enum value or to
        @c wxENDIAN_INVALID if the given string is not a valid endianness
        string (i.e. does not contain nor little nor big strings).
    */
    static wxEndianness GetEndianness(const wxString& end);

    /**
        Returns the endianness ID of this wxPlatformInfo instance.
    */
    wxEndianness GetEndianness() const;

    /**
        Returns name for the given wxEndianness enumeration value.
    */
    static wxString GetEndiannessName(wxEndianness end);

    /**
        Returns the name for the endianness of this wxPlatformInfo instance.
    */
    wxString GetEndiannessName() const;

    /**
        Returns the run-time major version of the OS associated with this
        wxPlatformInfo instance.

        @see ::wxGetOsVersion(), CheckOSVersion()
    */
    int GetOSMajorVersion() const;

    /**
        Returns the run-time minor version of the OS associated with this
        wxPlatformInfo instance.

        @see ::wxGetOsVersion(), CheckOSVersion()
    */
    int GetOSMinorVersion() const;

    /**
        Returns the operating system family name for the given wxOperatingSystemId
        enumeration value: @c Unix for @c wxOS_UNIX, @c Macintosh for @c wxOS_MAC,
        @c Windows for @c wxOS_WINDOWS, @c DOS for @c wxOS_DOS, @c OS/2 for @c wxOS_OS2.
    */
    static wxString GetOperatingSystemFamilyName(wxOperatingSystemId os);

    /**
        Returns the operating system family name of the OS associated with this
        wxPlatformInfo instance.
    */
    wxString GetOperatingSystemFamilyName() const;

    /**
        Converts the given string to a wxOperatingSystemId enum value or to @c
        wxOS_UNKNOWN if the given string is not a valid operating system name.
    */
    static wxOperatingSystemId GetOperatingSystemId(const wxString& name);

    /**
        Returns the operating system ID of this wxPlatformInfo instance.
    */
    wxOperatingSystemId GetOperatingSystemId() const;

    /**
        Returns the name for the given operating system ID value.

        This can be a long name (e.g. <tt>Microsoft Windows NT</tt>);
        use GetOperatingSystemFamilyName() to retrieve a short, generic name.
    */
    static wxString GetOperatingSystemIdName(wxOperatingSystemId os);

    /**
        Returns the operating system name of the OS associated with this wxPlatformInfo
        instance.
    */
    wxString GetOperatingSystemIdName() const;


    /**
        Converts the given string to a wxWidgets port ID value or to @c wxPORT_UNKNOWN
        if the given string does not match any of the wxWidgets canonical name ports
        ("wxGTK", "wxMSW", etc) nor any of the short wxWidgets name ports ("gtk", "msw", etc).
    */
    static wxPortId GetPortId(const wxString& portname);

    /**
        Returns the wxWidgets port ID associated with this wxPlatformInfo instance.
    */
    wxPortId GetPortId() const;

    /**
        Returns the name of the given wxWidgets port ID value.
        The @a usingUniversal argument specifies whether the port is in its native
        or wxUniversal variant.

        The returned string always starts with the "wx" prefix and is a mixed-case string.
    */
    static wxString GetPortIdName(wxPortId port, bool usingUniversal);

    /**
        Returns the name of the wxWidgets port ID associated with this wxPlatformInfo
        instance.
    */
    wxString GetPortIdName() const;

    /**
        Returns the short name of the given wxWidgets port ID value.
        The @a usingUniversal argument specifies whether the port is in its native
        or wxUniversal variant.

        The returned string does not start with the "wx" prefix and is always lower case.
    */
    static wxString GetPortIdShortName(wxPortId port,
                                       bool usingUniversal);

    /**
        Returns the short name of the wxWidgets port ID associated with this
        wxPlatformInfo instance.
    */
    wxString GetPortIdShortName() const;

    /**
        Returns the run-time major version of the toolkit associated with this
        wxPlatformInfo instance.

        Note that if GetPortId() returns @c wxPORT_BASE, then this value is zero
        (unless externally modified with SetToolkitVersion()); that is, no native
        toolkit is in use.
        See wxAppTraits::GetToolkitVersion() for more info.

        @see CheckToolkitVersion()
    */
    int GetToolkitMajorVersion() const;

    /**
        Returns the run-time minor version of the toolkit associated with this
        wxPlatformInfo instance.

        Note that if GetPortId() returns @c wxPORT_BASE, then this value is zero
        (unless externally modified with SetToolkitVersion()); that is, no native
        toolkit is in use.
        See wxAppTraits::GetToolkitVersion() for more info.

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

