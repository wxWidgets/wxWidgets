///////////////////////////////////////////////////////////////////////////////
// Name:        msw/regconf.h  
// Purpose:     Registry based implementation of wxConfigBase
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     27.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
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
    // will store data in HKLM\strRegHive and HKCU\strRegHive
  wxRegConfig(const wxString& strRegHive);
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
  virtual uint GetNumberOfEntries(bool bRecursive = FALSE) const;
  virtual uint GetNumberOfGroups(bool bRecursive = FALSE) const;

  // read/write
  virtual bool Read(wxString *pStr, const char *szKey,
                    const char *szDefault = 0) const;
  virtual bool Read(long *result, const char *szKey, long lDefault = 0) const;
  virtual bool Write(const char *szKey, const char *szValue);
  virtual bool Write(const char *szKey, long Value);
  virtual bool Flush(bool /* bCurrentOnly = FALSE */ ) { return TRUE; }

  // delete
  virtual bool DeleteEntry(const char *szKey, bool bGroupIfEmptyAlso);
  virtual bool DeleteGroup(const char *szKey);
  virtual bool DeleteAll();

private:
  // these keys are opened during all lifetime of wxRegConfig object
  wxRegKey  m_keyLocalRoot,  m_keyLocal,
            m_keyGlobalRoot, m_keyGlobal;

  // current path (not '/' terminated)
  wxString  m_strPath;
};

#endif  //_REGCONF_H
