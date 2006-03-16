/*
 * Name:        wx/os2/chkconf.h
 * Purpose:     Compiler-specific configuration checking
 * Author:      Julian Smart
 * Modified by:
 * Created:     01/02/97
 * RCS-ID:      $Id$
 * Copyright:   (c) Julian Smart
 * Licence:     wxWindows licence
 */

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#ifndef _WX_OS2_CHKCONF_H_
#define _WX_OS2_CHKCONF_H_

#ifdef __WATCOMC__

/* Watcom builds for OS/2 port are setup.h driven and setup.h is
   automatically generated from include/wx/setup_inc.h so we have
   to disable here features not supported currently or enable
   features required */

#if wxUSE_DISPLAY
#   undef wxUSE_DISPLAY
#   define wxUSE_DISPLAY 0
#endif /* wxUSE_DISPLAY */

#if wxUSE_STACKWALKER
#   undef wxUSE_STACKWALKER
#   define wxUSE_STACKWALKER 0
#endif /* wxUSE_STACKWALKER */

#if !wxUSE_POSTSCRIPT
#   undef wxUSE_POSTSCRIPT
#   define wxUSE_POSTSCRIPT 1
#endif

#endif /* __WATCOM__ */

#endif /* _WX_OS2_CHKCONF_H_ */
