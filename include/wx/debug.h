/////////////////////////////////////////////////////////////////////////////
// Name:        wx/debug.h
// Purpose:     Misc debug functions and macros
// Author:      Vadim Zeitlin
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998-2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DEBUG_H_
#define _WX_DEBUG_H_

#if !defined(__WXPALMOS5__) && !defined(__WXWINCE__)
    #include  <assert.h>
#endif // systems without assert.h

#include <limits.h>          // for CHAR_BIT used below

#include "wx/chartype.h"     // for __TFILE__ and wxChar
#include "wx/cpp.h"          // for __WXFUNCTION__

// ----------------------------------------------------------------------------
// Defines controlling the debugging macros
// ----------------------------------------------------------------------------

// if _DEBUG is defined (MS VC++ and others use it in debug builds), define
// __WXDEBUG__ too
#ifdef _DEBUG
    #ifndef __WXDEBUG__
        #define __WXDEBUG__
    #endif // !__WXDEBUG__
#endif // _DEBUG

// if NDEBUG is defined (<assert.h> uses it), undef __WXDEBUG__ and WXDEBUG
#ifdef NDEBUG
    #undef __WXDEBUG__
    #undef WXDEBUG
#endif // NDEBUG

// if __WXDEBUG__ is defined, make sure that WXDEBUG is defined and >= 1
#ifdef __WXDEBUG__
    #if !defined(WXDEBUG) || !WXDEBUG
        #undef WXDEBUG
        #define WXDEBUG 1
    #endif // !WXDEBUG
#endif // __WXDEBUG__

// ----------------------------------------------------------------------------
// Debugging macros
//
// All debugging macros rely on ASSERT() which in turn calls the user-defined
// OnAssert() function. To keep things simple, it's called even when the
// expression is true (i.e. everything is ok) and by default does nothing: just
// returns the same value back. But if you redefine it to do something more sexy
// (popping up a message box in your favourite GUI, sending you e-mail or
// whatever) it will affect all ASSERTs, FAILs and CHECKs in your code.
//
// Warning: if you don't like advice on programming style, don't read
// further! ;-)
//
// Extensive use of these macros is recommended! Remember that ASSERTs are
// disabled in final build (without __WXDEBUG__ defined), so they add strictly
// nothing to your program's code. On the other hand, CHECK macros do stay
// even in release builds, but in general are not much of a burden, while
// a judicious use of them might increase your program's stability.
// ----------------------------------------------------------------------------

// Macros which are completely disabled in 'release' mode
//
// NB: these functions are implemented in src/common/appcmn.cpp
#if defined(__WXDEBUG__)
  /*
    This function is called whenever one of debugging macros fails (i.e.
    condition is false in an assertion). To customize its behaviour, override
    wxApp::OnAssertFailure().

    Parameters:
       szFile and nLine - file name and line number of the ASSERT
       szFunc           - function name of the ASSERT, may be NULL (NB: ASCII)
       szCond           - text form of the condition which failed
       szMsg            - optional message explaining the reason
  */

  /* this version is for compatibility with wx 2.8 Unicode build only: */
  extern void WXDLLIMPEXP_BASE wxOnAssert(const wxChar *szFile,
                                          int nLine,
                                          const char *szFunc,
                                          const wxChar *szCond,
                                          const wxChar *szMsg = NULL);

#if wxUSE_UNICODE
  /* char versions are used by debugging macros; we have to provide
     wxChar* szMsg version because it's common to use _T() in the macros
     and finally, we can't use const wx(char)* szMsg = NULL, because that
     would be ambiguous: */
  extern void WXDLLIMPEXP_BASE wxOnAssert(const char *szFile,
                                          int nLine,
                                          const char *szFunc,
                                          const char *szCond);

  extern void WXDLLIMPEXP_BASE wxOnAssert(const char *szFile,
                                          int nLine,
                                          const char *szFunc,
                                          const char *szCond,
                                          const char *szMsg);

  extern void WXDLLIMPEXP_BASE wxOnAssert(const char *szFile,
                                          int nLine,
                                          const char *szFunc,
                                          const char *szCond,
                                          const wxChar *szMsg);
#endif /* wxUSE_UNICODE */

  class WXDLLIMPEXP_FWD_BASE wxString;
  class WXDLLIMPEXP_FWD_BASE wxCStrData;

  /* these two work when szMsg passed to debug macro is a string,
     we also have to provide wxCStrData overload to resolve ambiguity
     which would otherwise arise from wxASSERT( s.c_str() ): */
  extern void WXDLLIMPEXP_BASE wxOnAssert(const wxString& szFile,
                                          int nLine,
                                          const wxString& szFunc,
                                          const wxString& szCond,
                                          const wxString& szMsg);

  extern void WXDLLIMPEXP_BASE wxOnAssert(const wxString& szFile,
                                          int nLine,
                                          const wxString& szFunc,
                                          const wxString& szCond);

  extern void WXDLLIMPEXP_BASE wxOnAssert(const char *szFile,
                                          int nLine,
                                          const char *szFunc,
                                          const char *szCond,
                                          const wxCStrData& msg);

  extern void WXDLLIMPEXP_BASE wxOnAssert(const char *szFile,
                                          int nLine,
                                          const char *szFunc,
                                          const char *szCond,
                                          const wxString& szMsg);

  // call this function to break into the debugger unconditionally (assuming
  // the program is running under debugger, of course)
  extern void WXDLLIMPEXP_BASE wxTrap();

  // generic assert macro
  #define wxASSERT(cond) wxASSERT_MSG(cond, (const char*)NULL)


  // assert with additional message explaining its cause

  // Note: some compilers will give  a warning (such as          
  // "possible unwanted ;") when using a ";" instead of the "{}".
  #define wxASSERT_MSG(cond, msg)                                           \
    if ( cond )                                                             \
    {}                                                                      \
    else                                                                    \
        wxOnAssert(__FILE__, __LINE__, __WXFUNCTION__, #cond, msg)

  // special form of assert: always triggers it (in debug mode)
  #define wxFAIL wxFAIL_MSG((const char*)NULL)

  // FAIL with some message
  #define wxFAIL_MSG(msg) wxFAIL_COND_MSG("wxAssertFailure", msg)

  // FAIL with some message and a condition
  #define wxFAIL_COND_MSG(cond, msg)                                          \
      wxOnAssert(__FILE__, __LINE__,  __WXFUNCTION__, cond, msg)

  // An assert helper used to avoid warning when testing constant expressions,
  // i.e. wxASSERT( sizeof(int) == 4 ) can generate a compiler warning about
  // expression being always true, but not using
  // wxASSERT( wxAssertIsEqual(sizeof(int), 4) )
  //
  // NB: this is made obsolete by wxCOMPILE_TIME_ASSERT() and should no
  //     longer be used.
  extern bool WXDLLIMPEXP_BASE wxAssertIsEqual(int x, int y);
#else
  #define wxTrap()

  // nothing to do in release mode (hopefully at this moment there are
  // no more bugs ;-)
  #define wxASSERT(cond)
  #define wxASSERT_MSG(cond, msg)
  #define wxFAIL
  #define wxFAIL_MSG(msg)
  #define wxFAIL_COND_MSG(cond, msg)
#endif  /* __WXDEBUG__ */

// Use of wxFalse instead of false suppresses compiler warnings about testing
// constant expression
extern WXDLLIMPEXP_DATA_BASE(const bool) wxFalse;

#define wxAssertFailure wxFalse

// NB: the following macros also work in release mode!

/*
  These macros must be used only in invalid situation: for example, an
  invalid parameter (e.g. a NULL pointer) is passed to a function. Instead of
  dereferencing it and causing core dump the function might try using
  CHECK( p != NULL ) or CHECK( p != NULL, return LogError("p is NULL!!") )
*/

// check that expression is true, "return" if not (also FAILs in debug mode)
#define wxCHECK(cond, rc)            wxCHECK_MSG(cond, rc, (const char*)NULL)

// as wxCHECK but with a message explaining why we fail
#define wxCHECK_MSG(cond, rc, msg)   wxCHECK2_MSG(cond, return rc, msg)

// check that expression is true, perform op if not
#define wxCHECK2(cond, op)           wxCHECK2_MSG(cond, op, (const char*)NULL)

// as wxCHECK2 but with a message explaining why we fail

#define wxCHECK2_MSG(cond, op, msg)                                       \
    if ( cond )                                                           \
    {}                                                                    \
    else                                                                  \
    {                                                                     \
        wxFAIL_COND_MSG(#cond, msg);                                      \
        op;                                                               \
    }                                                                     \
    struct wxDummyCheckStruct /* just to force a semicolon */

// special form of wxCHECK2: as wxCHECK, but for use in void functions
//
// NB: there is only one form (with msg parameter) and it's intentional:
//     there is no other way to tell the caller what exactly went wrong
//     from the void function (of course, the function shouldn't be void
//     to begin with...)
#define wxCHECK_RET(cond, msg)       wxCHECK2_MSG(cond, return, msg)

// ----------------------------------------------------------------------------
// Compile time asserts
//
// Unlike the normal assert and related macros above which are checked during
// the program tun-time the macros below will result in a compilation error if
// the condition they check is false. This is usually used to check the
// expressions containing sizeof()s which cannot be tested with the
// preprocessor. If you can use the #if's, do use them as you can give a more
// detailed error message then.
// ----------------------------------------------------------------------------

/*
  How this works (you don't have to understand it to be able to use the
  macros): we rely on the fact that it is invalid to define a named bit field
  in a struct of width 0. All the rest are just the hacks to minimize the
  possibility of the compiler warnings when compiling this macro: in
  particular, this is why we define a struct and not an object (which would
  result in a warning about unused variable) and a named struct (otherwise we'd
  get a warning about an unnamed struct not used to define an object!).
 */

#define wxMAKE_UNIQUE_ASSERT_NAME           wxMAKE_UNIQUE_NAME(wxAssert_)

/*
  The second argument of this macro must be a valid C++ identifier and not a
  string. I.e. you should use it like this:

    wxCOMPILE_TIME_ASSERT( sizeof(int) >= 2, YourIntsAreTooSmall );

 It may be used both within a function and in the global scope.
*/
#if defined(__WATCOMC__)
    /* avoid "unused symbol" warning */
    #define wxCOMPILE_TIME_ASSERT(expr, msg) \
        class wxMAKE_UNIQUE_ASSERT_NAME { \
          unsigned int msg: expr; \
          wxMAKE_UNIQUE_ASSERT_NAME() { wxUnusedVar(msg); } \
        }
#else
    #define wxCOMPILE_TIME_ASSERT(expr, msg) \
        struct wxMAKE_UNIQUE_ASSERT_NAME { unsigned int msg: expr; }
#endif

/*
   When using VC++ 6 with "Edit and Continue" on, the compiler completely
   mishandles __LINE__ and so wxCOMPILE_TIME_ASSERT() doesn't work, provide a
   way to make "unique" assert names by specifying a unique prefix explicitly
 */
#define wxMAKE_UNIQUE_ASSERT_NAME2(text) wxCONCAT(wxAssert_, text)

#define wxCOMPILE_TIME_ASSERT2(expr, msg, text) \
    struct wxMAKE_UNIQUE_ASSERT_NAME2(text) { unsigned int msg: expr; }

// helpers for wxCOMPILE_TIME_ASSERT below, for private use only
#define wxMAKE_BITSIZE_MSG(type, size) type ## SmallerThan ## size ## Bits

// a special case of compile time assert: check that the size of the given type
// is at least the given number of bits
#define wxASSERT_MIN_BITSIZE(type, size) \
    wxCOMPILE_TIME_ASSERT(sizeof(type) * CHAR_BIT >= size, \
                          wxMAKE_BITSIZE_MSG(type, size))

// ----------------------------------------------------------------------------
// other miscellaneous debugger-related functions
// ----------------------------------------------------------------------------

/*
    Return true if we're running under debugger.

    Currently this only really works under Win32 and Mac in CodeWarrior builds,
    it always returns false in other cases.
 */
#if defined(__WXMAC__) || defined(__WIN32__)
    extern bool WXDLLIMPEXP_BASE wxIsDebuggerRunning();
#else // !Mac
    inline bool wxIsDebuggerRunning() { return false; }
#endif // Mac/!Mac

#endif // _WX_DEBUG_H_
