/////////////////////////////////////////////////////////////////////////////
// Name:        defs.h
// Purpose:     Declarations/definitions common to all wx source files
// Author:      Julian Smart and others
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DEFS_H_
#define _WX_DEFS_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "defs.h"
#endif

// ----------------------------------------------------------------------------
// compiler and OS identification
// ----------------------------------------------------------------------------

#include "wx/platform.h"

// Make sure the environment is set correctly
#if defined(__WXMSW__) && defined(__X__)
#error "Target can't be both X and Windows"
#elif !defined(__WXMOTIF__) && !defined(__WXMSW__) && !defined(__WXGTK__) && \
      !defined(__WXPM__) && !defined(__WXMAC__) && !defined(__WXCOCOA__) && \
      !defined(__X__) && !defined(__WXMGL__) && !defined(__WXX11__) && \
      wxUSE_GUI
#ifdef __UNIX__
#error "No Target! You should use wx-config program for compilation flags!"
#else // !Unix
#error "No Target! You should use supplied makefiles for compilation!"
#endif // Unix/!Unix
#endif

// include the feature test macros
#include "wx/features.h"

// suppress some Visual C++ warnings
#ifdef __VISUALC__
#   pragma warning(disable:4201)    // nonstandard extension used: nameless struct/union
#   pragma warning(disable:4244)    // conversion from double to float
#   pragma warning(disable:4100)    // unreferenced formal parameter
#   pragma warning(disable:4511)    // copy ctor couldn't be generated
#   pragma warning(disable:4512)    // operator=() couldn't be generated
#   pragma warning(disable:4699)    // using precompiled header
#   pragma warning(disable:4134)    // conversion between pointers to members of same class
#   pragma warning(disable:4710)    // function not inlined
#ifndef WIN32
#   pragma warning(disable:4135)    // conversion between different integral types
#   pragma warning(disable:4769)    // assignment of near pointer to long integer
// This one is really annoying, since it occurs for each cast to (HANDLE)...
#   pragma warning(disable:4305)    // truncation of long to near ptr
#endif
#endif // __VISUALC__

// suppress some Watcom C++ warnings
#ifdef __WATCOMC__
#   pragma warning 849 9            // Disable 'virtual function hidden'
#   pragma warning 549 9            // Disable 'operand contains compiler generated information'
#endif // __VISUALC__

// suppress some Salford C++ warnings
#ifdef __SALFORDC__
#   pragma suppress 353             // Possible nested comments
#   pragma suppress 593             // Define not used
#   pragma suppress 61              // enum has no name (doesn't suppress!)
#   pragma suppress 106             // unnamed, unused parameter
#   pragma suppress 571             // Virtual function hiding
#endif // __SALFORDC__

// ----------------------------------------------------------------------------
// wxWindows version and compatibility defines
// ----------------------------------------------------------------------------

#include "wx/version.h"

// possibility to build non GUI apps is new, so don't burden ourselves with
// compatibility code
#if !wxUSE_GUI
#undef WXWIN_COMPATIBILITY_2
#undef WXWIN_COMPATIBILITY_2_2

#define WXWIN_COMPATIBILITY_2 0
#define WXWIN_COMPATIBILITY_2_2 0
#endif // !GUI

// ============================================================================
// non portable C++ features
// ============================================================================

// ----------------------------------------------------------------------------
// compiler defects workarounds
// ----------------------------------------------------------------------------

#if defined(__VISUALC__) && !defined(WIN32) && !defined(__WXWINCE__)
    // VC1.5 does not have LPTSTR type
#define LPTSTR  LPSTR
#define LPCTSTR LPCSTR
#elif defined(__BORLANDC__) && !defined(__WIN32__)
#ifndef LPTSTR
#define LPTSTR  LPSTR
#endif
#ifndef LPCTSTR
#define LPCTSTR LPSTR
#endif
#endif


/*
   Digital Unix C++ compiler only defines this symbol for .cxx and .hxx files,
   so define it ourselves (newer versions do it for all files, though, and
   don't allow it to be redefined)
 */
#if defined(__DECCXX) && !defined(__VMS) && !defined(__cplusplus)
#define __cplusplus
#endif /* __DECCXX */

// Resolves linking problems under HP-UX when compiling with gcc/g++
#if defined(__HPUX__) && defined(__GNUG__)
#define va_list __gnuc_va_list
#endif // HP-UX

// ----------------------------------------------------------------------------
// check for native bool type and TRUE/FALSE constants
// ----------------------------------------------------------------------------

// Add more tests here for Windows compilers that already define bool
// (under Unix, configure tests for this)
#ifndef HAVE_BOOL
    #if defined( __MWERKS__ )
        #if (__MWERKS__ >= 0x1000) && __option(bool)
            #define HAVE_BOOL
        #endif
    #elif defined(__APPLE__) && defined(__APPLE_CC__)
        // Apple bundled gcc supports bool
        #define HAVE_BOOL
    #elif defined(__VISUALC__) && (__VISUALC__ == 1020)
        // in VC++ 4.2 the bool keyword is reserved (hence can't be typedefed)
        // but not implemented, so we must #define it
        #define bool unsigned int
    #elif defined(__VISUALC__) && (__VISUALC__ == 1010)
        // For VisualC++ 4.1, we need to define
        // bool as something between 4.0 & 5.0...
        typedef unsigned int wxbool;
        #define bool wxbool
        #define HAVE_BOOL
    #elif defined(__VISUALC__) && (__VISUALC__ > 1020)
        // VC++ supports bool since 4.2
        #define HAVE_BOOL
    #elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x500)
        // Borland 5.0+ supports bool
        #define HAVE_BOOL
    #elif defined(__WATCOMC__) && (__WATCOMC__ >= 1100)
        // Watcom 11+ supports bool
        #define HAVE_BOOL
    #elif defined(__GNUWIN32__) || defined(__MINGW32__) || defined(__CYGWIN__)
        // Cygwin supports bool
        #define HAVE_BOOL
    #elif defined(__VISAGECPP__)
        #if __IBMCPP__ < 400
            typedef unsigned long bool;
        #endif
        #define HAVE_BOOL
    #endif // compilers
#endif // HAVE_BOOL

#if !defined(HAVE_BOOL) && !defined(bool) && !defined(VMS)
    // NB: of course, this doesn't replace the standard type, because, for
    //     example, overloading based on bool/int parameter doesn't work and
    //     so should be avoided in portable programs
    typedef unsigned int bool;
#endif // bool

// deal with TRUE/true stuff: we assume that if the compiler supports bool, it
// supports true/false as well and that, OTOH, if it does _not_ support bool,
// it doesn't support these keywords (this is less sure, in particular VC++
// 4.x could be a problem here)
#ifndef HAVE_BOOL
    #define true ((bool)1)
    #define false ((bool)0)
#endif

// for backwards compatibility, also define TRUE and FALSE
//
// note that these definitions should work both in C++ and C code, so don't
// use true/false below
#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

typedef short int WXTYPE;

// special care should be taken with this type under Windows where the real
// window id is unsigned, so we must always do the cast before comparing them
// (or else they would be always different!). Usign wxGetWindowId() which does
// the cast itself is recommended. Note that this type can't be unsigned
// because wxID_ANY == -1 is a valid (and largely used) value for window id.
typedef int wxWindowID;

// ----------------------------------------------------------------------------
// other feature tests
// ----------------------------------------------------------------------------

// Every ride down a slippery slope begins with a single step..
//
// Yes, using nested classes is indeed against our coding standards in
// general, but there are places where you can use them to advantage
// without totally breaking ports that cannot use them.  If you do, then
// wrap it in this guard, but such cases should still be relatively rare.
#ifndef __WIN16__
    #define wxUSE_NESTED_CLASSES    1
#else
    #define wxUSE_NESTED_CLASSES    0
#endif

// check for explicit keyword support
#ifndef HAVE_EXPLICIT
    #if defined(__VISUALC__) && (__VISUALC__ >= 1100)
        // VC++ 6.0 and 5.0 have explicit (what about the earlier versions?)
        #define HAVE_EXPLICIT
    #elif ( defined(__MINGW32__) || defined(__CYGWIN32__) ) \
          && wxCHECK_GCC_VERSION(2, 95)
        // GCC 2.95 has explicit, what about earlier versions?
        #define HAVE_EXPLICIT
    #elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x0520)
        // BC++ 4.52 doesn't support explicit, CBuilder 1 does
        #define HAVE_EXPLICIT
    #elif defined(__MWERKS__) && (__MWERKS__ >= 0x2400)
        // Metrowerks CW6 or higher has explicit
        #define HAVE_EXPLICIT
    #endif
#endif // !HAVE_EXPLICIT

#ifdef HAVE_EXPLICIT
    #define wxEXPLICIT explicit
#else // !HAVE_EXPLICIT
    #define wxEXPLICIT
#endif // HAVE_EXPLICIT/!HAVE_EXPLICIT

// check for static/const/reinterpret_cast<>()
#ifndef HAVE_STATIC_CAST
    #if defined(__VISUALC__) && (__VISUALC__ >= 1100)
        // VC++ 6.0 and 5.0 have C++ casts (what about the earlier versions?)
        #define HAVE_CXX_CASTS
    #elif ( defined(__MINGW32__) || defined(__CYGWIN32__) ) \
          && wxCHECK_GCC_VERSION(2, 95)
        // GCC 2.95 has C++ casts, what about earlier versions?
        #define HAVE_CXX_CASTS
    #endif
#endif // HAVE_STATIC_CAST

#ifdef HAVE_CXX_CASTS
    #ifndef HAVE_CONST_CAST
        #define HAVE_CONST_CAST
    #endif
#endif // HAVE_CXX_CASTS

// ----------------------------------------------------------------------------
// portable calling conventions macros
// ----------------------------------------------------------------------------

// stdcall is used for all functions called by Windows under Windows
#if defined(__WINDOWS__)
    #if defined(__GNUWIN32__)
        #define wxSTDCALL __attribute__((stdcall))
    #else
        // both VC++ and Borland understand this
        #define wxSTDCALL _stdcall
    #endif

#else // Win
    // no such stupidness under Unix
    #define wxSTDCALL
#endif // platform

// LINKAGEMODE mode is empty for everyting except OS/2
#ifndef LINKAGEMODE
    #define LINKAGEMODE
#endif // LINKAGEMODE

// wxCALLBACK should be used for the functions which are called back by
// Windows (such as compare function for wxListCtrl)
#if defined(__WIN32__) && !defined(__WXMICROWIN__)
    #define wxCALLBACK wxSTDCALL
#else
    // no stdcall under Unix nor Win16
    #define wxCALLBACK
#endif // platform

// generic calling convention for the extern "C" functions

#if defined(__VISUALC__)
  #define   wxC_CALLING_CONV    _cdecl
#elif defined(__VISAGECPP__)
  #define   wxC_CALLING_CONV    _Optlink
#else   // !Visual C++
  #define   wxC_CALLING_CONV
#endif  // compiler

// callling convention for the qsort(3) callback
#define wxCMPFUNC_CONV wxC_CALLING_CONV

// compatibility :-(
#define CMPFUNC_CONV wxCMPFUNC_CONV

// ----------------------------------------------------------------------------
// Making or using wxWindows as a Windows DLL
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    // __declspec works in BC++ 5 and later, Watcom C++ 11.0 and later as well
    // as VC++ and gcc
    #if defined(__VISUALC__) || defined(__BORLANDC__) || defined(__GNUC__) || defined(__WATCOMC__)
        #define WXEXPORT __declspec(dllexport)
        #define WXIMPORT __declspec(dllimport)
    #else // compiler doesn't support __declspec()
        #define WXEXPORT
        #define WXIMPORT
    #endif
#elif defined(__WXPM__)
    #if defined (__WATCOMC__)
        #define WXEXPORT __declspec(dllexport)
        // __declspec(dllimport) prepends __imp to imported symbols. We do NOT
        // want that!
        #define WXIMPORT
    #elif (!(defined(__VISAGECPP__) && (__IBMCPP__ < 400 || __IBMC__ < 400 )))
        #define WXEXPORT _Export
        #define WXIMPORT _Export
    #endif
#elif defined(__WXMAC__) || defined(__WXCOCOA__)
    #ifdef __MWERKS__
        #define WXEXPORT __declspec(export)
        #define WXIMPORT __declspec(import)
    #endif
#endif

// for other platforms/compilers we don't anything
#ifndef WXEXPORT
    #define WXEXPORT
    #define WXIMPORT
#endif

// WXDLLEXPORT maps to export declaration when building the DLL, to import
// declaration if using it or to nothing at all if we don't use wxWin DLL
#ifdef WXMAKINGDLL
    #define WXDLLEXPORT WXEXPORT
    #define WXDLLEXPORT_DATA(type) WXEXPORT type
    #define WXDLLEXPORT_CTORFN
#elif defined(WXUSINGDLL)
    #define WXDLLEXPORT WXIMPORT
    #define WXDLLEXPORT_DATA(type) WXIMPORT type
    #define WXDLLEXPORT_CTORFN
#else // not making nor using DLL
    #define WXDLLEXPORT
    #define WXDLLEXPORT_DATA(type) type
    #define WXDLLEXPORT_CTORFN
#endif

// For ostream, istream ofstream
#if defined(__BORLANDC__) && defined( _RTLDLL )
#  define WXDLLIMPORT __import
#else
#  define WXDLLIMPORT
#endif

#ifdef __cplusplus
class WXDLLEXPORT wxObject;
class WXDLLEXPORT wxEvent;
#endif

// symbolic constant used by all Find()-like functions returning positive
// integer on success as failure indicator
#define wxNOT_FOUND       (-1)

// ----------------------------------------------------------------------------
// Very common macros
// ----------------------------------------------------------------------------

// Printf-like attribute definitions to obtain warnings with GNU C/C++
#if defined(__GNUC__) && !wxUSE_UNICODE
#  ifndef ATTRIBUTE_PRINTF
#    define ATTRIBUTE_PRINTF(m, n) __attribute__ ((__format__ (__printf__, m, n)))
#    define ATTRIBUTE_PRINTF_1 ATTRIBUTE_PRINTF(1, 2)
#    define ATTRIBUTE_PRINTF_2 ATTRIBUTE_PRINTF(2, 3)
#    define ATTRIBUTE_PRINTF_3 ATTRIBUTE_PRINTF(3, 4)
#    define ATTRIBUTE_PRINTF_4 ATTRIBUTE_PRINTF(4, 5)
#    define ATTRIBUTE_PRINTF_5 ATTRIBUTE_PRINTF(5, 6)
#  endif /* ATTRIBUTE_PRINTF */
#else
#  ifndef ATTRIBUTE_PRINTF
#    define ATTRIBUTE_PRINTF
#    define ATTRIBUTE_PRINTF_1
#    define ATTRIBUTE_PRINTF_2
#    define ATTRIBUTE_PRINTF_3
#    define ATTRIBUTE_PRINTF_4
#    define ATTRIBUTE_PRINTF_5
#  endif /* ATTRIBUTE_PRINTF */
#endif

// Macro to issue warning when using deprecated functions with gcc3 or MSVC7:
#if wxCHECK_GCC_VERSION(3, 1)
    #define wxDEPRECATED(x) x __attribute__ ((deprecated))
#elif defined(__VISUALC__) && (__VISUALC__ >= 1300)
    #define wxDEPRECATED(x) __declspec(deprecated) x
#else
    #define wxDEPRECATED(x) x
#endif

// everybody gets the assert and other debug macros
#ifdef __cplusplus
#include "wx/debug.h"
#endif

// NULL declaration: it must be defined as 0 for C++ programs (in particular,
// it must not be defined as "(void *)0" which is standard for C but completely
// breaks C++ code)
#include <stddef.h>

// Macro to cut down on compiler warnings.
#if REMOVE_UNUSED_ARG
    #define WXUNUSED(identifier) /* identifier */
#else  // stupid, broken compiler
    #define WXUNUSED(identifier) identifier
#endif

// some arguments are only used in debug mode, but unused in release one
#ifdef __WXDEBUG__
    #define WXUNUSED_UNLESS_DEBUG(param)  param
#else
    #define WXUNUSED_UNLESS_DEBUG(param)  WXUNUSED(param)
#endif

// delete pointer if it is not NULL and NULL it afterwards
// (checking that it's !NULL before passing it to delete is just a
//  a question of style, because delete will do it itself anyhow, but it might
//  be considered as an error by some overzealous debugging implementations of
//  the library, so we do it ourselves)
#define wxDELETE(p)      if ( (p) != NULL ) { delete p; p = NULL; }

// delete an array and NULL it (see comments above)
#define wxDELETEA(p)     if ( (p) ) { delete [] (p); p = NULL; }

// size of statically declared array
#define WXSIZEOF(array)   (sizeof(array)/sizeof(array[0]))

// ----------------------------------------------------------------------------
// compiler specific settings
// ----------------------------------------------------------------------------

// to allow compiling with warning level 4 under Microsoft Visual C++ some
// warnings just must be disabled
#ifdef  __VISUALC__
  #pragma warning(disable: 4514) // unreferenced inline func has been removed
/*
  you might be tempted to disable this one also: triggered by CHECK and FAIL
  macros in debug.h, but it's, overall, a rather useful one, so I leave it and
  will try to find some way to disable this warning just for CHECK/FAIL. Anyone?
*/
  #pragma warning(disable: 4127) // conditional expression is constant
#endif  // VC++

#if defined(__MWERKS__)
    #undef try
    #undef except
    #undef finally
    #define except(x) catch(...)
#endif // Metrowerks

// where should i put this? we need to make sure of this as it breaks
// the <iostream> code.
#if !wxUSE_IOSTREAMH && defined(__WXDEBUG__)
#  ifndef __MWERKS__
// #undef __WXDEBUG__
#    ifdef wxUSE_DEBUG_NEW_ALWAYS
#    undef wxUSE_DEBUG_NEW_ALWAYS
#    define wxUSE_DEBUG_NEW_ALWAYS 0
#    endif
#  endif
#endif

// Callback function type definition
#ifdef __cplusplus
typedef void (*wxFunction) (wxObject&, wxEvent&);
#endif

// ----------------------------------------------------------------------------
// OS mnemonics -- Identify the running OS (useful for Windows)
// ----------------------------------------------------------------------------

// Not all platforms are currently available or supported
enum
{
    wxUNKNOWN_PLATFORM,
    wxCURSES,                 // Text-only CURSES
    wxXVIEW_X,                // Sun's XView OpenLOOK toolkit
    wxMOTIF_X,                // OSF Motif 1.x.x
    wxCOSE_X,                 // OSF Common Desktop Environment
    wxNEXTSTEP,               // NeXTStep
    wxMAC,                    // Apple Mac OS 8/9/X with Mac paths
    wxMAC_DARWIN,             // Apple Mac OS X with Unix paths
    wxBEOS,                   // BeOS
    wxGTK,                    // GTK on X
    wxGTK_WIN32,              // GTK on Win32
    wxGTK_OS2,                // GTK on OS/2
    wxGTK_BEOS,               // GTK on BeOS
    wxGEOS,                   // GEOS
    wxOS2_PM,                 // OS/2 Workplace
    wxWINDOWS,                // Windows or WfW
    wxMICROWINDOWS,           // MicroWindows
    wxPENWINDOWS,             // Windows for Pen Computing
    wxWINDOWS_NT,             // Windows NT
    wxWIN32S,                 // Windows 32S API
    wxWIN95,                  // Windows 95
    wxWIN386,                 // Watcom 32-bit supervisor modus
    wxMGL_UNIX,               // MGL with direct hardware access
    wxMGL_X,                  // MGL on X
    wxMGL_WIN32,              // MGL on Win32
    wxMGL_OS2,                // MGL on OS/2
    wxMGL_DOS,                // MGL on MS-DOS
    wxWINDOWS_OS2,            // Native OS/2 PM
    wxUNIX,                   // wxBase under Unix
    wxX11                     // Plain X11 and Universal widgets
};

// ----------------------------------------------------------------------------
// standard wxWindows types
// ----------------------------------------------------------------------------

// the type for screen and DC coordinates

#if wxUSE_COMPATIBLE_COORD_TYPES
    // to ensure compatibility with 2.0, we must use long
    #define wxCoord long
#else  // !wxUSE_COMPATIBLE_COORD_TYPES
    #ifdef __WIN16__
        // under Win16, int is too small, so use long to allow for bigger
        // virtual canvases
        typedef long wxCoord;
    #else // !Win16
        // other platforms we support have at least 32bit int - quite enough
        typedef int wxCoord;
    #endif // Win16/!Win16
#endif // wxUSE_COMPATIBLE_COORD_TYPES/!wxUSE_COMPATIBLE_COORD_TYPES

// fixed length types

#define wxInt8    char    signed
#define wxUint8   char  unsigned

#ifdef __WINDOWS__
    #if defined(__WIN16__)
        #define wxInt16    int    signed
        #define wxUint16   int  unsigned
        #define wxInt32   long    signed
        #define wxUint32  long  unsigned
    #elif defined(__WIN32__)
        #define wxInt16  short    signed
        #define wxUint16 short  unsigned
        #define wxInt32    int    signed
        #define wxUint32   int  unsigned
    #else
        // Win64 will have different type sizes
        #error "Please define a 32 bit type"
    #endif
#else // !Windows
    // SIZEOF_XXX are defined by configure
    #if defined(SIZEOF_INT) && (SIZEOF_INT == 4)
        #define wxInt16  short    signed
        #define wxUint16 short  unsigned
        #define wxInt32    int    signed
        #define wxUint32   int  unsigned
    #elif defined(SIZEOF_INT) && (SIZEOF_INT == 2)
        #define wxInt16    int    signed
        #define wxUint16   int  unsigned
        #define wxInt32   long    signed
        #define wxUint32  long  unsigned
    #else
        // assume sizeof(int) == 4 - what else can we do
        wxCOMPILE_TIME_ASSERT( sizeof(int) == 4, IntMustBeExactly4Bytes);

        #define wxInt16  short    signed
        #define wxUint16 short  unsigned
        #define wxInt32    int    signed
        #define wxUint32   int  unsigned
    #endif
#endif // Win/!Win

#if defined(SIZEOF_LONG) && (SIZEOF_LONG == 8)
#define wxInt64   long    signed
#define wxUint64  long  unsigned
#elif defined(SIZEOF_LONG_LONG) && (SIZEOF_LONG_LONG == 8)
#define wxInt64   long long    signed
#define wxUint64  long long  unsigned
#else   // FIXME: what else can we do here aside from implementing wxULongLong
#define wxInt64   wxLongLong
#define wxUint64  wxULongLong
#endif

#define  wxByte   wxUint8
#define  wxWord   wxUint16

// base floating point types
// wxFloat32 : 32 bit IEEE float ( 1 sign , 8 exponent bits , 23 fraction bits
// wxFloat64 : 64 bit IEEE float ( 1 sign , 11 exponent bits , 52 fraction bits
// wxDouble : native fastest representation that has at least wxFloat64
//            precision, so use the IEEE types for storage , and this for calculations

typedef float wxFloat32 ;
#if (defined( __WXMAC__ ) || defined(__WXCOCOA__))  && defined (__MWERKS__)
    typedef short double wxFloat64;
#else
    typedef double wxFloat64;
#endif

#if defined( __WXMAC__ )  && !defined( __POWERPC__ )
    typedef long double wxDouble;
#else
    typedef double wxDouble ;
#endif

// ----------------------------------------------------------------------------
// byte ordering related definition and macros
// ----------------------------------------------------------------------------

// byte sex

#define  wxBIG_ENDIAN     4321
#define  wxLITTLE_ENDIAN  1234
#define  wxPDP_ENDIAN     3412

#ifdef WORDS_BIGENDIAN
#define  wxBYTE_ORDER  wxBIG_ENDIAN
#else
#define  wxBYTE_ORDER  wxLITTLE_ENDIAN
#endif

// byte swapping

#if defined (__MWERKS__) && ( (__MWERKS__ < 0x0900) || macintosh )
// assembler versions for these
#ifdef __POWERPC__
    inline wxUint16 wxUINT16_SWAP_ALWAYS( wxUint16 i )
        {return (__lhbrx( &i , 0 ) ) ;}
    inline wxInt16 wxINT16_SWAP_ALWAYS( wxInt16 i )
        {return (__lhbrx( &i , 0 ) ) ;}
    inline wxUint32 wxUINT32_SWAP_ALWAYS( wxUint32 i )
        {return (__lwbrx( &i , 0 ) ) ;}
    inline wxInt32 wxINT32_SWAP_ALWAYS( wxInt32 i )
        {return (__lwbrx( &i , 0 ) ) ;}
#else
    #pragma parameter __D0 wxUINT16_SWAP_ALWAYS(__D0)
    pascal wxUint16 wxUINT16_SWAP_ALWAYS(wxUint16 value)
        = { 0xE158 };

    #pragma parameter __D0 wxINT16_SWAP_ALWAYS(__D0)
    pascal wxInt16 wxINT16_SWAP_ALWAYS(wxInt16 value)
        = { 0xE158 };

    #pragma parameter __D0 wxUINT32_SWAP_ALWAYS (__D0)
    pascal wxUint32 wxUINT32_SWAP_ALWAYS(wxUint32 value)
        = { 0xE158, 0x4840, 0xE158 };

    #pragma parameter __D0 wxINT32_SWAP_ALWAYS (__D0)
    pascal wxInt32 wxINT32_SWAP_ALWAYS(wxInt32 value)
        = { 0xE158, 0x4840, 0xE158 };

#endif
#else // !MWERKS
#define wxUINT16_SWAP_ALWAYS(val) \
   ((wxUint16) ( \
    (((wxUint16) (val) & (wxUint16) 0x00ffU) << 8) | \
    (((wxUint16) (val) & (wxUint16) 0xff00U) >> 8)))

#define wxINT16_SWAP_ALWAYS(val) \
   ((wxInt16) ( \
    (((wxUint16) (val) & (wxUint16) 0x00ffU) << 8) | \
    (((wxUint16) (val) & (wxUint16) 0xff00U) >> 8)))

#define wxUINT32_SWAP_ALWAYS(val) \
   ((wxUint32) ( \
    (((wxUint32) (val) & (wxUint32) 0x000000ffU) << 24) | \
    (((wxUint32) (val) & (wxUint32) 0x0000ff00U) <<  8) | \
    (((wxUint32) (val) & (wxUint32) 0x00ff0000U) >>  8) | \
    (((wxUint32) (val) & (wxUint32) 0xff000000U) >> 24)))

#define wxINT32_SWAP_ALWAYS(val) \
   ((wxInt32) ( \
    (((wxUint32) (val) & (wxUint32) 0x000000ffU) << 24) | \
    (((wxUint32) (val) & (wxUint32) 0x0000ff00U) <<  8) | \
    (((wxUint32) (val) & (wxUint32) 0x00ff0000U) >>  8) | \
    (((wxUint32) (val) & (wxUint32) 0xff000000U) >> 24)))
#endif
// machine specific byte swapping

#if defined(SIZEOF_LONG) && (SIZEOF_LONG == 8)
#define wxUINT64_SWAP_ALWAYS(val) \
   ((wxUint64) ( \
    (((wxUint64) (val) & (wxUint64) 0x00000000000000ffUL) << 56) | \
    (((wxUint64) (val) & (wxUint64) 0x000000000000ff00UL) << 40) | \
    (((wxUint64) (val) & (wxUint64) 0x0000000000ff0000UL) << 24) | \
    (((wxUint64) (val) & (wxUint64) 0x00000000ff000000UL) <<  8) | \
    (((wxUint64) (val) & (wxUint64) 0x000000ff00000000UL) >>  8) | \
    (((wxUint64) (val) & (wxUint64) 0x0000ff0000000000UL) >> 24) | \
    (((wxUint64) (val) & (wxUint64) 0x00ff000000000000UL) >> 40) | \
    (((wxUint64) (val) & (wxUint64) 0xff00000000000000UL) >> 56)))

#define wxINT64_SWAP_ALWAYS(val) \
   ((wxInt64) ( \
    (((wxUint64) (val) & (wxUint64) 0x00000000000000ffUL) << 56) | \
    (((wxUint64) (val) & (wxUint64) 0x000000000000ff00UL) << 40) | \
    (((wxUint64) (val) & (wxUint64) 0x0000000000ff0000UL) << 24) | \
    (((wxUint64) (val) & (wxUint64) 0x00000000ff000000UL) <<  8) | \
    (((wxUint64) (val) & (wxUint64) 0x000000ff00000000UL) >>  8) | \
    (((wxUint64) (val) & (wxUint64) 0x0000ff0000000000UL) >> 24) | \
    (((wxUint64) (val) & (wxUint64) 0x00ff000000000000UL) >> 40) | \
    (((wxUint64) (val) & (wxUint64) 0xff00000000000000UL) >> 56)))

#elif defined(SIZEOF_LONG_LONG) && (SIZEOF_LONG_LONG == 8)
#define wxUINT64_SWAP_ALWAYS(val) \
   ((wxUint64) ( \
    (((wxUint64) (val) & (wxUint64) 0x00000000000000ffULL) << 56) | \
    (((wxUint64) (val) & (wxUint64) 0x000000000000ff00ULL) << 40) | \
    (((wxUint64) (val) & (wxUint64) 0x0000000000ff0000ULL) << 24) | \
    (((wxUint64) (val) & (wxUint64) 0x00000000ff000000ULL) <<  8) | \
    (((wxUint64) (val) & (wxUint64) 0x000000ff00000000ULL) >>  8) | \
    (((wxUint64) (val) & (wxUint64) 0x0000ff0000000000ULL) >> 24) | \
    (((wxUint64) (val) & (wxUint64) 0x00ff000000000000ULL) >> 40) | \
    (((wxUint64) (val) & (wxUint64) 0xff00000000000000ULL) >> 56)))

#define wxINT64_SWAP_ALWAYS(val) \
   ((wxInt64) ( \
    (((wxUint64) (val) & (wxUint64) 0x00000000000000ffULL) << 56) | \
    (((wxUint64) (val) & (wxUint64) 0x000000000000ff00ULL) << 40) | \
    (((wxUint64) (val) & (wxUint64) 0x0000000000ff0000ULL) << 24) | \
    (((wxUint64) (val) & (wxUint64) 0x00000000ff000000ULL) <<  8) | \
    (((wxUint64) (val) & (wxUint64) 0x000000ff00000000ULL) >>  8) | \
    (((wxUint64) (val) & (wxUint64) 0x0000ff0000000000ULL) >> 24) | \
    (((wxUint64) (val) & (wxUint64) 0x00ff000000000000ULL) >> 40) | \
    (((wxUint64) (val) & (wxUint64) 0xff00000000000000ULL) >> 56)))

#else
#define wxUINT64_SWAP_ALWAYS(val) \
   ((wxUint64) ( \
    ((wxULongLong(val) & wxULongLong(0L, 0x000000ffU)) << 56) | \
    ((wxULongLong(val) & wxULongLong(0L, 0x0000ff00U)) << 40) | \
    ((wxULongLong(val) & wxULongLong(0L, 0x00ff0000U)) << 24) | \
    ((wxULongLong(val) & wxULongLong(0L, 0xff000000U)) <<  8) | \
    ((wxULongLong(val) & wxULongLong(0x000000ffL, 0U)) >>  8) | \
    ((wxULongLong(val) & wxULongLong(0x0000ff00L, 0U)) >> 24) | \
    ((wxULongLong(val) & wxULongLong(0x00ff0000L, 0U)) >> 40) | \
    ((wxULongLong(val) & wxULongLong(0xff000000L, 0U)) >> 56)))

#define wxINT64_SWAP_ALWAYS(val) \
   ((wxInt64) ( \
    ((wxLongLong(val) & wxLongLong(0L, 0x000000ffU)) << 56) | \
    ((wxLongLong(val) & wxLongLong(0L, 0x0000ff00U)) << 40) | \
    ((wxLongLong(val) & wxLongLong(0L, 0x00ff0000U)) << 24) | \
    ((wxLongLong(val) & wxLongLong(0L, 0xff000000U)) <<  8) | \
    ((wxLongLong(val) & wxLongLong(0x000000ffL, 0U)) >>  8) | \
    ((wxLongLong(val) & wxLongLong(0x0000ff00L, 0U)) >> 24) | \
    ((wxLongLong(val) & wxLongLong(0x00ff0000L, 0U)) >> 40) | \
    ((wxLongLong(val) & wxLongLong(0xff000000L, 0U)) >> 56)))
#endif

#ifdef WORDS_BIGENDIAN
  #define wxUINT16_SWAP_ON_BE(val)  wxUINT16_SWAP_ALWAYS(val)
  #define wxINT16_SWAP_ON_BE(val)   wxINT16_SWAP_ALWAYS(val)
  #define wxUINT16_SWAP_ON_LE(val)  (val)
  #define wxINT16_SWAP_ON_LE(val)   (val)
  #define wxUINT32_SWAP_ON_BE(val)  wxUINT32_SWAP_ALWAYS(val)
  #define wxINT32_SWAP_ON_BE(val)   wxINT32_SWAP_ALWAYS(val)
  #define wxUINT32_SWAP_ON_LE(val)  (val)
  #define wxINT32_SWAP_ON_LE(val)   (val)
  #define wxUINT64_SWAP_ON_BE(val)  wxUINT64_SWAP_ALWAYS(val)
  #define wxUINT64_SWAP_ON_LE(val)  (val)
#else
  #define wxUINT16_SWAP_ON_LE(val)  wxUINT16_SWAP_ALWAYS(val)
  #define wxINT16_SWAP_ON_LE(val)   wxINT16_SWAP_ALWAYS(val)
  #define wxUINT16_SWAP_ON_BE(val)  (val)
  #define wxINT16_SWAP_ON_BE(val)   (val)
  #define wxUINT32_SWAP_ON_LE(val)  wxUINT32_SWAP_ALWAYS(val)
  #define wxINT32_SWAP_ON_LE(val)   wxINT32_SWAP_ALWAYS(val)
  #define wxUINT32_SWAP_ON_BE(val)  (val)
  #define wxINT32_SWAP_ON_BE(val)   (val)
  #define wxUINT64_SWAP_ON_LE(val)  wxUINT64_SWAP_ALWAYS(val)
  #define wxUINT64_SWAP_ON_BE(val)  (val)
#endif

// Macros to convert from unsigned long to void pointer.
// High order truncation occurs if the respective type is not large enough.
#define WXPTRULONGSLICE (((wxBYTE_ORDER==wxBIG_ENDIAN)&&(sizeof(void*)==8)&&(sizeof(unsigned long)<8))?1:0)
#define wxPtrToULong(p) (((unsigned long*)(&(p)))[WXPTRULONGSLICE])
#define wxULongToPtr(p,n) (p=NULL,wxPtrToULong(p)=(unsigned long)(n),p)

// ----------------------------------------------------------------------------
// Geometric flags
// ----------------------------------------------------------------------------

enum wxGeometryCentre
{
    wxCENTRE                  = 0x0001,
    wxCENTER                  = wxCENTRE
};

// centering into frame rather than screen (obsolete)
#define wxCENTER_FRAME          0x0000
// centre on screen rather than parent
#define wxCENTRE_ON_SCREEN      0x0002
#define wxCENTER_ON_SCREEN      wxCENTRE_ON_SCREEN

enum wxOrientation
{
    wxHORIZONTAL              = 0x0004,
    wxVERTICAL                = 0x0008,

    wxBOTH                    = (wxVERTICAL | wxHORIZONTAL)
};

enum wxDirection
{
    wxLEFT                    = 0x0010,
    wxRIGHT                   = 0x0020,
    wxUP                      = 0x0040,
    wxDOWN                    = 0x0080,

    wxTOP                     = wxUP,
    wxBOTTOM                  = wxDOWN,

    wxNORTH                   = wxUP,
    wxSOUTH                   = wxDOWN,
    wxWEST                    = wxLEFT,
    wxEAST                    = wxRIGHT,

    wxALL                     = (wxUP | wxDOWN | wxRIGHT | wxLEFT)
};

enum wxAlignment
{
    wxALIGN_NOT               = 0x0000,
    wxALIGN_CENTER_HORIZONTAL = 0x0100,
    wxALIGN_CENTRE_HORIZONTAL = wxALIGN_CENTER_HORIZONTAL,
    wxALIGN_LEFT              = wxALIGN_NOT,
    wxALIGN_TOP               = wxALIGN_NOT,
    wxALIGN_RIGHT             = 0x0200,
    wxALIGN_BOTTOM            = 0x0400,
    wxALIGN_CENTER_VERTICAL   = 0x0800,
    wxALIGN_CENTRE_VERTICAL   = wxALIGN_CENTER_VERTICAL,

    wxALIGN_CENTER            = (wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL),
    wxALIGN_CENTRE            = wxALIGN_CENTER,

    // a mask to extract alignment from the combination of flags
    wxALIGN_MASK              = 0x0f00
};

enum wxStretch
{
    wxSTRETCH_NOT             = 0x0000,
    wxSHRINK                  = 0x1000,
    wxGROW                    = 0x2000,
    wxEXPAND                  = wxGROW,
    wxSHAPED                  = 0x4000,
    wxADJUST_MINSIZE          = 0x8000,
    wxTILE                    = 0xc000
};

// border flags: the values are chosen for backwards compatibility
enum wxBorder
{
    // this is different from wxBORDER_NONE as by default the controls do have
    // border
    wxBORDER_DEFAULT = 0,

    wxBORDER_NONE   = 0x00200000,
    wxBORDER_STATIC = 0x01000000,
    wxBORDER_SIMPLE = 0x02000000,
    wxBORDER_RAISED = 0x04000000,
    wxBORDER_SUNKEN = 0x08000000,
    wxBORDER_DOUBLE = 0x10000000,

    // a mask to extract border style from the combination of flags
    wxBORDER_MASK   = 0x1f200000
};

// ----------------------------------------------------------------------------
// Window style flags
// ----------------------------------------------------------------------------

/*
 * Values are chosen so they can be |'ed in a bit list.
 * Some styles are used across more than one group,
 * so the values mustn't clash with others in the group.
 * Otherwise, numbers can be reused across groups.
 *
 * From version 1.66:
 * Window (cross-group) styles now take up the first half
 * of the flag, and control-specific styles the
 * second half.
 *
 */

/*
 * Window (Frame/dialog/subwindow/panel item) style flags
 */
#define wxVSCROLL               0x80000000
#define wxHSCROLL               0x40000000
#define wxCAPTION               0x20000000

// New styles (border styles are now in their own enum)
#define wxDOUBLE_BORDER         wxBORDER_DOUBLE
#define wxSUNKEN_BORDER         wxBORDER_SUNKEN
#define wxRAISED_BORDER         wxBORDER_RAISED
#define wxBORDER                wxBORDER_SIMPLE
#define wxSIMPLE_BORDER         wxBORDER_SIMPLE
#define wxSTATIC_BORDER         wxBORDER_STATIC
#define wxNO_BORDER             wxBORDER_NONE

// Override CTL3D etc. control colour processing to allow own background
// colour.
// Override CTL3D or native 3D styles for children
#define wxNO_3D                 0x00800000

// OBSOLETE - use wxNO_3D instead
#define wxUSER_COLOURS          wxNO_3D

// wxALWAYS_SHOW_SB: instead of hiding the scrollbar when it is not needed,
// disable it - but still show (see also wxLB_ALWAYS_SB style)
//
// NB: as this style is only supported by wxUniversal so far as it doesn't use
//     wxUSER_COLOURS/wxNO_3D, we reuse the same style value
#define wxALWAYS_SHOW_SB        0x00800000

// Clip children when painting, which reduces flicker in e.g. frames and
// splitter windows, but can't be used in a panel where a static box must be
// 'transparent' (panel paints the background for it)
#define wxCLIP_CHILDREN         0x00400000

// Note we're reusing the wxCAPTION style because we won't need captions
// for subwindows/controls
#define wxCLIP_SIBLINGS         0x20000000

#define wxTRANSPARENT_WINDOW    0x00100000

// Add this style to a panel to get tab traversal working outside of dialogs
// (on by default for wxPanel, wxDialog, wxScrolledWindow)
#define wxTAB_TRAVERSAL         0x00080000

// Add this style if the control wants to get all keyboard messages (under
// Windows, it won't normally get the dialog navigation key events)
#define wxWANTS_CHARS           0x00040000

// Make window retained (mostly Motif, I think) -- obsolete (VZ)?
#define wxRETAINED              0x00020000
#define wxBACKINGSTORE          wxRETAINED

// set this flag to create a special popup window: it will be always shown on
// top of other windows, will capture the mouse and will be dismissed when the
// mouse is clicked outside of it or if it loses focus in any other way
#define wxPOPUP_WINDOW          0x00020000

// don't invalidate the whole window (resulting in a PAINT event) when the
// window is resized (currently, makes sense for wxMSW only)
#define wxNO_FULL_REPAINT_ON_RESIZE 0x00010000

/*
 * Extra window style flags (use wxWS_EX prefix to make it clear that they
 * should be passed to wxWindow::SetExtraStyle(), not SetWindowStyle())
 */

// by default, TransferDataTo/FromWindow() only work on direct children of the
// window (compatible behaviour), set this flag to make them recursively
// descend into all subwindows
#define wxWS_EX_VALIDATE_RECURSIVELY    0x00000001

// wxCommandEvents and the objects of the derived classes are forwarded to the
// parent window and so on recursively by default. Using this flag for the
// given window allows to block this propagation at this window, i.e. prevent
// the events from being propagated further upwards. The dialogs have this
// flag on by default.
#define wxWS_EX_BLOCK_EVENTS            0x00000002

// don't use this window as an implicit parent for the other windows: this must
// be used with transient windows as otherwise there is the risk of creating a
// dialog/frame with this window as a parent which would lead to a crash if the
// parent is destroyed before the child
#define wxWS_EX_TRANSIENT               0x00000004

// Use this style to add a context-sensitive help to the window (currently for
// Win32 only and it doesn't work if wxMINIMIZE_BOX or wxMAXIMIZE_BOX are used)
#define wxFRAME_EX_CONTEXTHELP  0x00000004
#define wxDIALOG_EX_CONTEXTHELP 0x00000004

/*
 * wxFrame/wxDialog style flags
 */
#define wxSTAY_ON_TOP           0x8000
#define wxICONIZE               0x4000
#define wxMINIMIZE              wxICONIZE
#define wxMAXIMIZE              0x2000
#define wxCLOSE_BOX                 0x1000

#define wxSYSTEM_MENU           0x0800
#define wxMINIMIZE_BOX          0x0400
#define wxMAXIMIZE_BOX          0x0200
#define wxTINY_CAPTION_HORIZ    0x0100
#define wxTINY_CAPTION_VERT     0x0080
#define wxRESIZE_BORDER         0x0040

#define wxDIALOG_NO_PARENT      0x0001  // Don't make owned by apps top window
#define wxFRAME_NO_TASKBAR      0x0002  // No taskbar button (MSW only)
#define wxFRAME_TOOL_WINDOW     0x0004  // No taskbar button, no system menu
#define wxFRAME_FLOAT_ON_PARENT 0x0008  // Always above its parent

// deprecated versions defined for compatibility reasons
#define wxRESIZE_BOX            wxMAXIMIZE_BOX
#define wxTHICK_FRAME           wxRESIZE_BORDER

// obsolete styles, unused any more
#define wxDIALOG_MODAL          0x0020  // free flag value 0x0020
#define wxDIALOG_MODELESS       0x0000

/*
 * MDI parent frame style flags
 * Can overlap with some of the above.
 */

#define wxFRAME_NO_WINDOW_MENU  0x0100

#if WXWIN_COMPATIBILITY
#define wxDEFAULT_FRAME wxDEFAULT_FRAME_STYLE
#endif

#define wxDEFAULT_FRAME_STYLE \
  (wxSYSTEM_MENU | wxRESIZE_BORDER | \
   wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | \
   wxCAPTION | wxCLIP_CHILDREN)

#define wxDEFAULT_DIALOG_STYLE  (wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX)

/*
 * wxExtDialog style flags
 */
#define wxED_CLIENT_MARGIN      0x0004
#define wxED_BUTTONS_BOTTOM     0x0000  // has no effect
#define wxED_BUTTONS_RIGHT      0x0002
#define wxED_STATIC_LINE        0x0001

#if defined(__WXMSW__) || defined(__WXMAC__)
#   define wxEXT_DIALOG_STYLE  (wxDEFAULT_DIALOG_STYLE|wxED_CLIENT_MARGIN)
#else
#   define wxEXT_DIALOG_STYLE  (wxDEFAULT_DIALOG_STYLE|wxED_CLIENT_MARGIN|wxED_STATIC_LINE)
#endif

/*
 * wxMenuBar style flags
 */
// use native docking
#define wxMB_DOCKABLE       0x0001

/*
 * wxMenu style flags
 */
#define wxMENU_TEAROFF      0x0001

/*
 * Apply to all panel items
 */
#define wxCOLOURED          0x0800
#define wxFIXED_LENGTH      0x0400

/*
 * Styles for wxListBox
 */
#define wxLB_SORT           0x0010
#define wxLB_SINGLE         0x0020
#define wxLB_MULTIPLE       0x0040
#define wxLB_EXTENDED       0x0080
// wxLB_OWNERDRAW is Windows-only
#define wxLB_OWNERDRAW      0x0100
#define wxLB_NEEDED_SB      0x0200
#define wxLB_ALWAYS_SB      0x0400
#define wxLB_HSCROLL        wxHSCROLL
// always show an entire number of rows
#define wxLB_INT_HEIGHT     0x0800

// deprecated synonyms
#define wxPROCESS_ENTER     0x0400  // wxTE_PROCESS_ENTER
#define wxPASSWORD          0x0800  // wxTE_PASSWORD

/*
 * wxComboBox style flags
 */
#define wxCB_SIMPLE         0x0004
#define wxCB_SORT           0x0008
#define wxCB_READONLY       0x0010
#define wxCB_DROPDOWN       0x0020

/*
 * wxRadioBox style flags
 */
// should we number the items from left to right or from top to bottom in a 2d
// radiobox?
#define wxRA_LEFTTORIGHT    0x0001
#define wxRA_TOPTOBOTTOM    0x0002

// New, more intuitive names to specify majorDim argument
#define wxRA_SPECIFY_COLS   wxHORIZONTAL
#define wxRA_SPECIFY_ROWS   wxVERTICAL

// Old names for compatibility
#define wxRA_HORIZONTAL     wxHORIZONTAL
#define wxRA_VERTICAL       wxVERTICAL

/*
 * wxRadioButton style flag
 */
#define wxRB_GROUP          0x0004
#define wxRB_SINGLE         0x0008

/*
 * wxGauge flags
 */
#define wxGA_HORIZONTAL      wxHORIZONTAL
#define wxGA_VERTICAL        wxVERTICAL
#define wxGA_PROGRESSBAR     0x0010
// Windows only
#define wxGA_SMOOTH          0x0020

/*
 * wxSlider flags
 */
#define wxSL_HORIZONTAL      wxHORIZONTAL // 4
#define wxSL_VERTICAL        wxVERTICAL   // 8
// The next one is obsolete - use scroll events instead
#define wxSL_NOTIFY_DRAG     0x0000
#define wxSL_TICKS           0x0010
#define wxSL_AUTOTICKS       wxSL_TICKS // we don't support manual ticks
#define wxSL_LABELS          0x0020
#define wxSL_LEFT            0x0040
#define wxSL_TOP             0x0080
#define wxSL_RIGHT           0x0100
#define wxSL_BOTTOM          0x0200
#define wxSL_BOTH            0x0400
#define wxSL_SELRANGE        0x0800

/*
 * wxScrollBar flags
 */
#define wxSB_HORIZONTAL      wxHORIZONTAL
#define wxSB_VERTICAL        wxVERTICAL

/*
 * wxSpinButton flags.
 * Note that a wxSpinCtrl is sometimes defined as
 * a wxTextCtrl, and so the flags must be different
 * from wxTextCtrl's.
 */
#define wxSP_HORIZONTAL       wxHORIZONTAL // 4
#define wxSP_VERTICAL         wxVERTICAL   // 8
#define wxSP_ARROW_KEYS       0x1000
#define wxSP_WRAP             0x2000

/*
 * wxSplitterWindow flags
 */
#define wxSP_NOBORDER         0x0000
#define wxSP_NOSASH           0x0010
#define wxSP_BORDER           0x0020
#define wxSP_PERMIT_UNSPLIT   0x0040
#define wxSP_LIVE_UPDATE      0x0080
#define wxSP_3DSASH           0x0100
#define wxSP_3DBORDER         0x0200
#define wxSP_FULLSASH         0x0400
#define wxSP_3D               (wxSP_3DBORDER | wxSP_3DSASH)
#define wxSP_SASH_AQUA        0x0800

/*
 * wxNotebook flags
 */
#define wxNB_FIXEDWIDTH       0x0010
#define wxNB_TOP              0x0000    // default
#define wxNB_LEFT             0x0020
#define wxNB_RIGHT            0x0040
#define wxNB_BOTTOM           0x0080
#define wxNB_MULTILINE        0x0100

/*
 * wxTabCtrl flags
 */
#define wxTC_RIGHTJUSTIFY     0x0010
#define wxTC_FIXEDWIDTH       0x0020
#define wxTC_TOP              0x0000    // default
#define wxTC_LEFT             0x0020
#define wxTC_RIGHT            0x0040
#define wxTC_BOTTOM           0x0080
#define wxTC_MULTILINE        wxNB_MULTILINE
#define wxTC_OWNERDRAW        0x0200

// wxToolBar style flags
#define wxTB_HORIZONTAL     wxHORIZONTAL    // == 0x0004
#define wxTB_VERTICAL       wxVERTICAL      // == 0x0008
#define wxTB_3DBUTTONS      0x0010
#define wxTB_FLAT           0x0020          // supported only under Win98+/GTK
#define wxTB_DOCKABLE       0x0040          // use native docking under GTK
#define wxTB_NOICONS        0x0080          // don't show the icons
#define wxTB_TEXT           0x0100          // show the text
#define wxTB_NODIVIDER      0x0200          // don't show the divider (Windows)
#define wxTB_NOALIGN        0x0400          // no automatic alignment (Windows)

/*
 * wxStatusBar95 flags
 */
#define wxST_SIZEGRIP         0x0010

/*
 * wxStaticText flags
 */
#define wxST_NO_AUTORESIZE    0x0001

/*
 * wxStaticBitmap flags
 */
#define wxBI_EXPAND           wxEXPAND

/*
 * wxStaticLine flags
 */
#define wxLI_HORIZONTAL         wxHORIZONTAL
#define wxLI_VERTICAL           wxVERTICAL

/*
 * wxProgressDialog flags
 */
#define wxPD_CAN_ABORT          0x0001
#define wxPD_APP_MODAL          0x0002
#define wxPD_AUTO_HIDE          0x0004
#define wxPD_ELAPSED_TIME       0x0008
#define wxPD_ESTIMATED_TIME     0x0010
// wxGA_SMOOTH = 0x0020 may also be used with wxProgressDialog
// NO!!! This is wxDIALOG_MODAL and will cause the progress dialog to
// be modal. No progress will then be made at all.
#define wxPD_REMAINING_TIME     0x0040

/*
 * wxDirDialog styles
 */

#define wxDD_NEW_DIR_BUTTON     0x0080

/*
 * extended dialog specifiers. these values are stored in a different
 * flag and thus do not overlap with other style flags. note that these
 * values do not correspond to the return values of the dialogs (for
 * those values, look at the wxID_XXX defines).
 */

// wxCENTRE already defined as  0x00000001
#define wxYES                   0x00000002
#define wxOK                    0x00000004
#define wxNO                    0x00000008
#define wxYES_NO                (wxYES | wxNO)
#define wxCANCEL                0x00000010

#define wxYES_DEFAULT           0x00000000  // has no effect (default)
#define wxNO_DEFAULT            0x00000080

#define wxICON_EXCLAMATION      0x00000100
#define wxICON_HAND             0x00000200
#define wxICON_WARNING          wxICON_EXCLAMATION
#define wxICON_ERROR            wxICON_HAND
#define wxICON_QUESTION         0x00000400
#define wxICON_INFORMATION      0x00000800
#define wxICON_STOP             wxICON_HAND
#define wxICON_ASTERISK         wxICON_INFORMATION
#define wxICON_MASK             (0x00000100|0x00000200|0x00000400|0x00000800)

#define  wxFORWARD              0x00001000
#define  wxBACKWARD             0x00002000
#define  wxRESET                0x00004000
#define  wxHELP                 0x00008000
#define  wxMORE                 0x00010000
#define  wxSETUP                0x00020000

// ----------------------------------------------------------------------------
// standard IDs
// ----------------------------------------------------------------------------

// any id: means that we don't care about the id, whether when installing an
// event handler or when creating a new window
enum
{
    wxID_ANY = -1
};

// id for a separator line in the menu (invalid for normal item)
enum
{
    wxID_SEPARATOR = -1
};

// Standard menu IDs
enum
{
    wxID_LOWEST = 4999,

    wxID_OPEN,
    wxID_CLOSE,
    wxID_NEW,
    wxID_SAVE,
    wxID_SAVEAS,
    wxID_REVERT,
    wxID_EXIT,
    wxID_UNDO,
    wxID_REDO,
    wxID_HELP,
    wxID_PRINT,
    wxID_PRINT_SETUP,
    wxID_PREVIEW,
    wxID_ABOUT,
    wxID_HELP_CONTENTS,
    wxID_HELP_COMMANDS,
    wxID_HELP_PROCEDURES,
    wxID_HELP_CONTEXT,
    wxID_CLOSE_ALL,

    wxID_CUT = 5030,
    wxID_COPY,
    wxID_PASTE,
    wxID_CLEAR,
    wxID_FIND,
    wxID_DUPLICATE,
    wxID_SELECTALL,

    wxID_FILE1 = 5050,
    wxID_FILE2,
    wxID_FILE3,
    wxID_FILE4,
    wxID_FILE5,
    wxID_FILE6,
    wxID_FILE7,
    wxID_FILE8,
    wxID_FILE9,

    // Standard button IDs
    wxID_OK = 5100,
    wxID_CANCEL,
    wxID_APPLY,
    wxID_YES,
    wxID_NO,
    wxID_STATIC,
    wxID_FORWARD,
    wxID_BACKWARD,
    wxID_DEFAULT,
    wxID_MORE,
    wxID_SETUP,
    wxID_RESET,
    wxID_CONTEXT_HELP,
    wxID_YESTOALL,
    wxID_NOTOALL,
    wxID_ABORT,
    wxID_RETRY,
    wxID_IGNORE,

    // System menu IDs (used by wxUniv):
    wxID_SYSTEM_MENU = 5200,
    wxID_CLOSE_FRAME,
    wxID_MOVE_FRAME,
    wxID_RESIZE_FRAME,
    wxID_MAXIMIZE_FRAME,
    wxID_ICONIZE_FRAME,
    wxID_RESTORE_FRAME,

    // IDs used by generic file dialog (13 consecutive starting from this value)
    wxID_FILEDLGG = 5900,

    wxID_HIGHEST = 5999
};

// ----------------------------------------------------------------------------
// other constants
// ----------------------------------------------------------------------------

// menu and toolbar item kinds
enum wxItemKind
{
    wxITEM_SEPARATOR = -1,
    wxITEM_NORMAL,
    wxITEM_CHECK,
    wxITEM_RADIO,
    wxITEM_MAX
};

// hit test results
enum wxHitTest
{
    wxHT_NOWHERE,

    // scrollbar
    wxHT_SCROLLBAR_FIRST = wxHT_NOWHERE,
    wxHT_SCROLLBAR_ARROW_LINE_1,    // left or upper arrow to scroll by line
    wxHT_SCROLLBAR_ARROW_LINE_2,    // right or down
    wxHT_SCROLLBAR_ARROW_PAGE_1,    // left or upper arrow to scroll by page
    wxHT_SCROLLBAR_ARROW_PAGE_2,    // right or down
    wxHT_SCROLLBAR_THUMB,           // on the thumb
    wxHT_SCROLLBAR_BAR_1,           // bar to the left/above the thumb
    wxHT_SCROLLBAR_BAR_2,           // bar to the right/below the thumb
    wxHT_SCROLLBAR_LAST,

    // window
    wxHT_WINDOW_OUTSIDE,            // not in this window at all
    wxHT_WINDOW_INSIDE,             // in the client area
    wxHT_WINDOW_VERT_SCROLLBAR,     // on the vertical scrollbar
    wxHT_WINDOW_HORZ_SCROLLBAR,     // on the horizontal scrollbar
    wxHT_WINDOW_CORNER,             // on the corner between 2 scrollbars

    wxHT_MAX
};

// ----------------------------------------------------------------------------
// Possible SetSize flags
// ----------------------------------------------------------------------------

// Use internally-calculated width if -1
#define wxSIZE_AUTO_WIDTH       0x0001
// Use internally-calculated height if -1
#define wxSIZE_AUTO_HEIGHT      0x0002
// Use internally-calculated width and height if each is -1
#define wxSIZE_AUTO             (wxSIZE_AUTO_WIDTH|wxSIZE_AUTO_HEIGHT)
// Ignore missing (-1) dimensions (use existing).
// For readability only: test for wxSIZE_AUTO_WIDTH/HEIGHT in code.
#define wxSIZE_USE_EXISTING     0x0000
// Allow -1 as a valid position
#define wxSIZE_ALLOW_MINUS_ONE  0x0004
// Don't do parent client adjustments (for implementation only)
#define wxSIZE_NO_ADJUSTMENTS   0x0008

// ----------------------------------------------------------------------------
// GDI descriptions
// ----------------------------------------------------------------------------

enum
{
    // Text font families
    wxDEFAULT    = 70,
    wxDECORATIVE,
    wxROMAN,
    wxSCRIPT,
    wxSWISS,
    wxMODERN,
    wxTELETYPE,  /* @@@@ */

    // Proportional or Fixed width fonts (not yet used)
    wxVARIABLE   = 80,
    wxFIXED,

    wxNORMAL     = 90,
    wxLIGHT,
    wxBOLD,
    // Also wxNORMAL for normal (non-italic text)
    wxITALIC,
    wxSLANT,

    // Pen styles
    wxSOLID      =   100,
    wxDOT,
    wxLONG_DASH,
    wxSHORT_DASH,
    wxDOT_DASH,
    wxUSER_DASH,

    wxTRANSPARENT,

    // Brush & Pen Stippling. Note that a stippled pen cannot be dashed!!
    // Note also that stippling a Pen IS meaningfull, because a Line is
    wxSTIPPLE_MASK_OPAQUE, //mask is used for blitting monochrome using text fore and back ground colors
    wxSTIPPLE_MASK,        //mask is used for masking areas in the stipple bitmap (TO DO)
    // drawn with a Pen, and without any Brush -- and it can be stippled.
    wxSTIPPLE =          110,
    wxBDIAGONAL_HATCH,
    wxCROSSDIAG_HATCH,
    wxFDIAGONAL_HATCH,
    wxCROSS_HATCH,
    wxHORIZONTAL_HATCH,
    wxVERTICAL_HATCH,

    wxJOIN_BEVEL =     120,
    wxJOIN_MITER,
    wxJOIN_ROUND,

    wxCAP_ROUND =      130,
    wxCAP_PROJECTING,
    wxCAP_BUTT
};

// VZ: why doesn't it start with "wx"? FIXME
#define IS_HATCH(s)    ((s)>=wxBDIAGONAL_HATCH && (s)<=wxVERTICAL_HATCH)

// Logical ops
typedef enum
{
  wxCLEAR,        wxROP_BLACK = wxCLEAR,             wxBLIT_BLACKNESS = wxCLEAR,        // 0
  wxXOR,          wxROP_XORPEN = wxXOR,              wxBLIT_SRCINVERT = wxXOR,          // src XOR dst
  wxINVERT,       wxROP_NOT = wxINVERT,              wxBLIT_DSTINVERT = wxINVERT,       // NOT dst
  wxOR_REVERSE,   wxROP_MERGEPENNOT = wxOR_REVERSE,  wxBLIT_00DD0228 = wxOR_REVERSE,    // src OR (NOT dst)
  wxAND_REVERSE,  wxROP_MASKPENNOT = wxAND_REVERSE,  wxBLIT_SRCERASE = wxAND_REVERSE,   // src AND (NOT dst)
  wxCOPY,         wxROP_COPYPEN = wxCOPY,            wxBLIT_SRCCOPY = wxCOPY,           // src
  wxAND,          wxROP_MASKPEN = wxAND,             wxBLIT_SRCAND = wxAND,             // src AND dst
  wxAND_INVERT,   wxROP_MASKNOTPEN = wxAND_INVERT,   wxBLIT_00220326 = wxAND_INVERT,    // (NOT src) AND dst
  wxNO_OP,        wxROP_NOP = wxNO_OP,               wxBLIT_00AA0029 = wxNO_OP,         // dst
  wxNOR,          wxROP_NOTMERGEPEN = wxNOR,         wxBLIT_NOTSRCERASE = wxNOR,        // (NOT src) AND (NOT dst)
  wxEQUIV,        wxROP_NOTXORPEN = wxEQUIV,         wxBLIT_00990066 = wxEQUIV,         // (NOT src) XOR dst
  wxSRC_INVERT,   wxROP_NOTCOPYPEN = wxSRC_INVERT,   wxBLIT_NOTSCRCOPY = wxSRC_INVERT,  // (NOT src)
  wxOR_INVERT,    wxROP_MERGENOTPEN = wxOR_INVERT,   wxBLIT_MERGEPAINT = wxOR_INVERT,   // (NOT src) OR dst
  wxNAND,         wxROP_NOTMASKPEN = wxNAND,         wxBLIT_007700E6 = wxNAND,          // (NOT src) OR (NOT dst)
  wxOR,           wxROP_MERGEPEN = wxOR,             wxBLIT_SRCPAINT = wxOR,            // src OR dst
  wxSET,          wxROP_WHITE = wxSET,               wxBLIT_WHITENESS = wxSET           // 1
} form_ops_t;

// Flood styles
enum
{
    wxFLOOD_SURFACE = 1,
    wxFLOOD_BORDER
};

// Polygon filling mode
enum
{
    wxODDEVEN_RULE = 1,
    wxWINDING_RULE
};

// ToolPanel in wxFrame (VZ: unused?)
enum
{
    wxTOOL_TOP = 1,
    wxTOOL_BOTTOM,
    wxTOOL_LEFT,
    wxTOOL_RIGHT
};

// the values of the format constants should be the same as correspondign
// CF_XXX constants in Windows API
enum wxDataFormatId
{
    wxDF_INVALID =          0,
    wxDF_TEXT =             1,  /* CF_TEXT */
    wxDF_BITMAP =           2,  /* CF_BITMAP */
    wxDF_METAFILE =         3,  /* CF_METAFILEPICT */
    wxDF_SYLK =             4,
    wxDF_DIF =              5,
    wxDF_TIFF =             6,
    wxDF_OEMTEXT =          7,  /* CF_OEMTEXT */
    wxDF_DIB =              8,  /* CF_DIB */
    wxDF_PALETTE =          9,
    wxDF_PENDATA =          10,
    wxDF_RIFF =             11,
    wxDF_WAVE =             12,
    wxDF_UNICODETEXT =      13,
    wxDF_ENHMETAFILE =      14,
    wxDF_FILENAME =         15, /* CF_HDROP */
    wxDF_LOCALE =           16,
    wxDF_PRIVATE =          20,
    wxDF_HTML =             30, /* Note: does not correspond to CF_ constant */
    wxDF_MAX
};

// Virtual keycodes
enum wxKeyCode
{
    WXK_BACK    =    8,
    WXK_TAB     =    9,
    WXK_RETURN  =    13,
    WXK_ESCAPE  =    27,
    WXK_SPACE   =    32,
    WXK_DELETE  =    127,

    WXK_START   = 300,
    WXK_LBUTTON,
    WXK_RBUTTON,
    WXK_CANCEL,
    WXK_MBUTTON,
    WXK_CLEAR,
    WXK_SHIFT,
    WXK_ALT,
    WXK_CONTROL,
    WXK_MENU,
    WXK_PAUSE,
    WXK_CAPITAL,
    WXK_PRIOR,  // Page up
    WXK_NEXT,   // Page down
    WXK_END,
    WXK_HOME,
    WXK_LEFT,
    WXK_UP,
    WXK_RIGHT,
    WXK_DOWN,
    WXK_SELECT,
    WXK_PRINT,
    WXK_EXECUTE,
    WXK_SNAPSHOT,
    WXK_INSERT,
    WXK_HELP,
    WXK_NUMPAD0,
    WXK_NUMPAD1,
    WXK_NUMPAD2,
    WXK_NUMPAD3,
    WXK_NUMPAD4,
    WXK_NUMPAD5,
    WXK_NUMPAD6,
    WXK_NUMPAD7,
    WXK_NUMPAD8,
    WXK_NUMPAD9,
    WXK_MULTIPLY,
    WXK_ADD,
    WXK_SEPARATOR,
    WXK_SUBTRACT,
    WXK_DECIMAL,
    WXK_DIVIDE,
    WXK_F1,
    WXK_F2,
    WXK_F3,
    WXK_F4,
    WXK_F5,
    WXK_F6,
    WXK_F7,
    WXK_F8,
    WXK_F9,
    WXK_F10,
    WXK_F11,
    WXK_F12,
    WXK_F13,
    WXK_F14,
    WXK_F15,
    WXK_F16,
    WXK_F17,
    WXK_F18,
    WXK_F19,
    WXK_F20,
    WXK_F21,
    WXK_F22,
    WXK_F23,
    WXK_F24,
    WXK_NUMLOCK,
    WXK_SCROLL,
    WXK_PAGEUP,
    WXK_PAGEDOWN,

    WXK_NUMPAD_SPACE,
    WXK_NUMPAD_TAB,
    WXK_NUMPAD_ENTER,
    WXK_NUMPAD_F1,
    WXK_NUMPAD_F2,
    WXK_NUMPAD_F3,
    WXK_NUMPAD_F4,
    WXK_NUMPAD_HOME,
    WXK_NUMPAD_LEFT,
    WXK_NUMPAD_UP,
    WXK_NUMPAD_RIGHT,
    WXK_NUMPAD_DOWN,
    WXK_NUMPAD_PRIOR,
    WXK_NUMPAD_PAGEUP,
    WXK_NUMPAD_NEXT,
    WXK_NUMPAD_PAGEDOWN,
    WXK_NUMPAD_END,
    WXK_NUMPAD_BEGIN,
    WXK_NUMPAD_INSERT,
    WXK_NUMPAD_DELETE,
    WXK_NUMPAD_EQUAL,
    WXK_NUMPAD_MULTIPLY,
    WXK_NUMPAD_ADD,
    WXK_NUMPAD_SEPARATOR,
    WXK_NUMPAD_SUBTRACT,
    WXK_NUMPAD_DECIMAL,
    WXK_NUMPAD_DIVIDE,

    WXK_WINDOWS_LEFT,
    WXK_WINDOWS_RIGHT,
    WXK_WINDOWS_MENU
};

// Mapping modes (same values as used by Windows, don't change)
enum
{
    wxMM_TEXT = 1,
    wxMM_LOMETRIC,
    wxMM_HIMETRIC,
    wxMM_LOENGLISH,
    wxMM_HIENGLISH,
    wxMM_TWIPS,
    wxMM_ISOTROPIC,
    wxMM_ANISOTROPIC,
    wxMM_POINTS,
    wxMM_METRIC
};

/* Shortcut for easier dialog-unit-to-pixel conversion */
#define wxDLG_UNIT(parent, pt) parent->ConvertDialogToPixels(pt)

/* Paper types */
typedef enum
{
    wxPAPER_NONE,               // Use specific dimensions
    wxPAPER_LETTER,             // Letter, 8 1/2 by 11 inches
    wxPAPER_LEGAL,              // Legal, 8 1/2 by 14 inches
    wxPAPER_A4,                 // A4 Sheet, 210 by 297 millimeters
    wxPAPER_CSHEET,             // C Sheet, 17 by 22 inches
    wxPAPER_DSHEET,             // D Sheet, 22 by 34 inches
    wxPAPER_ESHEET,             // E Sheet, 34 by 44 inches
    wxPAPER_LETTERSMALL,        // Letter Small, 8 1/2 by 11 inches
    wxPAPER_TABLOID,            // Tabloid, 11 by 17 inches
    wxPAPER_LEDGER,             // Ledger, 17 by 11 inches
    wxPAPER_STATEMENT,          // Statement, 5 1/2 by 8 1/2 inches
    wxPAPER_EXECUTIVE,          // Executive, 7 1/4 by 10 1/2 inches
    wxPAPER_A3,                 // A3 sheet, 297 by 420 millimeters
    wxPAPER_A4SMALL,            // A4 small sheet, 210 by 297 millimeters
    wxPAPER_A5,                 // A5 sheet, 148 by 210 millimeters
    wxPAPER_B4,                 // B4 sheet, 250 by 354 millimeters
    wxPAPER_B5,                 // B5 sheet, 182-by-257-millimeter paper
    wxPAPER_FOLIO,              // Folio, 8-1/2-by-13-inch paper
    wxPAPER_QUARTO,             // Quarto, 215-by-275-millimeter paper
    wxPAPER_10X14,              // 10-by-14-inch sheet
    wxPAPER_11X17,              // 11-by-17-inch sheet
    wxPAPER_NOTE,               // Note, 8 1/2 by 11 inches
    wxPAPER_ENV_9,              // #9 Envelope, 3 7/8 by 8 7/8 inches
    wxPAPER_ENV_10,             // #10 Envelope, 4 1/8 by 9 1/2 inches
    wxPAPER_ENV_11,             // #11 Envelope, 4 1/2 by 10 3/8 inches
    wxPAPER_ENV_12,             // #12 Envelope, 4 3/4 by 11 inches
    wxPAPER_ENV_14,             // #14 Envelope, 5 by 11 1/2 inches
    wxPAPER_ENV_DL,             // DL Envelope, 110 by 220 millimeters
    wxPAPER_ENV_C5,             // C5 Envelope, 162 by 229 millimeters
    wxPAPER_ENV_C3,             // C3 Envelope, 324 by 458 millimeters
    wxPAPER_ENV_C4,             // C4 Envelope, 229 by 324 millimeters
    wxPAPER_ENV_C6,             // C6 Envelope, 114 by 162 millimeters
    wxPAPER_ENV_C65,            // C65 Envelope, 114 by 229 millimeters
    wxPAPER_ENV_B4,             // B4 Envelope, 250 by 353 millimeters
    wxPAPER_ENV_B5,             // B5 Envelope, 176 by 250 millimeters
    wxPAPER_ENV_B6,             // B6 Envelope, 176 by 125 millimeters
    wxPAPER_ENV_ITALY,          // Italy Envelope, 110 by 230 millimeters
    wxPAPER_ENV_MONARCH,        // Monarch Envelope, 3 7/8 by 7 1/2 inches
    wxPAPER_ENV_PERSONAL,       // 6 3/4 Envelope, 3 5/8 by 6 1/2 inches
    wxPAPER_FANFOLD_US,         // US Std Fanfold, 14 7/8 by 11 inches
    wxPAPER_FANFOLD_STD_GERMAN, // German Std Fanfold, 8 1/2 by 12 inches
    wxPAPER_FANFOLD_LGL_GERMAN, // German Legal Fanfold, 8 1/2 by 13 inches

    wxPAPER_ISO_B4,             // B4 (ISO) 250 x 353 mm
    wxPAPER_JAPANESE_POSTCARD,  // Japanese Postcard 100 x 148 mm
    wxPAPER_9X11,               // 9 x 11 in
    wxPAPER_10X11,              // 10 x 11 in
    wxPAPER_15X11,              // 15 x 11 in
    wxPAPER_ENV_INVITE,         // Envelope Invite 220 x 220 mm
    wxPAPER_LETTER_EXTRA,       // Letter Extra 9 \275 x 12 in
    wxPAPER_LEGAL_EXTRA,        // Legal Extra 9 \275 x 15 in
    wxPAPER_TABLOID_EXTRA,      // Tabloid Extra 11.69 x 18 in
    wxPAPER_A4_EXTRA,           // A4 Extra 9.27 x 12.69 in
    wxPAPER_LETTER_TRANSVERSE,  // Letter Transverse 8 \275 x 11 in
    wxPAPER_A4_TRANSVERSE,      // A4 Transverse 210 x 297 mm
    wxPAPER_LETTER_EXTRA_TRANSVERSE, // Letter Extra Transverse 9\275 x 12 in
    wxPAPER_A_PLUS,             // SuperA/SuperA/A4 227 x 356 mm
    wxPAPER_B_PLUS,             // SuperB/SuperB/A3 305 x 487 mm
    wxPAPER_LETTER_PLUS,        // Letter Plus 8.5 x 12.69 in
    wxPAPER_A4_PLUS,            // A4 Plus 210 x 330 mm
    wxPAPER_A5_TRANSVERSE,      // A5 Transverse 148 x 210 mm
    wxPAPER_B5_TRANSVERSE,      // B5 (JIS) Transverse 182 x 257 mm
    wxPAPER_A3_EXTRA,           // A3 Extra 322 x 445 mm
    wxPAPER_A5_EXTRA,           // A5 Extra 174 x 235 mm
    wxPAPER_B5_EXTRA,           // B5 (ISO) Extra 201 x 276 mm
    wxPAPER_A2,                 // A2 420 x 594 mm
    wxPAPER_A3_TRANSVERSE,      // A3 Transverse 297 x 420 mm
    wxPAPER_A3_EXTRA_TRANSVERSE // A3 Extra Transverse 322 x 445 mm

} wxPaperSize;

/* Printing orientation */
#ifndef wxPORTRAIT
#define wxPORTRAIT      1
#define wxLANDSCAPE     2
#endif

/* Duplex printing modes
 */

enum wxDuplexMode
{
    wxDUPLEX_SIMPLEX, // Non-duplex
    wxDUPLEX_HORIZONTAL,
    wxDUPLEX_VERTICAL
};

/* Print quality.
 */

#define wxPRINT_QUALITY_HIGH    -1
#define wxPRINT_QUALITY_MEDIUM  -2
#define wxPRINT_QUALITY_LOW     -3
#define wxPRINT_QUALITY_DRAFT   -4

typedef int wxPrintQuality;

/* Print mode (currently PostScript only)
 */

enum wxPrintMode
{
    wxPRINT_MODE_NONE =    0,
    wxPRINT_MODE_PREVIEW = 1,   // Preview in external application
    wxPRINT_MODE_FILE =    2,   // Print to file
    wxPRINT_MODE_PRINTER = 3    // Send to printer
};

// ----------------------------------------------------------------------------
// miscellaneous
// ----------------------------------------------------------------------------

// define this macro if font handling is done using the X font names
#if (defined(__WXGTK__) && !defined(__WXGTK20__)) || defined(__X__)
    #define _WX_X_FONTLIKE
#endif

// macro to specify "All Files" on different platforms
#if defined(__WXMSW__) || defined(__WXPM__)
#   define wxALL_FILES_PATTERN   wxT("*.*")
#   define wxALL_FILES           gettext_noop("All files (*.*)|*.*")
#else
#   define wxALL_FILES_PATTERN   wxT("*")
#   define wxALL_FILES           gettext_noop("All files (*)|*")
#endif

// ---------------------------------------------------------------------------
// macros that enable wxWindows apps to be compiled in absence of the
// sytem headers, although some platform specific types are used in the
// platform specific (implementation) parts of the headers
// ---------------------------------------------------------------------------

#ifdef __WXMAC__

typedef unsigned char WXCOLORREF[6];
typedef void*       WXHBITMAP;
typedef void*       WXHMETAFILE ;
typedef void*       WXHICON ;
typedef void*       WXHCURSOR ;
typedef void*       WXHRGN ;
typedef void*       WXRECTPTR ;
typedef void*       WXPOINTPTR ;
typedef void*       WXHWND ;
typedef void*       WXEVENTREF ;
typedef void*		WXEVENTHANDLERREF ;
typedef void*       WXEVENTHANDLERCALLREF ; 
typedef void*       WXAPPLEEVENTREF ;
typedef void*       WXHDC ;
typedef void*       WXHMENU ;
typedef unsigned int    WXUINT;
typedef unsigned long   WXDWORD;
typedef unsigned short  WXWORD;

typedef void*       WXWidget ;
typedef void*       WXWindow ;
typedef void*       WXDisplay ;
/*
typedef WindowPtr       WXHWND;
typedef Handle          WXHANDLE;
typedef CIconHandle     WXHICON;
//typedef unsigned long   WXHFONT;
typedef MenuHandle      WXHMENU;
//typedef unsigned long   WXHPEN;
//typedef unsigned long   WXHBRUSH;
//typedef unsigned long   WXHPALETTE;
typedef CursHandle      WXHCURSOR;
typedef RgnHandle       WXHRGN;
//typedef unsigned long   WXHACCEL;
//typedef unsigned long   WXHINSTANCE;
//typedef unsigned long   WXHIMAGELIST;
//typedef unsigned long   WXHGLOBAL;
typedef GrafPtr         WXHDC;
//typedef unsigned int    WXWPARAM;
//typedef long            WXLPARAM;
//typedef void *          WXRGNDATA;
//typedef void *          WXMSG;
//typedef unsigned long   WXHCONV;
//typedef unsigned long   WXHKEY;
//typedef void *          WXDRAWITEMSTRUCT;
//typedef void *          WXMEASUREITEMSTRUCT;
//typedef void *          WXLPCREATESTRUCT;
typedef int (*WXFARPROC)();

typedef WindowPtr       WXWindow;
typedef ControlHandle   WXWidget;
*/
#endif

#ifdef __WXCOCOA__

#if defined(__OBJC__)
    #include <objc/objc.h>
    #define DECLARE_WXCOCOA_OBJC_CLASS(klass) \
    @class klass;                   typedef klass *WX_##klass
#elif defined(wxI_LIKE_OBJC_ID)
    #define DECLARE_WXCOCOA_OBJC_CLASS(klass) \
    typedef id WX_##klass
#else // the goal is to get rid of this secion at some time!
    #define DECLARE_WXCOCOA_OBJC_CLASS(klass) \
    typedef void *WX_##klass
#endif

DECLARE_WXCOCOA_OBJC_CLASS(NSApplication);
DECLARE_WXCOCOA_OBJC_CLASS(NSBox);
DECLARE_WXCOCOA_OBJC_CLASS(NSButton);
DECLARE_WXCOCOA_OBJC_CLASS(NSControl);
DECLARE_WXCOCOA_OBJC_CLASS(NSMenu);
DECLARE_WXCOCOA_OBJC_CLASS(NSMenuItem);
DECLARE_WXCOCOA_OBJC_CLASS(NSPanel);
DECLARE_WXCOCOA_OBJC_CLASS(NSTableView);
DECLARE_WXCOCOA_OBJC_CLASS(NSTextField);
DECLARE_WXCOCOA_OBJC_CLASS(NSWindow);
DECLARE_WXCOCOA_OBJC_CLASS(NSView);
typedef WX_NSView WXWidget; // wxWindows BASE definition
#endif // __WXCOCOA__

#if defined(__WXMSW__) || defined(__WXPM__)

// the keywords needed for WinMain() declaration
#ifdef __WIN16__
#  ifdef __VISUALC__
#    define WXFAR __far
#  else
#    define WXFAR _far
#  endif
#else  // Win32
#  ifndef WXFAR
#    define WXFAR
#  endif
#endif // Win16/32

// Stand-ins for Windows types or OS/2, to avoid #including all of windows.h or os2.h
typedef unsigned long   WXHWND;
typedef unsigned long   WXHANDLE;
typedef unsigned long   WXHICON;
typedef unsigned long   WXHFONT;
typedef unsigned long   WXHMENU;
typedef unsigned long   WXHPEN;
typedef unsigned long   WXHBRUSH;
typedef unsigned long   WXHPALETTE;
typedef unsigned long   WXHCURSOR;
typedef unsigned long   WXHRGN;
typedef unsigned long   WXHACCEL;
typedef void WXFAR  *   WXHINSTANCE;
typedef unsigned long   WXHBITMAP;
typedef unsigned long   WXHIMAGELIST;
typedef unsigned long   WXHGLOBAL;
typedef unsigned long   WXHDC;
typedef unsigned int    WXUINT;
typedef unsigned long   WXDWORD;
typedef unsigned short  WXWORD;

typedef unsigned long   WXCOLORREF;
typedef void *          WXRGNDATA;
typedef void *          WXMSG;
typedef unsigned long   WXHCONV;
typedef unsigned long   WXHKEY;
typedef unsigned long   WXHTREEITEM;

typedef void *          WXDRAWITEMSTRUCT;
typedef void *          WXMEASUREITEMSTRUCT;
typedef void *          WXLPCREATESTRUCT;

typedef WXHWND          WXWidget;

#endif // MSW or OS2


#ifdef __WXMSW__
typedef unsigned int    WXWPARAM;
typedef long            WXLPARAM;

#if !defined(__WIN32__) || defined(__GNUWIN32__) || defined(__WXMICROWIN__)
typedef int             (*WXFARPROC)();
#else
typedef int             (__stdcall *WXFARPROC)();
#endif
#endif // __WXMSW__


#if defined(__WXPM__)
#ifdef __EMX__
/* Need a well-known type for WXFARPROC
   below. MPARAM is typedef'ed too late. */
#define WXWPARAM        void *
#define WXLPARAM        void *
#else
#define WXWPARAM        MPARAM
#define WXLPARAM        MPARAM
#endif
#define RECT            RECTL
#define LOGFONT         FATTRS
#define LOWORD          SHORT1FROMMP
#define HIWORD          SHORT2FROMMP

typedef unsigned long   WXMPARAM;
typedef unsigned long   WXMSGID;
typedef void*           WXRESULT;
//typedef int             (*WXFARPROC)();
// some windows handles not defined by PM
typedef unsigned long   HANDLE;
typedef unsigned long   HICON;
typedef unsigned long   HFONT;
typedef unsigned long   HMENU;
typedef unsigned long   HPEN;
typedef unsigned long   HBRUSH;
typedef unsigned long   HPALETTE;
typedef unsigned long   HCURSOR;
typedef unsigned long   HINSTANCE;
typedef unsigned long   HIMAGELIST;
typedef unsigned long   HGLOBAL;
typedef unsigned long   DWORD;
typedef unsigned short  WORD;

// WIN32 graphics types for OS/2 GPI

// RGB under OS2 is more like a PALETTEENTRY struct under Windows so we need a real RGB def
// WARNING: The OS/2 headers typedef BYTE simply as 'char'; if the default is signed, all
// hell will break loose!
//#define OS2RGB(r,g,b) ((DWORD ((BYTE) (b) | ((WORD) (g) << 8)) | (((DWORD)(BYTE)(r)) << 16)))
#define OS2RGB(r,g,b) ((DWORD)((unsigned char)(b) | ((unsigned char)(g) << 8)) | ((unsigned char)(r) << 16))

typedef unsigned long COLORREF;
#define GetBValue(rgb) ((BYTE)((rgb) >> 16))
#define GetGValue(rgb) ((BYTE)(((WORD)(rgb)) >> 8))
#define GetRValue(rgb) ((BYTE)(rgb))
#define PALETTEINDEX(i) ((COLORREF)(0x01000000 | (DWORD)(WORD)(i)))
#define PALETTERGB(r,g,b) (0x02000000 | OS2RGB(r,g,b))
// OS2's RGB/RGB2 is backwards from this
typedef struct tagPALETTEENTRY
{
    char bRed;
    char bGreen;
    char bBlue;
    char bFlags;
} PALETTEENTRY;
typedef struct tagLOGPALETTE
{
    WORD palVersion;
    WORD palNumentries;
    WORD PALETTEENTRY[1];
} LOGPALETTE;

#if (defined(__VISAGECPP__) && (__IBMCPP__ < 400)) || defined (__WATCOMC__)
    // VA 3.0 for some reason needs base data types when typedefing a proc proto???
typedef void* (_System *WXFARPROC)(unsigned long, unsigned long, void*, void*);
#else
#ifdef __EMX__
#define _System
#endif
typedef WXRESULT (_System *WXFARPROC)(WXHWND, WXMSGID, WXWPARAM, WXLPARAM);
#endif

#endif //__WXPM__


#if defined(__WXMOTIF__) || defined(__WXX11__)
/* Stand-ins for X/Xt/Motif types */
typedef void*           WXWindow;
typedef void*           WXWidget;
typedef void*           WXAppContext;
typedef void*           WXColormap;
typedef void*           WXColor;
typedef void            WXDisplay;
typedef void            WXEvent;
typedef void*           WXCursor;
typedef void*           WXPixmap;
typedef void*           WXFontStructPtr;
typedef void*           WXGC;
typedef void*           WXRegion;
typedef void*           WXFont;
typedef void*           WXImage;
typedef void*           WXFontList;
typedef void*           WXRendition;
typedef void*           WXRenderTable;
typedef void*           WXFontType; /* either a XmFontList or XmRenderTable */
typedef void*           WXString;

typedef unsigned long   Atom;  /* this might fail on a few architectures */

#endif // Motif

#ifdef __WXGTK__

/* Stand-ins for GLIB types */
typedef char           gchar;
typedef signed char    gint8;
typedef int            gint;
typedef unsigned       guint;
typedef unsigned long  gulong;
typedef void*          gpointer;
typedef struct _GSList GSList;

/* Stand-ins for GDK types */
typedef struct _GdkColor        GdkColor;
typedef struct _GdkColormap     GdkColormap;
typedef struct _GdkFont         GdkFont;
typedef struct _GdkGC           GdkGC;
typedef struct _GdkVisual       GdkVisual;

#ifdef __WXGTK20__
typedef struct _GdkAtom        *GdkAtom;
typedef struct _GdkDrawable     GdkWindow;
typedef struct _GdkDrawable     GdkBitmap;
typedef struct _GdkDrawable     GdkPixmap;
#else // GTK+ 1.2
typedef gulong                  GdkAtom;
typedef struct _GdkWindow       GdkWindow;
typedef struct _GdkWindow       GdkBitmap;
typedef struct _GdkWindow       GdkPixmap;
#endif // GTK+ 1.2/2.0

typedef struct _GdkCursor       GdkCursor;
typedef struct _GdkRegion       GdkRegion;
typedef struct _GdkDragContext  GdkDragContext;

#ifdef HAVE_XIM
typedef struct _GdkIC           GdkIC;
typedef struct _GdkICAttr       GdkICAttr;
#endif

/* Stand-ins for GTK types */
typedef struct _GtkWidget         GtkWidget;
typedef struct _GtkStyle          GtkStyle;
typedef struct _GtkAdjustment     GtkAdjustment;
typedef struct _GtkList           GtkList;
typedef struct _GtkToolbar        GtkToolbar;
typedef struct _GtkTooltips       GtkTooltips;
typedef struct _GtkNotebook       GtkNotebook;
typedef struct _GtkNotebookPage   GtkNotebookPage;
typedef struct _GtkAccelGroup     GtkAccelGroup;
typedef struct _GtkItemFactory    GtkItemFactory;
typedef struct _GtkSelectionData  GtkSelectionData;

typedef GtkWidget *WXWidget;

#ifndef __WXGTK20__
#define GTK_OBJECT_GET_CLASS(object) (GTK_OBJECT(object)->klass)
#define GTK_CLASS_TYPE(klass) ((klass)->type)
#endif

#ifdef __WXGTK20__
/* Input method thing */
typedef struct _GtkIMMulticontext    GtkIMMulticontext;
#endif // __WXGTK20__

#endif // __WXGTK__

#if defined(__WXGTK20__) || (defined(__WXX11__) && wxUSE_UNICODE)
#define wxUSE_PANGO 1
#else
#define wxUSE_PANGO 0
#endif

#if wxUSE_PANGO
/* Stand-ins for Pango types */
typedef struct _PangoContext         PangoContext;
typedef struct _PangoLayout          PangoLayout;
typedef struct _PangoFontDescription PangoFontDescription;
#endif

#ifdef __WXMGL__
typedef struct window_t *WXWidget;
#endif // MGL

// This is required because of clashing macros in windows.h, which may be
// included before or after wxWindows classes, and therefore must be
// disabled here before any significant wxWindows headers are included.
#ifdef __WXMSW__
#ifdef GetClassInfo
#undef GetClassInfo
#endif

#ifdef GetClassName
#undef GetClassName
#endif

#ifdef DrawText
#undef DrawText
#endif

#ifdef GetCharWidth
#undef GetCharWidth
#endif

#ifdef StartDoc
#undef StartDoc
#endif

#ifdef FindWindow
#undef FindWindow
#endif

#ifdef FindResource
#undef FindResource
#endif
#endif
  // __WXMSW__

// ---------------------------------------------------------------------------
// macro to define a class without copy ctor nor assignment operator
// ---------------------------------------------------------------------------

#define DECLARE_NO_COPY_CLASS(classname)        \
    private:                                    \
        classname(const classname&);            \
        classname& operator=(const classname&);

#endif
    // _WX_DEFS_H_
