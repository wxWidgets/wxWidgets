///////////////////////////////////////////////////////////////////////////////
// Name:        msw/registry.cpp
// Purpose:     implementation of registry classes and functions
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     03.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
// TODO:        - parsing of registry key names
//              - support of other (than REG_SZ/REG_DWORD) registry types
//              - add high level functions (RegisterOleServer, ...)
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "registry.h"
#endif

// for compilers that support precompilation, includes "wx.h".
#include  "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// other wxWindows headers
#include  "wx/string.h"
#include  "wx/intl.h"
#include  "wx/log.h"
#include  "wx/config.h"    // for wxExpandEnvVars

#ifndef __WIN16__

// Windows headers
/*
#define   STRICT
#define   WIN32_LEAN_AND_MEAN
*/

#include  <windows.h>

// other std headers
#include  <stdlib.h>      // for _MAX_PATH

#ifndef _MAX_PATH
	#define _MAX_PATH 512
#endif

// our header
#define   HKEY_DEFINED    // already defined in windows.h
#include  "wx/msw/registry.h"

// some registry functions don't like signed chars
typedef unsigned char *RegString;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the standard key names, short names and handles all bundled together for
// convenient access
static struct
{
  HKEY        hkey;
  const wxChar *szName;
  const wxChar *szShortName;
}
aStdKeys[] = 
{ 
  { HKEY_CLASSES_ROOT,      _T("HKEY_CLASSES_ROOT"),      _T("HKCR") },
#ifdef  __WIN32__
  { HKEY_CURRENT_USER,      _T("HKEY_CURRENT_USER"),      _T("HKCU") },
  { HKEY_LOCAL_MACHINE,     _T("HKEY_LOCAL_MACHINE"),     _T("HKLM") },
  { HKEY_USERS,             _T("HKEY_USERS"),             _T("HKU")  }, // short name?
  { HKEY_PERFORMANCE_DATA,  _T("HKEY_PERFORMANCE_DATA"),  _T("HKPD") },
#if     WINVER >= 0x0400
  { HKEY_CURRENT_CONFIG,    _T("HKEY_CURRENT_CONFIG"),    _T("HKCC") },
#ifndef __GNUWIN32__
  { HKEY_DYN_DATA,          _T("HKEY_DYN_DATA"),          _T("HKDD") }, // short name?
#endif  //GNUWIN32
#endif  //WINVER >= 4.0
#endif  //WIN32
};

// the registry name separator (perhaps one day MS will change it to '/' ;-)
#define   REG_SEPARATOR     _T('\\')

// useful for Windows programmers: makes somewhat more clear all these zeroes
// being passed to Windows APIs
#define   RESERVED        (NULL)

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------
// @ const_cast<> is not yet supported by all compilers
#define CONST_CAST    ((wxRegKey *)this)->

#if   !USE_MUTABLE
  #define m_dwLastError   CONST_CAST m_dwLastError
#endif

// ----------------------------------------------------------------------------
// non member functions
// ----------------------------------------------------------------------------

// removes the trailing backslash from the string if it has one
static inline void RemoveTrailingSeparator(wxString& str);

// returns TRUE if given registry key exists
static bool KeyExists(WXHKEY hRootKey, const wxChar *szKey);

// combines value and key name (uses static buffer!)
static const wxChar *GetFullName(const wxRegKey *pKey, 
                               const wxChar *szValue = NULL);

// ============================================================================
// implementation of wxRegKey class
// ============================================================================

// ----------------------------------------------------------------------------
// static functions and variables
// ----------------------------------------------------------------------------

const size_t wxRegKey::nStdKeys = WXSIZEOF(aStdKeys);

// @@ should take a `StdKey key', but as it's often going to be used in loops
//    it would require casts in user code.  
const wxChar *wxRegKey::GetStdKeyName(size_t key)
{
  // return empty string if key is invalid
  wxCHECK_MSG( key < nStdKeys, _T(""), _T("invalid key in wxRegKey::GetStdKeyName") );

  return aStdKeys[key].szName;
}

const wxChar *wxRegKey::GetStdKeyShortName(size_t key)
{
  // return empty string if key is invalid
  wxCHECK( key < nStdKeys, _T("") );

  return aStdKeys[key].szShortName;
}

wxRegKey::StdKey wxRegKey::ExtractKeyName(wxString& strKey)
{
  wxString strRoot = strKey.Left(REG_SEPARATOR);

  HKEY hRootKey = 0;
  size_t ui;
  for ( ui = 0; ui < nStdKeys; ui++ ) {
    if ( strRoot.CmpNoCase(aStdKeys[ui].szName) == 0 || 
         strRoot.CmpNoCase(aStdKeys[ui].szShortName) == 0 ) {
      hRootKey = aStdKeys[ui].hkey;
      break;
    }
  }

  if ( ui == nStdKeys ) {
    wxFAIL_MSG(_T("invalid key prefix in wxRegKey::ExtractKeyName."));

    hRootKey = HKEY_CLASSES_ROOT;
  }
  else {
    strKey = strKey.After(REG_SEPARATOR);
    if ( !strKey.IsEmpty() && strKey.Last() == REG_SEPARATOR )
      strKey.Truncate(strKey.Len() - 1);
  }

  return (wxRegKey::StdKey)(int)hRootKey;
}

wxRegKey::StdKey wxRegKey::GetStdKeyFromHkey(WXHKEY hkey)
{
  for ( size_t ui = 0; ui < nStdKeys; ui++ ) {
    if ( (int) aStdKeys[ui].hkey == (int) hkey )
      return (StdKey)ui;
  }
  
  wxFAIL_MSG(_T("non root hkey passed to wxRegKey::GetStdKeyFromHkey."));

  return HKCR;
}

// ----------------------------------------------------------------------------
// ctors and dtor
// ----------------------------------------------------------------------------

wxRegKey::wxRegKey()
{
  m_hKey = 0;
  m_hRootKey = (WXHKEY) aStdKeys[HKCR].hkey;
  m_dwLastError = 0;
}

wxRegKey::wxRegKey(const wxString& strKey) : m_strKey(strKey)
{
  m_hRootKey  = (WXHKEY) aStdKeys[ExtractKeyName(m_strKey)].hkey;
  m_hKey      = (WXHKEY) NULL;
  m_dwLastError = 0;
}

// parent is a predefined (and preopened) key
wxRegKey::wxRegKey(StdKey keyParent, const wxString& strKey) : m_strKey(strKey)
{
  RemoveTrailingSeparator(m_strKey);
  m_hRootKey  = (WXHKEY) aStdKeys[keyParent].hkey;
  m_hKey      = (WXHKEY) NULL;
  m_dwLastError = 0;
}

// parent is a normal regkey
wxRegKey::wxRegKey(const wxRegKey& keyParent, const wxString& strKey)
        : m_strKey(keyParent.m_strKey)
{
  // combine our name with parent's to get the full name
  if ( !m_strKey.IsEmpty() && 
       (strKey.IsEmpty() || strKey[0] != REG_SEPARATOR) ) {
      m_strKey += REG_SEPARATOR;
  }

  m_strKey += strKey;
  RemoveTrailingSeparator(m_strKey);

  m_hRootKey  = keyParent.m_hRootKey;
  m_hKey      = (WXHKEY) NULL;
  m_dwLastError = 0;
}

// dtor closes the key releasing system resource
wxRegKey::~wxRegKey()
{
  Close();
}

// ----------------------------------------------------------------------------
// change the key name/hkey
// ----------------------------------------------------------------------------

// set the full key name
void wxRegKey::SetName(const wxString& strKey)
{
  Close();

  m_strKey = strKey;
  m_hRootKey = (WXHKEY) aStdKeys[ExtractKeyName(m_strKey)].hkey;
}

// the name is relative to the parent key
void wxRegKey::SetName(StdKey keyParent, const wxString& strKey)
{
  Close();

  m_strKey = strKey;
  RemoveTrailingSeparator(m_strKey);
  m_hRootKey = (WXHKEY) aStdKeys[keyParent].hkey;
}

// the name is relative to the parent key
void wxRegKey::SetName(const wxRegKey& keyParent, const wxString& strKey)
{
  Close();

  // combine our name with parent's to get the full name
  m_strKey = keyParent.m_strKey;
  if ( !strKey.IsEmpty() && strKey[0] != REG_SEPARATOR )
    m_strKey += REG_SEPARATOR;
  m_strKey += strKey;

  RemoveTrailingSeparator(m_strKey);

  m_hRootKey = keyParent.m_hRootKey;
}

// hKey should be opened and will be closed in wxRegKey dtor
void wxRegKey::SetHkey(WXHKEY hKey)
{
  Close();

  m_hKey = hKey;
}

// ----------------------------------------------------------------------------
// info about the key
// ----------------------------------------------------------------------------

// returns TRUE if the key exists
bool wxRegKey::Exists() const
{
  // opened key has to exist, try to open it if not done yet
  return IsOpened() ? TRUE : KeyExists(m_hRootKey, m_strKey);
}

// returns the full name of the key (prefix is abbreviated if bShortPrefix)
wxString wxRegKey::GetName(bool bShortPrefix) const
{
  StdKey key = GetStdKeyFromHkey((StdKey) m_hRootKey);
  wxString str = bShortPrefix ? aStdKeys[key].szShortName 
                              : aStdKeys[key].szName;
  if ( !m_strKey.IsEmpty() )
    str << "\\" << m_strKey;

  return str;
}

#if defined( __GNUWIN32__ ) && !defined(wxUSE_NORLANDER_HEADERS)
bool wxRegKey::GetKeyInfo(size_t* pnSubKeys,
                          size_t* pnMaxKeyLen,
                          size_t* pnValues,
                          size_t* pnMaxValueLen) const
#else
bool wxRegKey::GetKeyInfo(ulong *pnSubKeys,
                          ulong *pnMaxKeyLen,
                          ulong *pnValues,
                          ulong *pnMaxValueLen) const
#endif
{
#if defined(__WIN32__) && !defined(__TWIN32__)
  m_dwLastError = ::RegQueryInfoKey
                  (
                    (HKEY) m_hKey,
                    NULL,           // class name
                    NULL,           // (ptr to) size of class name buffer
                    RESERVED,
                    pnSubKeys,      // [out] number of subkeys
                    pnMaxKeyLen,    // [out] max length of a subkey name
                    NULL,           // longest subkey class name
                    pnValues,       // [out] number of values
                    pnMaxValueLen,  // [out] max length of a value name
                    NULL,           // longest value data
                    NULL,           // security descriptor
                    NULL            // time of last modification
                  );

  if ( m_dwLastError != ERROR_SUCCESS ) {
    wxLogSysError(m_dwLastError, _("can't get info about registry key '%s'"),
                  GetName().c_str());
    return FALSE;
  }
  else
    return TRUE;
#else // Win16
  wxFAIL_MSG("GetKeyInfo() not implemented");

  return FALSE;
#endif
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

// opens key (it's not an error to call Open() on an already opened key)
bool wxRegKey::Open()
{
  if ( IsOpened() )
    return TRUE;

  HKEY tmpKey;
  m_dwLastError = RegOpenKey((HKEY) m_hRootKey, m_strKey, &tmpKey);
  if ( m_dwLastError != ERROR_SUCCESS ) {
    wxLogSysError(m_dwLastError, _("can't open registry key '%s'"),
                  GetName().c_str());
    return FALSE;
  }
  else
  {
    m_hKey = (WXHKEY) tmpKey;
    return TRUE;
  }
}

// creates key, failing if it exists and !bOkIfExists
bool wxRegKey::Create(bool bOkIfExists)
{
  // check for existence only if asked (i.e. order is important!)
  if ( !bOkIfExists && Exists() ) {
    return FALSE;
  }

  if ( IsOpened() )
    return TRUE;

  HKEY tmpKey;
  m_dwLastError = RegCreateKey((HKEY) m_hRootKey, m_strKey, &tmpKey);
  if ( m_dwLastError != ERROR_SUCCESS ) {
    wxLogSysError(m_dwLastError, _("can't create registry key '%s'"),
                  GetName().c_str());
    return FALSE;
  }
  else
  {
    m_hKey = (WXHKEY) tmpKey;
    return TRUE;
  }
}

// close the key, it's not an error to call it when not opened
bool wxRegKey::Close()
{
  if ( IsOpened() ) {
    m_dwLastError = RegCloseKey((HKEY) m_hKey);
    if ( m_dwLastError != ERROR_SUCCESS ) {
      wxLogSysError(m_dwLastError, _("can't close registry key '%s'"),
                    GetName().c_str());

      m_hKey = 0;
      return FALSE;
    }
    else {
      m_hKey = 0;
    }
  }

  return TRUE;
}

// ----------------------------------------------------------------------------
// delete keys/values
// ----------------------------------------------------------------------------
bool wxRegKey::DeleteSelf()
{
  {
    wxLogNull nolog;
    if ( !Open() ) {
      // it already doesn't exist - ok!
      return TRUE;
    }
  }

  // prevent a buggy program from erasing one of the root registry keys or an
  // immediate subkey (i.e. one which doesn't have '\\' inside) of any other
  // key except HKCR (HKCR has some "deleteable" subkeys)
  if ( m_strKey.IsEmpty() || (m_hRootKey != HKCR &&
       m_strKey.Find(REG_SEPARATOR) == wxNOT_FOUND) ) {
      wxLogError(_("Registry key '%s' is needed for normal system operation,\n"
                   "deleting it will leave your system in unusable state:\n"
                   "operation aborted."), GetFullName(this));

      return FALSE;
  }

  // we can't delete keys while enumerating because it confuses GetNextKey, so
  // we first save the key names and then delete them all
  wxArrayString astrSubkeys;

  wxString strKey;
  long lIndex;
  bool bCont = GetFirstKey(strKey, lIndex);
  while ( bCont ) {
    astrSubkeys.Add(strKey);

    bCont = GetNextKey(strKey, lIndex);
  }

  size_t nKeyCount = astrSubkeys.Count();
  for ( size_t nKey = 0; nKey < nKeyCount; nKey++ ) {
    wxRegKey key(*this, astrSubkeys[nKey]);
    if ( !key.DeleteSelf() )
      return FALSE;
  }

  // now delete this key itself
  Close();

  m_dwLastError = RegDeleteKey((HKEY) m_hRootKey, m_strKey);
  if ( m_dwLastError != ERROR_SUCCESS ) {
    wxLogSysError(m_dwLastError, _("can't delete key '%s'"),
                  GetName().c_str());
    return FALSE;
  }

  return TRUE;
}

bool wxRegKey::DeleteKey(const wxChar *szKey)
{
  if ( !Open() )
    return FALSE;

  wxRegKey key(*this, szKey);
  return key.DeleteSelf();
}

bool wxRegKey::DeleteValue(const wxChar *szValue)
{
  if ( !Open() )
    return FALSE;

#if defined(__WIN32__) && !defined(__TWIN32__)
    m_dwLastError = RegDeleteValue((HKEY) m_hKey, WXSTRINGCAST szValue);
    if ( m_dwLastError != ERROR_SUCCESS ) {
      wxLogSysError(m_dwLastError, _("can't delete value '%s' from key '%s'"),
                    szValue, GetName().c_str());
      return FALSE;
    }
  #else   //WIN16
    // named registry values don't exist in Win16 world
    wxASSERT( IsEmpty(szValue) );

    // just set the (default and unique) value of the key to ""
    m_dwLastError = RegSetValue((HKEY) m_hKey, NULL, REG_SZ, "", RESERVED);
    if ( m_dwLastError != ERROR_SUCCESS ) {
      wxLogSysError(m_dwLastError, _("can't delete value of key '%s'"),
                    GetName().c_str());
      return FALSE;
    }
  #endif  //WIN16/32

  return TRUE;
}

// ----------------------------------------------------------------------------
// access to values and subkeys
// ----------------------------------------------------------------------------

// return TRUE if value exists
bool wxRegKey::HasValue(const wxChar *szValue) const
{
  // this function should be silent, so suppress possible messages from Open()
  wxLogNull nolog;
  
  #ifdef  __WIN32__
    if ( CONST_CAST Open() ) {
      return RegQueryValueEx((HKEY) m_hKey, WXSTRINGCAST szValue, RESERVED,
                             NULL, NULL, NULL) == ERROR_SUCCESS;
    }
    else
      return FALSE;
  #else   // WIN16
    // only unnamed value exists
    return IsEmpty(szValue);
  #endif  // WIN16/32
}

// returns TRUE if this key has any values
bool wxRegKey::HasValues() const
{
  // suppress possible messages from GetFirstValue()
  wxLogNull nolog;
  
  // just call GetFirstValue with dummy parameters
  wxString str;
  long     l;
  return CONST_CAST GetFirstValue(str, l);
}

// returns TRUE if this key has any subkeys
bool wxRegKey::HasSubkeys() const
{
  // suppress possible messages from GetFirstKey()
  wxLogNull nolog;
  
  // just call GetFirstKey with dummy parameters
  wxString str;
  long     l;
  return CONST_CAST GetFirstKey(str, l);
}

// returns TRUE if given subkey exists
bool wxRegKey::HasSubKey(const wxChar *szKey) const
{
  // this function should be silent, so suppress possible messages from Open()
  wxLogNull nolog;
  
  if ( CONST_CAST Open() )
    return KeyExists(m_hKey, szKey);
  else
    return FALSE;
}

wxRegKey::ValueType wxRegKey::GetValueType(const wxChar *szValue) const
{
  #ifdef  __WIN32__
    if ( ! CONST_CAST Open() )
      return Type_None;

    DWORD dwType;
    m_dwLastError = RegQueryValueEx((HKEY) m_hKey, WXSTRINGCAST szValue, RESERVED,
                                    &dwType, NULL, NULL);
    if ( m_dwLastError != ERROR_SUCCESS ) {
      wxLogSysError(m_dwLastError, _("can't read value of key '%s'"),
                    GetName().c_str());
      return Type_None;
    }

    return (ValueType)dwType;
  #else   //WIN16
    return IsEmpty(szValue) ? Type_String : Type_None;
  #endif  //WIN16/32
}

#ifdef  __WIN32__
bool wxRegKey::SetValue(const wxChar *szValue, long lValue)
{
#ifdef __TWIN32__
  wxFAIL_MSG("RegSetValueEx not implemented by TWIN32");
  return FALSE;
#else
  if ( CONST_CAST Open() ) {
    m_dwLastError = RegSetValueEx((HKEY) m_hKey, szValue, (DWORD) RESERVED, REG_DWORD,
                                  (RegString)&lValue, sizeof(lValue));
    if ( m_dwLastError == ERROR_SUCCESS )
      return TRUE;
  }

  wxLogSysError(m_dwLastError, _("can't set value of '%s'"),
                GetFullName(this, szValue));
  return FALSE;
#endif
}

bool wxRegKey::QueryValue(const wxChar *szValue, long *plValue) const
{
  if ( CONST_CAST Open() ) {
    DWORD dwType, dwSize = sizeof(DWORD);
    RegString pBuf = (RegString)plValue;
    m_dwLastError = RegQueryValueEx((HKEY) m_hKey, WXSTRINGCAST szValue, RESERVED,
                                    &dwType, pBuf, &dwSize);
    if ( m_dwLastError != ERROR_SUCCESS ) {
      wxLogSysError(m_dwLastError, _("can't read value of key '%s'"),
                    GetName().c_str());
      return FALSE;
    }
    else {
      // check that we read the value of right type
      wxASSERT_MSG( dwType == REG_DWORD, 
                    _T("Type mismatch in wxRegKey::QueryValue().")  );

      return TRUE;
    }
  }
  else
    return FALSE;
}

#endif  //Win32

bool wxRegKey::QueryValue(const wxChar *szValue, wxString& strValue) const
{
  if ( CONST_CAST Open() ) {
    #ifdef  __WIN32__
      // first get the type and size of the data
      DWORD dwType, dwSize;
      m_dwLastError = RegQueryValueEx((HKEY) m_hKey, WXSTRINGCAST szValue, RESERVED,
                                      &dwType, NULL, &dwSize);
      if ( m_dwLastError == ERROR_SUCCESS ) {
        RegString pBuf = (RegString)strValue.GetWriteBuf(dwSize);
        m_dwLastError = RegQueryValueEx((HKEY) m_hKey, WXSTRINGCAST szValue, RESERVED,
                                        &dwType, pBuf, &dwSize);
        strValue.UngetWriteBuf();
        if ( m_dwLastError == ERROR_SUCCESS ) {
          // check that it was the right type
          wxASSERT_MSG( dwType == REG_SZ, 
                        _T("Type mismatch in wxRegKey::QueryValue().") );

          return TRUE;
        }
      }
    #else   //WIN16
      // named registry values don't exist in Win16
      wxASSERT( IsEmpty(szValue) );

      m_dwLastError = RegQueryValue((HKEY) m_hKey, 0, strValue.GetWriteBuf(256), &l);
      strValue.UngetWriteBuf();
      if ( m_dwLastError == ERROR_SUCCESS )
        return TRUE;
    #endif  //WIN16/32
  }

  wxLogSysError(m_dwLastError, _("can't read value of '%s'"),
                GetFullName(this, szValue));
  return FALSE;
}

bool wxRegKey::SetValue(const wxChar *szValue, const wxString& strValue)
{
  if ( CONST_CAST Open() ) {
#if defined( __WIN32__) && !defined(__TWIN32__)
      m_dwLastError = RegSetValueEx((HKEY) m_hKey, szValue, (DWORD) RESERVED, REG_SZ,
                                    (RegString)strValue.c_str(), 
                                    strValue.Len() + 1);
      if ( m_dwLastError == ERROR_SUCCESS )
        return TRUE;
    #else   //WIN16
      // named registry values don't exist in Win16
      wxASSERT( IsEmpty(szValue) );

      m_dwLastError = RegSetValue((HKEY) m_hKey, NULL, REG_SZ, strValue, NULL);
      if ( m_dwLastError == ERROR_SUCCESS )
        return TRUE;
    #endif  //WIN16/32
  }

  wxLogSysError(m_dwLastError, _("can't set value of '%s'"),
                GetFullName(this, szValue));
  return FALSE;
}

wxRegKey::operator wxString() const
{
  wxString str;
  QueryValue(NULL, str);
  return str;
}

// ----------------------------------------------------------------------------
// enumeration
// NB: all these functions require an index variable which allows to have
//     several concurrently running indexations on the same key
// ----------------------------------------------------------------------------

bool wxRegKey::GetFirstValue(wxString& strValueName, long& lIndex)
{
  if ( !Open() )
    return FALSE;

  lIndex = 0;
  return GetNextValue(strValueName, lIndex);
}

bool wxRegKey::GetNextValue(wxString& strValueName, long& lIndex) const
{
  wxASSERT( IsOpened() );

  // are we already at the end of enumeration?
  if ( lIndex == -1 )
    return FALSE;

#if defined( __WIN32__) && !defined(__TWIN32__)
    wxChar  szValueName[1024];                  // @@ use RegQueryInfoKey...
    DWORD dwValueLen = WXSIZEOF(szValueName);

    m_dwLastError = RegEnumValue((HKEY) m_hKey, lIndex++,
                                 szValueName, &dwValueLen,
                                 RESERVED, 
                                 NULL,            // [out] type 
                                 NULL,            // [out] buffer for value
                                 NULL);           // [i/o]  it's length

    if ( m_dwLastError != ERROR_SUCCESS ) {
      if ( m_dwLastError == ERROR_NO_MORE_ITEMS ) {
        m_dwLastError = ERROR_SUCCESS;
        lIndex = -1;
      }
      else {
        wxLogSysError(m_dwLastError, _("can't enumerate values of key '%s'"),
                      GetName().c_str());
      }

      return FALSE;
    }

    strValueName = szValueName;
  #else   //WIN16
    // only one unnamed value
    wxASSERT( lIndex == 0 );

    lIndex = -1;
    strValueName.Empty();
  #endif

  return TRUE;
}

bool wxRegKey::GetFirstKey(wxString& strKeyName, long& lIndex)
{
  if ( !Open() )
    return FALSE;

  lIndex = 0;
  return GetNextKey(strKeyName, lIndex);
}

bool wxRegKey::GetNextKey(wxString& strKeyName, long& lIndex) const
{
  wxASSERT( IsOpened() );

  // are we already at the end of enumeration?
  if ( lIndex == -1 )
    return FALSE;

  wxChar szKeyName[_MAX_PATH + 1];
  m_dwLastError = RegEnumKey((HKEY) m_hKey, lIndex++, szKeyName, WXSIZEOF(szKeyName));

  if ( m_dwLastError != ERROR_SUCCESS ) {
    if ( m_dwLastError == ERROR_NO_MORE_ITEMS ) {
      m_dwLastError = ERROR_SUCCESS;
      lIndex = -1;
    }
    else {
      wxLogSysError(m_dwLastError, _("can't enumerate subkeys of key '%s'"),
                    GetName().c_str());
    }

    return FALSE;
  }

  strKeyName = szKeyName;
  return TRUE;
}

// returns TRUE if the value contains a number (else it's some string)
bool wxRegKey::IsNumericValue(const wxChar *szValue) const
  { 
      ValueType type = GetValueType(szValue);
      switch ( type ) {
        case Type_Dword:
        case Type_Dword_little_endian:
        case Type_Dword_big_endian:
            return TRUE;

        default:
            return FALSE;
      }
  }

// ============================================================================
// implementation of global private functions
// ============================================================================
bool KeyExists(WXHKEY hRootKey, const wxChar *szKey)
{
  HKEY hkeyDummy;
  if ( RegOpenKey( (HKEY) hRootKey, szKey, &hkeyDummy) == ERROR_SUCCESS ) {
    RegCloseKey(hkeyDummy);
    return TRUE;
  }
  else
    return FALSE;
}

const wxChar *GetFullName(const wxRegKey *pKey, const wxChar *szValue)
{
  static wxString s_str;
  s_str = pKey->GetName();
  if ( !wxIsEmpty(szValue) )
    s_str << _T("\\") << szValue;

  return s_str.c_str();
}

void RemoveTrailingSeparator(wxString& str)
{
  if ( !str.IsEmpty() && str.Last() == REG_SEPARATOR )
    str.Truncate(str.Len() - 1);
}

#endif
  // __WIN16__

