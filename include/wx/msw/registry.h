///////////////////////////////////////////////////////////////////////////////
// Name:        msw/registry.h
// Purpose:     Registry classes and functions
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     03.04.198
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _REGISTRY_H
#define   _REGISTRY_H

#ifdef __GNUG__
#pragma interface "registry.h"
#endif

// ----------------------------------------------------------------------------
// mutable hack (see also registry.cpp)
// ----------------------------------------------------------------------------
#if   wxUSE_MUTABLE
  #define MUTABLE mutable
#else
  #define MUTABLE
#endif

// ----------------------------------------------------------------------------
// types used in this module
// ----------------------------------------------------------------------------

/*
#ifndef   HKEY_DEFINED
  #define HKEY_DEFINED
  #define HKEY unsigned long
#endif
*/

typedef unsigned long ulong;

// ----------------------------------------------------------------------------
// class wxRegKey encapsulates window HKEY handle
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxRegKey 
{
public:
  // NB: do _not_ change the values of elements in these enumerations!

  // registry value types (with comments from winnt.h)
  enum ValueType
  {
    Type_None,                       // No value type
    Type_String,                     // Unicode nul terminated string
#ifdef  __WIN32__
    Type_Expand_String,              // Unicode nul terminated string
                                     // (with environment variable references)
    Type_Binary,                     // Free form binary
    Type_Dword,                      // 32-bit number
    Type_Dword_little_endian,        // 32-bit number (same as Type_DWORD)
    Type_Dword_big_endian,           // 32-bit number
    Type_Link,                       // Symbolic Link (unicode)
    Type_Multi_String,               // Multiple Unicode strings
    Type_Resource_list,              // Resource list in the resource map
    Type_Full_resource_descriptor,   // Resource list in the hardware description
    Type_Resource_requirements_list  // ???
#endif  //WIN32
  };

  // predefined registry keys
  enum StdKey
  {
    HKCR     // classes root
#ifdef  __WIN32__
    , HKCU,     // current user
    HKLM,     // local machine
    HKUSR,    // users
    HKPD     // performance data (@@ NT only?)
#if    WINVER >= 0x0400
    , HKCC,     // current config
    HKDD     // dynamic data
#endif  // Winver
#endif  // Win32/16
  };

  // information about standard (predefined) registry keys
    // number of standard keys
  static const size_t nStdKeys;
    // get the name of a standard key
  static const wxChar *GetStdKeyName(size_t key);
    // get the short name of a standard key
  static const wxChar *GetStdKeyShortName(size_t key);
    // get StdKey from root HKEY
  static StdKey GetStdKeyFromHkey(WXHKEY hkey);

  // extacts the std key prefix from the string (return value) and
  // leaves only the part after it (i.e. modifies the string passed!)
  static StdKey ExtractKeyName(wxString& str);

  // ctors
    // root key is set to HKCR (the only root key under Win16)
  wxRegKey();
    // strKey is the full name of the key (i.e. starting with HKEY_xxx...)
  wxRegKey(const wxString& strKey);
    // strKey is the name of key under (standard key) keyParent
  wxRegKey(StdKey keyParent, const wxString& strKey);
    // strKey is the name of key under (previously created) keyParent
  wxRegKey(const wxRegKey& keyParent, const wxString& strKey);
    //
 ~wxRegKey();

  // change key (closes the previously opened key if any)
    // the name is absolute, i.e. should start with HKEY_xxx
  void  SetName(const wxString& strKey);
    // the name is relative to the parent key
  void  SetName(StdKey keyParent, const wxString& strKey);
    // the name is relative to the parent key
  void  SetName(const wxRegKey& keyParent, const wxString& strKey);
    // hKey should be opened and will be closed in wxRegKey dtor
  void  SetHkey(WXHKEY hKey);

  // get infomation about the key
    // get the (full) key name. Abbreviate std root keys if bShortPrefix.
  wxString GetName(bool bShortPrefix = TRUE) const;
    // return true if the key exists
  bool  Exists() const;
    // get the info about key (any number of these pointers may be NULL)

#if defined( __GNUWIN32_OLD__ )
  bool  GetKeyInfo(size_t *pnSubKeys,      // number of subkeys
                   size_t *pnMaxKeyLen,    // max len of subkey name
                   size_t *pnValues,       // number of values
                   size_t *pnMaxValueLen) const;
#else
  bool  GetKeyInfo(ulong *pnSubKeys,      // number of subkeys
                   ulong *pnMaxKeyLen,    // max len of subkey name
                   ulong *pnValues,       // number of values
                   ulong *pnMaxValueLen) const;
#endif
    // return true if the key is opened
  bool  IsOpened() const { return m_hKey != 0; }
    // for "if ( !key ) wxLogError(...)" kind of expressions
  operator bool()  const { return m_dwLastError == 0; }

  // operations on the key itself
    // explicitly open the key (will be automatically done by all functions
    // which need the key to be opened if the key is not opened yet)
  bool  Open();
    // create the key: will fail if the key already exists and bOkIfExists
  bool  Create(bool bOkIfExists = TRUE);
    // close the key (will be automatically done in dtor)
  bool  Close();

  // deleting keys/values
    // deletes this key and all of it's subkeys/values
  bool  DeleteSelf();
    // deletes the subkey with all of it's subkeys/values recursively
  bool  DeleteKey(const wxChar *szKey);
    // deletes the named value (may be NULL to remove the default value)
  bool  DeleteValue(const wxChar *szValue);

  // access to values and subkeys
    // get value type
  ValueType GetValueType(const wxChar *szValue) const;
    // returns TRUE if the value contains a number (else it's some string)
  bool IsNumericValue(const wxChar *szValue) const;

    // assignment operators set the default value of the key
  wxRegKey& operator=(const wxString& strValue)
    { SetValue(NULL, strValue); return *this; }
  wxRegKey& operator=(long lValue)
    { SetValue(NULL, lValue); return *this; }

    // conversion operators query the default value of the key
  operator wxString() const;

    // set the string value
  bool  SetValue(const wxChar *szValue, const wxString& strValue);
    // return the string value
  bool  QueryValue(const wxChar *szValue, wxString& strValue) const;

#ifdef  __WIN32__
    // set the numeric value
  bool  SetValue(const wxChar *szValue, long lValue);
    // return the numeric value
  bool  QueryValue(const wxChar *szValue, long *plValue) const;
#endif  //Win32

  // query existence of a key/value
    // return true if value exists
  bool HasValue(const wxChar *szKey) const;
    // return true if given subkey exists
  bool HasSubKey(const wxChar *szKey) const;
    // return true if any subkeys exist
  bool HasSubkeys() const;
    // return true if any values exist
  bool HasValues() const;
    // return true if the key is empty (nothing under this key)
  bool IsEmpty() const { return !HasSubkeys() && !HasValues(); }

  // enumerate values and subkeys
  bool  GetFirstValue(wxString& strValueName, long& lIndex);
  bool  GetNextValue (wxString& strValueName, long& lIndex) const;

  bool  GetFirstKey  (wxString& strKeyName  , long& lIndex);
  bool  GetNextKey   (wxString& strKeyName  , long& lIndex) const;

private:
  // no copy ctor/assignment operator
  wxRegKey(const wxRegKey& key);            // not implemented
  wxRegKey& operator=(const wxRegKey& key); // not implemented

  WXHKEY      m_hKey,           // our handle
              m_hRootKey;       // handle of the top key (i.e. StdKey)
  wxString    m_strKey;         // key name (relative to m_hRootKey)

  MUTABLE long m_dwLastError;   // last error (0 if none)
};

#endif  //_REGISTRY_H

