/////////////////////////////////////////////////////////////////////////////
// Name:        common/sysopt.cpp
// Purpose:     wxSystemOptions
// Author:      Julian Smart
// Modified by:
// Created:     2001-07-10
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "sysopt.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#if wxUSE_SYSTEM_OPTIONS

#ifndef WX_PRECOMP
    #include "wx/list.h"
#endif

#include "wx/string.h"
#include "wx/sysopt.h"
#include "wx/arrstr.h"

// ----------------------------------------------------------------------------
// private globals
// ----------------------------------------------------------------------------

static wxArrayString gs_optionNames,
                     gs_optionValues;

// ============================================================================
// wxSystemOptions implementation
// ============================================================================

// Option functions (arbitrary name/value mapping)
void wxSystemOptions::SetOption(const wxString& name, const wxString& value)
{
    int idx = gs_optionNames.Index(name, false);
    if (idx == wxNOT_FOUND)
    {
        gs_optionNames.Add(name);
        gs_optionValues.Add(value);
    }
    else
    {
        gs_optionNames[idx] = name;
        gs_optionValues[idx] = value;
    }
}

void wxSystemOptions::SetOption(const wxString& name, int value)
{
    SetOption(name, wxString::Format(wxT("%d"), value));
}

wxString wxSystemOptions::GetOption(const wxString& name)
{
    int idx = gs_optionNames.Index(name, false);
    if (idx == wxNOT_FOUND)
        return wxEmptyString;
    else
        return gs_optionValues[idx];
}

int wxSystemOptions::GetOptionInt(const wxString& name)
{
    return wxAtoi(GetOption(name));
}

bool wxSystemOptions::HasOption(const wxString& name)
{
    return gs_optionNames.Index(name, false) != wxNOT_FOUND;
}

#endif // wxUSE_SYSTEM_OPTIONS

