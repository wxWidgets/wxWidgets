///////////////////////////////////////////////////////////////////////////////
// Name:        palmos/regconf.cpp
// Purpose:
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "regconf.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include  "wx/string.h"
  #include  "wx/intl.h"
#endif //WX_PRECOMP

#include "wx/event.h"
#include "wx/app.h"
#include "wx/log.h"

#if wxUSE_CONFIG

#include "wx/config.h"

#include "wx/palmos/registry.h"
#include "wx/palmos/regconf.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// we put our data in HKLM\SOFTWARE_KEY\appname
#define SOFTWARE_KEY    wxString(wxT("Software\\"))

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

// get the value if the key is opened and it exists
bool TryGetValue(const wxRegKey& key, const wxString& str, wxString& strVal)
{
    return false;
}

bool TryGetValue(const wxRegKey& key, const wxString& str, long *plVal)
{
    return false;
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------

// create the config object which stores its data under HKCU\vendor\app and, if
// style & wxCONFIG_USE_GLOBAL_FILE, under HKLM\vendor\app
wxRegConfig::wxRegConfig(const wxString& appName, const wxString& vendorName,
                         const wxString& strLocal, const wxString& strGlobal,
                         long style)
           : wxConfigBase(appName, vendorName, strLocal, strGlobal, style)
{
}

wxRegConfig::~wxRegConfig()
{
}

// ----------------------------------------------------------------------------
// path management
// ----------------------------------------------------------------------------

void wxRegConfig::SetPath(const wxString& strPath)
{
}

// ----------------------------------------------------------------------------
// enumeration (works only with current group)
// ----------------------------------------------------------------------------

#define LOCAL_MASK        0x8000
#define IS_LOCAL_INDEX(l) (((l) & LOCAL_MASK) != 0)

bool wxRegConfig::GetFirstGroup(wxString& str, long& lIndex) const
{
    return false;
}

bool wxRegConfig::GetNextGroup(wxString& str, long& lIndex) const
{
    return false;
}

bool wxRegConfig::GetFirstEntry(wxString& str, long& lIndex) const
{
    return false;
}

bool wxRegConfig::GetNextEntry(wxString& str, long& lIndex) const
{
    return false;
}

size_t wxRegConfig::GetNumberOfEntries(bool WXUNUSED(bRecursive)) const
{
  return 0;
}

size_t wxRegConfig::GetNumberOfGroups(bool WXUNUSED(bRecursive)) const
{
  return 0;
}

// ----------------------------------------------------------------------------
// tests for existence
// ----------------------------------------------------------------------------

bool wxRegConfig::HasGroup(const wxString& key) const
{
    return false;
}

bool wxRegConfig::HasEntry(const wxString& key) const
{
    return false;
}

wxConfigBase::EntryType wxRegConfig::GetEntryType(const wxString& key) const
{
    return wxConfigBase::Type_Integer;
}

// ----------------------------------------------------------------------------
// reading/writing
// ----------------------------------------------------------------------------

bool wxRegConfig::DoReadString(const wxString& key, wxString *pStr) const
{
    return FALSE;
}

// this exactly reproduces the string version above except for ExpandEnvVars(),
// we really should avoid this code duplication somehow...

bool wxRegConfig::DoReadLong(const wxString& key, long *plResult) const
{
    return FALSE;
}

bool wxRegConfig::DoWriteString(const wxString& key, const wxString& szValue)
{
    return false;
}

bool wxRegConfig::DoWriteLong(const wxString& key, long lValue)
{
    return false;
}

// ----------------------------------------------------------------------------
// renaming
// ----------------------------------------------------------------------------

bool wxRegConfig::RenameEntry(const wxString& oldName, const wxString& newName)
{
    return false;
}

bool wxRegConfig::RenameGroup(const wxString& oldName, const wxString& newName)
{
    return false;
}

// ----------------------------------------------------------------------------
// deleting
// ----------------------------------------------------------------------------

bool wxRegConfig::DeleteEntry(const wxString& value, bool bGroupIfEmptyAlso)
{
    return false;
}

bool wxRegConfig::DeleteGroup(const wxString& key)
{
    return false;
}

bool wxRegConfig::DeleteAll()
{
    return false;
}

#endif
  // wxUSE_CONFIG
