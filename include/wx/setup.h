/*
 * The main configuration file for wxWindows.
 *
 * NB: this file can be included in .c files, so it must be compileable by a C
 *     compiler - use #ifdef __cplusplus for C++ specific features and avoid
 *     using C++ style comments
 */

#ifndef _WX_SETUP_H_BASE_
#define _WX_SETUP_H_BASE_

/* compatibility code, to be removed asap: */

#if !defined(__WXMSW__) && !defined(__WXGTK__) && !defined(__WXMOTIF__) && !defined(__WXQT__) && !defined(__WXSTUBS__) && !defined(__WXMAC__) && !defined(__WXPM__)
#error No __WXxxx__ define set! Please define one of __WXGTK__,__WXMSW__,__WXMOTIF__,__WXMAC__,__WXQT__,__WXPM__,__WXSTUBS__
#endif

#ifdef __VMS
#include "[--]setup.h"
#elif
#if defined(__WXMSW__)
#include "wx/msw/setup.h"
#elif defined(__WXMAC__)
#include "wx/mac/setup.h"
#elif defined(__WXQT__)
#include "wx/qt/setup.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/setup.h"
#elif defined(__WXPM__)
#include "wx/os2/setup.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/setup.h"
#elif defined(__WXGTK__)
#include "wx/gtk/setup.h"
#endif
#endif

/*
 * Compatibility defines: note that, in general, there is a a reason for not
 * compatible changes, so you should try to avoid defining WXWIN_COMPATIBILITY
 * and do so only if your program really can't be compiled otherwise.
 */

/*
 * Compatibility with 1.66 API.
 * Level 0: no backward compatibility, all new features
 * Level 1: wxDC, OnSize (etc.) compatibility, but
 * some new features such as event tables
 */
#define WXWIN_COMPATIBILITY 0

/*
 * wxWindows 2.0 API compatibility. Possible values are:
 * 0:   no backwards compatibility
 * 1:   some backwards compatibility, but if it conflicts with the new
 *      features, use the new code, not the old one
 * 2:   maximum backwards compatiblity: even if compatibility can only be
 *      achieved by disabling new features, do it.
 */
#define WXWIN_COMPATIBILITY_2 1

/*
 * wxWindows 2.0 uses long for wxPoint/wxRect/wxSize member fields and wxDC
 * method arguments, wxWindows 2.1 and later uses wxCoord typedef which is
 * usually int. Using long leads to (justified) warnings about long to int
 * conversions from some compilers and is, generally speaking, unneeded.
 */
#define wxUSE_COMPATIBLE_COORD_TYPES 0

/*
 * Maximum compatibility with 2.0 API
 */
#if WXWIN_COMPATIBILITY_2 == 2
#undef wxUSE_COMPATIBLE_COORD_TYPES
#define wxUSE_COMPATIBLE_COORD_TYPES 1
#endif /* WXWIN_COMPATIBILITY_2_MAX */

#endif
	/* _WX_SETUP_H_BASE_ */
