/////////////////////////////////////////////////////////////////////////////
// Name:        dynlib.cpp
// Purpose:     Dynamic library management
// Author:      Guilhem Lavaux
// Modified by:
// Created:     20/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dynlib.h"
#endif

#include <wx/dynlib.h>
#include <wx/filefn.h>
#include <wx/list.h>
#include <wx/string.h>

// ---------------------------------------------------------------------------
// System dependent include
// ---------------------------------------------------------------------------

#ifdef linux
#include <dlfcn.h>
#endif

// ---------------------------------------------------------------------------
// Global variables
// ---------------------------------------------------------------------------

wxLibraries wxTheLibraries;

// ---------------------------------------------------------------------------
// wxLibrary (one instance per dynamic library
// ---------------------------------------------------------------------------

wxLibrary::wxLibrary(void *handle)
{
  typedef wxClassLibrary *(*t_get_list)(void);
  t_get_list get_list;

  m_handle = handle;

  get_list = (t_get_list)GetSymbol("GetClassList");
  m_liblist = (*get_list)();
}

wxLibrary::~wxLibrary()
{
  if (m_handle) {
    typedef void (*t_free_list)(wxClassLibrary *);
    t_free_list free_list;

    free_list = (t_free_list) GetSymbol("FreeClassList");
    if (free_list != NULL)
      free_list(m_liblist);
    else
      delete m_liblist;

    dlclose(m_handle);
  }
}

wxObject *wxLibrary::CreateObject(const wxString& name)
{
  return m_liblist->CreateObject(name);
}

void *wxLibrary::GetSymbol(const wxString& symbname)
{
#ifdef linux
  return dlsym(m_handle, symbname.GetData());
#endif
}

// ---------------------------------------------------------------------------
// wxLibraries (only one instance should normally exist)
// ---------------------------------------------------------------------------

wxLibraries::wxLibraries()
{
}

wxLibraries::~wxLibraries()
{
  wxNode *node = m_loaded.First();

  while (node) {
    wxLibrary *lib = (wxLibrary *)node->Data();
    delete lib;

    node = node->Next();
  }
}

wxLibrary *wxLibraries::LoadLibrary(const wxString& name)
{
  wxString lib_name = name;
  wxNode *node;
  wxLibrary *lib;

  if ( (node = m_loaded.Find(name.GetData())) )
    return ((wxLibrary *)node->Data());

#ifdef linux
  lib_name.Prepend("./lib");
  lib_name += ".so";

  printf("lib_name = %s\n", WXSTRINGCAST lib_name);

  void *handle = dlopen(lib_name.GetData(), RTLD_LAZY);

  printf("handle = %x\n", handle);
  lib = new wxLibrary(handle);

#endif
#ifdef __WINDOWS__
  lib_name += ".dll";

#endif

  m_loaded.Append(name.GetData(), lib);
  return lib;
}

wxObject *wxLibraries::CreateObject(const wxString& path)
{
  wxNode *node = m_loaded.First();
  wxObject *obj;

  while (node) {
    obj = ((wxLibrary *)node->Data())->CreateObject(path);
    if (obj) 
      return obj;

    node = node->Next();
  }
  return NULL;
}

// ---------------------------------------------------------------------------
// wxClassLibrary (this class is used to access the internal class)
// ---------------------------------------------------------------------------

wxClassLibrary::wxClassLibrary(void)
{
}

wxClassLibrary::~wxClassLibrary(void)
{
  uint i;

  for (i=0;i<m_list.Count();i++)
    delete (m_list[i]);
}

void wxClassLibrary::RegisterClass(wxClassInfo *class_info,
                                   const wxString& path)
{
  wxClassLibInfo *info = new wxClassLibInfo;

  info->class_info = class_info;
  info->path = path;
  m_list.Add(info);
}

void wxClassLibrary::UnregisterClass(wxClassInfo *class_info)
{
  uint i = 0;

  while (i < m_list.Count()) {
    if (m_list[i]->class_info == class_info) {
      delete (m_list[i]);
      m_list.Remove(i);
      return;
    }
    i++;
  }
}

bool wxClassLibrary::CreateObjects(const wxString& path,
                                   wxArrayClassInfo& objs)
{
  wxClassLibInfo *info;
  uint i = 0;

  while (i < m_list.Count()) {
    info = m_list[i];
    if (wxMatchWild(path, info->path))
      objs.Add(info->class_info);
    i++;
  }
  return (i > 0);
}

bool wxClassLibrary::FetchInfos(const wxString& path,
                                wxArrayClassLibInfo& infos)
{
  wxClassLibInfo *info;
  uint i = 0;

  while (i < m_list.Count()) {
    info = m_list[i];
    if (wxMatchWild(path, info->path)) {
      wxClassLibInfo *inf = new wxClassLibInfo;
      *inf = *info;
      infos.Add(inf);
    }
    i++;
  }
  return (i > 0);
}

wxObject *wxClassLibrary::CreateObject(const wxString& path)
{
  wxClassLibInfo *info;
  uint i = 0;

  while (i < m_list.Count()) {
    info = m_list[i];
    if (wxMatchWild(path, info->path))
      return info->class_info->CreateObject();
    i++;
  }
  return NULL;
}
