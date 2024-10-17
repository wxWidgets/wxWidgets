///////////////////////////////////////////////////////////////////////////////
// Name:        wx/versioninfo.h
// Purpose:     declaration of wxVersionInfo class
// Author:      Troels K
// Created:     2010-11-22
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VERSIONINFO_H_
#define _WX_VERSIONINFO_H_

#include "wx/string.h"

// ----------------------------------------------------------------------------
// wxVersionContext: defines which version information to retrieve
// ----------------------------------------------------------------------------

enum class wxVersionContext
{
    RunTime,        // Version used during run-time.
    BuildTime       // Version that the application was built with.
};

// ----------------------------------------------------------------------------
// wxVersionInfo: represents version information
// ----------------------------------------------------------------------------

class wxVersionInfo
{
public:
    wxVersionInfo(const wxString& name = wxString(),
                  int major = 0,
                  int minor = 0,
                  int micro = 0,
                  int revision = 0,
                  const wxString& description = wxString(),
                  const wxString& copyright = wxString())
    {
        Init(name, major, minor, micro, revision, description, copyright);
    }

    // This constructor exists for backward compatibility (before the revision
    // part was added).
    wxVersionInfo(const wxString& name,
                  int major, int minor, int micro,
                  const wxString& description,
                  const wxString& copyright = wxString())
    {
        Init(name, major, minor, micro, 0, description, copyright);
    }

    // Default copy ctor, assignment operator and dtor are ok.


    // Check if this version is at least equal to the given one.
    bool AtLeast(int major, int minor = 0, int micro = 0) const
    {
        if ( m_major > major )
            return true;

        if ( m_major < major )
            return false;

        if ( m_minor > minor )
            return true;

        if ( m_minor < minor )
            return false;

        return m_micro >= micro;
    }

    // Return true if this version object actually has any version information.
    bool IsOk() const
    {
        return AtLeast(0) || !m_name.empty() || !m_description.empty();
    }

    const wxString& GetName() const { return m_name; }

    int GetMajor() const { return m_major; }
    int GetMinor() const { return m_minor; }
    int GetMicro() const { return m_micro; }
    int GetRevision() const { return m_revision; }

    wxString ToString() const
    {
        return HasDescription() ? GetDescription() : GetVersionString();
    }

    wxString GetNumericVersionString() const
    {
        wxString str;
        str << GetMajor() << '.' << GetMinor();
        if ( GetMicro() || GetRevision() )
        {
            str << '.' << GetMicro();
            if ( GetRevision() )
                str << '.' << GetRevision();
        }

        return str;
    }

    wxString GetVersionString() const
    {
        wxString str;
        str << m_name << ' ' << GetNumericVersionString();

        return str;
    }

    bool HasDescription() const { return !m_description.empty(); }
    const wxString& GetDescription() const { return m_description; }

    bool HasCopyright() const { return !m_copyright.empty(); }
    const wxString& GetCopyright() const { return m_copyright; }

private:
    void Init(const wxString& name,
              int major, int minor, int micro, int revision,
              const wxString& description,
              const wxString& copyright)
    {
        m_name = name;
        m_description = description;
        m_copyright = copyright;
        m_major = major;
        m_minor = minor;
        m_micro = micro;
        m_revision = revision;
    }

    wxString m_name,
             m_description,
             m_copyright;

    int m_major,
        m_minor,
        m_micro,
        m_revision;
};

#endif // _WX_VERSIONINFO_H_
