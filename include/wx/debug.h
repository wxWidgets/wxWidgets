/////////////////////////////////////////////////////////////////////////////
// Name:        wx/debug.h
// Purpose:     Misc debug functions and macros
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_DEBUG_H_
#define   _WX_DEBUG_H_

#include  <assert.h>

#include  "wx/wxchar.h"

// ----------------------------------------------------------------------------
// Debugging macros
//
// All debugging macros rely on ASSERT() which in turn calls user-defined
// OnAssert() function. To keep things simple, it's called even when the
// expression is TRUE (i.e. everything is ok) and by default does nothing: just
// returns the same value back. But if you redefine it to do something more sexy
// (popping up a message box in your favourite GUI, sending you e-mail or
// whatever) it will affect all ASSERTs, FAILs and CHECKs in your code.
//
// Warning: if you don't like advices on programming style, don't read
// further! ;-)
//
// Extensive use of these macros is recommended! Remember that ASSERTs are
// disabled in final (without __WXDEBUG__ defined) build, so they add strictly
// nothing to your program's code. On the other hand, CHECK macros do stay
// even in release builds, but in general are not much of a burden, while
// a judicious use of them might increase your program's stability.
// ----------------------------------------------------------------------------

// Use of these suppresses compiler warnings about testing constant expression
WXDLLEXPORT_DATA(extern const bool) wxTrue;
WXDLLEXPORT_DATA(extern const bool) wxFalse;

// Macros which are completely disabled in 'release' mode
#ifdef  __WXDEBUG__
  /*
    this function may be redefined to do something non trivial and is called
    whenever one of debugging macros fails (i.e. condition is false in an
    assertion)

    parameters:
       szFile and nLine - file name and line number of the ASSERT
       szMsg            - optional message explaining the reason
  */
  void WXDLLEXPORT wxOnAssert(const wxChar *szFile, int nLine, const wxChar *szMsg = (const wxChar *) NULL);

  /*
    notice the usage of else at the end of wxASSERT macro: this ensures that
    the following code

        if ( ... )
            wxASSERT(...);
        else
            ...

    works like expected: if there were no "else", the one in the code above
    would be matched with a wrong "if"
  */

  // generic assert macro
  #define wxASSERT(cond) if ( !(cond) ) wxOnAssert(__TFILE__, __LINE__); else

  // assert with additional message explaining it's cause
  #define wxASSERT_MSG(cond, msg) \
                    if ( !(cond) ) wxOnAssert(__TFILE__, __LINE__, msg); else
#else
  // nothing to do in release modes (hopefully at this moment there are
  // no more bugs ;-)
  #define wxASSERT(cond)
  #define wxASSERT_MSG(x, m)
#endif  //__WXDEBUG__

// special form of assert: always triggers it (in debug mode)
#define wxFAIL                 wxASSERT(wxFalse)

// FAIL with some message
#define wxFAIL_MSG(msg)        wxASSERT_MSG(wxFalse, msg)

// NB: the following macros work also in release mode!

/*
  These macros must be used only in invalid situation: for example, an
  invalid parameter (NULL pointer) is passed to a function. Instead of
  dereferencing it and causing core dump the function might try using
  CHECK( p != NULL ) or CHECK( p != NULL, return LogError("p is NULL!!") )
*/

// check that expression is true, "return" if not (also FAILs in debug mode)
#define wxCHECK(x, rc)            if (!(x)) {wxFAIL; return rc; }

// as wxCHECK but with a message explaining why we fail
#define wxCHECK_MSG(x, rc, msg)   if (!(x)) {wxFAIL_MSG(msg); return rc; }

// check that expression is true, perform op if not
#define wxCHECK2(x, op)           if (!(x)) {wxFAIL; op; }

// as wxCHECK2 but with a message explaining why we fail
#define wxCHECK2_MSG(x, op, msg)  if (!(x)) {wxFAIL_MSG(msg); op; }

// special form of wxCHECK2: as wxCHECK, but for use in void functions
//  NB: there is only one form (with msg parameter) and it's intentional:
//      there is no other way to tell the caller what exactly went wrong
//      from the void function (of course, the function shouldn't be void
//      to begin with...)
#define wxCHECK_RET(x, msg)       if (!(x)) {wxFAIL_MSG(msg); return; }

#endif  // _WX_DEBUG_H_

