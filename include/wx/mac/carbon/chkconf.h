/*
 * Name:        wx/mac/chkconf.h
 * Purpose:     Compiler-specific configuration checking
 * Author:      Julian Smart
 * Modified by:
 * Created:     01/02/97
 * RCS-ID:      $Id$
 * Copyright:   (c) Julian Smart
 * Licence:     wxWindows licence
 */

#ifndef _WX_MAC_CHKCONF_H_
#define _WX_MAC_CHKCONF_H_

/*
 * disable the settings which don't work for some compilers
 */

/*
 * wxUSE_DEBUG_NEW_ALWAYS doesn't work with CodeWarrior
 */

#if defined(__MWERKS__)
    #undef wxUSE_DEBUG_NEW_ALWAYS
    #define wxUSE_DEBUG_NEW_ALWAYS      0
#endif

#endif
    /* _WX_MAC_CHKCONF_H_ */

