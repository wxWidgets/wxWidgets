#ifndef _WX_DYNLIB_H__
#define _WX_DYNLIB_H__

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/string.h>
#include <wx/list.h>
#include <wx/dynarray.h>
#include <wx/hash.h>

#ifdef LoadLibrary
#undef LoadLibrary
#endif

// ---------------------------------------------------------------------------
// wxLibrary

class wxLibrary: public wxObject {
 protected:
  void *m_handle;
  bool m_destroy;
 public:
  wxHashTable classTable;

 public:
  wxLibrary(void *handle);
  ~wxLibrary(void);

  // Get a symbol from the dynamic library
  void *GetSymbol(const wxString& symbname);

  // Create the object whose classname is "name"
  wxObject *CreateObject(const wxString& name);

  // Merge the symbols with the main symbols: WARNING! the library will not
  // be unloaded.
  void MergeWithSystem();

 protected:
  void PrepareClasses(wxClassInfo **first);
};

// ---------------------------------------------------------------------------
// wxLibraries

class wxLibraries {
 protected:
  wxList m_loaded;
 public:
  wxLibraries(void);
  ~wxLibraries(void);

  wxLibrary *LoadLibrary(const wxString& name);
  wxObject *CreateObject(const wxString& name);
};

// ---------------------------------------------------------------------------
// Global variables

extern wxLibraries wxTheLibraries;

// ---------------------------------------------------------------------------
// Interesting defines

#define WXDLL_ENTRY_FUNCTION() \
extern "C" wxClassInfo **wxGetClassFirst(); \
wxClassInfo **wxGetClassFirst() { \
  return &wxClassInfo::first; \
}

#endif
