#ifndef _WX_DYNLIB_H__
#define _WX_DYNLIB_H__

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/string.h>
#include <wx/list.h>
#include <wx/dynarray.h>

// ---------------------------------------------------------------------------
// Some more info on a class

typedef struct {
  wxClassInfo *class_info;
  wxString path;
} wxClassLibInfo;

// ---------------------------------------------------------------------------
// Useful arrays

WX_DEFINE_ARRAY(wxClassInfo *, wxArrayClassInfo);
WX_DEFINE_ARRAY(wxClassLibInfo *, wxArrayClassLibInfo);

// ---------------------------------------------------------------------------
// wxClassLibrary

class wxClassLibrary {
protected:
  wxArrayClassLibInfo m_list;
public:
  wxClassLibrary(void);
  ~wxClassLibrary(void);

  // Dynamic (un)register a (new) class in the database
  void RegisterClass(wxClassInfo *class_info, const wxString& path);
  void UnregisterClass(wxClassInfo *class_info);

  // Fetch all infos whose name matches the string (wildcards allowed)
  bool FetchInfos(const wxString& path, wxArrayClassLibInfo& infos);

  // Create all objects whose name matches the string (wildcards allowed)
  bool CreateObjects(const wxString& path, wxArrayClassInfo& objs);

  // Create one object using the EXACT name
  wxObject *CreateObject(const wxString& path);
};

// ---------------------------------------------------------------------------
// wxLibrary

class wxLibrary: public wxObject {
protected:
  wxClassLibrary *m_liblist;
  void *m_handle;
public:
  wxLibrary(void *handle);
  ~wxLibrary(void);

  // Get a symbol from the dynamic library
  void *GetSymbol(const wxString& symbname);

  // Create the object whose classname is "name"
  wxObject *CreateObject(const wxString& name);

  wxClassLibrary *ClassLib() const;
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

#define WXDLL_ENTRY_FUNCTION() extern "C" wxClassLibrary *GetClassList()
#define WXDLL_EXIT_FUNCTION(param) extern "C" void FreeClassList(wxClassLibrary *param)

#endif
