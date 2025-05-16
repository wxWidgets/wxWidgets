/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/sysopt.cpp
// Purpose:     wxSystemOptions
// Author:      Julian Smart
// Created:     2001-07-10
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_SYSTEM_OPTIONS

#include "wx/sysopt.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/string.h"
    #include "wx/arrstr.h"
#endif

// Include header containing wxIsCatchUnhandledExceptionsDisabled() declaration.
#include "wx/private/safecall.h"

#include <unordered_map>

// ----------------------------------------------------------------------------
// private globals
// ----------------------------------------------------------------------------

namespace
{

// Keys of this map are option names and values are their (potentially empty)
// values.
std::unordered_map<wxString, wxString> gs_options;

// Name of a system option that we handle specially for performance reasons.
constexpr char CATCH_UNHANDLED_EXCEPTIONS[] = "catch-unhandled-exceptions";

// Cached return value of wxIsCatchUnhandledExceptionsDisabled().
int gs_catchUnhandledExceptionsDisabled = -1;

} // anonymous namespace

// ============================================================================
// wxSystemOptions implementation
// ============================================================================

// Option functions (arbitrary name/value mapping)
void wxSystemOptions::SetOption(const wxString& name, const wxString& value)
{
    if ( name == CATCH_UNHANDLED_EXCEPTIONS )
    {
        // Invalidate the cached value.
        gs_catchUnhandledExceptionsDisabled = -1;
    }

    gs_options[name] = value;
}

void wxSystemOptions::SetOption(const wxString& name, int value)
{
    SetOption(name, wxString::Format(wxT("%d"), value));
}

wxString wxSystemOptions::GetOption(const wxString& name)
{
    auto it = gs_options.find(name);

    if ( it == gs_options.end() )
    {
        // look in the environment: first for a variable named "wx_appname_name"
        // which can be set to affect the behaviour or just this application
        // and then for "wx_name" which can be set to change the option globally
        wxString var(name);
        var.Replace(wxT("."), wxT("_"));  // '.'s not allowed in env var names
        var.Replace(wxT("-"), wxT("_"));  // and neither are '-'s

        wxString appname;
        if ( wxTheApp )
            appname = wxTheApp->GetAppName();

        wxString val;
        if ( !appname.empty() )
            val = wxGetenv(wxT("wx_") + appname + wxT('_') + var);

        if ( val.empty() )
            val = wxGetenv(wxT("wx_") + var);

        // save it even if it is empty to avoid calling wxGetenv() in the
        // future if this option is requested again
        it = gs_options.insert({name, val}).first;
    }

    return it->second;
}

int wxSystemOptions::GetOptionInt(const wxString& name)
{
    return wxAtoi (GetOption(name));
}

bool wxSystemOptions::HasOption(const wxString& name)
{
    return !GetOption(name).empty();
}

bool wxIsCatchUnhandledExceptionsDisabled()
{
    if ( gs_catchUnhandledExceptionsDisabled == -1 )
    {
        gs_catchUnhandledExceptionsDisabled =
            wxSystemOptions::IsFalse(CATCH_UNHANDLED_EXCEPTIONS);
    }

    return gs_catchUnhandledExceptionsDisabled;
}

#endif // wxUSE_SYSTEM_OPTIONS
