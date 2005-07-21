/**
*  Name:        wx/debug.h
*  Purpose:     Misc debug functions and macros
*  Author:      Vadim Zeitlin
*  Modified by: Ryan Norton (Converted to C)
*  Created:     29/01/98
*  RCS-ID:      $Id$
*  Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
*  Licence:     wxWindows licence
*/

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#ifndef   _WX_DEBUG_H_
#define   _WX_DEBUG_H_

#ifndef __WXWINCE__
#include  <assert.h>
#endif
#include  <limits.h>            /*  for CHAR_BIT used below */

#include  "wx/wxchar.h"         /*  for __TFILE__ and wxChar */

/*  ---------------------------------------------------------------------------- */
/*  Defines controlling the debugging macros */
/*  ---------------------------------------------------------------------------- */

/*  if _DEBUG is defined (MS VC++ and others use it in debug builds), define */
/*  __WXDEBUG__ too */
#ifdef _DEBUG
    #ifndef __WXDEBUG__
        #define __WXDEBUG__
    #endif /*  !__WXDEBUG__ */
#endif /*  _DEBUG */

/*  if NDEBUG is defined (<assert.h> uses it), undef __WXDEBUG__ and WXDEBUG */
#ifdef NDEBUG
    #undef __WXDEBUG__
    #undef WXDEBUG
#endif /*  NDEBUG */

/*  if __WXDEBUG__ is defined, make sure that WXDEBUG is defined and >= 1 */
#ifdef __WXDEBUG__
    #if !defined(WXDEBUG) || !WXDEBUG
        #undef WXDEBUG
        #define WXDEBUG 1
    #endif /*  !WXDEBUG */
#endif /*  __WXDEBUG__ */

/*  ---------------------------------------------------------------------------- */
/*  Debugging macros */
/*  */
/*  All debugging macros rely on ASSERT() which in turn calls user-defined */
/*  OnAssert() function. To keep things simple, it's called even when the */
/*  expression is true (i.e. everything is ok) and by default does nothing: just */
/*  returns the same value back. But if you redefine it to do something more sexy */
/*  (popping up a message box in your favourite GUI, sending you e-mail or */
/*  whatever) it will affect all ASSERTs, FAILs and CHECKs in your code. */
/*  */
/*  Warning: if you don't like advice on programming style, don't read */
/*  further! ;-) */
/*  */
/*  Extensive use of these macros is recommended! Remember that ASSERTs are */
/*  disabled in final build (without __WXDEBUG__ defined), so they add strictly */
/*  nothing to your program's code. On the other hand, CHECK macros do stay */
/*  even in release builds, but in general are not much of a burden, while */
/*  a judicious use of them might increase your program's stability. */
/*  ---------------------------------------------------------------------------- */

/*  Macros which are completely disabled in 'release' mode */
/*  */
/*  NB: these functions are implemented in src/common/appcmn.cpp */
#if defined(__cplusplus) && defined(__WXDEBUG__)
  /*
    this function may be redefined to do something non trivial and is called
    whenever one of debugging macros fails (i.e. condition is false in an
    assertion)

    parameters:
       szFile and nLine - file name and line number of the ASSERT
       szMsg            - optional message explaining the reason
  */
  extern void WXDLLIMPEXP_BASE wxOnAssert(const wxChar *szFile,
                                          int nLine,
                                          const wxChar *szCond,
                                          const wxChar *szMsg = NULL);

  /*  call this function to break into the debugger unconditionally (assuming */
  /*  the program is running under debugger, of course) */
  extern void WXDLLIMPEXP_BASE wxTrap();

  /*  helper function used to implement wxASSERT and wxASSERT_MSG */
  /*  */
  /*  note using "int" and not "bool" for cond to avoid VC++ warnings about */
  /*  implicit conversions when doing "wxAssert( pointer )" and also use of */
  /*  "!!cond" below to ensure that everything is converted to int */
  extern void WXDLLIMPEXP_BASE wxAssert(int cond,
                                        const wxChar *szFile,
                                        int nLine,
                                        const wxChar *szCond,
                                        const wxChar *szMsg = NULL) ;

  /*  generic assert macro */
  #define wxASSERT(cond) wxAssert(!!(cond), __TFILE__, __LINE__, _T(#cond))

  /*  assert with additional message explaining it's cause */
  #define wxASSERT_MSG(cond, msg) \
    wxAssert(!!(cond), __TFILE__, __LINE__, _T(#cond), msg)

  /*  an assert helper used to avoid warning when testing constant expressions, */
  /*  i.e. wxASSERT( sizeof(int) == 4 ) can generate a compiler warning about */
  /*  expression being always true, but not using */
  /*  wxASSERT( wxAssertIsEqual(sizeof(int), 4) ) */
  /*  */
  /*  NB: this is made obsolete by wxCOMPILE_TIME_ASSERT() and shouldn't be */
  /*      used any longer */
  extern bool WXDLLIMPEXP_BASE wxAssertIsEqual(int x, int y);
#else
  #define wxTrap()

  /*  nothing to do in release modes (hopefully at this moment there are */
  /*  no more bugs ;-) */
  #define wxASSERT(cond)
  #define wxASSERT_MSG(x, m)
#endif  /* __WXDEBUG__ */

#ifdef __cplusplus
    /*  Use of wxFalse instead of false suppresses compiler warnings about testing */
    /*  constant expression */
    extern WXDLLIMPEXP_DATA_BASE(const bool) wxFalse;
#endif

#define wxAssertFailure wxFalse

/*  special form of assert: always triggers it (in debug mode) */
#define wxFAIL                 wxASSERT(wxAssertFailure)

/*  FAIL with some message */
#define wxFAIL_MSG(msg)        wxASSERT_MSG(wxAssertFailure, msg)

/*  NB: the following macros work also in release mode! */

/*
  These macros must be used only in invalid situation: for example, an
  invalid parameter (NULL pointer) is passed to a function. Instead of
  dereferencing it and causing core dump the function might try using
  CHECK( p != NULL ) or CHECK( p != NULL, return LogError("p is NULL!!") )
*/

/*  check that expression is true, "return" if not (also FAILs in debug mode) */
#define wxCHECK(x, rc)            if (!(x)) {wxFAIL; return rc; }

/*  as wxCHECK but with a message explaining why we fail */
#define wxCHECK_MSG(x, rc, msg)   if (!(x)) {wxFAIL_MSG(msg); return rc; }

/*  check that expression is true, perform op if not */
#define wxCHECK2(x, op)           if (!(x)) {wxFAIL; op; }

/*  as wxCHECK2 but with a message explaining why we fail */
#define wxCHECK2_MSG(x, op, msg)  if (!(x)) {wxFAIL_MSG(msg); op; }

/*  special form of wxCHECK2: as wxCHECK, but for use in void functions */
/*  */
/*  NB: there is only one form (with msg parameter) and it's intentional: */
/*      there is no other way to tell the caller what exactly went wrong */
/*      from the void function (of course, the function shouldn't be void */
/*      to begin with...) */
#define wxCHECK_RET(x, msg)       if (!(x)) {wxFAIL_MSG(msg); return; }

/*  ---------------------------------------------------------------------------- */
/*  Compile time asserts */
/*  */
/*  Unlike the normal assert and related macros above which are checked during */
/*  the program tun-time the macros below will result in a compilation error if */
/*  the condition they check is false. This is usually used to check the */
/*  expressions containing sizeof()s which cannot be tested with the */
/*  preprocessor. If you can use the #if's, do use them as you can give a more */
/*  detailed error message then. */
/*  ---------------------------------------------------------------------------- */

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
#if defined(__WATCOMC__) && defined(__cplusplus)
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

/*  helpers for wxCOMPILE_TIME_ASSERT below, for private use only */
#define wxMAKE_BITSIZE_MSG(type, size) type ## SmallerThan ## size ## Bits

/*  a special case of compile time assert: check that the size of the given type */
/*  is at least the given number of bits */
#define wxASSERT_MIN_BITSIZE(type, size) \
    wxCOMPILE_TIME_ASSERT(sizeof(type) * CHAR_BIT >= size, \
                          wxMAKE_BITSIZE_MSG(type, size))

/*  ---------------------------------------------------------------------------- */
/*  other miscellaneous debugger-related functions */
/*  ---------------------------------------------------------------------------- */

/*
    Return true if we're running under debugger.

    Currently this only really works under Win32 and Mac in CodeWarrior builds,
    it always returns false in other cases.
 */
#ifdef __cplusplus
    /* ABX: check __WIN32__ instead of __WXMSW__ for the same MSWBase in any Win32 port */
    #if defined(__WXMAC__) || defined(__WIN32__)
        extern bool WXDLLIMPEXP_BASE wxIsDebuggerRunning();
    #else /*  !Mac */
        inline bool wxIsDebuggerRunning() { return false; }
    #endif /*  Mac/!Mac */
#endif /* __cplusplus */

#endif  /*  _WX_DEBUG_H_ */
