///////////////////////////////////////////////////////////////////////////////
// Name:        msw/regconf.h  
// Purpose:     Registry based implementation of wxConfigBase
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     27.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _REGCONF_H
#define   _REGCONF_H

#ifdef __GNUG__
#pragma interface "regconf.h"
#endif

#ifndef   _REGISTRY_H
  #include <wx/msw/registry.h>
#endif

// ----------------------------------------------------------------------------
// wxRegConfig
// ----------------------------------------------------------------------------

class wxRegConfig : public wxConfigBase
{
public:
  // ctor & dtor
    // will store data in HKLM\appName and HKCU\appName
  wxRegConfig(const wxString& appName = "", const wxString& vendorName = "",
              const wxString& localFilename = "", const wxString& globalFilename = "",
              long style = 0);

    // dtor will save unsaved data
  virtual ~wxRegConfig();

  // implement inherited pure virtual functions
  // ------------------------------------------

  // path management
  virtual void SetPath(const wxString& strPath);
  virtual const wxString& GetPath() const { return m_strPath; }

  // entry/subgroup info
    // enumerate all of them
  virtual bool GetFirstGroup(wxString& str, long& lIndex) const;
  virtual bool GetNextGroup (wxString& str, long& lIndex) const;
  virtual bool GetFirstEntry(wxString& str, long& lIndex) const;
  virtual bool GetNextEntry (wxString& str, long& lIndex) const;

    // tests for existence
  virtual bool HasGroup(const wxString& strName) const;
  virtual bool HasEntry(const wxString& strName) const;

    // get number of entries/subgroups in the current group, with or without
    // it's subgroups
  virtual size_t GetNumberOfEntries(bool bRecursive = FALSE) const;
  virtual size_t GetNumberOfGroups(bool bRecursive = FALSE) const;

  // read/write
  bool Read(const wxString& key, wxString *pStr) const;
  bool Read(const wxString& key, wxString *pStr, const wxString& szDefault) const;
  bool Read(const wxString& key, long *plResult) const;

  // The following are necessary to satisfy the compiler
  wxString Read(const wxString& key, const wxString& defVal) const
    { return wxConfigBase::Read(key, defVal); }
  bool Read(const wxString& key, long *pl, long defVal) const
    { return wxConfigBase::Read(key, pl, defVal); }
  bool Read(const wxString& key, int *pi, int defVal) const
    { return wxConfigBase::Read(key, pi, defVal); }
  bool Read(const wxString& key, int *pi) const
    { return wxConfigBase::Read(key, pi); }
  long Read(const wxString& key, long defVal) const
    { return wxConfigBase::Read(key, defVal); }
  bool Read(const wxString& key, double* val) const
    { return wxConfigBase::Read(key, val); }
  bool Read(const wxString& key, double* val, double defVal) const
    { return wxConfigBase::Read(key, val, defVal); }

  bool Write(const wxString& key, const wxString& szValue);
  bool Write(const wxString& key, long lValue);

  virtual bool Flush(bool /* bCurrentOnly = FALSE */ ) { return TRUE; }

  // delete
  virtual bool DeleteEntry(const wxString& key, bool bGroupIfEmptyAlso);
  virtual bool DeleteGroup(const wxString& key);
  virtual bool DeleteAll();

private:
  // these keys are opened during all lifetime of wxRegConfig object
  wxRegKey  m_keyLocalRoot,  m_keyLocal,
            m_keyGlobalRoot, m_keyGlobal;

  // current path (not '/' terminated)
  wxString  m_strPath;
};

#endif  //_REGCONF_H
