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

#if defined(__WINDOWS__)
    #include "wx/msw/private.h"
#endif

#include "wx/dynlib.h"
#include "wx/filefn.h"
#include "wx/intl.h"
#include "wx/log.h"
#include "wx/tokenzr.h"

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

#if defined(__WXPM__) || defined(__EMX__)
#  define INCL_DOS
#  include <os2.h>
#  define wxDllOpen(error, lib, handle)     DosLoadModule(error, sizeof(error), lib, &handle)
#  define wxDllGetSymbol(handle, modaddr)   DosQueryProcAddr(handle, 1L, NULL, (PFN*)modaddr)
#  define wxDllClose(handle)                DosFreeModule(handle)
#elif defined(HAVE_DLOPEN)
    // note about dlopen() flags: we use RTLD_NOW to have more Windows-like
    // behaviour (Win won't let you load a library with missing symbols) and
    // RTLD_GLOBAL because it is needed sometimes and probably doesn't hurt
    // otherwise. On True64-Unix RTLD_GLOBAL is not allowed and on VMS the
    // second argument on dlopen is ignored.
#ifdef __VMS
# define wxDllOpen(lib)                dlopen(lib.fn_str(), 0 )
#elif defined( __osf__ )
# define wxDllOpen(lib)                dlopen(lib.fn_str(), RTLD_LAZY )
#else
# define wxDllOpen(lib)                dlopen(lib.fn_str(), RTLD_LAZY | RTLD_GLOBAL)
#endif
#define wxDllGetSymbol(handle, name)  dlsym(handle, name)
#   define wxDllClose                    dlclose
#elif defined(HAVE_SHL_LOAD)
#   define wxDllOpen(lib)                shl_load(lib.fn_str(), BIND_DEFERRED, 0)
#   define wxDllClose                    shl_unload

    static inline void *wxDllGetSymbol(shl_t handle, const wxString& name)
    {
        void *sym;
        if ( shl_findsym(&handle, name.mb_str(), TYPE_UNDEFINED, &sym) == 0 )
            return sym;
        else
            return (void *)0;
    }
#elif defined(__APPLE__) && defined(__UNIX__)
char *dlopen(char *path, int mode /* mode is ignored */);
void *dlsym(void *handle, char *symbol);
int   dlclose(void *handle);
char *dlerror();

#   define wxDllOpen(lib)                dlopen(lib.fn_str(), 0)
#   define wxDllGetSymbol(handle, name)  dlsym(handle, name)
#   define wxDllClose                    dlclose
#elif defined(__WINDOWS__)
    // using LoadLibraryEx under Win32 to avoid name clash with LoadLibrary
#   ifdef __WIN32__
#ifdef _UNICODE
#      define wxDllOpen(lib)                  ::LoadLibraryExW(lib, 0, 0)
#else
#      define wxDllOpen(lib)                  ::LoadLibraryExA(lib, 0, 0)
#endif
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

// ============================================================================
// implementation
// ============================================================================

// construct the full name from the base shared object name: adds a .dll
// suffix under Windows or .so under Unix
static wxString ConstructLibraryName(const wxString& basename)
{
    wxString fullname;
    fullname << basename << wxDllLoader::GetDllExt();

    return fullname;
}

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
wxString wxDllLoader::GetDllExt()
{
    wxString ext;

#if defined(__WINDOWS__) || defined(__WXPM__) || defined(__EMX__)
    ext = _T(".dll");
#elif defined(__UNIX__)
#   if defined(__HPUX__)
        ext = _T(".sl");
#   else //__HPUX__
        ext = _T(".so");
#   endif //__HPUX__
#endif

    return ext;
}

/* static */
wxDllType
wxDllLoader::GetProgramHandle(void)
{
#if defined( HAVE_DLOPEN ) && !defined(__EMX__)
   // optain handle for main program
   return dlopen(NULL, RTLD_NOW/*RTLD_LAZY*/); 
#elif defined (HAVE_SHL_LOAD)
   // shl_findsymbol with NULL handle looks up in main program
   return 0; 
#else
   wxFAIL_MSG( wxT("This method is not implemented under Windows or OS/2"));
   return 0;
#endif
}

/* static */
wxDllType
wxDllLoader::LoadLibrary(const wxString & libname, bool *success)
{
    wxDllType handle;

#if defined(__WXMAC__) && !defined(__UNIX__)
    FSSpec myFSSpec ;
    Ptr myMainAddr ;
    Str255 myErrName ;

    wxMacPathToFSSpec( libname , &myFSSpec ) ;
    if (GetDiskFragment( &myFSSpec , 0 , kCFragGoesToEOF , "\p" , kPrivateCFragCopy , &handle , &myMainAddr ,
                myErrName ) != noErr )
    {
        p2cstr( myErrName ) ;
        wxASSERT_MSG( 1 , (char*)myErrName ) ;
        return NULL ;
    }
#elif defined(__WXPM__) || defined(__EMX__)
    char zError[256] = "";
    wxDllOpen(zError, libname, handle);
#else // !Mac
    handle = wxDllOpen(libname);
#endif // OS

    if ( !handle )
    {
        wxString msg(_("Failed to load shared library '%s'"));

#ifdef HAVE_DLERROR
        const char *errmsg = dlerror();
        if ( errmsg )
        {
            // the error string format is "libname: ...", but we already have
            // libname, so cut it off
            const char *p = strchr(errmsg, ':');
            if ( p )
            {
                if ( *++p == ' ' )
                    p++;
            }
            else
            {
                p = errmsg;
            }

            msg += _T(" (%s)");
            wxLogError(msg, libname.c_str(), p);
        }
        else
#endif // HAVE_DLERROR
        {
            wxLogSysError(msg, libname.c_str());
        }
    }

    if ( success )
    {
        *success = handle != 0;
    }

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

#if defined(__WXMAC__) && !defined(__UNIX__)
    Ptr symAddress ;
    CFragSymbolClass symClass ;
    Str255 symName ;

#if TARGET_CARBON
    c2pstrcpy( (StringPtr) symName , name ) ;
#else
    strcpy( (char *) symName , name ) ;
	c2pstr( (char *) symName ) ;
#endif

    if ( FindSymbol( dllHandle , symName , &symAddress , &symClass ) == noErr )
        symbol = (void *)symAddress ;
#elif defined( __WXPM__ ) || defined(__EMX__)
    wxDllGetSymbol(dllHandle, symbol);
#else
    // mb_str() is necessary in Unicode build
    symbol = wxDllGetSymbol(dllHandle, name.mb_str());
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

wxLibraries::wxLibraries():m_loaded(wxKEY_STRING)
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

#if defined(__VISAGECPP__)
    node = m_loaded.Find(name.GetData());
    if (node != NULL)
        return ((wxLibrary *)node->Data());
#else // !OS/2
    if ( (node = m_loaded.Find(name.GetData())) )
        return ((wxLibrary *)node->Data());
#endif
    // If DLL shares data, this is necessary.
    old_sm_first = wxClassInfo::sm_first;
    wxClassInfo::sm_first = NULL;

    wxString libname = ConstructLibraryName(name);

/*
  Unix automatically builds that library name, at least for dlopen()
*/
#if 0
#if defined(__UNIX__)
    // found the first file in LD_LIBRARY_PATH with this name
    wxString libPath("/lib:/usr/lib"); // system path first
    const char *envLibPath = getenv("LD_LIBRARY_PATH");
    if ( envLibPath )
        libPath << wxT(':') << envLibPath;
    wxStringTokenizer tokenizer(libPath, wxT(':'));
    while ( tokenizer.HasMoreToken() )
    {
        wxString fullname(tokenizer.NextToken());

        fullname << wxT('/') << libname;
        if ( wxFileExists(fullname) )
        {
            libname = fullname;

            // found the library
            break;
        }
    }
    //else: not found in the path, leave the name as is (secutiry risk?)

#endif // __UNIX__
#endif

    bool success = FALSE;
    wxDllType handle = wxDllLoader::LoadLibrary(libname, &success);
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
