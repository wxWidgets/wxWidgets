///////////////////////////////////////////////////////////////////////////////
// Name:        confbase.h
// Purpose:     declaration of the base class of all config implementations
//              (see also: fileconf.h and msw/regconf.h)
// Author:      Karsten Ballüder & Vadim Zeitlin
// Modified by:
// Created:     07.04.98 (adapted from appconf.h)
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Karsten Ballüder   Ballueder@usa.net
//                       Vadim Zeitlin      <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_CONFBASE_H_
#define   _WX_CONFBASE_H_

#ifdef __GNUG__
#pragma interface "confbase.h"
#endif

// ----------------------------------------------------------------------------
// compile options
// ----------------------------------------------------------------------------

// it won't compile without it anyhow
#ifndef wxUSE_CONFIG
  #error "Please define wxUSE_CONFIG or remove config.cpp from your makefile"
#endif // wxUSE_CONFIG

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

/// shall we be case sensitive in parsing variable names?
#ifndef wxCONFIG_CASE_SENSITIVE
  #define  wxCONFIG_CASE_SENSITIVE       FALSE
#endif

/// separates group and entry names (probably shouldn't be changed)
#ifndef wxCONFIG_PATH_SEPARATOR
  #define   wxCONFIG_PATH_SEPARATOR     '/'
#endif

/// introduces immutable entries
// (i.e. the ones which can't be changed from the local config file)
#ifndef wxCONFIG_IMMUTABLE_PREFIX
  #define   wxCONFIG_IMMUTABLE_PREFIX   '!'
#endif

/// should we use registry instead of configuration files under Win32?
// (i.e. whether wxConfigBase::Create() will create a wxFileConfig (if it's
//  FALSE) or wxRegConfig (if it's true and we're under Win32) or wxIniConfig
//  (under Win16))
#ifndef   wxCONFIG_WIN32_NATIVE
  #define wxCONFIG_WIN32_NATIVE          TRUE
#endif

// Style flags for constructor style parameter
#define wxCONFIG_USE_LOCAL_FILE         1
#define wxCONFIG_USE_GLOBAL_FILE        2

// ----------------------------------------------------------------------------
// various helper global functions
// ----------------------------------------------------------------------------

/*
  Replace environment variables ($SOMETHING) with their values. The format is
  $VARNAME or ${VARNAME} where VARNAME contains alphanumeric characters and
  '_' only. '$' must be escaped ('\$') in order to be taken literally.
 */
extern wxString wxExpandEnvVars(const wxString &sz);

/*
  Split path into parts removing '..' in progress
 */
extern void wxSplitPath(wxArrayString& aParts, const char *sz);

// ----------------------------------------------------------------------------
// abstract base class wxConfigBase which defines the interface for derived 
// classes
//
// wxConfig organizes the items in a tree-like structure (modeled after the
// Unix/Dos filesystem). There are groups (directories) and keys (files).
// There is always one current group given by the current path.
//
// Keys are pairs "key_name = value" where value may be of string or integer
// (long) type (@@@ doubles and other types such as wxDate coming soon).
// ----------------------------------------------------------------------------
class wxConfigBase
{
public:
  // static functions
    // sets the config object, returns the previous pointer
  static wxConfigBase *Set(wxConfigBase *pConfig);
    // get the config object, creates it on demand unless DontCreateOnDemand
    // was called
  static wxConfigBase *Get() { if ( !ms_pConfig ) Create(); return ms_pConfig; }
    // create a new config object: this function will create the "best"
    // implementation of wxConfig available for the current platform, see
    // comments near definition wxCONFIG_WIN32_NATIVE for details. It returns
    // the created object and also sets it as ms_pConfig.
  static wxConfigBase *Create();
    // should Get() try to create a new log object if the current one is NULL?
  static void DontCreateOnDemand() { ms_bAutoCreate = FALSE; }

  // ctor & virtual dtor
    // environment variable expansion is on by default
//  wxConfigBase() { m_bExpandEnvVars = TRUE; m_bRecordDefaults = FALSE; }

  // ctor

  // Not all args will always be used by derived classes, but
  // including them all in each class ensures compatibility.
  // If appName is empty, uses wxApp name
  wxConfigBase(const wxString& appName = "", const wxString& vendorName = "",
               const wxString& localFilename = "", const wxString& globalFilename = "",
               long style = 0);

    // empty but ensures that dtor of all derived classes is virtual
  virtual ~wxConfigBase() { }

  // path management
    // set current path: if the first character is '/', it's the absolute path,
    // otherwise it's a relative path. '..' is supported. If the strPath
    // doesn't exist it is created.
  virtual void SetPath(const wxString& strPath) = 0;
    // retrieve the current path (always as absolute path)
  virtual const wxString& GetPath() const = 0;

  // enumeration: all functions here return false when there are no more items.
  // you must pass the same lIndex to GetNext and GetFirst (don't modify it)
    // enumerate subgroups
  virtual bool GetFirstGroup(wxString& str, long& lIndex) const = 0;
  virtual bool GetNextGroup (wxString& str, long& lIndex) const = 0;
    // enumerate entries
  virtual bool GetFirstEntry(wxString& str, long& lIndex) const = 0;
  virtual bool GetNextEntry (wxString& str, long& lIndex) const = 0;
    // get number of entries/subgroups in the current group, with or without
    // it's subgroups
  virtual size_t GetNumberOfEntries(bool bRecursive = FALSE) const = 0;
  virtual size_t GetNumberOfGroups(bool bRecursive = FALSE) const = 0;

  // tests of existence
    // returns TRUE if the group by this name exists
  virtual bool HasGroup(const wxString& strName) const = 0;
    // same as above, but for an entry
  virtual bool HasEntry(const wxString& strName) const = 0;
    // returns TRUE if either a group or an entry with a given name exist
  bool Exists(const wxString& strName) const
    { return HasGroup(strName) || HasEntry(strName); }

  // key access: returns TRUE if value was really read, FALSE if default used
  // (and if the key is not found the default value is returned.)
    // read a string from the key
  virtual bool Read(const wxString& key, wxString *pStr) const = 0;
  virtual bool Read(const wxString& key, wxString *pStr, const wxString& defVal) const;

  virtual wxString Read(const wxString& key, const wxString& defVal = "") const;

  virtual bool Read(const wxString& key, long *pl) const = 0;
  virtual bool Read(const wxString& key, long *pl, long defVal) const;

  virtual long Read(const wxString& strKey, long defVal) const
    { long l; Read(strKey, &l, defVal); return l; }

  // Convenience functions that are built on other forms
  // double
  virtual bool Read(const wxString& key, double* val) const;
  virtual bool Read(const wxString& key, double* val, double defVal) const;

  // bool
  virtual bool Read(const wxString& key, bool* val) const;
  virtual bool Read(const wxString& key, bool* val, bool defVal) const;

    // write the value (return true on success)
  virtual bool Write(const wxString& key, const wxString& value) = 0;
  virtual bool Write(const wxString& key, long value) = 0;

  // Convenience functions
  virtual bool Write(const wxString& key, double value);
  virtual bool Write(const wxString& key, bool value);

    // permanently writes all changes
  virtual bool Flush(bool bCurrentOnly = FALSE) = 0;

  // delete entries/groups
    // deletes the specified entry and the group it belongs to if
    // it was the last key in it and the second parameter is true
  virtual bool DeleteEntry(const wxString& key,
                           bool bDeleteGroupIfEmpty = TRUE) = 0;
    // delete the group (with all subgroups)
  virtual bool DeleteGroup(const wxString& key) = 0;
    // delete the whole underlying object (disk file, registry key, ...)
    // primarly for use by desinstallation routine.
  virtual bool DeleteAll() = 0;

  // options
    // we can automatically expand environment variables in the config entries
    // (this option is on by default, you can turn it on/off at any time)
  bool IsExpandingEnvVars() const { return m_bExpandEnvVars; }
  void SetExpandEnvVars(bool bDoIt = TRUE) { m_bExpandEnvVars = bDoIt; }
    // recording of default values
  void SetRecordDefaults(bool bDoIt = TRUE) { m_bRecordDefaults = bDoIt; }
  bool IsRecordingDefaults() const { return m_bRecordDefaults; }
  // does expansion only if needed
  wxString ExpandEnvVars(const wxString& str) const
    {
        wxString tmp; // Required for BC++
        if (IsExpandingEnvVars())
            tmp = wxExpandEnvVars(str);
        else
            tmp = str;
        return tmp;
    }

    // misc accessors
  inline wxString GetAppName() const { return m_appName; }
  inline wxString GetVendorName() const { return m_vendorName; }

  inline void SetAppName(const wxString& appName) { m_appName = appName; }
  inline void SetVendorName(const wxString& vendorName) { m_vendorName = vendorName; }

  inline void SetStyle(long style) { m_style = style; }
  inline long GetStyle() const { return m_style; }

protected:
  static bool IsImmutable(const wxString& key)
    { return key[0] == wxCONFIG_IMMUTABLE_PREFIX; }

private:
  // are we doing automatic environment variable expansion?
  bool m_bExpandEnvVars;
  // do we record default values?
  bool m_bRecordDefaults;
  
  // static variables
  static wxConfigBase *ms_pConfig;
  static bool          ms_bAutoCreate;

  // Application name and organisation name
  wxString          m_appName;
  wxString          m_vendorName;

  // Style flag
  long              m_style;
};

  // a handy little class which changes current path to the path of given entry
  // and restores it in dtor: so if you declare a local variable of this type,
  // you work in the entry directory and the path is automatically restored
  // when the function returns
  // Taken out of wxConfig since not all compilers can cope with nested classes.
  class wxConfigPathChanger
  {
  public:
    // ctor/dtor do path changing/restorin
    wxConfigPathChanger(const wxConfigBase *pContainer, const wxString& strEntry);
   ~wxConfigPathChanger();

    // get the key name
    const wxString& Name() const { return m_strName; }

  private:
    wxConfigBase *m_pContainer;   // object we live in
    wxString      m_strName,      // name of entry (i.e. name only)
                  m_strOldPath;   // saved path
    bool          m_bChanged;     // was the path changed?
  };


// ----------------------------------------------------------------------------
// the native wxConfigBase implementation
// ----------------------------------------------------------------------------

// under Windows we prefer to use the native implementation
#if defined(__WXMSW__) && wxCONFIG_WIN32_NATIVE
  #ifdef __WIN32__
    #define wxConfig  wxRegConfig
    #define sm_classwxConfig sm_classwxRegConfig
  #else  //WIN16
    #define wxConfig  wxIniConfig
    #define sm_classwxConfig sm_classwxIniConfig
  #endif
#else // either we're under Unix or wish to use files even under Windows
  #define wxConfig  wxFileConfig
  #define sm_classwxConfig sm_classwxFileConfig
#endif



#endif  // _WX_CONFIG_H_

