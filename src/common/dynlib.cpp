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

#include  "wx/wxprec.h"

#ifdef    __BORLANDC__
  #pragma hdrstop
#endif  //__BORLANDC__

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include <wx/dynlib.h>
#include <wx/filefn.h>
#include <wx/list.h>
#include <wx/string.h>

// ---------------------------------------------------------------------------
// System dependent include
// ---------------------------------------------------------------------------

#if defined(__UNIX__)
#include <dlfcn.h>
#endif

#ifdef __WINDOWS__
#include <windows.h>
#endif

#ifdef LoadLibrary
#undef LoadLibrary
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
  typedef wxClassInfo *(*t_get_first)(void);
  t_get_first get_first;

  m_handle = handle;
  m_destroy = TRUE;

  // Some system may use a local heap for library.
  get_first = (t_get_first)GetSymbol("wxGetClassFirst");
  // It is a wxWindows DLL.
  if (get_first)
    PrepareClasses(get_first());
}

wxLibrary::~wxLibrary()
{
  if (m_handle && m_destroy) {
#if defined(__UNIX__)
    dlclose(m_handle);
#endif
#ifdef __WINDOWS__
    FreeLibrary((HMODULE)m_handle);
#endif
  }
}

wxObject *wxLibrary::CreateObject(const wxString& name)
{
  wxClassInfo *info = (wxClassInfo *)classTable.Get(name);

  if (!info)
    return NULL;

  return info->CreateObject();
}

void wxLibrary::PrepareClasses(wxClassInfo *first)
{
  // Index all class infos by their class name
  wxClassInfo *info = first;
  while (info)
  {
    if (info->m_className)
      classTable.Put(info->m_className, (wxObject *)info);
    info = info->GetNext();
  }

  // Set base pointers for each wxClassInfo
  info = first;
  while (info)
  {
    if (info->GetBaseClassName1())
      info->m_baseInfo1 = (wxClassInfo *)classTable.Get(info->GetBaseClassName1());
    if (info->GetBaseClassName2())
      info->m_baseInfo2 = (wxClassInfo *)classTable.Get(info->GetBaseClassName2());
    info = info->m_next;
  }
}

void *wxLibrary::GetSymbol(const wxString& symbname)
{
#if defined(__UNIX__)
  return dlsym(m_handle, WXSTRINGCAST symbname);
#elif defined( __WINDOWS__ )
  return GetProcAddress((HINSTANCE) m_handle, WXSTRINGCAST symbname);
#elif defined( __WXMAC__ )
  Ptr symAddress ;
  CFragSymbolClass symClass ;
  Str255	symName ;
	
  strcpy( (char*) symName , symbname ) ;
  c2pstr( (char*) symName ) ;
	
  if ( FindSymbol( (CFragConnectionID) m_handle , symName , &symAddress , &symClass ) == noErr )
  {
    return symAddress ; 
  }
#endif
  return NULL;
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
  wxClassInfo *old_sm_first;

  if ( (node = m_loaded.Find(name.GetData())) )
    return ((wxLibrary *)node->Data());

  // If DLL shares data, this is necessary.
  old_sm_first = wxClassInfo::sm_first;
  wxClassInfo::sm_first = NULL;

#if defined(__UNIX__)
  lib_name.Prepend("./lib");
  lib_name += ".so";

  printf("lib_name = %s\n", WXSTRINGCAST lib_name);

  void *handle = dlopen(WXSTRINGCAST lib_name, RTLD_LAZY);

  printf("error = %s\n", dlerror());

  if (!handle)
    return NULL;
#elif defined(__WINDOWS__)
  lib_name += ".dll";

#ifdef UNICODE
  HMODULE handle = LoadLibraryW(lib_name);
#else
#ifdef __WIN16__
  HMODULE handle = ::LoadLibrary(lib_name);
#else
  HMODULE handle = LoadLibraryA(lib_name);
#endif
#endif
  if (!handle)
    return NULL;
#elif defined(__WXMAC__)
  FSSpec myFSSpec ;
  CFragConnectionID handle ;
  Ptr	myMainAddr ;
  Str255	myErrName ;
	
  wxMacPathToFSSpec( lib_name , &myFSSpec ) ;
  if (GetDiskFragment( &myFSSpec , 0 , kCFragGoesToEOF , "\p" , kPrivateCFragCopy , &handle , &myMainAddr ,
    myErrName ) != noErr )
  {
    p2cstr( myErrName ) ;
    wxASSERT_MSG( 1 , (char*)myErrName ) ;
	return NULL ;
  }
#else
  return NULL;
#endif

  lib = new wxLibrary((void *)handle);

  wxClassInfo::sm_first = old_sm_first;

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
