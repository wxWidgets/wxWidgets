///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/regconf.h
// Purpose:     Registry based implementation of wxConfigBase
// Author:      Vadim Zeitlin
// Modified by:
// Created:     27.04.98
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_REGCONF_H_
#define _WX_MSW_REGCONF_H_

#include "wx/defs.h"

#if wxUSE_CONFIG && wxUSE_REGKEY

#include "wx/msw/registry.h"
#include "wx/object.h"
#include "wx/confbase.h"
#include "wx/buffer.h"

// ----------------------------------------------------------------------------
// wxRegConfig
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxRegConfig : public wxConfigBase
{
public:
  // ctor & dtor
    // will store data in HKLM\appName and HKCU\appName
  wxRegConfig(const wxString& appName = wxEmptyString,
              const wxString& vendorName = wxEmptyString,
              const wxString& localFilename = wxEmptyString,
              const wxString& globalFilename = wxEmptyString,
              long style = wxCONFIG_USE_GLOBAL_FILE);

    // dtor will save unsaved data
  virtual ~wxRegConfig(){}

  // implement inherited pure virtual functions
  // ------------------------------------------

  // path management
  virtual void SetPath(const wxString& strPath) wxOVERRIDE;
  virtual const wxString& GetPath() const wxOVERRIDE { return m_strPath; }

  // entry/subgroup info
    // enumerate all of them
  virtual bool GetFirstGroup(wxString& str, long& lIndex) const wxOVERRIDE;
  virtual bool GetNextGroup (wxString& str, long& lIndex) const wxOVERRIDE;
  virtual bool GetFirstEntry(wxString& str, long& lIndex) const wxOVERRIDE;
  virtual bool GetNextEntry (wxString& str, long& lIndex) const wxOVERRIDE;

    // tests for existence
  virtual bool HasGroup(const wxString& strName) const wxOVERRIDE;
  virtual bool HasEntry(const wxString& strName) const wxOVERRIDE;
  virtual EntryType GetEntryType(const wxString& name) const wxOVERRIDE;

    // get number of entries/subgroups in the current group, with or without
    // it's subgroups
  virtual size_t GetNumberOfEntries(bool bRecursive = false) const wxOVERRIDE;
  virtual size_t GetNumberOfGroups(bool bRecursive = false) const wxOVERRIDE;

  virtual bool Flush(bool WXUNUSED(bCurrentOnly) = false) wxOVERRIDE { return true; }

  // rename
  virtual bool RenameEntry(const wxString& oldName, const wxString& newName) wxOVERRIDE;
  virtual bool RenameGroup(const wxString& oldName, const wxString& newName) wxOVERRIDE;

  // delete
  virtual bool DeleteEntry(const wxString& key, bool bGroupIfEmptyAlso = true) wxOVERRIDE;
  virtual bool DeleteGroup(const wxString& key) wxOVERRIDE;
  virtual bool DeleteAll() wxOVERRIDE;

protected:
  // opens the local key creating it if necessary and returns it
  wxRegKey& LocalKey() const // must be const to be callable from const funcs
  {
      wxRegConfig* self = wxConstCast(this, wxRegConfig);

      if ( !m_keyLocal.IsOpened() )
      {
          // create on demand
          self->m_keyLocal.Create();
      }

      return self->m_keyLocal;
  }

  // implement read/write methods
  virtual bool DoReadString(const wxString& key, wxString *pStr) const wxOVERRIDE;
  virtual bool DoReadLong(const wxString& key, long *plResult) const wxOVERRIDE;
#if wxUSE_BASE64
  virtual bool DoReadBinary(const wxString& key, wxMemoryBuffer* buf) const wxOVERRIDE;
#endif // wxUSE_BASE64

  virtual bool DoWriteString(const wxString& key, const wxString& szValue) wxOVERRIDE;
  virtual bool DoWriteLong(const wxString& key, long lValue) wxOVERRIDE;
#if wxUSE_BASE64
  virtual bool DoWriteBinary(const wxString& key, const wxMemoryBuffer& buf) wxOVERRIDE;
#endif // wxUSE_BASE64

private:
  // these keys are opened during all lifetime of wxRegConfig object
  wxRegKey  m_keyLocalRoot,  m_keyLocal,
            m_keyGlobalRoot, m_keyGlobal;

  // current path (not '/' terminated)
  wxString  m_strPath;

  wxDECLARE_NO_COPY_CLASS(wxRegConfig);
  wxDECLARE_ABSTRACT_CLASS(wxRegConfig);
};

#endif // wxUSE_CONFIG && wxUSE_REGKEY

#endif // _WX_MSW_REGCONF_H_
