///////////////////////////////////////////////////////////////////////////////
// Name:        msw/regconf.cpp
// Purpose:
// Author:      Vadim Zeitlin
// Modified by:
// Created:     27.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "regconf.h"
#endif

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
  #pragma implementation "regconf.h"
#endif

#include  "wx/wxprec.h"

#ifdef    __BORLANDC__
  #pragma hdrstop
#endif  //__BORLANDC__

#ifndef WX_PRECOMP
  #include  <wx/string.h>
#endif //WX_PRECOMP

#include <wx/log.h>
#include <wx/config.h>
#include <wx/msw/registry.h>
#include <wx/msw/regconf.h>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// we put our data in HKLM\SOFTWARE_KEY\appname
#define SOFTWARE_KEY    wxString("Software\\")

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

// get the value if the key is opened and it exists
bool TryGetValue(const wxRegKey& key, const wxString& str, wxString& strVal)
{
  return key.IsOpened() && key.HasValue(str) && key.QueryValue(str, strVal);
}

bool TryGetValue(const wxRegKey& key, const wxString& str, long *plVal)
{
  return key.IsOpened() && key.HasValue(str) && key.QueryValue(str, plVal);
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------
wxRegConfig::wxRegConfig(const wxString& strRoot)
           : m_keyLocalRoot(wxRegKey::HKCU, SOFTWARE_KEY + strRoot),
             m_keyLocal(m_keyLocalRoot, ""),
             m_keyGlobalRoot(wxRegKey::HKLM, SOFTWARE_KEY + strRoot),
             m_keyGlobal(m_keyGlobalRoot, "")
{
  // Create() will Open() if key already exists
  m_keyLocalRoot.Create();

  wxLogNull nolog;
  m_keyGlobalRoot.Open();
}

wxRegConfig::~wxRegConfig()
{
  // nothing to do - key will be closed in their dtors
}

// ----------------------------------------------------------------------------
// path management
// ----------------------------------------------------------------------------
void wxRegConfig::SetPath(const wxString& strPath)
{
  wxArrayString aParts;

  if ( strPath.IsEmpty() )
    return;

  if ( strPath[0] == wxCONFIG_PATH_SEPARATOR ) {
    // absolute path
    wxSplitPath(aParts, strPath);
  }
  else {
    // relative path, combine with current one
    wxString strFullPath = GetPath();
    strFullPath << wxCONFIG_PATH_SEPARATOR << strPath;
    wxSplitPath(aParts, strFullPath);
  }

  // recombine path parts in one variable
  wxString strRegPath;
  m_strPath.Empty();
  for ( uint n = 0; n < aParts.Count(); n++ ) {
    strRegPath << '\\' << aParts[n];
    m_strPath << wxCONFIG_PATH_SEPARATOR << aParts[n];
  }

  // change current key(s)
  m_keyLocal.SetName(m_keyLocalRoot, strRegPath);
  m_keyGlobal.SetName(m_keyGlobalRoot, strRegPath);
  m_keyLocal.Create();

  wxLogNull nolog;
  m_keyGlobal.Open();
}

// ----------------------------------------------------------------------------
// enumeration (works only with current group)
// ----------------------------------------------------------------------------

/*
  We want to enumerate all local keys/values after the global ones, but, of
  course, we don't want to repeat a key which appears locally as well as
  globally twice.

  We use the 15th bit of lIndex for distinction between global and local.
 */

#define LOCAL_MASK        0x8000
#define IS_LOCAL_INDEX(l) (((l) & LOCAL_MASK) != 0)

bool wxRegConfig::GetFirstGroup(wxString& str, long& lIndex) const
{
  lIndex = 0;
  return GetNextGroup(str, lIndex);
}

bool wxRegConfig::GetNextGroup(wxString& str, long& lIndex) const
{
  // are we already enumerating local entries?
  if ( m_keyGlobal.IsOpened() && !IS_LOCAL_INDEX(lIndex) ) {
    // try to find a global entry which doesn't appear locally
    do {
      if ( !m_keyGlobal.GetNextKey(str, lIndex) ) {
        // no more global entries
        lIndex |= LOCAL_MASK;
        break;
      }
    } while( m_keyLocal.HasSubKey(str) );
  }

  // much easier with local entries: get the next one we find
  // (don't forget to clear our flag bit and set it again later)
  lIndex &= ~LOCAL_MASK;
  bool bOk = m_keyLocal.GetNextKey(str, lIndex);
  lIndex |= LOCAL_MASK;

  return bOk;
}

bool wxRegConfig::GetFirstEntry(wxString& str, long& lIndex) const
{
  lIndex = 0;
  return GetNextEntry(str, lIndex);
}

bool wxRegConfig::GetNextEntry(wxString& str, long& lIndex) const
{
  // are we already enumerating local entries?
  if ( m_keyGlobal.IsOpened() && !IS_LOCAL_INDEX(lIndex) ) {
    // try to find a global entry which doesn't appear locally
    do {
      if ( !m_keyGlobal.GetNextValue(str, lIndex) ) {
        // no more global entries
        lIndex |= LOCAL_MASK;
        break;
      }
    } while( m_keyLocal.HasValue(str) );
  }

  // much easier with local entries: get the next one we find
  // (don't forget to clear our flag bit and set it again later)
  lIndex &= ~LOCAL_MASK;
  bool bOk = m_keyLocal.GetNextValue(str, lIndex);
  lIndex |= LOCAL_MASK;

  return bOk;
}

uint wxRegConfig::GetNumberOfEntries(bool bRecursive) const
{
  uint nEntries = 0;

  // dummy vars
  wxString str;
  long l;
  bool bCont = ((wxRegConfig*)this)->GetFirstEntry(str, l);
  while ( bCont ) {
    nEntries++;

    bCont = ((wxRegConfig*)this)->GetNextEntry(str, l);
  }

  return nEntries;
}

uint wxRegConfig::GetNumberOfGroups(bool bRecursive) const
{
  uint nGroups = 0;

  // dummy vars
  wxString str;
  long l;
  bool bCont = ((wxRegConfig*)this)->GetFirstGroup(str, l);
  while ( bCont ) {
    nGroups++;

    bCont = ((wxRegConfig*)this)->GetNextGroup(str, l);
  }

  return nGroups;
}

// ----------------------------------------------------------------------------
// tests for existence
// ----------------------------------------------------------------------------

bool wxRegConfig::HasGroup(const wxString& strName) const
{
  return m_keyLocal.HasSubKey(strName) || m_keyGlobal.HasSubKey(strName);
}

bool wxRegConfig::HasEntry(const wxString& strName) const
{
  return m_keyLocal.HasValue(strName) || m_keyGlobal.HasValue(strName);
}

// ----------------------------------------------------------------------------
// reading/writing
// ----------------------------------------------------------------------------

bool wxRegConfig::Read(wxString *pStr,
                       const char *szKey,
                       const char *szDefault) const
{
  PathChanger path(this, szKey);

  bool bQueryGlobal = TRUE;

  // if immutable key exists in global key we must check that it's not
  // overriden by the local key with the same name
  if ( IsImmutable(path.Name()) ) {
    if ( TryGetValue(m_keyGlobal, path.Name(), *pStr) ) {
      if ( m_keyLocal.HasValue(path.Name()) ) {
        wxLogWarning("User value for immutable key '%s' ignored.",
                   path.Name().c_str());
      }

      return TRUE;
    }
    else {
      // don't waste time - it's not there anyhow
      bQueryGlobal = FALSE;
    }
  }

  // first try local key
  if ( TryGetValue(m_keyLocal, path.Name(), *pStr) ||
       (bQueryGlobal && TryGetValue(m_keyGlobal, path.Name(), *pStr)) ) {
    return TRUE;
  }

  if(IsRecordingDefaults())
     ((wxRegConfig*)this)->Write(szKey,szDefault);

  // default value
  *pStr = szDefault;
  return FALSE;
}

bool wxRegConfig::Read(long *plResult, const char *szKey, long lDefault) const
{
  PathChanger path(this, szKey);

  bool bQueryGlobal = TRUE;

  // if immutable key exists in global key we must check that it's not
  // overriden by the local key with the same name
  if ( IsImmutable(path.Name()) ) {
    if ( TryGetValue(m_keyGlobal, path.Name(), plResult) ) {
      if ( m_keyLocal.HasValue(path.Name()) ) {
        wxLogWarning("User value for immutable key '%s' ignored.",
                     path.Name().c_str());
      }

      return TRUE;
    }
    else {
      // don't waste time - it's not there anyhow
      bQueryGlobal = FALSE;
    }
  }

  // first try local key
  if ( TryGetValue(m_keyLocal, path.Name(), plResult) ||
       (bQueryGlobal && TryGetValue(m_keyGlobal, path.Name(), plResult)) ) {
    return TRUE;
  }

  // default
  *plResult = lDefault;
  return FALSE;
}

bool wxRegConfig::Write(const char *szKey, const char *szValue)
{
  PathChanger path(this, szKey);

  if ( IsImmutable(path.Name()) ) {
    wxLogError("Can't change immutable entry '%s'.", path.Name().c_str());
    return FALSE;
  }

  return m_keyLocal.SetValue(path.Name(), szValue);
}

bool wxRegConfig::Write(const char *szKey, long lValue)
{
  PathChanger path(this, szKey);

  if ( IsImmutable(path.Name()) ) {
    wxLogError("Can't change immutable entry '%s'.", path.Name().c_str());
    return FALSE;
  }

  return m_keyLocal.SetValue(path.Name(), lValue);
}

// ----------------------------------------------------------------------------
// deleting
// ----------------------------------------------------------------------------
bool wxRegConfig::DeleteEntry(const char *szValue, bool bGroupIfEmptyAlso)
{
  PathChanger path(this, szValue);

  if ( !m_keyLocal.DeleteValue(path.Name()) )
    return FALSE;

  if ( !m_keyLocal.HasSubkeys() ) {
    wxString strKey = GetPath().Right(wxCONFIG_PATH_SEPARATOR);
    SetPath("..");  // changes m_keyLocal
    return m_keyLocal.DeleteKey(strKey);
  }

  return TRUE;
}

bool wxRegConfig::DeleteGroup(const char *szKey)
{
  PathChanger path(this, szKey);

  return m_keyLocal.DeleteKey(path.Name());
}

bool wxRegConfig::DeleteAll()
{
  m_keyLocal.Close();
  m_keyGlobal.Close();

  bool bOk = m_keyLocalRoot.DeleteSelf();
  if ( bOk )
    bOk = m_keyGlobalRoot.DeleteSelf();

  return bOk;
}
