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

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
#   pragma implementation "dynlib.h"
#endif

#include  "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_DYNLIB_CLASS

#include "wx/dynlib.h"
#include "wx/filefn.h"
#include "wx/intl.h"
#include "wx/log.h"
#include "wx/tokenzr.h"

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

#if defined(HAVE_DLOPEN)
#   define wxDllOpen(lib)                dlopen(lib.fn_str(), RTLD_NOW/*RTLD_LAZY*/)
#   define wxDllGetSymbol(handle, name)  dlsym(handle, name.mb_str())
#   define wxDllClose                    dlclose
#elif defined(HAVE_SHL_LOAD)
#   define wxDllOpen(lib)                shl_load(lib.fn_str(), BIND_DEFERRED, 0)
#   define wxDllClose      shl_unload

    static inline void *wxDllGetSymbol(shl_t handle, const wxString& name)
    {
        void *sym;
        if ( shl_findsym(&handle, name.mb_str(), TYPE_UNDEFINED, &sym) == 0 )
            return sym;
        else
            return (void *)0;
    }
#elif defined(__WINDOWS__)
#   include <windows.h>

    // using LoadLibraryEx under Win32 to avoid name clash with LoadLibrary
#   ifdef __WIN32__
#      define wxDllOpen(lib)                  ::LoadLibraryEx(lib, 0, 0)
#   else   // Win16
#      define wxDllOpen(lib)                  ::LoadLibrary(lib)
#   endif  // Win32/16
#   define wxDllGetSymbol(handle, name)    ::GetProcAddress(handle, name)
#   define wxDllClose                      ::FreeLibrary
#else
#   error "Don't know how to load shared libraries on this platform."
#endif // OS

// ---------------------------------------------------------------------------
// Global variables
// ---------------------------------------------------------------------------

wxLibraries wxTheLibraries;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// construct the full name from the base shared object name: adds a .dll
// suffix under Windows or .so under Unix
static wxString ConstructLibraryName(const wxString& basename)
{
    wxString fullname(basename);

#if defined(__UNIX__)
#   if defined(__HPUX__)
        fullname << ".sl";
#   else	//__HPUX__
        fullname << ".so";
#   endif	//__HPUX__
#elif defined(__WINDOWS__)
    fullname << ".dll";
#endif

    return fullname;
}

// ============================================================================
// implementation
// ============================================================================


// ---------------------------------------------------------------------------
// wxLibrary (one instance per dynamic library)
// ---------------------------------------------------------------------------

wxLibrary::wxLibrary(wxDllType handle)
{
    typedef wxClassInfo *(*t_get_first)(void);
    t_get_first get_first;

    m_handle = handle;

    // Some system may use a local heap for library.
    get_first = (t_get_first)GetSymbol("wxGetClassFirst");
    // It is a wxWindows DLL.
    if (get_first)
        PrepareClasses(get_first());
}

wxLibrary::~wxLibrary()
{
    if ( m_handle )
    {
        wxDllClose(m_handle);
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
   return wxDllLoader::GetSymbol(m_handle, symbname);
}

// ---------------------------------------------------------------------------
// wxDllLoader
// ---------------------------------------------------------------------------

/* static */
wxDllType
wxDllLoader::GetProgramHandle(void)
{
#ifdef __UNIX__
    return dlopen(NULL, RTLD_NOW/*RTLD_LAZY*/);
#else
    wxFAIL_MSG(_("This method is not implemented under Windows"));

    return 0;
#endif   
}


/* static */
wxDllType
wxDllLoader::LoadLibrary(const wxString & lib_name, bool *success)
{
   wxASSERT(success);
   
   wxDllType handle;

#if defined(__WXMAC__)
   FSSpec myFSSpec ;
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
#else // !Mac
   handle = wxDllOpen(lib_name);
#endif // OS

   if ( !handle )
   {
      wxLogSysError(_("Failed to load shared library '%s'"),
                    lib_name.c_str());
      *success = FALSE;
      return NULL;
   }
   *success = TRUE;
   return handle;
}


/* static */
void
wxDllLoader::UnloadLibrary(wxDllType handle)
{
   wxDllClose(handle);
}

/* static */
void *
wxDllLoader::GetSymbol(wxDllType dllHandle, const wxString &name)
{
   void *symbol = NULL;    // return value

#if defined( __WXMAC__ )
   Ptr symAddress ;
   CFragSymbolClass symClass ;
   Str255	symName ;
   
   strcpy( (char*) symName , name ) ;
   c2pstr( (char*) symName ) ;
   
   if ( FindSymbol( dllHandle , symName , &symAddress , &symClass ) == noErr )
      symbol = (void *)symAddress ; 
#else
    symbol = wxDllGetSymbol(dllHandle, name);
#endif

    if ( !symbol )
    {
       wxLogSysError(_("Couldn't find symbol '%s' in a dynamic library"),
                     name.c_str());
    }
    return symbol;
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
    wxNode *node;
    wxLibrary *lib;
    wxClassInfo *old_sm_first;

    if ( (node = m_loaded.Find(name.GetData())) )
        return ((wxLibrary *)node->Data());

    // If DLL shares data, this is necessary.
    old_sm_first = wxClassInfo::sm_first;
    wxClassInfo::sm_first = NULL;

    wxString lib_name = ConstructLibraryName(name);

/*
  Unix automatically builds that library name, at least for dlopen()
*/
#if 0
#if defined(__UNIX__)
    // found the first file in LD_LIBRARY_PATH with this name
    wxString libPath("/lib:/usr/lib"); // system path first
    const char *envLibPath = getenv("LD_LIBRARY_PATH");
    if ( envLibPath )
        libPath << ':' << envLibPath;
    wxStringTokenizer tokenizer(libPath, _T(':'));
    while ( tokenizer.HasMoreToken() )
    {
        wxString fullname(tokenizer.NextToken());

        fullname << '/' << lib_name;
        if ( wxFileExists(fullname) )
        {
            lib_name = fullname;

            // found the library
            break;
        }
    }
    //else: not found in the path, leave the name as is (secutiry risk?)

#endif // __UNIX__
#endif

    bool success = FALSE;
    wxDllType handle = wxDllLoader::LoadLibrary(lib_name, &success);
    if(success)
    {
       lib = new wxLibrary(handle);
       wxClassInfo::sm_first = old_sm_first;
       m_loaded.Append(name.GetData(), lib);
    }
    else
       lib = NULL;
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

#endif // wxUSE_DYNLIB_CLASS
