///////////////////////////////////////////////////////////////////////////////
// Name:        
// Purpose:     
// Author:      Karsten Ballüder & Vadim Zeitlin
// Modified by: 
// Created:     07.04.98 (adapted from appconf.h)
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Karsten Ballüder   Ballueder@usa.net  
//                       Vadim Zeitlin      <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef   _APPCONF_H
#define   _APPCONF_H

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

/// shall we be case sensitive in parsing variable names?
#ifndef APPCONF_CASE_SENSITIVE
  #define  APPCONF_CASE_SENSITIVE       FALSE
#endif

/// separates group and entry names
#ifndef APPCONF_PATH_SEPARATOR
  #define   APPCONF_PATH_SEPARATOR     '/'
#endif

/// introduces immutable entries
#ifndef APPCONF_IMMUTABLE_PREFIX
  #define   APPCONF_IMMUTABLE_PREFIX   '!'
#endif

/// should we use registry instead of configuration files under Win32?
#ifndef   APPCONF_WIN32_NATIVE
  #define APPCONF_WIN32_NATIVE          TRUE
#endif

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

/*
  Replace environment variables ($SOMETHING) with their values. The format is
  $VARNAME or ${VARNAME} where VARNAME contains alphanumeric characters and
  '_' only. '$' must be escaped ('\$') in order to be taken literally.
*/
wxString ExpandEnvVars(const wxString& str);

// ----------------------------------------------------------------------------
// abstract base class wxConfig which defines the interface for derived classes
//
// wxConfig organizes the items in a tree-like structure (modeled after the 
// Unix/Dos filesystem). There are groups (directories) and keys (files). 
// There is always one current group given by the current path.
//
// Keys are pairs "key_name = value" where value may be of string or integer
// (long) type (@@@ doubles and other types such as wxDate coming soon).
// ----------------------------------------------------------------------------
class wxConfig
{
public:
  // ctor & virtual dtor
  wxConfig() { }
  virtual ~wxConfig();

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
  virtual bool GetFirstGroup(wxString& str, long& lIndex) = 0;
  virtual bool GetNextGroup (wxString& str, long& lIndex) = 0;
    // enumerate entries
  virtual bool GetFirstEntry(wxString& str, long& lIndex) = 0;
  virtual bool GetNextEntry (wxString& str, long& lIndex) = 0;

  // key access
    // read a string or long value from the key. If the key is not
    // found the default value is returned.
    virtual const char *Read(const char *szKey,
                             const char *szDefault = NULL) const = 0;
    virtual long Read(const char *szKey, long lDefault) const = 0;

    // write the value (return true on success)
  virtual bool Write(const char *szKey, const char *szValue) = 0;
  virtual bool Write(const char *szKey, long lValue) = 0;
    // permanently writes all changes
  virtual bool Flush(bool bCurrentOnly = FALSE) = 0;

  // delete entries/groups
    // deletes the specified entry and the group it belongs to if
    // it was the last key in it and the second parameter is true
  virtual bool DeleteEntry(const char *szKey,
                           bool bDeleteGroupIfEmpty = TRUE) = 0;
    // delete the group (with all subgroups)
  virtual bool DeleteGroup(const char *szKey) = 0;
    // delete the whole underlying object (disk file, registry key, ...)
    // primarly for use by desinstallation routine.
  virtual bool DeleteAll() = 0;

protected:
  // true if environment variables are to be auto-expanded
  bool m_bExpandEnvVars;
};

// ----------------------------------------------------------------------------
// functions to create different config implementations
// ----------------------------------------------------------------------------

wxConfig *CreateFileConfig(const wxString& strFile, bool bLocalOnly = FALSE);

#endif  //_APPCONF_H

