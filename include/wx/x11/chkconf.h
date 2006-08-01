/*
 * Name:        wx/x11/chkconf.h
 * Purpose:     Compiler-specific configuration checking
 * Author:      Julian Smart
 * Modified by:
 * Created:     01/02/97
 * RCS-ID:      $Id$
 * Copyright:   (c) Julian Smart
 * Licence:     wxWindows licence
 */

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#ifndef _WX_X11_CHKCONF_H_
#define _WX_X11_CHKCONF_H_


/* wxPalette is always needed */
#if !wxUSE_PALETTE
#   error "wxX11 requires wxUSE_PALETTE=1"
#endif

#endif /* _WX_X11_CHKCONF_H_ */
