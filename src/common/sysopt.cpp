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

#ifndef WX_PRECOMP
    #include "wx/list.h"
#endif

#if wxUSE_SYSTEM_OPTIONS

#include "wx/string.h"
#include "wx/sysopt.h"
#include "wx/module.h"
#include "wx/arrstr.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// the module which is used to clean up wxSystemOptions data (this is a
// singleton class so it can't be done in the dtor)
class wxSystemOptionsModule : public wxModule
{
    friend class WXDLLIMPEXP_BASE wxSystemOptions;
public:
    virtual bool OnInit();
    virtual void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxSystemOptionsModule)

    static wxArrayString   sm_optionNames;
    static wxArrayString   sm_optionValues;
};

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// wxSystemOptionsModule
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSystemOptionsModule, wxModule)

wxArrayString wxSystemOptionsModule::sm_optionNames;
wxArrayString wxSystemOptionsModule::sm_optionValues;

bool wxSystemOptionsModule::OnInit()
{
    return true;
}

void wxSystemOptionsModule::OnExit()
{
    sm_optionNames.Clear();
    sm_optionValues.Clear();
}

// ----------------------------------------------------------------------------
// wxSystemOptions
// ----------------------------------------------------------------------------

// Option functions (arbitrary name/value mapping)
void wxSystemOptions::SetOption(const wxString& name, const wxString& value)
{
    int idx = wxSystemOptionsModule::sm_optionNames.Index(name, false);
    if (idx == wxNOT_FOUND)
    {
        wxSystemOptionsModule::sm_optionNames.Add(name);
        wxSystemOptionsModule::sm_optionValues.Add(value);
    }
    else
    {
        wxSystemOptionsModule::sm_optionNames[idx] = name;
        wxSystemOptionsModule::sm_optionValues[idx] = value;
    }
}

void wxSystemOptions::SetOption(const wxString& name, int value)
{
    wxString valStr;
    valStr.Printf(wxT("%d"), value);
    SetOption(name, valStr);
}

wxString wxSystemOptions::GetOption(const wxString& name)
{
    int idx = wxSystemOptionsModule::sm_optionNames.Index(name, false);
    if (idx == wxNOT_FOUND)
        return wxEmptyString;
    else
        return wxSystemOptionsModule::sm_optionValues[idx];
}

int wxSystemOptions::GetOptionInt(const wxString& name)
{
    return wxAtoi(GetOption(name));
}

bool wxSystemOptions::HasOption(const wxString& name)
{
    return (wxSystemOptionsModule::sm_optionNames.Index(name, false) != wxNOT_FOUND);
}

#endif
    // wxUSE_SYSTEM_OPTIONS

