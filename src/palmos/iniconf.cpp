///////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/iniconf.cpp
// Purpose:     implementation of wxIniConfig class
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "iniconf.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef   WX_PRECOMP
  #include  "wx/string.h"
  #include  "wx/intl.h"
  #include  "wx/event.h"
  #include  "wx/app.h"
  #include  "wx/utils.h"
#endif  //WX_PRECOMP

// Doesn't yet compile in Unicode mode

#if wxUSE_CONFIG && !wxUSE_UNICODE

#include  "wx/dynarray.h"
#include  "wx/log.h"
#include  "wx/config.h"
#include  "wx/file.h"

#include  "wx/palmos/iniconf.h"

#include  "wx/palmos/wrapwin.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// we replace all path separators with this character
#define PATH_SEP_REPLACE  '_'

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ctor & dtor
// ----------------------------------------------------------------------------

wxIniConfig::wxIniConfig(const wxString& strAppName,
                         const wxString& strVendor,
                         const wxString& localFilename,
                         const wxString& globalFilename,
                         long style)
           : wxConfigBase(strAppName, strVendor, localFilename, globalFilename, style)
{
}

wxIniConfig::~wxIniConfig()
{
}

// ----------------------------------------------------------------------------
// path management
// ----------------------------------------------------------------------------

void wxIniConfig::SetPath(const wxString& strPath)
{
}

const wxString& wxIniConfig::GetPath() const
{
  static wxString s_str;

  return s_str;
}

wxString wxIniConfig::GetPrivateKeyName(const wxString& szKey) const
{
  wxString strKey;

  return strKey;
}

wxString wxIniConfig::GetKeyName(const wxString& szKey) const
{
  wxString strKey;

  return strKey;
}

// ----------------------------------------------------------------------------
// enumeration
// ----------------------------------------------------------------------------

bool wxIniConfig::GetFirstGroup(wxString& WXUNUSED(str), long& WXUNUSED(lIndex)) const
{
  return FALSE;
}

bool wxIniConfig::GetNextGroup (wxString& WXUNUSED(str), long& WXUNUSED(lIndex)) const
{
  return FALSE;
}

bool wxIniConfig::GetFirstEntry(wxString& WXUNUSED(str), long& WXUNUSED(lIndex)) const
{
  return FALSE;
}

bool wxIniConfig::GetNextEntry (wxString& WXUNUSED(str), long& WXUNUSED(lIndex)) const
{
  return FALSE;
}

// ----------------------------------------------------------------------------
// misc info
// ----------------------------------------------------------------------------

size_t wxIniConfig::GetNumberOfEntries(bool WXUNUSED(bRecursive)) const
{
  return (size_t)-1;
}

size_t wxIniConfig::GetNumberOfGroups(bool WXUNUSED(bRecursive)) const
{
  return (size_t)-1;
}

bool wxIniConfig::HasGroup(const wxString& WXUNUSED(strName)) const
{
  return FALSE;
}

bool wxIniConfig::HasEntry(const wxString& WXUNUSED(strName)) const
{
  return FALSE;
}

// is current group empty?
bool wxIniConfig::IsEmpty() const
{
  return TRUE;
}

// ----------------------------------------------------------------------------
// read/write
// ----------------------------------------------------------------------------

bool wxIniConfig::DoReadString(const wxString& szKey, wxString *pstr) const
{
  return FALSE;
}

bool wxIniConfig::DoReadLong(const wxString& szKey, long *pl) const
{
  return FALSE ;
}

bool wxIniConfig::DoWriteString(const wxString& szKey, const wxString& szValue)
{
  return FALSE;
}

bool wxIniConfig::DoWriteLong(const wxString& szKey, long lValue)
{
    return FALSE;
}

bool wxIniConfig::Flush(bool /* bCurrentOnly */)
{
    return FALSE;
}

// ----------------------------------------------------------------------------
// delete
// ----------------------------------------------------------------------------

bool wxIniConfig::DeleteEntry(const wxString& szKey, bool bGroupIfEmptyAlso)
{
    return FALSE;
}

bool wxIniConfig::DeleteGroup(const wxString& szKey)
{
    return FALSE;
}

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

bool wxIniConfig::DeleteAll()
{
  return FALSE;
}

bool wxIniConfig::RenameEntry(const wxString& WXUNUSED(oldName),
                              const wxString& WXUNUSED(newName))
{
    return FALSE;
}

bool wxIniConfig::RenameGroup(const wxString& WXUNUSED(oldName),
                              const wxString& WXUNUSED(newName))
{
    return FALSE;
}

#endif
    // wxUSE_CONFIG && wxUSE_UNICODE
