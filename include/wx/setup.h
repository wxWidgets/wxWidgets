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

#if !defined(__WXBASE__) && !defined(__WXMSW__) && !defined(__WXGTK__) && !defined(__WXMOTIF__) && !defined(__WXQT__) && !defined(__WXSTUBS__) && !defined(__WXMAC__) && !defined(__WXPM__)
#error No __WXxxx__ define set! Please define one of __WXBASE__,__WXGTK__,__WXMSW__,__WXMOTIF__,__WXMAC__,__WXQT__,__WXPM__,__WXSTUBS__
#endif

// wxUniversal is defined together with one of other ports, so test for it
// first
#ifdef __WXUNIVERSAL__
#if defined(__USE_WXCONFIG__) && defined(__WXDEBUG__)
#include "wx/univd/setup.h"
#else
#include "wx/univ/setup.h"
#endif

#elif defined(__WXBASE__)
#if defined(__USE_WXCONFIG__) && defined(__WXDEBUG__)
#include "wx/based/setup.h"
#else
#include "wx/base/setup.h"
#endif

#elif defined(__VMS)
#include "wx_root:[wxwindows]setup.h"
#elif defined(__WXMSW__)
#include "wx/msw/setup.h"
#elif defined(__WXMAC__)
#include "wx/mac/setup.h"

#elif defined(__WXQT__)
#if defined(__USE_WXCONFIG__) && defined(__WXDEBUG__)
#include "wx/qtd/setup.h"
#else
#include "wx/qt/setup.h"
#endif

#elif defined(__WXMOTIF__)
#if defined(__USE_WXCONFIG__) && defined(__WXDEBUG__)
#include "wx/motifd/setup.h"
#else
#include "wx/motif/setup.h"
#endif

#elif defined(__WXPM__)
#include "wx/os2/setup.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/setup.h"

#elif defined(__WXGTK__)
#if defined(__USE_WXCONFIG__) && defined(__WXDEBUG__)
#include "wx/gtkd/setup.h"
#else
#include "wx/gtk/setup.h"
#endif

#endif

#include "chkconf.h"

#endif /* _WX_SETUP_H_BASE_ */
