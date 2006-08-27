/*
 * Name:        wx/dfb/chkconf.h
 * Author:      Vaclav Slavik
 * Purpose:     Compiler-specific configuration checking
 * Created:     2006-08-10
 * RCS-ID:      $Id$
 * Copyright:   (c) 2006 REA Elektronik GmbH
 * Licence:     wxWindows licence
 */

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#ifndef _WX_DFB_CHKCONF_H_
#define _WX_DFB_CHKCONF_H_

#ifndef __WXUNIVERSAL__
#   error "wxDirectFB cannot be built without wxUniversal"
#endif

#endif /* _WX_DFB_CHKCONF_H_ */
