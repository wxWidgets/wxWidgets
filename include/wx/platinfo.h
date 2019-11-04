///////////////////////////////////////////////////////////////////////////////
// Name:        wx/platinfo.h
// Purpose:     declaration of the wxPlatformInfo class
// Author:      Francesco Montorsi
// Modified by:
// Created:     07.07.2006 (based on wxToolkitInfo)
// Copyright:   (c) 2006 Francesco Montorsi
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PLATINFO_H_
#define _WX_PLATINFO_H_

#include "wx/string.h"

// ----------------------------------------------------------------------------
// wxPlatformInfo enums & structs
// ----------------------------------------------------------------------------

// VERY IMPORTANT: when changing these enum values, also change the relative
//                 string tables in src/common/platinfo.cpp


// families & sub-families of operating systems
enum wxOperatingSystemId
{
    wxOS_UNKNOWN = 0,                 // returned on error

    wxOS_MAC_OS         = 1 << 0,     // Apple Mac OS 8/9/X with Mac paths
    wxOS_MAC_OSX_DARWIN = 1 << 1,     // Apple Mac OS X with Unix paths
    wxOS_MAC = wxOS_MAC_OS|wxOS_MAC_OSX_DARWIN,

    wxOS_WINDOWS_9X     = 1 << 2,     // obsolete
    wxOS_WINDOWS_NT     = 1 << 3,     // obsolete
    wxOS_WINDOWS_MICRO  = 1 << 4,     // obsolete
    wxOS_WINDOWS_CE     = 1 << 5,     // obsolete
    wxOS_WINDOWS = wxOS_WINDOWS_9X      |
                   wxOS_WINDOWS_NT      |
                   wxOS_WINDOWS_MICRO   |
                   wxOS_WINDOWS_CE,

    wxOS_UNIX_LINUX     = 1 << 6,       // Linux
    wxOS_UNIX_FREEBSD   = 1 << 7,       // FreeBSD
    wxOS_UNIX_OPENBSD   = 1 << 8,       // OpenBSD
    wxOS_UNIX_NETBSD    = 1 << 9,       // NetBSD
    wxOS_UNIX_SOLARIS   = 1 << 10,      // SunOS
    wxOS_UNIX_AIX       = 1 << 11,      // AIX
    wxOS_UNIX_HPUX      = 1 << 12,      // HP/UX
    wxOS_UNIX = wxOS_UNIX_LINUX     |
                wxOS_UNIX_FREEBSD   |
                wxOS_UNIX_OPENBSD   |
                wxOS_UNIX_NETBSD    |
                wxOS_UNIX_SOLARIS   |
                wxOS_UNIX_AIX       |
                wxOS_UNIX_HPUX,

    // 1<<13 and 1<<14 available for other Unix flavours

    wxOS_DOS            = 1 << 15,      // obsolete
    wxOS_OS2            = 1 << 16       // obsolete
};

// list of wxWidgets ports - some of them can be used with more than
// a single toolkit.
enum wxPortId
{
    wxPORT_UNKNOWN  = 0,            // returned on error

    wxPORT_BASE     = 1 << 0,       // wxBase, no native toolkit used

    wxPORT_MSW      = 1 << 1,       // wxMSW, native toolkit is Windows API
    wxPORT_MOTIF    = 1 << 2,       // wxMotif, using [Open]Motif or Lesstif
    wxPORT_GTK      = 1 << 3,       // wxGTK, using GTK+ 1.x, 2.x, 3.x
    wxPORT_DFB      = 1 << 4,       // wxDFB, using wxUniversal
    wxPORT_X11      = 1 << 5,       // wxX11, using wxUniversal
    wxPORT_PM       = 1 << 6,       // obsolete
    wxPORT_OS2      = wxPORT_PM,    // obsolete
    wxPORT_MAC      = 1 << 7,       // wxOSX (former wxMac), using Cocoa or iPhone API
    wxPORT_OSX      = wxPORT_MAC,   // wxOSX, using Cocoa or iPhone API
    wxPORT_COCOA    = 1 << 8,       // wxCocoa, using Cocoa NextStep/Mac API
    wxPORT_WINCE    = 1 << 9,       // obsolete
    wxPORT_QT       = 1 << 10       // wxQT, using Qt 5+
};

// architecture of the operating system
// (regardless of the build environment of wxWidgets library - see
// wxIsPlatform64bit documentation for more info)
enum wxArchitecture
{
    wxARCH_INVALID = -1,        // returned on error

    wxARCH_32,                  // 32 bit
    wxARCH_64,

    wxARCH_MAX
};


// endian-ness of the machine
enum wxEndianness
{
    wxENDIAN_INVALID = -1,      // returned on error

    wxENDIAN_BIG,               // 4321
    wxENDIAN_LITTLE,            // 1234
    wxENDIAN_PDP,               // 3412

    wxENDIAN_MAX
};

// information about a linux distro returned by the lsb_release utility
struct wxLinuxDistributionInfo
{
    wxString Id;
    wxString Release;
    wxString CodeName;
    wxString Description;

    bool operator==(const wxLinuxDistributionInfo& ldi) const
    {
        return Id == ldi.Id &&
               Release == ldi.Release &&
               CodeName == ldi.CodeName &&
               Description == ldi.Description;
    }

    bool operator!=(const wxLinuxDistributionInfo& ldi) const
    { return !(*this == ldi); }
};


// ----------------------------------------------------------------------------
// wxPlatformInfo
// ----------------------------------------------------------------------------

// Information about the toolkit that the app is running under and some basic
// platform and architecture info
class WXDLLIMPEXP_BASE wxPlatformInfo
{
public:
    wxPlatformInfo();
    wxPlatformInfo(wxPortId pid,
                   int tkMajor = -1, int tkMinor = -1,
                   wxOperatingSystemId id = wxOS_UNKNOWN,
                   int osMajor = -1, int osMinor = -1,
                   wxArchitecture arch = wxARCH_INVALID,
                   wxEndianness endian = wxENDIAN_INVALID,
                   bool usingUniversal = false);

    // default copy ctor, assignment operator and dtor are ok

    bool operator==(const wxPlatformInfo &t) const;

    bool operator!=(const wxPlatformInfo &t) const
        { return !(*this == t); }

    // Gets a wxPlatformInfo already initialized with the values for
    // the currently running platform.
    static const wxPlatformInfo& Get();



    // string -> enum conversions
    // ---------------------------------

    static wxOperatingSystemId GetOperatingSystemId(const wxString &name);
    static wxPortId GetPortId(const wxString &portname);

    static wxArchitecture GetArch(const wxString &arch);
    static wxEndianness GetEndianness(const wxString &end);

    // enum -> string conversions
    // ---------------------------------

    static wxString GetOperatingSystemFamilyName(wxOperatingSystemId os);
    static wxString GetOperatingSystemIdName(wxOperatingSystemId os);
    static wxString GetPortIdName(wxPortId port, bool usingUniversal);
    static wxString GetPortIdShortName(wxPortId port, bool usingUniversal);

    static wxString GetArchName(wxArchitecture arch);
    static wxString GetEndiannessName(wxEndianness end);


    // getters
    // -----------------

    int GetOSMajorVersion() const
        { return m_osVersionMajor; }
    int GetOSMinorVersion() const
        { return m_osVersionMinor; }
    int GetOSMicroVersion() const
        { return m_osVersionMicro; }

    // return true if the OS version >= major.minor
    bool CheckOSVersion(int major, int minor, int micro = 0) const;

    int GetToolkitMajorVersion() const
        { return m_tkVersionMajor; }
    int GetToolkitMinorVersion() const
        { return m_tkVersionMinor; }
    int GetToolkitMicroVersion() const
        { return m_tkVersionMicro; }

    bool CheckToolkitVersion(int major, int minor, int micro = 0) const
    {
        return DoCheckVersion(GetToolkitMajorVersion(),
                              GetToolkitMinorVersion(),
                              GetToolkitMicroVersion(),
                              major,
                              minor,
                              micro);
    }

    bool IsUsingUniversalWidgets() const
        { return m_usingUniversal; }

    wxOperatingSystemId GetOperatingSystemId() const
        { return m_os; }
    wxLinuxDistributionInfo GetLinuxDistributionInfo() const
        { return m_ldi; }
    wxPortId GetPortId() const
        { return m_port; }
    wxArchitecture GetArchitecture() const
        { return m_arch; }
    wxEndianness GetEndianness() const
        { return m_endian; }


    // string getters
    // -----------------

    wxString GetOperatingSystemFamilyName() const
        { return GetOperatingSystemFamilyName(m_os); }
    wxString GetOperatingSystemIdName() const
        { return GetOperatingSystemIdName(m_os); }
    wxString GetPortIdName() const
        { return GetPortIdName(m_port, m_usingUniversal); }
    wxString GetPortIdShortName() const
        { return GetPortIdShortName(m_port, m_usingUniversal); }
    wxString GetArchName() const
        { return GetArchName(m_arch); }
    wxString GetEndiannessName() const
        { return GetEndiannessName(m_endian); }
    wxString GetOperatingSystemDescription() const
        { return m_osDesc; }
    wxString GetDesktopEnvironment() const
        { return m_desktopEnv; }

    static wxString GetOperatingSystemDirectory();
        // doesn't make sense to store inside wxPlatformInfo the OS directory,
        // thus this function is static; note that this function simply calls
        // wxGetOSDirectory() and is here just to make it easier for the user to
        // find it that feature (global functions can be difficult to find in the docs)

    // setters
    // -----------------

    void SetOSVersion(int major, int minor, int micro = 0)
    {
        m_osVersionMajor = major;
        m_osVersionMinor = minor;
        m_osVersionMicro = micro;
    }

    void SetToolkitVersion(int major, int minor, int micro = 0)
    {
        m_tkVersionMajor = major;
        m_tkVersionMinor = minor;
        m_tkVersionMicro = micro;
    }

    void SetOperatingSystemId(wxOperatingSystemId n)
        { m_os = n; }
    void SetOperatingSystemDescription(const wxString& desc)
        { m_osDesc = desc; }
    void SetPortId(wxPortId n)
        { m_port = n; }
    void SetArchitecture(wxArchitecture n)
        { m_arch = n; }
    void SetEndianness(wxEndianness n)
        { m_endian = n; }

    void SetDesktopEnvironment(const wxString& de)
        { m_desktopEnv = de; }
    void SetLinuxDistributionInfo(const wxLinuxDistributionInfo& di)
        { m_ldi = di; }


    // miscellaneous
    // -----------------

    bool IsOk() const
    {
        return m_osVersionMajor != -1 && m_osVersionMinor != -1 &&
               m_osVersionMicro != -1 &&
               m_os != wxOS_UNKNOWN &&
               !m_osDesc.IsEmpty() &&
               m_tkVersionMajor != -1 && m_tkVersionMinor != -1 &&
               m_tkVersionMicro != -1 &&
               m_port != wxPORT_UNKNOWN &&
               m_arch != wxARCH_INVALID &&
               m_endian != wxENDIAN_INVALID;

               // do not check linux-specific info; it's ok to have them empty
    }


protected:
    static bool DoCheckVersion(int majorCur, int minorCur, int microCur,
                               int major, int minor, int micro)
    {
        return majorCur > major
            || (majorCur == major && minorCur > minor)
            || (majorCur == major && minorCur == minor && microCur >= micro);
    }

    bool m_initializedForCurrentPlatform;

    void InitForCurrentPlatform();


    // OS stuff
    // -----------------

    // Version of the OS; valid if m_os != wxOS_UNKNOWN
    // (-1 means not initialized yet).
    int m_osVersionMajor,
        m_osVersionMinor,
        m_osVersionMicro;

    // Operating system ID.
    wxOperatingSystemId m_os;

    // Operating system description.
    wxString m_osDesc;


    // linux-specific
    // -----------------

    wxString m_desktopEnv;
    wxLinuxDistributionInfo m_ldi;


    // toolkit
    // -----------------

    // Version of the underlying toolkit
    // (-1 means not initialized yet; zero means no toolkit).
    int m_tkVersionMajor, m_tkVersionMinor, m_tkVersionMicro;

    // name of the wxWidgets port
    wxPortId m_port;

    // is using wxUniversal widgets?
    bool m_usingUniversal;


    // others
    // -----------------

    // architecture of the OS/machine
    wxArchitecture m_arch;

    // endianness of the machine
    wxEndianness m_endian;
};



#endif // _WX_PLATINFO_H_
